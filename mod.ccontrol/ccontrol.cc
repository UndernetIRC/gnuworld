/* ccontrol.cc
 * Author: Daniel Karrels dan@karrels.com
 */

#include	<string>
#include	<vector>
#include	<iostream>
#include	<algorithm>

#include	<cstring>

#include	"client.h"
#include	"iClient.h"
#include	"EConfig.h"
#include	"events.h"
#include	"StringTokenizer.h"
#include	"misc.h"
#include	"Network.h"
#include	"ELog.h"
#include	"libpq++.h"
#include	"ccontrol.h"
 
const char CControl_h_rcsId[] = __CCONTROL_H ;
const char CControl_cc_rcsId[] = "$Id: ccontrol.cc,v 1.13 2001/02/22 20:27:32 dan_karrels Exp $" ;

using std::string ;
using std::vector ;
using std::cout ;
using std::endl ; 

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
    return new ccontrol( args );
  }

} 
 
ccontrol::ccontrol( const string& configFileName )
 : xClient( configFileName )
{

// Read the config file
EConfig conf( configFileName ) ;

string sqlHost = conf.Find("sql_host" )->second;
string sqlDb = conf.Find( "sql_db" )->second;
string sqlPort = conf.Find( "sql_port" )->second;

string Query = "host=" + sqlHost + " dbname=" + sqlDb + " port=" + sqlPort;

elog	<< "cmaster::cmaster> Attempting to connect to "
	<< sqlHost
	<< "; Database: "
	<< sqlDb
	<< endl;
 
SQLDb = new (nothrow) cmDatabase( Query.c_str() ) ;
assert( SQLDb != 0 ) ;

//-- Make sure we connected to the SQL database; if
// we didn't we exit entirely.
if (SQLDb->ConnectionBad ())
	{
	elog	<< "cmaster::cmaster> Unable to connect to SQL server."
		<< endl 
		<< "cmaster::cmaster> PostgreSQL error message: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	::exit( 0 ) ;
	}
else
	{
	elog	<< "cmaster::cmaster> Connection established to SQL server. "
		<< "Backend PID: " << SQLDb->getPID()
		<< endl ;
	}

// operChanReason is the reason used when kicking non-opers from
// oper-only channels
operChanReason = conf.Find( "operchanreason" )->second ;

// operChanModes are the modes to set when setting up an oper-only
// channel
operChanModes = conf.Find( "operchanmodes" )->second ;

// gLength is the length of time (in seconds) for default glines
gLength = atoi( conf.Find( "glength" )->second.c_str() ) ;

// Set up the oper channels
EConfig::const_iterator ptr = conf.Find( "operchan" ) ;
while( ptr != conf.end() && ptr->first == "operchan" )
	{
	operChans.push_back( ptr->second ) ;
	++ptr ;
	}

// Read out the client's message channel
msgChan = conf.Find( "msgchan" )->second ;

// Make sure that the msgChan is in the list of operchans
if( operChans.end() == find( operChans.begin(), operChans.end(), msgChan ) )
	{
	// Not found, add it to the list of operChans
	operChans.push_back( msgChan ) ;
	}

// Be sure to use all capital letters for the command name

RegisterCommand( new HELPCommand( this, "HELP", "[topic]"
	"\t\tObtain general help or help for a specific command",flg_HELP ) ) ;
RegisterCommand( new INVITECommand( this, "INVITE", "<#channel> "
	"\t\tRequest an invitation to a channel",flg_INVITE ) ) ;
RegisterCommand( new JUPECommand( this, "JUPE", "<servername> <reason> "
	"Jupe a server for the given reason.",flg_JUPE ) ) ;
RegisterCommand( new MODECommand( this, "MODE", "<channel> <modes> "
	"Change modes on the given channel",flg_MODE ) ) ;
RegisterCommand( new GLINECommand( this, "GLINE", "[duration (sec)] <user@host> <reason> "
	"Gline a given user@host for the given reason",flg_GLINE ) ) ;
RegisterCommand( new SCANGLINECommand( this, "SCANGLINE", "<mask> "
	"Search current network glines for glines matching <mask>",flg_SGLINE ) ) ;
RegisterCommand( new REMGLINECommand( this, "REMGLINE", "<user@host> "
	"Remove the gline matching <mask>",flg_REMGLINE ) ) ;
RegisterCommand( new TRANSLATECommand( this, "TRANSLATE", "<numeric>"
	"Translate a numeric into user information",flg_TRANS ) ) ;
RegisterCommand( new WHOISCommand( this, "WHOIS", "<nickname>"
	"Obtain information on a given nickname",flg_WHOIS ) ) ;
RegisterCommand( new KICKCommand( this, "KICK", "<channel> <nick> <reason>"
	"Kick a user from a channel",flg_KICK ) ) ;
RegisterCommand( new ADDOPERCHANCommand( this, "ADDOPERCHAN", "<channel>"
	"Add an oper channel",flg_ADDOPCHN ) ) ;
RegisterCommand( new REMOPERCHANCommand( this, "REMOPERCHAN", "<channel>"
	"Remove an oper channel",flg_REMOPCHN ) ) ;
RegisterCommand( new LISTOPERCHANSCommand( this, "LISTOPERCHANS",
	"List current IRCoperator only channels",flg_LOPCHN ) ) ;
RegisterCommand( new CHANINFOCommand( this, "CHANINFO", "<channel>"
	"Obtain information about a given channel",flg_CHINFO ) ) ;
RegisterCommand( new ACCESSCommand( this, "ACCESS",
	"Obtain the access list",flg_ACCESS ) ) ;
RegisterCommand( new LOGINCommand( this, "LOGIN", "<USER> <PASS> "
	"Authenticate with the bot",flg_LOGIN ) ) ;
RegisterCommand( new DEAUTHCommand( this, "DEAUTH", ""
	"Deauthenticate with the bot",flg_LOGIN ) ) ;
RegisterCommand( new ADDNEWOPERCommand( this, "ADDNEWOPER", "<USER> <OPERTYPE> <PASS> "
	"Add a new oper",flg_ADDNOP ) ) ;
RegisterCommand( new REMOVEOPERCommand( this, "REMOVEOPER", "<USER> <PASS> "
	"Remove an oper",flg_REMOP ) ) ;
RegisterCommand( new ADDCOMMANDCommand( this, "ADDCOMMAND", "<USER> <COMMAND> "
	"Add a new command to an oper",flg_ADDCMD ) ) ;
RegisterCommand( new REMOVECOMMANDCommand( this, "DELCOMMAND", "<USER> <COMMAND> "
	"Remove a command from oper",flg_DELCMD ) ) ;
RegisterCommand( new NEWPASSCommand( this, "NEWPASS", "<PASSWORD> "
	"Change password",flg_NEWPASS ) ) ;
RegisterCommand( new SUSPENDOPERCommand( this, "SUSPEND", "<OPER> "
	"Suspend an oper",flg_SUSPEND ) ) ;
RegisterCommand( new UNSUSPENDOPERCommand( this, "UNSUSPEND", "<OPER> "
	"UnSuspend an oper",flg_UNSUSPEND ) ) ;
RegisterCommand( new MODOPERCommand( this, "MODOPER", "<OPER> <OPTION> <NEWVALUE>"
	"Modify an oper",flg_UNSUSPEND ) ) ;

}


