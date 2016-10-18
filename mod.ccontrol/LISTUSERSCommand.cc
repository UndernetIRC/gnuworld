/**
 * LISTUSERSCommand.cc
 * List the users who has access to the bot
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
 * $Id: REMCOMMANDCommand.cc,v 1.16 2009/06/13 06:43:34 hidden1 Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include 	"ccUser.h"
#include	"misc.h"
#include	"gnuworld_config.h"
#include    "UserFilter.h"

namespace gnuworld {

using std::string;

namespace uworld {

bool LISTUSERSCommand::Exec(iClient* theClient, const string& Message) {
	StringTokenizer st(Message);
	UserFilterComposite filters;
	ByLevelsUserFilter levelsFilter;
	bool wantedLevels = false;
	unsigned int pos = 1;
	while(pos < st.size()) {
		if(!strcmp(st[pos].c_str(),"-l")) {
			pos++;
			if(pos == st.size()) {
				bot->Notice(theClient,"-l option must get the level of the oper to show");
				return false;
			}
			int level = bot->strToLevel(st[pos++]);
			if(level < 0) {
				bot->Notice(theClient,"invalid oper level %s, valid options are: coder/smt/admin/oper/uhs",st[pos].c_str());
				return false;
			}
			wantedLevels = true;
			levelsFilter.addLevel(level);

		}
		pos++;
	}

	if(wantedLevels) {
		filters.addFilter(&levelsFilter);
	}
	ccUser* user;
	ccontrol::usersconstiterator uItr = bot->usersmap_begin();
	int count = 0;
	stringstream sstr;
	for (; uItr != bot->usersmap_end(); uItr++) {
		user = uItr->second;
		
		if(filters.filter(user)) {
			if(sstr.str().size() > 0) {
				sstr << ",";
			}
			sstr << "User: " << user->getUserName() << " Level: " << bot->levelToStr(user->getType());
			count ++;
			if(count % 2 == 0) {
				bot->Notice(theClient,"%s",sstr.str().c_str());
				sstr.str("");
			}
		}
	}
	if (!sstr.str().empty()) {
		bot->Notice(theClient, "%s", sstr.str().c_str());
	}

	bot->Notice(theClient,"Finished listing %d users",count);

	return true;

}

}
}

