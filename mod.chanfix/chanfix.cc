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
 *
 * $Id: chanfix.cc,v 1.2 2004/05/18 20:55:02 jeekay Exp $
 */

#include <string>

#include "EConfig.h"

#include "chanfix.h"

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

confOpAuth = atoi(config->Require("opAuth")->second.c_str());
confPeriod = atoi(config->Require("period")->second.c_str());
confStartDelay = atoi(config->Require("startDelay")->second.c_str());
}

/** Destructor doing nothing as we have no heap. */
chanfix::~chanfix()
{
}

} // namespace chanfix

} // namespace gnuworld
