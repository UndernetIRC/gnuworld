/*
 * This file contains the various levels required to do various
 * interesting things.
 * Excuse the number of namespaces.
 */

#ifndef _LEVELS_H
#define _LEVELS_H "$Id: levels.h,v 1.4 2002/11/25 04:48:40 jeekay Exp $"

namespace gnuworld {

namespace ns {

namespace level {

namespace admin {
  const unsigned int stats = 1;
  
  const unsigned int info = 100;
  
  const unsigned int remove = 500;

  const unsigned int consolelevel = 800;
  
  const unsigned int shutdown = 900;
} // namespace admin

namespace set {
  const unsigned int logmask = 100;
} // namespace set

} // namespace level

} // namespace ns

} // namespace gnuworld

#endif
