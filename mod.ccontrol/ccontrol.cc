/* ccontrol.cc
 * Authors: Daniel Karrels dan@karrels.com
	    Tomer Cohen    MrBean@toughguy.net
 */

#include	<new>
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
const char CControl_cc_rcsId[] = "$Id: ccontrol.cc,v 1.57 2001/07/18 06:42:35 mrbean_ Exp $" ;

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

inBurst = true;
inRefresh = false;

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

// CCEmail is the email ccontrol will post the last com report under
CCEmail = conf.Require( "ccemail" )->second;

//AbuseMail is the mail that the lastcom report will be post to
AbuseMail = conf.Require( "abuse_mail" )->second;

//GLInterval is the inteval in which ccontrol will check for expired glines
GLInterval = atoi( conf.Require( "gline_interval" )->second.c_str() );

//Sendmail  is the full path of the sendmail program
Sendmail_Path = conf.Require("SendMail")->second;

//SendReport flag that tells ccontrol if the user want the report to be mailed
SendReport = atoi(conf.Require("mail_report")->second.c_str());

userMaxConnection = atoi(conf.Require("max_connection")->second.c_str());

maxGlineLen = atoi(conf.Require("max_GLen")->second.c_str());

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
RegisterCommand( new GLINECommand( this, "GLINE", "<user@host> <duration>[time units (s,d,h)] <reason> "
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
RegisterCommand( new ADDNEWOPERCommand( this, "ADDUSER", "<USER> <OPERTYPE> <PASS> "
	"Add a new oper",flg_ADDNOP ) ) ;
RegisterCommand( new REMOVEOPERCommand( this, "REMUSER", "<USER> <PASS> "
	"Remove an oper",flg_REMOP ) ) ;
RegisterCommand( new ADDCOMMANDCommand( this, "ADDCOMMAND", "<USER> <COMMAND> "
	"Add a new command to an oper",flg_ADDCMD ) ) ;
RegisterCommand( new REMOVECOMMANDCommand( this, "REMCOMMAND", "<USER> <COMMAND> "
	"Remove a command from oper",flg_DELCMD ) ) ;
RegisterCommand( new NEWPASSCommand( this, "NEWPASS", "<PASSWORD> "
	"Change password",flg_NEWPASS ) ) ;
RegisterCommand( new SUSPENDOPERCommand( this, "SUSPEND", "<OPER> <DURATION> <TIME UNITS>"
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
	"Update the servers database according to the current situation",flg_LEARNNET ) ) ;
RegisterCommand( new REMOVESERVERCommand( this, "REMSERVER", "<Server name>"
	"Removes a server from the bot database",flg_REMSERVER ) ) ;
RegisterCommand( new CHECKNETWORKCommand( this, "CHECKNET", ""
	"Checks if all known servers are in place",flg_CHECKNET ) ) ;
RegisterCommand( new LASTCOMCommand( this, "LASTCOM", "[number of lines to show]"
	"Post you the bot logs",flg_LASTCOM ) ) ;
RegisterCommand( new FORCEGLINECommand( this, "FORCEGLINE", "[duration (sec)] <user@host> <reason> "
	"Gline a given user@host for the given reason",flg_FGLINE ) ) ;
RegisterCommand( new EXCEPTIONCommand( this, "EXCEPTIONS", "(list / add / del) [host mask]"
	"Add connection exceptions on hosts",flg_EXCEPTIONS ) ) ;
RegisterCommand( new LISTIGNORESCommand( this, "LISTIGNORES", ""
	"List the ignore list",flg_LISTIGNORES ) ) ;
RegisterCommand( new REMOVEIGNORECommand( this, "REMIGNORE", "(nick/host)"
	" Removes a host/nick from the  ignore list",flg_REMIGNORE ) ) ;

RegisterCommand( new LISTCommand( this, "LIST", "(glines)"
	" Get all kinds of lists from the bot",flg_LIST ) ) ;

loadGlines();
loadExceptions();

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
// Deallocate each gline entry
for(glineIterator GLptr = glineList.begin(); GLptr != glineList.end(); ++GLptr)
	{
	glineList.erase(GLptr);
	}

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

expiredGlines = theServer->RegisterTimer(::time(0) + GLInterval,this,NULL);
expiredIgnores = theServer->RegisterTimer(::time(0) + 60,this,NULL);

if(SendReport)
	{
	struct tm Now = convertToTmTime(::time(0));
	time_t theTime = ::time(0) + ((24 - Now.tm_hour)*3600 - (Now.tm_min)*60) ; //Set the daily timer to 24:00
	postDailyLog = theServer->RegisterTimer(theTime, this, NULL); 
	}

theServer->RegisterEvent( EVT_KILL, this );
theServer->RegisterEvent( EVT_QUIT, this );
theServer->RegisterEvent( EVT_NETJOIN, this );
theServer->RegisterEvent( EVT_BURST_CMPLT, this );
theServer->RegisterEvent( EVT_GLINE , this );
theServer->RegisterEvent( EVT_NICK , this );

//theServer->RegisterEvent( EVT_NETBREAK, this );

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
else if( (ComAccess) && !(ComAccess & theUser->get_Access()))
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
		ccLogin *tempLogin = findLogin(tmpUser->getCharYYXXX());
		if(tempLogin)
			{
			removeLogin(tempLogin);
			if(tempLogin->get_IgnoredHost() != "")
				{
				tempLogin->set_Numeric("0");
				tempLogin->resetLogins();
				}
			else
				{
				delete tempLogin;
				}		
			}
			
		break ;
		} // case EVT_KILL/case EVT_QUIT
	
	case EVT_NETJOIN:
		{
		inBurst = true;
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
		break;
		}
	case EVT_BURST_CMPLT:
		{
		inBurst = false;
		refreshGlines();
		burstGlines();
		break;
		}	
	case EVT_NICK:
		{
		iClient* NewUser = static_cast< iClient* >( Data1);
		int CurConnections = Network->countHost(NewUser->getInsecureHost());		
		if(CurConnections  > getExceptions("*@" + NewUser->getInsecureHost()))
			{
			ccGline *tmpGline;
			tmpGline = findGline("*@" + NewUser->getInsecureHost()); 
			if(!tmpGline)
				{
				tmpGline = new ccGline(SQLDb);
				tmpGline->set_Host("*@" + NewUser->getInsecureHost());
				tmpGline->set_Expires(::time(0) + maxGlineLen);
				tmpGline->set_Reason("Automatically banned for exccessive connections");
				tmpGline->set_AddedOn(::time(0));
				tmpGline->Insert();
				wallopsAsServer("Adding gline on %s for %s",tmpGline->get_Host().c_str(),tmpGline->get_Reason().c_str());
				tmpGline->loadData(tmpGline->get_Host());
				addGline(tmpGline);
				}
			else
				wallopsAsServer("Refreshing gline time on %s for %s",tmpGline->get_Host().c_str(),tmpGline->get_Reason().c_str());
			MyUplink->setGline( nickName,
					tmpGline->get_Host(),
					tmpGline->get_Reason(),
					tmpGline->get_Expires() - ::time(0) ) ;
			
			}
		else if(!inBurst)
			{	
			ccGline * tempGline = findMatchingGline(NewUser->getUserName() + '@' + NewUser->getInsecureHost());
			if((tempGline) && (tempGline->get_Expires() > ::time(0)))
				{
				addGline(tempGline);
				MyUplink->setGline(tempGline->get_AddedBy()
				,tempGline->get_Host(),tempGline->get_Reason()
				,tempGline->get_Expires() - ::time(0));
				}
			}			
		break;
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
	// Create the lastcom report 
	CreateReport(::time(0) - 24*3600,::time(0));

	// Email the report to the abuse team
	MailReport(AbuseMail.c_str(),"Report.log");

	/* Refresh Timers */			
	time_t theTime = time(NULL) + 24*3600; 
	postDailyLog = MyUplink->RegisterTimer(theTime, this, NULL); 
	}
else if (timer_id == expiredGlines)
	{
	refreshGlines();
	expiredGlines = MyUplink->RegisterTimer(::time(0) + GLInterval,
		this,NULL);
	}
else if (timer_id == expiredIgnores)
	{
	refreshIgnores();
	expiredIgnores = MyUplink->RegisterTimer(::time(0) + 60,this,NULL);
	}

return true;
}

