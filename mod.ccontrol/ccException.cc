/* 
 * ccException.cc
 * 
 * Exception class
 * 
 * $Id: ccException.cc,v 1.3 2001/08/16 20:18:38 mrbean_ Exp $
 */
 
#include	<strstream>
#include	<string> 

#include	<ctime>
#include	<cstring> 
#include	<cstdlib>

#include	"ELog.h"
#include	"misc.h"
#include	"match.h"

#include	"ccException.h" 

const char ccException_h_rcsId[] = __CCEXCEPTION_H ;
const char ccException_cc_rcsId[] = "$Id: ccException.cc,v 1.3 2001/08/16 20:18:38 mrbean_ Exp $" ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::strstream ;
using std::ends ;

namespace uworld
{

ccException::ccException(PgDatabase* _SQLDb)
 : Host(""),
   Connections(0),
   AddedBy(""),
   AddedOn(0),
   SQLDb(_SQLDb)
{
}

ccException::~ccException()
{}


int ccException::loadData(const string& HostName)
{

static const char Main[] = "SELECT * FROM Exceptions WHERE lower(Host) = '";

strstream theQuery;
theQuery	<< Main
		<< string_lower(HostName)
		<< "'"
		<< ends;

elog	<< "ccException::loadData> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( (PGRES_TUPLES_OK != status) && (SQLDb->Tuples() > 0) )
	{
	return false;
	}

Host = SQLDb->GetValue(0,0);
Connections = atoi(SQLDb->GetValue(0,1));
AddedBy = SQLDb->GetValue(0,2);
AddedOn = atoi(SQLDb->GetValue(0,0));

return true;

}

int ccException::updateData()
{
static const char *Main = "UPDATE Exceptions SET AddedBy = '";

strstream theQuery;
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
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

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
strstream query;
query		<< quer
		<< Host << "',"
		<< Connections
		<< ",'" << AddedBy
		<< "'," << AddedOn
		<< ")" << ends;

elog	<< "ccException::insertException> "
	<< query.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( query.str() ) ;
delete[] query.str();
if(PGRES_COMMAND_OK != status)
	elog	<< "ccontrol::Gline::Update> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;

return (PGRES_COMMAND_OK == status) ;
}

bool ccException::Delete()
{

static const char *quer = "DELETE FROM exceptions WHERE host = '";

strstream query;
query		<< quer
		<< Host << "'"
		<< ends;

elog 		<< "ccException::delException> "
		<< query.str()
		<< endl ;

ExecStatusType status = SQLDb->Exec( query.str() ) ;
delete[] query.str();

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