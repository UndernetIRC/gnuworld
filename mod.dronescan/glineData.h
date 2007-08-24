/**
 * glineData.h
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

#ifndef GLINEDATA_H
#define GLINEDATA_H

namespace gnuworld {

namespace ds {


/**
 * This class is a data container for glines which are in the gline queue
 */
class glineData {
public:
	/** Constructor sets initial state to UNKNOWN. */
	inline glineData(std::string _host, std::string _reason, time_t _expires)
		{ host = _host; reason = _reason; expires = _expires;}

	inline const std::string getHost() const
		{ return host; }

	inline const std::string getReason() const
		{ return reason; }

	inline const time_t getExpires() const
		{ return expires; }


protected:
	/** The gline host */
	std::string	host;
	
	/** The gline reason */
	std::string 	reason;
	
	/** The gline expiry */
	time_t 		expires;

}; // class glineData

} // namespace ds

} // namespace gnuworld

#endif
