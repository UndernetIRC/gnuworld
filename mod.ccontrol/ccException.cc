/* 
 * ccException.cc
 * 
 * Exception class
 * 
 * $Id: ccException.cc,v 1.8 2002/07/02 11:38:28 mrbean_ Exp $
 */
 
#include	<sstream>
#include	<string> 

#include	<ctime>
#include	<cstring> 
#include	<cstdlib>

#include	"ELog.h"
#include	"misc.h"
#include	"match.h"
#include	"ccException.h" 
#include	"ccontrol.h"

const char ccException_h_rcsId[] = __CCEXCEPTION_H ;
const char ccException_cc_rcsId[] = "$Id: ccException.cc,v 1.8 2002/07/02 11:38:28 mrbean_ Exp $" ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::stringstream ;
using std::ends ;

namespace uworld
{

//extern unsigned int dbConnected;

unsigned int ccException::numAllocated = 0;

ccException::ccException(PgDatabase* _SQLDb)
 : Host(string()),
   Connections(0),
   AddedBy(string()),
   AddedOn(0),
   SQLDb(_SQLDb)
{
++numAllocated;
}

ccException::~ccException()
{
--numAllocated;
}

int ccException::loadData(const string& HostName)
{

static const char Main[] = "SELECT * FROM Exceptions WHERE lower(Host) = '";

if((!dbConnected) || !(SQLDb))
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< string_lower(HostName)
		<< "'"
		<< ends;

elog	<< "ccException::loadData> "
	<< theQuery.str().c_str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;

if( (PGRES_TUPLES_OK != status) && (SQLDb->Tuples() > 0) )
	{
	return false;
	}

Host = SQLDb->GetValue(0,0);
Connections = atoi(SQLDb->GetValue(0,1));
AddedBy = SQLDb->GetValue(0,2);
AddedOn = atoi(SQLDb->GetValue(0,3));

return true;

}

int ccException::updateData()
{
static const char *Main = "UPDATE Exceptions SET AddedBy = '";

if(!dbConnected)	
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< AddedBy
		<< "', Connections = "
		<< Connections
		<< ", AddedOn = "
		<< AddedOn
		<< "' WHERE lower(Host) = '" 
		<< string_lower(Host) << "'"
		<<  ends;

elog	<< "ccException::Update> "
	<< theQuery.str().c_str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;

if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccException::Update> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}

}

bool ccException::Insert()
{
static const char *quer = "INSERT into exceptions(host,connections,addedby,addedon) VALUES ('";

if(!dbConnected)
	{
	return false;
	}

stringstream query;
query		<< quer
		<< Host << "',"
		<< Connections
		<< ",'" << AddedBy
		<< "'," << AddedOn
		<< ")" << ends;

elog	<< "ccException::insertException> "
	<< query.str().c_str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( query.str().c_str() ) ;
if(PGRES_COMMAND_OK != status)
	{
	elog	<< "ccontrol::Gline::Update> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	}

return (PGRES_COMMAND_OK == status) ;
}

bool ccException::Delete()
{

static const char *quer = "DELETE FROM exceptions WHERE host = '";

if(!dbConnected)
	{
	return false;
	}

stringstream query;
query		<< quer
		<< Host << "'"
		<< ends;

elog 		<< "ccException::delException> "
		<< query.str().c_str()
		<< endl ;

ExecStatusType status = SQLDb->Exec( query.str().c_str() ) ;

if( PGRES_COMMAND_OK != status )
	{
	elog	<< "ccException::findException> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;		    
	}
return true;
}

}
}
