/*
 * SEARCHCommand.cc
 *
 * 11/02/2001 - David Henriksen <david@itwebnet.dk>
 * Command written, and finished.
 *
 * Searches through the registered channels list, for a matching string in the channels'
 * keywords. Max 10 matches will be outputted.
 *
 * Caveats: None.
 *
 * $Id: SEARCHCommand.cc,v 1.5 2001/03/07 15:10:53 dan_karrels Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"libpq++.h"
#include	"responses.h"
#include	"cservice_config.h"

const char SEARCHCommand_cc_rcsId[] = "$Id: SEARCHCommand.cc,v 1.5 2001/03/07 15:10:53 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;

static const char* queryHeader =    "SELECT channels.name,channels.keywords FROM channels ";
static const char* queryCondition = "WHERE channels.keywords ~* ";
static const char* queryFooter =    "ORDER BY channels.name DESC;";

bool SEARCHCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

sqlUser* theUser = bot->isAuthed(theClient, false);

string matchString = st.assemble(1);
size_t results = 0;

strstream extraCond;
extraCond	<< "'"
		<< escapeSQLChars(matchString)
		<< "' "
		<< ends;

strstream theQuery;
theQuery	<< queryHeader
		<< queryCondition
		<< extraCond.str()
		<< queryFooter
		<< ends;

#ifdef LOG_SQL
	elog	<< "SEARCH::sqlQuery> "
		<< theQuery.str()
		<< endl;
#endif
	
ExecStatusType status = bot->SQLDb->Exec(theQuery.str());
delete[] theQuery.str() ;
delete[] extraCond.str() ;

if( PGRES_TUPLES_OK != status )
	{
	elog	<< "SEARCH> SQL Error: "
		<< bot->SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}

for( int i = 0 ; i < bot->SQLDb->Tuples(); i++ )
	{
	results++;
	bot->Notice(theClient, "\026%-14s \026 - %s",
		    bot->SQLDb->GetValue(i, 0),
		    bot->SQLDb->GetValue(i, 1));

	if(results >= MAX_SEARCH_RESULTS)
		{
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::exc_search,
				string("There are more than %i entries matching [%s]")).c_str(),
				MAX_SEARCH_RESULTS,
				matchString.c_str()
		);
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::restrict_search,
				string("Please restrict your search mask")).c_str()
		);
		break;
		}
	} // for()

if( 0 == results )
	{
	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::no_search_match,
			string("No matching entries for [%s]")).c_str(),
			matchString.c_str() );
	}
 
return true ;
} 

} // namespace gnuworld.
