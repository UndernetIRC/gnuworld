#ifndef __CSERVICE_H
#define __CSERVICE_H "$Id: cservice.h,v 1.3 2000/12/10 19:10:28 gte Exp $"

#include	<string>
#include	<vector>

#include	<ctime>

#include	"client.h"
#include	"iClient.h"
#include	"iServer.h"
#include	"EConfig.h"
 
#define STMT_ALLCHANS "SELECT channel from channels" // Provide a result set with 1 column, 'Channel Name'.

using std::string ;
using std::vector ;

class PgDatabase;

namespace gnuworld
{


class cservice : public xClient
{

public:
	cservice(const string& args);
	virtual ~cservice();

	virtual int OnConnect();
    virtual int BurstChannels();
	virtual int OnPrivateMessage( iClient*, const string& );

protected:

    PgDatabase* SQLDb; /* PostgreSQL Database */
	EConfig* cserviceConfig; /* Configfile */
 
} ;

} // namespace euworld

#endif // __CSERVICE_H
