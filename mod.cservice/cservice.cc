#include	<vector>
#include	<iostream>
#include	<strstream>
#include	<string>

#include	<ctime>
#include	<cstdlib>
 
#include	"client.h" 
#include  	"cservice.h"
#include	"EConfig.h"
#include	"events.h"
#include	"ip.h"
#include	"Network.h"
#include	"StringTokenizer.h"
#include	"misc.h"
#include	"ELog.h" 
#include	"libpq++.h"
#include	"constants.h"
#include	"networkData.h"
#include	"levels.h"
#include	"match.h"

using std::vector ;
using std::endl ;
using std::strstream ;
using std::ends ;
using std::string ; 

namespace gnuworld
{

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

void cservice::ImplementServer( xServer* theServer )
{

	for( commandMapType::iterator ptr = commandMap.begin() ; ptr != commandMap.end() ;
	        ++ptr )
	        {
	        ptr->second->setServer( theServer ) ;
	        }
	 
 	// Attempt to register our interest in recieving NOTIFY events.
	if (SQLDb->ExecCommandOk("LISTEN channels_u; LISTEN bans_u; LISTEN users_u; LISTEN levels_u;"))
	{
		elog << "cmaster::ImplementServer> Successfully registered LISTEN event for Db updates." << endl;
		// Start the Db update timer rolling.
		time_t theTime = time(NULL) + updateInterval;
		theServer->RegisterTimer(theTime, this, NULL); 
	} else 
	{
		elog << "cmaster::ImplementServer> PostgreSQL error while attempting to register LISTEN event: " << SQLDb->ErrorMessage() << endl;
	}

	if (SQLDb->Exec("SELECT now()::abstime::int4;") == PGRES_TUPLES_OK) 
	{ 
		// Set our "Last Refresh" timers to the current database system time.
		time_t serverTime = atoi(SQLDb->GetValue(0,0));
		lastChannelRefresh = serverTime;
		lastUserRefresh = serverTime;
		lastLevelRefresh = serverTime;
		lastBanRefresh = serverTime;

		/* 
		 * Calculate the current time offset from the DB server.
		 * We always want to talk in DB server time. 
		 */ 

		dbTimeOffset = serverTime - ::time(NULL);
		elog << "cservice::ImplementServer> Current DB server time: " << currentTime() << endl; 
	} else {
	 	elog << "Unable to retrieve time from postgres server!" << endl;
		::exit(0);
	}
 
	// Register our interest in recieving some Network events from gnuworld.

	theServer->RegisterEvent( EVT_KILL, this );
	theServer->RegisterEvent( EVT_QUIT, this );
	theServer->RegisterEvent( EVT_NICK, this );

xClient::ImplementServer( theServer ) ;
}

cservice::cservice(const string& args)
{ 
    /*
	 *  Register command handlers.
	 */
 
    RegisterCommand(new SHOWCOMMANDSCommand(this, "SHOWCOMMANDS", "<#channel>", 3));
    RegisterCommand(new LOGINCommand(this, "LOGIN", "<username> <password>", 10)); 
    RegisterCommand(new ACCESSCommand(this, "ACCESS", "[channel] [nick] [-min n] [-max n] [-autoop] [-noautoop] [-modif [mask]]", 5));
    RegisterCommand(new CHANINFOCommand(this, "CHANINFO", "<#channel>", 3)); 
    RegisterCommand(new ISREGCommand(this, "ISREG", "<#channel>", 4)); 
    RegisterCommand(new VERIFYCommand(this, "VERIFY", "<nick>", 3));
    RegisterCommand(new SEARCHCommand(this, "SEARCH", "<keywords>", 5));
    RegisterCommand(new MOTDCommand(this, "MOTD", "", 4));
    RegisterCommand(new RANDOMCommand(this, "RANDOM", "", 4));
    RegisterCommand(new SHOWIGNORECommand(this, "SHOWIGNORE", "", 3));

    RegisterCommand(new OPCommand(this, "OP", "<#channel> [nick] [nick] ..", 3));
    RegisterCommand(new DEOPCommand(this, "DEOP", "<#channel> [nick] [nick] ..", 3));
    RegisterCommand(new VOICECommand(this, "VOICE", "<#channel> [nick] [nick] ..", 3));
    RegisterCommand(new DEVOICECommand(this, "DEVOICE", "<#channel> [nick] [nick] ..", 3));
    RegisterCommand(new ADDUSERCommand(this, "ADDUSER", "<#channel> <nick> <access>", 8));
    RegisterCommand(new REMUSERCommand(this, "REMUSER", "<#channel> <nick>", 4));
    RegisterCommand(new MODINFOCommand(this, "MODINFO", "<#channel> [ACCESS <nick> <level>] [AUTOMODE <NONE|OP|VOICE>]", 6));
    RegisterCommand(new SETCommand(this, "SET", "[#channel] <variable> <value> or, SET <invisible> <ON|OFF>", 6));
    RegisterCommand(new INVITECommand(this, "INVITE", "<#channel>", 2));
    RegisterCommand(new TOPICCommand(this, "TOPIC", "<#channel> <topic>", 4));
    RegisterCommand(new CHANINFOCommand(this, "CHANINFO", "<#channel>", 3));
    RegisterCommand(new CHANINFOCommand(this, "INFO", "<#channel>", 3));
    RegisterCommand(new BANLISTCommand(this, "BANLIST", "<#channel>", 3));
    RegisterCommand(new KICKCommand(this, "KICK", "<#channel> <nick> <reason>", 4));
    RegisterCommand(new STATUSCommand(this, "STATUS", "<#channel>", 4));
    RegisterCommand(new SUSPENDCommand(this, "SUSPEND", "<#channel> <nick> [duration]", 5));
    RegisterCommand(new UNSUSPENDCommand(this, "UNSUSPEND", "<#channel> <nick>", 5));
    RegisterCommand(new BANCommand(this, "BAN", "<#channel> <nick | *!*user@*.host> [duration] [level] [reason]", 5));
    RegisterCommand(new UNBANCommand(this, "UNBAN", "<#channel> <*!*user@*.host>", 5));
    RegisterCommand(new LBANLISTCommand(this, "LBANLIST", "<#channel>", 5));
    RegisterCommand(new NEWPASSCommand(this, "NEWPASS", "<new passphrase>", 5));

    RegisterCommand(new REGISTERCommand(this, "REGISTER", "<#channel>", 0));
    RegisterCommand(new PURGECommand(this, "PURGE", "<#channel> <reason>", 0));
    RegisterCommand(new FORCECommand(this, "FORCE", "<#channel>", 0));
    RegisterCommand(new UNFORCECommand(this, "UNFORCE", "<#channel>", 0));
    RegisterCommand(new SERVNOTICECommand(this, "SERVNOTICE", "<#channel> <text>", 0));
    RegisterCommand(new SAYCommand(this, "SAY", "<#channel> <text>", 0));

	//-- Load in our cservice configuration file. 
	cserviceConfig = new EConfig( args ) ;
	string sqlHost = cserviceConfig->Require( "sql_host" )->second;
	string sqlDb = cserviceConfig->Require( "sql_db" )->second;
	string sqlPort = cserviceConfig->Require( "sql_port" )->second;
	
	string Query = "host=" + sqlHost + " dbname=" + sqlDb + " port=" + sqlPort;

	elog << "cmaster::cmaster> Attempting to connect to " << sqlHost << "; Database: " << sqlDb << endl;
 
	SQLDb = new cmDatabase( Query.c_str() ) ;

	//-- Make sure we connected to the SQL database; if we didn't we exit entirely.

	if (SQLDb->ConnectionBad ())
	{
		elog << "cmaster::cmaster> Unable to connect to SQL server." << endl 
		     << "cmaster::cmaster> PostgreSQL error message: " << SQLDb->ErrorMessage() << endl ;

		::exit( 0 ) ;
	}
	else
	{
		elog << "cmaster::cmaster> Connection established to SQL server. Backend PID: " << SQLDb->getPID() << endl ;
	}

	//-- Retrieve user, nick, host, and description from the configuration file -- these are all REQUIRED
	//   and the program will exit unless these are present in cservice.conf!

	nickName = cserviceConfig->Require( "nick" )->second ;
	userName = cserviceConfig->Require( "user" )->second ;
	hostName = cserviceConfig->Require( "host" )->second ;
	userDescription = cserviceConfig->Require( "description" )->second ; 
	relayChan = cserviceConfig->Require( "relay_channel" )->second ; 
	updateInterval = atoi((cserviceConfig->Require( "update_interval" )->second).c_str());
	input_flood = atoi((cserviceConfig->Require( "input_flood" )->second).c_str()); 
	output_flood = atoi((cserviceConfig->Require( "output_flood" )->second).c_str());
	flood_duration = atoi((cserviceConfig->Require( "flood_duration" )->second).c_str());
	topic_duration = atoi((cserviceConfig->Require( "topic_duration" )->second).c_str());

	//-- Move this to the configuration file?  This should be fairly static..

	Mode( "+idk" ) ;

	userHits = 0;
	userCacheHits = 0;
	channelHits = 0;
	channelCacheHits = 0;
 
	// Load our translation tables.
	loadTranslationTable(); 
}

cservice::~cservice()
{
	delete cserviceConfig ;	
}

int cservice::BurstChannels()
{
	ExecStatusType status;
 
	/*
	 *   Need to join every channel with AUTOJOIN set. (But not * ;))
	 *   Various other things must be done, such as setting the topic if AutoTopic
	 *   is on, gaining ops if AlwaysOp is on, and so forth.
	 */ 
	strstream theQuery;
	theQuery << "SELECT " << sql::channel_fields << " FROM channels WHERE lower(name) <> '*' AND registered_ts <> 0 AND deleted = 0" << ends;
	elog << "cmaster::BurstChannels> " << theQuery.str() << endl; 

	if ((status = SQLDb->Exec(theQuery.str())) == PGRES_TUPLES_OK)
	{
		for (int i = 0 ; i < SQLDb->Tuples (); i++)
		{ 
 
			/* Add this information to the channel cache. */

			sqlChannel* newChan = new sqlChannel(SQLDb);
			newChan->setAllMembers(i);
			sqlChannelCache.insert(sqlChannelHashType::value_type(newChan->getName(), newChan));

			/*
			 *  Check the auto-join flag is set, if so - join. :)
			 */ 

			if (newChan->getFlag(sqlChannel::F_AUTOJOIN))
			{
				MyUplink->JoinChannel( this, 
					newChan->getName(), newChan->getChannelMode(), newChan->getChannelTS(), true ); 
				MyUplink->RegisterChannelEvent( newChan->getName(), this ) ;
				newChan->setInChan(true);
			}

			/* If neccessary, set the auto topic. */
			if (newChan->getFlag(sqlChannel::F_AUTOTOPIC))
			{
				doAutoTopic(newChan);
			}


			// Update the interal channel record to reflect the modes fetched for
			// this channel from the database?  should be doing in 'JoinChannel'.
		
	}
	}

	return xClient::BurstChannels();
}
	
int cservice::OnConnect()
{
	return 0;
}

unsigned short cservice::getFloodPoints(iClient* theClient)
{
	/*
	 * This function gets an iClient's flood points. 
	 */

	networkData* tmpData = (networkData*)theClient->getCustomData(this);
	assert(tmpData != NULL);
 
	return tmpData->flood_points;
}	

void cservice::setIgnored(iClient* theClient, bool polarity)
{
	networkData* tmpData = (networkData*)theClient->getCustomData(this);
	assert(tmpData != NULL);
	
	tmpData->ignored = polarity;
}

bool cservice::isIgnored(iClient* theClient)
{
	networkData* tmpData = (networkData*)theClient->getCustomData(this);
	assert(tmpData != NULL);
	
	return tmpData->ignored;
}	

void cservice::setFloodPoints(iClient* theClient, unsigned short amount)
{ 
	networkData* tmpData = (networkData*)theClient->getCustomData(this);
	assert(tmpData != NULL);

	tmpData->flood_points = amount; 
}	
 
void cservice::setLastRecieved(iClient* theClient, time_t last_recieved)
{
	/*
	 * This function sets a timestamp for when we last recieved
	 * a message from this iClient.
	 */

	networkData* tmpData = (networkData*)theClient->getCustomData(this);
	assert(tmpData != NULL);

	tmpData->messageTime = last_recieved;
}	

time_t cservice::getLastRecieved(iClient* theClient)
{
	/*
	 * This function gets a timestamp from this iClient
	 * for flood control.
	 */

	networkData* tmpData = (networkData*)theClient->getCustomData(this);
	assert(tmpData != NULL);

	return tmpData->messageTime;
}	

bool cservice::hasFlooded(iClient* theClient)
{
	if( (getLastRecieved(theClient) + flood_duration) <= ::time(NULL) )
	{
	/*
	 *  Reset a few things, they're out of the flood period now.
	 *  Or, this is the first message from them.
	 */

	setFloodPoints(theClient, 0);
	setLastRecieved(theClient, ::time(NULL));
	} else 
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
			if (theUser && getAdminAccessLevel(theUser)) return false;

			// Bad boy!
			setIgnored(theClient, true);
			setFloodPoints(theClient, 0);
			setLastRecieved(theClient, ::time(NULL)); 
			Notice(theClient, "Flood me will you? I'm not going to listen to you anymore."); 
		
			// Send a silence numeric target, and mask to ignore messages from this user.
			strstream s;
			s << getCharYYXXX() << " SILENCE " << theClient->getCharYYXXX() << " *!*" 
			  << theClient->getUserName() << "@" << theClient->getInsecureHost() << ends; 
			Write( s );
			delete[] s.str();
		
			logAdminMessage("MSG-FLOOD from %s", theClient->getNickUserHost().c_str());
			return true;
		}
	}

	return false;
} 

