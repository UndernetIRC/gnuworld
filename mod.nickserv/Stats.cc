#include <new>

#include "Stats.h"

namespace gnuworld {

/* Initialise our static data members */
Stats* Stats::theStats = 0;

/*********************************
 ** S T A T I C   M E T H O D S **
 *********************************/

/**
 * Implementation of singleton. This is the only way to get a reference to a
 * Stats object.
 */
Stats* Stats::getInstance()
{
  if(!theStats) theStats = new Stats();
  
  return theStats;
}


/*********************************
 ** C O N / D E S T R U C T O R **
 *********************************/

/**
 * Empty constructor
 */
Stats::Stats()
{
}

/**
 * Empty destructor
 */
Stats::~Stats()
{
}


/*******************************
 ** S T A T S   C O N T R O L **
 *******************************/

/**
 * This function allows incrementing of stats
 */
void Stats::incStat(const string& theStat, unsigned int amount)
{
  statsMapType::iterator ptr = statsMap.find(theStat);
  
  if(ptr == statsMap.end()) {
    statsMap.insert(statsMapType::value_type(theStat, amount));
  } else {
    ptr->second += amount;
  }
}


/**
 * This function allows access to a single stats count
 */
unsigned int Stats::getStat(const string& theStat)
{
  statsMapType::const_iterator ptr = statsMap.find(theStat);
  
  if(ptr == statsMap.end()) {
    return 0;
   } else {
    return ptr->second;
   }
}

} // namespace gnuworld
