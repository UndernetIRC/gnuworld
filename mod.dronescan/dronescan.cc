/*
 * dronescan.cc
 */

#include <stdio.h>	/* *printf */
#include <stdarg.h>	/* va_list */
#include <sys/time.h>	/* gettimeofday() */

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

/* Initialise statistics */
customDataCounter = 0;

/* Set up our timer. */
theTimer = new Timer();
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
	theServer->RegisterEvent( EVT_BURST_CMPLT, this );
	theServer->RegisterEvent( EVT_NETJOIN, this );
	theServer->RegisterEvent( EVT_NICK, this );
	theServer->RegisterEvent( EVT_KILL, this );
	theServer->RegisterEvent( EVT_QUIT, this );
	
	theServer->RegisterChannelEvent( xServer::CHANNEL_ALL, this );

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

	MyUplink->RegisterChannelEvent(consoleChannel, this);

	/* We need to assign ourselves a clientData */
	clientData *ourData = new clientData();
	ourData->setState(NORMAL);
	this->getInstance()->setCustomData(this, ourData);
	customDataCounter++;

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
			elog << "Got BURST_CMPLT" << endl;
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

	/* For a ChannelEvent, Data1 is always the target iClient */
	iClient *theClient = static_cast < iClient* > ( Data1 );
	
	/*
	 * If the client is normal, we don't care as they cannot cause a
	 * channel to be marked abnormal. This also prevents spurious warning
	 * messages when people enter a channel to investigate.
	 */
	
	if(isNormal(theClient)) return 0;
	
	/* Is the client opered? If so, don't generate warnings. */
	if(theClient->isOper()) return 0;
	
	/* An abnormal client has entered a channel. Check it. */
	unsigned int abnormals;
	if(!(abnormals = isAbnormal(theChannel))) return 0;
	
	/* We are above the channelMargin threshold for abnormals. Warn. */
	log(WARN, "WARNING: %s joined %s. Membership is now %d/%d.",
		theClient->getNickName().c_str(),
		theChannel->getName().c_str(),
		abnormals,
		theChannel->size()
		);
	
	return 0;
}


/**
 * Here we receive private messages from iClients.
 */