ccontrol::~ccontrol()
{
// Deallocate each command handler
for( commandMapType::iterator ptr = commandMap.begin() ;
	ptr != commandMap.end() ; ++ptr )
	{
	delete ptr->second ;
	ptr->second = 0 ;
	}
commandMap.clear() ;
}

// Register a command handler
bool ccontrol::RegisterCommand( Command* newComm )
{
#ifdef EDEBUG
  assert( newComm != NULL ) ;
#endif

// Unregister the command handler first; prevent memory leaks
UnRegisterCommand( newComm->getName() ) ;

// Insert the new handler
return commandMap.insert( pairType( newComm->getName(), newComm ) ).second ;
}

bool ccontrol::UnRegisterCommand( const string& commName )
{
// Find the command handler
commandMapType::iterator ptr = commandMap.find( commName ) ;

// Was the handler found?
if( ptr == commandMap.end() )
	{
	// Nope
	return false ;
	}

// Deallocate the handler
delete ptr->second ;

// Remove the handler
commandMap.erase( ptr ) ;

// Return success
return true ;
}

int ccontrol::BurstChannels()
{
//elog << "ccontrol::BurstChannels()\n" ;

// msgChan is an operChan as well, no need to burst it separately
for( vector< string >::size_type i = 0 ; i < operChans.size() ; i++ )
	{
	// Burst our channels
	MyUplink->JoinChannel( this, operChans[ i ], operChanModes ) ;

	// Receive events for this channel
	MyUplink->RegisterChannelEvent( operChans[ i ], this ) ;
	}

// Don't forget to call the base class BurstChannels() method
return xClient::BurstChannels() ;

}

