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

RCSTAG( "$Id: ccException.cc,v 1.17 2008/12/27 23:34:31 hidden1 Exp $" ) ;

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
unsigned int ccShellco::numAllocated = 0;
unsigned int ccShellnb::numAllocated = 0;

ccException::ccException(dbHandle* _SQLDb)
 : Host(string()),
   Connections(0),
   AddedBy(string()),
   AddedOn(0),
   Reason(string()),
   SQLDb(_SQLDb)
{
++numAllocated;
}

ccException::~ccException()
{
--numAllocated;
}

void ccShellnb::setCidr( const string& _cidr )
{
cidr = _cidr;
StringTokenizer st(cidr,'/');
if (st.size() == 2) {
	cidr1 = st[0];
	cidr2 = atoi(st[1].c_str());
}
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
		<< ccontrol::removeSqlChars(string_lower(HostName))
		<< "'"
		<< ends;

elog	<< "ccException::loadData> "
	<< theQuery.str().c_str()
	<< endl; 

// TODO: Isn't this impossible?
if( !SQLDb->Exec( theQuery, true ) && (SQLDb->Tuples() > 0) )
//if( (PGRES_TUPLES_OK != status) && (SQLDb->Tuples() > 0) )
	{
	return false;
	}

Host = SQLDb->GetValue(0,0);
Connections = atoi(SQLDb->GetValue(0,1).c_str());
AddedBy = SQLDb->GetValue(0,2);
AddedOn = atoi(SQLDb->GetValue(0,3).c_str());
Reason = SQLDb->GetValue(0,4);

theQuery.str("");

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
		<< ccontrol::removeSqlChars(AddedBy)
		<< "', Connections = "
		<< Connections
		<< ", AddedOn = "
		<< AddedOn
		<< ", Reason = '"
		<< ccontrol::removeSqlChars(Reason)
		<< "' WHERE lower(Host) = '" 
		<< ccontrol::removeSqlChars(string_lower(Host)) << "'"
		<<  ends;

elog	<< "ccException::Update> "
	<< theQuery.str().c_str()
	<< endl; 

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
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
static const char *quer = "INSERT INTO exceptions(host,connections,addedby,addedon,reason) VALUES ('";

if(!dbConnected)
	{
	return false;
	}

stringstream query;
query		<< quer
		<< ccontrol::removeSqlChars(Host) << "',"
		<< Connections
		<< ",'" << ccontrol::removeSqlChars(AddedBy)
		<< "'," << AddedOn
		<< ",' " << ccontrol::removeSqlChars(Reason)
		<< "')" << ends;

elog	<< "ccException::Insert> "
	<< query.str().c_str()
	<< endl; 

if( !SQLDb->Exec( query ) )
//if(PGRES_COMMAND_OK != status)
	{
	elog	<< "ccException::Insert> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}
return true ;
//return (PGRES_COMMAND_OK == status) ;
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
		<< ccontrol::removeSqlChars(Host) << "'"
		<< ends;

elog 		<< "ccException::delException> "
		<< query.str().c_str()
		<< endl ;

if( !SQLDb->Exec( query ) )
//if( PGRES_COMMAND_OK != status )
	{
	elog	<< "ccException::findException> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;		    
	}
return true;
}

ccShellco::ccShellco(dbHandle* _SQLDb)
{
	++numAllocated;
	maxlimit = 0;
	active = 1;
	AddedOn = ::time(0);
	ModOn = ::time(0);
	SQLDb = _SQLDb;
}


ccShellco::~ccShellco()
{
	--numAllocated;
}

ccShellnb::ccShellnb(dbHandle* _SQLDb)
{
	++numAllocated;
	shellco = 0;
	AddedOn = ::time(0);
	SQLDb = _SQLDb;
	companyid = 0;
	cidr2 = 32;
}


ccShellnb::~ccShellnb()
{
	--numAllocated;
}


int ccShellco::loadData(const string& Name)
{
int i = 0;
static const char Main[] = "SELECT name,id,AddedBy,AddedOn,lastmodby,lastmodon,maxlimit,active FROM ShellCompanies WHERE name = '";

if((!dbConnected) || !(SQLDb))
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< ccontrol::removeSqlChars(Name)
		<< "'"
		<< ends;

elog	<< "ccShellco::loadData> "
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

theQuery.str("");

return true;

}


int ccShellco::updateData()
{
static const char *Main = "UPDATE ShellCompanies SET AddedBy = '";

if(!dbConnected)	
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< ccontrol::removeSqlChars(AddedBy)
		<< "', maxlimit = "
		<< maxlimit
		<< ", addedon = "
		<< AddedOn
		<< ", active = "
		<< active
		<< ", lastmodon = "
		<< ModOn
		<< ", lastmodby = '"
		<< ccontrol::removeSqlChars(ModBy)
		<< "', name = '"
		<< ccontrol::removeSqlChars(Name)
		<< "' WHERE id = " 
		<< id
		<<  ends;

elog	<< "ccShellco::Update> "
	<< theQuery.str().c_str()
	<< endl; 

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccShellco::Update> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}

}

