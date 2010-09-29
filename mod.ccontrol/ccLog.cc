/**
 * ccLog.cc
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
 * $Id: ccLog.cc,v 1.10 2007/09/12 13:36:02 kewlio Exp $
 */
 
#include "ccLog.h"
#include <fstream>
#include "ELog.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "StringTokenizer.h"

#include	"gnuworld_config.h"

RCSTAG( "$Id: ccLog.cc,v 1.10 2007/09/12 13:36:02 kewlio Exp $" ) ;

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
sprintf(time,"%ld",(long)::time(0));
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
