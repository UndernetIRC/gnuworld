/**
 * levels.h
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

#ifndef LEVELS_H
#define LEVELS_H

namespace gnuworld {

namespace ds {

namespace level {

	/* Try to keep anything non-authed opers need below 200 */

	/* Standard level is 300 */

	const unsigned short access	= 300;
	const unsigned short check	= 300;
	const unsigned short fake	= 300;
	const unsigned short list	= 300;
	const unsigned short status	= 300;

	/* Higher admin commands */

	const unsigned short adduser	= 750;
	const unsigned short moduser	= 750;
	const unsigned short remuser	= 750;
	const unsigned short addExceptionalChannel = 750;
	const unsigned short remExceptionalChannel = 750;
	const unsigned short reload = 750;

	/* Commands at 1000 should really be debug commands only */

	const unsigned short analyse	= 1000;
	const unsigned short quote	= 1000;

} // namespace level

} // namespace ds

} // namespace gnuworld

#endif
