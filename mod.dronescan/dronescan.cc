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
 *
 * $Id: dronescan.cc,v 1.30 2003/08/09 23:15:35 dan_karrels Exp $
 */

#include	<string>

#include <cstdarg>	/* va_list */
#include <cstdio>	/* *printf() */

#include "libpq++.h"

#include "config.h"
#include "EConfig.h"
#include "Network.h"
#include "server.h"
#include "StringTokenizer.h"

#include "clientData.h"
#include "dronescan.h"
#include "dronescanCommands.h"
#include "dronescanTests.h"
#include "sqlUser.h"
#include "Timer.h"

RCSTAG("$Id: dronescan.cc,v 1.30 2003/08/09 23:15:35 dan_karrels Exp $");

namespace gnuworld {

namespace ds {

using std::string ;

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

/* Set up variables that our tests will need */
typedef vector<string> testVarsType;
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
			<< endl;
	} else {
		elog	<< "dronescan> No test accepted variable: "
			<< *itr << " @ " << theValue
			<< endl;
		::exit(1);
	}
}

/* Set up database */
string sqlHost(dronescanConfig->Require("sqlHost")->second);
string sqlPort(dronescanConfig->Require("sqlPort")->second);
string sqlDB(dronescanConfig->Require("sqlDB")->second);
string sqlUser(dronescanConfig->Require("sqlUser")->second);
string sqlPass(dronescanConfig->Require("sqlPass")->second);

stringstream connectString;
connectString	<< "host=" << sqlHost << " "
		<< "port=" << sqlPort << " "
		<< "dbname=" << sqlDB << " "
		<< "user=" << sqlUser << " "
		<< "password=" << sqlPass
		;
elog << "dronescan::dronescan> Connecting to SQL Server." << endl;

SQLDb = new PgDatabase(connectString.str().c_str());
if(SQLDb->ConnectionBad()) {
	elog	<< "dronescan> Failed to connect to SQL server: "
		<< SQLDb->ErrorMessage()
		<< endl;
	::exit(0);
}

elog << "dronescan> Established connection to SQL server." << endl;

/* Preload users */
preloadUserCache();

/* Initialise statistics */
customDataCounter = 0;

/* Set up our timer. */
theTimer = new Timer();

/* Register commands available to users */
RegisterCommand(new ACCESSCommand(this, "ACCESS", "() (<user>)"));
RegisterCommand(new ADDUSERCommand(this, "ADDUSER", "<user> <access>"));
RegisterCommand(new CHECKCommand(this, "CHECK", "(<#channel>) (<user>)"));
RegisterCommand(new LISTCommand(this, "LIST", "(active)"));
RegisterCommand(new MODUSERCommand(this, "MODUSER", "(ACCESS <user> <level>"));
RegisterCommand(new QUOTECommand(this, "QUOTE", "<string>"));
RegisterCommand(new REMUSERCommand(this, "REMUSER", "<user>"));
RegisterCommand(new STATUSCommand(this, "STATUS", ""));
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
void dronescan::ImplementServer( xServer* theServer )
{
	/* Register for global network events */
	theServer->RegisterEvent( EVT_BURST_CMPLT, this );
	theServer->RegisterEvent( EVT_NETJOIN, this );
	theServer->RegisterEvent( EVT_NICK, this );
	theServer->RegisterEvent( EVT_KILL, this );
	theServer->RegisterEvent( EVT_QUIT, this );
	
	/* Register for all channel events */
	theServer->RegisterChannelEvent( xServer::CHANNEL_ALL, this );

	/* Set up clearing active channels */
	time_t theTime = time(0) + dcInterval;
	tidClearActiveList = theServer->RegisterTimer(theTime, this, 0);

	/* Set up our JC counter */
	theTime = time(0) + jcInterval;
	tidClearJoinCounter = theServer->RegisterTimer(theTime, this, 0);

	xClient::ImplementServer( theServer );
} // dronescan::ImplementServer(xServer*)


/**
 * Here we decide what channels our xClient needs to burst in to.
 * The only channel of any interest to us is our console channel, which is
 * loaded from the configuration file.
 */
