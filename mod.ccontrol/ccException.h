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

typedef std::map<string, int> 		ipLclonesMapType;


class ccIpLisp
{
public:	
	ccIpLisp(dbHandle* _SQLDb);
	virtual ~ccIpLisp();
	

	inline bool		isActive() const
		{ return (active == 1 ? true : false); }

	inline bool		isNoGline() const
		{ return (nogline == 1 ? true : false); }

	inline bool		isGlunidented() const
		{ return (glunidented == 1 ? true : false); }

	inline bool		isForcecount() const
		{ return (forcecount == 1 ? true : false); }

	inline int		isv6() const
		{ return v6; }

	inline bool		isGroup() const
		{ return (group == 1 ? true : false); }

	inline const string& 		getName() const
		{ return Name; }
		
	inline const int&		getID() const
		{ return id; }

	inline const int&		getLimit() const
		{ return maxlimit; }
	
	inline const int&		getIdentLimit() const
		{ return maxIdentlimit; }
	
	inline const string&		getEmail() const
		{ return email; }
	
	inline const int&		getCloneCidr() const
		{ return clonecidr; }
	
	inline const string&		getAddedBy() const
		{ return AddedBy; }
		
	inline time_t			getAddedOn() const
		{ return AddedOn; }

	inline const string&		getModBy() const
		{ return ModBy; }
		
	inline time_t			getModOn() const
		{ return ModOn; }

	inline const int& 		getCount() const
			{ return count; }

	inline void 		setName( const string& _Name ) 
		{ Name = _Name; }
		
	inline void		setLimit( const int _maxlimit ) 
		{  maxlimit = _maxlimit; }
	
	inline void		setIdentLimit( const int _maxIdentlimit ) 
		{  maxIdentlimit = _maxIdentlimit; }
	
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
		
	inline void		setEmail( const string& _email )
		{ email = _email; }

	inline void		setCloneCidr( const int _clonecidr ) 
		{ clonecidr = _clonecidr; }
	
	inline void 	setActive( const int _active) 
		{ active = _active; }

	inline void 	setNoGline( const int _nogline)
		{ nogline = _nogline; }

	inline void 	setGlunidented( const int _glunidented) 
		{ glunidented = _glunidented; }

	inline void 	setForcecount( const int _forcecount) 
		{ forcecount = _forcecount; }

	inline void 	setv6( const int _v6) 
		{ v6 = _v6; }

	inline void 	setGroup( const int _group) 
		{ group = _group; }

	inline void 		incCount( const int _count ) 
		{ count += _count; }
			
	inline void		setSqldb(dbHandle* _SQLDb)
		{ SQLDb = _SQLDb; }
	
	int		loadData(const string& Name);
	
	int		updateData();
				
	bool		Insert();
	
	bool		Delete();
	
	static unsigned int numAllocated;

	ipLclonesMapType 	ipLidentclonesMap;


protected:
	
	int		id;
	int		forcecount;
	int		glunidented;
	int		v6;
	int		group;
	string 		Name;
	string 		email;
	int		clonecidr;
	int 		maxlimit;
	int 		maxIdentlimit;
	int		count;
	int		active;
	int		nogline;
	string 		AddedBy;
	time_t		AddedOn;
	string 		ModBy;
	time_t		ModOn;
	dbHandle* SQLDb;

}; 

class ccIpLnb
{
public:	
	ccIpLnb(dbHandle* _SQLDb);
	virtual ~ccIpLnb();
	
	inline bool operator==( const string& ExceptionHost ) const
		{ return (!strcasecmp( cidr, ExceptionHost )
		    || !match(cidr.c_str(),ExceptionHost.c_str())) ; }

	inline bool		isActive() const
		{ return ipLisp->isActive(); }

	inline bool		isNoGline() const
		{ return ipLisp->isNoGline(); }

	inline bool		isForcecount() const
		{ return ipLisp->isForcecount(); }

	inline const string& 		getCidr() const
			{ return cidr; }
		
	inline const string& 		getCidr1() const
			{ return cidr1; }

	inline const int& 		getCidr2() const
			{ return cidr2; }

	inline const int& 		getIpLispID() const
			{ return ipLispid; }

	inline const int& 		getCount() const
			{ return count; }

	inline const ccIpLisp*		getIpLisp() const
			{ return ipLisp; }
	
	inline const int& 		getCloneCidr() const
			{ return ipLisp->getCloneCidr(); }

	inline const int& 		getLimit() const
			{ return ipLisp->getLimit(); }

	inline const int& 		getIdentLimit() const
			{ return ipLisp->getIdentLimit(); }

	inline const string&		getAddedBy() const
		{ return AddedBy; }
		
	inline time_t			getAddedOn() const
		{ return AddedOn; }
	
	inline void 		setIpLisp( ccIpLisp* _ipLisp ) 
		{ ipLisp = _ipLisp; }

	inline void 		setIpLispID( const int _ipLispid ) 
		{ ipLispid = _ipLispid; }
			
	inline void 		incCount( const int _count ) 
		{ count += _count; }
			
	inline void		setAddedBy( const string& _AddedBy) 
		{ AddedBy = _AddedBy; }
		
	inline void		setAddedOn( const time_t _AddedOn )
		{ AddedOn = _AddedOn; }

	void 		setCidr( const string& _cidr );

	inline void		setSqldb(dbHandle* _SQLDb)
		{ SQLDb = _SQLDb; }



	bool		Insert();
	
	bool		Delete();

	ipLclonesMapType 	ipLclonesMap;


	ccIpLisp*	ipLisp;
	
	static unsigned int numAllocated;

protected:

	int		ipLispid;
	int		count;
	string 		cidr;
	string 		cidr1;
	int		cidr2;
	string 		AddedBy;
	time_t		AddedOn;
	dbHandle* SQLDb;

}; 
}
}
#endif // __CCEXCEPTION_H