int cservice::OnPrivateMessage( iClient* theClient, const string& Message,
	bool secure )
{ 
	/*
	 *	Private message handler. Pass off the command to the relevant
	 *  handler.
	 */

	/* Don't talk to naughty people. */
	if (isIgnored(theClient)) return false; 

	StringTokenizer st( Message ) ;
	if( st.empty() )
	{
		Notice( theClient, "Incomplete command" ) ;
		return 0 ;
	}

	/*
	 * Do flood checking - admins at 750 or above are excempt.
	 * N.B: Only check that *after* someone has flooded ;)
	 */

	const string Command = string_upper( st[ 0 ] ) ;

	/* Attempt to find a handler for this method. */

	commandMapType::iterator commHandler = commandMap.find( Command ) ;
	if( commHandler == commandMap.end() )
	{
		/* Don't reply to unknown commands, but add to their flood 
		 * total :) */
		if (hasFlooded(theClient)) return false;
		setFloodPoints(theClient, getFloodPoints(theClient) + 3); 
	}
	else
	{
		/*
		 *  Check users flood limit, if exceeded..
		 */

 		if (hasFlooded(theClient)) return false;

		setFloodPoints(theClient, getFloodPoints(theClient) + commHandler->second->getFloodPoints() );
		commHandler->second->Exec( theClient, Message ) ;
	}

return xClient::OnPrivateMessage( theClient, Message ) ;
}
 
