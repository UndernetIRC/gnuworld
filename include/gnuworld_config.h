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
 * TOPIC_TRACK
 * Set if you want gnuworld to track topics in the network channels
 */
#define TOPIC_TRACK

/**
 * DAEMON
 * Set this to make GNUWorld fork into the background when run.
 * default is OFF to maintain original operation
 */
#undef DAEMON

#endif /* __GNUWORLD_CONFIG_H */