bool ccontrol::isOperChan( const string& theChan ) const
{
vector< string >::const_iterator ptr = operChans.begin(),
	end = operChans.end() ;
while( ptr != end )
	{
	if( !strcasecmp( *ptr, theChan ) )
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
	if( !strcasecmp( (*ptr)->get_Numeric(), Numeric ) )
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
	if( (*ptr)->get_Id() == UserId )
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
	TempAuth->set_Id(TempUser->getID());
        TempAuth->set_Name(TempUser->getUserName());
        TempAuth->set_Access(TempUser->getAccess());
        TempAuth->set_Flags(TempUser->getFlags());
        TempAuth->set_SuspendExpires(TempUser->getSuspendExpires());
        TempAuth->set_SuspendedBy(TempUser->getSuspendedBy());
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

TempAuth->set_Id(TempUser->getID());
TempAuth->set_Name(TempUser->getUserName());
TempAuth->set_Access(TempUser->getAccess());
TempAuth->set_Flags(TempUser->getFlags());
TempAuth->set_Numeric(TempUser->getNumeric());
TempAuth->set_SuspendExpires(TempUser->getSuspendExpires());
TempAuth->set_SuspendedBy(TempUser->getSuspendedBy());

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
return true;
}

ccGline* ccontrol::findMatchingGline( const string& Host )
{
ccGline *theGline = 0;
for(glineIterator ptr = glineList.begin(); ptr != glineList.end(); ++ptr)
	{
	theGline = *ptr;
	if(match(theGline->get_Host(),Host) == 0) 
		{
    		if(theGline->get_Expires() > ::time(0))
			{
			return theGline;
			}
		}
	}

return NULL ;
}

ccGline* ccontrol::findGline( const string& HostName )
{
ccGline *theGline;
for(glineIterator ptr = glineList.begin(); ptr != glineList.end();ptr++)
	{
	theGline = *ptr;
    	if(theGline->get_Host() == HostName)
		if(theGline->get_Expires() > ::time(0))
			return theGline;
	}

return NULL ;
}

struct tm ccontrol::convertToTmTime(time_t NOW)
{
return *gmtime(&NOW);
}

// TODO: This method should never exist
char *ccontrol::convertToAscTime(time_t NOW)
{
time_t *tNow = &NOW;
struct tm* Now = gmtime(tNow);
char *ATime = asctime(Now);
ATime[strlen(ATime)-1] = '\0';
return ATime;
}

bool ccontrol::MsgChanLog(const char *Msg, ... )
{
if(!Network->findChannel(msgChan))
	{
	return false;
	}

char buffer[ 1024 ] = { 0 } ;
va_list list;

va_start( list, Msg ) ;
vsprintf( buffer, Msg, list ) ;
va_end( list ) ;

Message(Network->findChannel(msgChan),buffer);

for( authListType::const_iterator ptr = authList.begin() ;
        ptr != authList.end() ; ++ptr )
        {
        if((*ptr)->get_Flags() & getLOGS )
                { 
                Message(Network->findClient((*ptr)->get_Numeric()),buffer);
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
iClient *theClient = Network->findClient(Oper->get_Numeric());
buffer[512]= '\0';
static const char *Main = "INSERT into comlog (ts,oper,command) VALUES (now()::abstime::int4,'";

strstream theQuery;
theQuery	<< Main
		<< Oper->get_Name() 
		<< " (" << theClient->getNickUserHost() <<")','"
		<< buffer << "')"
		<< ends;

elog	<< "ccontrol::DailyLog> "
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
	elog	<< "ccontrol::DailyLog> SQL Error: "
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
	
elog	<< "ccontrol::CreateReport> " 
	<< theQuery.str() 
	<< endl;
	
ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::CreateReport> SQL Error: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

// SQL Query succeeded
ofstream LogFile;
LogFile.open("Report.log",ios::out);
if(!LogFile)
	{
	return false;
	}

LogFile	<< "ccontrol log for command issued between "
	<< convertToAscTime(From)
	<< " and up til "
	<< convertToAscTime(Til)
	<< endl;

for (int i = 0 ; i < SQLDb->Tuples(); i++)
	{
	LogFile	<< "[ "
		<< convertToAscTime(atoi(SQLDb->GetValue(i, 0)))
		<< " - "
		<< SQLDb->GetValue(i,1)
		<< " ] "
		<< SQLDb->GetValue(i,2)
		<< endl;
	}

LogFile << "End of debug log"
	<< endl ;

LogFile.close();
return true;
}

bool ccontrol::MailReport(const char *MailTo, char *ReportFile)
{

ifstream Report;
Report.open(Sendmail_Path.c_str(),ios::in);

if(!Report)
	{
	MsgChanLog("Error cant find sendmail, check the conf setting and"
		" try again\n");
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

string line ;

while( std::getline( Report, line ) )
	{
	TMail	<< line
		<< endl ;
	}
TMail.close();
Report.close();
char SendMail[256] = { 0 };

sprintf(SendMail, "%s -f %s %s < Report.mail\n",
	Sendmail_Path.c_str(),
	CCEmail.c_str(),
	MailTo);

system(SendMail);
return true;
}

// TODO: This method should be in C++, not C!
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
int TotalHost = Network->countMatchingUserHost(GlineHost);
if(TotalHost > 256)
    return HUH_NO_USERS;
else if(TotalHost > 32)
    return FORCE_NEEDED_USERS;
if(Len > 24*3600*2) //Longer than 2 days ? 
	return FORCE_NEEDED_TIME;
if(strchr(Host,'*') == NULL)
	return GLINE_OK;
return FORCE_NEEDED_HOST;
}

bool ccontrol::isSuspended(AuthInfo *theUser)
{
if( (theUser) && (theUser->get_Flags() & isSUSPENDED))
	{
	//Check if the suspend hadnt already expired
	if(::time( 0 ) - theUser->get_SuspendExpires() < 0)
		{
		return true;
		}
	}
return false;
}

// TODO: This method should be in C++ not C!
int ccontrol::countCinS(char *St,char Sign)
{
int count =0;
char *CurC;
for(CurC = St;*CurC != '\0';CurC++)
    if(*CurC == Sign)
	count++;
return count;
}

bool ccontrol::refreshGlines()
{

int totalFound = 0;
inRefresh = true;

for(glineIterator ptr = glineList.begin();ptr != glineList.end();) 
	{
	if((*ptr)->get_Expires() <= ::time(0))
		{
		//remove the gline from the core
		MyUplink->removeGline((*ptr)->get_Host());
		//remove the gline from ccontrol structure
		//finally remove the gline from the database
		(*ptr)->Delete();
		ptr = glineList.erase(ptr);
		totalFound++;
		}
	else
		ptr++;
	}

inRefresh = false;

return true;

}

bool ccontrol::burstGlines()
{

ccGline *theGline = 0 ;
for(glineIterator ptr = glineList.begin(); ptr != glineList.end(); ptr++)
	{
	theGline = *ptr;
	MyUplink->setGline(theGline->get_AddedBy(),
		theGline->get_Host(),
		theGline->get_Reason(),
		theGline->get_Expires() - ::time(0));
	}

return true;
}

bool ccontrol::loadGlines()
{
//static const char *Main = "SELECT * FROM glines where ExpiresAt > now()::abstime::int4";
static const char *Main = "SELECT * FROM glines";

strstream theQuery;
theQuery	<< Main
		<< ends;

elog	<< "ccontrol::loadGlines> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::loadGlines> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false;
	}

// BUG: This should crash, if only this variable was initialized
// as it should be!
ccGline *tempGline = NULL;

inRefresh = true;

for( int i = 0 ; i < SQLDb->Tuples() ; i++ )
	{
	tempGline =  new (nothrow) ccGline(SQLDb);
	assert( tempGline != NULL ) ;

	tempGline->set_Id(SQLDb->GetValue(i,0));
	tempGline->set_Host(SQLDb->GetValue(i,1));
	tempGline->set_AddedBy(SQLDb->GetValue(i,2)) ;
	tempGline->set_AddedOn(static_cast< time_t >( atoi( SQLDb->GetValue(i,3) ) )) ;
	tempGline->set_Expires(static_cast< time_t >( atoi( SQLDb->GetValue(i,4) ) )) ;
	tempGline->set_Reason(SQLDb->GetValue(i,5));
	addGline(tempGline);
	}
return true;	
} 

void ccontrol::wallopsAsServer(const char *Msg,...)
{
if( 0 == MyUplink )
	{
	return ;
	}

char buffer[ 1024 ] = { 0 } ;
va_list list;

va_start( list , Msg) ;
vsprintf( buffer, Msg , list ) ;
va_end( list ) ;

MyUplink->Wallops( buffer ) ;
}

int ccontrol::getExceptions( const string &Host )
{
int Exception = userMaxConnection;

for(exceptionIterator ptr = exception_begin();ptr != exception_end();ptr++)
	{
	if(*(*ptr) == Host)
		{
		MsgChanLog("Found an Exception for %d connections\n",(*ptr)->get_Connections()); 
		if((*ptr)->get_Connections() > Exception)
			{
			Exception = (*ptr)->get_Connections();
			}
		} 
	}

return Exception;
}

bool ccontrol::listExceptions( iClient *theClient )
{

Notice(theClient,"-= Exceptions list - listing a total of %d exceptions =-"
	,exceptionList.size());
	
for(exceptionIterator ptr = exception_begin();ptr != exception_end();ptr++)
	Notice(theClient,"Host : %s  Connections : %d AddedBy : %s"
	       ,(*ptr)->get_Host().c_str()
	       ,(*ptr)->get_Connections()
	       ,(*ptr)->get_AddedBy().c_str());

Notice(theClient,"-= End of exception list =-");

return true;
}

bool ccontrol::isException( const string & Host )
{
for(exceptionIterator ptr = exception_begin();ptr != exception_end();ptr++)
	{
	if(*(*ptr) == Host)
		return true;
	}
return false;
}


bool ccontrol::insertException( iClient *theClient , const string& Host , int Connections )
{
//Check if there is already an exception on that host
if(isException(Host))
	{
	Notice(theClient,
		"There is already an exception for host %s, "
		"please use update",
		Host.c_str());		
	return true;
	}

//Create a new ccException structure 
ccException* tempException = new (nothrow) ccException(SQLDb);
assert(tempException != NULL);

tempException->set_Host(Host);
tempException->set_Connections(Connections);
tempException->set_AddedBy(theClient->getNickUserHost());
tempException->set_AddedOn(::time(0));

//Update the database, and the internal list
if(!tempException->Insert())
	{
	return false;
	}

exceptionList.push_back(tempException);
return true;
}


bool ccontrol::delException( iClient *theClient , const string &Host )
{

if(!isException(Host))
	{
	Notice(theClient,"Cant find exception for host %s",Host.c_str());
	return true;
	}
ccException *tempException = NULL;

for(exceptionIterator ptr = exception_begin();ptr != exception_end();)
	{
	tempException = *ptr;
	if(*tempException == Host)
		{
		bool status = tempException->Delete();
		ptr = exceptionList.erase(ptr);
		delete tempException;
		if(!status)
			return false;
		}
	    
	else
		ptr++;
	}
return true;
}	

ccLogin *ccontrol::findLogin( const string & Numeric )
{
for(loginIterator ptr = login_begin() ; ptr != login_end() ; ++ptr)
	{
	if((*ptr)->get_Numeric() == Numeric)
		{
		return *ptr;
		}
	}
return NULL;
}

void ccontrol::removeLogin( ccLogin *tempLogin )
{
for(loginIterator ptr = login_begin() ; ptr != login_end() ;)
	{
	if((*ptr) == tempLogin)
		{
		ptr = loginList.erase(ptr);
		}
	else
		ptr++;
	}
}

void ccontrol::addLogin( const string & Numeric)
{
ccLogin *LogInfo = findLogin(Numeric);
if(LogInfo == NULL)
	{
	LogInfo = new (nothrow) ccLogin(Numeric);
	assert(LogInfo != NULL);

	loginList.push_back(LogInfo);
	}

LogInfo->add_Login();
if(LogInfo->get_Logins() > 5)
	{
	ignoreUser(LogInfo);
	}
}	

int ccontrol::removeIgnore( const string &Host )
{

ccLogin *tempLogin = 0;
int retMe = IGNORE_NOT_FOUND;

for(loginIterator ptr = ignore_begin();ptr!=ignore_end();)
	{
	tempLogin = *ptr;
	if(tempLogin->get_IgnoredHost() == Host)
		{
		strstream s;
		s	<< getCharYYXXX() 
			<< " SILENCE " 
			<< tempLogin->get_Numeric() 
			<< " -" 
			<< tempLogin->get_IgnoredHost()
			<< ends; 
		Write( s );
		delete[] s.str();
		tempLogin->resetIgnore();
		tempLogin->resetLogins();
		ptr = ignoreList.erase(ptr);
		if(tempLogin->get_Numeric() == "0")
			{
			delete tempLogin;
			}
		retMe = IGNORE_REMOVED;
		}
	else
		ptr++;
	}
return retMe;
}	

int ccontrol::removeIgnore( iClient *theClient )
{
string Host = string( "*!*" )
		+ theClient->getUserName() 
		+ string( "@" )
		+ theClient->getInsecureHost();
int retMe = removeIgnore(Host);
return retMe;
}

void ccontrol::ignoreUser( ccLogin *User )
{
iClient *theClient = Network->findClient(User->get_Numeric());

// TODO: Might wanna reconsider this assert()
assert(theClient != NULL);

Notice(theClient,"Hmmmz i dont think i like you anymore , consider yourself ignored");
MsgChanLog("Added %s to my ignore list\n",theClient->getNickUserHost().c_str());

string silenceMask = string( "*!*" )
	+ theClient->getUserName()
	+ "@"
	+ theClient->getInsecureHost();

strstream s;
s	<< getCharYYXXX() 
	<< " SILENCE " 
	<< theClient->getCharYYXXX() 
	<< " " 
	<< silenceMask
	<< ends; 

Write( s );
delete[] s.str();

User->set_IgnoreExpires(::time(0)+60);
User->set_IgnoredHost(silenceMask);

ignoreList.push_back(User);
}

bool ccontrol::listIgnores( iClient *theClient )
{
Notice(theClient,"-= Listing Ignore List =-");			
ccLogin *tempLogin;
for(loginIterator ptr = ignore_begin();ptr!=ignore_end();ptr++)
	{
	tempLogin = *ptr;
	if(tempLogin->get_IgnoreExpires() > ::time(0))
		{
		Notice(theClient,"Host : %s Expires At %s[%d]",
		tempLogin->get_IgnoredHost().c_str(),
		convertToAscTime(tempLogin->get_IgnoreExpires()),
			tempLogin->get_IgnoreExpires());
		}
	}
Notice(theClient,"-= End Of Ignore List =-");			
return true ;
}

bool ccontrol::refreshIgnores()
{
ccLogin *tempLogin;
for(loginIterator ptr = ignore_begin();ptr!=ignore_end();)
	{
	tempLogin = *ptr;
	if((tempLogin) &&(tempLogin->get_IgnoreExpires() <= ::time(0)))
		{
		tempLogin->set_IgnoreExpires(0);
		strstream s;
		s	<< getCharYYXXX() 
			<< " SILENCE " 
			<< tempLogin->get_Numeric() 
			<< " -" 
			<< tempLogin->get_IgnoredHost()
			<< ends; 

		Write( s );
		delete[] s.str();
		tempLogin->set_IgnoredHost("");
		if(tempLogin->get_Numeric() == "0")
			{
			delete tempLogin;
			}
		ptr = ignoreList.erase(ptr);
		}
	else
		ptr++;
	}

return true;

}
bool ccontrol::loadExceptions()
{
static const char *Main = "SELECT * FROM Exceptions";

strstream theQuery;
theQuery	<< Main
		<< ends;

elog	<< "ccontrol::loadExceptions> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::loadExceptions> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false;
	}

ccException *tempException = NULL;

for( int i = 0 ; i < SQLDb->Tuples() ; i++ )
	{
	tempException =  new (nothrow) ccException(SQLDb);
	assert( tempException != 0 ) ;

	tempException->set_Host(SQLDb->GetValue(i,0));
	tempException->set_Connections(atoi(SQLDb->GetValue(i,1)));
	tempException->set_AddedBy(SQLDb->GetValue(i,2)) ;
	tempException->set_AddedOn(static_cast< time_t >( atoi( SQLDb->GetValue(i,3) ) )) ;
	exceptionList.push_back(tempException);
	}
return true;	
} 

void ccontrol::listGlines( iClient *theClient )
{

ccGline* tempGline;
Notice(theClient,"-= Gline List =-");
for(glineIterator ptr = gline_begin();ptr != gline_end();ptr++)
	{
	tempGline =*ptr;
	if(tempGline ->get_Expires() > ::time(0))
		{
		Notice(theClient,"Host : %s , Expires At : %s[%d] , AddedBy %s"
			,tempGline->get_Host().c_str()
			,convertToAscTime(tempGline->get_Expires())
			,tempGline->get_Expires()
			,tempGline->get_AddedBy().c_str());
		}
	}
Notice(theClient,"-= End Of Gline List =-");

}
			
/*void ccontrol::listSuspended( iClient * )
{

}*/

	
void ccontrol::listServers( iClient * )
{

}

} // namespace gnuworld