int cservice::OnCTCP( iClient* theClient, const string& CTCP,
                    const string& Message, bool Secure)
{
	/*
	 * CTCP hander. Deal with PING, GENDER and VERSION. 
	 * Hit users with a '5' flood score for CTCP's.
	 */

	if (hasFlooded(theClient)) return false;

	setFloodPoints(theClient, getFloodPoints(theClient) + 5 ); 

	StringTokenizer st( CTCP ) ;
	if( st.empty() )
		{
			return 0;
		}

	const string Command = string_upper(st[0]);

	if(Command == "PING")
	{
		xClient::DoCTCP(theClient, CTCP.c_str(), Message.c_str());
		return true;
	}

	if(Command == "GENDER")
	{
		xClient::DoCTCP(theClient, CTCP.c_str(), "I am Bender; please insert girder.");
		return true;
	}

	if(Command == "VERSION")
	{
		xClient::DoCTCP(theClient, CTCP.c_str(), "Undernet P10 Channel Services Version 2 [" __DATE__ " " __TIME__ "] ($Id: cservice.cc,v 1.65 2001/01/28 23:16:33 gte Exp $)");
		return true;
	}
 
	return true;
} 
 
sqlUser* cservice::isAuthed(iClient* theClient, bool alert)
{
	/*
	 *  Confirms a user is logged in by returning a pointer to
	 *  the sqlUser record. 
	 *  If 'alert' is true, send a notice to the user informing
	 *  them that they must be logged in.
	 */

	networkData* tmpData = (networkData*)theClient->getCustomData(this); 

	sqlUser* theUser = tmpData->currentUser;

	if(theUser) {
		return theUser;
	}

	if (alert) Notice(theClient, "Sorry, You must be logged in to use this command.");
	return 0;
}
	
