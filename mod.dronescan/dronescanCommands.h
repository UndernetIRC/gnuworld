#ifndef DRONESCANCOMMANDS_H
#define DRONESCANCOMMANDS_H "$Id: dronescanCommands.h,v 1.2 2003/06/15 16:56:15 jeekay Exp $"

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

/* Debug commands */
DECLARE_COMMAND( CHECK )

} // namespace ds

} // namespace gnuworld

#endif
