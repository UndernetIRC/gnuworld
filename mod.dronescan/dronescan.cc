/*
 * dronescan.cc
 */

#include <stdarg.h>	/* va_list */
#include <stdio.h>	/* *printf() */

#include "EConfig.h"
#include "Network.h"
#include "server.h"
#include "StringTokenizer.h"

#include "clientData.h"
#include "dronescan.h"

namespace gnuworld {

namespace ds {

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

/* What tests do we enable? */
enabledTests = atoi(dronescanConfig->Require("enabledTests")->second.c_str());

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

if(testEnabled(TST_ABNORMALS))
	{
	RegisterTest(new ABNORMALSTest(this, "ABNORMALS", "Checks for the percentage of normal clients."));
	}

if(testEnabled(TST_HASOP))
	{
	RegisterTest(new HASOPTest(this, "HASOP", "Checks if a channel has any ops."));
	}

if(testEnabled(TST_JOINCOUNT))
	{
	/* Set up join counter config options. */
	jcInterval = atoi(dronescanConfig->Require("jcInterval")->second.c_str());
	jcCutoff = atoi(dronescanConfig->Require("jcCutoff")->second.c_str());
	}

if(testEnabled(TST_CHANRANGE))
	{
	/* Set up channel range config options */
	channelRange = atof(dronescanConfig->Require("channelRange")->second.c_str());
	
	/* Register the test */
	RegisterTest(new RANGETest(this, "RANGE", "Checks the entropy range."));
	}

if(testEnabled(TST_MAXCHANS))
	{
	/* Set up the max chans config options */
	maxChans = atoi(dronescanConfig->Require("maxChans")->second.c_str());
	
	/* Register the test */
	RegisterTest(new MAXCHANSTest(this, "MAXCHANS", "Checks the max channel membership of a channel."));
	}

/* Set up console logging level. */
consoleLevel = atoi(dronescanConfig->Require("consoleLevel")->second.c_str());

/* Initialise statistics */
customDataCounter = 0;

/* Set up our timer. */
theTimer = new Timer();

/* Register commands available to users */
RegisterCommand(new CHECKCommand(this, "CHECK", "(<#channel>) (<user>)"));
} // dronescan::dronescan(const string&)



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

	/* Set up our JC counter if needed */
	if(testEnabled(TST_JOINCOUNT))
		{
		time_t theTime = time(NULL) + jcInterval;
		tidClearJoinCounter = theServer->RegisterTimer(theTime, this, 0);
		}

	xClient::ImplementServer( theServer );
} // dronescan::ImplementServer(xServer*)


/**
 * Here we decide what channels our xClient needs to burst in to.
 * The only channel of any interest to us is our console channel, which is
 * loaded from the configuration file.
 */
