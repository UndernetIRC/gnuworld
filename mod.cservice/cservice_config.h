/* cservice_config.h */

#ifndef __CSERVICE_CONFIG_H
#define __CSERVICE_CONFIG_H "$Id: cservice_config.h,v 1.3 2002/02/06 18:56:25 gte Exp $"

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
 * Define this if you want your users to be able to login
 * to the same account more than once at the same time.
 */

#undef MULTI_AUTH

#endif // __CSERVICE_CONFIG_H
