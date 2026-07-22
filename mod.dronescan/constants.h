/**
 * constants.h
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
 * Contains things that are used more than once across
 * the module but which need to remain uniform.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

namespace gnuworld {

namespace ds {

namespace sql {

const std::string fakeclients = "SELECT fc.id, fc.nickname, "
                                "fc.username, fc.hostname, fc.realname, u.user_name, "
                                "fc.created_by, fc.created_on, fc.last_updated, fc.flags "
                                "FROM fakeclients AS fc JOIN users AS u ON "
                                "fc.created_by=u.id";

} // namespace sql

/**
 * Bitmask constants for spam_events.target.
 * Values correspond to the traffic source(s) that an event monitors.
 * Combine with bitwise OR; ALL (255) matches every source.
 *
 *   CHAN_PRIV = 1   channel PRIVMSG (text sent to a channel)
 *   PRIVMSG   = 2   PRIVMSG sent directly to the bot or a spy client
 *   CHAN_NOT   = 4   NOTICE sent to a channel
 *   PART      = 8   part messages
 *   QUIT      = 16  quit messages (matching deferred - see OnEvent EVT_QUIT)
 *   NOTICE    = 32  NOTICE sent directly to the bot or a spy client
 *   CTCP_PRIV = 64  CTCP sent directly to the bot or a spy client
 *   CTCP_CHAN = 128 CTCP sent to a channel, seen by the bot or a spy client
 *                   (e.g. CTCP ACTION/"/me", or a DCC request - match DCC with a
 *                   regex like "^DCC" scoped to ctcp_priv/ctcp_chan, no separate DCC bit)
 *   ALL       = 255 all of the above
 */
namespace spam_target {

const int CHAN_PRIV = 1;   // channel PRIVMSG
const int PRIVMSG   = 2;   // PRIVMSG sent directly to the bot or a spy client
const int CHAN_NOT   = 4;   // NOTICE sent to a channel
const int PART      = 8;   // part messages
const int QUIT      = 16;  // quit messages
const int NOTICE    = 32;  // NOTICE sent directly to the bot or a spy client
const int CTCP_PRIV = 64;  // CTCP sent directly to the bot or a spy client
const int CTCP_CHAN = 128; // CTCP sent to a channel, seen by the bot or a spy client
const int ALL       = 255; // all of the above

} // namespace spam_target

} // namespace ds

} // namespace gnuworld

#endif
