/* cservice_config.h */

#ifndef __CSERVICE_CONFIG_H
#define __CSERVICE_CONFIG_H "$Id: cservice_config.h,v 1.8 2002/12/30 03:43:39 gte Exp $"

/**
 * Define this if you wish for all SQL queries to be sent
 * the standard logging stream (elog).
 */
#define LOG_SQL

/**
 * Define this if you wish to log all cache hits.
 */
#define LOG_CACHE_HITS

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

#define USE_NEWPASS

/**
 * Do you want to use the Noteserv functionality?
 */

#define USE_NOTES

/**
 * Do you want silly oper-type people to mess around with channels?
 */

#define USE_OPERPARTJOIN

/**
 * Do you want users to have the ability to set their own MAXLOGINS setting?
 */

#define USE_SETMAXLOGINS

#endif // __CSERVICE_CONFIG_H
