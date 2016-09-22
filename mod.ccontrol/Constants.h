/**
 * Constants.h
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
 * $Id: Constants.h,v 1.17 2009/07/25 18:12:34 hidden1 Exp $
 */

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
	"splitedon,lastnumeric,splitreason,version,addedon,lastupdated,reportmissing"
	" FROM servers ";
}

namespace User
{
const unsigned int MaxName = 32;
static const char Query[] = "SELECT user_id,user_name,password,access,saccess,flags,suspend_expires,suspended_by,server,isSuspended,IsUhs,IsOper,IsAdmin,IsSmt,IsCoder,GetLogs,NeedOp,Email,Suspend_Level,Suspend_Reason,notice,GetLag,LastPassChangeTS,Sso,Ssooo,AutoOp,Account,AccountTS FROM opers";
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
const unsigned int BAD_CIDRLEN = 0x800;
const unsigned int BAD_CIDRMASK = 0x1000;
const unsigned int BAD_CIDROVERRIDE = 0x2000;
const unsigned int HUH_IS_EXCEPTION = 0x4000;
const unsigned int HUH_IS_IP_OF_OPER = 0x8000;
const unsigned int MGLINE_TIME = 3*24*3600;
const unsigned int MGLINE_WILD_NOID_TIME = 7200;
const unsigned int MGLINE_WILD_TIME = 24*3600;
const unsigned int MFGLINE_USERS = 255;
const unsigned int MFGLINE_TIME = 14*3600*24;
const unsigned int PERM_TIME = 730*3600*24;
const unsigned int MFU_TIME = 100*3600*24;
const unsigned int NOLOGIN_TIME = 3600;	
const unsigned int MAX_REASON_LENGTH = 255;
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

namespace badChannels
{
static const char Query[] = "SELECT Name,Reason,AddedBy FROM BadChannels";
}

namespace scan
{
static const unsigned int MAX_SHOW = 15;
}

namespace exceptions
{
static const char Query[] = "SELECT Host,Connections,AddedBy,AddedOn,Reason FROM Exceptions WHERE lower(Host)='";
const unsigned int MAX_REASON = 449;
}

}
}

#endif
