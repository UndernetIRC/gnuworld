/**
 * ccBadChannel.cc
 * Holds a bad channel record
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
 * $Id: ccBadChannel.cc,v 1.4 2003/08/09 23:15:34 dan_karrels Exp $
 */

#include	<string>
#include	"libpq++.h"
#include	"ccontrol.h"
#include 	"ccBadChannel.h"
#include	"config.h"

RCSTAG( "$Id: ccBadChannel.cc,v 1.4 2003/08/09 23:15:34 dan_karrels Exp $" ) ;

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
