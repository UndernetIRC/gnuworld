/**
 * ccBadChannel.h
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
 * $Id: ccBadChannel.h,v 1.2 2003/06/28 01:21:19 dan_karrels Exp $
 */
 
#ifndef __CCBADCHANNEL_H_
#define __CCBADCHANNEL_H_

#include <libpq++.h>
#include <string>

using namespace std;

namespace gnuworld
{

namespace uworld
{

class ccBadChannel
{

public:
	ccBadChannel() {}
	
	ccBadChannel(PgDatabase* , unsigned int);
	
	ccBadChannel(const string& _Name, const string& _Reason , 
		     const string& _AddedBy) : Name(_Name),
		     Reason(_Reason),AddedBy(_AddedBy) {};
	
	~ccBadChannel(){}
	
	bool Update(PgDatabase*);
	
	bool Insert(PgDatabase*);
	
	bool Delete(PgDatabase*);
	
	const string& getName() const
	{ return Name; }
	
	const string& getReason() const
	{ return Reason; }

	const string& getAddedBy() const
	{ return AddedBy; }
	
	void setName(const string& _Name)
	{ Name = _Name; }
	
	void setReason(const string& _Reason)
	{ Reason = _Reason; }
	
	void setAddedBy(const string& _AddedBy)
	{ AddedBy = _AddedBy; }
	
private:
	
	string Name;
	
	string Reason;
	
	string AddedBy;
	
	
};	

}

}

#endif 
