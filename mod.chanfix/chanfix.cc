/**
 * chanfix.cc
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
 */

#include <string>

#include "EConfig.h"
#include "ELog.h"

#include "cfChannel.h"
#include "chanfix.h"
#include "chanfix-commands.h"

namespace gnuworld {

namespace chanfix {

using std::string;

extern "C" {
	xClient* _gnuwinit(const string& args) {
		return new chanfix( args ) ;
	}
}

/** Constructor taking the configuration file as an argument. */
chanfix::chanfix( const string& configFileName )
	: xClient( configFileName )
{
/* Get our config */
EConfig *config = new EConfig(configFileName);
assert( config != 0 );

/* Initialise our config variables */
confConsoleChannel = config->Require("consoleChannel")->second;
confConsoleModes = config->Require("consoleModes")->second;

//confLogLevel = atoi(config->Require("logLevel")->second.c_str());
confLogLevel = 255;

confPointsAuth = atoi(config->Require("pointsAuth")->second.c_str());
confMaxPoints = atoi(config->Require("maxPoints")->second.c_str());
confPeriod = atoi(config->Require("period")->second.c_str());
confStartDelay = atoi(config->Require("startDelay")->second.c_str());

/* Register our commands */
RegisterCommand(new CHECKCommand(this, "CHECK", "<#channel>"));
}

/** Destructor doing nothing as we have no heap. */
chanfix::~chanfix()
{
for( mapChannels::iterator itr = channels.begin() ;
     itr != channels.end() ;
     ++itr ) {
	delete itr->second;
}
}

} // namespace chanfix

} // namespace gnuworld
