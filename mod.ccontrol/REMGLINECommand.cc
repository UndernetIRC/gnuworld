/**
 * REMGLINECommand.cc
 * Removes a gline
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
 * $Id: REMGLINECommand.cc,v 1.27 2009/06/14 01:29:54 hidden1 Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: REMGLINECommand.cc,v 1.27 2009/06/14 01:29:54 hidden1 Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

// remgline user@host
bool REMGLINECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}
bot->MsgChanLog("REMGLINE %s\n",st.assemble(1).c_str());

if(st[1].substr(0,1) == "#")
	{
	bot->Notice(theClient,"Please use REMGCHAN to remove a BADCHAN gline");
	return false;
	}
if(st[1].substr(0,1) == "$")
	{
	bot->Notice(theClient,"Please use REMSGLINE to remove a realname gline");
	return false;
	}

if (st[1].find('@',1) == string::npos)
	{
	bot->Notice(theClient, "Invalid G-line. Please specify a user@. i.e: REMGLINE *@ip");
	return false;
	}

unsigned int dummy;

if(bot->checkGline(st[1],0,dummy) & gline::HUH_NO_HOST)
	{
	bot->Notice(theClient,"Please use REMSGLINE to remove a this gline");
	return false;
	}
	
ccGline *tmpGline = bot->findGline(st[1]);
if(tmpGline != NULL)
	{
	if(!tmpGline->Delete())
		bot->MsgChanLog("Error while removing gline for host %s from the db\n",st[1].c_str());
	bot->remGline(tmpGline);
	delete tmpGline;
	}	
server->removeGline(st[1],bot);
bot->Notice( theClient, "Removal of gline succeeded\n" ) ;
return true ;
}

}
} // namespace gnuworld

