/* SUPPORTCommand.cc */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"

const char SUPPORTCommand_cc_rcsId[] = "$Id: SUPPORTCommand.cc,v 1.5 2002/03/23 18:53:34 gte Exp $" ;

namespace gnuworld
{
using std::string ;

bool SUPPORTCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.SUPPORT");

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser)
	{
	return false;
	}

string support = st[2];

/*
 * First, check they can type the command correctly.
 */

char supportChar = '?';

if (string_lower(support) == "no")
{
	supportChar = 'N';
} else if (string_lower(support) == "yes")
{
	supportChar = 'Y';
} else
{
	bot->Notice(theClient, "Valid SUPPORT choices are YES and NO.");
	return false;
}

/*
 * Next, check to see if this channel is actually pending.
 */

string channelName = st[1];

strstream theQuery;

theQuery	<< "SELECT channels.id FROM pending,channels"
			<< " WHERE lower(channels.name) = '"
			<< escapeSQLChars(string_lower(channelName))
			<< "'"
			<< " AND channels.id = pending.channel_id"
			<< " AND pending.status = 0"
			<< ends;

elog	<< "SUPPORTCommand::sqlQuery> "
		<< theQuery.str()
		<< endl;

ExecStatusType status = bot->SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "SUPPORTCommand> SQL Error: "
			<< bot->SQLDb->ErrorMessage()
			<< endl ;
	return false ;
	}

if (bot->SQLDb->Tuples() <= 0)
{
	bot->Notice(theClient,
		"The channel %s doesn't appear to have a pending application. Please ensure you have spelt the name correctly.",
			channelName.c_str());
	return false;
}

/*
 * So it is, lets check they haven't already voiced their support about this channel.
 */

unsigned int channel_id = atoi(bot->SQLDb->GetValue(0, 0));

strstream supQuery;
supQuery 	<< "SELECT support FROM supporters"
			<< " WHERE channel_id = "
			<< channel_id
			<< " AND user_id = "
			<< theUser->getID()
			<< ends;

elog	<< "SUPPORTCommand::sqlQuery> "
		<< supQuery.str()
		<< endl;

status = bot->SQLDb->Exec( supQuery.str() ) ;
delete[] supQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "SUPPORTCommand> SQL Error: "
			<< bot->SQLDb->ErrorMessage()
			<< endl ;
	return false ;
	}

if (bot->SQLDb->Tuples() <= 0)
{
	bot->Notice(theClient,
		"You don't appear to be listed as a supporter in %s, please ensure you have spelt the channel name correctly.",
			channelName.c_str());
	return false;
}

string currentSupport = bot->SQLDb->GetValue(0, 0);

if ((currentSupport == "Y") || (currentSupport == "N"))
{
	bot->Notice(theClient,
		"You have already made your decision on supporting %s, you cannot change it.",
			channelName.c_str());
		return false;
}

/*
 * Save the changes.
 */

strstream updateQuery;
updateQuery	<< "UPDATE supporters SET support = '"
			<< supportChar
			<< "'"
			<< ", last_updated = now()::abstime::int4"
			<< " WHERE channel_id = "
			<< channel_id
			<< " AND user_id = "
			<< theUser->getID()
			<< ends;

elog	<< "SUPPORTCommand::sqlQuery> "
		<< updateQuery.str()
		<< endl;

status = bot->SQLDb->Exec( updateQuery.str() ) ;
delete[] updateQuery.str() ;

if( PGRES_COMMAND_OK != status )
	{
	elog	<< "SUPPORTCommand> SQL Error: "
			<< bot->SQLDb->ErrorMessage()
			<< endl ;

	bot->Notice(theClient, "An Error occured whilst processing your support. Please contact a CService Administrator.");
	return false ;
	}

bot->Notice(theClient, "Done. Set your support for %s to %s.",
	channelName.c_str(), support.c_str());

bot->logDebugMessage("%s has set their support for %s to %c.",
	theUser->getUserName().c_str(), channelName.c_str(), supportChar);

/*
 * Right, now if they've voted "YES", and there are 10 supporters who have said YES
 * we move this to traffic check stage.
 */

