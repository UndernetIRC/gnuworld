#ifndef _NETDATA_H
#define _NETDATA_H "$Id: netData.h,v 1.2 2002/08/27 20:55:53 jeekay Exp $"

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
