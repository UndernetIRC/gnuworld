/**
 * ccontrol.cc
 * Main ccontrol implementation class
 *
 * @author Daniel Karrels dan@karrels.com
 * @author Tomer Cohen	   MrBean@Undernet.org
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
 * $Id: ccontrol.cc,v 1.243 2010/09/12 20:28:24 hidden1 Exp $
*/

#define MAJORVER "1"
#define MINORVER "2pl6"
#define RELDATE "16th March, 2007"

#include        <sys/types.h> 
#include        <sys/socket.h>
#include	<unistd.h>
#include        <netinet/in.h>
#include        <netdb.h>
#include	<fcntl.h>
#include	<list>

#include	<new>
#include	<string>
#include	<sstream>
#include	<vector>
#include	<iostream>
#include	<algorithm>
#include 	<fstream>

#include	<cstring>
#include	<csignal>
#include	<cstdio>
#include	<cerrno>

#include	"client.h"
#include	"iClient.h"
#include	"EConfig.h"
#include	"events.h"
#include	"StringTokenizer.h"
#include	"misc.h"
#include	"Network.h"
#include	"ELog.h"
#include        "ccUser.h"
#include	"dbHandle.h"
#include	"ccontrol.h"
#include        "server.h"
#include 	"Constants.h"
#include	"commLevels.h"
#include	"ccFloodData.h"
#include	"ccUserData.h"
#include	"ip.h"
#include	"ccontrol_generic.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: ccontrol.cc,v 1.243 2010/09/12 20:28:24 hidden1 Exp $" ) ;

