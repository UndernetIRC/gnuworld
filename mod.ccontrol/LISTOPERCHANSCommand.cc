/**
 * LISTOPERCHANSCommand.cc
 * List the channels thare are marked as ircops only
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
 * $Id: LISTOPERCHANSCommand.cc,v 1.9 2005/01/12 03:50:29 dan_karrels Exp $
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: LISTOPERCHANSCommand.cc,v 1.9 2005/01/12 03:50:29 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

// listoperchans
bool LISTOPERCHANSCommand::Exec( iClient* theClient, const string& )
{

bot->Notice( theClient, "There are currently %d IRCoperator only channels",
	bot->operChan_size() ) ;

if( bot->operChan_empty() )
	{
	return true ;
	}

string chanList = "" ;
ccontrol::const_operChanIterator ptr = bot->operChan_begin() ;

while( ptr != bot->operChan_end() )
	{
	if( !chanList.empty() )
		{
		chanList += ", " ;
		chanList += *ptr ;
		}
	else
		{
		chanList = *ptr ;
		}
	++ptr ;
	}

bot->Notice( theClient, chanList ) ;
return true ;
}
}
}
