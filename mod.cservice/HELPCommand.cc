/**
 * HELPCommand.cc
 *
 * 18/02/2001 - David Henriksen <david@itwebnet.dk>
 * Initial Version.
 *
 * Outputs channel service help.
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
 * $Id: HELPCommand.cc,v 1.5 2003/06/28 01:21:20 dan_karrels Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include 	"responses.h"

namespace gnuworld
{
using namespace gnuworld;

bool HELPCommand::Exec( iClient* theClient, const string& Message )
{
	StringTokenizer st( Message ) ;

	string topic = "" ;
	if( st.size() < 2 )
		topic = "HELP" ;
	else
		topic = string_upper(st.assemble(1));

	sqlUser* theUser = bot->isAuthed(theClient, false);
	string msg = bot->getHelpMessage(theUser, topic);

	if (msg.empty())
		msg = bot->getHelpMessage(theUser, "INDEX");

	if (!msg.empty())
	{
		bot->Notice(theClient, msg);
		return true;
	}
	else
		msg = bot->getHelpMessage(NULL, topic);

	if (!msg.empty())
		bot->Notice(theClient, msg);
	else
		bot->Notice(theClient, "There is no help available for that topic.");

	return true ;
}

} // namespace gnuworld.
