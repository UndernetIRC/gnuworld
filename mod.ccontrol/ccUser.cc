/* 
 * ccUser.cc
 * 
 * Storage class for accessing user information 
 * 
 * $Id: ccUser.cc,v 1.7 2001/07/23 10:28:51 mrbean_ Exp $
 */
 
#include	<strstream>
#include	<string> 

#include	<cstring> 

#include	"libpq++.h"
#include	"ELog.h"
#include	"misc.h"
#include	"ccUser.h" 

const char ccUser_h_rcsId[] = __CCUSER_H ;
const char ccUser_cc_rcsId[] = "$Id: ccUser.cc,v 1.7 2001/07/23 10:28:51 mrbean_ Exp $" ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::strstream ;
using std::ends ;

namespace uworld
{

ccUser::ccUser(PgDatabase* _SQLDb)
 : Id( 0 ),
   Server( "" ),
   IsSuspended(0),
   SuspendExpires(0),
   Access( 0 ),
   Flags( 0 ), 
   IsUhs(0),
   IsOper(0),
   IsAdmin(0),
   IsSmt(0),
   IsCoder(0),
   GetLogs(0),
   NeedOp(0),
   SQLDb( _SQLDb )
{
}

ccUser::~ccUser()
{}

bool ccUser::loadData(const string& Name)
{

static const char Main[] = "SELECT user_id,user_name,password,access,flags,suspend_expires,suspended_by,server,isSuspended,IsUhs,IsOper,IsAdmin,IsSmt,IsCoder,GetLogs,NeedOp FROM opers WHERE lower(user_name) = '";

strstream theQuery;
theQuery	<< Main
		<< string_lower(Name)
		<< "'"
		<< ends;

elog	<< "ccUser::loadData> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( (PGRES_TUPLES_OK == status) && (SQLDb->Tuples() > 0) )
	{
        GetParm();
	return true;
	}

return false;
}

bool ccUser::loadData( const unsigned int Id)
{
static const char Main[] = "SELECT user_id,user_name,password,access,flags,suspend_expires,suspended_by,server,isSuspended,IsUhs,IsOper,IsAdmin,IsSmt,IsCoder,GetLogs,NeedOp FROM opers WHERE user_id = ";
strstream theQuery;

theQuery	<< Main
		<< Id
		<< ';'
		<< ends;

elog	<< "ccontrol::loadData> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = SQLDb->Exec( theQuery.str() ) ;
delete[] theQuery.str() ;

if( (PGRES_TUPLES_OK == status) && (SQLDb->Tuples() > 0) )
	{
	GetParm();
	return true;
	}

return false;
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
Server = SQLDb->GetValue(0,7);
IsSuspended = (!strcasecmp(SQLDb->GetValue(0,8),"t") ? 1 : 0 );
IsUhs = (!strcasecmp(SQLDb->GetValue(0,9),"t") ? 1 : 0 );
IsOper = (!strcasecmp(SQLDb->GetValue(0,10),"t") ? 1 : 0 );
IsAdmin = (!strcasecmp(SQLDb->GetValue(0,11),"t") ? 1 : 0 );
IsSmt = (!strcasecmp(SQLDb->GetValue(0,12),"t") ? 1 : 0 );
IsCoder = (!strcasecmp(SQLDb->GetValue(0,13),"t") ? 1 : 0 );
GetLogs = (!strcasecmp(SQLDb->GetValue(0,14),"t") ? 1 : 0 );
NeedOp = (!strcasecmp(SQLDb->GetValue(0,15),"t") ? 1 : 0 );
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
		<< "' ,server = '"
		<< Server
		<< "' ,isSuspended = "
		<< (IsSuspended ? "'t'" : "'n'")
		<< ",isUhs = "
		<< (IsUhs ? "'t'" : "'n'")
		<< ",isOper = "
		<< (IsOper ? "'t'" : "'n'")
		<< ",isAdmin = "
		<< (IsAdmin ? "'t'" : "'n'")
		<< ",isSmt = "
		<< (IsSmt ? "'t'" : "'n'")
		<< ",isCoder = "
		<< (IsCoder ? "'t'" : "'n'")
		<< ",GetLogs = "
		<< (GetLogs ? "'t'" : "'n'")
		<< ",NeedOp = "
		<< (NeedOp ? "'t'" : "'n'")
		<< " WHERE lower(user_name) = '" 
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

void ccUser::setUhs()
{
IsUhs = true;
IsOper = IsAdmin = IsSmt = IsCoder = false;
}

void ccUser::setOper()
{
IsOper = true;
IsUhs = IsAdmin = IsSmt = IsCoder = false;
}

void ccUser::setAdmin()
{
IsAdmin = true;
IsOper = IsUhs = IsSmt = IsCoder = false;
}

void ccUser::setSmt()
{
IsSmt = true;
IsOper = IsAdmin = IsUhs = IsCoder = false;
}

void ccUser::setCoder()
{
IsCoder = true;
IsOper = IsAdmin = IsSmt = IsUhs = false;
}

unsigned int ccUser::getType()
{
return (operLevel::UHSLEVEL * IsUhs + operLevel::OPERLEVEL * IsOper 
	+ operLevel::ADMINLEVEL * IsAdmin + operLevel::SMTLEVEL * IsSmt 
	+ operLevel::CODERLEVEL * IsCoder);
}	
	
void ccUser::setType(unsigned int Type)
{
switch (Type)
	{
	case operLevel::UHSLEVEL:
		IsUhs = true;
		IsOper = false;
		IsAdmin = false;
		IsSmt = false;
		IsCoder = false;
		break;
	case operLevel::OPERLEVEL:
		IsUhs = false;
		IsOper = true;
		IsAdmin = false;
		IsSmt = false;
		IsCoder = false;
		break;
	case operLevel::ADMINLEVEL:
		IsUhs = false;
		IsOper = false;
		IsAdmin = true;
		IsSmt = false;
		IsCoder = false;
		break;
	case operLevel::SMTLEVEL:
		IsUhs = false;
		IsOper = false;
		IsAdmin = false;
		IsSmt = true;
		IsCoder = false;
		break;
	case operLevel::CODERLEVEL:
		IsUhs = false;
		IsOper = false;
		IsAdmin = false;
		IsSmt = false;
		IsCoder = true;
	default:;
	}
}

}
	
} //  namespace gnuworld
