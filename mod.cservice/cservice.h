#ifndef __CSERVICE_H
#define __CSERVICE_H "$Id: cservice.h,v 1.23 2001/01/13 18:54:18 gte Exp $"

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

/*
 *  Sublcass the postgres API to create our own accessor
 *  to get at the PID information.
 */

class cmDatabase : public PgDatabase {
public:
	cmDatabase(const char* conninfo) : PgDatabase(conninfo) {}
	inline int getPID()
		{ return pgConn->be_pid; }
};

namespace gnuworld
{ 
 
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
	virtual int OnEvent( const eventType&,
		void*, void*, void*, void*);
    virtual int OnCTCP( iClient* Sender,
                const string& CTCP,
                const string& Message,
                bool Secure = false ) ;
	virtual int OnTimer(xServer::timerID, void*);

	// Sends a notice to a channel from the server.
	bool serverNotice( Channel*, const char*, ... );
	bool serverNotice( Channel*, const string& );

	// Log an administrative alert to the relay channel & log.
	bool logAdminMessage(const char*, ... );

    typedef commandMapType::const_iterator constCommandIterator ; 
    constCommandIterator command_begin() const
                { return commandMap.begin() ; } 

    constCommandIterator command_end() const
                { return commandMap.end() ; } 

    constCommandIterator findCommand( const string& theComm ) const
                { return commandMap.find( theComm ) ; } 
 
	// Returns what access theUser has in channel theChan.
	short getAccessLevel( sqlUser* theUser, sqlChannel* theChan );

	// Returns what admin access a user has.
	short getAdminAccessLevel( sqlUser* );

	// Fetch a user record for a user.
	sqlUser* getUserRecord( const string& );

	// Checks if this client is logged in, returns a sqlUser if true.
	// If bool, send a notice to the client telling them off.
	sqlUser* isAuthed(iClient*, bool );

	// Fetch a channel record for a channel.
	sqlChannel* getChannelRecord( const string& );

	// Fetch a access level record for a user/channel combo.
	sqlLevel* getLevelRecord(sqlUser*, sqlChannel*);

	const string& prettyDuration( int );

	// Increments the flood counter for this iClient.
 	unsigned short getFloodPoints(iClient*);
 	void setFloodPoints(iClient*, unsigned short);
	bool hasFlooded(iClient* theClient);

	// Sets the timestamp for when we last recieved a msg from this client.
	void setLastRecieved(iClient*, time_t);
	time_t getLastRecieved(iClient*);

	void setIgnored(iClient*, bool);
	bool isIgnored(iClient*);

	// Typedef's for user/channel Hashmaps.
	// User hash, Key is Username.
	typedef hash_map< string, sqlUser*, eHash, eqstr > sqlUserHashType ;

	// Channel hash, Key is channelname.
	typedef hash_map< string, sqlChannel*, eHash, eqstr > sqlChannelHashType ;

	// Accesslevel cache, key is pair(chanid, userid).
	typedef map < pair <int, int>, sqlLevel* > sqlLevelHashType ;

	// Ban cache, key is channel id.
	typedef map < int, vector < sqlBan* > > sqlBanHashType ;

	//typedef priority_queue < unsigned int, vector< string >, less <unsigned int > > silenceListType;
	// Decided there aren't going to be that many silences anyway + we need iterative/random removal
	// access for show/remignore.

	typedef vector < string >  silenceListType;
	silenceListType silenceList;

	// Cache of user records.
	sqlUserHashType sqlUserCache;

	// Cache of channel records.
	sqlChannelHashType sqlChannelCache;

	// Cache of Level records.
	sqlLevelHashType sqlLevelCache;
 
	// Some counters for statistical purposes.
	unsigned int userHits;
	unsigned int userCacheHits;
	unsigned int channelHits;
	unsigned int channelCacheHits; 			
	unsigned int levelHits;
	unsigned int levelCacheHits; 

	// To keep track of how many custom data chunks
	// we have allocated.
	unsigned int customDataAlloc;

	// Flood/Notice relay channel - Loaded via config.
	string relayChan;
 
	// Loaded via config.
	// Internal at which we pick up updates from the Db.
	int updateInterval;

	// Input flood rate.
	int input_flood;
	int output_flood;
	int flood_duration;

	// Timestamp's of when we last checked the database for updates.
	time_t lastChannelRefresh;
	time_t lastUserRefresh;
	time_t lastLevelRefresh;
	time_t lastBanRefresh;

	// Language translations table (Loaded from Db).
	typedef map < pair <int, int>, string > translationTableType ;
	translationTableType translationTable;

	void loadTranslationTable();
	// Function to retrieve a translation string.
	const string& getResponse( sqlUser*, int );

	// Check for valid hostmask.
	virtual bool validUserMask(iClient* theClient, const string& userMask); 

	// Deop everyone on this channel.
	void cservice::deopAllOnChan(Channel*);
} ;
 
} // namespace gnuworld

#endif // __CSERVICE_H

