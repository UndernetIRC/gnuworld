/**
 * NOMODECommand.cc
 * Adds a channel to the bad channels list
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
 * $Id: NOMODECommand.cc,v 1.8 2006/09/26 17:36:00 kewlio Exp $
 */

#include	<string>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Constants.h"
#include	"ccBadChannel.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: NOMODECommand.cc,v 1.8 2006/09/26 17:36:00 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool NOMODECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage( theClient ) ;
	return true ;
	}

if(st[2].size() > channel::MaxName)
	{
	bot->Notice(theClient,"Channel name can't be more than %d characters",
		channel::MaxName);
	return false;
	}

bot->MsgChanLog("NOMODE %s\n",bot->removeSqlChars(st.assemble(1)).c_str());
	    
if(!strcasecmp(st[1],"ADD"))
	{
	if( st.size() < 4 )
	{
	    Usage( theClient ) ;
		return true ;
	}

	ccBadChannel* NewChannel = bot->isBadChannel(st[2]);
	if(NewChannel)
		{
		bot->Notice(theClient,"There is already a NOMODE entry for channel %s",st[2].c_str());
		return true;
		}
	
	NewChannel = new (std::nothrow) ccBadChannel(bot->removeSqlChars(st[2]),
					bot->removeSqlChars(st.assemble(3)),
					theClient->getRealNickUserHost());
	assert(NewChannel != NULL);
	if(!NewChannel->Insert(bot->SQLDb))
		{
		bot->Notice(theClient,"Error while inserting the NOMODE entry into the database");
		return false;
		}
	bot->addBadChannel(NewChannel);
	bot->Notice(theClient,"NOMODE for %s added successfully.", st[2].c_str());
	}
else if(!strcasecmp(st[1],"REM"))
	{
	ccBadChannel* OldChannel = bot->isBadChannel(st[2]);
	if(!OldChannel)
		{
		bot->Notice(theClient,"There is no NOMODE entry for channel %s",st[2].c_str());
		return true;
		}
	
	bot->remBadChannel(OldChannel);
	if(!OldChannel->Delete(bot->SQLDb))
		{
		bot->Notice(theClient,"Error while removing the NOMODE entry from the database");
		return false;
		}
	delete OldChannel;
	bot->Notice(theClient,"NOMODE for %s removed successfully.", st[2].c_str());
	}
else
	{
	bot->Notice(theClient,"NOMODE must get ADD/REM as first parameter");
	return true;
	}

	
return true ;
}

} // namepsace uworld

} // namespace gnuworld
