/**
 * dronescanCommands.h
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
 * $Id: dronescanCommands.h,v 1.4 2003/06/19 23:47:43 jeekay Exp $
 */

#ifndef DRONESCANCOMMANDS_H
#define DRONESCANCOMMANDS_H "$Id: dronescanCommands.h,v 1.4 2003/06/19 23:47:43 jeekay Exp $"

namespace gnuworld {

namespace ds {

class dronescan;

class Command {
public:
	Command( dronescan* _bot, const string& _commandName, const string& _help ) :
		bot(_bot), server(0), commandName(_commandName), help(_help)
		{ }

	virtual ~Command() { } ;
	
	virtual bool Exec(const iClient*, const string&) = 0;
	
	void setServer(xServer *_server)
		{ server = _server; }
	
	virtual inline string getInfo() const
		{ return commandName + ' ' + help; }
	
	virtual void Usage(const iClient*);
	
	inline const string& getName() const
		{ return commandName; }
	
	inline const string& getHelp() const
		{ return help; }
	
protected:
	dronescan	*bot;
	xServer		*server;
	string		commandName;
	string		help;

}; // class Command

/* Mmmmm we like big convoluted defines! */
#define DECLARE_COMMAND(commandName) \
class commandName##Command : public Command \
{ \
  public: \
    commandName##Command(dronescan* _bot, \
      const string& _commandName, \
      const string& _help) : \
        Command(_bot, _commandName, _help) {} \
    virtual ~commandName##Command() {} \
    virtual bool Exec(const iClient*, const string&); \
};

/* Admin commands */
DECLARE_COMMAND( ACCESS )
DECLARE_COMMAND( LIST )

/* Debug commands */
DECLARE_COMMAND( CHECK )

} // namespace ds

} // namespace gnuworld

#endif
