/**
 * CHECKCommand.cc
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
 * Display information about a given channel or user.
 */

#include <string>

#include "gnuworld_config.h"
#include "Network.h"
#include "StringTokenizer.h"

#include "dronescan.h"
#include "dronescanCommands.h"

namespace gnuworld {

namespace ds {

void CHECKCommand::Exec( const iClient *theClient,
	const std::string& Message, const sqlUser* )
{
	StringTokenizer st(Message);

	if(st.size() < 2) {
		Usage(theClient);
		return ;
	}

	if(st[1][0] == '#')
		{
		/* We are looking for a channel */
		Channel *theChannel = Network->findChannel(st[1]);

		if(!theChannel) {
			bot->Reply(theClient, "Unable to find channel %s.",
				st[1].c_str());
			return ;
		}

		bot->Reply(theClient, "Checking channel %s:",
			theChannel->getName().c_str());

		double totalEntropy = 0;
		double minEntropy = 0;
		double maxEntropy = 0;
		unsigned int totalUsers = 0;

		Channel::const_userIterator chanItr =
			theChannel->userList_begin();
		for( ; chanItr != theChannel->userList_end() ; ++chanItr )
			{
			++totalUsers;
			ChannelUser *theCU = chanItr->second;
			iClient *targetClient = theCU->getClient();

			double userEntropy = bot->calculateEntropy(targetClient->getNickName());
			totalEntropy += userEntropy;
			if(userEntropy < minEntropy || minEntropy == 0) minEntropy = userEntropy;
			if(userEntropy > maxEntropy) maxEntropy = userEntropy;

			bot->Reply(theClient, "[%s] (%s) %0.3lf %s (%s)",
				bot->isNormal(targetClient) ? "N" : "A",
				targetClient->getCharYY().c_str(),
				userEntropy,
				targetClient->getRealNickUserHost().c_str(),
				targetClient->getDescription().c_str()
				);

#if 0
			bot->Reply(theClient, "  %15s: %0.3lf (%s)",
				targetClient->getNickName().c_str(),
				userEntropy,
				bot->isNormal(targetClient) ? "Normal" : "Abnormal"
				);
#endif
			}

		assert(totalUsers == theChannel->size());

		double averageEntropy = totalEntropy / totalUsers;

		bot->Reply(theClient, "min/avg/max channel entropy : %0.3lf / %0.3lf / %0.3lf",
			minEntropy,
			averageEntropy,
			maxEntropy
			);
		bot->Reply(theClient, "Range above/below/total     : %0.3lf / %0.3lf / %0.3lf",
			maxEntropy - averageEntropy,
			averageEntropy - minEntropy,
			maxEntropy - minEntropy
			);

		bot->checkChannel(theChannel, theClient);

		return ;
		}
	else
		{
		/* We are looking for a user */
		iClient *targetClient = Network->findNick(st[1]);

		if(!targetClient) {
			bot->Reply(theClient, "Unable to find user %s.",
				st[1].c_str());
			return ;
		}

		bot->Reply(theClient, "Checking user %s:",
			targetClient->getNickName().c_str());

		bot->Reply(theClient, "  User entropy: %0.3lf (%s)",
			bot->calculateEntropy(targetClient->getNickName()),
			bot->isNormal(targetClient) ? "Normal" : "Abnormal"
			);

		return ;
		}

	return ;
} // CHECKCommand::Exec(iClient*, const std::string&)

} // namespace ds

} // namespace gnuworld
