/* sqlChannel.cc */

#include	<string>

#include	"sqlChannel.h"

const char sqlChannel_h_rcsId[] = __SQLCHANNEL_H ;
const char sqlChannel_cc_rcsId[] = "$Id: sqlChannel.cc,v 1.2 2000/12/11 00:46:31 gte Exp $" ;

sqlChannel::sqlChannel(PgDatabase* SQLDb, const string& channelName)
{
/*
 *  With the open database handle 'SQLDb', retrieve information about
 *  'channelName' and propegate our member variables.
 */

}

sqlChannel::~sqlChannel()
{
}