// I don't really like doing this.
// In order for each of this bot's Command's to have a valid server
// pointer, this method must be overloaded and server must be
// explicitly set for each Command.
void ccontrol::ImplementServer( xServer* theServer )
{
for( commandMapType::iterator ptr = commandMap.begin() ;
	ptr != commandMap.end() ; ++ptr )
	{
	ptr->second->setServer( theServer ) ;
	}

theServer->RegisterEvent( EVT_KILL, this );
theServer->RegisterEvent( EVT_QUIT, this );

xClient::ImplementServer( theServer ) ;
}

int ccontrol::OnPrivateMessage( iClient* theClient, const string& Message,
	bool )
{

//elog << "ccontrol::OnPrivateMessage()\n" ;

// Only allow opers or services clients to use this client
/*if( !theClient->isOper() && !theClient->getMode( iClient::MODE_SERVICES ) )
	{
	Notice( theClient, "You must be an IRCoperator to use this service." ) ;
	return 0 ;
	}*/

// Tokenize the message
StringTokenizer st( Message ) ;

// Make sure there is a command present
if( st.empty() )
	{
	Notice( theClient, "Incomplete command" ) ;
	return 0 ;
	}

// This is no longer necessary, but oh well *shrug*
const string Command = string_upper( st[ 0 ] ) ;

// Attempt to find a handler for this method.
commandMapType::iterator commHandler = commandMap.find( Command ) ;

// Was a handler found?
if( commHandler == commandMap.end() )
	{
	// Nope, notify the client
	Notice( theClient, "Unknown command" ) ;
	return 0 ; 
	}

// Check if the user is logged in , and he got
// access to that command

int ComAccess = commHandler->second->getFlags();
AuthInfo* theUser = IsAuth(theClient->getCharYYXXX());

if((!theUser) && (ComAccess != 0))
	{
	Notice( theClient,
		"You must be logged in to issue that command" ) ;
	}
else if( (ComAccess != 0) && !(ComAccess & theUser->Access))
	{
	Notice( theClient, "You dont have access to that command" ) ;
	}
else if( (theUser) && (theUser->Flags & isSUSPENDED))
	{
	if( (::time( 0 ) - theUser->SuspendExpires < 0)
		&& (ComAccess != 0))
		{
		Notice( theClient,
			"Sorry but you are suspended");
		}
	else 
		{
		if( ::time( 0 ) - theUser->SuspendExpires >= 0)
			{	
			User* tmpUser = GetUser(theUser->Name);

			tmpUser->SuspendExpires = 0;
			tmpUser->Flags &= ~isSUSPENDED;
			tmpUser->SuspendedBy ="";

			UpdateOper(tmpUser);
    			delete tmpUser;
			}
		}
	// Execute the command handler
	commHandler->second->Exec( theClient, Message) ;
	}		

// Call the base class OnPrivateMessage() method
return xClient::OnPrivateMessage( theClient, Message ) ;
}

