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
#include	<chrono>

#include	"client.h"
#include	"cservice.h"
#include	"EConfig.h"
#include	"events.h"
#include	"ip.h"
#include	"Network.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"dbHandle.h"
#include	"constants.h"
#include	"networkData.h"
#include	"levels.h"
#include	"cservice_config.h"
#include	"cservice_confvars.h"
#include	"match.h"
#include	"md5hash.h"
#include	"responses.h"
#include	"banMatcher.h"
#include	"sqlUser.h"

#ifdef HAVE_LIBOATH
extern "C" {
#include <liboath/oath.h>
}
#endif

namespace gnuworld
{

using std::pair ;
using std::vector ;
using std::list;
using std::map;
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
	ptr->second->setLogger( logger.get() ) ;
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

// Start the pending chan notification timer rolling.
theTime = time(NULL) + pendingNotifPeriod;
pendingNotif_timerID = MyUplink->RegisterTimer(theTime, this, NULL);

// Start the floating Limit timer rolling.
theTime = time(NULL) + limitCheckPeriod;
limit_timerID = MyUplink->RegisterTimer(theTime, this, NULL);

// Start the channels flood period timer rolling.
theTime = time(NULL) + channelsFloodPeriod;
channels_flood_timerID = MyUplink->RegisterTimer(theTime, this, NULL);

// Start prometheus metrics timer rolling.
theTime = time(NULL) + 60;
prometheus_timerID = MyUplink->RegisterTimer(theTime, this, NULL);

/* Start the web relay timer rolling.
 * First, empty out any old notices that may be present.
 */
if (SQLDb->Exec("DELETE FROM webnotices WHERE created_ts < (date_part('epoch', CURRENT_TIMESTAMP)::int - 600)"))
{
	/* only register the timer if the query is ok.
	 * if the query fails, we most likely don't have
	 * the table setup, so pointless checking it.
	 */
	theTime = time(NULL) + webrelayPeriod;
	webrelay_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
} else {
	/* log the error */
	LOG( ERROR, "Unable to empty webnotices table, not checking webnotices." );
	LOGSQL_ERROR( SQLDb ) ;
}

/* Register our interest in recieving some Network events from gnuworld. */

MyUplink->RegisterEvent( EVT_KILL, this );
MyUplink->RegisterEvent( EVT_QUIT, this );
MyUplink->RegisterEvent( EVT_NICK, this );
MyUplink->RegisterEvent( EVT_ACCOUNT, this );
MyUplink->RegisterEvent( EVT_BURST_ACK, this );
MyUplink->RegisterEvent( EVT_XQUERY, this );
MyUplink->RegisterEvent( EVT_XREPLY, this );
MyUplink->RegisterEvent( EVT_GLINE , this );
MyUplink->RegisterEvent( EVT_REMGLINE , this );
MyUplink->RegisterEvent( EVT_NETBREAK, this );

xClient::OnAttach() ;
}

void cservice::OnShutdown(const std::string& reason)
{
	/* handle client shutdown */
	MyUplink->UnloadClient(this, reason);
}

cservice::cservice(const string& args)
 : xClient( args )
#ifdef USE_THREAD
   , threadWorker()
#endif
{

/* Register custom logger objects. */
registerLogHandlers() ;

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
#ifdef NEW_IRCU_FEATURES
RegisterCommand(new CERTCommand(this, "CERT", "<ADD|REM|LIST> [fingerprint] [note]", 10));
#endif
RegisterCommand(new OPCommand(this, "OP", "<#channel> [nick] [nick] ..", 3));
RegisterCommand(new DEOPCommand(this, "DEOP", "<#channel> [nick] [nick] ..", 3));
RegisterCommand(new VOICECommand(this, "VOICE", "<#channel> [nick] [nick] ..", 3));
RegisterCommand(new DEVOICECommand(this, "DEVOICE", "<#channel> [nick] [nick] ..", 3));
RegisterCommand(new ADDUSERCommand(this, "ADDUSER", "<#channel> <username> <access>", 8));
RegisterCommand(new REMUSERCommand(this, "REMUSER", "<#channel> <username>", 4));
RegisterCommand(new MODINFOCommand(this, "MODINFO", "<#channel> [ACCESS <username> <level>] [AUTOMODE <username> <NONE|OP|VOICE>] [INVITE <ON|OFF>]", 6));
RegisterCommand(new SETCommand(this, "SET", "[#channel] <variable> <value> or, SET <invisible> <ON|OFF> or, SET LANG <language> or, SET MAXLOGINS <max-logins>.", 6));
RegisterCommand(new INVITECommand(this, "INVITE", "<#channel> <#channel> <#channel> ... ", 2));
RegisterCommand(new TOPICCommand(this, "TOPIC", "<#channel> <topic>", 4));
RegisterCommand(new CHANINFOCommand(this, "CHANINFO", "<#channel>", 3));
RegisterCommand(new CHANINFOCommand(this, "INFO", "<username>", 3));
RegisterCommand(new BANLISTCommand(this, "BANLIST", "<#channel>", 3));
RegisterCommand(new KICKCommand(this, "KICK", "<#channel> <nicks | *!*@*.host> [reason]", 4));
RegisterCommand(new STATUSCommand(this, "STATUS", "<#channel>", 4));
RegisterCommand(new SUSPENDCommand(this, "SUSPEND", "<#channel> <username> <duration> [level] [reason]", 5));
RegisterCommand(new UNSUSPENDCommand(this, "UNSUSPEND", "<#channel> <username>", 5));
RegisterCommand(new BANCommand(this, "BAN", "<#channel> <nicks | *!*user@*.host> [duration] [level] [reason]", 5));
RegisterCommand(new UNBANCommand(this, "UNBAN", "<#channel> <nicks | *!*user@*.host>", 5));
RegisterCommand(new LBANLISTCommand(this, "LBANLIST", "<#channel> <banmask>", 5));
RegisterCommand(new NEWPASSCommand(this, "NEWPASS", "<new passphrase>", 8));
RegisterCommand(new JOINCommand(this, "JOIN", "<#channel>", 8));
RegisterCommand(new PARTCommand(this, "PART", "<#channel>", 8));
RegisterCommand(new OPERJOINCommand(this, "OPERJOIN", "<#channel>", 8));
RegisterCommand(new OPERPARTCommand(this, "OPERPART", "<#channel>", 8));
RegisterCommand(new CLEARMODECommand(this, "CLEARMODE", "<#channel>", 4));
RegisterCommand(new SUSPENDMECommand(this, "SUSPENDME", "<password>", 15));
RegisterCommand(new MODECommand(this, "MODE", "<#channel> <modes> [arguments]", 4));

RegisterCommand(new WHITELISTCommand(this, "WHITELIST", "<ADD|REM|VIEW> <IP> [duration] [reason]", 10));
RegisterCommand(new SCANHOSTCommand(this, "SCANHOST", "<mask> [-sort <user|cmd|host|ip|time>] [-max n] [-order <asc|desc>]", 10));
RegisterCommand(new SCANUNAMECommand(this, "SCANUNAME", "<mask> [-all]", 10));
RegisterCommand(new SCANEMAILCommand(this, "SCANEMAIL", "<mask> [-all]", 10));
RegisterCommand(new REMIGNORECommand(this, "REMIGNORE", "<mask>", 5));
RegisterCommand(new REGISTERCommand(this, "REGISTER", "<#channel>", 8));
RegisterCommand(new REMOVEALLCommand(this, "REMOVEALL", "<#channel>", 15));
RegisterCommand(new PURGECommand(this, "PURGE", "<#channel> [-noop] <reason>", 8));
RegisterCommand(new FORCECommand(this, "FORCE", "<#channel>", 8));
RegisterCommand(new UNFORCECommand(this, "UNFORCE", "<#channel>", 8));
RegisterCommand(new SERVNOTICECommand(this, "SERVNOTICE", "<#channel> <text>", 5));
RegisterCommand(new SAYCommand(this, "SAY", "<#channel> <text>", 5));
RegisterCommand(new SAYCommand(this, "DO", "<#channel> <text>", 5));
RegisterCommand(new QUOTECommand(this, "QUOTE", "<text>", 5));
RegisterCommand(new REHASHCommand(this, "REHASH", "[translations|help|variables|config|motd]", 5));
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
SQLDb = new (std::nothrow) dbHandle( this, confSqlHost,
	atoi( confSqlPort ),
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

if (SQLDb->Exec("SELECT date_part('epoch', CURRENT_TIMESTAMP)::int;",true))
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

loadConfigVariables();
loadConfigData();

/* Initiate pushover. */
if( pushoverEnable )
  {
  elog  << "*** [CMaster]: Enabling Pushover notifications for "
        << pushoverUserKeys.size()
        << " user keys..."
        << endl ;

#ifdef USE_THREAD
  pushover = std::make_shared< PushoverClient >( this, pushoverToken, pushoverUserKeys, &threadWorker ) ;
#else
  pushover = std::make_shared< PushoverClient >( this, pushoverToken, pushoverUserKeys ) ;
#endif
  logger->addNotifier( pushover, pushoverVerbosity ) ;
  pushover->sendMessage( "cmaster init", "cmaster connecting..." ) ;
  }

/* Initiate prometheus. */
#ifdef HAVE_PROMETHEUS
if( prometheusEnable )
	{
	elog  << "*** [CMaster]: Enabling Prometheus metrics at "
		<< prometheusIP
		<< ":"
		<< prometheusPort
		<< "..."
		<< endl ;

	try
		{
		prometheus = std::make_shared< PrometheusClient >( this, prometheusIP, prometheusPort ) ;
		logger->addNotifier( prometheus ) ;
		}
	catch( const std::exception& e )
		{
		elog << "*** [CMaster]: Unable to start Prometheus on " << prometheusIP << ":" << prometheusPort << endl ;
		elog << "*** [CMaster]: Prometheus error message: " << e.what() << endl ;
		prometheus.reset() ;
		::exit( 0 ) ;
		}
	}
#endif

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

/* Preload fingerprints */
preloadFingerprintCache();

/* Load the glines from db */
loadGlines();

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
saslRequests.clear() ;
fingerprintMap.clear() ;
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
	if (theChan->getName() == "*")
	{	//Do not do anything for the admins channel
		++ptr;
		continue;
	}
	
	/* The channel is purged */
	if (theChan->getRegisteredTS() == 0)
	{
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
		if (tempModes.empty())
			tempModes = "+";
		if (tempModes.find('R') == string::npos)
			tempModes += 'R';

		/* If FLOATLIM is activated, we don't set +l. The correct limit will be set shortly after burst. */
		if (theChan->getFlag(sqlChannel::F_FLOATLIM))
			stripModes(tempModes, "l");

		MyUplink->JoinChannel( this,
			theChan->getName(),
			tempModes,
			theChan->getChannelTS(),
			true );

		theChan->setInChan(true);
		incrementJoinCount();

		doTheRightThing(tmpChan);

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

	LOG( INFO, "Channel join complete." ) ;

	return xClient::BurstChannels();
}

void cservice::OnConnect()
{
#ifdef NEW_IRCU_FEATURES
auto saslServer = Network->findNetConf( "sasl.server" ) ;
if( !saslServer || saslServer->first != MyUplink->getName() )
  {
  MyUplink->Write( "%s CF %d sasl.server :%s",
    getCharYY().c_str(),
    time(nullptr),
    MyUplink->getName().c_str() ) ;
  }

auto saslMechanisms = Network->findNetConf( "sasl.mechanisms" ) ;
if( !saslMechanisms || saslMechanisms->first != saslMechsAdvertiseList() )
  {
  MyUplink->Write( "%s CF %d sasl.mechanisms :%s",
	getCharYY().c_str(),
    time(nullptr),
    saslMechsAdvertiseList().c_str() ) ;
  }

auto netSaslTimeout = Network->findNetConf( "sasl.timeout" ) ;
if( !netSaslTimeout || std::stoul( netSaslTimeout->first ) != saslTimeout )
  {
  MyUplink->Write( "%s CF %d sasl.timeout :%d",
    getCharYY().c_str(),
    time(nullptr),
    saslTimeout ) ;
  }
#endif // NEW_IRCU_FEATURES

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
	ipFloodMap[xIP(theClient->getIP()).GetNumericIP(true)] = 0;
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
		string silenceMask = createBanMask(theClient->getNickUserHost());

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

		if (ipFloodMap[xIP(theClient->getIP()).GetNumericIP(true)]>input_flood*5)
		{
			setLastRecieved(theClient, ::time(NULL));

			string silenceMask = createBanMask(theClient->getNickUserHost());

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
		string silenceMask = createBanMask(theClient->getNickUserHost());

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
	ipFloodMap[xIP(theClient->getIP()).GetNumericIP(true)] += 3;
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
	ipFloodMap[xIP(theClient->getIP()).GetNumericIP(true)] += commHandler->second->getFloodPoints();

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

	if( commHandler->second->Exec( theClient, Message ) )
		{
		incrementTotalCommands() ;
		incStat( "COMMANDS." + Command ) ;

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
			{
				LOGSQL_ERROR( SQLDb ) ;
			}
		}

		/* Log command to logfile here, if command logging enabled */
		if( commandLog )
			{
			std::string jsonMessage ;
			std::string jsonParams = "\"command\":\"" + Command + "\"" ;
			/*if( secure )
				jsonParams += ",\"secure\":true" ;
			else
				jsonParams += ",\"secure\":false" ;*/

			if( theClient->isModeR() )
				jsonParams += ",\"user_id\":" + std::to_string( theClient->getAccountID() ) + ",\"user_name\":\"" + theClient->getAccount() + "\"" ;

			jsonParams += "\"client_nick\":\"" + escapeJsonString( theClient->getNickName() ) + "\",\"client_userhost\":\"" + escapeJsonString( theClient->getRealUserHost() ) + "\"" ;

			jsonMessage = Command ;
			if( Command != "NEWPASS" && Command != "SUSPENDME" && Command != "LOGIN" && st.size() > 0 )
				jsonMessage += " " + escapeJsonString( st.assemble( 1 ) ) ;

			logger->writeLog( INFO, "cservice::OnPrivateMessage", jsonParams, jsonMessage ) ;
			}
		}
	}

xClient::OnPrivateMessage( theClient, Message ) ;
}

// This corresponds to channel action ... /me, /ame
void cservice::OnChannelCTCP( iClient* Sender, Channel* theChan, const string& CTCPCommand,
                const string& Message )
{
	sqlChannel* sqlChan = getChannelRecord(theChan->getName());
	if (!sqlChan) return;
	if (!sqlChan->getInChan()) return;
	if (Sender->getMode(iClient::MODE_SERVICES)) return;
	// Exempt users with channel access
	sqlUser* theUser = isAuthed(Sender, false);
	if (theUser)
	{
		int level = getEffectiveAccessLevel(theUser, sqlChan, false);
		if (level >= 1) return;
	}
	// Exempt users who are opped
	ChannelUser* tmpChanUser = theChan->findUser(Sender);
	if (tmpChanUser)
	{
		if (tmpChanUser->getMode(ChannelUser::MODE_O)) return;
	}
	if (!sqlChan->getFlag(sqlChannel::F_FLOODPRO)) return;
	StringTokenizer st(CTCPCommand);
	string cmd = string_upper(st[0]);
	string msg = st.assemble(1);
	unsigned short banLevel = 75;
	unsigned int banTime = 3 * 3600;
	unsigned int glineTime = 1 * 3600;
	string kickReason = "### Message Flood Protection Triggered ###";
	string repeatReason = "### Channel Repeat Protection Triggered ###";
	string glineReason = "Possible flood abuse";
	if ((cmd == "ACTION") && (sqlChan->getFloodMsg() > 0))
	{
		string Mask = Channel::createBan(Sender);
		sqlChan->setCurrentTime(currentTime());
		sqlChan->ExpireMessagesForChannel(sqlChan);
		sqlChan->handleNewMessage(sqlChannel::FLOOD_MSG, Mask, msg);
		unsigned int repeatCount = sqlChan->getRepeatMessageCount(msg).first;

		if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_NONE)
		{
			if (sqlChan->getTotalMessageCount(Mask) == sqlChan->getFloodMsg())
				Kick(theChan, Sender, kickReason);
			if (sqlChan->getTotalMessageCount(Mask) > sqlChan->getFloodMsg())
			{
				doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, kickReason);
				sqlChan->RemoveFlooderMask(Mask);
			}
			repeatCount = sqlChan->getRepeatMessageCount(msg,Mask).first;
			if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
			{
				if (repeatCount == sqlChan->getRepeatCount())
					Kick(theChan, Sender, repeatReason);
				if (repeatCount > sqlChan->getRepeatCount())
				{
					doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, repeatReason);
					sqlChan->RemoveFlooderMask(Mask);
				}
			}
		}
		else
		{
			if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_KICK)
			{

				if (sqlChan->getTotalMessageCount(Mask) == sqlChan->getFloodMsg())
					KickAllWithFloodMessage(theChan, msg, kickReason, false);
				if (sqlChan->getTotalMessageCount(Mask) > sqlChan->getFloodMsg())
				{
					doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, kickReason);
					sqlChan->RemoveFlooderMask(Mask);
				}

				if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
				{
					repeatCount = sqlChan->getRepeatMessageCount(msg,Mask).first;
					if (repeatCount > sqlChan->getRepeatCount())
					{
						doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, repeatReason);
						sqlChan->RemoveFlooderMask(Mask);
					}
					KickAllWithFloodMessage(theChan, msg, repeatReason, false);
				}
			}

			if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_BAN)
			{
				if (sqlChan->getTotalMessageCount(Mask) >= sqlChan->getFloodMsg())
					KickBanAllWithFloodMessage(theChan, msg, banLevel, banTime, kickReason);
				if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
					KickBanAllWithFloodMessage(theChan, msg, banLevel, banTime, repeatReason);
			}
			if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_GLINE)
			{
				if ((sqlChan->getTotalMessageCount(Mask) >= sqlChan->getFloodMsg())
					|| ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount())))
					GlineAllWithFloodMessage(sqlChan, msg, time_t(glineTime), glineReason);
			}
			if ((sqlChan->getTotalMessageCount(Mask) >= sqlChan->getFloodMsg())
					|| ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount())))
				sqlChan->setLastFloodTime(currentTime());
		}
		checkFloodproLevel(sqlChan, msg);
	}
	//if ((cmd == "PING") || (cmd == "VERSION") || (cmd == "TIME"))
	if ((cmd != "ACTION") && (sqlChan->getFloodCTCP() > 0))
	{
		kickReason = "### CTCP Flood Protection Triggered ###";
		string Mask = Channel::createBan(Sender);
		sqlChan->setCurrentTime(currentTime());
		sqlChan->ExpireMessagesForChannel(sqlChan);
		sqlChan->handleNewMessage(sqlChannel::FLOOD_CTCP, Mask, msg);
		unsigned int repeatCount = sqlChan->getRepeatMessageCount(msg).first;

		if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_NONE)
		{
			if (sqlChan->getTotalCTCPCount(Mask) == sqlChan->getFloodCTCP())
				Kick(theChan, Sender, kickReason);
			if (sqlChan->getTotalCTCPCount(Mask) > sqlChan->getFloodCTCP())
			{
				doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, kickReason);
				sqlChan->RemoveFlooderMask(Mask);
			}
			repeatCount = sqlChan->getRepeatMessageCount(msg,Mask).first;
			if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
			{
				if (repeatCount == sqlChan->getRepeatCount())
					Kick(theChan, Sender, repeatReason);
				if (repeatCount > sqlChan->getRepeatCount())
				{
					doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, repeatReason);
					sqlChan->RemoveFlooderMask(Mask);
				}
			}
		}
		else
		{
			if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_KICK)
			{
				if (sqlChan->getTotalCTCPCount(Mask) == sqlChan->getFloodCTCP())
					KickAllWithFloodMessage(theChan, msg, kickReason, false);
				if (sqlChan->getTotalCTCPCount(Mask) > sqlChan->getFloodCTCP())
				{
					doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, kickReason);
					sqlChan->RemoveFlooderMask(Mask);
				}

				if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
				{
					repeatCount = sqlChan->getRepeatMessageCount(msg,Mask).first;
					if (repeatCount > sqlChan->getRepeatCount())
					{
						doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, repeatReason);
						sqlChan->RemoveFlooderMask(Mask);
					}
					KickAllWithFloodMessage(theChan, msg, repeatReason, false);
				}
			}
			if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_BAN)
			{
				if (sqlChan->getTotalCTCPCount(Mask) >= sqlChan->getFloodCTCP())
					KickBanAllWithFloodMessage(theChan, msg, banLevel, banTime, kickReason);
				if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
					KickBanAllWithFloodMessage(theChan, msg, banLevel, banTime, repeatReason);
			}
			if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_GLINE)
			{
				if ((sqlChan->getTotalCTCPCount(Mask) >= sqlChan->getFloodCTCP())
					|| ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount())))
					GlineAllWithFloodMessage(sqlChan, msg, time_t(glineTime), glineReason);
			}
			if ((sqlChan->getTotalCTCPCount(Mask) >= sqlChan->getFloodCTCP())
					|| ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount())))
				sqlChan->setLastFloodTime(currentTime());
		}
		checkFloodproLevel(sqlChan, msg);
	}
	return xClient::OnChannelCTCP(Sender, theChan, CTCPCommand, Message);
}

void cservice::OnChannelMessage( iClient* Sender, Channel* theChan, const std::string& Message )
{
	sqlChannel* sqlChan = getChannelRecord(theChan->getName());
	if (!sqlChan) return;
	if (!sqlChan->getInChan()) return;
	if (Sender->getMode(iClient::MODE_SERVICES)) return;
	// Exempt users with channel access
	sqlUser* theUser = isAuthed(Sender, false);
	if (theUser)
	{
		int level = getEffectiveAccessLevel(theUser, sqlChan, false);
		if (level >= 1) return;
	}
	// Exempt users who are opped
	ChannelUser* tmpChanUser = theChan->findUser(Sender);
	if (tmpChanUser)
	{
		if (tmpChanUser->getMode(ChannelUser::MODE_O)) return;
	}
	if (!sqlChan->getFlag(sqlChannel::F_FLOODPRO)) return;
	if (!sqlChan->getFloodMsg()) return;
	unsigned short banLevel = 75;
	unsigned int banTime = 3 * 3600;
	unsigned int glineTime = 1 * 3600;
	string kickReason = "### Message Flood Protection Triggered ###";
	string repeatReason = "### Channel Repeat Protection Triggered ###";
	string glineReason = "Possible flood abuse";
	string Mask = Channel::createBan(Sender);
	sqlChan->setCurrentTime(currentTime());
	sqlChan->ExpireMessagesForChannel(sqlChan);
	sqlChan->handleNewMessage(sqlChannel::FLOOD_MSG, Mask, Message);
	unsigned int repeatCount = sqlChan->getRepeatMessageCount(Message).first;

	if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_NONE)
	{
		if (sqlChan->getTotalMessageCount(Mask) == sqlChan->getFloodMsg())
			Kick(theChan, Sender, kickReason);
		if (sqlChan->getTotalMessageCount(Mask) > sqlChan->getFloodMsg())
		{
			doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, kickReason);
			sqlChan->RemoveFlooderMask(Mask);
		}
		repeatCount = sqlChan->getRepeatMessageCount(Message,Mask).first;
		if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
		{
			if (repeatCount == sqlChan->getRepeatCount())
				Kick(theChan, Sender, repeatReason);
			if (repeatCount > sqlChan->getRepeatCount())
			{
				doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, repeatReason);
				sqlChan->RemoveFlooderMask(Mask);
			}
		}
	}
	else
	{
		if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_KICK)
		{
			if (sqlChan->getTotalMessageCount(Mask) == sqlChan->getFloodMsg())
				KickAllWithFloodMessage(theChan, Message, kickReason, false);
			if (sqlChan->getTotalMessageCount(Mask) > sqlChan->getFloodMsg())
			{
				doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, kickReason);
				sqlChan->RemoveFlooderMask(Mask);
			}

			if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
			{
				repeatCount = sqlChan->getRepeatMessageCount(Message,Mask).first;
				if (repeatCount > sqlChan->getRepeatCount())
				{
					doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, repeatReason);
					sqlChan->RemoveFlooderMask(Mask);
				}
				KickAllWithFloodMessage(theChan, Message, repeatReason, false);
			}
		}
		if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_BAN)
		{
			if (sqlChan->getTotalMessageCount(Mask) >= sqlChan->getFloodMsg())
				KickBanAllWithFloodMessage(theChan, Message, banLevel, banTime, kickReason);
			if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
				KickBanAllWithFloodMessage(theChan, Message, banLevel, banTime, repeatReason);
		}
		if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_GLINE)
		{
			if ((sqlChan->getTotalMessageCount(Mask) >= sqlChan->getFloodMsg())
				|| ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount())))
				GlineAllWithFloodMessage(sqlChan, Message, time_t(glineTime), glineReason);
		}
		if ((sqlChan->getTotalMessageCount(Mask) >= sqlChan->getFloodMsg())
				|| ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount())))
			sqlChan->setLastFloodTime(currentTime());
	}
	checkFloodproLevel(sqlChan, Message);
	xClient::OnChannelMessage(Sender, theChan, Message);
}

void cservice::OnChannelNotice( iClient* Sender, Channel* theChan, const std::string& Message )
{
	sqlChannel* sqlChan = getChannelRecord(theChan->getName());
	if (!sqlChan) return;
	if (!sqlChan->getInChan()) return;
	if (Sender->getMode(iClient::MODE_SERVICES)) return;
	// Exempt users with channel access
	sqlUser* theUser = isAuthed(Sender, false);
	if (theUser)
	{
		int level = getEffectiveAccessLevel(theUser, sqlChan, false);
		if (level >= 1) return;
	}
	// Exempt users who are opped
	ChannelUser* tmpChanUser = theChan->findUser(Sender);
	if (tmpChanUser)
	{
		if (tmpChanUser->getMode(ChannelUser::MODE_O)) return;
	}
	if (!sqlChan->getFlag(sqlChannel::F_FLOODPRO)) return;
	if (!sqlChan->getFloodNotice()) return;
	unsigned short banLevel = 75;
	unsigned int banTime = 3 * 3600;
	unsigned int glineTime = 1 * 3600;
	string kickReason = "### Notice Flood Protection Triggered ###";
	string repeatReason = "### Channel Repeat Protection Triggered ###";
	string glineReason = "Possible flood abuse";
	string Mask = Channel::createBan(Sender);
	sqlChan->setCurrentTime(currentTime());
	sqlChan->ExpireMessagesForChannel(sqlChan);
	sqlChan->handleNewMessage(sqlChannel::FLOOD_NOTICE, Mask, Message);
	unsigned int repeatCount = sqlChan->getRepeatMessageCount(Message).first;

	if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_NONE)
	{
		if (sqlChan->getTotalNoticeCount(Mask) == sqlChan->getFloodNotice())
			Kick(theChan, Sender, kickReason);
		if (sqlChan->getTotalNoticeCount(Mask) > sqlChan->getFloodNotice())
		{
			doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, kickReason);
			sqlChan->RemoveFlooderMask(Mask);
		}
		repeatCount = sqlChan->getRepeatMessageCount(Message,Mask).first;
		if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
		{
			if (repeatCount == sqlChan->getRepeatCount())
				Kick(theChan, Sender, repeatReason);
			if (repeatCount > sqlChan->getRepeatCount())
			{
				doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, repeatReason);
				sqlChan->RemoveFlooderMask(Mask);
			}
		}
	}
	else
	{
		if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_KICK)
		{
			if (sqlChan->getTotalNoticeCount(Mask) == sqlChan->getFloodNotice())
				KickAllWithFloodMessage(theChan, Message, kickReason, false);
			if (sqlChan->getTotalNoticeCount(Mask) > sqlChan->getFloodNotice())
			{
				doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, kickReason);
				sqlChan->RemoveFlooderMask(Mask);
			}

			if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
			{
				repeatCount = sqlChan->getRepeatMessageCount(Message,Mask).first;
				if (repeatCount > sqlChan->getRepeatCount())
				{
					doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, repeatReason);
					sqlChan->RemoveFlooderMask(Mask);
				}
				KickAllWithFloodMessage(theChan, Message, repeatReason, false);
			}
		}

		if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_BAN)
		{
			if (sqlChan->getTotalNoticeCount(Mask) >= sqlChan->getFloodNotice())
				KickBanAllWithFloodMessage(theChan, Message, banLevel, banTime, kickReason);
			if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
				KickBanAllWithFloodMessage(theChan, Message, banLevel, banTime, repeatReason);
		}
		if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_GLINE)
		{
			if ((sqlChan->getTotalNoticeCount(Mask) >= sqlChan->getFloodNotice())
				|| ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount())))
				GlineAllWithFloodMessage(sqlChan, Message, time_t(glineTime), glineReason);
		}
		if ((sqlChan->getTotalNoticeCount(Mask) >= sqlChan->getFloodNotice())
				|| ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount())))
			sqlChan->setLastFloodTime(currentTime());
	}
	checkFloodproLevel(sqlChan, Message);
	xClient::OnChannelNotice(Sender, theChan, Message);
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
ipFloodMap[xIP(theClient->getIP()).GetNumericIP(true)]+=5;

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
		"] Release 2.0.0B1");
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

void cservice::handleChannelPart( iClient* Sender, Channel* theChan, const string& Message )
{
	sqlChannel* sqlChan = getChannelRecord(theChan->getName());
	if (!sqlChan) return;
	if (Message.empty()) return;
	if (!sqlChan->getInChan()) return;
	if (Sender->getMode(iClient::MODE_SERVICES)) return;

    // Exempt users with channel access
    sqlUser* theUser = isAuthed(Sender, false);
    if (theUser)
    {
            int level = getEffectiveAccessLevel(theUser, sqlChan, false);
            if  (level >= 1) return;
    }

    // Exempt users who are opped
    ChannelUser* tmpChanUser = theChan->findUser(Sender);
	if (tmpChanUser)
	{
		if (tmpChanUser->getMode(ChannelUser::MODE_O)) return;
	}


	if (!sqlChan->getFlag(sqlChannel::F_FLOODPRO)) return;
	if (!sqlChan->getFloodMsg()) return;

	unsigned short banLevel = 75;
	unsigned int banTime = 3 * 3600;
	unsigned int glineTime = 1 * 3600;
	string kickReason = "### Message Flood Protection Triggered ###";
	string repeatReason = "### Channel Repeat Protection Triggered ###";
	string glineReason = "Possible flood abuse";
	string Mask = Channel::createBan(Sender);
	sqlChan->setCurrentTime(currentTime());
	sqlChan->handleNewMessage(sqlChannel::FLOOD_MSG, Mask, Message);
	unsigned int repeatCount = sqlChan->getRepeatMessageCount(Message).first;

	if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_NONE)
	{
		if (sqlChan->getTotalMessageCount(Mask) == sqlChan->getFloodMsg())
			Kick(theChan, Sender, kickReason);
		if (sqlChan->getTotalMessageCount(Mask) > sqlChan->getFloodMsg())
		{
			doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, kickReason);
			sqlChan->RemoveFlooderMask(Mask);
		}
		repeatCount = sqlChan->getRepeatMessageCount(Message,Mask).first;
		if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
		{
			if (repeatCount == sqlChan->getRepeatCount())
				Kick(theChan, Sender, repeatReason);
			if (repeatCount > sqlChan->getRepeatCount())
			{
				doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, repeatReason);
				sqlChan->RemoveFlooderMask(Mask);
			}
		}
	}
	else
	{
		if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_KICK)
		{
			if (sqlChan->getTotalMessageCount(Mask) == sqlChan->getFloodMsg())
				KickAllWithFloodMessage(theChan, Message, kickReason, false);
			if (sqlChan->getTotalMessageCount(Mask) > sqlChan->getFloodMsg())
			{
				doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, kickReason);
				sqlChan->RemoveFlooderMask(Mask);
			}
			repeatCount = sqlChan->getRepeatMessageCount(Message, Mask).first;
			if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
			{
				if (repeatCount == sqlChan->getRepeatCount())
					Kick(theChan, Sender, repeatReason);
				if (repeatCount > sqlChan->getRepeatCount())
				{
					doInternalBanAndKick(sqlChan, Sender, banLevel, banTime, repeatReason);
					sqlChan->RemoveFlooderMask(Mask);
				}
			}

		}
		if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_BAN)
		{
			if (sqlChan->getTotalMessageCount(Mask) >= sqlChan->getFloodMsg())
				KickBanAllWithFloodMessage(theChan, Message, banLevel, banTime, kickReason);
			if ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount()))
				KickBanAllWithFloodMessage(theChan, Message, banLevel, banTime, repeatReason);
		}
		if (sqlChan->getFloodproLevel() == sqlChannel::FLOODPRO_GLINE)
		{
			if ((sqlChan->getTotalMessageCount(Mask) >= sqlChan->getFloodMsg())
				|| ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount())))
				GlineAllWithFloodMessage(sqlChan, Message, time_t(glineTime), glineReason);
		}
		if ((sqlChan->getTotalMessageCount(Mask) >= sqlChan->getFloodMsg())
				|| ((sqlChan->getRepeatCount() > 0) && (repeatCount >= sqlChan->getRepeatCount())))
			sqlChan->setLastFloodTime(currentTime());
	}
	checkFloodproLevel(sqlChan, Message);
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
	LOG_MSG( TRACE, "Cache hit for {user_id}" )
		.with( "user_id", id )
		.logStructured() ;

	ptr->second->setLastUsed(currentTime());
	userCacheHits++;
	return ptr->second ;
	}

