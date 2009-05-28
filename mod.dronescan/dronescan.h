/**
 * dronescan.h
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

#ifndef DRONESCAN_H
#define DRONESCAN_H "$Id: dronescan.h,v 1.38 2009/05/28 10:37:31 hidden1 Exp $"

#include <map>
#include <string>

#include "client.h"

#include "clientData.h"
#include "jfChannel.h"
#include "dbHandle.h"
#include "glineData.h"

#ifdef ENABLE_LOG4CPLUS
#include <log4cplus/loglevel.h>
#include <log4cplus/configurator.h>
#include <iomanip>

#define JF_GLINED "gnuworld.ds.jf.glined"
#define JF_CSERVICE "gnuworld.ds.jf.cservice"

#endif
#define RecentlyGlinedIpsSize 150


namespace gnuworld {

class EConfig;
class Timer;

namespace ds {

class activeChannel;
class Command;
class sqlFakeClient;
class sqlUser;
class Test;

enum DS_STATE {
	BURST,
	RUN
};

enum LOG_TYPE {
	DBG,
	INFO,
	WARN,
	ERR
};

struct JCFLOODCLIENTS {
	std::list<std::string> log;
	std::list<std::string> chans;
	std::list<std::string> nicks;
    int count;
    int ctime;
};


class dronescan : public xClient {
public:
	/** Constructor receives a configuration file name. */
	dronescan( const std::string& ) ;

	/** Destructor to clean up after ourselves. */
	inline virtual ~dronescan() ;

	/***********************************************************
	 ** O V E R R I D E N   X C L I E N T   F U N C T I O N S **
	 ***********************************************************/

	/** This method is called when we have attached to the xServer. */
	virtual void OnAttach() ;

	/** This method is called after server connection. */
	virtual void BurstChannels() ;

	/** This is called when we receive a CTCP */
	virtual void OnCTCP( iClient*, const std::string&,
			const std::string&, bool ) ;

	/** Receive network events. */
	virtual void OnEvent( const eventType&, void*, void*, void*, void* ) ;

	/** Receive channel events. */
	virtual void OnChannelEvent( const channelEventType&, Channel*,
		void*, void*, void*, void* ) ;

	/** Receive private messages. */
	virtual void OnPrivateMessage( iClient*, const std::string&,
			bool ) ;

	/** When we are being detached by the xServer */
	virtual void OnDetach() ;

	/** Receive our own timed events. */
	virtual void OnTimer( const xServer::timerID& , void* ) ;

	/*****************************************
	 ** D R O N E S C A N   T Y P E D E F S **
	 *****************************************/

	typedef unsigned short int testEnabledType;

	typedef std::map< std::string , sqlUser* , noCaseCompare > 
			userMapType;
	typedef std::map< unsigned int , sqlFakeClient* > fcMapType;
	typedef std::map< std::string , unsigned int > clientsIPMapType;
	typedef struct JCFLOODCLIENTS jcFloodClients;
	typedef std::map< std::string , jcFloodClients* > clientsIPFloodMapType;
	typedef std::list< std::string > IPJQueueType;
	typedef std::list< glineData* > glineQueueType;
	typedef std::list< std::pair< std::string, int > > recentlyGlinedIpsType;
	
	/*******************************************
	 ** D R O N E S C A N   F U N C T I O N S **
	 *******************************************/

	/** Process the join / part floods */
	void processJoinPartChannels();
	
	/** Process the gline queue */
	void processGlineQueue();
	
	/** Report a SQL error to the appropriate places. */
	void doSqlError(const std::string&, const std::string&);

	/** Change the current state. */
	void changeState(DS_STATE) ;

	/** Update the state according to the current network state */
	void updateState();

	/** handles a channel join */
	void handleChannelJoin( Channel*, iClient* );

	/** handles a channel part */
	void handleChannelPart( Channel*, iClient* );
	
	/** This function handles new clients as they connect. */
	void handleNewClient( iClient* ) ;

	/** This function handles nick changes. */
	void handleNickChange( iClient* ) ;

	/** Calculate global entropy and store it. */
	void calculateEntropy() ;

	/** Set states on global nicks. */
	void setNickStates() ;

	/** Convenience function to reset states and check channels. */
	inline void resetAndCheck()
		{ setConsoleTopic(); setNickStates(); checkChannels(); }

	/**
	 * Check global channels for drones. This is a convenience wrapper
	 * for isAbnormal( Channel* ).
	 */
	void checkChannels() ;
	bool checkChannel( const Channel* , const iClient* = 0 ) ;

	/** Calculate the entropy of a given string. */
	double calculateEntropy( const std::string& ) ;

	/** Return the entropy of a given client. */
	double calculateEntropy( const iClient* ) ;

	/** Check if a channel is normal. */
	/** Check if an iClient's nick is normal. */
	bool isNormal( const iClient* ) ;

	/** Set a clientData's state depending on the iClient. */
	CLIENT_STATE setClientState( iClient* );

	/** Log a message. */
	void log(LOG_TYPE, const char*, ...) ;

	#ifdef ENABLE_LOG4CPLUS
	void log(char*,const char*, ...);
	#endif

	/** Set the topic of the console channel. */
	void setConsoleTopic() ;

	/** Reply to a given iClient. */
	void Reply(const iClient*, const char*, ...) ;

	/** Return a users access */
	sqlUser *getSqlUser( const std::string& ) ;

	/* Preloaders */
	bool updateDue(std::string);
	void preloadFakeClientCache();
	void preloadUserCache();
	bool preloadExceptionalChannels();
	
	/* Allow commands access to the database pointer */
	inline dbHandle *getSqlDb()
		{ return SQLDb; }

	inline const DS_STATE getCurrentState()
		{ return currentState; }
		
	inline int GetGlineQueueSize()
		{ return glineQueue.size(); }

	/** Internal variables */
	userMapType userMap;
	fcMapType fakeClients;
	clientsIPMapType clientsIPMap;
	clientsIPFloodMapType clientsIPFloodMap;
	int lastBurstTime;

	/** Typedef of currently seen drone channels */
	typedef std::map< std::string , activeChannel* > droneChannelsType;
	droneChannelsType droneChannels;

	/** Typedef of channels which are not checked for join/part floods */
	typedef std::list< std::string > exceptionalChannelsType;
	exceptionalChannelsType exceptionalChannels;
	
	/** The gline queue */
	glineQueueType glineQueue;
	IPJQueueType IPJQueue;
	recentlyGlinedIpsType recentlyGlinedIps;
	
	bool isExceptionalChannel(const std::string&);
	
	bool addExceptionalChannel(const std::string&);
	
	bool remExceptionalChannel(const std::string&);
	 
	/** Join counter config options */
	unsigned int jcInterval;
	unsigned int jcCutoff;
	unsigned int jcMinJoinToGline;
	unsigned int jcMinJoinToGlineJOnly;
	unsigned int jcMinJoinsPerIPToGline;
	unsigned int jcJoinsPerIPTime;
	unsigned int jcMinJFSizeToGline;
	unsigned int jcMinJFJOnlySizeToGline;
	bool jcGlineEnable;
	bool jcGlineEnableConf;
	std::string jcGlineReason;
	unsigned int jcGlineLength; 
	unsigned int pcCutoff;			
	unsigned int ncInterval;
	unsigned int ncCutoff;
	typedef std::map< std::string , jfChannel* , noCaseCompare >
		jcChanMapType;
	typedef jcChanMapType::const_iterator jcChanMapIterator;
	jcChanMapType jcChanMap;
	typedef std::map< std::string , unsigned int , noCaseCompare >
		ncChanMapType;
	ncChanMapType ncChanMap;
	
	/** Gline queue config options */
	unsigned int gbCount;
	unsigned int gbInterval;

	/* Test control */
	/** Test map type. */
	typedef std::map< std::string, Test* > testMapType;
	typedef testMapType::value_type testPairType;
	bool RegisterTest(Test*);
	bool UnRegisterTest(const std::string&);

	/** Tests map */
	testMapType testMap;

	/** Set a variable in one of the tests. */
	Test *setTestVariable(const std::string&, const std::string&);

	/* Configuration variables used by tests. */

	/** Global options */
	unsigned int voteCutoff;

	/** Abnormals options */
	double channelMargin;

