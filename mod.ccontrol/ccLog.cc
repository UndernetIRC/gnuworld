
/*

 */
 
#include "ccLog.h"
#include <fstream>
#include "ELog.h"
#include <string>
#include <stdio.h>
#include "StringTokenizer.h"

using namespace std;

namespace gnuworld
{

namespace uworld
{

bool ccLog::Save(fstream& out)
{

if(out.bad())
	{
	return false;
	}

char time[20];
time[0] = '\0';
sprintf(time,"%li",::time(0));
string Line = time;
Line  += " ";
string::size_type pos;
for(pos = 0; pos < User.size();++pos)
	{
	if(User[pos] != '\\') 
		{
		Line += User[pos];
		}
	else
		{
		Line += "\\\\";
		}
	}
Line += ' ';	
for(pos = 0; pos < Host.size();++pos)
	{
	if(Host[pos] != '\\') 
		{
		Line += Host[pos];
		}
	else
		{
		Line += "\\\\";
		}
	}
Line += ' ';
for(pos = 0; pos < Desc.size();++pos)
	{
	if(Desc[pos] != '\\') 
		{
		Line += Desc[pos];
		}
	else
		{
		Line += "\\\\";
		}
	}

/*out 	<< Time
        << " " << User.c_str()
	<< " " << Host.c_str()
	<< " " << Desc.c_str()
	<< "\n";*/
out << Line << "\n";	
return true;

}

bool ccLog::Load(fstream& in)
{

if((in.bad()) || (in.eof()))
	{
	return false;
	}
char read[513];
if(!in.getline(read,512))
	{
	return false;
	}
StringTokenizer st(read);
if(st.size() < 4)
	{
	elog << "invalid number of args in logs!" << endl;
	return false;
	}
Time = atoi(st[0].c_str());
User = st[1];
Host = st[2];
CommandName = st[3];
Desc = st.assemble(3);

return true;

}


}
}
