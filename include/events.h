/* Events.h
 * This file defines the basic network and channel events that
 * the GNUWorld server may distribute to its services clients.
 * Hopefully most of these are pretty self explanatory.
 */

#ifndef __EVENTS_H
#define __EVENTS_H "$Id: events.h,v 1.2 2000/07/08 01:41:05 dan_karrels Exp $"

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
	EVT_CLONES,
	EVT_TSC,
	EVT_BURST_CMPLT,
	EVT_BURST_ACK,
	EVT_GLINE,
	EVT_AUTOGLINE,
	EVT_REMGLINE,
	EVT_JUPE,
	EVT_UNJUPE,
	EVT_NICK_JUPE,
	EVT_NICK_UNJUPE,
	EVT_AUTO_JUPE,
	EVT_QUIT,
	EVT_KILL,
	EVT_SIGNAL,
	EVT_NICK,

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
	EVT_MODE,
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
 *  1) iClient* or iServer* - source
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
 *
 *
 * Channel Events
 * --------------
 * EVT_CREATE
 *  1) Channel*
 *  2) iClient*
 * EVT_PART
 *  1) Channel*
 *  2) iClient*
 * EVT_KICK
 *  1) Channel*
 *  2) iClient*
 * EVT_JOIN
 *  1) Channel*
 *  2) iClient*
 * EVT_MODE
 *  1) Channel*
 *  2) string* - mode string
 */

} // namespace gnuworld

#endif // __EVENTS_H
