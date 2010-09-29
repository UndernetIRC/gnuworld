/**
 * ccLog.h
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
 * $Id: ccLog.h,v 1.5 2003/06/28 01:21:19 dan_karrels Exp $
 */
 
#ifndef __CCLOG_H_
#define __CCLOG_H_

#include <string>
#include <fstream>
#include <sys/time.h>

using namespace std;

namespace gnuworld
{

namespace uworld
{

class ccLog
{

public:
	ccLog() : Time(0) , User() , Host(), CommandName() , Desc() {}
	
	ccLog(time_t _Time, string _User, string _Host 
	,string _CommName , string _Desc)
	: Time(_Time), User(_User) , Host(_Host)
	,CommandName(_CommName), Desc(_Desc)
	{}
	
	ccLog(fstream& in)
	{
		Load(in);
	}
	
	time_t Time;
	
	string User;
	
	string Host;
	
	string CommandName;
	
	string Desc;
	
	bool Save(fstream& out);
	
	bool Load(fstream& in);

	
};	

}

}

#endif 
