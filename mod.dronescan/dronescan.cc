/*
 * dronescan.cc
 */

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
changeState(LEARN);
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
	theServer->RegisterEvent( EVT_NICK, this );
	theServer->RegisterEvent( EVT_KILL, this );
	theServer->RegisterEvent( EVT_QUIT, this );
	
	xClient::ImplementServer( theServer );
} // dronescan::ImplementServer(xServer*)


/**
 * Here we decide what channels our xClient needs to burst in to.
 * The only channel of any interest to us is our console channel, which is
 * loaded from the configuration file.
 */
int dronescan::BurstChannels()
{
/*	MyUplink->JoinChannel(this, consoleChannel,
		dronescanConfig->Require("consoleChannelModes")->second);
*/
	MyUplink->RegisterChannelEvent(consoleChannel, this);

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
			changeState(START);
			break;
			} // EVT_BURST_CMPLT
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
		Notice(theClient, "Allocated custom data: %d", customDataCounter);
		return 0;
		}
	
	return 0;
}



/*******************************************
 ** D R O N E S C A N   F U N C T I O N S **
 *******************************************/
 
/** This function allows us to change our current state. */
void dronescan::changeState(DS_STATE newState)
{
	/* First, do what we need to exit our current state */
	switch( currentState )
		{
		case LEARN :
			{
			/* First, normalise the character entropies */
			for( charMapType::iterator ptr = charMap.begin();
				ptr != charMap.end(); ++ptr)
				{
				ptr->second /= totalNicks;
				elog << ptr->first << ": " << ptr->second << endl;
				}
			
			elog	<< "*** DroneScan: Exiting state LEARN"
				<< endl;
			break;
			}
		case START :
			{
			elog	<< "*** DroneScan: Exiting state START"
				<< endl;
			break;
			}
		case RUN :
			{
			elog	<< "*** DroneScan: We are exiting state RUN!"
				<< endl;
			::exit(1);
			}
		}

	currentState = newState;
	
	switch( currentState )
		{
		case LEARN :
			{
			elog	<< "*** DroneScan: Entering state LEARN"
				<< endl;
			break;
			}
		case START :
			{
			elog	<< "*** DroneScan: Entering state START"
				<< endl;
			
			/* We now need to calculate the average entropy */
			double totalEntropy = 0;
			
			for( xNetwork::const_clientIterator ptr = Network->clients_begin()
			     ; ptr != Network->clients_end() ; ++ptr)
				{
				if( ptr->second->isModeK() ) continue;
				totalEntropy += calculateEntropy(ptr->second->getNickName());
				}
			
			elog << "Total entropy  : " << totalEntropy << endl;
			elog << "Total nicks    : " << totalNicks << endl;
			
			averageEntropy = totalEntropy / totalNicks;
			elog << "Average entropy: " << averageEntropy << endl;

			/* We need to assign ourselves a clientData */
			clientData *ourData = new clientData();
			ourData->setState(NORMAL);
			this->getInstance()->setCustomData(this, ourData);
			customDataCounter++;
			
			/* Now we must assign a state to each nick we see */
			for( xNetwork::const_clientIterator ptr = Network->clients_begin()
			     ; ptr != Network->clients_end() ; ++ptr)
				{
				setClientState( ptr->second );
				}

			changeState(RUN);
			
			break;
			}
		case RUN   : 
			{
			elog	<< "*** DroneScan: Entering state RUN"
				<< endl;
				
			/*
			 * Check for channels with >channelMargin abnormals.
			 * If the channel has <=5 members we ignore it.
			 * If the channel has a +k user in it, we ignore it (we assume
			 *    a registered channel is not used as a drone channel).
			 */
			xNetwork::constChannelIterator ptr =
				Network->channels_begin();
			
			for( ; ptr != Network->channels_end() ; ++ptr )
				{
				unsigned int abnormals;
				if((abnormals = isAbnormal(ptr->second)))
					{
					elog	<< "AC: "
						<< ptr->second->getName()
						<< " ("
						<< abnormals
						<< "/"
						<< ptr->second->size()
						<< ")"
						<< endl;
					}
				}
			break;
			}
		}
}
 

/** Here we handle new clients as they connect to the network. */
void dronescan::handleNewClient( iClient* theClient )
{
	/* First, all new clients must be assigned a clientData */
	clientData* theData = new clientData();
	assert(theClient->setCustomData(this, theData));
	++customDataCounter;


	/* If we are still bursting, calculate letter frequencies */
	if(currentState == LEARN)
		{
		/* First, learn the entropy from this nick */
		const char *ptr = theClient->getNickName().c_str();
		for( ; *ptr ; ++ptr)
			{
			charMap[*ptr]++;
			}
		++totalNicks;
		}
	else if(currentState == START)
		{
		/* We should never see new clients in this state */
		assert(0);
		}
	else if(currentState == RUN)
		{
		setClientState(theClient);
		}
}


/** Calculate the entropy of a given string. */
double dronescan::calculateEntropy( const string& theString )
{
	/* If we have not yet calibrated, we cannot calculate an entropy */
	assert(currentState != LEARN);

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
	/* We should never see this called during LEARN */
	assert(currentState != LEARN);
	
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

} // namespace ds

} // namespace gnuworld
