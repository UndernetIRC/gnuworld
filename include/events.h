/**
 * Events.h
 * This file defines the basic network and channel events that
 * the GNUWorld server may distribute to its services clients.
 * Hopefully most of these are pretty self explanatory.
 * This file was originally created by Orlando Bassotto.
 *
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
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
 * $Id: events.h,v 1.13 2002/07/31 03:14:04 dan_karrels Exp $
 */

#ifndef __EVENTS_H
#define __EVENTS_H "$Id: events.h,v 1.13 2002/07/31 03:14:04 dan_karrels Exp $"

namespace gnuworld
{

/**
 * This is used to enumerate the possible network (non-channel)
 * events that gnuworld will track and deliver to registered
 * clients.
 */
enum
{
	EVT_OPER,
	EVT_NETBREAK,
	EVT_NETJOIN,
	EVT_BURST_CMPLT,
	EVT_BURST_ACK,
	EVT_GLINE,
	EVT_REMGLINE,
	EVT_JUPE,
	EVT_UNJUPE,
	EVT_QUIT,
	EVT_KILL,
	EVT_NICK,
	EVT_CHNICK,
	EVT_ACCOUNT,
	EVT_RAW,

	// EVT_NOOP must always be last
	EVT_NOOP

} ;

/**
 * The type used to represent network (non-channel) events.
 */
typedef int eventType ;

/**
 * This enumerates the possible channel events that gnuworld will
 * track and deliver to registered clients.
 */
enum
{
	EVT_JOIN = EVT_NOOP,
	EVT_PART,
	EVT_TOPIC,
	EVT_KICK,
	EVT_CREATE
} ;

/**
 * The type used to store possible channel events.
 */
typedef int channelEventType ;

/**
 * Arguments for the various events:
 * EVT_KILL
 *  1) iClient* or iServer* - source, could be NULL
 *  2) iClient* - target
 *  3) string* - reason
 * EVT_QUIT
 *  1) iClient*
 * EVT_SQUIT
 *  1) iServer* - server being squit
 *  2) string* - source
 *  3) string* - reason
 * EVT_BURST_ACK
 *  1) iServer*
 * EVT_NICK
 *  1) iClient*
 * EVT_NETJOIN
 *  1) iServer* - newly joined server
 *  2) iServer* - newly joined server's uplink server
 * EVT_BURST_ACK
 *  1) iServer*
 * EVT_CHNICK
 *  1) iClient* (nick updated)
 *  2) string* (old nick)
 * EVT_GLINE
 *  1) Gline*
 * EVT_REMGLINE
 *  1) Gline*
 * EVT_ACCOUNT
 *  1) iClient*
 *
 * Channel Events
 * --------------
 * EVT_CREATE
 *  1) iClient*
 * EVT_PART
 *  1) iClient*
 * EVT_KICK
 *  1) iClient*
 * EVT_JOIN
 *  1) iClient*
 *  2) ChannelUser*
 */

} // namespace gnuworld

#endif // __EVENTS_H
