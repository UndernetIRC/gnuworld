/*
 * LASTCOMCommand.cc
 *
 * Sends an oper a list of the last n commands
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char LASTCOMCommand_cc_rcsId[] = "$Id: LASTCOMCommand.cc,v 1.6 2001/12/23 09:07:57 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool LASTCOMCommand::Exec( iClient* theClient, const string& Message )
{

unsigned int NumOfCom;
unsigned int Days = 0;
StringTokenizer st( Message ) ;
strstream theQuery;

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

if( st.size() == 1 )
	{
	NumOfCom = 20;
	static const char* queryHeader
        = "SELECT * FROM comlog ";
	theQuery 	<< queryHeader 
			<< " ORDER BY ts DESC"
			<< " LIMIT " << NumOfCom
			<< ends;
	ccUser* tmpUser = bot->IsAuth(theClient);
	if(tmpUser)
        bot->MsgChanLog("(%s) - %s : LASTCOM\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str());
	}
else 
	{
	ccUser* tmpUser = bot->IsAuth(theClient);
	if(tmpUser)
        bot->MsgChanLog("(%s) - %s : LASTCOM %s\n",tmpUser->getUserName().c_str()
                        ,theClient->getNickUserHost().c_str(),st.assemble(1).c_str());
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
	<< theQuery.str() 
	<< endl;
	
ExecStatusType status = bot->SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

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

bot->Notice(theClient,"End of debug log");
return true;
}

}
}

	
									