int ccontrol::OnEvent( const eventType& theEvent,
	void* Data1, void* Data2, void* Data3, void* Data4 )
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
	
		iClient* tmpUser = (theEvent == EVT_QUIT) ?
			static_cast< iClient* >( Data1 ) :
			static_cast< iClient* >( Data2 ) ;

		AuthInfo *TempAuth = IsAuth(tmpUser->getCharYYXXX());
		if(TempAuth)
	    	    deAuthUser(tmpUser->getCharYYXXX());
		
		break ;
		} // case EVT_KILL/case EVT_QUIT
	
	} // switch()

return 0;

return xClient::OnEvent( theEvent, Data1, Data2, Data3, Data4 ) ;
}

int ccontrol::OnChannelEvent( const channelEventType& theEvent,
	Channel* theChan,
	void* Data1, void* Data2, void* Data3, void* Data4 )
{

switch( theEvent )
	{
	case EVT_JOIN:
		if( !isOperChan( theChan ) )
			{
			// We really don't care otherwise
			// Note, this shouldn't happen
			break ;
			}

		iClient* theClient = static_cast< iClient* >( Data1 ) ;
		if( theClient->isOper() )
			{
			Op( theChan, theClient ) ;
			}
		break ;
	}

// Call the base class OnChannelEvent()
return xClient::OnChannelEvent( theEvent, theChan,
	Data1, Data2, Data3, Data4 ) ;
}

bool ccontrol::isOperChan( const string& theChan ) const
{
vector< string >::const_iterator ptr = operChans.begin(),
	end = operChans.end() ;
while( ptr != end )
	{
	if( !strcasecmp( (*ptr).c_str(), theChan.c_str() ) )
		{
		return true ;
		}
	++ptr ;
	}
return false ;
}

bool ccontrol::isOperChan( const Channel* theChan ) const
{
#ifndef NDEBUG
  assert( theChan != 0 ) ;
#endif

return isOperChan( theChan->getName() ) ;
}

// This method does NOT add the channel to any internal tables
bool ccontrol::Join( const string& chanName, const string& chanModes,
	time_t joinTime, bool getOps )
{
if( isOnChannel( chanName ) )
	{
	// Already on this channel
	return true ;
	}
bool result = xClient::Join( chanName, chanModes, joinTime, getOps ) ;
if( result )
	{
	MyUplink->RegisterChannelEvent( chanName, this ) ;
	}

operChans.push_back( chanName ) ;
return result ;
}

bool ccontrol::Part( const string& chanName )
{
bool foundChannel = false ;
for( vector< string >::iterator ptr = operChans.begin() ;
	ptr != operChans.end() ; ++ptr )
	{
	if( !strcasecmp( (*ptr).c_str(), chanName.c_str() ) )
		{
		operChans.erase( ptr ) ;
		foundChannel = true ;
		break ;
		}
	} // for()

if( !foundChannel )
	{
	// The bot isn't on the channel
	return false ;
	}

bool result = xClient::Part( chanName ) ;
if( result )
	{
	MyUplink->UnRegisterChannelEvent( chanName, this ) ;
	}

return result ;
}

bool ccontrol::Kick( Channel* theChan, iClient* theClient,
	const string& reason )
{
#ifndef NDEBUG
  assert( theChan != NULL ) ;
#endif

if( !isOnChannel( theChan->getName() ) )
	{
	return false ;
	}

return xClient::Kick( theChan, theClient, reason ) ;
}

bool ccontrol::addOperChan( const string& chanName )
{
return addOperChan( chanName, operChanReason ) ;
}

bool ccontrol::addOperChan( const string& chanName, const string& reason )
{
if( isOperChan( chanName ) )
	{
	return false ;
	}

xClient::Join( chanName, operChanModes, 0, true ) ;
MyUplink->RegisterChannelEvent( chanName, this ) ;
operChans.push_back( chanName ) ;

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	elog	<< "ccontrol::addOperChan> Unable to find channel: "
		<< chanName << endl ;
	return false ;
	}

