/**
 * chanfix.h
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
 * $Id: chanfix.h,v 1.10 2010/03/04 04:24:12 hidden1 Exp $
 */

#ifndef __CHANFIX_H
#define __CHANFIX_H "$Id: chanfix.h,v 1.10 2010/03/04 04:24:12 hidden1 Exp $"

#include	<string>
#include	<vector>
#include	<map>
#include	<list>
#include	<sstream>

#include	"client.h"
#include	"EConfig.h"
#include	"ELog.h"
#include	"Timer.h"

#include	"chanfixCommands.h"
#include	"chanfix_config.h"
#include	"sqlChannel.h"

/* This must be declared before sqlChanOp.h is #include'd */
namespace gnuworld
{
namespace cf
{
extern short currentDay;
}
}

#include	"sqlChanOp.h"
#include	"sqlManager.h"
#include	"sqlcfUser.h"

namespace gnuworld
{

class Timer;

namespace cf
{

class chanfix : public xClient {

public:

	/**
	 * Current network state.
	 */
	enum STATE {
		BURST,
		RUN,
		SPLIT,
		INIT
	};

	/**
	 * Constructor receives a configuration file name.
	 */
	chanfix( const std::string& ) ;

	/**
	 * Destructor does normal stuff.
	 */
	virtual ~chanfix() ;

	virtual void OnTimer(const gnuworld::xServer::timerID&, void*) ;

	/**
	 * This method is called when a network client sends
	 * a private message (PRIVMSG or NOTICE) to this xClient.
	 * The first argument is a pointer to the source client,
	 * and the second argument is the actual message (minus
	 * all of the server command stuff).
	 */
	virtual void OnPrivateMessage( iClient*, const std::string&,
		bool secure = false ) ;

	/**
	 * This method is called by the server when a server connection
	 * is established.  The purpose of this method is to inform
	 * the xServer of the channels this client wishes to burst.
	 */
	virtual void BurstChannels() ;

	/**
	 * This method is invoked when the server has been requested
	 * to shutdown.  If currently connected to the network, this
	 * method gives xClient's a chance to gracefully QUIT from
	 * the network, or whatever other processing is useful.
	 * To force data to be written before final shutdown (again,
	 * if connected), set xServer::FlushData().
	 * Timers will be executed after this method is invoked, once,
	 * depending upon target time of course :)
	 */
	virtual void OnShutdown( const std::string& reason ) ;

	/**
	 * This method is invoked when this module is first loaded.
	 * This is a good place to setup timers, connect to DB, etc.
	 * At this point, the server may not yet be connected to the
	 * network, so please do not issue join/nick requests.
	 */
	virtual void OnAttach() ;

	/**
	 * This method is called when this module is being unloaded from
	 * the server.  This is a good place to cleanup, including
	 * deallocating timers, closing connections, closing log files,
	 * and deallocating private data stored in iClients.
	 */
	virtual void OnDetach( const std::string& =
			std::string( "Shutting down" ) ) ;

	/**
	 * This method is called when the server connects to the network.
	 * Note that if this module is attached while already connected
	 * to a network, this method is still invoked.
	 */
	virtual void OnConnect() ;

	/**
	 * This method is invoked when the server disconnects from
	 * its uplink.
	 */
	virtual void OnDisconnect() ;

	/**
	 * This method will register a given command handler, removing
	 * (and deallocating) the existing handler for this command,
	 * should one exist.
	 */
	virtual bool RegisterCommand( Command* ) ;

	/**
	 * This method will unregister the command handler for the command
	 * of the given command name, deallocating the object from the
	 * heap as well.
	 */
	virtual bool UnRegisterCommand( const std::string& ) ;

	/**
	 * This method is invoked each time a channel event occurs
	 * for one of the channels for which this client has registered
	 * to receive channel events.
	 */
	virtual void	OnChannelEvent( const channelEventType&, Channel*,
		void* data1 = 0, void* data2 = 0,
		void* data3 = 0, void* data4 = 0 ) ;

