#include	<vector>
#include	<iostream>
#include	<strstream>
#include	<string>

#include	<ctime>
#include	<cstdlib>

#include	"client.h" 
#include    "cservice.h"
#include	"EConfig.h"
#include	"ip.h"
#include	"Network.h"
#include	"StringTokenizer.h"
#include	"misc.h"
#include	"ELog.h" 
#include    "libpq++.h"

using std::vector ;
using std::endl ;
using std::strstream ;
using std::ends ;
using std::string ;

namespace gnuworld
{

extern "C"
{
  xClient* _gnuwinit(const string& args)
  { 
    return new cservice(args);
  }

} 
 
cservice::cservice(const string& args)
{ 
  /*
   *  Open a connection (via TCP) to the postgres server.
   */

  SQLDb = new PgDatabase("host=defiant.atomicrevs.net dbname=cservice");

  if (SQLDb->ConnectionBad()) {
    elog << "[SQL]: Unable to connect to SQL server." << endl
         << "[SQL]: Postgres error message: " << SQLDb->ErrorMessage() << endl;
    exit(0);
  } else { 
    elog << "[SQL]: Connection established to SQL server." << endl;
  }

  nickName = "W";
  userName = "cservice";
  hostName = "undernet.org";
  userDescription = "For help type: /msg W help"; 
  Mode("+idk");
}

cservice::~cservice()
{
}

int cservice::BurstChannels()
{
ExecStatusType status; // PGSql Status report container.
 
  /* 
   *  Execute statement to retrieve all channels.
   */
  if ((status = SQLDb->Exec( STMT_ALLCHANS )) == PGRES_TUPLES_OK)
  {
    for (int i=0; i < SQLDb->Tuples(); i++) { 
      StringTokenizer data( SQLDb->GetValue(i,0) ) ; 
      MyUplink->JoinChannel(this, data[0], "+tn");
    } 
  }
 
  return xClient::BurstChannels(); 
}

int cservice::OnConnect()
{
  return 0;
}

int cservice::OnPrivateMessage( iClient* theClient, const string& Message )
{ 
StringTokenizer st(Message);
ExecStatusType status; // PGSql Status report container.
 
if( st.empty() )
	{
	return 0 ;
	}

string command(string_upper(st[0]));

if( command == "DUMPDB" )
{
  Notice( theClient, "All Channels in the Database:") ;
  if ( (status = SQLDb->Exec( STMT_ALLCHANS )) == PGRES_TUPLES_OK ) 
  {
    for (int i=0; i < SQLDb->Tuples(); i++) {
      strstream s;
      StringTokenizer data(SQLDb->GetValue(i,0)) ;
      s << "Name: " << data[0] << ends;
      Notice(theClient, s.str()); 
      delete[] s.str();
    }
  }
}

return 0 ;
}

} // namespace euworld
