#ifndef __CSERVICE_H
#define __CSERVICE_H "$Id: cservice.h,v 1.4 2000/12/11 00:46:31 gte Exp $"

#include	<string>
#include	<vector>

#include	<ctime>

#include	"client.h"
#include	"iClient.h"
#include	"iServer.h"
#include	"EConfig.h"
#include	"cserviceCommands.h"
 
#define STMT_ALLCHANS "SELECT channel from channels" // Provide a result set with 1 column, 'Channel Name'.

using std::string ;
using std::vector ;

class PgDatabase; 
 
namespace gnuworld
{ 

class Command;
 
class cservice : public xClient
{
protected:

    PgDatabase* SQLDb; /* PostgreSQL Database */
	EConfig* cserviceConfig; /* Configfile */
	typedef map< string, Command*, noCaseCompare > commandMapType ;
    typedef commandMapType::value_type pairType ;
    commandMapType          commandMap;

public:
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

} ;
 
} // namespace gnuworld

#endif // __CSERVICE_H