int ccShellnb::updateData()
{
static const char *Main = "UPDATE ShellNetblocks SET AddedBy = '";

if(!dbConnected)	
	{
	return false;
	}

stringstream theQuery;
theQuery	<< Main
		<< ccontrol::removeSqlChars(AddedBy)
		<< "', addedon = "
		<< AddedOn
		<< ", companyid = "
		<< companyid
		<< " WHERE cidr = '" 
		<< ccontrol::removeSqlChars(cidr) << "'"
		<<  ends;

elog	<< "ccShellco::Update> "
	<< theQuery.str().c_str()
	<< endl; 

if( SQLDb->Exec( theQuery ) )
//if( PGRES_COMMAND_OK == status ) 
	{
	return true;
	}
else
	{
	elog	<< "ccShellco::Update> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;
	}

}

bool ccShellco::Insert()
{
static const char *quer = "INSERT INTO ShellCompanies(name,maxlimit,addedby,addedon,lastmodby,lastmodon) VALUES ('";

if(!dbConnected)
	{
	return false;
	}

stringstream query;
query		<< quer
		<< ccontrol::removeSqlChars(Name) << "',"
		<< maxlimit
		<< ",'" << ccontrol::removeSqlChars(AddedBy)
		<< "'," << AddedOn
		<< ",'" << ccontrol::removeSqlChars(ModBy)
		<< "'," << ModOn
		<< ")" << ends;

elog	<< "ccShellco::Insert> "
	<< query.str().c_str()
	<< endl; 

if( !SQLDb->Exec( query ) )
//if(PGRES_COMMAND_OK != status)
	{
	elog	<< "ccShellco::Insert> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}
return true ;
//return (PGRES_COMMAND_OK == status) ;
}

bool ccShellnb::Insert()
{
static const char *quer = "INSERT INTO ShellNetblocks(cidr,companyid,addedby,addedon) VALUES ('";

if(!dbConnected)
	{
	return false;
	}

stringstream query;
query		<< quer
		<< ccontrol::removeSqlChars(cidr) << "',"
		<< companyid
		<< ",'" << ccontrol::removeSqlChars(AddedBy)
		<< "'," << AddedOn
		<< ")" << ends;

elog	<< "ccShellnb::Insert> "
	<< query.str().c_str()
	<< endl; 

if( !SQLDb->Exec( query ) )
//if(PGRES_COMMAND_OK != status)
	{
	elog	<< "ccShellnb::Insert> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false ;
	}
return true ;
//return (PGRES_COMMAND_OK == status) ;
}

bool ccShellco::Delete()
{
static const char *quer = "DELETE FROM ShellCompanies WHERE name = '";

if(!dbConnected)
	{
	return false;
	}

stringstream query;
query		<< quer
		<< ccontrol::removeSqlChars(Name) << "'"
		<< ends;

elog 		<< "ccShellco::delException> "
		<< query.str().c_str()
		<< endl ;

if( !SQLDb->Exec( query ) )
//if( PGRES_COMMAND_OK != status )
	{
	elog	<< "ccShellco::findException> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;		    
	}
return true;
}

bool ccShellnb::Delete()
{
static const char *quer = "DELETE FROM ShellNetblocks WHERE cidr = '";

if(!dbConnected)
	{
	return false;
	}

stringstream query;
query		<< quer
		<< ccontrol::removeSqlChars(cidr) << "'"
		<< ends;

elog 		<< "ccShellnb::delException> "
		<< query.str().c_str()
		<< endl ;

if( !SQLDb->Exec( query ) )
//if( PGRES_COMMAND_OK != status )
	{
	elog	<< "ccShellnb::findException> SQL Failure: "
		<< SQLDb->ErrorMessage()
		<< endl ;
	return false;		    
	}
return true;
}

bool ccShellnb::set24Mask()
{
	int client_addr[4] = { 0 };
	struct in_addr tmp_ip;
	unsigned long mask_ip;
	int i = 0;
	string cidrmask;
	string tIP;
	char *client_ip;

	tIP = cidr1;

	if (cidr2 <= 24) {
        str24 = "";
		return true;
	}

	/* CIDR checks */
	/* convert ip to longip */
	i = sscanf(tIP.c_str(), "%d.%d.%d.%d", &client_addr[0], &client_addr[1], &client_addr[2], &client_addr[3]);
	mask_ip = ntohl((client_addr[0]) | (client_addr[1] << 8) | (client_addr[2] << 16) | (client_addr[3] << 24));
	/* bitshift ip to strip the last (32-cidrmask) bits (leaving a mask for the ip) */
	for (i = 0; i < (32-24); i++) {
		/* right shift */
		mask_ip >>= 1;
	}
	for (i = 0; i < (32-24); i++) {
		/* left shift */
		mask_ip <<= 1;
	}
	/* convert longip back to ip */
	mask_ip = htonl(mask_ip);
	tmp_ip.s_addr = mask_ip;
	client_ip = inet_ntoa(tmp_ip);

	str24 = string(client_ip);
	return true;
}


}
}
