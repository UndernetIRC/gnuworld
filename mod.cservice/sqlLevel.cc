/* 
 * sqlLevel.cc
 * 
 * Storage class for accessing channel user/level information either from the backend
 * or internal storage.
 * 
 * $Id: sqlLevel.cc,v 1.1 2000/12/22 00:29:32 gte Exp $
 */
 
#include	<strstream>
#include	<string> 
#include	<cstring> 
#include	"ELog.h"
#include	"misc.h"
#include	"sqlLevel.h"
#include	"sqlUser.h"
#include	"sqlChannel.h"

using std::string ; 
using std::endl ; 
 
const char sqlLevel_h_rcsId[] = __SQLLEVEL_H ;
const char sqlLevel_cc_rcsId[] = "$Id: sqlLevel.cc,v 1.1 2000/12/22 00:29:32 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld ;


sqlLevel::sqlLevel(PgDatabase* _SQLDb)
{
	/*
	 *  Constructor, sets the database handle.
	 */
	SQLDb = _SQLDb;
}
 
bool sqlLevel::loadData(unsigned int userID, unsigned int channelID)
{
	/*
	 * Fetch a matching Level record for this channel and user ID combo.
	 */ 

	ExecStatusType status;
	elog << "sqlLevel> Attempting to load level data for channel-id: " << channelID << " and user-id: " << userID << endl;
	
	strstream queryString;
	queryString << "SELECT channel_id,user_id,access,flags,suspend_expires,suspend_by,added,added_by,last_Modif,last_Modif_By,last_Updated FROM levels WHERE channel_id = " << channelID << " AND user_id = " << userID << ends;

	elog << "sqlQuery> " << queryString.str() << endl;

	if ((status = SQLDb->Exec(queryString.str())) == PGRES_TUPLES_OK)
	{ 
		/*
		 *  If this combo doesn't exist, we won't get any rows back.
		 */ 

	if(SQLDb->Tuples() < 1) { 
			return (false);
		} 
		setAllMembers(); 
		delete[] queryString.str() ;
		return (true);
	} 

	delete[] queryString.str() ;
	return (false); 
} 


void sqlLevel::setAllMembers()
{
	/*
	 *  Support function for both loadData's.
	 *  Assumes SQLDb contains a valid results set for all Level information.
	 */

	channel_id = atoi(SQLDb->GetValue(0, 0));
	user_id = atoi(SQLDb->GetValue(0, 1));
	access = atoi(SQLDb->GetValue(0, 2));
	flags = atoi(SQLDb->GetValue(0, 3));
	suspend_expires = atoi(SQLDb->GetValue(0, 4));
	suspend_by = SQLDb->GetValue(0, 5);
	added = atoi(SQLDb->GetValue(0, 6));
	added_by = SQLDb->GetValue(0, 7);
	last_modif = atoi(SQLDb->GetValue(0, 8));
	last_modif_by = SQLDb->GetValue(0, 9);
	last_update = atoi(SQLDb->GetValue(0, 10)); 
}

sqlLevel::~sqlLevel()
{
}

} // Namespace gnuworld.
