/**
 * config.h
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
 * $Id: config.h,v 1.18 2003/06/03 20:43:29 dan_karrels Exp $
 */

#ifndef __CONFIG_H
#define __CONFIG_H "$Id: config.h,v 1.18 2003/06/03 20:43:29 dan_karrels Exp $"

/* config.h */
/* GNUWorld main configuration file */
/* All core settings are set/defined here.
 */

/**
 * EDEBUG
 * This define is pretty obvious...Will probably consume
 * massive quantities of CPU when debugging, recommended
 * only for debugging/testing purposes.
 */
#define EDEBUG

#ifdef EDEBUG

/**
 * DEBUGFILE
 * The file to which the debugger will write.
 */
#define DEBUGFILE "debug.log"

#endif // EDEBUG

/**
 * CONFFILE
 * This is the name of the default file from which GNUWorld
 * will read its server characteristics.
 */
#define CONFFILE "GNUWorld.conf"

/**
 * LOGFILE
 * Name of the file to which gnuworld logs :)
 */
#define LOGFILE "gnuworld.log"

/**
 * LOG_SOCKET
 * Define this variable to enable logging of incoming raw data.
 * This is very useful for offline debugging.
 */
#define LOG_SOCKET

#ifdef LOG_SOCKET

/**
 * LOG_SOCKET_NAME
 * The name of the default file to which to write raw data
 * from the network.
 */
#define LOG_SOCKET_NAME "socket.log"

#endif /* LOG_SOCKET */

/**
 * CLIENT_DESC
 * Define this variable if you want gnuworld to track the
 * real name (description) of each network client.  This
 * can use quite a bit more memory on large networks.
 */
#undef CLIENT_DESC

/**
 * MAXLOOPCOUNT_DEFAULT
 * This variable represents how many times to attempt a system
 * call before giving up.
 */
#define MAXLOOPCOUNT_DEFAULT 10

/**
 * MAX_CHAN_MODES
 * The maximum number of channel modes per command.
 */
#define MAX_CHAN_MODES 10

/**
 * TOPIC_TRACK
 * Set if you want gnuworld to track topics in the network channels
 */
#undef TOPIC_TRACK

#endif /* __CONFIG_H */
