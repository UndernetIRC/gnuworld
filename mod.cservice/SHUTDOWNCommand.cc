/*
 * SHUTDOWNCommand.cc
 *
 * 28/12/2001 - Matthias Crauwels <ultimate_@wol.be>
 * Initial Version.
 *
 * Shuts down the bot and squits the server
 *
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"

const char SHUTDOWNCommand_cc_rcsId[] = "$Id: SHUTDOWNCommand.cc,v 1.1 2001/12/29 17:51:15 gte Exp $" ;
namespace gnuworld
{
using std::string ;
using namespace level;

bool SHUTDOWNCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.SHUTDOWN");

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

/*
 *  Fetch the sqlUser record attached to this client. If there isn't one,
 *  they aren't logged in - tell them they should be.
 */

sqlUser* theUser = bot->isAuthed(theClient, false);
if (!theUser)
	{
	return false;
	}

int admLevel = bot->getAdminAccessLevel(theUser);
if (admLevel < level::shutdown)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access,
			string("Sorry, you have insufficient access to perform that command.")));
	return false;
	}

bot->logAdminMessage("%s issued the shutdown command! Bye! xxx!\n", theClient->getNickName().c_str());
bot->Exit( st.assemble(1).c_str());

char buf[ 512 ] = { 0 } ;

sprintf( buf, "%s SQ %s :%s made me shutdown (%s)\n", bot->getCharYY(), bot->getUplinkName().c_str(),
			theClient->getNickName().c_str(), st.assemble(1).c_str() );

bot->QuoteAsServer( buf );

return true;
}

} // namespace gnuworld.
