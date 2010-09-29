/**
 * netData.h
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

#ifndef NETDATA_H
#define NETDATA_H

#include "sqlUser.h"

#include "logTarget.h"

namespace gnuworld
{

namespace ns
{

class netData
{
  public:

    /** Empty Constructor */
   netData();

    /** Empty Destructor */
    ~netData();

    /** Pointer to sqlUser if authed */
    sqlUser* authedUser;

    /** Has this user been warned? */
    bool warned;

    /** This users logging status */
   logging::events::eventType logMask;
}; // class netData

} // namespace ns

} // namespace gnuworld

#endif