sqlUser* cservice::getUserRecord(const string& id)
{
	/*
	 *  Locates a cservice user record by 'id', the username of this user. 
	 */

	/*
	 *  Check if this record is already in the cache.
	 */
 
	sqlUserHashType::iterator ptr = sqlUserCache.find(id);
	if(ptr != sqlUserCache.end()) // Found something!
	{
		elog << "cmaster::getUserRecord> Cache hit for " << id << endl;
		userCacheHits++;
		return ptr->second ;
	}

	/*
	 *  We didn't find anything in the cache, fetch the data from
	 *  the backend and create a new sqlUser object.
	 */

	sqlUser* theUser = new sqlUser(SQLDb);

	if (theUser->loadData(id)) 
	{ 
	 	sqlUserCache.insert(sqlUserHashType::value_type(id, theUser));
		elog << "cmaster::getUserRecord> There are " << sqlUserCache.size() << " elements in the cache." << endl;
		userHits++;
		return theUser;
	}

	delete theUser;
	return 0;
}	

vector<sqlBan*>* cservice::getBanRecords(sqlChannel* theChan)
{
	/*
	 *  Returns a vector of sqlBan's for a given channel.
	 *  If not found in the cache, load from the database.
	 */

	sqlBanHashType::iterator ptr = sqlBanCache.find(theChan->getID());
	if(ptr != sqlBanCache.end()) // Found something!
	{
		elog << "cmaster::getBanRecords> Cache hit for " << theChan->getID() << endl;
		banCacheHits++;
		return ptr->second ;
	}

	/*
	 *  We didn't find anything in the cache, fetch the data from
	 *  the backend and create a new vector<sqlban*> object.
	 *  If we find no bans.. return a new blank container.
	 */ 

	vector<sqlBan*>* banList = new vector<sqlBan*>;

	/*
	 * Execute some SQL to get all bans relating to this channel.
	 */

	ExecStatusType status;
	strstream theQuery;
	theQuery << "SELECT " << sql::ban_fields << " FROM bans WHERE channel_id = " << theChan->getID() << ends;
	elog << "cmaster::getBanRecords> " << theQuery.str() << endl; 

	if ((status = SQLDb->Exec(theQuery.str())) == PGRES_TUPLES_OK)
	{ 
		for (int i = 0 ; i < SQLDb->Tuples(); i++)
		{ 
			sqlBan* newBan = new sqlBan(SQLDb);
			newBan->setAllMembers(i);
			banList->push_back(newBan);
			elog << "Loaded ban for : " << newBan->getBanMask() << endl;
		}
	}

	 /* Insert into the cache - even if its empty */

	sqlBanCache.insert(sqlBanHashType::value_type(theChan->getID(), banList));
	elog << "cmaster::getBanRecords> There are " << sqlBanCache.size() << " elements in the cache." << endl;
	banHits++;

	return banList;
}	
 
sqlChannel* cservice::getChannelRecord(const string& id)
{ 
	/*
	 *  Locates a cservice user record by 'id', the channel name. 
	 */

	/*
	 *  Check if this record is already in the cache.
	 */
 
	sqlChannelHashType::iterator ptr = sqlChannelCache.find(id);
	if(ptr != sqlChannelCache.end()) // Found something!
	{
		elog << "cmaster::getChannelRecord> Cache hit for " << id << endl;
		channelCacheHits++;
		return ptr->second ;
	} 

	/*
	 *  We didn't find anything in the cache, fetch the data from
	 *  the backend and create a new sqlUser object.
	 */

	sqlChannel* theChan = new sqlChannel(SQLDb);
 
	if (theChan->loadData(id)) {
	 	sqlChannelCache.insert(sqlChannelHashType::value_type(id, theChan));
		elog << "cmaster::getChannelRecord> There are " << sqlChannelCache.size() << " elements in the cache." << endl;
		channelHits++;
		return theChan;
	}

	delete theChan;
	return 0;
} 

sqlLevel* cservice::getLevelRecord( sqlUser* theUser, sqlChannel* theChan )
{
	// Check if the record is already in the cache.
	pair<int, int> thePair;
	thePair = make_pair(theUser->getID(), theChan->getID());

	sqlLevelHashType::iterator ptr = sqlLevelCache.find(thePair);
	if(ptr != sqlLevelCache.end()) // Found something!
	{ 
		elog << "cmaster::getLevelRecord> Cache hit for user-id:chan-id " << theUser->getID() << ":" << theChan->getID() << endl;
		levelCacheHits++;
		return ptr->second ;
	} 

	/*
	 *  We didn't find anything in the cache, fetch the data from
	 *  the backend and create a new sqlUser object.
	 */

	sqlLevel* theLevel = new sqlLevel(SQLDb);
 
	if (theLevel->loadData(theUser->getID(), theChan->getID())) {
	 	sqlLevelCache.insert(sqlLevelHashType::value_type(thePair, theLevel));
		elog << "cmaster::getLevelRecord> There are " << sqlLevelCache.size() << " elements in the cache." << endl;
		levelHits++;
		theLevel->setFlag(sqlLevel::F_ONDB); // Flag to let us know this has been loaded from the Db.
		theLevel->removeFlag(sqlLevel::F_FORCED); // Remove this incase someone forced an existing record, and
												  // modified it causing it to be committed with the forced flag
												  // still. (Remove when new single field commit scheme in place).
		return theLevel;
	}

	delete theLevel;
	return 0;
}	
 
