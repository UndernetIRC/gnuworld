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
		const string& _help, const int _flags)
	 : bot( _bot ),
	   server( 0 ),
	   commName( _commName ),
	   help( _help ),
	   Flags ( _flags )
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

	inline const string& getName() const
		{ return commName ; }
	inline const string& getHelp() const
		{ return help ; }
        inline const int getFlags() const
	        { return Flags ; }	

protected:
	ccontrol*	bot ;
	xServer*	server ;
	string		commName ;
	string		help ;
	int             Flags;

} ;

#define DECLARE_COMMAND(commName) \
class commName##Command : public Command \
{ \
public: \
	commName##Command( ccontrol* _bot, \
		const string& _commName, \
		const string& _help,  \
	        int _flags ) \
	: Command( _bot, _commName, _help,_flags) \
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
DECLARE_COMMAND( ADDNEWOPER )
DECLARE_COMMAND( REMOVEOPER )
DECLARE_COMMAND( ADDCOMMAND )
DECLARE_COMMAND( REMOVECOMMAND )
DECLARE_COMMAND( NEWPASS )
DECLARE_COMMAND( SUSPENDOPER )
DECLARE_COMMAND( UNSUSPENDOPER )
DECLARE_COMMAND( MODOPER )
DECLARE_COMMAND( MODERATE )
DECLARE_COMMAND( UNMODERATE )
DECLARE_COMMAND( OP )
DECLARE_COMMAND( DEOP )
DECLARE_COMMAND( LISTHOSTS )
DECLARE_COMMAND( CLEARCHAN )
DECLARE_COMMAND( ADDNEWSERVER )
DECLARE_COMMAND( LEARNNETWORK )
DECLARE_COMMAND( REMOVESERVER )
DECLARE_COMMAND( CHECKNETWORK )
DECLARE_COMMAND( LASTCOM )
DECLARE_COMMAND( FORCEGLINE )
DECLARE_COMMAND( EXCEPTION )
DECLARE_COMMAND( LISTIGNORES )
DECLARE_COMMAND( REMOVEIGNORE )
DECLARE_COMMAND( LIST )
/*
 Patch for uworld commands level 
 Added by : |MrBean| (MrBean@toughguy.net)

 These are the commands flags 
 To enable a command for an oper he must have the command 
 flag set in his access 
 
 **NOTE** Commands with access 0 can be accessed without loginin first  
 

*/

const int flg_NOLOG = 0x80000000; //causes the bot not to log this command
const int flg_NEEDOP = 0x40000000; //needs to be operd to use the command

const int flg_ACCESS   = 0x01 | flg_NOLOG; 
const int flg_HELP     = 0x01 | flg_NOLOG;
const int flg_LOGIN    = 0x0 | flg_NOLOG | flg_NEEDOP;
const int flg_DEAUTH    = 0x01 | flg_NOLOG;

const int flg_NEWPASS  = 0x01 | flg_NOLOG;
const int flg_MODE     = 0x02;
const int flg_OP = 0x02;
const int flg_DEOP = 0x02;
const int flg_MODERATE = 0x02;
const int flg_UNMODERATE = 0x02;
const int flg_INVITE   = 0x04;
const int flg_JUPE     = 0x08;
const int flg_GLINE    = 0x10;
const int flg_SGLINE   = 0x10;
const int flg_REMGLINE = 0x10;
const int flg_REMOPCHN = 0x20;
const int flg_ADDOPCHN = 0x20;
const int flg_LOPCHN   = 0x20;
const int flg_CHINFO   = 0x40;
const int flg_WHOIS    = 0x80;
const int flg_ADDNOP   = 0x100;
const int flg_REMOP    = 0x100;
const int flg_MODOP    = 0x100;
const int flg_TRANS    = 0x200;
const int flg_KICK     = 0x400;
const int flg_ADDCMD   = 0x800; 
const int flg_DELCMD   = 0x800;
const int flg_SUSPEND  = 0x1000;
const int flg_UNSUSPEND  = 0x1000;
const int flg_CLEARCHAN = 0x2000;

const int flg_LISTHOSTS = 0x100;
const int flg_ADDSERVER = 0x4000;
const int flg_LEARNNET = 0x4000;
const int flg_REMSERVER = 0x4000;
const int flg_CHECKNET = 0x4000;
const int flg_LASTCOM = 0x4000 | flg_NOLOG;
const int flg_FGLINE = 0x8000;
const int flg_EXCEPTIONS = 0x10000;
const int flg_LISTIGNORES = 0x20000;
const int flg_REMIGNORE = 0x40000;
const int flg_LIST = 0x80000;

/*
 Default commands that are added upon adding a new oper
*/

const int OPER = flg_MODE | flg_INVITE | flg_GLINE | flg_CHINFO | flg_WHOIS | flg_TRANS | flg_KICK | flg_CLEARCHAN |flg_NEWPASS | flg_LIST & ~flg_NOLOG;
const int ADMIN = OPER | flg_ADDOPCHN | flg_ADDNOP | flg_ADDCMD | flg_SUSPEND | flg_JUPE | flg_ADDSERVER | flg_FGLINE | flg_EXCEPTIONS & ~flg_NOLOG;
const int SMT = ADMIN & ~flg_NOLOG;;
const int CODER = SMT & ~flg_NOLOG;;

//Oper flags 

const int isSUSPENDED = 0x01;
const int isOPER      = 0x02;
const int isADMIN     = 0x04;
const int isSMT     = 0x08;
const int isCODER     = 0x20;
const int getLOGS      = 0x10;

const int noACCESS = flg_NOLOG | flg_NEEDOP;

const int noFLAG = isSUSPENDED | getLOGS;

}

} // namespace gnuworld

#endif // __CCONTROLCOMMANDS_H
