/**
 * REMOVEIGNORECommand.cc
 * Removes ignored host
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
 * $Id: REMOVEIGNORECommand.cc,v 1.10 2005/01/12 03:50:29 dan_karrels Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: REMOVEIGNORECommand.cc,v 1.10 2005/01/12 03:50:29 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;

// remoperchan #channel

namespace uworld
{

bool REMOVEIGNORECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}
bot->MsgChanLog("REMIGNORE %s\n",st.assemble(1).c_str());

string::size_type atPos = st[ 1 ].find_first_of( '@' ) ;
if( string::npos == atPos )
	{
	iClient *igClient = Network->findNick(st[1]);
	if(!igClient)
		{
		bot->Notice(theClient,"There is no such user as %s",st[1].c_str());
		return false;
		}
	if(bot->removeIgnore(igClient) == IGNORE_REMOVED)
		{
		bot->Notice(theClient,"Successfully removed ignore for user %s",st[1].c_str());
		bot->MsgChanLog("Removed ignore for %s by %s request\n",st[1].c_str(),theClient->getNickName().c_str());
		}

	else
		bot->Notice(theClient,"Cant find ignore for user %s",st[1].c_str());
	}
else
	if(bot->removeIgnore(st[1]) == IGNORE_REMOVED)
		{	
		bot->Notice(theClient,"Successfully removed ignore for host %s",st[1].c_str());
		bot->MsgChanLog("Removed ignore for %s by %s request\n",st[1].c_str(),theClient->getNickName().c_str());
		}
	else
		bot->Notice(theClient,"Cant find ignore for host %s",st[1].c_str());

return true;
}

}
} // namespace gnuworld

