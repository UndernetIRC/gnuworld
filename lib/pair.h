/* pair.h */

#ifndef __PAIR_H
#define __PAIR_H "$Id: pair.h,v 1.1 2001/07/29 22:44:06 dan_karrels Exp $"

namespace gnuworld
{

template< class type1, class type2 >
class pair
{

public:
	typedef type1 first_type ;
	typedef type2 second_type ;

	first_type	first ;
	second_type	second ;

	pair( const first_type& _first, const second_type& _second )
	 : first( _first ), second( _second )
	{}

	pair()
	 : first( first_type() ), second( second_type() )
	{}

} ;

} // namespace gnuworld

#endif // __PAIR_H
