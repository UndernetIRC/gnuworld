#ifndef __CSERVICECOMMANDS_H
#define __CSERVICECOMMANDS_H "$Id: cserviceCommands.h,v 1.1 2000/12/11 00:49:31 gte Exp $"

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
                const string& _help )
         : bot( _bot ),
           server( 0 ),
           commName( _commName ),
           help( _help )
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

protected:
        cservice*         bot ;
        xServer*        server ;
        string          commName ;
        string          help ;

} ;
 
#define DECLARE_COMMAND(commName) \
class commName##Command : public Command \
{ \
public: \
        commName##Command( cservice* _bot, \
                const string& _commName, \
                const string& _help ) \
        : Command( _bot, _commName, _help ) \
        {} \
        virtual bool Exec( iClient*, const string& ) ; \
        virtual ~commName##Command() {} \
} ;

DECLARE_COMMAND( ACCESS )
 
} // namespace gnuworld

#endif // __CSERVICECOMMANDS_H

