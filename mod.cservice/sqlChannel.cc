/**
 * sqlChannel.cc
 *
 * Storage class for accessing user information either from the backend
 * or internal storage.
 *
 * 20/12/2000: Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 * 30/12/2000: Moved static SQL data to constants.h --Gte
 * Set loadData up to take data from rows other than 0.
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
 * $Id: sqlChannel.cc,v 1.43 2007/08/28 16:10:12 dan_karrels Exp $
 */
#include	<sstream>
#include	<string>
#include	<iostream>
#include	<cstring>
#include	"ELog.h"
#include	"misc.h"
#include	"sqlChannel.h"
#include	"constants.h"
#include	"cservice.h"
#include	"cservice_config.h"
#include	"dbHandle.h"

namespace gnuworld
{
using std::string ;
using std::endl ;
using std::stringstream ;
using std::ends ;

chanFloodType::chanFloodType()
	:last_time((time_t)(0)),
	 msgCount(0),
	 noticeCount(0),
	 ctcpCount(0)
{
	 repCount = 1;
}

chanFloodType::~chanFloodType()
{
}

const sqlChannel::flagType sqlChannel::F_NOPURGE  = 0x00000001 ;
const sqlChannel::flagType sqlChannel::F_SPECIAL  = 0x00000002 ;
const sqlChannel::flagType sqlChannel::F_NOREG    = 0x00000004 ;
const sqlChannel::flagType sqlChannel::F_NEVREG   = 0x00000008 ;
const sqlChannel::flagType sqlChannel::F_SUSPEND  = 0x00000010 ;
const sqlChannel::flagType sqlChannel::F_TEMP     = 0x00000020 ;
const sqlChannel::flagType sqlChannel::F_CAUTION  = 0x00000040 ;
const sqlChannel::flagType sqlChannel::F_VACATION = 0x00000080 ;
const sqlChannel::flagType sqlChannel::F_LOCKED   = 0x00000100 ;
const sqlChannel::flagType sqlChannel::F_FLOATLIM = 0x00000200 ;
const sqlChannel::flagType sqlChannel::F_MIA      = 0x00000400 ;
const sqlChannel::flagType sqlChannel::F_JOINLIM  = 0x00000800 ;

const sqlChannel::flagType sqlChannel::F_ALWAYSOP  = 0x00010000 ;
const sqlChannel::flagType sqlChannel::F_STRICTOP  = 0x00020000 ;
const sqlChannel::flagType sqlChannel::F_NOOP      = 0x00040000 ;
const sqlChannel::flagType sqlChannel::F_AUTOTOPIC = 0x00080000 ;
const sqlChannel::flagType sqlChannel::F_OPONLY    = 0x00100000 ; // Deprecated
const sqlChannel::flagType sqlChannel::F_AUTOJOIN  = 0x00200000 ;
const sqlChannel::flagType sqlChannel::F_NOFORCE   = 0x00400000 ; // Reserved for use by Planetarion.
const sqlChannel::flagType sqlChannel::F_NOVOICE   = 0x00800000 ;
const sqlChannel::flagType sqlChannel::F_NOTAKE    = 0x01000000 ;
const sqlChannel::flagType sqlChannel::F_FLOODPRO  = 0x02000000 ;
const sqlChannel::flagType sqlChannel::F_FLOODPROGLINE = 0x04000000;
const sqlChannel::flagType sqlChannel::F_OPLOG = 0x08000000;

const int sqlChannel::EV_MISC     = 1 ;
const int sqlChannel::EV_JOIN     = 2 ;
const int sqlChannel::EV_PART     = 3 ;
const int sqlChannel::EV_OPERJOIN = 4 ;
const int sqlChannel::EV_OPERPART = 5 ;
const int sqlChannel::EV_FORCE    = 6 ;
const int sqlChannel::EV_REGISTER = 7 ;
const int sqlChannel::EV_PURGE    = 8 ;

/* Manually added Comment */
const int sqlChannel::EV_COMMENT 	= 9  ;
const int sqlChannel::EV_REMOVEALL	= 10 ;
const int sqlChannel::EV_IDLE		= 11 ;

/* other events */
const int sqlChannel::EV_MGRCHANGE	= 12 ;
const int sqlChannel::EV_ADMREJECT	= 13 ;
const int sqlChannel::EV_WITHDRAW	= 14 ;
const int sqlChannel::EV_NEWAPP		= 15 ;
const int sqlChannel::EV_NONSUPPORT	= 16 ;
const int sqlChannel::EV_ADMREVIEW	= 17 ;
const int sqlChannel::EV_CLRREVIEW	= 18 ;
const int sqlChannel::EV_SUSPEND	= 19 ;
const int sqlChannel::EV_UNSUSPEND	= 20 ;

sqlChannel::sqlChannel(cservice* _bot)
 : id(0),
   name(),
   flags(0),
   mass_deop_pro(3),
   flood_pro(0),
   msg_period(0),
   notice_period(0),
   ctcp_period(0),
   flood_period(0),
   repeat_count(0),
   floodlevel(FLOODPRO_KICK),
   man_floodlevel(FLOODPRO_KICK),
   url(),
   description(),
   comment(),
   keywords(),
   welcome(),
   registered_ts(0),
   channel_ts(0),
   channel_mode(),
   userflags(0),
   last_topic(0),
   inChan(false),
   last_used(0),
   limit_offset(3),
   limit_period(20),
   last_limit_check(0),
   last_flood(0),
   limit_grace(2),
   limit_max(0),
   limit_joinmax(0),
   limit_joinsecs(0),
   limit_joinmode(),
   limit_joinperiod(0),
   limit_jointimerID(0),
   max_bans(0),
   no_take(0),
   logger(_bot->getLogger()),
   SQLDb(_bot->SQLDb)
{
}


bool sqlChannel::loadData(const string& channelName)
{
/*
 *  With the open database handle 'SQLDb', retrieve information about
 *  'channelName' and fill our member variables.
 */

#ifdef LOG_DEBUG
	elog	<< "sqlChannel::loadData> Attempting to load data for"
		<< " channel-name: "
		<< channelName
		<< endl;
#endif

stringstream queryString ;
queryString	<< "SELECT "
		<< sql::channel_fields
		//<< " FROM channels WHERE registered_ts <> 0 AND lower(name) = '"
		<< " FROM channels WHERE lower(name) = '"
		<< escapeSQLChars(string_lower(channelName))
		<< "'"
		<< ends ;

if( SQLDb->Exec(queryString, true ) )
//if( PGRES_TUPLES_OK == status )
	{
	/*
	 *  If the channel doesn't exist, we won't get any rows back.
	 */

	if(SQLDb->Tuples() < 1)
		{
		return (false);
		}

	setAllMembers(0);
	return (true);
	}
return (false);
}

bool sqlChannel::loadData(int channelID)
{
/*
 *  With the open database handle 'SQLDb', retrieve information about
 *  'channelID' and fill our member variables.
 */

#ifdef LOG_DEBUG
	elog	<< "sqlChannel::loadData> Attempting to load data for "
		<< "channel-id: "
		<< channelID
		<< endl;
#endif

stringstream queryString;
queryString	<< "SELECT "
		<< sql::channel_fields
		//<< " FROM channels WHERE registered_ts <> 0 AND id = "
		<< " FROM channels WHERE id = "
		<< channelID
		<< ends ;

if( SQLDb->Exec(queryString, true ) )
//if( PGRES_TUPLES_OK == status )
	{
	/*
	 *  If the channel doesn't exist, we won't get any rows back.
	 */

	if(SQLDb->Tuples() < 1)
		{
		return (false);
		}

	setAllMembers(0);
	return (true);
	}

return (false);
}


void sqlChannel::setAllMembers(int row)
{
/*
 *  Support function for both loadData's.
 *  Assumes SQLDb contains a valid results set for all channel information.
 */

id = atoi(SQLDb->GetValue(row, 0).c_str());
name = SQLDb->GetValue(row, 1);
flags = atoi(SQLDb->GetValue(row, 2).c_str());
mass_deop_pro = atoi(SQLDb->GetValue(row,3).c_str());
flood_pro = atoi(SQLDb->GetValue(row,4).c_str());
url = SQLDb->GetValue(row,5);
description = SQLDb->GetValue(row,6);
comment = SQLDb->GetValue(row,7);
keywords = SQLDb->GetValue(row,8);
registered_ts = atoi(SQLDb->GetValue(row,9).c_str());
channel_ts = atoi(SQLDb->GetValue(row,10).c_str());
channel_mode = SQLDb->GetValue(row,11);
userflags = atoi(SQLDb->GetValue(row,12));
last_updated = atoi(SQLDb->GetValue(row,13));
limit_offset = atoi(SQLDb->GetValue(row,14));
limit_period = atoi(SQLDb->GetValue(row,15));
limit_grace = atoi(SQLDb->GetValue(row,16));
limit_max = atoi(SQLDb->GetValue(row,17));
max_bans = atoi(SQLDb->GetValue(row,18));
no_take = atoi(SQLDb->GetValue(row,19));
welcome = SQLDb->GetValue(row, 20);
limit_joinmax = atoi(SQLDb->GetValue(row,21));
limit_joinsecs = atoi(SQLDb->GetValue(row,22));
limit_joinperiod = atoi(SQLDb->GetValue(row,23));
limit_joinmode = SQLDb->GetValue(row,24);

setAllFlood();
}

bool sqlChannel::commit()
{
/*
 *  Build an SQL statement to commit the transient data in this storage class
 *  back into the database.
 */

static const char* queryHeader =    "UPDATE channels ";
static const char* queryCondition = "WHERE id = ";

stringstream queryString;
queryString	<< queryHeader
		<< "SET flags = " << flags << ", "
		<< "mass_deop_pro = " << mass_deop_pro << ", "
		<< "flood_pro = " << flood_pro << ", "
		<< "url = '" << escapeSQLChars(url) << "', "
		<< "keywords = '" << escapeSQLChars(keywords) << "', "
		<< "welcome = '" << escapeSQLChars(welcome) << "', "
		<< "registered_ts = " << registered_ts << ", "
		<< "channel_ts = " << channel_ts << ", "
		<< "channel_mode = '" << escapeSQLChars(channel_mode) << "', "
		<< "userflags = " << userflags << ", "
		<< "last_updated = date_part('epoch', CURRENT_TIMESTAMP)::int, "
		<< "limit_offset = " << limit_offset << ", "
		<< "limit_period = " << limit_period << ", "
		<< "limit_grace = " << limit_grace << ", "
		<< "limit_max = " << limit_max << ", "
		<< "max_bans = " << max_bans << ", "
		<< "description = '" << escapeSQLChars(description) << "', "
		<< "comment = '" << escapeSQLChars(comment) << "', "
		<< "no_take = " << no_take << ", "
		<< "limit_joinmax = " << limit_joinmax << ", "
		<< "limit_joinsecs = " << limit_joinsecs << ", "
		<< "limit_joinperiod = " << limit_joinperiod << ", "
		<< "limit_joinmode = '" << escapeSQLChars(limit_joinmode) << "' "
		<< queryCondition << id
		<< ends;

if( !SQLDb->Exec(queryString ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
	return false;
 	}

return true;
}

bool sqlChannel::insertRecord()
{
static const char* queryHeader = "INSERT INTO channels (name, flags, registered_ts, channel_ts, channel_mode, last_updated, no_take) VALUES (";

stringstream queryString;
queryString	<< queryHeader
			<< "'" << escapeSQLChars(name) << "', "
			<< flags << ", "
			<< registered_ts << ", "
			<< channel_ts << ", '"
			<< escapeSQLChars(channel_mode) << "', "
			<< "date_part('epoch', CURRENT_TIMESTAMP)::int,"
			<< no_take
			<< ")"
			<< ends;

if( !SQLDb->Exec(queryString ) )
	{
	LOGSQL_ERROR( SQLDb ) ;
	return false ;
 	}

return true;
}

void sqlChannel::setAllFlood()
{
	if (flood_pro == 0) return;
	unsigned int tmp = flood_pro;
	div_t divresult;
	divresult = div (tmp,0x100);
	msg_period = divresult.rem;
	tmp = divresult.quot;
	divresult = div (tmp, 0x10);
	notice_period = divresult.rem;
	tmp = divresult.quot;
	divresult = div (tmp, 0x10);
	ctcp_period = divresult.rem;
	tmp = divresult.quot;
	divresult = div (tmp, 0x10);
	flood_period = divresult.rem;
	tmp = divresult.quot;
	divresult = div (tmp, 0x10);
    repeat_count = divresult.rem;
}

void sqlChannel::setFloodMsg(const unsigned short& _floodMsg)
{
	msg_period = _floodMsg;
	div_t divresult;
	divresult = div (flood_pro,0x100);
	flood_pro = flood_pro - divresult.rem  + _floodMsg;
}

void sqlChannel::setFloodNotice(const unsigned short& _floodNotice)
{
	notice_period = _floodNotice;
	div_t div1,div2;
	div1 = div (flood_pro,0x100);
	int tmp = div1.quot;
	div2 = div (tmp,0x10);
	tmp = tmp - div2.rem + _floodNotice;
	flood_pro = tmp * 0x100 + div1.rem;
}

void sqlChannel::setFloodCTCP(const unsigned short& _floodCTCP)
{
	ctcp_period = _floodCTCP;
	div_t div1,div2;
	div1 = div (flood_pro,0x1000);
	int tmp = div1.quot;
	div2 = div (tmp,0x10);
	tmp = tmp - div2.rem + _floodCTCP;
	flood_pro = tmp * 0x1000 + div1.rem;
}

void sqlChannel::setFloodPeriod(const unsigned short& _floodPrd)
{
	flood_period = _floodPrd;
	div_t div1,div2;
	div1 = div (flood_pro,0x10000);
	int tmp = div1.quot;
	div2 = div (tmp,0x10);
	tmp = tmp - div2.rem + _floodPrd;
	flood_pro = tmp * 0x10000 + div1.rem;
}

void sqlChannel::setRepeatCount(const unsigned short& _repCount)
{
    repeat_count = _repCount;
	div_t div1,div2;
	div1 = div (flood_pro,0x100000);
	int tmp = div1.quot;
	div2 = div (tmp,0x10);
    tmp = tmp - div2.rem + _repCount;
	flood_pro = tmp * 0x100000 + div1.rem;
}

void sqlChannel::setDefaultFloodproValues()
{
	setFloodMsg(15);
	setFloodNotice(5);
	setFloodCTCP(5);
	setFloodPeriod(15);
	setRepeatCount(5);
	setFloodproLevel(sqlChannel::FLOODPRO_KICK);
	setManualFloodproLevel(sqlChannel::FLOODPRO_KICK);
}

unsigned int sqlChannel::getTotalMessageCount(const string& Mask)
{
	chanFloodMapType::iterator itr = chanFloodMap.find(Mask);
	chanFloodType* theChFlood = itr->second;
	if (itr != chanFloodMap.end())
		return theChFlood->getTotalMessageCount();
	return 0;
}

unsigned int sqlChannel::getTotalNoticeCount(const string& Mask)
{
	chanFloodMapType::iterator itr = chanFloodMap.find(Mask);
	chanFloodType* theChFlood = itr->second;
	if (itr != chanFloodMap.end())
		return theChFlood->getTotalNoticeCount();
	return 0;
}

unsigned int sqlChannel::getTotalCTCPCount(const string& Mask)
{
	chanFloodMapType::iterator itr = chanFloodMap.find(Mask);
	chanFloodType* theChFlood = itr->second;
	if (itr != chanFloodMap.end())
		return theChFlood->getTotalCTCPCount();
	return 0;
}

string sqlChannel::getFloodLevelName(const FloodProLevel& floodLevel)
{
	if (floodLevel == FLOODPRO_NONE) return "NONE";
	if (floodLevel == FLOODPRO_KICK) return "KICK";
	if (floodLevel == FLOODPRO_BAN) return "BAN";
	if (floodLevel == FLOODPRO_GLINE) return "GLINE";
	return "NONE";
}

sqlChannel::repeatMaskMapType sqlChannel::getRepeatMessageCount(const string& Message, string Mask)
{
	unsigned int sum = 0;
	repeatMaskMapType res;
	std::list < string > MaskList;
	chanFloodMapType::iterator ptr = chanFloodMap.begin();
	while (ptr != chanFloodMap.end())
	{
		bool incMask = false;
		chanFloodType* currChanFloodMap = ptr->second;
		// If a Mask is specified, means we want to count repetition referring to a single Mask
		if ((!Mask.empty()) && (Mask != ptr->first))
		{
			ptr++;
			continue;
		}
		if (currChanFloodMap->getLastMessage() == Message)
		{
			sum += currChanFloodMap->repCount;
			incMask = true;
		}
		if (incMask) MaskList.push_back(ptr->first);
		ptr++;
	}
	//if (MaskList.size() > 1) incFloodPro();
	res = std::make_pair(sum, MaskList);
	return res;
}

time_t sqlChannel::getMaskLastTime(const string& Mask)
{
	if (chanFloodMap.find(Mask) != chanFloodMap.end())
		return chanFloodMap[Mask]->getLastTime();
	return (time_t)(0);
}

//void sqlChannel::setMaskLastTime(const string& Mask)
//{
//	if (chanFloodMap.find(Mask) != chanFloodMap.end())
//		chanFloodMap[Mask]->setLastTime(now);
//}

void sqlChannel::RemoveFlooderMask(const string& Mask)
{
	if (chanFloodMap.find(Mask) != chanFloodMap.end())
	{
		//elog << "sqlChannel::RemoveFlooderMask> chanFloodMap.find(" << Mask << ") != chanFloodMap.end() ERASING" << endl;
		delete chanFloodMap[Mask];
		chanFloodMap.erase(Mask);
	}
}

void sqlChannel::handleNewMessage(const FloodType& floodtype, const string& Mask, const string& Message)
{
	chanFloodType* chanFlooder;
	chanFloodMapType::iterator itr = chanFloodMap.find(Mask);
	if (itr == chanFloodMap.end())
	{
		chanFlooder = new (std::nothrow) chanFloodType();
		assert(chanFlooder != 0);
		chanFlooder->setLastTime(now);
		chanFlooder->setLastMessage(Message);
		chanFlooder->repCount = 1;
		if (floodtype == FLOOD_MSG)
		{
			chanFlooder->messageFloodMap[Message].first = now;
			chanFlooder->messageFloodMap[Message].second = 1;
		}
		if (floodtype == FLOOD_NOTICE)
		{
			chanFlooder->noticeFloodMap[Message].first = now;
			chanFlooder->noticeFloodMap[Message].second = 1;
		}
		if (floodtype == FLOOD_CTCP)
		{
			chanFlooder->ctcpFloodMap[Message].first = now;
			chanFlooder->ctcpFloodMap[Message].second = 1;
		}
		chanFloodMap.insert(std::make_pair(Mask, chanFlooder));
	}
	else
	{
		//If expired ...
		chanFlooder = itr->second;
		if ((now - chanFlooder->getLastTime()) > (time_t)flood_period)
		{
			chanFlooder->messageFloodMap.clear();
			chanFlooder->noticeFloodMap.clear();
			chanFlooder->ctcpFloodMap.clear();
			chanFlooder->messageFloodMap[Message].second = 0;
			chanFlooder->noticeFloodMap[Message].second = 0;
			chanFlooder->ctcpFloodMap[Message].second = 0;
			chanFlooder->repCount = 0;
		}
		floodMessageType currMessageMap;
		if (floodtype == FLOOD_MSG)
		{
			currMessageMap = chanFlooder->messageFloodMap;
			if (currMessageMap.find(Message) != currMessageMap.end())
			{
				chanFlooder->messageFloodMap[Message].second++;
				if (chanFlooder->getLastMessage() == Message)
					chanFlooder->repCount++;
				else
					chanFlooder->repCount = 1;
			}
			else
			{
				chanFlooder->messageFloodMap[Message].second = 1;
				chanFlooder->repCount = 1;
			}
			chanFlooder->messageFloodMap[Message].first = now;
		}
		if (floodtype == FLOOD_NOTICE)
		{
			currMessageMap = chanFlooder->noticeFloodMap;
			if (currMessageMap.find(Message) != currMessageMap.end())
			{
				chanFlooder->noticeFloodMap[Message].second++;
				if (chanFlooder->getLastMessage() == Message)
					chanFlooder->repCount++;
				else
					chanFlooder->repCount = 1;
			}
			else
			{
				chanFlooder->noticeFloodMap[Message].second = 1;
				chanFlooder->repCount = 1;
			}
			chanFlooder->noticeFloodMap[Message].first = now;
		}
		if (floodtype == FLOOD_CTCP)
		{
			currMessageMap = chanFlooder->ctcpFloodMap;
			if (currMessageMap.find(Message) != currMessageMap.end())
			{
				chanFlooder->ctcpFloodMap[Message].second++;
				if (chanFlooder->getLastMessage() == Message)
					chanFlooder->repCount++;
				else
					chanFlooder->repCount = 1;
			}
			else
			{
				chanFlooder->ctcpFloodMap[Message].second = 1;
				chanFlooder->repCount = 1;
			}
			chanFlooder->ctcpFloodMap[Message].first = now;
		}
		chanFlooder->setLastTime(now);
		chanFlooder->setLastMessage(Message);
	}
	if (floodtype == FLOOD_MSG) calcTotalMessageCount(Mask);
	if (floodtype == FLOOD_NOTICE) calcTotalNoticeCount(Mask);
	if (floodtype == FLOOD_CTCP) calcTotalCTCPCount(Mask);
}

void sqlChannel::calcTotalMessageCount(const string& Mask)
{
	sqlChannel::chanFloodMapType::iterator ptr = chanFloodMap.find(Mask);
	if (ptr != chanFloodMap.end())
	{
		unsigned int sum = 0;
		chanFloodType* currChanFloodMap = ptr->second;
		if ((now - currChanFloodMap->getLastTime()) > (time_t)flood_period)
		{
			RemoveFlooderMask(Mask);
			return;
		}
		floodMessageType currMessageMap = currChanFloodMap->messageFloodMap;
		floodMessageType::iterator ptr2 = currMessageMap.begin();
		while (ptr2 != currMessageMap.end())
		{
			if ((now - ptr2->second.first) > (time_t)flood_period)
				currMessageMap.erase(ptr2++);
			else
			{
				sum += ptr2->second.second;
				ptr2++;
			}
		}
		currChanFloodMap->setTotalMessageCount(sum);
	}
}

void sqlChannel::calcTotalNoticeCount(const string& Mask)
{
	sqlChannel::chanFloodMapType::iterator ptr = chanFloodMap.find(Mask);
	if (ptr != chanFloodMap.end())
	{
		unsigned int sum = 0;
		chanFloodType* currChanFloodMap = ptr->second;
		if ((now - currChanFloodMap->getLastTime()) > (time_t)flood_period)
		{
			RemoveFlooderMask(Mask);
			return;
		}
		floodMessageType currMessageMap = currChanFloodMap->noticeFloodMap;
		floodMessageType::iterator ptr2 = currMessageMap.begin();
		while (ptr2 != currMessageMap.end())
		{
			if ((now - ptr2->second.first) > (time_t)flood_period)
				currMessageMap.erase(ptr2++);
			else
			{
				sum += ptr2->second.second;
				ptr2++;
			}
		}
		currChanFloodMap->setTotalNoticeCount(sum);
	}
}

void sqlChannel::calcTotalCTCPCount(const string& Mask)
{
	sqlChannel::chanFloodMapType::iterator ptr = chanFloodMap.find(Mask);
	if (ptr != chanFloodMap.end())
	{
		unsigned int sum = 0;
		chanFloodType* currChanFloodMap = ptr->second;
		if ((now - currChanFloodMap->getLastTime()) > (time_t)flood_period)
		{
			RemoveFlooderMask(Mask);
			return;
		}
		floodMessageType currMessageMap = currChanFloodMap->ctcpFloodMap;
		floodMessageType::iterator ptr2 = currMessageMap.begin();
		while (ptr2 != currMessageMap.end())
		{
			if ((now - ptr2->second.first) > (time_t)flood_period)
				currMessageMap.erase(ptr2++);
			else
			{
				sum += ptr2->second.second;
				ptr2++;
			}
		}
		currChanFloodMap->setTotalCTCPCount(sum);
	}
}

void sqlChannel::ExpireMessagesForChannel(sqlChannel* theChan)
{
	sqlChannel::chanFloodMapType::iterator itr = theChan->chanFloodMap.begin();
	while (itr != theChan->chanFloodMap.end())
	{
		time_t iplasttime = theChan->getMaskLastTime(itr->first);
		if (!iplasttime)
		{
			++itr;
			continue;
		}
		//theChan->ExpireMessagesForMask(itr->first, currentTime());
		time_t lastTime = now - iplasttime;
		if ((lastTime) > (time_t)theChan->getFloodPeriod())
		{
			theChan->RemoveFlooderMask(itr++->first);
		}
		else
			++itr;
	}
	return;
}

sqlChannel::~sqlChannel()
{
	/* TODO: Clean up bans */
	chanFloodMap.clear();
}
} // Namespace gnuworld
