#ifndef __CSERVICE_H
#define __CSERVICE_H "$Id: cservice.h,v 1.58 2001/03/13 22:39:33 gte Exp $"

#include	<string>
#include	<vector>
#include	<hash_map>
#include	<map> 
#include	<ctime>

#include	"client.h"
#include	"iClient.h"
#include	"iServer.h"
#include	"EConfig.h"
#include	"cserviceCommands.h" 
#include	"sqlChannel.h"
#include	"sqlUser.h"
#include	"sqlLevel.h"
#include	"sqlBan.h"
#include	"libpq-int.h"
 
using std::string ;
using std::vector ;
using std::hash_map ;
using std::map ;

class PgDatabase; 

namespace gnuworld
{ 

/**
 *  Sublcass the postgres API to create our own accessor
 *  to get at the PID information.
 */ 
class Command;
 
class cmDatabase : public PgDatabase
{
public:
	cmDatabase(const char* conninfo)
	  : PgDatabase(conninfo) {}
	virtual ~cmDatabase() {}

	inline int getPID() const
		{ return pgConn->be_pid; }
};
 
class Command;
 
class cservice : public xClient
{
protected:
 
	EConfig* cserviceConfig; /* Configfile */
	typedef map< string, Command*, noCaseCompare > commandMapType ;
	typedef commandMapType::value_type pairType ;
	commandMapType          commandMap;

public:

	cmDatabase* SQLDb; /* PostgreSQL Database */

	cservice(const string& args);
	virtual ~cservice();

	virtual int OnConnect();
	virtual int BurstChannels();
	virtual int OnPrivateMessage( iClient*, const string&,
		bool = false  );
	virtual void ImplementServer( xServer* ) ;
	virtual bool isOnChannel( const string& ) const;
	virtual bool RegisterCommand( Command* ) ;
	virtual bool UnRegisterCommand( const string& ) ; 
	virtual void OnChannelModeO( Channel*, ChannelUser*,
		const xServer::opVectorType& ) ; 
	virtual int OnChannelEvent( const channelEventType& whichEvent,
		Channel* theChan,
		void* data1, void* data2, void* data3, void* data4 ); 
	virtual int OnEvent( const eventType&,
		void*, void*, void*, void*);
	virtual int OnCTCP( iClient* Sender,
                const string& CTCP,
                const string& Message,
                bool Secure = false ) ;
	virtual int OnTimer(xServer::timerID, void*);
	virtual int Notice( const iClient* Target,
		const char* Message, ... ) ; 
	virtual int Notice( const iClient* Target, const string& ) ;
	virtual int OnWhois( iClient* sourceClient,
			iClient* targetClient ); 

	// Sends a notice to a channel from the server.
	bool serverNotice( Channel*, const char*, ... );
	bool serverNotice( Channel*, const string& );

	/* Log an administrative alert to the relay channel & log. */
	bool logAdminMessage(const char*, ... );

	/* Log an debug message to the debug channel */
	bool logDebugMessage(const char*, ... ); 

	/* Write a channel log */
	void writeChannelLog(sqlChannel*, iClient*, unsigned short, const string&); 

	typedef commandMapType::const_iterator constCommandIterator ; 
	constCommandIterator command_begin() const
                { return commandMap.begin() ; } 

	constCommandIterator command_end() const
                { return commandMap.end() ; } 

	constCommandIterator findCommand( const string& theComm ) const
                { return commandMap.find( theComm ) ; } 
 
	/* Returns the access sqlUser has in channel sqlChan. */
	short getAccessLevel( sqlUser*, sqlChannel* );

	/* Returns the access sqlUser has in channel sqlChan taking into account
	 * suspensions, etc.
	 * If "bool" is true, then send sqlUser a notice about why they don't
	 * have a particular access. */
	short getEffectiveAccessLevel( sqlUser*, sqlChannel*, bool );

	/* Returns what admin access a user has. */
	short getAdminAccessLevel( sqlUser* );

	/* Fetch a user record for a user. */
	sqlUser* getUserRecord( const string& ); 

	/* Checks if this client is logged in, returns a sqlUser if true.
	 * If "bool" is true, send a notice to the client telling them off. */
	sqlUser* isAuthed(iClient*, bool );

	/* Checks to see if this users is forced on this channel */
	unsigned short isForced(sqlChannel*, sqlUser*);

	/* Fetch a channel record for a channel. */
	sqlChannel* getChannelRecord( const string& );
	sqlChannel* getChannelRecord( int );

	/* Fetch a access level record for a user/channel combo. */
	sqlLevel* getLevelRecord(sqlUser*, sqlChannel*);

	/* Fetch the user status flags. L = in cache, P = got password, U = Is authed. */
	string userStatusFlags( const string& );
	
	/* Formats a timestamp into a "X Days, XX:XX:XX" from 'Now'. */
	const string prettyDuration( int ) const ;

	/* Returns the current "Flood Points" this iClient has. */ 
 	unsigned short getFloodPoints(iClient*);

	/* Sets the flood counter for this iClient. */
 	void setFloodPoints(iClient*, unsigned short);

	/* Determins if a client is in "Flooded" state, and if so Notice them. */
	bool hasFlooded(iClient* theClient);

	/* Sets the timestamp for when we first recieved a msg from this client. 
	 * within the flood period. */
	void setLastRecieved(iClient*, time_t);

	/* Find out when we first heard from this chap. */ 
	time_t getLastRecieved(iClient*);

	void setOutputTotal(const iClient* theClient, unsigned int count);
	unsigned int getOutputTotal(const iClient* theClient);
	bool hasOutputFlooded(iClient* theClient);

