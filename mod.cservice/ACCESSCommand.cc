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
 * $Id: ACCESSCommand.cc,v 1.32 2001/02/20 15:37:21 dan_karrels Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"libpq++.h"
#include	"match.h"
#include	"responses.h"
#define MAX_RESULTS 15
 
const char ACCESSCommand_cc_rcsId[] = "$Id: ACCESSCommand.cc,v 1.32 2001/02/20 15:37:21 dan_karrels Exp $" ;

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
 
sqlUser* theUser = bot->isAuthed(theClient, false);
sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan) 
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::chan_not_reg,
				string("Sorry, the channel %s isn't registered with me.")).c_str(), 
			st[1].c_str()
		);
		return false;
	} 

/* Don't let ordinary people view * accesses */
if (theChan->getName() == "*") 
{
	sqlUser* theUser = bot->isAuthed(theClient, false);
	if (!theUser) 
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::chan_not_reg,
				string("Sorry, the channel %s isn't registered with me.")).c_str(), 
			st[1].c_str()
		);		
		return false;
	}

	if (theUser && !bot->getAdminAccessLevel(theUser))
	{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::chan_not_reg,
				string("Sorry, the channel %s isn't registered with me.")).c_str(), 
			st[1].c_str()
		);		
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
				bot->Notice(theClient, 
					bot->getResponse(theUser,
						language::inval_min_lvl,
						string("Invalid minimum level."))
				);
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
				bot->Notice(theClient, 
					bot->getResponse(theUser,
						language::inval_max_lvl,
						string("Invalid maximum level."))
				);
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
	string matchString = st[2];
	
	if(matchString[0] == '-') matchString = "*";
	
	for (int i = 0 ; i < bot->SQLDb->Tuples(); i++)
		{

		autoMode = "None";
		/* Does the username match the query? */ 
		if (match(matchString, bot->SQLDb->GetValue(i, 1)) == 0)
		{
			results++;			
			flag = atoi(bot->SQLDb->GetValue(i, 3));
			duration = atoi(bot->SQLDb->GetValue(i, 4));
			suspend_expires = atoi(bot->SQLDb->GetValue(i, 5));
			suspend_expires_d = suspend_expires - bot->currentTime();
			suspend_expires_f = bot->currentTime() - suspend_expires_d;
	 
			if (flag & sqlLevel::F_AUTOOP) autoMode = "OP";
			if (flag & sqlLevel::F_AUTOVOICE) autoMode = "VOICE"; 

			if(aVoice == true || aOp == true || aNone == true)
				{
				if(aNone == true)
					{
					if(!aVoice && (flag & sqlLevel::F_AUTOVOICE)) continue;
					if(!aOp && (flag & sqlLevel::F_AUTOOP)) continue;
					}
				else
					{
					if(!(flag & sqlLevel::F_AUTOVOICE) &&
				           !(flag & sqlLevel::F_AUTOOP)) continue;
					if(!aVoice && (flag & sqlLevel::F_AUTOVOICE)) continue;
					if(!aOp && (flag & sqlLevel::F_AUTOOP)) continue; 
					}
				}
				
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::user_access_is,
					string("USER: %s ACCESS: %s %s")).c_str(),
				bot->SQLDb->GetValue(i, 1),
				bot->SQLDb->GetValue(i, 2),
				bot->userStatusFlags(bot->SQLDb->GetValue(i, 1)).c_str()
			);	
	
			bot->Notice(theClient, 
				bot->getResponse(theUser,
					language::channel_automode_is,
					string("CHANNEL: %s -- AUTOMODE: %s")).c_str(),
				bot->SQLDb->GetValue(i, 0), 
				autoMode.c_str()
			);
	
			if(modif)
				{
				bot->Notice(theClient, 
					bot->getResponse(theUser,
						language::last_mod,
						string("LAST MODIFIED: %s (%s ago)")).c_str(), 
					bot->SQLDb->GetValue(i, 7),
					bot->prettyDuration(atoi(bot->SQLDb->GetValue(i,6))).c_str()
				);	
				}
	
			if(suspend_expires != 0)
				{
				bot->Notice(theClient,
					bot->getResponse(theUser,
						language::suspend_expires_in,
						string("** SUSPENDED ** - Expires in %s")).c_str(),
					bot->prettyDuration(suspend_expires_f).c_str()
				);
				}
			bot->Notice(theClient,
				bot->getResponse(theUser,
						language::last_seen,
						string("LAST SEEN: %s ago.")).c_str(), 
				bot->prettyDuration(duration).c_str()
			);
		}
		if ((results >= MAX_RESULTS) && !showAll) break;

	} // for()
	 
	if ((results >= MAX_RESULTS) && !showAll)
		{
			bot->Notice(theClient, 
				bot->getResponse(theUser,
						language::more_than_max,
						string("There are more than 15 matching entries."))
			);
			bot->Notice(theClient,
				bot->getResponse(theUser,
						language::restrict_query,
						string("Please restrict your query."))
			);
		} else if (results > 0)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
						language::end_access_list,
						string("End of access list"))
			);
		} 
			else
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
						language::no_match,
						string("No Match!"))
			);
		}

	} 

delete[] theQuery.str() ;
delete[] extraCond.str() ;
 
return true ;
} 

} // namespace gnuworld.
