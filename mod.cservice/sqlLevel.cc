/* 
 * sqlLevel.cc
 * 
 * Storage class for accessing channel user/level information either from the backend
 * or internal storage.
 * 
 * $Id: sqlLevel.cc,v 1.3 2001/01/02 07:55:12 gte Exp $
 */
 
#include	<strstream>
#include	<string> 
#include	<cstring> 
#include	"ELog.h"
#include	"misc.h"
#include	"sqlLevel.h"
#include	"sqlUser.h"
#include	"sqlChannel.h"
#include	"constants.h"

using std::string ; 
using std::endl ; 
 
const char sqlLevel_h_rcsId[] = __SQLLEVEL_H ;
const char sqlLevel_cc_rcsId[] = "$Id: sqlLevel.cc,v 1.3 2001/01/02 07:55:12 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld ;


sqlLevel::sqlLevel(PgDatabase* _SQLDb)
{
	/*
	 *  Constructor, sets the database handle.
	 */
	SQLDb = _SQLDb;
	forced = false;
}
 
bool sqlLevel::loadData(unsigned int userID, unsigned int channelID)
{
	/*
	 * Fetch a matching Level record for this channel and user ID combo.
	 */ 

	ExecStatusType status;
	elog << "sqlLevel::loadData> Attempting to load level data for channel-id: " << channelID << " and user-id: " << userID << endl;
	
	strstream queryString;
	queryString << "SELECT " << sql::level_fields 
		<< " FROM levels WHERE channel_id = " << channelID 
		<< " AND user_id = " << userID 
		<< ends;

	elog << "sqlLevel::loadData> " << queryString.str() << endl;

	if ((status = SQLDb->Exec(queryString.str())) == PGRES_TUPLES_OK)
	{ 
		/*
		 *  If this combo doesn't exist, we won't get any rows back.
		 */ 

	if(SQLDb->Tuples() < 1) { 
			return (false);
		} 
		setAllMembers(0); // Fetch dat from row '0'
		delete[] queryString.str() ;
		return (true);
	} 

	delete[] queryString.str() ;
	return (false); 
} 


void sqlLevel::setAllMembers(int row)
{
	/*
	 *  Support function for both loadData's.
	 *  Assumes SQLDb contains a valid results set for all Level information.
	 */

	channel_id = atoi(SQLDb->GetValue(row, 0));
	user_id = atoi(SQLDb->GetValue(row, 1));
	access = atoi(SQLDb->GetValue(row, 2));
	flags = atoi(SQLDb->GetValue(row, 3));
	suspend_expires = atoi(SQLDb->GetValue(row, 4));
	suspend_by = SQLDb->GetValue(row, 5);
	added = atoi(SQLDb->GetValue(row, 6));
	added_by = SQLDb->GetValue(row, 7);
	last_modif = atoi(SQLDb->GetValue(row, 8));
	last_modif_by = SQLDb->GetValue(row, 9);
	last_update = atoi(SQLDb->GetValue(row, 10)); 
}

sqlLevel::~sqlLevel()
{
}

} // Namespace gnuworld.
