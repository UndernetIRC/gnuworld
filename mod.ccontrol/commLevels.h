/* commLevels.h
 */

#ifndef __COMMANDLEVELS_H
#define __COMMANDLEVELS_H

#include	<string>


using std::string ;

namespace gnuworld
{

namespace uworld
{

namespace commandLevel
{
/*

 These are the commands flags 
 To enable a command for an oper he must have the command 
 flag set in his access 
 
 **NOTE** Commands with access 0 can be accessed without loginin first  
 
*/


const unsigned long int flg_ACCESS   = 0x01;
const unsigned long int flg_HELP     = 0x02;
const unsigned int flg_LOGIN    = 0x0; 
const unsigned long int flg_DEAUTH    = 0x04;
const unsigned long int flg_NEWPASS  = 0x08;
const unsigned long int flg_MODE     = 0x10;
const unsigned long int flg_OP = 0x20;
const unsigned long int flg_DEOP = 0x40;
const unsigned long int flg_MODERATE = 0x80;
const unsigned long int flg_UNMODERATE = 0x100;
const unsigned long int flg_INVITE   = 0x200;
const unsigned long int flg_JUPE     = 0x400;
const unsigned long int flg_GLINE    = 0x800;
const unsigned long int flg_SGLINE   = 0x1000;
const unsigned long int flg_REMGLINE = 0x2000;
const unsigned long int flg_REMOPCHN = 0x4000;
const unsigned long int flg_ADDOPCHN = 0x8000;
const unsigned long int flg_LOPCHN   = 0x10000;
const unsigned long int flg_CHINFO   = 0x20000;
const unsigned long int flg_WHOIS    = 0x40000;
const unsigned long int flg_ADDNOP   = 0x80000;
const unsigned long int flg_REMOP    = 0x100000;
const unsigned long int flg_MODOP    = 0x200000;
const unsigned long int flg_TRANS    = 0x400000;
const unsigned long int flg_KICK     = 0x800000;
const unsigned long int flg_ADDCMD   = 0x1000000; 
const unsigned long int flg_DELCMD   = 0x2000000;
const unsigned long int flg_SUSPEND  = 0x4000000;
const unsigned long int flg_UNSUSPEND = 0x8000000;
const unsigned long int flg_CLEARCHAN = 0x10000000;

const unsigned long int flg_LISTHOSTS = 0x20000000;
const unsigned long int flg_ADDSERVER = 0x40000000;
const unsigned long int flg_LEARNNET = 0x1;
const unsigned long int flg_REMSERVER = 0x2; //From here saccess must be toggled
const unsigned long int flg_CHECKNET = 0x4;
const unsigned long int flg_LASTCOM = 0x8;
const unsigned long int flg_FGLINE = 0x10;
const unsigned long int flg_EXCEPTIONS = 0x20;
const unsigned long int flg_LISTIGNORES = 0x40;
const unsigned long int flg_REMIGNORE = 0x80;
const unsigned long int flg_LIST 	 = 0x100;
const unsigned long int flg_COMMANDS  = 0x200;
const unsigned long int flg_NOTES = 0x400;
const unsigned long int flg_GCHAN = 0x800;
const unsigned long int flg_RGCHAN = 0x1000;
const unsigned long int flg_USERINFO = 0x2000;


/*
 Default commands that are added upon adding a new oper
*/

const unsigned long int OPER = flg_ACCESS | flg_HELP | flg_DEAUTH | flg_NEWPASS 
			| flg_MODE | flg_OP | flg_DEOP | flg_MODERATE 
		        | flg_UNMODERATE | flg_INVITE | flg_GLINE | flg_SGLINE 
			| flg_REMGLINE | flg_LOPCHN | flg_CHINFO | flg_WHOIS 
			| flg_TRANS | flg_KICK | flg_CLEARCHAN | flg_LISTHOSTS;
const unsigned long int SOPER = flg_CHECKNET | flg_LISTIGNORES | flg_LIST 
			| flg_NOTES | flg_USERINFO;
const unsigned long int ADMIN = OPER | flg_JUPE | flg_REMOPCHN | flg_ADDOPCHN 
			| flg_ADDNOP | flg_REMOP |flg_MODOP | flg_ADDCMD 
			| flg_DELCMD | flg_SUSPEND | flg_UNSUSPEND;
const unsigned long int SADMIN = SOPER | flg_LEARNNET | flg_LASTCOM 
			| flg_FGLINE | flg_REMIGNORE;
const unsigned long int SMT = ADMIN | flg_ADDSERVER;
const unsigned long int SSMT = SADMIN | flg_REMSERVER;
const unsigned long int CODER = SMT;
const unsigned long int SCODER =  SSMT | flg_COMMANDS| flg_GCHAN | flg_RGCHAN
			| flg_EXCEPTIONS;

}

}
}
#endif // __COMMANDLEVELS_H
