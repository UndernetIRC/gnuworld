#ifndef _STATS_H
#define _STATS_H "$Id: Stats.h,v 1.2 2002/09/05 16:02:42 dan_karrels Exp $"

#include <map>
#include <string>

using std::string ;
using std::map ;

namespace gnuworld {

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
