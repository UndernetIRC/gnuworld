/**
 * SUPPORTCommand.cc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: SUPPORTCommand.cc,v 1.9 2007/08/28 16:10:11 dan_karrels Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"

namespace gnuworld
{
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

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

stringstream theQuery;

theQuery	<< "SELECT channels.id FROM pending,channels"
			<< " WHERE lower(channels.name) = '"
			<< escapeSQLChars(string_lower(channelName))
			<< "'"
			<< " AND channels.id = pending.channel_id"
			<< " AND pending.status = 0"
			<< ends;

if( !bot->SQLDb->Exec( theQuery, true ) )
	{
	LOG( ERROR, "SUPPORTCommand SQL Error:") ;
	LOGSQL_ERROR( bot->SQLDb ) ;
	return false ;
	}

if (bot->SQLDb->Tuples() <= 0)
{
	bot->Notice(theClient,
		"The channel %s doesn't appear to have a pending application, please ensure you have spelled the name correctly.",
			channelName.c_str());
	return false;
}

/*
 * So it is, lets check they haven't already voiced their support about this channel.
 */

unsigned int channel_id = atoi(bot->SQLDb->GetValue(0, 0));

stringstream supQuery;
supQuery 	<< "SELECT support FROM supporters"
			<< " WHERE channel_id = "
			<< channel_id
			<< " AND user_id = "
			<< theUser->getID()
			<< ends;

if( !bot->SQLDb->Exec( supQuery, true ) )
	{
	LOG( ERROR, "SUPPORTCommand SQL Error:") ;
	LOGSQL_ERROR( bot->SQLDb ) ;
	return false ;
	}