bool dronescan::BurstChannels()
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
		DoCTCP(theClient, CTCP, "GNUWorld DroneScan v0.0.5");
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
			log(DEBUG, "Caught EOB. Resetting frequencies.");
			changeState(RUN);
			break;
			} // EVT_BURST_CMPLT
		case EVT_NETJOIN :
			{
			changeState(BURST);
			break;
			}
		case EVT_NICK :
			{
			handleNewClient( static_cast< iClient* >( Data1 ) );
			break;
			} // EVT_NICK
		
		case EVT_KILL : /* Intentional drop through */
		case EVT_QUIT :
			{
			iClient *theClient = static_cast< iClient* >
				( theEvent == EVT_KILL ?
					Data2 :
					Data1 );
			
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
	/* If this is not a join, we don't care. */
	if(theEvent != EVT_JOIN) return ;
	
	/* If we are bursting, we don't want to be checking joins. */
	if(currentState == BURST) return ;

	/* If this channel is too small, don't test it. */
	if(theChannel->size() < channelCutoff) return ;

	/* Iterate over our available tests, checking this channel */
	if(find(droneChannels.begin(), droneChannels.end(), theChannel->getName()) == droneChannels.end()) {
		/* This channel is not currently listed as active */
		checkChannel( theChannel );
	}
	
	/* Do join count processing if applicable */
	string channelName = theChannel->getName();

	jcChanMap[channelName]++;

	unsigned int joinCount = jcChanMap[channelName];

	if(joinCount == jcCutoff)
		{
		log(WARN, "%s is being join flooded.",
			channelName.c_str()
			);
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

	if(!theUser) return ;
	
	/* We have now seen this user! */
	theUser->setLastSeen(::time(0));
	theUser->commit();

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
	if("RELOAD" == Command)
		{
		getUplink()->UnloadClient(this, "Reloading...");
		getUplink()->LoadClient("libdronescan.la", getConfigFileName());
		return ;
		}

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
void dronescan::OnQuit()
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
   !MyUplink->UnRegisterTimer(tidClearActiveList, 0)) {
	elog	<< "dronescan::~dronescan> "
		<< "Could not unregister timer. Expect problems shortly."
		<< endl;
}

/* Done! */

}


/** Receive our own timed events. */
void dronescan::OnTimer( xServer::timerID theTimer , void *)
{
	time_t theTime;

	if(theTimer == tidClearActiveList)
		{
		log(DEBUG, "Clearing %u active channels records.",
			droneChannels.size()
			);
		droneChannels.clear();
		
		theTime = time(0) + dcInterval;
		tidClearActiveList = MyUplink->RegisterTimer(theTime, this, 0);
		}

	if(theTimer == tidClearJoinCounter)
		{
		for(jcChanMapType::const_iterator itr = jcChanMap.begin() ;
		    itr != jcChanMap.end() ; ++itr) {
			if(itr->second >= jcCutoff)
				log(WARN, "Join flood over in %s. Total joins: %u",
					itr->first.c_str(),
					itr->second
					);
		}
		
		log(DEBUG, "Clearing %u records from the join counter.",
			jcChanMap.size()
			);
		jcChanMap.clear();
		
		theTime = time(0) + jcInterval;
		tidClearJoinCounter = MyUplink->RegisterTimer(theTime, this, 0);
		}
}

/*******************************************
 ** D R O N E S C A N   F U N C T I O N S **
 *******************************************/
 
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
				<< endl;
			break;
			}
		case RUN :
			{
			elog	<< "*** DroneScan: Exiting state RUN"
				<< endl;
			}
		}

	currentState = newState;
	
	switch( currentState )
		{
		case BURST :
			{
			charMap.clear();
			elog	<< "*** DroneScan: Entering state BURST"
				<< endl;
			break;
			}
		case RUN   : 
			{
			elog	<< "*** DroneScan: Entering state RUN"
				<< endl;
			checkChannels();
			break;
			}
		}
	
	log(INFO, "Changed state in: %u ms", stateTimer.stopTimeMS());
}
 

