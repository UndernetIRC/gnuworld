/* ccException.h */

#ifndef __CCEXCEPTION_H
#define __CCEXCEPTION_H "$Id: ccException.h,v 1.1 2001/06/07 15:59:06 mrbean_ Exp $"

#include	<string>

#include	<ctime>

#include	"libpq++.h"

namespace gnuworld
{ 

using std::string ;

class ccException
{
public:
	
	ccException(PgDatabase* _SQLDb);
	virtual ~ccException();
	
	inline bool operator==( const string& ExceptionHost ) const
		{ return (0 == strcasecmp( Host, ExceptionHost )) ; }
	
	inline const string 		get_Host() const
			{ return Host; }
		
	inline const int		get_Connections() const
		{ return Connections; }
	
	inline const string		get_AddedBy() const
		{ return AddedBy; }
		
	inline time_t			get_AddedOn() const
		{ return AddedOn; }

	inline void 		set_Host( const string& _Host ) 
		{ Host = _Host; }
		
	inline void		set_Connections( const int _Connections ) 
		{  Connections = _Connections; }
	
	inline void		set_AddedBy( const string& _AddedBy) 
		{ AddedBy = _AddedBy; }
		
	inline void		set_AddedOn( const time_t _AddedOn )
		{ AddedOn = _AddedOn; }
	
	int		loadData(const string& Host);
	
	int		updateData();
				
	bool		Insert();
	
	bool		Delete();
	
protected:
	
	string 		Host;
	int 		Connections;
	string 		AddedBy;
	time_t		AddedOn;
	PgDatabase* SQLDb;

}; 
}
#endif // __CCEXCEPTION_H
