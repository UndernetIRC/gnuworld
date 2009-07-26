/**
 * ccUser.cc
 * Storage class for accessing user information 
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: ccUser.cc,v 1.22 2009/07/26 18:30:38 mrbean_ Exp $
 */
 
#include	<sstream>
#include	<string> 

#include	<cstring> 

#include	"dbHandle.h"
#include	"ELog.h"
#include	"misc.h"
#include	"ccUser.h" 
#include	"commLevels.h"
#include	"ccontrol.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: ccUser.cc,v 1.22 2009/07/26 18:30:38 mrbean_ Exp $" ) ;

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::stringstream ;
using std::ends ;

namespace uworld
{

unsigned int ccUser::numAllocated = 0;

ccUser::ccUser(dbHandle* _SQLDb)
 : Id( 0 ),
   Server( "" ),
   IsSuspended(0),
   SuspendExpires(0),
   SuspendLevel(0),
   SuspendReason(""),      
   Access( 0 ),
   SAccess( 0 ),
   Flags( 0 ), 
   IsUhs(0),
   IsOper(0),
   IsAdmin(0),
   IsSmt(0),
   IsCoder(0),
   GetLogs(0),
   GetLag(0),
   NeedOp(0),
   Notice(0),
   Client(NULL),
   SQLDb( _SQLDb ),
   PassChangeTS(0)
{
++numAllocated;
}

ccUser::~ccUser()
{
--numAllocated;
}

bool ccUser::loadData(const string& Name)
{
if(!dbConnected)
	{
	return false;
	}
	
static const char Main[] = "SELECT user_id,user_name,password,access,saccess,flags,suspend_expires,suspended_by,server,isSuspended,IsUhs,IsOper,IsAdmin,IsSmt,IsCoder,GetLogs,NeedOp,Email,Suspend_Level,Suspend_Reason,Notice,GetLag,LastPassChangeTS FROM opers WHERE lower(user_name) = '";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< ccontrol::removeSqlChars(string_lower(Name))
		<< "'"
		<< ends;

elog	<< "ccUser::loadData> "
	<< theQuery.str().c_str()
	<< endl; 

if( SQLDb->Exec( theQuery, true ) && (SQLDb->Tuples() > 0) )
//if( (PGRES_TUPLES_OK == status) && (SQLDb->Tuples() > 0) )
	{
        GetParm();
	return true;
	}

return false;
}

bool ccUser::loadData( const unsigned int Id)
{

if(!dbConnected)
	{
	return false;
	}
	
static const char Main[] = "SELECT user_id,user_name,password,access,saccess,flags,suspend_expires,suspended_by,server,isSuspended,IsUhs,IsOper,IsAdmin,IsSmt,IsCoder,GetLogs,NeedOp,Email,Suspend_Level,Suspend_Reason,Notice,GetLag,LastPassChangeTS FROM opers WHERE user_id = ";
stringstream theQuery;

if(!dbConnected)
	{
	return false;
	}

theQuery	<< Main
		<< Id
		<< ';'
		<< ends;

elog	<< "ccontrol::loadData> "
	<< theQuery.str().c_str()
	<< endl; 

if( SQLDb->Exec( theQuery, true ) && (SQLDb->Tuples() > 0) )
//if( (PGRES_TUPLES_OK == status) && (SQLDb->Tuples() > 0) )
	{
	GetParm();
	return true;
	}

return false;
}

void ccUser::GetParm()
{
Id = atoi(SQLDb->GetValue(0, 0).c_str());
UserName = SQLDb->GetValue(0, 1);
Password = SQLDb->GetValue(0, 2);
Access = atol(SQLDb->GetValue(0, 3).c_str());
SAccess = atol(SQLDb->GetValue(0, 4).c_str());
Flags = atoi(SQLDb->GetValue(0, 5).c_str());
SuspendExpires = atoi(SQLDb->GetValue(0,6).c_str());
SuspendedBy = SQLDb->GetValue(0,7);
Server = SQLDb->GetValue(0,8);
IsSuspended = (!strcasecmp(SQLDb->GetValue(0,9),"t") ? 1 : 0 );
IsUhs = (!strcasecmp(SQLDb->GetValue(0,10),"t") ? 1 : 0 );
IsOper = (!strcasecmp(SQLDb->GetValue(0,11),"t") ? 1 : 0 );
IsAdmin = (!strcasecmp(SQLDb->GetValue(0,12),"t") ? 1 : 0 );
IsSmt = (!strcasecmp(SQLDb->GetValue(0,13),"t") ? 1 : 0 );
IsCoder = (!strcasecmp(SQLDb->GetValue(0,14),"t") ? 1 : 0 );
GetLogs = (!strcasecmp(SQLDb->GetValue(0,15),"t") ? 1 : 0 );
NeedOp = (!strcasecmp(SQLDb->GetValue(0,16),"t") ? 1 : 0 );
Email = SQLDb->GetValue(0,17);
SuspendLevel = atoi(SQLDb->GetValue(0,18).c_str());
SuspendReason = SQLDb->GetValue(0,19);
Notice = (!strcasecmp(SQLDb->GetValue(0,20),"t") ? 1 : 0 );
GetLag = (!strcasecmp(SQLDb->GetValue(0,21),"t") ? 1 : 0 );
PassChangeTS = atoi(SQLDb->GetValue(0,22).c_str());
}    

bool ccUser::Update()
{
static const char *Main = "UPDATE opers SET password = '";

if(!dbConnected)
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< ccontrol::removeSqlChars(Password)
		<< "', Access = "
		<< Access
		<< ", SAccess = "
		<< SAccess
		<< ", last_updated_by = '"
		<< ccontrol::removeSqlChars(last_updated_by)
		<< "',last_updated = now()::abstime::int4,flags = "
		<< Flags
		<<  ",suspend_expires = "
		<< SuspendExpires
		<< " ,suspended_by = '"
		<< SuspendedBy
		<< "' ,suspend_level = "
		<< SuspendLevel
		<< ", suspend_reason = '"
		<< SuspendReason
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
		<< ",GetLag = "
		<< (GetLag ? "'t'" : "'n'")
		<<  ",LastPassChangeTS = "
		<< PassChangeTS
		<< ",NeedOp = "
		<< (NeedOp ? "'t'" : "'n'")
		<< ", Email = '"
		<<  ccontrol::removeSqlChars(Email)
		<< "',Notice = "
		<< (Notice ? "'t'" : "'n'")
		<< " WHERE lower(user_name) = '" 
		<< ccontrol::removeSqlChars( string_lower(UserName)) << "'"
		<<  ends;

elog	<< "ccontrol::UpdateOper> "
	<< theQuery.str().c_str()
	<< endl; 

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
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

bool ccUser::gotAccess(Command* Comm)
{
if(Comm->getSecondAccess())
	{
	return ((SAccess & Comm->getFlags()) != 0 ? true : false);
	}
else
	{
	return ((Access & Comm->getFlags()) != 0 ? true : false);	
	}
return false;
}

void ccUser::addCommand(Command* Comm)
{
if(Comm->getSecondAccess())
	SAccess |= Comm->getFlags();
else
	Access |= Comm->getFlags();
}
		
void ccUser::removeCommand(Command* Comm)
{
if(Comm->getSecondAccess())
	SAccess &= ~Comm->getFlags();
else
	Access &= ~Comm->getFlags();
}

void ccUser::updateAccessFromFlags()
{
if(IsOper)
	{
	Access = commandLevel::OPER;
	SAccess = commandLevel::SOPER;
	}
else if(IsAdmin)
	{
	Access = commandLevel::ADMIN;
	SAccess = commandLevel::SADMIN;
	}
else if(IsSmt)
	{
	Access = commandLevel::SMT;
	SAccess = commandLevel::SSMT;
	}
else if(IsCoder)
	{
	Access = commandLevel::CODER;
	SAccess = commandLevel::SCODER;
	}
}

void ccUser::updateAccess(unsigned int Type)
{
switch(Type)
	{
	case operLevel::OPERLEVEL:
		Access = commandLevel::OPER;
		SAccess = commandLevel::SOPER;
		break;
	case operLevel::ADMINLEVEL:
		Access = commandLevel::ADMIN;
		SAccess = commandLevel::SADMIN;
		break;
	case operLevel::SMTLEVEL:
		Access = commandLevel::SMT;
		SAccess = commandLevel::SSMT;
		break;
	case operLevel::CODERLEVEL:
		Access = commandLevel::CODER;
		SAccess = commandLevel::SCODER;
		break;
	}
}

}
	
} //  namespace gnuworld
