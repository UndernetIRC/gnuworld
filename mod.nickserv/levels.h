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
 *
 * This file contains the various levels required to do various
 * interesting things.
 * Excuse the number of namespaces.
 */

#ifndef LEVELS_H
#define LEVELS_H

namespace gnuworld {

namespace ns {

namespace level {

namespace admin {
  const unsigned int stats = 1;

  const unsigned int info = 100;
  const unsigned int invite = 100;

  const unsigned int remove = 500;

  const unsigned int consolelevel = 800;

  const unsigned int moduser = 850;

  const unsigned int shutdown = 900;
} // namespace admin

namespace set {
  const unsigned int logmask = 100;
} // namespace set

} // namespace level

} // namespace ns

} // namespace gnuworld

#endif
