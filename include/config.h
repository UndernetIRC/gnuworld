/* config.h
 * $Id: config.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $
 */

#ifndef __CONFIG_H
#define __CONFIG_H "$Id: config.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $"

/* config.h */
/* EWorld main configuration file */
/* blah blah blah goes here, but who cares,
 * Damnit Jim! Im a programmer not an author!
 */

/* EDEBUG
 * This define is pretty obvious...Will probably consume
 * massive quantities of CPU when debugging, recommended
 * only for debugging/testing/optimization purposes
 */
#define EDEBUG

#ifdef EDEBUG

/* DEBUGFILE
 * The file that the debugger will write to
 */
#define DEBUGFILE "debug.log"

#endif // EDEBUG

/* CONFFILE
 * This is the name of the file that Eworld will read
 * it's server characteristics from
 */
#define CONFFILE "Eworld.conf"

/* LOGFILE
 * Name of the file that eworld logs to =P
 */
#define LOGFILE "Eworld.log"

/* LOG_SOCKET
 * This define allows to log incoming and outgoing data from/to sockets.
 * WARNING!!! Use it at your own risk! =P It sucks CPU and DISK!
 * --NeXTPhase
 */
#define LOG_SOCKET

#ifdef LOG_SOCKET

/* LOG_SOCKET_NAME
 * The name of the file to which to write the raw data read
 * from the network
 */
#define LOG_SOCKET_NAME "socket.log"

#endif /* LOG_SOCKET */

#endif /* __CONFIG_H */
