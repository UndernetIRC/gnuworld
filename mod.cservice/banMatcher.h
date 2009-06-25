/**
 * banMatcher.h
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
 * $Id: banMatcher.h,v 1.3 2009/06/25 19:59:05 mrbean_ Exp $
 */

#ifndef __BANMATCHER_H
#define __BANMATCHER_H "$Id: banMatcher.h,v 1.3 2009/06/25 19:59:05 mrbean_ Exp $"

#include	<string>
#include	<ctime>
#include	"iClient.h"
 
using std::string ;

namespace gnuworld
{ 
 
class banMatcher
{

public:
	virtual bool matches(iClient*) = 0;
	
	virtual ~banMatcher(){}
} ;

} 
#endif // __BANMATCHER_H