	/**
	 * This method is invoked when a user sets or removes
	 * one or more channel mode (o).  Keep in mind that the
	 * source ChannelUser may be NULL if a server is
	 * setting the mode.
	 */
	virtual void OnChannelModeO( Channel*, ChannelUser*,
			const xServer::opVectorType& ) ;

	/**
	 * This method is invoked each time a network event occurs.
	 */
	virtual void	OnEvent( const eventType& theEvent,
		void* data1 = 0, void* data2 = 0,
		void* data3 = 0, void* data4 = 0 ) ;

	virtual void OnCTCP( iClient*, const std::string&, const std::string&, bool ) ;

	virtual void OnSignal( int sig ) ;

	/**
	 * Our functions.
	 */

	void readConfigFile(const std::string&);

	sqlChanOp* newChanOp(const std::string&, const std::string&);
	sqlChanOp* newChanOp(Channel*, iClient*);

	sqlChanOp* findChanOp(const std::string&, const std::string&);
	sqlChanOp* findChanOp(Channel*, iClient*);

	dbHandle* getLocalDBHandle() { return localDBHandle; }

	size_t countMyOps(const std::string&);
	size_t countMyOps(Channel*);

	sqlcfUser* isAuthed(const std::string);

	void precacheChanOps();
	void precacheChannels();
	void precacheUsers();

	void printResourceStats();

	void changeState(STATE);

	time_t currentTime() { return ::time(0); }
	
	time_t getSecsTilMidnight() { return 86400 - (currentTime() % 86400); }

	size_t countAutoFixes() { return autoFixQ.size(); }
	size_t countManFixes() { return manFixQ.size(); }

	void updatePoints();
	void giveAllOpsPoints();

	void givePoints(Channel*, iClient*);
	void gotOpped(Channel*, iClient*);

	bool hasIdent(iClient*);

	void JoinChan(Channel* theChan);
	void PartChan(Channel* theChan);

	void checkNetwork();
	void checkChannelServiceLink(iServer*, const eventType&);
	void findChannelService();
	const int getLastFix(sqlChannel*);

	void insertop(sqlChanOp*, sqlChannel*);
	bool findop(sqlChanOp*, sqlChannel*);
	void removechan(sqlChannel*);

	bool simFix(sqlChannel*, bool, time_t, iClient*, sqlcfUser*);
	bool simulateFix(sqlChannel*, bool, iClient*, sqlcfUser*);

	bool shouldCJoin(sqlChannel*, bool);

	void autoFix();
	void manualFix(Channel*);

	bool logLastComMessage(iClient*, const std::string&);
	bool msgTopOps(Channel*);

	bool fixChan(sqlChannel*, bool);
	void stopFixingChan(Channel*, bool);

	bool accountIsOnChan(const std::string&, const std::string&);

	sqlChannel* getChannelRecord(const std::string&);
	sqlChannel* getChannelRecord(Channel*);

	sqlChannel* newChannelRecord(const std::string&);
	sqlChannel* newChannelRecord(Channel*);

	bool deleteChannelRecord(sqlChannel*);

	static size_t countChanOps(const Channel*);

	bool needsModesRemoved(Channel*);

	bool canScoreChan(Channel*);

	void startTimers();

	void processQueue();
	
	void rotateDB();
	
	void expireTempBlocks();

	void prepareUpdate(bool);
	void updateDB();
#ifdef ENABLE_NEWSCORES
	int getNewScore(sqlChanOp*, time_t);
#endif

	bool isBeingFixed(Channel*);
	bool isBeingAutoFixed(Channel*);
	bool isBeingChanFixed(Channel*);

	bool isTempBlocked(const std::string&);

	bool removeFromAutoQ(Channel*);
	bool removeFromManQ(Channel*);

	char getFlagChar(const sqlcfUser::flagType&);
	const std::string getFlagsString(const sqlcfUser::flagType&);
	sqlcfUser::flagType getFlagType(const char);

	const std::string getEventName(const int);

	const std::string prettyDuration( int );

	const std::string tsToDateTime(time_t, bool);

	const std::string getHostList( sqlcfUser* );
	
	const std::string getChanNickName(const std::string&, const std::string&);

	const int getCurrentGMTHour(); /* returns the current hour in GMT (00-23) */

	char *convertToAscTime(time_t);

