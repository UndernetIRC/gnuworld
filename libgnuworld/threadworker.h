/**
 * threadworker.h
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
 */

#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <string>
#include <atomic>
#include <functional>

#include "client.h"
#include "logger.h"

namespace gnuworld
{

class ThreadWorker {
private:
  std::queue< std::function< void() > > jobs ;
  mutable std::mutex mutex ;
  std::condition_variable cv ;
  std::thread worker ;
  bool stop ;

  /* The bot. */
  xClient* bot ;

  /* Pointer to the xClient's logger. */
  Logger* logger ;

  void run() ;

public:
  ThreadWorker( xClient* ) ;
  ~ThreadWorker() ;

  template< typename Callable >
  void submitJob( Callable&& task ) {
    {
      std::lock_guard< std::mutex > lock( mutex ) ;
      jobs.emplace( std::forward< Callable >( task ) ) ;
    }
    cv.notify_one() ;
  }

  bool hasPendingJobs() const
    {
    std::lock_guard< std::mutex > lock( mutex ) ;
    return !jobs.empty() ;
    }

  size_t getPendingJobCount() const
    {
    std::lock_guard< std::mutex > lock( mutex ) ;
    return jobs.size() ;
    }
} ;

} // namespace gnuworld