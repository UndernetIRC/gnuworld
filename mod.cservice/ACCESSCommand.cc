/* 
 * ACCESSCommand.cc 
 *
 * 24/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Displays all "Level" records for a specified channel.
 * Can optionally narrow down selection using a number of switches.
 * Can display all channels a user has access on (TODO). 
 *
 * $Id: ACCESSCommand.cc,v 1.13 2001/01/24 01:13:51 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"libpq++.h"

const char ACCESSCommand_cc_rcsId[] = "$Id: ACCESSCommand.cc,v 1.13 2001/01/24 01:13:51 gte Exp $" ;

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
	static const char* queryHeader =    "SELECT channels.name,users.user_name,levels.access,levels.flags,users_lastseen.last_seen,levels.suspend_expires,levels.last_modif,levels.last_modif_by FROM levels,channels,users,users_lastseen ";
	static const char* queryCondition = "WHERE levels.channel_id=channels.id AND levels.user_id=users.id AND users.id=users_lastseen.user_id ";
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

	unsigned int specificId = 0;

	if ( st.size() == 3 && (st[2][0] != '-') && (st[2] != "*") ) /* /msg x access #foo username */
	{
		sqlUser* targetUser = bot->getUserRecord(st[2]);
		if (!targetUser)
		{
			bot->Notice(theClient, "Sorry, I don't know who %s is.", st[2].c_str());
			return false; 
		}

		specificId = targetUser->getID();
	} 


	/*
	 *  Figure out the switches and append to the SQL statement accordingly.
	 */ 

	unsigned short currentType = 0; /* 0 = None, 1 = min, 2 = max, 3 = modif. */
	unsigned short autoOp = 0; /* 0 = any, 1 = just autoop, 2 = not autoop */
	unsigned int minAmount = 0;
	unsigned int maxAmount = 0;
	bool modif = false;
	string modifMask;

	for( StringTokenizer::const_iterator ptr = st.begin() ; ptr != st.end() ;
	++ptr )
	{
		if (string_lower(*ptr) == "-min")
		{
			currentType = 1;
			continue;
		}

		if (string_lower(*ptr) == "-max")
		{
			currentType = 2;
			continue;
		}

		if (string_lower(*ptr) == "-modif")
		{
			currentType = 3;
			modif = true;
			continue;
		}

		switch(currentType)
		{
			case 1: /* Min */
			{
				minAmount = atoi( (*ptr).c_str() );
				if ((minAmount > 1000) || (minAmount < 0))
				{											  
					bot->Notice(theClient, "Invalid minimum level.");
					return false;
				}
				currentType = 0;
				break;
			}
			case 2: /* Max */ 
			{ 
				maxAmount = atoi( (*ptr).c_str() );
				if ((maxAmount > 1000) || (maxAmount < 0))
				{											  
					bot->Notice(theClient, "Invalid maximum level.");
					return false;
				}

				currentType = 0;
				break;
			}
			case 3: /* Modif */ 
			{ 
				// [22:13] <DrCkTaiL> backburner 
				break;
			} 
		} 
	}

	/* Sort out the additional conditions */

	strstream extraCond;
	if (minAmount) extraCond << "AND levels.access >= " << minAmount << " ";
	if (maxAmount) extraCond << "AND levels.access <= " << maxAmount << " ";
	if (specificId) extraCond << "AND levels.user_id = " << specificId << " ";
	extraCond << ends;

 	strstream theQuery;
	theQuery << queryHeader << queryCondition << extraCond.str() << "AND levels.channel_id = " << theChan->getID() << " " << queryFooter << ends;

	elog << "sqlQuery> " << theQuery.str() << endl; 

 	/*
	 *  All done, display the output. (Only fetch 15 results).
	 */
 
	if ((status = bot->SQLDb->Exec(theQuery.str())) == PGRES_TUPLES_OK)
	{
		sqlLevel::flagType flag; 
		unsigned short int autoOp;
		unsigned short int autoVoice;
		int duration = 0;
		int suspend_expires = 0;
		int suspend_expires_d = 0;
		int suspend_expires_f = 0;

		for (int i = 0 ; i < bot->SQLDb->Tuples(); i++)
		{

			flag = atoi(bot->SQLDb->GetValue(i, 3));
			duration = atoi(bot->SQLDb->GetValue(i, 4));
			suspend_expires = atoi(bot->SQLDb->GetValue(i, 5));
			suspend_expires_d = suspend_expires - time(NULL);
			suspend_expires_f = time(NULL) - suspend_expires_d;
 
			autoOp = (flag & sqlLevel::F_AUTOOP);
			autoVoice = (flag & sqlLevel::F_AUTOVOICE);

			bot->Notice(theClient, "USER: %s ACCESS: %s", bot->SQLDb->GetValue(i, 1), bot->SQLDb->GetValue(i, 2));
			bot->Notice(theClient, "CHANNEL: %s -- AUTOOP: %s -- AUTOVOICE: %s", bot->SQLDb->GetValue(i, 0), 
				autoOp ? "ON" : "OFF", autoVoice ? "ON" : "OFF");

			if(modif)
			{
				bot->Notice(theClient, "LAST MODIFIED: %s (%s ago)", 
					bot->SQLDb->GetValue(i, 7), bot->prettyDuration(atoi(bot->SQLDb->GetValue(i,6))).c_str() );
			}

			if(suspend_expires != 0)
			{
				bot->Notice(theClient, "** SUSPENDED ** - Expires in %s", bot->prettyDuration(suspend_expires_f).c_str());
			}
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