if (supportChar == 'Y')
{
	/*
	 * Check to see if all people have said 'Yes'.
	 */

	strstream tenQuery;
	tenQuery 	<< "SELECT support FROM supporters"
				<< " WHERE channel_id = "
				<< channel_id
				<< ends;

	elog	<< "SUPPORTCommand::sqlQuery> "
			<< tenQuery.str()
			<< endl;

	status = bot->SQLDb->Exec( tenQuery.str() ) ;
	delete[] tenQuery.str() ;

	if( PGRES_TUPLES_OK != status )
		{
		elog	<< "SUPPORTCommand> SQL Error: "
				<< bot->SQLDb->ErrorMessage()
				<< endl ;
		return false ;
		}

	bool allSupporting = true;

	/*
	 * Iterate over the results, if just one of the
	 * supporter records isn't a Y, then its not
	 * yet passed.
	 */

	string support;
	int supporterCount = bot->SQLDb->Tuples();

	for (int i = 0 ; i < supporterCount; i++)
		{
		support = bot->SQLDb->GetValue(i,0);
		if (support != "Y") allSupporting = false;
		}

	if (allSupporting)
	{
		strstream updatePendingQuery;
		updatePendingQuery	<< "UPDATE pending SET status = '1',"
							<< " last_updated = now()::abstime::int4,"
							<< " check_start_ts = now()::abstime::int4"
							<< " WHERE channel_id = "
							<< channel_id
							<< " AND status = '0'"
							<< ends;

		elog	<< "SUPPORTCommand::sqlQuery> "
				<< updatePendingQuery.str()
				<< endl;

		bot->SQLDb->Exec( updatePendingQuery.str() ) ;
		delete[] updatePendingQuery.str() ;
		bot->logDebugMessage("%s has just made it to traffic check phase with %i supporters.",
			channelName.c_str(), supporterCount);
	}

return true;
}

/*
 * Now, if they don't support it - deny this application, and do all sorts
 * of other stuff.
 */

if (supportChar == 'N')
{
	/*
	 * Reject the application.
	 */

	strstream updatePendingQuery;
	updatePendingQuery	<< "UPDATE pending SET status = '9',"
						<< " last_updated = now()::abstime::int4,"
						<< " decision_ts = now()::abstime::int4,"
						<< " decision = '--AUTOMATIC (REGPROC)-- NON-SUPPORT'"
						<< " WHERE channel_id = "
						<< channel_id
						<< " AND status = '0'"
						<< ends;

	elog	<< "SUPPORTCommand::sqlQuery> "
			<< updatePendingQuery.str()
			<< endl;

	bot->SQLDb->Exec( updatePendingQuery.str() ) ;
	delete[] updatePendingQuery.str() ;
	bot->logDebugMessage("%s has just been declined due to non-support.",
		channelName.c_str());

	/*
	 * Now, add some non-support entries.
	 * First, get the username and email address of the channel manager.
	 */

	strstream mgrQuery;
	mgrQuery 	<< "SELECT user_name,email FROM users,pending"
				<< " WHERE pending.manager_id = users.id "
				<< " AND pending.channel_id = "
				<< channel_id
				<< " AND pending.status = '9'"
				<< ends;

	elog	<< "SUPPORTCommand::sqlQuery> "
			<< mgrQuery.str()
			<< endl;

	status = bot->SQLDb->Exec( mgrQuery.str() ) ;
	delete[] mgrQuery.str() ;

	if( PGRES_TUPLES_OK != status )
	{
		elog	<< "SUPPORTCommand> SQL Error: "
				<< bot->SQLDb->ErrorMessage()
				<< endl ;
		return false ;
	}

	if(bot->SQLDb->Tuples() >= 0)
	{
		string managerName = bot->SQLDb->GetValue(0,0);
		string managerEmail = bot->SQLDb->GetValue(0,1);
		static const char* cmdHeader = "INSERT INTO noreg (user_name,email,channel_name,type,expire_time,created_ts,set_by,reason) VALUES ";

		strstream noregQuery;
		noregQuery	<< cmdHeader
					<< "('', '','"
					<< escapeSQLChars(channelName) << "',"
					<< "1, (now()::abstime::int4 + (86400*3)), now()::abstime::int4, '* REGPROC', '-NON SUPPORT-'"
					<< ")" << ends;

		elog	<< "SUPPORTCommand::sqlQuery> "
				<< noregQuery.str()
				<< endl;

		bot->SQLDb->Exec( noregQuery.str() ) ;
		delete[] noregQuery.str() ;

		/*
	  	 * Add a user-based noreg entry too.
		 */

		strstream usernoregQuery;
		usernoregQuery	<< cmdHeader
					<< "('"
					<< escapeSQLChars(managerName) << "', '"
					<< escapeSQLChars(managerEmail) << "', "
					<< "'', "
					<< "1, (now()::abstime::int4 + (86400*3)), now()::abstime::int4, '* REGPROC', '-NON SUPPORT-'"
					<< ")" << ends;

		elog	<< "SUPPORTCommand::sqlQuery> "
				<< usernoregQuery.str()
				<< endl;

		bot->SQLDb->Exec( usernoregQuery.str() ) ;
		delete[] usernoregQuery.str() ;

		/*
		 * Sigh, and a channel-log entry!
		 */

		strstream clogQuery;
		clogQuery	<< "INSERT INTO channellog (ts, channelid, event, message, last_updated) VALUES ("
					<< "now()::abstime::int4, "
					<< channel_id << ", "
					<< "1, 'Non-support by "
					<< escapeSQLChars(theUser->getUserName()) << "', "
					<< "now()::abstime::int4)"
					<< ends;

		elog	<< "SUPPORTCommand::sqlQuery> "
				<< clogQuery.str()
				<< endl;

		bot->SQLDb->Exec( clogQuery.str() ) ;
		delete[] clogQuery.str() ;
	}

}

return true ;
}

} // namespace gnuworld.
