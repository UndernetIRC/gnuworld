/* cservice_config.h */

#ifndef __CSERVICE_CONFIG_H
#define __CSERVICE_CONFIG_H "$Id: cservice_config.h,v 1.1 2001/03/06 02:34:32 dan_karrels Exp $"

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
 * blah blah blah
 * Somebody who knows what this is please complete
 * this description.
 */
#define MAX_RESULTS 15

#endif // __CSERVICE_CONFIG_H
