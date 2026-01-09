/**
 * cservice_config.h
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
 * $Id: cservice_config.h,v 1.13 2006/09/26 17:36:04 kewlio Exp $
 */

#ifndef __CSERVICE_CONFIG_H
#define __CSERVICE_CONFIG_H "$Id: cservice_config.h,v 1.13 2006/09/26 17:36:04 kewlio Exp $"

/**
 * Define this if you wish for general debugging information
 * be logged.
 */
#define LOG_DEBUG

/**
 * The maximum number of results to return to the user on
 * an LBANLIST query.
 */
#define MAX_LBAN_RESULTS 10

/**
 * The maximum number of results to return to the user on
 * an ACCESS query.
 */
#define MAX_ACCESS_RESULTS 15

/**
 * The maximum number of search results to return to the user.
 */
#define MAX_SEARCH_RESULTS 10

/**
 * Do you want NEWPASS command or not?
 */

#undef USE_NEWPASS

/**
 * Do you want to use the Noteserv functionality?
 */

#undef USE_NOTES

/**
 * Do you want silly oper-type people to mess around with channels?
 */

#undef USE_OPERPARTJOIN

/**
 * Do you want users to have the ability to set their own MAXLOGINS setting?
 */

#undef USE_SETMAXLOGINS

/**
 * Define this if you want to enable the HELLO command.
 */
#undef ALLOW_HELLO

 /**
 * Define this if you want to enable the WELCOME feature.
 */
#undef USE_WELCOME

/**
 * Define this if you want the IP restriction to default to REJECT when no
 * entries are in the SQL table.  Default setting allows login from any IP
 * if there are no entries for the username in the SQL table.
 */
#define IPR_DEFAULT_REJECT

/**
 * Define this if you want TOTP authentication, note you must have liboath
 * installed.
 */
#define TOTP_AUTH_ENABLED

/**
 * Define this if you want channel registration process to be handled internally within gnuworld
 * otherwise you can use external script
 */
#define USE_INTERNAL_THE_JUDGE

/**
 * Enable NOVOICE channel flag and function?
 * Prevents anyone voiced on a channel having this flag enabled
 */
#undef USE_NOVOICE

/**
 * Define this if you want to enable NOTAKE channel flag for your network,
 * it will report and prevent any 'take over' attempt: banning through X the *!*@* mask
 */
#define USE_NOTAKE

/**
 * Enable FLOODNET channel flag and function?
 * Protection against mass channel messages, channel notices, channel CTCP's
 */
#define USE_FLOODPRO

/*
 * Enable this Under CARREFULL CONSIDERATION, recommended for smaller networks, where admins can
 * take care of possible abuse of the function
 */
#undef GLINE_ON_FLOODPRO

/*
 * Enable this to enable the following features currently not released in ircu:
 * - Send new AC messages upon changes of sqlUser flags.
 * - TLS connections
 * - SASL authentication
 * - Network config
 * - Autohide hostmask (+x)
 * All servers on the network must run the appropriate version of ircu
 * in order not to get protocol violation messages.
 */
#undef NEW_IRCU_FEATURES

/**
 * Define the number of iterations for the SCRAM record generation.
 * Must not exceed 4096.
 */
#define CRYPT_ITERATIONS 4096

/**
 * Define the length of the salt for the SCRAM record generation.
 * Must be in the range of 12 to 64 bytes.
 */
#define CRYPT_SALT_LEN 16

#endif // __CSERVICE_CONFIG_H
