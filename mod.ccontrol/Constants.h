#ifndef __CONSTANTS_H_
#define __CONSTANTS_H_

namespace gnuworld
{

namespace uworld
{

namespace channel
{

const unsigned int MaxName = 300;

}

namespace server
{

	const unsigned int MaxName = 63;
        static const char Query[] = "SELECT name,lastuplink,lastconnected,"
	"splitedon,lastnumeric,splitreason,version,addedon,lastupdated"
	" FROM servers ";

}

namespace User
{

	const unsigned int MaxName = 32;
	static const char Query[] = "SELECT user_id,user_name,password,access,saccess,flags,suspend_expires,suspended_by,server,isSuspended,IsUhs,IsOper,IsAdmin,IsSmt,IsCoder,GetLogs,NeedOp,Email,Suspend_Level,Suspend_Reason FROM opers";

}

namespace gline
        {

        const unsigned int GLINE_OK =0x01;
        const unsigned int FORCE_NEEDED_HOST = 0x02;
        const unsigned int FORCE_NEEDED_TIME = 0x04;
        const unsigned int FU_NEEDED_USERS = 0x08;
        const unsigned int HUH_NO_HOST = 0x10;
        const unsigned int HUH_NO_USERS = 0x20;
        const unsigned int BAD_HOST = 0x40;
        const unsigned int BAD_TIME = 0x80;
        const unsigned int FORCE_NEEDED_WILDTIME = 0x100;
        const unsigned int FU_NEEDED_TIME = 0x200;
        const unsigned int NEG_TIME = 0x400;
        const unsigned int MGLINE_TIME = 24*3600;
        const unsigned int MGLINE_WILD_TIME = 10*60;
        const unsigned int MFGLINE_USERS = 255;
        const unsigned int MFGLINE_TIME = 14*3600*24;
        const unsigned int PERM_TIME = 730*3600*24;
        const unsigned int MFU_TIME = 100*3600*24;

        }

namespace flood
{
	const unsigned int MESSAGE_POINTS = 4;
	const unsigned int CTCP_POINTS = 5;
	const unsigned int RESET_TIME = 5;
	const unsigned int FLOOD_POINTS = 20;
	const int IGNORE_TIME = 15*60;
}

namespace password
{
	const unsigned int MIN_SIZE = 5;
	const unsigned int TOO_SHORT = 1;
	const unsigned int LIKE_UNAME = 2;
	const unsigned int PASS_OK = 3;
	
}

}
}

#endif