/** Here we handle new clients as they connect to the network. */
void dronescan::handleNewClient( iClient* theClient )
{
	/* First, all new clients must be assigned a clientData */
	clientData* theData = new clientData();
	assert(theClient->setCustomData(this, theData));
	++customDataCounter;


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

/** Calculate the global entropy. */
void dronescan::calculateEntropy()
{
	/* Zero out everything */
	charMap.clear();
	averageEntropy = 0;
	totalNicks = 0;
	
	theTimer->Start();
	
	elog << "dronescan::calculateEntropy> Calculating frequencies." << endl;
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
	
	log(DEBUG, "Calculated frequencies in: %u ms", theTimer->stopTimeMS());
	
	theTimer->Start();
	
	elog << "dronescan::calculateEntropy> Normalising frequencies." << endl;
	for( charMapType::iterator itr = charMap.begin() ;
	     itr != charMap.end(); ++itr)
		itr->second /= totalNicks;
	
	log(DEBUG, "Normalised frequencies in: %u ms", theTimer->stopTimeMS());
	
	elog << "dronescan::calculateEntropy> Calculating average entropy." << endl;

	double totalEntropy = 0;
	
	theTimer->Start();

	for( xNetwork::const_clientIterator ptr = Network->clients_begin() ;
	     ptr != Network->clients_end() ; ++ptr)
		{
		if( ptr->second->isModeK() ) continue;
		totalEntropy += calculateEntropy(ptr->second->getNickName());
		}
	
	log(DEBUG, "Total entropy  : %lf", totalEntropy);
	log(DEBUG, "Total nicks    : %u", totalNicks);
			
	averageEntropy = totalEntropy / totalNicks;
	log(DEBUG, "Average entropy: %lf ", averageEntropy);
	
	log(DEBUG, "Found entropy in: %u ms", theTimer->stopTimeMS());
}


/** Return the entropy of a given client. */
double dronescan::calculateEntropy( const iClient *theClient )
{
	clientData *theData = static_cast< clientData* > ( theClient->getCustomData(this) );

	assert(theData->getEntropy() != 0);
	
	return theData->getEntropy();	
}


/** Calculate state of all nicks. */
void dronescan::setNickStates()
{
	elog << "dronescan::setNickStates> Finding states of all nicks." << endl;
	
	theTimer->Start();
	
	/* Now we must assign a state to each nick we see */
	for( xNetwork::const_clientIterator ptr = Network->clients_begin()
	     ; ptr != Network->clients_end() ; ++ptr)
		{
		setClientState( ptr->second );
		}
	
	log(DEBUG, "Set all nick states in: %u ms",
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
		stringstream chanStat, chanParams;
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
		
		log(WARN, "[%u] (%3u) %s +%s %s",
			failed,
			theChannel->size(),
			theChannel->getName().c_str(),
			chanStat.str().c_str(),
			chanParams.str().c_str()
			);
		
		/* Add this channel to the actives list */
		droneChannels.push_back(theChannel->getName());

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
void dronescan::log(LOG_TYPE logType, char *format, ...)
{
	if(logType < consoleLevel) return;
	
	stringstream newMessage;
	
	switch(logType) {
		case DEBUG	: newMessage << "[D] ";	break;
		case INFO	: newMessage << "[I] ";	break;
		case WARN	: newMessage << "[W] ";	break;
		case ERROR	: newMessage << "[E] ";	break;
		default		: newMessage << "[U] ";	break;
	}

	char buffer[512] = {0};
	va_list _list;
	
	va_start(_list, format);
	vsnprintf(buffer, 512, format, _list);
	va_end(_list);
	
	newMessage << buffer;
	
	Message(consoleChannel, newMessage.str().c_str());
}


/** Set the topic of the console channel. */
void dronescan::setConsoleTopic()
{
	stringstream setTopic;
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
			;

	Write(setTopic);
}


/** Reply to an iClient. */
void dronescan::Reply(const iClient *theClient, char *format, ...)
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


/** Preload the users cache */
void dronescan::preloadUserCache()
{
	stringstream theQuery;
	theQuery	<< "SELECT user_name,last_seen,last_updated_by,last_updated,flags,access,created "
			<< "FROM users"
			;
	
	ExecStatusType status = SQLDb->Exec(theQuery.str().c_str());
	
	if(PGRES_TUPLES_OK == status) {
		/* First we need to clear the current cache. */
		for(userMapType::iterator itr = userMap.begin() ;
		    itr != userMap.end() ; ++itr) {
			delete itr->second;
		}
		userMap.clear();
		
		for(int i = 0; i < SQLDb->Tuples(); ++i) {
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
		<< endl
		;
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


/** Return usage information for a client */
void Command::Usage( const iClient *theClient )
{
	bot->Reply(theClient, "SYNTAX: %s", getInfo().c_str());
}



} // namespace ds

} // namespace gnuworld
