/* CControlCommands.h
 */

#ifndef __CCONTROLCOMMANDS_H
#define __CCONTROLCOMMANDS_H

#include	<string>

#include	"iClient.h"
#include        "ccUser.h"

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
		const bool _noLog,const int _minLevel)
	 : bot( _bot ),
	   server( 0 ),
	   commName( _commName ),
	   commRealName( _commName ),    	  
	   help( _help ),
	   flags ( _flags ),
	   isDisabled ( _disabled ),
	   needOp ( _needOp),
	   noLog ( _noLog ),
	   minLevel ( _minLevel )    	   	
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
		
	void	setMinLevel ( const int _minLevel )
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

	inline const int getMinLevel() const
		{ return minLevel ; }
				
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
	int		minLevel;
} ;

#define DECLARE_COMMAND(commName) \
class commName##Command : public Command \
{ \
public: \
	commName##Command( ccontrol* _bot, \
		const string& _commName, \
		const string& _help,  \
	        int _flags , bool isDisabled, \
		bool needOp, bool noLog, int minLevel ) \
	: Command( _bot, _commName, _help,_flags,isDisabled \
	,needOp,noLog,minLevel) \
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
/*

 These are the commands flags 
 To enable a command for an oper he must have the command 
 flag set in his access 
 
 **NOTE** Commands with access 0 can be accessed without loginin first  
 
*/


const unsigned int flg_ACCESS   = 0x01;
const unsigned int flg_HELP     = 0x01;
const unsigned int flg_LOGIN    = 0x0; 
const unsigned int flg_DEAUTH    = 0x01;

const unsigned int flg_NEWPASS  = 0x01;
const unsigned int flg_MODE     = 0x02;
const unsigned int flg_OP = 0x02;
const unsigned int flg_DEOP = 0x02;
const unsigned int flg_MODERATE = 0x02;
const unsigned int flg_UNMODERATE = 0x02;
const unsigned int flg_INVITE   = 0x04;
const unsigned int flg_JUPE     = 0x08;
const unsigned int flg_GLINE    = 0x10;
const unsigned int flg_SGLINE   = 0x10;
const unsigned int flg_REMGLINE = 0x10;
const unsigned int flg_REMOPCHN = 0x20;
const unsigned int flg_ADDOPCHN = 0x20;
const unsigned int flg_LOPCHN   = 0x20;
const unsigned int flg_CHINFO   = 0x40;
const unsigned int flg_WHOIS    = 0x80;
const unsigned int flg_ADDNOP   = 0x100;
const unsigned int flg_REMOP    = 0x100;
const unsigned int flg_MODOP    = 0x100;
const unsigned int flg_TRANS    = 0x200;
const unsigned int flg_KICK     = 0x400;
const unsigned int flg_ADDCMD   = 0x800; 
const unsigned int flg_DELCMD   = 0x800;
const unsigned int flg_SUSPEND  = 0x1000;
const unsigned int flg_UNSUSPEND  = 0x1000;
const unsigned int flg_CLEARCHAN = 0x2000;

const unsigned int flg_LISTHOSTS = 0x100;
const unsigned int flg_ADDSERVER = 0x4000;
const unsigned int flg_LEARNNET = 0x4000;
const unsigned int flg_REMSERVER = 0x4000;
const unsigned int flg_CHECKNET = 0x4000;
const unsigned int flg_LASTCOM = 0x4000;
const unsigned int flg_FGLINE = 0x8000;
const unsigned int flg_EXCEPTIONS = 0x10000;
const unsigned int flg_LISTIGNORES = 0x20000;
const unsigned int flg_REMIGNORE = 0x40000;
const unsigned int flg_LIST = 0x80000;
const unsigned int flg_COMMANDS = 0x100000;



/*
 Default commands that are added upon adding a new oper
*/

const unsigned int OPER = flg_MODE | flg_INVITE | flg_GLINE | flg_CHINFO | flg_WHOIS | flg_TRANS | flg_KICK | flg_CLEARCHAN |flg_NEWPASS | flg_LIST;
const unsigned int ADMIN = OPER | flg_ADDOPCHN | flg_ADDNOP | flg_ADDCMD | flg_SUSPEND | flg_JUPE | flg_ADDSERVER | flg_FGLINE | flg_EXCEPTIONS;
const unsigned int SMT = ADMIN;
const unsigned int CODER = SMT | flg_COMMANDS;

//Oper flags 

//const int isOPER      = 0x02;
//const int isADMIN     = 0x04;
//const int isSMT     = 0x08;
//const int isCODER     = 0x20;

}

} // namespace gnuworld

#endif // __CCONTROLCOMMANDS_H
