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
	
	for( eventType i = 0 ; i != EVT_NOOP ; ++i )
	{
	  theServer->RegisterEvent( i, this );
	} 

 	// Attempt to register our interest in recieving NOTIFY events.
	if (SQLDb->ExecCommandOk("LISTEN channels_u; LISTEN bans_u; LISTEN users_u; LISTEN levels_u;")) {
		elog << "cmaster::ImplementServer> Successfully registered LISTEN event for Db updates." << endl;
		// Start the Db update timer rolling.
		time_t theTime = time(NULL) + updateInterval;
		theServer->RegisterTimer(theTime, this, NULL); 
	} else 
	{
		elog << "cmaster::ImplementServer> PostgreSQL error while attempting to register LISTEN event: " << SQLDb->ErrorMessage() << endl;
	}


xClient::ImplementServer( theServer ) ;
}

cservice::cservice(const string& args)
{ 
    /*
	 *  Register command handlers.
	 */
 
    RegisterCommand(new SHOWCOMMANDSCommand(this, "SHOWCOMMANDS", "<#channel>"));
    RegisterCommand(new LOGINCommand(this, "LOGIN", "<username> <password>")); 
    RegisterCommand(new ACCESSCommand(this, "ACCESS", "[channel] [nick] [-min n] [-max n] [-autoop] [-noautoop] [-modif [mask]]"));
    RegisterCommand(new CHANINFOCommand(this, "CHANINFO", "<#channel>")); 
    RegisterCommand(new ISREGCommand(this, "ISREG", "<#channel>")); 
    RegisterCommand(new VERIFYCommand(this, "VERIFY", "<nick>"));
    RegisterCommand(new SEARCHCommand(this, "SEARCH", "<keywords>"));
    RegisterCommand(new MOTDCommand(this, "MOTD", ""));
    RegisterCommand(new RANDOMCommand(this, "RANDOM", ""));
    RegisterCommand(new SHOWIGNORECommand(this, "SHOWIGNORE", ""));

    RegisterCommand(new OPCommand(this, "OP", "<#channel> [nick] [nick] .."));
    RegisterCommand(new DEOPCommand(this, "DEOP", "<#channel> [nick] [nick] .."));
    RegisterCommand(new VOICECommand(this, "VOICE", "<#channel> [nick] [nick] .."));
    RegisterCommand(new DEVOICECommand(this, "DEVOICE", "<#channel> [nick] [nick] .."));
    RegisterCommand(new ADDUSERCommand(this, "ADDUSER", "<#channel> <nick> <access>"));
    RegisterCommand(new REMUSERCommand(this, "REMUSER", "<#channel> <nick>"));
    RegisterCommand(new MODINFOCommand(this, "MODINFO", "<#channel> [ACCESS <nick> <level>] [AUTOOP <nick> <on|off>]"));
    RegisterCommand(new SETCommand(this, "SET", "[#channel] <variable> <value>"));
    RegisterCommand(new INVITECommand(this, "INVITE", "<#channel>"));
    RegisterCommand(new TOPICCommand(this, "TOPIC", "<#channel> <topic>"));
    RegisterCommand(new CHANINFOCommand(this, "CHANINFO", "<#channel>"));
    RegisterCommand(new CHANINFOCommand(this, "INFO", "<#channel>"));
    RegisterCommand(new BANLISTCommand(this, "BANLIST", "<#channel>"));
    RegisterCommand(new KICKCommand(this, "KICK", "<#channel> <nick> <reason>"));

    RegisterCommand(new REGISTERCommand(this, "REGISTER", "<#channel>"));
    RegisterCommand(new FORCECommand(this, "FORCE", "<#channel>"));
    RegisterCommand(new UNFORCECommand(this, "UNFORCE", "<#channel>"));

	//-- Load in our cservice configuration file. 
	cserviceConfig = new EConfig( args ) ;
	string sqlHost = cserviceConfig->Require( "sql_host" )->second;
	string sqlDb = cserviceConfig->Require( "sql_db" )->second;
	
	string Query = "host=" + sqlHost + " dbname=" + sqlDb ;

	elog << "cmaster::cmaster> Attempting to connect to " << sqlHost << "; Database: " << sqlDb << endl;
 
	SQLDb = new cmDatabase( Query.c_str() ) ;

	//-- Make sure we connected to the SQL database; if we didn't we exit entirely.

	if (SQLDb->ConnectionBad ())
	{
		elog << "cmaster::cmaster> Unable to connect to SQL server." << endl 
		     << "cmaster::cmaster> PostgreSQL error message: " << SQLDb->ErrorMessage () << endl ;

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

	//-- Move this to the configuration file?  This should be fairly static..

	Mode( "+idk" ) ;

	userHits = 0;
	userCacheHits = 0;
	channelHits = 0;
	channelCacheHits = 0;
	lastChannelRefresh = time(NULL);
	lastUserRefresh = time(NULL);
	lastLevelRefresh = time(NULL);
	lastBanRefresh = time(NULL);

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

	if ((status = SQLDb->Exec( "SELECT name,flags,channel_ts,channel_mode,channel_key,channel_limit FROM channels WHERE name <> '*' AND registered_ts <> ''" )) == PGRES_TUPLES_OK)
	{
		for (int i = 0 ; i < SQLDb->Tuples (); i++)
		{
			StringTokenizer data( SQLDb->GetValue( i, 0 ) ) ;
			/*
			 *  Check the auto-join flag is set, if so - join. :)
			 */ 

			MyUplink->JoinChannel( this, data[ 0 ], "+tn" ) ; 
		}
	}

	return xClient::BurstChannels () ;
}
	
int cservice::OnConnect()
{
	return 0;
}

int cservice::OnPrivateMessage( iClient* theClient, const string& Message )
{ 
 /*
  *	Private message handler. Pass off the command to the relevant
  * handler.
  */

	StringTokenizer st( Message ) ;
	if( st.empty() )
	{
		Notice( theClient, "Incomplete command" ) ;
		return 0 ;
	}

	const string Command = string_upper( st[ 0 ] ) ;

	// Attempt to find a handler for this method.
	commandMapType::iterator commHandler = commandMap.find( Command ) ;
	if( commHandler == commandMap.end() )
	{
		Notice( theClient, "Unknown command" ) ;
	}
	else
	{
		commHandler->second->Exec( theClient, Message ) ;
	}

return xClient::OnPrivateMessage( theClient, Message ) ;
}

 
int cservice::OnCTCP( iClient* theClient, const string& CTCP,
                    const string& Message, bool Secure)
{
	/*
	 * CTCP hander. Deal with PING, GENDER and VERSION. 
	 */

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
		xClient::DoCTCP(theClient, CTCP.c_str(), "Undernet Channel Services Version 2 [" __DATE__ " " __TIME__ "] ($Id: cservice.cc,v 1.27 2001/01/02 07:55:12 gte Exp $)");
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
	sqlUser* theUser = (sqlUser*)theClient->getCustomData(this);
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
		elog << "cmaster::getAccessLevel> Cache hit for user-id:chan-id " << theUser->getID() << ":" << theChan->getID() << endl;
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
		return theLevel;
	}

	delete theLevel;
	return 0;
}	
 