int dronescan::BurstChannels()
{
	MyUplink->JoinChannel(this, consoleChannel,
		dronescanConfig->Require("consoleChannelModes")->second);

	/* We need to assign a clientData to all xClients on this xServer */
	xNetwork::const_localClientIterator itr = Network->localClient_begin();
	for( ; itr != Network->localClient_end(); ++itr )
		{
		xClient *theXClient = *itr;
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


/**
 * Here we receive network events that we are registered for.
 */
int dronescan::OnEvent( const eventType& theEvent,
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

	return xClient::OnEvent( theEvent, Data1, Data2, Data3, Data4);
}


/** Receive channel events. */
int dronescan::OnChannelEvent( const channelEventType& theEvent,
	Channel *theChannel, void *Data1, void *Data2, void *Data3, void *Data4 )
{
	/* If this is not a join, we don't care. */
	if(theEvent != EVT_JOIN) return 0;
	
	/* If we are bursting, we don't want to be checking joins. */
	if(currentState == BURST) return 0;

	/* If this channel is too small, don't test it. */
	if(theChannel->size() < channelCutoff) return 0;

	/* Iterate over our available tests, checking this channel */
	checkChannel( theChannel );
	
	/* Do join count processing if applicable */
	if(testEnabled(TST_JOINCOUNT))
		{
		string channelName = theChannel->getName();
		
		jcChanMap[channelName]++;
		
		unsigned int joinCount = jcChanMap[channelName];
		
		if(joinCount > jcCutoff)
			{
			log(WARN, "%s has had %u joins within the last %us.",
				channelName.c_str(),
				joinCount,
				jcInterval
				);
			}
		}
	
	return 0;
}


/**
 * Here we receive private messages from iClients.
 */
int dronescan::OnPrivateMessage( iClient* theClient, const string& Message, bool secure )
{
	if(!theClient->isOper()) return 0;
	
	StringTokenizer st(Message);
	
	if(st.size() < 1) return 0;
	
	string Command = string_upper(st[0]);
	commandMapType::iterator commandHandler = commandMap.find(Command);
	
	if(commandHandler != commandMap.end())
		{
		commandHandler->second->Exec(theClient, Message);
		return 1;
		}
		
	if("INVITE" == Command)
		{
		Invite(theClient, consoleChannel);
		return 0;
		}
	
	if("STATS" == Command)
		{
		Reply(theClient, "Allocated custom data: %d", customDataCounter);
		if(1)
			Reply(theClient, "CM/NM/CC: %0.2lf/%0.2lf/%u",
				channelMargin,
				nickMargin,
				channelCutoff
				);
		if(testEnabled(TST_JOINCOUNT))
			Reply(theClient, "jcI/jcC : %u/%u",
				jcInterval,
				jcCutoff
				);
		if(testEnabled(TST_CHANRANGE))
			Reply(theClient, "CR      : %0.3lf",
				channelRange);
		if(testEnabled(TST_MAXCHANS))
			Reply(theClient, "MaxChans: %u",
				maxChans);
		return 0;
		}
	
	if("RESET" == Command)
		{
		resetAndCheck();
		return 0;
		}
	
	if(st.size() < 2) return 0;
	
	if("INFO" == Command)
		{
		string nick = st[1];
		iClient *targetClient = Network->findNick(nick);
		if(!targetClient) return 0;
		Reply(theClient, "Status of %s: %s",
			targetClient->getNickName().c_str(),
			(isNormal(targetClient) ? "Normal" : "Abnormal" )
			);
		return 0;
		}
	
	if(st.size() < 3) return 0;
	
	if("SET" == Command)
		{
		string Option = string_upper(st[1]);
		
		/* Global entropy options */
		if("CC" == Option)
			{
			unsigned int newCC = atoi(st[2].c_str());
			channelCutoff = newCC;
			resetAndCheck();
			}
		if("CM" == Option)
			{
			double newCM = atof(st[2].c_str());
			if(newCM < 0 || newCM > 1) return 0;
			channelMargin = newCM;
			resetAndCheck();
			}
		if("NM" == Option)
			{
			double newNM = atof(st[2].c_str());
			if(newNM < 0 || newNM > 1) return 0;
			nickMargin = newNM;
			resetAndCheck();
			}
		
		/* Channel entropy options */
		if("CR" == Option)
			{
			double newCR = atof(st[2].c_str());
			if(newCR < 0) return 0;
			channelRange = newCR;
			resetAndCheck();
			}
		}
	
	return 0;
}


/** Receive our own timed events. */
int dronescan::OnTimer( xServer::timerID theTimer , void *)
{
	if(theTimer == tidClearJoinCounter)
		{
		log(DEBUG, "Clearing %u records from the join counter.",
			jcChanMap.size()
			);
		jcChanMap.clear();
		
		time_t theTime = time(0) + jcInterval;
		tidClearJoinCounter = MyUplink->RegisterTimer(theTime, this, 0);
		}
	
	return 0;
}



/*******************************************
 ** D R O N E S C A N   F U N C T I O N S **
 *******************************************/
 
/** This function allows us to change our current state. */
void dronescan::changeState(DS_STATE newState)
{
	elog << BURST << " " << RUN << " - " << currentState << " " << newState << endl;
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
	unsigned int failure = 0;

	theTimer->Start();

	xNetwork::constChannelIterator ptr = Network->channels_begin();
			
	for( ; ptr != Network->channels_end() ; ++ptr )
		{
		++noChannels;
		
		if(ptr->second->size() < channelCutoff) continue;
		
		if(!checkChannel( ptr->second )) ++failure;
		}
	
	log(INFO, "Finished checking %u channels. %u possibles. Duration: %u ms",
		noChannels,
		failure,
		theTimer->stopTimeMS()
		);
}


/** Check a channel for drones. */
bool dronescan::checkChannel( const Channel *theChannel , const iClient *theClient )
{
	unsigned short int normal = 0;

	/* Iterate over the tests. */
	for(testVectorType::iterator testItr = testVector.begin() ;
	    testItr != testVector.end() ; ++testItr )
		{
		bool hasPassed = (*testItr)->isNormal(theChannel);

		if(theClient)
			{
			Reply(theClient, "%20s: %s",
				(*testItr)->getName().c_str(),
				hasPassed ? "PASSED" : "FAILED"
				);
			}

		if(hasPassed) ++normal;
		}

	/* If we were checking for a client, don't output to console. */
	if(theClient) return true;
	
	/* If the normal count is over half of the total test numbers
	 * we report that it is normal. Else it is abnormal. */
	
	/* Use >= so for an even size() we need over half to fail */
	if(normal >= (testVector.size() / 2))
		{
		/* This channel is voted normal. */
		return true;
		}
	else
		{
		/* This channel is voted abnormal. */
		log(WARN, "  AC: %20s - %u/%u tests failed - %u users",
			theChannel->getName().c_str(),
			(testVector.size() - normal),
			testVector.size(),
			theChannel->size()
			);
		return false;
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
			<< "enabledTests: " << enabledTests
			;
	
	if(1)
		{
		setTopic	<< "  ||"
				<< "  channelMargin: " << channelMargin
				<< "  nickMargin: " << nickMargin
				<< "  channelCutoff: " << channelCutoff
				;
		}
	
	if(testEnabled(TST_JOINCOUNT))
		{
		setTopic	<< "  ||"
				<< "  jcInterval: " << jcInterval
				<< "  jcCutoff: " << jcCutoff
				;
		}
	
	if(testEnabled(TST_CHANRANGE))
		{
		setTopic	<< "  ||"
				<< "  channelRange: " << channelRange
				;
		}
	
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


/** Register a new command. */
bool dronescan::RegisterCommand( Command *theCommand )
{
	return commandMap.insert( commandPairType(theCommand->getName(), theCommand)).second;
}


/** Register a new test. */
void dronescan::RegisterTest( Test *theTest )
{
	testVector.push_back(theTest);
}



/** Return usage information for a client */
void Command::Usage( const iClient *theClient )
{
	bot->Reply(theClient, "SYNTAX: %s", getInfo().c_str());
}



} // namespace ds

} // namespace gnuworld
