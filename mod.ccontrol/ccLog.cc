
/*

 */
 
#include "ccLog.h"
#include <string>
#include <fstream>

using namespace std;

namespace gnuworld
{

namespace uworld
{

bool ccLog::Save(fstream& out)
{

if((!out) || (out.bad()))
	{
	return false;
	}

out.write((char*)&Time,sizeof(Time));

if((!out) || (out.bad()))
	{
	return false;
	}
int tmpLen = CommandName.size();
out.write((char*)&tmpLen,sizeof(int));
out.write(CommandName.c_str(),tmpLen);

if((!out) || (out.bad()))
	{
	return false;
	}
tmpLen = User.size();
out.write((char*)&tmpLen,sizeof(int));
out.write(User.c_str(),tmpLen);

if((!out) || (out.bad()))
	{
	return false;
	}
tmpLen = Host.size();
out.write((char*)&tmpLen,sizeof(int));
out.write(Host.c_str(),tmpLen);

if((!out) || (out.bad()))
	{
	return false;
	}
tmpLen = Desc.size();
out.write((char*)&tmpLen,sizeof(int));
out.write(Desc.c_str(),tmpLen);

return true;

}

bool ccLog::Load(fstream& in)
{

if((!in) || (in.eof()))
	{
	return false;
	}


in.read((char*)&Time,sizeof(Time));

if((!in) || (in.eof()))
	{
	return false;
	}

int tmpLen; 
char *tchar;
in.read((char*)&tmpLen,sizeof(int));
tchar = new char[tmpLen+1];
in.read(tchar,tmpLen);
tchar[tmpLen] = '\0';
CommandName = tchar;
delete[] tchar;

if((!in) || (in.eof()))
	{
	return false;
	}

in.read((char*)&tmpLen,sizeof(int));
tchar = new char[tmpLen+1];
in.read(tchar,tmpLen);
tchar[tmpLen] = '\0';
User = tchar;
delete[] tchar;


if((!in) || (in.eof()))
	{
	return false;
	}

in.read((char*)&tmpLen,sizeof(int));
tchar = new char[tmpLen+1];
in.read(tchar,tmpLen);
tchar[tmpLen] = '\0';
Host = tchar;
delete[] tchar;

if((!in) || (in.eof()))
	{
	return false;
	}

in.read((char*)&tmpLen,sizeof(int));
tchar = new char[tmpLen+1];
in.read(tchar,tmpLen);
tchar[tmpLen] = '\0';
Desc = tchar;
delete[] tchar;

return true;

}

}
}
