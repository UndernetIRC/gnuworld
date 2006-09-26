/**
 * DEAUTHCommand.cc
 * Deauthenticates a user
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
 * $Id: DEAUTHCommand.cc,v 1.14 2006/09/26 17:35:58 kewlio Exp $
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: DEAUTHCommand.cc,v 1.14 2006/09/26 17:35:58 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool DEAUTHCommand::Exec( iClient* theClient, const string& )
{
//Fetch the user authenticate entry
ccUser* tmpUser = bot->IsAuth(theClient->getCharYYXXX());
if (!tmpUser) 
	{
	bot->Notice(theClient,"You are not currently authenticated");
	return false;
	}
if(bot->deAuthUser(tmpUser))
	{
	bot->Notice(theClient,"DeAuthentication successfull");
	bot->MsgChanLog("(%s) - %s : Deauthenticated\n",tmpUser->getUserName().c_str()
			,theClient->getRealNickUserHost().c_str());
	/* forget their details to stop them getting auto-authenticated after netsplit */
	tmpUser->setLastAuthTS(1);		/* if uninitialised, this would be 0 - so 1 should be a safe figure! */
	tmpUser->setLastAuthNumeric("");
	return true;
	}
else
	{
	bot->Notice(theClient,"DeAuthentication failed");
	return false;
	}
}	

}
}
