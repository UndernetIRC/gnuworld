/*
 * LASTCOMCommand.cc
 *
 * Sends an oper a list of the last n commands
 *
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

const char LASTCOMCommand_cc_rcsId[] = "$Id: LASTCOMCommand.cc,v 1.12 2002/06/07 17:58:24 mrbean_ Exp $";

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
bot->MsgChanLog("LASTCOM %s\n");
#ifndef LOGTOHD
unsigned int NumOfCom;
unsigned int Days = 0;
stringstream theQuery;

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

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
	
ExecStatusType status = bot->SQLDb->Exec( theQuery.str().c_str() ) ;

if( PGRES_TUPLES_OK != status )
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
	bot->Notice(theClient,"[ %s - %s ] %s",bot->convertToAscTime(atoi(bot->SQLDb->GetValue(i, 0))),bot->SQLDb->GetValue(i,1),bot->SQLDb->GetValue(i,2));
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

