/* 
 * cclUser.cc
 * 
 * Storage class for accessing user information 
 * 
 * $Id: ccUser.cc,v 1.1 2001/02/24 18:31:27 mrbean_ Exp $
 */
 
#include	<strstream>
#include	<string> 
#include	<cstring> 
#include	"ELog.h"
#include	"misc.h"
#include	"ccUser.h" 
//#include	"constants.h"

using std::string ; 
using std::endl ; 

namespace gnuworld
{

using namespace gnuworld ;

 
ccUser::ccUser(PgDatabase* _SQLDb)
 : Id( 0 ),
   UserName(""),
   Password(""),
   last_updated_by(""),
   Numeric(""),
   SuspendExpires(0),
   SuspendedBy(""),
   Access( 0 ),
   flags( 0 ), 
   SQLDb( _SQLDb )
{
}

bool ccUser::loadData(const string& Name)
{

static const char Main[] = "SELECT user_id,user_name,password,access,flags,suspend_expires,suspended_by FROM opers WHERE lower(user_name) = '";

strstream theQuery;
theQuery	<< Main
		<< string_lower(Name)
		<< "'"
		<< ends;

elog	<< "ACCESS::sqlQuery> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( (PGRES_TUPLES_OK == status) && (SQLDb->Tuples() > 0) )
	{
        GetParm();
	return true;
	}

return NULL;
}

ccUser::loadData( const unsigned int Id)
{
static const char Main[] = "SELECT user_id,user_name,password,access,flags,suspend_expires,suspended_by FROM opers WHERE user_id = ";

strstream theQuery;

theQuery	<< Main
		<< Id
		<< ';'
		<< ends;

elog	<< "ACCESS::sqlQuery> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( (PGRES_TUPLES_OK == status) && (SQLDb->Tuples() > 0) )
	{
	GetParm();
	return false;
	}

return NULL;
}

void ccUser::GetParm()
{
Id = atoi(SQLDb->GetValue(0, 0));
UserName = SQLDb->GetValue(0, 1);
Password = SQLDb->GetValue(0, 2);
Access = atoi(SQLDb->GetValue(0, 3));
Flags = atoi(SQLDb->GetValue(0, 4));
SuspendExpires = atoi(SQLDb->GetValue(0,5));
SuspendedBy = SQLDb->GetValue(0,6);
}    

bool ccUser::Update()
{
static const char *Main = "UPDATE opers SET password = '";

strstream theQuery;
theQuery	<< Main
		<< Password
		<< "', Access = "
		<< Access
		<< ", last_updated_by = '"
		<<last_updated_by
		<< "',last_updated = now()::abstime::int4,flags = "
		<< Flags
		<<  ",suspend_expires = "
		<< SuspendExpires
		<< " ,suspended_by = '"
		<< SuspendedBy
		<< "' WHERE lower(user_name) = '" 
		<< string_lower(UserName) << "'"
		<<  ends;

elog	<< "ccontrol::UpdateOper> "
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
	elog	<< "ccontrol::UpdateOper> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}
}

}
