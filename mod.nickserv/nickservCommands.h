/**
 * nickservCommands.h
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
 * $Id: nickservCommands.h,v 1.9 2004/02/13 18:13:27 jeekay Exp $
 */

#ifndef _NICKSERVCOMMANDS_H
#define _NICKSERVCOMMANDS_H "$Id: nickservCommands.h,v 1.9 2004/02/13 18:13:27 jeekay Exp $"

/**
 * All this code is stolen straight from mod.cservice, which possibly had
 * stolen it from somewhere else. I'm increasingly convinced that only
 * ripper knows how any of this works.
 */

namespace gnuworld
{

class iClient;
class xServer;

namespace ns
{

class nickserv;

class Command {
  public:
    Command(nickserv* _bot, const string& _commName, const string& _help) :
      bot(_bot), server(0), commName(_commName), help(_help)
      {}
    
    virtual ~Command() {}
    
    virtual bool Exec(iClient*, const string&) = 0;
    
    void setServer(xServer* _server)
      { server = _server; }
    
    virtual string getInfo() const
      { return commName + ' ' + help; }
    
    virtual void Usage(iClient*);
    
    inline const string& getName() const
      { return commName; }
    
    inline const string& getHelp() const
      { return help; }
    
  protected:
    nickserv* bot;
    xServer*  server;
    string    commName;
    string    help;
}; // class Command

/* Big nasty #define time! */

#define DECLARE_COMMAND(commName) \
class commName##Command : public Command \
{ \
  public: \
    commName##Command(nickserv* _bot, \
      const string& _commName, \
      const string& _help) : \
        Command(_bot, _commName, _help) {} \
    virtual ~commName##Command() {} \
    virtual bool Exec(iClient*, const string&); \
};

/* Level 0 Commands */
DECLARE_COMMAND( RECOVER )
DECLARE_COMMAND( REGISTER )
DECLARE_COMMAND( RELEASE )
DECLARE_COMMAND( SET )
DECLARE_COMMAND( WHOAMI )

/* Admin commands */
DECLARE_COMMAND( INFO )
DECLARE_COMMAND( INVITE )
DECLARE_COMMAND( MODUSER )
DECLARE_COMMAND( SHUTDOWN )
DECLARE_COMMAND( STATS )

} // namespace ns

} // namespace gnuworld

#endif