	/* Server notices */
	bool serverNotice( Channel*, const char*, ... );
	bool serverNotice( Channel*, const std::string& );

	/*
	 * Send private messages or notices to authenticated users
	 * By default, send notices
	 */
	void SendTo( iClient*, const char*, ... );
	void SendTo( iClient*, const std::string& );

	void SendFmtTo(iClient*, const std::string&);

	/* Admin message logs */
	bool logAdminMessage(const char*, ... );
	bool logDebugMessage(const char*, ... );

	void doSqlError(const std::string&, const std::string&);
	
	/**
	 * Our sqlManager instance for DB communication
	 */
	sqlManager* theManager;

	/**
	 * Commands map
	 */
	typedef std::map <std::string, Command*, noCaseCompare> commandMapType;
	commandMapType commandMap;

	/**
	 * ChannelOp map
	 */
	// map contents: channel string (account string, sqlChanOp object)
	typedef std::map <std::string, std::map <std::string, sqlChanOp*, noCaseCompare>, noCaseCompare> sqlChanOpsType;
	sqlChanOpsType		sqlChanOps;

	typedef std::map <std::string, sqlChannel*, noCaseCompare> sqlChannelCacheType;
	sqlChannelCacheType	sqlChanCache;

	typedef std::set <std::string> clientOpsType;
	clientOpsType*		findMyOps(iClient*);
	void 			lostOp(const std::string&, iClient*, clientOpsType*);

	typedef std::list <sqlChanOp*> chanOpsType;
	chanOpsType		getMyOps(Channel*);
	chanOpsType		getMyOps(const std::string&);
	chanOpsType     getMyOps(const std::string&, bool);
	
	typedef std::map <std::string, time_t, noCaseCompare> tempBlockType;
	tempBlockType		tempBlockList;
	
	typedef struct {
	    std::string account;
	    std::string channel;
	} simOppedStruct;

	typedef std::multimap<std::string, simOppedStruct> SimMapType;
	SimMapType		simMap;

	/**
	 * The snapshot map for updating the SQL database
	 */
	typedef struct {
	    std::string	account;
	    std::string	lastSeenAs;
	    time_t	firstOpped;
	    time_t	lastOpped;
	    short	day[DAYSAMPLES];
	} snapShotStruct;

	typedef std::multimap<std::string, snapShotStruct> DBMapType;
	DBMapType		snapShot;

	/**
	 * The db clients map
	 */
	typedef std::map <std::string, sqlcfUser*, noCaseCompare> usersMapType;

	/**
	 * Holds the authenticated user list
	 */
	usersMapType		usersMap;
	
	typedef usersMapType::iterator	usersIterator;

	usersIterator		usersMap_begin()
		{ return usersMap.begin(); }

	usersIterator		usersMap_end()
		{ return usersMap.end(); }

	/**
	 * Channels that chanfix should join
	 */
	typedef std::vector <std::string> joinChansType;
	joinChansType	chansToJoin;

	/**
	 * Queues to process.
	 */
	typedef std::map <std::string, time_t, noCaseCompare> fixQueueType;
	fixQueueType	autoFixQ;
	fixQueueType	manFixQ;

	typedef std::vector< iClient* > acctListType; //For reopping all logged in users to an acct.
	acctListType	findAccount(Channel*, const std::string&);

	typedef std::map < std::pair <int, std::string>, std::string > helpTableType;
	helpTableType	helpTable;

	void loadHelpTable();
	const std::string getHelpMessage(sqlcfUser*, std::string);

	typedef std::map < std::string, std::pair <int, std::string> > languageTableType;
	languageTableType	languageTable;

	typedef std::map < std::pair <int, int>, std::string > translationTableType ;
	translationTableType	translationTable;
	
	typedef std::map < std::string, std::list< iClient* >, noCaseCompare > authMapType;
	authMapType	authMap;

	void loadTranslationTable();

	const std::string getResponse(sqlcfUser*, int, std::string = std::string());