/*
 *  We didn't find anything in the cache, fetch the data from
 *  the backend and create a new sqlUser object.
 */

sqlUser* theUser = new (std::nothrow) sqlUser(this);
assert( theUser != 0 ) ;

if (theUser->loadData(id))
	{
 	sqlUserCache.insert(sqlUserHashType::value_type(id, theUser));

	LOG( TRACE, "There are {} elements in the cache.", sqlUserCache.size() ) ;

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
 *  Locates a cservice user record by 'id', the userId number of this user.
 */
sqlUser* cservice::getUserRecord(int Id)
{
	stringstream theQuery;
	theQuery	<< "SELECT user_name FROM users WHERE id = "
				<< Id
				<< ends;
	if (!SQLDb->Exec(theQuery, true))
	{
		LOGSQL_ERROR( SQLDb ) ;
		return NULL;
	} else if (SQLDb->Tuples() == 0)
	{
		LOG( ERROR, "getUserRecordUserIdQuery = 0 ({})!", Id ) ;
		return NULL;
	}
	string id = SQLDb->GetValue(0,0);
/*
 *  Check if this record is already in the cache.
 */

sqlUserHashType::iterator ptr = sqlUserCache.find(id);
if(ptr != sqlUserCache.end())
	{
	// Found something!
	LOG_MSG( TRACE, "Cache hit for {user_id}" )
		.with( "user_id", id )
		.logStructured() ;

	ptr->second->setLastUsed(currentTime());
	userCacheHits++;
	return ptr->second ;
	}

/*
 *  We didn't find anything in the cache, fetch the data from
 *  the backend and create a new sqlUser object.
 */

sqlUser* theUser = new (std::nothrow) sqlUser(this);
assert( theUser != 0 ) ;

if (theUser->loadData(id))
	{
 	sqlUserCache.insert(sqlUserHashType::value_type(id, theUser));

	LOG( TRACE, "There are {} elements in the cache.", sqlUserCache.size() ) ;

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
sqlChannel* cservice::getChannelRecord(const string& id, bool historysearch)
{

/*
 *  Check if this record is already in the cache.
 */

sqlChannelHashType::iterator ptr = sqlChannelCache.find(id);
if (ptr != sqlChannelCache.end())
{
	channelCacheHits++;
	ptr->second->setLastUsed(currentTime());

	// Return the channel to the caller
	return ptr->second;
}
else if (historysearch)
{
	sqlChannel* tempHistoryChan = new (std::nothrow) sqlChannel(this);
	assert( tempHistoryChan != 0 ) ;
	if (tempHistoryChan->loadData(id))
		return tempHistoryChan;
	else
		return NULL;
}

/*
 *  We didn't find anything in the cache.
 */

return 0;
}

/**
 *  Loads a channel from the cache by 'id'.
 */
sqlChannel* cservice::getChannelRecord(int id, bool historysearch)
{

/*
 *  Check if this record is already in the cache.
 */

sqlChannelIDHashType::iterator ptr = sqlChannelIDCache.find(id);
if (ptr != sqlChannelIDCache.end())
{
	channelCacheHits++;
	ptr->second->setLastUsed(currentTime());

	// Return the channel to the caller
	return ptr->second;
}
else if (historysearch)
{
	sqlChannel* tempHistoryChan = new (std::nothrow) sqlChannel(this);
	assert( tempHistoryChan != 0 ) ;
	if (tempHistoryChan->loadData(id))
		return tempHistoryChan;
	else
		return NULL;
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
	LOG_MSG( TRACE, "Cache hit for user-id:chan-id {user_id}:{chan_id}" )
		.with( "user", theUser )
		.with( "chan", theChan )
		.logStructured() ;

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

bool cservice::updateIPRlast_used(sqlUser* theUser, const string& ipr_ipvalue)
{
	stringstream theQuery;
	theQuery	<< "UPDATE ip_restrict SET last_used = date_part('epoch', CURRENT_TIMESTAMP)::int"
				<< " WHERE user_id = "
				<< theUser->getID()
				<< " AND value = '" << ipr_ipvalue << "'"
				<< ends;

	if( !SQLDb->Exec(theQuery, true ) )
		{
		LOGSQL_ERROR( SQLDb ) ;
		return false;
		}
	return true;
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

bool cservice::hasIPR( sqlUser* theUser )
{
	stringstream theQuery;
	theQuery	<< "SELECT type FROM "
			<< "ip_restrict WHERE user_id = "
			<< theUser->getID()
			<< " AND type = 1"
			<< ends;

	if( !SQLDb->Exec(theQuery, true ) )
		{
		LOGSQL_ERROR( SQLDb );
		return false;
        }

	if (SQLDb->Tuples() < 1)
	{
//#ifdef IPR_DEFAULT_REJECT
		/* no entries, fail them */
		return false;
//#else
		/* no entries, allow them */
//		return true;
//#endif
	}
	else
		return true;
}

unsigned int cservice::hasFP( sqlUser* theUser )
{
stringstream theQuery ;
theQuery	<< "SELECT COUNT(*) FROM "
			<< "users_fingerprints WHERE user_id = "
			<< theUser->getID()
			<< endl ;
#ifdef LOG_SQL
elog	<< "cservice::hasFP::sqlQuery> "
		<< theQuery.str()
		<< endl ;
#endif

if( !SQLDb->Exec(theQuery, true ) )
	{
	elog    << "cservice::hasFP> SQL Error: "
			<< SQLDb->ErrorMessage()
			<< endl;
	return 0 ;
	}

return SQLDb->Tuples() ;
}

/**
 *  Check a user against IP restrictions
 */
bool cservice::checkIPR( iClient* theClient, sqlUser* theUser )
{
	unsigned int ipr_ts;
	if (checkIPR(xIP(theClient->getIP()).GetNumericIP(),theUser,ipr_ts))
	{
		setIPRts(theClient, ipr_ts);
			return true;
	}
	return false;
}

/**
 *  Check a user against IP restrictions
 *  Note a NULL expiry means permanent
 */
bool cservice::checkIPR(const string& ip, sqlUser* theUser,unsigned int& ipr_ts)
{
	ipr_ts = 0;
	bool exp_match = false;
	bool ipr_res = true;

	//First check after expirable entries
	stringstream theQuery;
	theQuery	<< "SELECT value,expiry,added FROM ip_restrict WHERE (user_id = "
			<< theUser->getID()
			<< ") AND ((expiry IS NOT NULL) AND (expiry <> 0)) AND (type = 1)"
			<< ends;

	if( !SQLDb->Exec(theQuery, true ) )
		{
		/* SQL error, fail them */
		LOGSQL_ERROR( SQLDb );
		return false;
		}

	if (SQLDb->Tuples() > 0)
	{
		exp_match = true;
		/* cycle through results to find a match */
		for (unsigned int i=0; i < SQLDb->Tuples(); i++)
		{
			/* get some variables out of the db row */
			string ipr_ipvalue = SQLDb->GetValue(i, 0);
			unsigned int ipr_expiry = atoi(SQLDb->GetValue(i, 1).c_str());
			ipr_ts = atoi(SQLDb->GetValue(i, 2).c_str());

			if (ipr_expiry < (unsigned)currentTime()) continue;
			ipr_res = false;
			if (!match(ipr_ipvalue, ip))
			{
				/* IP restriction check passed - mark it against this user */
				updateIPRlast_used(theUser, ipr_ipvalue);
				return true;
			}
		}
	}
    //Now after permanent entries
    theQuery.str("");
    theQuery        << "SELECT value,added FROM ip_restrict WHERE (user_id = "
			<< theUser->getID()
			<< ") AND ((expiry IS NULL) OR (expiry = 0)) AND (type = 1)"
			<< ends;

    if( !SQLDb->Exec(theQuery, true ) )
    	{
		LOGSQL_ERROR( SQLDb );
        return false;
    	}

    if ((SQLDb->Tuples() < 1) && (!exp_match))
    {	
	// Any IPR forcing rule applies only for admins
	if (getAdminAccessLevel(theUser, true) > 0)
	{
	#ifdef IPR_DEFAULT_REJECT
            /* no entries, fail them */
            return false;
	#else
            /* no entries, allow them */
            return true;
	#endif
	}
    }
	if (SQLDb->Tuples() > 0) ipr_res = false;
	/* cycle through results to find a match */
	for (unsigned int i=0; i < SQLDb->Tuples(); i++)
	{
		/* get some variables out of the db row */
		string ipr_ipvalue = SQLDb->GetValue(i, 0);
		ipr_ts = atoi(SQLDb->GetValue(i, 1).c_str());
		if (!match(ipr_ipvalue, ip))
		{
			/* IP restriction check passed - mark it against this user */
			updateIPRlast_used(theUser, ipr_ipvalue);
			return true;
		}
	}
	return ipr_res;
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
		LOG( FATAL, "Unable to locate channel '*'!" ) ;
		::exit(0);
	}
	bool isAdmin = false;
	sqlLevel* theLevel = getLevelRecord(theUser, theChan);
	if (theLevel)
	{
		if (theLevel->getAccess() > 0)
			isAdmin = true;
	}
	if ((theUser->getFlag(sqlUser::F_TOTP_ENABLED)) && (!theUser->getFlag(sqlUser::F_TOTP_REQ_IPR)))
		return false;
	if (isAdmin) return true;
	if (theUser->getFlag(sqlUser::F_TOTP_REQ_IPR)) // && (theUser->getFlag(sqlUser::F_TOTP_ENABLED))
		return true;
	if ((hasIPR(theUser)) && (theUser->getFlag(sqlUser::F_TOTP_ENABLED)) && (theUser->getFlag(sqlUser::F_TOTP_REQ_IPR)))
		return true;
	if (hasIPR(theUser))
		return true;

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
	LOG( FATAL, "Unable to locate channel '*'! Sorry, I can't continue.." ) ;
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
	LOG( FATAL, "Unable to locate channel '{}'! Sorry, I can't continue..", coderChan ) ;
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
	else // the user forgot to ask with SET keyword
	{
		topic = "SET " + topic;
		thePair.second = topic;
		ptr = helpTable.find(thePair);
		if (ptr != helpTable.end())
			return ptr->second;
	}

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

elog	<< "*** [CMaster::loadHelpTable]: Loaded "
		<< helpTable.size()
		<< " help messages."
		<< endl;
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

elog	<< "*** [CMaster::loadTranslationTable]: Loaded "
		<< languageTable.size()
		<< " languages."
		<< endl;

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

	elog	<< "*** [CMaster::loadTranslationTable]: Loaded "
		<< translationTable.size()
		<< " translations."
		<< endl;
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
LOG( TRACE, "Checking for expired Suspensions.." ) ;
time_t expiredTime = currentTime();
stringstream expireQuery;
expireQuery	<< "SELECT user_id,channel_id FROM levels "
		<< "WHERE suspend_expires <= "
		<< expiredTime
		<< " AND suspend_expires <> 0"
		<< ends;

if( !SQLDb->Exec(expireQuery, true ) )
	{
	LOGSQL_ERROR( SQLDb );
	return ;
	}

/*
 *  Loops over the results set, and attempt to locate
 *  this level record in the cache.
 */

LOG( TRACE, "Found {} expired suspensions.", SQLDb->Tuples() ) ;

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
			LOG( TRACE, "Found level record in cache: {}:{}", resultPtr->first, resultPtr->second ) ;

			(Lptr->second)->setSuspendExpire(0);
			(Lptr->second)->setSuspendLevel(0);
			(Lptr->second)->setSuspendBy(string());
			(Lptr->second)->setSuspendReason(string());
			}

		/*
		 *  Execute a query to update the status in the db.
		 */

		} // for()
stringstream updateQuery;
updateQuery << "UPDATE levels SET suspend_expires = 0, suspend_level = 0, suspend_by = '', suspend_reason = ''"
		<< " WHERE suspend_expires <= "
		<< expiredTime
		<< " AND suspend_expires <> 0";

if( !SQLDb->Exec(updateQuery ) )
	{
	LOG( ERROR, "Unable to update record while unsuspending." ) ;
	LOGSQL_ERROR( SQLDb ) ;
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
LOG( TRACE, "Checking for expired bans.." ) ;

time_t expiredTime = currentTime();
stringstream expireQuery;
expireQuery	<< "SELECT channel_id,id FROM bans "
		<< "WHERE expires > 0 AND expires <= "
		<< expiredTime
		<< " ORDER BY channel_id"
		<< ends;

if( !SQLDb->Exec(expireQuery, true ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
	return ;
	}

/*
 *  Loops over the results set, and attempt to locate
 *  this ban in the cache.
 */

LOG( TRACE, "Found {} expired bans.", SQLDb->Tuples() ) ;

/*
 *  Place our query results into temporary storage, because
 *  we might have to execute other queries inside the
 *  loop which will invalidate our results set.
 */
std::vector < pair < unsigned int, unsigned int > > expireVector ;
std::vector < unsigned int > channelsVector ;

for (size_t i = 0 ; i < SQLDb->Tuples(); i++)
	{
	expireVector.emplace_back(
		atoi(SQLDb->GetValue(i, 0)),
		atoi(SQLDb->GetValue(i, 1))
		);
	channelsVector.push_back(atoi(SQLDb->GetValue(i, 0)));
	}

/* Delete duplicates from channelsVector. */
sort( channelsVector.begin(), channelsVector.end() ) ;
auto it = unique( channelsVector.begin(), channelsVector.end() ) ;
channelsVector.erase( it, channelsVector.end() ) ;

for (const auto& chanPtr : channelsVector)
	{
	sqlChannel* theChan = getChannelRecord( chanPtr );
	if (!theChan)
		{
		LOG( DEBUG, "Unable to find channel-ID {}", chanPtr ) ;

		continue;
		}

	Channel* tmpChan = Network->findChannel( theChan->getName() ) ;
	if (!tmpChan)
		{
		LOG( TRACE, "Unable to find network channel {}", theChan->getName() ) ;
		}

	LOG( TRACE, "Checking bans for {}", theChan->getName() ) ;

	/* Vector to store bans for each channel. */
	xServer::banVectorType	banVector ;

	for( const auto& [chanID, banID] : expireVector )
		{
		/* Skip bans not related to the present channel. */
		if( chanID != theChan->getID() ) continue ;

		/* Attempt to find the ban according to its id */
		auto ptr = theChan->banList.find( banID ) ;

		/* Was a ban found ? */
		if (ptr != theChan->banList.end())
			{
			sqlBan* theBan = ptr->second;
			theChan->banList.erase(ptr);

			/* Only add to unbanVector (to set -b) if the ban is set on the channel. */
			if( tmpChan && tmpChan->findBan( theBan->getBanMask() ) )
				{
				banVector.push_back( xServer::banVectorType::value_type(
					false, theBan->getBanMask() ) ) ;
				}

			LOG( TRACE, "Cleared ban {} from cache", theBan->getBanMask() ) ;

			delete(theBan); theBan = nullptr ;
			}
		else
			{
			LOG( DEBUG, "Unable to find ban with id {}", banID ) ;
			}
		} // for() expireVector

		if (tmpChan && !banVector.empty())
			UnBan( tmpChan, banVector ) ;
	} // for() channelsVector

stringstream deleteQuery;
deleteQuery	<< "DELETE FROM bans "
		<< "WHERE expires <= "
		<< expiredTime
		<< " AND expires <> 0"
		<< ends;

if( !SQLDb->Exec(deleteQuery ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
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
	LOG( INFO, "Beginning User cache cleanup:" ) ;
	sqlUserHashType::iterator ptr = sqlUserCache.begin();
	sqlUser* tmpUser;
	const auto startTime = std::chrono::high_resolution_clock::now();
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

			if( SQLDb->Exec(queryString, true ) )
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
							<< ",date_part('epoch', CURRENT_TIMESTAMP)::int,date_part('epoch', CURRENT_TIMESTAMP)::int)"
							<< ends;

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
			LOG( TRACE, "Purging user-id: {} from cache. Last used: {}", tmpUser->getID(), tmpUser->getLastUsed() ) ;
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
	LOG( INFO, "User cache cleanup complete; Removed {} user records in {} ms.",
		purgeCount, elapsedMs(startTime) ) ;
	LOG( INFO, "I also updated {} last_seen records for people logged in for >24 hours.",
		updateCount ) ;
}

void cservice::cacheExpireLevels()
{
	LOG( INFO, "Beginning Channel Level-cache cleanup:" ) ;

	/*
	 *  While we are at this, we'll clear out any FORCE'd access's
	 *  in channels.
	 */

	sqlChannelHashType::iterator ptr = sqlChannelCache.begin();
	while (ptr != sqlChannelCache.end())
	{
		sqlChannel* theChan = (ptr)->second;

		/* The channel is purged */
		if (theChan->getRegisteredTS() == 0)
		{
			++ptr;
			continue;
		}
		if(theChan->forceMap.size() > 0)
		{
			LOG( INFO, "Clearing out {} FORCE(s) from channel {}", theChan->forceMap.size(), theChan->getName() );
			theChan->forceMap.clear();
		}
		/*
		 * While we're here, lets see if this channel has been idle for a while.
		 * If so, we might want to part and turn off autojoin.. etc.
		 */

		if ( ((unsigned int)(currentTime() - theChan->getLastUsed()) >= partIdleChan)
			&& theChan->getInChan()
			&& !theChan->getFlag(sqlChannel::F_SPECIAL) )
		{
			/*
			 * So long! and thanks for all the fish.
			 */

			theChan->setInChan(false);
			theChan->removeFlag(sqlChannel::F_AUTOJOIN);
			theChan->commit();
			decrementJoinCount();
			writeChannelLog(theChan, me, sqlChannel::EV_IDLE, "");
			LOG_MSG( INFO, "I've just left {chan_name} because its too quiet." )
				.with( "chan", theChan )
				.logStructured() ;
			Part(theChan->getName(), "So long! (And thanks for all the fish)");
		}

		++ptr;
	}
	LOG( INFO, "Channel Level cache-cleanup complete." );
}

void cservice::performReops()
{
	for (xNetwork::channelIterator itr = Network->channels_begin(); itr != Network->channels_end(); itr++)
	{
		Channel* tmpChan = itr->second;
		ChannelUser* tmpBotUser = tmpChan->findUser(getInstance());
		if (!tmpBotUser)
		{
			continue;
		}

		// If the bot has no op, op it
		if (tmpChan && !tmpBotUser->getMode(ChannelUser::MODE_O))
			doTheRightThing(tmpChan);
	}
}

bool cservice::deleteUserFromTable(unsigned int userId, const string& table)
{
	/* We can safely do this, because the user is so long seen
	 * that is *very probably* is not in the users cache
	 */
	string userIdStr = "user_id";
	if (table == "pending") userIdStr = "manager_id";
	if (table == "users") userIdStr = "id";
	if (table == "pending_mgrchange") userIdStr = "new_manager_id";
	stringstream queryString;
	queryString << "DELETE FROM " << table << " WHERE " << userIdStr << " = "
    	 		<< userId
        		<< endl;

    if (!SQLDb->Exec(queryString,true))
    {
    	LOG( ERROR, "FAILED to delete user {} from {}", userId, table ) ;
		LOGSQL_ERROR( SQLDb ) ;
    	return false;
    }
    return true;
}

bool cservice::wipeUser(unsigned int userId, bool expired)
{
	sqlUser* tmpUser = getUserRecord(userId);
	assert(tmpUser != 0);
	string removeKey;
	time_t last_seen = 0;
	bool deleted = false;
	if (tmpUser->getFlag(sqlUser::F_NOPURGE))
	{
		goto cacheclean;
	}
	if (expired)
	{
		if (tmpUser->getSignupTS() + neverLoggedInUsersExpireTime > currentTime())
		{
			return false;
		}
		last_seen = tmpUser->getLastSeen();
	}
	deleteUserFromTable(userId,"acl");
	deleteUserFromTable(userId,"levels");
	deleteUserFromTable(userId,"notices");
	deleteUserFromTable(userId,"notes");
	deleteUserFromTable(userId,"pending");
	deleteUserFromTable(userId,"pending_emailchanges");
	deleteUserFromTable(userId,"pending_pwreset");
	deleteUserFromTable(userId,"pending_mgrchange");
	deleteUserFromTable(userId,"supporters");
	deleteUserFromTable(userId,"objections");
	deleteUserFromTable(userId,"userlog");
	deleteUserFromTable(userId,"fraud_list_data");
	deleteUserFromTable(userId,"users_lastseen");
	deleteUserFromTable(userId,"users");

	/* Notify services. */
	doXQToAllServices( "AnyCServiceRouting", "REMUSER " + std::to_string(userId));

	deleted = true;

	if (expired)
	{
		if (last_seen > 0)
			LOG( INFO, "User {} ({}) has expired", tmpUser->getUserName(), tmpUser->getEmail() ) ;
		else
			LOG( INFO, "User {} ({}) has expired (Never logged in)", tmpUser->getUserName(), tmpUser->getEmail() ) ;
	}
	else
		LOG( INFO, "Deleted(wipeUser) {} ({}) from the database.", tmpUser->getUserName(), userId ) ;

	cacheclean:
        sqlUserHashType::iterator usrItr = sqlUserCache.find(tmpUser->getUserName());
    	if (usrItr != sqlUserCache.end())
    	{
    		removeKey = usrItr->first;
    		delete(usrItr->second);
    		sqlUserCache.erase(removeKey);
    	}

	return deleted;
}

void cservice::ExpireUsers()
{
	if (UsersExpireDBDays == 0) return;
	LOG( INFO, "Performing Database Users Expire");
	int usersCount=0;
	stringstream queryString;
	queryString	<< "SELECT user_id FROM users_lastseen WHERE last_seen<="
				<< currentTime()-UsersExpireDBDays
				<< " AND last_seen > 0"
				<< ends;

	if( !SQLDb->Exec(queryString, true ))
	{
	   LOGSQL_ERROR( SQLDb ) ;
	   return;
	}
	if (SQLDb->Tuples() < 1)
	{
		LOG( INFO, "Removed 0 users from the database");
		return;
	}
	vector <unsigned int> UserIDs;
	for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		UserIDs.push_back(atoi(SQLDb->GetValue(i,0).c_str()));

	// Now all the users who are not present in users_lastseen table
	queryString.str(std::string());
	queryString	<< "SELECT id FROM users WHERE id NOT IN (SELECT user_id FROM users_lastseen)"
				<< ends;

	if( !SQLDb->Exec(queryString, true ))
	{
	   LOGSQL_ERROR( SQLDb );
	   return;
	}
	for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		UserIDs.push_back(atoi(SQLDb->GetValue(i,0).c_str()));

	for (unsigned int i = 0 ; i < UserIDs.size(); i++)
	    if (wipeUser(UserIDs.at(i),true)) ++usersCount;
    UserIDs.clear();
    LOG( INFO, "Removed {} users from the database",usersCount);
    return;
}

/**
 * Check the database to see if anything has changed.
 * TODO: Lots.
 */
void cservice::processDBUpdates()
{
	LOG( INFO, "[DB-UPDATE]: Looking for changes:" ) ;
	UpdatePendingOpLists();
	checkTrafficPass();
	updateChannels();
	updateUsers();
	updateLevels();
	updateBans();
	updateFingerprints();
	LOG( INFO, "[DB-UPDATE]: Complete.");
}

struct newChanData
{
	string chanName;
	unsigned int mngr_userId;
};

void cservice::updateChannels()
{
stringstream theQuery ;

theQuery	<< "SELECT "
			<< sql::channel_fields
			<< ",pending.manager_id as mngr_id,date_part('epoch', CURRENT_TIMESTAMP)::int as db_unixtime FROM "
			<< "channels LEFT JOIN pending ON channels.id = pending.channel_id " 
			<< "WHERE channels.last_updated >= "
			<< lastChannelRefresh
			<< " AND channels.registered_ts <> 0" ;

if( !SQLDb->Exec( theQuery, true ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
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

vector<newChanData*> newChanList;

for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
{
	sqlChannelHashType::iterator ptr =
		sqlChannelCache.find(SQLDb->GetValue(i, 1));

	if(ptr != sqlChannelCache.end())
		{
		/* Found something! Update it. */
		sqlChannel* updateChannel = ptr->second;
		updateChannel->setAllMembers(i);
		doAutoTopic(updateChannel);
		updates++;
		}
	else
		{
		/*
		 * Not in the cache.. must be a new channel.
		 */
		unsigned int mngr_id = (unsigned int)atoi(SQLDb->GetValue(i, "mngr_id"));
		if( mngr_id == 0 )
			{
			LOG_MSG( ERROR, "Channel {chan_name} has no manager assigned, skipping for now." )
			.with( "chan_name", SQLDb->GetValue(i, 1) )
			.logStructured() ;
			continue;
			}

		newChanData* newChan = new (std::nothrow) newChanData;
		newChan->chanName = SQLDb->GetValue(i, 1);
		newChan->mngr_userId = mngr_id;
		newChanList.push_back(newChan);

		LOG_MSG( INFO, "[DB-UPDATE]: Found new channel: {chan_name}" )
		.with( "chan_name", newChan->chanName )
		.logStructured() ;
		newchans++;
	}
}
if (!newChanList.empty())
{
	for (vector<newChanData*>::iterator itr = newChanList.begin(); itr != newChanList.end(); itr++)
	{
		sqlUser* mngrUser = getUserRecord((*itr)->mngr_userId);
		if (!sqlRegisterChannel(getInstance(), mngrUser, (*itr)->chanName))
			LOG_MSG( ERROR, "FAILED to sqlRegisterChannel for {chan_name} with manager {}", (*itr)->mngr_userId )
			.with( "chan_name", (*itr)->chanName )
			.logStructured() ;
	}
	newChanList.clear();
}

LOG( INFO, "[DB-UPDATE]: Refreshed {} channel records, loaded {} new channel(s).",
	updates, newchans ) ;

/* Set the "Last refreshed from channels table" timestamp. */
lastChannelRefresh = atoi(SQLDb->GetValue(0,"db_unixtime").c_str());
}

//Upde One specified user's level record (used for global unsuspension)
void cservice::updateUserLevels(sqlUser* theUser)
{
	stringstream theQuery ;

	theQuery	<< "SELECT "
				<< sql::level_fields
				<< " FROM levels WHERE user_id = "
				<< theUser->getID()
				<< ends;

	if( !SQLDb->Exec(theQuery, true ) )
		{
		LOGSQL_ERROR( SQLDb ) ;
		return;
		}

	if (SQLDb->Tuples() <= 0)
		{
		/* Nothing to see here.. */
		return;
		}

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

		if (ptr != sqlLevelCache.end())
		{
			/* Found something! Update it. */
			(ptr->second)->setAllMembers(i);
		}
		else
		{
			/*
			 * Must be a new level record, add it.
			 */
			sqlLevel* newLevel = new (std::nothrow) sqlLevel(this);
			newLevel->setAllMembers(i);
			sqlLevelCache.insert(sqlLevelHashType::value_type(thePair, newLevel));
		}
	}
	return;
}

/*
 * Check the levels table for recent updates.
 * if channelId is not 0, then we are looking after a new manager's 500 access
 */
void cservice::updateLevels(int channelId)
{
stringstream theQuery ;

theQuery	<< "SELECT "
			<< sql::level_fields
			<< ",date_part('epoch', CURRENT_TIMESTAMP)::int as db_unixtime FROM ";
		if (channelId > 0)
		{
			theQuery << "levels WHERE channel_id = " << channelId
					<< ends;
		}
		else
		{
			theQuery << "levels WHERE last_updated >= "
			<< lastLevelRefresh
			<< ends;
		}

if( !SQLDb->Exec(theQuery, true ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
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

	if (ptr != sqlLevelCache.end())
	{
		/* Found something! Update it. */
		(ptr->second)->setAllMembers(i);
		updates++;
	}
	else
	{
		/*
		 * Must be a new level record, add it.
		 */

		sqlLevel* newLevel = new (std::nothrow) sqlLevel(this);
		newLevel->setAllMembers(i);
		sqlLevelCache.insert(sqlLevelHashType::value_type(thePair, newLevel));
		newlevs++;
	}
}

LOG( INFO, "[DB-UPDATE]: Refreshed {} level record(s), loaded {} new level record(s).",
	updates, newlevs ) ;

/* Set the "Last refreshed from levels table" timestamp. */
lastLevelRefresh = atoi(SQLDb->GetValue(0,"db_unixtime").c_str());
}

vector<sqlUser*> cservice::getChannelManager(int channelId)
{
	vector<sqlUser*> resultVec;
	for (sqlLevelHashType::iterator itr = sqlLevelCache.begin(); itr != sqlLevelCache.end(); ++itr)
	{
		if (itr->first.second != channelId)
		{
			continue;
		}
		unsigned short access = itr->second->getAccess();
		if (access == 500)
		{
			sqlUser* managerUser = getUserRecord(itr->second->getUserId());
			if (managerUser)
				resultVec.push_back(managerUser);
		}
	}
	return resultVec;
}

/*
 * Check the users table to see if there have been any updates since we last looked.
 */
void cservice::updateUsers()
{
	stringstream theQuery ;

	theQuery	<< "SELECT "
				<< sql::user_fields
				<< ",date_part('epoch', CURRENT_TIMESTAMP)::int as db_unixtime FROM "
				<< "users WHERE last_updated >= "
				<< lastUserRefresh
				<< ends;

	if( !SQLDb->Exec(theQuery, true ) )
		{
		LOGSQL_ERROR( SQLDb ) ;
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
			sendAccountFlags(ptr->second);
			updates++;
			}
		}

	LOG( INFO, "[DB-UPDATE]: Refreshed {} user record(s).",
		updates);

	/* Set the "Last refreshed from Users table" timestamp. */
	lastUserRefresh = atoi(SQLDb->GetValue(0,"db_unixtime").c_str());
}

/*
 * Check the fingerpritns table to see if there have been any updates since we last looked.
 */
void cservice::updateFingerprints()
{
const std::string theQuery = "SELECT fingerprint,user_id FROM users_fingerprints" ;

#ifdef LOG_SQL
elog	<< "*** [CMaster::updateFingerprints]: sqlQuery: "
		<< theQuery
		<< endl ;
#endif

if( !SQLDb->Exec(theQuery, true ) )
	{
	elog	<< "*** [CMaster::updateFingerprints]: SQL error: "
			<< SQLDb->ErrorMessage()
			<< endl ;
	return;
	}

if( SQLDb->Tuples() <= 0 )
	{
	return ;
	}

fingerprintMap.clear() ;
for( unsigned int i = 0 ; i < SQLDb->Tuples() ; i++ )
	fingerprintMap.emplace( SQLDb->GetValue( i, 0 ), std::stoul( SQLDb->GetValue( i, 1 ) ) ) ;

LOG(INFO, "[DB-UPDATE]: Refreshed fingerprint(s)." ) ;
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
void cservice::OnTimer(const xServer::timerID& timer_id, void* parms)
{
/**
 * If the timer is called with a param, we check:
 * 1: Whether the param is a sqlChannel object; and
 * 2: Whether the timer_id mathes the sqlChannel's getLimitJoinTimer()
 */

if (parms != nullptr)
{
	LOG( TRACE, "Called with parms (JOINLIM)" ) ;
	sqlChannel *sqlChan = static_cast<sqlChannel *>(parms);
	if (sqlChan != nullptr && sqlChan->getLimitJoinTimer() == timer_id && sqlChan->getLimitJoinActive())
	{
		undoJoinLimits(sqlChan);
		LOG( DEBUG, "Calling undoJoinLimits for {} (ID: {})", sqlChan->getName(), sqlChan->getID() ) ;
	}
}

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
	expireGlines();
	expireWhitelist();

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
	webrelayQuery += "created_ts <= date_part('epoch', CURRENT_TIMESTAMP)::int ";
	webrelayQuery += "ORDER BY created_ts";
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
			if (!SQLDb->Exec(webrelayQuery) )
				LOGSQL_ERROR( SQLDb ) ;
		}
	}

	/* Refresh Timer */
	time_t theTime = time(NULL) + webrelayPeriod;
	webrelay_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
}

if (timer_id == pending_timerID)
{
	checkIncomings(true);

//	*** The Judge *** //
#ifdef USE_INTERNAL_THE_JUDGE
	checkValidUsersAndChannelsState();
	checkNewIncomings();
#endif
	loadPendingChannelList();
#ifdef USE_INTERNAL_THE_JUDGE
	//checkTrafficPass();	// Moved to a faster update_timerID
	checkObjections();
	checkAccepts();
	checkReviews();
	cleanUpReviews();
	cleanUpPendings();
//-------------------------
	ExpireUsers();	//(not The Judge member)
//-------------------------
#endif
	/* Refresh Timer */
	time_t theTime = time(NULL) +pendingChanPeriod;
	pending_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
}

if (timer_id == pendingNotif_timerID)
	{

	checkIncomings();

	/*
	 * Load a list of channels in NOTIFICATION stage and send them
	 * a notice.
	 */
	stringstream theQuery;
	theQuery	<<  "SELECT channels.name,channels.id,pending.created_ts"
				<< " FROM pending,channels"
				<< " WHERE channels.id = pending.channel_id"
				<< " AND pending.status IN (2, 8);"
				<< ends;

	unsigned int noticeCount = 0;
	if( SQLDb->Exec(theQuery, true ) )
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
				MyUplink->serverNotice(tmpChan,
				"This channel is currently being processed for registration. "
				"If you wish to view the details of the application or to object, please visit: "
				"%s?id=%i-%i", pendingPageURL.c_str(), created_ts, channel_id);
			}
		}
	}

	LOG( INFO, "Loaded Pending Channels notification list, I have just notified {} channels that they are under registration.",
		noticeCount ) ;

	/* Refresh Timer */
	time_t theTime = time(NULL) +pendingNotifPeriod;
	pendingNotif_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
	}

	if (timer_id == channels_flood_timerID)
	{
		checkChannelsFlood();
		time_t theTime = time(NULL) + channelsFloodPeriod;
		channels_flood_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
	}

	if (timer_id == prometheus_timerID)
	{
		updatePrometheusMetrics();
		time_t theTime = time(NULL) + 60;
		prometheus_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
	}
}

void cservice::updatePrometheusMetrics()
{
	if( !prometheus)  return ;

	/* Update the prometheus metrics */
	prometheus->setGauge( "memory_usage", getMemoryUsage() ) ;
	prometheus->setGauge( "cpu_usage", getCPUTime() ) ;
	prometheus->setGauge( "irc_clients", Network->clientList_size() ) ;
	prometheus->setGauge( "irc_channels", Network->channelList_size() ) ;
	prometheus->setGauge( "irc_servers", Network->serverList_size() ) ;
}
/*
 * Happens when the bot Joins a channel ...
 */
 void cservice::OnJoin( const std::string& chanName)
 {
	//elog << "cservice::OnJoin " << chanName << endl;
	xClient::OnJoin(chanName);
	Channel* tmpChan = Network->findChannel(chanName);
	if (!tmpChan)
	{
		LOG( TRACE, "Could not find network channel {}", chanName ) ;
		return;
	}

	doTheRightThing(tmpChan);
 }

/**
 * Register log handlers for custom objects.
 */
void cservice::registerLogHandlers() {
// Register sqlUser* handler
logger->registerObjectHandler<sqlUser>(
	[](std::map<std::string, std::string>& fields, const std::string& key, sqlUser* user) -> bool {
		if (!user) {
			fields[key + "_name"] = "nullptr";
			return true;
		}
		
		fields[key + "_id"] = std::to_string(user->getID());
		fields[key + "_name"] = user->getUserName();
		//fields[key + "_is_authed"] = user->isAuthed() ? "true" : "false";

		return true;
	});

// Register sqlChannel* handler
logger->registerObjectHandler<sqlChannel>(
	[](std::map<std::string, std::string>& fields, const std::string& key, sqlChannel* channel) -> bool {
		if (!channel) {
			fields[key + "_name"] = "nullptr";
			return true;
		}
		
		fields[key + "_id"] = std::to_string(channel->getID());
		fields[key + "_name"] = channel->getName();
		//fields[key + "_ts"] = std::to_string(channel->getRegisteredTS());

		return true;
	});

// Register sqlBan* handler
logger->registerObjectHandler<sqlBan>(
		[](std::map<std::string, std::string>& fields, const std::string& key, sqlBan* ban) -> bool {
			if (!ban) {
				fields[key + "_id"] = "nullptr";
				return true;
			}
			
			fields[key + "_id"] = std::to_string(ban->getID());
			fields[key + "_mask"] = ban->getBanMask();
	
			return true;
		});
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
	LOG( WARN, "Unable to locate relay channel {} on network!", relayChan ) ;
	return false;
	}

string message = string( "[" ) + nickName + "] " + buf ;
MyUplink->serverNotice(tmpChan, message);
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
		LOG( WARN, "Unable to locate prileved relay channel {} on network!", privrelayChan ) ;
		return false;
	}

	string message = string("[") + nickName + "] " + buf;
	MyUplink->serverNotice(tmpChan, message);
	return true;
}

/****************************************************/
/*   * * *   The Judge related functions   * * *    */
/****************************************************/

bool cservice::logTheJudgeMessage(const char* format, ... )
{
	char buf[1024] = { 0 };
	va_list _list ;

	va_start(_list, format);
	vsnprintf(buf, 1024, format, _list);
	va_end(_list);

	string logChannel;
	if (LogToAdminConsole)
		logChannel = relayChan;
	else
		logChannel = debugChan;
	/* try to locate the privileged relay channel */
	Channel* tmpChan = Network->findChannel(logChannel);
	if (!tmpChan)
	{
		LOG( WARN, "Unable to locate logging channel {} on network!", logChannel ) ;
		return false;
	}

	string message = string("[") + nickName + "] " + buf;
	MyUplink->serverNotice(tmpChan, message);
	return true;
}

void cservice::AddToValidResponseString(const string& resp)
{
	if (validResponseString == "") validResponseString = resp;
	else validResponseString += "," + resp;
	return;
}

bool cservice::isValidUser(const string& userName)
{
	validResponseString = "";
	sqlUser* tmpUser = getUserRecord(userName);
	if (!tmpUser)
	{
		validResponseString = "INEXISTENT";
		return false;
	}
	bool isValid = true;
	/* Check if user is NOREG/LOCKED/locked werification answer
	 * if user noreg type is 0, then it is NOREG, if type=5 then it is LOCKED, if type = 6 then user_name is referring to the locked verification answer.
	 */
	stringstream theQuery;
	theQuery	<< "SELECT user_name,email,type FROM noreg WHERE user_name IS NOT NULL OR email IS NOT NULL"
				<< ends;
	if (!SQLDb->Exec(theQuery, true))
	{
		LOGSQL_ERROR( SQLDb ) ;
		return false;
	}
	else if (SQLDb->Tuples() != 0)
	{
		ValidUserDataListType ValidUserDataList;
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
			ValidUserData current;
			current.UserName = SQLDb->GetValue(i,0);
			current.Email = SQLDb->GetValue(i,1);
			current.type = atoi(SQLDb->GetValue(i,2));
			ValidUserDataList.push_back(current);
		}
		for (ValidUserDataListType::const_iterator itr = ValidUserDataList.begin() ; itr != ValidUserDataList.end(); ++itr)
		{
			if (itr->type == 6)
			{
				if (itr->UserName[0] == '!')
				{
					string matchString = itr->UserName;
					matchString.erase(0,1);
					if (!match(matchString,tmpUser->getVerifData()))
					{
						if (validResponseString.find("INVALID VERIF") == string::npos)
							AddToValidResponseString("INVALID VERIF");
						isValid = false;
					}
				}
				else
				{   /* This should be the matchcase metod - TODO: need to find a solution */
					if (!casematch(itr->UserName,tmpUser->getVerifData()))
					//if (0 != strcmp(itr->UserName,tmpUser->getVerifData()))
					{
						if (validResponseString.find("INVALID VERIF") == string::npos)
							AddToValidResponseString("INVALID VERIF");
						isValid = false;
					}
				}
			}
			if (!match(itr->UserName,tmpUser->getUserName()) && (itr->UserName != "*"))
			{
				if (itr->type < 4)
				{
					if (validResponseString.find("NOREG") == string::npos)
						AddToValidResponseString("NOREG");
					isValid = false;
				}
				//This case is taken account by the webinterface
				//We skip this because we only look after *existing* usernames
				//if (itr->type == 4)
				//	AddToValidResponseString("FRAUD");
				if (itr->type == 5)
				{
					if (validResponseString.find("LOCKED") == string::npos)
						AddToValidResponseString("LOCKED");
					isValid = false;
				}
			}
			if (!match(fixAddress(itr->Email), fixAddress(tmpUser->getEmail())))
			{
				if (itr->type < 4)
				{
					if (validResponseString.find("INV.E-MAIL") == string::npos)
						AddToValidResponseString("INV.E-MAIL");
					isValid = false;
				}
				//This case is taken account by the webinterface
				//TODO: probably we should handle this case too
				//if (itr->type == 4)
				//	AddToValidResponseString("FRAUD");
				if (itr->type == 5)
				{
					if (validResponseString.find("LOCKED E-MAIL") == string::npos)
						AddToValidResponseString("LOCKED E-MAIL");
					isValid = false;
				}
			}
		}
		ValidUserDataList.clear();
	}
	if (tmpUser->getFlag(sqlUser::F_FRAUD))
	{
		AddToValidResponseString("FRAUD");
		isValid = false;
	}
	if (tmpUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
	{
		AddToValidResponseString("SUSPEND");
		isValid = false;
	}
	if (!isValid) return false;
	return true;
}

/* Check if channelname is locked */
bool cservice::isValidChannel(const string& chName)
{
	stringstream theQuery;
	theQuery	<< "SELECT channel_name,type FROM noreg WHERE channel_name IS NOT NULL"
				<< ends;
	if (!SQLDb->Exec(theQuery, true))
	{
		LOG( ERROR, "Error on Judge.isValidChannelchannel_nameQuery" ) ;
		LOGSQL_ERROR( SQLDb ) ;
		return false;
	} else if (SQLDb->Tuples() != 0)
	{
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
			if (!match(string_lower(SQLDb->GetValue(i,0)),string_lower(chName)))
			{
				if (atoi(SQLDb->GetValue(i,1)) < 4)
					validResponseString = "NOREG";
				if (atoi(SQLDb->GetValue(i,1)) == 5)
					validResponseString = "LOCKED";
				return false;
			}
	}
	return true;
}

// We use this when we need to know if a channel in the meantime got registered in the database (eg. by the webpage)
bool cservice::isDBRegisteredChannel(const string& chanName)
{
	stringstream theQuery;
	theQuery	<< "SELECT COUNT(*) FROM channels WHERE registered_ts <> 0 AND lower(name) = '"
				<< escapeSQLChars(string_lower(chanName))
				<< "'"
				<< ends;

	if( !SQLDb->Exec(theQuery, true ) )
	{
		LOGSQL_ERROR( SQLDb ) ;
		return false;
	}
	unsigned int isReg = atoi(SQLDb->GetValue(0,0));
	if (isReg > 0)
		return true;
	else
		return false;

}

bool cservice::RejectChannel(unsigned int chanId, const string& reason)
{
	stringstream theQuery;
	theQuery	<< "UPDATE pending SET status = '9',"
				<< " last_updated = date_part('epoch', CURRENT_TIMESTAMP)::int,"
				<< " decision_ts = date_part('epoch', CURRENT_TIMESTAMP)::int,"
				<< " decision = 'by The Judge: "
				<< reason
				<< "', reviewed = 'Y', reviewed_by_id = "
				<< ReviewerId
				<< " WHERE channel_id = " << chanId
				<< ends;

	if (!SQLDb->Exec(theQuery, true))
	{
		LOGSQL_ERROR( SQLDb ) ;
		return false;
	} else if (SQLDb->Tuples() != 0) return true; else return false;
}

bool cservice::ReviewChannel(unsigned int chanId)
{
	stringstream theQuery;
	theQuery	<< "UPDATE pending SET status = '8',"
				<< "last_updated = date_part('epoch', CURRENT_TIMESTAMP)::int,"
				<< "check_start_ts = date_part('epoch', CURRENT_TIMESTAMP)::int "
				<< "WHERE channel_id = " << chanId
				<< ends;

	if (!SQLDb->Exec(theQuery, true))
	{
		LOGSQL_ERROR( SQLDb ) ;
		return false;
	} else if (SQLDb->Tuples() != 0) return true; else return false;
}

bool cservice::AcceptChannel(unsigned int chanId, const string& reason)
{
	stringstream theQuery;
	theQuery	<< "UPDATE pending SET status = '3',"
				<< " last_updated = date_part('epoch', CURRENT_TIMESTAMP)::int,"
				<< " decision_ts = date_part('epoch', CURRENT_TIMESTAMP)::int,"
				<< " decision = 'by The Judge: "
				<< reason
				<< "', reviewed = 'Y', reviewed_by_id = "
				<< ReviewerId
				<< " WHERE channel_id = " << chanId
				<< ends;

	if (!SQLDb->Exec(theQuery, true))
	{
		LOGSQL_ERROR( SQLDb ) ;
		return false;
	} else if (SQLDb->Tuples() != 0) return true; else return false;
}

bool cservice::sqlRegisterChannel(iClient* theClient, sqlUser* mngrUsr, const string& chanName)
{
	bool byTheJudge = false;
	sqlUser* theUser = isAuthed(theClient, true);
	if (!theUser)
	{
		byTheJudge = true;
		theUser = new (std::nothrow) sqlUser(this);
		if (theClient == getInstance())
			theUser->setUserName("The Judge");
		else
			theUser->setUserName("Not Logged In");
	}
	if (!mngrUsr)
	{
		string errormsg = TokenStringsParams("FAILED to sqlRegister %s to non-existing manager user by %s",chanName.c_str(), theUser->getUserName().c_str());
		LOG( ERROR, errormsg ) ;
		logPrivAdminMessage(errormsg.c_str());
		return false;
	}

	unsigned int channel_ts = 0;
	Channel* tmpChan = Network->findChannel(chanName);
	channel_ts = tmpChan ? tmpChan->getCreationTime() : ::time(NULL);

	bool isUnclaimed = false;
	sqlChannel* newChan = getChannelRecord(chanName, true);
	if (newChan)
		isUnclaimed = true;
	if (!isUnclaimed)
	{
		newChan = new (std::nothrow) sqlChannel(this);
	}
	newChan->setName(chanName);
	newChan->setChannelTS(channel_ts);
	newChan->setRegisteredTS(currentTime());
	newChan->setChannelMode("+tnR");
	newChan->setLastUsed(currentTime());
	newChan->setFlag(sqlChannel::F_AUTOJOIN);
	newChan->setFlag(sqlChannel::F_NOTAKE);
	newChan->setNoTake(1);
	if (!isUnclaimed)
	{
		// Here we get the assigned Id by the database
		newChan->insertRecord();
		newChan->loadData(newChan->getName());
	}
	else
		newChan->commit();

	sqlChannelCache.insert(cservice::sqlChannelHashType::value_type(newChan->getName(), newChan));
	sqlChannelIDCache.insert(cservice::sqlChannelIDHashType::value_type(newChan->getID(), newChan));

	// First delete previous levels
	stringstream theQuery ;

	theQuery	<< "DELETE FROM levels WHERE channel_id = "
				<< newChan->getID()
				<< ends;

	if( !SQLDb->Exec(theQuery ) )
		{
		LOGSQL_ERROR( SQLDb ) ;
		return false ;
		}

	/*
	 * Delete Level records for this channel.
	 */
	cservice::sqlLevelHashType::const_iterator lvlptr = sqlLevelCache.begin();
	cservice::sqlLevelHashType::key_type lvlPair;

	while (lvlptr != sqlLevelCache.end())
	{
		sqlLevel* tmpLevel = lvlptr->second;
		unsigned int channel_id = lvlptr->first.second;

		if (channel_id == newChan->getID())
		{
			lvlPair = lvlptr->first;

			LOG( DEBUG, "Purging Level Record for: {} (UID: {})", lvlPair.second, lvlPair.first ) ;

			++lvlptr;
			sqlLevelCache.erase(lvlPair);

			delete(tmpLevel);
		} else
		{
			++lvlptr;
		}
	}

	cservice::pendingChannelListType::iterator ptr = pendingChannelList.find(newChan->getName());
	if (ptr != pendingChannelList.end())
	{
		sqlPendingChannel* pendingChan = ptr->second;
		pendingChan->commit();
		ptr->second = NULL;
		delete(pendingChan);
		pendingChannelList.erase(ptr);
	}

	theQuery.str("");
	theQuery	<< "DELETE FROM pending_chanfix_scores WHERE channel_id = "
				<< newChan->getID()
				<< ends;

	if( !SQLDb->Exec(theQuery ) )
		{
		LOGSQL_ERROR( SQLDb ) ;
		return false ;
		}

	logAdminMessage("%s (%s) has registered %s to %s", theClient->getNickName().c_str(),
		theUser->getUserName().c_str(), chanName.c_str(), mngrUsr->getUserName().c_str());

	NoteAllAuthedClients(mngrUsr, "Your channel application of %s is Accepted", chanName.c_str());

	if (theClient != getInstance())
	{
		Notice(theClient,
			getResponse(theUser,
				language::regged_chan,
				string("Registered channel %s")).c_str(),
				newChan->getName().c_str());
	}

	/*
	 *  Finally, commit a channellog entry, but just if theClient is not this client (X) because in that case it is already done by the webpage
	 */
	if (theClient != getInstance())
		writeChannelLog(newChan, theClient, sqlChannel::EV_REGISTER, "to " + mngrUsr->getUserName());

	/*
	 * Create the new manager.
	 */
	sqlLevel* newManager = new (std::nothrow) sqlLevel(this);
	newManager->setChannelId(newChan->getID());
	newManager->setUserId(mngrUsr->getID());
	newManager->setAccess(500);
	newManager->setAdded(currentTime());
	if (theClient == getInstance())
	{
		newManager->setAddedBy(getInstance()->getNickUserHost());
		newManager->setLastModifBy("*** The Judge ***");
	}
	else
	{
		newManager->setAddedBy("(" + theUser->getUserName() + ") " + theClient->getNickUserHost());
		newManager->setLastModifBy("(" + theUser->getUserName() + ") " + theClient->getNickUserHost());
	}
	newManager->setLastModif(currentTime());

	if (!newManager->insertRecord())
	{
		Notice(theClient, "Couldn't automatically add the level 500 Manager, check it doesn't already exist.");
		delete(newManager);
		return (false);
	}

	/*
	 * Insert this new 500 into the level cache.
	 */
	pair<int, int> thePair( newManager->getUserId(), newManager->getChannelId());
	sqlLevelCache.insert(cservice::sqlLevelHashType::value_type(thePair, newManager));

	getUplink()->RegisterChannelEvent(chanName, this);
	Join(newChan->getName(), string("+tnR"), newChan->getChannelTS(), true);
	newChan->setInChan(true);
	incrementJoinCount();

	//Send a welcome notice to the channel
	if (!welcomeNewChanMessage.empty())
		Notice(newChan->getName(), TokenStringsParams(welcomeNewChanMessage.c_str(), newChan->getName().c_str()).c_str());

	//Set a welcome topic of the new channel, only if the actual topic is empty and the channel has not been registered with the register command
#ifdef TOPIC_TRACK
	if (tmpChan && tmpChan->getTopic().empty() && !welcomeNewChanTopic.empty() && theClient == getInstance())
		Topic(tmpChan, welcomeNewChanTopic);
#endif

	if (byTheJudge)
	{
		delete theUser;
		theUser = NULL;
	}
	return true;
}

bool cservice::wipeChannel(unsigned int id)
{
	stringstream theQuery;
	theQuery	<< "DELETE FROM pending WHERE channel_id = " << id << ends;
	if (!SQLDb->Exec(theQuery, true))
		{
		LOG( ERROR, "Error on Judge.WipePendingQuery" ) ;
		LOGSQL_ERROR( SQLDb ) ;
		return false;
		}
	theQuery.str("");
	theQuery	<< "DELETE FROM objections WHERE channel_id = " << id << ends;
	if (!SQLDb->Exec(theQuery, true))
	{
		LOG( ERROR, "Error on Judge.WipeObjectionsQuery" ) ;
		LOGSQL_ERROR( SQLDb ) ;
		return false;
	}
	theQuery.str("");
	theQuery	<< "DELETE FROM supporters WHERE channel_id = " << id << ends;
	if (!SQLDb->Exec(theQuery, true))
	{
		LOG( ERROR, "Error on Judge.WipeSupportersQuery" ) ;
		LOGSQL_ERROR( SQLDb ) ;
		return false;
	}
	return true;
}

void cservice::checkValidUsersAndChannelsState()
{
	//First we check out channel<--> manager validity
	typedef std::vector <std::pair<unsigned int,string> > pendingChanListType;
	pendingChanListType chanList, managerList;
	unsigned int currentChanId = 0;
	stringstream theQuery;
	theQuery	<< "SELECT channel_id,channels.name,manager_id,users.user_name FROM channels,pending,users "
				<< "WHERE channels.id = pending.channel_id AND users.id = pending.manager_id "
				<< "AND (pending.status <> 3 AND pending.status <> 9 AND pending.status <> 4)"
				<< ends;
	if (!SQLDb->Exec(theQuery, true))
	{
		LOG( ERROR, "Error on Judge.validChanAndMngrQuery" ) ;
		LOGSQL_ERROR( SQLDb ) ;
		return;
	} else if (SQLDb->Tuples() != 0)
	{
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
			unsigned int chanId = atoi(SQLDb->GetValue(i,0));
			string chanName = SQLDb->GetValue(i,1);
			unsigned int mngrId = atoi(SQLDb->GetValue(i,2));
			string mngrUser = SQLDb->GetValue(i,3);
			//chanList.push_back(std::make_pair(chanId,chanName));
			//managerList.push_back(std::make_pair(mngrId,mngrUser));
			chanList.push_back(pendingChanListType::value_type(chanId,chanName));
			managerList.push_back(pendingChanListType::value_type(mngrId,mngrUser));
		}
	}
	if (!chanList.empty())
	{
		LOG( INFO, "Checking all pending channels validity ..." ) ;
		for (size_t i = 0; i < chanList.size(); ++i)
		{
			//logDebugMessage("Checking channel %s's validity ...",chanList[i].second.c_str());
			if (!isValidChannel(chanList.at(i).second))
			{
				string rejectReason = "Invalid channel (" + validResponseString +")";
				validResponseString.clear();
				RejectChannel(chanList[i].first,rejectReason);
				sqlUser* mgrUsr = getUserRecord(managerList[i].first);
				NoteAllAuthedClients(mgrUsr,"Your channel application of %s has been rejected with reason: %s", chanList[i].second.c_str(),rejectReason.c_str());
				logTheJudgeMessage("Rejected channel %s: ",rejectReason.c_str());
			} //else logDebugMessage("VALID");
			//logDebugMessage("Checking channel %s's Manager validity ...",chanList[i].second.c_str());
			if (!isValidUser(managerList[i].second))
			{
				string rejectReason = "Invalid applicant (" + validResponseString +")";
				validResponseString.clear();
				RejectChannel(chanList[i].first,rejectReason);
				sqlUser* mgrUsr = getUserRecord(managerList[i].first);
				NoteAllAuthedClients(mgrUsr,"Your channel application of %s has been rejected with reason: %s", chanList[i].second.c_str(),rejectReason.c_str());
				logTheJudgeMessage(rejectReason.c_str());
				logTheJudgeMessage("Rejected channel %s: ",chanList[i].second.c_str());
			}// else logDebugMessage("VALID");
		} //end of if (chanList.size() > 0)
	}
	//Now we iterate through all the supporters, and looking for invalidity
	//And we don't forget that one supporter might be supporting for multiple channels
	theQuery.str("");
	theQuery	<< "SELECT user_id, user_name,channel_id,channels.name FROM supporters,users,channels "
				<< "WHERE channels.id = supporters.channel_id AND users.id = supporters.user_id"
				<< ends;
	if (!SQLDb->Exec(theQuery, true))
	{
		LOG( ERROR, "Error on Judge.validSuppsQuery" ) ;
		LOGSQL_ERROR( SQLDb ) ;
		return;
	} else if (SQLDb->Tuples() != 0)
	{
		LOG( INFO, "Checking all supporters validity ..." ) ;
		//logDebugMessage("Found %i supporters,",SQLDb->Tuples());
		unsigned int suppUserId, chanId;
		string suppUserName,chanName;
		pendingChanListType suppChanList, suppList;
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
			suppUserId = atoi(SQLDb->GetValue(i,0));
			suppUserName = SQLDb->GetValue(i,1);
			chanId = atoi(SQLDb->GetValue(i,2));
			chanName = SQLDb->GetValue(i,3);
			suppChanList.push_back(pendingChanListType::value_type(chanId,chanName));
			suppList.push_back(pendingChanListType::value_type(suppUserId,suppUserName));
			//logDebugMessage("Processed %i",i);
		}
		for (size_t i=0; i<suppList.size(); i++)
		{
			suppUserName = suppList[i].second;
			chanName = suppChanList[i].second;
			chanId = suppChanList[i].first;
			string mngrUsr(""); // = managerList.find(chanId);
			//logDebugMessage("Checking validity of suporter %s supporting channel %s",suppUserName.c_str(),chanName.c_str());
			if (!isValidUser(suppUserName))
			{
				string rejectReason = "Invalid supporter: " + suppUserName + " (" + validResponseString +")";
				validResponseString.clear();
				logTheJudgeMessage(rejectReason.c_str());
				for (unsigned int j=0; j < chanList.size(); j++ )
				{
					if (chanList[j].first == chanId)
					{
						mngrUsr = managerList[j].second;
						//logDebugMessage("FOUND MANAGER %s",mngrUsr.c_str());
						break;
					}
				}
				if (mngrUsr.empty())
				{
					//logDebugMessage("Not Found the Manager");
					//logDebugMessage("Because the channel is or Rejected Or Accepted !");
					return;
				}
				sqlUser* mgrUsr = getUserRecord(mngrUsr);
				NoteAllAuthedClients(mgrUsr,rejectReason.c_str());
				if (currentChanId != chanId)
				{
					RejectChannel(chanId,rejectReason);
					logTheJudgeMessage("Rejected channel %s",chanName.c_str());
					NoteAllAuthedClients(mgrUsr,"Your channel application of %s has been rejected with reason: %s", chanName.c_str(),rejectReason.c_str());
				}
				currentChanId = chanId;
			} //isValidUser
		} //for
		suppChanList.clear();
		suppList.clear();
	} //Tuples != 0
	else if (!chanList.empty()) LOG( WARN, "WARNING: Not found any supporter!");
	chanList.clear();
	managerList.clear();
	return;
}

