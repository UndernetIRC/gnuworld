/*
 * ccBadChannel.cc
 *
 * Holds a bad channel record
 *
*/

#include	<string>
#include	"libpq++.h"
#include	"ccontrol.h"
#include 	"ccBadChannel.h"

const char ccBadChannel_cc_rcsId[] = "$Id: ccBadChannel.cc,v 1.2 2003/02/10 12:22:09 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

ccBadChannel::ccBadChannel(PgDatabase* SQLDb, unsigned int Place)
{
Name = SQLDb->GetValue(Place,0);
Reason = SQLDb->GetValue(Place,1);
AddedBy = SQLDb->GetValue(Place,2);
}

bool ccBadChannel::Update(PgDatabase* SQLDb)
{
        
if(!dbConnected) 
        {       
        return false;
        }

stringstream theQuery;
theQuery        << "update BadChannels set Reason = '"
		<< ccontrol::removeSqlChars(Reason)
		<< "', AddedBy = '"
		<< ccontrol::removeSqlChars(AddedBy)
		<< "' where lower(Name) = '"
		<< ccontrol::removeSqlChars(string_lower(Name)) << "'"
                << ends;
         
elog    << "ccBadChannel::Update> "
        << theQuery.str()
        << endl;

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;
//delete[] theQuery.str() ;

if (PGRES_COMMAND_OK != status)
        {
        elog << "ccBadChannel::Update SQL ERROR : " 
	     << SQLDb->ErrorMessage() << endl;
	return false;
        }
return true;
}

bool ccBadChannel::Delete(PgDatabase* SQLDb)
{
        
if(!dbConnected) 
        {       
        return false;
        }

stringstream theQuery;
theQuery        << "delete from  BadChannels where lower(Name) = '"
		<< ccontrol::removeSqlChars(string_lower(Name)) << "'"
                << ends;
         
elog    << "ccBadChannel::Delete> "
        << theQuery.str()
        << endl;

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;
//delete[] theQuery.str() ;

if (PGRES_COMMAND_OK != status)
        {
        elog << "ccBadChannel::Delete SQL ERROR : " 
	     << SQLDb->ErrorMessage() << endl;

        return false;
        }
return true;
}

bool ccBadChannel::Insert(PgDatabase* SQLDb)
{
        
if(!dbConnected) 
        {       
        return false;
        }

stringstream theQuery;
theQuery        << "Insert into BadChannels (Name,Reason,AddedBy) VALUES ('"
		<< ccontrol::removeSqlChars(Name) << "','"
		<< ccontrol::removeSqlChars(Reason)
		<< "','"
		<< ccontrol::removeSqlChars(AddedBy) 
		<< "')"
                << ends;
         
elog    << "ccBadChannel::Insert> "
        << theQuery.str()
        << endl;

ExecStatusType status = SQLDb->Exec( theQuery.str().c_str() ) ;
//delete[] theQuery.str() ;

if (PGRES_COMMAND_OK != status)
        {
        elog << "ccBadChannel::Insert SQL ERROR : " 
	     << SQLDb->ErrorMessage() << endl;

        return false;
        }

return true;
}

}
}
