/* CControlCommands.h
 */

#ifndef __CCONTROLCOMMANDS_H
#define __CCONTROLCOMMANDS_H

#include	<string>

#include	"iClient.h"

using std::string ;

namespace gnuworld
{

namespace uworld
{

using gnuworld::xServer;

class ccontrol ;
class xServer ;

class Command
{

public:
	Command( ccontrol* _bot, const string& _commName,
		const string& _help, const unsigned long _flags,
		const bool _disabled, const bool _needOp,
		const bool _noLog,const int unsigned _minLevel
		, const bool _secondAccess)
	 : bot( _bot ),
	   server( 0 ),
	   commName( _commName ),
	   commRealName( _commName ),    	  
	   help( _help ),
	   flags ( _flags ),
	   isDisabled ( _disabled ),
	   needOp ( _needOp),
	   noLog ( _noLog ),
	   minLevel ( _minLevel ),
	   secondAccess ( _secondAccess )    	   	
	{}
	virtual ~Command() {}

	/// Exec returns true if the command was successfully
	/// executed, false otherwise.
	virtual bool Exec( iClient*, const string&) = 0 ;

	void	setServer( xServer* _server )
		{ server = _server ; }
	virtual string getInfo() const
		{ return commName + ' ' + help ; }
	virtual void Usage( iClient* theClient ) ;

	void	setName( const string& _name )
		{ commName = string_upper( _name ); }
	
	void	Disable()
		{ isDisabled = true ; }
	
	void 	Enable()
		{ isDisabled = false ; }
		
	void 	setNeedOp ( const bool _needOp )
		{ needOp = _needOp ; }

	void 	setNoLog ( const bool _noLog )
		{ noLog = _noLog ; }
		
	void	setMinLevel ( const int unsigned _minLevel )
		{ minLevel = _minLevel ; }
		
	inline const string& getName() const
		{ return commName ; }
	
	inline const string& getRealName() const
		{ return commRealName ; }
	
	inline const string& getHelp() const
		{ return help ; }

        inline const unsigned long getFlags() const
	        { return flags ; }	

	inline const bool getIsDisabled() const
		{ return isDisabled ; }

	inline const bool getNeedOp() const
		{ return needOp ; }

	inline const bool getNoLog() const
		{ return noLog ; }

	inline const unsigned int getMinLevel() const
		{ return minLevel ; }

	inline const bool getSecondAccess() const
		{ return secondAccess ; }				
protected:
	ccontrol*	bot ;
	xServer*	server ;
	string		commName ;
	string		commRealName ;
	string		help ;
	unsigned long   flags;
	bool		isDisabled;
	bool		needOp;
	bool		noLog;
	unsigned int	minLevel;
	bool		secondAccess;
} ;

#define DECLARE_COMMAND(commName) \
class commName##Command : public Command \
{ \
public: \
	commName##Command( ccontrol* _bot, \
		const string& _commName, \
		const string& _help,  \
	        int _flags , bool isDisabled, \
		bool needOp, bool noLog, \
		int minLevel , bool secondAccess ) \
	: Command( _bot, _commName, _help,_flags,isDisabled \
	,needOp,noLog,minLevel,secondAccess) \
	{} \
	virtual bool Exec( iClient*, const string&) ; \
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
DECLARE_COMMAND( CHANINFO )
// Added for levels
DECLARE_COMMAND( ACCESS )
DECLARE_COMMAND( LOGIN )
DECLARE_COMMAND( DEAUTH )
DECLARE_COMMAND( ADDUSER )
DECLARE_COMMAND( REMUSER )
DECLARE_COMMAND( ADDCOMMAND )
DECLARE_COMMAND( REMCOMMAND )
DECLARE_COMMAND( NEWPASS )
DECLARE_COMMAND( SUSPEND )
DECLARE_COMMAND( UNSUSPEND )
DECLARE_COMMAND( MODUSER )
DECLARE_COMMAND( MODERATE )
DECLARE_COMMAND( UNMODERATE )
DECLARE_COMMAND( OP )
DECLARE_COMMAND( DEOP )
DECLARE_COMMAND( LISTHOSTS )
DECLARE_COMMAND( CLEARCHAN )
DECLARE_COMMAND( ADDSERVER )
DECLARE_COMMAND( LEARNNET )
DECLARE_COMMAND( REMSERVER )
DECLARE_COMMAND( CHECKNET )
DECLARE_COMMAND( LASTCOM )
DECLARE_COMMAND( FORCEGLINE )
DECLARE_COMMAND( EXCEPTION )
DECLARE_COMMAND( LISTIGNORES )
DECLARE_COMMAND( REMOVEIGNORE )
DECLARE_COMMAND( LIST )
DECLARE_COMMAND( COMMANDS )

}

} // namespace gnuworld

#endif // __CCONTROLCOMMANDS_H
