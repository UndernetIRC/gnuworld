/* config.h
 * $Id: config.h,v 1.11 2002/02/24 21:36:38 mrbean_ Exp $
 */

#ifndef __CONFIG_H
#define __CONFIG_H "$Id: config.h,v 1.11 2002/02/24 21:36:38 mrbean_ Exp $"

/* config.h */
/* GNUWorld main configuration file */
/* blah blah blah goes here, but who cares,
 * Damnit Jim! Im a programmer not an author!
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
#define CONFFILE "gnuworld.conf"

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
 * HIDDEN_HOST
 * The suffix to be appended to the authenticated username
 * to form the hidden host-mask when a user sets +x.
 */
#define HIDDEN_HOST ".users.undernet.org"

/**
 * TOPIC_TRACK
 * Set if you want gnuworld to track topics sets 
 */

#undef TOPIC_TRACK

#endif /* __CONFIG_H */
