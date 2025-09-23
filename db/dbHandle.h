/**
 * dbHandle.h
 */

#ifndef __DBHANDLE_H
#define __DBHANDLE_H "$Id: dbHandle.h,v 1.1 2007/08/28 16:09:58 dan_karrels Exp $"

#include	<string>
#include	<exception>

#include	"defs.h"

#ifdef HAVE_PGSQL
#include	"pgsqlDB.h"
#endif

namespace gnuworld
{

class dbHandle
#ifdef HAVE_PGSQL
 : public pgsqlDB
#endif
{
public:
	dbHandle( xClient* bot,
		const std::string& dbHost,
		const unsigned short int dbPort,
		const std::string& dbName,
		const std::string& userName,
		const std::string& password )
#ifdef HAVE_PGSQL
		: pgsqlDB( bot,
		dbHost,
		dbPort,
		dbName,
		userName,
		password )
#endif
	{}
	dbHandle( xClient* bot, const std::string& connectInfo )
#ifdef HAVE_PGSQL
	: pgsqlDB( bot, connectInfo )
#endif
	{}
	virtual ~dbHandle() {}

} ;

} // namespace gnuworld

#endif // __DBHANDLE_H
