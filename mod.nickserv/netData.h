#ifndef _NETDATA_H
#define _NETDATA_H "$Id: netData.h,v 1.1 2002/08/10 15:41:02 jeekay Exp $"

#include "sqlUser.h"

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
}; // class netData

} // namespace ns

} // namespace gnuworld

#endif
