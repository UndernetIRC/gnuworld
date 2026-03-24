/**
 * sqlChanOp.cc
 *
 * Author: Matthias Crauwels <ultimate_@wol.be>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * $Id: sqlChanOp.cc,v 1.6 2010/03/04 04:24:12 hidden1 Exp $
 */

#include <sstream>
#include <string>

#include "dbHandle.h"

#include "ELog.h"
#include "misc.h"

#include "chanfix.h"
#include "sqlChanOp.h"

namespace gnuworld {

namespace cf {

sqlChanOp::sqlChanOp(sqlManager* _myManager, unsigned int daySamples)
    : channel(""), account(""), nickUserHost(""), points(0), ts_firstopped(0), ts_lastopped(0),
      day(daySamples, 0) {
    myManager = _myManager;
    OldestOp = false;
    dirty = false;
}

void sqlChanOp::setAllMembers(dbHandle* theDB, int row) {
    channel = theDB->GetValue(row, 0);
    account = theDB->GetValue(row, 1);
    nickUserHost = theDB->GetValue(row, 2);
    ts_firstopped = atoi(theDB->GetValue(row, 3));
    ts_lastopped = atoi(theDB->GetValue(row, 4));
    /* Day points are loaded separately from chanops_daily table */
    calcTotalPoints();
    dirty = false;
}

void sqlChanOp::calcTotalPoints() {
    points = 0;
    for (size_t i = 0; i < day.size(); i++) {
        points += day[i];
    }
}

sqlChanOp::~sqlChanOp() {
    // No heap space allocated
}

} // namespace cf

} // namespace gnuworld