void cservice::checkNewIncomings()
{
	string chanName;
	unsigned int chanId;
	//int mngrId;
	string mngrUser;
	string reason = "Failed supporters confirmation.";
	std::vector<std::pair<std::pair<int,string>, string> > rejectList;
	unsigned int pendingTime = SupportDays * JudgeDaySeconds; //currentTime()
	stringstream theQuery;
//	theQuery	<< "SELECT channels.name,channels.id,manager_id,users.user_name FROM channels,pending,users WHERE channels.id = pending.channel_id "
	theQuery	<< "SELECT channels.name,channels.id,users.user_name FROM channels,pending,users WHERE channels.id = pending.channel_id "
				<< "AND pending.status = 0 AND (pending.created_ts + "
				<< pendingTime
				<< ") < date_part('epoch', CURRENT_TIMESTAMP)::int "
				<< " AND users.id = manager_id"
				<< ends;
	if (!SQLDb->Exec(theQuery, true))
	{
		LOG( ERROR, "Error on Judge.IncomingQuery" );
		LOGSQL_ERROR( SQLDb ) ;
		return;
	} else if (SQLDb->Tuples() != 0)
	{
		logTheJudgeMessage("List of expiring Incoming applications:");
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
			chanName = SQLDb->GetValue(i,0);
			chanId = atoi(SQLDb->GetValue(i,1));
			mngrUser = SQLDb->GetValue(i,2);
			rejectList.push_back(std::make_pair(std::make_pair(chanId,chanName),mngrUser));
			logTheJudgeMessage("Rejected channel application %s with reason: %s",chanName.c_str(),reason.c_str());
		}
		if (!rejectList.empty())
		{
			for (unsigned int i=0; i<rejectList.size(); i++ )
			{
				sqlUser* mgrUsr = getUserRecord(rejectList[i].second);
				reason = "Failed supporters confirmation.";
				RejectChannel(rejectList[i].first.first,reason);
				NoteAllAuthedClients(mgrUsr,"Your channel application of %s has been rejected with reason: %s", rejectList[i].first.second.c_str(),reason.c_str());
			}
		}
		rejectList.clear();
	}// else logDebugMessage("No Expiring Incoming Applications Found!");
	return;
}

void cservice::checkTrafficPass()
{
	if (pendingChannelList.size() == 0)
	{
		//logDebugMessage("No Expiring pending channel application found");
		return;
	}

	pendingChannelListType::iterator ptr = pendingChannelList.begin();
	while (ptr != pendingChannelList.end())
	{
		sqlPendingChannel* pendingChan = ptr->second;

		//Let's begin with optimism
		bool JoinsPass = true;
		bool uniqueJoinsPass = true;
		bool minSupportersPass = true;
		bool minSupportersJoinPass = true;
		string rejectReason = "";

		unsigned int trafficTime = MaxDays * JudgeDaySeconds;
		time_t elapsedDays = pendingChan->checkStart + time_t(trafficTime);
		//Check if the channel was visited at least at ONCE by the required MinSupporters count supporters
		unsigned int actualMinSupporters = (unsigned int)pendingChan->uniqueSupporterList.size();
		if (actualMinSupporters < MinSupporters)
		{
			//rejectReason = "Insuffucient number of supporters that visited the channel";
			minSupportersPass = false;
			if (elapsedDays < currentTime())
				logTheJudgeMessage("Insufficient number of supporters that visited the pending channel %s (%i/%i)",ptr->first.c_str(),actualMinSupporters,MinSupporters);
		}
		else // <- if yes, we check if one of the supporters has an insufficient MinSupportersJoin joincount
		{
			sqlPendingChannel::trafficListType::iterator ptr2 = pendingChan->uniqueSupporterList.begin();
			while (ptr2 != pendingChan->uniqueSupporterList.end())
			{
				if (ptr2->second->join_count < MinSupportersJoin)
				{
					string suppUserName = "\002Error\002";
					sqlUser* supporterUser = getUserRecord(atoi(ptr2->first));
					if (supporterUser)
						suppUserName = supporterUser->getUserName();
					unsigned int actualMinSupportersJoin = ptr2->second->join_count;
					minSupportersJoinPass = false;
					if (elapsedDays < currentTime())
						logTheJudgeMessage("Insufficient supporter joincount of supporter user %s (%i/%i) on pending channel %s",suppUserName.c_str(),actualMinSupportersJoin,MinSupportersJoin,ptr->first.c_str());
				}
				++ptr2;
			} //end while
		} //end else

		//Next, we check after general channel activity:
		//(total) join_count, and the unique_join_count
		if (pendingChan->unique_join_count < UniqueJoins)
		{
			uniqueJoinsPass = false;
			if (elapsedDays < currentTime())
				logTheJudgeMessage("Insufficient number of IP's (%i/%i) that visited the pending channel %s",pendingChan->unique_join_count,UniqueJoins,ptr->first.c_str());
		}
		if (pendingChan->join_count < Joins)
		{
			JoinsPass = false;
			if (elapsedDays < currentTime())
				logTheJudgeMessage("Insufficient number of joincounts (%i/%i) that visited the pending channel %s",pendingChan->join_count,Joins,ptr->first.c_str());
		}
		//We all passed, so we move the channel to the next Notification phase
		if ((JoinsPass) && (uniqueJoinsPass) && (minSupportersPass) && (minSupportersJoinPass))
		{
			stringstream theQuery;
			theQuery 	<< "UPDATE pending SET status = '2',"
						<< "check_start_ts = date_part('epoch', CURRENT_TIMESTAMP)::int,"
						<< "last_updated = date_part('epoch', CURRENT_TIMESTAMP)::int "
						<< "WHERE channel_id = " << pendingChan->channel_id
						<< ends;
			if( !SQLDb->Exec(theQuery, true ) )
				{
				LOG( ERROR, "Error on update pending trafficCheck -> notification" ) ;
				LOGSQL_ERROR( SQLDb ) ;
				}
			else
			logTheJudgeMessage("Channel %s has passed traffic checking, successfully moved to Notification stage",ptr->first.c_str());
			pendingChan->commit();
			ptr->second = NULL;
			delete(pendingChan);
			pendingChannelList.erase(ptr++);
			continue;
		}
		if (elapsedDays < currentTime())
		{
			MyUplink->UnRegisterChannelEvent(ptr->first, this);
			if ((!uniqueJoinsPass) || (!JoinsPass))
				rejectReason = "Insufficient channel activity";
			if ((!minSupportersPass) || (!minSupportersJoinPass))
				rejectReason = "Insufficient supporter activity. *All* supporters need to be active users of the channel";
			if (!rejectReason.empty())
			{
				RejectChannel(pendingChan->channel_id,rejectReason);
				logTheJudgeMessage("Rejecting channel %s with reason: %s",ptr->first.c_str(),rejectReason.c_str());
				pendingChan->commit();
				ptr->second = NULL;
				delete(pendingChan);
				pendingChannelList.erase(ptr++);
				continue;
			}
		}
		++ptr;
	} /* while() */
}