// Kick any users from the channel that aren't opers
vector< iClient* > clientsToKick ;
for( Channel::const_userIterator ptr = theChan->userList_begin() ;
	ptr != theChan->userList_end() ; ++ptr )
	{
	if( !ptr->second->isOper() )
		{
		clientsToKick.push_back( ptr->second->getClient() ) ;
		}
	}

if( !clientsToKick.empty() )
	{
	xClient::Kick( theChan, clientsToKick, reason ) ;
	}

// TODO: set operChanModes

return true ;
}

bool ccontrol::removeOperChan( const string& chanName )
{
// Part() will remove the channel from this client's tables.
Part( chanName ) ;

return true ;
}

AuthInfo* ccontrol::IsAuth( const string& Numeric ) const
{
for( authListType::const_iterator ptr = authList.begin() ;
	ptr != authList.end() ; ++ptr )
	{
	if( !strcasecmp( (*ptr)->Numeric.c_str(), Numeric.c_str() ) )
		{
		return *ptr ;
		}
	}
return 0 ;
}
   
AuthInfo* ccontrol::IsAuth( const int UserId ) const
{
for( authListType::const_iterator ptr = authList.begin() ;
	ptr != authList.end() ; ++ptr )
	{
	if( (*ptr)->Id == UserId )
		{
		return *ptr ;
		}
	}
return 0 ;
}

void ccontrol::UpdateAuth(int Id)
{
    AuthInfo* TempAuth = IsAuth(Id);
    if(TempAuth)
    {
	User* TempUser = GetUser(Id);
	TempAuth->Id = TempUser->Id;
        TempAuth->Name = TempUser->UserName;
        TempAuth->Access = TempUser->Access;
        TempAuth->Flags = TempUser->Flags;
        TempAuth->Next = NULL;
        TempAuth->SuspendExpires = TempUser->SuspendExpires;
        TempAuth->SuspendedBy = TempUser->SuspendedBy;
    }
}

User* ccontrol::GetUser( const string& Name )
{
static const char Main[] = "SELECT user_id,user_name,password,access,flags,suspend_expires,suspended_by FROM opers WHERE lower(user_name) = '";

strstream theQuery;
theQuery	<< Main
		<< string_lower(Name)
		<< "'"
		<< ends;

elog	<< "ACCESS::sqlQuery> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( (PGRES_TUPLES_OK == status) && (SQLDb->Tuples() > 0) )
	{
        return GetParm();
	}

return NULL;
}

User* ccontrol::GetUser( const int Id)
{
static const char Main[] = "SELECT user_id,user_name,password,access,flags,suspend_expires,suspended_by FROM opers WHERE user_id = ";

strstream theQuery;

theQuery	<< Main
		<< Id
		<< ';'
		<< ends;

elog	<< "ACCESS::sqlQuery> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( (PGRES_TUPLES_OK == status) && (SQLDb->Tuples() > 0) )
	{
	return GetParm();
	}

return NULL;
}

User* ccontrol::GetParm ()
{
User* TempUser = new (nothrow) User;
assert (TempUser != NULL);

TempUser->Id = atoi(SQLDb->GetValue(0, 0));
TempUser->UserName = SQLDb->GetValue(0, 1);
TempUser->Password = SQLDb->GetValue(0, 2);
TempUser->Access = atoi(SQLDb->GetValue(0, 3));
TempUser->Flags = atoi(SQLDb->GetValue(0, 4));
TempUser->SuspendExpires = atoi(SQLDb->GetValue(0,5));
TempUser->SuspendedBy = SQLDb->GetValue(0,6);

return TempUser;
}

