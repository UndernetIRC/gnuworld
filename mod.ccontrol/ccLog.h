
/*
 $Id: ccLog.h,v 1.2 2002/02/01 11:14:04 mrbean_ Exp $
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

	const static char sep = '\t';

	const static short foundGood = 0;
	
	const static short foundEOF = 1;
	
	const static short foundBad = 2;
	
	static short findGood(ifstream& in);
	
	static short calcLen(short len)
		{ return ((((len & 0x3F80) << 2) 
		           | ((len & 0x7F) << 1)) | 0x101); }

	static short getLen(short len)
		{ return ((( len & 0xFE) >> 1) 
		  | ((len & 0xFE00) >> 2)); }
	
	static const char Seperator = '\0';
	
};	

}

}

#endif 