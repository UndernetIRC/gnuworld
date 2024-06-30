/**
 * ccException.cc
 * Exception class
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
 * $Id: ccException.cc,v 1.17 2008/12/27 23:34:31 hidden1 Exp $
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
#include	"gnuworld_config.h"
#include	"StringTokenizer.h"

namespace gnuworld
{

using std::string ; 
using std::endl ; 
using std::stringstream ;
using std::ends ;

namespace uworld
{

//extern unsigned int dbConnected;
unsigned int ccIpLnb::numAllocated = 0;
unsigned int ccIpLisp::numAllocated = 0;

void ccIpLnb::setCidr( const string& _cidr )
{
cidr = _cidr;
StringTokenizer st(cidr,'/');
if (st.size() == 2) {
	cidr1 = st[0];
	cidr2 = atoi(st[1].c_str());
}
}

ccIpLisp::ccIpLisp(dbHandle* _SQLDb)
{
	++numAllocated;
	maxlimit = 0;
	maxIdentlimit = 0;
	active = 1;
	nogline = 0;
	v6 = 2;
	AddedOn = ::time(0);
	ModOn = ::time(0);
	SQLDb = _SQLDb;
	count = 0;
	email = "N/A";
	clonecidr = 64;
	forcecount = 0;
	glunidented = 0;
}


ccIpLisp::~ccIpLisp()
{
	--numAllocated;
}

ccIpLnb::ccIpLnb(dbHandle* _SQLDb)
{
	++numAllocated;
	ipLisp = 0;
	AddedOn = ::time(0);
	SQLDb = _SQLDb;
	ipLispid = 0;
	cidr2 = 128;
	count = 0;
}


ccIpLnb::~ccIpLnb()
{
	--numAllocated;
}


int ccIpLisp::loadData(const string& Name)
{
int i = 0;
static const char Main[] = "SELECT name,id,AddedBy,AddedOn,lastmodby,lastmodon,maxlimit,active,email,clonecidr,forcecount,glunidented,isgroup,maxidentlimit,nogline FROM ipLISPs WHERE name = '";

if((!dbConnected) || !(SQLDb))
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< ccontrol::removeSqlChars(Name)
		<< "'"
		<< ends;

elog	<< "ccIpLisp::loadData> "
	<< theQuery.str().c_str()
	<< endl; 

// TODO: Isn't this impossible?
if( !SQLDb->Exec( theQuery, true ) && (SQLDb->Tuples() > 0) )
//if( (PGRES_TUPLES_OK != status) && (SQLDb->Tuples() > 0) )
	{
	return false;
	}

setName(SQLDb->GetValue(i,0));
setID(atoi(SQLDb->GetValue(i,1).c_str()));
setAddedBy(SQLDb->GetValue(i,2)) ;
setAddedOn(static_cast< time_t >(
	atoi( SQLDb->GetValue(i,3).c_str() ) )) ;
setModBy(SQLDb->GetValue(i,4)) ;
setModOn(static_cast< time_t >(
	atoi( SQLDb->GetValue(i,5).c_str() ) )) ;
setLimit(atoi(SQLDb->GetValue(i,6).c_str()));
setActive(atoi(SQLDb->GetValue(i,7).c_str()));
setEmail(SQLDb->GetValue(i,8));
setCloneCidr(atoi(SQLDb->GetValue(i,9).c_str()));
setForcecount(atoi(SQLDb->GetValue(i,10).c_str()));
setGlunidented(atoi(SQLDb->GetValue(i,11).c_str()));
setGroup(atoi(SQLDb->GetValue(i,12).c_str()));
setIdentLimit(atoi(SQLDb->GetValue(i,13).c_str()));
setNoGline(atoi(SQLDb->GetValue(i,14).c_str()));

theQuery.str("");

return true;

}


int ccIpLisp::updateData()
{
static const char *Main = "UPDATE ipLISPs SET AddedBy = '";

if(!dbConnected)	
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< ccontrol::removeSqlChars(AddedBy)
		<< "', maxlimit = "
		<< maxlimit
		<< ", maxidentlimit = "
		<< maxIdentlimit
		<< ", addedon = "
		<< AddedOn
		<< ", active = "
		<< active
		<< ", nogline = "
		<< nogline
		<< ", lastmodon = "
		<< ModOn
		<< ", clonecidr = "
		<< clonecidr
		<< ", forcecount = "
		<< forcecount
		<< ", glunidented = "
		<< glunidented
		<< ", isgroup = "
		<< group
		<< ", lastmodby = '"
		<< ccontrol::removeSqlChars(ModBy)
		<< "', email = '"
		<< ccontrol::removeSqlChars(email)
		<< "', name = '"
		<< ccontrol::removeSqlChars(Name)
		<< "' WHERE id = " 
		<< id
		<<  ends;

elog	<< "ccIpLisp::Update> "
	<< theQuery.str().c_str()
	<< endl; 

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccIpLisp::Update> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}

}

bool ccIpLisp::Insert()
{
static const char *quer = "INSERT INTO ipLISPs(name,maxlimit,maxidentlimit,addedby,addedon,lastmodby,lastmodon,email,clonecidr,forcecount,glunidented,isgroup) VALUES ('";

if(!dbConnected)
	{
	return false;
	}

stringstream query;
query		<< quer
		<< ccontrol::removeSqlChars(Name) << "',"
		<< maxlimit
		<< "," << maxIdentlimit
		<< ",'" << ccontrol::removeSqlChars(AddedBy)
		<< "'," << AddedOn
		<< ",'" << ccontrol::removeSqlChars(ModBy)
		<< "'," << ModOn
		<< ",'" << ccontrol::removeSqlChars(email)
		<< "'," << clonecidr
		<< "," << forcecount
		<< "," << glunidented
		<< "," << group
		<< ")" << ends;

elog	<< "ccIpLisp::Insert> "
	<< query.str().c_str()
	<< endl; 

if( !SQLDb->Exec( query ) )
//if(PGRES_COMMAND_OK != status)
	{
	elog	<< "ccIpLisp::Insert> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}
return true ;
//return (PGRES_COMMAND_OK == status) ;
}

bool ccIpLnb::Insert()
{
static const char *quer = "INSERT INTO ipLNetblocks(cidr,ispid,addedby,addedon) VALUES ('";

if(!dbConnected)
	{
	return false;
	}

stringstream query;
query		<< quer
		<< ccontrol::removeSqlChars(cidr) << "',"
		<< ipLispid
		<< ",'" << ccontrol::removeSqlChars(AddedBy)
		<< "'," << AddedOn
		<< ")" << ends;

elog	<< "ccIpLnb::Insert> "
	<< query.str().c_str()
	<< endl; 

if( !SQLDb->Exec( query ) )
//if(PGRES_COMMAND_OK != status)
	{
	elog	<< "ccIpLnb::Insert> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}
return true ;
//return (PGRES_COMMAND_OK == status) ;
}

bool ccIpLisp::Delete()
{
static const char *quer = "DELETE FROM ipLISPs WHERE name = '";

if(!dbConnected)
	{
	return false;
	}

stringstream query;
query		<< quer
		<< ccontrol::removeSqlChars(Name) << "'"
		<< ends;

elog 		<< "ccIpLisp::delException> "
		<< query.str().c_str()
		<< endl ;

if( !SQLDb->Exec( query ) )
//if( PGRES_COMMAND_OK != status )
	{
	elog	<< "ccIpLisp::findException> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;		    
	}
return true;
}

bool ccIpLnb::Delete()
{
static const char *quer = "DELETE FROM ipLNetblocks WHERE cidr = '";

if(!dbConnected)
	{
	return false;
	}

stringstream query;
query		<< quer
		<< ccontrol::removeSqlChars(cidr) << "'"
		<< " and ispid = "
		<< ipLispid
		<< ends;

elog 		<< "ccIpLnb::delException> "
		<< query.str().c_str()
		<< endl ;

if( !SQLDb->Exec( query ) )
//if( PGRES_COMMAND_OK != status )
	{
	elog	<< "ccIpLnb::findException> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;		    
	}
return true;
}


}
}
