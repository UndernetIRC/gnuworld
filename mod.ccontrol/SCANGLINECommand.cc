/**
 * SCANGLINECommand.cc
 * Check if a certain host is glined, if it is returns the glines info
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
 * $Id: SCANGLINECommand.cc,v 1.13 2006/09/26 17:36:01 kewlio Exp $
 */

#include	<string>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: SCANGLINECommand.cc,v 1.13 2006/09/26 17:36:01 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool SCANGLINECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

bot->MsgChanLog("SCANGLINE %s\n",st.assemble(1).c_str());

bot->listGlines(theClient,st[1]);

/*vector< const Gline* > glines = server->matchGline( st[ 1 ] ) ;

bot->Notice( theClient, "Found %d matches", glines.size() ) ;
if( glines.empty() )
	{
	return true ;
	}

bot->Notice( theClient, "Current time: %ld", (long)::time( 0 ) ) ;

for( vector< const Gline* >::const_iterator ptr = glines.begin() ;
	ptr != glines.end() ; ++ptr )
	{
	bot->Notice( theClient, "%s expires at %d, set by %s because %s",
		(*ptr)->getUserHost().c_str(),
		(*ptr)->getExpiration(),
		(*ptr)->getSetBy().c_str(),
		(*ptr)->getReason().c_str() ) ;
	}*/

return true ;
}

}
} // namespace gnuworld

