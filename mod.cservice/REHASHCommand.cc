/**
 * REHASHCommand.cc
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
 * $Id: REHASHCommand.cc,v 1.6 2003/06/28 01:21:20 dan_karrels Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"

const char REHASHCommand_cc_rcsId[] = "$Id: REHASHCommand.cc,v 1.6 2003/06/28 01:21:20 dan_karrels Exp $" ;

namespace gnuworld
{
using std::string ;

bool REHASHCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.REHASH");

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser) return false;

int level = bot->getAdminAccessLevel(theUser);
if (level < level::rehash_admin)
{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::insuf_access,
			string("You have insufficient access to perform that command.")));
	return false;
}

string option = string_upper(st[1]);

if (option == "MOTD")
	{
                int lastupdate = bot->rehashMOTD();
                if (lastupdate > 0) {
                        bot->Notice(theClient, "Done. Rehashed the MOTD, last updated %s ago.",
                                bot->prettyDuration(lastupdate).c_str(), lastupdate);
			return true;
                } else {
                        bot->Notice(theClient, "Couldn't update the MOTD.");
			return false;
                }
	}

if (option == "HELP")
	{
		bot->helpTable.clear();
		bot->loadHelpTable();
		bot->Notice(theClient, "Done. %i entries in help table.",
			bot->helpTable.size());
		return true;
	}
if (option == "TRANSLATIONS")
	{
		bot->languageTable.clear();
		bot->translationTable.clear();
		bot->loadTranslationTable();
		bot->Notice(theClient, "Done. %i entries in language table.",
			bot->translationTable.size());
		return true;
	}
/* Options below require a higher level to rehash */
if (level < level::rehash_coder)
{
        bot->Notice(theClient,
                bot->getResponse(theUser,
                        language::insuf_access,
                        string("You have insufficient access to perform that command.")));
        return false;
}

if (option == "CONFIG")
	{
		bot->configTable.clear();
		bot->loadConfigData();
		bot->Notice(theClient, "Done. %i entries in config table.",
			bot->configTable.size());
		// TODO: Free up the memory allocated in building this table ;)
	}

if (option == "VARIABLES")
{
	bot->clearConfigVariables();
	bot->parseConfigFile();
	bot->loadConfigVariables();
	bot->Notice(theClient, "Done. Red %i configuration variables.",
		bot->getConfigVariableSize());
}

return true ;
}

} // namespace gnuworld.
