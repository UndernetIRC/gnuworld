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

xClient::ImplementServer( theServer ) ;
}

cservice::cservice(const string& args)
{ 
    /*
	 *  Register command handlers.
	 */
 
    RegisterCommand(new SHOWCOMMANDSCommand(this, "SHOWCOMMANDS", "TBA")); 
    RegisterCommand(new LOGINCommand(this, "LOGIN", "<usernamne | userid> <password>"));
    RegisterCommand(new SEARCHCommand(this, "SEARCH", "TBA"));
    RegisterCommand(new ACCESSCommand(this, "ACCESS", "[channel] [nick] [-min n] [-max n] [-autoop] [-noautoop] [-modif [mask]]"));
    RegisterCommand(new CHANINFOCommand(this, "CHANINFO", "TBA"));
    RegisterCommand(new MOTDCommand(this, "MOTD", "TBA"));
    RegisterCommand(new ISREGCommand(this, "ISREG", "#channel"));
    RegisterCommand(new SHOWIGNORECommand(this, "SHOWIGNORE", "TBA"));
    RegisterCommand(new VERIFYCommand(this, "VERIFY", "<nick>"));
    RegisterCommand(new RANDOMCommand(this, "RANDOM", "TBA"));

    RegisterCommand(new OPCommand(this, "OP", "#channel [nick][,nick] .."));
    RegisterCommand(new VOICECommand(this, "VOICE", "#channel [nick][,nick] .."));

	//-- Load in our cservice configuration file.
	cserviceConfig = new EConfig( args ) ;
	string sqlHost = cserviceConfig->Require( "sql_host" )->second;
	string sqlDb = cserviceConfig->Require( "sql_db" )->second;
	
	string Query = "host=" + sqlHost + " dbname=" + sqlDb ;

	elog << "[SQL]: Attempting to connect to " << sqlHost << "; Database: " << sqlDb << endl;

	SQLDb = new PgDatabase( Query.c_str() ) ;

	//-- Make sure we connected to the SQL database; if we didn't we exit entirely.

	if (SQLDb->ConnectionBad ())
	{
		elog << "[SQL]: Unable to connect to SQL server." << endl 
		     << "[SQL]: PostgreSQL error message: " << SQLDb->ErrorMessage () << endl ;

		::exit( 0 ) ;
	}
	else
	{
		elog << "[SQL]: Connection established to SQL server." << endl ;
	}

	//-- Retrieve user, nick, host, and description from the configuration file -- these are all REQUIRED
	//   and the program will exit unless these are present in cservice.conf!

	nickName = cserviceConfig->Require( "nick" )->second ;
	userName = cserviceConfig->Require( "user" )->second ;
	hostName = cserviceConfig->Require( "host" )->second ;
	userDescription = cserviceConfig->Require( "description" )->second ;

	//-- Move this to the configuration file?  This should be fairly static..

	Mode( "+idk" ) ;
}

cservice::~cservice()
{
	delete cserviceConfig ;	
}

int cservice::BurstChannels()
{
	ExecStatusType status;
 
	//-- We need to join every channel in the database that has been ADDCHAN'd by the
	//   manager.  Various other things must be done, such as setting the topic if AutoTopic
	//   is on, gaining ops if AlwaysOp is on, and so forth.

	if ((status = SQLDb->Exec( "select name,flags,channel_ts,channel_mode,channel_key,channel_limit from channels" )) == PGRES_TUPLES_OK)
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
		elog << "sqlUserCache> Cache hit for " << id << endl;
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
		elog << "sqlUserCache> " << sqlUserCache.size() << " elements in hash." << endl;
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
		elog << "sqlChannelCache> Cache hit for " << id << endl;
		return ptr->second ;
	} 

	/*
	 *  We didn't find anything in the cache, fetch the data from
	 *  the backend and create a new sqlUser object.
	 */

	sqlChannel* theChan = new sqlChannel(SQLDb);
 
	if (theChan->loadData(id)) {
	 	sqlChannelCache.insert(sqlChannelHashType::value_type(id, theChan));
		elog << "sqlChannelCache> " << sqlChannelCache.size() << " elements in hash." << endl;
		return theChan;
	}

	delete theChan;
	return 0;
} 
 
short cservice::getAccessLevel( sqlUser* theUser, sqlChannel* theChan )
{
	/*
	 *  Returns the access level a particular user has on a particular
	 *  channel.
	 */

	sqlLevel theLevel(SQLDb);
	if(theLevel.loadData(theUser->getID(), theChan->getID()))
	{
		return theLevel.getAccess();
	}

	// By default, users have level 0 access on a channel.
	return 0;
}
 
bool cservice::isOnChannel( const string& chanName ) const
{
	return true;
}

const string& cservice::prettyDuration( int duration )
{ 
	// Pretty format a 'duration' in seconds to
	// x days, xx:xx:xx.
	static string result;
	result = "TBA";
	return result;
}	
	
void Command::Usage( iClient* theClient )
{
	bot->Notice( theClient, string( "Usage:" ) + ' ' + getInfo() ) ;
}
 
} // namespace gnuworld