short cservice::getAccessLevel( sqlUser* theUser, sqlChannel* theChan )
{
	/*
	 *  Returns the access level a particular user has on a particular
	 *  channel.
	 */

	sqlLevel* theLevel = getLevelRecord(theUser, theChan);
	if(theLevel)
	{
		return theLevel->getAccess();
	}

	// By default, users have level 0 access on a channel.
	return 0;
}
 
const string& cservice::getResponse( sqlUser* theUser, int response_id )
{ 
	/*
 	 * Returns response id "response_id" for this user's prefered
	 * language. 
	 */

	static string result;

	int lang_id = theUser->getLanguageId();
	pair<int, int> thePair;
	thePair = make_pair(lang_id, response_id);

	translationTableType::iterator ptr =  translationTable.find(thePair);
	if(ptr != translationTable.end()) // Found something!
	{ 
		return ptr->second ;
	} 

	/* 
	 * Realistically we should bomb here, however it might be wise to 'fallback'
	 * to a lower language ID and try again, only bombing if we can't find an
	 * english variant. (Carrying on here could corrupt numerous varg lists, and
	 * will most likely segfault anyway).
	 */

	result = "Unable to retrieve response. If you see this, you are already dead :)";
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

    elog << "cmaster::OnTimer> Checking for updates.." << endl;
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
	if (notify->be_pid != SQLDb->getPID())
	{ 
		if (string(notify->relname) == "channels_u") 
		{
			updateType = 1;
			theQuery << "SELECT " << sql::channel_fields
			<< " FROM channels WHERE last_update > " << lastChannelRefresh;
			// Fetch updated channel information.
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

		elog << "cmaster::OnTimer> Found " << SQLDb->Tuples() << " updated channel records." << endl;
		/*
		 *  Now, update the cache with information in this results set.
		 */

		if (SQLDb->Tuples() > 0) // We could get no results back if the last_update field wasn't set.
		{ 
			for (int i = 0 ; i < SQLDb->Tuples(); i++)
			{ 
				sqlChannelHashType::iterator ptr = sqlChannelCache.find(SQLDb->GetValue(i, 1));
				elog << "timer: looking for " << SQLDb->GetValue(i, 2) << endl;
				if(ptr != sqlChannelCache.end()) // Found something!
				{
					elog << "timer: found " << (ptr->second)->getName() << " in cache!" << endl;
					(ptr->second)->setAllMembers(i);
				}

			}
			
		}
 
	} else // Our own notification.
	{
		elog << "cmaster::OnTimer> Notification from our Backend PID, ignoring update." << endl;
	} 


	return(0);
}
 
const string& cservice::prettyDuration( int duration )
{ 
        // Pretty format a 'duration' in seconds to
        // x day(s), xx:xx:xx.
        static string result;

		char tmp[63] = {0};
        int days = 0, hours = 0, mins = 0, secs = 0, res = 0;
        res = time(NULL) - duration;
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

bool cservice::validUserMask(iClient* theClient, const string& userMask)
{
        char tmpmask[512], *n, *u, *h;
        
        strncpy(tmpmask, userMask.c_str(), 511);
        tmpmask[511] = '\0';
        
        n = strtok(tmpmask, "!");
        u = strtok(NULL, "@");
        h = strtok(NULL, " ");
        
        if(!n || !*n || !u || !*u || !h || !*h)
        {
                Notice(theClient, "ERROR: Invalid hostmask, use n!u@h.");
                return false;
        }
        if(strlen(n) > 9)
        {
                Notice(theClient, "ERROR: The specified nick!*@* is too long, max 9 chars allowed.");
                return false;
        }
        if(strlen(u) > 12)
        {
                Notice(theClient, "ERROR: The specified *!user@* is too long, max 12 chars allowed.");
                return false;

        }
        if(strlen(h) > 128)
        {
                Notice(theClient, "ERROR: The specified *!*@host is too long, max 128 chars allowed.");
                return false;
        }

        return true;
}
 
void Command::Usage( iClient* theClient )
{
	bot->Notice( theClient, string( "SYNTAX: " ) + getInfo() ) ;
}
 
} // namespace gnuworld
