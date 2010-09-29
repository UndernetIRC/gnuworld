/**
 * Buffer.cc
 * Author: Daniel Karrels dan@karrels.com
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>    
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
 * $Id: Buffer.cc,v 1.6 2003/07/20 14:13:15 dan_karrels Exp $
 */

#include	<string>

#include	"Buffer.h"

const char rcsId[] = "$Id: Buffer.cc,v 1.6 2003/07/20 14:13:15 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;

Buffer::Buffer( char delimiter )
 : delim( delimiter )
{}

Buffer::~Buffer()
{ /* No heap space allocated */ }

bool Buffer::ReadLine( string& retMe )
{
/* remove any leading new line characters */
while( !empty() && ('\n' == buf[ 0 ]) )
	{
	/* just remove the first char */
	buf.erase( buf.begin() ) ;
	}

/* Check to see if there is anything left */
if( empty() )
	{
	return false ;
	}

/* find() returns the index of the character, or 
 * npos if it was not found
 */
size_type pos = buf.find( delim ) ;

if( string::npos == pos )
	{
	// Unable to find the delimiter
	return false ;
	}

// else I need to add this substring into retMe
// and remove it from buf
retMe = buf.substr( 0, pos + 1 ) ;
buf.erase( 0, pos + 1 ) ;

return true ;
}

// Delete numBytes bytes from the beginning
// of the Buffer
void Buffer::Delete( const size_type& numBytes )
{
if( numBytes >= size() )
	{
	// Clear the entire Buffer
//	buf.clear() ;
	buf.erase() ;
	return ;
	}

// Else just erase the number of bytes given
buf.erase( 0, numBytes ) ;
}

} // namespace gnuworld