protected:

	void outputNames(const std::string&,std::stringstream&,bool,bool);
	/** Configuration file. */
	EConfig *dronescanConfig;

	/** Our database instance */
	dbHandle *SQLDb;

	/** Configuration variables. */
	std::string consoleChannel;
	std::string consoleChannelModes;

	/** State variable. */
	DS_STATE currentState;

	/** Character occurance frequencies. */
	typedef std::map< char , double > charMapType;
	charMapType charMap;

	/** Average nickname entropy. */
	double averageEntropy;
	unsigned int totalNicks;

	/** Margins. */
	double nickMargin;
	unsigned int channelCutoff;

	/** How often to show drone channels on join test */
	unsigned int dcInterval;
	xServer::timerID tidClearActiveList;

	/** What level to log to the console. */
	int consoleLevel;

	/** Stats. */
	int customDataCounter;

	/** Internally used timer. */
	Timer *theTimer;

	/** Timers for GNUWorld triggered events. */
	xServer::timerID tidClearJoinCounter;
	xServer::timerID tidClearNickCounter;
	xServer::timerID tidRefreshCaches;
	xServer::timerID tidGlineQueue;

	/** Command map type. */
	typedef std::map< std::string , Command* , noCaseCompare >
		commandMapType;
	typedef commandMapType::value_type commandPairType;
	commandMapType commandMap;
	bool RegisterCommand(Command*);

	/** Time of the last cache. */
	std::map < std::string , time_t > lastUpdated;

	/** How often to refresh caches. */
	unsigned int rcInterval;

	/** Fake sqlUser record for opered clients without accounts. */
	sqlUser *fakeOperUser;
	
}; // class dronescan


} // namespace ds

} // namespace gnuworld

#endif
