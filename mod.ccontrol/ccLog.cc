
/*

 */
 
#include "ccLog.h"
#include <string.h>
#include <fstream>
#include "ELog.h"
#include <unistd.h>
#include <string>
#include <cstring>

using namespace std;

namespace gnuworld
{

namespace uworld
{

const char ccLog::Seperator;

const short ccLog::foundGood;

const short ccLog::foundEOF;

const short ccLog::foundBad;
bool ccLog::Save(fstream& out)
{

if(out.bad())
	{
	return false;
	}

out.write(&Seperator,sizeof(Seperator));

if(out.bad())
	{
	return false;
	}
strstream tTime;
tTime << Time;
//char tTime[15];
//sprintf(tTime,"%d",Time);

short tmpLen = calcLen(strlen(tTime.str()));
out.write((char*)&tmpLen,sizeof(tmpLen));
out.write(tTime.str(),strlen(tTime.str()));
delete tTime.str();
if(out.bad())
	{
	return false;
	}
tmpLen = calcLen(CommandName.size());
out.write((char*)&tmpLen,sizeof(tmpLen));
out.write(CommandName.c_str(),CommandName.size());

if(out.bad())
	{
	elog << "Error writing name!\n";
	return false;
	}
tmpLen = calcLen(User.size());
out.write((char*)&tmpLen,sizeof(tmpLen));
out.write(User.c_str(),User.size());

if(out.bad())
	{
	return false;
	}
tmpLen = calcLen(Host.size());
out.write((char*)&tmpLen,sizeof(tmpLen));
out.write(Host.c_str(),Host.size());

if(out.bad())
	{
	return false;
	}
tmpLen = calcLen(Desc.size());
out.write((char*)&tmpLen,sizeof(tmpLen));
out.write(Desc.c_str(),Desc.size());
//out.close();
return true;

}

bool ccLog::Load(fstream& in)
{

if((in.bad()) || (in.eof()))
	{
	return false;
	}

char tmpChar;
in.read(&tmpChar,sizeof(tmpChar));
if(tmpChar != Seperator)
	{
	return false;
	}

if((in.bad()) || (in.eof()))
	{
	return false;
	}
short tmpLen; 
char *tchar;
in.read((char*)&tmpLen,sizeof(tmpLen));
tmpLen = getLen(tmpLen);
tchar = new char[tmpLen+1];
in.read(tchar,tmpLen);
tchar[tmpLen] = '\0';
Time = atoi(tchar);
delete tchar;
if((in.bad()) || (in.eof()))
	{
	return false;
	}


in.read((char*)&tmpLen,sizeof(tmpLen));
tmpLen = getLen(tmpLen);
tchar = new char[tmpLen+1];
in.read(tchar,tmpLen);
tchar[tmpLen] = '\0';
CommandName = tchar;
delete[] tchar;

if((in.bad()) || (in.eof()))
	{
	return false;
	}

in.read((char*)&tmpLen,sizeof(tmpLen));
tmpLen = getLen(tmpLen);
tchar = new char[tmpLen+1];
in.read(tchar,tmpLen);
tchar[tmpLen] = '\0';
User = tchar;
delete[] tchar;


if((in.bad()) || (in.eof()))
	{
	return false;
	}

in.read((char*)&tmpLen,sizeof(tmpLen));
tmpLen = getLen(tmpLen);
tchar = new char[tmpLen+1];
in.read(tchar,tmpLen);
tchar[tmpLen] = '\0';
Host = tchar;
delete[] tchar;

if((in.bad()) || (in.eof()))
	{
	return false;
	}

in.read((char*)&tmpLen,sizeof(tmpLen));
tmpLen = getLen(tmpLen);
tchar = new char[tmpLen+1];
in.read(tchar,tmpLen);
tchar[tmpLen] = '\0';
Desc = tchar;
delete[] tchar;

return true;

}

short ccLog::findGood(ifstream& in)
{

bool found = false;
char tchar;
while((!found) && !(in.eof()) && !(in.bad()))
	{
	in.read(&tchar,sizeof(tchar));
	if(tchar == Seperator)
		found = true;
	}
	
if(found)
	{
	in.seekg(-1,ios::cur);
	return foundGood;
	}	
if(in.eof())
	{
	return foundEOF;
	}
return foundBad;

}

}
}
