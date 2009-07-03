/**
 * dronescan.cc
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
 */

#include	<string>
#include	<vector>
#include	<sstream>
#include	<iostream>
#include 	<list>
#include <cstdarg>	/* va_list */
#include <cstdio>	/* *printf() */
#include "dbHandle.h"

#include "gnuworld_config.h"
#include "EConfig.h"
#include "Network.h"
#include "server.h"
#include "StringTokenizer.h"

#include "activeChannel.h"
#include "clientData.h"
#include "constants.h"
#include "dronescan.h"
#include "dronescanCommands.h"
#include "dronescanTests.h"
#include "sqlFakeClient.h"
#include "sqlUser.h"
#include "Timer.h"
#include "ip.h"

#ifdef ENABLE_LOG4CPLUS
#include <log4cplus/logger.h>
#endif

namespace gnuworld {

namespace ds {

using std::string ;
using std::endl;

/*
 * Exported function to be used by moduleLoader to gain an
 * instance of this module.
 */
extern "C"
{
	xClient* _gnuwinit(const string& args)
	{
		return new dronescan(args);
	}
}

/***************************
 ** C O N S T R U C T O R **
 ***************************/

/**
 * This constructor calls the base class constructor.  The xClient
 * constructor will open the configuration file given and retrieve
 * basic client info (nick/user/host/etc).
 * Any additional processing must be done here.
 */
dronescan::dronescan( const string& configFileName )
	: xClient( configFileName )
{
/* Load the config file */
dronescanConfig = new (std::nothrow) EConfig(configFileName);
assert(dronescanConfig != 0);

/* Do config file processing here */
consoleChannel = dronescanConfig->Require("consoleChannel")->second;

/* Set our initial state */
//elog << "dronescan.start> getUplink()->getStartTime()) = " << getUplink()->getStartTime() << endl;
int i = 0;
for (xNetwork::serverIterator Itr = Network->servers_begin(); Itr != Network->servers_end(); Itr++)
	{
	i++;
	}
elog << "droneScan.start> i = " << i << endl;
if (i == 1) //if i == 1, it means i'm not connectd to a hub. If i > 1, it means the RELOAD command was sent
	currentState = BURST;
averageEntropy = 0;
totalNicks = 0;

/* What is the voting cutoff? */
voteCutoff = atoi(dronescanConfig->Require("voteCutoff")->second.c_str());

/*
 * Set up initial margins.
 * These, particularly nickMargin,  will require tweaking to make them work well.
 * It is probably wise to not run these too close to false positive since
 * the average entropy and such will vary over time. Two decimal places is
 * probably the furthest I would push it for now.
 */
channelMargin = atof(dronescanConfig->Require("channelMargin")->second.c_str());
nickMargin = atof(dronescanConfig->Require("nickMargin")->second.c_str());
channelCutoff = atoi(dronescanConfig->Require("channelCutoff")->second.c_str());

RegisterTest(new ABNORMALSTest(this, "ABNORMALS", "Checks for the percentage of normal clients.", 10));
RegisterTest(new COMMONREALTest(this, "COMMONREAL", "Checks for common realnames", 10));
RegisterTest(new HASALLOPTest(this, "HASALLOP", "Checks if a channel has all ops.", 10));
RegisterTest(new HASOPTest(this, "HASOP", "Checks if a channel has no ops.", 10));
RegisterTest(new MAXCHANSTest(this, "MAXCHANS", "Checks the max channel membership of a channel.", 10));
RegisterTest(new RANGETest(this, "RANGE", "Checks the entropy range.", 10));

/* Set up core options */
dcInterval = atoi(dronescanConfig->Require("dcInterval")->second.c_str());
consoleLevel = atoi(dronescanConfig->Require("consoleLevel")->second.c_str());
jcInterval = atoi(dronescanConfig->Require("jcInterval")->second.c_str());
jcCutoff = atoi(dronescanConfig->Require("jcCutoff")->second.c_str());
//pcCutoff = atoi(dronescanConfig->Require("pcCutoff")->second.c_str());
ncInterval = atoi(dronescanConfig->Require("ncInterval")->second.c_str());
ncCutoff = atoi(dronescanConfig->Require("ncCutoff")->second.c_str());
rcInterval = atoi(dronescanConfig->Require("rcInterval")->second.c_str());
jcMinJoinToGline = atoi(dronescanConfig->Require("jcMinJoinToGline")->second.c_str());
jcMinJoinToGlineJOnly = atoi(dronescanConfig->Require("jcMinJoinToGlineJOnly")->second.c_str());
jcMinJoinsPerIPToGline = atoi(dronescanConfig->Require("jcMinJoinsPerIPToGline")->second.c_str());
jcJoinsPerIPTime = atoi(dronescanConfig->Require("jcJoinsPerIPTime")->second.c_str());
jcMinJFSizeToGline = atoi(dronescanConfig->Require("jcMinJFSizeToGline")->second.c_str());
jcMinJFJOnlySizeToGline = atoi(dronescanConfig->Require("jcMinJFJOnlySizeToGline")->second.c_str());
jcGlineEnable = atoi(dronescanConfig->Require("jcGlineEnable")->second.c_str()) == 1 ? true : false;
jcGlineEnableConf = jcGlineEnable;
jcGlineReason = dronescanConfig->Require("jcGlineReason")->second.c_str();
jcGlineLength = atoi(dronescanConfig->Require("jcGlineLength")->second.c_str());
gbCount = atoi(dronescanConfig->Require("gbCount")->second.c_str());
gbInterval = atoi(dronescanConfig->Require("gbInterval")->second.c_str());
/* Set up variables that our tests will need */
typedef std::vector<string> testVarsType;
testVarsType testVars;

/* Cycle over tests to get configuration variable name */
for( testMapType::const_iterator itr = testMap.begin() ;
     itr != testMap.end() ; ++itr) {
	testVars.push_back( itr->second->getVariable() );
}

for( testVarsType::const_iterator itr = testVars.begin() ;
     itr != testVars.end() ; ++itr) {
	if(*itr == "") continue;

	string theValue = dronescanConfig->Require(*itr)->second;

	if(Test *theTest = setTestVariable(string_upper(*itr), theValue)) {
		elog	<< "dronescan> Test " << theTest->getName()
			<< " accepted parameter " << *itr
			<< " @ " << theValue
			<< std::endl;
	} else {
		elog	<< "dronescan> No test accepted variable: "
			<< *itr << " @ " << theValue
			<< std::endl;
		::exit(1);
	}
}

/* Set up database */
string sqlHost(dronescanConfig->Require("sqlHost")->second);
string sqlPort(dronescanConfig->Require("sqlPort")->second);
string sqlDB(dronescanConfig->Require("sqlDB")->second);
string sqlUsername(dronescanConfig->Require("sqlUser")->second);
string sqlPass(dronescanConfig->Require("sqlPass")->second);

std::stringstream connectString;
connectString	<< "host=" << sqlHost << " "
		<< "port=" << sqlPort << " "
		<< "dbname=" << sqlDB << " "
		<< "user=" << sqlUsername << " "
		<< "password=" << sqlPass
		;
elog << "dronescan::dronescan> Connecting to SQL Server." << std::endl;

//SQLDb = new PgDatabase(connectString.str().c_str());
SQLDb = new dbHandle( sqlHost,
	atoi( sqlPort ),
	sqlDB,
	sqlUsername,
	sqlPass ) ;
if(SQLDb->ConnectionBad()) {
	elog	<< "dronescan> Failed to connect to SQL server: "
		<< SQLDb->ErrorMessage()
		<< std::endl;
	::exit(0);
}

elog << "dronescan> Established connection to SQL server." << std::endl;

/* Preload caches */
preloadFakeClientCache();
preloadUserCache();
if (preloadExceptionalChannels() == false) {
	//If exceptional channels can't be loaded, we don't want glines to be turned on
	jcGlineEnable = false;
}

/* Initialise statistics */
customDataCounter = 0;

/* Setup fake oper record if necessary */
unsigned short fakeOperLevel = atoi(dronescanConfig->Require("fakeOperLevel")->second.c_str());
if( fakeOperLevel > 0 ) {
	/* Create a fake record for opers to use */
	fakeOperUser = new sqlUser(0);
	fakeOperUser->setAccess(fakeOperLevel);
	fakeOperUser->setFlags(0);
	fakeOperUser->setLastUpdated(::time(0));
	fakeOperUser->setLastUpdatedBy("Internal");
} else {
	fakeOperUser = 0;
}

/* Set up our timer. */
theTimer = new Timer();

/* Register commands available to users */
RegisterCommand(new ACCESSCommand(this, "ACCESS", "() (<user>)"));
RegisterCommand(new ADDUSERCommand(this, "ADDUSER", "<user> <access>"));
RegisterCommand(new ADDEXCEPTIONALCHANNELCommand(this, "ADDEXCEPTIONALCHANNEL","<channel name>"));
RegisterCommand(new ANALYSECommand(this, "ANALYSE", "<#channel>"));
RegisterCommand(new CHECKCommand(this, "CHECK", "(<#channel>) (<user>)"));
RegisterCommand(new FAKECommand(this, "FAKE", "(activate)"));
RegisterCommand(new LISTCommand(this, "LIST", "(active|fakeclients|joinflood|users)"));
RegisterCommand(new MODUSERCommand(this, "MODUSER", "(ACCESS <user> <level>"));
RegisterCommand(new QUOTECommand(this, "QUOTE", "<string>"));
RegisterCommand(new REMEXCEPTIONALCHANNELCommand(this, "REMEXCEPTIONALCHANNEL","<channel name>"));
RegisterCommand(new REMUSERCommand(this, "REMUSER", "<user>"));
RegisterCommand(new STATUSCommand(this, "STATUS", ""));
RegisterCommand(new RELOADCommand(this, "RELOAD", ""));

// in case RELOAD command is used, have to iterate the userlist to know how many clients per IP are online for the gline count.
clientsIPMap.clear();
for( xNetwork::const_clientIterator cItr = Network->clients_begin() ; cItr != Network->clients_end() ; ++cItr )
	{
	string IP = xIP(cItr->second->getIP()).GetNumericIP();
	/* Store usercount per IPs to a map */
	if (clientsIPMap.find(IP) == clientsIPMap.end())
		clientsIPMap.insert(std::make_pair(IP, 1));
	else
		clientsIPMap[IP]++;
	}

} // dronescan::dronescan(const string&)


/*************************
 ** D E S T R U C T O R **
 *************************/
dronescan::~dronescan()
{
}


/*****************************************************
 ** X C L I E N T   M E M B E R   F U N C T I O N S **
 *****************************************************/

/**
 * Here we run whatever needs doing as soon as we have attached to
 * the xServer. We are not attached to the network at this point.
 */
void dronescan::OnAttach()
{
	/* Register for global network events */
	MyUplink->RegisterEvent( EVT_BURST_CMPLT, this );
	MyUplink->RegisterEvent( EVT_NETJOIN, this );
	MyUplink->RegisterEvent( EVT_NETBREAK, this );
	MyUplink->RegisterEvent( EVT_NICK, this );
	MyUplink->RegisterEvent( EVT_KILL, this );
	MyUplink->RegisterEvent( EVT_QUIT, this );

	/* Register for all channel events */
	MyUplink->RegisterChannelEvent( xServer::CHANNEL_ALL, this );

	/* Set up clearing active channels */
	time_t theTime = time(0) + dcInterval;
	tidClearActiveList = MyUplink->RegisterTimer(theTime, this, 0);

	/* Set up our JC counter */
	theTime = time(0) + jcInterval;
	tidClearJoinCounter = MyUplink->RegisterTimer(theTime, this, 0);

	/* Set up cache refresh timer */
	theTime = time(0) + rcInterval;
	tidRefreshCaches = MyUplink->RegisterTimer(theTime, this, 0);

	theTime = time(0) + ncInterval;
	tidClearNickCounter = MyUplink->RegisterTimer(theTime, this, 0);
	
	theTime = time(0) + gbInterval;
	tidGlineQueue = MyUplink->RegisterTimer(theTime,this,0);

	xClient::OnAttach() ;
} // dronescan::OnAttach()

/**
 * Here we decide what channels our xClient needs to burst in to.
 * The only channel of any interest to us is our console channel, which is
 * loaded from the configuration file.
 */
void dronescan::BurstChannels()
{
	MyUplink->JoinChannel(this, consoleChannel,
		dronescanConfig->Require("consoleChannelModes")->second);

	/* We need to assign a clientData to all xClients on this xServer */
	xNetwork::const_localClientIterator itr = Network->localClient_begin();
	for( ; itr != Network->localClient_end(); ++itr )
		{
		xClient *theXClient = itr->second;
		iClient *theClient = theXClient->getInstance();

		clientData *newData = new clientData();
		newData->setState(NORMAL);

		theClient->setCustomData(this, newData);
		customDataCounter++;
		}

	/* Set the topic */
	setConsoleTopic();

	return xClient::BurstChannels();
} // dronescan::BurstChannels()

void dronescan::OnCTCP( iClient* theClient, const string& CTCP,
    const string& Message, bool Secure )
{
	StringTokenizer st(CTCP);

	if(st.empty()) return ;

	string Command = string_upper(st[0]);

	if("DCC" == Command) {
		DoCTCP(theClient, CTCP, "REJECT");
	} else if("PING" == Command) {
		DoCTCP(theClient, CTCP, Message);
	} else if("VERSION" == Command) {
		DoCTCP(theClient, CTCP, "GNUWorld DroneScan v0.0.9.2");
	}

	xClient::OnCTCP(theClient, CTCP, Message, Secure);
}

/**
 * Here we receive network events that we are registered for.
 */
void dronescan::OnEvent( const eventType& theEvent,
	void *Data1, void *Data2, void *Data3, void *Data4)
{
	switch( theEvent )
		{
		case EVT_BURST_CMPLT :
			{
			log(DBG, "Caught EOB. Resetting frequencies.");
			updateState();
			break;
			} // EVT_BURST_CMPLT
		case EVT_NETJOIN :
			{
			changeState(BURST);
			break;
			}
		case EVT_NETBREAK :
			{
			updateState();
			break;
			}
		case EVT_NICK :
			{
			handleNewClient( static_cast< iClient* >( Data1 ) );
			break;
			} // EVT_NICK
		case EVT_CHNICK:
			{
			handleNickChange( static_cast< iClient* >( Data1 ) );
			}

		case EVT_KILL : /* Intentional drop through */
		case EVT_QUIT :
			{
			iClient *theClient = static_cast< iClient* >
				( theEvent == EVT_KILL ?
					Data2 :
					Data1 );

			/* Store usercount per IPs to a map */
			string IP = xIP(theClient->getIP()).GetNumericIP();
			if (clientsIPMap[IP] <= 1)
				clientsIPMap.erase(IP);
			else
				clientsIPMap[IP]--;

			clientData *theData = static_cast< clientData* >
				( theClient->removeCustomData(this) );

			delete(theData);
			--customDataCounter;
			break;
			}
		} // switch( theEvent )

	xClient::OnEvent( theEvent, Data1, Data2, Data3, Data4);
}

/** Receive channel events. */
void dronescan::OnChannelEvent( const channelEventType& theEvent,
	Channel *theChannel, void *Data1, void *Data2, void *Data3, void *Data4 )
{
	/* If we are bursting, we don't want to be checking joins. */
	if(currentState == BURST) return ;

	/* If this is not a join and not part, we don't care. */
	if(theEvent != EVT_JOIN && theEvent != EVT_PART) return ;

	iClient* theClient = static_cast< iClient* > (Data1);
	if(theEvent == EVT_JOIN)
		{
		handleChannelJoin(theChannel,theClient);
		}
	else if (theEvent == EVT_PART)
		{
		handleChannelPart(theChannel,theClient);
		}
xClient::OnChannelEvent( theEvent, theChannel,
	Data1, Data2, Data3, Data4 ) ;
}

/**
 * Here we receive private messages from iClients.
 */
void dronescan::OnPrivateMessage( iClient* theClient,
	const string& Message, bool )
{
	sqlUser *theUser = getSqlUser(theClient->getAccount());

	/* If the client is opered, we might have a fake account */
	if( !theUser && theClient->isOper() && fakeOperUser ) {
		theUser = fakeOperUser;

		fakeOperUser->setUserName(theClient->getNickName());
		fakeOperUser->setCreated(::time(0));
		fakeOperUser->setLastSeen(::time(0));
	}

	if(!theUser) return ;
	if (!theClient->isOper())
		return;

	/* We have now seen this user! */
	theUser->setLastSeen(::time(0));
	theUser->commit();

	/* If we are currently in BURST, we don't accept commands */
	/*if(BURST == currentState) {
		Reply(theClient, "Sorry, I do not accept commands during a burst.");
		return ;
	}*/

	StringTokenizer st(Message);

	if(st.size() < 1) return ;

	string Command = string_upper(st[0]);
	commandMapType::iterator commandHandler = commandMap.find(Command);

	if(commandHandler != commandMap.end())
		{
		commandHandler->second->Exec(theClient, Message, theUser);
		return ;
		}

	if("INVITE" == Command)
		{
		Invite(theClient, consoleChannel);
		return ;
		}

/* This is commented out because it doesn't work at the moment */
/*	if("RELOAD" == Command)
		{
		getUplink()->UnloadClient(this, "Reloading...");
		getUplink()->LoadClient("libdronescan.la", getConfigFileName());
		return ;
		}
*/
	if("STATS" == Command)
		{
		Reply(theClient, "Allocated custom data: %d", customDataCounter);

		Reply(theClient, "CM/NM/CC: %0.2lf/%0.2lf/%u",
			channelMargin,
			nickMargin,
			channelCutoff
			);

		Reply(theClient, "jcI/jcC : %u/%u",
			jcInterval,
			jcCutoff
			);
		Reply(theClient, "ncI/ncC : %u/%u",
			ncInterval,
			ncCutoff
			);
		return ;
		}

	if("RESET" == Command)
		{
		resetAndCheck();
		return ;
		}

	if(st.size() < 2) return ;

	if("INFO" == Command)
		{
		string nick = st[1];
		iClient *targetClient = Network->findNick(nick);
		if(!targetClient) return ;
		Reply(theClient, "Status of %s: %s",
			targetClient->getNickName().c_str(),
			(isNormal(targetClient) ? "Normal" : "Abnormal" )
			);
		return ;
		}

	if(st.size() < 3) return ;

	if("SET" == Command)
		{
		string Option = string_upper(st[1]);

		//Glines on/off
		if("GL" == Option) {
			if ("ON" == string_upper(st[2])) {
				if (jcGlineEnable) {
					Reply(theClient, "I am already setting glines - no change");
					return ;
				}
				else {
					if (jcGlineEnableConf) {
						jcGlineEnable = true;
						Reply(theClient, "Glines activated");
						log(INFO, "%s is activating auto-glines", theClient->getNickName().c_str());
					}
					else {
						Reply(theClient, "You have to enable auto-glines in the conf first");
					}
					return ;
				}
			}
			else if ("OFF" == string_upper(st[2])) {
				if (!jcGlineEnable) {
					Reply(theClient, "I'm not setting glines - no change");
					return ;
				}
				else {
					jcGlineEnable = false;
					Reply(theClient, "Glines deactivated");
					log(INFO, "%s is deactivating auto-glines", theClient->getNickName().c_str());
					return ;
				}
			}
			else {
				Reply(theClient, "Syntax: set GL <ON|OFF>");
				return ;
			}
		}

			/* Global entropy options */
		if("CC" == Option)
			{
			unsigned int newCC = atoi(st[2].c_str());
			channelCutoff = newCC;
			resetAndCheck();
			return ;
			}
		if("CM" == Option)
			{
			double newCM = atof(st[2].c_str());
			if(newCM < 0 || newCM > 1) return ;
			channelMargin = newCM;
			resetAndCheck();
			return ;
			}
		if("NM" == Option)
			{
			double newNM = atof(st[2].c_str());
			if(newNM < 0 || newNM > 1) return ;
			nickMargin = newNM;
			resetAndCheck();
			return ;
			}

		/* None of the hardcoded options have hit. Try dynamic. */
		if(Test *theTest = setTestVariable(Option, st[2])) {
			log(INFO, "%s set %s to %s in %s",
				theClient->getNickName().c_str(),
				Option.c_str(),
				st[2].c_str(),
				theTest->getName().c_str()
				);

			resetAndCheck();
			return ;
		} else  {
			Reply(theClient, "No test accepted the variable %s",
				Option.c_str()
				);
			return ;
		}
		}
}


/** Clean up after ourselves */
void dronescan::OnDetach()
{
/* We need to delete() anything we have new()d
 * Currently this is:
 *  dronescanConfig
 *  Tests
 *  Commands
 *  clientData for every client on the net
 *  theTimer
 *
 * It is important this is kept up to date so that reload() does not leak.
 * We also need to unregister any timers we have so GNUworld doesn't segfault :)
 */

/* Delete our config */
delete dronescanConfig;

/* Delete our timer */
delete theTimer;

/* Delete commands */
for(commandMapType::iterator itr = commandMap.begin() ;
    itr != commandMap.end() ; ++itr) {
	delete itr->second;
}
commandMap.clear();

/* Delete tests */
for(testMapType::iterator itr = testMap.begin() ;
    itr != testMap.end() ; ++itr) {
	delete itr->second;
}
testMap.clear();

/* Iterate over clients to delete clientData */
for(xNetwork::const_clientIterator ptr = Network->clients_begin() ;
    ptr != Network->clients_end() ; ++ptr) {
	iClient *theClient = ptr->second;
	clientData *theData = static_cast< clientData* > (theClient->removeCustomData(this));
	delete theData;
}

/* Unregister the join counting timer */
if(!MyUplink->UnRegisterTimer(tidClearJoinCounter, 0) ||
   !MyUplink->UnRegisterTimer(tidClearNickCounter, 0) ||
   !MyUplink->UnRegisterTimer(tidClearActiveList, 0)) {
	elog	<< "dronescan::~dronescan> "
		<< "Could not unregister timer. Expect problems shortly."
		<< std::endl;
}

/* Done! */

}



/** Receive our own timed events. */
void dronescan::OnTimer( const xServer::timerID& theTimer , void *)
{
	time_t theTime;

	if(theTimer == tidClearActiveList)
		{
		droneChannelsType::iterator dcitr, next_dcitr;
		time_t joinSince = ::time(0) - dcInterval;

		for( dcitr = droneChannels.begin() ;
		     dcitr != droneChannels.end() ; dcitr = next_dcitr ) {
		 	next_dcitr = dcitr;
			++next_dcitr;

			if( dcitr->second->getLastJoin() <= joinSince ) {
				log(DBG, "Removing %s.",
					dcitr->first.c_str()
					);

				delete dcitr->second;
				droneChannels.erase(dcitr);
			}
		}

		theTime = time(0) + dcInterval;
		tidClearActiveList = MyUplink->RegisterTimer(theTime, this, 0);
		}

	if(theTimer == tidClearJoinCounter)
		{
		processJoinPartChannels();
		}

	if(theTimer == tidClearNickCounter)
		{
		for(ncChanMapType::const_iterator itr = ncChanMap.begin() ;
		    itr != ncChanMap.end() ; ++itr) {
			Channel* theChan = Network->findChannel(
				itr->first ) ;
			if( 0 == theChan )
				{
				elog	<< "dronescan::OnTimer> Unable "
					<< "to find channel: "
					<< itr->first
					<< std::endl ;
				continue ;
				}

			if(itr->second >= ncCutoff)
				log(WARN, "Nick flood over in %s. Total joins: %u. Total size: %d",
					itr->first.c_str(),
					itr->second,
					theChan->size()
					);
		}

		log(DBG, "Clearing %u records from the join counter.",
			ncChanMap.size()
			);
		ncChanMap.clear();

		theTime = time(0) + ncInterval;
		tidClearNickCounter = MyUplink->RegisterTimer(theTime, this, 0);
		}

	if(theTimer == tidRefreshCaches) {
		log(DBG, "Refreshing caches");

		preloadUserCache();
		preloadExceptionalChannels();
		
		theTime = time(0) + rcInterval;
		tidRefreshCaches = MyUplink->RegisterTimer(theTime, this, 0);
	}
	
	if(theTimer == tidGlineQueue) {
		processGlineQueue();
		
		theTime = time(0) + gbInterval;
		tidGlineQueue = MyUplink->RegisterTimer(theTime, this , 0);
	}
}		

/*******************************************
 ** D R O N E S C A N   F U N C T I O N S **
 *******************************************/


void dronescan::processJoinPartChannels()
{
	static time_t jcFCInterval = ::time(0);
for(jcChanMapType::const_iterator itr = jcChanMap.begin() ;
    itr != jcChanMap.end() ; ++itr) 
	{
	Channel* theChan = Network->findChannel(
		itr->first ) ;
	if( 0 == theChan )
		{
		elog	<< "dronescan::OnTimer> Unable "
			<< "to find channel: "
			<< itr->first
			<< std::endl ;
			/* The join/flood is now reported/glined even if the channel is empty.
			   We now have to make sure theChan != 0 below.
			*/
		}
	jfChannel* jChannel = itr->second;
			
	if(jChannel->getNumOfJoins() >= jcCutoff) 
		{
		log(WARN, "Join flood over in %s. Total joins: %u. Total parts: %u. Total size: %d",
			itr->first.c_str(),
			jChannel->getNumOfJoins(),
			jChannel->getNumOfParts(),
			(theChan == 0 ? 0 : theChan->size())
			);
		std::list<std::pair<glineData*,std::list<string> > > glined;
		jfChannel::joinPartMapIterator joinPartIt = jChannel->joinPartBegin();
		jfChannel::joinPartMapIterator joinPartEnd = jChannel->joinPartEnd();
		std::stringstream names;
		std::stringstream excluded; 
		std::stringstream tempNames;
		int clientcount = 0;

		bool isoktogline = false;
		bool isoktogline2 = false;
		for(;joinPartIt != joinPartEnd; ++joinPartIt )
			{
				isoktogline = ((::time(0) - lastBurstTime) > 25 && jcGlineEnable && jChannel->getNumOfJoins() > jcMinJFSizeToGline && (jChannel->getNumOfParts() > jcMinJFSizeToGline || (joinPartIt->second.numOfJoins >= jcMinJoinToGlineJOnly && jChannel->getNumOfJoins() >= jcMinJFJOnlySizeToGline))) ? true : false;
				if (isoktogline)
					isoktogline2 = true;
				int numOfUsernames = 0;
				if((joinPartIt->second.numOfJoins >= jcMinJoinToGline &&
			    joinPartIt->second.numOfParts >= jcMinJoinToGline) || (joinPartIt->second.numOfJoins >= jcMinJoinToGlineJOnly && jChannel->getNumOfJoins() >= jcMinJFJOnlySizeToGline))
				{
				if(!joinPartIt->second.seenOper && !joinPartIt->second.seenLoggedInUser)
					{
					std::list<string>::const_iterator numericsIt = joinPartIt->second.numerics.begin();
					tempNames << joinPartIt->first.c_str() << std::string("[") 
					<< joinPartIt->second.numOfJoins 
					<< std::string("]<");
					std::list<std::string> clients;
					for(;numericsIt != joinPartIt->second.numerics.end();++numericsIt)
						{
						iClient* theClient = Network->findClient(*numericsIt);
						if(theClient && !strcmp(xIP(theClient->getIP()).GetNumericIP().c_str()
							,joinPartIt->first.c_str()))
							{
							tempNames 	<< theClient->getNickName() 
								<< ",";
							if (isoktogline == true)
								{
								clients.push_back(theClient->getNickName() + "!" + theClient->getUserName() +"@"
									+ xIP(theClient->getIP()).GetNumericIP() + " " + theClient->getDescription());
									
								}
							}
						}
						tempNames << ">";
						if(names.str().size() + tempNames.str().size() > 400)
								{
								outputNames(itr->first,names,false,isoktogline);
								}
						names << " " << tempNames.str();
						tempNames.str("");
						if (isoktogline == true)
								{
								glineData* theGline = new (std::nothrow) glineData("*@" +joinPartIt->first,jcGlineReason,jcGlineLength);
								assert(theGline != 0);
								glined.push_back(std::pair<glineData*,std::list<std::string> >(theGline,clients));
								clientcount += clients.size();
								}
					} else  {
					excluded << joinPartIt->first.c_str()
						 << "[";
					if(joinPartIt->second.seenOper)
						{
						excluded << "O";
						} 
					if(joinPartIt->second.seenLoggedInUser)
						{
						excluded << "L";
						}
					excluded << "],";
					if(excluded.str().size() > 400)
						{
						outputNames(itr->first,excluded,true,isoktogline);
						}	  

#ifdef ENABLE_LOG4CPLUS
					std::list<std::pair<std::string,std::string > >::const_iterator userNamesIt = joinPartIt->second.userNames.begin();
					for(;userNamesIt != joinPartIt->second.userNames.end(); ++ userNamesIt)
						{
						iClient* theClient = Network->findClient(userNamesIt->first);
						if(theClient && !strcmp(xIP(theClient->getIP()).GetNumericIP().c_str()
						,joinPartIt->first.c_str()))
							{
							if(numOfUsernames++ == 0)
								{
								log(JF_CSERVICE, "Join flood over in %s. Total joins: %u. Total parts: %u. Total size: %d",
								itr->first.c_str(),
								jChannel->getNumOfJoins(),
								jChannel->getNumOfParts(),
								(theChan == 0 ? 0 : theChan->size())
								);
								}
								/*log(JF_CSERVICE,"%s %s!%s@%s %s",userNamesIt->second.c_str(),
									theClient->getNickName().c_str(),
									theClient->getUserName().c_str(),
									xIP(theClient->getIP()).GetNumericIP().c_str(),
									theClient->getDescription().c_str());*/
								log(JF_CSERVICE,"(%s) %s!%s@%s.users.undernet.org %s",xIP(theClient->getIP()).GetNumericIP().c_str(),
									theClient->getNickName().c_str(),
									theClient->getUserName().c_str(),
									userNamesIt->second.c_str(),
									theClient->getDescription().c_str());
						
							}
						}
#endif
					}	 
				}
			}

		isoktogline = isoktogline2;

		if(names.str().size() > 0)
			{
			outputNames(itr->first,names,false,isoktogline);
			}
		if(excluded.str().size() > 0)
			{
			outputNames(itr->first,excluded,true,isoktogline);
			}
		if (isoktogline == true)
			{ 
			if ((glined.size() < 3) && (clientcount < 8))
				{
					if (glined.size() != 0) {
					log(WARN,"Aborting glines for channel %s because only %d flooding clients from %d addresses were found",
						itr->first.c_str(),clientcount,glined.size());
					}
				}  
#ifdef ENABLE_LOG4CPLUS
				else {
				log(JF_GLINED,"Join flood over in %s. Total joins: %u. Total parts: %u. Total size: %d. Total addresses glined %d.",
				itr->first.c_str(),
				jChannel->getNumOfJoins(),
				jChannel->getNumOfParts(),
				(theChan == 0 ? 0 : theChan->size()),
				glined.size());
				}
#endif
			}
		std::list<std::pair<glineData*,std::list<string> > >::iterator glinesIt = glined.begin();
		for(; glinesIt != glined.end();++glinesIt)
			{
			glineData* curGline = glinesIt->first;
			if ((glined.size() >= 3) || (clientcount >= 8))
				{
				/*
				string IP = xIP(theClient->getIP()).GetNumericIP();
				clientsIPFloodMapType::iterator tmpItr = clientsIPFloodMap.find(IP);
				if (tmpItr != clientsIPFloodMap.end()) {
					jcFloodClients* aptr = tmpItr->second;
					clientsIPFloodMap.erase(tmpItr);
					delete aptr;
				}
				IPJQueueType::iterator tmpItr2 = IPJQueue.begin();
				for (; tmpItr2 != IPJQueue.end(); tmpItr2++) {
					if (*tmpItr2 == IP) {
						IPJQueue.erase(tmpItr2);
						break;
					}
				}
				*/
				std::list<string>::iterator clientsIt = glinesIt->second.begin();
#ifdef ENABLE_LOG4CPLUS
				for(; clientsIt != glinesIt->second.end(); ++ clientsIt)
					{
					log(JF_GLINED,(*clientsIt).c_str());
					}
#endif
				//Checking if the gline is already present in the queue
				glineQueueType::iterator git = glineQueue.begin();
				bool found_gline_in_queue = false;
				for (; git != glineQueue.end(); ++git) {
					if (strcasecmp((*git)->getHost().c_str(),curGline->getHost().c_str()) == 0)
						found_gline_in_queue = true;
				}
				//skipping findGline() check, cpu expensive.
				//if ((MyUplink->findGline(curGline->getHost()) == 0) && (found_gline_in_queue == false)) {
				if (found_gline_in_queue == false) {
					glineQueue.push_back(curGline);
				}
				} 
			else {
				delete curGline;
				}
			}

		if ((glined.size() >= 3) || (clientcount >= 8))
			log(WARN, "Glining %d floodbots from %d different ips", clientcount, glined.size());
		}
	
	delete itr->second;
				
	}


	IPJQueueType::iterator Itr2 = IPJQueue.begin();
	std::stringstream tempNames;
	int clientcount = 0;
	for (; Itr2 != IPJQueue.end(); Itr2++) {
		glineQueueType::iterator git = glineQueue.begin();
		bool found_gline_in_queue = false;
		for (; git != glineQueue.end(); ++git) {
			std::string gmask = "*@" + *Itr2;
			if (strcasecmp((*git)->getHost().c_str(),gmask.c_str()) == 0)
				found_gline_in_queue = true;
		}
		if (found_gline_in_queue == true) {
			continue;
		}
#ifdef ENABLE_LOG4CPLUS
		if (clientcount == 0)
			log(JF_GLINED,"--- Multiple join floods without parts ---");
#endif
		clientcount++;
		if (tempNames.str().size() > 380) {
			std::stringstream s;
			s << "Glining the following clients:" << tempNames.str();
			log(WARN, "%s", s.str().c_str());
			tempNames.str("");
		}
		tempNames << " ";
		jcFloodClients* jcFC = clientsIPFloodMap[*Itr2];
		std::list<string>::const_iterator Itr3 = jcFC->nicks.begin();
		tempNames << *Itr2 << std::string("[") 
		<< jcFC->count 
		<< std::string("]<");
		for (; Itr3 != jcFC->nicks.end(); Itr3++) {
			tempNames << *Itr3 << ",";
		}
		tempNames << ">(";
		Itr3 = jcFC->chans.begin();
		for (; Itr3 != jcFC->chans.end(); Itr3++) {
			tempNames << *Itr3 << ",";
		}
		tempNames << ")";

		glineData* theGline = new (std::nothrow) glineData("*@" +*Itr2,jcGlineReason,jcGlineLength);
		assert(theGline != 0);
		glineQueue.push_back(theGline);
#ifdef ENABLE_LOG4CPLUS
		Itr3 = jcFC->log.begin();
		for (; Itr3 != jcFC->log.end(); Itr3++) {
			log(JF_GLINED,"%s",(*Itr3).c_str());
		}
#endif

	}
	if (clientcount > 0) {
#ifdef ENABLE_LOG4CPLUS
		log(JF_GLINED,"--- End of multiple join floods without parts ---");
#endif
		std::string astr = "Glining the following clients:" + tempNames.str();
		log(WARN, "%s", astr.c_str());
		log(WARN, "Glining %d different ips for multiple joins during join floods", clientcount);
	}
	if (IPJQueue.size() > 0)
		IPJQueue.clear();
	if ((unsigned int) (jcFCInterval - ::time(0)) >= jcJoinsPerIPTime) {
		jcFloodClients* jcFC;
		jcFCInterval = ::time(0);
		clientsIPFloodMapType::iterator anItr = clientsIPFloodMap.begin();
		for (; anItr != clientsIPFloodMap.end(); anItr++) {
			jcFC = anItr->second;
			if ((unsigned int) (::time(0) - jcFC->ctime) > jcJoinsPerIPTime) {
				clientsIPFloodMap.erase(anItr);
				delete jcFC;
			}
		}
	}

log(DBG, "Clearing %u records from the join counter.",
	jcChanMap.size()
	);
jcChanMap.clear();

time_t theTime = time(0) + jcInterval;
tidClearJoinCounter = MyUplink->RegisterTimer(theTime, this, 0);

}

void dronescan::outputNames(const std::string& chanName,std::stringstream& names,bool exclude,bool isoktogline)
{
names << "\r\n";
//std::string gString = exclude ? "Excluding" : jcGlineEnable ? "Glining" : "Suppose to gline";
std::string gString = exclude ? "Excluding" : isoktogline ? "Glining" : "Suppose to gline";
log(WARN,"%s the following clients from %s: %s",gString.c_str(),
	chanName.c_str(),names.str().c_str());
	names.str("");
}

void dronescan::processGlineQueue() {
if(glineQueue.size() > 0) 
		{
		unsigned int count = 0;
		int userCount;
		glineData* curGline;
		char us[100];
		log(DBG,"Processing gline queue\r\n");
		for(; count < gbCount && glineQueue.size() > 0;)
				{
				curGline = glineQueue.front();
				glineQueue.pop_front();
				//userCount = Network->countMatchingRealUserHost(curGline->getHost());
				StringTokenizer st(curGline->getHost(),'@');

				recentlyGlinedIpsType::iterator rItr = recentlyGlinedIps.begin();
				bool alreadyGlined = false;
				for (; rItr != recentlyGlinedIps.end(); rItr++) {
					if ((rItr->first == st[1]) && ((::time(0) - rItr->second) < 120)) {
						alreadyGlined = true;
					}
				}
				if (alreadyGlined) {
					delete curGline;
					continue;
				}

				userCount = clientsIPMap[st[1]];
				us[0] = '\0';
				sprintf(us,"%d",userCount);
				std::string glineReason = string("AUTO [") + us + string("] ") + curGline->getReason(); 
				MyUplink->setGline(nickName,curGline->getHost(),
				glineReason,curGline->getExpires(),::time(0),this);

				recentlyGlinedIps.push_front(std::pair<std::string,int>(st[1],::time(0)));
				if (recentlyGlinedIps.size() > RecentlyGlinedIpsSize) {
					recentlyGlinedIps.pop_back();
				}
				delete curGline;
				count++;
				}

		log(DBG,"Processed %d glines from the gline queue, %d glines are left in the queue",count,glineQueue.size());
		}
}
/** Report a SQL error as necessary. */
void dronescan::doSqlError(const string& theQuery, const string& theError)
{
	/* First, log it to error out */
	elog	<< "SQL> Whilst executing: "
		<< theQuery
		<< std::endl;
	elog	<< "SQL> "
		<< theError
		<< std::endl;
}

/** This function allows us to change our current state. */
void dronescan::changeState(DS_STATE newState)
{
	if(currentState == newState) return;

	/* Instantiate our own timer so we don't interfere with anyone elses */
	Timer stateTimer;
	stateTimer.Start();

	/* First, do what we need to exit our current state */
	switch( currentState )
		{
		case BURST :
			{
			calculateEntropy();
			setNickStates();
			elog	<< "*** DroneScan: Exiting state BURST"
				<< std::endl;
			lastBurstTime = ::time(0);
			break;
			}
		case RUN :
			{
			elog	<< "*** DroneScan: Exiting state RUN"
				<< std::endl;
			}
		}

	currentState = newState;

	switch( currentState )
		{
		case BURST :
			{
			charMap.clear();
			elog	<< "*** DroneScan: Entering state BURST"
				<< std::endl;
			break;
			}
		case RUN   :
			{
			elog	<< "*** DroneScan: Entering state RUN"
				<< std::endl;
			checkChannels();
			break;
			}
		}

	log(INFO, "Changed state in: %u ms", stateTimer.stopTimeMS());
}

void dronescan::updateState() 
{
    std::list< iServer* > burstingServers = Network->getAllBurstingServers();
    std::list<iServer*>::iterator ptr = burstingServers.begin();
    for(;ptr != burstingServers.end();++ptr)
    {
	log(DBG,"Server %s is bursting",(*ptr)->getName().c_str());
    }
    changeState(burstingServers.size() == 0 ? RUN : BURST);
}

void dronescan::handleChannelJoin( Channel* theChannel, iClient* theClient)
{
/* If this channel is too small, don't test it. */
if(theChannel->size() < channelCutoff) return ;

/* If this is an exceptional channel, dont test it. */
if(isExceptionalChannel(theChannel->getName())) return;

/* Iterate over our available tests, checking this channel */
if(droneChannels.find(theChannel->getName()) == droneChannels.end()) {
	/* This channel is not currently listed as active */
	// Check the channel for abnormalities, if enough
	// are found then it will be added to the
	// droneChannels structure by checkChannel()
	checkChannel( theChannel );
}

/* Reset lastjoin on the active channel */
droneChannelsType::iterator droneChanItr =
	droneChannels.find( theChannel->getName() ) ;

// If the channel is still not in the droneChannels
// structure then it is a "normal" channel
if( droneChanItr != droneChannels.end() )
	{
	droneChanItr->second->setLastJoin( ::time( 0 ) ) ;
	}

/* Do join count processing if applicable */
const string& channelName = theChannel->getName();

jcChanMapIterator jcChanIt = jcChanMap.find(channelName);
jfChannel* channel;
if( jcChanIt != jcChanMap.end() )
	{
	channel = jcChanIt->second;
	}
else
	{
	channel = new (std::nothrow) jfChannel(channelName);
	assert(channel != NULL);
	jcChanMap[channelName] = channel;
	}
unsigned int joinCount = channel->advanceChannelJoin();

if(joinCount == jcCutoff)
	{
	log(WARN, "%s is being join flooded.",
		channelName.c_str()
		);
	//if the channel was not in flooded state, clear its join record
	channel->setJoinFlooded(true);
	}
if(channel->getJoinFlooded())
	{
	channel->addJoin(theClient);
	if (joinCount >= jcCutoff) {
		string IP = xIP(theClient->getIP()).GetNumericIP();
		jcFloodClients* jcFC;
		if ((::time(0) - lastBurstTime) > 25 && jcGlineEnable) {
			clientsIPFloodMapType::const_iterator Itr = clientsIPFloodMap.find(IP);
			if (Itr != clientsIPFloodMap.end()) {
				jcFC = Itr->second;
				if ((unsigned int) (::time(0) - jcFC->ctime) > jcJoinsPerIPTime) {
					jcFC->ctime = ::time(0);
					jcFC->count = 0;
					jcFC->chans.clear();
					jcFC->nicks.clear();
					jcFC->log.clear();
				}
			}
			else {
				jcFC = new (std::nothrow) jcFloodClients;
				assert(jcFC != NULL);
				jcFC->count = 0;
				jcFC->ctime = ::time(0);
				clientsIPFloodMap[IP] = jcFC;
			}
			jcFC->count++;
			std::list< string >::iterator sItr;
			bool isMatchFound = false;
			for (sItr = jcFC->chans.begin(); sItr != jcFC->chans.end(); sItr++) {
				if (*sItr == channelName) {
					isMatchFound = true;
					break;
				}
			}
			if (!isMatchFound)
				jcFC->chans.push_back(channelName);
			isMatchFound = false;
			for (sItr = jcFC->nicks.begin(); sItr != jcFC->nicks.end(); sItr++) {
				if (*sItr == theClient->getNickName()) {
					isMatchFound = true;
					break;
				}
			}
			if (!isMatchFound)
				jcFC->nicks.push_back(theClient->getNickName());

			std::stringstream s;
			s << theClient->getNickName()
				<< "!" << theClient->getUserName()
				<< "@" << xIP(theClient->getIP()).GetNumericIP()
				<< " " << theClient->getDescription()
				<< " " << channelName <<  " " << ::time(0);
			jcFC->log.push_back(s.str());


			if ((unsigned int) jcFC->count >= jcMinJoinsPerIPToGline) {
				IPJQueue.push_back(IP);
			}
		}
	}

	}
}


void dronescan::handleChannelPart(Channel* theChan,iClient* theClient)
{

/* If this is an exceptional channel, dont test it. */
if(isExceptionalChannel(theChan->getName())) return;

jcChanMapIterator jcChanIt = jcChanMap.find(theChan->getName());
if(jcChanIt != jcChanMap.end() && jcChanIt->second->getJoinFlooded())
	{
	jcChanIt->second->advanceChannelParts();
//	if(partCount == pcCutOff)
//		{
//		log(DEBUG,"%s is being part flooded.",
//		    channelName.c_str());
//		jcChanIt->second->setPartFlooded(true);
//		}    
//	if(jcChanIt->second->getPartFlooded())
//		{
		jcChanIt->second->addPart(theClient);
//		}
	}
}

/** Here we handle new clients as they connect to the network. */
void dronescan::handleNewClient( iClient* theClient )
{
	/* First, all new clients must be assigned a clientData */
	clientData* theData = new clientData();
	assert(theClient->setCustomData(this, theData));
	++customDataCounter;

	string IP = xIP(theClient->getIP()).GetNumericIP();
	/* Store usercount per IPs to a map */
	if (clientsIPMap.find(IP) == clientsIPMap.end())
		clientsIPMap.insert(std::make_pair(IP, 1));
	else
		clientsIPMap[IP]++;



	/* If we are still bursting, calculate letter frequencies */
	if(currentState == BURST)
		{
		/* Do nothing - this will all get done later */
		}
	else if(currentState == RUN)
		{
		setClientState(theClient);
		}
}

void dronescan::handleNickChange( iClient* theClient )
{
	for(iClient::const_channelIterator it=theClient->channels_begin();
			it!=theClient->channels_end();
			++it) {
		Channel *theChannel=*it;
		/* If this channel is too small, don't test it. */
		if(theChannel->size() < channelCutoff) return ;

		/* Iterate over our available tests, checking this channel */
		if(droneChannels.find(theChannel->getName()) == droneChannels.end()) {
			/* This channel is not currently listed as active */
			// Check the channel for abnormalities, if enough
			// are found then it will be added to the
			// droneChannels structure by checkChannel()
			checkChannel( theChannel );
		}

		/* Reset lastnick on the active channel */
		droneChannelsType::iterator droneChanItr =
			droneChannels.find( theChannel->getName() ) ;

		// If the channel is still not in the droneChannels
		// structure then it is a "normal" channel
		if( droneChanItr != droneChannels.end() )
			{
			droneChanItr->second->setLastNick( ::time( 0 ) ) ;
			}

		/* Do nick count processing if applicable */
		const string& channelName = theChannel->getName();

		ncChanMap[channelName]++;

		unsigned int nickCount = ncChanMap[channelName];

		if(nickCount == ncCutoff)
			{
			log(WARN, "%s is being nick flooded.",
				channelName.c_str()
				);
			}
	}

}

/** Calculate the global entropy. */
void dronescan::calculateEntropy()
{
	/* Zero out everything */
	charMap.clear();
	averageEntropy = 0;
	totalNicks = 0;

	theTimer->Start();

	elog << "dronescan::calculateEntropy> Calculating frequencies."
		<< std::endl;
	/* First, learn the entropy from all nicks */
	for( xNetwork::const_clientIterator ptr = Network->clients_begin() ;
	     ptr != Network->clients_end() ; ++ptr)
		{
		const char *nickPtr = ptr->second->getNickName().c_str();
		for( ; *nickPtr ; ++nickPtr)
			{
			charMap[*nickPtr]++;
			}
		++totalNicks;
		}

	log(DBG, "Calculated frequencies in: %u ms", theTimer->stopTimeMS());

	theTimer->Start();

	elog << "dronescan::calculateEntropy> Normalising frequencies."
		<< std::endl;
	for( charMapType::iterator itr = charMap.begin() ;
	     itr != charMap.end(); ++itr)
		itr->second /= totalNicks;

	log(DBG, "Normalised frequencies in: %u ms", theTimer->stopTimeMS());

	elog << "dronescan::calculateEntropy> Calculating average entropy."
		<< std::endl;

	double totalEntropy = 0;

	theTimer->Start();

	for( xNetwork::const_clientIterator ptr = Network->clients_begin() ;
	     ptr != Network->clients_end() ; ++ptr)
		{
		if( ptr->second->isModeK() ) continue;
		totalEntropy += calculateEntropy(ptr->second->getNickName());
		}

	log(DBG, "Total entropy  : %lf", totalEntropy);
	log(DBG, "Total nicks    : %u", totalNicks);

	averageEntropy = totalEntropy / totalNicks;
	log(DBG, "Average entropy: %lf ", averageEntropy);

	log(DBG, "Found entropy in: %u ms", theTimer->stopTimeMS());
}


/** Return the entropy of a given client. */
double dronescan::calculateEntropy( const iClient *theClient )
{
	clientData *theData = static_cast< clientData* > ( theClient->getCustomData(this) );
	if( 0 == theData ) return 0.0 ;

	return theData->getEntropy();
}


/** Calculate state of all nicks. */
void dronescan::setNickStates()
{
	elog << "dronescan::setNickStates> Finding states of all nicks."
		<< std::endl;

	theTimer->Start();

	/* Now we must assign a state to each nick we see */
	for( xNetwork::const_clientIterator ptr = Network->clients_begin()
	     ; ptr != Network->clients_end() ; ++ptr)
		{
		setClientState( ptr->second );
		}

	log(DBG, "Set all nick states in: %u ms",
		theTimer->stopTimeMS()
		);
}

/** Check global channels for drones. */
void dronescan::checkChannels()
{
	log(INFO, "Checking channels for drones:");

	unsigned int noChannels = 0;
	unsigned int noDrones = 0;
	unsigned int failure = 0;

	theTimer->Start();

	xNetwork::const_channelIterator ptr = Network->channels_begin();

	for( ; ptr != Network->channels_end() ; ++ptr )
		{
		++noChannels;

		if(ptr->second->size() < channelCutoff) continue;

		if(!checkChannel( ptr->second )) {
			++failure;
			noDrones += ptr->second->size();
		}
		}

	log(INFO, "Finished checking %u channels. %u/%u total possible channels/clients. Duration: %u ms",
		noChannels,
		failure,
		noDrones,
		theTimer->stopTimeMS()
		);
}


/** Check a channel for drones. */
bool dronescan::checkChannel( const Channel *theChannel , const iClient *theClient )
{
	unsigned short int failed = 0;

	/* Iterate over the tests. */
	for(testMapType::iterator testItr = testMap.begin() ;
	    testItr != testMap.end() ; ++testItr )
		{
		bool hasPassed = testItr->second->isNormal(theChannel);

		if(theClient)
			{
			Reply(theClient, "%20s: %s",
				testItr->second->getName().c_str(),
				hasPassed ? "PASSED" : "FAILED"
				);
			}

		if(!hasPassed) failed += testItr->second->getWeight();
		}

	/* If we were checking for a client, don't output to console. */
	if(theClient) return true;

	/* If the failure count is over or equal to the vote cutoff
	 * report this channel as abnormal. */

	if(failed >= voteCutoff)
		{
		/* This channel is voted abnormal. */
		std::stringstream chanStat, chanParams;
		if(theChannel->getMode(Channel::MODE_I)) chanStat << "i";
		if(theChannel->getMode(Channel::MODE_R)) chanStat << "r";

		if(theChannel->getMode(Channel::MODE_K)) {
			chanStat << "k";
			chanParams << theChannel->getKey();
		}
		if(theChannel->getMode(Channel::MODE_L)) {
			chanStat << "l";
			if(theChannel->getMode(Channel::MODE_K)) chanParams << " ";
			chanParams << theChannel->getLimit();
		}

		log(WARN, "[%u] (%4u) %s +%s %s",
			failed,
			theChannel->size(),
			theChannel->getName().c_str(),
			chanStat.str().c_str(),
			chanParams.str().c_str()
			);

		/* Add this channel to the actives list */
		activeChannel *newActive = new activeChannel(theChannel->getName(), ::time(0));
		droneChannels[theChannel->getName()] = newActive;

		return false;
		}
	else
		{
		/* This channel is voted normal. */
		return true;
		}
}

/** Calculate the entropy of a given string. */
double dronescan::calculateEntropy( const string& theString )
{
	/* If we have not yet calibrated, we cannot calculate an entropy */
	assert(!charMap.empty());

	double entropy = 0;
	const char* ptr = theString.c_str();

	for( ; *ptr ; ++ptr )
		entropy += charMap[*ptr];

	return entropy / theString.length();
}


/** Check whether an iClient's nick is `normal'. */
bool dronescan::isNormal( const iClient* theClient )
{
	/* We should never see this called during BURST */
	assert(currentState != BURST);

	clientData *theData = static_cast< clientData* > (theClient->getCustomData(this));
	assert( theData != 0 ) ;

	/* We should never see an unknown state for an initialised client */
	assert(!theData->isUnknown());

	return theData->isNormal();
}


/** Set the iClient's state depending on certain features. */
CLIENT_STATE dronescan::setClientState( iClient *theClient )
{
	clientData* theData = static_cast< clientData* > (theClient->getCustomData(this));
	assert(theData != 0);

	double userEntropy = calculateEntropy(theClient->getNickName());
	theData->setEntropy(userEntropy);

	/* Make sure our services don't skew anything */
	if(theClient->isModeK()) return theData->setState(NORMAL);

	/*
	 * First, check if the first 8 chars of the realname are the same as the
	 * first 8 chars of the nickname.
	 */
	if( strncmp(	theClient->getNickName().c_str(),
			theClient->getDescription().c_str(),
			8 ) == 0)
		{
		return theData->setState(ABNORMAL);
		}

	/*
	 * Second, check whether the nickname itself is abnormal.
	 */
	if( userEntropy > averageEntropy * (1 + nickMargin) ||
	    userEntropy < averageEntropy * (1 - nickMargin) )
		return theData->setState(ABNORMAL);

	/*
	 * It has passed the checks. It is therefore normal.
	 */
	return theData->setState(NORMAL);
}


/** Log a message. */
void dronescan::log(LOG_TYPE logType, const char *format, ...)
{
	if(logType < consoleLevel) return;

	std::stringstream newMessage;

	switch(logType) {
		case DBG	: newMessage << "[D] ";	break;
		case INFO	: newMessage << "[I] ";	break;
		case WARN	: newMessage << "[W] ";	break;
		case ERR	: newMessage << "[E] ";	break;
		default		: newMessage << "[U] ";	break;
	}

	char buffer[512] = {0};
	va_list _list;

	va_start(_list, format);
	vsnprintf(buffer, 512, format, _list);
	va_end(_list);

	newMessage << buffer;

	Message(consoleChannel, "%s",newMessage.str().c_str());
}

#ifdef ENABLE_LOG4CPLUS
void dronescan::log(char* cat,const char* format, ...)
{
	char buffer[512] = {0};
	va_list _list;

	va_start(_list, format);
	vsnprintf(buffer, 512, format, _list);
	va_end(_list);

	log4cplus::Logger::getInstance(cat).log(log4cplus::INFO_LOG_LEVEL,buffer);

}
#endif

/** Set the topic of the console channel. */
void dronescan::setConsoleTopic()
{
	std::stringstream setTopic;
	setTopic	<< getCharYYXXX() << " T "
			<< consoleChannel << " :"
			;

	setTopic	<< "  ||"
			<< "  channelMargin: " << channelMargin
			<< "  nickMargin: " << nickMargin
			<< "  channelCutoff: " << channelCutoff
			;

	setTopic	<< "  ||"
			<< "  jcInterval: " << jcInterval
			<< "  jcCutoff: " << jcCutoff
			<< "  ncInterval: " << ncInterval
			<< "  ncCutoff: " << ncCutoff
			;

	Write(setTopic);
}


/** Reply to an iClient. */
void dronescan::Reply(const iClient *theClient, const char *format, ...)
{
	char buffer[512] = {0};
	va_list _list;

	va_start(_list, format);
	vsnprintf(buffer, 512, format, _list);
	va_end(_list);

	Message(theClient, buffer);
}


sqlUser *dronescan::getSqlUser(const string& theNick)
{
	userMapType::const_iterator itr = userMap.find(theNick);

	if(itr != userMap.end()) {
		return itr->second;
	}

	return 0;
}


/** Are we due an update? */
bool dronescan::updateDue(string _table)
{
	std::stringstream check;
	check	<< "SELECT max(last_updated) FROM " << _table;

	if( !SQLDb->Exec(check, true ) )
//	if( PGRES_TUPLES_OK != status )
		{
		doSqlError(check.str(), SQLDb->ErrorMessage());
		return false;
		}

	time_t maxUpdated = atoi(SQLDb->GetValue(0, 0));

	if( maxUpdated > lastUpdated[_table] ) return true;

	return false;
}


/** Preload the fake clients cache */
void dronescan::preloadFakeClientCache()
{
	/* Are we due to update? */
	if(!updateDue("FAKECLIENTS")) return;

	std::stringstream theQuery;
	theQuery	<< sql::fakeclients ;

	if(!SQLDb->Exec(theQuery, true)) {
		doSqlError(theQuery.str(), SQLDb->ErrorMessage());
		return;
	}

	for(fcMapType::iterator itr = fakeClients.begin() ;
	    itr != fakeClients.end(); ++itr) {
		delete itr->second;
	}

	fakeClients.clear();

/*
	string yyxxx( MyUplink->getCharYY() + "]]]" );
*/

	for(unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
		sqlFakeClient *newFake = new sqlFakeClient(SQLDb);
		assert(newFake != 0);

		newFake->setAllMembers(i);
		fakeClients.insert(fcMapType::value_type(newFake->getId(), newFake));

/*
		iClient *fakeClient = new iClient(
			MyUplink->getIntYY(),
			yyxxx,
			newFake->getNickName(),
			newFake->getUserName(),
			"AKAQEK",
			newFake->getHostName(),
			newFake->getHostName(),
			"+i",
			string(),
			0,
			newFake->getRealName(),
			::time(0)
			);

		assert( fakeClient != 0 );

		MyUplink->AttachClient( fakeClient );
*/
	}

	elog	<< "dronescan::preloadFakeClientCache> Loaded "
		<< fakeClients.size()
		<< " fake clients."
		<< std::endl;
	log(INFO, "Loaded %u fake clients.", fakeClients.size());
}


/** Preload the users cache */
void dronescan::preloadUserCache()
{
	/* Are we due to update? */
	if(!updateDue("USERS")) return;

	std::stringstream theQuery;
	theQuery	<< "SELECT user_name,last_seen,last_updated_by,last_updated,flags,access,created "
			<< "FROM users"
			;

	if( SQLDb->Exec(theQuery,true ) )
//	if(PGRES_TUPLES_OK == status)
		{
		/* First we need to clear the current cache. */
		for(userMapType::iterator itr = userMap.begin() ;
		    itr != userMap.end() ; ++itr) {
			delete itr->second;
		}
		userMap.clear();

		for(unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
			sqlUser *newUser = new sqlUser(SQLDb);
			assert(newUser != 0);

			newUser->setAllMembers(i);
			userMap.insert(userMapType::value_type(newUser->getUserName(), newUser));
		}
	} else {
		elog	<< "dronescan::preloadUserCache> "
			<< SQLDb->ErrorMessage();
	}

	elog	<< "dronescan::preloadUserCache> Loaded "
		<< userMap.size()
		<< " users."
		<< std::endl ;
}

bool dronescan::preloadExceptionalChannels()
{
std::stringstream theQuery;
theQuery	<< "SELECT name FROM exceptionalChannels";

//if(PGRES_TUPLES_OK == status) {
if( SQLDb->Exec(theQuery, true ) )
	{
	/* First we need to clear the current cache. */
	exceptionalChannels.clear();
	for(unsigned int i = 0; i < SQLDb->Tuples(); ++i) 
		{
		exceptionalChannels.push_back(SQLDb->GetValue(i,0));
		}
	}
else
	{
	elog	<< "dronescan::preloadExceptionalChannels> "
		<< SQLDb->ErrorMessage();
		return false;
	}

elog	<< "dronescan::preloadExceptionalChannels> Loaded "
	<< exceptionalChannels.size()
	<< " exceptional channels."
	<< std::endl ;
return true;
}

/** Register a new command. */
bool dronescan::RegisterCommand( Command *theCommand )
{
	return commandMap.insert( commandPairType(theCommand->getName(), theCommand)).second;
}


/** Register a new test. */
bool dronescan::RegisterTest( Test *theTest )
{
	return testMap.insert( testPairType(theTest->getName(), theTest) ).second;
}

/** Unregister a test. */
bool dronescan::UnRegisterTest( const string& testName )
{
	testMapType::iterator ptr = testMap.find( testName );
	if( ptr == testMap.end() ) return false;

	delete ptr->second;
	testMap.erase(ptr);

	return true;
}

/** Set a test variable. */
Test *dronescan::setTestVariable( const string& var, const string& value )
{
	for( testMapType::iterator testItr = testMap.begin() ;
	     testItr != testMap.end() ; ++testItr ) {
		if(testItr->second->setVariable(var, value))
			return testItr->second;
	}

	return 0;
}

bool dronescan::isExceptionalChannel(const string& chanName)
{
    exceptionalChannelsType::iterator it = exceptionalChannels.begin();
    for(;it != exceptionalChannels.end();++it)
    {
	    if( !strcasecmp((*it).c_str(),chanName.c_str()))
	    {
		    return true;
	    }
    }
    return false;
}



bool dronescan::addExceptionalChannel(const string& chanName)
{
std::stringstream insertQ;
insertQ << "INSERT into exceptionalChannels(name) VALUES('"
        << chanName << "');" << std::ends;

//ExecStatusType status = SQLDb->Exec(insertQ.str().c_str());
if( !SQLDb->Exec(insertQ) )
//if (PGRES_COMMAND_OK != status)
	{
	elog << "ERROR while adding exceptionalChannel: " << SQLDb->ErrorMessage() << std::endl;
	return false;    
	}
exceptionalChannels.push_back(chanName);
return true;
}

bool dronescan::remExceptionalChannel(const string& chanName)
{
std::stringstream insertQ;
insertQ << "DELETE from exceptionalChannels where name='"
        << chanName << "';" << std::ends;

//ExecStatusType status = SQLDb->Exec(insertQ.str().c_str());
if( !SQLDb->Exec(insertQ) )
//if (PGRES_COMMAND_OK != status)
	{
	elog << "ERROR while removing exceptionalChannel: " << SQLDb->ErrorMessage() << std::endl;
	return false;    
	}
exceptionalChannelsType::iterator it = exceptionalChannels.begin();
for(;it != exceptionalChannels.end();++it)
	{
        if( !strcasecmp((*it).c_str(),chanName.c_str()))
		{
		exceptionalChannels.erase(it);
    	        return true;
		}
	}
return true;
}

/** Return usage information for a client */
void Command::Usage( const iClient *theClient )
{
bot->Reply(theClient, "SYNTAX: %s", getInfo().c_str());
}

} // namespace ds

} // namespace gnuworld