void cservice::checkObjections()
{
	if (!DecideOnObject) return;
	std::vector<std::pair<std::pair<int,string>, string> > objectList;
	unsigned int notifTime = NotifyDays * JudgeDaySeconds;
	int actualChan = 0;
	stringstream theQuery;
	theQuery	<< "SELECT channels.name,channels.id,users.user_name FROM channels,pending,users,objections "
				<< "WHERE channels.id = pending.channel_id "
				<< "AND pending.status = 2 AND (pending.check_start_ts + "
				<< notifTime
				<< ") < date_part('epoch', CURRENT_TIMESTAMP)::int "
				<< " AND users.id = manager_id"
				<< " AND objections.channel_id = pending.channel_id"
				//<< " LIMIT 1"
				<< ends;

	if (!SQLDb->Exec(theQuery, true))
	{
		LOG( ERROR, "Error on Judge.ObjectionkQuery" );
		LOGSQL_ERROR( SQLDb ) ;
		return;
	}
	else if (SQLDb->Tuples() != 0)
	{
		logTheJudgeMessage("List of applications moved to Ready to review:");
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
			string chanName = SQLDb->GetValue(i,0);
			int chanId = atoi(SQLDb->GetValue(i,1));
			string mngrUser = SQLDb->GetValue(i,2);
			if (chanId != actualChan) //more than one objection per channel lead to multiple db hit, so we need to limit to one result
			{
				actualChan = chanId;
				objectList.push_back(std::make_pair(std::make_pair(chanId,chanName),mngrUser));
				logTheJudgeMessage(chanName.c_str());
			}
		}
	}
	if (!objectList.empty())
		for (unsigned int i=0; i<objectList.size(); i++ )
		{
			ReviewChannel(objectList[i].first.first);
			logTheJudgeMessage("Channel %s has been moved to Ready to review",objectList[i].first.second.c_str());
			sqlUser* mgrUsr = getUserRecord(objectList[i].second.c_str());
			NoteAllAuthedClients(mgrUsr,"Your channel application of %s is now at state Ready to review", objectList[i].first.second.c_str());
		}
	objectList.clear();
return;
}

void cservice::checkAccepts()
{
	std::vector<std::pair<std::pair<int,string>, std::pair<string, char> > > acceptList;
	unsigned int notifTime = NotifyDays * JudgeDaySeconds;
	stringstream theQuery;
	theQuery	<< "SELECT channels.name,channels.id,users.user_name,pending.reviewed FROM channels,pending,users "
				<< "WHERE channels.id = pending.channel_id "
				<< "AND pending.status = 2 AND (pending.check_start_ts + "
				<< notifTime
				<< ") < date_part('epoch', CURRENT_TIMESTAMP)::int "
				<< " AND users.id = manager_id"
				<< ends;

	if (!SQLDb->Exec(theQuery, true))
	{
		LOG( ERROR, "Error on Judge.AcceptQuery" );
		LOGSQL_ERROR( SQLDb ) ;
		return;
	}
	else if (SQLDb->Tuples() != 0)
	{
		//logTheJudgeMessage("List of completed applications:");
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
			string chanName = SQLDb->GetValue(i,0);
			int chanId = atoi(SQLDb->GetValue(i,1));
			string mngrUser = SQLDb->GetValue(i,2);
			char acc = (char)SQLDb->GetValue(i,3)[0];
			acceptList.push_back(std::make_pair(std::make_pair(chanId,chanName),std::make_pair(mngrUser,acc)));
			//logTheJudgeMessage(chanName.c_str());
		}
	}
	if (!acceptList.empty())
	for (unsigned int i=0; i<acceptList.size(); i++ )
	{
		sqlUser* mgrUsr = getUserRecord(acceptList[i].second.first.c_str());
		bool reviewed = false;
		if (acceptList[i].second.second == 'Y')
			reviewed = true;
		if (DecideOnCompleted || (reviewed == false && RequireReview))
		{
			ReviewChannel(acceptList[i].first.first);
			logTheJudgeMessage("Channel application %s completed, moved to 'Ready to review'",acceptList[i].first.second.c_str());
			NoteAllAuthedClients(mgrUsr,"Your channel application of %s is now in 'Ready to review'", acceptList[i].first.second.c_str());
		}
		else if ((reviewed == true || !RequireReview) && !DecideOnCompleted)
		{
			AcceptChannel(acceptList[i].first.first,"ACCEPTED");
			if (!sqlRegisterChannel(getInstance(), mgrUsr, acceptList[i].first.second.c_str()))
				LOG( ERROR, "FAILED to sqlRegisterChannel" );
		}
	}
	acceptList.clear();
}

void cservice::checkReviews()
{
	std::vector<std::pair<std::pair<int,string>, std::pair<string, char> > > acceptList;
	stringstream theQuery;
	theQuery	<< "SELECT channels.name,channels.id,users.user_name,pending.reviewed FROM channels,pending,users "
				<< "WHERE channels.id = pending.channel_id "
				<< "AND pending.status = 8 "
				<< "AND users.id = manager_id "
				<< "AND NOT EXISTS (SELECT pending.channel_id FROM pending,objections WHERE pending.channel_id = objections.channel_id)"
				<< ends;

	if (!SQLDb->Exec(theQuery, true))
		{
		LOG( ERROR, "Error on Judge.checkReviewsQuery" );
		LOGSQL_ERROR( SQLDb ) ;
		return;
		}
	else if (SQLDb->Tuples() != 0)
	{
		//logTheJudgeMessage("List of completed applications:");
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
			string chanName = SQLDb->GetValue(i,0);
			int chanId = atoi(SQLDb->GetValue(i,1));
			string mngrUser = SQLDb->GetValue(i,2);
			char acc = (char)SQLDb->GetValue(i,3)[0];
			acceptList.push_back(std::make_pair(std::make_pair(chanId,chanName),std::make_pair(mngrUser,acc)));
			//logTheJudgeMessage(chanName.c_str());
		}
	}
	if (!acceptList.empty())
	for (unsigned int i=0; i<acceptList.size(); i++ )
	{
		sqlUser* mgrUsr = getUserRecord(acceptList[i].second.first.c_str());
		bool reviewed = false;
		if (acceptList[i].second.second == 'Y')
			reviewed = true;
                if ((reviewed == true || !RequireReview) && !DecideOnCompleted)
		{
			AcceptChannel(acceptList[i].first.first,"ACCEPTED");
			if (!sqlRegisterChannel(getInstance(), mgrUsr, acceptList[i].first.second.c_str()))
				LOG( ERROR, "FAILED to sqlRegisterChannel" );
		}
	}
	acceptList.clear();
}

/**
 * After a time we clenup any "never" reviewed channel
 */
void cservice::cleanUpReviews()
{
	if (!ReviewsExpireTime) return;
	std::vector<std::pair<std::pair<int,string>, string> > reviewList;
	unsigned int reviewTime = ReviewsExpireTime * JudgeDaySeconds;
	stringstream theQuery;
	theQuery	<< "SELECT channels.name,channels.id,users.user_name FROM channels,pending,users "
	   			<< "WHERE channels.id = pending.channel_id "
	   			<< "AND pending.status = 8 AND (pending.check_start_ts + "
	   			<< reviewTime
	   			<< ") < date_part('epoch', CURRENT_TIMESTAMP)::int "
	   			<< " AND users.id = manager_id"
	   			<< ends;

	if (!SQLDb->Exec(theQuery, true))
	{
		LOG( ERROR, "Error on Judge.cleanUpReviewsQuery" );
		LOGSQL_ERROR( SQLDb ) ;
		return;
	} else if (SQLDb->Tuples() != 0)
	{
		logTheJudgeMessage("List of Wiped applications:");
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
			string chanName = SQLDb->GetValue(i,0);
			int chanId = atoi(SQLDb->GetValue(i,1));
			string mngrUser = SQLDb->GetValue(i,2);
			reviewList.push_back(std::make_pair(std::make_pair(chanId,chanName),mngrUser));
			logTheJudgeMessage(chanName.c_str());
		}
	}
	if (!reviewList.empty())
		for (unsigned int i=0; i<reviewList.size(); i++ )
	    {
	    	if (wipeChannel(reviewList[i].first.first))
	    		logTheJudgeMessage("Expired and wiped Ready to Review channel application %s",reviewList[i].first.second.c_str());
	    	 else
	    		logTheJudgeMessage("(The Judge) Failed to wipeChannel(ready to Review) %s",reviewList[i].first.second.c_str());
	    }
	    reviewList.clear();
}

// After a time, we cleanup the database from old application datas: pending channels, supporters, etc
//But this applies *only* for Accepted OR Rejected channels !
void cservice::cleanUpPendings()
{
	//If PendingsExpireTime == 0 than feature is disabled
	if (!PendingsExpireTime) return;
	unsigned int expireTime = PendingsExpireTime * JudgeDaySeconds;
	stringstream theQuery;
		theQuery	<< "SELECT channel_id FROM pending "
					<< "WHERE (pending.status = 3 OR pending.status = 9 OR pending.status = 4) "
					<< "AND (pending.last_updated + "
					<< expireTime
					<< ") < date_part('epoch', CURRENT_TIMESTAMP)::int "
					<< ends;
		if (!SQLDb->Exec(theQuery, true))
		{
			LOG( ERROR, "Error on Judge.checkPendingCleanupsQuery" ) ;
			LOGSQL_ERROR( SQLDb ) ;
			return;
		} else if (SQLDb->Tuples() != 0)
		{
			LOG( INFO, "Found {} channel(s) to pendingCleanup", SQLDb->Tuples() ) ;
			vector <unsigned int> wipeChanList;
			for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
				wipeChanList.push_back(atoi(SQLDb->GetValue(i,0)));
			for (unsigned int i = 0; i < wipeChanList.size(); i++)
			{
				wipeChannel(wipeChanList[i]);
				LOG( INFO, "Wiped channel {}", wipeChanList[i] );
			}
			wipeChanList.clear();
		} //else logDebugMessage("No pendingCleanup found!");
	return;
}

/*     * * *   End The Judge functions   * * *      */

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

void cservice::OnChannelModeV( Channel* theChan, ChannelUser* theChanUser,
	const xServer::voiceVectorType& theTargets)
{
	sqlChannel* reggedChan = getChannelRecord(theChan->getName());
	if(!reggedChan)
		{
	//	elog	<< "cservice::OnChannelModeV> WARNING, unable to "
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

#ifndef USE_NOVOICE
	if (reggedChan->getFlag(sqlChannel::F_NOVOICE))
		reggedChan->removeFlag(sqlChannel::F_NOVOICE);
	return;
#endif

	// List of clients to devoice.
	vector< iClient* > deVoiceList;

	for( xServer::voiceVectorType::const_iterator ptr = theTargets.begin() ;
		ptr != theTargets.end() ; ++ptr )
		{
		ChannelUser* tmpUser = ptr->second;
		bool polarity = ptr->first;

		if (polarity)
			{
			// If somebody is being voiced.
			// If the channel is NOVOICE, devoice everyone who tries to get voiced!
			if (reggedChan->getFlag(sqlChannel::F_NOVOICE))
				{
				if ( !tmpUser->getClient()->getMode(iClient::MODE_SERVICES) )
					deVoiceList.push_back(tmpUser->getClient());
				}
			}
		} // for()

	/*
	 *  Send notices and perform the devoice's. (But don't deop anything thats +k).
	 */

	if( !deVoiceList.empty() )
	{
		if ((theChanUser) && (reggedChan->getFlag(sqlChannel::F_NOVOICE)) )
		{
			Notice(theChanUser->getClient(),
				/*getResponse(theUser,
					language::novoice_set,*/string("The NOVOICE flag is set on %s")/*)*/.c_str(),
				reggedChan->getName().c_str());

		}
		if (theChanUser->getMode(ChannelUser::MODE_O))
			if (!theChanUser->getClient()->getMode(iClient::MODE_SERVICES))
				DeOp(theChan,theChanUser->getClient());
		DeVoice(theChan, deVoiceList);
	}
	return;
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
		if (tmpUser->getClient() == me)
			doTheRightThing(theChan);
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
			xClient::Op(theChan, me);
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
			"### Mass Deop Protection Triggered ###");
	}
}

void cservice::OnEvent( const eventType& theEvent,
	void* data1, void* data2, void* data3, void* data4 )
{
switch( theEvent )
	{
	case EVT_NETBREAK:
		{
		iServer* theServer = static_cast< iServer* >( data1 ) ;
		saslRequests.erase(
			std::remove_if(
				saslRequests.begin(),
				saslRequests.end(),
				[&]( const SaslRequest& req ) { return req.theServer == theServer ; }
			),
			saslRequests.end()
		) ;
		break ;
		}
	case EVT_XQUERY:
		{
		iServer* theServer = static_cast< iServer* >( data1 );
		const char* Routing = reinterpret_cast< char* >( data2 );
		const char* Message = reinterpret_cast< char* >( data3 );
		// elog << "CSERVICE.CC XQUERY: " << theServer->getName() << " " << Routing << " " << Message << endl;
		//As it is possible to run multiple GNUWorld clients on one server, first parameter should be a nickname.
		//If it ain't us, ignore the message, the message is probably meant for another client here.
		StringTokenizer st( Message ) ;
		if( st.size() < 2 )
        		{
			//No command or no nick supplied
        		break;
       			}
		string Command = string_upper(st[0]);
		if ((Command == "LOGIN") || (Command == "LOGIN2"))
			{
			doXQLogin(theServer, Routing, Message);
			}

		if ((Command == "ISUSER") || (Command == "ISCHAN"))
			{
			doXQIsCheck(theServer, Routing, Command, Message);
			}
		if (Command == "SASL")
			{
			doXQSASL(theServer, Routing, Message);
			}
		break;
		}
	case EVT_XREPLY:
		{
		iServer* theServer = static_cast< iServer* >(data1);
		const char* Routing = reinterpret_cast< char* >(data2);
		const char* Message = reinterpret_cast< char* >(data3);
		//elog << "CSERVICE.CC XREPLY: " << theServer->getName() << " " << Routing << " " << Message << endl;
		//As it is possible to run multiple GNUWorld clients on one server, first parameter should be a nickname.
		//If it ain't us, ignore the message, the message is probably meant for another client here.
		StringTokenizer st(Message);
		if (st.size() < 2)
		{
			// No command or data supplied
			break;
		}
		string Command = string_upper(st[0]);
		if (Command == "OPLIST")
		{
			// Process the channel OPLIST data from mod.openchanfix
			doXROplist(theServer, Routing, Message);
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
			LOG_MSG( TRACE, "Deauthenticated client {client_nick} from user: {user_name}" )
				.with( "client", tmpUser )
				.with( "user", tmpSqlUser )
				.with( "quit_event", (theEvent == EVT_QUIT) ? "QUIT" : "KILL" )
				.logStructured() ;
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
				/* This function check whether the account flags are correct, if not send update. */
				sendAccountFlags(theUser, tmpUser);
			}

			iServer* tmpServer = Network->findServer(tmpUser->getIntYY());
			if ((this->getUplink()->isBursting() || tmpServer->isBursting()) && theUser)
			{
				newData->currentUser = theUser;
				theUser->addAuthedClient(tmpUser);
			}
			else
				doCommonAuth(tmpUser);
		}
		break;
		} // case EVT_NICK
	case EVT_GLINE:
		{
                if(!data1) //TODO: find out how we get this (Do we even ever get this?)
                        {
                        return ;
                        }
                Gline* newG = static_cast< Gline* >(data1);

                csGline* newGline = findGline(newG->getUserHost());
                if(!newGline)
                        {
                        newGline = new (std::nothrow) csGline(this);
                        assert (newGline != NULL);
                        iServer* serverAdded = Network->findServer(newG->getSetBy());
                        if(serverAdded)
                                newGline->setAddedBy(serverAdded->getName());
                        else
                                newGline->setAddedBy("Unknown");
                        }
                else
                        {
                        if(newGline->getLastUpdated() >= newG->getLastmod())
                                {
                                return ;
                                }

                        }
                newGline->setAddedOn(::time(0));
                newGline->setHost(newG->getUserHost());
                newGline->setReason(newG->getReason());
                newGline->setExpires(newG->getExpiration());
                        
		newGline->Insert();
                newGline->loadData(newGline->getHost());
                
		addGline(newGline);

                break;               
		} // case EVT_GLINE
	case EVT_REMGLINE:
		{
                if(!data1)
                        {
                        return ;
                        }

                Gline* newG = static_cast< Gline* >(data1);
                csGline* newGline = findGline(newG->getUserHost());
                if(newGline)
                        {
                        remGline(newGline);
                        newGline->Delete();
                        delete newGline;
                        }
                break;
		} // case EVT_REMGLINE
	} // switch()
xClient::OnEvent( theEvent,
	data1, data2, data3, data4 ) ;
}

/**
 * Support function to deVoice all voiced users on a channel.
 */
void cservice::deVoiceAllOnChan(Channel* theChan)
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

vector< iClient* > deVoiceList;

for( Channel::const_userIterator ptr = theChan->userList_begin();
	ptr != theChan->userList_end() ; ++ptr )
	{
		if( ptr->second->getMode(ChannelUser::MODE_V))
		{
			deVoiceList.push_back( ptr->second->getClient());
		}
	}

if( !deVoiceList.empty() )
	{
	DeVoice(theChan, deVoiceList);
	}
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

void cservice::deopSuspendedOnChan(Channel* theChan, sqlUser* theUser)
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
	if (!theUser->networkClientList.empty())
		for( sqlUser::networkClientListType::iterator cliPtr = theUser->networkClientList.begin() ;
			cliPtr != theUser->networkClientList.end() ; ++cliPtr )
		{
			iClient* tmpClient = (*cliPtr);
			ChannelUser* tmpUser = theChan->findUser(tmpClient);
			if ((tmpUser) && (tmpUser->getMode(ChannelUser::MODE_O) && (!tmpClient->getMode(iClient::MODE_SERVICES))))
				deopList.push_back(tmpClient);
		}

	if( !deopList.empty() )
		DeOp(theChan, deopList);
	return;
}

void cservice::doAllBansOnChan(Channel* tmpChan)
{
	if (!tmpChan)
	{
		/* Don't try this on a null channel. */
		return;
	}

	sqlChannel* reggedChan = getChannelRecord(tmpChan->getName());

	if( !reggedChan || !reggedChan->getInChan() )
	{
		return;
	}

	ChannelUser* tmpBotUser = tmpChan->findUser(getInstance());
	if (!tmpBotUser || !tmpBotUser->getMode(ChannelUser::MODE_O))
	{
		return;
	}

	vector<ChannelUser*> tmpList;
	for (Channel::userIterator chanUsers = tmpChan->userList_begin();
		chanUsers != tmpChan->userList_end(); ++chanUsers)
	{
		ChannelUser* tmpUser = chanUsers->second;
		tmpList.push_back(tmpUser);
	}
	for (vector<ChannelUser*>::iterator tItr = tmpList.begin();
		tItr != tmpList.end(); tItr++)
	{
		ChannelUser* tmpUser = *tItr;
		/* check if this user is banned */
		checkBansOnJoin(tmpChan, reggedChan, tmpUser->getClient());
	}
	tmpList.clear();
	return;
}

void cservice::doTheRightThing(Channel* tmpChan)
{
	if (!tmpChan)
	{
		/* Don't try this on a null channel. */
		return;
	}

	//elog << "cservice::doTheRightThing> channel: "	<< tmpChan->getName() << endl;

	sqlChannel* reggedChan = getChannelRecord(tmpChan->getName());

	if( !reggedChan || !reggedChan->getInChan() )
	{
		return;
	}

	ChannelUser* tmpBotUser = tmpChan->findUser(getInstance());
	if (!tmpBotUser)
	{
		return;
	}

	// If the bot has no op, op it
	if (tmpChan && !tmpBotUser->getMode(ChannelUser::MODE_O))
	{
		// Make sure +R is also set for the channel
		MyUplink->Mode(NULL, tmpChan, "+R", std::string());

		if (reggedChan->getInChan())
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

			tmpBotUser->setMode(ChannelUser::MODE_O);

			if (reggedChan->getChannelMode() != "")
			{
				MyUplink->Mode(this, tmpChan, reggedChan->getChannelMode().c_str(), std::string() );
			}

			LOG_MSG( INFO, "Performed reop for channel {chan_name}" )
			.with( "chan", tmpChan )
			.logStructured();
		}
	}

	doAutoTopic(reggedChan);

	if (getConfigVar("BAN_CHECK_ON_BURST")->asInt() == 1)
	{
		/* check current inhabitants of the channel against our banlist */
		doAllBansOnChan(tmpChan);
	}

	if (reggedChan->getFlag(sqlChannel::F_NOOP) || reggedChan->getFlag(sqlChannel::F_SUSPEND))
	{
		deopAllOnChan(tmpChan);
	}
	if (reggedChan->getFlag(sqlChannel::F_STRICTOP))
	{
		deopAllUnAuthedOnChan(tmpChan);
	}
	if (reggedChan->getFlag(sqlChannel::F_NOVOICE))
	{
		deVoiceAllOnChan(tmpChan);
	}
	return;
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
bool burstJoin = false ;

switch( whichEvent )
	{
	case EVT_BURST:
		burstJoin = true ;
		// fall through
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

		if (ptr != pendingChannelList.end() && (!isDBRegisteredChannel(theChan->getName())))
			{
			/*
			 * Firstly, is this join a result of a server bursting onto the network?
			 * If this is the case, its not a manual /join.
			 */

			if (!burstJoin)
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

				string NumericIP = fixToCIDR64(theClient->getNumericIP());

				sqlPendingChannel::trafficListType::iterator Tptr =
					ptr->second->trafficList.find(NumericIP);

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

						trafRecord = new (std::nothrow) sqlPendingTraffic(this);
						trafRecord->ip_number = NumericIP;
						trafRecord->join_count = 1;
						trafRecord->channel_id = ptr->second->channel_id;
						trafRecord->insertRecord();

						ptr->second->trafficList.insert(sqlPendingChannel::trafficListType::value_type(
								NumericIP, trafRecord));
						LOG_MSG( INFO, "Created a new IP traffic record for IP#{ip} ({client_userhost}) on {chan_name}" )
							.with( "ip", NumericIP )
							.with( "client", theClient )
							.with( "chan", theChan )
							.logStructured() ;
						} else
						{
						/* Already cached, update and save. */
						trafRecord = Tptr->second;
						trafRecord->join_count++;
						trafRecord->commit();
						}

						ptr->second->unique_join_count = ptr->second->trafficList.size();
						ptr->second->commit();

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
							LOG( INFO, "New total for Supporter #{} ({}) on {} is {}.", theUser->getID(),
								theUser->getUserName(), theChan->getName(), Supptr->second ) ;
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

#ifdef USE_WELCOME
		if (strlen(reggedChan->getWelcome().c_str()) > 0)
		{
			Notice(theClient, "(%s) %s",
				theChan->getName().c_str(),
				reggedChan->getWelcome().c_str());
		}
#endif

		/* Is it time to set an autotopic? */
		if (reggedChan->getFlag(sqlChannel::F_AUTOTOPIC) &&
			(reggedChan->getLastTopic()
			+ topic_duration <= currentTime()))
			{
			doAutoTopic(reggedChan);
			}

		sqlUser *theUser = isAuthed(theClient, false);
		/**
		 * Check if JOINLIM is enabled, and this is a unidented host,
		 * user is not logged in, and the nick is not from a bursting server.
		 */
		if (!theUser &&
			theClient->getUserName()[0] == '~' &&
			reggedChan->getFlag(sqlChannel::F_JOINLIM) &&
			!burstJoin)
			doJoinLimit(reggedChan, theChan);

		/* Deal with auto-op first - check this users access level. */
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
			if (!reggedChan->getFlag(sqlChannel::F_NOVOICE))
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
	case EVT_PART:
	{
		theClient = static_cast< iClient* >( data1 ) ;
		string partMsg;
		if (data2 != NULL)
			partMsg = string(*(static_cast<string*>(data2)));
		//elog << "cservice::EVT_PART> " << theClient << " Part " << theChan->getName() << " (" << partMsg << ")" << endl;
		handleChannelPart(theClient, theChan, partMsg);
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
		LOG_MSG( ERROR, "Null ban record in {chan_name}'s ban list." )
			.with( "chan", theChan )
			.logStructured() ;
		continue ;
		}
	if (banMatch(theBan->getBanMask(), theClient))
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

stringstream s;
s	<< getCharYY()
	<< " 311 "
	<< sourceClient->getCharYYXXX()
	<< " " << targetClient->getNickName()
	<< " " << targetClient->getUserName()
	<< " " << targetClient->getInsecureHost()
	<< " * :" << targetClient->getDescription()
	<< ends;
Write( s );

if (targetClient->isOper())
	{
	s.str("");
	s	<< getCharYY()
		<< " 313 "
		<< sourceClient->getCharYYXXX()
		<< " " << targetClient->getNickName()
		<< " :is an IRC Operator"
		<< ends;
	Write( s );
	}

sqlUser* theUser = isAuthed(targetClient, false);

if (theUser)
	{
	s.str("");
	s	<< getCharYY()
		<< " 330 "
		<< sourceClient->getCharYYXXX()
		<< " " << targetClient->getNickName()
		<< " " << theUser->getUserName()
		<< " :is logged in as"
		<< ends;
	Write( s );
	}

if (isIgnored(targetClient))
	{
	s.str("");
	s	<< getCharYY()
		<< " 316 "
		<< sourceClient->getCharYYXXX()
		<< " " << targetClient->getNickName()
		<< " :is currently being ignored."
		<< ends;
	Write( s );
	}

s.str("");
s	<< getCharYY()
	<< " 318 "
	<< sourceClient->getCharYYXXX()
	<< " " << targetClient->getNickName()
	<< " :End of /WHOIS list."
	<< ends;
Write( s );
}

bool cservice::Kick(Channel* theChan, iClient* theClient, const string& reason, bool modeAsServer)
{
	return xClient::Kick(theChan, theClient, reason, modeAsServer);
}

bool cservice::Kick(Channel* theChan, const std::vector<iClient*>& theClients, const string& reason, bool modeAsServer)
{
	return xClient::Kick(theChan, theClients, reason, modeAsServer);
}

/**
 * Kick all users from a channel that matches the specified mask, join/part if necessary.
 */
