/* sqlChannel.cc */

#include	<string>

#include	"sqlChannel.h"

const char sqlChannel_h_rcsId[] = __SQLCHANNEL_H ;
const char sqlChannel_cc_rcsId[] = "$Id: sqlChannel.cc,v 1.3 2000/12/11 02:04:28 gte Exp $" ;

sqlChannel::sqlChannel(PgDatabase* SQLDb, const string& channelName)
{
/*
 *  With the open database handle 'SQLDb', retrieve information about
 *  'channelName' and propegate our member variables.
 */

	ExecStatusType status;
    string queryString =  "SELECT name,flags,channel_ts,channel_mode,channel_key,channel_limit,description FROM channels WHERE name = " + channelName;
	if ((status = SQLDb->Exec(queryString.c_str())) == PGRES_TUPLES_OK)
	{ 
	// Check ->tuples == 1.
	//StringTokenizer data( SQLDb->GetValue( 0, 0 ) ) ; 
	}

}

sqlChannel::~sqlChannel()
{
}

