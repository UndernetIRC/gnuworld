#ifndef _NICKSERVCOMMANDS_H
#define _NICKSERVCOMMANDS_H "$Id: nickservCommands.h,v 1.5 2002/08/25 22:38:49 jeekay Exp $"

/**
 * All this code is stolen straight from mod.cservice, which possibly had
 * stolen it from somewhere else. I'm increasingly convinced that only
 * ripper knows how any of this works.
 */

#include "iClient.h"
#include "server.h"

namespace gnuworld
{

//class iClient;
//class xServer;

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
DECLARE_COMMAND( SET )
DECLARE_COMMAND( STATS )
DECLARE_COMMAND( WHOAMI )

} // namespace ns

} // namespace gnuworld

#endif
