#include	<vector>
#include	<iostream>
#include	<strstream>
#include	<string>

#include	<ctime>
#include	<cstdlib>

#include	"client.h" 
#include  	"cservice.h"
#include	"EConfig.h"
#include	"ip.h"
#include	"Network.h"
#include	"StringTokenizer.h"
#include	"misc.h"
#include	"ELog.h" 
#include	"libpq++.h"

using std::vector ;
using std::endl ;
using std::strstream ;
using std::ends ;
using std::string ;

namespace gnuworld
{

/*
 *  Exported function used by moduleLoader to gain an
 *  instance of this module.
 */

extern "C"
{
  xClient* _gnuwinit(const string& args)
  { 
    return new cservice( args );
  }

} 

//-- Connect to the PG SQL database and load the configuration file.
 
cservice::cservice(const string& args)
{ 
	//-- Load in our cservice configuration file.
	cserviceConfig = new EConfig( args ) ;
	string sqlHost = cserviceConfig->Require( "sql_host" )->second;
	string sqlDb = cserviceConfig->Require( "sql_db" )->second;
	
	string Query = "host=" + sqlHost + " dbname=" + sqlDb ;

	elog << "[SQL]: Attempting to connect to " << sqlHost << "; Database: " << sqlDb << endl;

	SQLDb = new PgDatabase( Query.c_str() ) ;

	//-- Make sure we connected to the SQL database; if we didn't we exit entirely.

	if (SQLDb->ConnectionBad ())
	{
		elog << "[SQL]: Unable to connect to SQL server." << endl 
		     << "[SQL]: PostgreSQL error message: " << SQLDb->ErrorMessage () << endl ;

		::exit( 0 ) ;
	}
	else
	{
		elog << "[SQL]: Connection established to SQL server." << endl ;
	}

	//-- Retrieve user, nick, host, and description from the configuration file -- these are all REQUIRED
	//   and the program will exit unless these are present in cservice.conf!

	nickName = cserviceConfig->Require( "nick" )->second ;
	userName = cserviceConfig->Require( "user" )->second ;
	hostName = cserviceConfig->Require( "host" )->second ;
	userDescription = cserviceConfig->Require( "description" )->second ;

	//-- Move this to the configuration file?  This should be fairly static..

	Mode( "+idk" ) ;
}

cservice::~cservice()
{
	delete cserviceConfig ;	
}

int cservice::BurstChannels()
{
	ExecStatusType status; 
 
	//-- We need to join every channel in the database that has been ADDCHAN'd by the
	//   manager.  Various other things must be done, such as setting the topic if AutoTopic
	//   is on, gaining ops if AlwaysOp is on, and so forth.

	if ((status = SQLDb->Exec( "select name,flags,channel_ts,channel_mode,channel_key,channel_limit,description from channel" )) == PGRES_TUPLES_OK)
	{
		for (int i = 0 ; i < SQLDb->Tuples (); i++)
		{
			StringTokenizer data( SQLDb->GetValue( i, 0 ) ) ;
			MyUplink->JoinChannel( this, data[ 0 ], "+tn" ) ;
		}
	}

	return xClient::BurstChannels () ;
}
	
int cservice::OnConnect()
{
	return 0;
}

int cservice::OnPrivateMessage( iClient* theClient, const string& Message )
{ 
	StringTokenizer	st( Message ) ;
	ExecStatusType	status ;

	if( st.empty () )
	{
		return 0 ;
	}

	string	command( string_upper( st[ 0 ] ) ) ;

	// if( command == "COMMAND" ) 
	// {
	//	...perform command actions...
	// }

	if( command == "DUMP" )
	{
		string	dumpMe	= st.assemble( 1 ) ;
		QuoteAsServer( dumpMe ) ;
	}


	return 0 ;
}

} // namespace gnuworld
