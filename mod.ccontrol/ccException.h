/**
 * ccException.h
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
 * $Id: ccException.h,v 1.13 2009/07/26 18:30:37 mrbean_ Exp $
 */

#ifndef __CCEXCEPTION_H
#define __CCEXCEPTION_H "$Id: ccException.h,v 1.13 2009/07/26 18:30:37 mrbean_ Exp $"

#include	<string>

#include	<ctime>

#include	"dbHandle.h"

#include	"match.h"

namespace gnuworld
{ 

using std::string ;

namespace uworld
{

class ccException
{
public:	
	ccException(dbHandle* _SQLDb);
	virtual ~ccException();
	
	inline bool operator==( const string& ExceptionHost ) const
		{ return (!strcasecmp( Host, ExceptionHost )
		    || !match(Host.c_str(),ExceptionHost.c_str())) ; }

	inline const string& 		getHost() const
			{ return Host; }
		
	inline const int&		getConnections() const
		{ return Connections; }
	
	inline const string&		getAddedBy() const
		{ return AddedBy; }
		
	inline time_t			getAddedOn() const
		{ return AddedOn; }

	inline const string& 		getReason() const
			{ return Reason; }

	inline void 		setHost( const string& _Host ) 
		{ Host = _Host; }
		
	inline void		setConnections( const int _Connections ) 
		{  Connections = _Connections; }
	
	inline void		setAddedBy( const string& _AddedBy) 
		{ AddedBy = _AddedBy; }
		
	inline void		setAddedOn( const time_t _AddedOn )
		{ AddedOn = _AddedOn; }
	
	inline void 		setReason( const string& _Reason ) 
		{ Reason = _Reason; }

	inline void		setSqldb(dbHandle* _SQLDb)
		{ SQLDb = _SQLDb; }
		
	int		loadData(const string& Host);
	
	int		updateData();
				
	bool		Insert();
	
	bool		Delete();
	
	static unsigned int numAllocated;

protected:
	
	string 		Host;
	int 		Connections;
	string 		AddedBy;
	time_t		AddedOn;
	string 		Reason;
	dbHandle* SQLDb;

};

class ccShellco
{
public:	
	ccShellco(dbHandle* _SQLDb);
	virtual ~ccShellco();
	

	inline bool		isActive() const
		{ return (active == 1 ? true : false); }
	inline const string& 		getName() const
		{ return Name; }
		
	inline const int&		getID() const
		{ return id; }

	inline const int&		getLimit() const
		{ return maxlimit; }
	
	inline const string&		getAddedBy() const
		{ return AddedBy; }
		
	inline time_t			getAddedOn() const
		{ return AddedOn; }

	inline const string&		getModBy() const
		{ return ModBy; }
		
	inline time_t			getModOn() const
		{ return ModOn; }

	inline void 		setName( const string& _Name ) 
		{ Name = _Name; }
		
	inline void		setLimit( const int _maxlimit ) 
		{  maxlimit = _maxlimit; }
	
	inline void		setID( const int _id ) 
		{  id = _id; }

	inline void		setAddedBy( const string& _AddedBy) 
		{ AddedBy = _AddedBy; }
		
	inline void		setAddedOn( const time_t _AddedOn )
		{ AddedOn = _AddedOn; }
	
	inline void		setModBy( const string& _ModBy) 
		{ ModBy = _ModBy; }
		
	inline void		setModOn( const time_t _ModOn )
		{ ModOn = _ModOn; }
		
	inline void 	setActive( const int _active) 
		{ active = _active; }

	inline void		setSqldb(dbHandle* _SQLDb)
		{ SQLDb = _SQLDb; }
	
	int		loadData(const string& Name);
	
	int		updateData();
				
	bool		Insert();
	
	bool		Delete();
	
	static unsigned int numAllocated;

protected:
	
	int			id;
	string 		Name;
	int 		maxlimit;
	int			active;
	string 		AddedBy;
	time_t		AddedOn;
	string 		ModBy;
	time_t		ModOn;
	dbHandle* SQLDb;

}; 

class ccShellnb
{
public:	
	ccShellnb(dbHandle* _SQLDb);
	virtual ~ccShellnb();
	
	inline bool operator==( const string& ExceptionHost ) const
		{ return (!strcasecmp( cidr, ExceptionHost )
		    || !match(cidr.c_str(),ExceptionHost.c_str())) ; }

	inline const string& 		getCidr() const
			{ return cidr; }
		
	inline const string& 		getCidr1() const
			{ return cidr1; }

	inline const int& 		getCompanyID() const
			{ return companyid; }

	inline const int& 		getCidr2() const
			{ return cidr2; }
	
	inline const string&		getAddedBy() const
		{ return AddedBy; }
		
	inline time_t			getAddedOn() const
		{ return AddedOn; }
	
	inline const string&	get24Mask() const
		{ return str24; }

	inline void 		setShellco( ccShellco* _shellco ) 
		{ shellco = _shellco; }

	inline void 		setCompanyID( const int _companyid ) 
		{ companyid = _companyid; }
			
	inline void		setAddedBy( const string& _AddedBy) 
		{ AddedBy = _AddedBy; }
		
	inline void		setAddedOn( const time_t _AddedOn )
		{ AddedOn = _AddedOn; }

	void 		setCidr( const string& _cidr );
	bool		set24Mask();

	inline void		setSqldb(dbHandle* _SQLDb)
		{ SQLDb = _SQLDb; }


	ccShellco*	shellco;

	int		updateData();
				
	bool		Insert();
	
	bool		Delete();
	
	static unsigned int numAllocated;

protected:
	
	int			companyid;
	string 		cidr;
	string 		cidr1;
	int			cidr2;
	string 		AddedBy;
	time_t		AddedOn;
	string		str24;
	dbHandle* SQLDb;

}; 

}
}
#endif // __CCEXCEPTION_H
