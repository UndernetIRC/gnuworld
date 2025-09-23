/**
 * ACCESSCommand.cc
 *
 * 24/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 * 15/02/2001 - David Henriksen <david@itwebnet.dk>
 * Added -op/-voice/-none support
 *
 * 01/03/01 - Daniel Simard <svr@undernet.org>
 * Fixed Language module stuff.
 *
 * Displays all "Level" records for a specified channel.
 * Can optionally narrow down selection using a number of switches.
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
 * $Id: ACCESSCommand.cc,v 1.50 2009/06/09 15:40:29 mrbean_ Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"dbHandle.h"
#include	"match.h"
#include	"responses.h"
#include	"cservice_config.h"
#include	"Network.h"

namespace gnuworld
{
using std::endl ;
using std::ends ;
using std::stringstream ;
using std::string ;

static const char* queryHeader =    "SELECT channels.name,users.user_name,levels.access,levels.flags,users_lastseen.last_seen,levels.suspend_expires,levels.last_modif,levels.last_modif_by,levels.suspend_level,levels.suspend_reason FROM levels,channels,users,users_lastseen ";
static const char* queryCondition = "WHERE levels.channel_id=channels.id AND levels.user_id=users.id AND users.id=users_lastseen.user_id ";
static const char* queryFooter =    "ORDER BY levels.access DESC;";

bool ACCESSCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.ACCESS");

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

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser)
{
	return false;
}

bool historysearch = false;

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if (!theChan && !bot->getAdminAccessLevel(theUser))
{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::chan_not_reg).c_str(),
		st[1].c_str()
		);
	return false;
}
else if (!theChan && bot->getAdminAccessLevel(theUser))
{
	theChan = bot->getChannelRecord(st[1], true);
	if (!theChan)
	{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::chan_not_reg).c_str(),
			st[1].c_str()
			);
		return false;
	}
	else
	{
		historysearch = true;
	}
}
/* Don't let ordinary people view * accesses */
if (theChan->getName() == "*")
	{
	sqlUser* theUser = bot->isAuthed(theClient, false);
	if (!theUser)
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::chan_not_reg).c_str(),
			st[1].c_str()
		);
		return false;
		}

	if (theUser && !bot->getAdminAccessLevel(theUser))
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::chan_not_reg).c_str(),
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
		if( tmpUser  && bot->getAdminAccessLevel(tmpUser) )
			{
			showAll = true;
			}
		continue;
		}

	switch(currentType)
		{
		case 1: /* Min */
			{
			minAmount = atoi( (*ptr).c_str() );
			if ((minAmount > 1000) /* || (minAmount < 0) */)
				{
				bot->Notice(theClient,
					bot->getResponse(theUser,
						language::inval_min_lvl).c_str()
				);
				return false;
				}
			currentType = 0;
			break;
			}
		case 2: /* Max */
			{
			maxAmount = atoi( (*ptr).c_str() );
			if ((maxAmount > 1000) /* || (maxAmount < 0) */)
				{
				bot->Notice(theClient,
					bot->getResponse(theUser,
						language::inval_max_lvl).c_str()
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

stringstream extraCond;
if (minAmount)
	{
	extraCond << "AND levels.access >= " << minAmount << " ";
	}
if (maxAmount)
	{
	extraCond << "AND levels.access <= " << maxAmount << " ";
	}
extraCond << ends;

stringstream theQuery;
theQuery	<< queryHeader
		<< queryCondition
		<< extraCond.str().c_str()
		<< "AND levels.channel_id = "
		<< theChan->getID()
		<< " "
		<< queryFooter
		<< ends;

// If theChan is no needed after this point, better to delete it now
if (historysearch)
{
	delete theChan;
	theChan = NULL;
}

/*
 *  All done, display the output. (Only fetch 15 results).
 */

if( !bot->SQLDb->Exec( theQuery, true ) )
	{
	LOG( ERROR, "ACCESS SQL Error:") ;
	LOGSQL_ERROR( bot->SQLDb ) ;
	return false ;
	}

sqlLevel::flagType flag = 0 ;

string autoMode;
int duration = 0;
int suspend_expires = 0;
int suspend_expires_d = 0;
int suspend_expires_f = 0;
int results = 0;
string matchString = st[2];

if(matchString[0] == '-')
	{
	matchString = "*";
	}

/*
 * Convert =nick to username.
 */

if (matchString[0] == '=')
	{
	const char* theNick = matchString.c_str();
	// Skip the '='
	++theNick;

	iClient *theClient = Network->findNick(theNick);
	if (theClient)
		{
		sqlUser* tmpUser = bot->isAuthed(theClient,false);
		if (tmpUser)
			{
			matchString = tmpUser->getUserName();
			}
		}
	}

if (historysearch)
	bot->Notice(theClient, "\002   *** Access history search results ***\002");

for (unsigned int i = 0 ; i < bot->SQLDb->Tuples(); i++)
	{
	autoMode = "None";

	/* Does the username match the query? */
	if (match(matchString, bot->SQLDb->GetValue(i, 1)) == 0)
		{
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

		results++;

		bot->Notice(theClient,
			bot->getResponse(theUser, language::user_access_is).c_str(),
			bot->SQLDb->GetValue(i, 1).c_str(),
			bot->SQLDb->GetValue(i, 2).c_str(),
			bot->userStatusFlags(bot->SQLDb->GetValue(i, 1)).c_str()
		);

		bot->Notice(theClient,
			bot->getResponse(theUser, language::channel_automode_is).c_str(),
			bot->SQLDb->GetValue(i, 0).c_str(),
			autoMode.c_str());

		unsigned int suspendLevel = atoi(bot->SQLDb->GetValue(i, 8));
		string suspReason = bot->SQLDb->GetValue(i,9);

		if( suspend_expires > bot->currentTime() )
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::suspend_expires_in).c_str(),
				prettyDuration(suspend_expires_f).c_str(),
				suspendLevel);
			bot->Notice(theClient,bot->getResponse(theUser,
					language::susp_reason,string("Reason: %s")).c_str(),suspReason.c_str());
		}
		else if ((suspReason != "") && (suspend_expires == 0))
			bot->Notice(theClient,bot->getResponse(theUser,
				language::unsusp_reason,string("UNSUSPENDED - %s")).c_str(),suspReason.c_str());
		bot->Notice(theClient,
			bot->getResponse(theUser,
					language::last_seen).c_str(),
			prettyDuration(duration).c_str());

		if(modif)
			{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::last_mod).c_str(),
				bot->SQLDb->GetValue(i, 7).c_str(),
				prettyDuration(atoi(bot->SQLDb->GetValue(i,6))).c_str()
			);
			}
		}
	if ((results >= MAX_ACCESS_RESULTS) && !showAll) break;

	} // for()

if ((results >= MAX_ACCESS_RESULTS) && !showAll)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::more_than_max).c_str(),
		MAX_ACCESS_RESULTS
	);
	bot->Notice(theClient,
		bot->getResponse(theUser, language::restrict_query).c_str()
		);
	}
else if (results > 0)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::end_access_list).c_str()
		);
	}
else
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::no_match).c_str()
		);
	}

return true ;
}

} // namespace gnuworld.