bool cservice::Kick( Channel* theChan, const string& mask, const std::string& reason, bool modeAsServer)
{
	assert( theChan != NULL ) ;

	if( !isConnected() )
	{
		return false ;
	}

	if ( mask.empty() )
	{
		return true ;
	}

	bool OnChannel = xClient::isOnChannel( theChan ) ;
	if( !OnChannel )
	{
		// Join, giving ourselves ops
		Join( theChan, string(), 0, true ) ;
	}
	else
	{
		// Bot is already on the channel
		ChannelUser* meUser = theChan->findUser( me ) ;
		assert( meUser != 0 ) ;

		// Make sure we have ops
		if( !meUser->getMode( ChannelUser::MODE_O ) )
		{
			// The bot does NOT have ops
			return false ;
		}

	// The bot has ops
	}
	std::vector <iClient*> toBoot;
	for(Channel::userIterator chanUsers = theChan->userList_begin(); chanUsers != theChan->userList_end(); ++chanUsers)
	{
		ChannelUser* tmpUser = chanUsers->second;
		// To protect a +x-ed user, if the mask is not a hiddenhost-suffix, then skip (no match)
		if ((tmpUser->getClient()->isModeX() && tmpUser->getClient()->isModeR()) && (mask.find(tmpUser->getClient()->getHiddenHostSuffix()) == string::npos))
			continue;
		if (!match(mask, tmpUser->getClient()))
		{
			/* Don't kick +k things */
			if (!tmpUser->getClient()->getMode(iClient::MODE_SERVICES))
			{
				toBoot.push_back(tmpUser->getClient());
			}
		}
	}
	return Kick(theChan, toBoot, reason, modeAsServer);
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

		/* The channel is purged */
		if (theChan->getRegisteredTS() == 0)
		{
			++ptr;
			continue;
		}

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

void cservice::undoJoinLimits(sqlChannel *reggedChan)
{
	reggedChan->setLimitJoinTime(0);
	reggedChan->setLimitJoinCount(0);

	Channel *theChan = Network->findChannel(reggedChan->getName());
	std::stringstream ss(reggedChan->getLimitJoinModeSet());
	std::string parm;
	std::vector<std::string> parms;

	while (ss >> parm)
	{ // Get array of parameters, first one is the modes
		parms.push_back(parm);
	}

	unsigned int parmcount = 1;
	for (char c : parms[0])
	{
		if (c == 'k')
		{ // Key
			theChan->removeMode(Channel::MODE_K);
			theChan->setKey("");
			parmcount++;
		}
		else if (c == 'b')
		{ // Ban
			theChan->removeBan(parms[parmcount]);
			parmcount++;
		}
		else
		{ // Normal mode
			if (c == 'D')
				theChan->removeMode(Channel::MODE_D);
			if (c == 'c')
				theChan->removeMode(Channel::MODE_C);
			if (c == 'C')
				theChan->removeMode(Channel::MODE_CTCP);
			if (c == 'i')
				theChan->removeMode(Channel::MODE_I);
			if (c == 'm')
				theChan->removeMode(Channel::MODE_M);
			// if (c == 'M') theChan->removeMode(Channel::MODE_MNOREG);
			// if (c == 'u') theChan->removeMode(Channel::MODE_PART);
			if (c == 'r')
				theChan->removeMode(Channel::MODE_R);
			if (c == 's')
				theChan->removeMode(Channel::MODE_S);
		}
	}

	// Set the mode in channel
	stringstream s;
	s << getCharYYXXX()
		<< " M "
		<< reggedChan->getName()
		<< " -"
		<< reggedChan->getLimitJoinModeSet()
		<< " "
		<< theChan->getCreationTime()
		<< ends;
	Write(s);
	incStat("CORE.JOINLIM.ALTER");

	reggedChan->setLimitJoinActive(false);
}

void cservice::stopTimer(xServer::timerID timerID)
{
	MyUplink->UnRegisterTimer(timerID, 0);
}

void cservice::doJoinLimit(sqlChannel *reggedChan, Channel *theChan)
{
	if (reggedChan->getLimitJoinActive())
	{
		// When a new client joins, reset the JOINPERIOD to keep it alive
		timerID tmpTimer = MyUplink->UnRegisterTimer(reggedChan->getLimitJoinTimer(), NULL);

		// Start new timer with remaining time + joinsecs
		time_t theTime = ::time( nullptr) + reggedChan->getLimitJoinPeriod();
		tmpTimer = MyUplink->RegisterTimer(theTime, this, reggedChan);

		reggedChan->setLimitJoinTimer(tmpTimer);
		reggedChan->setLimitJoinTimeExpire(theTime);

		return; // We are already active, just reset the timer
	}

	if (reggedChan->getLimitJoinTime() == 0 || (reggedChan->getLimitJoinTime() + reggedChan->getLimitJoinSecs()) < currentTime())
	{ // We have passed the session, start a new one
		reggedChan->setLimitJoinTime(currentTime());
		reggedChan->setLimitJoinCount(1);
		return;
	}

	reggedChan->addLimitJoinCount(); // Increment number of unidented joins in last period
	if (reggedChan->getLimitJoinCount() >= (reggedChan->getLimitJoinMax()))
	{ // Check if we're above threshold
		// Check if we're opped
		ChannelUser *tmpBotUser = theChan->findUser(getInstance());
		if (!tmpBotUser)
			return;
		if (!tmpBotUser->getMode(ChannelUser::MODE_O))
			return;

		// Filter out already set channel modes from the mode to set, to avoid removing them when the mode is lifted
		std::string chanModes = theChan->getModeString();
		std::string wantModes = reggedChan->getLimitJoinMode();
		std::string resultModes = "";

		bool doneparms = false;
		for (char c : wantModes)
		{
			if (c == ' ')
				doneparms = true; // Done parsing modes, now we got to parameters
			if (doneparms)
			{
				resultModes += c;
			}
			else
			{
				if (chanModes.find(c) == std::string::npos || c == 'b')
				{ // If mode is not already set, and ignore bans
					resultModes += c;
				}
			}
		}

		std::stringstream ss(wantModes);
		std::string parm;
		std::vector<std::string> parms;

		while (ss >> parm)
		{ // Get array of parameters, first one is the modes
			parms.push_back(parm);
		}

		unsigned int parmcount = 1;
		for (char c : parms[0])
		{
			if (c == 'k')
			{ // Key
				theChan->setMode(Channel::MODE_K);
				theChan->setKey(parms[parmcount]);
				parmcount++;
			}
			else if (c == 'b')
			{ // Ban
				std::string banMask = parms[parmcount];
				theChan->setBan(banMask);
				parmcount++;
			}
			else
			{ // Normal mode
				if (c == 'D')
					theChan->setMode(Channel::MODE_D);
				if (c == 'c')
					theChan->setMode(Channel::MODE_C);
				if (c == 'C')
					theChan->setMode(Channel::MODE_CTCP);
				if (c == 'i')
					theChan->setMode(Channel::MODE_I);
				if (c == 'm')
					theChan->setMode(Channel::MODE_M);
				// if (c == 'M') theChan->setMode(Channel::MODE_MNOREG);
				// if (c == 'u') theChan->setMode(Channel::MODE_PART);
				if (c == 'r')
					theChan->setMode(Channel::MODE_R);
				if (c == 's')
					theChan->setMode(Channel::MODE_S);
			}
		}

		// If no modes are set, because they are already set in the channel, abort.
		if (resultModes.empty())
			return;

		// Send action opnotice to channel 
		NoticeChannelOps(theChan->getName(),
			"Activated JOINLIM protection and set chanmode +%s for %i seconds",
			resultModes.c_str(), reggedChan->getLimitJoinPeriod());

		// Save mode for unsetting later
		reggedChan->setLimitJoinModeSet(resultModes);
		reggedChan->setLimitJoinActive(true);

		// Set the mode in channel
		stringstream s;
		s << getCharYYXXX()
			<< " M "
			<< theChan->getName()
			<< " +"
			<< resultModes
			<< " "
			<< theChan->getCreationTime()
			<< ends;
		Write(s);

		// Register a timer to lift this
		time_t theTime = time(NULL) + reggedChan->getLimitJoinPeriod();
		if (reggedChan->getLimitJoinTimer() > 0)
			stopTimer(reggedChan->getLimitJoinTimer()); // Unregister old one

		reggedChan->setLimitJoinTimer(MyUplink->RegisterTimer(theTime, this, reggedChan));
		reggedChan->setLimitJoinTimeExpire(theTime);

		incStat("CORE.JOINLIM.ALTER");
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
	if (!theChan->getFlag(sqlChannel::F_AUTOTOPIC))
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

// This function is used to ban a mask, to who no matching client is currently existing on the channel
bool cservice::doSingleBan(sqlChannel* theChan,
    const string& banMask, unsigned short banLevel, unsigned int banExpire, const string& theReason)
{
	/*
	 *  Check to see if this banmask already exists in the
	 *  channel. (Ugh, and overlapping too.. hmm).
	 */

	Channel* netChan = Network->findChannel(theChan->getName());

	if (netChan)
	{
		stringstream s;
		s	<< getCharYYXXX()
			<< " M "
			<< netChan->getName()
			<< " +b "
			<< banMask
			<< ends;

		Write( s );

		/* remove the ban (even if it doesnt exist, it will return false anyway) */
		netChan->removeBan(banMask) ;
		/* set the ban */
		netChan->setBan(banMask) ;
	}

	/*
	 *  Check for duplicates, if none found -
	 *  add to internal list and commit to the db.
	 */
	map< int,sqlBan* >::const_iterator ptr = theChan->banList.begin();
	while (ptr != theChan->banList.end())
	{
		const sqlBan* theBan = ptr->second;

		if(string_lower(banMask) == string_lower(theBan->getBanMask()))
		{
			/*
			 * If this mask is already banned, we're just getting
			 * lagged info.
			 */
			return true;
		}
	    ++ptr;
	}

	/* Create a new Ban record */
	sqlBan* newBan = new (std::nothrow) sqlBan(this);
	assert( newBan != 0 ) ;

	// TODO: Build a suitable constructor in sqlBan
	newBan->setChannelID(theChan->getID());
	newBan->setBanMask(banMask);
	newBan->setSetBy(getNickName());
	newBan->setSetTS(currentTime());
	newBan->setLevel(banLevel);

	newBan->setExpires(banExpire + currentTime());
	newBan->setReason(theReason);

	/* Insert this new record into the database. */
	newBan->insertRecord();

	/* Insert to our internal List. */
	//theChan->banList[newBan->getID()] = newBan;
	theChan->banList.insert(std::map<int,sqlBan*>::value_type(newBan->getID(),newBan));

	return true ;
}

bool cservice::doSingleBanAndKick(sqlChannel* theChan,
		iClient* theClient, unsigned short banLevel, unsigned int banExpire, const string& theReason)
{
	// First of all, don't kickban any priviledged clients like services
	if (theClient->getMode(iClient::MODE_SERVICES))
		return true;
	/*
	 *  Check to see if this banmask already exists in the
	 *  channel. (Ugh, and overlapping too.. hmm).
	 */
	string banTarget = Channel::createBan(theClient);

	Channel* netChan = Network->findChannel(theChan->getName());

	if (netChan)
	{
		stringstream s;
		s	<< getCharYYXXX()
			<< " M "
			<< netChan->getName()
			<< " +b "
			<< banTarget
			<< ends;

		Write( s );

		/* remove the ban (even if it doesnt exist, it will return false anyway) */
		netChan->removeBan(banTarget) ;
		/* set the ban */
		netChan->setBan(banTarget) ;
	}

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
			if (netChan != NULL)
				Kick( netChan, theClient, theReason ) ;
			return true;
		}
	    ++ptr;
	}

	/* Create a new Ban record */
	sqlBan* newBan = new (std::nothrow) sqlBan(this, theChan->getID(), banTarget,
												getNickName(), currentTime(), banLevel,
												banExpire + currentTime(), theReason);
	assert( newBan != 0 ) ;

	/* Insert this new record into the database. */
	newBan->insertRecord();

	/* Insert to our internal List. */
	//theChan->banList[newBan->getID()] = newBan;
	theChan->banList.insert(std::map<int,sqlBan*>::value_type(newBan->getID(),newBan));

	if (netChan != NULL)
		Kick( netChan, theClient, theReason ) ;
	return true ;
}

/**
 * Bans a user via IRC and the database with 'theReason',
 * and then kicks. theChan cannot be null.
 */
bool cservice::doInternalBanAndKick(sqlChannel* theChan,
	iClient* theClient, const string& theReason)
{
	unsigned short banLevel = 25;
	unsigned int banExpire = 300;

	/*
	 * If this guy is auth'd.. suspend his account.
	 */
	sqlUser* theUser = isAuthed(theClient, false);
	if (theUser)
	{
		sqlLevel* accessRec = getLevelRecord(theUser, theChan);
		if (accessRec && (accessRec->getSuspendExpire() < (currentTime() + 300)))
		{
			int susLev = accessRec->getAccess() + 1;
			if (susLev <= 500)
			{
				banLevel = susLev;
				if (accessRec->getSuspendLevel() < susLev)
					accessRec->setSuspendLevel(susLev);
				accessRec->setSuspendExpire(currentTime() + 300);
				accessRec->setSuspendBy(nickName);
				accessRec->setSuspendReason(theReason);
				accessRec->commit();
			}
		}
	}
	if (banLevel > 500)
		return true;

	Channel* netChan = Network->findChannel(theChan->getName());
	/* Even if the channel is currently empty, presumably the sqlChannel exists in the db */
	if (!netChan)
	{
		doSingleBanAndKick(theChan, theClient, banLevel, banExpire, theReason);
		return true;
	}
	vector< iClient* > clientsToKick ;
	for (Channel::userIterator chanUsers = netChan->userList_begin(); chanUsers != netChan->userList_end(); ++chanUsers)
	{
		ChannelUser* tmpUser = chanUsers->second;
		if (Channel::createBan(tmpUser->getClient()) == Channel::createBan(theClient))
		{
			clientsToKick.push_back(tmpUser->getClient());
		}
	}
	if (!clientsToKick.empty())
	{
		for (vector<iClient*>::iterator itr = clientsToKick.begin(); itr != clientsToKick.end(); ++itr)
			doSingleBanAndKick(theChan, *itr, banLevel, banExpire, theReason);
	}
	return true ;
}

bool cservice::doInternalBanAndKick(sqlChannel* theChan,
    iClient* theClient, unsigned short banLevel, unsigned int banExpire, const string& theReason)
{
	Channel* netChan = Network->findChannel(theChan->getName());
	/* Even if the channel is currently empty, presumably the sqlChannel exists in the db */
	if (!netChan)
	{
		doSingleBanAndKick(theChan, theClient, banLevel, banExpire, theReason);
		return true;
	}
	vector< iClient* > clientsToKick ;
	// Add at least the Sender client to the list
	clientsToKick.push_back(theClient);
	for (Channel::userIterator chanUsers = netChan->userList_begin(); chanUsers != netChan->userList_end(); ++chanUsers)
	{
		ChannelUser* tmpUser = chanUsers->second;
		if (Channel::createBan(tmpUser->getClient()) == Channel::createBan(theClient))
		{	//Since we above initially added at least the Sender client to the list, we shouldn't add again
			if (tmpUser->getClient() != theClient)
				clientsToKick.push_back(tmpUser->getClient());
		}
	}
	if (!clientsToKick.empty())
	{
		for (vector<iClient*>::iterator itr = clientsToKick.begin(); itr != clientsToKick.end(); ++itr)
			doSingleBanAndKick(theChan, *itr, banLevel, banExpire, theReason);
	}
	return true ;
}

bool cservice::doInternalBanAndKick(sqlChannel* theChan,
    const string& banMask, unsigned short banLevel, unsigned int banExpire, const string& theReason)
{
	Channel* netChan = Network->findChannel(theChan->getName());
	/* Even if the channel is currently empty, presumably the sqlChannel exists in the db */
	if (!netChan)
	{
		/*
		 *  Check for duplicates, if none found -
		 *  add to internal list and commit to the db.
		 */
		map< int,sqlBan* >::const_iterator ptr = theChan->banList.begin();
		while (ptr != theChan->banList.end())
		{
			const sqlBan* theBan = ptr->second;

			if(string_lower(banMask) == string_lower(theBan->getBanMask()))
			{
				/*
				 * If this mask is already banned, we're just getting
				 * lagged info.
				 */
				return true;
			}
		    ++ptr;
		}

		/* Create a new Ban record */
		sqlBan* newBan = new (std::nothrow) sqlBan(this, theChan->getID(), banMask,
													getNickName(), currentTime(), banLevel,
													banExpire + currentTime(), theReason);
		assert( newBan != 0 ) ;

		/* Insert this new record into the database. */
		newBan->insertRecord();

		/* Insert to our internal List. */
		//theChan->banList[newBan->getID()] = newBan;
		theChan->banList.insert(std::map<int,sqlBan*>::value_type(newBan->getID(),newBan));
		return true;
	}

	vector< iClient* > clientsToKick ;
	for (Channel::userIterator chanUsers = netChan->userList_begin(); chanUsers != netChan->userList_end(); ++chanUsers)
	{
		ChannelUser* tmpUser = chanUsers->second;
		if ((tmpUser->getClient()->isModeX() && tmpUser->getClient()->isModeR()) && (banMask.find(tmpUser->getClient()->getHiddenHostSuffix()) == string::npos))
			continue;
		if (banMatch(banMask, tmpUser->getClient()))
		{
			clientsToKick.push_back(tmpUser->getClient());
		}
	}
	if (!clientsToKick.empty())
	{
		for (vector<iClient*>::iterator itr = clientsToKick.begin(); itr != clientsToKick.end(); ++itr)
			doSingleBanAndKick(theChan, *itr, banLevel, banExpire, theReason);
	}
	else	// make sure the ban is set even if no matching client was found on the channel
		doSingleBan(theChan, banMask, banLevel, banExpire, theReason);
	return true ;
}

bool cservice::doInternalSuspend(sqlChannel* theChan,
    iClient* theClient, unsigned short suspLevel, unsigned int suspExpire, const string& theReason)
{
	sqlUser* theUser = isAuthed(theClient, false);
	if (theUser)
	{
		sqlLevel* accessRec = getLevelRecord(theUser, theChan);
		if (accessRec && (accessRec->getSuspendExpire() < (currentTime() + (time_t)suspExpire)))
		{
			if (accessRec->getSuspendLevel() < suspLevel)
				accessRec->setSuspendLevel(suspLevel);
			accessRec->setSuspendExpire(currentTime() + (time_t)suspExpire);
			accessRec->setSuspendBy(getNickName());
			accessRec->setLastModif(currentTime());
			accessRec->setLastModifBy(getInstance()->getNickUserHost());
			accessRec->setSuspendReason(theReason);
			accessRec->commit();
		}
	} else return false;
	return true;
}

bool cservice::doInternalGline(iClient* theClient, const time_t& thePeriod, const string& theReason)
{
	string UserHost = "~*@" + theClient->getRealInsecureHost();
	if (theClient->getUserName()[0] != '~')
		UserHost = theClient->getUserName() + "@" + theClient->getRealInsecureHost();
	csGline *theGline = findGline(UserHost);
	bool Up = false;
	if(theGline)
		Up =  true;
	else { theGline = new (std::nothrow) csGline(this);
		assert(theGline != NULL); }
	theGline->setHost(UserHost);
	theGline->setExpires(unsigned(::time(0) + thePeriod));
	theGline->setAddedBy(getUplinkName());
	theGline->setReason(theReason);
	theGline->setAddedOn(::time(0));
	theGline->setLastUpdated(::time(0));
	addGlineToUplink(theGline);
	if (Up)
		theGline->Update();
	else
	{
		theGline->Insert();
		//We need to update the Id
		theGline->loadData(theGline->getHost());
		addGline(theGline);
	}
	return true ;
}

bool cservice::doInternalGline(const string& IP, const time_t& thePeriod, const string& theReason)
{
	string UserHost = "~*@" + IP;
	if (IP.find('@') != string::npos)
		UserHost = IP;
	csGline *theGline = findGline(UserHost);
	bool Up = false;
	if(theGline)
		Up =  true;
	else { theGline = new (std::nothrow) csGline(this);
		assert(theGline != NULL); }
	theGline->setHost(UserHost);
	theGline->setExpires(unsigned(::time(0) + thePeriod));
	theGline->setAddedBy(getUplinkName());
	theGline->setReason(theReason);
	theGline->setAddedOn(::time(0));
	theGline->setLastUpdated(::time(0));
	addGlineToUplink(theGline);
	if (Up)
		theGline->Update();
	else
	{
		theGline->Insert();
		//We need to update the Id
		theGline->loadData(theGline->getHost());
		addGline(theGline);
	}
	return true ;
}

bool cservice::KickAllWithFloodMessage(Channel* theChan, const string& Message, const string& kickMsg, bool clearcount)
{
	sqlChannel* sqlChan = getChannelRecord(theChan->getName());
	assert(sqlChan != 0);
	std::list < string > IPlist = sqlChan->getRepeatMessageCount(Message).second;
	std::list < string >::iterator itr = IPlist.begin();
	unsigned int index = 0;
	for ( ; itr != IPlist.end(); itr++)
	{
		Kick(theChan, *itr, kickMsg);
		if (clearcount)
		{
			if (((unsigned int)IPlist.size() - index) >= (unsigned int)sqlChan->getRepeatCount())
				sqlChan->RemoveFlooderMask(*itr);
		}
		index++;
	}
	return true;
}

bool cservice::KickBanAllWithFloodMessage(Channel* theChan, const string& Message, unsigned short banLevel, unsigned int banExpire, const string& theReason)
{
	sqlChannel* sqlChan = getChannelRecord(theChan->getName());
	assert(sqlChan != 0);
	std::list < string > MaskList = sqlChan->getRepeatMessageCount(Message).second;
	std::list < string >::iterator itr = MaskList.begin();
	unsigned int index = 0;
	for ( ; itr != MaskList.end(); itr++)
	{
		doInternalBanAndKick(sqlChan, *itr, banLevel, banExpire, theReason);
		/*  ** Race condition fix ** */
		/* Remove the Flooder IP only if the accumulated listsize is greather than the RepeatCount
		 * otherwise any remaining items won't be banned on ChannelMessage because they don't reach the RepeatCount to trigger this function
		 */
		if (((unsigned int)MaskList.size() - index) >= (unsigned int)sqlChan->getRepeatCount())
			sqlChan->RemoveFlooderMask(*itr);
		index++;
	}
	return true;
}

bool cservice::GlineAllWithFloodMessage(sqlChannel* sqlChan, const string& Message, const time_t& thePeriod, const string& theReason)
{
	std::list < string > IPlist = sqlChan->getRepeatMessageCount(Message).second;
	std::list < string >::iterator itr = IPlist.begin();
	unsigned int index = 0;
	for ( ; itr != IPlist.end(); itr++)
	{
		doInternalGline(*itr, thePeriod, theReason);
		if (((unsigned int)IPlist.size() - index) >= (unsigned int)sqlChan->getRepeatCount())
			sqlChan->RemoveFlooderMask(*itr);
		index++;
	}
	return true;
}

unsigned int cservice::checkFloodproLevel(sqlChannel* sqlChan, const string& Message)
{
	sqlChannel::repeatMaskMapType rep = sqlChan->getRepeatMessageCount(Message);
	unsigned int repeatCount = rep.first;
	unsigned int IPcount = (unsigned int)rep.second.size();
	if (sqlChan->getRepeatCount() == 0) return repeatCount;
	if ((repeatCount >= sqlChan->getRepeatCount()) && IPcount > 1)
	{
		sqlChannel::FloodProLevel currFN = sqlChan->getFloodproLevel();
		if (currFN > sqlChannel::FLOODPRO_NONE)
		{
			sqlChan->incFloodPro();
			if (sqlChan->getFloodproLevel() > currFN)
			{
				string FNmessage = TokenStringsParams("Increased FLOODPRO punishment level to \002%s\002 on channel \002%s\002",
						sqlChan->getFloodLevelName(sqlChan->getFloodproLevel()).c_str(), sqlChan->getName().c_str());
				NoticeChannelOps(sqlChan->getName(), FNmessage.c_str());
				logPrivAdminMessage(FNmessage.c_str());
			}
		}
	}
	return repeatCount;
}

void cservice::checkChannelsFlood()
{
	sqlChannelHashType::iterator ptr = sqlChannelCache.begin();
	while (ptr != sqlChannelCache.end())
	{
		sqlChannel* theChan = (ptr)->second;

		/* The channel is purged */
		if (theChan->getRegisteredTS() == 0)
		{
			++ptr;
			continue;
		}

		if ((theChan->getName() == "*") || (!theChan->getFlag(sqlChannel::F_FLOODPRO)))
		{
			++ptr;
			continue;
		}
		sqlChannel::chanFloodMapType::iterator itr = theChan->chanFloodMap.begin();
		while (itr != theChan->chanFloodMap.end())
		{
			time_t iplasttime = theChan->getMaskLastTime(itr->first);
			if (!iplasttime)
			{
				++itr;
				continue;
			}
			//theChan->ExpireMessagesForMask(itr->first, currentTime());
			time_t lastTime = currentTime() - iplasttime;
			if ((lastTime) > (time_t)theChan->getFloodPeriod())
			{
				theChan->RemoveFlooderMask(itr++->first);
			}
			else
				++itr;
		}
		if (theChan->getFloodproLevel() > sqlChannel::FLOODPRO_NONE)
		{
			time_t lastFloodTime = currentTime() - theChan->getLastFloodTime();
			if ((theChan->getFloodproLevel() > theChan->getManualFloodproLevel())
					&& ((lastFloodTime) > ((time_t)floodproRelaxTime)))
			{
				theChan->decFloodPro();
				theChan->setLastFloodTime(currentTime());
				string FNmessage = TokenStringsParams("Relaxed FLOODPRO punishment level to \002%s\002 on channel \002%s\002",
						theChan->getFloodLevelName(theChan->getFloodproLevel()).c_str(), theChan->getName().c_str());
				NoticeChannelOps(theChan->getName(), FNmessage.c_str());
				logPrivAdminMessage(FNmessage.c_str());
			}
		}
		++ptr;
	}
	return;
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
	<< escapeSQLChars(theClient->getNickUserHost())
	<< " (" << userExtra << ") "
	<< escapeSQLChars(theMessage)
	<< "', "
	<< currentTime()
	<< ")"
	<< ends;


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
	unsigned short eventType, time_t eventTime)
{
	unsigned int ts;
	stringstream queryString;

	if (eventTime == 0)
		ts = currentTime();
	else
		ts = eventTime;

	queryString	<< "SELECT ts, message FROM channellog WHERE "
			<< "channelid = "
			<< theChannel->getID()
			<< " AND event = "
			<< eventType
			<< " AND ts <= "
			<< ts
			<< " ORDER BY ts DESC LIMIT 1"
			<< ends;

	if (SQLDb->Exec(queryString, true))
	{
		if (SQLDb->Tuples() < 1)
			return "";
		time_t purgeTime = atoi(SQLDb->GetValue(0, 0));
		string timeStr = (string)ctime(&purgeTime);
		timeStr.erase(timeStr.length() - 1);
		string reason = "[" + timeStr + "] " + SQLDb->GetValue(0, 1);
		return reason;
	}
	return "";
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

bool cservice::Topic(Channel* theChan, const string& Message)
{
	return xClient::Topic(theChan, Message);
}

void cservice::dbErrorMessage(iClient* theClient)
{
Notice(theClient,
	"An error occured while performing this action, "
	"the database may be unavailable. Please try again later.");
dbErrors++;
}

void cservice::setSupporterNoticedStatus(int suppId, int chanId, bool noticed)
{
	stringstream theQuery;
	string noticedStr = "Y";
	if (!noticed)
		noticedStr = "N";
	theQuery	<< "UPDATE supporters SET noticed = '"
				<< noticedStr
				<< "' WHERE channel_id = "
				<< chanId
				<< " AND user_id = "
				<< suppId
				<< ends;

		if (!SQLDb->Exec(theQuery, true))
		{
			LOGSQL_ERROR( SQLDb ) ;
		}
}

/* This is a bug-workaround function, for some reason on cservice::doCommonAuth
 * doesn't work with the int chanId
 */
void cservice::setSupporterNoticedStatus(int suppId, const string& chanName, bool noticed)
{
	stringstream theQuery;
	theQuery	<< "SELECT id FROM channels WHERE name = '"
				<< chanName
				<< "'"
				<< ends;

	if (!SQLDb->Exec(theQuery, true))
		{
		LOGSQL_ERROR( SQLDb ) ;
		}

	int chanId = atoi(SQLDb->GetValue(0,0).c_str());

	theQuery.str("");
	string noticedStr = "Y";
	if (!noticed)
		noticedStr = "N";
	theQuery	<< "UPDATE supporters SET noticed = '"
				<< noticedStr
				<< "' WHERE channel_id = "
				<< chanId
				<< " AND user_id = "
				<< suppId
				<< ends;

	if (!SQLDb->Exec(theQuery, true))
		{
		LOGSQL_ERROR( SQLDb ) ;
		}
}

void cservice::UpdatePendingOpLists()
{
	stringstream theQuery;
	theQuery	<<  "SELECT channels.name"
				<< " FROM pending,channels"
				<< " WHERE channels.id = pending.channel_id"
				<< " AND pending.status IN (0, 1, 2)"
				<< ends;

	if( SQLDb->Exec(theQuery, true ) )
	{
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
			string channelName = SQLDb->GetValue(i,0).c_str();
			doXQOplist(channelName);
		}
	}
	else
	{
		LOGSQL_ERROR( SQLDb ) ;
	}
	return;
}

/*
 * FirstNoticing is used to prevent noticing '2x times' the supporters:
 * - 1a. When the supporter logs in to X
 * - 2a. When within pending_duration (pending_timerID) is first time found a new incoming application
 * -  b. In the 'usual' pending_notif_duration when the uspporter is kept noticed.
 */
void cservice::checkIncomings(bool FirstNoticing)
{
	suppIdListType suppIdList;
	stringstream theQuery;
	theQuery	<< "SELECT channels.id, channels.name, supporters.user_id, users.user_name, supporters.noticed"
				<< " FROM pending,supporters,channels,users"
				<< " WHERE pending.channel_id = supporters.channel_id"
				<< " AND channels.id = pending.channel_id"
				<< " AND users.id = pending.manager_id"
				<< " AND pending.status = 0"
				//<< " AND supporters.noticed = 'N'"
				<< " AND supporters.support = '?'"
				<< ends;

	if( SQLDb->Exec(theQuery, true ) )
	{
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); ++i)
		{
			int suppId = atoi(SQLDb->GetValue(i,2).c_str());
			IncomingData current;
			current.chanId = atoi(SQLDb->GetValue(i,0).c_str());
			current.chanName = SQLDb->GetValue(i,1).c_str();
			current.mngrUserName = SQLDb->GetValue(i,3).c_str();
			if (string(SQLDb->GetValue(i,4).c_str()) == "Y")
				current.noticed = true;
			else
				current.noticed = false;
			suppIdList[suppId].push_back(current);
		}
	}
	else
	{
		LOGSQL_ERROR( SQLDb ) ;
		return;
	}
	if ((int)suppIdList.size() == 0)
		return;
	suppIdListType::const_iterator suppItr = suppIdList.begin();
	IncomingDataListType::const_iterator currItr;
	stringstream message;
	stringstream chanEnum;
	for (suppItr = suppIdList.begin(); suppItr != suppIdList.end(); ++suppItr)
	{
		sqlUser* suppUser = getUserRecord(suppItr->first);
		if (!suppUser)
			continue;
		//Don't do anything, for offline usernames!
		if (suppUser->networkClientList.size() == 0)
			continue;
		message.str("");
		chanEnum.str("");
		if (suppItr->second.size() == 1)
		{
			currItr = suppItr->second.begin();
			//if (FirstNoticing) message << "\002[FIRST] \002";	//DEBUG
			message << "You are listed as a supporter for registration application for " << currItr->chanName
					<< ". To progress the channel registration application you required to lodge your support of the registrant "
					<< "(" << currItr->mngrUserName << "). "
					<< "Please type: \002/msg " << this->getNickName() << " support " << currItr->chanName << " <yes|no>\002" << endl;

			if ((FirstNoticing) && (!currItr->noticed))
			{
				noticeAllAuthedClients(suppUser, message.str().c_str());
				LOG_MSG( DEBUG, "setSupporterNoticedStatus for chanId={} userId={user_id}", currItr->chanId )
				.with( "user", suppUser )
				.logStructured() ;
				setSupporterNoticedStatus(suppUser->getID(), currItr->chanId, true);
			}
			if ((!FirstNoticing) && (currItr->noticed))
				noticeAllAuthedClients(suppUser, message.str().c_str());
		}
		else
		{
			for (currItr = suppItr->second.begin(); currItr != suppItr->second.end(); ++currItr)
			{
				if (!chanEnum.str().empty())
					chanEnum << ", ";
				//if (FirstNoticing) message << "\002[FIRST] \002";	//DEBUG
				chanEnum << currItr->chanName << " (" << currItr->mngrUserName << ")";

				if ((FirstNoticing) && (!currItr->noticed))
					setSupporterNoticedStatus(suppUser->getID(), currItr->chanId, true);
			}
			chanEnum << ends;
			//Set back to the first IncomingData item, to be able to check a 'noticed' status
			currItr = suppItr->second.begin();
			if (((FirstNoticing) && (!currItr->noticed))
					|| ((!FirstNoticing) && (currItr->noticed)))
			{
				noticeAllAuthedClients(suppUser, "You are listed as a supporter of the channel application for the following channels: %s", chanEnum.str().c_str());
				message << "To progress the channel registration application you required to lodge your support of the registrant. "
					"For each channel please type '\002/msg " << getNickName().c_str() << " support <#chan> <YES|NO>\002' "
					"to confirm or deny your support for each channel." << ends;
				noticeAllAuthedClients(suppUser, message.str().c_str());
			}
		}
	}
	suppIdList.clear();
	return;
}

void cservice::initialiseSupport(const string& chanName, sqlPendingChannel::supporterListType suppList)
{
	Channel* theChan = Network->findChannel(chanName);
	if (!theChan)
	{
		/* Can this happen?! what would we do?!
		 * Maybe they are in netsplit momentarly?!
		 */
		LOG_MSG( WARN, "Warning: New empty channel application of {chan_name} (no users found on channel)" )
			.with( "chan_name", chanName )
			.logStructured() ;
		return;
	}
	int totalUsers = 0;
	int clonesCount = 0;
	pendingChannelListType::iterator ptr = pendingChannelList.find(chanName);
	sqlPendingChannel* pendingChan = ptr->second;
	for (Channel::userIterator userItr = theChan->userList_begin();
		userItr != theChan->userList_end(); ++userItr)
	{
		ChannelUser* theUser = userItr->second;
		//No way, but ...
		//if (theUser->getClient()->isModeK())
		//	continue;
		//Use the /64 form for IPv6 addresses ...
		string insertIP = xIP(theUser->getIP()).GetNumericIP(true);
		sqlUser* loggedUser = isAuthed(theUser->getClient(), false);
		//Is this user a supporter?
		bool isSupporter = false;
		if (loggedUser)
		{
			if (suppList.find(loggedUser->getID()) != suppList.end())
				isSupporter = true;
		}
		if (isSupporter)
		{
			sqlPendingChannel::supporterListType::iterator Supptr = pendingChan->supporterList.find(loggedUser->getID());
			if (Supptr == pendingChan->supporterList.end())
			{
				//elog << "cservice::initializeInitialIPs> New supporter for channel " << chanName << " suppUser = " << loggedUser->getUserName() << " inserting with joincount 1 " << endl;
				pendingChan->supporterList.insert(sqlPendingChannel::supporterListType::value_type(loggedUser->getID(), 1));
			}
			else
			{
				//elog << "cservice::initializeInitialIPs> Already existing supporter for channel " << chanName << " suppUser = " << loggedUser->getUserName() << " joinCount = " << Supptr->second << " reset joincount to 1" << endl;
				Supptr->second = 1;
			}
			LOG_MSG( INFO, "New total for Supporter #{user_id} ({user_name}) on {chan_name} is {supporters}." )
			.with( "user_id", loggedUser->getID() )
			.with( "user_name", loggedUser->getUserName() )
			.with( "chan", theChan )
			.with( "supporters", Supptr->second )
			.logStructured() ;
			pendingChan->commitSupporter(Supptr->first, Supptr->second);
		}
		sqlPendingTraffic* trafRecord;
		sqlPendingChannel::trafficListType::iterator Tptr = pendingChan->trafficList.find(insertIP);
		if (Tptr == pendingChan->trafficList.end())
		{
			pendingChan->unique_join_count++;
			trafRecord = new (std::nothrow) sqlPendingTraffic(this);
			trafRecord->channel_id = pendingChan->channel_id;
			trafRecord->ip_number = insertIP;
			trafRecord->join_count = 1;
			pendingChan->trafficList.insert(sqlPendingChannel::trafficListType::value_type(insertIP, trafRecord));
			trafRecord->insertRecord();
		}
		else
		{
			trafRecord = Tptr->second;
			trafRecord->join_count = 1;
			trafRecord->channel_id = pendingChan->channel_id;
			trafRecord->commit();
			clonesCount++;
		}
		totalUsers++;
		if (totalUsers > 50)
		{
			// Hmm, what a strange large channel
			LOG_MSG( INFO, "Weird large new channel application of {chan_name}, usercount={}, clones={}", totalUsers, clonesCount )
			.with( "chan", theChan )
			.logStructured() ;
			logAdminMessage("Weird large new channel application of %s, usercount=%i, clones=%i", theChan->getName().c_str(), totalUsers, clonesCount);
		}
	}
	pendingChan->join_count = totalUsers;
	pendingChan->initialised = true;
	pendingChan->commit();
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
		LOGSQL_ERROR( SQLDb ) ;
	}

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
		LOGSQL_ERROR( SQLDb ) ;
	}

	pendingChannelList.clear();
}

