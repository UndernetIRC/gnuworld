/**
 * TRANSLATECommand.cc
 * Translates a numeric to a nick
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
 * $Id: TRANSLATECommand.cc,v 1.14 2005/01/12 03:50:29 dan_karrels Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"Network.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: TRANSLATECommand.cc,v 1.14 2005/01/12 03:50:29 dan_karrels Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

// translate yyxxx
bool TRANSLATECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() != 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

iClient* Target = Network->findClient( st[ 1 ] ) ;

if( NULL == Target )
	{
	bot->Notice( theClient, "Unable to find numeric %s\n",
		st[ 1 ].c_str() ) ;
	return true ;
	}
bot->MsgChanLog("TRANSLATE %s\n",st.assemble(1).c_str());

iServer* theServer = Network->findServer( Target->getIntYY() ) ;
if( NULL == theServer )
	{
	elog	<< "TRANSLATECommand> Unable to find server\n" ;
	return false ;
	}

if((Target->isModeR()) && (Target->isModeX()))
	{
	bot->Notice( theClient, "%s is %s (%s) on server %s",
		st[ 1 ].c_str(),
		Target->getNickUserHost().c_str(),
		Target->getRealNickUserHost().c_str(),
		theServer->getName().c_str() ) ;
	}
	
else
	{
	
	bot->Notice( theClient, "%s is %s on server %s",
		st[ 1 ].c_str(),
		Target->getNickUserHost().c_str(),
		theServer->getName().c_str() ) ;
	}

return true ;

}
}
}
