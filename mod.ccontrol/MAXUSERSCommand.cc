/**
 * MAXUSERSCommand.cc 
 * Shows the maximum number of online users ever recorded
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
 * $Id: MAXUSERSCommand.cc,v 1.7 2006/09/26 17:35:59 kewlio Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"ccontrol_generic.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: MAXUSERSCommand.cc,v 1.7 2006/09/26 17:35:59 kewlio Exp $" ) ;

namespace gnuworld
{

namespace uworld
{

bool MAXUSERSCommand::Exec( iClient* theClient, const string& )
{
bot->Notice(theClient,"Current number of users is: %d",bot->getCurUsers());
bot->Notice(theClient,"Maximum number of users ever recorded is: %d",bot->getMaxUsers());
bot->Notice(theClient,"Recorded on %s (%s ago)",bot->convertToAscTime(bot->getDateMax()),
	Ago(bot->getDateMax()));

return true;
}

}

} // namespace gnuworld