namespace gnuworld
{

using std::ends ;
using std::stringstream ;
using std::string ;
using std::vector ;
using std::cout ;
using std::endl ; 
using std::count ;

namespace uworld
{

using namespace std;

/**
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

bool dbConnected = false;
 
ccontrol::ccontrol( const string& configFileName )
 : xClient( configFileName )
{

elog << "Initializing ccontrol version "
     << MAJORVER << "." << MINORVER 
     << " please standby... " << endl;

myHub = 0;
ccHub = 0;
// Read the config file
EConfig conf( configFileName ) ;

sqlHost = conf.Find("sql_host" )->second;
sqlDb = conf.Find( "sql_db" )->second;
sqlPort = conf.Find( "sql_port" )->second;
sqlPass = conf.Require( "sql_pass" )->second;
sqlUser = conf.Require( "sql_user" )->second;

inBurst = true;
inRefresh = false;

string Query = "host=" + sqlHost + " dbname=" + sqlDb + " port=" + sqlPort;
if (strcasecmp(sqlUser,"''"))
	{
	Query += (" user=" + sqlUser);
	}

if (strcasecmp(sqlPass,"''"))
	{
	Query += (" password=" + sqlPass);
	}
	
elog	<< Query
	<< endl ;
elog	<< "ccontrol::ccontrol> Attempting to connect to "
	<< sqlHost
	<< "; Database: "
	<< sqlDb
	<< endl;
 
SQLDb = new dbHandle( sqlHost,
	::atoi( sqlPort.c_str() ),
	sqlDb,
	sqlUser,
	sqlPass ) ;
//(std::nothrow) cmDatabase( Query.c_str() ) ;
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
		<< endl ;
	}
dbConnected = true;

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
ExpiredInterval = atoi( conf.Require( "Expired_interval" )->second.c_str() );

//Sendmail  is the full path of the sendmail program
Sendmail_Path = conf.Require("SendMail")->second;

//SendReport flag that tells ccontrol if the user want the report to be mailed
SendReport = atoi(conf.Require("mail_report")->second.c_str());

maxThreads = atoi(conf.Require("max_threads")->second.c_str());

checkClones = atoi(conf.Require("check_clones")->second.c_str());

showCGIpsInLogs = atoi(conf.Require("showCGIpsInLogs")->second.c_str());

dbConnectionTimer = atoi(conf.Require("dbinterval")->second.c_str());

AnnounceNick = conf.Require("AnnounceNick")->second;

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
	"\t\tObtain general help or help for a specific command",
	true,
	commandLevel::flg_HELP,
	false,
	false,
	true,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new INVITECommand( this, "INVITE", "<#channel> "
	"\t\tRequest an invitation to a channel",
	false,
	commandLevel::flg_INVITE,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new JUPECommand( this, "JUPE", "<servername> <reason> "
	"Jupe a server for the given reason.",
	false,
	commandLevel::flg_JUPE,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new MODECommand( this, "MODE", "<channel> <modes> "
	"Change modes on the given channel",
	false,
	commandLevel::flg_MODE,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new SCHANGLINECommand( this, "SCHANGLINE",
	"[-u] <#channel> <duration>[time units (s,d,h)] <reason> "
	"Gline a given channel for the given reason",
	true,
	commandLevel::flg_SCHANGLINE,
	false,
	false,
	false,
	operLevel::CODERLEVEL,
	true ) ) ;
RegisterCommand( new FORCECHANGLINECommand( this, "FORCECHANGLINE",
		"[-u] <#channel> <duration>[time units (s,d,h)] <reason> "
		"Gline a given channel for the given reason",
		true,
		commandLevel::flg_FORCECHANGLINE,
		false,
		false,
		false,
		operLevel::OPERLEVEL,
		true ) ) ;
RegisterCommand( new GLINECommand( this, "GLINE",
	"<user@host> <duration>[time units (s,d,h)] <reason> "
	"Gline a given user@host for the given reason",
	true,
	commandLevel::flg_GLINE,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new SCANGLINECommand( this, "SCANGLINE", "<mask> "
	"Search current network glines for glines matching <mask>",
	false,
	commandLevel::flg_SCGLINE,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new REMGLINECommand( this, "REMGLINE", "<user@host> "
	"Remove the gline matching <mask>",
	true,
	commandLevel::flg_REMGLINE,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new TRANSLATECommand( this, "TRANSLATE", "<numeric>"
	"Translate a numeric into user information",
	false,
	commandLevel::flg_TRANS,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new WHOISCommand( this, "WHOIS", "<nickname>"
	"Obtain information on a given nickname",
	false,
	commandLevel::flg_WHOIS,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new KICKCommand( this, "KICK", "<channel> <nick> <reason>"
	"Kick a user from a channel",
	false,
	commandLevel::flg_KICK,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;

// The following commands deals with operchans, if you want operchans
// just uncomment them
/*

RegisterCommand( new ADDOPERCHANCommand( this, "ADDOPERCHAN", "<channel>"
	"Add an oper channel",
	false,
	commandLevel::flg_ADDOPCHN,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new REMOPERCHANCommand( this, "REMOPERCHAN", "<channel>"
	"Remove an oper channel",
	false,
	commandLevel::flg_REMOPCHN,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new LISTOPERCHANSCommand( this, "LISTOPERCHANS",
	"List current IRCoperator only channels",
	false,
	commandLevel::flg_LOPCHN,
	false,
	false,
	false,operLevel::OPERLEVEL,
	false ) ) ;
*/	

RegisterCommand( new CHANINFOCommand( this, "CHANINFO", "<channel>"
	"Obtain information about a given channel",
	false,
	commandLevel::flg_CHINFO,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new LOGINCommand( this, "LOGIN", "<USER> <PASS> "
	"Authenticate with the bot",
	true,
	commandLevel::flg_LOGIN,
	false,
	true,
	true,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new DEAUTHCommand( this, "DEAUTH", ""
	"Deauthenticate with the bot",
	false,
	commandLevel::flg_DEAUTH,
	false,
	false,
	true,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new DEAUTHCommand( this, "LOGOUT", ""
        "Deauthenticate with the bot",
	false,
        commandLevel::flg_DEAUTH,
        false,
        false,
        true,
        operLevel::OPERLEVEL,
        false ) ) ;
RegisterCommand( new ADDUSERCommand( this, "ADDUSER",
	"<USER> <OPERTYPE> [SERVER*] <PASS> "
	"Add a new oper",
	true,
	commandLevel::flg_ADDNOP,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new REMUSERCommand( this, "REMUSER", "<USER> "
	"Remove an oper",
	true,
	commandLevel::flg_REMOP,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new ADDCOMMANDCommand( this, "ADDCOMMAND",
	"<USER> <COMMAND> "
	"Add a new command to an oper",
	true,
	commandLevel::flg_ADDCMD,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new REMCOMMANDCommand( this, "REMCOMMAND",
	"<USER> <COMMAND> "
	"Remove a command from oper",
	true,
	commandLevel::flg_DELCMD,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new NEWPASSCommand( this, "NEWPASS", "<PASSWORD> "
	"Change password",
	true,
	commandLevel::flg_NEWPASS,
	false,
	false,
	true,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new SUSPENDCommand( this, "SUSPEND",
	"<OPER> <DURATION> [-l level] <REASON>"
	"Suspend an oper",
	true,
	commandLevel::flg_SUSPEND,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new UNSUSPENDCommand( this, "UNSUSPEND", "<OPER> "
	"UnSuspend an oper",
	true,
	commandLevel::flg_UNSUSPEND,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new MODUSERCommand( this, "MODUSER",
	"<OPER> <OPTION> <NEWVALUE> [OPTION] [NEWVALUE] ... "
	"Modify an oper",
	true,
	commandLevel::flg_MODOP,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new MODERATECommand( this, "MODERATE", "<#Channel> "
	"Moderate A Channel",
	false,
	commandLevel::flg_MODERATE,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new UNMODERATECommand( this, "UNMODERATE", "<#Channel> "
	"UNModerate A Channel",
	false,
	commandLevel::flg_UNMODERATE,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new OPCommand( this, "OP", "<#Channel> <nick> [nick] .. "
	"Op user(s) on a Channel",
	false,
	commandLevel::flg_OP,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new DEOPCommand( this, "DEOP", "<#Channel> <nick> [nick] .. "
	"Deop user(s) on a Channel",
	false,
	commandLevel::flg_DEOP,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new LISTHOSTSCommand( this, "LISTHOSTS", "<oper> "
	"Shows an oper hosts list",
	true,
	commandLevel::flg_LISTHOSTS,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new CLEARCHANCommand( this, "CLEARCHAN", "<#chan> "
	"Removes all channel modes",
	false,
	commandLevel::flg_CLEARCHAN,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new ADDSERVERCommand( this, "ADDSERVER", "<Server> "
	"Add a new server to the bot database",
	true,
	commandLevel::flg_ADDSERVER,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;
RegisterCommand( new LEARNNETCommand( this, "LEARNNET", ""
	"Update the servers database according to the current situation",
	true,
	commandLevel::flg_LEARNNET,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;
RegisterCommand( new REMSERVERCommand( this, "REMSERVER", "<Server name>"
	"Removes a server from the bot database",
	true,
	commandLevel::flg_REMSERVER,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;
RegisterCommand( new CHECKNETCommand( this, "CHECKNET", ""
	"Checks if all known servers are in place",
	true,
	commandLevel::flg_CHECKNET,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;
RegisterCommand( new LASTCOMCommand( this, "LASTCOM",
	"[number of lines to show]"
	"Post you the bot logs",
	true,
	commandLevel::flg_LASTCOM,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true) ) ;
RegisterCommand( new LASTCOMCommand( this, "LASTCOMM",
	"[number of lines to show]"
	"Post you the bot logs",
	true,
	commandLevel::flg_LASTCOM,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true) ) ;

RegisterCommand( new FORCEGLINECommand( this, "FORCEGLINE",
	"<user@host> <duration>[time units] <reason> "
	"Gline a given user@host for the given reason",
	true,
	commandLevel::flg_FGLINE,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;
RegisterCommand( new SGLINECommand( this, "SGLINE",
	"<user@host> <duration>[time units] <reason> "
	"Gline a given user@host for the given reason",
	true,
	commandLevel::flg_SGLINE,
	false,
	false,
	false,
	operLevel::CODERLEVEL,
	true ) ) ;

RegisterCommand( new REMSGLINECommand( this, "REMSGLINE",
	"<user@host> Removes a gline on a given host",
	true,
	commandLevel::flg_REMSGLINE,
	false,
	false,
	false,
	operLevel::CODERLEVEL,
	true ) ) ;

RegisterCommand( new EXCEPTIONCommand( this, "EXCEPTIONS",
	"(list / add / del) [host mask]"
	"Add connection exceptions on hosts",
	true,
	commandLevel::flg_EXCEPTIONS,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;
RegisterCommand( new SHELLSCommand( this, "SHELLS",
	"(addcompany / addnetblock / delcompany / delnetblock / list / chlimit / chname / clearall)",
	true,
	commandLevel::flg_SHELLS,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;
RegisterCommand( new ANNOUNCECommand( this, "ANNOUNCE",
	"<message>",
	false,
	commandLevel::flg_ANNOUNCE,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;
RegisterCommand( new LISTIGNORESCommand( this, "LISTIGNORES",
	"List the ignore list",
	false,
	commandLevel::flg_LISTIGNORES,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;
RegisterCommand( new REMOVEIGNORECommand( this, "REMIGNORE", "(nick/host)"
	" Removes a host/nick from the  ignore list",
	false,
	commandLevel::flg_REMIGNORE,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;
RegisterCommand( new LISTCommand( this, "LIST", "(glines/servers/nomodechannels/exceptions/shells/channels)"
	" Get all kinds of lists from the bot",
	false,
	commandLevel::flg_LIST,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;
RegisterCommand( new COMMANDSCommand( this, "COMMANDS",
	"<command> <option> <new value>"
	" Change commands options",
	true,
	commandLevel::flg_COMMANDS,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;
RegisterCommand( new GCHANCommand( this, "GCHAN",
	"#channel <length/-per> <reason>"
	" Set a BADCHAN gline",
	true,
	commandLevel::flg_GCHAN,
	false,
	false,
	false,
	operLevel::CODERLEVEL,
	true ) ) ;
RegisterCommand( new REMGCHANCommand( this, "REMGCHAN", "#channel "
	" Removes a BADCHAN gline",
	true,
	commandLevel::flg_GCHAN,
	false,
	false,
	false,
	operLevel::CODERLEVEL,
	true ) ) ;
RegisterCommand( new USERINFOCommand( this, "USERINFO",
	"<usermask/servermask> [-cl] Get information about opers (with optional command listing)",
	false,
	commandLevel::flg_USERINFO,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;
RegisterCommand( new STATUSCommand( this, "STATUS", "Shows debug status ",
	false,
	commandLevel::flg_STATUS,
	false,
	false,
	false,
	operLevel::CODERLEVEL,
	true ) ) ;
RegisterCommand( new SHUTDOWNCommand( this, "SHUTDOWN",
	" <REASON> Shutdown the bot ",
	false,
	commandLevel::flg_SHUTDOWN,
	false,
	false,
	false,
	operLevel::CODERLEVEL,
	true ) ) ;
RegisterCommand( new SCANCommand( this, "SCAN",
	" -h <host> / -n <real name> [-v] [-i]"
	" Scans for all users which much a certain host / real name ",
	false,
	commandLevel::flg_SCAN,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;
RegisterCommand( new MAXUSERSCommand( this, "MAXUSERS", 
	"Shows the maximum number of online users ever recorded ",
	false,
	commandLevel::flg_MAXUSERS,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;

RegisterCommand( new CONFIGCommand( this, "CONFIG",
	" -GTime <duration in secs> / -VClones <amount> -Clones <amount>"
	" -CClones <amount> -CClonesCIDR <size> -CClonesGline <Yes/No>"
	" -CClonesGTime <duration>"
	" -IClones <amount> -IClonesGline <Yes/No>"
	" -CClonesTime <seconds> / -GBCount <count> / -GBInterval <interval in secs> "
	" -SGline <Yes/No> "
	"Manages all kinds of configuration related values ",
	false,
	commandLevel::flg_CONFIG,
	false,
	false,
	false,
	operLevel::CODERLEVEL,
	true ) ) ;

RegisterCommand( new NOMODECommand( this, "NOMODE", 
	"<ADD/REM> <#channel> [reason]  Manage the nomode list  ",
	false,
	commandLevel::flg_NOMODE,
	false,
	false,
	false,
	operLevel::SMTLEVEL,
	true ) ) ;

RegisterCommand( new SAYCommand( this, "SAY", 
	"<-s/-b> <#chan/nick> Forced the bot to \"talk\" as the uplink or the bot",
	false,
	commandLevel::flg_SAY,
	false,
	false,
	false,
	operLevel::CODERLEVEL,
	true ) ) ;
RegisterCommand( new SAYCommand( this, "DO",
        "<-s/-b> <#chan/nick> Forces the bot to \"/me\" as the uplink or the bot",
	false,
        commandLevel::flg_SAY,
        false,
        false,
        false,
        operLevel::CODERLEVEL,
        true ) ) ;  
RegisterCommand( new REOPCommand( this, "REOP", "<#chan> <nick> "
	"Removes all channel ops, and reops the specified nick",
	false,
	commandLevel::flg_REOP,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	false ) ) ;

RegisterCommand( new UNJUPECommand( this, "UNJUPE",
	"<server> removes a jupiter on a server",
	false,
	commandLevel::flg_UNJUPE,
	false,
	false,
	false,
	operLevel::OPERLEVEL,
	true ) ) ;

elog << "Loading commands ......... ";

loadCommands();
elog << "Done!" << endl;

elog << "Loading glines ........... ";
if(loadGlines())
	{
	elog << "Done!" << endl;
	}
else
	{
	elog << "Failed!!!" << endl;
	}

elog << "Loading exceptions ....... ";
if(loadExceptions())
	{
	elog << "Done!" << endl;
	}
else
	{
	elog	<< "Error while loading exceptions!!!! ,"
		<< " shutting down"
		<< endl;
	::exit(1);
	}
	
elog << "Loading users ............ ";
if(loadUsers())
	{
	elog << "Done!" << endl;
	}
else
	{
	elog << "Failed!!!" << endl;
	}

elog << "Loading servers info ..... ";
if(loadServers())
	{
	elog << "Done!" << endl;
	}
else
	{
	elog << "Failed!!!" << endl;
	}

loadMaxUsers();
loadVersions();
loadBadChannels();

if(!loadMisc())
	{
	glineBurstInterval = 5;
	glineBurstCount = 5;
	}
	
connectCount = 0;
connectRetry = 5;
curUsers = 0;

#ifdef LOGTOHD
	initLogs();
#endif

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
	delete GLptr->second;
	}

glineList.clear();

for(glineIterator GLptr = rnGlineList.begin(); GLptr != rnGlineList.end(); ++GLptr)
	{
	delete GLptr->second;
	}

rnGlineList.clear();

}

// Register a command handler
bool ccontrol::RegisterCommand( Command* newComm )
{
assert( newComm != NULL ) ;

// Unregister the command handler first; prevent memory leaks
UnRegisterCommand( newComm->getName() ) ;

/*if(!UpdateCommandFromDb(newComm))
	elog << "Error cant find command "
	     << newComm->getName() 
	     << " In database"
	     << ends;*/


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
//delete ptr->second ;

// Remove the handler
commandMap.erase( ptr ) ;

// Return success
return true ;
}

void ccontrol::BurstChannels()
{
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

bool ccontrol::BurstGlines()
{
ccGline *theGline = 0 ;
for(glineListType::iterator ptr = glineList.begin()
    ; ptr != glineList.end(); ++ptr)
	{
	theGline = ptr->second;
	addGlineToUplink(theGline);
	}

for(glineListType::iterator ptr = rnGlineList.begin()
    ; ptr != rnGlineList.end(); ++ptr)
	{
	theGline = ptr->second;
	addGlineToUplink(theGline);
	}

	
return xClient::BurstGlines();
}

// I don't really like doing this.
// In order for each of this bot's Command's to have a valid server
// pointer, this method must be overloaded and server must be
// explicitly set for each Command.
void ccontrol::OnAttach()
{
for( commandMapType::iterator ptr = commandMap.begin() ;
	ptr != commandMap.end() ; ++ptr )
	{
	ptr->second->setServer( MyUplink ) ;
	}


expiredTimer = MyUplink->RegisterTimer(::time(0) + ExpiredInterval,this,NULL);
dbConnectionCheck = MyUplink->RegisterTimer(::time(0) + dbConnectionTimer,this,NULL);
glineQueueCheck = MyUplink->RegisterTimer(::time(0) + glineBurstInterval, this,NULL);


#ifndef LOGTOHD
if(SendReport)
	{
	struct tm Now = convertToTmTime(::time(0));
	time_t theTime = ::time(0) + ((24 - Now.tm_hour)*3600 - (Now.tm_min)*60) ; //Set the daily timer to 24:00
	postDailyLog = MyUplink->RegisterTimer(theTime, this, NULL); 
	}
#endif
MyUplink->RegisterEvent( EVT_KILL, this );
MyUplink->RegisterEvent( EVT_QUIT, this );
MyUplink->RegisterEvent( EVT_NETJOIN, this );
MyUplink->RegisterEvent( EVT_BURST_CMPLT, this );
MyUplink->RegisterEvent( EVT_GLINE , this );
MyUplink->RegisterEvent( EVT_REMGLINE , this );
MyUplink->RegisterEvent( EVT_NICK , this );
MyUplink->RegisterEvent( EVT_OPER , this );

MyUplink->RegisterEvent( EVT_NETBREAK, this );

xClient::OnAttach() ;
}

void ccontrol::OnShutdown(const std::string& reason)
{
	/* handle client shutdown */
	MyUplink->UnloadClient(this, reason);
}

void ccontrol::OnPrivateMessage( iClient* theClient,
	const string& Message, bool )
{
// Tokenize the message
StringTokenizer st( Message ) ;

if(theClient == Network->findClient(this->getCharYYXXX()))
	{
	/* 
	 Got message from one self, this should never happen
	 but in case it does, dont want to create an endless loop
	 */
	xClient::OnPrivateMessage( theClient, Message ) ;
	return ;
	}
// Make sure there is a command present
if( st.empty() )
	{
	//Notice( theClient, "Incomplete command" ) ;
	return ;
	}

const string Command = string_upper( st[ 0 ] ) ;

ccUser* theUser = IsAuth(theClient);

if((!theUser) && !(theClient->isOper()))
	{
	// We need to add the flood points for this user
	if(!theClient->getCustomData(this))
		{
		elog << "Couldnt find custom data for " 
		     << theClient->getNickName() << endl;
		return ;
		}
		
	ccFloodData* floodData = (static_cast< ccUserData* >(
	theClient->getCustomData(this) ))->getFlood() ;
	
	if(floodData->addPoints(flood::MESSAGE_POINTS))
		{
		// yes we need to ignore this user
		ignoreUser(floodData);
		
		MsgChanLog("[FLOOD MESSAGE: %s has been ignored",
			theClient->getNickName().c_str());
		}
	xClient::OnPrivateMessage( theClient, Message ) ;
	return ;
	}

// Attempt to find a handler for this method.
commandIterator commHandler = findCommand( Command ) ;

// Was a handler found?
if( commHandler == command_end() )
	{
	// Nope, notify the client if he's authenticated
	//if(theUser)
	if(!theClient->getMode(iClient::MODE_SERVICES))	
		Notice( theClient, "Unknown command" ) ;
	return ; 
	}

// Check if the user is logged in , and he got
// access to that command

int ComAccess = commHandler->second->getFlags();

if((!theUser) && (ComAccess) && !(theClient->isOper()))
	{
	xClient::OnPrivateMessage( theClient, Message ) ;	
	return ;
	}

if((!theUser) && !(ComAccess & commandLevel::flg_NOLOGIN) && (ComAccess))
	{//The user isnt authenticated, 
	 //and he must be to run this command
	if((theClient->isOper()) && !(theClient->getMode(iClient::MODE_SERVICES)))
		Notice(theClient,"Sorry, you must be authenticated to run this command");

	xClient::OnPrivateMessage( theClient, Message ) ;
	return ;
	}	 	 

if((theUser) && (!theClient->isOper()) && (theUser->getNeedOp()))
	{
	if(!theClient->getMode(iClient::MODE_SERVICES))
		Notice(theClient,
			"You must be oper'd to use this command");
	}
else if( (ComAccess) && (theUser) && !(theUser->gotAccess(commHandler->second)))
	{
	if(!theClient->getMode(iClient::MODE_SERVICES))
		Notice( theClient, "You don't have access to that command" ) ;
	}
else if(( (theUser) && isSuspended(theUser) ) && ( ComAccess ) )
		{
		if(theClient->getMode(iClient::MODE_SERVICES))
			Notice( theClient,
				"Sorry, you are suspended");
		}
else if(commHandler->second->getIsDisabled())
	{
	if(!theClient->getMode(iClient::MODE_SERVICES))
		Notice(theClient,
			"Sorry, this command is currently disabled");
	}
else
	{
		/* do we need a DB connection? */
		if (commHandler->second->needsDB())
		{
			/* check database connection */
			if (!dbConnected)
			{
				/* failed */
				Notice(theClient, "Sorry, the database connection "
					"is currently down, please try again later.");
				xClient::OnPrivateMessage(theClient, Message);
				return;
			}
			/* database connection is fine */
		}
	// Log the command
	if(!commHandler->second->getNoLog()) //Dont log command which arent suppose to be logged
		{	
#ifndef LOGTOHD
		if(theUser)
			DailyLog(theUser,"%s",Message.c_str());
		else
		    	DailyLog(theClient,"%s",Message.c_str());
#else
		ccLog* newLog = new (std::nothrow) ccLog();
		newLog->Time = ::time(0);
		newLog->Desc = Message.c_str();
		newLog->Host = theClient->getRealNickUserHost().c_str();
		if(theUser)
			newLog->User = theUser->getUserName().c_str();
		else
			newLog->User = "Unknown";			
		newLog->CommandName = Command;
		DailyLog(newLog);
#endif
		}
	// Execute the command handler
	commHandler->second->Exec( theClient, Message) ;
	}
xClient::OnPrivateMessage( theClient, Message ) ;
}

void ccontrol::OnServerMessage( iServer* Server, const string& Message,
	bool )
{
StringTokenizer st( Message ) ;

if(st.size() < 2)
    {
    xClient::OnServerMessage(Server,Message);
    return ;
    }

//This is kinda lame 
//TODO : add a server message parser
	if (!strcasecmp(st[1], "RO")) {
		if (st.size() < 7) {
			elog	<< "ccontrol::OnServerMessage> Invalid number "
				<< "on parameters on RO! :"
				<< Message
				<< endl;

		    xClient::OnServerMessage(Server,Message);
			return;
		}

		else if (st.size() < 8) {
			elog	<< "ccontrol::OnServerMessage> Expected 7 "
				<< "parameters on RO! :"
				<< Message
				<< endl;

		    xClient::OnServerMessage(Server,Message);
			return;
		}

		ccServer* tmpServer = serversMap[Server->getName()];
		if (!tmpServer)
			{
			serversMap.erase(Server->getName());
		    xClient::OnServerMessage(Server,Message);
			return;
			}
		timeval now = { 0, 0 } ;
		if( ::gettimeofday( &now, 0 ) < 0 )
			{
			elog	<< "ccontrol(rpingCheck)> gettimeofday() failed: "
				<< strerror( errno )
				<< endl ;
	        xClient::OnServerMessage(Server,Message);
			return;
			}


		time_t lagTime;
		lagTime = (now.tv_sec - atoi(st[6])) * 1000 + (now.tv_usec - atoi(st[7])) / 1000;
		tmpServer->setLagTime(lagTime);
		tmpServer->setLastLagRecv(::time(0));
		//if ((lagTime > LAG_TOO_BIG) && ((::time(0) - tmpServer->getLastLagReport()) > LAG_REPORT_INTERVAL)) {
		//	tmpServer->setLastLagReport(::time(0));
		//	MsgChanLag("[lag] %s is %ds lagged", Server->getName().c_str(), (int) (lagTime / 1000));
		//}
	}

//At 391 BGAAA abuseexploits.undernet.org 1246137209 50 :Saturday June 27 2009 -- 23:12 +02:00
	if (!strcasecmp(st[1], "391"))
		{
		ccServer* tmpServer = serversMap[Server->getName()];
		if (!tmpServer)
			{
			serversMap.erase(Server->getName());
		    xClient::OnServerMessage(Server,Message);
			return;
			}
		timeval now = { 0, 0 } ;
		if( ::gettimeofday( &now, 0 ) < 0 )
			{
			elog	<< "ccontrol(rpingCheck)> gettimeofday() failed: "
				<< strerror( errno )
				<< endl ;
	        xClient::OnServerMessage(Server,Message);
			return;
			}

		if (st.size() < 5)
			{
			elog	<< "ccontrol::OnServerMessage> Invalid number "
				<< "on parameters on 391! :"
				<< Message
				<< endl;

		    xClient::OnServerMessage(Server,Message);
			return;
			}
		int lagTime = tmpServer->getLagTime();
		int timeDiff = abs(::time(0) - (atoi(st[3]) - atoi(st[4])));
		if ((timeDiff >= MAX_TIME_DIFF) && (lagTime < 5000) && ((::time(0) - timediffServersMap[Server]) > TIMEDIFF_REPORT_INTERVAL))
			{
			MsgChanLog("Time diff for %s: %ds", tmpServer->getName().c_str(), timeDiff);
			timediffServersMap[Server] = ::time(0);
			}
		//MsgChanLog("Message: %s", Message.c_str());
		}

if(!strcasecmp(st[1],"351"))
	{
	if(st.size() < 5)
		{
		elog	<< "ccontrol::OnServerMessage> Invalid number "
			<< "on parameters on 351! :"
			<< Message
			<< endl;

		xClient::OnServerMessage(Server,Message);
		return ;
		}
	ccServer* tmpServer = serversMap[Server->getName()];
	
	if(tmpServer)
		{
		tmpServer->setVersion(st[2] + " - " + st[4]);
		}
	else
		{
		serversMap.erase(serversMap.find(Server->getName()));
		}
	}
xClient::OnServerMessage(Server,Message);
}

bool ccontrol::Notice( const iClient* Target, const string& Message )
{
ccUser* tmpUser = IsAuth(Target);
if((tmpUser) && !(tmpUser->getNotice()))
	{
	return xClient::Message(Target,"%s",Message.c_str());
	}
return xClient::Notice(Target,Message);
}
         
bool ccontrol::Notice( const iClient* Target, const char* Message, ... )
{
char buffer[ 1024 ] = { 0 } ;
va_list list;

va_start(list, Message);
vsnprintf(buffer, 1024, Message, list);
va_end(list);

ccUser* tmpUser = IsAuth(Target);
if((tmpUser) && !(tmpUser->getNotice()))
	{
	return xClient::Message(Target,"%s",buffer);
	}
return xClient::Notice(Target,"%s",buffer);
                        
}        

void ccontrol::OnCTCP( iClient* theClient, const string& CTCP,
	const string& Message, bool ) 
{
ccUser* theUser = IsAuth(theClient);
if((!theUser) && !(theClient->isOper()))
	{ //We need to add the flood points for this user
	if(!theClient->getCustomData(this))
		{
		elog << "ccontrol::OnCTCP> Couldnt find custom data for " 
		     << *theClient << endl;
		return ;
		}

	ccFloodData* floodData = (static_cast< ccUserData* >(
	theClient->getCustomData(this) ))->getFlood() ;
	if(floodData->addPoints(flood::CTCP_POINTS))
		{ //yes we need to ignore this user
		ignoreUser(floodData);
		MsgChanLog("[FLOOD MESSAGE]: %s has been ignored"
			,theClient->getNickName().c_str());
		return ;
		}
	}
else
	{
	StringTokenizer st(CTCP);
	if(st.empty())
		{
		xClient::DoCTCP(theClient,CTCP,"Error Aren't we missing something?");
		return ;
		}
	else if(st[0] == "PING")
		{
		xClient::DoCTCP(theClient,CTCP,Message);
		return ;
		}
	else if(st[0] == "GENDER")
		{
		xClient::DoCTCP(theClient,CTCP,
		    "Thats a question i am still trying to find the answer to");
		return ;
		}
	else if(st[0] == "SEX")
		{
		xClient::DoCTCP(theClient,CTCP,
		    "Sorry i am booked till the end of the year");
		return ;
		}
	else if(st[0] == "POLICE")
		{
		xClient::DoCTCP(theClient,CTCP,
		    "Oh crap! where would i hide the drugs now?");
		return ;
		}
	else if(st[0] == "VERSION")
		{
		xClient::DoCTCP(theClient,CTCP,
		    " CControl version "
		    + string(MAJORVER) + "." + string(MINORVER)
		    + " release date: " + RELDATE);
		return ;
		}
	}
}

void ccontrol::OnEvent( const eventType& theEvent,
	void* Data1, void* Data2, void* Data3, void* Data4 )
{
int i=0;
int client_addr[4] = { 0 };
struct in_addr tmp_ip;
unsigned long mask_ip;
char *client_ip;
char Log[200];
switch( theEvent )
	{
	case EVT_QUIT:
	case EVT_KILL:
		{
		/*
		 *  The user disconnected,
		 *  remove his authentication,
		 *  and flood data, and login data
		 */
	
		iClient* tmpUser = (theEvent == EVT_QUIT) ?
			static_cast< iClient* >( Data1 ) :
			static_cast< iClient* >( Data2 ) ;
		--curUsers;
		string tIP = xIP( tmpUser->getIP()).GetNumericIP();
				/* Shell stuff here */
				for (shellnbIterator ptr = shellnbList.begin(); ptr != shellnbList.end(); ptr++) {
					if (isCidrMatch((*ptr)->getCidr(),tIP)) {
						if (shellnbMap.find(*ptr) == shellnbMap.end()) {
							shellnbMap.insert(std::make_pair<ccShellnb*,int>(*ptr, 0));
						}
						else {
							shellnbMap[*ptr]--;
						}
	
						if (shellcoMap.find((*ptr)->shellco) == shellcoMap.end()) {
							shellcoMap.insert(std::make_pair<ccShellco*,int>((*ptr)->shellco, 0));
						}
						else {
							shellcoMap[(*ptr)->shellco]--;
						}
					}
				}
				/* End of shell stuff */
		if(checkClones)
			{

	                        /* CIDR checks */
	                        /* convert ip to longip */
	                        i = sscanf(tIP.c_str(), "%d.%d.%d.%d", &client_addr[0], &client_addr[1], &client_addr[2], &client_addr[3]);
	                        mask_ip = ntohl((client_addr[0]) | (client_addr[1] << 8) | (client_addr[2] << 16) | (client_addr[3] << 24));
	                        /* bitshift ip to strip the last (32-cidrmask) bits (leaving a mask for the ip) */
	                        for (i = 0; i < (32-CClonesCIDR); i++)
	                        {
	                                /* right shift */
	                                mask_ip >>= 1;
	                        }
	                        for (i = 0; i < (32-CClonesCIDR); i++)
	                        {
	                                /* left shift */
	                                mask_ip <<= 1;
	                        }
	                        /* convert longip back to ip */
	                        mask_ip = htonl(mask_ip);
				tmp_ip.s_addr = mask_ip;
	                        client_ip = inet_ntoa(tmp_ip);
				if ((clientsIp24Map.find(client_ip) != clientsIp24Map.end()) && 
					(--clientsIp24Map[client_ip] < 1))
	                        {
	                                clientsIp24Map.erase(clientsIp24Map.find(client_ip));
					if ((clientsIp24MapLastWarn.find(client_ip) != clientsIp24MapLastWarn.end()) &&
						(clientsIp24MapLastWarn[client_ip] > 0))
							clientsIp24MapLastWarn.erase(clientsIp24MapLastWarn.find(client_ip));
	                        }

				/* ident clones */
				sprintf(Log, "%s/%d-%s", client_ip, CClonesCIDR, tmpUser->getUserName().c_str());
				if ((clientsIp24IdentMap.find(Log) != clientsIp24IdentMap.end()) &&
					(--clientsIp24IdentMap[Log] < 1))
				{
					clientsIp24IdentMap.erase(clientsIp24IdentMap.find(Log));
					if ((clientsIp24IdentMapLastWarn.find(Log) != clientsIp24IdentMapLastWarn.end()) &&
						(clientsIp24IdentMapLastWarn[Log] > 0))
							clientsIp24IdentMapLastWarn.erase(clientsIp24IdentMapLastWarn.find(Log));
				}

			if ((clientsIpMap.find(tIP) != clientsIpMap.end()) && (--clientsIpMap[tIP] < 1))
				{
				clientsIpMap.erase(clientsIpMap.find(tIP));
				}
				string virtualHost = tmpUser->getDescription() + "@";
				int dots = 0;
				for(string::size_type ptr = 0;ptr < tIP.size() && dots < 3;++ptr)
					{
					if(tIP[ptr] == '.')
						{
						++dots;
						}
					virtualHost += tIP[ptr];
					}
				virtualHost += '*';
			if ((virtualClientsMap.find(virtualHost) != virtualClientsMap.end()) &&
				(--virtualClientsMap[virtualHost] < 1))
				{
				virtualClientsMap.erase(virtualClientsMap.find(virtualHost));
				if ((virtualClientsMapLastWarn.find(virtualHost) != virtualClientsMapLastWarn.end()) &&
					(virtualClientsMapLastWarn[virtualHost] > 0))
						virtualClientsMapLastWarn.erase(virtualClientsMapLastWarn.find(virtualHost));
				}
			}
		ccUserData* UserData = static_cast< ccUserData* >(
		tmpUser->getCustomData(this))  ;
		tmpUser->removeCustomData(this);

		if( UserData )
			{
			ccUser *TempAuth = UserData->getDbUser();
			if(TempAuth)
		    		{
				UserData->setDbUser(NULL);
				TempAuth->setClient(NULL);
				}

			ccFloodData *tempLogin = UserData->getFlood();
			if(tempLogin)
				{
				removeLogin(tempLogin);
				if(tempLogin->getIgnoredHost() != "")
					{
					tempLogin->setNumeric("0");
					tempLogin->resetLogins();
					}
	    			else
					{
					delete tempLogin;
					}		
				}
			delete UserData;
			}
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
		if(NewServer->isJupe()) //Is the server juped?
			{
			break;
			}
		/* use Network->findServer as AttachServer sends an iClient rather
		 * than an iServer object as Data2.  Either way should work fine.
		 */
		iServer* UplinkServer = Network->findServer(NewServer->getUplinkIntYY());
		ccServer* CheckServer = getServer(NewServer->getName());
		inBurst = true;
		if(!CheckServer)
			{    	
			MsgChanLog("Unknown server just connected: %s via %s\n",
				NewServer->getName().c_str(),
				UplinkServer->getName().c_str());
			}
		else 
			{
			CheckServer->setLastConnected(::time (0));
			CheckServer->setUplink(UplinkServer->getName());
			CheckServer->setLastNumeric(NewServer->getCharYY());
			CheckServer->setNetServer(NewServer);
			if(dbConnected)
				{
				CheckServer->Update();
				}
			}
		break;
		}
	case EVT_NETBREAK:
		{
		iServer* NewServer = static_cast< iServer* >( Data1);
		string Reason = *(static_cast<string *>(Data3));

		if( 0 == Network->findFakeServer( NewServer ) )
// NOTE: Changed --dan
//		if(!getUplink()->isJuped(NewServer))
			{
			ccServer* CheckServer = getServer(NewServer->getName());
			if(CheckServer)
				{
				CheckServer->setSplitReason(Reason);
				CheckServer->setLastSplitted(::time(NULL));
				CheckServer->setNetServer(NULL);
				CheckServer->setLastLagRecv(0);
				CheckServer->setLastLagSent(0);
				CheckServer->setLagTime(0);
				CheckServer->setLastLagReport(0);
				if(dbConnected)
					{
					CheckServer->Update();
					}
				}
			}
		inBurst = false;
		ccServer* curServer;
		const iServer* curNetServer; 
		for(serversConstIterator ptr = serversMap_begin();
		        ptr != serversMap_end() && !inBurst; ++ptr)
			{
			curServer = ptr->second;
			curNetServer = curServer->getNetServer();
			if((curNetServer) && (curNetServer->isBursting()))
				{
				    inBurst = true;
				}
		
		    	}

		break;
		}
	case EVT_BURST_CMPLT:
		{
		inBurst = false;
		ccServer* curServer;
		//refreshOpersIPMap();
		const iServer* curNetServer; 
		for(serversConstIterator ptr = serversMap_begin();
		        ptr != serversMap_end() && !inBurst; ++ptr)
			{
			curServer = ptr->second;
			curNetServer = curServer->getNetServer();
			if((curNetServer) && (curNetServer->isBursting()))
				{
				    inBurst = true;
				}
		
		    	}
		checkMaxUsers();
//		if(!inBurst)
//			{
			refreshVersions();
//			}
		break;
		}	
	case EVT_GLINE:
		{
		if(!Data1) //TODO: find out how we get this
			{
			return ;
			}

		if(!saveGlines)
			{
			return ;
			}
		Gline* newG = static_cast< Gline* >(Data1);

		ccGline* newGline = findGline(newG->getUserHost());
		if(!newGline)
			{
			newGline = new (std::nothrow) ccGline(SQLDb);
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
		//newGline->setLastUpdated(::time(0));
		newGline->setHost(newG->getUserHost());
		newGline->setReason(newG->getReason());
		newGline->setExpires(newG->getExpiration());
		if(saveGlines)
			{
			newGline->Insert();
			//need to load the id
			newGline->loadData(newGline->getHost());
			}
		else
			{
			newGline->setId("-1");
			}
		addGline(newGline);
		break;
		}
	case EVT_REMGLINE:
		{
		if(!Data1)  
			{
			return ;
			}
			
		Gline* newG = static_cast< Gline* >(Data1);
		ccGline* newGline = findGline(newG->getUserHost());
		if(newGline)
			{
			remGline(newGline);
			newGline->Delete();
			delete newGline;
			}
		break;
		}
	case EVT_NICK:
		{
		iClient* NewUser = static_cast< iClient* >( Data1);
		handleNewClient(NewUser);
		break;
		}
	case EVT_OPER:
		{
		iClient* theUser = static_cast< iClient* >( Data1);
		isNowAnOper(theUser);
		break;
		}
	} // switch()

xClient::OnEvent( theEvent, Data1, Data2, Data3, Data4 ) ;
}

void ccontrol::OnChannelEvent( const channelEventType& theEvent,	
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
xClient::OnChannelEvent( theEvent, theChan,
	Data1, Data2, Data3, Data4 ) ;
}

void ccontrol::OnTimer(const xServer::timerID& timer_id, void*)
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
else if (timer_id == expiredTimer)
	{
	refreshGlines();
	refreshIgnores();
	refreshSuspention();
	expiredTimer = MyUplink->RegisterTimer(::time(0) + ExpiredInterval,
		this,NULL);
	}
else if(timer_id == dbConnectionCheck)
	{
	checkDbConnection();
	dbConnectionCheck = MyUplink->RegisterTimer(::time(0) + dbConnectionTimer,this,NULL);
	}
else if(timer_id == glineQueueCheck)
	{
	processGlineQueue();
	glineQueueCheck = MyUplink->RegisterTimer(::time(0) + glineBurstInterval,this,NULL);	
	}
else if (timer_id == timeCheck)
	{
	ccServer* TmpServer;
	for (serversConstIterator ptr = serversMap_begin(); ptr != serversMap_end(); ++ptr)
		{
		TmpServer = ptr->second;
		if (TmpServer->getNetServer())
			{
			Write("%s TI :%s", getCharYYXXX().c_str(), TmpServer->getNetServer()->getCharYY().c_str());
			}
		}
	timeCheck = MyUplink->RegisterTimer(::time(0) + 7200, this, NULL);
	}
else if (timer_id == rpingCheck)
	{
	if (myHub == NULL)
		{
		rpingCheck = MyUplink->RegisterTimer(::time(0) + 60, this, NULL);
		return;
		}
	timeval now = { 0, 0 } ;
	if( ::gettimeofday( &now, 0 ) < 0 )
		{
		elog	<< "ccontrol(rpingCheck)> gettimeofday() failed: "
			<< strerror( errno )
			<< endl ;
		return;
		}

	stringstream s;
	s << now.tv_usec ;

	static int tID = 19;
	tID++;
	if (tID == 20)
		tID = 0;

	//iServer* myHub = Network->findServer(getUplink()->getUplinkCharYY());
	//ccServer* ccHub = getServer(myHub->getName());

	if (ccHub)
		if ((ccHub->getLastLagSent() > ccHub->getLastLagRecv()) && ((::time(0) - ccHub->getLastLagSent()) >= 2))
			ccHub->setLagTime((::time(0) - ccHub->getLastLagSent()) * 1000);

	ccServer* TmpServer;
	int counter = -1;
	for (serversConstIterator ptr = serversMap_begin(); ptr != serversMap_end(); ++ptr)
		{
		TmpServer = ptr->second;
		/* don't bother reporting on juped servers - they dont exist */
		if (TmpServer && (TmpServer->getNetServer()) && (!TmpServer->getNetServer()->isJupe()))
			{
			counter++;
			if ((TmpServer->getLastLagRecv() > 0) && (TmpServer->getLastLagSent() > TmpServer->getLastLagRecv()))
				{
				if ((::time(0) - TmpServer->getLastLagSent()) >= 2)
					TmpServer->setLagTime((::time(0) - TmpServer->getLastLagSent()) * 1000);
				if ((::time(0) - TmpServer->getLastLagSent()) >= LAG_TOO_BIG)
					{
					if ((::time(0) - TmpServer->getLastLagReport()) > LAG_REPORT_INTERVAL)
						{
						// If it's euworld's hub that is lagged, only report euworld's hub, not every servers.
						if ((ccHub == NULL) || (ccHub->getLagTime() < 14000) || (TmpServer->getNetServer()->getIntYY() == myHub->getIntYY()))
							{
							TmpServer->setLastLagReport(::time(0));
							MsgChanLag("[lag] %s is >%ds lagged", TmpServer->getName().c_str(), (int) (TmpServer->getLagTime() / 1000));
							}
						}
					}
				continue;
				}
			// Send RPING to euworld's hub each 30 seconds instead of each minute.
			if (((counter % 20) != tID) && (TmpServer->getNetServer()->getIntYY() != myHub->getIntYY()))
				continue;
			if ((TmpServer->getNetServer()->getIntYY() == myHub->getIntYY()) && ((::time(0) - ccHub->getLastLagSent()) < 30))
				continue;
			Write("%s RI %s %s %d %s :%d %s", getCharYY().c_str(), TmpServer->getNetServer()->getCharYY().c_str(), getCharYYXXX().c_str(), ::time(0), s.str().c_str(), ::time(0), s.str().c_str());
			TmpServer->setLastLagSent(::time(0));
			}
		//BG RI C] BGAAA 1246224483 960943 :1246224474
		}
	rpingCheck = MyUplink->RegisterTimer(::time(0) + 3, this, NULL);
	}
}

void ccontrol::OnConnect()
{
rpingCheck = MyUplink->RegisterTimer(::time(0) + 60, this, NULL);
timeCheck = MyUplink->RegisterTimer(::time(0) + 300, this, NULL);

iServer* tmpServer = Network->findServer(getUplink()->getUplinkCharYY());
ccServer* tServer = getServer(tmpServer->getName());

ccHub = tServer;
myHub = tmpServer;

if(tServer)
	{
	tServer->setNetServer(tmpServer);
	Write("%s V :%s\n",getCharYYXXX().c_str(),
		tmpServer->getCharYY().c_str());
	}

xClient::OnConnect();
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

/*bool ccontrol::Kick( Channel* theChan, iClient* theClient,
	const string& reason )
{
assert( theChan != NULL ) ;

if( !isOnChannel( theChan->getName() ) )
	{
	return false ;
	}

return xClient::Kick( theChan, theClient, reason ) ;
}*/

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

void ccontrol::handleNewClient( iClient* NewUser)
{
bool glSet = false;
bool DoGline = false;
int gDuration = maxGlineLen;
int i=0, AffectedUsers = 0;
int client_addr[4] = { 0 };
unsigned long mask_ip;
struct in_addr tmp_ip;
char *clientip;
char client_ip[19];
char Log[200], GlineMask[250], GlineReason[250];
bool isShellException = false;
int shellglinecounter=0;
std::stringstream s;
stringListType *OthersList;

GlineReason[0] = '\0';
curUsers++;
if (NewUser->isOper())
	isNowAnOper(NewUser);
if(!inBurst)
	{
	checkMaxUsers();
	}
//Create our flood data for this user
ccFloodData* floodData = new (std::nothrow) ccFloodData(NewUser->getCharYYXXX());
assert( floodData != 0 ) ;
ccUserData* UserData = new (std::nothrow) ccUserData(floodData);
NewUser->setCustomData(this,
	static_cast< void* >( UserData ) );
if(dbConnected)
	{
	string tIP = xIP( NewUser->getIP()).GetNumericIP();
			/* Shell stuff here */
			for (shellnbIterator ptr = shellnbList.begin(); ptr != shellnbList.end(); ptr++) {
				if (isCidrMatch((*ptr)->getCidr(),tIP)) {
					if (shellnbMap.find(*ptr) == shellnbMap.end()) {
						shellnbMap.insert(std::make_pair<ccShellnb*,int>(*ptr, 1));
					}
					else {
						shellnbMap[*ptr]++;
					}

					if (shellcoMap.find((*ptr)->shellco) == shellcoMap.end()) {
						shellcoMap.insert(std::make_pair<ccShellco*,int>((*ptr)->shellco, 1));
					}
					else {
						shellcoMap[(*ptr)->shellco]++;
					}
					if ((*ptr)->shellco->isActive()) {
						isShellException = true;
						if (shellcoMap[(*ptr)->shellco] > (*ptr)->shellco->getLimit()) {
							for (shellnbIterator nptr = shellnbList.begin(); nptr != shellnbList.end(); nptr++) {
								if ((*nptr)->shellco == (*ptr)->shellco) {

									shellglinecounter++;
									if ((shellglinecounter == 1) || (s.str().size() > 250)) {
										if (s.str().size() > 250)
											MsgChanLog("%s", s.str().c_str());
										s.str("");
										s << "Excessive connections (" << shellcoMap[(*ptr)->shellco] << ") from SHELL " << (*ptr)->shellco->getName().c_str() << " (will GLINE): *@";
									}
									else
										s << ", *@";
									s << (*nptr)->getCidr() << " (" << shellnbMap[*nptr] << ")";
									sprintf(Log,"Glining SHELL *@%s (%s) for excessive connections (%d)",
										(*nptr)->getCidr().c_str(), (*nptr)->shellco->getName().c_str(), shellnbMap[(*nptr)]);
									sprintf(GlineMask,"*@%s", (*nptr)->getCidr().c_str());
									//AffectedUsers = shellnbMap[(*nptr)];
									AffectedUsers = shellcoMap[(*nptr)->shellco];
									/* set the gline reason */
									//sprintf(GlineReason,"AUTO [%d] Automatically banned for excessive connections",AffectedUsers);
									sprintf(GlineReason,"AUTO [%d] (shell: %s) Automatically banned for excessive connections",AffectedUsers, (*nptr)->shellco->getName().c_str());
									gDuration = maxGlineLen;

				iClient* theClient = Network->findClient(this->getCharYYXXX());
#ifndef LOGTOHD
				DailyLog(theClient,"%s",Log);
#else
				ccLog* newLog = new (std::nothrow) ccLog();
				newLog->Time = ::time(0);
				newLog->Desc = Log;
				newLog->Host = theClient->getRealNickUserHost().c_str();
				newLog->User = "Me";			
				newLog->CommandName = "AUTOGLINE";
				DailyLog(newLog);
#endif
				glSet = true;
				ccGline *tmpGline;
				tmpGline = new ccGline(SQLDb);
				tmpGline->setHost(GlineMask);
				tmpGline->setExpires(::time(0) + gDuration);
				tmpGline->setReason(GlineReason);
				tmpGline->setAddedOn(::time(0));
				tmpGline->setAddedBy(nickName);
				tmpGline->setLastUpdated(::time(0));
				tmpGline->Insert();
				tmpGline->loadData(tmpGline->getHost());
				addGline(tmpGline);
				if(!getUplink()->isBursting())
					addGlineToUplink(tmpGline);
								}
							}
							MsgChanLog("%s", s.str().c_str());
						}
					}
				}
			}
			/* End of shell stuff */

	if(checkClones)
		{
		if(strcasecmp(tIP,"0.0.0.0"))			
			{
			

                                /* CIDR checks */
                                /* convert ip to longip */
                                i = sscanf(tIP.c_str(), "%d.%d.%d.%d", &client_addr[0], &client_addr[1], &client_addr[2], &client_addr[3]);
                                mask_ip = ntohl((client_addr[0]) | (client_addr[1] << 8) | (client_addr[2] << 16) | (client_addr[3] << 24));
                                /* bitshift ip to strip the last (32-cidrmask) bits (leaving a mask for the ip) */
                                for (i = 0; i < (32-CClonesCIDR); i++)
                                {
                                        /* right shift */
                                        mask_ip >>= 1;
                                }
                                for (i = 0; i < (32-CClonesCIDR); i++)
                                {
                                        /* left shift */
                                        mask_ip <<= 1;
                                }
                                /* convert longip back to ip */
                                mask_ip = htonl(mask_ip);
				tmp_ip.s_addr = mask_ip;
                                clientip = inet_ntoa(tmp_ip);
								strncpy(client_ip, clientip, 18);
								client_ip[18] = '\0';
				sprintf(Log, "%s/%d-%s", client_ip, CClonesCIDR, NewUser->getUserName().c_str());
				int CurIdentConnections = ++clientsIp24IdentMap[Log];
                                int CurCIDRConnections = ++clientsIp24Map[client_ip];
                                sprintf(Log,"*@%s/%d", client_ip, CClonesCIDR);


				/* check idents to see if we have too many */
				if ((CurIdentConnections > maxIClones) && (CurIdentConnections > getExceptions(NewUser->getUserName() + "@" + tIP)) &&
					(CurIdentConnections > getExceptions(NewUser->getUserName() + "@" + NewUser->getRealInsecureHost())))
				{
					/* too many - send a warning to the chanlog if within warning range */
					if ((clientsIp24IdentMapLastWarn[Log] + CClonesTime) <= time(NULL))
					{
						MsgChanLog("Excessive CIDR Ident clones (%d) for %s@%s/%d (will%s GLINE)\n",
							CurIdentConnections, NewUser->getUserName().c_str(), client_ip,
							CClonesCIDR, IClonesGline ? "" : " _NOT_");
						clientsIp24IdentMapLastWarn[Log] = time(NULL);
					}
					/* check for auto-gline feature */
					if (IClonesGline)
					{
						sprintf(Log,"Glining %s@%s/%d for excessive CIDR ident connections (%d)",
							NewUser->getUserName().c_str(), client_ip, CClonesCIDR, CurIdentConnections);
						sprintf(GlineMask,"%s@%s/%d", NewUser->getUserName().c_str(), client_ip, CClonesCIDR);
						AffectedUsers = CurIdentConnections;
						/* set the gline reason */
						sprintf(GlineReason,"AUTO [%d] Automatically banned for excessive CIDR ident connections",AffectedUsers);
						DoGline = true;
						gDuration = maxGlineLen;
					}
				}
  
				if ((!isShellException) && (CurCIDRConnections > maxCClones) && (CurCIDRConnections > getExceptions(NewUser->getUserName()+"@" + tIP)) &&
                                        (CurCIDRConnections > getExceptions(NewUser->getUserName()+"@"+NewUser->getRealInsecureHost())))
				{
						// Shell stuff again here
						if (CClonesCIDR == 24)
						{
							int NewLimit = maxCClones;
							int m = 256;
							OthersList = 0;
							int theCount = CurCIDRConnections;
							string strclient_ip = string(client_ip);
							for (shellnbIterator ptr = shellnbList.begin(); ptr != shellnbList.end(); ptr++)
							{
								//elog << "ShellDebug: " << (*ptr)->getCidr() << ":  get24Mask(): " << (*ptr)->get24Mask() << " client_ip: " << string(client_ip) << endl;
								if ((*ptr)->get24Mask() == strclient_ip) /* There is a shell exception somewhere on that /24, but not for that IP */
								{
									std::stringstream ss;
									if (!isShellException) {
										ss << strclient_ip << "/" << CClonesCIDR;
									    OthersList = getOtherCidrs(ss.str());
									}

									isShellException = true;
									int j=1;
									int l=0;
									l = (*ptr)->getCidr2();
									if (l <= 24) {
										elog << "ShellDebug: This shouldn't happen. l = " << l << endl;
									}
									for (int k=32; k>l; k--)
									{
										j = j * 2;
									}
									m -= j;
									theCount -= shellnbMap[*ptr];
								}
							}
							NewLimit = m / 256 * maxCClones;
							if ((OthersList != 0) && (OthersList->size() > 0))
							{
								if (CurCIDRConnections > NewLimit)
								{
									shellglinecounter = 0;
									for (stringListType::iterator nptr = OthersList->begin(); nptr != OthersList->end(); nptr++) {
											shellglinecounter++;
											if ((shellglinecounter == 1) || (s.str().size() > 250)) {
												if (s.str().size() > 250)
													MsgChanLog("%s", s.str().c_str());
												s.str("");
												s << "Excessive connections (" << theCount << ") from subnet *@" << strclient_ip << "/" << CClonesCIDR << " (will GLINE): *@";

											}
											else
												s << ", *@";
											s << (*nptr);

											sprintf(Log,"Glining non-exempted SHELL *@%s for excessive connections (%d)",
												(*nptr).c_str(), theCount);
											sprintf(GlineMask,"*@%s", (*nptr).c_str());
	                                        AffectedUsers = theCount;
											/* set the gline reason */
											sprintf(GlineReason,"AUTO [%d] Automatically banned for excessive CIDR connections",AffectedUsers);
											gDuration = CClonesGTime;

				iClient* theClient = Network->findClient(this->getCharYYXXX());
#ifndef LOGTOHD
				DailyLog(theClient,"%s",Log);
#else
				ccLog* newLog = new (std::nothrow) ccLog();
				newLog->Time = ::time(0);
				newLog->Desc = Log;
				newLog->Host = theClient->getRealNickUserHost().c_str();
				newLog->User = "Me";			
				newLog->CommandName = "AUTOGLINE";
				DailyLog(newLog);
#endif
				glSet = true;
				ccGline *tmpGline;
				tmpGline = new ccGline(SQLDb);
				tmpGline->setHost(GlineMask);
				tmpGline->setExpires(::time(0) + gDuration);
				tmpGline->setReason(GlineReason);
				tmpGline->setAddedOn(::time(0));
				tmpGline->setAddedBy(nickName);
				tmpGline->setLastUpdated(::time(0));
				tmpGline->Insert();
				tmpGline->loadData(tmpGline->getHost());
				addGline(tmpGline);
				if(!getUplink()->isBursting())
					addGlineToUplink(tmpGline);
									}
									MsgChanLog("%s", s.str().c_str());

								}
							}
						}
					if ((clientsIp24MapLastWarn[client_ip] + CClonesTime) <= time(NULL))
					{
						
						if (shellglinecounter == 0)
						{
							
				                            MsgChanLog("Excessive connections (%d) from subnet *@%s/%d (will%s GLINE)\n",
			                                        CurCIDRConnections, client_ip, CClonesCIDR, CClonesGline ? "" : " _NOT_");
						clientsIp24MapLastWarn[client_ip] = time(NULL);
						}
					}
						

					/* check for auto-gline feature */
					if ((CClonesGline) && (shellglinecounter == 0))
					{
	                                        sprintf(Log,"Glining *@%s/%d for excessive connections (%d)",
	                                                client_ip, CClonesCIDR, CurCIDRConnections);
	                                        sprintf(GlineMask,"*@%s/%d", client_ip, CClonesCIDR);
	                                        AffectedUsers = CurCIDRConnections;
						/* set the gline reason */
						sprintf(GlineReason,"AUTO [%d] Automatically banned for excessive CIDR connections",AffectedUsers);
                                                DoGline = true;
						gDuration = CClonesGTime;
					}
                                }
  
                                int CurConnections = ++clientsIpMap[tIP];
  
                                if (((CurConnections > maxClones)) && (CurConnections  > getExceptions(NewUser->getUserName()+"@" + tIP)) &&
                                        (CurConnections > getExceptions(NewUser->getUserName()+"@"+NewUser->getRealInsecureHost())) && (!DoGline))
                                {
                                        sprintf(Log,"*@%s", NewUser->getRealInsecureHost().c_str());
                                        MsgChanLog("Excessive connections [%d] from host *@%s [%s] server:{%s}\n",
                                                CurConnections,NewUser->getRealInsecureHost().c_str(), tIP.c_str(),
						NewUser->getServer()->getName().c_str());
                                        sprintf(Log,"Glining *@%s/32 for excessive connections (%d)",
                                                tIP.c_str(),CurConnections);
                                        sprintf(GlineMask,"*@%s/32",tIP.c_str());
                                        AffectedUsers = CurConnections;
					/* set the gline reason */
					sprintf(GlineReason,"AUTO [%d] Automatically banned for excessive connections",AffectedUsers);
                                        DoGline = true;
					gDuration = maxGlineLen;
                                }
  
                                if ((DoGline) && (shellglinecounter == 0))
								{
                                        iClient* theClient = Network->findClient(this->getCharYYXXX());
#ifndef LOGTOHD
				DailyLog(theClient,"%s",Log);
#else
				ccLog* newLog = new (std::nothrow) ccLog();
				newLog->Time = ::time(0);
				newLog->Desc = Log;
				newLog->Host = theClient->getRealNickUserHost().c_str();
				newLog->User = "Me";			
				newLog->CommandName = "AUTOGLINE";
				DailyLog(newLog);
#endif
				glSet = true;
				ccGline *tmpGline;
				tmpGline = new ccGline(SQLDb);
				tmpGline->setHost(GlineMask);
				tmpGline->setExpires(::time(0) + gDuration);
				tmpGline->setReason(GlineReason);
				tmpGline->setAddedOn(::time(0));
				tmpGline->setAddedBy(nickName);
				tmpGline->setLastUpdated(::time(0));
				tmpGline->Insert();
				tmpGline->loadData(tmpGline->getHost());
				addGline(tmpGline);
				if(!getUplink()->isBursting())
					addGlineToUplink(tmpGline);
				}	
			else
				{
				int dots = 0;
				string ipClass = "";
				for(string::size_type ptr = 0;ptr < tIP.size() && dots < 3;++ptr)
					{
					if(tIP[ptr] == '.')
						{
						++dots;
						}
					ipClass += tIP[ptr];
					}
				ipClass += '*';
				CurConnections = ++virtualClientsMap[NewUser->getDescription() + "@" + ipClass];
				if((CurConnections > maxVClones) &&
				     (CurConnections > getExceptions("*@" + ipClass)))
					{
						/* check for rate limiting */
						if ((virtualClientsMapLastWarn[NewUser->getDescription() + "@" + ipClass] + CClonesTime) <= time(NULL))
						{
							/* send the chanlog message and dont warn for another CClonesTime seconds */
							MsgChanLog("Virtual clones for real name %s on %s, total connections %d\n",
							    NewUser->getDescription().c_str()
							    ,ipClass.c_str()
							    ,CurConnections);
							virtualClientsMapLastWarn[NewUser->getDescription() + "@" + ipClass] = time(NULL);
						}
					}
				}
			}
		}
/* TODO: actually remove this once we're certain.
	if((!glSet)) 
		{	
		ccGline * tempGline = findMatchingRNGline(NewUser);
		if((tempGline) && (tempGline->getExpires() > ::time(0)))
			{
			glSet = true;
			string tIP = xIP( NewUser->getIP()).GetNumericIP();
			ccGline * theGline = new (std::nothrow) ccGline(SQLDb);
			theGline->setHost(string("*@") + tIP);
			theGline->setAddedBy(tempGline->getAddedBy());
			theGline->setExpires((tempGline->getExpires() > 3600 + ::time(0)) ? 3600 : tempGline->getExpires() - time(0));
			theGline->setAddedOn(tempGline->getAddedOn());
			theGline->setLastUpdated(tempGline->getLastUpdated());
			theGline->setReason(tempGline->getReason());
			queueGline(theGline,false);
			}
		} */
	}
        /* check if they are already logged into us */
        usersIterator tIterator = usersMap.begin();
        while (tIterator != usersMap.end())
        {
                ccUser* tUser = tIterator->second;
                if ((tUser->getLastAuthTS() == NewUser->getConnectTime()) &&
                        (tUser->getLastAuthNumeric() == NewUser->getCharYYXXX()))
                {
                        /* it seems they are! authenticate them */
                        /* check if someone else is already authed as them */
                        if (tUser->getClient())
                        {
                                const iClient *tClient = tUser->getClient();
                                Notice(tClient, "You have just been deauthenticated");
                                MsgChanLog("Login conflict for user %s from %s and %s\n",
                                        tUser->getUserName().c_str(), NewUser->getNickName().c_str(),
                                        tClient->getNickName().c_str());
                                deAuthUser(tUser);
                        }
//                      tUser->setUserName(tUser->getUserName);         // not required as we're already set
                        tUser->setNumeric(NewUser->getCharYYXXX());
                        // Try creating an authentication entry for the user
                        if (AuthUser(tUser, NewUser))
                                if (!(isSuspended(tUser)))
                                        Notice(NewUser, "Automatic authentication after netsplit successful!", tUser->getUserName().c_str());
                                else
                                        Notice(NewUser, "Automatic authentication after netsplit successful, however you are suspended", tUser->getUserName().c_str());
                        else
                                Notice(NewUser, "Error in authentication", tUser->getUserName().c_str());
                        MsgChanLog("(%s) - %s: AUTO-AUTHENTICATED (after netsplit)\n", tUser->getUserName().c_str(),
                                NewUser->getRealNickUserHost().c_str());
                        /* had enough checking, break out of the loop */
                        break;
                }
                ++tIterator;
        }
        /* if we get here, there's no matching user */
}

void ccontrol::addGlineToUplink(ccGline* theGline)
{
int Expires;
if((theGline->getHost().substr(0,1) == "#")
   && (theGline->getExpires() == 0))
        {
        Expires = gline::PERM_TIME;
		}
else
	{
	Expires = theGline->getExpires() - time(0);
	}
MyUplink->setGline(theGline->getAddedBy()
    ,theGline->getHost(),theGline->getReason()
    ,Expires,theGline->getLastUpdated(),this);
}

ccUser* ccontrol::IsAuth( const iClient* theClient ) 
{
if(!theClient)
	{
	return NULL;
	}

ccUserData *tempData = static_cast< ccUserData* >(theClient->getCustomData(this));
if(tempData)
	{
	return tempData->getDbUser() ;
	}
else
	{
	return NULL;
	}
}

ccUser* ccontrol::IsAuth( const string& Numeric ) 
{
return IsAuth(Network->findClient(Numeric));
}

bool ccontrol::AddOper (ccUser* Oper)
{
static const char *Main = "INSERT INTO opers (user_name,password,access,saccess,last_updated_by,last_updated,flags,server,isSuspended,suspend_expires,suspended_by,suspend_level,suspend_reason,isUhs,isOper,isAdmin,isSmt,isCoder,GetLogs,NeedOp,Notice,GetLag,LastPassChangeTS) VALUES ('";

if(!dbConnected)
	{
	return false;
	}
stringstream theQuery;
theQuery	<< Main
		<< removeSqlChars(Oper->getUserName()) <<"','"
		<< removeSqlChars(Oper->getPassword()) << "',"
		<< Oper->getAccess() << ","
		<< Oper->getSAccess() << ",'"
		<< removeSqlChars(Oper->getLast_Updated_by())
		<< "',now()::abstime::int4,"
		<< Oper->getFlags() << ",'"
		<< removeSqlChars(Oper->getServer()) 
		<< "' ," 
		<< (Oper->getIsSuspended() ? "'t'" : "'n'") 
		<< "," << Oper->getSuspendExpires()
		<< ",'" << Oper->getSuspendedBy()
		<< "'," << Oper->getSuspendLevel()
		<< ",'" << Oper->getSuspendReason()
		<< "'," << (Oper->isUhs() ? "'t'" : "'n'")
		<< "," << (Oper->isOper() ? "'t'" : "'n'")
		<< "," << (Oper->isAdmin() ? "'t'" : "'n'")
		<< "," << (Oper->isSmt() ? "'t'" : "'n'")
		<< "," << (Oper->isCoder() ? "'t'" : "'n'")
		<< "," << (Oper->getLogs() ? "'t'" : "'n'")
		<< "," << (Oper->getNeedOp() ? "'t'" : "'n'")
		<< "," << (Oper->getNotice() ? "'t'" : "'n'")
		<< "," << (Oper->getLag() ? "'t'" : "'n'")
		<< ",now()::abstime::int4"
		<< ")"
		<< ends;

if( SQLDb->Exec( theQuery.str().c_str() ) )
	{
	if(!Oper->loadData(Oper->getUserName()))
		return false;
	usersMap[Oper->getUserName()] = Oper;
	return true;
	}

elog	<< "ccontrol::AddOper> SQL Failure: "
	<< SQLDb->ErrorMessage()
	<< endl ;
return false;
}

bool ccontrol::DeleteOper (const string& Name)
{
if(!dbConnected)
	{
	return false;
	}

ccUser* tUser = usersMap[Name];
//Delete the user hosts
if(tUser)
    {
    static const char *tMain = "DELETE FROM hosts WHERE User_Id = ";    
    stringstream HostQ;
    HostQ << tMain;
    HostQ << tUser->getID();
    HostQ << ends;

#ifdef LOG_SQL
elog	<< "ccontrol::DeleteOper> "
	<< HostQ.str().c_str()
	<< endl; 
#endif
if( !SQLDb->Exec( HostQ ) )
//if( PGRES_COMMAND_OK != status ) 
	{
	elog	<< "ccontrol::DeleteOper> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}
	usersMap.erase(usersMap.find(Name));
    }    

static const char *Main = "DELETE FROM opers WHERE lower(user_name) = '";

stringstream theQuery;
theQuery	<< Main
		<< removeSqlChars(Name)
		<< "'"
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::DeleteOper> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if(  SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
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

bool ccontrol::AuthUser( ccUser* TempUser,iClient* tUser)
{
if(!tUser->getCustomData(this))
	{
	elog << "Couldnt find custom data for " 
	     << tUser->getNickName() << endl;
	return false;
	}

ccUserData* UserData= static_cast<ccUserData*>(tUser->getCustomData(this));
UserData->setDbUser(TempUser);
TempUser->setClient(tUser);

return true;
}    

bool ccontrol::deAuthUser( ccUser* tUser)
{

const iClient* tClient = tUser->getClient();
if(tClient)
	{
	if(!tClient->getCustomData(this))
		{
		elog << "Couldnt find custom data for " 
		     << tClient->getNickName() << endl;
		return false;
		}

	(static_cast<ccUserData*>(tClient->getCustomData(this)))->setDbUser(NULL);
	tUser->setClient(NULL);
	}
		
return true;
}

bool ccontrol::UserGotMask( ccUser* user, const string& Host )
{
static const char *Main = "SELECT host FROM hosts WHERE user_id = ";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< user->getID()
		<< ';'
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::UserGotMask> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( !SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::UserGotMask> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false ;
	}

for( unsigned int i = 0 ; i < SQLDb->Tuples() ; i++ )
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

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< user->getID()
		<< ';'
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::UserGotHost> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK == status )
	{
	for( unsigned int i = 0 ; i < SQLDb->Tuples() ; i++ )
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
 
stringstream output;
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

return string( salt + output.str().c_str() );
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

if(!dbConnected)
	{
	return false;
	}

static const char *Main = "INSERT INTO hosts (user_id,host) VALUES (";

stringstream theQuery;
theQuery	<< Main
		<< user->getID() <<",'"
		<< removeSqlChars(host) << "')"
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::AddHost> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
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

if(!dbConnected)
	{
	return false;
	}

static const char *Main = "DELETE FROM hosts WHERE user_id = ";

stringstream theQuery;
theQuery	<< Main
		<< user->getID()
		<< " And host = '"
		<< removeSqlChars(host) << "'"
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::DelHost> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
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

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< queryHeader 
		<< User->getID()
		<< ends;

if( !SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "LISTHOSTS> SQL Error: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

// SQL Query succeeded
Notice(theClient,"Host list for %s",User->getUserName().c_str());
for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
	{
	Notice(theClient,"%s",SQLDb->GetValue(i, 0).c_str());
	}
return true;
}	
	    

bool ccontrol::GetHelp( iClient* user, const string& command )
{
static const char *Main = "SELECT line,help FROM help WHERE lower(command) = '";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< string_lower(removeSqlChars(command))
		<< "' ORDER BY line"
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::GetHelp> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK == status )
	{
	if(SQLDb->Tuples() > 0 )
		{
		DoHelp(user);
		}
	else
		{
		Notice( user,
			"Couldn't find help for %s",
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

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< string_lower(removeSqlChars(command))
		<< "' and lower(subcommand) = '"
		<< string_lower(removeSqlChars(subcommand))
		<< "' ORDER BY line"
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::GetHelp> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK == status )
	{
	if(SQLDb->Tuples() > 0 )
		{
		DoHelp(user);
		}
	else
		{
		Notice( user,
			"Couldn't find help for %s",
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
for( unsigned int i = 0 ; i < SQLDb->Tuples() ; i++ )
	{
	string commInfo = replace(
		SQLDb->GetValue( i, 1 ),
		"$BOT$",
		nickName ) ;
	Notice(theClient,"%s",commInfo.c_str());
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
ccUser* tempUser = usersMap[Name];
if(!tempUser)
	{
	usersMap.erase(usersMap.find(Name));
	}
return tempUser;
}

ccUser* ccontrol::GetOper( unsigned int ID)
{
usersIterator tIterator = usersMap.begin();
while(tIterator != usersMap.end())
	{
	ccUser* tUser = tIterator->second;;
	if(tUser->getID() == ID)
		{
		return tIterator->second;
		}
	++tIterator;
	}
return NULL;
}

bool ccontrol::addGline( ccGline* TempGline)
{

glineIterator ptr;
if(TempGline->getHost().substr(0,1) == "$") //check if its a realname gline
	{	

	ptr = rnGlineList.find(TempGline->getHost());
	if(ptr != rnGlineList.end())
		{
		if(ptr->second != TempGline)
			{
			delete ptr->second;
			rnGlineList.erase(ptr);
			}
		}				
		rnGlineList[TempGline->getHost()] = TempGline;

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

bool ccontrol::remGline( ccGline* TempGline)
{
if(TempGline->getHost().substr(0,1) == "$")
	{
	rnGlineList.erase(TempGline->getHost());
	}
else
	{	
	glineList.erase(TempGline->getHost()) ;
	}
return true;
}

ccGline* ccontrol::findMatchingGline( const iClient* theClient )
{
ccGline *theGline = 0;
string Host = theClient->getUserName() + '@' + theClient->getRealInsecureHost();
string IP = theClient->getUserName() + '@' + xIP( theClient->getIP()).GetNumericIP();
string RealName = theClient->getDescription();
string glineHost;
for(glineIterator ptr = glineList.begin(); ptr != glineList.end(); ++ptr)
	{
	theGline = ptr->second;
	if((match(theGline->getHost(),Host) == 0) || 
	    ((match(theGline->getHost(),IP) == 0)))
		{
    		if(theGline->getExpires() > ::time(0))
			{
			return theGline;
			}
		}
	}

for(glineIterator ptr = rnGlineList.begin(); ptr != rnGlineList.end(); ++ptr)
	{
	theGline = ptr->second;
	glineHost = theGline->getHost().substr(1,theGline->getHost().size() - 1);
	if(match(glineHost,RealName) == 0)
		{
    		if(theGline->getExpires() > ::time(0))
			{
			return theGline;
			}
		}
	}

return NULL ;
}

ccGline* ccontrol::findMatchingRNGline( const iClient* theClient )
{
ccGline *theGline = 0;
string RealName = theClient->getDescription();
string glineHost;

for(glineIterator ptr = rnGlineList.begin(); ptr != rnGlineList.end(); ++ptr)
	{
	theGline = ptr->second;
	glineHost = theGline->getHost().substr(1,theGline->getHost().size() - 1);
	if(match(glineHost,RealName) == 0)
		{
    		if(theGline->getExpires() > ::time(0))
			{
			return theGline;
			}
		}
	}

return NULL ;
}

ccGline* ccontrol::findGline( const string& HostName )
{

glineIterator ptr = glineList.find(HostName);
if(ptr != glineList.end())
	{
	return ptr->second;
	}
	
return NULL ;
}

ccGline* ccontrol::findRealGline( const string& HostName )
{

glineIterator ptr = rnGlineList.find(HostName);
if(ptr != rnGlineList.end())
	{
	return ptr->second;
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

xClient::Notice((Network->findChannel(msgChan))->getName(),"%s",buffer);
usersIterator uIterator;
ccUser* tempUser;
for( uIterator = usersMap.begin();uIterator != usersMap.end();++uIterator)
        {
        tempUser = uIterator->second;
	if((tempUser) && (tempUser->getLogs() ) && (tempUser->getClient()))
                { 
                Notice(tempUser->getClient(),"%s",buffer);
                }
	}
return true;
}

bool ccontrol::MsgChanLag(const char *Msg, ... ) 
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

xClient::Notice((Network->findChannel(msgChan))->getName(),"%s",buffer);
usersIterator uIterator;
ccUser* tempUser;
for( uIterator = usersMap.begin();uIterator != usersMap.end();++uIterator)
        {
        tempUser = uIterator->second;
	if((tempUser) && (tempUser->getLag() ) && (tempUser->getClient()))
                { 
                Notice(tempUser->getClient(),"%s",buffer);
                }
	}
return true;
}

#ifndef LOGTOHD
bool ccontrol::DailyLog(ccUser* Oper, const char *Log, ... )
{

if(!dbConnected)
	{
	return false;
	}

char buffer[ 1024 ] ;
memset( buffer, 0, 1024 ) ;

va_list list;

va_start( list, Log ) ;
vsprintf( buffer, Log, list ) ;
va_end( list ) ;

iClient* theClient = 0 ;
if(Oper)
	{	
	theClient = Network->findClient(Oper->getNumeric());
	}

iServer* targetServer = Network->findServer( theClient->getIntYY() ) ;
if( NULL == targetServer )
	{
		elog	<< "ccontrol:> Unable to find server: "
		<< theClient->getIntYY() << endl ;
	return false ;
	}

buffer[ 512 ] = 0 ;
static const char *Main = "INSERT INTO comlog (ts,oper,command) VALUES (now()::abstime::int4,'";
StringTokenizer st(buffer);
commandIterator tCommand = findCommand((string_upper(st[0])));
string log;
if(tCommand != command_end())
	{
	if(!strcasecmp(tCommand->second->getRealName(),"LOGIN"))
		{
		if (st.size() > 1)
			log.assign(string("LOGIN ") + st[1] + string(" *****") + string(" (") + targetServer->getName() + string(")"));
		}
	else if(!strcasecmp(tCommand->second->getRealName(),"NEWPASS"))
		{
		log.assign("NEWPASS *****");
		}
	else if(!strcasecmp(tCommand->second->getRealName(),"MODUSER"))
		{
		if(st.size() > 2)
			{
			log.assign("MODUSER " + st[1] + " ");
			unsigned int place = 2;
			while(place < st.size())
				{
				if(!strcasecmp(st[place],"-p"))
					{
					log.append(" -p ******");
					place+=2;
					}
				else	
					{
					log.append(" " + st[place]);
					place++;
					}
				}
			}
		}
	else if(!strcasecmp(tCommand->second->getRealName(),"ADDUSER"))
		{
		if(st.size() > 3)
			{
			log.assign("ADDUSER " + st[1] + string(" ") + st[2]+ " *****");
			}
		}
	else
		{
		log.assign(buffer);
		}
	}
else
	{
	log.assign(buffer);
	}
strcpy(buffer,log.c_str());
					
stringstream theQuery;
theQuery	<< Main;
if(Oper)
	{
	theQuery << Oper->getUserName();
	}
else
	{
	theQuery << "Unknown";
	}
theQuery	<< " (" << removeSqlChars(theClient->getRealNickUserHost()) <<")','"
		<< removeSqlChars(buffer) << "')"
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::DailyLog> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
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

bool ccontrol::DailyLog(iClient* theClient, const char *Log, ... )
{

if(!dbConnected)
	{
	return false;
	}

char buffer[ 1024 ] = { 0 } ;
va_list list;

va_start( list, Log ) ;
vsprintf( buffer, Log, list ) ;
va_end( list ) ;
buffer[512]= '\0';
static const char *Main = "INSERT INTO comlog (ts,oper,command) VALUES (now()::abstime::int4,'";
StringTokenizer st(buffer);
commandIterator tCommand = findCommand((string_upper(st[0])));
string log;

iServer* targetServer = Network->findServer( theClient->getIntYY() ) ;
if( NULL == targetServer )
	{
		elog	<< "ccontrol:> Unable to find server: "
		<< theClient->getIntYY() << endl ;
	return false ;
	}

if(tCommand != command_end())
	{
	if(!strcasecmp(tCommand->second->getRealName(),"LOGIN"))
		{
		if (st.size() > 1)
			log.assign(string("LOGIN ") + st[1] + string(" *****") + string(" (") + targetServer->getName() + string(")"));
		}
	else if(!strcasecmp(tCommand->second->getRealName(),"NEWPASS"))
		{
		log.assign("NEWPASS *****");
		}
	else if(!strcasecmp(tCommand->second->getRealName(),"MODUSER"))
		{
		if(st.size() > 2)
			{
			log.assign("MODUSER " + st[1] + " ");
			unsigned int place = 2;
			while(place < st.size())
				{
				if(!strcasecmp(st[place],"-p"))
					{
					log.append(" -p ******");
					place+=2;
					}
				else	
					{
					log.append(" " + st[place]);
					place++;
					}
				}
			}
		}
	else if(!strcasecmp(tCommand->second->getRealName(),"ADDUSER"))
		{
		if(st.size() > 3)
			{
			log.assign("ADDUSER " + st[1] + string(" ") + st[2]+ " *****");
			}
		}
	else
		{
		log.assign(buffer);
		}
	}
else
	{
	log.assign(buffer);
	}
strcpy(buffer,log.c_str());
					
stringstream theQuery;
theQuery	<< Main
		<< "Unknown"
		<< " (" << removeSqlChars(theClient->getRealNickUserHost()) <<")','"
		<< removeSqlChars(buffer) << "')"
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::DailyLog> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
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

#else
bool ccontrol::DailyLog(ccLog* newLog)
{
commandIterator tCommand = findCommand(newLog->CommandName);
string log;
StringTokenizer st(newLog->Desc);
StringTokenizer st2(newLog->Host,'!');
if (st.size() == 0)
	return 0;
iClient* theClient = Network->findNick(st2[0]);
iServer* targetServer;
if (theClient != NULL)
	{
	targetServer = Network->findServer( theClient->getIntYY() ) ;
	if( NULL == targetServer )
		{
			elog	<< "ccontrol:> Unable to find server: "
			<< theClient->getIntYY() << endl ;
		return false ;
		}
	}

if(tCommand != command_end())
	{
	if(!strcasecmp(tCommand->second->getRealName(),"LOGIN"))
		{
		if (st.size() > 1)
			{
			if (theClient != NULL)
				log.assign(string("LOGIN ") + st[1] + string(" *****") + string(" (") + targetServer->getName() + string(")"));
			else
				log.assign(string("LOGIN ") + st[1] + string(" *****"));
			}
		}
	else if(!strcasecmp(tCommand->second->getRealName(),"NEWPASS"))
		{
		log.assign("NEWPASS *****");
		}
	else if(!strcasecmp(tCommand->second->getRealName(),"MODUSER"))
		{
		if(st.size() > 2)
			{
			log.assign("MODUSER " + st[1] + " ");
			unsigned int place = 2;
			while(place < st.size())
				{
				if(!strcasecmp(st[place],"-p"))
					{
					log.append(" -p ******");
					place+=2;
					}
				else	
					{
					log.append(" " + st[place]);
					place++;
					}
				}
			}
		}
	else if(!strcasecmp(tCommand->second->getRealName(),"ADDUSER"))
		{
		if(st.size() > 3)
			{
			log.assign("ADDUSER " + st[1] + string(" ") + st[2]+ " *****");
			}
		}
	else
		{
		log.assign(newLog->Desc);
		}
	}
else
	{
	log.assign(newLog->Desc);
	}

newLog->Desc = log;
if(!LogFile.is_open())
	{
	LogFile.open(LogFileName.c_str(),ios::out|ios::app);
	//LogFile.setbuf(NULL,0);
	}
if(LogFile.bad())
	{//There was a problem in opening the log file
	MsgChanLog("Error while logging to the log file %s!\n",LogFileName.c_str());
	return true;
	}

LogFile.seekp(0,ios::end);
if(!newLog->Save(LogFile))
	{
	MsgChanLog("Error while logging to the log file!\n");
	}
addLog(newLog);
LogFile.close();

if(NumOfLogs > 0)
	NumOfLogs++;
return true;
}

#endif

bool ccontrol::CreateReport(time_t From, time_t Til)
{

if(!dbConnected)
	{
	return false;
	}

static const char* queryHeader = "SELECT * FROM comlog WHERE ts >";
stringstream theQuery;
theQuery 	<< queryHeader 
		<< From
		<< " AND ts < "
		<< Til
		<< " ORDER BY ts DESC"
		<< ends;
	
#ifdef LOG_SQL
elog	<< "ccontrol::CreateReport> " 
	<< theQuery.str().c_str() 
	<< endl;
#endif
	
if( !SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::CreateReport> SQL Error: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

// SQL Query succeeded
ofstream tLogFile;
tLogFile.open("Report.log",ios::out);
if(!tLogFile)
	{
	return false;
	}

tLogFile << "ccontrol log for command issued between "
	<< convertToAscTime(From)
	<< " and up til "
	<< convertToAscTime(Til)
	<< endl;

for (unsigned int i = 0 ; i < SQLDb->Tuples(); i++)
	{
	tLogFile	<< "[ "
		<< convertToAscTime(atoi(SQLDb->GetValue(i, 0).c_str()))
		<< " - "
		<< SQLDb->GetValue(i,1)
		<< " ] "
		<< SQLDb->GetValue(i,2)
		<< endl;
	}

tLogFile << "End of debug log"
	<< endl ;

tLogFile.close();
return true;
}

bool ccontrol::MailReport(const char *MailTo, const char *ReportFile)
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


int ccontrol::checkGline(const string Host,unsigned int Len,unsigned int &Affected)
{

const unsigned int isWildCard = 0x01;
const unsigned int isIP = 0x02;
unsigned int Mask = 0;
unsigned int Dots = 0;
unsigned int GlineType = isIP;
bool ParseEnded = false;
bool isCIDR = false;
int retMe = 0, i = 0;
char CIDRip[16];
int client_addr[4] = { 0 };
unsigned long mask_ip, orig_mask_ip;
string::size_type pos = Host.find_first_of('@');
string Ident = Host.substr(0,pos);
string Hostname = Host.substr(pos+1);
if(Len >  gline::MFU_TIME)  //Check for maximum time
	retMe |=  gline::BAD_TIME;
if((signed int) Len < 0)
	retMe |=  gline::NEG_TIME;
if (Hostname[0] == '.')
	retMe |= gline::BAD_HOST;
for(string::size_type pos = 0; pos < Hostname.size();++pos)
	{
	if(Hostname[pos] =='.')
		{
		Dots++;
		if((GlineType & (isWildCard | isIP)) == isIP)
			Mask+=8; //Keep track of the mask
			if (Hostname[pos+1] == '.')
				retMe |= gline::BAD_HOST;
		}
	else if((Hostname[pos] =='*') || (Hostname[pos] == '?'))
		GlineType |= isWildCard;
	else if(Hostname[pos] == '/')
		{
                        if (!(GlineType & isIP))        // must be an ip to specify 
                                return  gline::BAD_HOST;

			if (Dots>3)			// can't have more than 3 dots
				return	gline::BAD_HOST;
  
                        if (GlineType & isWildCard)     // cidr may not contain wildcards
                                return  gline::BAD_HOST;
  
                        /* copy the mask to CIDRip */
                        if (pos > 15)
                                pos = 15;
                        for (i=0; i<(int) pos; i++)
                                CIDRip[i] = Hostname[i];
                        CIDRip[i++] = '\0';
  
                        Mask = atol((Hostname.substr(++pos)).c_str());
                        isCIDR = true;
                        retMe |= gline::FORCE_NEEDED_HOST;
                        /* check if the mask matches the cidr size */
                        i = sscanf(CIDRip, "%d.%d.%d.%d", &client_addr[0], &client_addr[1], &client_addr[2], &client_addr[3]);
                        mask_ip = ntohl((client_addr[0]) | (client_addr[1] << 8) | (client_addr[2] << 16) | (client_addr[3] << 24));
                        orig_mask_ip = mask_ip;
                        for (i = 0; i < (32 - (int) Mask); i++)
                        {
                                /* right shift */
                                mask_ip >>= 1;
                        }
                        for (i = 0; i < (32 - (int) Mask); i++)
                        {
                                /* left shift */
                                mask_ip <<= 1;
                        }
                        if (mask_ip != orig_mask_ip)
                        {
                                /* mask no longer matches the original mask - ip was not on the bit boundary */
                                retMe |= gline::BAD_CIDRMASK;
                        }
                        if (!(Mask) || (Mask > 32))
                                retMe |= gline::BAD_HOST;
                        if (Mask < 16)
                                retMe |= gline::BAD_CIDRLEN;
                        if (Mask < 32)
                                GlineType |= isWildCard;
                        if ((GlineType & isIP) && (isCIDR) && (Dots != 3))
                                retMe |= gline::BAD_CIDROVERRIDE;
                        ParseEnded = true;                      
                        break;
		 }
	else if((Hostname[pos] > '9') || (Hostname[pos] < '0')) 
		GlineType &= ~isIP;
	}

Affected = Network->countMatchingRealUserHost(Host); //Calculate the number of affected
if((Dots > 3) && (GlineType & isIP)) //IP addy cant have more than 3 dots
	retMe |=  gline::BAD_HOST;
if(((GlineType & (isIP || isWildCard)) == isIP) && !(ParseEnded))
	Mask +=8; //Add the last mask count if needed
if((GlineType & isIP) && (Mask < 24))
	retMe |=  gline::HUH_NO_HOST;  //Its too wide
if(!(GlineType & isIP) && (Dots < 2) && (GlineType & isWildCard))
	retMe |=  gline::HUH_NO_HOST; //Wildcard gline must have atleast 2 dots
if(Affected >  gline::MFGLINE_USERS) 
	retMe |=  gline::FU_NEEDED_USERS; //This gline must be set with -fu flag
if(Len >  gline::MFGLINE_TIME)
	retMe |=  gline::FU_NEEDED_TIME;
if(Len >  gline::MGLINE_TIME)
	retMe |=  gline::FORCE_NEEDED_TIME;
if(GlineType & (isWildCard))
	{//Need to check the Ident now
	bool hasId = false;
	for(string::size_type pos = 0; pos < Ident.size();++pos)
		{
		if((Ident[pos] == '*') || (Ident[pos] == '?'))
			{
			continue;
			}
		else
			{ //Its not */? so we have a legal ident
			hasId = true;
			break;
			}
		}
	if((hasId & (Len >  gline::MGLINE_WILD_TIME)) 
		|| (!hasId & (Len >  gline::MGLINE_WILD_NOID_TIME)))
		{
		retMe |=  gline::FORCE_NEEDED_WILDTIME;
		}
	}
if (getExceptions("*@" + Hostname) > 0) 
	{
	retMe |= gline::HUH_IS_EXCEPTION;
	}
if (isIpOfOper(Hostname))
	retMe |= gline::HUH_IS_IP_OF_OPER;
	
//if(GlineType & (isWildCard & (Len >  gline::MGLINE_WILD_TIME)))
//	retMe |=  gline::FORCE_NEEDED_WILDTIME;
if(!retMe)
	retMe =  gline::GLINE_OK;
return retMe;
}

int ccontrol::checkSGline(const string Host,unsigned int Len,unsigned int &Affected)
{

const unsigned int isWildCard = 0x01;
const unsigned int isIP = 0x02;
unsigned int Mask = 0;
unsigned int Dots = 0;
unsigned int GlineType = isIP;
bool ParseEnded = false;
bool hasId = false;
bool isCIDR = false;
int retMe = 0, i = 0;
char CIDRip[16];
int client_addr[4] = { 0 };
unsigned long mask_ip, orig_mask_ip;
string::size_type pos = Host.find_first_of('@');
string Ident = Host.substr(0,pos);
string Hostname = Host.substr(pos+1);
if((signed int)Len < 0)
	retMe |=  gline::NEG_TIME;
//Check the ident first, if its valid then the gline is ok 
Affected = Network->countMatchingUserHost(Host); //Calculate the number of affected
for(string::size_type pos = 0; pos < Ident.size();++pos)
	{
	if((Ident[pos] == '*') || (Ident[pos] == '?'))
		{
		continue;
		}
	else
		{ //Its not */? so we have a legal ident
		hasId = true;
		break;
		}
	}
if(hasId)
	return gline::GLINE_OK;

if (Hostname[0]=='.')
	retMe |= gline::BAD_HOST;
for(string::size_type pos = 0; pos < Hostname.size();++pos)
	{
	if(Hostname[pos] =='.')
		{
		Dots++;
		if((GlineType & (isWildCard | isIP)) == isIP)
			Mask+=8; //Keep track of the mask
			if (Hostname[pos+1] == '.')
				retMe |= gline::BAD_HOST;
		}
	else if((Hostname[pos] =='*') || (Hostname[pos] == '?'))
		GlineType |= isWildCard;
	else if(Hostname[pos] == '/')
		{
                       if (!(GlineType & isIP))        // must be an ip to specify CIDR mask
                               return  gline::BAD_HOST;
                       if (GlineType & isWildCard)     // cidr can't contain wildcards
                               return  gline::BAD_HOST;
                       /* copy the mask to CIDRip */
                       if (pos > 15)
                               pos = 15;
                       for (i=0; i<(int) pos; i++)
                               CIDRip[i] = Hostname[i];
                       CIDRip[i++] = '\0';
 
                       Mask = atol((Hostname.substr(++pos)).c_str());
                       isCIDR = true;
 
                       /* check if the mask matches the cidr size */
                       i = sscanf(CIDRip, "%d.%d.%d.%d", &client_addr[0], &client_addr[1], &client_addr[2], &client_addr[3]);
                       mask_ip = ntohl((client_addr[0]) | (client_addr[1] << 8) | (client_addr[2] << 16) | (client_addr[3] << 24));
                       orig_mask_ip = mask_ip;
                       for (i = 0; i < (32 - (int) Mask); i++)
                       {
                               /* right shift */
                               mask_ip >>= 1;
                       }
                       for (i = 0; i < (32 - (int) Mask); i++)
                       {
                               /* left shift */
                               mask_ip <<= 1;
                       }
                       if (mask_ip != orig_mask_ip)
                       {
                               /* mask no longer matches the original mask - ip was not on the bit boundary */
                               retMe |= gline::BAD_CIDRMASK;
                       }
                       if(!(Mask) || (Mask > 32))      // must be under a /32 to be valid
                               retMe |= gline::BAD_HOST;
                       if(Mask < 8)                    // must be a /8 or more specific
                               retMe |= gline::BAD_CIDRLEN;
                       if(Mask < 32)
                               GlineType |= isWildCard;
                       ParseEnded = true;                      
                       break;
		}
	else if((Hostname[pos] > '9') || (Hostname[pos] < '0')) 
		GlineType &= ~isIP;
	}



if((Dots > 3) && (GlineType & isIP)) //IP addy cant have more than 3 dots
	retMe |=  gline::BAD_HOST;
if((GlineType & isIP) && (isCIDR) && (Dots != 3))
	retMe |= gline::BAD_CIDROVERRIDE;
if(((GlineType & (isIP || isWildCard)) == isIP) && !(ParseEnded))
	Mask +=8; //Add the last mask count if needed
if((GlineType & isIP) && (Mask < 8))
	retMe |=  gline::HUH_NO_HOST;  //Its too wide
if(!(GlineType & isIP) && (Dots < 1) && (GlineType & isWildCard))
	retMe |=  gline::HUH_NO_HOST; //Wildcard gline must have atleast 2 dots
	
if(!retMe)
	retMe =  gline::GLINE_OK;
return retMe;
}

bool ccontrol::isSuspended(ccUser *theUser)
{
if( (theUser) && (theUser->getIsSuspended()))
	{
	//Check if the suspend hadnt already expired
	if(::time( 0 ) - theUser->getSuspendExpires() < 0)
		{
		return true;
		}
	}
return false;
}


bool ccontrol::refreshSuspention()
{

int totalCount = 0;
usersIterator curUser = usersMap.begin();
ccUser* tempUser;
for(;curUser != usersMap.end();++curUser)
	{
	tempUser = curUser->second;
	if((tempUser->getIsSuspended()) && (tempUser->getSuspendExpires() < ::time(0)))
		{
		tempUser->setSuspendExpires(0);
		tempUser->setSuspendedBy("");
		tempUser->setIsSuspended(false);
		tempUser->setSuspendLevel(0);
		tempUser->setSuspendReason("");
		++totalCount;
		}
	}

static const char *DelMain = "UPDATE opers SET isSuspended = 'n',Suspend_Expires = 0, Suspended_by = '',suspend_level = 0,suspend_reason='' WHERE IsSuspended = 'y' AND suspend_expires < now()::abstime::int4";

stringstream DelQuery;
DelQuery	<< DelMain
		<< ends;
#ifdef LOG_SQL
elog	<< "ccontrol::RefreshSuspention> "
	<< DelQuery.str().c_str()
	<< endl; 
#endif

if( !SQLDb->Exec( DelQuery ) )
//if( PGRES_COMMAND_OK != status )
	{
	elog	<< "ccontrol::refreshSuspention> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
		return false ;
	}
if(totalCount > 0)
	MsgChanLog("[Refresh Suspend] - %d expired\n",totalCount);		

return true;
}

bool ccontrol::refreshVersions()
{
ccServer* curServer;
const iServer* curNetServer; 
for(serversConstIterator ptr = serversMap_begin();
    ptr != serversMap_end(); ++ptr)
	{
	curServer = ptr->second;
	curNetServer = curServer->getNetServer();
	if(curNetServer)
		{
		Write("%s V :%s\n",getCharYYXXX().c_str(),
			curNetServer->getCharYY().c_str());		
		}
		
	}
return true;    
}

bool ccontrol::refreshGlines()
{

if(!dbConnected)
	{
	return false;
	}

int totalFound = 0;
inRefresh = true;
ccGline * tempGline;
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
//		ptr = glineList.erase(ptr);
		delete tempGline;
		++totalFound;
		}
	}

for(remIterator = remList.begin();remIterator != remList.end();)
	{
	glineList.erase(*remIterator);
	remIterator = remList.erase(remIterator);
	}
	
for(glineIterator ptr = rnGlineList.begin();ptr != rnGlineList.end();++ptr) 
	{
	tempGline = ptr->second;
	if(tempGline->getExpires() <= ::time(0)) 
		{
		//remove the gline from the database
		tempGline->Delete();
		//ptr = rnGlineList.erase(ptr);
		remList.push_back(ptr->first);
		delete tempGline;
		++totalFound;
		}

	}

for(remIterator = remList.begin();remIterator != remList.end();)
	{
	rnGlineList.erase(*remIterator);
	remIterator = remList.erase(remIterator);
	}


inRefresh = false;

//if(totalFound > 0)
//	MsgChanLog("[Refresh Glines] - %d expired\n",totalFound);
return true;

}

void ccontrol::queueGline(ccGline* theGline, bool shouldAdd)
{
glineQueue.push_back(glineQueueDataType(theGline,shouldAdd));
}

bool ccontrol::processGlineQueue()
{


if (getUplink()->isBursting() || glineQueue.empty())
        {
        return true;
        }

glineQueueDataType  curGlinePtr;
ccGline* curGline; 
unsigned int count;
char us[100];
us[0] = '\0';

for(unsigned int i = 0; i< (glineQueue.size() > glineBurstCount ? glineBurstCount : glineQueue.size());++i)
	{
	curGlinePtr = glineQueue.front();
	glineQueue.pop_front();
	curGline = curGlinePtr.first;
	count = Network->countMatchingRealUserHost(curGline->getHost());
	us[0] = '\0';
	sprintf(us,"%d",count);
	curGline->setReason(string("[") + us + string("] ") + curGline->getReason());
	curGline->setExpires(curGline->getExpires() + ::time(0));
	addGlineToUplink(curGline);
	if(curGlinePtr.second) //Do we need to add it to the db?
		{
		if(!curGline->Insert())
			{
			MsgChanLog("Error while adding gline on host %s to the database!\n",
				    curGline->getHost().c_str());
			}
		else
			{
			curGline->loadData(curGline->getHost());			
			}
		addGline(curGline);			
		} else {
			delete curGline;
		}
	}
		
return true;	
}


bool ccontrol::loadGlines()
{
//static const char *Main = "SELECT * FROM glines WHERE ExpiresAt > now()::abstime::int4";

if(!dbConnected)
	{
	return false;
	}

static const char *Main = "SELECT Id,Host,AddedBy,AddedOn,ExpiresAt,LastUpdated,Reason FROM glines";

stringstream theQuery;
theQuery	<< Main
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::loadGlines> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( !SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::loadGlines> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false;
	}

ccGline *tempGline = NULL;

inRefresh = true;

for( unsigned int i = 0 ; i < SQLDb->Tuples() ; i++ )
	{
	tempGline =  new (std::nothrow) ccGline(SQLDb);
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

bool ccontrol::loadUsers()
{
 
if(!dbConnected)
        {   
        return false;
        }
   
stringstream theQuery;
theQuery        << User::Query
                << ends;

#ifdef LOG_SQL
elog    << "ccotrol::loadUsers> "
        << theQuery.str().c_str()
        << endl; 
#endif

if( !SQLDb->Exec( theQuery, true ) )
//if (PGRES_TUPLES_OK != status)
        {
        return false;
        }
ccUser* tempUser = 0;
for(unsigned int i =0;i<SQLDb->Tuples();++i)
	{
	tempUser = new (std::nothrow) ccUser(SQLDb);
	tempUser->setID(atoi(SQLDb->GetValue(i, 0).c_str()));
        tempUser->setUserName(SQLDb->GetValue(i, 1));
        tempUser->setPassword(SQLDb->GetValue(i, 2));
	tempUser->setAccess(atol(SQLDb->GetValue(i, 3).c_str()));
	tempUser->setSAccess(atol(SQLDb->GetValue(i, 4).c_str()));
	tempUser->setFlags(atoi(SQLDb->GetValue(i, 5).c_str()));
	tempUser->setSuspendExpires(atoi(SQLDb->GetValue(i,6).c_str()));
	tempUser->setSuspendedBy(SQLDb->GetValue(i,7));
	tempUser->setServer(SQLDb->GetValue(i,8));
	tempUser->setIsSuspended(!strcasecmp(SQLDb->GetValue(i,9),"t"));
	if(!strcasecmp(SQLDb->GetValue(i,10),"t"))
		{
		tempUser->setType(operLevel::UHSLEVEL);
		}
	else if(!strcasecmp(SQLDb->GetValue(i,11),"t"))
		{
		tempUser->setType(operLevel::OPERLEVEL);
		}
	else if(!strcasecmp(SQLDb->GetValue(i,12),"t"))
		{
		tempUser->setType(operLevel::ADMINLEVEL);
		}
	else if(!strcasecmp(SQLDb->GetValue(i,13),"t"))
		{
		tempUser->setType(operLevel::SMTLEVEL);
		}
	else if(!strcasecmp(SQLDb->GetValue(i,14),"t"))
		{
		tempUser->setType(operLevel::CODERLEVEL);
		}
	tempUser->setLogs(!strcasecmp(SQLDb->GetValue(i,15),"t"));
	tempUser->setNeedOp(!strcasecmp(SQLDb->GetValue(i,16),"t"));
	tempUser->setEmail(SQLDb->GetValue(i,17));
	tempUser->setSuspendLevel(atoi(SQLDb->GetValue(i,18).c_str()));
	tempUser->setSuspendReason(SQLDb->GetValue(i,19));
	if(!strcasecmp(SQLDb->GetValue(i,20),"t"))
		{
		tempUser->setNotice(true);
		}
	else
		{
		tempUser->setNotice(false);
		}
	tempUser->setLag(!strcasecmp(SQLDb->GetValue(i,21),"t"));
	tempUser->setPassChangeTS(atoi(SQLDb->GetValue(i,22).c_str()));
	usersMap[tempUser->getUserName()]=tempUser;

	}
return true;
}

bool ccontrol::loadServers()
{
 
if(!dbConnected)
        {   
        return false;
        }
   
stringstream theQuery;
theQuery        << server::Query
                << ends;

#ifdef LOG_SQL
elog    << "ccotrol::loadServers> "
        << theQuery.str().c_str()
        << endl; 
#endif

if( !SQLDb->Exec( theQuery, true ) )
//if (PGRES_TUPLES_OK != status)
        {
        return false;
        }
ccServer* tempServer = 0;
for(unsigned int i =0;i<SQLDb->Tuples();++i)
	{
	tempServer = new (std::nothrow) ccServer(SQLDb);
	assert(tempServer != NULL);
	tempServer->loadDataFromDB(i);
	serversMap[tempServer->getName()] = tempServer;
	}
return true;
}

bool ccontrol::loadMaxUsers()
{
 
if(!dbConnected)
        {   
        return false;
        }
   
stringstream theQuery;
theQuery        << "SELECT * FROM misc WHERE VarName = 'MaxUsers';"
                << ends;

#ifdef LOG_SQL
elog    << "ccotrol::loadMaxUsers> "
        << theQuery.str().c_str()
        << endl; 
#endif

if( !SQLDb->Exec( theQuery, true ) )
//if (PGRES_TUPLES_OK != status)
        {
        elog << "Error on loading maxusers: " << SQLDb->ErrorMessage() << endl;
	return false;
        }
if(SQLDb->Tuples() == 0)
	{
	maxUsers = 0;
	dateMax = 0;
	stringstream insertQ;
	insertQ << "INSERT INTO misc (VarName,Value1,Value2) VALUES ('MaxUsers',0,0);"
		<< ends;

	if( ! SQLDb->Exec( insertQ ) )
//	if (PGRES_COMMAND_OK != status)
    		{
		return false;
	        }
	}
else
	{
	maxUsers = atoi(SQLDb->GetValue(0,1).c_str());
	dateMax = atoi(SQLDb->GetValue(0,2).c_str());
	}
return true;
}

bool ccontrol::loadVersions()
{

if(!dbConnected)
        {   
        return false;
        }
   
if( !SQLDb->Exec( "SELECT * FROM misc WHERE VarName = 'Version'", true ) )
//if (PGRES_TUPLES_OK != status)
        {
        return false;
        }

for(unsigned int i =0;i<SQLDb->Tuples();++i)
	{
	VersionsList.push_back(SQLDb->GetValue(i,1));
	}

if( !SQLDb->Exec( "SELECT * FROM misc WHERE VarName = 'CheckVer'", true ) )
//if (PGRES_TUPLES_OK != status)
        {
        return false;
        }

if(SQLDb->Tuples() == 0)
	{
	checkVer = false;
	if( !SQLDb->Exec( "INSERT INTO misc (VarName,Value1) VALUES ('CheckVer',0)") )
//	if(PGRES_COMMAND_OK != status)
		{
		return false;
		}
	}
else
	{
	checkVer = atoi(SQLDb->GetValue(0,1).c_str());
	}
return true;
}

bool ccontrol::loadBadChannels()
{
        
if(!dbConnected) 
        {       
        return false;
        }

stringstream theQuery;
theQuery        << badChannels::Query
                << ends;
         
#ifdef LOG_SQL
elog    << "ccotrol::loadBadChannels> "
        << theQuery.str()
        << endl;
#endif

if( !SQLDb->Exec( theQuery.str(), true ) )
	{
//delete[] theQuery.str() ;
//if (PGRES_TUPLES_OK != status)
        return false;
        }
ccBadChannel* tempBad;
for(unsigned int i =0;i<SQLDb->Tuples();++i)
        {
        tempBad = new (std::nothrow) ccBadChannel(SQLDb,i);
        assert(tempBad != NULL);
        badChannelsMap[tempBad->getName()] = tempBad;
        }
return true;
}

bool ccontrol::loadMisc()
{
bool gotInterval= false;
bool gotCount = false;
bool gotVClones = false;
bool gotClones = false;
bool gotCClones = false;
bool gotCClonesCIDR = false;
bool gotCClonesTime = false;
bool gotCClonesGline = false;
bool gotCClonesGTime = false;
bool gotIClones = false;
bool gotIClonesGline = false;
bool gotGLen = false;
bool gotSave = false;
 
if(!dbConnected)
        {   
        return false;
        }
   
stringstream theQuery;
theQuery        << "SELECT * FROM misc"
                << ends;

#ifdef LOG_SQL
elog    << "ccotrol::loadMisc()> "
        << theQuery.str().c_str()
        << endl; 
#endif

if( !SQLDb->Exec( theQuery, true ) )
//if (PGRES_TUPLES_OK != status)
        {
        elog << "Error on loading misc : " << SQLDb->ErrorMessage() << endl;
	return false;
        }

for(unsigned int i=0; i< SQLDb->Tuples();++i)
	{
	if(!strcasecmp(SQLDb->GetValue(i,0),"GlineBurstCount"))
		{
		gotCount = true;
		glineBurstCount = atoi(SQLDb->GetValue(i,1).c_str());
		}
	else if(!strcasecmp(SQLDb->GetValue(i,0),"GlineBurstInterval"))
		{
		gotInterval = true;
		glineBurstInterval = atoi(SQLDb->GetValue(i,1).c_str());
		}
	else if(!strcasecmp(SQLDb->GetValue(i,0),"VClones"))
		{
		gotVClones = true;
		maxVClones = atoi(SQLDb->GetValue(i,1).c_str());
		}
	else if(!strcasecmp(SQLDb->GetValue(i,0),"Clones"))
		{
		gotClones = true;
		maxClones = atoi(SQLDb->GetValue(i,1).c_str());
		}
        else if(!strcasecmp(SQLDb->GetValue(i,0),"CClones"))
                {
                gotCClones = true;
                maxCClones = atoi(SQLDb->GetValue(i,1).c_str());
                }
        else if(!strcasecmp(SQLDb->GetValue(i,0),"CClonesCIDR"))
                {
                gotCClonesCIDR = true;
                CClonesCIDR = atoi(SQLDb->GetValue(i,1).c_str());
                }
	else if(!strcasecmp(SQLDb->GetValue(i,0),"CClonesTime"))
		{
		gotCClonesTime = true;
		CClonesTime = atoi(SQLDb->GetValue(i,1).c_str());
		}
        else if(!strcasecmp(SQLDb->GetValue(i,0),"CClonesGline"))
                {
                gotCClonesGline = true;
                CClonesGline = (atoi(SQLDb->GetValue(i,1).c_str()) == 1);
                }
	else if(!strcasecmp(SQLDb->GetValue(i,0),"IClones"))
		{
		gotIClones = true;
		maxIClones = atoi(SQLDb->GetValue(i,1).c_str());
		}
	else if(!strcasecmp(SQLDb->GetValue(i,0),"IClonesGline"))
		{
		gotIClonesGline = true;
		IClonesGline = (atoi(SQLDb->GetValue(i,1).c_str()) == 1);
		}
	else if(!strcasecmp(SQLDb->GetValue(i,0),"GTime"))
		{
		gotGLen = true;
		maxGlineLen = atoi(SQLDb->GetValue(i,1).c_str());
		}

	else if(!strcasecmp(SQLDb->GetValue(i,0),"SGline"))
		{
		gotSave = true;
		saveGlines = (atoi(SQLDb->GetValue(i,1).c_str()) == 1);
		}

	else if(!strcasecmp(SQLDb->GetValue(i,0),"CClonesGTime"))
		{
		gotCClonesGTime = true;
		CClonesGTime = atoi(SQLDb->GetValue(i,1).c_str());
		}
	}
if(!gotCount)
	{
	glineBurstCount = 5;
	updateMisc("GlineBurstCount",glineBurstCount);
	}
if(!gotInterval)
	{
	glineBurstInterval = 5;
	updateMisc("GlineBurstInterval",glineBurstInterval);
	}
if(!gotClones)
	{
	maxClones = 32;
	updateMisc("Clones",maxClones);
	}
if(!gotVClones)
	{
	maxVClones = 32;
	updateMisc("VClones",maxVClones);
	}
if(!gotCClones)
        {
        maxCClones = 275;
        updateMisc("CClones",maxCClones);
        }
if(!gotCClonesCIDR)
        {
        CClonesCIDR = 24;
        updateMisc("CClonesCIDR",CClonesCIDR);
        }
if(!gotCClonesTime)
	{
	CClonesTime = 60;
	updateMisc("CClonesTime",CClonesTime);
	}
if(!gotCClonesGline)
        {
        CClonesGline = false;
        updateMisc("CClonesGline",CClonesGline);
        }
if(!gotIClones)
	{
	maxIClones = 20;
	updateMisc("IClones",maxIClones);
	}
if(!gotIClonesGline)
	{
	IClonesGline = false;
	updateMisc("IClonesGline",IClonesGline);
	}
if(!gotGLen)
	{
	maxGlineLen = 3600;
	updateMisc("GTime",maxGlineLen);
	}
if(!gotSave)
	{
	saveGlines = true;
	updateMisc("SGline",saveGlines);
	}
if(!gotCClonesGTime)
	{
	CClonesGTime = maxGlineLen;
	updateMisc("CClonesGTime",CClonesGTime);
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

void ccontrol::addGlinedException( const string &Host )
{
	glinedExceptionList.push_back(pair<string,int>(Host, ::time(0)));
}

int ccontrol::isGlinedException( const string &Host )
{
	for (glinedExceptionListType::iterator ptr = glinedExceptionList.begin(); ptr != glinedExceptionList.end(); ptr++) {
		if ((::time(0) - (ptr)->second) > 300) {
			glinedExceptionList.erase(ptr);
			continue;
		}
		if (Host == (ptr)->first) {
			return (ptr)->second;
		}
	}
	return 0;
}

int ccontrol::getExceptions( const string &Host )
{
int Exception = 0;
string::size_type pos = Host.find_first_of('@');
string::size_type Maskpos;
string Ident = Host.substr(0,pos);
string Hostname = Host.substr(pos+1);
string MaskHostname, MaskIdent;

for(exceptionIterator ptr = exception_begin();ptr != exception_end();ptr++)
	{
        /* move the exception host into MaskHostname and strip off the user */
        MaskHostname = (*ptr)->getHost().c_str();
        Maskpos = MaskHostname.find_first_of('@');
        MaskIdent = MaskHostname.substr(0,Maskpos);
        if((*(*ptr) == Hostname) || !(match(MaskHostname.substr(Maskpos+1),Hostname)))
		{
                /* ok, we matched hostname(ip) - check if we match ident too */
                if (!match(MaskIdent, Ident))
			if((*ptr)->getConnections() > Exception)
			{
				Exception = (*ptr)->getConnections();
			}
		} 
	}

return Exception;
}

bool ccontrol::isCidrMatch( const string& cidrmask1, const string& cidrmask2 )
{
	int client_addr[4] = { 0 };
	struct in_addr tmp_ip;
	unsigned long mask_ip;
	char *client_ip;
	int i = 0;
	int CIDR;
	int CIDR1;
	int CIDR2;
	string cidrmask;
	string tIP;

	StringTokenizer st;

	
	StringTokenizer st1(cidrmask1, '/');

	if (st1.size() != 2)
		CIDR1 = 32;
	else
		CIDR1 = atoi(st1[1].c_str());
	StringTokenizer st2(cidrmask2, '/');

	if (st2.size() != 2)
		CIDR2 = 32;
	else
		CIDR2 = atoi(st2[1].c_str());

	if (CIDR2 > CIDR1) {
		CIDR = CIDR1;
		cidrmask = cidrmask1;
		st = st1;
		tIP = st2[0];
	}
	else {
		CIDR = CIDR2;
		cidrmask = cidrmask2;
		st = st2;
		tIP = st1[0];
	}



	/* CIDR checks */
	/* convert ip to longip */
	i = sscanf(tIP.c_str(), "%d.%d.%d.%d", &client_addr[0], &client_addr[1], &client_addr[2], &client_addr[3]);
	mask_ip = ntohl((client_addr[0]) | (client_addr[1] << 8) | (client_addr[2] << 16) | (client_addr[3] << 24));
	/* bitshift ip to strip the last (32-cidrmask) bits (leaving a mask for the ip) */
	for (i = 0; i < (32-CIDR); i++) {
		/* right shift */
		mask_ip >>= 1;
	}
	for (i = 0; i < (32-CIDR); i++) {
		/* left shift */
		mask_ip <<= 1;
	}
	/* convert longip back to ip */
	mask_ip = htonl(mask_ip);
	tmp_ip.s_addr = mask_ip;
	client_ip = inet_ntoa(tmp_ip);
	if (strcmp(client_ip,st[0].c_str()) == 0) {
		return true;
	}
	return false;
}


bool ccontrol::isValidCidr( const string& cidrmask )
{
	int client_addr[4] = { 0 };
	struct in_addr tmp_ip;
	unsigned long mask_ip;
	char *client_ip;
	int i = 0;
	int CIDR = 32;


	if (cidrmask.size() > 18)
		return false;

	StringTokenizer st(cidrmask,'/');
	if (st.size() != 2)
		return false;
	StringTokenizer st2(cidrmask,'.');
	if (st2.size() != 4)
		return false;

	CIDR = atoi(st[1].c_str());
	if ((CIDR < 0) || (CIDR > 32))
		return false;


	/* CIDR checks */
	/* convert ip to longip */
	i = sscanf(st[0].c_str(), "%d.%d.%d.%d", &client_addr[0], &client_addr[1], &client_addr[2], &client_addr[3]);
	mask_ip = ntohl((client_addr[0]) | (client_addr[1] << 8) | (client_addr[2] << 16) | (client_addr[3] << 24));
	/* bitshift ip to strip the last (32-cidrmask) bits (leaving a mask for the ip) */
	for (i = 0; i < (32-CIDR); i++) {
		/* right shift */
		mask_ip >>= 1;
	}
	for (i = 0; i < (32-CIDR); i++) {
		/* left shift */
		mask_ip <<= 1;
	}
	/* convert longip back to ip */
	mask_ip = htonl(mask_ip);
	tmp_ip.s_addr = mask_ip;
	client_ip = inet_ntoa(tmp_ip);
	if (strcmp(client_ip,st[0].c_str()) == 0) {
		return true;
	}
	return false;
}

std::list< string >* ccontrol::getOtherCidrs( const string& cidrmask )
//stringListType* ccontrol::getOtherCidrs( const string& cidrmask )
{
	/* If we're here, it's because there's an exception on a netblock smaller than CClonesCIDR (a /24) and we want
	 * to get a list of all netblocks that would cover the non-exempted shell netblocks.
	 * You will probably dislike how this function was written, I do too. It's not efficient at all,
	 * but atleast it does the work.
	 */


	int i=0;
	int j=0;
	int k=0;
	int highest = CClonesCIDR + 1;
	int lowest = 32;
	std::list< ccShellnb* > tmpList;
	static stringListType* theList = 0;
	StringTokenizer st(cidrmask,'/');
	string c;

    i = cidrmask.find_last_of(".");
	c = cidrmask.substr(0,i+1);

	elog << "Debug: " << cidrmask << ": " 
	     << c << endl;



	if (theList == 0) {
		theList = new (std::nothrow) stringListType;
		assert(theList != 0);
	}
	theList->clear();

	// This function will only work for /24s
	if (atoi(st[1].c_str()) != 24) {
        return theList;
	}


	for (shellnbIterator ptr = shellnbList.begin(); ptr != shellnbList.end(); ptr++) {
		if (isCidrMatch(cidrmask, (*ptr)->getCidr())) {
			tmpList.push_back((*ptr));
			StringTokenizer st3((*ptr)->getCidr(),'/');
			if (st3.size() != 2)
				continue;
			if (atoi(st3[1].c_str()) > highest)
				highest = atoi(st3[1].c_str());
			//if (atoi(st3[1].c_str()) < lowest)
			//	lowest = atoi(st3[1].c_str());
		}
	}

	k = 1;
	lowest = 24;
	for (i = 32; i > lowest; i--)
		k = k * 2;

	for (i = lowest; i <= highest; i++) {

		for (j = 0; j != 256; j += k) {
			bool matched = false;
			std::stringstream tmpCidr;
			tmpCidr << c << j << "/" << i;
			for (shellnbIterator ptr = tmpList.begin(); ptr != tmpList.end(); ptr++) {
				if (isCidrMatch((*ptr)->getCidr(),tmpCidr.str())) {
					matched = true;
					break;
				}
			}
			if (matched)
				continue;
			for (stringListType::iterator ptr = theList->begin(); ptr != theList->end(); ptr++) {
				if (isCidrMatch((*ptr),tmpCidr.str())) {
					matched = true;
					break;
				}
			}
			if (!matched) {
				theList->push_back(tmpCidr.str());
			}
		}
		k = k / 2;
	}

	return theList;
}

bool ccontrol::test( iClient *theClient, const string& Cidr )
{
		std::list< string >* astr;
		astr = getOtherCidrs(Cidr);
		std::list< string >::iterator ptr = astr->begin();
		for (; ptr != astr->end(); ptr++)
		{
			Notice(theClient, "%s", (*ptr).c_str());
		}
		return true;
}

ccShellco* ccontrol::getShellco( const string& Name )
{
for (shellcoIterator ptr = shellcoList.begin(); ptr != shellcoList.end(); ptr++)
	if (!strcasecmp(Name.c_str(),(*ptr)->getName().c_str()))
		return *ptr;
return 0;
}

ccShellco* ccontrol::getShellcobyID( const int& id)
{
for (shellcoIterator ptr = shellcoList.begin(); ptr != shellcoList.end(); ptr++)
	if (id == (*ptr)->getID())
		return *ptr;
return 0;
}

ccShellnb* ccontrol::getShellnb( const string& Cidr )
{
for (shellnbIterator ptr = shellnbList.begin(); ptr != shellnbList.end(); ptr++)
	if (Cidr == (*ptr)->getCidr())
		return *ptr;
return 0;
}

bool ccontrol::listShellExceptions( iClient *theClient )
{

Notice(theClient,"-= Shell Exceptions list - listing a total of %d shell companies =-",
	shellcoList.size());

for (shellcoIterator ptr = shellcoList.begin(); ptr != shellcoList.end(); ptr++) {
	int i = 0;
	bool multiple_lines = false;
	stringstream s;
	s << (*ptr)->getName() << ":   Limit: " << (*ptr)->getLimit() << " (" << shellcoMap[*ptr] << " online)    Netblocks: ";
	for (shellnbIterator nptr = shellnbList.begin(); nptr != shellnbList.end(); nptr++) {
		if ((*nptr)->shellco == *ptr) {
			i++;
			if (i > 1)
				s << ", ";
			s << (*nptr)->getCidr() << " (" << shellnbMap[*nptr] << ")";
			if (s.str().size() > 200) {
				multiple_lines = true;
				i = 0;
				Notice(theClient, "%s", s.str().c_str());
				s.str("");
			}
		}
	}
	if ((i != 0) || (multiple_lines == false)) {
		Notice(theClient, "%s", s.str().c_str());
	}
}

Notice(theClient,"-= End of shell exception list =-");

return true;
}

bool ccontrol::listExceptions( iClient *theClient )
{

Notice(theClient,"-= Exceptions list - listing a total of %d exceptions =-",
	exceptionList.size());

for(exceptionIterator ptr = exception_begin();ptr != exception_end();ptr++)
	Notice(theClient,"Host: %s   Connections: %d   AddedBy: %s   Reason: %s",
		(*ptr)->getHost().c_str(),
		(*ptr)->getConnections(),
		(*ptr)->getAddedBy().c_str(),
		(*ptr)->getReason().c_str());

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



bool ccontrol::insertException( iClient *theClient , const string& Host , int Connections, const string& Reason )
{

if(!dbConnected)
	{
	return false;
	}

if(isException(Host))
	{
	Notice(theClient,
		"There is already an exception for host %s, "
		"please use update",
		Host.c_str());		
	return true;
	}

//Create a new ccException structure 
ccException* tempException = new (std::nothrow) ccException(SQLDb);
assert(tempException != NULL);

tempException->setHost(removeSqlChars(Host));
tempException->setConnections(Connections);
tempException->setAddedBy(removeSqlChars(theClient->getRealNickUserHost()));
tempException->setAddedOn(::time(0));
tempException->setReason(Reason);
//Update the database, and the internal list
if(!tempException->Insert())
	{
	delete tempException;
	return false;
	}

exceptionList.push_back(tempException);
return true;
}

bool ccontrol::insertShellco( iClient *theClient , const string& Name , int Connections )
{
//Check if there is already an exception on that host

if(!dbConnected)
	{
	Notice(theClient, "error: DB not connected.");
	return false;
	}

if(getShellco(Name) != 0)
	{
	Notice(theClient,
		"There is already a shell company called %s, "
		"please use update",
		Name.c_str());		
	return false;
	}

//Create a new ccShellco structure 
ccShellco* tempShellco = new (std::nothrow) ccShellco(SQLDb);
assert(tempShellco != NULL);

tempShellco->setName(removeSqlChars(Name));
tempShellco->setLimit(Connections);
tempShellco->setAddedBy(removeSqlChars(theClient->getRealNickUserHost()));
tempShellco->setAddedOn(::time(0));
tempShellco->setModBy(removeSqlChars(theClient->getRealNickUserHost()));
tempShellco->setModOn(::time(0));
//Update	the database, and the internal list
if(!tempShellco->Insert())
	{
	delete tempShellco;
	Notice(theClient, "SQL Insertion failed.");
	return false;
	}

tempShellco->loadData(tempShellco->getName()); //The inserted id is needed in memory

shellcoMap.insert(std::make_pair<ccShellco*,int>(tempShellco,0));
shellcoList.push_back(tempShellco);
return true;
}

bool ccontrol::insertShellnb( iClient *theClient , const string& Cidr, int Company )
{
int count=0;
StringTokenizer st(Cidr,'/');

if (!isValidCidr(Cidr)) 
	{
		Notice(theClient, "Invalid cidr: %s", Cidr.c_str());
		return false;
	}

if(!dbConnected)
	{
	Notice(theClient, "error: DB not connected.");
	return false;
	}


if (CClonesCIDR != 24)
	{
	Notice(theClient, "SHELLS exception will only work properly if you type that command: /msg <mynick> CONFIG -CClonesCIDR 24");
	return false;
	}

if (atoi(st[1].c_str()) < 8)
	{
	Notice(theClient, "You can't add an exception for something bigger than a /8");
	return false;
	}
for (shellnbIterator ptr = shellnbList.begin(); ptr != shellnbList.end(); ptr++) 
	{
	if (isCidrMatch(Cidr,(*ptr)->getCidr()))
		{
			if ((*ptr)->getCompanyID() == Company)
				{
				Notice(theClient, "Can't add netblock %s because netblock %s is assigned to the %s company. Some IPs would match the two.", Cidr.c_str(), (*ptr)->getCidr().c_str(), (*ptr)->shellco->getName().c_str());
				return false;
				}
			else if (((*ptr)->getCidr2() > 24) && (atoi(st[1].c_str()) > 24))
				{
				Notice(theClient, "Can't add netblock %s: Some IPs are already covered by %s and would match the two netblocks inside the /24.", Cidr.c_str(), (*ptr)->getCidr().c_str());
				return false;
				}
		}
	}

//Create a new ccShellnb structure 
ccShellnb* tempShellnb = new (std::nothrow) ccShellnb(SQLDb);
assert(tempShellnb != NULL);

tempShellnb->setCidr(removeSqlChars(Cidr));
tempShellnb->setAddedBy(removeSqlChars(theClient->getRealNickUserHost()));
tempShellnb->setAddedOn(::time(0));
//tempShellnb->setModBy(removeSqlChars(theClient->getRealNickUserHost()));
//tempShellnb->setModOn(::time(0));
tempShellnb->setCompanyID(Company);
tempShellnb->set24Mask();
//Update the database, and the internal list
if(!tempShellnb->Insert())
	{
	delete tempShellnb;
	Notice(theClient, "SQL Insertion failed.");
	return false;
	}


tempShellnb->shellco = getShellcobyID(Company);
shellnbList.push_back(tempShellnb);

for( xNetwork::const_clientIterator cItr = Network->clients_begin() ; cItr != Network->clients_end() ; ++cItr )	{
	const iClient* tmpClient = cItr->second;
    const string tIP = xIP(tmpClient->getIP()).GetNumericIP();
		if (isCidrMatch(tempShellnb->getCidr(),tIP)) {
			count++;
			if (shellnbMap.find(tempShellnb) == shellnbMap.end()) {
				shellnbMap.insert(std::make_pair<ccShellnb*,int>(tempShellnb, 1));
			}
			else {
				shellnbMap[tempShellnb]++;
			}
			if (shellcoMap.find(tempShellnb->shellco) == shellcoMap.end()) {
				shellcoMap.insert(std::make_pair<ccShellco*,int>(tempShellnb->shellco, 1));
			}
			else {
				shellcoMap[tempShellnb->shellco]++;
			}
		}
}
Notice(theClient, "There are currently %c%d users%c online from %s", 2, count, 2, Cidr.c_str());
return true;
}

bool ccontrol::delShellnb( iClient *theClient , const string &Host )
{
ccShellnb* Shellnb;
if(!dbConnected) 
	{
	Notice(theClient, "error: DB not connected.");
	return false;
	}

Shellnb = getShellnb(Host);
if (Shellnb == 0) {
	Notice(theClient, "Can't find shell exception for host %s", Host.c_str());
	return false;
}
bool status = Shellnb->Delete();
shellnbListType::iterator Itr = shellnbList.begin();
while (Itr != shellnbList.end()) {
	if (*Itr == Shellnb) {
		Itr = shellnbList.erase(Itr);
	}
	else
		Itr++;
}
shellcoMap[Shellnb->shellco] -= shellnbMap[Shellnb];
shellnbMap.erase(Shellnb);

delete Shellnb;
return status;

}

bool ccontrol::clearShells( iClient *theClient )
{
bool ret = true;
list<string> s;
for (shellcoIterator ptr = shellcoList.begin(); ptr != shellcoList.end(); ptr++) {
	string Name = (*ptr)->getName();
	s.push_back(Name);
}
for (list<string>::iterator ptr = s.begin(); ptr != s.end(); ptr++) {
	Notice(theClient, "Deleting Shellco '%s'", ptr->c_str());
	if (delShellco(theClient, *ptr) == false)
		ret = false;
}
s.clear();
return ret;
}

bool ccontrol::delShellco( iClient *theClient , const string &Name )
{
ccShellco* Shellco;
if(!dbConnected) 
	{
	Notice(theClient, "error: DB not connected.");
	return false;
	}

Shellco = getShellco(Name);
if (Shellco == 0) {
	Notice(theClient, "Can't find shell company: %s", Name.c_str());
	return false;
}

shellcoListType::iterator Itr = shellcoList.begin();
while (Itr != shellcoList.end()) {
	if (*Itr == Shellco) {
		Itr = shellcoList.erase(Itr);
	}
	else
		Itr++;
}

shellnbIterator ptr = shellnbList.begin();
while (ptr != shellnbList.end()) {
	shellnbIterator ptr2;
	if ((*ptr)->shellco == Shellco) {
		ptr2 = ptr;
		ptr2++;
		if (!delShellnb(theClient, (*ptr)->getCidr())) {
			Notice(theClient,"Error while deleting netblock '%s'",(*ptr)->getCidr().c_str());
		}
		ptr = ptr2;
	}
	else
		ptr++;
}
bool status = Shellco->Delete();
shellcoMap.erase(Shellco);

delete Shellco;

return status;

}

bool ccontrol::delException( iClient *theClient , const string &Host )
{

if(!dbConnected)
	{
	Notice(theClient, "error: DB not connected.");
	return false;
	}

if(!isException(removeSqlChars(Host)))
	{
	Notice(theClient,"Can't find exception for host %s",Host.c_str());
	return true;
	}
ccException *tempException = NULL;

for(exceptionIterator ptr = exception_begin();ptr != exception_end();)
	{
	tempException = *ptr;
	if(*tempException == removeSqlChars(Host))
		{
		bool status = tempException->Delete();
		ptr = exceptionList.erase(ptr);
		delete tempException;
		if(!status) 
			{
			return false;
			}
		}
	    
	else
		ptr++;
	}
return true;
}	

ccFloodData *ccontrol::findLogin( const string & Numeric )
{
for(ignoreIterator ptr = ignore_begin() ; ptr != ignore_end() ; ++ptr)
	{
	if((*ptr)->getNumeric() == Numeric)
		{
		return *ptr;
		}
	}
return NULL;
}

void ccontrol::removeLogin( ccFloodData *tempLogin )
{
for(ignoreIterator ptr = ignore_begin() ; ptr != ignore_end() ;)
	{
	if((*ptr) == tempLogin)
		{
		ptr = ignoreList.erase(ptr);
		}
	else
		ptr++;
	}
}

void ccontrol::addLogin( iClient* tClient)
{
if(!tClient->getCustomData(this))
	{
	elog << "Couldn't find custom data for " 
	     << tClient->getNickName() << endl;
	return;
	}

ccFloodData *LogInfo = static_cast<ccUserData*>( tClient->getCustomData(this))->getFlood();

LogInfo->add_Login();
if(LogInfo->getLogins() > 5)
	{
	ignoreUser(LogInfo);
	}
}	

int ccontrol::removeIgnore( const string &Host )
{

ccFloodData *tempLogin = 0;
int retMe = IGNORE_NOT_FOUND;

for(ignoreIterator ptr = ignore_begin();ptr!=ignore_end();)
	{
	tempLogin = *ptr;
	if(tempLogin->getIgnoredHost() == Host)
		{
		stringstream s;
		s	<< getCharYYXXX() 
			<< " SILENCE " 
			<< "*"
			<< " -" 
			<< tempLogin->getIgnoredHost()
			<< ends; 
		Write( s );

		tempLogin->resetIgnore();
		tempLogin->resetLogins();
		ptr = ignoreList.erase(ptr);
		if(tempLogin->getNumeric() == "0")
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
		+ theClient->getRealInsecureHost();
int retMe = removeIgnore(Host);
return retMe;
}

void ccontrol::ignoreUser( ccFloodData* Flood )
{

const iClient* theClient = Network->findClient(Flood->getNumeric());
Notice(theClient,"I don't think I like you anymore, consider yourself ignored!");
MsgChanLog("Added %s to my ignore list\n",theClient->getRealNickUserHost().c_str());

string silenceMask = string( "*!*" )
	+ theClient->getUserName()
	+ "@"
	+ theClient->getRealInsecureHost();

stringstream s;
s	<< getCharYYXXX() 
	<< " SILENCE " 
	<< theClient->getCharYYXXX() 
	<< " " 
	<< silenceMask
	<< ends; 

Write( s );
Flood->setIgnoreExpires(::time(0)+ flood::IGNORE_TIME);
Flood->setIgnoredHost(silenceMask);

ignoreList.push_back(Flood);
}

bool ccontrol::listIgnores( iClient *theClient )
{
Notice(theClient,"-= Listing Ignore List =-");			
ccFloodData *tempLogin;
for(ignoreIterator ptr = ignore_begin();ptr!=ignore_end();ptr++)
	{
	tempLogin = *ptr;
	if(tempLogin->getIgnoreExpires() > ::time(0))
		{
		Notice(theClient,"Host: %s Expires: %s [%ld]",
		tempLogin->getIgnoredHost().c_str(),
		convertToAscTime(tempLogin->getIgnoreExpires()),
		(long)tempLogin->getIgnoreExpires());
		}
	}
Notice(theClient,"-= End Of Ignore List =-");			
return true ;
}

bool ccontrol::refreshIgnores()
{
ccFloodData *tempLogin;
unsigned int TotalFound =0;
for(ignoreIterator ptr = ignore_begin();ptr!=ignore_end();)
	{
	tempLogin = *ptr;
	if((tempLogin) &&(tempLogin->getIgnoreExpires() <= ::time(0)))
		{
		tempLogin->setIgnoreExpires(0);
		stringstream s;
		s	<< getCharYYXXX() 
			<< " SILENCE " 
			<< "*"
			<< " -" 
			<< tempLogin->getIgnoredHost()
			<< ends; 

		Write( s );

		tempLogin->setIgnoredHost("");
		if(tempLogin->getNumeric() == "0")
			{
			delete tempLogin;
			}
		ptr = ignoreList.erase(ptr);
		++TotalFound;
		}
	else
		ptr++;
	}
if(TotalFound > 0)
	MsgChanLog("[Refresh Ignores] - %d expired\n",TotalFound);

return true;

}
bool ccontrol::loadExceptions()
{
static const char Query[] = "SELECT Host,Connections,AddedBy,AddedOn,Reason FROM Exceptions";
static const char Query2[] = "SELECT name,id,AddedBy,AddedOn,lastmodby,lastmodon,maxlimit,active FROM ShellCompanies";
static const char Query3[] = "SELECT cidr,companyid,AddedBy,AddedOn FROM ShellNetblocks";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Query
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::loadExceptions> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( !SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::loadExceptions> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	
	return false;
	}

ccException *tempException = NULL;

for( unsigned int i = 0 ; i < SQLDb->Tuples() ; i++ )
	{
	tempException =  new (std::nothrow) ccException(SQLDb);
	assert( tempException != 0 ) ;

	tempException->setHost(SQLDb->GetValue(i,0));
	tempException->setConnections(atoi(SQLDb->GetValue(i,1).c_str()));
	tempException->setAddedBy(SQLDb->GetValue(i,2)) ;
	tempException->setAddedOn(static_cast< time_t >(
		atoi( SQLDb->GetValue(i,3).c_str() ) )) ;
	tempException->setReason(SQLDb->GetValue(i,4));
	exceptionList.push_back(tempException);
	}


theQuery.str("");
theQuery	<< Query2
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::loadExceptions> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( !SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::loadExceptions> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	
	return false;
	}

ccShellco *tempShellco = NULL;

for( unsigned int i = 0 ; i < SQLDb->Tuples() ; i++ )
	{

	tempShellco =  new (std::nothrow) ccShellco(SQLDb);
	assert( tempShellco != 0 ) ;

	tempShellco->setName(SQLDb->GetValue(i,0));
	tempShellco->setID(atoi(SQLDb->GetValue(i,1).c_str()));
	tempShellco->setAddedBy(SQLDb->GetValue(i,2)) ;
	tempShellco->setAddedOn(static_cast< time_t >(
		atoi( SQLDb->GetValue(i,3).c_str() ) )) ;
	tempShellco->setModBy(SQLDb->GetValue(i,4)) ;
	tempShellco->setModOn(static_cast< time_t >(
		atoi( SQLDb->GetValue(i,5).c_str() ) )) ;
	tempShellco->setLimit(atoi(SQLDb->GetValue(i,6).c_str()));
	tempShellco->setActive(atoi(SQLDb->GetValue(i,7).c_str()));
	shellcoList.push_back(tempShellco);
	}


theQuery.str("");
theQuery	<< Query3
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::loadExceptions> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( !SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::loadExceptions> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	
	return false;
	}

ccShellnb *tempShellnb = NULL;

for( unsigned int i = 0 ; i < SQLDb->Tuples() ; i++ ) {

	tempShellnb =  new (std::nothrow) ccShellnb(SQLDb);
	assert( tempShellnb != 0 ) ;

	tempShellnb->setCidr(SQLDb->GetValue(i,0));
	int companyID = atoi(SQLDb->GetValue(i,1).c_str());
	tempShellnb->setCompanyID(companyID);
	tempShellnb->shellco = getShellcobyID(companyID);
	tempShellnb->setAddedBy(SQLDb->GetValue(i,2)) ;
	tempShellnb->setAddedOn(static_cast< time_t >(
		atoi( SQLDb->GetValue(i,3).c_str() ) )) ;
	//tempShellnb->setModBy(SQLDb->GetValue(i,4)) ;
	//tempShellnb->setModOn(static_cast< time_t >(
	//	atoi( SQLDb->GetValue(i,5).c_str() ) )) ;
	//tempShellnb->setLimit(atoi(SQLDb->GetValue(i,6).c_str()));
	tempShellnb->set24Mask();

	if (getShellcobyID(companyID) == 0) { //This should not happen
		MsgChanLog("Loading Shell netblock failed for %s. CompanyID %d not found.", tempShellnb->getCidr().c_str(), companyID);
		delete tempShellnb;
		continue;
	}
	shellnbList.push_back(tempShellnb);
}

return true;	
} 

void ccontrol::listGlines( iClient *theClient, string Mask )
{

ccGline* tempGline;
char gline_set[256];

Notice(theClient,"-= Gline List =-");
for(glineIterator ptr = gline_begin();ptr != gline_end();++ptr)
	{
	tempGline = ptr->second;
	if((tempGline ->getExpires() > ::time(0)) 
	    && (!match(Mask,tempGline->getHost())))
		{
		sprintf(gline_set, "%s", Duration(time(NULL) - tempGline->getAddedOn()));
		Notice(theClient,"Host: %s, Expires: [%ld] %s, Added by %s at [%ld] %s ago",
			tempGline->getHost().c_str(),
			tempGline->getExpires(),
			Duration(tempGline->getExpires() - time(NULL)),
			tempGline->getAddedBy().c_str(),
			tempGline->getAddedOn(),
			gline_set);
		}
	}

Notice(theClient,"-= RealName Gline List =-");
for(glineIterator ptr = rnGlineList.begin();ptr != rnGlineList.end();++ptr)
	{
	tempGline = ptr->second;
	if((tempGline ->getExpires() > ::time(0)) 
	    && (!match(Mask,tempGline->getHost())))
		{
		sprintf(gline_set, "%s", Duration(time(NULL) - tempGline->getAddedOn()));
		Notice(theClient,"Host: %s, Expires: [%ld] %s, Added by %s at [%ld] %s ago.",
			tempGline->getHost().c_str(),
			tempGline->getExpires(),
			Duration(tempGline->getExpires() - time(NULL)),
			tempGline->getAddedBy().c_str(),
			tempGline->getAddedOn(),
			gline_set);
		}
	}
Notice(theClient,"-= End Of Gline List =-");

}
			
void ccontrol::listSuspended( iClient * )
{

}

	
void ccontrol::listServers( iClient * theClient)
{
ccServer* tmpServer;
for(serversConstIterator ptr = serversMap_begin();ptr != serversMap_end();++ptr)
	{
	tmpServer = ptr->second;
	if(tmpServer->getNetServer())
		{
		char buf[512];
		stringstream s;
		s << "(";
		if (tmpServer->getLagTime() > 10000)
			{
			s << "\002" << (int) (tmpServer->getLagTime() / 1000) << "seconds\002)";
			}
		else if (tmpServer->getLagTime() > 5000)
			{
			s << "\002" << tmpServer->getLagTime() << "ms\002)";
			}
		else
			{
			s << tmpServer->getLagTime() << "ms)";
			}

		snprintf(buf, sizeof(buf) - 1, "%3s (%4d) Name: %s Version: %s",
			tmpServer->getLastNumeric().c_str(),
			base64toint(tmpServer->getLastNumeric().c_str()),
			tmpServer->getName().c_str(),
			tmpServer->getVersion().c_str());
		buf[sizeof(buf)] = '\0'; //Just in case
		Notice(theClient, "%s %s", buf, s.str().c_str());

		//Notice(theClient,"%3s (%4d) Name: %s Version: %s",
		//	tmpServer->getLastNumeric().c_str(),
		//	base64toint(tmpServer->getLastNumeric().c_str()),
		//	tmpServer->getName().c_str(),
		//	tmpServer->getVersion().c_str());
		}
	else if(tmpServer->getReportMissing())
		{
		Notice(theClient,"%3s (%4d) Name: %s Version: %s (\002*MISSING*\002)",
			tmpServer->getLastNumeric().c_str(),
			base64toint(tmpServer->getLastNumeric().c_str()),
			tmpServer->getName().c_str(),
			tmpServer->getVersion().c_str());
		}
	else
		{
		Notice(theClient,"           Name: %s Version: %s",
			tmpServer->getName().c_str(),
			tmpServer->getVersion().c_str());
		}		
	}
	 
}

void ccontrol::listBadChannels( iClient* theClient)
{
ccBadChannel* tempBadChan;
Notice(theClient, "-= Bad Channels (NOMODE) List =-");
for(badChannelsIterator ptr = badChannels_begin();ptr != badChannels_end();++ptr)
        {
        tempBadChan = ptr->second;
        Notice(theClient,"Channel: %s - Reason: %s - AddedBy: %s",tempBadChan->getName().c_str(),
                tempBadChan->getReason().c_str(),tempBadChan->getAddedBy().c_str());
        }
Notice(theClient, "-= End of Bad Channels (NOMODE) List =-");
}

void ccontrol::loadCommands()
{

static const char *Main = "SELECT * FROM Commands";

if(!dbConnected)
	{
	return;
	}

stringstream theQuery;
theQuery	<< Main
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::loadCommands> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( !SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::loadCommands> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return;
	}

Command* NewCom;

for( unsigned int i = 0 ; i < SQLDb->Tuples() ; i++ )
	{
	NewCom = findRealCommand(SQLDb->GetValue(i,0));
	if(!NewCom)
		{
		/*elog	<< "Can't find handler for command "
			<< SQLDb->GetValue(i,0)
			<< endl;	*/
		}
	else
		{
		NewCom->setName(SQLDb->GetValue(i,1));
		if(!strcasecmp(SQLDb->GetValue(i,3),"f"))
			NewCom->Enable();
		else
			NewCom->Disable();
		NewCom->setNeedOp(!strcasecmp(SQLDb->GetValue(i,4),"t") ? true : false);			
		NewCom->setNoLog(!strcasecmp(SQLDb->GetValue(i,5),"t") ? true : false);			

		}
	}

}

	
bool ccontrol::updateCommand ( Command* Comm)
{
static const char *Main = "UPDATE Commands set name = '";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< removeSqlChars(Comm->getName())
		<< "', isDisabled = "
		<< (Comm->getIsDisabled() ? "'t'" : "'n'")
		<< ", NeedOp = "
		<< (Comm->getNeedOp() ? "'t'" : "'n'")
		<< ", NoLog = "
		<< (Comm->getNoLog() ? "'t'" : "'n'")
		<< ", MinLevel = "
		<< Comm->getMinLevel() 
		<< " WHERE lower(RealName) = '"
		<< string_lower(removeSqlChars(Comm->getRealName()))
		<< "'"
		<< ends;

#ifdef LOG_SQL
elog	<< "ccontrol::updateCommands> "
	<< theQuery.str().c_str()
	<< endl; 
#endif

if( !SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK != status )
	{
	elog	<< "ccontrol::updateCommands> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false;
	}

RegisterCommand(Comm);
return true;

}
	
Command* ccontrol::findRealCommand( const string& commName)
{
for( commandMapType::iterator ptr = commandMap.begin() ;
	ptr != commandMap.end() ; ++ptr )
	if(!strcasecmp(ptr->second->getRealName(),commName))
		return ptr->second;
return NULL;
}

Command* ccontrol::findCommandInMem( const string& commName)
{
for( commandMapType::iterator ptr = commandMap.begin() ;
	ptr != commandMap.end() ; ++ptr )
	if(!strcasecmp(ptr->second->getName(),commName))
		return ptr->second;
return NULL;
}

		
bool ccontrol::UpdateCommandFromDb ( Command* Comm )
{
static const char *Main = "SELECT * FROM Commands WHERE lower(RealName) = '";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< string_lower(removeSqlChars(Comm->getRealName()))
		<< "'" << ends;

if( !SQLDb->Exec( theQuery, true ) )
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ccontrol::LoadCommand> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

	return false;
	}
if(SQLDb->Tuples() == 0)
	return false;
Comm->setName(SQLDb->GetValue(0,1));
if(!strcasecmp(SQLDb->GetValue(0,3),"t"))
	Comm->Disable();
else
	Comm->Enable();
Comm->setNeedOp((!strcasecmp(SQLDb->GetValue(0,4),"t")) ? true : false);
Comm->setNoLog((!strcasecmp(SQLDb->GetValue(0,5),"t")) ? true : false);
Comm->setMinLevel(atoi(SQLDb->GetValue(0,6).c_str()));

return true;

}

const string ccontrol::expandDbServer(const string& Name)
{

serversMapType::iterator serverIt;
for(serverIt = serversMap.begin();serverIt != serversMap.end();++serverIt)
	{
	if(!match(Name,serverIt->first))
		return serverIt->first;
	}
return "";

}

const string ccontrol::removeSqlChars(const string& Msg)
{
string NewString;

for(string::const_iterator ptr = Msg.begin(); ptr != Msg.end() ; ++ptr)
	{
	if(*ptr == ';')
		{
		NewString += ' ';
		}
	else if(*ptr == '\'')
		{
		NewString += "\\\047";
		}
	else if(*ptr == '\\')
		{
		NewString += "\\\134";
		}
	else
		{
		NewString += *ptr;
		}
	}
return NewString;

}

void ccontrol::checkDbConnection()
{
if( SQLDb->ConnectionBad() )
//if(SQLDb->Status() == CONNECTION_BAD) //Check if the connection had died
	{
	delete(SQLDb);
	dbConnected = false;
	updateSqldb(NULL);
	MsgChanLog("PANIC! - The connection with the database was lost!\n");
	MsgChanLog("Attempting to reconnect, attempt %d out of %d\n",
		    connectCount+1,connectRetry+1);
	string Query = "host=" + sqlHost + " dbname=" + sqlDb + " port=" + sqlPort;
	if (strcasecmp(sqlUser,"''"))
		{
		Query += (" user=" + sqlUser);
		}

	if (strcasecmp(sqlPass,"''"))
		{
		Query += (" password=" + sqlPass);
		}
	SQLDb = new dbHandle( sqlHost,
		atoi( sqlPort.c_str() ),
		sqlDb,
		sqlUser,
		sqlPass ) ;
//	SQLDb = new (std::nothrow) cmDatabase(Query.c_str());
	assert(SQLDb != NULL);
	
	if(SQLDb->ConnectionBad())
		{
		++connectCount;
		if(connectCount > connectRetry)
			{
			MsgChanLog("Can't connect to the database, quitting (no more retries)\n");
			::exit(1);
			}
		else
			{
			MsgChanLog("Attempt failed\n");
			}
		}
	else
		{
		dbConnected = true;
		MsgChanLog("The PANIC is over, database connectivity restored\n");
		updateSqldb(SQLDb);
		connectCount = 0;
		}
	}
	
	
}

void ccontrol::updateSqldb(dbHandle* _SQLDb)
{
for(glineIterator ptr = glineList.begin();ptr != glineList.end();++ptr) 
	{
	(ptr->second)->setSqldb(_SQLDb);
	}

for(glineIterator ptr = rnGlineList.begin();ptr != rnGlineList.end();++ptr) 
	{
	(ptr->second)->setSqldb(_SQLDb);
	}

for(exceptionIterator ptr = exception_begin();ptr != exception_end();++ptr)
	{
	(*ptr)->setSqldb(_SQLDb);
	}

for(shellcoIterator ptr = shellcoList.begin();ptr != shellcoList.end();++ptr)
	{
	(*ptr)->setSqldb(_SQLDb);
	}

for(shellnbIterator ptr = shellnbList.begin();ptr != shellnbList.end();++ptr)
	{
	(*ptr)->setSqldb(_SQLDb);
	}

for(usersIterator ptr = usersMap.begin();ptr != usersMap.end();++ptr)
	{
	ptr->second->setSqldb(_SQLDb);
	}

for(serversIterator ptr = serversMap.begin();ptr != serversMap.end();++ptr)
	{
	ptr->second->setSqldb(_SQLDb);
	}

}

void ccontrol::showStatus(iClient* tmpClient)
{
Notice(tmpClient,"CControl version %s.%s [%s]",MAJORVER,MINORVER,RELDATE);
Notice(tmpClient, "Service Uptime: %s", Ago(getUplink()->getStartTime()));
if(checkClones)
	{
	Notice(tmpClient,"Monitoring %d different clones hosts\n",clientsIpMap.size());
	Notice(tmpClient,"and %d different CIDR clones hosts (%d rate-limit entries)\n",
		clientsIp24Map.size(), clientsIp24MapLastWarn.size());
	Notice(tmpClient,"and %d different CIDR ident clones hosts (%d rate-limit entries)\n",
		clientsIp24IdentMap.size(), clientsIp24IdentMapLastWarn.size());
	Notice(tmpClient,"and %d different virtual clones hosts (%d rate-limit entries)\n",
		virtualClientsMap.size(), virtualClientsMapLastWarn.size());
	}	
Notice(tmpClient,"%d glines are waiting in the gline queue",glineQueue.size());
Notice(tmpClient,"Allocated Structures:");
Notice(tmpClient,"ccServer: %d, ccGline: %d, ccException: %d, ccUser: %d, ccShellco: %d, ccShellnb: %d",
	ccServer::numAllocated,
	ccGline::numAllocated,
	ccException::numAllocated,
	ccUser::numAllocated,
	ccShellco::numAllocated,
	ccShellnb::numAllocated);
Notice(tmpClient,"Total of %d users in the map",usersMap.size()); 
Notice(tmpClient,"(Gline Burst) - GBCount: %d , GBInterval: %d",
	glineBurstCount,
	glineBurstInterval);
Notice(tmpClient,"Max Clones: %d, Max Virtual Clones: %d",maxClones,maxVClones);
Notice(tmpClient,"Max Ident Clones: %d per /%d - Auto-Gline: %s",
	maxIClones,
	CClonesCIDR,
	IClonesGline ? "True" : "False");
Notice(tmpClient,"Max CIDR Clones: %d per /%d - Auto-Gline: %s (for %s)",
	maxCClones,CClonesCIDR,CClonesGline ? "YES" : "NO",
	Duration(CClonesGTime));
Notice(tmpClient,"  (%s between announcements per block)", Duration(CClonesTime));
Notice(tmpClient,"Save gline is: %s",saveGlines ? "Enabled" : "Disabled"); 
Notice(tmpClient,"Currently Bursting: %s",inBurst ? "YES" : "NO");
}

bool ccontrol::updateMisc(const string& varName, const unsigned int Value)
{
 
if(!strcasecmp(varName,"GlineBurstCount"))
	{
	glineBurstCount = Value;
	}
else if(!strcasecmp(varName,"GlineBurstInterval"))
	{
	glineBurstInterval = Value;
	}
else if(!strcasecmp(varName,"Clones"))
	{
	maxClones = Value;
	}
else if(!strcasecmp(varName,"VClones"))
	{
	maxVClones = Value;
	}
else if(!strcasecmp(varName,"CClones"))
        {
        maxCClones = Value;
        }
else if(!strcasecmp(varName,"CClonesCIDR"))
        {
        CClonesCIDR = Value;
        }
else if(!strcasecmp(varName,"CClonesTime"))
	{
	CClonesTime = Value;
	}
else if(!strcasecmp(varName,"CClonesGline"))
        {
        CClonesGline = (Value == 1);
	}
else if(!strcasecmp(varName,"IClones"))
	{
	maxIClones = Value;
	}
else if(!strcasecmp(varName,"IClonesGline"))
	{
	IClonesGline = (Value == 1);
	}
else if(!strcasecmp(varName,"GTime"))
	{
	maxGlineLen = Value;
	}
else if(!strcasecmp(varName,"SGline"))
	{
	saveGlines = (Value == 1);
	}
else if(!strcasecmp(varName,"CClonesGTime"))
	{
	CClonesGTime = Value;
	}

if(!dbConnected)
        {   
        return false;
        }
stringstream theQuery;   
theQuery        << "SELECT * FROM misc WHERE VarName = '"
		<< varName << "'"
                << ends;

#ifdef LOG_SQL
elog    << "ccotrol::updateMisc> "
        << theQuery.str().c_str()
        << endl; 
#endif

if( !SQLDb->Exec( theQuery, true ) )
//if (PGRES_TUPLES_OK != status)
        {
        elog << "Error update misc table : " << SQLDb->ErrorMessage() << endl;
	return false;
        }
if(SQLDb->Tuples() == 0)
	{
	stringstream insertQ;
	insertQ << "INSERT INTO misc (VarName,Value1) VALUES ('"
		<< varName << "',"
		<< Value <<")"
		<< ends;

	if( !SQLDb->Exec( insertQ ) )
//	if (PGRES_COMMAND_OK != status)
    		{
		return false;
	        }
	}
else
	{
	stringstream updateQ;
	updateQ << "UPDATE misc SET Value1 = "
		<< Value 
		<< " WHERE VarName = '"	
		<< varName << "'"
		<< ends;

	if( !SQLDb->Exec( updateQ ) )
//	if (PGRES_COMMAND_OK != status)
    		{
		elog << "Error update misc table : " << SQLDb->ErrorMessage() << endl;
		return false;
	        }
	}
return true;
}

unsigned int ccontrol::checkPassword(string NewPass , ccUser* tmpUser)
{
if(NewPass.size() < password::MIN_SIZE)
	{
	return password::TOO_SHORT;
	}
if(!strcasecmp(NewPass,tmpUser->getUserName()))
	{
	return password::LIKE_UNAME;
	}
return password::PASS_OK;
}

ccServer* ccontrol::getServer(const string& Name)
{

serversMapType::iterator serverIt;
serverIt = serversMap.find(Name);
if(serverIt != serversMap.end())
	{
	return serverIt->second;
	}
return NULL;

}


void ccontrol::addServer(ccServer* tempServer)
{
if(!serversMap[tempServer->getName()])
	{
	serversMap[tempServer->getName()] = tempServer;
	}
}

void ccontrol::remServer(ccServer* tempServer)
{
serversMap.erase(serversMap.find(tempServer->getName()));
}

#ifdef LOGTOHD
void ccontrol::initLogs()
{
//TODO: Get this from the conf file
LogFileName = "CommandsLog.Log";
//TODO: Get this from the conf file
LogsToSave = 100;
NumOfLogs = 0;
/* disable buffering */
LogFile.rdbuf()->pubsetbuf(0,0);
/* open file for writing and append */
LogFile.open(LogFileName.c_str(),ios::out|ios::app);

if(LogFile.bad())
	{//There was a problem in opening the log file
	elog << "Error while initilizing the logs file!\n";
	return ;
	} else {
	LogFile	<< "** GNUWorld mod.ccontrol restarted **\n";
	}
//LogFile.setbuf(NULL,0);
LogFile.close();
}

void ccontrol::addLog(ccLog* newLog)
{
ccLog* oldLog;
while(LogList.size() >= LogsToSave)
	{
	oldLog = LogList.back();
	LogList.pop_back();
	delete oldLog;
	}
LogList.push_front(newLog);
}

/*
 ccontrol::showLogs - sends the lastcom log to a client
 loading the logs from the file is done using the 
 lazy evaluation tactic, meaning it only loads the data from
 the hardisk if it has to , thous saving time

*/
void ccontrol::showLogs(iClient* theClient, unsigned int Amount)
{
if(Amount > LogsToSave)
	{
	Notice(theClient,"Sorry, but you can't view more than the last %d commands",
		LogsToSave);
	Amount = LogsToSave;
	}

if((LogList.size() < Amount) 
	&& ((NumOfLogs > LogList.size()) || (NumOfLogs == 0)))
	{
	if((!LogFile.eof() && LogFile.bad()) || !(LogFile.is_open()))
		{
		LogFile.close();
		LogFile.open(LogFileName.c_str(),ios::in|ios::out);
		//LogFile.setbuf(NULL,0);
		if(LogFile.bad())
			{
			Notice(theClient,"Error while reading the lastcom report");
			MsgChanLog("Error while reading from the lastcom file!\n");
			return;
			}
		}
	//Clean the list first
	for(ccLogIterator ptr= LogList.begin(); ptr != LogList.end();)
		{
		delete *ptr;
		ptr = LogList.erase(ptr);
		}
	/*
	    since every record has its own size, there is no way of knowing
	    where in the file the last X records are, so we start reading
	    from the begining of the file, this saves hd space but may 
	    cost some time.
	    
	    this is done only once if at all per restart, so its
	    not a big deal *g*
	*/
	ccLog* tmpLog = 0 ;
	LogFile.seekg(0,ios::beg);
	NumOfLogs = 0;
	while(!LogFile.eof())
		{
		tmpLog = new (std::nothrow) ccLog();
		if(!tmpLog->Load(LogFile))
			{
			if(!LogFile.eof())
				{
				Notice(theClient,"Error while reading the lastcom report");
				delete tmpLog ;
				return;
				}
			}
		else
			{
			++NumOfLogs;
			addLog(tmpLog); 
			}
		}
	LogFile.close();
//	LogFile.open(LogFileName.c_str(),ios::in|ios::out);
//	LogFile.setbuf(NULL,0);
	delete tmpLog;
	}
//At this point, we should have the log list full of the last LogsToSave
//commands, and we need to show only Amount of them
unsigned int Left = LogList.size();
if(Left == 0)
	return;
ccLogIterator curLog = LogList.end();
while(Left > Amount)
	{
	--curLog;
	Left--;
	}
if(curLog == LogList.end())
	{
	--curLog;
	}
while(Left > 0)
	{
	Notice(theClient,"[%s] - [(%s) - %s] - %s",
		convertToAscTime((*curLog)->Time),
		(*curLog)->User.c_str(),
		(*curLog)->Host.c_str(),
		(*curLog)->Desc.c_str());
	--curLog;
	Left--;
	}

}

#endif

void ccontrol::OnSignal(int sig)
{ 
if(sig == SIGUSR1)
	saveServersInfo();
else
if(sig == SIGUSR2)
	saveChannelsInfo();

xClient::OnSignal(sig);
}

void ccontrol::saveServersInfo()
{
ofstream servFile("ServerList.txt",ios::out);
if(!servFile)
	{
	elog << "Error while opening server list file!" << endl;
	return;
	}
gnuworld::xNetwork::const_serverIterator sIterator =
	Network->servers_begin();
iServer* curServer;
for(;sIterator != Network->servers_end();++sIterator)
	{
	curServer = sIterator->second;
	if(!curServer)
		continue;
	servFile << curServer->getName().c_str()
		 << " " << curServer->getConnectTime()
		 << " " << Network->countClients(curServer)
		 << endl;
	}
servFile.close();
}

void ccontrol::saveChannelsInfo()
{
ofstream chanFile("ChannelsList.txt",ios::out);
if(!chanFile)
	{
	elog << "Error while opening server list file!\n";
	return;
	}
gnuworld::xNetwork::const_channelIterator cIterator =
	Network->channels_begin();
Channel* curChannel;
for(;cIterator != Network->channels_end();++cIterator)
	{
	curChannel = cIterator->second;
	if(!curChannel)
		continue;
	chanFile << curChannel->getName()
		 << " " << curChannel->getCreationTime()
		 << " " << curChannel->getModeString()
		 << " " << curChannel->size()
#ifdef TOPIC_TRACK
		 << " " << curChannel->getTopic()
#endif
		 << endl;
	}
chanFile.close();
}

void ccontrol::checkMaxUsers()
{
if(maxUsers < curUsers)
	{
	maxUsers = curUsers;
	dateMax = ::time(0);
	
	static const char *UPMain = "UPDATE Misc SET Value1 = ";

	stringstream DelQuery;
	DelQuery	<< UPMain
			<< maxUsers
			<< ", Value2 = " 
			<< dateMax
			<< " WHERE VarName = 'MaxUsers'"
			<< ends;
#ifdef LOG_SQL
	elog	<< "ccontrol::checkMaxUsers> "
		<< DelQuery.str().c_str()
		<< endl; 
#endif
	if( !SQLDb->Exec( DelQuery ) )
//	if( PGRES_COMMAND_OK != status )
		{
		elog	<< "ccontrol::checkMaxUsers> SQL Failure: "
			<< SQLDb->ErrorMessage()
			<< endl ;
		}

	}
}

bool ccontrol::addVersion(const string& newVer)
{
static const char* verChar = "INSERT INTO misc (VarName,Value5) VALUES ('Version','";
stringstream VerQuery;
VerQuery	<< verChar
		<< newVer
		<< "')" 
		<< ends;
#ifdef LOG_SQL
elog		<< "ccontrol::addVersion> "
		<< VerQuery.str().c_str()
		<< endl; 
#endif

if( !SQLDb->Exec( VerQuery ) )
//if( PGRES_COMMAND_OK != status )
	{
	elog	<< "ccontrol::addVersion> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	}

return true;
}

bool ccontrol::remVersion(const string& oldVer)
{
versionsIterator ptr = VersionsList.begin();
for(;ptr != VersionsList.end();)
	{
	if(!strcasecmp(oldVer,*ptr))
		{ //Found the version in the list
		ptr = VersionsList.erase(ptr);
		}
	else
		{
		++ptr;
		}
	}

string delS = "DELETE FROM misc WHERE VarName = 'Version' AND lower(Value5) = '" 
		+ string_lower(removeSqlChars(oldVer)) + "'";
return SQLDb->Exec(delS);
return true;
}

bool ccontrol::isValidVersion(const string& checkVer)
{
versionsIterator ptr = VersionsList.begin();
for(;ptr != VersionsList.end();++ptr)
	{
	if(!strcasecmp(checkVer,*ptr))
		{ //Found the version in the list
		return true;
		}
	}
return false;
}

void ccontrol::listVersions(iClient*)
{
}

bool ccontrol::updateCheckVer(const bool newVal)
{
checkVer = newVal;
stringstream ups;
ups 	<< "UPDATE misc SET Value1 = "
	<< (newVal ? 1 : 0)
        << " WHERE VarName = 'CheckVer'"
	<< ends;

if( !SQLDb->Exec( ups ) )
//if( PGRES_COMMAND_OK != status )
	{
	elog	<< "ccontrol::updateCheckVer> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}
return true;
}

ccBadChannel* ccontrol::isBadChannel(const string& Channel)
{
badChannelsIterator ptr = badChannelsMap.find(Channel);
if(ptr == badChannels_end())
	{
	return NULL;
	}

return ptr->second;

}

void ccontrol::addBadChannel(ccBadChannel* Channel)
{
badChannelsMap[Channel->getName()] = Channel;

}

void ccontrol::remBadChannel(ccBadChannel* Channel)
{
badChannelsMap.erase(badChannelsMap.find(Channel->getName()));
}

bool ccontrol::glineChannelUsers(iClient* theClient, Channel* theChan, const string& reason, unsigned int gLength, const string& addedBy,bool excludeChanWithOper,bool unidented)
{
ccGline *TmpGline;
iClient *TmpClient;
string curIP;
string newMsg = "IP addresses glined from " + theChan->getName() + ": ";
ccUser* tmpUser = IsAuth(theClient);
typedef map<string , int> GlineMapType;
GlineMapType glineList;
GlineMapType::iterator gptr;
list<ccGline*> neededGlines;
list<string> ipList;
int count = 0;
bool foundOper = false;
bool success = true;
bool foundException = false;
bool exceptionForce = false;
for (Channel::const_userIterator ptr = theChan->userList_begin();
	ptr != theChan->userList_end(); ++ptr)
	{
	TmpClient = ptr->second->getClient();
	if((excludeChanWithOper) && (TmpClient->isOper() 
	    || TmpClient->getMode(iClient::MODE_SERVICES))) //Need to stop glining this channel
		{
		foundOper = true;
		success = false;
		break; 
		}
	curIP = xIP(TmpClient->getIP()).GetNumericIP();
	if (getExceptions("*@" + curIP) > 0) 
		{
		foundException = true;
		Notice(theClient, "There is an exception for this user: %s!%s@%s", TmpClient->getNickName().c_str(), TmpClient->getUserName().c_str(), curIP.c_str());
		}
	if (isIpOfOper(curIP)) 
		{
		Notice(theClient, "%s opered globally before and uses the same IP this user uses: %s!%s@%s", getLastNUHOfOperFromIP(curIP).c_str(), TmpClient->getNickName().c_str(), TmpClient->getUserName().c_str(), curIP.c_str());
		foundException = true;
		}
	if ((unidented) && (TmpClient->getUserName().substr(0,1)) != "~")
		continue;
	gptr = glineList.find("*@" + curIP);
	if (gptr != glineList.end())
		{
		/* duplicate gline - continue to next channel user */
		continue;
		} else {
		glineList["*@" + curIP] = 1;
		}
	if ((!TmpClient->getMode(iClient::MODE_SERVICES)) &&
		!(IsAuth(TmpClient)) && !(TmpClient->isOper()))
		{
		/* create a new gline and queue it */
		TmpGline = new ccGline(SQLDb);
		assert(TmpGline != NULL);
		if (unidented)
			TmpGline->setHost("~*@"  + curIP);
		else
			TmpGline->setHost("*@"  + curIP);
		TmpGline->setExpires(unsigned(gLength));
		TmpGline->setAddedBy(addedBy);
		TmpGline->setReason(reason);
		TmpGline->setAddedOn(::time(0));
		TmpGline->setLastUpdated(::time(0));
		neededGlines.push_back(TmpGline);
		if (showCGIpsInLogs)
			{
			count++;
			if (count > 1)
				newMsg += ",";
			newMsg += curIP;
			if (newMsg.size() > 300)
				{
				ipList.push_back(newMsg);
				newMsg = "IPs: ";
				count = 0;
				}
			}
		}
	}
if (showCGIpsInLogs)
	ipList.push_back(newMsg);

if (foundException)
	{
	if (isGlinedException(theChan->getName()) > 0) 
		{
		exceptionForce = true;
		if (!(excludeChanWithOper && foundOper))
			Notice(theClient,"There is an exception for at least one host on the channel. Channel G-line sent (forced)");
		}
	else if (!(excludeChanWithOper && foundOper))
		{
		Notice(theClient,"There is an exception for at least one host on the channel. Send the channel gline again to force.");
		addGlinedException(theChan->getName());
		}
	}

list<ccGline*>::iterator glinesIt = neededGlines.begin();
for(; glinesIt != neededGlines.end(); ++glinesIt)
	{
	TmpGline = *glinesIt;
	((excludeChanWithOper && foundOper) || (foundException && !exceptionForce)) ? delete TmpGline : queueGline(TmpGline);
	}
if (showCGIpsInLogs && (!excludeChanWithOper || !foundOper) && (!foundException || exceptionForce))
	{
	for (list<string>::iterator sItr = ipList.begin(); sItr != ipList.end(); sItr++)
		{
		newMsg = *sItr;
#ifndef LOGTOHD
		if(tmpUser)
			DailyLog(tmpUser,"%s",newMsg.c_str());
		else
    			DailyLog(theClient,"%s",newMsg.c_str());
#else
		ccLog* newLog = new (std::nothrow) ccLog();
		newLog->Time = ::time(0);
		newLog->Desc = newMsg.c_str();
		newLog->Host = theClient->getRealNickUserHost().c_str();
		if(tmpUser)
			newLog->User = tmpUser->getUserName().c_str();
		else
			newLog->User = "Unknown";			
		newLog->CommandName = (excludeChanWithOper) ? "FORCECHANGLINE" : "SCHANGLINE";
		DailyLog(newLog);
		}
#endif
	}

return success;
}

void ccontrol::isNowAnOper (iClient* theUser)
{
	string IP = xIP(theUser->getIP()).GetNumericIP();
	opersIPMap[IP] = theUser->getRealNickUserHost();
}

bool ccontrol::isIpOfOper (const string& IP)
{
	opersIPMapType::iterator oItr;
	if ((oItr = opersIPMap.find(IP)) != opersIPMap.end())
        return true;
	return false;
}

string ccontrol::getLastNUHOfOperFromIP (const string& IP)
{
	if (isIpOfOper(IP))
		return opersIPMap[IP];
	return string("error");
}

void ccontrol::refreshOpersIPMap()
//That function won't be needed afterall.
{
	int count=0;
	gnuworld::xNetwork::clientIterator cItr = Network->clients_begin();
	for (; cItr != Network->clients_end(); cItr++) {
		iClient* theUser = (*cItr).second;
		if (theUser->isOper()) {
			opersIPMap[xIP(theUser->getIP()).GetNumericIP()] = theUser->getRealNickUserHost();
			count++;
		}
	}
    MsgChanLog("Refreshed opers IP list. %d global opers online right now. I have cumulated %d different IPs since I started", count, opersIPMap.size());
}

void ccontrol::announce(iClient* theClient, const string& text)
{
	ccUser* tmpUser = IsAuth(theClient);

	string yyxxx( MyUplink->getCharYY() + "]]]" ) ;
	if (Network->findNick(AnnounceNick) != NULL) {
		if (tmpUser && !tmpUser->getLogs())
			Notice(theClient, "Nick %s is already in use for ANNOUNCE. Will use my own", AnnounceNick.c_str());
		MsgChanLog("Nick %s is already in use for ANNOUNCE. Will use my own", AnnounceNick.c_str());
		MyUplink->Write("%s O $* :%s", getCharYYXXX().c_str(), text.c_str());
		return;
	}

	iClient* newClient = new iClient(
		MyUplink->getIntYY(),
		yyxxx,
		AnnounceNick,
		"announce",
		"AAAAAA",
		getHostName(),
		getHostName(),
		"+ok",
		string(),
		0,
		"Announcement Service.",
		::time( 0 ) ) ;
	assert( newClient != 0 );

	if(!MyUplink->AttachClient( newClient, this ) ) {
		if (tmpUser && !tmpUser->getLogs())
			Notice(theClient, "Error attaching announce client. Using my own nick for the announce.");
		MsgChanLog("Error attaching announce client. Using my own nick for the announce.");
		MyUplink->Write("%s O $* :%s", getCharYYXXX().c_str(), text.c_str());
		delete newClient;
		return;
	}

	MyUplink->Write("%s O $* :%s", newClient->getCharYYXXX().c_str(), text.c_str());

	stringstream Quit;
	Quit << "Did what I had to do! (At " << theClient->getNickName() << "'s request)";
	if( MyUplink->DetachClient( newClient, Quit.str() ) ) {
		delete newClient;
	}
	
}

}

} // namespace gnuworld
