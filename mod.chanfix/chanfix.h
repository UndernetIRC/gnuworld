/**
 * chanfix.h
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
 * $Id: chanfix.h,v 1.1 2004/05/16 11:29:32 jeekay Exp $
 */

#ifndef CHANFIX_H
#define CHANFIX_H

#include <string.h>

#include "client.h"

namespace gnuworld {

namespace chanfix {

class chanfix : public xClient {
public:
	/** Constructor taking a configuration filename. */
	chanfix( const std::string& );
	
	/** Destructor. */
	virtual ~chanfix();
};

} // namespace chanfix

} // namespace gnuworld

#endif
