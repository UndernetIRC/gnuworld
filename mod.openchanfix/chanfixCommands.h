/**
 * chanfixCommands.h
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
 * $Id: chanfixCommands.h,v 1.5 2006/12/22 03:00:02 buzlip01 Exp $
 */

#ifndef __CHANFIXCOMMANDS_H
#define __CHANFIXCOMMANDS_H "$Id: chanfixCommands.h,v 1.5 2006/12/22 03:00:02 buzlip01 Exp $"

namespace gnuworld {

namespace cf {

class chanfix;
class sqlcfUser;

class Command {
public:
	Command( chanfix* _bot, const std::string& _commandName, const std::string& _help, std::string::size_type _numParams, unsigned short int _requiredFlags ) :
		bot(_bot), server(0), commandName(_commandName), help(_help),
		numParams(_numParams), requiredFlags(_requiredFlags)
		{ }

	virtual ~Command() { }
	
	virtual void Exec(iClient*, sqlcfUser*, const std::string&) = 0;
	
	void setServer(xServer *_server)
		{ server = _server; }
	
	virtual inline std::string getInfo() const
		{ return commandName + ' ' + help; }
	
	virtual void Usage(iClient*);
	
	inline const std::string& getName() const
		{ return commandName; }
	
	inline const std::string& getHelp() const
		{ return help; }

	inline const std::string::size_type getNumParams() const
		{ return numParams; }

	inline const unsigned short int getRequiredFlags() const
		{ return requiredFlags; }

protected:
	chanfix				*bot;
	xServer				*server;
	std::string			commandName;
	std::string			help;
	std::string::size_type		numParams;
	unsigned short int		requiredFlags;

};

#define DECLARE_COMMAND(commandName) \
class commandName##Command : public Command \
{ \
  public: \
    commandName##Command(chanfix* _bot, \
      const std::string& _commandName, \
      const std::string& _help, \
      std::string::size_type _numParams, \
      unsigned short int _requiredFlags) : \
	Command(_bot, _commandName, _help, _numParams, _requiredFlags) {} \
    virtual ~commandName##Command() {} \
    virtual void Exec(iClient*, sqlcfUser*, const std::string&); \
};

/* Normal user commands */
DECLARE_COMMAND( CANFIX )
DECLARE_COMMAND( REQUESTOP )

/* Oper commands */
DECLARE_COMMAND( HELP )
DECLARE_COMMAND( HISTORY )
DECLARE_COMMAND( INFO )
DECLARE_COMMAND( SCORE )
DECLARE_COMMAND( STATUS )

/* Logged-in commands */
DECLARE_COMMAND( CHECK )
DECLARE_COMMAND( LISTHOSTS )
DECLARE_COMMAND( USERSCORES )
DECLARE_COMMAND( USET )
DECLARE_COMMAND( WHOIS )
DECLARE_COMMAND( WHOFLAG )

/* User management commands */
DECLARE_COMMAND( ADDFLAG )
DECLARE_COMMAND( ADDHOST )
DECLARE_COMMAND( ADDUSER )
DECLARE_COMMAND( DELFLAG )
DECLARE_COMMAND( DELHOST )
DECLARE_COMMAND( DELUSER )
DECLARE_COMMAND( SETGROUP )
DECLARE_COMMAND( SUSPEND )
DECLARE_COMMAND( UNSUSPEND )
DECLARE_COMMAND( WHOGROUP )

/* Block commands */
DECLARE_COMMAND( BLOCK )
DECLARE_COMMAND( UNBLOCK )
DECLARE_COMMAND( LISTBLOCKED )
DECLARE_COMMAND( TEMPBLOCK )
DECLARE_COMMAND( UNTEMPBLOCK )
DECLARE_COMMAND( LISTTEMPBLOCKED )

/* Comment commands */
DECLARE_COMMAND( ADDNOTE )
DECLARE_COMMAND( ALERT )
DECLARE_COMMAND( DELNOTE )
DECLARE_COMMAND( UNALERT )

/* Chanfix commands */
DECLARE_COMMAND( CHANFIX )
DECLARE_COMMAND( OPLIST )
DECLARE_COMMAND( OPNICKS )
DECLARE_COMMAND( SIMULATE )

/* Owner commands */
DECLARE_COMMAND( DEBUG )
DECLARE_COMMAND( INVITE )
DECLARE_COMMAND( LASTCOM )
DECLARE_COMMAND( QUOTE )
DECLARE_COMMAND( REHASH )
DECLARE_COMMAND( RELOAD )
DECLARE_COMMAND( SAY )
DECLARE_COMMAND( SET )
DECLARE_COMMAND( SHUTDOWN )

} // namespace cf

} // namespace gnuworld

#endif
