/**
 * cservice.cc
 * Author: Greg Sikorski
 * Purpose: Overall control client.
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
 * $Id: cservice.cc,v 1.298 2010/08/31 21:16:46 denspike Exp $
 */

#include	<new>
#include	<map>
#include	<vector>
#include	<iostream>
#include	<sstream>
#include	<string>
#include	<iomanip>
#include	<utility>

#include	<ctime>
#include	<cstdlib>
#include	<cstring>
#include	<cstdarg>

#include	"client.h"
#include  	"cservice.h"
#include	"EConfig.h"
#include	"events.h"
#include	"ip.h"
#include	"Network.h"
#include	"StringTokenizer.h"
#include	"misc.h"
#include	"ELog.h"
#include	"dbHandle.h"
#include	"constants.h"
#include	"networkData.h"
#include	"levels.h"
#include	"cservice_config.h"
#include	"match.h"
#include	"md5hash.h"
#include	"responses.h"

namespace gnuworld
{

using std::pair ;
using std::vector ;
using std::endl ;
using std::stringstream ;
using std::ends ;
using std::string ;
using std::clog ;

/*
 *  Exported function used by moduleLoader to gain an
 *  instance of this module.
 */

extern "C"
{
  xClient* _gnuwinit(const string& args)
  {
    return new cservice( args );
  }
}

bool cservice::RegisterCommand( Command* newComm )
{
UnRegisterCommand( newComm->getName() ) ;
return commandMap.insert( pairType( newComm->getName(), newComm ) ).second ;
}

bool cservice::UnRegisterCommand( const string& commName )
{
commandMapType::iterator ptr = commandMap.find( commName ) ;
if( ptr == commandMap.end() )
        {
        return false ;
        }
delete ptr->second ;
commandMap.erase( ptr ) ;
return true ;
}

void cservice::OnAttach()
{
for( commandMapType::iterator ptr = commandMap.begin() ;
	ptr != commandMap.end() ; ++ptr )
	{
	ptr->second->setServer( MyUplink ) ;
	}

// Start the Db checker timer rolling.
time_t theTime = time(NULL) + connectCheckFreq;
dBconnection_timerID = MyUplink->RegisterTimer(theTime, this, NULL);

// Start the Db update/Reop timer rolling.
theTime = time(NULL) + updateInterval;
update_timerID = MyUplink->RegisterTimer(theTime, this, NULL);

// Start the ban suspend/expire timer rolling.
theTime = time(NULL) + expireInterval;
expire_timerID = MyUplink->RegisterTimer(theTime, this, NULL);

// Start the cache expire timer rolling.
theTime = time(NULL) + cacheInterval;
cache_timerID = MyUplink->RegisterTimer(theTime, this, NULL);

// Start the pending chan timer rolling.
theTime = time(NULL) + pendingChanPeriod;
pending_timerID = MyUplink->RegisterTimer(theTime, this, NULL);

// Start the floating Limit timer rolling.
theTime = time(NULL) + limitCheckPeriod;
limit_timerID = MyUplink->RegisterTimer(theTime, this, NULL);

/* Start the web relay timer rolling.
 * First, empty out any old notices that may be present.
 */
#ifdef LOG_SQL
	elog	<< "cservice::OnAttach::sqlQuery> DELETE FROM "
		<< "webnotices WHERE created_ts < "
		<< "now()::abstime::int4 - 600)"
		<< endl;
#endif
if (SQLDb->Exec("DELETE FROM webnotices WHERE created_ts < (now()::abstime::int4 - 600)"))
{
	/* only register the timer if the query is ok.
	 * if the query fails, we most likely don't have
	 * the table setup, so pointless checking it.
	 */
	theTime = time(NULL) + webrelayPeriod;
	webrelay_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
} else {
	/* log the error */
	elog	<< "cservice::OnAttach> Unable to empty webnotices "
		<< "table, not checking webnotices."
		<< endl;
}

if (SQLDb->Exec("SELECT now()::abstime::int4;",true))
	{
	// Set our "Last Refresh" timers to the current database system time.
	time_t serverTime = atoi(SQLDb->GetValue(0,0).c_str());
	lastChannelRefresh = serverTime;
	lastUserRefresh = serverTime;
	lastLevelRefresh = serverTime;
	lastBanRefresh = serverTime;

	/*
	 * Calculate the current time offset from the DB server.
	 * We always want to talk in DB server time.
	 */

	dbTimeOffset = serverTime - ::time(NULL);
	elog	<< "*** [CMaster::ImplementServer]:  Current DB server time: "
		<< currentTime()
		<< endl;
	}
else
	{
 	elog	<< "Unable to retrieve time from postgres server!"
		<< endl;
	::exit(0);
	}

/* Register our interest in recieving some Network events from gnuworld. */

MyUplink->RegisterEvent( EVT_KILL, this );
MyUplink->RegisterEvent( EVT_QUIT, this );
MyUplink->RegisterEvent( EVT_NICK, this );
MyUplink->RegisterEvent( EVT_ACCOUNT, this );
MyUplink->RegisterEvent( EVT_BURST_ACK, this );
MyUplink->RegisterEvent( EVT_XQUERY, this );

xClient::OnAttach() ;
}

void cservice::OnShutdown(const std::string& reason)
{
	/* handle client shutdown */
	MyUplink->UnloadClient(this, reason);
}

cservice::cservice(const string& args)
 : xClient( args )
{

#ifdef USE_COMMAND_LOG
commandLog.open(commandlogPath.c_str());
#endif
/*
 *  Register command handlers.
 */

RegisterCommand(new SHOWCOMMANDSCommand(this, "SHOWCOMMANDS", "[#channel]", 3));
RegisterCommand(new LOGINCommand(this, "LOGIN", "<username> <password>", 30));
RegisterCommand(new ACCESSCommand(this, "ACCESS", "[channel] [username] [-min n] [-max n] [-op] [-voice] [-none] [-modif]", 5));
RegisterCommand(new CHANINFOCommand(this, "CHANINFO", "<#channel>", 3));
RegisterCommand(new ISREGCommand(this, "ISREG", "<#channel>", 4));
RegisterCommand(new VERIFYCommand(this, "VERIFY", "<nick>", 3));
//RegisterCommand(new SEARCHCommand(this, "SEARCH", "[-min <amount>] <keywords>", 5));
RegisterCommand(new MOTDCommand(this, "MOTD", "", 4));
RegisterCommand(new HELPCommand(this, "HELP", "[command]", 4));
RegisterCommand(new RANDOMCommand(this, "RANDOM", "", 4));
RegisterCommand(new SHOWIGNORECommand(this, "SHOWIGNORE", "", 3));
RegisterCommand(new SUPPORTCommand(this, "SUPPORT", "#channel <YES|NO>", 15));
RegisterCommand(new NOTECommand(this, "NOTE", "send <username> <message>, read all, erase <all|message id>", 10));
RegisterCommand(new NOTECommand(this, "NOTES", "send <username> <message>, read all, erase <all|message id>", 10));

RegisterCommand(new OPCommand(this, "OP", "<#channel> [nick] [nick] ..", 3));
RegisterCommand(new DEOPCommand(this, "DEOP", "<#channel> [nick] [nick] ..", 3));
RegisterCommand(new VOICECommand(this, "VOICE", "<#channel> [nick] [nick] ..", 3));
RegisterCommand(new DEVOICECommand(this, "DEVOICE", "<#channel> [nick] [nick] ..", 3));
RegisterCommand(new ADDUSERCommand(this, "ADDUSER", "<#channel> <username> <access>", 8));
RegisterCommand(new REMUSERCommand(this, "REMUSER", "<#channel> <username>", 4));
RegisterCommand(new MODINFOCommand(this, "MODINFO", "<#channel> [ACCESS <username> <level>] [AUTOMODE <username> <NONE|OP|VOICE>] [INVITE <ON|OFF>]", 6));
RegisterCommand(new SETCommand(this, "SET", "[#channel] <variable> <value> or, SET <invisible> <ON|OFF> or, SET LANG <language> or, SET MAXLOGINS <max-logins>.", 6));
RegisterCommand(new INVITECommand(this, "INVITE", "<#channel>", 2));
RegisterCommand(new TOPICCommand(this, "TOPIC", "<#channel> <topic>", 4));
RegisterCommand(new CHANINFOCommand(this, "CHANINFO", "<#channel>", 3));
RegisterCommand(new CHANINFOCommand(this, "INFO", "<username>", 3));
RegisterCommand(new BANLISTCommand(this, "BANLIST", "<#channel>", 3));
RegisterCommand(new KICKCommand(this, "KICK", "<#channel> <nick> <reason>", 4));
RegisterCommand(new STATUSCommand(this, "STATUS", "<#channel>", 4));
RegisterCommand(new SUSPENDCommand(this, "SUSPEND", "<#channel> <username> <duration> [level]", 5));
RegisterCommand(new UNSUSPENDCommand(this, "UNSUSPEND", "<#channel> <username>", 5));
RegisterCommand(new BANCommand(this, "BAN", "<#channel> <nick | *!*user@*.host> [duration] [level] [reason]", 5));
RegisterCommand(new UNBANCommand(this, "UNBAN", "<#channel> <*!*user@*.host>", 5));
RegisterCommand(new LBANLISTCommand(this, "LBANLIST", "<#channel> <banmask>", 5));
RegisterCommand(new NEWPASSCommand(this, "NEWPASS", "<new passphrase>", 8));
RegisterCommand(new JOINCommand(this, "JOIN", "<#channel>", 8));
RegisterCommand(new PARTCommand(this, "PART", "<#channel>", 8));
RegisterCommand(new OPERJOINCommand(this, "OPERJOIN", "<#channel>", 8));
RegisterCommand(new OPERPARTCommand(this, "OPERPART", "<#channel>", 8));
RegisterCommand(new CLEARMODECommand(this, "CLEARMODE", "<#channel>", 4));
RegisterCommand(new SUSPENDMECommand(this, "SUSPENDME", "<password>", 15));

RegisterCommand(new SCANHOSTCommand(this, "SCANHOST", "<mask> [-all]", 10));
RegisterCommand(new SCANUNAMECommand(this, "SCANUNAME", "<mask> [-all]", 10));
RegisterCommand(new REMIGNORECommand(this, "REMIGNORE", "<mask>", 5));
RegisterCommand(new REGISTERCommand(this, "REGISTER", "<#channel> <username>", 8));
RegisterCommand(new REMOVEALLCommand(this, "REMOVEALL", "<#channel>", 15));
RegisterCommand(new PURGECommand(this, "PURGE", "<#channel> [-noop] <reason>", 8));
RegisterCommand(new FORCECommand(this, "FORCE", "<#channel>", 8));
RegisterCommand(new UNFORCECommand(this, "UNFORCE", "<#channel>", 8));
RegisterCommand(new SERVNOTICECommand(this, "SERVNOTICE", "<#channel> <text>", 5));
RegisterCommand(new SAYCommand(this, "SAY", "<#channel> <text>", 5));
RegisterCommand(new SAYCommand(this, "DO", "<#channel> <text>", 5));
RegisterCommand(new QUOTECommand(this, "QUOTE", "<text>", 5));
RegisterCommand(new REHASHCommand(this, "REHASH", "[translations | help | config | motd]", 5));
RegisterCommand(new STATSCommand(this, "STATS", "", 8));
RegisterCommand(new ADDCOMMENTCommand(this, "ADDCOMMENT", "<username> <comment>", 10));
RegisterCommand(new SHUTDOWNCommand(this, "SHUTDOWN", "[reason]", 10));

#ifdef ALLOW_HELLO
  RegisterCommand( new HELLOCommand( this,
	"HELLO", "<username> <email> <email>", 10 ) ) ;
#endif // ALLOW_HELLO

cserviceConfig = new (std::nothrow) EConfig( args ) ;
assert( cserviceConfig != 0 ) ;

confSqlHost = cserviceConfig->Require( "sql_host" )->second;
confSqlPass = cserviceConfig->Require( "sql_pass" )->second;
confSqlDb = cserviceConfig->Require( "sql_db" )->second;
confSqlPort = cserviceConfig->Require( "sql_port" )->second;
confSqlUser = cserviceConfig->Require( "sql_user" )->second;

string Query = "host=" + confSqlHost + " dbname=" + confSqlDb + " port=" + confSqlPort + " user=" + confSqlUser
			   + " password=" + confSqlPass;

elog	<< "*** [CMaster]: Attempting to make PostgreSQL connection to: "
		<< confSqlHost
		<< "; Database Name: "
		<< confSqlDb
		<< endl;

//SQLDb = new (std::nothrow) cmDatabase( Query.c_str() ) ;
SQLDb = new (std::nothrow) dbHandle( confSqlHost,
	atoi( confSqlPort.c_str() ),
	confSqlDb,
	confSqlUser,
	confSqlPass ) ;
assert( SQLDb != 0 ) ;

if (SQLDb->ConnectionBad ())
	{
	elog	<< "*** [CMaster]: Unable to connect to SQL server."
			<< endl
			<< "*** [CMaster]: PostgreSQL error message: "
			<< SQLDb->ErrorMessage()
			<< endl ;

	::exit( 0 ) ;
	}
else
	{
	elog	<< "*** [CMaster]: Connection established to SQL server. "
			<< endl ;
	}

// The program will exit if these variables are not defined in the
// configuration file.
relayChan = cserviceConfig->Require( "relay_channel" )->second ;
privrelayChan = cserviceConfig->Require( "priv_relay_channel" )->second ;
debugChan = cserviceConfig->Require( "debug_channel" )->second ;
coderChan = cserviceConfig->Require( "coder_channel" )->second ;
pendingPageURL = cserviceConfig->Require( "pending_page_url" )->second ;
updateInterval = atoi((cserviceConfig->Require( "update_interval" )->second).c_str());
expireInterval = atoi((cserviceConfig->Require( "expire_interval" )->second).c_str());
cacheInterval = atoi((cserviceConfig->Require( "cache_interval" )->second).c_str());
webrelayPeriod = atoi((cserviceConfig->Require( "webrelay_interval" )->second).c_str());
input_flood = atoi((cserviceConfig->Require( "input_flood" )->second).c_str());
output_flood = atoi((cserviceConfig->Require( "output_flood" )->second).c_str());
flood_duration = atoi((cserviceConfig->Require( "flood_duration" )->second).c_str());
topic_duration = atoi((cserviceConfig->Require( "topic_duration" )->second).c_str());
pendingChanPeriod = atoi((cserviceConfig->Require( "pending_duration" )->second).c_str());
connectCheckFreq = atoi((cserviceConfig->Require( "connection_check_frequency" )->second).c_str());
connectRetry = atoi((cserviceConfig->Require( "connection_retry_total" )->second).c_str());
limitCheckPeriod = atoi((cserviceConfig->Require( "limit_check" )->second).c_str());
loginDelay = atoi((cserviceConfig->Require( "login_delay" )->second).c_str());
noteDuration = atoi((cserviceConfig->Require( "note_duration" )->second).c_str());
noteLimit = atoi((cserviceConfig->Require( "note_limit" )->second).c_str());
preloadUserDays = atoi((cserviceConfig->Require( "preload_user_days" )->second).c_str());
partIdleChan = atoi((cserviceConfig->Require( "part_idle_chan" )->second).c_str());
#ifdef USE_COMMAND_LOG
commandlogPath = cserviceConfig->Require( "command_logfile" )->second ;
#endif
/* adminlogPath = cserviceConfig->Require( "admin_logfile" )->second ; */

#ifdef ALLOW_HELLO
  helloBlockPeriod = atoi( cserviceConfig->Require( 
    "hello_block_period" )->second.c_str() ) ;
#endif // ALLOW_HELLO

loadConfigData();

userHits = 0;
userCacheHits = 0;
channelHits = 0;
channelCacheHits = 0;
levelHits = 0;
levelCacheHits = 0;
banHits = 0;
banCacheHits = 0;
dbErrors = 0;
joinCount = 0;
connectRetries = 0;
totalCommands = 0;

/* Load our translation tables. */
loadTranslationTable();

/* Load help messages */
loadHelpTable();

/* Preload the Channel Cache */
preloadChannelCache();

/* Preload the Ban Cache */
preloadBanCache();

/* Preload the Level cache */
preloadLevelsCache();

/* Preload any user accounts we want to */
preloadUserCache();

/*
 * Init the admin log.
 */

/* adminLog.open( adminlogPath.c_str() ) ;
if( !adminLog.is_open() )
	{
	clog	<< "*** Unable to open CMaster admin log file: "
			<< adminlogPath
			<< endl ;
	::exit( 0 ) ;
	} */

}

cservice::~cservice()
{
delete cserviceConfig ;	cserviceConfig = 0 ;
delete SQLDb ; SQLDb = 0 ;

for( commandMapType::iterator ptr = commandMap.begin() ;
	ptr != commandMap.end() ; ++ptr )
	{
	delete ptr->second ;
	}

commandMap.clear() ;
}

void cservice::BurstChannels()
{
	/*
	 *   Need to join every channel with AUTOJOIN set. (But not * ;))
 	 *   Various other things must be done, such as setting the topic if AutoTopic
 	 *   is on.
 	 */

	Channel* tmpChan;
	sqlChannelHashType::iterator ptr = sqlChannelCache.begin();
	while (ptr != sqlChannelCache.end())
	{
	sqlChannel* theChan = (ptr)->second;

	/* we're now interested in registered channels even if we're not in it... */
	if(theChan->getName() == "*")
	{	//Do not do anything for the admins channel
		++ptr;
		continue;
	}
	
	MyUplink->RegisterChannelEvent( theChan->getName(), this ) ;

	tmpChan = Network->findChannel(theChan->getName());
	
	if (tmpChan && tmpChan->getCreationTime() < theChan->getChannelTS())
			theChan->setChannelTS(tmpChan->getCreationTime());	

	if (theChan->getFlag(sqlChannel::F_AUTOJOIN)) 
		{
		string tempModes = theChan->getChannelMode();
		tempModes += 'R';
		MyUplink->JoinChannel( this,
			theChan->getName(),
			tempModes,
			theChan->getChannelTS(),
			true );

		theChan->setInChan(true);
		joinCount++;

			if (getConfigVar("BAN_CHECK_ON_BURST")->asInt() == 1)
			{
				/* check current inhabitants of the channel against our banlist */
				for (Channel::userIterator chanUsers = tmpChan->userList_begin();
					chanUsers != tmpChan->userList_end(); ++chanUsers)
				{
					ChannelUser* tmpUser = chanUsers->second;
					/* check if this user is banned */
					(void)checkBansOnJoin(tmpChan, theChan, tmpUser->getClient());
				}
			}
		} else {
			/* although AUTOJOIN isn't set, set the channel to +R if
			 * it exists on the Network.
			 */
			if (tmpChan)
			{
				stringstream tmpTS;
				tmpTS << theChan->getChannelTS();
				string channelTS = tmpTS.str();
				MyUplink->Mode(NULL, tmpChan, string("+R"), channelTS );
			}
		}
	++ptr;
	}

	logDebugMessage("Channel join complete.");

	return xClient::BurstChannels();
}

void cservice::OnConnect()
{
// TODO: I changed this from return 0
xClient::OnConnect() ;
}

unsigned short cservice::getFloodPoints(iClient* theClient)
{
/*
 * This function gets an iClient's flood points.
 */

networkData* tmpData =
	static_cast< networkData* >( theClient->getCustomData(this) ) ;

if(!tmpData)
	{
	return 0;
	}
//assert(tmpData != NULL);

return tmpData->flood_points;
}

void cservice::setFloodPoints(iClient* theClient, unsigned short amount)
{
networkData* tmpData =
	static_cast< networkData* >( theClient->getCustomData(this) ) ;

if (!tmpData)
	{
	return;
	}
//assert(tmpData != NULL);

tmpData->flood_points = amount;
}

/**
 * This method sets a timestamp for when we last recieved
 * a message from this iClient.
 */
void cservice::setLastRecieved(iClient* theClient, time_t last_recieved)
{
networkData* tmpData =
	static_cast< networkData* >( theClient->getCustomData(this) ) ;

if(!tmpData)
	{
	return;
	}
//assert(tmpData != NULL);

tmpData->messageTime = last_recieved;
}

/**
 * This method gets a timestamp from this iClient
 * for flood control.
 */
time_t cservice::getLastRecieved(iClient* theClient)
{
networkData* tmpData =
	static_cast< networkData* >( theClient->getCustomData(this) ) ;

if(!tmpData)
	{
	return 0;
	}
//assert(tmpData != NULL);

return tmpData->messageTime;
}

bool cservice::isIgnored(iClient* theClient)
{

networkData* tmpData =
	static_cast< networkData* >( theClient->getCustomData(this) ) ;

if(!tmpData)
	{
	return 0;
	}

return tmpData->ignored;
}

void cservice::setIgnored(iClient* theClient, bool _ignored)
{

networkData* tmpData =
	static_cast< networkData* >( theClient->getCustomData(this) ) ;

if(!tmpData)
	{
	return;
	}

tmpData->ignored = _ignored;
}

bool cservice::hasFlooded(iClient* theClient, const string& type)
{
if( (getLastRecieved(theClient) + flood_duration) <= ::time(NULL) )
	{
	/*
	 *  Reset a few things, they're out of the flood period now.
	 *  Or, this is the first message from them.
	 */

	setFloodPoints(theClient, 0);
	setOutputTotal(theClient, 0);
	setLastRecieved(theClient, ::time(NULL));
	ipFloodMap[theClient->getIP()]=0;
	}
else
	{
	/*
	 *  Inside the flood period, check their points..
	 */

	if(getFloodPoints(theClient) > input_flood)
		{
		/*
		 *  Check admin access, if present then
		 *  don't trigger.
		 */

		sqlUser* theUser = isAuthed(theClient, false);
		if (theUser && getAdminAccessLevel(theUser))
			{
			return false;
			}

		// Bad boy!
		setFloodPoints(theClient, 0);
		setLastRecieved(theClient, ::time(NULL));
		Notice(theClient,
			"Flood me will you? I'm not going to listen to "
			"you anymore.");

		// Send a silence numeric target, and mask to ignore
		// messages from this user.
		string silenceMask = string( "*!*" )
			+ theClient->getUserName()
			+ "@"
			+ theClient->getInsecureHost();

		stringstream s;
		s	<< getCharYYXXX()
			<< " SILENCE "
			<< theClient->getCharYYXXX()
			<< " "
			<< silenceMask
			<< ends;
		Write( s );

		time_t expireTime = currentTime() + 3600;
		silenceList.insert(silenceListType::value_type(silenceMask,
			std::make_pair(expireTime, theClient->getCharYYXXX())));

		setIgnored(theClient, true);

		string floodComment;
		StringTokenizer st(type) ;

		if( st.size() >= 2 )
		{
			floodComment = st[0] + ' ' + st[1];
		} else {
			floodComment = st[0];
		}

		if (getConfigVar("FLOOD_MESSAGES")->asInt()==1)
		{
			logAdminMessage("MSG-FLOOD from %s (%s)",
				theClient->getNickUserHost().c_str(),
				floodComment.c_str());
		}
		return true;
		} // if()

		if (ipFloodMap[theClient->getIP()]>input_flood*5)
		{
			setLastRecieved(theClient, ::time(NULL));

			string silenceMask = string( "*!*" )
				+ theClient->getUserName()
				+ "@"
				+ theClient->getInsecureHost();

			stringstream s;
			s	<< getCharYYXXX()
				<< " SILENCE "
				<< theClient->getCharYYXXX()
				<< " "
				<< silenceMask
				<< ends;
			Write( s );

			Notice(theClient,
				"Flood me will you? I'm not going to listen to "
				"you or your friends anymore.");

			time_t expireTime = currentTime() + 3600;
			silenceList.insert(silenceListType::value_type(silenceMask,
				std::make_pair(expireTime, theClient->getCharYYXXX())));

			setIgnored(theClient, true);

			string floodComment;
			StringTokenizer st(type);

			if (st.size() >= 2)
			{
				floodComment = st[0] + ' ' + st[1];
			} else {
				floodComment = st[0];
			}

			if (getConfigVar("FLOOD_MESSAGES")->asInt()==1)
			{
				logAdminMessage("IP-FLOOD from %s (%s)",
					theClient->getNickUserHost().c_str(),
					floodComment.c_str());
			}
			return true;
		}
	} // else()

return false;
}

void cservice::setOutputTotal(const iClient* theClient, unsigned int count)
{
/*
 * This function sets the current output total in bytes
 * for this client.
 */

networkData* tmpData =
	static_cast< networkData* >( theClient->getCustomData(this) );

if (!tmpData)
	{
	return;
	}
//assert(tmpData != NULL);

tmpData->outputCount = count;
}

unsigned int cservice::getOutputTotal(const iClient* theClient)
{
networkData* tmpData =
	static_cast< networkData* >( theClient->getCustomData(this) );

if (!tmpData)
	{
	return 0;
	}
//assert(tmpData != NULL);

return tmpData->outputCount;
}

bool cservice::hasOutputFlooded(iClient* theClient)
{

if( (getLastRecieved(theClient) + flood_duration) <= ::time(NULL) )
	{
	/*
	 *  Reset a few things, they're out of the flood period now.
	 *  Or, this is the first message from them.
	 */

	setOutputTotal(theClient, 0);
	setFloodPoints(theClient, 0);
	setLastRecieved(theClient, ::time(NULL));
	}
else
	{
	/*
	 *  Inside the flood period, check their output count.
	 */

	if(getOutputTotal(theClient) > output_flood)
		{
		/*
		 *  Check admin access, if present then
		 *  don't trigger.
		 */

		sqlUser* theUser = isAuthed(theClient, false);
		if (theUser && getAdminAccessLevel(theUser))
			{
			return false;
 			}

		setOutputTotal(theClient, 0);
		setLastRecieved(theClient, ::time(NULL));
		Notice(theClient, "I think I've sent you a little "
			"too much data, I'm going to ignore you "
			"for a while.");

		// Send a silence numeric target, and mask to ignore
		// messages from this user.
		string silenceMask = string( "*!*" )
			+ theClient->getUserName()
			+ "@"
			+ theClient->getInsecureHost();

		stringstream s;
		s	<< getCharYYXXX()
			<< " SILENCE "
			<< theClient->getCharYYXXX()
			<< " "
			<< silenceMask
			<< ends;
		Write( s );

		time_t expireTime = currentTime() + 3600;

		silenceList.insert(silenceListType::value_type(silenceMask,
			std::make_pair(expireTime, theClient->getCharYYXXX())));

		setIgnored(theClient, true);

		if (getConfigVar("FLOOD_MESSAGES")->asInt()==1)
		{
			logAdminMessage("OUTPUT-FLOOD from %s",
				theClient->getNickUserHost().c_str());
		}
		return true;
		}
	}

return false;
}

void cservice::OnPrivateMessage( iClient* theClient,
	const string& Message,
	bool secure )
{
/*
 * Private message handler. Pass off the command to the relevant
 * handler.
 */

if (isIgnored(theClient)) return ;

StringTokenizer st( Message ) ;
if( st.empty() )
	{
	Notice( theClient, "Incomplete command");
	return ;
	}

#ifdef USE_COMMAND_LOG
commandLog << (secure ? "[" : "<") << theClient->getNickUserHost() << (secure ? "] " : "> ")  << Message << endl;
#endif

/*
 * Do flood checking - admins at 750 or above are excempt.
 * N.B: Only check that *after* someone has flooded ;)
 */
const string Command = string_upper( st[ 0 ] ) ;

/*
 *  Just quickly, abort if someone tries to LOGIN or NEWPASS
 *  unsecurely.
 */

if (!secure && ((Command == "LOGIN") || (Command == "NEWPASS") || (Command == "SUSPENDME")) )
	{
	Notice(theClient, "To use %s, you must /msg %s@%s",
		Command.c_str(),
		nickName.c_str(),
		getUplinkName().c_str());
	return ;
	}

/*
 * If the person issuing this command is an authenticated admin, we need to log
 * it to the admin log for accountability purposes.
 *
 * Moved this logging further down the function (to SQL) so that only recognised
 * commands are logged.  This stops logging of passwords etc
 *
		sqlUser* theUser = isAuthed(theClient, false);
		if (theUser && getAdminAccessLevel(theUser))
			{
			adminLog << ::time(NULL) << " " << theClient->getRealNickUserHost() << " " << st.assemble() << endl;
 			}
 */

/* Attempt to find a handler for this method. */

commandMapType::iterator commHandler = commandMap.find( Command ) ;
if( commHandler == commandMap.end() )
	{
	/* Don't reply to unknown commands, but add to their flood
	 * total :)
	 */
	if (hasFlooded(theClient, "PRIVMSG"))
		{
		return ;
		}

	if (hasOutputFlooded(theClient))
		{
		return ;
		}

	// Why use 3 here?  Should be in config file
	// (Violation of "rule of numbers")
	setFloodPoints(theClient, getFloodPoints(theClient) + 3);
	ipFloodMap[theClient->getIP()] += 3;
	}
else
	{
	/*
	 *  Check users flood limit, if exceeded..
	 */

	if (hasFlooded(theClient, Message))
		{
		return ;
		}

	if (hasOutputFlooded(theClient))
		{
		return ;
		}

	setFloodPoints(theClient, getFloodPoints(theClient)
		+ commHandler->second->getFloodPoints() );
	ipFloodMap[theClient->getIP()] += commHandler->second->getFloodPoints();

	totalCommands++;

	sqlUser* theUser = isAuthed(theClient, false);

	/* Check IP restriction (if admin level) - this is in case you get added as admin AFTER logging in */
	if (theUser && needIPRcheck(theUser) && (Command != "LOGIN"))
	{
		/* ok, they have a valid user and are listed as admin (and this is not a login request) */
		if (!passedIPR(theClient))
		{
			/* not passed IPR yet, do it */
			if (!checkIPR(theClient, theUser))
			{
				/* failed IPR! */
				Notice(theClient, "Insufficient access for this command. (IPR)");
				return;
			}
		}
	}
	/* Log command to SQL here, if Admin */
	if (theUser && getAdminAccessLevel(theUser) && (Command != "LOGIN") && (Command != "NEWPASS") && (Command != "SUSPENDME"))
	{
		stringstream theLog;
		theLog  << "INSERT INTO adminlog (user_id,cmd,args,timestamp,issue_by)"
			<< " VALUES("
			<< theUser ->getID()
			<< ","
			<< "'" << escapeSQLChars(Command) << "'"
			<< ","
			<< "'" << escapeSQLChars(st.assemble(1)) << "'"
			<< ","
			<< currentTime()
			<< ","
			<< "'" << escapeSQLChars(theClient->getRealNickUserHost()) << "'"
			<< ")"
			<< ends;

		if( !SQLDb->Exec(theLog ) )
//		if( PGRES_COMMAND_OK != status )
		{
			elog    << "cservice::adminlog> Something went wrong: "
				<< theLog.str().c_str()
				<< " "
				<< SQLDb->ErrorMessage()
				<< endl;
		}
	}

	commHandler->second->Exec( theClient, Message ) ;
	}

xClient::OnPrivateMessage( theClient, Message ) ;
}

void cservice::OnCTCP( iClient* theClient, const string& CTCP,
                    const string& Message, bool )
{
/**
 * CTCP hander. Deal with PING, GENDER and VERSION.
 * Hit users with a '5' flood score for CTCP's.
 * This should be in the config file.
 */

incStat("CORE.CTCP");

if (isIgnored(theClient)) return ;

if (hasFlooded(theClient, "CTCP"))
	{
	return ;
	}

setFloodPoints(theClient, getFloodPoints(theClient) + 5 );
ipFloodMap[theClient->getIP()]+=5;

StringTokenizer st( CTCP ) ;
if( st.empty() )
	{
	return ;
	}

const string Command = string_upper(st[0]);

if(Command == "PING" || Command=="ECHO")
	{
	xClient::DoCTCP(theClient, CTCP, Message);
	}
else if(Command == "GENDER")
	{
	xClient::DoCTCP(theClient, CTCP,
		"Tried to be a man again - there was a slip - now I'm an IT");
	}
else if(Command == "VERSION")
	{
	xClient::DoCTCP(theClient, CTCP,
		"Undernet P10 Channel Services II ["
		__DATE__ " " __TIME__
		"] Release 1.4.1");
	}
else if(Command == "PROBLEM?")
	{
	xClient::DoCTCP(theClient, CTCP.c_str(), "Blame Bleep!");
	}
else if(Command == "PUSHER")
	{
	xClient::DoCTCP(theClient, CTCP.c_str(), "Pak, Chooie, Unf.");
	}
else if(Command == "SOUND")
	{
	xClient::DoCTCP(theClient, CTCP.c_str(), "I'm deaf, remember?");
	}
else if(Command == "DCC")
	{
	xClient::DoCTCP(theClient, CTCP.c_str(), "REJECT");
	}
else if(Command == "FLOOD")
	/* Ignored */;
else if(Command == "PAGE")
	{
	xClient::DoCTCP(theClient, CTCP.c_str(), "I'm always here, no need to page");
	}
else if(Command == "USERINFO")
	{
	xClient::DoCTCP(theClient, CTCP.c_str(), "I'm a user, not an abuser");
	}
else if(Command == "TIME")
	{
	xClient::DoCTCP(theClient, CTCP.c_str(), "Time you got a watch?");
	}
else
	{
	xClient::DoCTCP(theClient, "ERRMSG", CTCP.c_str());
	}
}

/**
 * Check to see if this user is 'forced' onto this channel.
 */
unsigned short cservice::isForced(sqlChannel* theChan, sqlUser* theUser)
{
if (!theChan->forceMap.empty())
	{
	sqlChannel::forceMapType::iterator ptr = theChan->forceMap.find(theUser->getID());
	if(ptr != theChan->forceMap.end())
		{
		/* So they do, return their forced level. */
		return ptr->second.first;
		}
	}

return 0;
}

/**
 *  Confirms a user is logged in by returning a pointer to
 *  the sqlUser record.
 *  If 'alert' is true, send a notice to the user informing
 *  them that they must be logged in.
 */
sqlUser* cservice::isAuthed(iClient* theClient, bool alert)
{
networkData* tmpData =
	static_cast< networkData* >( theClient->getCustomData(this) ) ;

if(!tmpData)
	{
	return 0;
	}

//assert( tmpData != 0 ) ;

sqlUser* theUser = tmpData->currentUser;

if( theUser )
	{
	return theUser;
	}

if( alert )
	{
	Notice(theClient,
		"Sorry, You must be logged in to use this command.");
	}
return 0;
}

/**
 *  Locates a cservice user record by 'id', the username of this user.
 */
sqlUser* cservice::getUserRecord(const string& id)
{
/*
 *  Check if this is a lookup by nick
 */
if (id[0]=='=')
	{
	const char* theNick = id.c_str();
	// Skip the '='
	++theNick;

	iClient *client = Network->findNick(theNick);
	if (client) return isAuthed(client,false);

	return 0;
	}
/*
 *  Check if this record is already in the cache.
 */

sqlUserHashType::iterator ptr = sqlUserCache.find(id);
if(ptr != sqlUserCache.end())
	{
	// Found something!
	#ifdef LOG_CACHE_HITS
		elog	<< "cmaster::getUserRecord> Cache hit for "
			<< id
			<< endl;
	#endif

	ptr->second->setLastUsed(currentTime());
	userCacheHits++;
	return ptr->second ;
	}

/*
 *  We didn't find anything in the cache, fetch the data from
 *  the backend and create a new sqlUser object.
 */

sqlUser* theUser = new (std::nothrow) sqlUser(SQLDb);
assert( theUser != 0 ) ;

if (theUser->loadData(id))
	{
 	sqlUserCache.insert(sqlUserHashType::value_type(id, theUser));

	#ifdef LOG_SQL
		elog	<< "cmaster::getUserRecord> There are "
			<< sqlUserCache.size()
			<< " elements in the cache."
		<< endl;
	#endif

	userHits++;

	// Return the new user to the caller
	theUser->setLastUsed(currentTime());
	return theUser;
	}
else
	{
	delete theUser ;
	}

return 0;
}

/**
 *  Locates a channel record by 'id', the channel name.
 */
sqlChannel* cservice::getChannelRecord(const string& id)
{

/*
 *  Check if this record is already in the cache.
 */

sqlChannelHashType::iterator ptr = sqlChannelCache.find(id);
if(ptr != sqlChannelCache.end())
	{
	channelCacheHits++;
	ptr->second->setLastUsed(currentTime());

	// Return the channel to the caller
	return ptr->second ;
	}

/*
 *  We didn't find anything in the cache.
 */

return 0;
}

/**
 *  Loads a channel from the cache by 'id'.
 */
sqlChannel* cservice::getChannelRecord(int id)
{

/*
 *  Check if this record is already in the cache.
 */

sqlChannelIDHashType::iterator ptr = sqlChannelIDCache.find(id);
if(ptr != sqlChannelIDCache.end())
	{
	channelCacheHits++;
	ptr->second->setLastUsed(currentTime());

	// Return the channel to the caller
	return ptr->second ;
	}

/*
 *  We didn't find anything in the cache.
 */

return 0;
}


sqlLevel* cservice::getLevelRecord( sqlUser* theUser, sqlChannel* theChan )
{
// Check if the record is already in the cache.
pair<int, int> thePair( theUser->getID(), theChan->getID() );

sqlLevelHashType::iterator ptr = sqlLevelCache.find(thePair);
if(ptr != sqlLevelCache.end())
	{
	// Found something!
	#ifdef LOG_CACHE_HITS
		elog	<< "cmaster::getLevelRecord> Cache hit for "
			<< "user-id:chan-id "
			<< theUser->getID() << ":"
			<< theChan->getID()
			<< endl;
	#endif

	levelCacheHits++;
	ptr->second->setLastUsed(currentTime());
	return ptr->second ;
	}

/*
 *  We didn't find anything in the cache.
 */

return 0;
}

/**
 * Check if a user has already passed IPR checks
 */
bool cservice::passedIPR(iClient* theClient)
{
	networkData* tmpData = static_cast< networkData* >(theClient->getCustomData(this));

	/* if the user has no network data, the haven't passed IPR */
	if (!tmpData)
		return false;

	if (tmpData->ipr_ts > 0)
		return true;

	return false;
}

/**
 * Set the client's IPR timestamp (can also be used to clear it)
 */

void cservice::setIPRts(iClient* theClient, unsigned int _ipr_ts)
{
	networkData* tmpData = static_cast< networkData* >(theClient->getCustomData(this));

	if (!tmpData)
		return;

	/* set the timestamp */
	tmpData->ipr_ts = _ipr_ts;

	return;
}

/**
 *  Check a user against IP restrictions
 */
bool cservice::checkIPR( iClient* theClient, sqlUser* theUser )
{
	stringstream theQuery;
	theQuery	<< "SELECT allowmask,allowrange1,allowrange2,added FROM "
			<< "ip_restrict WHERE user_id = "
			<< theUser->getID()
			<< ends;
#ifdef LOG_SQL
	elog	<< "cservice::checkIPR::sqlQuery> "
		<< theQuery.str().c_str()
		<< endl;
#endif

	if( !SQLDb->Exec(theQuery, true ) )
//	if (PGRES_TUPLES_OK != status)
	{
		/* SQL error, fail them */
		elog    << "cservice::checkIPR> SQL Error: "
			<< SQLDb->ErrorMessage()
			<< endl;
		return false;
        }
	if (SQLDb->Tuples() < 1)
	{
#ifdef IPR_DEFAULT_REJECT
		/* no entries, fail them */
		return false;
#else
		/* no entries, allow them */
		return true;
#endif
	}
	/* cycle through results to find a match */
	bool ipr_match = false;
	unsigned int ipr_ts = 0;
	unsigned int tmpIP = xIP(theClient->getIP()).GetLongIP();
	for (unsigned int i=0; i < SQLDb->Tuples(); i++)
	{
		/* get some variables out of the db row */
		std::string ipr_allowmask = SQLDb->GetValue(i, 0);
		unsigned int ipr_allowrange1 = atoi(SQLDb->GetValue(i, 1).c_str());
		unsigned int ipr_allowrange2 = atoi(SQLDb->GetValue(i, 2).c_str());
		ipr_ts = atoi(SQLDb->GetValue(i, 3).c_str());

		/* is this an IP range? */
		if (ipr_allowrange2 > 0)
		{
			/* yes it is, is the client IP between range1 and range2? */
			if ((tmpIP >= ipr_allowrange1) && (tmpIP <= ipr_allowrange2))
			{
				ipr_match = true;
				break;
			}
		} else {
			/* no, is it a single IP? */
			if (ipr_allowrange1 > 0)
			{
				/* yes it is, does the IP match range1? */
				if (tmpIP == ipr_allowrange1)
				{
					ipr_match = true;
					break;
				}
			} else {
				/* no, is it a hostmask? */
				if (ipr_allowmask.size() > 0)
				{
					/* yes it is, does it match our hostname? */
					if (!match(ipr_allowmask, theClient->getRealInsecureHost()) ||
						!match(ipr_allowmask, xIP(theClient->getIP()).GetNumericIP()))
					{
						ipr_match = true;
						break;
					}
				} else {
					/* no, fail */
				}
			}
		}
	}
	/* check if we found a match yet */
	if (!ipr_match)
	{
		/* no match, fail them */
		return false;
	} else {
		/* IP restriction check passed - mark it against this user */
		setIPRts(theClient, ipr_ts);
		return true;
	}
}

/**
 * Fetch the number of failed logins for a client
 */
unsigned int cservice::getFailedLogins(iClient* theClient)
{
	networkData* tmpData = static_cast< networkData* >(theClient->getCustomData(this));

	/* if the user has no network data, they have zero failed logins */
	if (!tmpData)
		return 0;

	return tmpData->failed_logins;
}

/**
 * Set the number of failed logins for a client
 */
void cservice::setFailedLogins(iClient* theClient, unsigned int _failed_logins)
{
	networkData* tmpData = static_cast< networkData* >(theClient->getCustomData(this));

	/* if the user has no network data, we cant set it */
	if (!tmpData)
		return;

	/* set the failed login counter */
	tmpData->failed_logins = _failed_logins;

	return;
}

/**
 *  Returns true or false to whether IPR checks are required.
 */
bool cservice::needIPRcheck(sqlUser* theUser)
{
	/* don't need to check in the case of alumni */
	if (theUser->getFlag(sqlUser::F_ALUMNI))
		return 0;
	/* check if they have access to '*' */
	sqlChannel* theChan = getChannelRecord("*");
	if (!theChan)
	{
		elog	<< "cservice::needIPRcheck> Unable to "
			<< "locate channel '*'!"
			<< endl ;
		::exit(0);
	}
	sqlLevel* theLevel = getLevelRecord(theUser, theChan);
	if (theLevel)
	{
		if (theLevel->getAccess() > 0)
			return true;
	}
	/* if we reach here, no IPR checks are needed */
	return false;
}

/**
 *  Returns the admin access level a particular user has.
 */
short int cservice::getAdminAccessLevel( sqlUser* theUser, bool verify )
{

/*
 *  First thing, check if this ACCOUNT has been globally
 *  suspended.
 */
if (theUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
	{
	return 0;
	}

if (theUser->getFlag(sqlUser::F_NOADMIN))
	{
	return 0;
	}

/* Are they an alumni (check except for 'verify' command) ? */
if (!verify && (theUser->getFlag(sqlUser::F_ALUMNI)))
	return 0;

sqlChannel* theChan = getChannelRecord("*");
if (!theChan)
	{
	elog	<< "cservice::getAdminAccessLevel> Unable to "
		<< "locate channel '*'! Sorry, I can't continue.."
		<< endl;
	::exit(0);
	}

sqlLevel* theLevel = getLevelRecord(theUser, theChan);
if(theLevel)
	{
	/* check if they have been suspended! */
	if (theLevel->getSuspendExpire() > currentTime())
		return 0;
	return theLevel->getAccess();
	}

// By default, users have level 0 admin access.
return 0;
}

/**
 *  Returns the coder access level a particular user has.
 */
short int cservice::getCoderAccessLevel( sqlUser* theUser )
{
if (theUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
	{
	return 0;
	}

sqlChannel* theChan = getChannelRecord(coderChan);
if (!theChan)
	{
	elog	<< "cservice::getAdminAccessLevel> Unable to "
		<< "locate channel '"
		<< coderChan.c_str()
		<< "'! Sorry, I can't continue.."
		<< endl;
	::exit(0);
	}

sqlLevel* theLevel = getLevelRecord(theUser, theChan);
if(theLevel)
	{
	return theLevel->getAccess();
	}

// By default, users have level 0 admin access.
return 0;
}

/**
 * Returns the access level a particular user has on a particular
 * channel taking into account channel & user level suspensions.
 * Also used to return the level of access granted to a forced access.
 *
 * Usage: When determining if we should grant a permission to a user
 * to access a particular command/function.
 * To determine the effect access level of a target user.
 */
short int cservice::getEffectiveAccessLevel( sqlUser* theUser,
	sqlChannel* theChan, bool notify )
{

/*
 *  First thing, check if this ACCOUNT has been globally
 *  suspended.
 */

if (theUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
	{
	if (theUser->isAuthed() && notify)
		{
		noticeAllAuthedClients(theUser, "Your account has been suspended.");
		}
	return 0;
	}

/*
 *  Have a look to see if this user has forced some access.
 */

unsigned short forcedAccess = isForced(theChan, theUser);
if (forcedAccess)
	{
	return forcedAccess;
	}

sqlLevel* theLevel = getLevelRecord(theUser, theChan);
if( !theLevel )
	{
	return 0 ;
	}

/* Then, check to see if the channel has been suspended. */

if (theChan->getFlag(sqlChannel::F_SUSPEND))
	{
	/* Send them a notice to let them know they've been bad? */
	if (theUser->isAuthed() && notify)
		{
		noticeAllAuthedClients(theUser, "The channel %s has been suspended by a cservice administrator.",
			theChan->getName().c_str());
		}
	return 0;
	}

/*
 *  Check to see if this particular access record has been
 *  suspended too.
 */

if (theLevel->getSuspendExpire() > currentTime())
	{
	// Send them a notice.
	if (theUser->isAuthed() && notify)
		{
		noticeAllAuthedClients(theUser, "Your access on %s has been suspended.",
			theChan->getName().c_str());
		}
	return 0;
	}

/*  We need to use getAdminAccessLevel in case the check is made against *,
 *  otherwise userflags (like ALUMNI) are not taken into account.
 */

sqlChannel* adminChan = getChannelRecord("*");
if (theChan == adminChan)
	{
	return getAdminAccessLevel(theUser, false);
	}


return theLevel->getAccess();
}

/**
 *  Returns the access level a particular user has on a particular
 *  channel. Plain and simple. If the user has 500 in the channel
 *  record, this function returns 500.
 */
short int cservice::getAccessLevel( sqlUser* theUser,
	sqlChannel* theChan )
{
sqlLevel* theLevel = getLevelRecord(theUser, theChan);
if(theLevel)
	{
	return theLevel->getAccess();
 	}

/* By default, users have level 0 access on a channel. */
return 0;
}

/**
 * Returns the help message for the specified topic in
 * this user's prefered language
 */
const string cservice::getHelpMessage(sqlUser* theUser, string topic)
{
	int lang_id = 1;

	if (theUser)
		lang_id = theUser->getLanguageId();

	pair <int, string> thePair(lang_id, topic);
	helpTableType::iterator ptr = helpTable.find(thePair);
	if (ptr != helpTable.end())
		return ptr->second;

	if (lang_id != 1)
		return getHelpMessage(NULL, topic);

	return string("");
}

/**
 *  Execute an SQL query to retrieve all the help messages.
 */
void cservice::loadHelpTable()
{
if( SQLDb->Exec("SELECT language_id,topic,contents FROM help", true ) )
//if (PGRES_TUPLES_OK == status)
	for (unsigned int i = 0; i < SQLDb->Tuples(); i++)
		helpTable.insert(helpTableType::value_type(std::make_pair(
			atoi(SQLDb->GetValue(i, 0).c_str()),
			SQLDb->GetValue(i, 1)),
			SQLDb->GetValue(i, 2)));

#ifdef LOG_SQL
	elog	<< "*** [CMaster::loadHelpTable]: Loaded "
			<< helpTable.size()
			<< " help messages."
			<< endl;
#endif
}

/**
 * Returns response id "response_id" for this user's prefered
 * language.
 */
const string cservice::getResponse( sqlUser* theUser, int response_id,
	string msg )
{

// Language defaults to English
int lang_id = 1;

if (theUser)
	{
	lang_id = theUser->getLanguageId();
	}

pair<int, int> thePair( lang_id, response_id );

translationTableType::iterator ptr = translationTable.find(thePair);
if(ptr != translationTable.end())
	{
	/* Found something! */
	return ptr->second ;
	}

/*
 * Can't find this response Id within a valid language.
 * Realistically we should bomb here, however it might be wise
 * to 'fallback' to a lower language ID and try again, only bombing if we
 * can't find an english variant. (Carrying on here could corrupt
 * numerous varg lists, and will most likely segfault anyway).
 */
if (lang_id != 1)
	{
	pair<int, int> thePair( 1, response_id );
	translationTableType::iterator ptr = translationTable.find(thePair);
	if(ptr != translationTable.end())
		return ptr->second ;
	}

if( !msg.empty() )
	{
	return msg;
	}

return string( "Unable to retrieve response. Please contact a cservice "
	"administrator." ) ;
}

/**
 *  Execute an SQL query to retrieve all the translation data.
 */
void cservice::loadTranslationTable()
{
if( SQLDb->Exec("SELECT id,code,name FROM languages", true ) )
//if (PGRES_TUPLES_OK == status)
	for (unsigned int i = 0; i < SQLDb->Tuples(); i++)
		languageTable.insert(languageTableType::value_type(SQLDb->GetValue(i, 1),
			std::make_pair(atoi(SQLDb->GetValue(i, 0).c_str()),
				SQLDb->GetValue(i, 2))));

#ifdef LOG_SQL
	elog	<< "*** [CMaster::loadTranslationTable]: Loaded "
			<< languageTable.size()
			<< " languages."
			<< endl;
#endif

if( SQLDb->Exec(
	"SELECT language_id,response_id,text FROM translations", true ) )
//if( PGRES_TUPLES_OK == status )
	{
	for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
		/*
		 *  Add to our translations table.
		 */

		int lang_id = atoi(SQLDb->GetValue( i, 0 ).c_str());
		int resp_id = atoi(SQLDb->GetValue( i, 1 ).c_str());

		pair<int, int> thePair( lang_id, resp_id ) ;

		translationTable.insert(
			translationTableType::value_type(
				thePair, SQLDb->GetValue( i, 2 )) );
		}
	}

#ifdef LOG_SQL
	elog	<< "*** [CMaster::loadTranslationTable]: Loaded "
		<< translationTable.size()
		<< " translations."
		<< endl;
#endif

}

int cservice::rehashMOTD() {
/* 
 * Trying to rehash the MOTD.
 * If it succeeds, we return when it was last adjusted. 
 */

int lang_id = 1;
pair<int, int> thePair( lang_id, language::motd );
translationTableType::iterator ptr = translationTable.find(thePair);
if(ptr != translationTable.end())
	{
	translationTable.erase(thePair);
	stringstream selectQuery;
	selectQuery	<< "SELECT text,last_updated FROM "
        	        << "translations WHERE language_id = "
			<< lang_id
			<< "AND response_id = "
			<< language::motd
			<< ends;
	if( SQLDb->Exec( selectQuery, true ) ) 
		{
		translationTable.insert(translationTableType::value_type(thePair, SQLDb->GetValue(0,0)) );
		int last_updated = atoi(SQLDb->GetValue(0,1));
		return last_updated;			
		}
	}
return 0;
}


bool cservice::isOnChannel( const string& /* chanName */ ) const
{
return true;
}

/**
 * This member function executes an SQL query to return all
 * suspended access level records, and unsuspend them.
 * TODO
 */
void cservice::expireSuspends()
{
#ifdef LOG_DEBUG
elog	<< "cservice::expireSuspends> Checking for expired Suspensions.."
	<< endl;
#endif
time_t expiredTime = currentTime();
stringstream expireQuery;
expireQuery	<< "SELECT user_id,channel_id FROM levels "
		<< "WHERE suspend_expires <= "
		<< expiredTime
		<< " AND suspend_expires <> 0"
		<< ends;

#ifdef LOG_SQL
	elog	<< "expireSuspends::sqlQuery> "
		<< expireQuery.str().c_str()
		<< endl;
#endif

if( !SQLDb->Exec(expireQuery, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "cservice::expireSuspends> SQL Error: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return ;
	}

/*
 *  Loops over the results set, and attempt to locate
 *  this level record in the cache.
 */

#ifdef LOG_SQL
	elog	<< "cservice::expireSuspends> Found "
		<< SQLDb->Tuples()
		<< " expired suspensions."
		<< endl;
#endif

/*
 *  Place our query results into temporary storage, because
 *  we might have to execute other queries inside the
 *  loop which will invalidate our results set.
 */

typedef vector < pair < string, string > > expireVectorType;
expireVectorType expireVector;

for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
	{
	expireVector.push_back(expireVectorType::value_type(
		SQLDb->GetValue(i, 0),
		SQLDb->GetValue(i, 1) )
		);
	}

for (expireVectorType::const_iterator resultPtr = expireVector.begin();
	resultPtr != expireVector.end(); ++resultPtr)
		{
		/*
		 * Attempt to find this level record in the cache.
		 */
		pair<int, int> thePair( atoi(resultPtr->first.c_str()),
			atoi(resultPtr->second.c_str()) );

		sqlLevelHashType::iterator Lptr
			= sqlLevelCache.find(thePair);
		if(Lptr != sqlLevelCache.end())
			{
			/* Found it in the cache, remove suspend. */
#ifdef LOG_CACHE_HITS
			elog	<< "cservice::expireSuspends> "
				<< "Found level record in cache: "
				<< resultPtr->first
				<< ":"
				<< resultPtr->second
				<< endl;
#endif
			(Lptr->second)->setSuspendExpire(0);
			(Lptr->second)->setSuspendBy(string());
			}

		/*
		 *  Execute a query to update the status in the db.
		 */

		} // for()
stringstream updateQuery;
updateQuery << "UPDATE levels SET suspend_expires = "
	    << "0, suspend_by = '' WHERE suspend_expires <= "
	    << expiredTime 
	    << " and suspend_expires <> 0";

#ifdef LOG_SQL
	elog	<< "expireSuspends::sqlQuery> "
		<< updateQuery.str().c_str()
		<< endl;
#endif

if( !SQLDb->Exec(updateQuery ) )
//if( status != PGRES_COMMAND_OK)
	{
	elog	<< "cservice::expireSuspends> Unable to "
		<< "update record while unsuspending."
		<< endl;
	}


}

/**
 * This function removes any ignores that have expired.
 */
void cservice::expireSilence()
{

silenceListType::iterator ptr = silenceList.begin();
while (ptr != silenceList.end())
	{
	if ( ptr->second.first < currentTime() )
		{
		string theMask = ptr->first;
		stringstream s;
		s	<< getCharYYXXX()
			<< " SILENCE "
			<< "*"
			<< " -"
			<< theMask
			<< ends;
		Write( s );

		/*
		 * Locate this user by numeric.
		 * If the numeric is still in use, clear the ignored flag.
		 * If someone else has inherited this numeric, no prob,
		 * its cleared anyway.
		 */

		iClient* theClient = Network->findClient(ptr->second.second);
		if (theClient)
			{
				setIgnored(theClient, false);
			}

		++ptr;
		silenceList.erase(theMask);
		} else {
			++ptr;
		}

	} // while()

}

/**
 * This member function executes an SQL query to return all
 * bans that have expired. It removes them from the internal
 * cache as well as the database.
 */
void cservice::expireBans()
{
#ifdef LOG_DEBUG
elog	<< "cservice::expireBans> Checking for expired bans.."
	<< endl;
#endif
time_t expiredTime = currentTime();
stringstream expireQuery;
expireQuery	<< "SELECT channel_id,id FROM bans "
		<< "WHERE expires <= "
		<< expiredTime
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlQuery> "
		<< expireQuery.str().c_str()
		<< endl;
#endif

if( !SQLDb->Exec(expireQuery, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "cservice::expireBans> SQL Error: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return ;
	}

/*
 *  Loops over the results set, and attempt to locate
 *  this ban in the cache.
 */

#ifdef LOG_SQL
	elog	<< "cservice::expireBans> Found "
		<< SQLDb->Tuples()
		<< " expired bans."
		<< endl;
#endif

/*
 *  Place our query results into temporary storage, because
 *  we might have to execute other queries inside the
 *  loop which will invalidate our results set.
 */
typedef vector < pair < unsigned int, unsigned int > > expireVectorType;
expireVectorType expireVector;

for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
	{
	expireVector.push_back(expireVectorType::value_type(
		atoi(SQLDb->GetValue(i, 0).c_str()),
		atoi(SQLDb->GetValue(i, 1).c_str()) )
		);
	}

for (expireVectorType::const_iterator resultPtr = expireVector.begin();
	resultPtr != expireVector.end(); ++resultPtr)
	{
	sqlChannel* theChan = getChannelRecord( resultPtr->first );
	if (!theChan)
		{
		// TODO: Debuglog.
		continue;
		}

	#ifdef LOG_DEBUG
		elog	<< "Checking bans for "
			<< theChan->getName()
			<< endl;
	#endif

	/* Attempt to find the ban according to its id */
	map< int,sqlBan* >::iterator ptr =  
		theChan->banList.find(resultPtr->second);

	/* Was a ban found ? */
	if (ptr != theChan->banList.end())
		{
		sqlBan* theBan = ptr->second;
		theChan->banList.erase(ptr);

		Channel* tmpChan = Network->findChannel(
			theChan->getName());
		if (tmpChan)
			{
			UnBan(tmpChan, theBan->getBanMask());
			}

		#ifdef LOG_DEBUG
			elog	<< "Cleared Ban "
				<< theBan->getBanMask()
				<< " from cache"
				<< endl;
		#endif

		delete(theBan);
		}
	else
		{
		// BUG: Correct me if Im wrong, but this will crash, 
		// right?
		sqlBan* theBan = ptr->second;
		elog << "Unable to find ban "
		     << theBan->getBanMask()
		     << " with id " 
		     << theBan->getID()
		     << endl;
		}     
	} /* Forall results in set */

stringstream deleteQuery;
deleteQuery	<< "DELETE FROM bans "
		<< "WHERE expires <= "
		<< expiredTime
		<< ends;

#ifdef LOG_SQL
	elog	<< "sqlQuery> "
		<< deleteQuery.str()
		<< endl;
#endif

if( !SQLDb->Exec(deleteQuery ) )
//if( PGRES_COMMAND_OK != status )
	{
	elog	<< "cservice::expireBans> SQL Error: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return ;
	}

}

/**
 * This function iterates over the usercache, and removes
 * accounts not accessed in a certain timeframe.
 * N.B: do *NOT* expire those with a networkClient set.
 * (Ie: those currently logged in).
 */
void cservice::cacheExpireUsers()
{
	logDebugMessage("Beginning User cache cleanup:");
	sqlUserHashType::iterator ptr = sqlUserCache.begin();
	sqlUser* tmpUser;
	clock_t startTime = ::clock();
	clock_t endTime = 0;
	int purgeCount = 0;
	int updateCount = 0;
	string removeKey;

	while (ptr != sqlUserCache.end())
	{
		tmpUser = ptr->second;
		/*
		 * Have a quick look if this person has been logged in more than 24 hrs.
		 * If so, update the last-seen so their account doesn't expire after xx days. ;)
		 */
		if ( tmpUser->isAuthed() && ((tmpUser->getInstantiatedTS() + 86400) < ::time(NULL))  )
		{
			/* check to see if we have a last seen time (bug workaround) - if not, make one */
       
			stringstream queryString;
			queryString	<< "SELECT last_seen FROM users_lastseen WHERE user_id="
					<< tmpUser->getID()
					<< ends;
#ifdef LOG_SQL
			elog	<< "cservice::cacheExpireUsers::sqlQuery> "
				<< queryString.str().c_str()
				<< endl; 
#endif

			if( SQLDb->Exec(queryString, true ) )
//			if (PGRES_TUPLES_OK == status)
			{
				if (SQLDb->Tuples() < 1)
				{
					/* no rows returned - create a dummy record that will be updated
					 * by setLastSeen after this loop
					 */
					stringstream updateQuery;
					updateQuery	<< "INSERT INTO users_lastseen (user_id,"
							<< "last_seen,last_updated) VALUES("
							<< tmpUser->getID()
							<< ",now()::abstime::int4,now()::abstime::int4)"
							<< ends;

#ifdef LOG_SQL
					elog	<< "cservice::cacheExpireUsers::sqlQuery> "
						<< updateQuery
						<< endl;
#endif
					SQLDb->Exec(updateQuery.str());
				}
			}
			/* update their details */
			tmpUser->setLastSeen(currentTime());
			tmpUser->setInstantiatedTS(::time(NULL));
			updateCount++;
		}

		/*
	 	 *  If this user has been idle one hour, and currently
		 *  isn't logged in, boot him out the window.
		 */
		if ( ((tmpUser->getLastUsed() + 3600) < currentTime()) &&
			!tmpUser->isAuthed() )
		{
#ifdef LOG_DEBUG
			elog << "cservice::cacheExpireUsers> "
			<< tmpUser->getUserName()
			<< "; last used: "
			<< tmpUser->getLastUsed()
			<< endl;
#endif
			purgeCount++;
			removeKey = ptr->first;
			delete(ptr->second);
			/* Advance the iterator past the soon to be
			 * removed element. */
			++ptr;
			sqlUserCache.erase(removeKey);
		}
			else
		{
			++ptr;
		}
	}
	endTime = ::clock();
	logDebugMessage("User cache cleanup complete; Removed %i user records in %i ms.",
		purgeCount, (endTime - startTime) /  CLOCKS_PER_SEC);
	logDebugMessage("I also updated %i last_seen records for people logged in for >24 hours.",
		updateCount);
}

void cservice::cacheExpireLevels()
{
	logDebugMessage("Beginning Channel Level-cache cleanup:");

	/*
	 *  While we are at this, we'll clear out any FORCE'd access's
	 *  in channels.
	 */

	sqlChannelHashType::iterator ptr = sqlChannelCache.begin();
	while (ptr != sqlChannelCache.end())
	{
		sqlChannel* theChan = (ptr)->second;
		if(theChan->forceMap.size() > 0)
		{
			logDebugMessage("Clearing out %i FORCE(s) from channel %s",
						theChan->forceMap.size(), theChan->getName().c_str());
			theChan->forceMap.clear();
		}
		/*
		 * While we're here, lets see if this channel has been idle for a while.
		 * If so, we might want to part and turn off autojoin.. etc.
		 */

		if ( ((currentTime() - theChan->getLastUsed()) >= partIdleChan)
			&& theChan->getInChan()
			&& !theChan->getFlag(sqlChannel::F_SPECIAL) )
		{
			/*
			 * So long! and thanks for all the fish.
			 */

			theChan->setInChan(false);
			theChan->removeFlag(sqlChannel::F_AUTOJOIN);
			theChan->commit();
			joinCount--;
			writeChannelLog(theChan, me, sqlChannel::EV_IDLE, "");
			logDebugMessage("I've just left %s because its too quiet.",
					theChan->getName().c_str());
			Part(theChan->getName(), "So long! (And thanks for all the fish)");
		}

		++ptr;
	}
	logDebugMessage("Channel Level cache-cleanup complete.");
}

/**
 * This member function checks the reop buffer for any
 * pending reop's, performing them if neccessary.
 */
void cservice::performReops()
{
/* TODO: Rewrite this bit? --Gte */

if( reopQ.empty() )
	{
	return;
	}

reopQType::iterator ptr = reopQ.begin();
while ( ptr != reopQ.end() )
{
if (ptr->second <= currentTime())
	{
	Channel* tmpChan = Network->findChannel(ptr->first);
	if (tmpChan)
		{
		ChannelUser* tmpChanUser;
		tmpChanUser = tmpChan->findUser(me);

		/* Don't op ourself if we're already opped.. */
		if (tmpChanUser && !tmpChanUser->getMode(ChannelUser::MODE_O))
			{
			stringstream s;
			s	<< MyUplink->getCharYY()
				<< " M "
				<< tmpChan->getName()
				<< " +o "
				<< getCharYYXXX()
				<< " "
				<< tmpChan->getCreationTime()
				<< ends;

			Write( s );

			/*
			 *  Update the channel state.
			 */

			tmpChanUser->setMode(ChannelUser::MODE_O);

			#ifdef LOG_DEBUG
				elog	<< "cservice::OnTimer> REOP "
					<< tmpChan->getName()
					<< endl;
			#endif
			}

			/*
			 *  If STRICTOP or NOOP is set, do the 'right thing.
			 */

			sqlChannel* theChan = getChannelRecord(tmpChan->getName());
			if (theChan)
			{
				if(theChan->getFlag(sqlChannel::F_NOOP))
					{
					deopAllOnChan(tmpChan);
					}
				if(theChan->getFlag(sqlChannel::F_STRICTOP))
					{
					deopAllUnAuthedOnChan(tmpChan);
					}
				/*
				 * Send default modes.
				 */
				if (theChan->getChannelMode() != "")
					{
						/* use the xServer::Mode code to set these modes */
						MyUplink->Mode(this, tmpChan, theChan->getChannelMode().c_str(), std::string() );
					}
			}

		} /* If channel exists */
		reopQ.erase(ptr++->first);	/* erase pointer, also increment it */
	} else {
		++ptr;
	}
} /* While */

}

/**
 * Check the database to see if anything has changed.
 * TODO: Lots.
 */
void cservice::processDBUpdates()
{
	logDebugMessage("[DB-UPDATE]: Looking for changes:");
	updateChannels();
	updateUsers();
	updateLevels();
	updateBans();
	logDebugMessage("[DB-UPDATE]: Complete.");
}

void cservice::updateChannels()
{
stringstream theQuery ;

theQuery	<< "SELECT "
			<< sql::channel_fields
			<< ",now()::abstime::int4 as db_unixtime FROM "
			<< "channels WHERE last_updated >= "
			<< lastChannelRefresh
			<< " AND registered_ts <> 0"
			<< ends;

#ifdef LOG_SQL
elog	<< "*** [CMaster::processDBUpdates]:sqlQuery: "
		<< theQuery.str().c_str()
		<< endl;
#endif

if( !SQLDb->Exec(theQuery, true ) )
//if (status != PGRES_TUPLES_OK)
	{
	elog	<< "*** [CMaster::updateChannels]: SQL error: "
			<< SQLDb->ErrorMessage()
			<< endl;
	return;
	}

if (SQLDb->Tuples() <= 0)
	{
	/* Nothing to see here.. */
	return;
	}

/* Update our time offset incase things drift.. */
dbTimeOffset = atoi(SQLDb->GetValue(0,"db_unixtime").c_str()) - ::time(NULL);
unsigned int updates = 0;
unsigned int newchans = 0;

for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
	{
	sqlChannelHashType::iterator ptr =
		sqlChannelCache.find(SQLDb->GetValue(i, 1));

	if(ptr != sqlChannelCache.end())
		{
		/* Found something! Update it. */
		(ptr->second)->setAllMembers(i);
		updates++;
		}
	else
		{
		/*
		 * Not in the cache.. must be a new channel.
		 * Create new channel record, insert in cache.
		 */

		sqlChannel* newChan = new (std::nothrow) sqlChannel(SQLDb);
		assert( newChan != 0 ) ;

		newChan->setAllMembers(i);
		sqlChannelCache.insert(sqlChannelHashType::value_type(newChan->getName(), newChan));
		sqlChannelIDCache.insert(sqlChannelIDHashType::value_type(newChan->getID(), newChan));
		MyUplink->RegisterChannelEvent(newChan->getName(),this);
		logDebugMessage("[DB-UPDATE]: Found new channel: %s", newChan->getName().c_str());
		newchans++;
		}

	}

logDebugMessage("[DB-UPDATE]: Refreshed %i channel records, loaded %i new channel(s).",
	updates, newchans);

/* Set the "Last refreshed from channels table" timestamp. */
lastChannelRefresh = atoi(SQLDb->GetValue(0,"db_unixtime").c_str());
}

/*
 * Check the levels table for recent updates.
 */
void cservice::updateLevels()
{
stringstream theQuery ;

theQuery	<< "SELECT "
			<< sql::level_fields
			<< ",now()::abstime::int4 as db_unixtime FROM "
			<< "levels WHERE last_updated >= "
			<< lastLevelRefresh
			<< ends;

#ifdef LOG_SQL
elog	<< "*** [CMaster::updateLevels]: sqlQuery: "
		<< theQuery.str().c_str()
		<< endl;
#endif

if( !SQLDb->Exec(theQuery, true ) )
//if (status != PGRES_TUPLES_OK)
	{
	elog	<< "*** [CMaster::updateLevels]: SQL error: "
			<< SQLDb->ErrorMessage()
			<< endl;
	return;
	}

if (SQLDb->Tuples() <= 0)
	{
	/* Nothing to see here.. */
	return;
	}

/* Update our time offset incase things drift.. */
dbTimeOffset = atoi(SQLDb->GetValue(0,"db_unixtime").c_str()) - ::time(NULL);
unsigned int updates = 0;
unsigned int newlevs = 0;

for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
	{
	unsigned int channel_id = atoi(SQLDb->GetValue(i, 0).c_str());
	unsigned int user_id = atoi(SQLDb->GetValue(i, 1).c_str());
	sqlChannel* theChan = getChannelRecord(channel_id);

	/*
	 * If we don't have the channel cached, its not registered so
	 * we aren't interested in this level record.
	 */

	if (!theChan) continue;

	pair<int, int> thePair( user_id, channel_id );

	sqlLevelHashType::iterator ptr = sqlLevelCache.find(thePair);

	if(ptr != sqlLevelCache.end())
		{
		/* Found something! Update it. */
		(ptr->second)->setAllMembers(i);
		updates++;
		} else
		{
		/*
		 * Must be a new level record, add it.
		 */

		sqlLevel* newLevel = new (std::nothrow) sqlLevel(SQLDb);
		newLevel->setAllMembers(i);
		sqlLevelCache.insert(sqlLevelHashType::value_type(thePair, newLevel));
		newlevs++;
		}
	}

logDebugMessage("[DB-UPDATE]: Refreshed %i level record(s), loaded %i new level record(s).",
	updates, newlevs);

/* Set the "Last refreshed from levels table" timestamp. */
lastLevelRefresh = atoi(SQLDb->GetValue(0,"db_unixtime").c_str());
}

/*
 * Check the users table to see if there have been any updates since we last looked.
 */
void cservice::updateUsers()
{
	stringstream theQuery ;

	theQuery	<< "SELECT "
				<< sql::user_fields
				<< ",now()::abstime::int4 as db_unixtime FROM "
				<< "users WHERE last_updated >= "
				<< lastUserRefresh
				<< ends;

	#ifdef LOG_SQL
	elog	<< "*** [CMaster::updateUsers]: sqlQuery: "
			<< theQuery.str().c_str()
			<< endl;
	#endif

	if( !SQLDb->Exec(theQuery, true ) )
//	if (status != PGRES_TUPLES_OK)
		{
		elog	<< "*** [CMaster::updateUsers]: SQL error: "
				<< SQLDb->ErrorMessage()
				<< endl;
		return;
		}

	if (SQLDb->Tuples() <= 0)
		{
		/* Nothing to see here.. */
		return;
		}

	/* Update our time offset incase things drift.. */
	dbTimeOffset = atoi(SQLDb->GetValue(0,"db_unixtime").c_str()) - ::time(NULL);
	unsigned int updates = 0;

	for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
		sqlUserHashType::iterator ptr =
			sqlUserCache.find(SQLDb->GetValue(i, 1));

		if(ptr != sqlUserCache.end())
			{
			/* Found something! Update it */
			(ptr->second)->setAllMembers(i);
			updates++;
			}
		}

	logDebugMessage("[DB-UPDATE]: Refreshed %i user record(s).",
		updates);

	/* Set the "Last refreshed from Users table" timestamp. */
	lastUserRefresh = atoi(SQLDb->GetValue(0,"db_unixtime").c_str());
}

void cservice::updateBans()
{
/* Todo */
}

/**
 * Timer handler.
 * This member handles a number of timers, dispatching
 * control to the relevant member for the timer
 * triggered.
 */
void cservice::OnTimer(const xServer::timerID& timer_id, void*)
{
if (timer_id == limit_timerID)
	{
	updateLimits();

	/* Refresh Timers */
	time_t theTime = time(NULL) + limitCheckPeriod;
	limit_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
	}

if (timer_id == dBconnection_timerID)
	{
	checkDbConnectionStatus();
	performReops();

	/* Refresh Timers */
	time_t theTime = time(NULL) + connectCheckFreq;
	dBconnection_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
	}

if (timer_id ==  update_timerID)
	{
	processDBUpdates();

	/* Refresh Timer */
	time_t theTime = time(NULL) + updateInterval;
	update_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
	}

if (timer_id == expire_timerID)
	{
	expireBans();
	expireSuspends();
	expireSilence();

	/* Refresh Timer */
	time_t theTime = time(NULL) + expireInterval;
	expire_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
	}

if (timer_id == cache_timerID)
	{
	cacheExpireUsers();
	cacheExpireLevels();

	/* Refresh Timer */
	time_t theTime = time(NULL) + cacheInterval;
	cache_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
	}

if (timer_id == webrelay_timerID)
{
	/* Check for new webrelay messages */
	string webrelayQuery;

	webrelayQuery = "SELECT created_ts,contents FROM webnotices WHERE ";
	webrelayQuery += "created_ts <= now()::abstime::int4 ";
	webrelayQuery += "ORDER BY created_ts";
#ifdef LOG_SQL
	elog	<< "cservice::OnTimer::sqlQuery> "
		<< webrelayQuery.c_str()
		<< endl;
#endif
	int webrelay_messagecount = 0;

	if( SQLDb->Exec(webrelayQuery, true ) )
//	if (PGRES_TUPLES_OK == status)
	{
		/* process messages */
		webrelay_messagecount = SQLDb->Tuples();
		string webrelay_msg;
		unsigned long webrelay_ts = 0;
		for (int i=0; i < webrelay_messagecount; i++)
		{
			webrelay_ts = atoi(SQLDb->GetValue(i,0).c_str());
			webrelay_msg = SQLDb->GetValue(i,1);

			logAdminMessage("%s", webrelay_msg.c_str());
		}
		/* delete old messages */
		if (webrelay_messagecount > 0)
		{
			char web_ts[15];
			sprintf(web_ts, "%li", webrelay_ts);
			webrelayQuery = "DELETE FROM webnotices WHERE created_ts <= ";
			webrelayQuery += web_ts;
#ifdef LOG_SQL
			elog	<< "cservice::OnTimer::sqlQuery> "
				<< webrelayQuery.c_str()
				<< endl;
#endif
			if (!SQLDb->Exec(webrelayQuery) )
// PGRES_COMMAND_OK)
			{
				/* log error */
				elog	<< "cservice::webrelay> SQL Query Error: "
					<< SQLDb->ErrorMessage()
					<< endl;
			}
		}
	}

	/* Refresh Timer */
	time_t theTime = time(NULL) + webrelayPeriod;
	webrelay_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
}

if (timer_id == pending_timerID)
	{
	/*
	 * Load the list of pending channels and calculate/save some stats.
	 */

	loadPendingChannelList();

	/*
	 * Load a list of channels in NOTIFICATION stage and send them
	 * a notice.
	 */

	stringstream theQuery;
	theQuery	<<  "SELECT channels.name,channels.id,pending.created_ts"
				<< " FROM pending,channels"
				<< " WHERE channels.id = pending.channel_id"
				<< " AND pending.status = 2;"
				<< ends;

#ifdef LOG_SQL
		elog	<< "cmaster::loadPendingChannelList> "
			<< theQuery.str().c_str()
			<< endl;
#endif

	unsigned int noticeCount = 0;
	if( SQLDb->Exec(theQuery, true ) )
//	if( PGRES_TUPLES_OK == status )
		{
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
			{
			noticeCount++;
			string channelName = SQLDb->GetValue(i,0);
			unsigned int channel_id =
				atoi(SQLDb->GetValue(i, 1).c_str());
			unsigned int created_ts =
				atoi(SQLDb->GetValue(i, 2).c_str());
			Channel* tmpChan = Network->findChannel(channelName);

			if (tmpChan)
				{
				serverNotice(tmpChan,
				"This channel is currently being processed for registration. "
				"If you wish to view the details of the application or to object, please visit: "
				"%s?id=%i-%i", pendingPageURL.c_str(), created_ts, channel_id);
				}
			}
		}

	logDebugMessage("Loaded Pending Channels notification list, I have just notified %i channels that they are under registration.",
		noticeCount);

	/* Refresh Timer */

	time_t theTime = time(NULL) +pendingChanPeriod;
	pending_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
	}
}

/**
 * Send a notice to a channel from the server.
 * TODO: Move this method to xServer.
 */
bool cservice::serverNotice( Channel* theChannel, const char* format, ... )
{
char buf[ 1024 ] = { 0 } ;
va_list _list ;

va_start( _list, format ) ;
vsnprintf( buf, 1024, format, _list ) ;
va_end( _list ) ;

stringstream s;
s	<< MyUplink->getCharYY()
	<< " O "
	<< theChannel->getName()
	<< " :"
	<< buf
	<< ends;

Write( s );

return false;
}

/**
 * Send a notice to a channel from the server.
 * TODO: Move this method to xServer.
 */
bool cservice::serverNotice( Channel* theChannel, const string& Message)
{
stringstream s;
s	<< MyUplink->getCharYY()
	<< " O "
	<< theChannel->getName()
	<< " :"
	<< Message
	<< ends;

Write( s );

return false;
}

/**
 *  Log a message to the admin channel and the logfile.
 */
bool cservice::logAdminMessage(const char* format, ... )
{

char buf[ 1024 ] = { 0 } ;
va_list _list ;

va_start( _list, format ) ;
vsnprintf( buf, 1024, format, _list ) ;
va_end( _list ) ;

// Try and locate the relay channel.
//Channel* tmpChan = Network->findChannel(getConfigVar("CMASTER.RELAY_CHAN")->asString());
Channel* tmpChan = Network->findChannel(relayChan);
if (!tmpChan)
	{
	elog	<< "cservice::logAdminMessage> Unable to locate relay "
		<< "channel on network!"
		<< endl;
	return false;
	}

string message = string( "[" ) + nickName + "] " + buf ;
serverNotice(tmpChan, message);
return true;
}

/**
 * Log a privileged admin channel message
 */
bool cservice::logPrivAdminMessage(const char* format, ... )
{
	char buf[1024] = { 0 };
	va_list _list ;

	va_start(_list, format);
	vsnprintf(buf, 1024, format, _list);
	va_end(_list);

	/* try to locate the privileged relay channel */
	Channel* tmpChan = Network->findChannel(privrelayChan);
	if (!tmpChan)
	{
		elog	<< "cservice::logPrivAdminMessage> Unable to locate "
			<< "prileved relay channel on network!"
			<< endl;
		return false;
	}

	string message = string("[") + nickName + "] " + buf;
	serverNotice(tmpChan, message);
	return true;
}

bool cservice::logDebugMessage(const char* format, ... )
{

char buf[ 1024 ] = { 0 } ;
va_list _list ;

va_start( _list, format ) ;
vsnprintf( buf, 1024, format, _list ) ;
va_end( _list ) ;

// Try and locate the debug channel.
//Channel* tmpChan = Network->findChannel(getConfigVar("CMASTER.DEBUG_CHAN")->asString());
Channel* tmpChan = Network->findChannel(debugChan);
if (!tmpChan)
	{
	elog	<< "cservice::logAdminMessage> Unable to locate debug "
		<< "channel on network!"
		<< endl;
	return false;
	}

string message = string( "[" ) + nickName + "] " + buf ;
serverNotice(tmpChan, message);
return true;
}


string cservice::userStatusFlags( const string& theUser )
{

string flagString = "";

sqlUserHashType::iterator ptr = sqlUserCache.find(theUser);

if(ptr != sqlUserCache.end())
	{
	flagString = 'L';
	/* (depreciated)
	 * if(tmpUser->getFlag(sqlUser::F_LOGGEDIN)) flagString += 'U';
	 */
	}

return flagString;
}

const string cservice::prettyDuration( int duration ) const
{

// Pretty format a 'duration' in seconds to
// x day(s), xx:xx:xx.

char tmpBuf[ 64 ] = {0};

int	res = currentTime() - duration,
	secs = res % 60,
	mins = (res / 60) % 60,
	hours = (res / 3600) % 24,
	days = (res / 86400) ;

sprintf(tmpBuf, "%i day%s, %02d:%02d:%02d",
	days,
	(days == 1 ? "" : "s"),
	hours,
	mins,
	secs );

return string( tmpBuf ) ;
}

bool cservice::validUserMask(const string& userMask) const
{

// Check that a '!' exists, and that the nickname
// is no more than 15 characters
StringTokenizer st1( userMask, '!' ) ;
if( (st1.size() != 2) || (st1[ 0 ].size() > 15) )
	{
	return false ;
	}

// Check that a '@' exists and that the username is
// no more than 12 characters
StringTokenizer st2( st1[ 1 ], '@' ) ;

if( (st2.size() != 2) || (st2[ 0 ].size() > 12) )
	{
	return false ;
	}

// Be sure that the hostname is no more than 128 characters
if( st2[ 1 ].size() > 128 )
	{
	return false ;
	}

// Tests have passed
return true ;
}

void cservice::OnChannelModeO( Channel* theChan, ChannelUser* theChanUser,
	const xServer::opVectorType& theTargets)
{
/*
 * There are a number of things to do when we recieve a mode O for
 * a channel/targets.
 * Firstly, we check the status of certain channel flags is it set
 * NOOP? is it set STRICTOP?
 * We will only ever recieve events for channels that are registered.
 */

sqlChannel* reggedChan = getChannelRecord(theChan->getName());
if(!reggedChan)
	{
//	elog	<< "cservice::OnChannelModeO> WARNING, unable to "
//		<< "locate channel record"
//		<< " for registered channel event: "
//		<< theChan->getName()
//		<< endl;
	return;
	}

if(!reggedChan->getInChan()) 
	{
	//Do not monitor channels i am not in
	return;
	}
// List of clients to deop.
vector< iClient* > deopList;

// If we find a situation where we need to deop the person who has
// performed the mode, do so.
bool sourceHasBeenBad = false;

int deopCounter = 0;

for( xServer::opVectorType::const_iterator ptr = theTargets.begin() ;
	ptr != theTargets.end() ; ++ptr )
	{
	ChannelUser* tmpUser = ptr->second;
	bool polarity = ptr->first;

	if (polarity)
		{
		// If somebody is being opped.

		// If the channel is NOOP, deop everyone who tries to
		// get opped!
		if (reggedChan->getFlag(sqlChannel::F_NOOP))
			{
			if ( !tmpUser->getClient()->getMode(iClient::MODE_SERVICES) )
			deopList.push_back(tmpUser->getClient());
			}

		sqlUser* authUser = isAuthed(tmpUser->getClient(), false);

		// Has the target user's account been suspended?
		if (authUser && authUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
		{
			if (theChanUser) Notice(theChanUser->getClient(), "The user %s (%s) has been suspended by a CService Administrator.",
				authUser->getUserName().c_str(), tmpUser->getClient()->getNickName().c_str());
			deopList.push_back(tmpUser->getClient());
			sourceHasBeenBad = true;
		}

		// If the channel is STRICTOP, deop everyone who isn't
		// authenticated or and doesn't have access on the
		// channel.

		if (reggedChan->getFlag(sqlChannel::F_STRICTOP))
			{
			if (!authUser)
				{
				// Not authed, deop.
				if ( !tmpUser->getClient()->getMode(iClient::MODE_SERVICES) )
				deopList.push_back(tmpUser->getClient());
				sourceHasBeenBad = true;
				// Authed but doesn't have access... deop.
				}
			else if (!(getEffectiveAccessLevel(authUser,reggedChan, false) >= level::op))
				{
				if ( !tmpUser->getClient()->getMode(iClient::MODE_SERVICES) )
				deopList.push_back(tmpUser->getClient());
				sourceHasBeenBad = true;
				}
			}

		/*
		 *  The 'Fun' Part. Scan through channel bans to see if this hostmask
		 *  is 'banned' at 75 or below.
		 */

		sqlBan* theBan = isBannedOnChan(reggedChan, tmpUser->getClient());
		if( theBan && (theBan->getLevel() <= 75) )
			{
				if ( !tmpUser->getClient()->getMode(iClient::MODE_SERVICES) )
					{
					deopList.push_back(tmpUser->getClient());
					sourceHasBeenBad = true;

					/* Tell the person bein op'd that they can't */
					Notice(tmpUser->getClient(),
						"You are not allowed to be opped on %s",
						reggedChan->getName().c_str());

					/* Tell the person doing the op'ing this is bad */
					if (theChanUser)
						{
						Notice(theChanUser->getClient(),
							"%s isn't allowed to be opped on %s",
							tmpUser->getClient()->getNickName().c_str(),
							reggedChan->getName().c_str());
						}
					}
			}

		} // if()
	else
		{
		/* Somebody is being deopped? */
		deopCounter++;

		/* What if someone deop'd us?! */
		if (tmpUser->getClient() == me)
			{
			logAdminMessage("I've been deopped on %s!",
				reggedChan->getName().c_str());
			/* Add this chan to the reop queue, ready to op itself in 15 seconds. */
			reopQ.insert(cservice::reopQType::value_type(reggedChan->getName(),
				currentTime() + 15) );
			}
		}
	} // for()

/*
 *  Send notices and perform the deop's. (But don't deop anything thats +k).
 */

if (theChanUser && sourceHasBeenBad && !theChanUser->getClient()->getMode(iClient::MODE_SERVICES))
	deopList.push_back(theChanUser->getClient());

if( !deopList.empty() )
	{
	if ((theChanUser) && (reggedChan->getFlag(sqlChannel::F_NOOP)) )
		{
		Notice( theChanUser->getClient(),
			"The NOOP flag is set on %s",
			reggedChan->getName().c_str());
		}

	if ((theChanUser) && (reggedChan->getFlag(sqlChannel::F_STRICTOP)) )
		{
		Notice( theChanUser->getClient(),
			"The STRICTOP flag is set on %s",
			reggedChan->getName().c_str());
		}

	DeOp(theChan, deopList);
	}

/*
 *  Have more than 'maxdeoppro' been deopped?
 *  If so, suspend and kick 'em. (Unless they're +k of course ;)
 */

if ((theChanUser) && (deopCounter >= reggedChan->getMassDeopPro())
	&& (reggedChan->getMassDeopPro() > 0) && !theChanUser->getClient()->getMode(iClient::MODE_SERVICES))
	{
		doInternalBanAndKick(reggedChan, theChanUser->getClient(),
			"### MASSDEOPPRO TRIGGERED! ###");
	}
}

void cservice::OnEvent( const eventType& theEvent,
	void* data1, void* data2, void* data3, void* data4 )
{
switch( theEvent )
	{
	case EVT_XQUERY:
		{
		iServer* theServer = static_cast< iServer* >( data1 );
		const char* Routing = reinterpret_cast< char* >( data2 );
		const char* Message = reinterpret_cast< char* >( data3 );
		elog << "CSERVICE.CC: " << theServer->getName() << " " << Routing << " " << Message << endl;
		//As it is possible to run multiple GNUWorld clients on one server, first parameter should be a nickname.
		//If it ain't us, ignore the message, the message is probably meant for another client here.
		StringTokenizer st( Message ) ;
		if( st.size() < 2 )
        		{
			//No command or no nick supplied
        		break;
       			}
		if (st[0] == getNickName()) 
			{
			string Command = string_upper(st[1]);
			if (Command == "LOGIN")
				{
				doXQLogin(theServer, Routing, Message);		
				}
			}
		break;
		}
	case EVT_ACCOUNT:
		{
		iClient* tmpUser = static_cast< iClient* >( data1 ) ;
		networkData* tmpData = static_cast< networkData* >(tmpUser->getCustomData(this) ) ;
		/* Lookup this user account, if its not there.. trouble */
		sqlUser* theUser = getUserRecord(tmpUser->getAccount());
		if (theUser)
			{
			tmpData->currentUser = theUser;
			theUser->addAuthedClient(tmpUser);
			}
		break;
		}
	case EVT_BURST_ACK:
		{
//		iServer* theServer = static_cast< iServer* >( data1 );
//		if ( theServer == MyUplink->Uplink )
//			{
//			}
		break;
		}
	case EVT_QUIT:
	case EVT_KILL:
		{
		/*
		 *  We need to deauth this user if they're authed.
		 *  Also, clean up their custom data memory.
		 */

		iClient* tmpUser = (theEvent == EVT_QUIT) ?
			static_cast< iClient* >( data1 ) :
			static_cast< iClient* >( data2 ) ;

		sqlUser* tmpSqlUser = isAuthed(tmpUser, false);
		if (tmpSqlUser)
			{
			tmpSqlUser->removeAuthedClient(tmpUser);
			tmpSqlUser->removeFlag(sqlUser::F_LOGGEDIN);
#ifdef LOG_DEBUG
			elog	<< "cservice::OnEvent> Deauthenticated "
				<< "client: " << tmpUser << " from "
				<< "user: "
				<< tmpSqlUser->getUserName()
				<< endl;
#endif
			}

		// Clear up the custom data structure we appended to
		// this iClient.
		networkData* tmpData = static_cast< networkData* >(
			tmpUser->getCustomData(this) ) ;
		tmpUser->removeCustomData(this);

		delete(tmpData);
		customDataAlloc--;

		break ;
		} // case EVT_KILL/case EVT_QUIT

	case EVT_NICK:
		{
		/*
		 *  Give this new user a custom data structure!
		 */

		iClient* tmpUser =
			static_cast< iClient* >( data1 );
		networkData* newData = new (std::nothrow) networkData();
		assert( newData != 0 ) ;

		customDataAlloc++;

		// Not authed.. (yet!)
		newData->currentUser = NULL;
		tmpUser->setCustomData(this,
			static_cast< void* >( newData ) );

		/*
		 * Well.. they might be already auth'd.
		 * In which case, we'll receieve mode r and accountname for
		 * this person.
		 */
		if (tmpUser->isModeR())
			{
				/* Lookup this user account, if its not there.. trouble */
				sqlUser* theUser = getUserRecord(tmpUser->getAccount());
				if (theUser)
					{
					newData->currentUser = theUser;
					theUser->addAuthedClient(tmpUser);
					}
			}

		break;
		} // case EVT_NICK
	} // switch()
xClient::OnEvent( theEvent,
	data1, data2, data3, data4 ) ;
}

/**
 * Support function to deop all opped users on a channel.
 */
void cservice::deopAllOnChan(Channel* theChan)
{
if( !theChan )
	{
	/* Don't try this on a null channel. */
	return;
	}

sqlChannel* reggedChan = getChannelRecord(theChan->getName());

if (!reggedChan)
	{
	return;
	}

if (!reggedChan->getInChan())
	{
	return;
	}

/* Check we're actually opped first.. */

ChannelUser* tmpBotUser = theChan->findUser(getInstance());
if( !tmpBotUser || !tmpBotUser->getMode(ChannelUser::MODE_O) )
	{
	return;
	}

vector< iClient* > deopList;

for( Channel::const_userIterator ptr = theChan->userList_begin();
	ptr != theChan->userList_end() ; ++ptr )
	{
	if( ptr->second->getMode(ChannelUser::MODE_O))
		{

		/* Don't deop +k things */
		if ( !ptr->second->getClient()->getMode(iClient::MODE_SERVICES) )
			deopList.push_back( ptr->second->getClient() );

		} // If opped.
	}

if( !deopList.empty() )
	{
	DeOp(theChan, deopList);
	}

}

size_t cservice::countChanOps(const Channel* theChan)
{
if( !theChan )
	{
	/* Don't try this on a null channel. */
	return 0;
	}

size_t chanOps = 0;

for( Channel::const_userIterator ptr = theChan->userList_begin();
	ptr != theChan->userList_end() ; ++ptr )
	{
	if( ptr->second->getMode(ChannelUser::MODE_O))
		{
		chanOps++;
		} // If opped.
	}

return chanOps;
}

/**
 * Support function to deop all non authed opped users on a channel.
 */
void cservice::deopAllUnAuthedOnChan(Channel* theChan)
{
// TODO: assert( theChan != 0 ) ;

if( !theChan )
	{
	/* Don't try this on a null channel. */
	return;
	}

sqlChannel* reggedChan = getChannelRecord(theChan->getName());

if( !reggedChan || !reggedChan->getInChan() )
	{
	return;
	}

/* Check we're actually opped first.. */

ChannelUser* tmpBotUser = theChan->findUser(getInstance());
if(! tmpBotUser || !tmpBotUser->getMode(ChannelUser::MODE_O))
	{
	return;
	}

vector< iClient* > deopList;

for( Channel::const_userIterator ptr = theChan->userList_begin();
	ptr != theChan->userList_end() ; ++ptr )
	{
	if( ptr->second->getMode(ChannelUser::MODE_O))
		{
		/* Are they authed? */
		sqlUser* authUser = isAuthed(ptr->second->getClient(), false);

		if (!authUser)
			{
			/* Not authed, deop this guy + Don't deop +k things */
			if ( !ptr->second->getClient()->getMode(iClient::MODE_SERVICES) )
				{
				deopList.push_back( ptr->second->getClient() );
				}

		/* Authed but no access? Tough. :) */
			}
		else if ((reggedChan) && !(getEffectiveAccessLevel(authUser, reggedChan, false) >= level::op))
			{
			/* Don't deop +k things */
			if ( !ptr->second->getClient()->getMode(iClient::MODE_SERVICES) )
				{
				deopList.push_back( ptr->second->getClient() );
				}
			}

		} // if opped.
	} // forall users in channel.

if( !deopList.empty() )
	{
	DeOp(theChan, deopList);
	}

}

/**
 * Handler for registered channel events.
 * Performs a number of functions, autoop, autovoice, bankicks, etc.
 */
void cservice::OnChannelEvent( const channelEventType& whichEvent,
	Channel* theChan,
	void* data1, void* data2, void* data3, void* data4 )
{
iClient* theClient = 0 ;

switch( whichEvent )
	{
	case EVT_CREATE:
	case EVT_JOIN:
		{
		/*
		 * We should only ever recieve events for registered channels, or those
		 * that are 'pending'. If we do get past the pending check, there must be
		 * some kind of database inconsistancy.
		 */

		theClient = static_cast< iClient* >( data1 ) ;

		pendingChannelListType::iterator ptr = pendingChannelList.find(theChan->getName());

		if(ptr != pendingChannelList.end())
			{
			/*
			 * Firstly, is this join a result of a server bursting onto the network?
			 * If this is the case, its not a manual /join.
			 */

			iServer* theServer = Network->findServer( theClient->getIntYY() ) ;
			if (!theServer->isBursting())
				{
				/*
				 *  Yes, this channel is pending registration, update join count
				 *  and check out this user joining.
				 */

				ptr->second->join_count++;

				/*
				 *  Now, has this users IP joined this channel before?
				 *  If not - we keep a record of it.
				 */

				sqlPendingChannel::trafficListType::iterator Tptr =
					ptr->second->trafficList.find(theClient->getIP());

				sqlPendingTraffic* trafRecord;

				/*
				 * If we have more than 50 unique IP's join, we don't bother
				 * recording anymore.
				 */

				if (ptr->second->unique_join_count < 50)
					{
					if(Tptr == ptr->second->trafficList.end())
						{
						/* New IP, create and write the record. */

						trafRecord = new sqlPendingTraffic(SQLDb);
						trafRecord->ip_number = theClient->getIP();
						trafRecord->join_count = 1;
						trafRecord->channel_id = ptr->second->channel_id;
						trafRecord->insertRecord();

						ptr->second->trafficList.insert(sqlPendingChannel::trafficListType::value_type(
							theClient->getIP(), trafRecord));
#ifdef LOG_DEBUG
						logDebugMessage("Created a new IP traffic record for IP#%u (%s) on %s",
							theClient->getIP(), theClient->getNickUserHost().c_str(),
							theChan->getName().c_str());
#endif
						} else
						{
						/* Already cached, update and save. */
						trafRecord = Tptr->second;
						trafRecord->join_count++;
						//trafRecord->commit();
						}

						ptr->second->unique_join_count = ptr->second->trafficList.size();

						//logDebugMessage("New total for IP#%u on %s is %i",
						//	theClient->getIP(), theChan->getName().c_str(),
						//	trafRecord->join_count);
					}

				sqlUser* theUser = isAuthed(theClient, false);
				if (!theUser)
					{
					/*
					 *  If this user isn't authed, he can't possibly be flagged
					 *  as one of the valid supporters, so we drop out.
					 */

					xClient::OnChannelEvent( whichEvent, theChan,
						data1, data2, data3, data4 );
					return ;
					}

					/*
					 * Now, if this guy is a supporter, we bump his join count up.
					 */

					sqlPendingChannel::supporterListType::iterator Supptr = ptr->second->supporterList.find(theUser->getID());
					if (Supptr != ptr->second->supporterList.end())
						{
							Supptr->second++;
							ptr->second->commitSupporter(Supptr->first, Supptr->second);
#ifdef LOG_DEBUG
							logDebugMessage("New total for Supporter #%i (%s) on %s is %i.", theUser->getID(),
								theUser->getUserName().c_str(), theChan->getName().c_str(), Supptr->second);
#endif
						}

				xClient::OnChannelEvent( whichEvent, theChan,
					data1, data2, data3, data4 );
				return ;

				} /* Is server bursting? */
			} /* Is channel on pending list */

		sqlChannel* reggedChan = getChannelRecord(theChan->getName());
		if(!reggedChan)
			{
//			elog	<< "cservice::OnChannelEvent> WARNING, "
//				<< "unable to locate channel record"
//				<< " for registered channel event: "
//				<< theChan->getName()
//				<< endl;
			return ;
			}

		/* This is a registered channel, check it is set +R.
		 * If not, set it to +R (channel creation)
		 */
		if (!theChan->getMode(Channel::MODE_REG)) {
			stringstream tmpTS;
			tmpTS << reggedChan->getChannelTS();
			string channelTS = tmpTS.str();
			MyUplink->Mode(NULL, theChan, string("+R"), channelTS );
		}

		/* If this is a registered channel, but we're not in it -
		 * then we're not interested in the following commands!
		 */
		if (!reggedChan->getInChan())
		{
			break;
		}

		/*
		 * First thing we do - check if this person is banned.
		 * If so, they're booted out.
		 */

		if (checkBansOnJoin(theChan, reggedChan, theClient))
			{
			break;
			}

		/* Is it time to set an autotopic? */
		if (reggedChan->getFlag(sqlChannel::F_AUTOTOPIC) &&
			(reggedChan->getLastTopic()
			+ topic_duration <= currentTime()))
			{
			doAutoTopic(reggedChan);
			}

		/* Deal with auto-op first - check this users access level. */
		sqlUser* theUser = isAuthed(theClient, false);
		if (!theUser)
			{
			/* If not authed, bye. */
			break;
			}

		/* Check access in this channel. */
		int accessLevel = getEffectiveAccessLevel(theUser, reggedChan, false);
		if (!accessLevel)
			{
			/* No access.. */
			break;
 			}

		sqlLevel* theLevel = getLevelRecord(theUser, reggedChan);
		if(!theLevel)
			{
			break;
			}

		/* Auto voice? */
		if (theLevel->getFlag(sqlLevel::F_AUTOVOICE))
			{
			Voice(theChan, theClient);
			break;
			}

		/* Check noop isn't set */
		if (reggedChan->getFlag(sqlChannel::F_NOOP))
			{
			break;
			}

		/* Check strictop isn't on, and this user is < 100 */
		if (reggedChan->getFlag(sqlChannel::F_STRICTOP))
			{
			if (!(accessLevel >= level::op))
				{
				break;
				}
			}

		/* Next, see if they have auto op set. */
		if (theLevel->getFlag(sqlLevel::F_AUTOOP))
			{
			Op(theChan, theClient);
			break;
			}

		break;
		}

	default:
		break;
	} // switch()

xClient::OnChannelEvent( whichEvent, theChan,
	data1, data2, data3, data4 );
}

/**
 *  This function matches a client against the bans stored in the
 *  database for this channel.
 *  Returns an sqlBan if it matches, false otherwise.
 *  'theChan' and 'theClient' must _not_ be null.
 */
sqlBan* cservice::isBannedOnChan(sqlChannel* theChan, iClient* theClient)
{
map < int,sqlBan* >::const_iterator ptr = theChan->banList.begin();

for( ; ptr != theChan->banList.end() ; ++ptr )
	{
	// NOTE: This is a band-aid, it does not correct the actual
	// problem..
	sqlBan* theBan = ptr->second;
	if( 0 == theBan )
		{
		elog	<< "cservice::isBannedOnChan> Invalid ban!"
			<< endl ;
		continue ;
		}
	if(theBan->getMatcher()->matches(theClient))
		{
		return theBan;
		}
	} /* for() */

return NULL;
}

/**
 * This function compares a client with any active bans set in the DB.
 * If matched, the ban is applied and the user is kicked.
 * Returns true if matched, false if not.
 * N.B: Called from OnChannelEvent, theClient is guarantee'd to be in the
 * channel and netChan will exist.
 *--------------------------------------------------------------------------*/
bool cservice::checkBansOnJoin( Channel* netChan, sqlChannel* theChan,
	iClient* theClient )
{

sqlBan* theBan = isBannedOnChan(theChan, theClient);

// TODO: Don't reapply this ban to the network if it already is set in the channel object.
//       (ircu banlist could be full).
// TODO: Ban through the server.
// TODO: Violation of rule of numbers
/* If we found a matching ban */
if( theBan && (theBan->getLevel() >= 75) )
	{
	stringstream s;
	s	<< getCharYYXXX()
		<< " M "
		<< theChan->getName()
		<< " +b "
		<< theBan->getBanMask()
		<< ends;

	Write( s );

	/* remove the ban (even if it doesnt exist, it will return false anyway) */
	netChan->removeBan( theBan->getBanMask() ) ;
	/* set the ban */
	netChan->setBan( theBan->getBanMask() ) ;

	/* Don't kick banned +k bots */
	if ( !theClient->getMode(iClient::MODE_SERVICES) )
		{
		Kick(netChan, theClient,
			string( "("
			+ theBan->getSetBy()
			+ ") "
			+ theBan->getReason()) );
		}

		return true;
	} /* Matching Ban > 75 */

/*
 * If they're banned < 75, return true, but don't
 * do anything.
 */
if( theBan && (theBan->getLevel() < 75) )
	{
	return true;
	}

return false;
}

void cservice::OnWhois( iClient* sourceClient,
			iClient* targetClient )
{
	/*
	 *  Return info about 'targetClient' to 'sourceClient'
	 */
// TODO: Only use one stringstream here

stringstream s;
s	<< getCharYY()
	<< " 311 "
	<< sourceClient->getCharYYXXX()
	<< " " << targetClient->getNickName()
	<< " " << targetClient->getUserName()
	<< " " << targetClient->getInsecureHost()
	<< " * :"
	<< ends;
Write( s );

stringstream s4;
s4	<< getCharYY()
	<< " 317 "
	<< sourceClient->getCharYYXXX()
	<< " " << targetClient->getNickName()
	<< " 0 " << targetClient->getConnectTime()
	<< " :seconds idle, signon time"
	<< ends;
Write( s4 );

if (targetClient->isOper())
	{
	stringstream s5;
	s5	<< getCharYY()
		<< " 313 "
		<< sourceClient->getCharYYXXX()
		<< " " << targetClient->getNickName()
		<< " :is an IRC Operator"
		<< ends;
	Write( s5 );
	}

sqlUser* theUser = isAuthed(targetClient, false);

if (theUser)
	{
	stringstream s6;
	s6	<< getCharYY()
		<< " 316 "
		<< sourceClient->getCharYYXXX()
		<< " " << targetClient->getNickName()
		<< " :is Logged in as "
		<< theUser->getUserName()
		<< ends;
	Write( s6 );
	}

if (isIgnored(targetClient))
	{
	stringstream s7;
	s7	<< getCharYY()
		<< " 316 "
		<< sourceClient->getCharYYXXX()
		<< " " << targetClient->getNickName()
		<< " :is currently being ignored. "
		<< ends;
	Write( s7 );
	}

stringstream s3;
s3	<< getCharYY()
	<< " 318 "
	<< sourceClient->getCharYYXXX()
	<< " " << targetClient->getNickName()
	<< " :End of /WHOIS list."
	<< ends;
Write( s3 );
}

void cservice::updateLimits()
{
	/*
	 * Forall cached channel records, perform an update of the
	 * channel limit.
	 */
	 sqlChannelHashType::iterator ptr = sqlChannelCache.begin();
	 	while (ptr != sqlChannelCache.end())
	 	{
		sqlChannel* theChan = (ptr)->second;

		/*
		 * Don't have the Floating Limit flag set?
		 */
		if (!theChan->getFlag(sqlChannel::F_FLOATLIM))
			{
			++ptr;
			continue;
			}
		/*
		 * X isn't even in the channel?
		 */
		if (!theChan->getInChan())
			{
			++ptr;
			continue;
			}

		Channel* tmpChan = Network->findChannel(theChan->getName());

		/*
		 * For some magical reason the channel doesn't even exist?
		 */
		if (!tmpChan)
			{
			++ptr;
			continue;
			}

		/*
		 * If its not time to update the limit for this channel yet.
		 */
		if (theChan->getLastLimitCheck() + theChan->getLimitPeriod() > currentTime())
			{
			++ptr;
			continue;
			}

		doFloatingLimit(theChan, tmpChan);

		++ptr;
		}
}

void cservice::doFloatingLimit(sqlChannel* reggedChan, Channel* theChan)
{
/*
 * This event is triggered when its "time" to do autolimits.
 */
 	unsigned int newLimit = theChan->size() + reggedChan->getLimitOffset();

 	/* Don't bother if the new limit is the same as the old one. */
 	if (newLimit == theChan->getLimit()) return;

	/* Also don't bother if the difference between the old limit and
	 * the new limit is < 'grace' */
	int currentDif = abs((int)theChan->getLimit() - (int)newLimit);
	if (currentDif <= (int)reggedChan->getLimitGrace()) return;

	/*
	 * If the new limit is above our max limit, don't bother
	 * either.
	 */

	if (reggedChan->getLimitMax() && (newLimit >= reggedChan->getLimitMax())) return;

	/*
 	 * Check we're actually opped.
	 */

	ChannelUser* tmpBotUser = theChan->findUser(getInstance());
	if (!tmpBotUser) return;
	if (!tmpBotUser->getMode(ChannelUser::MODE_O)) return;

	theChan->setMode(Channel::MODE_L);
	theChan->setLimit(newLimit);
	reggedChan->setLastLimitCheck(currentTime());

	incStat("CORE.FLOATLIM.ALTER");

	stringstream s;
	s	<< getCharYYXXX()
		<< " M "
		<< theChan->getName()
		<< " +l "
		<< newLimit
		<< ends;

	Write( s );

	incStat("CORE.FLOATLIM.ALTER.BYTES", strlen(s.str().c_str()));
}

/*--doAutoTopic---------------------------------------------------------------
 *
 * This support function sets the autotopic in a particular channel.
 */
void cservice::doAutoTopic(sqlChannel* theChan)
{

/* Quickly drop out if nothing is set.. */
if ( theChan->getDescription().empty() && theChan->getURL().empty() )
	{
	return;
	}

string extra ;
if( !theChan->getURL().empty() )
	{
	extra = " ( " + theChan->getURL() + " )" ;
	}

stringstream s;
s	<< getCharYYXXX()
	<< " T "
	<< theChan->getName()
	<< " :"
	<< theChan->getDescription()
	<< extra
	<< ends;

Write( s );

theChan->setLastTopic(currentTime());
}

/**
 * Bans a user via IRC and the database with 'theReason',
 * and then kicks. theChan cannot be null.
 */
bool cservice::doInternalBanAndKick(sqlChannel* theChan,
	iClient* theClient, const string& theReason)
{
/*
 *  Check to see if this banmask already exists in the
 *  channel. (Ugh, and overlapping too.. hmm).
 */

/* Create a new Ban record */
sqlBan* newBan = new (std::nothrow) sqlBan(SQLDb);
assert( newBan != 0 ) ;

string banTarget = Channel::createBan(theClient);

// TODO: Build a suitable constructor in sqlBan
newBan->setChannelID(theChan->getID());
newBan->setBanMask(banTarget);
newBan->setSetBy(nickName);
newBan->setSetTS(currentTime());
newBan->setLevel(25);

/* Move 360 to config */
newBan->setExpires( 300 + currentTime());
newBan->setReason(theReason);

/*
 *  Check for duplicates, if none found -
 *  add to internal list and commit to the db.
 */

map< int,sqlBan* >::const_iterator ptr = theChan->banList.begin();
while (ptr != theChan->banList.end())
	{
	const sqlBan* theBan = ptr->second;

	if(string_lower(banTarget) == string_lower(theBan->getBanMask()))
		{
			/*
			 * If this mask is already banned, we're just getting
			 * lagged info.
			 */
			return true;
		}
	++ptr;
	}

//theChan->banList[newBan->getID()] = newBan;

/* Insert this new record into the database. */
newBan->insertRecord();

/* Insert to our internal List. */
theChan->banList.insert(std::map<int,sqlBan*>::value_type(newBan->getID(),newBan));

/*
 * Finally, if this guy is auth'd.. suspend his account.
 */

sqlUser* theUser = isAuthed(theClient, false);
if (theUser)
{
  sqlLevel* accessRec = getLevelRecord(theUser, theChan);
  if (accessRec && (accessRec->getSuspendExpire() < (currentTime() + 300)))
  {
    int susLev = accessRec->getAccess() + 1;
    if (accessRec->getSuspendLevel() < susLev)
      accessRec->setSuspendLevel(susLev);
    accessRec->setSuspendExpire(currentTime() + 300);
    accessRec->setSuspendBy(nickName);
    accessRec->commit();
  }
}

Channel* netChan = Network->findChannel(theChan->getName());

// Oh dear?
if (!netChan)
	{
	return true;
	}

Kick( netChan, theClient, theReason ) ;

return true ;
}

/**
 * This method writes a 'channellog' record, recording an event that has
 * occured in this channel.
 */

void cservice::writeChannelLog(sqlChannel* theChannel, iClient* theClient,
	unsigned short eventType, const string& theMessage)
{
sqlUser* theUser = isAuthed(theClient, false);
string userExtra = theUser ? theUser->getUserName() : "Not Logged In";

stringstream theLog;
theLog	<< "INSERT INTO channellog (ts, channelID, event, message, "
	<< "last_updated) VALUES "
	<< "("
	<< currentTime()
	<< ", "
	<< theChannel->getID()
	<< ", "
	<< eventType
	<< ", "
 	<< "'["
	<< nickName
	<< "]: "
	<< theClient->getNickUserHost()
	<< " (" << userExtra << ") "
	<< escapeSQLChars(theMessage)
	<< "', "
	<< currentTime()
	<< ")"
	<< ends;

#ifdef LOG_SQL
	elog	<< "cservice::writeChannelLog> "
		<< theLog.str().c_str()
		<< endl;
#endif

// TODO: Is this right?
SQLDb->Exec(theLog);
//SQLDb->ExecCommandOk(theLog.str().c_str());
}

/**
 *  This function returns the last channel event of type 'eventType'
 *  (up to 'eventTime') for the channel given.
 *  It returns a blank string if none found.
 */
const string cservice::getLastChannelEvent(sqlChannel* theChannel,
	unsigned short eventType, unsigned int& eventTime)
{
	unsigned int ts;
	stringstream queryString;

	if (eventTime == 0)
		ts = currentTime();
	else
		ts = eventTime;

	queryString	<< "SELECT message FROM channellog WHERE "
			<< "channelid = "
			<< theChannel->getID()
			<< " AND event = "
			<< eventType
			<< " AND ts <= "
			<< ts
			<< " ORDER BY ts DESC LIMIT 1"
			<< ends;

#ifdef LOG_SQL
	elog	<< "cservice::getLastChannelEvent> "
		<< queryString.str().c_str()
		<< endl;
#endif

	if (SQLDb->Exec(queryString, true))
	{
		if (SQLDb->Tuples() < 1)
			return "";
		string reason = SQLDb->GetValue(0, 0);
		return reason;
	}
	return "";
}

/**
 * Global method to replace ' with \' in strings for safe placement in
 * SQL statements.
 */
const string escapeSQLChars(const string& theString)
{
string retMe ;

for( string::const_iterator ptr = theString.begin() ;
	ptr != theString.end() ; ++ptr )
	{
	if( *ptr == '\'' )
		{
		retMe += "\\\047" ;
		}
	else if ( *ptr == '\\' )
		{
		retMe += "\\\134" ;
		}
	else
		{
		retMe += *ptr ;
		}
	}
return retMe ;
}

const string searchSQL(const string& theString)
{
string retMe ;

for( string::const_iterator ptr = theString.begin() ;
        ptr != theString.end() ; ++ptr )
        {
        if( *ptr == '*' )
                {
                retMe += "%" ;
                }
        else if ( *ptr == '?' )
		{
		retMe += "_" ;
		}
        else
		{
                retMe += *ptr ;
                }
        }
return retMe ;
}

time_t cservice::currentTime() const
{
/* Returns the current time according to the postgres server. */
return dbTimeOffset + ::time(NULL);
}

bool cservice::Notice( const iClient* Target, const string& Message )
{
bool returnMe = false ;
if( Connected && MyUplink )
	{
	setOutputTotal( Target, getOutputTotal(Target) + Message.size() );
	char buffer[512] = { 0 };
	char *b = buffer ;
	const char *m = 0 ;

	// TODO: This should be fixed.
	// A walking timebomb.
	for (m=Message.c_str();*m!=0;m++)
		{
		if (*m == '\n' || *m == '\r')
			{
			*b='\0';
			MyUplink->Write( "%s O %s :%s\r\n",
				getCharYYXXX().c_str(),
				Target->getCharYYXXX().c_str(),
				buffer ) ;
			b=buffer;
			}
		else
			{
			if (b<buffer+509)
			  *(b++)=*m;
			}

		}
        *b='\0';
	returnMe = MyUplink->Write( "%s O %s :%s\r\n",
		getCharYYXXX().c_str(),
		Target->getCharYYXXX().c_str(),
		buffer ) ;
	}
return returnMe ;
}

bool cservice::Notice( const iClient* Target, const char* Message, 
	... )
{
if( Connected && MyUplink && Message && Message[ 0 ] != 0 )
	{
	char buffer[ 512 ] = { 0 } ;
	va_list list;

	va_start(list, Message);
	vsnprintf(buffer, 512, Message, list);
	va_end(list);

	setOutputTotal( Target, getOutputTotal(Target) + strlen(buffer) );
	return MyUplink->Write("%s O %s :%s\r\n",
		getCharYYXXX().c_str(),
		Target->getCharYYXXX().c_str(),
		buffer ) ;
	}
return false ;
}

void cservice::dbErrorMessage(iClient* theClient)
{
Notice(theClient,
	"An error occured while performing this action, "
	"the database may be unavailable. Please try again later.");
dbErrors++;
}

void cservice::loadPendingChannelList()
{
/*
 *  First thing first, if the list has something in it, we want to dump it
 *  out to the database.
 *  Then, clear the list free'ing up memory and finally emptying the list.
 */

if (pendingChannelList.size() > 0)
{
	pendingChannelListType::iterator ptr = pendingChannelList.begin();

	if( !SQLDb->Exec("BEGIN;" ) )
//	if( PGRES_COMMAND_OK != beginStatus )
	{
		elog << "Error starting transaction." << endl;
	}

#ifdef LOG_SQL
	elog << "BEGIN" << endl;
#endif

	while (ptr != pendingChannelList.end())
		{
		sqlPendingChannel* pendingChan = ptr->second;

		/* Commit the record */
		pendingChan->commit();

		/* Stop listening on this channels events for now. */
		MyUplink->UnRegisterChannelEvent(ptr->first, this);

		ptr->second = NULL;
		delete(pendingChan);
		++ptr;
		} /* while() */

	if( !SQLDb->Exec("END;") )
//	if( PGRES_COMMAND_OK != endStatus )
	{
		elog << "Error Ending transaction." << endl;
	}

#ifdef LOG_SQL
	elog << "END" << endl;
#endif

	pendingChannelList.clear();
}

/*
 * For simplicity, we assume that if a pending channel is in state "1", then it has 10 valid
 * supporters who have said "Yes" and we're looking at them.
 */

stringstream theQuery;
theQuery	<<  "SELECT channels.name, pending.channel_id, user_id, pending.join_count, supporters.join_count, pending.unique_join_count"
			<< " FROM pending,supporters,channels"
			<< " WHERE pending.channel_id = supporters.channel_id"
			<< " AND channels.id = pending.channel_id"
			<< " AND pending.status = 1;"
			<< ends;

#ifdef LOG_SQL
elog	<< "*** [CMaster::loadPendingChannelList]: Loading pending channel details."
	<< theQuery.str().c_str()
	<< endl;
#endif

if( SQLDb->Exec(theQuery, true ) )
//if( PGRES_TUPLES_OK == status )
	{
	for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
		string chanName = SQLDb->GetValue(i,0);
		sqlPendingChannel* newPending;

		/*
		 *  Is this channel already in our pending list? If so, simply
		 *  lookup and append this supporter to its list.
		 *  If not, create a new 'pending' channel entry.
		 */

		pendingChannelListType::iterator ptr = pendingChannelList.find(chanName);

		if(ptr != pendingChannelList.end())
			{
			// It already exists.
			newPending = ptr->second;
			}
				else
			{
			newPending = new sqlPendingChannel(SQLDb);
			newPending->channel_id =
				atoi(SQLDb->GetValue(i,1).c_str());
			newPending->join_count =
				atoi(SQLDb->GetValue(i,3).c_str());
			newPending->unique_join_count =
				atoi(SQLDb->GetValue(i,5).c_str());
			pendingChannelList.insert( pendingChannelListType::value_type(chanName, newPending) );

			/*
			 *  Lets register our interest in listening on JOIN events for this channel.
			 */
			MyUplink->RegisterChannelEvent(chanName, this);
			}

		/*
		 *  Next, update the internal supporters list.
		 */
		newPending->supporterList.insert(  sqlPendingChannel::supporterListType::value_type(
			atoi(SQLDb->GetValue(i, 2).c_str()), 
			atoi(SQLDb->GetValue(i, 4).c_str()) )  );
		}
	}

	logDebugMessage("Loaded Pending Channels, there are currently %i channels being traffic monitored.",
		pendingChannelList.size());

#ifdef LOG_DEBUG
	elog	<< "Loaded pending channels, there are currently "
			<< pendingChannelList.size()
			<< " channels being notified and recorded."
			<< endl;
#endif

	/*
	 * For each pending channel, load up its IP traffic
	 * cache.
	 */

	pendingChannelListType::iterator ptr = pendingChannelList.begin();
	while (ptr != pendingChannelList.end())
		{
		sqlPendingChannel* pendingChan = ptr->second;
		pendingChan->loadTrafficCache();
		++ptr;
		};

}

void cservice::checkDbConnectionStatus()
{
	if( SQLDb->ConnectionBad() )
//	if(SQLDb->Status() == CONNECTION_BAD)
	{
		logAdminMessage("\002WARNING:\002 Backend database connection has been lost, attempting to reconnect.");
		elog	<< "cmaster::cmaster> Attempting to reconnect to database." << endl;

		/* Remove the old database connection object. */
		delete(SQLDb);

		string Query = "host=" + confSqlHost + " dbname=" + confSqlDb + " port=" + confSqlPort + " user=" + confSqlUser
					 + " password=" + confSqlPass;

		SQLDb = new (std::nothrow) dbHandle( confSqlHost,
			atoi( confSqlPort.c_str() ),
			confSqlDb,
			confSqlUser,
			confSqlPass ) ;
//		SQLDb = new (std::nothrow) cmDatabase( Query.c_str() ) ;
		assert( SQLDb != 0 ) ;

		if (SQLDb->ConnectionBad())
		{
			elog	<< "cmaster::cmaster> Unable to connect to SQL server."
					<< endl
					<< "cmaster::cmaster> PostgreSQL error message: "
					<< SQLDb->ErrorMessage()
					<< endl ;

			connectRetries++;
			if (connectRetries >= 6)
			{
				logAdminMessage("Unable to contact database after 6 attempts, shutting down.");
				//MyUplink->flushBuffer();
				::exit(0);
			} else
			{
				logAdminMessage("Connection failed, retrying:");
			}


		} else
		{
// TODO: Is this ok?
				SQLDb->Exec("LISTEN channels_u; LISTEN users_u; LISTEN levels_u;");
//				SQLDb->ExecCommandOk("LISTEN channels_u; LISTEN users_u; LISTEN levels_u;");
				logAdminMessage("Successfully reconnected to database server. Panic over ;)");
		}
	}

}

void cservice::preloadChannelCache()
{
stringstream theQuery;
theQuery	<< "SELECT " << sql::channel_fields
			<< " FROM channels WHERE "
			<< "registered_ts <> 0"
			<< ends;

elog	<< "*** [CMaster::preloadChannelCache]: Loading all registered channel records: "
		<< endl;

if( SQLDb->Exec(theQuery, true ) )
//if( PGRES_TUPLES_OK == status )
	{
	for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
 		/* Add this information to the channel cache. */

		sqlChannel* newChan = new (std::nothrow) sqlChannel(SQLDb);
		assert( newChan != 0 ) ;

		newChan->setAllMembers(i);
		newChan->setLastUsed(currentTime());

		sqlChannelCache.insert(sqlChannelHashType::value_type(newChan->getName(), newChan));
		sqlChannelIDCache.insert(sqlChannelIDHashType::value_type(newChan->getID(), newChan));

		} // for()
	} // if()

elog	<< "*** [CMaster::preloadChannelCache]: Done. Loaded "
		<< SQLDb->Tuples()
		<< " registered channel records."
		<< endl;
}

void cservice::preloadBanCache()
{
/*
 * Execute a query to return all bans.
 */

stringstream theQuery;
theQuery	<< "SELECT " << sql::ban_fields
		<< " FROM bans;"
		<< ends;

elog	<< "*** [CMaster::preloadBanCache]: Precaching Bans table: "
	<< endl;

if( SQLDb->Exec(theQuery, true ) )
//if( PGRES_TUPLES_OK == status )
	{
	for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
		/*
		 * First, lookup this channel in the channel cache.
		 */

		unsigned int channel_id = atoi(SQLDb->GetValue(i, 1).c_str());
		sqlChannel* theChan = getChannelRecord(channel_id);

		/*
		 * If we don't have the channel cached, its not registered so
		 * we aren't interested in this ban.
		 */

		if (!theChan) continue;

		sqlBan* newBan = new (std::nothrow) sqlBan(SQLDb);
		newBan->setAllMembers(i);
		theChan->banList.insert(
			std::make_pair( newBan->getID(), newBan ) ) ;
//		theChan->banList[newBan->getID()] = newBan;

		} // for()
	} // if()

elog	<< "*** [CMaster::preloadBanCache]: Done. Loaded "
	<< SQLDb->Tuples()
	<< " bans."
	<< endl;
}

void cservice::preloadLevelsCache()
{
/*
 * Execute a query to return all level records.
 */

stringstream theQuery;
theQuery	<< "SELECT " << sql::level_fields
			<< " FROM levels"
			<< ends;

elog		<< "*** [CMaster::preloadLevelCache]: Precaching Level table: "
			<< endl;

unsigned int goodCount = 0;
if( SQLDb->Exec(theQuery, true ) )
//if( PGRES_TUPLES_OK == status )
	{
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
		/*
		 * First, lookup this channel in the channel cache.
		 */

		unsigned int channel_id = atoi(SQLDb->GetValue(i, 0).c_str());
		unsigned int user_id = atoi(SQLDb->GetValue(i, 1).c_str());
		sqlChannel* theChan = getChannelRecord(channel_id);

		/*
		 * If we don't have the channel cached, its not registered so
		 * we aren't interested in this level record.
		 */

		if (!theChan) continue;

		pair<int, int> thePair( user_id, channel_id );
		sqlLevel* newLevel = new (std::nothrow) sqlLevel(SQLDb);
		newLevel->setAllMembers(i);

		sqlLevelCache.insert(sqlLevelHashType::value_type(thePair, newLevel));
		goodCount++;

		} // for()
	} // if()

elog	<< "*** [CMaster::preloadLevelCache]: Done. Loaded "
		<< goodCount << " level records out of " << SQLDb->Tuples()
		<< "."
		<< endl;
}

/*
 * Preload all the users within the last 'x' days, to
 * save doing loads of lookups when we recieve 25,000 +r'd users
 * during net.merge.
 */

void cservice::preloadUserCache()
{
	stringstream theQuery;
	theQuery	<< "SELECT " << sql::user_fields
				<< " FROM users,users_lastseen WHERE "
				<< " users_lastseen.user_id = users.id AND "
				<< " users_lastseen.last_seen >= "
				<< currentTime() - (preloadUserDays * 86400)
				<< ends;

	elog		<< "*** [CMaster::preloadUserCache]: Loading users accounts logged in within "
				<< preloadUserDays
				<< " days : "
				<< endl;

	if( SQLDb->Exec(theQuery, true ) )
//	if( PGRES_TUPLES_OK == status )
	{
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
			{
				sqlUser* newUser = new (std::nothrow) sqlUser(SQLDb);
				assert( newUser != 0 ) ;

				newUser->setAllMembers(i);
				newUser->setLastUsed(currentTime());
				sqlUserCache.insert(sqlUserHashType::value_type(newUser->getUserName(), newUser));
			}
	}

	elog	<< "*** [CMaster::preloadUserCache]: Done. Loaded "
			<< SQLDb->Tuples()
			<< " user accounts."
			<< endl;
}

void cservice::incStat(const string& name)
{
	statsMapType::iterator ptr = statsMap.find( name );

	if( ptr == statsMap.end() )
        {
        statsMap.insert(statsMapType::value_type(name, 1));
        }
        else
        {
			ptr->second++;
		}
}

void cservice::incStat(const string& name, unsigned int amount)
{
	statsMapType::iterator ptr = statsMap.find( name );

	if( ptr == statsMap.end() )
        {
        statsMap.insert(statsMapType::value_type(name, 1));
        }
        else
        {
			ptr->second += amount;
		}
}

void cservice::noticeAllAuthedClients(sqlUser* theUser, const char* Message, ... )
{
/*
 * Loop over everyone who is authed as this user and give them a message.
 */

if( Connected && MyUplink && Message && Message[ 0 ] != 0 )
	{
	char buffer[ 512 ] = { 0 } ;
	va_list list;

	va_start(list, Message);
	vsnprintf(buffer, 512, Message, list);
	va_end(list);

	/*
	 * Loop over all people auth'd as this user, and send them a
	 * message.
	 */

	for( sqlUser::networkClientListType::iterator ptr = theUser->networkClientList.begin() ;
		ptr != theUser->networkClientList.end() ; ++ptr )
		{
		iClient* Target = (*ptr);
		setOutputTotal( Target, getOutputTotal(Target) + strlen(buffer) );
		MyUplink->Write("%s O %s :%s\r\n",
			getCharYYXXX().c_str(),
			Target->getCharYYXXX().c_str(),
			buffer ) ;
		}
	}

}

/*
 * Checks if the password supplied correctly matches the password for
 * this user.
 */

bool cservice::isPasswordRight(sqlUser* theUser, const string& password)
{
/*
 *  Compare password with MD5 hash stored in user record.
 */

/* MD5 hash algorithm object. */
md5	hash;

/* MD5Digest algorithm object.*/
md5Digest digest;

/* check that we have a valid md5 hash to prevent coredumps */

if (theUser->getPassword().size() < 9)
{
	/* salt is 8 characters and there must be a password
	 * of some kind, so 9 is a 'safe' value.
	 *
	 * return a standard 'invalid password' to user
	 */
	return false;
}

string salt = theUser->getPassword().substr(0, 8);
string md5Part = theUser->getPassword().substr(8);
string guess = salt + password;

// Build a MD5 hash based on our salt + the guessed password.
hash.update( (const unsigned char *)guess.c_str(), guess.size() );
hash.report( digest );

// Convert the digest into an array of int's to output as hex for
// comparison with the passwords generated by PHP.
int data[ MD5_DIGEST_LENGTH ] = { 0 } ;

for( size_t ii = 0; ii < MD5_DIGEST_LENGTH; ii++ )
	{
	data[ii] = digest[ii];
	}

stringstream output;
output << std::hex;
output.fill('0');

for( size_t ii = 0; ii < MD5_DIGEST_LENGTH; ii++ )
	{
	output << std::setw(2) << data[ii];
	}
output << ends;

if(md5Part != output.str().c_str() ) // If the MD5 hash's don't match..
	{
	return false;
	}

return true;
}

/*
 * Return some slightly interesting stats as a result of a status *.
 */
void cservice::doCoderStats(iClient* theClient)
{
	float userTotal = userCacheHits + userHits;
	float userEf = userCacheHits ? ((float)userCacheHits / userTotal * 100) : 0;

	Notice(theClient, "CMaster Channel Services internal status:");

	Notice(theClient,"[User Record Stats] \002Cached Entries:\002 %i    \002DB Requests:\002 %i    \002Cache Hits:\002 %i    \002Efficiency:\002 %.2f%%",
		sqlUserCache.size(),
		userHits,
		userCacheHits,
		userEf);

	/*
	 * Count how many users are actually logged in right now.
	 */
	unsigned int authCount = 0;
	sqlUserHashType::iterator ptr = sqlUserCache.begin();
	sqlUser* tmpUser;

	while (ptr != sqlUserCache.end())
	{
		tmpUser = ptr->second;
		if (tmpUser->isAuthed()) authCount++;
		++ptr;
	}

	/*
	 * Iterate over all the clients on the network and
	 * see how many are +x.
	 */

	unsigned int plusXCount = 0;
	unsigned int plusWCount = 0;
	unsigned int plusDCount = 0;

	xNetwork::const_clientIterator ptr2 = Network->clients_begin();
	while(ptr2 != Network->clients_end())
	{
		iClient* tmpClient = ptr2->second;
		if (tmpClient->isModeX()) plusXCount++;
		if (tmpClient->isModeW()) plusWCount++;
		if (tmpClient->isModeD()) plusDCount++;
		++ptr2;
	}

	Notice(theClient, "--- Total clients : %i", Network->clientList_size());

	float authTotal = ((float)authCount / (float)Network->clientList_size()) * 100;
	Notice(theClient, "--- Total Auth'd  : %i (%.2f%% of total)",
		authCount, authTotal);

	float plusXTotal = ((float)plusXCount / (float)Network->clientList_size()) * 100;
	Notice(theClient, "--- Total umode +x: %i (%.2f%% of total)",
		plusXCount, plusXTotal);

	float plusWTotal = ((float)plusWCount / (float)Network->clientList_size()) * 100;
	Notice(theClient, "--- Total umode +w: %i (%.2f%% of total)",
		plusWCount, plusWTotal);

	float plusDTotal = ((float)plusDCount / (float)Network->clientList_size()) * 100;
	Notice(theClient, "--- Total umode +d: %i (%.2f%% of total)",
		plusDCount, plusDTotal);

	float joinTotal = ((float)joinCount / (float)Network->channelList_size()) * 100;
	Notice(theClient, "I am in %i channels out of %i on the network. (%.2f%%)",
		joinCount, Network->channelList_size(), joinTotal);

	unsigned int secs = (currentTime() - getUplink()->getStartTime());

	float cPerSec = (float)totalCommands / (float)secs;

	Notice(theClient, "I've received %i commands since I started (%.2f commands per second).",
		totalCommands, cPerSec);

	Notice(theClient,"\002Uptime:\002 %s",
		prettyDuration(getUplink()->getStartTime() + dbTimeOffset).c_str());

	/*
	 * Now, dump all the config settings.
	 */

	Notice(theClient, "-------------------------");
	Notice(theClient, "Configuration Variables: ");
	Notice(theClient, "-------------------------");

	for( cservice::configHashType::iterator ptr = configTable.begin() ;
	ptr != configTable.end() ; ++ptr )
	{
	Notice(theClient, "%s: %s", ptr->first.c_str(), ptr->second->asString().c_str());
	}
}

/*
 * Function to retrieve the specified config variable from the in-memory cache.
 */
ConfigData* cservice::getConfigVar(const string& variable)
{
	configHashType::iterator ptr = configTable.find(variable);

	if(ptr != configTable.end())
	{
		return ptr->second;
	} else {
		return &empty_config;
	}
}

/*
 * Pre-load the configuration information from the db.
 */
void cservice::loadConfigData()
{
	stringstream theQuery;
	theQuery	<< "SELECT var_name,contents FROM variables;"
				<< ends;

	if( SQLDb->Exec(theQuery, true ) )
//	if( PGRES_TUPLES_OK == status )
		{
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
			{
			ConfigData* newConfig = new (std::nothrow) ConfigData();
			assert( newConfig != 0 ) ;

			newConfig->string_value = SQLDb->GetValue(i, 1);
			newConfig->int_value = atoi(newConfig->string_value.c_str());

			configTable.insert(configHashType::value_type(SQLDb->GetValue(i, 0), newConfig));
			}
		}

}


bool cservice::doXQLogin(iServer* theServer, const string& Routing, const string& Message)
{
//What's going to be in Message?
//<ournick> LOGIN <ip> <username> <password>
StringTokenizer st( Message );
if( st.size() < 5 )
        {
	return false;
        }

/*
 *  Are we allowing logins yet?
 **/
unsigned int useLoginDelay = getConfigVar("USE_LOGIN_DELAY")->asInt();
unsigned int loginTime = getUplink()->getStartTime() + loginDelay;
if ( (useLoginDelay == 1) && (loginTime >= (unsigned int)currentTime()) )
	{
	//TODO Send error back
	return false;
	}



sqlUser* theUser = getUserRecord(st[3]);
if( !theUser )
        {
	elog 	<< "doXQLogin: "
		<< "Couldn't find user data for accountname: "
		<< st[3]
		<< endl;
	//TODO Send error back
        return false;
        }

// IPR checks are performed against st[2], the IP passed to us by iauth

stringstream theQuery;
theQuery        << "SELECT allowmask,allowrange1,allowrange2,added FROM "
                << "ip_restrict WHERE user_id = "
                << theUser->getID()
                << ends;
#ifdef LOG_SQL
        elog    << "cservice::checkIPR::sqlQuery> "
                << theQuery.str().c_str()
                << endl;
#endif

if( !SQLDb->Exec(theQuery, true ) )
        {
        /* SQL error, fail them */
        elog    << "cservice::checkIPR> SQL Error: "
                << SQLDb->ErrorMessage()
                << endl;
                return false;
        }

bool userHasIPR = true;
if (SQLDb->Tuples() < 1)
        {
	userHasIPR = false;
#ifdef IPR_DEFAULT_REJECT
        /* no entries, fail them */
        return false;
#else
	/* no entries, allow them to pass through*/
#endif
        }

if (userHasIPR) 
	{
	/* cycle through results to find a match */
        bool ipr_match = false;
        unsigned int ipr_ts = 0;
        unsigned int tmpIP = xIP(st[2], false).GetLongIP();
        for (unsigned int i=0; i < SQLDb->Tuples(); i++)
        {
                /* get some variables out of the db row */
                std::string ipr_allowmask = SQLDb->GetValue(i, 0);
                unsigned int ipr_allowrange1 = atoi(SQLDb->GetValue(i, 1).c_str());
                unsigned int ipr_allowrange2 = atoi(SQLDb->GetValue(i, 2).c_str());
                ipr_ts = atoi(SQLDb->GetValue(i, 3).c_str());

                /* is this an IP range? */
                if (ipr_allowrange2 > 0)
                {
                        /* yes it is, is the client IP between range1 and range2? */
                        if ((tmpIP >= ipr_allowrange1) && (tmpIP <= ipr_allowrange2))
						                        {
                                ipr_match = true;
                                break;
                        }
                } else {
                        /* no, is it a single IP? */
                        if (ipr_allowrange1 > 0)
                        {
                                /* yes it is, does the IP match range1? */
                                if (tmpIP == ipr_allowrange1)
                                {
                                        ipr_match = true;
                                        break;
                                }
                        } 
                }
        }
        /* check if we found a match yet */
        if (!ipr_match)
        {
                /* no match, fail them */
		//TODO Send error back
                return false;
        } else {
                /* IP restriction check passed */
        }
}	

if (theUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
        {
        elog    << "doXQLogin: "
                << "Globally suspended account tried to auth: "
                << st[3]
                << endl;
	//TODO Send error back
        return false;
        }

unsigned int max_failed_logins = getConfigVar("FAILED_LOGINS")->asInt();
unsigned int failed_login_rate = getConfigVar("FAILED_LOGINS_RATE")->asInt();

/* if it's not configured, default to every 15 minutes */
if (failed_login_rate==0)
	failed_login_rate = 900;

if (!isPasswordRight(theUser, st.assemble(4)))
        {
	theUser->incFailedLogins();

	if ((max_failed_logins > 0) && (theUser->getFailedLogins() > max_failed_logins) &&
		(theUser->getLastFailedLoginTS() < (time(NULL) - failed_login_rate)))
	{
		/* we have exceeded our maximum - alert relay channel
		 * work out a checksum for the password.  Yes, I could have
  		 * just used a checksum of the original password, but this
  		 * means it's harder to 'fool' the check digit with a real
  		 * password - create MD5 from original salt stored */
		unsigned char	checksum;
		md5		hash;
		md5Digest	digest;

		if (theUser->getPassword().size() < 9)
		{
			checksum = 0;
		} else {
			string salt = theUser->getPassword().substr(0, 8);
			string guess = salt + st.assemble(2);

			hash.update( (const unsigned char *)guess.c_str(), guess.size() );
			hash.report( digest );

			checksum = 0;
			for (size_t i = 0; i < MD5_DIGEST_LENGTH; i++)
			{
				/* add ascii value to check digit */
				checksum += digest[i];
			}
		}

		theUser->setLastFailedLoginTS(time(NULL));
		logPrivAdminMessage("%d failed logins for %s (last attempt from iauth with IP %s), checksum %d).",
			theUser->getFailedLogins(),
			theUser->getUserName().c_str(),
			st[2].c_str(),
			checksum);
	
	}
        elog    << "doXQLogin: "
                << "Wrong password supplied by: "
                << st[3] << "PASS: "
		<< st.assemble(4)
                << endl;
	//TODO Send error back
        return false;
	}

/*
 * Don't exceed MAXLOGINS.
 */
//TODO

/*
 * If this user account is already authed against, send a notice to the other
 * users warning them that someone else has logged in too.
 */
//TODO



elog    << "doXQLogin: "
        << "Succesful auth for "
        << st[3] 
	<< endl;
//TODO Return confirmation
return true;
}



/*
 * Display a summary of channels "theUser" has access on to "theClient".
 * Only show those above or equal to "minlevel".
 */
void cservice::outputChannelAccesses(iClient* theClient, sqlUser* theUser, sqlUser* tmpUser, unsigned int minlevel)
{
	stringstream channelsQuery;
	string channelList ;

	channelsQuery	<< "SELECT channels.name,levels.access,levels.flags FROM levels,channels "
			<< "WHERE levels.channel_id = channels.id AND channels.registered_ts <> 0 AND levels.user_id = "
			<< theUser->getID()
			<< " AND levels.access >= "
			<< minlevel
			<< " ORDER BY levels.access DESC"
			<< ends;

	#ifdef LOG_SQL
		elog	<< "CHANINFO::sqlQuery> "
			<< channelsQuery.str().c_str()
			<< endl;
	#endif

	string chanName ;
	string chanAccess ;
	unsigned int flags;

	if( !SQLDb->Exec(channelsQuery, true ) )
//	if( PGRES_TUPLES_OK != status )
		{
		Notice( theClient,
			"Internal error: SQL failed" ) ;

		elog	<< "CHANINFO> SQL Error: "
			<< SQLDb->ErrorMessage()
			<< endl ;
		return  ;
		}

	for(unsigned int i = 0; i < SQLDb->Tuples(); i++)
		{
		flags = atoi(SQLDb->GetValue(i, 2));
		if(flags & sqlLevel::F_AUTOINVITE)
		{
			chanName = "\002" + SQLDb->GetValue(i,0) + "\002";
		} else {
			chanName = SQLDb->GetValue(i,0);
		}
		chanAccess = SQLDb->GetValue(i,1);
		
		// 4 for 2 spaces, 2 brackets + comma.
		if ((channelList.size() + chanName.size() + chanAccess.size() +5) >= 450)
			{
			Notice(theClient,
				getResponse(tmpUser,
					language::channels,
					string("Channels: %s")).c_str(),
				channelList.c_str());
			channelList.erase( channelList.begin(),
				channelList.end() ) ;
			}

		if (channelList.size() != 0)
				{
				channelList += ", ";
				}
		channelList += chanName;
		channelList += " (";
		channelList += chanAccess;
		channelList +=  ")";
		} // for()

	Notice(theClient,
		getResponse(tmpUser,
			language::channels,
			string("Channels: %s")).c_str(),
		channelList.c_str());
}

void Command::Usage( iClient* theClient )
{
bot->Notice( theClient, string( "SYNTAX: " ) + getInfo() ) ;
}

string cservice::CryptPass( const string& pass )
{
StringTokenizer st( pass ) ;

const char validChars[] = 
	"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.$*_";

string salt ;
for( unsigned short int i = 0 ; i < 8 ; ++i )
	{
	int randNo = 1 + (int) (64.0 * rand() / (RAND_MAX + 1.0) );
	salt += validChars[ randNo ] ;
	}

/* Work out a MD5 hash of our salt + password */
md5		hash; // MD5 hash algorithm object.
md5Digest	digest; // MD5Digest algorithm object.

stringstream output;
string newPass;
newPass = salt + st.assemble(0);

hash.update( (const unsigned char *)newPass.c_str(),
	newPass.size() );
hash.report( digest );

/* Convert to Hex */
int data[ MD5_DIGEST_LENGTH ] = { 0 } ;
for( size_t ii = 0; ii < MD5_DIGEST_LENGTH; ii++ )
        {
        data[ii] = digest[ii];
        }

output << std::hex;
output.fill('0');
for( size_t ii = 0; ii < MD5_DIGEST_LENGTH; ii++ )
        {
        output << std::setw(2) << data[ii];
        }
output << ends;

return string( salt + output.str()  );
}

} // namespace gnuworld
