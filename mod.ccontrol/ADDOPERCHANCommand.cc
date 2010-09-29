/**
 * ADDOPERCHANCommand.cc
 * Add a new irc operator channel 
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
 * $Id: ADDOPERCHANCommand.cc,v 1.12 2006/09/26 17:35:57 kewlio Exp $
 */
 
#include	<string>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"

RCSTAG( "$Id: ADDOPERCHANCommand.cc,v 1.12 2006/09/26 17:35:57 kewlio Exp $" ) ;

namespace gnuworld
{

namespace uworld
{

using std::string ;

bool ADDOPERCHANCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

string chanName = st[ 1 ] ;
if( '#' != chanName[ 0 ] )
	{
	bot->Notice( theClient, "Invalid channel name" ) ;
	return true ;
	}

if(chanName.length() >  channel::MaxName)
	{
	bot->Notice( theClient,"Channel name too long" );
	return true;
	}

if( bot->addOperChan( chanName ) )
	{
	bot->Notice( theClient, "Addition of %s as oper chan has SUCCEEDED",
		chanName.c_str() ) ;
	}
else
	{
	bot->Notice( theClient, "Addition of %s as oper chan has FAILED",
		chanName.c_str() ) ;
	}
return true ;
}
}
}

