/**
 * constants.h
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
 * Contains things that are used more than once across
 * the module but which need to remain uniform.
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include	<string>

namespace gnuworld {

namespace ds {

namespace sql {

	const std::string fakeclients = "SELECT fc.id, fc.nickname, "
		"fc.username, fc.hostname, fc.realname, u.user_name, "
		"fc.created_by, fc.created_on, fc.last_updated, fc.flags "
		"FROM fakeclients AS fc JOIN users AS u ON "
		"fc.created_by=u.id";

} // namespace sql

} // namespace ds

} // namespace gnuworld

#endif
