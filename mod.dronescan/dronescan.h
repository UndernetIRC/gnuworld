#ifndef DRONESCAN_H
#define DRONESCAN_H "$Id: dronescan.h,v 1.3 2003/05/04 21:01:12 jeekay Exp $"

#include <map>

#include "client.h"

#include "clientData.h"
#include "Timer.h"

namespace gnuworld {

class EConfig;

namespace ds {

enum DS_STATE {
	BURST,
	RUN
};

enum LOG_TYPE {
	DEBUG,
	INFO,
	WARN,
	ERROR
};

/* The bit field values of the enabled tests */
#define TST_JOINCOUNT 0x0002

class dronescan : public xClient {
public:
	/** Constructor receives a configuration file name. */
	dronescan( const string& ) ;
	
	/** Destructor to clean up after ourselves. */
	inline virtual ~dronescan() { } ;
	
	/***********************************************************
	 ** O V E R R I D E N   X C L I E N T   F U N C T I O N S **
	 ***********************************************************/
	 
	/** This method is called when we have attached to the xServer. */
	virtual void ImplementServer( xServer* ) ;
	
	/** This method is called after server connection. */
	virtual int BurstChannels() ;
	
	/** Receive network events. */
	virtual int OnEvent( const eventType&, void*, void*, void*, void* ) ;
	
	/** Receive channel events. */
	virtual int OnChannelEvent( const channelEventType&, Channel*,
		void*, void*, void*, void* ) ;
	
	/** Receive private messages. */
	virtual int OnPrivateMessage( iClient*, const string&, bool ) ;
	
	/** Receive our own timed events. */
	virtual int OnTimer( xServer::timerID , void* ) ;
	
	
	/*****************************************
	 ** D R O N E S C A N   T Y P E D E F S **
	 *****************************************/
	
	typedef unsigned short int testEnabledType;
	
	
	/*******************************************
	 ** D R O N E S C A N   F U N C T I O N S **
	 *******************************************/
	
	/** Change the current state. */
	void changeState(DS_STATE) ;
	
	/** This function handles new clients as they connect. */
	void handleNewClient( iClient* ) ;
	
	/** Calculate global entropy and store it. */
	void calculateEntropy() ;
	
	/** Set states on global nicks. */
	void setNickStates() ;
	
	/** Convenience function to reset states and check channels. */
	inline void resetAndCheck()
		{ setConsoleTopic(); setNickStates(); checkChannels(); }
	
	/** Check global channels for drones. */
	void checkChannels() ;
	
	/** Calculate the entropy of a given string. */
	double calculateEntropy( const string& ) ;
	
	/** Check if a channel is normal. */
	unsigned int isAbnormal( const Channel* );
	/** Check if an iClient's nick is normal. */
	bool isNormal( const iClient* ) ;
	/** Check whether a string is normal. */
	bool isNormal( const string& ) ;
	
	/** Set a clientData's state depending on the iClient. */
	CLIENT_STATE setClientState( iClient* );
	
	/** Log a message. */
	void log(LOG_TYPE, char*, ...) ;
	
	/** Set the topic of the console channel. */
	void setConsoleTopic() ;
	
	/** Reply to a given iClient. */
	void Reply(iClient*, char*, ...);
	
	/** See if a given test is enabled. */
	inline bool testEnabled(testEnabledType theTest)
		{ return ((enabledTests & theTest) != 0); }
	
	 
protected:
	/** Configuration file. */
	EConfig *dronescanConfig;
	
	/** Configuration variables. */
	string consoleChannel;
	string consoleChannelModes;
	
	/** State variable. */
	DS_STATE currentState;
	
	/** Character occurance frequencies. */
	typedef map< char , double > charMapType;
	charMapType charMap;
	
	/** Average nickname entropy. */
	double averageEntropy;
	unsigned int totalNicks;
	
	/** What tests to enable. */
	testEnabledType enabledTests;
	
	/** Margins. */
	double channelMargin;
	double nickMargin;
	unsigned int channelCutoff;
	
	/** Join counter config options */
	unsigned int jcInterval;
	unsigned int jcCutoff;
	typedef map< string , unsigned int , noCaseCompare > jcChanMapType;
	jcChanMapType jcChanMap;
	
	/** Stats. */
	unsigned int customDataCounter;
	
	/** Internally used timer. */
	Timer *theTimer;
	
	/** Timers for GNUWorld triggered events. */
	xServer::timerID tidClearJoinCounter;
}; // class dronescan

} // namespace ds

} // namespace gnuworld

#endif
