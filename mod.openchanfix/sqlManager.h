/**
 * sqlManager.h
 *
 * Author: Rasmus Hansen <jeekay@netgamers.org>
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
 * $Id: sqlManager.h,v 1.1 2006/03/15 02:50:37 buzlip01 Exp $
 */

#ifndef __SQLMANAGER_H
#define __SQLMANAGER_H "$Id: sqlManager.h,v 1.1 2006/03/15 02:50:37 buzlip01 Exp $"

#include <string>
#include <vector>

#include "libpq++.h"

namespace gnuworld {

class sqlManager {
  public:
    /**
     * Implement sqlManager as a singleton
     * Only way to get a reference to the manager is through this method
     */
    static sqlManager* getInstance(const std::string&);

    /** Allow checking out of database connections */
    PgDatabase* getConnection();

    /** Allow checking in of database connections */
    void removeConnection(PgDatabase*);

    /** Allow removal of theManager */
    void removeManager();

  protected:
    /**
     * Disable the default constructor so that instances can only be gotten
     * through getInstance()
     * @see #getInstance
     */
    sqlManager(const std::string&);

    /**
     * Disable the default destructor so that other objects cannot destruct
     * the instance they have a reference to.
     */
    ~sqlManager();

    /** The string storing our DB connection path */
    std::string dbString;

    /** Our PgDatabase instance */
    PgDatabase* SQLDb;

    /** The current instance of sqlManager */
    static sqlManager* theManager;

}; // class sqlManager

} // namespace gnuworld

#endif // __SQLMANAGER_H
