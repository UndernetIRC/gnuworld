#include	<string>
#include	<cstdlib>
#include        <iomanip.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

namespace gnuworld
{

using std::string ;


bool ACCESSCommand::Exec( iClient* theClient, const string& Message)
{
static const char* queryHeader =    "SELECT user_name,access,last_updated_by FROM opers; ";
 
StringTokenizer st( Message ) ;

strstream theQuery;
theQuery	<< queryHeader 
		<< ends;

elog	<< "ACCESS> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = bot->SQLDb->Exec( theQuery.str() ) ;
if( PGRES_TUPLES_OK == status )
	{
	for (int i = 0 ; i < bot->SQLDb->Tuples(); i++)
		{
		bot->Notice(theClient, "USER: %s ACCESS: %s MODIFIED BY: %s",
			bot->SQLDb->GetValue(i, 0),
			bot->SQLDb->GetValue(i, 1),
			bot->SQLDb->GetValue(i, 2));
		}
	return true ;
	}
else
	{
	elog	<< "Error in query!"
		<< endl;
	}

return false;    
}
}
