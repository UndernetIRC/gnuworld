#include <sstream>

#include "dbHandle.h"

#include "ELog.h"

#include "sqlUser.h"
#include	"misc.h"

namespace gnuworld {

namespace ds {

using std::endl ;
using std::stringstream ;

const sqlUser::flagType sqlUser::F_SUSPENDED	= 0x0001 ;

sqlUser::sqlUser(dbHandle *_SQLDb) :
	user_name(),
	last_seen(0),
	last_updated_by(),
	last_updated(0),
	flags(0),
	SQLDb(_SQLDb)
{
}

sqlUser::~sqlUser()
{
}

void sqlUser::setAllMembers(int row)
{
user_name = SQLDb->GetValue(row, 0);
last_seen = atoi(SQLDb->GetValue(row, 1));
last_updated_by = SQLDb->GetValue(row, 2);
last_updated = atoi(SQLDb->GetValue(row, 3));
flags = atoi(SQLDb->GetValue(row, 4));
access = atoi(SQLDb->GetValue(row, 5));
created = atoi(SQLDb->GetValue(row, 6));
}

bool sqlUser::commit()
{

/* Special case if we have no SQLDb
 * ie if we are a fake user
 */
if( !SQLDb ) { return true; }

stringstream queryString;
queryString	<< "UPDATE users SET "
		<< "last_seen = " << last_seen << ", "
		<< "last_updated_by = '" << last_updated_by << "', "
		<< "last_updated = " << last_updated << ", "
		<< "flags = " << flags << ", "
		<< "access = " << access
		<< " WHERE "
		<< "user_name = '" << user_name << "'"
		;

#ifdef LOG_SQL
elog	<< "sqlUser::commit> "
	<< queryString
	<< endl;
#endif

if( !SQLDb->Exec(queryString ) )
//if(PGRES_COMMAND_OK != status)
	{
	elog << "sqlUser::commit> " << SQLDb->ErrorMessage();
	return false;
	}

return true;
}


bool sqlUser::insert()
{
stringstream insertString;
insertString	<< "INSERT INTO users "
		<< "(user_name, created, last_seen, last_updated, last_updated_by, flags, access) "
		<< "VALUES "
		<< "("
		<< "'" << user_name << "', "
		<< created << ", "
		<< last_seen << ", "
		<< last_updated << ", "
		<< "'" << last_updated_by << "', "
		<< flags << ", "
		<< access
		<< ")"
		;

#ifdef LOG_SQL
elog	<< "sqlUser::insert> "
	<< insertString
	<< endl;
#endif

if( !SQLDb->Exec(insertString ) )
//if(PGRES_COMMAND_OK != status)
	{
	elog << "sqlUser::insert> " << SQLDb->ErrorMessage();
	return false;
	}

return true;
} // sqlUser::insert()


bool sqlUser::remove()
{
stringstream deleteString;
deleteString	<< "DELETE FROM users "
		<< "WHERE user_name = '" << user_name << "'"
		;

if( !SQLDb->Exec(deleteString ) )
//if(PGRES_COMMAND_OK != status)
	{
	elog << "sqlUser::delete> " << SQLDb->ErrorMessage();
	return false;
	}

return true;
}

} // namespace ds

} // namespace gnuworld
