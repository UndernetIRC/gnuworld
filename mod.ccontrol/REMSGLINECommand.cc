/**
 * REMSGLINECommand.cc
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
 * $Id: REMSGLINECommand.cc,v 1.4 2003/06/28 01:21:19 dan_karrels Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"

const char REMSGLINECommand_cc_rcsId[] = "$Id: REMSGLINECommand.cc,v 1.4 2003/06/28 01:21:19 dan_karrels Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

// remgline user@host
bool REMSGLINECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

if(!dbConnected)
        {
        bot->Notice(theClient,"Sorry, but the db connection is down now, please try again alittle later");
        return false;
        }

if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}
bot->MsgChanLog("REMSGLINE %s\n",st.assemble(1).c_str());

if(st[1].substr(0,1) == "#")
	{
	bot->Notice(theClient,"Please use REMGCHAN to remove a BADCHAN gline");
	return false;
	}
ccGline *tmpGline;
bool realName = false;
if(st[1].substr(0,1) != "$")
    tmpGline = bot->findGline(st[1]);
else
	{    
	tmpGline = bot->findRealGline(st[1]);
	realName = true;
	}
if(tmpGline != NULL)
	{
	if(!tmpGline->Delete())
		bot->MsgChanLog("Error while removing gline for host %s from the db\n",st[1].c_str());
	bot->remGline(tmpGline);
	delete tmpGline;
	}	
if(!realName)
	server->removeGline(st[1],bot);
bot->Notice( theClient, "Removal of gline(%s) succeeded\n",st[1].c_str() ) ;
return true ;
}

}
} // namespace gnuworld

