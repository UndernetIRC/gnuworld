#ifndef __CSERVICE_H
#define __CSERVICE_H "$Id: cservice.h,v 1.16 2001/01/02 07:55:12 gte Exp $"

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
    virtual int OnPrivateMessage( iClient*, const string& );
    virtual void ImplementServer( xServer* ) ;
    virtual bool isOnChannel( const string& ) const;
    virtual bool RegisterCommand( Command* ) ;
    virtual bool UnRegisterCommand( const string& ) ; 
    virtual int OnCTCP( iClient* Sender,
                const string& CTCP,
                const string& Message,
                bool Secure = false ) ;
	virtual int OnTimer(xServer::timerID, void*);
    typedef commandMapType::const_iterator constCommandIterator ; 
    constCommandIterator command_begin() const
                { return commandMap.begin() ; } 

    constCommandIterator command_end() const
                { return commandMap.end() ; } 

    constCommandIterator findCommand( const string& theComm ) const
                { return commandMap.find( theComm ) ; } 
 
	// Return what access theUser has in channel theChan.
	short getAccessLevel( sqlUser* theUser, sqlChannel* theChan );

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
 
	// Typedef's for user/channel Hashmaps.
	typedef hash_map< string, sqlUser*, eHash, eqstr > sqlUserHashType ;
	typedef hash_map< string, sqlChannel*, eHash, eqstr > sqlChannelHashType ;
	typedef map < pair <int, int>, sqlLevel* > sqlLevelHashType ;

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

	// Flood/Notice relay channel - Loaded via config.
	string relayChan;

	// Internal at which we pick up updates from the Db.
	// Loaded via config.
	int updateInterval;

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
} ;
 
} // namespace gnuworld

#endif // __CSERVICE_H

