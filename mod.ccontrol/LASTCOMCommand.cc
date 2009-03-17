/**
 * LASTCOMCommand.cc
 *
 * Sends an oper a list of the last n commands
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
 * $Id: LASTCOMCommand.cc,v 1.18 2009/03/17 09:50:58 denspike Exp $
 */

#include	<sstream>
#include	<string>
#include	<iostream>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"iClient.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: LASTCOMCommand.cc,v 1.18 2009/03/17 09:50:58 denspike Exp $" ) ;

namespace gnuworld
{

using std::endl ;
using std::stringstream ;
using std::ends ;
using std::string ;

namespace uworld
{

bool LASTCOMCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
bot->MsgChanLog("LASTCOM\n");
#ifndef LOGTOHD
unsigned int NumOfCom;
unsigned int Days = 0;
stringstream theQuery;

if( st.size() == 1 )
	{
	NumOfCom = 20;
	static const char* queryHeader = "SELECT * FROM comlog ";
	theQuery 	<< queryHeader 
			<< " ORDER BY ts DESC"
			<< " LIMIT " << NumOfCom
			<< ends;
	}
else 
	{
	NumOfCom = atoi(st[1].c_str());
	if(st.size() > 2)
		{
		Days = atoi(st[2].c_str());
		if(Days > 365)
			{
			bot->Notice(theClient,"You can't see the log for more than a year ago");
			return false;
			}
		static const char* queryHeader
		    = "SELECT * FROM comlog where ts >";
		theQuery 	<< queryHeader 
				<< (::time(0) - Days * 24 * 3600)
				<< " ORDER BY ts ASC"
				<< " LIMIT " << NumOfCom
				<< ends;
		}
	else
		{	
		static const char* queryHeader
		    = "SELECT * FROM comlog ";
		theQuery 	<< queryHeader 
				<< " ORDER BY ts DESC"
				<< " LIMIT " << NumOfCom
				<< ends;
					    
		 
		}
	}
	
elog	<< "LASTCOM> " 
	<< theQuery.str().c_str() 
	<< endl;
	

if (!bot->SQLDb->Exec(theQuery, true))
//if( PGRES_TUPLES_OK != status )
	{
	elog	<< "LASTCOM> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

// SQL Query succeeded
bot->Notice(theClient,"Listing last %d messages from day %d",NumOfCom,Days);
for (int i = (bot->SQLDb->Tuples() - 1) ; i >= 0; i--)
	{
	bot->Notice(theClient,"[ %s - %s ] %s",bot->convertToAscTime(atoi(bot->SQLDb->GetValue(i, 0))),bot->SQLDb->GetValue(i,1).c_str(),bot->SQLDb->GetValue(i,2).c_str());
	}
#else
	if(st.size() > 1)
		bot->showLogs(theClient,atoi(st[1].c_str()));
	else
		bot->showLogs(theClient);
#endif
bot->Notice(theClient,"End of debug log");
return true;
}

}
}