if (bot->SQLDb->Tuples() <= 0)
{
	bot->Notice(theClient,
		"You don't appear to be listed as a supporter in %s, please ensure you have spelled the channel name correctly.",
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

stringstream updateQuery;
updateQuery	<< "UPDATE supporters SET support = '"
			<< supportChar
			<< "'"
			<< ", last_updated = date_part('epoch', CURRENT_TIMESTAMP)::int"
			<< " WHERE channel_id = "
			<< channel_id
			<< " AND user_id = "
			<< theUser->getID()
			<< ends;

if( !bot->SQLDb->Exec( updateQuery ) )
	{
	LOG( ERROR, "SUPPORTCommand SQL Error:") ;
	LOGSQL_ERROR( bot->SQLDb ) ;

	bot->Notice(theClient, "An Error occured whilst processing your support. Please contact a CService Administrator.");
	return false ;
	}

bot->Notice(theClient, "Done. Set your support for %s to %s.",
	channelName.c_str(), support.c_str());

LOG( INFO, "{} has set their support for {} to {}.",
	theUser->getUserName(), channelName, supportChar);

/*
 * Right, now if they've voted "YES", and there are 10 supporters who have said YES
 * we move this to traffic check stage.
 */

if (supportChar == 'Y')
{
	/*
	 * Check to see if all people have said 'Yes'.
	 */

	stringstream tenQuery;
	tenQuery 	<< "SELECT support FROM supporters"
				<< " WHERE channel_id = "
				<< channel_id
				<< ends;

	if( !bot->SQLDb->Exec( tenQuery, true ) )
		{
		LOG( ERROR, "SUPPORTCommand SQL Error:") ;
		LOGSQL_ERROR( bot->SQLDb ) ;
		return false ;
		}

	bool allSupporting = true;

	/*
	 * Iterate over the results, if just one of the
	 * supporter records isn't a Y, then its not
	 * yet passed.
	 */

	string support;
	unsigned int supporterCount = bot->SQLDb->Tuples();
	if (supporterCount < bot->getConfRequiredSupporters()) return false;

	for (unsigned int i = 0 ; i < supporterCount; i++)
		{
		support = bot->SQLDb->GetValue(i,0);
		if (support != "Y") allSupporting = false;
		}

	if (allSupporting)
	{
		stringstream updatePendingQuery;
		updatePendingQuery	<< "UPDATE pending SET status = '1',"
							<< " last_updated = date_part('epoch', CURRENT_TIMESTAMP)::int,"
							<< " check_start_ts = date_part('epoch', CURRENT_TIMESTAMP)::int"
							<< " WHERE channel_id = "
							<< channel_id
							<< " AND status = '0'"
							<< ends;

		bot->SQLDb->Exec( updatePendingQuery.str().c_str() ) ;
		LOG( INFO, "{} has just made it to traffic check phase with {} supporters.",
			channelName, supporterCount);
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

	stringstream updatePendingQuery;
	updatePendingQuery	<< "UPDATE pending SET status = '9',"
						<< " last_updated = date_part('epoch', CURRENT_TIMESTAMP)::int,"
						<< " decision_ts = date_part('epoch', CURRENT_TIMESTAMP)::int,"
						<< " decision = '--AUTOMATIC (REGPROC)-- NON-SUPPORT'"
						<< " WHERE channel_id = "
						<< channel_id
						<< " AND status = '0'"
						<< ends;

	bot->SQLDb->Exec( updatePendingQuery.str().c_str() ) ;
	LOG( INFO, "{} has just been declined due to non-support.", channelName ) ;

	/*
	 * Now, add some non-support entries.
	 * First, get the username and email address of the channel manager.
	 */

	stringstream mgrQuery;
	mgrQuery 	<< "SELECT user_name,email FROM users,pending"
				<< " WHERE pending.manager_id = users.id "
				<< " AND pending.channel_id = "
				<< channel_id
				<< " AND pending.status = '9'"
				<< ends;

	if( !bot->SQLDb->Exec( mgrQuery, true ) )
		{
		LOG( ERROR, "SUPPORTCommand SQL Error:") ;
		LOGSQL_ERROR( bot->SQLDb ) ;
		return false ;
		}

	// TODO: Comparing unsigned against 0
	if(bot->SQLDb->Tuples() > 0)
	{
		string managerName = bot->SQLDb->GetValue(0,0);
		string managerEmail = bot->SQLDb->GetValue(0,1);
		int noregTime = bot->getConfJudgeDaySeconds() * bot->getConfNoRegDaysOnNOSupport();
		static const char* cmdHeader = "INSERT INTO noreg (user_name,email,channel_name,type,expire_time,created_ts,set_by,reason) VALUES ";

		stringstream noregQuery;
		noregQuery	<< cmdHeader
					<< "('', '','"
					<< escapeSQLChars(channelName) << "',"
                    // << "1, (date_part('epoch', CURRENT_TIMESTAMP)::int + (86400*3)), date_part('epoch', CURRENT_TIMESTAMP)::int, '* REGPROC', '-NON SUPPORT-'"
					<< "1, (date_part('epoch', CURRENT_TIMESTAMP)::int + ("
					<< noregTime
					<< ")), date_part('epoch', CURRENT_TIMESTAMP)::int, '* REGPROC', '-NON SUPPORT-'"
					<< ")" << ends;

		bot->SQLDb->Exec( noregQuery.str().c_str() ) ;

		/*
	  	 * Add a user-based noreg entry too.
		 */

		stringstream usernoregQuery;
		usernoregQuery	<< cmdHeader
					<< "('"
					<< escapeSQLChars(managerName) << "', '"
					<< escapeSQLChars(managerEmail) << "', "
					<< "'', "
					<< "1, (date_part('epoch', CURRENT_TIMESTAMP)::int + (86400*3)), date_part('epoch', CURRENT_TIMESTAMP)::int, '* REGPROC', '-NON SUPPORT-'"
					<< ")" << ends;

		bot->SQLDb->Exec( usernoregQuery.str().c_str() ) ;

		/*
		 * Sigh, and a channel-log entry!
		 */

		stringstream clogQuery;
		clogQuery	<< "INSERT INTO channellog (ts, channelid, event, message, last_updated) VALUES ("
					<< "date_part('epoch', CURRENT_TIMESTAMP)::int, "
					<< channel_id << ", "
					<< "1, 'Non-support by "
					<< escapeSQLChars(theUser->getUserName()) << "', "
					<< "date_part('epoch', CURRENT_TIMESTAMP)::int)"
					<< ends;

		bot->SQLDb->Exec( clogQuery.str().c_str() ) ;
	}

}

return true ;
}

} // namespace gnuworld.
