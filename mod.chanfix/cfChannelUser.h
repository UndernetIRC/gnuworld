/**
 * cfChannelUser.h
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

#ifndef CF_CFCHANNELUSER_H
#define CF_CFCHANNELUSER_H

#include <string>

namespace gnuworld {

namespace chanfix {

class cfChannelUser {
public:
	cfChannelUser(const std::string&);
	virtual ~cfChannelUser();

	void addPoints(unsigned int);

	inline void setPoints(unsigned int _points)
		{ points = _points; }

	inline unsigned int getPoints() const
		{ return points; }

	inline const std::string& getName() const
		{ return name; }


protected:
	std::string name;

	unsigned int points;
}; // class cfChannelUser;

} // namespace chanfix

} // namespace gnuworld

#endif
