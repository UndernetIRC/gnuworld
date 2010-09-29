/**
 * Convert.h
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
 */

#ifndef CONVERT_H
#define CONVERT_H

#include <deque>
#include <string>

#include <ctime>

namespace gnuworld {

typedef std::deque<std::string> assembleType;

class Convert {
public:
	static std::string ConvertSecondsToString(time_t _seconds);

	static std::string Assemble(const assembleType& toAssm);

protected:
	/* Don't instantiate a static only class */
	Convert() { } ;
}; // class Convert

} // namespace gnuworld

#endif
