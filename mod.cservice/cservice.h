#ifndef __CSERVICE_H
#define __CSERVICE_H "$Id: cservice.h,v 1.6 2000/12/21 22:20:57 gte Exp $"

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
	int getAccessLevel( sqlUser* theUser, sqlChannel* theChan );
} ;
 
} // namespace gnuworld

#endif // __CSERVICE_H
