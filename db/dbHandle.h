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
	dbHandle( const std::string& dbHost,
		const unsigned short int dbPort,
		const std::string& dbName,
		const std::string& userName,
		const std::string& password ) throw( std::exception )
#ifdef HAVE_PGSQL
	: pgsqlDB( dbHost,
		dbPort,
		dbName,
		userName,
		password )
#endif
	{}
	dbHandle( const std::string& connectInfo ) throw( std::exception )
#ifdef HAVE_PGSQL
	: pgsqlDB( connectInfo )
#endif
	{}
	virtual ~dbHandle() {}

} ;

} // namespace gnuworld

#endif // __DBHANDLE_H
