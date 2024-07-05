/**
 * cserviceCommands.h
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
 * $Id: cserviceCommands.h,v 1.37 2005/04/03 22:11:45 dan_karrels Exp $
 */

#ifndef __CSERVICECOMMANDS_H
#define __CSERVICECOMMANDS_H "$Id: cserviceCommands.h,v 1.37 2005/04/03 22:11:45 dan_karrels Exp $"

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
DECLARE_COMMAND( HELP )
DECLARE_COMMAND( ISREG )
DECLARE_COMMAND( SHOWIGNORE )
DECLARE_COMMAND( VERIFY )
DECLARE_COMMAND( RANDOM )
DECLARE_COMMAND( SUPPORT )
DECLARE_COMMAND( NOTE )

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
DECLARE_COMMAND( SUSPEND )
DECLARE_COMMAND( UNSUSPEND )
DECLARE_COMMAND( BAN )
DECLARE_COMMAND( UNBAN )
DECLARE_COMMAND( LBANLIST )
DECLARE_COMMAND( NEWPASS )
DECLARE_COMMAND( JOIN )
DECLARE_COMMAND( PART )
DECLARE_COMMAND( CLEARMODE )
DECLARE_COMMAND( SUSPENDME )
DECLARE_COMMAND( MODE )

// IRCop commands.

DECLARE_COMMAND( OPERJOIN )
DECLARE_COMMAND( OPERPART )

// Admin level commands.

DECLARE_COMMAND( WHITELIST )
DECLARE_COMMAND( SCANHOST )
DECLARE_COMMAND( SCANUNAME )
DECLARE_COMMAND( SCANEMAIL )
DECLARE_COMMAND( REGISTER )
DECLARE_COMMAND( REMOVEALL )
DECLARE_COMMAND( PURGE )
DECLARE_COMMAND( FORCE )
DECLARE_COMMAND( UNFORCE )
DECLARE_COMMAND( REMIGNORE )
DECLARE_COMMAND( SERVNOTICE )
DECLARE_COMMAND( SAY )
DECLARE_COMMAND( QUOTE )
DECLARE_COMMAND( REHASH )
DECLARE_COMMAND( STATS )
DECLARE_COMMAND( ADDCOMMENT )
DECLARE_COMMAND( SHUTDOWN )
DECLARE_COMMAND( HELLO )

} // namespace gnuworld

#endif // __CSERVICECOMMANDS_H
