#ifndef __CSERVICE_H
#define __CSERVICE_H "$Id: cservice.h,v 1.8 2000/12/23 20:03:57 gte Exp $"

#include	<string>
#include	<vector>

#include	<ctime>

#include	"client.h"
#include	"iClient.h"
#include	"iServer.h"
#include	"EConfig.h"
#include	"cserviceCommands.h" 
#include	"sqlChannel.h"
#include	"sqlUser.h"
#include	"sqlLevel.h"
 
using std::string ;
using std::vector ;

class PgDatabase; 
 
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
    PgDatabase* SQLDb; /* PostgreSQL Database */

	cservice(const string& args);
	virtual ~cservice();

	virtual int OnConnect();
    virtual int BurstChannels();
	virtual int OnPrivateMessage( iClient*, const string& );
    virtual void ImplementServer( xServer* ) ;
    virtual bool RegisterCommand( Command* ) ;
    virtual bool UnRegisterCommand( const string& ) ; 
    typedef commandMapType::const_iterator constCommandIterator ; 
    constCommandIterator command_begin() const
                { return commandMap.begin() ; } 

    constCommandIterator command_end() const
                { return commandMap.end() ; } 

    constCommandIterator findCommand( const string& theComm ) const
                { return commandMap.find( theComm ) ; } 
 
	// Return what access theUser has in channel theChan.
	unsigned short getAccessLevel( sqlUser* theUser, sqlChannel* theChan );

	// Fetch a user record for a user.
	sqlUser* getUserRecord( const string& );

	// Checks if this client is logged in, returns a sqlUser if true.
	// If bool, send a notice to the client telling them off.
	sqlUser* isAuthed(iClient*, bool );

	// Fetch a channel record for a channel.
	sqlChannel* getChannelRecord(const string& );
} ;
 
} // namespace gnuworld

#endif // __CSERVICE_H
