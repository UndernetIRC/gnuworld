/* netData.cc
 * This class stores information about a given iClient.
 * It is used as a quick reference to logged in users and for checking warnings
 */

#include "netData.h"

const char netData_h_rcsId[] = _NETDATA_H;
const char netData_cc_rcsId[] = "$Id: netData.cc,v 1.1 2002/08/10 15:41:02 jeekay Exp $";

namespace gnuworld
{

namespace ns
{

netData::netData() :
  authedUser(0),
  warned(0)
{
}

netData::~netData()
{
}

} // namespace ns

} // namespace gnuworld