/*
 * For simplicity, we assume that if a pending channel is in state "1", then it has 5 valid
 * supporters who have said "Yes" and we're looking at them.
 */

stringstream theQuery;
theQuery	<<  "SELECT channels.name, pending.channel_id, user_id, pending.join_count, supporters.join_count, pending.unique_join_count, pending.first_init, pending.check_start_ts"
			<< " FROM pending,supporters,channels"
			<< " WHERE pending.channel_id = supporters.channel_id"
			<< " AND channels.id = pending.channel_id"
			<< " AND pending.status = 1;"
			<< ends;

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

		if (ptr != pendingChannelList.end())
		{
			// It already exists.
			newPending = ptr->second;
		}
		else
		{	
			newPending = new sqlPendingChannel(this);
			newPending->channel_id =
				atoi(SQLDb->GetValue(i,1).c_str());
			newPending->join_count =
				atoi(SQLDb->GetValue(i,3).c_str());
			newPending->unique_join_count =
				atoi(SQLDb->GetValue(i,5).c_str());
			string strInited = SQLDb->GetValue(i,6).c_str();
			if (strInited == "Y")
				newPending->initialised = true;	
			else 
				newPending->initialised = false;
			//newPending->initialised = 
			//	(SQLDb->GetValue(i,6).c_str() == "Y") ? true : false;
			newPending->checkStart = atoi(SQLDb->GetValue(i,7).c_str());
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

	LOG( INFO, "Loaded Pending Channels, there are currently {} channels being traffic monitored.",
		pendingChannelList.size() ) ;

	/*
	 * For each pending channel, load up its IP traffic
	 * cache.
	 */

	pendingChannelListType::iterator ptr = pendingChannelList.begin();
	while (ptr != pendingChannelList.end())
	{
		sqlPendingChannel* pendingChan = ptr->second;
		pendingChan->loadTrafficCache();
		pendingChan->loadSupportersTraffic();
		if (!pendingChan->initialised)
		{
			initialiseSupport(ptr->first, pendingChan->supporterList);
		}
		++ptr;
	};
}

void cservice::checkDbConnectionStatus()
{
	if( SQLDb->ConnectionBad() )
//	if(SQLDb->Status() == CONNECTION_BAD)
	{
		logAdminMessage("\002WARNING:\002 Backend database connection has been lost, attempting to reconnect.");
		LOG( WARN, "Attempting to reconnect to database." ) ;

		/* Remove the old database connection object. */
		delete(SQLDb);

		string Query = "host=" + confSqlHost + " dbname=" + confSqlDb + " port=" + confSqlPort + " user=" + confSqlUser
					 + " password=" + confSqlPass;

		SQLDb = new (std::nothrow) dbHandle( this, confSqlHost,
			atoi( confSqlPort ),
			confSqlDb,
			confSqlUser,
			confSqlPass ) ;
//		SQLDb = new (std::nothrow) cmDatabase( Query.c_str() ) ;
		assert( SQLDb != 0 ) ;

		if (SQLDb->ConnectionBad())
		{
			LOG( ERROR, "Unable to connect to SQL server." ) ;
			LOGSQL_ERROR( SQLDb ) ;

			connectRetries++;
			if (connectRetries == connectRetry)
				{
				logAdminMessage("Unable to contact database after %d attempts, shutting down.", connectRetry);
				LOG( FATAL, "Unable to contact database after {} attempts, shutting down.", connectRetry ) ;
				}
			else if (connectRetries > connectRetry)
				::exit(0);
			else
				logAdminMessage("Connection failed, retrying:");

		} else
		{
// TODO: Is this ok?
				SQLDb->Exec("LISTEN channels_u; LISTEN users_u; LISTEN levels_u;");
//				SQLDb->ExecCommandOk("LISTEN channels_u; LISTEN users_u; LISTEN levels_u;");
				logAdminMessage("Successfully reconnected to database server. Panic over ;)");
				connectRetries = 0 ;
		}
	}

}