bool ccontrol::AddOper (User* Oper)
{
static const char *Main = "INSERT into opers (user_name,password,access,last_updated_by,last_updated,flags) VALUES ('";

strstream theQuery;
theQuery	<< Main
		<< Oper->UserName <<"','"
		<< Oper->Password << "',"
		<< Oper->Access << ",'"
		<< Oper->last_updated_by
		<< "',now()::abstime::int4,"
		<< Oper->Flags << ")"
		<< ends;

elog	<< "ACCESS::sqlQuery> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccontrol::AddOper> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}
}

bool ccontrol::DeleteOper (const string& Name)
{
//    strstream Condition;
//    Condition << "WHERE user_id = " << Id << ';';

static const char *Main = "DELETE FROM opers WHERE lower(user_name) = '";

strstream theQuery;
theQuery	<< Main
		<< Name
		<< "'"
		<< ends;

elog	<< "ccontrol::DeleteOper> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccontrol::DeleteOper> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}
}

bool ccontrol::UpdateOper (User* Oper)
{
static const char *Main = "UPDATE opers SET password = '";

strstream theQuery;
theQuery	<< Main
		<< Oper->Password
		<< "', Access = "
		<< Oper->Access
		<< ", last_updated_by = '"
		<< Oper->last_updated_by
		<< "',last_updated = now()::abstime::int4,flags = "
		<< Oper->Flags
		<<  ",suspend_expires = "
		<< Oper->SuspendExpires
		<< " ,suspended_by = '"
		<< Oper->SuspendedBy
		<< "' WHERE lower(user_name) = '" 
		<< string_lower(Oper->UserName) << "'"
		<<  ends;

elog	<< "ccontrol::UpdateOper> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccontrol::UpdateOper> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}
}

int ccontrol::getCommandLevel( const string& Command)
{
commandMapType::iterator commHandler = commandMap.find( Command ) ;

// Was a handler found?
if( commHandler != commandMap.end() )
	{
	return commHandler->second->getFlags() ;
	}

return -1 ;
}	

bool ccontrol::AuthUser( User* TempUser)
{
AuthInfo *TempAuth = new (nothrow) AuthInfo;
assert( TempAuth != 0 ) ;

TempAuth->Id = TempUser->Id;
TempAuth->Name = TempUser->UserName;
TempAuth->Access = TempUser->Access;
TempAuth->Flags = TempUser->Flags;
TempAuth->Next = NULL;
TempAuth->Numeric = TempUser->Numeric;
TempAuth->SuspendExpires = TempUser->SuspendExpires;
TempAuth->SuspendedBy = TempUser->SuspendedBy;

authList.push_back( TempAuth ) ;
return true;
}    

bool ccontrol::deAuthUser( const string& Numeric)
{
AuthInfo *TempAuth = IsAuth(Numeric);
if(TempAuth)
	{
	authList.erase( std::find( authList.begin(),
		authList.end(),
		TempAuth ) ) ;

	delete TempAuth ; 
	}
return true;
}

bool ccontrol::UserGotMask( User* user, const string& Host )
{
static const char *Main = "SELECT host FROM hosts WHERE user_id = ";

strstream theQuery;
theQuery	<< Main
		<< user->Id
		<< ';'
		<< ends;

elog	<< "ccontrol::UserGotMask> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK == status )
	{
	for( int i = 0 ; i < SQLDb->Tuples() ; i++ )
		{
		if(match(SQLDb->GetValue(i,0),Host) == 0)
			{
			return true ;
			}
		}
	}
return false ;
}

bool ccontrol::UserGotHost( User* user, const string& Host )
{
static const char *Main = "SELECT host FROM hosts WHERE user_id = ";

strstream theQuery;
theQuery	<< Main
		<< user->Id
		<< ';'
		<< ends;

elog	<< "ccontrol::UserGotHost> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK == status )
	{
	for( int i = 0 ; i < SQLDb->Tuples() ; i++ )
		{
		if(!strcasecmp(SQLDb->GetValue(i,0),Host.c_str()))
			{
			return true ;
			}
		}
	}
return false ;
}