	/**
	 * Configuration variables
	 */
	std::string	consoleChan;
	std::string	consoleChanModes;
	bool		sendConsoleNotices;
	std::string	joinChanModes;
	bool		enableAutoFix;
	bool		enableChanFix;
	bool		enableChannelBlocking;
	bool		joinChannels;
	bool		autoFixNotice;
	bool		manualFixNotice;
	bool		stopAutoFixOnOp;
	bool		stopChanFixOnOp;
	bool		allowTopOpFix;
	bool		allowTopOpAlert;
	int		topOpPercent;
	int		minFixScore;
	int		minCanFixScore;
	int		minRequestOpTime;
	unsigned int	version;
	bool		useBurstToFix;
	unsigned int	nextFix;
	unsigned int	numServers;
	unsigned int	minServersPresent;
	std::string	chanServName;
	unsigned int	numTopScores;
	unsigned int	minClients;
	bool		clientNeedsIdent;
	unsigned int	connectCheckFreq;
	std::string	adminLogFile;
	std::string	debugLogFile;
	std::string	sqlHost;
	std::string	sqlPort;
	std::string	sqlcfUsername;
	std::string	sqlPass;
	std::string	sqlDB;

protected:
	/**
	 * Configuration file.
	 */
	EConfig*	chanfixConfig;

	/**
	 * State variable
	 */
	STATE		currentState;

	/**
	 * Channel service currently linked variable
	 */
	bool		chanServLinked;

	/**
	 * Update status variable
	 */
	bool		updateInProgress;

	/**
	 * Timer declarations
	 */
	xServer::timerID tidCheckDB;
	xServer::timerID tidAutoFix;
	xServer::timerID tidFixQ;
	xServer::timerID tidGivePoints;
	xServer::timerID tidRotateDB;
	xServer::timerID tidUpdateDB;
	xServer::timerID tidTempBlocks;

	/**
	 * Internal timer
	 */
	Timer *theTimer;

	/**
	 * Log-to-file streams
	 */
	std::ofstream	adminLog;
	std::ofstream	debugLog;

	/**
	 * DB Handle
	 */
	dbHandle*	localDBHandle;


public:

	/*
	 *  Methods to get data attributes.
	 */
	bool doAutoFix() { return enableAutoFix; }
	bool doChanFix() { return enableChanFix; }
	bool doChanBlocking() { return enableChannelBlocking; }
	bool doJoinChannels() { return joinChannels; }
	bool doAutoFixNotice() { return autoFixNotice; }
	bool doManualFixNotice() { return manualFixNotice; }
	STATE getState() { return currentState; }
	bool isChanServLinked() { return chanServLinked; }
	bool isUpdateRunning() { return updateInProgress; }
	bool isAllowingTopFix() { return allowTopOpFix; }
	bool isAllowingTopOpAlert() { return allowTopOpAlert; }
	unsigned int getTopOpPercent() { return topOpPercent; }
	unsigned int getMinFixScore() { return minFixScore; }
	unsigned int getMinCanFixScore() { return minCanFixScore; }
	unsigned int getMinRequestOpTime() { return minRequestOpTime; }
	unsigned int getNumServers() { return numServers; }
	unsigned int getMinServersPresent() { return minServersPresent; }
	unsigned int getNumTopScores() { return numTopScores; }
	unsigned int getMinClients() { return minClients; }
	unsigned int getNextFix() { return nextFix; }
	short getCurrentDay() { return currentDay; }

	/*
	 *  Methods to set data attributes.
	 */
	inline void	setNextFix(int _nextFix)
		{ nextFix = _nextFix; }
	inline void	setNumServers(int _numServers)
		{ numServers = _numServers; }
	inline void	setDoAutoFix(bool _enableAutoFix)
		{ enableAutoFix = _enableAutoFix; }
	inline void	setDoChanFix(bool _enableChanFix)
		{ enableChanFix = _enableChanFix; }
	inline void	setDoChanBlocking(bool _enableChannelBlocking)
		{ enableChannelBlocking = _enableChannelBlocking; }
	inline void	setCurrentDay()
		{ currentDay = currentTime() / 86400 % DAYSAMPLES; }

}; // class chanfix

const std::string escapeSQLChars(const std::string&);
bool atob(std::string);

} // namespace cf

} // namespace gnuworld

#endif // __CHANFIX_H