void cservice::preloadChannelCache()
{
stringstream theQuery;
theQuery	<< "SELECT " << sql::channel_fields
			<< " FROM channels"
			<< " WHERE registered_ts <> 0"
			<< ends;

elog	<< "*** [CMaster::preloadChannelCache]: Loading all registered channel records: "
		<< endl;

if( SQLDb->Exec(theQuery, true ) )
//if( PGRES_TUPLES_OK == status )
	{
	for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
		{
 		/* Add this information to the channel cache. */

		sqlChannel* newChan = new (std::nothrow) sqlChannel(this);
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

		sqlBan* newBan = new (std::nothrow) sqlBan(this);
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
		sqlLevel* newLevel = new (std::nothrow) sqlLevel(this);
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
				<< " users_lastseen.last_seen >= (date_part('epoch', CURRENT_TIMESTAMP)::int - "
				<< (preloadUserDays * 86400) << ")"
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
				sqlUser* newUser = new (std::nothrow) sqlUser(this);
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

void cservice::preloadFingerprintCache()
{
	stringstream theQuery;
	theQuery	<< "SELECT fingerprint,user_id FROM users_fingerprints"
				<< endl;

	elog		<< "*** [CMaster::preloadFingerprintCache]: Loading TLS fingerprints : "
				<< endl;

	if( SQLDb->Exec(theQuery, true ) )
	{
		for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
			{
			fingerprintMap.emplace(SQLDb->GetValue(i,0), std::stoul(SQLDb->GetValue(i,1)));
			}
	}

	elog	<< "*** [CMaster::preloadFingerprintCache]: Done. Loaded "
			<< SQLDb->Tuples()
			<< " fingerprints."
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

	if( prometheus )
		prometheus->incrementCounter(name);
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

void cservice::NoteAllAuthedClients(sqlUser* theUser, const char* Message, ... )
{
	if( Connected && MyUplink && Message && Message[ 0 ] != 0 )
	{
		char buffer[ 1024 ] = { 0 } ;
		va_list list;

		va_start(list, Message);
		vsnprintf(buffer, 1024, Message, list);
		va_end(list);

		/*
		 * Loop over all people auth'd as this user, and send them a
		 * message.
		 */
		if (theUser->networkClientList.size() != 0)
		{
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
			return;
		}

		string noteMessage = string(buffer);

		stringstream queryString;
		queryString	<< "DELETE FROM notices WHERE last_updated IN "
				<< "(SELECT MIN(last_updated) FROM notices "
				<< "WHERE user_id = "
				<< theUser->getID()
				<< " HAVING count(last_updated) >= "
				<< MAXnotes
				<< ")"
				<< ends;

		if( !SQLDb->Exec(queryString, true ) )
			{
			LOG( ERROR, "Something went wrong in the DELETE FROM query:" ) ;
			LOGSQL_ERROR( SQLDb ) ;
			return;
			}
		static const char* queryHeader = "INSERT INTO notices (user_id,message,last_updated) VALUES (";
		queryString.str("");
		queryString	<< queryHeader
				<< theUser->getID() << ", '"
				<< escapeSQLChars(noteMessage) << "', "
				<< "date_part('epoch', CURRENT_TIMESTAMP)::int);"
				<< ends;

		if( !SQLDb->Exec(queryString, true ) )
			{
			LOG( ERROR, "Something went wrong in the INSERT INTO query:" ) ;
			LOGSQL_ERROR( SQLDb ) ;
			return;
			}
	} //if MyUpLink
	return;
}

void cservice::NoteChannelManager(const string& theChan, const char* Message, ...)
{
	sqlChannel* sqlChan = getChannelRecord(theChan);
	if (!sqlChan)
	{
		LOG( ERROR, "Not found channel {}", theChan ) ;
		return;
	}
	vector<sqlUser*> mngrList = getChannelManager(sqlChan->getID());
	for (int i = 0; i < (int)mngrList.size(); ++i)
		NoteAllAuthedClients(mngrList.at(i), Message);
	return;
}

void cservice::NoteChannelManager(sqlChannel* theChan, const char* Message, ...)
{
	vector<sqlUser*> mngrList = getChannelManager(theChan->getID());
	for (int i = 0; i < (int)mngrList.size(); ++i)
		NoteAllAuthedClients(mngrList.at(i), Message);
	return;
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
	// *** Count all database users ***
	int userDBTotal = 0;
	stringstream theQuery;
	theQuery	<< "SELECT COUNT(*) FROM users"
				<< ends;

	if( !SQLDb->Exec(theQuery, true ) )
//	if (PGRES_TUPLES_OK != status)
	{
		LOGSQL_ERROR( SQLDb ) ;
		return;
	}
	if (SQLDb->Tuples() > 0)
		userDBTotal = atoi(SQLDb->GetValue(0,0));
	else
		LOG( ERROR, "Not found any users!" ) ;

	// *** Count all database users with TOTP ***
	int userDBTOTPTotal = 0;
	theQuery.str("");
	theQuery	<< "SELECT COUNT(*) FROM users WHERE flags & "
				<< sqlUser::F_TOTP_ENABLED
				<< " > 0"
				<< ends;

	if( !SQLDb->Exec(theQuery, true ) )
		{
		LOGSQL_ERROR( SQLDb ) ;
		return;
		}
	userDBTOTPTotal = atoi(SQLDb->GetValue(0,0));

	// *** Count all database users with IPR ***
	int userDBIPRTotal = 0;
	theQuery.str("");
	theQuery	<< "SELECT COUNT(*) FROM (SELECT DISTINCT user_id FROM ip_restrict) AS temp"
				<< ends;

	if( !SQLDb->Exec(theQuery, true ) )
		{
		LOGSQL_ERROR( SQLDb ) ;
		return;
        }
	userDBIPRTotal = atoi(SQLDb->GetValue(0,0));

	// *** Count all database users with TOTP and IPR ***
	int userDBTOTPIPRTotal = 0;
	theQuery.str("");
	//theQuery	<< "SELECT COUNT(*) FROM users,ip_restrict (SELECT DISTINCT user_id FROM ip_restrict) AS temp WHERE flags & "
	theQuery	<< "SELECT COUNT(*) FROM users,ip_restrict WHERE flags & "
				<< sqlUser::F_TOTP_ENABLED
				<< " > 0 AND users.id = ip_restrict.user_id"
				<< ends;

	if( !SQLDb->Exec(theQuery, true ) )
		{
		LOGSQL_ERROR( SQLDb ) ;
		return;
        }
	userDBTOTPIPRTotal = atoi(SQLDb->GetValue(0,0));

	int userDBFPTotal = 0;
	theQuery.str("");
	theQuery	<< "SELECT COUNT(*) FROM users,users_fingerprints WHERE "
				<< "users.id = users_fingerprints.user_id"
				<< endl;
#ifdef LOG_SQL
	elog	<< "cservice::doCoderStats::sqlQuery> "
		<< theQuery.str().c_str()
		<< endl;
#endif

	if( !SQLDb->Exec(theQuery, true ) )
	{
		elog    << "cservice::doCoderStats> SQL Error: "
			<< SQLDb->ErrorMessage()
			<< endl;
		return;
        }
	userDBFPTotal = atoi(SQLDb->GetValue(0,0));

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
	unsigned int iprCount = 0;
	unsigned int totpCount = 0;
	unsigned int ipr_totp_Count = 0;
	sqlUserHashType::iterator ptr = sqlUserCache.begin();
	sqlUser* tmpUser;

	while (ptr != sqlUserCache.end())
	{
		tmpUser = ptr->second;
		if (tmpUser->isAuthed()) 
		{
			if (hasIPR(tmpUser))
			{
				iprCount++;
				if (tmpUser->getFlag(sqlUser::F_TOTP_ENABLED))
				{
					totpCount++;
					ipr_totp_Count++;
				}
			}
			else 
				if (tmpUser->getFlag(sqlUser::F_TOTP_ENABLED))
					totpCount++;
		}
		++ptr;
	}
	
	/*
	 * Iterate over all the clients on the network and
	 * see how many are +x.
	 */

	unsigned int plusXCount = 0;
	unsigned int plusWCount = 0;
	unsigned int plusDCount = 0;
	unsigned int authCount = 0;

	xNetwork::const_clientIterator ptr2 = Network->clients_begin();
	while(ptr2 != Network->clients_end())
	{
		iClient* tmpClient = ptr2->second;
		if (tmpClient->isModeX()) plusXCount++;
		if (tmpClient->isModeW()) plusWCount++;
		if (tmpClient->isModeD()) plusDCount++;
		if (tmpClient->isModeR()) authCount++;
		++ptr2;
	}

	Notice(theClient, "--- Total clients : %i", Network->clientList_size());

	float cacheTotal = ((float)sqlUserCache.size() / (float)userDBTotal) * 100;
	Notice(theClient, "--- Total users in DB: %i (in cache: %i (%.2f%% of total))", userDBTotal, (int)sqlUserCache.size(), cacheTotal);
	float cacheTOTPTotal = ((float)totpCount / (float)sqlUserCache.size()) * 100;
	Notice(theClient, "--- Total users in DB with TOTP: %i (in cache: %i (%.2f%% of total))", userDBTOTPTotal, totpCount, cacheTOTPTotal);
	float cacheIPRTotal = ((float)iprCount / (float)sqlUserCache.size()) * 100;
	Notice(theClient, "--- Total users in DB with IPR: %i (in cache: %i (%.2f%% of total))", userDBIPRTotal, iprCount, cacheIPRTotal);
	float cacheIPRTOTPTotal = ((float)ipr_totp_Count / (float)sqlUserCache.size()) * 100;
	Notice(theClient, "--- Total users in DB with TOTP&IPR: %i (in cache: %i (%.2f%% of total))", userDBTOTPIPRTotal, ipr_totp_Count, cacheIPRTOTPTotal);
	Notice(theClient, "--- Total users in DB with TLS: %i (fingerprints in cache %i)", userDBFPTotal, fingerprintMap.size());

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

	float totpTotal = ((float)totpCount / (float)authCount) * 100;
	Notice(theClient, "--- Total TOTP: %i (%.2f%% of total)",
		totpCount, totpTotal);

	float iprTotal = ((float)iprCount / (float)authCount) * 100;
	Notice(theClient, "--- Total IPR: %i (%.2f%% of total)",
		iprCount, iprTotal);

	float totp_ipr_Total = ((float)ipr_totp_Count / (float)authCount) * 100;
	Notice(theClient, "--- Total TOTP&IPR: %i (%.2f%% of total)",
		ipr_totp_Count, totp_ipr_Total);

        float joinTotal = ((float)joinCount / (float)Network->channelList_size()) * 100;
        Notice(theClient, "I am in %i channels out of %i on the network. (%.2f%%)",
                joinCount, Network->channelList_size(), joinTotal);

	unsigned int secs = (currentTime() - getUplink()->getStartTime());

	float cPerSec = (float)totalCommands / (float)secs;

	Notice(theClient, "I've received %i commands since I started (%.2f commands per second).",
		totalCommands, cPerSec);

	Notice(theClient, "\002Memory usage:\002 %s KB",
		prettyNumber( getMemoryUsage() ).c_str() ) ;

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

/*
 * Register configuration variables.
 */
void cservice::registerConfigBindings()
{
#define CONFIG_VAR(type, name, key) \
configBindings.push_back( { \
  key, [this]( bool rehash ) { \
    if( rehash ) \
        name = cserviceConfig->TryRequire< type >( key, name ) ; \
    else \
        name = cserviceConfig->Require< type >( key ) ; \
  } \
} ) ;

CONFIG_VAR_LIST
#undef CONFIG_VAR
}

/*
 * Load variable values from cservice.conf
 */
void cservice::loadConfigVariables( bool rehash )
{
if( configBindings.empty() )
  registerConfigBindings() ;

for( auto& entry : configBindings )
  entry.load( rehash ) ;

/* Load pushover settings if enabled. */
if( pushoverEnable )
  {
  pushoverUserKeys.clear() ;
  if( rehash )
    {
    pushoverToken = cserviceConfig->TryRequire< std::string >( "pushover_token", pushoverToken ) ;
    pushoverVerbosity = cserviceConfig->TryRequire< unsigned int >( "pushover_verbosity", pushoverVerbosity ) ;
    }
  else
    {
    pushoverToken = cserviceConfig->Require< std::string >( "pushover_token" ) ;
    pushoverVerbosity = cserviceConfig->Require< unsigned int >( "pushover_verbosity" ) ;
    }

  auto confPtr = cserviceConfig->Find( "pushover_userkey" ) ;
  while( confPtr != cserviceConfig->end() && confPtr->first == "pushover_userkey" )
    {
    pushoverUserKeys.push_back( confPtr->second ) ;
    ++confPtr ;
    }
  }

/* Load prometheus settings if enabled. */
#ifdef HAVE_PROMETHEUS
if( prometheusEnable )
  {
  if( rehash )
	{
	prometheusIP = cserviceConfig->TryRequire< std::string >( "prometheus_ip", prometheusIP ) ;
	prometheusPort = cserviceConfig->TryRequire< unsigned int >( "prometheus_port", prometheusPort ) ;
	}
  else
	{
	prometheusIP = cserviceConfig->Require< std::string >( "prometheus_ip" ) ;
	prometheusPort = cserviceConfig->Require< unsigned int >( "prometheus_port" ) ;
	}
  }
#endif // HAVE_PROMETHEUS

if (MinSupporters > RequiredSupporters)
	MinSupporters = RequiredSupporters;

if (MAXnotes == 0) MAXnotes = 7;

#ifdef ALLOW_HELLO
	helloBlockPeriod = cserviceConfig->Require< unsigned int >( "hello_block_period" ) ;
#endif // ALLOW_HELLO

#ifdef TOTP_AUTH_ENABLED
  totpAuthEnabled = cserviceConfig->Require< bool >( "enable_totp" ) ;
#endif // TOTP_AUTH_ENABLED

if (daySeconds < 1)
	daySeconds = 1;
UsersExpireDBDays *= daySeconds;

/* Initiate logger. */
logger->setChannel( debugChan ) ;
logger->setLogVerbosity( logVerbosity ) ;
logger->setChanVerbosity( chanVerbosity ) ;
logger->setConsoleVerbosity( consoleVerbosity ) ;
logger->setLogSQL( logSQL ) ;
logger->setConsoleSQL( consoleSQL ) ;
}

void cservice::rehashConfigVariables()
{
/* Clear existing config variables. */
clearConfigVariables() ;

/* Parse the config file. */
parseConfigFile() ;

/* Load the config variables. */
loadConfigVariables( true ) ;

/* Prometheus is enabled in config but not running. */
#ifdef HAVE_PROMETHEUS
if( prometheusEnable && !prometheus )
	{
	try
		{
		prometheus = std::make_shared< PrometheusClient >( this, prometheusIP, prometheusPort ) ;
		logger->addNotifier( prometheus ) ;
		}
	catch( const std::exception& e )
		{
		LOG( ERROR, "Unable to start Prometheus on {}:{} - {}",
			prometheusIP, prometheusPort, e.what() ) ;
		prometheus.reset() ;
		}
	}
else if( !prometheusEnable && prometheus )
	{
	logger->removeNotifier( prometheus ) ;
	prometheus.reset() ;
	}
#endif

/* Pushover is enabled in config. */
if( pushoverEnable )
  {
  /* We have a running instance of pushover. Rehash settings. */
  if( pushover )
    {
    pushover->setUserKeys( pushoverUserKeys ) ;
    pushover->setToken( pushoverToken ) ;
    logger->updateNotifierVerbosity( pushover, pushoverVerbosity ) ;
    }
  else
    {
    /* Pushover is enabled in config, but not in gnuworld. Enable. */
#ifdef USE_THREAD
    pushover = std::make_shared< PushoverClient >( this, pushoverToken, pushoverUserKeys, &threadWorker ) ;
#else
    pushover = std::make_shared< PushoverClient >( this, pushoverToken, pushoverUserKeys ) ;
#endif
    logger->addNotifier( pushover, pushoverVerbosity ) ;
    }
  }
else
  {
  /* Pushover is disabled in config, but still enabled in gnuworld. Disable. */
  if( pushover )
    {
    logger->removeNotifier( pushover ) ;
    pushover.reset() ;
    }
  }
}

cservice::AuthResult cservice::authenticateUser( AuthStruct& auth )
{
unsigned int ipr_ts ;
bool certAuth = false ; // Set to true if the client has a certificate matching the username

/* 1: Check loginDelay. */
unsigned int useLoginDelay = getConfigVar("USE_LOGIN_DELAY")->asInt();
unsigned int loginTime = getUplink()->getStartTime() + loginDelay;
if ( (useLoginDelay == 1) && (loginTime >= (unsigned int)currentTime()) )
    {
    return TOO_EARLY_TOLOGIN ;
    }

/* 2: Check whether this iClient has exceeded the maximum number of failed login attempts. */
if( auth.theClient )
	{
	unsigned int maxFailedLogins = getConfigVar("MAX_FAILED_LOGINS")->asInt() ;
	unsigned int failedLogins = getFailedLogins( auth.theClient ) ;
	if( ( maxFailedLogins > 0 ) && ( failedLogins >= maxFailedLogins ) )
		return AUTH_FAIL_EXCEEDED ;
	}

/* 3: Check that the username exists. */
// TODO: Force a refresh of the user's info from the db
sqlUser* theUser = getUserRecord( auth.username ) ;
if( !theUser )
    {
	return AUTH_UNKNOWN_USER ;
    }

auth.theUser = theUser ;

/* 4: Check whether the username is glbobally suspended. */
if( theUser->getFlag( sqlUser::F_GLOBAL_SUSPEND ) )
	{
	return AUTH_SUSPENDED_USER ;
	}

/**
 * 5: If the client has a fingerprint, we check whether it matches the username.
 * If it does, we take that into account when parsing the password string to look for
 * the TOTP token (if any). If there is no match, we treat the password string as a
 * regular login.
 * 
 * If the client is authenticating with EXTERNAL, the auth fails if the fingerprint does not match the username.
 *
 */
if( !auth.fingerprint.empty() )
	{
	auto it = fingerprintMap.find( auth.fingerprint ) ;
	if( it != fingerprintMap.end() && it->second == theUser->getID() )
		certAuth = true ;
	}

if( auth.sasl == SaslMechanism::EXTERNAL && !certAuth )
	{
	return AUTH_INVALID_FINGERPRINT ;
	}

/* 6: If CERTONLY is set for this user, the auth fails if we did not get a fingerprint match. */
if( theUser->getFlag( sqlUser::F_CERTONLY ) && !certAuth )
	{
	return AUTH_CERTONLY ;
	}

/**
 * 7: Check whether a TOTP token has been provided.
 * TOTP is not required for fingerprint authentication if F_CERT_DISABLE_TOTP is set.
 */
StringTokenizer st( auth.password ) ;
StringTokenizer::size_type pass_end = st.size() ;

#ifdef TOTP_AUTH_ENABLED
bool totp_enabled = false ;

if( totpAuthEnabled && theUser->getFlag( sqlUser::F_TOTP_ENABLED ) )
	{
	/* If the user has a certificate matching the username or we are using SCRAM, the password is the TOTP token. */
    if( ( certAuth && !theUser->getFlag( sqlUser::F_CERT_DISABLE_TOTP ) )
		|| auth.sasl == SaslMechanism::SCRAM_SHA_256 ) 
		{
        if( st.size() < 1 )
            return AUTH_NO_TOKEN ;

        pass_end = st.size() - 1 ;
		totp_enabled = true ;
		}
	/* If the user does not have a certificate matching the username and we are not using SCRAM, the second part of the password is the TOTP token. */
    else if( !certAuth && auth.sasl != SaslMechanism::SCRAM_SHA_256 )
    	{
        if( st.size() < 2 )
            return AUTH_NO_TOKEN ;

        pass_end = st.size() - 1 ;
		totp_enabled = true ;
		}
	}
#endif

/**
 * 8: IPR
 * Check if this is a privileged user, if so check against IP restrictions.
 */
if( needIPRcheck( theUser ) )
	{
	/* ok, they have "*" access (excluding alumni's) */
	if( !checkIPR( auth.ip, theUser, ipr_ts ) )
		return AUTH_FAILED_IPR ;
	}

/*
 * 9: Check password.
 * Password will not be checked for EXTERNAL or SCRAM or when a matching certificate is provided.
 */
if( !certAuth && auth.sasl != SaslMechanism::SCRAM_SHA_256
	&& auth.sasl != SaslMechanism::EXTERNAL
	&& !isPasswordRight( theUser, st.assemble( 0, pass_end ) ) )
    {
    return AUTH_INVALID_PASS ;
    }

/*
 * Compute SCRAM RECORD if it does not exist.
 */
#ifdef HAVE_LIBSSL
if( !certAuth && auth.sasl != SaslMechanism::SCRAM_SHA_256
	&& auth.sasl != SaslMechanism::EXTERNAL
	&& theUser->getScramRecord().empty() )
	{
	std::string err ;
	auto recOpt = make_scram_sha256_record( st.assemble( 0, pass_end ), &err ) ;

	if( !recOpt )
		{
		LOG( ERROR, "[SCRAM] Record generation error: {}", err ) ;
		}
	else
		{
		std::string scram_record = *recOpt ;
		theUser->setScramRecord( scram_record ) ;
		if( auth.theClient )
			theUser->commit( auth.theClient ) ;
		else
			theUser->commit( auth.ident + "@" + auth.ip ) ;
		}
	}
#endif // HAVE_LIBSSL

/* 10: Check TOTP token. */
#ifdef TOTP_AUTH_ENABLED
if( totp_enabled )
	{
	char* key ;
	size_t len ;
	int res  = oath_base32_decode( theUser->getTotpKey().c_str(), theUser->getTotpKey().size(), &key, &len ) ;
	if( res != OATH_OK )
		return AUTH_ERROR ;

	res = oath_totp_validate( key, len, time(NULL), 30, 0, 1, st[ pass_end ].c_str() ) ;
	free( key ) ;
	if( res < 0 )
		return AUTH_INVALID_TOKEN;
	}
#endif

/* 12: Don't exceed MAXLOGINS. */
bool iploginallow = false ;
string clip = fixToCIDR64( auth.ip.c_str() ) ;
if( theUser->networkClientList.size() + 1 > theUser->getMaxLogins() )
	{
	/* They have exceeded their maxlogins setting, but check if they
	 * are allowed to login from the same IP - only applies if their
	 * maxlogins is set to ONE.
	 */
	uint32_t iplogins = getConfigVar("LOGINS_FROM_SAME_IP")->asInt() ;
	uint32_t iploginident = getConfigVar("LOGINS_FROM_SAME_IP_AND_IDENT")->asInt() ;
	if( ( theUser->getMaxLogins() == 1 ) && ( iplogins > 1 ) )
    	{
		/* ok, we're using the multi-logins feature (0=disabled) */
		if( theUser->networkClientList.size() + 1 <= iplogins )
			{
			/* Check their IP from previous session against
			 * current IP.  If it matches, allow the login.
			 * As this only applies if their maxlogin is 1, we
			 * know there is only 1 entry in their clientlist.
			 */
			if( clip == ( xIP( theUser->networkClientList.front()->getIP()).GetNumericIP( true ) ) )
				{
				if( iploginident == 1 )
					{
					/* need to check ident here */
					string oldident = theUser->networkClientList.front()->getUserName() ;
					if( ( oldident[0] =='~' ) || ( oldident == auth.ident ) )
						{
						/* idents match (or they are unidented) - allow this login */
						iploginallow = true ;
						}
					}
				else
					{
					/* don't need to check ident, this login is allowed */
					iploginallow = true ;
					}
				}
            }
        }

      if( !iploginallow )
		return AUTH_ML_EXCEEDED;
	}

/* Success. */
if( auth.theClient )
	setIPRts( auth.theClient, ipr_ts ) ;

return AUTH_SUCCEEDED ;
}

/* This function is called after authenticateUser() to process the authentication. */
bool cservice::processAuthentication( AuthStruct auth, std::string* Message )
{
/* Get background variables. */
unsigned int loginTime = getUplink()->getStartTime() + loginDelay ;
unsigned int max_failed_logins = getConfigVar("FAILED_LOGINS")->asInt() ;
unsigned int failed_login_rate = getConfigVar("FAILED_LOGINS_RATE")->asInt() ;

/* Fetch client info. */
string clientMask ;
if( auth.theClient )
	clientMask = auth.theClient->getRealNickUserHost() ;
else
	clientMask = auth.ident + "@" + auth.ip ;

string authResponse ;
bool setFailedLogin = false ;

switch( auth.result )
	{
	case cservice::TOO_EARLY_TOLOGIN:
		authResponse = TokenStringsParams( "AUTHENTICATION FAILED as %s (Unable to login during reconnection, please try again in %i seconds)",
        	        	auth.username.c_str(), ( loginTime - currentTime() ) ) ;
		break ;
	case cservice::AUTH_FAILED:
	    authResponse = TokenStringsParams( getResponse( NULL, language::auth_failed,
						string("AUTHENTICATION FAILED as %s") ).c_str(),
						auth.username.c_str() ) ;
		setFailedLogin = true ;
		break ;
	case cservice::AUTH_UNKNOWN_USER:
	    authResponse = TokenStringsParams( getResponse( NULL, language::not_registered,
						string("AUTHENTICATION FAILED as %s") ).c_str(),
						auth.username.c_str() ) ;
		setFailedLogin = true ;
		break ;
	case cservice::AUTH_SUSPENDED_USER:
		authResponse = TokenStringsParams( "AUTHENTICATION FAILED as %s (Suspended)",
        				auth.username.c_str() ) ;
		setFailedLogin = true ;
		break ;
	case cservice::AUTH_NO_TOKEN:
		authResponse = TokenStringsParams( "AUTHENTICATION FAILED as %s (Missing TOTP token)",
						auth.username.c_str() ) ;
		setFailedLogin = true ;
		break ;
	case cservice::AUTH_INVALID_PASS:
		if( failed_login_rate == 0 )
			failed_login_rate = 900 ;
	    authResponse = TokenStringsParams( getResponse( NULL, language::auth_failed,
						string("AUTHENTICATION FAILED as %s") ).c_str(),
                		auth.username.c_str() ) ;

        if( ( max_failed_logins > 0 ) && ( auth.theUser->getFailedLogins() > max_failed_logins ) &&
            ( auth.theUser->getLastFailedLoginTS() < ( time(NULL) - failed_login_rate ) ) )
		    {
			/* we have exceeded our maximum - alert relay channel */
			/* work out a checksum for the password.  Yes, I could have
				* just used a checksum of the original password, but this
				* means it's harder to 'fool' the check digit with a real
				* password - create MD5 from original salt stored */
			unsigned char   checksum ;
			md5             hash ;
			md5Digest       digest ;

			if( auth.theUser->getPassword().size() < 9 )
				checksum = 0 ;
			else
				{
				string salt = auth.theUser->getPassword().substr( 0, 8 ) ;
				string guess = salt + auth.password ;

				hash.update( (const unsigned char *)guess.c_str(), guess.size() ) ;
				hash.report( digest ) ;

				checksum = 0 ;
				for( size_t i = 0; i < MD5_DIGEST_LENGTH; i++ )
					{
					/* add ascii value to check digit */
					checksum += digest[ i ] ;
					}
				}

			auth.theUser->setLastFailedLoginTS( time(NULL) ) ;
			logPrivAdminMessage("%d failed logins for %s (last attempt by %s%s, checksum %d).",
					auth.theUser->getFailedLogins(),
					auth.theUser->getUserName().c_str(),
					clientMask.c_str(),
					auth.type == AuthType::XQUERY ? " (LoC)" : "",
					checksum ) ;
	        }
		setFailedLogin = true ;
		break ;
	case cservice::AUTH_INVALID_FINGERPRINT:
		authResponse = TokenStringsParams( "AUTHENTICATION FAILED as %s (Invalid fingerprint)",
						auth.username.c_str() ) ;
		setFailedLogin = true ;
		break ;
	case cservice::AUTH_ERROR:
		authResponse = TokenStringsParams( "AUTHENTICATION FAILED as %s due to an error. Please contact a CService representative",
						auth.username.c_str() ) ;
		break ;
	case cservice::AUTH_INVALID_TOKEN:
	    authResponse = TokenStringsParams( getResponse( NULL, language::auth_failed_token,
                        string("AUTHENTICATION FAILED as %s") ).c_str(),
                       	auth.username.c_str() ) ;

        setFailedLogin = true ;
		break ;
	case cservice::AUTH_FAILED_IPR:
		authResponse = TokenStringsParams("AUTHENTICATION FAILED as %s (IPR)",
						auth.username.c_str() ) ;

		/* notify the relay channel */
		logAdminMessage( "%s (%s) failed IPR check.",
				clientMask.c_str(),
				auth.username.c_str() ) ;

		if( ( max_failed_logins > 0 ) && ( auth.theUser->getFailedLogins() > max_failed_logins ) &&
			( auth.theUser->getLastFailedLoginTS() < ( time(NULL) - failed_login_rate ) ) )
			{
			/* we have exceeded our maximum - alert relay channel */
			auth.theUser->setLastFailedLoginTS( time(NULL) ) ;
			logPrivAdminMessage( "%d failed logins for %s (last attempt by %s).",
					auth.theUser->getFailedLogins(),
					auth.theUser->getUserName().c_str(),
					clientMask.c_str() ) ;
			}
		setFailedLogin = true ;
		break ;
	case cservice::AUTH_CERTONLY:
		authResponse = TokenStringsParams( "AUTHENTICATION FAILED as %s (Password login disabled and fingerprint mismatch)",
						auth.username.c_str() ) ;
		setFailedLogin = true ;
		break ;
	case cservice::AUTH_ML_EXCEEDED:
		authResponse = TokenStringsParams( "AUTHENTICATION FAILED as %s (Maximum concurrent logins exceeded)",
						auth.theUser->getUserName().c_str() ) ;

		/* Do not list the sessions for XQUERY logins. */
		if( auth.theClient )
			{
			string clientList ;
			for( sqlUser::networkClientListType::iterator ptr = auth.theUser->networkClientList.begin() ;
					ptr != auth.theUser->networkClientList.end() ; )
					{
					clientList += (*ptr)->getNickUserHost() ;
					++ptr ;
					if( ptr != auth.theUser->networkClientList.end() )
						clientList += ", " ;
					} // for()

			Notice( auth.theClient, "Current Sessions: %s", clientList.c_str() ) ;
			}
		setFailedLogin = true ;
		break ;
	case cservice::AUTH_FAIL_EXCEEDED:
		authResponse = TokenStringsParams( getResponse( NULL, language::max_failed_logins,
                		string("AUTHENTICATION FAILED as %s (Exceeded maximum login failures for this session)") ).c_str(),
                		auth.username.c_str() ) ;
		setFailedLogin = true ;
		break ;
	case cservice::AUTH_SUCCEEDED:
		break ;
	default:
		//Should never get here!
		elog << "Response " << auth.result << " while authenticating!" << endl ;
		authResponse = TokenStringsParams( "AUTHENTICATION FAILED as %s (due to an error)",
						auth.username.c_str() ) ;
		break ;
	}

if( setFailedLogin )
	{
	if( auth.theClient )
		{
		unsigned int failedLogins = getFailedLogins( auth.theClient ) ;
		setFailedLogins( auth.theClient, failedLogins + 1 ) ;
		}

	if( auth.theUser )
		auth.theUser->incFailedLogins() ;
	}

/* Send response only if it fails. If it succeeds, the message is sent from doCommonAuth() */
if( auth.theClient )
	{
	if( auth.result == AUTH_SUCCEEDED )
		{
		doCommonAuth( auth.theClient, auth.theUser->getUserName() ) ;
		return true ;
		}
	else
		{
		Notice( auth.theClient, authResponse ) ;
		return false ;
		}
	}
/**
 * If theClient is nullptr, it means that the authentication is done by SASL/XQUERY and auth will
 * occurr when the client is introduced to the network.
 */
else
	{
	*Message = authResponse ;
	}

return ( auth.result == AUTH_SUCCEEDED ) ;
}

bool cservice::parseSaslMechanism( const std::string& in, cservice::SaslMechanism& out )
{
    std::string up = string_upper( in ) ;
#define X(NAME, NAME_STR) if( up == NAME_STR ) { out = cservice::SaslMechanism::NAME ; return true ; }
    CSERVICE_SASL_MECH_LIST
#undef X
    return false ;
}

std::string cservice::saslMechanismToString( cservice::SaslMechanism mech )
{
#define X(NAME, NAME_STR) if( mech == cservice::SaslMechanism::NAME ) return NAME_STR ;
    CSERVICE_SASL_MECH_LIST
#undef X
    return "" ;
}

std::string cservice::saslMechsAdvertiseList()
{
//    std::string out = "MECHS " ;
	string out ;
    bool first = true ;
#define X(NAME, NAME_STR) \
    do { if( !first ) out += "," ; first = false ; out += string_lower( NAME_STR ) ; } while(0);
    CSERVICE_SASL_MECH_LIST
#undef X
    return out ;
}

bool cservice::doXQSASL( iServer* theServer, const string& Routing, const string& Message )
{
	elog << "cservice::doXQSASL: Routing: " << Routing << " Message: " << Message << "\n" ;

#ifdef HAVE_LIBSSL
	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
		{
		elog << "Received empty SASL message... Ignoring." << endl;
		return false ;
		}

	// Delete timed out requests.
	auto it = saslRequests.begin() ;
	while( it != saslRequests.end() )
		{
		if( currentTime() - it->last_ts > saslTimeout )
			it = saslRequests.erase(it) ;
		else
			++it ;
		}

	// Check if we have an existing entry with this routing and server
    it = std::find_if(
        saslRequests.begin(),
        saslRequests.end(),
        [&]( const SaslRequest& req )
			{ return req.routing == Routing && req.theServer == theServer ; }
    ) ;

	/* Found existing challenge. */
	if( it != saslRequests.end() )
    	{
        elog << "Found matching SASL request for Routing: " << Routing << "\n" ;
		string authMessage = st[ 1 ] ;

		it->last_ts = currentTime() ;

		// If the message is 400 bytes long we add it to the struct and wait for the next message.
		if( authMessage.size() == 400 )
			{
			it->credentials += authMessage ;
			return true ;
			}

		if( authMessage != "+" )
			it->credentials += authMessage ;

		if( it->mechanism == SaslMechanism::PLAIN )
			{
			auto bufferOpt = b64decode( it->credentials, nullptr, true ) ;
			if( !bufferOpt )
				{
				elog << "[PLAIN] Failed to decode credentials: " << it->credentials << endl ;
				incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
				doXResponse( theServer, Routing, "Invalid credentials", true ) ;
				saslRequests.erase( it ) ;
				return false ;
				}

			string decodedString = std::string( reinterpret_cast< char* >( bufferOpt->data() ), bufferOpt->size() ) ;
			size_t p1 = decodedString.find( '\0' ) ;
			size_t p2 = decodedString.find( '\0', p1 + 1 ) ;

			if( p1 == std::string::npos || p2 == std::string::npos || p1 == 0 )
				{
				elog << "[PLAIN] Invalid PLAIN format in decoded credentials: " << decodedString << endl ;
				incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
				doXResponse( theServer, Routing, "Invalid credentials", true ) ;
				saslRequests.erase( it ) ;
				return false ;
				}

			it->username = decodedString.substr( p1 + 1, p2 - ( p1 + 1 ) ) ;
			it->password = decodedString.substr( p2 + 1 ) ;

			if( it->username.empty() || it->password.empty() )
				{
				elog << "[PLAIN] Empty username or password in credentials" << endl ;
				incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
				doXResponse( theServer, Routing, "Invalid credentials", true ) ;
				saslRequests.erase( it ) ;
				return false ;
				}
			}
		else if( it->mechanism == SaslMechanism::EXTERNAL )
			{
			// A client certificate has to be provided.
			if( it->fingerprint.empty() )
				{
				incStat("SASL." + saslMechanismToString( it->mechanism ) + ".FAILED" ) ;
				doXResponse( theServer, Routing, "Client certificate required for SASL EXTERNAL authentication", true ) ;
				saslRequests.erase( it ) ;
				return false ;
				}

			// We only support EXTERNAL if a username has been provided.
			if( it->credentials.empty() )
				{
				incStat("SASL." + saslMechanismToString( it->mechanism ) + ".FAILED" ) ;
				doXResponse( theServer, Routing, "Username required for SASL EXTERNAL authentication", true ) ;
				saslRequests.erase( it ) ;
				return false ;
				}

			auto bufferOpt = b64decode( it->credentials, nullptr, true ) ;
			if( !bufferOpt )
				{
				elog << "[EXTERNAL] Failed to decode username: " << it->credentials << endl ;
				incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
				doXResponse( theServer, Routing, "Invalid username", true ) ;
				saslRequests.erase( it ) ;
				return false ;
				}

			string decodedString = std::string( reinterpret_cast< char* >( bufferOpt->data() ), bufferOpt->size() ) ;
			StringTokenizer st2( decodedString ) ;

			// Validate that we have at least a username
			if( st2.size() < 1 || st2[ 0 ].empty() )
				{
				elog << "[EXTERNAL] Invalid username in decoded credentials" << endl ;
				incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
				doXResponse( theServer, Routing, "Invalid username", true ) ;
				saslRequests.erase( it ) ;
				return false ;
				}

			it->username = st2[ 0 ] ;
			if( st2.size() > 1 )
				it->password = st2[ 1 ] ;
			}
		else if( it->mechanism == SaslMechanism::SCRAM_SHA_256 )
			{
			if( it->state == SaslState::INITIAL )
				{
				auto bufferOpt = b64decode( it->credentials, nullptr, true ) ;
				if( !bufferOpt )
					{
					elog << "[SCRAM] Failed to decode client first message: " << it->credentials << endl ;
					incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
					doXResponse( theServer, Routing, "Invalid credentials", true ) ;
					saslRequests.erase( it ) ;
					return false ;
					}

				string decodedString = std::string( reinterpret_cast< char* >( bufferOpt->data() ), bufferOpt->size() ) ;
				if( decodedString.find("n,,") != 0 )
					{
					elog << "[SCRAM] Invalid client first message: " << decodedString << endl ;
					incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
					doXResponse( theServer, Routing, "Invalid credentials", true ) ;
					saslRequests.erase( it ) ;
					return false ;
					}

				it->client_first = decodedString.substr( 3 ) ; // remove 'n,,'
				StringTokenizer st2( it->client_first, ',' ) ;
				for( size_t i = 0 ; i < st2.size() ; ++i )
					{
					if( st2[ i ].rfind( "n=", 0 ) == 0 )
						{
						StringTokenizer st3( st2[ i ].substr( 2 ) ) ;
						it->username =  st3[ 0 ] ;
						if( st3.size() > 1 )
							it->password = st3[ 1 ] ;
						}
					else if( st2[ i ].rfind( "r=", 0 ) == 0 )
						it->client_nonce = st2[ i ].substr( 2 ) ;
					}

				if( it->username.empty() || it->client_nonce.empty() )
					{
					elog << "[SCRAM] Missing username or client nonce in authentication: " << decodedString << endl ;
					incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
					doXResponse( theServer, Routing, "Invalid credentials", true ) ;
					saslRequests.erase( it ) ;
					return false ;
					}

				sqlUser* theUser = getUserRecord( it->username ) ;
				if( !theUser || theUser->getScramRecord().empty() )
					{
					incStat("SASL." + saslMechanismToString( it->mechanism ) + ".FAILED" ) ;
					doXResponse( theServer, Routing, "AUTHENTICATION FAILED as " + it->username, true ) ;
					saslRequests.erase( it ) ;
					return false ;
					}

				it->username = theUser->getUserName() ;

				auto nonceOpt = generateRandomNonce() ;
				if( !nonceOpt )
					{
					elog << "[SCRAM] Failed to generate server nonce." << endl ;
					incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
					doXResponse( theServer, Routing, "Internal error", true ) ;
					saslRequests.erase( it ) ;
					return false ;
					}
				it->server_nonce = *nonceOpt ;
				std::string combinedNonce = it->client_nonce + it->server_nonce ;

				std::string err ;
				auto parse_opt = parse_scram_sha256_record( theUser->getScramRecord(), &err ) ;
				if( !parse_opt )
					{
					incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
					LOG( ERROR, "[SCRAM] Record parse error: {}", err ) ;
					doXResponse( theServer, Routing, "Internal error", true ) ;
					return false ;
					}
				it->scram = *parse_opt ;

				/*elog << "Iterations: " << it->scram.iterations << endl;
				elog << "Salt (base64): " << b64encode(it->scram.salt.data(), it->scram.salt.size()) << endl;
				elog << "StoredKey (base64): " << b64encode(it->scram.storedKey.data(), it->scram.storedKey.size()) << endl;
				elog << "ServerKey (base64): " << b64encode(it->scram.serverKey.data(), it->scram.serverKey.size()) << endl;
				elog << "StoredKey (hex): ";
				for (auto c : it->scram.storedKey) elog << std::hex << (int)c << " ";
				elog << std::dec << "\n";
				elog << "ServerKey (hex): ";
				for (auto c : it->scram.serverKey) elog << std::hex << (int)c << " ";
				elog << std::dec << "\n";*/

				it->server_first = "r=" + combinedNonce + ",s=" + b64encode( it->scram.salt.data(), it->scram.salt.size() ) + ",i=" + std::to_string( it->scram.iterations ) ;
				std::string serverFirst_b64 = b64encode( reinterpret_cast< const unsigned char* >( it->server_first.data() ), it->server_first.size() ) ;
				MyUplink->XReply( theServer, Routing, "SASL " + serverFirst_b64 ) ;
				it->state = SaslState::SERVER_FIRST ;
				it->credentials.clear() ;
				return true ;
				}
			else if( it->state == SaslState::SERVER_FIRST )
				{
				auto bufferOpt = b64decode( it->credentials, nullptr, true ) ;
				if( !bufferOpt )
					{
					elog << "[SCRAM] Failed to decode client final message: " << it->credentials << endl ;
					incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
					doXResponse( theServer, Routing, "Invalid credentials", true ) ;
					saslRequests.erase( it ) ;
					return false ;
					}

				string decodedString = std::string( reinterpret_cast< char* >( bufferOpt->data() ), bufferOpt->size() ) ;
				if( decodedString.find("n,,") == 0 )
    				decodedString = decodedString.substr( 3 ) ;

				size_t p_pos = decodedString.rfind( ",p=" ) ;
				if( p_pos == std::string::npos )
					{
					elog << "[SCRAM] Missing ,p= in client final message: " << decodedString << endl ;
					incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
					doXResponse( theServer, Routing, "Invalid credentials", true ) ;
					saslRequests.erase( it ) ;
					return false ;
					}
				it->client_final = decodedString.substr(0, p_pos) ; // up to but not including ",p="

				std::string cbind_input, nonce, proof ;
				StringTokenizer st( decodedString, ',' ) ;
				for( size_t i = 0 ; i < st.size() ; ++i )
					{
					if( st[i].rfind("c=", 0 ) == 0 )
						cbind_input = st[i].substr( 2 ) ;
					else if( st[i].rfind("r=", 0 ) == 0 )
						nonce = st[i].substr( 2 ) ;
					else if( st[i].rfind("p=", 0 ) == 0 )
						proof = st[i].substr( 2 ) ;
					}

				if( cbind_input.empty() || nonce.empty() || proof.empty() )
					{
					elog << "[SCRAM] Missing c=, r= or p= in client final message: " << decodedString << endl ;
					incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
					doXResponse( theServer, Routing, "Invalid credentials", true ) ;
					saslRequests.erase( it ) ;
					return false ;
					}

				if( nonce != it->client_nonce + it->server_nonce )
					{
					elog << "[SCRAM] nonce match failed: " << decodedString << endl ;
					incStat("SASL." + saslMechanismToString( it->mechanism ) + ".FAILED" ) ;
					doXResponse( theServer, Routing, "AUTHENTICATION FAILED as " + it->username, true ) ;
					saslRequests.erase( it ) ;
					return false ;
					}

				std::string auth_message = it->client_first + "," + it->server_first + "," + it->client_final ;
				bool valid = validate_scram_sha256_proof(
					it->scram.storedKey,
					auth_message,
					proof
				) ;

				if( !valid )
					{
					elog << "[SCRAM] Proof validation failed for user " << it->username << endl ;
					incStat("SASL." + saslMechanismToString( it->mechanism ) + ".FAILED" ) ;
					doXResponse( theServer, Routing, "AUTHENTICATION FAILED as " + it->username, true ) ;
					saslRequests.erase( it ) ;
					return false ;
					}

				std::string server_signature = compute_server_signature( it->scram.serverKey, auth_message ) ;
				std::string server_final = "v=" + server_signature ;
				std::string server_final_b64 = b64encode( reinterpret_cast< const unsigned char* >( server_final.data() ), server_final.size() ) ;
				MyUplink->XReply( theServer, Routing, "SASL " + server_final_b64 ) ;
				it->state = SaslState::COMPLETE ;
				return true ; // Waiting for the + to complete the authentication
				}
			else if( it->state == SaslState::COMPLETE )
				{
				/* Pass through to authentication. */
				}
			else
				{
				elog << "[SCRAM] Invalid state in authentication." << endl ;
				incStat("SASL." + saslMechanismToString( it->mechanism ) + ".ERROR" ) ;
				doXResponse( theServer, Routing, "An error occurred", true ) ;
				saslRequests.erase( it ) ;
				return false ;
				}
			}

		AuthStruct auth = {
			AuthType::XQUERY,
			AUTH_ERROR,			// result (placeholder)
			it->username,		// username
			it->password,		// password/token
			it->ident,			// ident (empty for LoC)
			it->ip,				// ip
			it->fingerprint,	// tls fingerprint
			nullptr,			// sqlUser (placeholder)
			it->theClient,		// iClient (nullptr for LoC)
			it->mechanism
		} ;

		if( it->mechanism == SaslMechanism::SCRAM_SHA_256 )
			elog << "Authenticating with SCRAM" << endl;
		else
			elog << "Authenticating with username " << it->username << " and password " << mask(it->password) << endl ;
			
		AuthResult auth_res = authenticateUser( auth ) ;
		auth.result = auth_res ;

		/* Process result. */
		if( auth_res == AUTH_SUCCEEDED )
			{
			incStat("SASL." + saslMechanismToString( it->mechanism ) + ".SUCCESS" ) ;
			if( !auth.theClient )
				doXResponse( theServer, Routing, auth.theUser->getUserName() + ":" +
								std::to_string( auth.theUser->getID() ) + ":" +
								std::to_string( makeAccountFlags( auth.theUser ) ) +
								(auth.theUser->getFlag( sqlUser::F_AUTOHIDE ) ? " +x" : "") ) ;
			else
				{
				doCommonAuth( auth.theClient, auth.theUser->getUserName() ) ;
				doXResponse( theServer, Routing, string() ) ;
				}

			elog    << "cservice::doXQSASL: "
					<< "Succesful auth for "
					<< it->username
					<< endl ;

			saslRequests.erase( it ) ;
			return true;
			}

		/* The authentication failed. Process and send correct message. */
		string AuthResponse ;
		processAuthentication( auth, &AuthResponse ) ;

		/* Send response. */
		doXResponse( theServer, Routing, AuthResponse, true ) ;

		incStat("SASL." + saslMechanismToString( it->mechanism ) + ".FAILED" ) ;

		saslRequests.erase( it ) ;
		return false ;
    	}
	// It is the initial message consisting of the mechanism.
	else
    	{
		// Did we receive a numeric?
		iClient* tmpClient = Network->findClient( st[ 1 ] ) ;
		string IP ;
		string fingerprint ;
		string authMessage ;
		if( tmpClient )
			{
			if( st.size() < 3 )
				{
				elog << "Bogus SASL message with numeric" << endl ;
				doXResponse( theServer, Routing, "An error has occurred", true ) ;
				return false ;
				}

			IP = xIP( tmpClient->getIP() ).GetNumericIP() ;
			fingerprint = tmpClient->getTlsFingerprint().empty() ? "_" : tmpClient->getTlsFingerprint() ;
			authMessage = string_upper( st[ 2 ] ) ;
			}
		else
			{
			if( st.size() < 4 )
				{
				elog << "Bogus SASL message without numeric" << endl ;
				doXResponse( theServer, Routing, "An error has occurred", true ) ;
				return false ;
				}

			IP = st[ 1 ] ;
			fingerprint = st[ 2 ] ;
			authMessage = string_upper( st[ 3 ] ) ;
			}

		SaslMechanism mech ;
		if( !parseSaslMechanism( authMessage, mech ) )
			{
			LOG( ERROR, "Received invalid SASL mechanism: {}", authMessage ) ;
			doXResponse( theServer, Routing, "An error has occurred", true ) ;
			return false ;
			}
		
		if( mech == SaslMechanism::EXTERNAL && fingerprint == "_" )
			{
			incStat("SASL." + saslMechanismToString( mech ) + ".FAILED" ) ;
			doXResponse( theServer, Routing, "No TLS fingerprint provided", true ) ;
			return false ;
			}

		// Valid mechanism - store in cache for further use.
		elog << "Received valid SASL mechanism: " << authMessage << " from IP: " << IP << endl;
		SaslRequest newRequest ;
		newRequest.theClient = tmpClient ;
		newRequest.routing = Routing ;
		newRequest.theServer = theServer ;
		newRequest.ip = IP ;
		newRequest.added_ts = currentTime() ;
		newRequest.last_ts = currentTime() ;
		newRequest.fingerprint = fingerprint == "_" ? "" : fingerprint ;
		newRequest.mechanism = mech ;
		newRequest.ident = tmpClient ? tmpClient->getUserName() : "" ;

		saslRequests.push_back( newRequest ) ;

		MyUplink->XReply( theServer, Routing, "SASL +" ) ;
	}
#endif // HAVE_LIBSSL
    return true;
}

bool cservice::doXQLogin(iServer* theServer, const string& Routing, const string& Message)
{
	//What's going to be in Message?
	// AB XQ Az iauth:15_d :LOGIN Admin temPass
	// AB XQ Az iauth:15_d :LOGIN2 <ip-addr> <hostname> <ident> <username> <accountname password [totptoken]>
	// elog << "cservice::doXQLogin: Routing: " << Routing << " Message: " << Message << "\n";
	StringTokenizer st( Message );
	string username;
	string password;
	string ip;
	string hostname;
	string ident;

	if (st[0] == "LOGIN")
	{
		if (st.size() < 3)
		{
			LOG( TRACE, "XQ-LOGIN insufficient parameters" ) ;
			doXResponse(theServer, Routing, locMessage.c_str(), true);
			return false;
		}
		username = st[1];
		if (username.compare(0,1,":") == 0)
			username.erase(0,1);
		password = st.assemble(2);
		LOG( TRACE, "XQ-LOGIN: LOGIN {} {}", username, mask(password) ) ;
	}
	if (st[0] == "LOGIN2")
	{
		if (st.size() < 6)
		{
			LOG( TRACE, "XQ-LOGIN2 insufficient parameters" ) ;
			doXResponse(theServer, Routing, locMessage.c_str(), true);
			return false;
		}
		username = st[4];
		if (username.compare(0,1,":") == 0)
			username.erase(0,1);
		password = st.assemble(5);
		ip = st[1];
		hostname = st[2];
		ident = st[3];
		LOG( TRACE, "XQ-LOGIN2: LOGIN2 {} {} {} {} {}", ip, hostname, ident, username, mask(password) ) ;
	}

	AuthStruct auth = {
		AuthType::XQUERY, 	// auth type
		AUTH_ERROR,			// result (placeholder)
		username,			// username
		password,			// password/token
		ident,				// ident
		ip,					// ip
		string(),			// tls fingerprint
		nullptr,			// sqlUser (placeholder)
		nullptr,			// iClient (not in use for LoC)
		SaslMechanism::NO_SASL
	} ;

	AuthResult auth_res = authenticateUser( auth ) ;
	auth.result = auth_res ;

	/* Process result. */
	if( auth_res == AUTH_SUCCEEDED )
		{
		incStat( "LOC." + st[0] + ".SUCCESS" ) ;
		doXResponse(theServer, Routing, auth.theUser->getUserName() + ":" +
						std::to_string(auth.theUser->getID()) + ":" +
						std::to_string(makeAccountFlags(auth.theUser)) +
							(auth.theUser->getFlag( sqlUser::F_AUTOHIDE ) ? " +x" : "") ) ;
		elog    << "cservice::doXQLogin: "
				<< "Succesful auth for "
				<< username
				<< endl;
		return true;
		}

	/* The authentication failed. Process and send correct message. */
	string AuthResponse;
	processAuthentication(auth, &AuthResponse);

	/* Send response. */
	doXResponse(theServer, Routing, AuthResponse, true);
	incStat( "LOC." + st[0] + ".FAILED" ) ;

	return true;
}

void cservice::doXQToAllServices(const string& Routing, const string& Message)
{
for( const auto& [_, theServer] : Network->servers() )
	{
	if( !theServer->isService() )
		continue ;

	MyUplink->XQuery( theServer, Routing, Message ) ;
	}
}

bool cservice::doXQIsCheck(iServer* theServer, const string& Routing, const string& Command, const string& Message)
{
// AB XQ Az AnyCServiceRouting :ISUSER <user#> <user#> <user#...N>
// AB XQ Az AnyCServiceRouting :ISUSER +<username1> <username2> <username... N>
// AB XQ Az AnyCServiceRouting :ISCHAN <#chan> <#chan2> <#chan..N>
LOG( TRACE, "XQ-ISCHECK: Command: {} Routing: {} Message: {}", Command, Routing, Message ) ;
StringTokenizer st( Message ) ;

if( st.size() < 2 )
	return false ;

StringTokenizer st2( st.assemble( 1 ) ) ;

if( Command == "ISUSER")
	{
	std::stringstream theQuery;
	theQuery << "SELECT id,user_name,flags FROM users WHERE " ;

	/* Lookup usernames. */
	if( st2[ 0 ][ 0 ] == '+')
		{
		theQuery << "LOWER(user_name) IN ("
			 << "'" << string_lower( st2[ 0 ].substr( 1 ) ) << "'" ;

		if( st2.size() > 1 )
			theQuery << "," ;

		for( size_t i = 1 ; i < st2.size() ; ++i )
			{
			theQuery << "'" << string_lower( st2[ i ] ) << "'" ;

			if( i < st2.size() - 1 )
				theQuery << "," ;
			}
		}
	else
		{
		theQuery << "id IN (" ;

		for( size_t i = 0 ; i < st2.size() ; ++i )
			{
			theQuery << st2[ i ] ;

			if( i < st2.size() - 1 )
				theQuery << "," ;
			}
		}

	theQuery << ") " << std::endl ;

	if( !SQLDb->Exec( theQuery, true ) )
		{
		LOGSQL_ERROR( SQLDb ) ;
		return false ;
		}

	/* Store matching users from the SQL results */
	std::map< std::string, std::pair< std::string, std::string > > retUsers ;
	for( size_t i = 0 ; i < SQLDb->Tuples() ; i++ )
		retUsers[ SQLDb->GetValue( i, 0 ) ] = std::make_pair( SQLDb->GetValue( i, 1 ),
		std::to_string( makeAccountFlags( SQLDb->GetValue( i, 2 ) ) ) ) ;

	std::string trueString ;
	std::string falseString ;

	for( const std::string& id : st2 )
		{
		std::string user ;
		if( id[ 0 ] == '+' )
			user = id.substr( 1 ) ;
		else
			user = id ;

		/* Declare iterator. */
		auto it = retUsers.end() ;

		if( st2[ 0 ][ 0 ] == '+' )
			it = std::find_if( retUsers.begin(), retUsers.end(),
					[ &user ]( const std::pair<std::string, std::pair<std::string, std::string>>& pair )
					{ return string_lower( pair.second.first ) == string_lower( user ) ; } ) ;
		else
			it = retUsers.find( user ) ;

		if( it != retUsers.end() )
			{
			/* Flush buffer. */
			if( trueString.length() + it->first.length() + it->second.first.length() + it->second.second.length() + 2 > 450 )
				{
				const string theMessage = Command + " YES " + trueString ;
				MyUplink->XReply( theServer, Routing, theMessage ) ;
				trueString.clear() ;
				}

			/* Add to buffer. */
			trueString += it->second.first  + ":" + it->first + ":" + it->second.second + " " ;
			}
		else
			{
			/* We use 'id' here as we want to return the + (if any) */

			/* Flush buffer. */
			if( falseString.length() + id.length() + 1 > 450 )
				{
				const string theMessage = Command + " NO " + falseString ;
				MyUplink->XReply( theServer, Routing, theMessage ) ;
				}

			/* Add to buffer. */
			falseString += id + " " ;
			}
		}

	/* Send messages not already flushed. */
	if( !trueString.empty() )
		{
		const string theMessage = Command + " YES " + trueString ;
		MyUplink->XReply( theServer, Routing, theMessage ) ;
		}

	if( !falseString.empty() )
		{
		const string theMessage = Command + " NO " + falseString ;
		MyUplink->XReply( theServer, Routing, theMessage ) ;
		}

	MyUplink->XReply( theServer, Routing, Command + " EOL" ) ;

	return true ;
	}
else if ( Command == "ISCHAN" )
	{
	/* Create the SQL query string to check for registered channels and fetch managers' userID.
	 * It can be multiple managers. */
	std::stringstream theQuery ;
	theQuery	<< "SELECT channels.name, STRING_AGG(levels.user_id::TEXT, ',') AS managers "
			<< "FROM channels "
			<< "JOIN levels ON channels.id = levels.channel_id "
			<< "WHERE levels.access = 500 AND LOWER(channels.name) IN (" ;
	for( size_t i = 0 ; i < st2.size() ; ++i )
		{
		theQuery << "LOWER('" << st2[ i ] << "')" ;
		if( i < st2.size() - 1 )
			theQuery << "," ;
		}
	theQuery << ") GROUP BY channels.name" << std::endl ;

	if( !SQLDb->Exec( theQuery, true ) )
		{
		LOG( ERROR, "ISCHAN SQL Error:") ;
		LOGSQL_ERROR( SQLDb ) ;
		return false ;
		}

	/* Store registered channels and the manager uids from the SQL results */
	std::map< std::string, std::string, noCaseCompare > foundChannels ;
	for( size_t i = 0 ; i < SQLDb->Tuples() ; i++ )
		foundChannels[ SQLDb->GetValue( i, 0 ) ] = SQLDb->GetValue( i, 1 ) ;

	/* Create output strings for matching and non-matching channels */
	std::string trueString ;
	std::string falseString ;

	for( const std::string& chan : st2 )
		{
		auto it = foundChannels.find( chan ) ;
		if( it != foundChannels.end() )
			{
			/* Flush buffer. */
			if( trueString.length() + chan.length() + ( it->second ).length() + 2 > 450 )
				{
				const string theMessage = Command + " YES " + trueString ;
				MyUplink->XReply( theServer, Routing, theMessage ) ;
				trueString.clear() ;
				}

			/* Add to buffer. */
			trueString += it->first + ":" + it->second + " " ;
			}
		else
			{
			/* Flush buffer. */
			if( falseString.length() + chan.length() + 1 > 450 )
				{
				const string theMessage = Command + " NO " + falseString ;
				MyUplink->XReply( theServer, Routing, theMessage ) ;
				falseString.clear() ;
				}

			/* Add to buffer. */
			falseString += chan + " " ;
			}
		}

	/* Send messages not already flushed. */
	if( !trueString.empty() )
		{
		const string theMessage = Command + " YES " + trueString ;
		MyUplink->XReply( theServer, Routing, theMessage ) ;
		}

	if( !falseString.empty() )
		{
		const string theMessage = Command + " NO " + falseString ;
		MyUplink->XReply( theServer, Routing, theMessage ) ;
		}

	MyUplink->XReply( theServer, Routing, Command + " EOL" ) ;

	return true ;
	}

return false ;
}

bool cservice::doXQOplist(const string& chanName)
{
	LOG_MSG( TRACE, "Executing XQ for {chan}" )
		.with( "chan", chanName )
		.logStructured() ;
	//iServer* theServer = this->MyUplink->Uplink();
	iServer* chanfixServer = Network->findServerName(ChanfixServerName);
	if (!chanfixServer)
	{
		LOG( ERROR, "chanfixServer({}) NOT Found!", ChanfixServerName ) ;
		return false;
	}
//	else
//		elog << "<DEBUG> chanfixServer(" << ChanfixServerName << ") found" << endl;
	string Message = "OPLIST " + chanName;
	// AB XQ Az iauth:15_d :OPLIST #empfoo
	//elog << "cservice::doXQOplist: Routing: " << Routing << " Message: " << Message << "\n";
	return Write("%s XQ %s %s :%s", getCharYY().c_str(), chanfixServer->getCharYY().c_str(), "AnyCServiceRouting", Message.c_str());
}

bool cservice::doXROplist(iServer* /*theServer*/, const string& Routing, const string& Message)
{
	// AB XR Az iauth:15_d :OPLIST #empfoo
	LOG( TRACE, "XQ-OPLIST: Routing: {} Message: {}", Routing, Message ) ;
	StringTokenizer st(Message);

	if (st[2] == "NO")
	{
		LOG( TRACE, "NO oplist reported for channel {}", st[1] ) ;
		return true;
	}

	if (st.size() < 6)
	{
		LOG( ERROR, "OPLIST insufficient response parameters" ) ;
		return false;
	}
	string scoreChan = st[1];
	string opCount = st[2];
	string score = st[3];
	string account = st[4];
	//st[5] == "--";
	string firstOpped = st[6]; //tsToDateTime(atoi(st[5]), false);
	//st[7] == "/";
	string lastOpped = st[8]; //tsToDateTime(atoi(st[6], true);
	//st[9] == "/";
	//string lastNick = st[10];	// ... but may not be online anymore

	LOG( TRACE, "OPLIST scorechan = {} opCount(rank) = {} score = {} account = {} firstOpped = {} lastOpped = {}", scoreChan, opCount, score, account, firstOpped, lastOpped ) ;

	/*	Now we do the actual work to insert scores to SQL DB
		Rules:
			1). Only do work for channels with a pending application? (we only send XQUERY for these)
			2). Only INSERT if (channel_id,user_id) pair does not exist,
			otherwise we UPDATE
	*/
	stringstream queryString;
	queryString << "SELECT name,id FROM channels,pending WHERE lower(name)='"
		<< string_lower(scoreChan)
		<< "'"
		<< " AND channels.id = pending.channel_id"
		<< ends;

	if (SQLDb->Exec(queryString, true))
	{
		if (SQLDb->Tuples() < 1)
		{
			// No rows returned - no pending record
			LOG( DEBUG, "no pending channel found: {}", scoreChan ) ;
			return false;
		}
		else {
			// Pending channel found -- have we inserted this user & chan combo before?
			sqlUser* tmpUser = getUserRecord(account);
			if (!tmpUser)
			{
				LOG( ERROR, "tmpUser with account = {} could not be found (expired or purged)", account ) ;
				return false;
			}
			unsigned int userID = tmpUser->getID();
			unsigned int chanID = atoi(SQLDb->GetValue(0, 1).c_str());
			stringstream queryString;
			queryString << "SELECT channel_id,first FROM pending_chanfix_scores WHERE user_id='"
				<< userID
				<< "' AND channel_id=(SELECT id FROM channels WHERE lower(name)='"
				<< string_lower(scoreChan)
				<< "')"
				<< ends;

			if (SQLDb->Exec(queryString, true))
			{
				stringstream updateQuery;
				if (SQLDb->Tuples() < 1)
				{
					// no rows returned -- need to INSERT
					updateQuery << "INSERT INTO pending_chanfix_scores "
						<< "(channel_id,user_id,rank,score,account,first_opped,last_opped, first) VALUES ('"
						<< chanID
						<< "', '"
						<< userID
						<< "', '"
						<< opCount
						<< "', '"
						<< score
						<< "', '"
						<< account
						<< "', '"
						<< firstOpped
						<< "', '"
						<< lastOpped
						<< "', '"
						<< "Y"
						<< "')"
						<< ends;
				}
				else {
					// rows returned -- need to UPDATE
					int rescount = (int)SQLDb->Tuples();
					rescount--;
					int chanID = atoi(SQLDb->GetValue(rescount, 0).c_str());
					char first = (char)SQLDb->GetValue(rescount, 1)[0];
					LOG( DEBUG, " *** <DEBUG> first == {}", first ) ;
					if (first == 'N')
					{
						updateQuery << "UPDATE pending_chanfix_scores SET "
							<< "rank='"
							<< opCount
							<< "', "
							<< " score='"
							<< score
							<< "', "
							<< " first_opped='"
							<< firstOpped
							<< "',"
							<< " last_opped='"
							<< lastOpped
							<< "',"
							<< " last_updated=date_part('epoch', CURRENT_TIMESTAMP)::int, "
							<< "first='N'"
							<< " WHERE user_id='"
							<< userID
							<< "' AND channel_id='"
							<< chanID
							<< "' AND first='N'"
							<< ends;
					}
					else
					{
						updateQuery << "INSERT INTO pending_chanfix_scores "
							<< "(channel_id,user_id,rank,score,account,first_opped,last_opped, first) VALUES ('"
							<< chanID
							<< "', '"
							<< userID
							<< "', '"
							<< opCount
							<< "', '"
							<< score
							<< "', '"
							<< account
							<< "', '"
							<< firstOpped
							<< "', '"
							<< lastOpped
							<< "', '"
							<< "N"
							<< "')"
							<< ends;
					}
				}

				// send to SQL
				if (!SQLDb->Exec(updateQuery, true))
					{
					LOGSQL_ERROR( SQLDb ) ;
					return false ;
					}
			} // successful query
		} // end score exist lookup query
	} // end pending chan lookup query
	return true;
}

struct autoOpData {
	unsigned int channel_id;
	unsigned int flags;
	time_t suspend_expires;
} aOp;

bool cservice::doCommonAuth(iClient* theClient, string username)
{
	bool LoC = false;
	sqlUser* theUser;
	theUser = getUserRecord(theClient->getAccount());
	if (theUser)
		LoC = true;
	else
		theUser = getUserRecord(username);
	if (!theUser)
	{
		//This case shouldn't be happen
		LOG( ERROR, "Unknown user: {}", username ) ;
		return false;
	}
	/*
	 * If this user account is already authed against, send a notice to the other
	 * users warning them that someone else has logged in too.
	 */
	bool multiAuth = theUser->isAuthed();
	if (multiAuth)
	{
		noticeAllAuthedClients(theUser,
		"%s has just authenticated as you (%s). "
		"If this is not you, your account may have been compromised. "
		"If you wish to suspend all your access as a precautionary measure, "
		"type '\002/msg %s@%s suspendme <password>'\002 and contact a "
		"CService representative to resolve the problem."
		" \002** Note: You will NOT be able to use your account after "
		"you issue this command **\002",
			theClient->getNickUserHost().c_str(),
			theUser->getUserName().c_str(),
			getNickName().c_str(),
			getUplinkName().c_str());
	}

	/* check to see if we have a last seen time (bug workaround) - if not, make one */
	stringstream queryString;
	queryString	<< "SELECT last_seen FROM users_lastseen WHERE user_id="
			<< theUser->getID()
			<< ends;

	if( SQLDb->Exec(queryString, true ) )
	{
		if (SQLDb->Tuples() < 1)
		{
			/* no rows returned - create a dummy record that will be updated
			 * by setLastSeen after this loop
			 */
			stringstream updateQuery;
			updateQuery	<< "INSERT INTO users_lastseen (user_id,"
					<< "last_seen,last_updated) VALUES("
					<< theUser->getID()
					<< ",date_part('epoch', CURRENT_TIMESTAMP)::int,date_part('epoch', CURRENT_TIMESTAMP)::int)"
					<< ends;

			SQLDb->Exec(updateQuery);
		}
	}
	/* update their details */
	theUser->setLastSeen(currentTime(), theClient->getRealNickUserHost(), xIP( theClient->getIP()).GetNumericIP());
	theUser->setFlag(sqlUser::F_LOGGEDIN);

	theUser->addAuthedClient(theClient);

	//But I think the sqlUser data must be assigned to the customData structure in any case! --Seven
	networkData* newData =
		static_cast< networkData* >( theClient->getCustomData(this) ) ;
	if( NULL == newData )
		{
		Notice( theClient,
			"Internal error." ) ;
		LOG_MSG( ERROR, "newData is NULL for: {client_nick}" )
			.with( "client", theClient )
			.logStructured() ;
		return false ;
		}

	// Pointer back to the sqlUser from this iClient.
	newData->currentUser = theUser;

	// Everything's okay, push the data to the security history
	InsertUserHistory(theClient, "LOGIN");

	Notice(theClient,
		getResponse(theUser, language::auth_success).c_str(),
		theUser->getUserName().c_str());

	/* set failed logins counter back to zero after alerting the user (if required) */
	if (getConfigVar("ALERT_FAILED_LOGINS")->asInt()==1 &&
		theUser->getFailedLogins() > 0)
	{
		/* ok, alert the user */
		Notice(theClient,
			getResponse(theUser,
				language::auth_failed_logins,
				string("There were %d failed login attempts since your last successful login.")).c_str(),
				theUser->getFailedLogins());
		//theUser->setFailedLogins(0);
		//theUser->setLastFailedLoginTS(0);
	}
	theUser->setFailedLogins(0);
	theUser->setLastFailedLoginTS(0);

	if (multiAuth) {
	        Notice(theClient, getResponse(theUser,language::following_clients_auth,"The following clients are also authenticated as %s:").c_str(),theUser->getUserName().c_str());

	        for( sqlUser::networkClientListType::iterator ptr = theUser->networkClientList.begin() ;
	                ptr != theUser->networkClientList.end() ; ++ptr )
	                {
	                iClient* Target = (*ptr);
			if(Target != theClient) {
		                Notice(theClient,"    %s",Target->getNickUserHost().c_str());
			}
	                }
	        Notice(theClient,getResponse(theUser,language::suspend_if_not_you,"if any of these clients are not you, your account may have been compromised. "
	        "If you wish to suspend all your access as a precautionary measure, "
	        "type '\002/msg %s@%s suspendme <password>'\002 and contact a "
	        "CService representative to resolve the problem.").c_str(),  getNickName().c_str(),getUplinkName().c_str());
	        Notice(theClient,getResponse(theUser,language::account_unusable_suspendme,"\002** Note: You will NOT be able to use your account after "
	        "you issue this command **\002").c_str());
	}

	int tmpLevel = getAdminAccessLevel(theUser);
	if (tmpLevel > 0)
	{
		/* this is a privileged user, send a notice to _info */
		if (tmpLevel < 900)
		{
			/* regular admin, report to normal relay channel */
			logAdminMessage("%s (%s) has authenticated %sat level %d",
				theClient->getNickName().c_str(),
				theUser->getUserName().c_str(),
				theUser->getFlag(sqlUser::F_TOTP_ENABLED) ? "with TOTP " : "",
				tmpLevel);
		} else {
			/* if this is a high level admin, report to privileged relay channel */
			logPrivAdminMessage("%s (%s) has authenticated %sat level %d)",
				theClient->getNickName().c_str(),
				theUser->getUserName().c_str(),
				theUser->getFlag(sqlUser::F_TOTP_ENABLED) ? "with TOTP " : "",
				tmpLevel);
		}
	} else {
		string greeting = getResponse(theUser, language::greeting);
		if (!greeting.empty())
		{
			Notice(theClient, greeting.c_str());
		}
	}

	/*
	 * Send out AC token onto the network.	-- but only on not Login on Connect case
	 * The AC Token (Account) is read by ircu2.10.11 and any other
	 * service on the net that cares and added to the Client structure.
	 * Format:
	 * [Source Server] AC [Authenticated User's Numeric] [Authenticated UserName]
	 * Eg: AX AC APAFD gte
	 */
	if (!LoC)
		this->MyUplink->UserLogin(theClient, theUser->getUserName(), theUser->getID(), makeAccountFlags(theUser), this);

#ifdef NEW_IRCU_FEATURES
	/* Set remote +x if user has AUTOHIDE set */
	if (theUser->getFlag(sqlUser::F_AUTOHIDE) && !theClient->isModeX())
		MyUplink->Write("%s OM %s :+x", getCharYY().c_str(), theClient->getCharYYXXX().c_str());
#endif
	/*
	 * If the user account has been suspended, make sure they don't get
	 * auto-opped.
	 */

	if (theUser->getFlag(sqlUser::F_GLOBAL_SUSPEND))
		{
		Notice(theClient,
			"..however your account has been suspended by a CService administrator."
			" You will be unable to use any channel access you may have.");
		return true;
		}


	/*
	 * Check they aren't banned < 75 in any chan.
	 */
	for (const auto& theChan : theClient->channels())
	{
		sqlChannel* sqlChan = getChannelRecord(theChan->getName());
		/* Do not attempt to enforce bans if I am not in the channel */
		if (sqlChan && sqlChan->getInChan())
		{
			checkBansOnJoin(theChan, sqlChan, theClient);
		}
	}


	/*
	 * The fun part! For all channels this user has access on, and has
	 * AUTOP set, and isn't already op'd on - do the deed.
	 * We're auto-opping only on not LoC case, but Auto-Invites are done in any case !!	--Seven
	 */

	stringstream theQuery;
	theQuery	<< "SELECT channel_id,flags,suspend_expires FROM "
			<< "levels WHERE user_id = "
			<< theUser->getID()
			<< ends;

	if( !SQLDb->Exec(theQuery, true ) )
		{
		LOGSQL_ERROR( SQLDb ) ;
		return false ;
		}

	typedef vector < autoOpData > autoOpVectorType;
	autoOpVectorType autoOpVector;

	for(unsigned int i = 0; i < SQLDb->Tuples(); i++)
		{
		autoOpData current;

		current.channel_id = atoi(SQLDb->GetValue(i, 0));
		current.flags = atoi(SQLDb->GetValue(i, 1));
		current.suspend_expires = atoi(SQLDb->GetValue(i, 2));

		autoOpVector.push_back( autoOpVectorType::value_type(current) );
		}

	for (autoOpVectorType::const_iterator resultPtr = autoOpVector.begin();
		resultPtr != autoOpVector.end(); ++resultPtr)
		{

		/* If the autoop flag isn't set in this record */
		if (!(resultPtr->flags & sqlLevel::F_AUTOOP) &&
			!(resultPtr->flags & sqlLevel::F_AUTOVOICE) &&
			!(resultPtr->flags & sqlLevel::F_AUTOINVITE))
			{
			continue;
			}

		sqlChannel* theChan = getChannelRecord(resultPtr->channel_id);
		if (!theChan)
			{
			continue;
			}

		/*
		 * Make sure the channel isn't suspended..
		 */

		if (theChan->getFlag(sqlChannel::F_SUSPEND))
			{
			continue;
			}


		/*
		 * Check if they're already opped.
		 */

		Channel* netChan = Network->findChannel(theChan->getName());
		if (!netChan)
			{
			continue;
			}


		/*
		 * Don't attempt to op/invite if we're not in the channel, or not op'd.
		 */

		ChannelUser* tmpBotUser = netChan->findUser(getInstance());
		if (!tmpBotUser)
			{
			continue;
			}

		if (!theChan->getInChan() || !tmpBotUser->getMode(ChannelUser::MODE_O))
			{
			continue;
			}

		/*
		 *  Would probably be wise to check they're not suspended too :)
		 *  (*smack* Ace)
		 */

		if(resultPtr->suspend_expires > currentTime() )
			{
			continue;
			}

		ChannelUser* tmpChanUser = netChan->findUser(theClient) ;
		if(!tmpChanUser) {
			//The user is not in the channel, lets see if their autoinvite is on and act upon it
			if (resultPtr->flags & sqlLevel::F_AUTOINVITE)  {
				int level = getEffectiveAccessLevel(theUser, theChan, true);
				if(level >= level::invite) {
					sqlBan* tmpBan = isBannedOnChan(theChan, theClient);
					if (tmpBan && tmpBan->getLevel() >= 75) {
						Notice(theClient, "Can't invite you to channel %s, you are banned", theChan->getName().c_str());
						continue;
					}
					Invite(theClient,netChan);
				}
			}
			continue;
		}

		/*
		 * Check if the channel is NOOP.
		 * N.B: If the channel is strictop, we op them.
		 * They've just logged in! :P
		 */

		if(theChan->getFlag(sqlChannel::F_NOOP))
			{
			continue;
			}

		/*
	 	 *  If its AUTOOP, check for op's and do the deed.
		 *  Otherwise, its just AUTOVOICE :)
		 */

		if (resultPtr->flags & sqlLevel::F_AUTOOP)
			{
			if(!tmpChanUser->getMode(ChannelUser::MODE_O))
				{
				Op(netChan, theClient);
				}
			}
		else if (resultPtr->flags & sqlLevel::F_AUTOVOICE)
			{
			if ((!tmpChanUser->getMode(ChannelUser::MODE_V)) && (!theChan->getFlag(sqlChannel::F_NOVOICE)))
				{
				Voice(netChan, theClient);
				}
			}
		}

	/*
	 *  And last but by no means least, see if we have been nominated as
	 *  a supporter for any channel(s).
	 */

	stringstream supporterQuery;
	supporterQuery	<< "SELECT channels.name,pending.manager_id,channels.id FROM"
				<< " supporters,channels,pending WHERE"
				<< " supporters.channel_id = channels.id"
				<< " AND pending.channel_id = channels.id"
				<< " AND channels.registered_ts = 0"
				<< " AND supporters.support = '?'"
				<< " AND pending.status = 0"
				<< " AND user_id = "
				<< theUser->getID()
				<< ends;

	if( !SQLDb->Exec(supporterQuery, true ) )
		{
		LOGSQL_ERROR( SQLDb ) ;
		return false ;
		}

	if (SQLDb->Tuples() == 1)
	{
		string channelName = SQLDb->GetValue(0, 0);
		string mngrUserName = string();
		//int chanId = atoi(SQLDb->GetValue(0, 2).c_str());
		sqlUser* managerUser = getUserRecord(atoi(SQLDb->GetValue(0, 1).c_str()));
		if (managerUser)
			mngrUserName = managerUser->getUserName();
		Notice(theClient, "You have been named as a supporter in a "
			"new channel application for %s (by %s). You may visit the "
			"website to register your support or to make an "
			"objection. Alternatively, you can"
			" type '\002/msg %s support %s YES\002' or '\002/msg %s "
			"support %s NO\002' to confirm or deny your support.",
			channelName.c_str(),
			mngrUserName.c_str(),
			getNickName().c_str(),
			channelName.c_str(),
			getNickName().c_str(),
			channelName.c_str());
			//setSupporterNoticedStatus(theUser->getID(), chanId, true);
			setSupporterNoticedStatus(theUser->getID(), channelName, true);
	}
	else if (SQLDb->Tuples() > 1)
	{
		stringstream noticeStr;
		AppDataListType appList;
		for (unsigned int i = 0; i < SQLDb->Tuples(); i++)
		{
			AppData current;
			current.chanName = SQLDb->GetValue(i, 0);
			current.mngrUserId = atoi(SQLDb->GetValue(0, 1).c_str());
			current.chanId = atoi(SQLDb->GetValue(0, 2).c_str());
			//Debug Proof elogs ...
			//elog << "cservice::doCommonLogin>   current.chanId = " << current.chanId << endl;
			//elog << "cservice::doCommonLogin> current.chanName = " << current.chanName << endl;
			appList.push_back(current);
		}
		for (AppDataListType::const_iterator itr = appList.begin(); itr != appList.end(); ++itr)
		{
			string mngrUserName = string();
			string channelName = itr->chanName;
			sqlUser* managerUser = getUserRecord(itr->mngrUserId);
			if (managerUser)
				mngrUserName = managerUser->getUserName();
			if (!noticeStr.str().empty())
				noticeStr << ", ";
			noticeStr << channelName << " (" << mngrUserName << ")";
			// Debug Proof elogs ...
			//elog << "cservice::doCommonLogin>  itr->chanId = " << itr->chanId << endl;
			//elog << "cservice::doCommonLogin> itr->chanName = " << channelName << endl;
			//Not works with chanId ... (unknown reason)
			//setSupporterNoticedStatus(theUser->getID(), chanId, true);
			setSupporterNoticedStatus(theUser->getID(), channelName, true);
		}
		noticeStr << ends;
		Notice(theClient, "You have been named as a supporter for the following "
			"new channel applications: %s", noticeStr.str().c_str());
		Notice(theClient, "You may visit the "
			"website to register your support or to make an "
			"objection. Alternatively, you can"
			" type '\002/msg %s support <#chan> <YES|NO>\002' "
			"to confirm or deny your support for each channel.",
			getNickName().c_str());
	}

	/*
	 * See if they have any notes.
	 */
	#ifdef USE_NOTES

	if(!theUser->getFlag(sqlUser::F_NONOTES))
	        {
	        stringstream noteQuery;
	        noteQuery       << "SELECT message_id FROM notes "
	                        << "WHERE user_id = "
	                        << theUser->getID()
	                        << ends;

	        SQLDb->Exec(noteQuery, true) ;

	        unsigned int count = SQLDb->Tuples();
	        if(count)
	                {
	                Notice(theClient, "You have %i note(s). To read "
	                        "them type /msg %s notes read all",
	                        count,
	                        getNickName().c_str());
	                }
	        }

	#endif

	/*
	 * Send out and flush saved 'notices'
	 */
		stringstream noticeQuery;
		noticeQuery	<< "SELECT message FROM notices "
				<< "WHERE user_id = "
				<< theUser->getID()
				<< " ORDER BY last_updated ASC"
				<< ends;

		if (!SQLDb->Exec(noticeQuery, true))
			{
			LOG( ERROR, "Something went wrong when loading notes:" ) ;
			LOGSQL_ERROR( SQLDb ) ;
			return false;
			}

		unsigned int count = SQLDb->Tuples();
		if(count)
		    for (unsigned int i=0; i<count; i++)
			Notice(theClient,SQLDb->GetValue(i,0).c_str());

	 /* Delete automatically the notices */

		noticeQuery.str("");
		noticeQuery	<< "DELETE FROM notices WHERE user_id = "
				<< theUser->getID()
				<< ends;

		if (!SQLDb->Exec(noticeQuery, true))
			{
			LOG( ERROR, "Something went wrong when deleting notes:" ) ;
			LOGSQL_ERROR( SQLDb ) ;
			return false;
			}

	return true;
}

bool cservice::doXResponse(iServer* theServer, const string& Routing, const string& Message, bool kill)
{
	string doKill;
	if (kill) doKill="NO"; else doKill="OK";	
	LOG( TRACE, "XQ-RESPONSE: {} XR {} {} :{} {}", getCharYY().c_str(), theServer->getCharYY().c_str(), Routing.c_str(), doKill.c_str(), Message.c_str() ) ;
	return Write("%s XR %s %s :%s %s", getCharYY().c_str(),theServer->getCharYY().c_str(),Routing.c_str(),kill == false ? "OK" : "NO", Message.c_str());
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

	string chanName ;
	string chanAccess ;
	unsigned int flags;

	if( !SQLDb->Exec(channelsQuery, true ) )
		{
		Notice( theClient,
			"Internal error: SQL failed" ) ;

		LOGSQL_ERROR( SQLDb ) ;
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

bool cservice::addGline( csGline* TempGline)
{

glineIterator ptr;
if(TempGline->getHost().substr(0,1) == "$") //check if its a realname gline
        {
	return true;
        }
else
        {
        ptr = glineList.find(TempGline->getHost());
        if(ptr != glineList.end())
                {
                if(ptr->second != TempGline)
                        {
                        delete ptr->second;
                        glineList.erase(ptr);
                        }
                }
        glineList[TempGline->getHost()] = TempGline;

        }
return true;
}

bool cservice::remGline( csGline* TempGline)
{
if(TempGline->getHost().substr(0,1) == "$")
        {
	return true;
        }
else
        {
        glineList.erase(TempGline->getHost()) ;
        }
return true;
}

csGline* cservice::findGline( const string& HostName )
{
glineIterator ptr = glineList.find(HostName);
if(ptr != glineList.end())
        {
        return ptr->second;
        }

return NULL ;
}

bool cservice::loadGlines()
{
static const char *Main = "SELECT Id,Host,AddedBy,AddedOn,ExpiresAt,LastUpdated,Reason FROM glines";

stringstream eraseQuery;
eraseQuery	<< "DELETE FROM glines";
if( !SQLDb->Exec( eraseQuery, true ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
	return false;
	}

stringstream theQuery;
theQuery        << Main
                << ends;

if( !SQLDb->Exec( theQuery, true ) )
        {
        LOGSQL_ERROR( SQLDb ) ;
        return false;
        }

csGline *tempGline = NULL;

for( unsigned int i = 0 ; i < SQLDb->Tuples() ; i++ )
        {
        tempGline =  new (std::nothrow) csGline(this);
        assert( tempGline != NULL ) ;

        tempGline->setId(SQLDb->GetValue(i,0));
        tempGline->setHost(SQLDb->GetValue(i,1));
        tempGline->setAddedBy(SQLDb->GetValue(i,2)) ;
        tempGline->setAddedOn(static_cast< time_t >(
                atoi( SQLDb->GetValue(i,3).c_str() ) )) ;
        tempGline->setExpires(static_cast< time_t >(
                atoi( SQLDb->GetValue(i,4).c_str() ) )) ;
        tempGline->setLastUpdated(static_cast< time_t >(
                atoi( SQLDb->GetValue(i,5).c_str() ) )) ;
        tempGline->setReason(SQLDb->GetValue(i,6));
        addGline(tempGline);
        }
return true;
}

bool cservice::expireGlines()
{

int totalFound = 0;
csGline * tempGline;
list<string> remList;
list<string>::iterator remIterator;
for(glineIterator ptr = glineList.begin();ptr != glineList.end();++ptr)
        {
        tempGline = ptr->second;
        if((tempGline->getExpires() <= ::time(0))
            && ((tempGline->getHost().substr(0,1) != "#") ||
            (tempGline->getExpires() != 0)))

                {
                //remove the gline from the database
                tempGline->Delete();
                remList.push_back(ptr->first);
//              ptr = glineList.erase(ptr);
                delete tempGline;
                ++totalFound;
                }
        }

for(remIterator = remList.begin();remIterator != remList.end();)
        {
        glineList.erase(*remIterator);
        remIterator = remList.erase(remIterator);
        }

if(totalFound > 0)
	LOG( TRACE, "Expired {} glines", totalFound ) ;
return true;

}

bool cservice::expireWhitelist()
{
stringstream whitelistQuery;
whitelistQuery	<< "DELETE FROM whitelist WHERE "
		<< "expiresat <= date_part('epoch', CURRENT_TIMESTAMP)::int AND "
		<< "expiresat != 0" << ends;

if( !SQLDb->Exec(whitelistQuery, true ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
	return false;
	}
return true;
}

void cservice::addGlineToUplink(csGline* theGline)
{
	int Expires;
	if((theGline->getHost().substr(0,1) == "#")
			&& (theGline->getExpires() == 0))
	{
		Expires = 730*3600*24; //gline::PERM_TIME;
	}
	else
	{
		Expires = theGline->getExpires() - time(0);
	}
	MyUplink->setGline(theGline->getAddedBy()
    ,theGline->getHost(),theGline->getReason()
    ,Expires,theGline->getLastUpdated(),this);
}

bool cservice::InsertUserHistory(iClient* theClient, const string& command)
{
	stringstream theQuery;
	sqlUser* theUser = isAuthed(theClient, false);
	if (!theUser)
	{
		LOG( WARN, "theUser not found for {} ({})",
			theClient->getNickUserHost(), theClient->getAccount() ) ;
		return false;
	}
	theQuery << "INSERT INTO user_sec_history ("
			<< "user_id, user_name, command, ip, hostmask, timestamp) VALUES ("
			<< theUser->getID() << ", '"
			<< escapeSQLChars(theUser->getUserName()) << "', '"
			<< escapeSQLChars(string_upper(command)) << "', '"
			<< escapeSQLChars(xIP(theClient->getIP()).GetNumericIP()) << "', '"
			<< escapeSQLChars(theClient->getRealNickUserHost()) << "', "
			<< "date_part('epoch', CURRENT_TIMESTAMP)::int)"
			<< ends;

	if (!SQLDb->Exec(theQuery))
		{
		LOGSQL_ERROR( SQLDb ) ;
		return false;
		}

	return true;
}

/**
 * This method translates the sqlUser flags of a sqlUser into account flags,
 * and if there are changes, sends a new AC message for each iClient authed
 * as the relevant sqlUser.
 */
void cservice::sendAccountFlags( sqlUser* theUser ) const
{
for( sqlUser::networkClientListType::iterator ptr = theUser->networkClientList.begin() ;
	ptr != theUser->networkClientList.end() ; ++ptr )
	sendAccountFlags( theUser, *ptr ) ;
}

/**
 * This method translates the sqlUser flags of a sqlUser into account flags,
 * and if there are changes, sends a new AC message for the iClient.
 */
void cservice::sendAccountFlags( sqlUser* theUser, iClient* theClient ) const
{
const iClient::flagType newFlags = makeAccountFlags( theUser ) ;

if( theClient->getAccountFlags() == newFlags )
	return ;

#ifdef NEW_IRCU_FEATURES
MyUplink->Write( "%s AC %s %s %u %u",
	getCharYY().c_str(), theClient->getCharYYXXX().c_str(), theClient->getAccount().c_str(),
	theClient->getAccountID(), newFlags ) ;
#endif

theClient->setAccountFlags( newFlags ) ;

MyUplink->PostEvent( EVT_ACCOUNT_FLAGS,
	static_cast< void* >( theClient ), 0, 0, 0,
	this ) ;
}

/* Translates the sqlUser flags into account flags. */
iClient::flagType cservice::makeAccountFlags( sqlUser* theUser ) const
{
iClient::flagType retMe = 0 ;

for( const auto& [ sqlFlag, xFlag ] : flagMap )
	if( theUser->getFlag( sqlFlag ) )
		retMe |= xFlag ;

return retMe ;
}

/* This method translates sqlUser flags into accountFlags taking the flags (as a string) as an argument. */
iClient::flagType cservice::makeAccountFlags( string flagString ) const
{
return makeAccountFlags( static_cast< sqlUser::flagType >( std::stoul( flagString ) ) ) ;
}

/* This method translates sqlUser flags into accountFlags taking the flags as an argument. */
iClient::flagType cservice::makeAccountFlags( sqlUser::flagType existingFlags ) const
{
iClient::flagType retMe = 0 ;

for( const auto& [ sqlFlag, xFlag ] : flagMap )
	if( existingFlags & sqlFlag )
		retMe |= xFlag;

return retMe ;
}

} // namespace gnuworld
