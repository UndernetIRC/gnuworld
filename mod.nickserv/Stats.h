/**
 * Stats.h
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

#ifndef STATS_H
#define STATS_H

#include <map>
#include <string>

namespace gnuworld {

using std::map ;
using std::string ;

class Stats {
  public:
    /**
     * Implement Stats as a singleton
     * Only way to get a reference to the manager is through this method
     */
    static Stats* getInstance();

    /*********************
     ** T Y P E D E F S **
     *********************/

    typedef map< string, int > statsMapType;


    /*******************************
     ** S T A T S   C O N T R O L **
     *******************************/

    /** Increment a given statistic */
    void incStat(const string&, unsigned int = 1);

    /** Get given stat value */
    unsigned int getStat(const string&);

    /** Accessor for statsMap.begin() */
    inline const statsMapType::const_iterator getStatsMapBegin() const
      { return statsMap.begin(); }

    /** Accessor for statsMap.end() */
    inline const statsMapType::const_iterator getStatsMapEnd() const
      { return statsMap.end(); }

  protected:
    /**
     * Disable the default constructor so that instances can only be gotten
     * through getInstance()
     * @see #getInstance
     */
    Stats();

    /**
     * Disable the default destructor so that other objects cannot destruct
     * the instance they have a reference to.
     */
    ~Stats();

    /** The current instance of Stats */
    static Stats* theStats;

    /** The stats map */
    statsMapType statsMap;
}; // class Stats

} // namespace gnuworld

#endif
