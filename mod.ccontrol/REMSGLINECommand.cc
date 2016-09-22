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
 * $Id: REMSGLINECommand.cc,v 1.8 2006/09/26 17:36:01 kewlio Exp $
 */

#include	<string>
#include        <iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: REMSGLINECommand.cc,v 1.8 2006/09/26 17:36:01 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

// remgline user@host
bool REMSGLINECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

StringTokenizer::size_type pos = 1 ;

string cmdStr = "REMSGLINE ";

bool ForceRemove = (!strcasecmp(st[pos],"-fr"));
if (ForceRemove && (st.size() < 3))
{
	Usage(theClient);
	return true;
}

if (ForceRemove)
{
	cmdStr += "-fr ";
	pos++;
}

if(st[pos].substr(0,1) == "#")
	{
	bot->Notice(theClient,"Please use REMGCHAN to remove a BADCHAN gline");
	return false;
	}
string Ident = extractNickUser(st[pos]);
string Hostname = extractHostIP(st[pos]);
if (Ident.empty())
	Hostname = st[pos];
string sgHost;
//If CIDR mask is specified, help out and calculate the CIDR address
if (Hostname.find('/') != string::npos)
	fixToCIDR64(Hostname);
if (!Ident.empty())
	sgHost = Ident +'@'+ Hostname;
else 
	sgHost = Hostname;
cmdStr += sgHost;
bot->removeAllMatchingGlines(sgHost, ForceRemove);
bot->MsgChanLog("%s\n",cmdStr.c_str());
bot->Notice( theClient, "Removal of gline (%s) succeeded\n",sgHost.c_str() ) ;

return true ;
}

}
} // namespace gnuworld