short cservice::getAdminAccessLevel( sqlUser* theUser )
{
	/*
	 *  Returns the admin access level a particular user has. 
	 */

	sqlChannel* theChan = getChannelRecord("*");
	if (!theChan)
	{
		elog << "cservice::getAdminAccessLevel> Unable to locate channel '*'! Sorry, I can't continue.." << endl;
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

/*--getEffectiveAccessLevel---------------------------------------------------
 *
 *  Returns the access level a particular user has on a particular
 *  channel taking into account channel & user level suspensions.
 *  Also used to return the level of access granted to a forced access.
 *
 *  Usage: When determining if we should grant a permission to a user to access
 *  a particular command/function.
 *  To determine the effect access level of a target user.
 *--------------------------------------------------------------------------*/ 
short cservice::getEffectiveAccessLevel( sqlUser* theUser, sqlChannel* theChan, bool notify )
{
 
	sqlLevel* theLevel = getLevelRecord(theUser, theChan);
	if(theLevel)
	{
		
		if (theLevel->getFlag(sqlLevel::F_FORCED))
		{
			/* A forced access, return forced access level. */
			return theLevel->getForcedAccess();
		}

		/* Check to see if the channel has been suspended. */ 
		if (theChan->getFlag(sqlChannel::F_SUSPEND))
		{
			/* Send them a notice to let them know they've been bad? */
			iClient* theClient = theUser->isAuthed();
			if (theClient && notify)
				Notice(theClient, "The channel %s has been suspended by a cservice administrator.",
					theChan->getName().c_str());
			return 0;
		}

		/*
		 *  Check to see if this particular access record has been
		 *  suspended too.
		 */

		if (theLevel->getSuspendExpire() != 0)
		{
			// Send them a notice.
			iClient* theClient = theUser->isAuthed();
			if (theClient && notify)
				Notice(theClient, "Your access on %s has been suspended.",
					theChan->getName().c_str());
			return 0;
		}
 
		return theLevel->getAccess();
	}

	// By default, users have level 0 access on a channel.
	return 0;
}

/*--getAccessLevel------------------------------------------------------------
 *
 *  Returns the access level a particular user has on a particular
 *  channel. Plain and simple. If the user has 500 in the channel
 *  record, this function returns 500. 
 *--------------------------------------------------------------------------*/ 
short cservice::getAccessLevel( sqlUser* theUser, sqlChannel* theChan )
{ 
	sqlLevel* theLevel = getLevelRecord(theUser, theChan);
	if(theLevel) return theLevel->getAccess();
 
	/* By default, users have level 0 access on a channel. */
	return 0;
}

 
const string& cservice::getResponse( sqlUser* theUser, int response_id )
{ 
	/*
 	 * Returns response id "response_id" for this user's prefered
	 * language. 
	 */

	static string result;
	int lang_id;

	if (theUser) {
		lang_id = theUser->getLanguageId();
	} else {
		lang_id = 1; /* Default to english if not authenticated? */
	}
	
	pair<int, int> thePair;
	thePair = make_pair(lang_id, response_id);

	translationTableType::iterator ptr =  translationTable.find(thePair);
	if(ptr != translationTable.end()) /* Found something! */
	{ 
		return ptr->second ;
	} 

	/* 
	 * Can't find this response Id within a valid language.
	 * Realistically we should bomb here, however it might be wise to 'fallback'
	 * to a lower language ID and try again, only bombing if we can't find an
	 * english variant. (Carrying on here could corrupt numerous varg lists, and
	 * will most likely segfault anyway).
	 */

	result = "Unable to retrieve response. Please contact a cservice administrator.";
	return result;
}

void cservice::loadTranslationTable()
{
	/*
	 *  Execute an SQL query to retrieve all the translation data.
	 */

	ExecStatusType status;
 
	if ((status = SQLDb->Exec( "SELECT language_id,response_id,text FROM translations" )) == PGRES_TUPLES_OK)
	{

		for (int i = 0 ; i < SQLDb->Tuples(); i++)
		{
			pair<int, int> thePair;

			/*
			 *  Add to our translations table.
			 */

			int lang_id = atoi(SQLDb->GetValue( i, 0 ));
			int resp_id = atoi(SQLDb->GetValue( i, 1 ));
			thePair = make_pair(lang_id,resp_id);
			translationTable.insert( translationTableType::value_type(thePair, SQLDb->GetValue( i, 2 )) );
		}
	}

	elog << "cmaster::loadTranslationTable> Loaded " << translationTable.size() << " entries." << endl;
} 
 
bool cservice::isOnChannel( const string& chanName ) const
{
	return true;
}

int cservice::OnTimer(xServer::timerID, void*)
{
   /*
    *  Time to see if anyone updated the database while we weren't looking. :)
    */

//    elog << "cmaster::OnTimer> Checking for updates.." << endl;
	time_t theTime = time(NULL) + updateInterval;
	MyUplink->RegisterTimer(theTime, this, NULL);
 
	ExecStatusType status;
	strstream theQuery;
	unsigned short updateType = 0; // 1 = channel, 2 = user, 3 = level, 4 = ban. 

	PGnotify* notify = SQLDb->Notifies();

	/*
	 *  We got a notification event..
	 */
	if (!notify) {
		return true;
	}

	elog << "cmaster::OnTimer> Recieved a notification event for '" << notify->relname 
	<< "' from Backend PID '" << notify->be_pid << "'"
	<< endl;

	// Check we aren't getting our own updates.
	if (notify->be_pid == SQLDb->getPID())
	{
		elog << "cmaster::OnTimer> Notification from our Backend PID, ignoring update." << endl; 
		free(notify); 
		return false;
	}


	if (string(notify->relname) == "channels_u") 
	{
		updateType = 1;
		theQuery << "SELECT " << sql::channel_fields
		<< ",now()::abstime::int4 as db_unixtime FROM channels WHERE last_updated >= " << lastChannelRefresh;
		// Fetch updated channel information.
	}

	if (string(notify->relname) == "users_u") 
	{
		updateType = 2;
		theQuery << "SELECT " << sql::user_fields
		<< ",now()::abstime::int4 as db_unixtime FROM users,users_lastseen WHERE users.id = users_lastseen.user_id AND users.last_updated >= " << lastUserRefresh;
		// Fetch updated user information.
	} 

	if (string(notify->relname) == "levels_u")
	{
		updateType = 3;
		theQuery << "SELECT " << sql::level_fields
		<< ",now()::abstime::int4 as db_unixtime FROM levels WHERE last_updated >= " << lastLevelRefresh;
		// Fetch updated level information.
	} 

	theQuery << ends;
	elog << "sqlQuery> " << theQuery.str() << endl;

	// Execute query, parse results.
	status = SQLDb->Exec(theQuery.str());

	// Free memory allocated by postgres API object.
	free(notify); 

	if (status != PGRES_TUPLES_OK)
	{
		elog << "cmaster::OnTimer> Something went wrong: " << SQLDb->ErrorMessage() << endl; // Log to msgchan here.
		return false;
	}

	elog << "cmaster::OnTimer> Found " << SQLDb->Tuples() << " updated records." << endl;
	/*
	 *  Now, update the cache with information in this results set.
	 */

	if (SQLDb->Tuples() <= 0) // We could get no results back if the last_update field wasn't set.
	{ 
		return false;
	}

	/* Update our time offset incase things drift.. */
	dbTimeOffset = atoi(SQLDb->GetValue(0,"db_unixtime")) - ::time(NULL);

	switch(updateType)
	{
		case 1: // Channel update.
		{
			for (int i = 0 ; i < SQLDb->Tuples(); i++)
			{ 
				sqlChannelHashType::iterator ptr = sqlChannelCache.find(SQLDb->GetValue(i, 1)); 
				if(ptr != sqlChannelCache.end()) /* Found something! */
				{ 
					(ptr->second)->setAllMembers(i);
				} else  /* Not in the cache.. must be a new channel. */
				{
					/* Create new channel record, insert in cache. */
					sqlChannel* newChan = new sqlChannel(SQLDb);
					newChan->setAllMembers(i);
				 	sqlChannelCache.insert(sqlChannelHashType::value_type(newChan->getName(), newChan));
					/* Join the newly registered channel. */
					//Join(newChan->getName(), newChan->getChannelMode(), 0, true);
				}
			
			}
		
			// Set the "Last refreshed from channels table" timestamp.
			lastChannelRefresh = atoi(SQLDb->GetValue(0,"db_unixtime"));
			break;
		}

		case 2: // User updates.
		{
			for (int i = 0 ; i < SQLDb->Tuples(); i++)
			{ 
				sqlUserHashType::iterator ptr = sqlUserCache.find(SQLDb->GetValue(i, 1)); 
				// Found something..
				if(ptr != sqlUserCache.end()) (ptr->second)->setAllMembers(i); 
			}
		
			// Set the "Last refreshed from channels table" timestamp.
			lastUserRefresh = atoi(SQLDb->GetValue(0,"db_unixtime"));
			break;
		}

		case 3: // Level updates.
		{
			for (int i = 0 ; i < SQLDb->Tuples(); i++)
			{ 
				sqlLevelHashType::iterator ptr = sqlLevelCache.find(make_pair(atoi(SQLDb->GetValue(i, 1)), atoi(SQLDb->GetValue(i, 0)))); 
				// Found something..
				if(ptr != sqlLevelCache.end()) (ptr->second)->setAllMembers(i); 
			}
		
			// Set the "Last refreshed from channels table" timestamp.
			lastLevelRefresh = atoi(SQLDb->GetValue(0,"db_unixtime"));
			break;
		}
 
	}

	return(0);
}
 
bool cservice::serverNotice( Channel* theChannel, const char* format, ... )
{
	/*
	 *  Send a notice to a channel from the server.
	 */

	char buf[ 1024 ] = { 0 } ;
	va_list _list ;
	
	va_start( _list, format ) ;
	vsprintf( buf, format, _list ) ;
	va_end( _list ) ;
	
	string theMessage = buf;
 
	strstream s;
	s << MyUplink->getCharYY() << " O " << theChannel->getName() << " "
	<< ":" << theMessage << ends;

	Write( s );
	delete[] s.str();
	
	return false;
} 

bool cservice::serverNotice( Channel* theChannel, const string& Message)
{
	/*
	 *  Send a notice to a channel from the server.
	 */
 
	strstream s;
	s << MyUplink->getCharYY() << " O " << theChannel->getName() << " "
	<< ":" << Message << ends;

	Write( s );
	delete[] s.str();
	
	return false;
} 

bool cservice::logAdminMessage(const char* format, ... )
{
	/*
	 *  Log a message to the admin channel and the logfile.
	 */

	char buf[ 1024 ] = { 0 } ;
	va_list _list ;
	
	va_start( _list, format ) ;
	vsprintf( buf, format, _list ) ;
	va_end( _list ) ;
	
	// Try and locate the relay channel.
	Channel* tmpChan = Network->findChannel(relayChan);
	if (!tmpChan) 
	{
		elog << "cservice::logMessage> Unable to locate relay channel on network!" << endl;
		return false;
	}
	string message = "[" + nickName + "] " + string(buf);
	serverNotice(tmpChan, message);
	return true;
}
 
const string& cservice::prettyDuration( int duration )
{ 
        // Pretty format a 'duration' in seconds to
        // x day(s), xx:xx:xx.
        static string result;

		char tmp[63] = {0};
        int days = 0, hours = 0, mins = 0, secs = 0, res = 0;
        res = currentTime() - duration;
        secs = res % 60;
        mins = (res / 60) % 60;
        hours = (res / 3600) % 24;
        days = (res / 86400);
        sprintf(tmp, "%i day%s, %02d:%02d:%02d",
                days,
                (days == 1 ? "" : "s"),
                hours,
                mins,
                secs);
        result = tmp; 
        return result;
}  

bool cservice::validUserMask(const string& userMask)
{
        char tmpmask[512], *n, *u, *h;
        
        strncpy(tmpmask, userMask.c_str(), 511);
        tmpmask[511] = '\0';
        
        n = strtok(tmpmask, "!");
        u = strtok(NULL, "@");
        h = strtok(NULL, " ");
        
        if(!n || !*n || !u || !*u || !h || !*h) return false;

        if(strlen(n) > 9) return false;

        if(strlen(u) > 12) return false;

        if(strlen(h) > 128) return false; 

        return true;
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
		elog << "cservice::OnChannelModeO> WARNING, unable to locate channel record"
		<< " for registered channel event: " << theChan->getName() << endl;
		return;
	}

	vector< iClient* > deopList; // List of clients to deop.
	int deopCounter = 0;

	for( xServer::opVectorType::const_iterator ptr = theTargets.begin() ;
		ptr != theTargets.end() ; ++ptr )
	{
		ChannelUser* tmpUser = ptr->second;
		bool polarity = ptr->first;
		if (polarity) // If somebody is being opped.
		{
			// If the channel is NOOP, deop everyone who tries to get opped!
			if (reggedChan->getFlag(sqlChannel::F_NOOP)) deopList.push_back(tmpUser->getClient());

			// If the channel is STRICTOP, deop everyone who isn't authenticated or
			// and doesn't have access on the channe.
			if (reggedChan->getFlag(sqlChannel::F_STRICTOP))
			{
				sqlUser* authUser = isAuthed(tmpUser->getClient(), false);
				if (!authUser) // Not authed, deop.
				{
					deopList.push_back(tmpUser->getClient());
					// Authed but doesn't have access... deop.
				} else if (!(getEffectiveAccessLevel(authUser,reggedChan, false) >= level::op)) deopList.push_back(tmpUser->getClient());
			}	
		} else {
			/* Somebody is being deopped? */
			deopCounter++;
		}
	}

	/*
	 *  Send notices and perform the deop's.
	 */

	if (deopList.size() > 0)
	{
		if ((theChanUser) && (reggedChan->getFlag(sqlChannel::F_NOOP)) ) 
			Notice(theChanUser->getClient(), "The NOOP flag is set on %s",
			reggedChan->getName().c_str());

		if ((theChanUser) && (reggedChan->getFlag(sqlChannel::F_STRICTOP)) ) 
			Notice(theChanUser->getClient(), "The STRICTOP flag is set on %s",
			reggedChan->getName().c_str());

		DeOp(theChan, deopList);
	}

	/*
	 *  Have more than 'maxdeoppro' been deopped?
	 */

	if (deopCounter >= reggedChan->getMassDeopPro())
	{
		Notice(theChanUser->getClient(), "You just deopped more than %i people", 
			reggedChan->getMassDeopPro());
	} 
}

int cservice::OnEvent( const eventType& theEvent,
	void* data1, void* data2, void* data3, void* data4 )
{
	switch( theEvent )
	{
		case EVT_QUIT:
		case EVT_KILL:
		{
			/*
			 *  We need to deauth this user if they're authed.
			 *  Also, clean up their custom data memory.
			 */
	
			iClient* tmpUser = (theEvent == EVT_QUIT) ? (iClient*)data1 : (iClient*)data2;
			sqlUser* tmpSqlUser = isAuthed(tmpUser, false);
			if (tmpSqlUser)
			{
				tmpSqlUser->networkClient = NULL;
				elog << "cservice::OnEvent> Deauthenticated user " << tmpSqlUser->getUserName() << endl;
			}
			// Clear up the custom data structure we appended to this iClient.
			networkData* tmpData = (networkData*)tmpUser->getCustomData(this); 
			tmpUser->removeCustomData(this);
			delete(tmpData); 
			customDataAlloc--; 
			break ;
		}

		case EVT_NICK:
		{
			/*
			 *  Give this new user a custom data structure!
			 */

			iClient* tmpUser = (iClient*)data1;
			networkData* newData = new networkData(); 
			customDataAlloc++;
			newData->currentUser = NULL; // Not authed.
			tmpUser->setCustomData(this, (void*)newData);
			break;
		}
	}
 
	return 0;
}

/*--deopAllOnChan-------------------------------------------------------------
 *
 * Support function to deop all opped users on a channel.
 *--------------------------------------------------------------------------*/
void cservice::deopAllOnChan(Channel* theChan)
{ 
	if (!theChan) return; /* Don't try this on a null channel. */

	vector< iClient* > deopList;

	for( Channel::const_userIterator ptr = theChan->userList_begin();
	ptr != theChan->userList_end() ; ++ptr )
	{
	if( ptr->second->getMode(ChannelUser::MODE_O))
		{
			deopList.push_back( ptr->second->getClient() );
		}
	}

if( !deopList.empty() )
	{
		DeOp(theChan, deopList);
	}

}

/*--OnChannelEvent------------------------------------------------------------
 *
 * Handler for registered channel events.
 * Performs a number of functions, autoop, autovoice, bankicks, etc.
 *--------------------------------------------------------------------------*/ 
int cservice::OnChannelEvent( const channelEventType& whichEvent,
	Channel* theChan,
	void* data1, void* data2, void* data3, void* data4 )
{
 
	iClient* theClient = 0 ;

	switch( whichEvent )
		{
		case EVT_CREATE: /* God help us if we recieve this for a regg'd channel. */
		case EVT_JOIN: 
		{
			theClient = static_cast< iClient* >( data1 ) ;
			sqlChannel* reggedChan = getChannelRecord(theChan->getName());
			if(!reggedChan)
			{
				elog << "cservice::OnChannelEvent> WARNING, unable to locate channel record"
					<< " for registered channel event: " << theChan->getName() << endl;
				return 0;
			}

			/*
			 *	First thing we do - check if this person is banned.
			 */ 

			vector< sqlBan* >* banList = getBanRecords(reggedChan);
			vector< sqlBan* >::iterator ptr = banList->begin();
			bool deleted = false;

			while (ptr != banList->end())
			{
				sqlBan* theBan = *ptr; 

				/* Has this ban expired? */ 
				if (theBan->getExpires() <= currentTime())
				{
					/* Delete this ban.. */
					strstream s;
					s << getCharYYXXX() << " M " << reggedChan->getName() << " -b "
					<< theBan->getBanMask() << ends;
				
					Write( s );
					delete[] s.str();
					banList->erase(ptr); 
					theBan->deleteRecord();
					delete(theBan);
					deleted = true;
				} else { 
					/* Matching ban? */ 
					if(match(theBan->getBanMask().c_str(), theClient->getNickUserHost().c_str()) == 0)
					{ 
						strstream s;
						s << getCharYYXXX() << " M " << reggedChan->getName() << " +b "
						<< theBan->getBanMask() << ends;
					
						Write( s );
						delete[] s.str(); 
						Kick(theChan, theClient, string("(" + theBan->getSetBy() + ") " + theBan->getReason()) );
						/* 
						 * Thats it.. we aren't going to op them or anything.
						 * We break out of the select.
						 */
						break;
					} /* Matching Ban */ 
				} /* Not expired. */
				if (deleted) 
				{
					ptr = banList->begin();
					deleted = false;
				} else {
					++ptr;
				}
			} /* Loop over bans */
 
			/* Is it time to set an autotopic? */
			if (reggedChan->getFlag(sqlChannel::F_AUTOTOPIC) && (reggedChan->getLastTopic() + topic_duration <= ::time(NULL)))
			{
				doAutoTopic(reggedChan);
			}

			/* Check noop isn't set */ 
			if (reggedChan->getFlag(sqlChannel::F_NOOP)) break;

			/* Deal with auto-op first - check this users access level. */
			sqlUser* theUser = isAuthed(theClient, false);
			if (!theUser) break; /* If not authed.. */

			/* Check access in this channel. */
			sqlLevel* theLevel = getLevelRecord(theUser, reggedChan); 
			if (!theLevel) break; /* No access.. */
 
			/* Next, see if they have auto op set. */
			if (theLevel->getFlag(sqlLevel::F_AUTOOP)) 
			{
				/* If they are suspended, or somehow have less than 100, don't op them */
				if (getEffectiveAccessLevel(theUser, reggedChan, false) >= level::op)
				{
					Op(theChan, theClient); 
					break;
				}
			}

			/* Or auto voice? */
			if (theLevel->getFlag(sqlLevel::F_AUTOVOICE)) 
			{
				/* If they are suspended, or somehow have less than 75, don't voice them */
				if (getEffectiveAccessLevel(theUser, reggedChan, false) >= level::voice)
				{
					Voice(theChan, theClient);
					break;
				}
			}
 
			break;
		}

		default:
			break;
		}

return xClient::OnChannelEvent( whichEvent, theChan, data1, data2, data3, data4 );
}

/*--doAutoTopic---------------------------------------------------------------
 *
 * This support function sets the autotopic in a particular channel. 
 *--------------------------------------------------------------------------*/ 
void cservice::doAutoTopic(sqlChannel* theChan)
{
	strstream s;

	/* Quickly drop out if nothing is set.. */ 
	if ((theChan->getDescription() == "") && (theChan->getURL() == "")) return;

	string extra = (theChan->getURL().size() != 0) ? (" ( " + theChan->getURL() + " )") : "";

	s << getCharYYXXX() << " T " << theChan->getName() << " :" 
		<< theChan->getDescription() << extra << ends; 
	Write( s ); 
	delete[] s.str(); 

	theChan->setLastTopic(::time(NULL));
}	

/*--doInternalBanAndKick------------------------------------------------------
 *
 * Bans a user via IRC and the database with 'theReason' and then kicks.
 *--------------------------------------------------------------------------*/ 
bool cservice::doInternalBanAndKick(sqlChannel* theChan, iClient* theClient, const string& theReason)
{
	vector< sqlBan* >* banList = getBanRecords(theChan); 
}

/*--escapeSQLChars------------------------------------------------------------
 *
 * Global function to replace ' with \' in strings for safe placement in
 * SQL statements.
 *--------------------------------------------------------------------------*/ 
const string gnuworld::escapeSQLChars(const string& theString)
{ 
	static string result;
	result = theString;
 
	string search = "'";
	string replace = "\\\047";

	string::size_type idx;

	idx = string::npos;

	while (true)
	{ 
		if(idx == string::npos)
		{
			idx = result.find(search);
		} else {
			idx = result.find(search, idx+2);
		}

		if (idx == string::npos) break;

		result.replace(idx, search.size(), replace);
	}

	return result;
}	

time_t cservice::currentTime()
{
	/* Returns the current time according to the postgres server. */ 

	 return dbTimeOffset + ::time(NULL);
} 
 
void Command::Usage( iClient* theClient )
{
	bot->Notice( theClient, string( "SYNTAX: " ) + getInfo() ) ;
}
 
} // namespace gnuworld
