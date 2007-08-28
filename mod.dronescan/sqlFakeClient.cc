#include <string>
#include <sstream>
#include <iostream>

#include "dbHandle.h"

#include "ELog.h"

#include "constants.h"
#include "sqlFakeClient.h"
#include	"misc.h"

namespace gnuworld {

namespace ds {

using std::string ;
using std::endl;
using std::stringstream;

const sqlFakeClient::flagType sqlFakeClient::F_ACTIVE	= 0x0001 ;

sqlFakeClient::sqlFakeClient(dbHandle *_SQLDb) :
	id(0),
	nickname(),
	username(),
	hostname(),
	realname(),
	createdBy_i(0),
	createdBy_s(),
	createdOn(::time(0)),
	lastUpdated(::time(0)),
	flags(0),
	SQLDb(_SQLDb)
{
}

sqlFakeClient::~sqlFakeClient()
{
}

string sqlFakeClient::getFlagsString() const
{
/* If this grows, alter the field size in LISTCommand.cc */

stringstream theFlags;
if(flags & F_ACTIVE)	theFlags << "A";

return theFlags.str();
}

void sqlFakeClient::setAllMembers(int row)
{
id   = atoi(SQLDb->GetValue(row, 0));
nickname  = SQLDb->GetValue(row, 1);
username  = SQLDb->GetValue(row, 2);
hostname  = SQLDb->GetValue(row, 3);
realname  = SQLDb->GetValue(row, 4);
createdBy_s = SQLDb->GetValue(row, 5);
createdBy_i = atoi(SQLDb->GetValue(row, 6));
createdOn = atoi(SQLDb->GetValue(row, 7));
lastUpdated = atoi(SQLDb->GetValue(row, 8));
flags = atoi(SQLDb->GetValue(row, 9));
}

bool sqlFakeClient::insert()
{
/* First insert a fake record */
stringstream fakeInsert;
fakeInsert	<< "INSERT INTO fakeclients VALUES ("
		<< "DEFAULT,"
		<< "'" << nickname << "', "
		<< "'" << username << "', "
		<< "'" << hostname << "', "
		<< "'" << realname << "', "
		<< createdBy_i << ", "
		<< "now()::abstime::int4, "
		<< "now()::abstime::int4, 0"
		<< ")"
		;

if(!SQLDb->Exec(fakeInsert)) {
	return false;
}

/* Fake record now exists. Select the row and let setAllMembers do its thing.
 * This is so we pull back the new ID from the database. */

stringstream selectFake;
selectFake	<< sql::fakeclients;

if( !SQLDb->Exec(selectFake, true ) )
//if( status != PGRES_TUPLES_OK )
	return false;

setAllMembers(0);

return true;

}

string sqlFakeClient::getNickUserHost() const
{
stringstream theNickUserHost;
theNickUserHost	<< "("
		<< id
		<< ") "
		<< nickname
		<< "!"
		<< username
		<< "@"
		<< hostname
		<< " ["
		<< realname
		<< "]"
		;

return theNickUserHost.str();
}

} // namespace ds

} // namespace gnuworld
