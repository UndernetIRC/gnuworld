#ifndef __CSERVICECOMMANDS_H
#define __CSERVICECOMMANDS_H "$Id: cserviceCommands.h,v 1.15 2001/01/10 01:46:10 gte Exp $"

#include	<string>
#include	"iClient.h"
 
using std::string ;

namespace gnuworld
{ 

class cservice;
class xServer;
 
class Command
{

public:
        Command( cservice* _bot, const string& _commName,
                const string& _help, unsigned short _flood_points )
         : bot( _bot ),
           server( 0 ),
           commName( _commName ),
           help( _help ),
		   flood_points( _flood_points )
        {}
        virtual ~Command() {}

        /// Exec returns true if the command was successfully
        /// executed, false otherwise.
        virtual bool Exec( iClient*, const string& ) = 0 ;

        void    setServer( xServer* _server )
                { server = _server ; }
        virtual string getInfo() const
                { return commName + ' ' + help ; }
        virtual void Usage( iClient* theClient ) ;

        inline const string& getName() const
                { return commName ; }
        inline const string& getHelp() const
                { return help ; }
		inline const unsigned short& getFloodPoints() const
				{ return flood_points; }

protected:
        cservice*       bot ;
        xServer*        server ;
        string          commName ;
        string          help ;
		unsigned short  flood_points ;

} ;
 
#define DECLARE_COMMAND(commName) \
class commName##Command : public Command \
{ \
public: \
        commName##Command( cservice* _bot, \
                const string& _commName, \
                const string& _help, \
                unsigned short _flood_points) \
        : Command( _bot, _commName, _help, _flood_points ) \
        {} \
        virtual bool Exec( iClient*, const string& ) ; \
        virtual ~commName##Command() {} \
} ;

// Level 0 commands.
 
DECLARE_COMMAND( SHOWCOMMANDS )
DECLARE_COMMAND( LOGIN )
DECLARE_COMMAND( SEARCH )
DECLARE_COMMAND( ACCESS )
DECLARE_COMMAND( CHANINFO )
DECLARE_COMMAND( MOTD )
DECLARE_COMMAND( ISREG )
DECLARE_COMMAND( SHOWIGNORE )
DECLARE_COMMAND( VERIFY )
DECLARE_COMMAND( RANDOM )

// Channel user level commands.

DECLARE_COMMAND( OP )
DECLARE_COMMAND( VOICE )
DECLARE_COMMAND( DEOP )
DECLARE_COMMAND( DEVOICE ) 
DECLARE_COMMAND( ADDUSER )
DECLARE_COMMAND( REMUSER )
DECLARE_COMMAND( MODINFO )
DECLARE_COMMAND( SET ) 
DECLARE_COMMAND( INVITE )
DECLARE_COMMAND( TOPIC )
DECLARE_COMMAND( BANLIST )
DECLARE_COMMAND( KICK )
DECLARE_COMMAND( STATUS )

// Admin level commands.

DECLARE_COMMAND( REGISTER )
DECLARE_COMMAND( FORCE )
DECLARE_COMMAND( UNFORCE )
DECLARE_COMMAND( SERVNOTICE )
 
} // namespace gnuworld

#endif // __CSERVICECOMMANDS_H

