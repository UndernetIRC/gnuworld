
/*
 $Id: ccLog.h,v 1.4 2002/03/25 23:40:25 mrbean_ Exp $
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
