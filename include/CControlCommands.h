/* CControlCommands.h
 */

#ifndef __CCONTROLCOMMANDS_H
#define __CCONTROLCOMMANDS_H

#include	<string>

#include	"iClient.h"
//#include	"ccontrol.h"

using std::string ;

namespace gnuworld
{

class ccontrol ;
class xServer ;

namespace ccontrolns
{

class Command
{

public:
	Command( ccontrol* _bot, const string& _commName,
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

	void	setServer( xServer* _server )
		{ server = _server ; }
	virtual string getInfo() const
		{ return commName + ' ' + help ; }
	virtual void Usage( iClient* theClient ) ;

	inline const string& getName() const
		{ return commName ; }
	inline const string& getHelp() const
		{ return help ; }

protected:
	ccontrol*	bot ;
	xServer*	server ;
	string		commName ;
	string		help ;

} ;

#define DECLARE_COMMAND(commName) \
class commName##Command : public Command \
{ \
public: \
	commName##Command( ccontrol* _bot, \
		const string& _commName, \
		const string& _help ) \
	: Command( _bot, _commName, _help ) \
	{} \
	virtual bool Exec( iClient*, const string& ) ; \
	virtual ~commName##Command() {} \
} ;

DECLARE_COMMAND( INVITE )
DECLARE_COMMAND( HELP )
DECLARE_COMMAND( JUPE )
DECLARE_COMMAND( MODE )
DECLARE_COMMAND( GLINE )
DECLARE_COMMAND( SCANGLINE )
DECLARE_COMMAND( REMGLINE )
DECLARE_COMMAND( TRANSLATE )
DECLARE_COMMAND( WHOIS )
DECLARE_COMMAND( KICK )
DECLARE_COMMAND( ADDOPERCHAN )
DECLARE_COMMAND( REMOPERCHAN )
DECLARE_COMMAND( LISTOPERCHANS )
DECLARE_COMMAND( ADDCHAN )
DECLARE_COMMAND( REMCHAN )
DECLARE_COMMAND( CHANINFO )

} // namespace gnuworld
} // namespace ccontrolns

#endif // __CCONTROLCOMMANDS_H
