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
 * $Id: SEARCHCommand.cc,v 1.7 2002/04/28 16:02:26 gte Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"libpq++.h"
#include	"responses.h"
#include	"cservice_config.h"
#include	"Network.h"
#include	<iostream>

const char SEARCHCommand_cc_rcsId[] = "$Id: SEARCHCommand.cc,v 1.7 2002/04/28 16:02:26 gte Exp $" ;

namespace gnuworld
{

using std::string ;

static const char* queryHeader =    "SELECT channels.name,channels.keywords FROM channels ";
static const char* queryCondition = "WHERE channels.keywords ~* ";
static const char* queryFooter =    "ORDER BY channels.name DESC LIMIT 100;";

bool SEARCHCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.SEARCH");

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

int chanLimit = 0;
bool doChanLimit = false;
bool doneChanLimit = false;

unsigned int posOfSearch = 1;
for( StringTokenizer::const_iterator ptr = st.begin() ; ptr != st.end() ;
	++ptr )
{
	if ((string_lower(*ptr) == "-min") && !doChanLimit)
		{
		doChanLimit = true;
		posOfSearch++;
		continue;
		}

	if(!doneChanLimit && doChanLimit)
		{
			chanLimit = atoi( (*ptr).c_str() );
			doneChanLimit = true;
			posOfSearch++;
		}
}

sqlUser* theUser = bot->isAuthed(theClient, false);

string matchString = st.assemble(posOfSearch);

if (matchString.empty())
	{
	Usage(theClient);
	return true;
	}

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
	/*
	 * Look to see if the channel exists in memory.
	 */
	Channel* tmpChan = Network->findChannel(bot->SQLDb->GetValue(i, 0));

	/*
	 * Yes it does, if we have specified a minimum level of users on the chan,
	 * lets make sure we don't show quiet chans.
	 */

	int chanMembers = -1;
	if(tmpChan) chanMembers = tmpChan->size();

	if(chanMembers >= chanLimit)
	{
	results++;
	bot->Notice(theClient, "\026%-14s \026 - %s (%i users)",
		    bot->SQLDb->GetValue(i, 0),
		    bot->SQLDb->GetValue(i, 1),
		    chanMembers);
	}

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
	if(!chanLimit)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::no_search_match,
			string("No matching entries for [%s]")).c_str(),
			matchString.c_str() );
	} else {
		bot->Notice(theClient, "No matching entries for [%s] with a minimum of %i users.",
			matchString.c_str(), chanLimit);
	}
	}

return true ;
}

} // namespace gnuworld.