string ccontrol::CryptPass( const string& pass )
{
StringTokenizer st( pass ) ;
	
const char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.$*_";
	
string salt;

for ( unsigned short int i = 0 ; i < 8 ; i++ ) 
	{ 
	int randNo = 1+(int) (64.0*rand()/(RAND_MAX+1.0));
	salt += validChars[randNo]; 
	} 

/* Work out a MD5 hash of our salt + password */

md5	hash; // MD5 hash algorithm object.
md5Digest digest; // MD5Digest algorithm object.
 
strstream output;
string newPass;
newPass = salt + st.assemble(0);

hash.update( (const unsigned char *)newPass.c_str(), newPass.size() );
hash.report( digest );
	
/* Convert to Hex */
int data[ MD5_DIGEST_LENGTH ] = { 0 } ;
for( size_t ii = 0; ii < MD5_DIGEST_LENGTH; ii++ )
	{
	data[ii] = digest[ii];
	}

output << hex;
output.fill('0');
for( size_t ii = 0; ii < MD5_DIGEST_LENGTH; ii++ )
	{
	output << setw(2) << data[ii];
	}
output << ends;

return string( salt + output.str() );
}

bool ccontrol::validUserMask(const string& userMask) const
{

// Check that a '!' exists, and that the nickname
// is no more than 9 characters
StringTokenizer st1( userMask, '!' ) ;
if( (st1.size() != 2) || (st1[ 0 ].size() > 9) )
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

bool ccontrol::AddHost( User* user, const string& host )
{

static const char *Main = "INSERT into hosts (user_id,host) VALUES (";

strstream theQuery;
theQuery	<< Main
		<< user->Id <<",'"
		<< host << "')"
		<< ends;

elog	<< "ccontrol::AddHost> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccontrol::AddHost> SQL Error: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}
}

bool ccontrol::DelHost( User* user, const string& host )
{
//    strstream Condition;
//    Condition << "WHERE user_id = " << Id << ';';

static const char *Main = "DELETE FROM hosts WHERE user_id = ";

strstream theQuery;
theQuery	<< Main
		<< user->Id
		<< " And host = '"
		<< host << "'"
		<< ends;

elog	<< "ccontrol::DelHost> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccontrol::DelHost> SQL Error: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}
}

bool ccontrol::GetHelp( iClient* user, const string& command )
{
static const char *Main = "SELECT line,help FROM help WHERE lower(command) = '";

strstream theQuery;
theQuery	<< Main
		<< string_lower(command)
		<< "' ORDER BY line"
		<< ends;

elog	<< "ccontrol::GetHelp> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK == status )
	{
	if(SQLDb->Tuples() > 0 )
		{
		for( int i = 0 ; i < SQLDb->Tuples() ; i++ )
			{
			string commInfo = replace(
				SQLDb->GetValue( i, 1 ),
				"$BOT$",
				nickName ) ;

			if( commInfo.empty() )
				{
				elog	<< "ccontrol::GetHelp> Unable "
					<< "find $BOT$ in: "
					<< SQLDb->GetValue( i, 1 )
					<< ", when searching for "
					<< "help on command: "
					<< command
					<< endl ;
				}
			else
				{
				// All is well
				Notice( user, commInfo ) ;
				}
			} // for()
		} // if( SQLDb->Tuples() > 0 )
	else
		{
		Notice( user,
			"Couldnt find help for %s",
			command.c_str());
		}
	return true;
	}
else
	{
	elog	<< "ccontrol::GetHelp> SQL Error: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;	    
	}
}

string ccontrol::replace( const string& srcString,
	const string& from,
	const string& to )
{
string::size_type beginPos = srcString.find( from ) ;
if( string::npos == beginPos )
	{
	return string() ;
	}

string retMe( srcString.substr( 0, beginPos ) ) ;
retMe += to ;
retMe += srcString.substr( beginPos + from.size() ) ;

return retMe ;

}

} // namespace gnuworld
