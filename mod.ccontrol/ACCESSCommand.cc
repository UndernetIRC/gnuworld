/*
 * ACCESSCommand.cc 
 *
 * Shows all the users that has access to the bot
 
 * Note: access = bit mask of the commands that the oper got access to
 
 */

#include	<string>
#include	<strstream>
#include	<cstdlib>
#include	"StringTokenizer.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"

const char ACCESSCommand_cc_rcsId[] = "$Id: ACCESSCommand.cc,v 1.12 2001/11/08 23:13:29 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::strstream ;
using std::ends ;

namespace uworld
{

static const char* queryHeader
	= "SELECT user_name,access,flags,last_updated_by,server FROM opers ";

bool ACCESSCommand::Exec( iClient* theClient, const string& Message)
{

StringTokenizer st( Message ) ;



string Uname;
if(st.size() > 1)
	{
	if(st[1].size() > 64)
		{
		bot->Notice(theClient,"Oper name can't be more than 64 chars");
		return false;
		}
	Uname = "Where lower(user_name) = '" +  string_lower(bot->removeSqlChars(st[1])) + "'\n";
	}
else
	{
	Uname = "";
	}
	
strstream theQuery;
theQuery	<< queryHeader 
		<< Uname
		<< ends;

elog	<< "ACCESS> "
	<< theQuery.str()
	<< Uname; 

ExecStatusType status = bot->SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "ACCESS> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

// SQL Query succeeded
string Flags = "UnSet";
for (int i = 0 ; i < bot->SQLDb->Tuples(); i++)
	{
/*	if(atoi(bot->SQLDb->GetValue(i,2)) & isOPER)
		{
		Flags.assign("OPER");
		}
	else if(atoi(bot->SQLDb->GetValue(i,2)) & isADMIN)
		{
		Flags.assign("ADMIN");
		}
	else if(atoi(bot->SQLDb->GetValue(i,2)) & isSMT)
		{
		Flags.assign("SMT");
		}
	else if(atoi(bot->SQLDb->GetValue(i,2)) & isCODER)
		{
		Flags.assign("CODER");
		}*/
	bot->Notice(theClient, "USER: \002%s\002 ACCESS: \002 0x%x\002 FLAGS: \002%s\002 MODIFIED BY: \002%s\002 SERVER: \002%s\002",
		bot->SQLDb->GetValue(i, 0),
		atoi(bot->SQLDb->GetValue(i, 1)),
		Flags.c_str(),
		bot->SQLDb->GetValue(i, 3),
		bot->SQLDb->GetValue(i,4));
	}
if((Uname != "") && (bot->SQLDb->Tuples() == 0))
	{
	bot->Notice(theClient,"Can't find oper %s",st[1].c_str());
	}
else
	{
	bot->Notice(theClient,"-= End Of Access List =-");
	}

return true ;
}

}

} // namespace gnuworld