	// Typedef's for user/channel Hashmaps.
	// User hash, Key is Username.
	typedef hash_map< string, sqlUser*, eHash, eqstr > sqlUserHashType ;

	// Channel hash, Key is channelname.
	typedef hash_map< string, sqlChannel*, eHash, eqstr > sqlChannelHashType ;
	typedef map< int, sqlChannel* > sqlChannelIDHashType ;

	// Accesslevel cache, key is pair(chanid, userid).
	typedef map < pair <int, int>, sqlLevel* > sqlLevelHashType ;

	// Ban cache, key is channel id.
	typedef vector < sqlBan* > sqlBanVectorType;
	typedef map < int, sqlBanVectorType* > sqlBanHashType ; 

	vector<sqlBan*>* getBanRecords(sqlChannel* theChan);

 	/* Silence List */
	typedef vector < pair < time_t, string > > silenceListType ;
	silenceListType silenceList;
 
	// Cache of user records.
	sqlUserHashType sqlUserCache;

	// Cache of channel records. 
	sqlChannelHashType sqlChannelCache;

	// Cache of channel records indexed by channel ID.
	sqlChannelIDHashType sqlChannelIDCache;

	// Cache of Level records.
	sqlLevelHashType sqlLevelCache;

	// Cache of Ban Records.
	sqlBanHashType sqlBanCache;
 
	// Some counters for statistical purposes.
	unsigned int userHits;
	unsigned int userCacheHits;
	unsigned int channelHits;
	unsigned int channelCacheHits; 			
	unsigned int levelHits;
	unsigned int levelCacheHits; 
	unsigned int banHits;
	unsigned int banCacheHits; 
	unsigned int dbErrors;

	/* No of seconds offset our local time is from server time. */
	int dbTimeOffset;

	// To keep track of how many custom data chunks
	// we have allocated.
	unsigned int customDataAlloc;

	// Flood/Notice relay channel - Loaded via config.
	string relayChan;
	string debugChan;
 
	// Loaded via config.
	// Interval at which we pick up updates from the Db.
	int updateInterval;

	// Interval at which we check for expired bans/suspends.
	int expireInterval;

	// Interval at which we attempt to purge the cache(s).
	int cacheInterval;

	/* Duration in seconds at which an idle user/chan/level/ban
	 * record should be purged from the cache. */
	int idleUserPeriod;
	int idleChannelPeriod;
	int idleLevelPeriod;

	// Input flood rate.
	unsigned int input_flood;
	unsigned int output_flood;
	int flood_duration;
	int topic_duration;

	// Timestamp's of when we last checked the database for updates.
	time_t lastChannelRefresh;
	time_t lastUserRefresh;
	time_t lastLevelRefresh;
	time_t lastBanRefresh;

	/* TimerID we recieve every second (eg: Reop, DB update). */
	xServer::timerID update_timerID;

	/* TimerID we recieve every XX seconds for expiration of bans/suspend. */
	xServer::timerID expire_timerID; 
 
	/* TimerID we recieve every XX hours for expiration of cached entries */
	xServer::timerID cache_timerID; 
 
	// Language translations table (Loaded from Db).
	typedef map < pair <int, int>, string > translationTableType ;
	translationTableType translationTable;
	
	void loadTranslationTable();

	// Method to retrieve a translation string.
	const string getResponse( sqlUser*, int , string = string() );
 
	// Check for valid hostmask.
	virtual bool validUserMask(const string& userMask) const ;

	/**
	 * Count channel ops.
	 */
	static size_t countChanOps(const Channel*);
	
	// Deop everyone on this channel.
	void deopAllOnChan(Channel*); 
	void deopAllUnAuthedOnChan(Channel*);

	/* sets a description (url) topic combo. */
	void doAutoTopic(sqlChannel* theChan);

	/* Bans & kicks a specified user with a specific reason */
	bool doInternalBanAndKick(sqlChannel*, iClient*, const string&);

	/* Support function to check if a user is banned on a channel. */
	sqlBan* isBannedOnChan(sqlChannel*, iClient*);

	/* Matches DB bans, and kicks supplied user if neccessary */
	bool checkBansOnJoin( Channel*, sqlChannel* , iClient* );

	time_t currentTime() const ;

	/* Queue to hold pending reops */ 
	typedef map < string, time_t > reopQType;
	reopQType reopQ;

	/*
	 *  Timer Functions.
	 *  These support functions are called at periodic
	 *  intervals to perform maintainence, etc. 
	 */

	/*
	 * Expire suspends, ignores and bans respectively.
	 */
	void expireSuspends();
	void expireSilence();
	void expireBans(); 

	/*
	 *  Cache expiration functions.
	 *  To expire idle user/level/ban records from the
	 *  cache. 
	 *  N.B: We'll never expire out channel records because
	 *  this information may be used to cancel 'unused'
	 *  channels.
	 */

	void cacheExpireUsers();

	/*
	 *  Expire Ban records, only if the channel
	 *  record is 'idle'.
	 */

	void cacheExpireBans();

	/*
	 *  Expire idle Level records.
	 */

	void cacheExpireLevels();
 
	/*
	 * Process any pending reop requests by the bot.
	 */
	void performReops();

	/*
	 * Process any Postgres notification requests,
	 * reloading cached records if neccessary.
	 */
	void processDBUpdates();

	/*
	 *  Send a generic Error Message, may log/etc at a later date.
	 */
	void dbErrorMessage(iClient*); 

	/*
	 *  Misc uncategorisable functions.
	 */
} ;

const string escapeSQLChars(const string& theString);
 
} // namespace gnuworld

#endif // __CSERVICE_H
