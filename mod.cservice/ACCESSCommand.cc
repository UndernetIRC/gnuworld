/* 
 * ACCESSCommand.cc 
 *
 * 24/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Displays all "Level" records for a specified channel.
 * Can optionally narrow down selection using a number of switches. (TODO).
 * Can display all channels a user has access on (TODO). 
 *
 * $Id: ACCESSCommand.cc,v 1.6 2001/01/02 07:55:12 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"libpq++.h"

const char ACCESSCommand_cc_rcsId[] = "$Id: ACCESSCommand.cc,v 1.6 2001/01/02 07:55:12 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool ACCESSCommand::Exec( iClient* theClient, const string& Message )
{
/*
 * access::= 'access' [ LWS access_option ] LWS #channel [ LWS access_option ] 
 *                    [(userid|nick|hostmask)] [ LWS access_option ]
 * access_option::= '-modif' [ LWS access_option ] 
 *                 | '-min' LWS access [ LWS access_option ]
 *                 | '-max' LWS access [ LWS access_option ]
 *                 | '-' [ 'no' ] 'autoop' [ LWS access_option ]
 *                 | '-modif' [mask] [ LWS access_option ] 
 *
 *
 * This command will build up a custom SQL query and execute it on the 'levels'
 * table.
 *
 */

	ExecStatusType status;
	static const char* queryHeader =    "SELECT channels.name,users.user_name,levels.access,levels.flags,users.last_seen FROM levels,channels,users ";
	static const char* queryCondition = "WHERE levels.channel_id=channels.id AND levels.user_id=users.id ";
	static const char* queryFooter =    "ORDER BY levels.access DESC LIMIT 15;";

	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}

	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if (!theChan) 
	{
		bot->Notice(theClient, "Sorry, the channel %s isn't registered with me.", st[1].c_str());
		return false;
	} 

	/*
	 *  Figure out the switches and append to the SQL statement accordingly.
	 */

 	strstream theQuery;
	theQuery << queryHeader << queryCondition << "AND levels.channel_id = " << theChan->getID() << " " << queryFooter << ends;

	elog << "sqlQuery> " << theQuery.str() << endl; 

 	/*
	 *  All done, display the output. (Only fetch 15 results).
	 */
 
	if ((status = bot->SQLDb->Exec(theQuery.str())) == PGRES_TUPLES_OK)
	{
		sqlLevel::flagType flag; 
		unsigned short int autoOp;
		int duration = 0;

		for (int i = 0 ; i < bot->SQLDb->Tuples(); i++)
		{

			flag = atoi(bot->SQLDb->GetValue(i, 3));
			duration = atoi(bot->SQLDb->GetValue(i, 4));
			autoOp = (flag & sqlLevel::F_AUTOOP);
			bot->Notice(theClient, "USER: %s ACCESS: %s", bot->SQLDb->GetValue(i, 1), bot->SQLDb->GetValue(i, 2));
			bot->Notice(theClient, "CHANNEL: %s -- AUTOOP: %s", bot->SQLDb->GetValue(i, 0), autoOp ? "ON" : "OFF");
			bot->Notice(theClient, "LAST SEEN: %s ago.",  bot->prettyDuration(duration).c_str()); 
		}
			bot->Notice(theClient, "End of access list");
		if (bot->SQLDb->Tuples() == 15)
		{
			bot->Notice(theClient, "There are more than 15 matching entries.");
			bot->Notice(theClient, "Please restrict your query."); 
		}
	} 
 
	return true ;
} 

} // namespace gnuworld.