int dronescan::OnPrivateMessage( iClient* theClient, const string& message, bool secure )
{
	if(!theClient->isOper()) return 0;
	
	StringTokenizer st(message);
	
	if(st.size() < 1) return 0;
	
	string command = string_upper(st[0]);
	
	if("STATS" == command)
		{
		Reply(theClient, "Allocated custom data: %d", customDataCounter);
		return 0;
		}
	
	if("RESET" == command)
		{
		resetAndCheck();
		return 0;
		}
	
	if(st.size() < 2) return 0;
	
	if("INFO" == command)
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
	
	if("SET" == command)
		{
		string option = string_upper(st[1]);
		
		if("CC" == option)
			{
			unsigned int newCC = atoi(st[2].c_str());
			channelCutoff = newCC;
			resetAndCheck();
			}
		if("CM" == option)
			{
			double newCM = atof(st[2].c_str());
			channelMargin = newCM;
			resetAndCheck();
			}
		if("NM" == option)
			{
			double newNM = atof(st[2].c_str());
//			if(newNM <= 0 || newNM >= 1) return 0;
			nickMargin = newNM;
			resetAndCheck();
			}
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
	
	log(INFO, "Changed state in: %d ms", stateTimer.stopTimeMS());
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
	
	log(DEBUG, "Calculated frequencies in: %d ms", theTimer->stopTimeMS());
	
	theTimer->Start();
	
	elog << "dronescan::calculateEntropy> Normalising frequencies." << endl;
	for( charMapType::iterator itr = charMap.begin() ;
	     itr != charMap.end(); ++itr)
		itr->second /= totalNicks;
	
	log(DEBUG, "Normalised frequencies in: %d ms", theTimer->stopTimeMS());
	
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
	log(DEBUG, "Total nicks    : %d", totalNicks);
			
	averageEntropy = totalEntropy / totalNicks;
	log(DEBUG, "Average entropy: %lf ", averageEntropy);
	
	log(DEBUG, "Found entropy in: %d ms", theTimer->stopTimeMS());
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
	
	log(DEBUG, "Set all nick states in: %d ms",
		theTimer->stopTimeMS()
		);
}


/** Check global channels for drones. */
void dronescan::checkChannels()
{
	log(INFO, "Checking channels for drones:");

	unsigned int noChannels = 0;

	theTimer->Start();

	xNetwork::constChannelIterator ptr = Network->channels_begin();
			
	for( ; ptr != Network->channels_end() ; ++ptr )
		{
		++noChannels;
		unsigned int abnormals;
		if((abnormals = isAbnormal(ptr->second)))
			{
			log(WARN, "  AC: %s (%d/%d)",
				ptr->second->getName().c_str(),
				abnormals,
				ptr->second->size()
				);
			}
		}
	
	log(INFO, "Finished checking %d channels. Duration: %d ms",
		noChannels,
		theTimer->stopTimeMS()
		);
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


/** Decide whether a channel is `abnormal'. */
unsigned int dronescan::isAbnormal( const Channel* theChannel)
{
	if(theChannel->size() < channelCutoff) return 0;
	
	unsigned int abnormals = 0;
				
	Channel::const_userIterator chanItr =
		theChannel->userList_begin();
	for( ; chanItr != theChannel->userList_end() ; ++chanItr )
		{
		ChannelUser* theCU = chanItr->second;
		iClient* theClient = theCU->getClient();
					
		if(theClient->isModeK())	return 0;
		
		if(!isNormal(theClient))	++abnormals;
		}

	if((double)abnormals / (double)theChannel->size() > channelMargin)
		return abnormals;
	else
		return 0;
}


/** Check whether an iClient's nick is `normal'. */
bool dronescan::isNormal( const iClient* theClient)
{
	/* We should never see this called during BURST */
	assert(currentState != BURST);
	
	clientData *theData = static_cast< clientData* > (theClient->getCustomData(this));
	
	/* We should never see an unknown state for an initialised client */
	assert(!theData->isUnknown());
	
	return theData->isNormal();
}


/** Decide whether a given string is `normal'. */
bool dronescan::isNormal( const string& theString )
{
	double theEntropy = calculateEntropy(theString);
	
	if(
	    theEntropy > averageEntropy * (1 + nickMargin) ||
	    theEntropy < averageEntropy * (1 - nickMargin)
	  )
		return false;
	
	return true;
}


/** Set the iClient's state depending on certain features. */
CLIENT_STATE dronescan::setClientState( iClient *theClient )
{
	clientData* theData = static_cast< clientData* > (theClient->getCustomData(this));
	
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
	if(!isNormal( theClient->getNickName() ))
		{
		return theData->setState(ABNORMAL);
		}
	
	/*
	 * It has passed the checks. It is therefore normal.
	 */
	return theData->setState(NORMAL);
}


/** Log a message. */
void dronescan::log(LOG_TYPE logType, char *format, ...)
{
	if(logType < INFO) return;

	char buffer[1024] = {0};
	va_list _list;
	
	va_start(_list, format);
	vsnprintf(buffer, 1024, format, _list);
	va_end(_list);
	
	Message(consoleChannel, buffer);
}


/** Set the topic of the console channel. */
void dronescan::setConsoleTopic()
{
	stringstream setTopic;
	setTopic	<< getCharYYXXX() << " T "
			<< consoleChannel << " :"
			<< "channelMargin: " << channelMargin
			<< "  nickMargin: " << nickMargin
			<< "  channelCutoff: " << channelCutoff
			;
	Write(setTopic);
}


/** Reply to an iClient. */
void dronescan::Reply(iClient *theClient, char *format, ...)
{
	char buffer[1024] = {0};
	va_list _list;
	
	va_start(_list, format);
	vsnprintf(buffer, 1024, format, _list);
	va_end(_list);
	
	Message(theClient, buffer);
}


} // namespace ds

} // namespace gnuworld
