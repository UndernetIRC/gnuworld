/* 
 * ACCESSCommand.cc 
 *
 * 24/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 * 15/02/2001 - David Henriksen <david@itwebnet.dk>
 * Added -op/-voice/-none support
 *
 * Displays all "Level" records for a specified channel.
 * Can optionally narrow down selection using a number of switches. 
 *
 * $Id: ACCESSCommand.cc,v 1.27 2001/02/16 00:18:16 plexus Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"libpq++.h"
#include	"match.h"
#define MAX_RESULTS 15
 
const char ACCESSCommand_cc_rcsId[] = "$Id: ACCESSCommand.cc,v 1.27 2001/02/16 00:18:16 plexus Exp $" ;

namespace gnuworld
{

static const char* queryHeader =    "SELECT channels.name,users.user_name,levels.access,levels.flags,users_lastseen.last_seen,levels.suspend_expires,levels.last_modif,levels.last_modif_by FROM levels,channels,users,users_lastseen ";
static const char* queryCondition = "WHERE levels.channel_id=channels.id AND levels.user_id=users.id AND users.id=users_lastseen.user_id ";
static const char* queryFooter =    "ORDER BY levels.access DESC;";
 
bool ACCESSCommand::Exec( iClient* theClient, const string& Message )
{
/* 
 * This command will build up a custom SQL query and execute it on
 * the 'levels' table. 
 */

StringTokenizer st( Message ) ;
if( st.size() < 3 )
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

/* Don't let ordinary people view * accesses */
if (theChan->getName() == "*") 
{
	sqlUser* theUser = bot->isAuthed(theClient, false);
	if (!theUser) 
	{
		bot->Notice(theClient, "Sorry, the channel %s isn't registered with me.", st[1].c_str());
		return false;
	}

	if (theUser && !bot->getAdminAccessLevel(theUser))
	{
		bot->Notice(theClient, "Sorry, the channel %s isn't registered with me.", st[1].c_str());
		return false; 
	}
}
 
/*
 *  Figure out the switches and append to the SQL statement accordingly.
 */ 

/* 0 = None, 1 = min, 2 = max, 3 = modif, 4 = op, 5 = voice, 6 = none. */
unsigned short currentType = 0;

unsigned int minAmount = 0;
unsigned int maxAmount = 0;
bool modif = false;
bool showAll = false;
bool aOp = false;
bool aVoice = false;
bool aNone = false;

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
		
	if (string_lower(*ptr) == "-op")
		{
		currentType = 4;
		aOp = true;
		continue;
		}
	
	if (string_lower(*ptr) == "-voice")
		{
		currentType = 5;
		aVoice = true;
		continue;
		}
	
	if (string_lower(*ptr) == "-none")
		{
		currentType = 6;
		aNone = true;
		continue;
		}

	if (string_lower(*ptr) == "-all")
		{ 
		sqlUser* tmpUser = bot->isAuthed(theClient, false);
		if ((tmpUser) && (bot->getAdminAccessLevel(tmpUser))) showAll = true; 
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
		case 4: /* Automode Op */
			{
			break;
			}
		case 5: /* Automode Voice */
			{
			break;
			}
		case 6: /* Automode None */
			{
			break;
			} 
		} 
	}

/* Sort out the additional conditions */

strstream extraCond;
if (minAmount) extraCond << "AND levels.access >= " << minAmount << " ";
if (maxAmount) extraCond << "AND levels.access <= " << maxAmount << " ";
extraCond << ends; 

strstream theQuery;
theQuery	<< queryHeader << queryCondition << extraCond.str()
		<< "AND levels.channel_id = " << theChan->getID()
		<< " " << queryFooter << ends;

elog << "ACCESS::sqlQuery> " << theQuery.str() << endl; 

/*
 *  All done, display the output. (Only fetch 15 results).
 */

ExecStatusType status = bot->SQLDb->Exec( theQuery.str() ) ;
if( PGRES_TUPLES_OK == status )
	{
	sqlLevel::flagType flag = 0 ; 

	string autoMode;
	int duration = 0;
	int suspend_expires = 0;
	int suspend_expires_d = 0;
	int suspend_expires_f = 0;
	int results = 0;
	string matchString;
	
	if(st[2][0] == '-') matchString = "*";
	else matchString = st[2];
	
	for (int i = 0 ; i < bot->SQLDb->Tuples(); i++)
		{

		autoMode = "None";
		/* Does the username match the query? */ 
		if (match(st[2], bot->SQLDb->GetValue(i, 1)) == 0)
		{
			results++;			
			flag = atoi(bot->SQLDb->GetValue(i, 3));
			duration = atoi(bot->SQLDb->GetValue(i, 4));
			suspend_expires = atoi(bot->SQLDb->GetValue(i, 5));
			suspend_expires_d = suspend_expires - bot->currentTime();
			suspend_expires_f = bot->currentTime() - suspend_expires_d;
	 
			if (flag & sqlLevel::F_AUTOOP) autoMode = "OP";
			if (flag & sqlLevel::F_AUTOVOICE) autoMode = "VOICE"; 
			
			if(aNone == true)
				{
				if (!(flag & sqlLevel::F_AUTOVOICE) &&
				    !(flag & sqlLevel::F_AUTOOP)) continue;
				}
			if(aVoice == true)
				{
				if (!(flag & sqlLevel::F_AUTOVOICE)) continue;
				}
			if(aOp == true)
				{
				if (!(flag & sqlLevel::F_AUTOOP)) continue;
				}
	
			bot->Notice(theClient, "USER: %s ACCESS: %s", bot->SQLDb->GetValue(i, 1),
				bot->SQLDb->GetValue(i, 2));
	
			bot->Notice(theClient, "CHANNEL: %s -- AUTOMODE: %s",
				bot->SQLDb->GetValue(i, 0), 
				autoMode.c_str());
	
			if(modif)
				{
				bot->Notice(theClient, "LAST MODIFIED: %s (%s ago)", 
					bot->SQLDb->GetValue(i, 7),
					bot->prettyDuration(atoi(bot->SQLDb->GetValue(i,6))).c_str() );
				}
	
			if(suspend_expires != 0)
				{
				bot->Notice(theClient, "** SUSPENDED ** - Expires in %s",
					bot->prettyDuration(suspend_expires_f).c_str());
				}
			bot->Notice(theClient, "LAST SEEN: %s ago.", 
				bot->prettyDuration(duration).c_str());
		}
		if ((results >= MAX_RESULTS) && !showAll) break;

	} // for()
	 
	if ((results >= MAX_RESULTS) && !showAll)
		{
			bot->Notice(theClient, "There are more than 15 matching entries.");
			bot->Notice(theClient, "Please restrict your query."); 
		} else if (results > 0)
		{
			bot->Notice(theClient, "End of access list");
		} 
			else
		{
			bot->Notice(theClient, "No Match!");
		}

	} 

delete[] theQuery.str() ;
delete[] extraCond.str() ;
 
return true ;
} 

} // namespace gnuworld.
