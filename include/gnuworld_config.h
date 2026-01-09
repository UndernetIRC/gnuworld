/**
 * gnuworld_config.h
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
 * $Id: gnuworld_config.h,v 1.4 2006/09/26 17:35:57 kewlio Exp $
 */

#ifndef __GNUWORLD_CONFIG_H
#define __GNUWORLD_CONFIG_H "$Id: gnuworld_config.h,v 1.4 2006/09/26 17:35:57 kewlio Exp $"

/* GNUWorld main configuration file */
/* All core settings are set/defined here.
 */
#include	"defs.h"

/**
 * CONFFILE
 * This is the name of the default file from which GNUWorld
 * will read its server characteristics.
 */
#define CONFFILE "GNUWorld.conf"

/**
 * ASUKA
 * Define this variable if gnuworld will be running on Asuka.
 * Enabling this by default will not have adverse effects on gnuworld's
 * functionality, except to increase memory usage a bit.
 */
#undef ASUKA

/**
 * SRVX
 * Define this variable if gnuworld will be running on srvx.
 * Enabling this by default will not have adverse effects on gnuworld's
 * functionality, except to increase memory usage a bit.
 */
#undef SRVX

/**
 * MAX_CHAN_MODES
 * The maximum number of channel modes per command.
 */
#define MAX_CHAN_MODES 6

/**
 * MAX_KEY_LENGTH
 * The maximum number of characters in a channel key.
 */
#define MAX_KEY_LENGTH 23

/**
 * MAX_NICK_LENGTH
 * The maximum number of characters in a nickname.
 */
#define MAX_NICK_LENGTH 12

/**
 * MAX_TOPIC_LENGTH
 * The maximum number of characters in a topic.
 */
#define MAX_TOPIC_LENGTH 145

/**
 * TOPIC_TRACK
 * Set if you want gnuworld to track topics in the network channels
 */
#define TOPIC_TRACK

/**
 * NO_FINGERPRINT_BURST
 * Disable this if gnuworld is linked to a hub that doesn't support fingerprint burst.
 * This should only be enabled when the uplink is running ircu with tls support.
 */
#define NO_FINGERPRINT_BURST

/**
 * USE_THREAD
 * Set this to enable mutexes on shared objects. Relevant for running
 * modules with threading.
 */
#define USE_THREAD

#endif /* __GNUWORLD_CONFIG_H */
