/**
 * chanfix-xclient.cc
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
 */

#include <string>

#include "StringTokenizer.h"

#include "chanfix.h"
#include "chanfix-commands.h"

namespace gnuworld {

namespace chanfix {

void chanfix::OnAttach()
{
	xClient::OnAttach();

	/* Lastly, kick off any timers we might need */
	time_t next;

	next = ::time(0) + confStartDelay;
	timerCount = MyUplink->RegisterTimer(next, this, 0);
}


void chanfix::BurstChannels()
{
	xClient::BurstChannels();

	MyUplink->JoinChannel(this, confConsoleChannel, confConsoleModes);
}


void chanfix::OnCTCP( iClient *theClient, const std::string& CTCP,
	const std::string& Message, bool )
{
	StringTokenizer st(CTCP);

	if(st.empty()) return;

	std::string Command = string_upper(st[0]);

	if("DCC" == Command) {
		DoCTCP(theClient, CTCP, "REJECT");
	} else if("PING" == Command) {
		DoCTCP(theClient, CTCP, Message);
	} else if("VERSION" == Command) {
		DoCTCP(theClient, CTCP, "GNUWorld ChanFix v0.0.1");
	}
}


void chanfix::OnPrivateMessage( iClient *theClient,
	const std::string& Message, bool)
{
	/* Only speak to opers */
	if( ! theClient->isOper() ) return;

	StringTokenizer st(Message);

	if( st.empty() ) return;

	std::string Command = string_upper(st[0]);
	commandMapType::iterator commandHandler = commandMap.find(Command);

	if( commandHandler == commandMap.end() ) {
		Notice(theClient, "Invalid command: %s", Command.c_str());
		return;
	}

	commandHandler->second->Exec(theClient, Message);
}


void chanfix::OnTimer( const TimerHandler::timerID& theTimer , void* _data )
{
	xClient::OnTimer( theTimer , _data );

	time_t next = ::time(0);

	if( theTimer == timerCount ) {
		doCountUpdate();

		next += confPeriod;

		timerCount = MyUplink->RegisterTimer(next, this, 0);
	} else {
		assert(0);
	}
}

} // namespace chanfix

} // namespace gnuworld
