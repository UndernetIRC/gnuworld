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
#include        "ccUser.h"
#include	"libpq++.h"
#include	"ccontrol.h"
#include	"AuthInfo.h"
#include        "server.h"
const char CControl_h_rcsId[] = __CCONTROL_H ;
const char CControl_cc_rcsId[] = "$Id: ccontrol.cc,v 1.35 2001/05/14 21:26:37 mrbean_ Exp $" ;

namespace gnuworld
{

using std::string ;
using std::vector ;
using std::cout ;
using std::endl ; 
using std::count ;

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

CCEmail = conf.Require( "ccemail" )->second;
AbuseMail = conf.Require( "abuse_mail" )->second;
inBurst = true;

string Query = "host=" + sqlHost + " dbname=" + sqlDb + " port=" + sqlPort;

elog	<< "ccontrol::ccontrol> Attempting to connect to "
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
	elog	<< "ccontrol::ccontrol> Unable to connect to SQL server."
		<< endl 
		<< "ccontrol::ccontrol> PostgreSQL error message: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	::exit( 0 ) ;
	}
else
	{
	elog	<< "ccontrol::ccontrol> Connection established to SQL "
		<< "server. Backend PID: "
		<< SQLDb->getPID()
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

Sendmail_Path = conf.Require("SendMail")->second;
SendReport = atoi(conf.Require("mail_report")->second.c_str());

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
RegisterCommand( new ADDNEWOPERCommand( this, "ADDOPER", "<USER> <OPERTYPE> <PASS> "
	"Add a new oper",flg_ADDNOP ) ) ;
RegisterCommand( new REMOVEOPERCommand( this, "REMOPER", "<USER> <PASS> "
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
RegisterCommand( new MODERATECommand( this, "MODERATE", "<#Channel> "
	"Moderate A Channel",flg_UNSUSPEND ) ) ;
RegisterCommand( new UNMODERATECommand( this, "UNMODERATE", "<#Channel> "
	"UNModerate A Channel",flg_UNSUSPEND ) ) ;
RegisterCommand( new OPCommand( this, "OP", "<#Channel> <nick> [nick] .. "
	"Op user(s) on a Channel",flg_OP ) ) ;
RegisterCommand( new DEOPCommand( this, "DEOP", "<#Channel> <nick> [nick] .. "
	"Deop user(s) on a Channel",flg_DEOP ) ) ;
RegisterCommand( new LISTHOSTSCommand( this, "LISTHOSTS", "<oper> "
	"Shows an oper hosts list",flg_LISTHOSTS ) ) ;
RegisterCommand( new CLEARCHANCommand( this, "CLEARCHAN", "<#chan> "
	"Removes all channel modes",flg_CLEARCHAN ) ) ;
RegisterCommand( new ADDNEWSERVERCommand( this, "ADDSERVER", "<Server> "
	"Add a new server to the bot database",flg_ADDSERVER ) ) ;
RegisterCommand( new LEARNNETWORKCommand( this, "LEARNNET", ""
	"Update the servers database according to the current situation",flg_ADDSERVER ) ) ;
RegisterCommand( new REMOVESERVERCommand( this, "REMSERVER", "<Server name>"
	"Removes a server from the bot database",flg_REMSERVER ) ) ;
RegisterCommand( new CHECKNETWORKCommand( this, "CHECKNET", ""
	"Checks if all known servers are in place",flg_CHECKNET ) ) ;
RegisterCommand( new LASTCOMCommand( this, "LASTCOM", "[number of lines to show]"
	"Post you the bot logs",flg_LASTCOM ) ) ;
RegisterCommand( new FORCEGLINECommand( this, "FORCEGLINE", "[duration (sec)] <user@host> <reason> "
	"Gline a given user@host for the given reason",flg_GLINE ) ) ;

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
assert( newComm != NULL ) ;

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

if(SendReport)
	{
	struct tm Now = convertToTmTime(::time(0));
	time_t theTime = ::time(0) + ((24 - Now.tm_hour)*3600 - (Now.tm_min)*60) ; //Set the daily timer to 24:00
	postDailyLog = theServer->RegisterTimer(theTime, this, NULL); 
	}

theServer->RegisterEvent( EVT_KILL, this );
theServer->RegisterEvent( EVT_QUIT, this );
theServer->RegisterEvent( EVT_NETJOIN, this );
theServer->RegisterEvent( EVT_EB, this );
//theServer->RegisterEvent( EVT_NETBREAK, this );

xClient::ImplementServer( theServer ) ;
}

/*int ccontrol::OnPrivateMessage( iClient* theClient, const string& Message,
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
//StringTokenizer st( Message ) ;

// Make sure there is a command present
/*if( st.empty() )
	{
	Notice( theClient, "Incomplete command" ) ;
	return 0 ;
	}*/

// This is no longer necessary, but oh well *shrug*
//const string Command = string_upper( st[ 0 ] ) ;

// Attempt to find a handler for this method.
//commandIterator commHandler = findCommand( Command ) ;

// Was a handler found?
//if( commHandler == command_end() )
//	{
	// Nope, notify the client
//	Notice( theClient, "Unknown command" ) ;
//	return 0 ; 
//	}

// Check if the user is logged in , and he got
// access to that command

/*int ComAccess = commHandler->second->getFlags();

AuthInfo* theUser = IsAuth(theClient->getCharYYXXX());

if((!theUser) && (ComAccess != flg_NOLOG))
	{
	Notice( theClient,
		"You must be logged in to issue that command" ) ;
	}
else if( (ComAccess != flg_NOLOG) && !(ComAccess & theUser->Access))
	{
	Notice( theClient, "You dont have access to that command" ) ;
	}
else if(( isSuspended(theUser) ) && ( ComAccess != 0 ) )
/*if( (theUser) && (theUser->Flags & isSUSPENDED))
	{
	if( (::time( 0 ) - theUser->SuspendExpires < 0)
		&& (ComAccess != 0))*/
/*		{
		Notice( theClient,
			"Sorry but you are suspended");
		}
	else 
		{*/
		/*{ //if the suspend expired, unsuspend the user and execute the command
		if( ::time( 0 ) - theUser->SuspendExpires >= 0) 
			{	
//			ccUser* tmpUser = GetUser(theUser->Name);
			ccUser* tmpUser = GetOper(theUser->Name);

			tmpUser->setSuspendExpires(0);
			tmpUser->removeFlag(isSUSPENDED);
			tmpUser->setSuspendedBy("");
			tmpUser->Update();
    			delete tmpUser;
			}*/
		// Log the command
/*		if(!(ComAccess & flg_NOLOG)) //Dont log command which arent suppose to be logged
			DailyLog(theUser,Message.c_str());*/
		// Execute the command handler
//		commHandler->second->Exec( theClient, Message) ;
		//}
//		}		
//else
//	{	
	// Log the command
//	if(!(ComAccess & flg_NOLOG)) //Dont log command which arent suppose to be logged
//		DailyLog(theUser,Message.c_str());
//	commHandler->second->Exec( theClient, Message) ;
//	}
// Call the base class OnPrivateMessage() method
//return xClient::OnPrivateMessage( theClient, Message ) ;
//}*/
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
commandIterator commHandler = findCommand( Command ) ;

// Was a handler found?
if( commHandler == command_end() )
	{
	// Nope, notify the client
	Notice( theClient, "Unknown command" ) ;
	return 0 ; 
	}

// Check if the user is logged in , and he got
// access to that command

int ComAccess = commHandler->second->getFlags();

bool ShouldntLog = ComAccess & flg_NOLOG;

ComAccess &= ~flg_NOLOG;

AuthInfo* theUser = IsAuth(theClient->getCharYYXXX());

if((!theUser) && (ComAccess))
	{
	Notice( theClient,
		"You must be logged in to issue that command" ) ;
	}
else if( (ComAccess) && !(ComAccess & theUser->Access))
	{
	Notice( theClient, "You dont have access to that command" ) ;
	}
else if(( isSuspended(theUser) ) && ( ComAccess ) )
		{
		Notice( theClient,
			"Sorry but you are suspended");
		}
	else 
		{
		// Log the command
		if(!ShouldntLog) //Dont log command which arent suppose to be logged
			DailyLog(theUser,Message.c_str());
		// Execute the command handler
		commHandler->second->Exec( theClient, Message) ;
		}		
//else
//	{	
	// Log the command
//	if(!(ComAccess & flg_NOLOG)) //Dont log command which arent suppose to be logged
//		DailyLog(theUser,Message.c_str());
//	commHandler->second->Exec( theClient, Message) ;
//	}
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

		AuthInfo *TempAuth = IsAuth(tmpUser);
		if(TempAuth)
	    	    deAuthUser(tmpUser->getCharYYXXX());
		
		break ;
		} // case EVT_KILL/case EVT_QUIT
	
	case EVT_NETJOIN:
		{
		/*
		 * We need to update the servers table about the new
		 * server , and check if we know it
		 *
		 */
		iServer* NewServer = static_cast< iServer* >( Data1);
		iServer* UplinkServer = static_cast< iServer* >( Data2);
		ccServer* CheckServer = new (nothrow) ccServer(SQLDb);
		assert(CheckServer != NULL);
		if(CheckServer->loadNumericData(NewServer->getCharYY()))
			{
			if(strcasecmp(NewServer->getName(),CheckServer->get_Name()))
				{
				strstream s ;
				s	<< getCharYY() << " WA :"
    					<< "\002Database numeric collision warnning!\002 - "
					<< NewServer->getName()
					<< " != "
					<< CheckServer->get_Name()
					<< ends ;
				Write( s ) ;
				delete[] s.str();
				}
			}				
		if(!CheckServer->loadData(NewServer->getName()))
			{    	
			MsgChanLog("Unknown server connected : %s Uplink : %s\n"
				    ,NewServer->getName().c_str(),UplinkServer->getName().c_str());
			}
		else
			{
			CheckServer->set_LastConnected(::time (0));
			CheckServer->set_Uplink(UplinkServer->getName());
			CheckServer->set_LastNumeric(NewServer->getCharYY());
			CheckServer->Update();
			}
		delete CheckServer;
		break;
		}
	case EVT_NETBREAK:
		{
		/*iServer* NewServer = static_cast< iServer* >( Data1);
		iServer* UplinkServer = static_cast< iServer* >( Data2);*/
		// still not handled
		}
	case EVT_EB:
		{
		inBurst = true;
		}	
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

int ccontrol::OnTimer(xServer::timerID timer_id, void*)
{

if (timer_id ==  postDailyLog)
	{ 
	CreateReport(::time(0) - 24*3600,::time(0)); //Create the lastcom report 
	MailReport(AbuseMail.c_str(),"Report.log"); //Email the report to the abuse team
	/* Refresh Timers */			
	time_t theTime = time(NULL) + 24*3600; 
	postDailyLog = MyUplink->RegisterTimer(theTime, this, NULL); 
	}
return 1;
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
assert( theChan != 0 ) ;

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
assert( theChan != NULL ) ;

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
	// Dont kick opers and +k ppl
	if(( !ptr->second->isOper() ) && ( !ptr->second->getClient()->getMode(iClient::MODE_SERVICES) ))
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

AuthInfo* ccontrol::IsAuth( const iClient* theClient ) const
{
assert( theClient != 0 ) ;
return IsAuth( theClient->getCharYYXXX() ) ;
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
return NULL ;
}

AuthInfo* ccontrol::IsAuth( const ccUser* theUser ) const
{
assert( theUser != 0 ) ;
return IsAuth( theUser->getID() ) ;
}
   
AuthInfo* ccontrol::IsAuth( const unsigned int UserId ) const
{
for( authListType::const_iterator ptr = authList.begin() ;
	ptr != authList.end() ; ++ptr )
	{
	if( (*ptr)->Id == UserId )
		{
		return *ptr ;
		}
	}
return NULL ;
}

void ccontrol::UpdateAuth(ccUser* TempUser)
{
AuthInfo* TempAuth = IsAuth(TempUser);

if(TempAuth)
	{
	//ccUser* TempUser = GetUser(Id);
	TempAuth->Id = TempUser->getID();
        TempAuth->Name = TempUser->getUserName();
        TempAuth->Access = TempUser->getAccess();
        TempAuth->Flags = TempUser->getFlags();
        TempAuth->SuspendExpires = TempUser->getSuspendExpires();
        TempAuth->SuspendedBy = TempUser->getSuspendedBy();
	}
}


bool ccontrol::AddOper (ccUser* Oper)
{
static const char *Main = "INSERT into opers (user_name,password,access,last_updated_by,last_updated,flags) VALUES ('";

strstream theQuery;
theQuery	<< Main
		<< Oper->getUserName() <<"','"
		<< Oper->getPassword() << "',"
		<< Oper->getAccess() << ",'"
		<< Oper->getLast_Updated_by()
		<< "',now()::abstime::int4,"
		<< Oper->getFlags() << ")"
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

elog	<< "ccontrol::AddOper> SQL Failure: "
	<< SQLDb->ErrorMessage()
	<< endl ;
return false;
}

bool ccontrol::DeleteOper (const string& Name)
{
//    strstream Condition;
//    Condition << "WHERE user_id = " << Id << ';';
ExecStatusType status;
ccUser* tUser = GetOper(Name.c_str());
//Delete the user hosts
if(tUser)
    {
    static const char *tMain = "DELETE FROM hosts WHERE User_Id = ";    
    strstream HostQ;
    HostQ << tMain;
    HostQ << tUser->getID();
    HostQ << ends;
    status = SQLDb->Exec( HostQ.str() ) ;
elog	<< "ccontrol::DeleteOper> "
	<< HostQ.str()
	<< endl; 

    delete[] HostQ.str();
if( PGRES_COMMAND_OK != status ) 
	{
	elog	<< "ccontrol::DeleteOper> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}
	delete tUser;
    }    

static const char *Main = "DELETE FROM opers WHERE lower(user_name) = '";

strstream theQuery;
theQuery	<< Main
		<< Name
		<< "'"
		<< ends;

elog	<< "ccontrol::DeleteOper> "
	<< theQuery.str()
	<< endl; 

status = SQLDb->Exec( theQuery.str() ) ;
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

/*bool ccontrol::UpdateOper (User* Oper)
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
}*/

int ccontrol::getCommandLevel( const string& Command)
{
commandIterator commHandler = findCommand( Command ) ;

// Was a handler found?
if( commHandler != command_end() )
	{
	return commHandler->second->getFlags() ;
	}

return -1 ;
}	

bool ccontrol::AuthUser( ccUser* TempUser)
{
AuthInfo *TempAuth = new (nothrow) AuthInfo;
assert( TempAuth != 0 ) ;

TempAuth->Id = TempUser->getID();
TempAuth->Name = TempUser->getUserName();
TempAuth->Access = TempUser->getAccess();
TempAuth->Flags = TempUser->getFlags();
TempAuth->Numeric = TempUser->getNumeric();
TempAuth->SuspendExpires = TempUser->getSuspendExpires();
TempAuth->SuspendedBy = TempUser->getSuspendedBy();

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

bool ccontrol::UserGotMask( ccUser* user, const string& Host )
{
static const char *Main = "SELECT host FROM hosts WHERE user_id = ";

strstream theQuery;
theQuery	<< Main
		<< user->getID()
		<< ';'
		<< ends;

elog	<< "ccontrol::UserGotMask> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::UserGotMask> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false ;
	}

for( int i = 0 ; i < SQLDb->Tuples() ; i++ )
	{
	if(match(SQLDb->GetValue(i,0),Host) == 0)
		{
		return true ;
		}
	}

return false ;
}

bool ccontrol::UserGotHost( ccUser* user, const string& Host )
{
static const char *Main = "SELECT host FROM hosts WHERE user_id = ";

strstream theQuery;
theQuery	<< Main
		<< user->getID()
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

bool ccontrol::AddHost( ccUser* user, const string& host )
{

static const char *Main = "INSERT into hosts (user_id,host) VALUES (";

strstream theQuery;
theQuery	<< Main
		<< user->getID() <<",'"
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

bool ccontrol::DelHost( ccUser* user, const string& host )
{

static const char *Main = "DELETE FROM hosts WHERE user_id = ";

strstream theQuery;
theQuery	<< Main
		<< user->getID()
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

bool ccontrol::listHosts( ccUser* User, iClient* theClient )
{
static const char* queryHeader
	= "SELECT host FROM hosts WHERE user_id =  ";

strstream theQuery;
theQuery	<< queryHeader 
		<< User->getID()
		<< ends;

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "LISTHOSTS> SQL Error: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

// SQL Query succeeded
Notice(theClient,"Host list for %s",User->getUserName().c_str());
for (int i = 0 ; i < SQLDb->Tuples(); i++)
	{
	Notice(theClient,SQLDb->GetValue(i, 0));
	}
return true;
}	
	    

bool ccontrol::GetHelp( iClient* user, const string& command )
{
static const char *Main = "SELECT line,help FROM help WHERE lower(command) = '";

strstream theQuery;
theQuery	<< Main
		<< string_lower(command)
		<< "' and lower(subcommand) = "
		<< "NULL"
		<< " ORDER BY line"
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
		DoHelp(user);
		}
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

bool ccontrol::GetHelp( iClient* user, const string& command , const string& subcommand)
{
static const char *Main = "SELECT line,help FROM help WHERE lower(command) = '";

strstream theQuery;
theQuery	<< Main
		<< string_lower(command)
		<< "' and lower(subcommand) = '"
		<< string_lower(subcommand)
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
		DoHelp(user);
		}
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


void ccontrol::DoHelp(iClient* theClient)
{
for( int i = 0 ; i < SQLDb->Tuples() ; i++ )
	{
	string commInfo = replace(
		SQLDb->GetValue( i, 1 ),
		"$BOT$",
		nickName ) ;
	Notice(theClient,commInfo);
	} // for()
}
	
string ccontrol::replace( const string& srcString,
	const string& from,
	const string& to )
{
string retMe( srcString ) ;
string::size_type beginPos = 0 ;

while((beginPos = retMe.find(from)) <= retMe.size())
	{
	retMe.replace(beginPos,from.size(),to);
	}

return retMe ;

}

ccUser* ccontrol::GetOper( const string Name)
{
ccUser* tmpUser = new (nothrow) ccUser(SQLDb);
assert( tmpUser != 0 ) ;

if( !tmpUser->loadData(Name) )
	{
	delete tmpUser ; 
	tmpUser = 0 ;
	}
return tmpUser ;
}

ccUser* ccontrol::GetOper( unsigned int ID)
{
ccUser* tmpUser = new (nothrow) ccUser(SQLDb);
assert( tmpUser != 0 ) ;

if( !tmpUser->loadData(ID) )
	{
	delete tmpUser ; 
	tmpUser = 0 ;
	}
return tmpUser ;
}

bool ccontrol::addGline( ccGline* TempGline)
{

glineList.push_back( TempGline ) ;
return true;
}    


bool ccontrol::remGline( ccGline* TempGline)
{
glineList.erase( std::find( glineList.begin(),
glineList.end(),
TempGline ) ) ;
delete TempGline ; 
return true;
}

ccGline* ccontrol::findMatchingGline( const string& Host )
{
static const char *Main = "SELECT Id,Host FROM glines";

strstream theQuery;
theQuery	<< Main
		<< ends;

elog	<< "ccontrol::findMatchingGline> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::findMatchingGline> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return NULL ;
	}

for( int i = 0 ; i < SQLDb->Tuples() ; i++ )
	{
	if(match(SQLDb->GetValue(i,2),Host) == 0)
		{
		ccGline *tempGline = new (nothrow) ccGline(SQLDb);
		if(tempGline->loadData(atoi(SQLDb->GetValue(i,1))))
		    return tempGline;
		else
		    return NULL;
		}
	}

return NULL ;
}

struct tm ccontrol::convertToTmTime(time_t NOW)
{
time_t *tNow;
tNow = &NOW;
struct tm* Now;
Now = gmtime(tNow);
return *Now;
}

char *ccontrol::convertToAscTime(time_t NOW)
{
time_t *tNow = &NOW;
struct tm* Now;
Now = gmtime(tNow);
char *ATime = asctime(Now);
ATime[strlen(ATime)-1] = '\0';
return ATime;
}

bool ccontrol::MsgChanLog(const char *Msg, ... )
{
if(!Network->findChannel(msgChan))
	return false;
char buffer[ 1024 ] = { 0 } ;
va_list list;

va_start( list, Msg ) ;
vsprintf( buffer, Msg, list ) ;
va_end( list ) ;

if(Message(Network->findChannel(msgChan),buffer));
for( authListType::const_iterator ptr = authList.begin() ;
        ptr != authList.end() ; ++ptr )
        {
        if((*ptr)->Flags & getLOGS )
                { 
                if(Message(Network->findClient((*ptr)->Numeric),buffer));
                }
	}
return true;
}

bool ccontrol::DailyLog(AuthInfo* Oper, const char *Log, ... )
{

char buffer[ 1024 ] = { 0 } ;
va_list list;

va_start( list, Log ) ;
vsprintf( buffer, Log, list ) ;
va_end( list ) ;
iClient *theClient = Network->findClient(Oper->Numeric);

static const char *Main = "INSERT into comlog (ts,oper,command) VALUES (now()::abstime::int4,'";

strstream theQuery;
theQuery	<< Main
		<< Oper->Name 
		<< " (" << theClient->getNickUserHost() <<")','"
		<< buffer << "')"
		<< ends;

elog	<< "ccontrol::ComLog> "
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
	elog	<< "ccontrol::comLog> SQL Error: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}

return true;
}

bool ccontrol::CreateReport(time_t From, time_t Til)
{

static const char* queryHeader = "SELECT * FROM comlog where ts >";
strstream theQuery;
theQuery 	<< queryHeader 
		<< From
		<< " AND ts < "
		<< Til
		<< " ORDER BY ts DESC"
		<< ends;
	
elog	<< "LASTCOM> " 
	<< theQuery.str() 
	<< endl;
	
ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "LASTCOM> SQL Error: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

// SQL Query succeeded
ofstream LogFile;
LogFile.open("Report.log",ios::out);
if(!LogFile)
	return false;
LogFile << "ccontrol log for command issued between " << convertToAscTime(From); 
LogFile << " and up til " << convertToAscTime(Til) << endl;

for (int i = 0 ; i < SQLDb->Tuples(); i++)
	{
	LogFile << "[ " << convertToAscTime(atoi(SQLDb->GetValue(i, 0))) << " - " << SQLDb->GetValue(i,1) << " ] " << SQLDb->GetValue(i,2) << endl;
	}

LogFile << "End of debug log" << '\n';
LogFile.close();
return true;
}

bool ccontrol::MailReport(const char *MailTo, char *ReportFile)
{

ifstream Report;
Report.open(Sendmail_Path.c_str(),ios::in);

if(!Report)
	{
	MsgChanLog("Error cant find sendmail, check the conf setting and
try again\n");
	return false;
	}
Report.close();

Report.open(ReportFile,ios::in);
if(!Report)
	{
	MsgChanLog("Error while sending report\n");
	return false;
	}
ofstream TMail;
TMail.open("Report.mail",ios::out);
if(!TMail)
	{
	MsgChanLog("Error while sending report\n");
	return false;
	}
TMail << "Subject : CControl command log report\n";

char Buffer[1024];

while(!Report.eof())
	{
	Report.getline(Buffer,sizeof(Buffer),'\n');	
	TMail << Buffer << endl;
	}
TMail.close();
Report.close();
char SendMail[256];

sprintf(SendMail, "%s -f %s %s < Report.mail\n",Sendmail_Path.c_str(),CCEmail.c_str(),
MailTo);
system(SendMail);
return true;
}

int ccontrol::CheckGline(const char * GlineHost,unsigned int Len)
{

char *User;
char *Host;
char *TPos;
TPos = strchr(GlineHost,'@');
User = new char[(TPos - GlineHost) + 1];			
Host = new char[(GlineHost + strlen(GlineHost)) - TPos +1];
strncpy(User,GlineHost,TPos - GlineHost);
User[TPos - GlineHost] = '\0';
strcpy(Host,TPos+1);
if((countCinS(Host,'*') > 1) || (!strcasecmp(Host,"*")))
    return HUH_NO_HOST;
if(Network->countHost(GlineHost) > 256)
    return HUH_NO_USERS;
else if(Network->countHost(GlineHost) > 32)
    return FORCE_NEEDED_USERS;
if(Len > 24*3600*2) //Longer than 2 days ? 
	return FORCE_NEEDED_TIME;
if(strchr(Host,'*') == NULL)
	return GLINE_OK;
return FORCE_NEEDED_HOST;
}

bool ccontrol::isSuspended(AuthInfo *theUser)
{
if( (theUser) && (theUser->Flags & isSUSPENDED))
	{
	if(::time( 0 ) - theUser->SuspendExpires < 0)
		{
		return true;
		}
	else 
		{ //if the suspend expired, unsuspend the user and execute the command
		MsgChanLog("unsuspending %s\n",theUser->Name.c_str());
		ccUser* tmpUser = GetOper(theUser->Name);
		tmpUser->setSuspendExpires(0);
		tmpUser->removeFlag(isSUSPENDED);
		tmpUser->setSuspendedBy("");
		tmpUser->Update();
		delete tmpUser;
		}
	}
return false;
}

int ccontrol::countCinS(char *St,char Sign)
{
int count =0;
char *CurC;
for(CurC = St;*CurC != '\0';CurC++)
    if(*CurC == Sign)
	count++;
return count;
}
 
} // namespace gnuworld
