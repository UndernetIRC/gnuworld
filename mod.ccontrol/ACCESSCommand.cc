/*
 * ACCESSCommand.cc 
 *
 * Shows all the users that has access to the bot
 
 * Note: access = bit mask of the commands that the oper got access to
 
 */

#include	<string>
#include	<strstream>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"

const char ACCESSCommand_cc_rcsId[] = "$Id: ACCESSCommand.cc,v 1.6 2001/03/02 02:02:00 dan_karrels Exp $";

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::strstream ;
using std::ends ;

static const char* queryHeader
	= "SELECT user_name,access,last_updated_by FROM opers; ";

bool ACCESSCommand::Exec( iClient* theClient, const string& Message)
{

strstream theQuery;
theQuery	<< queryHeader 
		<< ends;

elog	<< "ACCESS> "
	<< theQuery.str()
	<< endl; 

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
for (int i = 0 ; i < bot->SQLDb->Tuples(); i++)
	{
	bot->Notice(theClient, "USER: %s ACCESS: %s MODIFIED BY: %s",
		bot->SQLDb->GetValue(i, 0),
		bot->SQLDb->GetValue(i, 1),
		bot->SQLDb->GetValue(i, 2));
	}

return true ;
}

} // namespace gnuworld
