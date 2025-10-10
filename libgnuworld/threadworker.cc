/**
 * threadworker.cc
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

#include <iostream>

#include "threadworker.h"
#include "misc.h"

namespace gnuworld
{

ThreadWorker::ThreadWorker( xClient* _bot ) : stop( false ), bot( _bot ), logger( _bot->getLogger() )
{
worker = std::thread( &ThreadWorker::run, this ) ;
}

ThreadWorker::~ThreadWorker()
{
  {
  std::lock_guard lock( mutex ) ;
  stop = true ;
  }
cv.notify_one() ;
if( worker.joinable() )
  worker.join() ;
}

void ThreadWorker::run()
{
while( true )
  {
  std::function< void() > job ;
    {
    std::unique_lock lock( mutex ) ;
    cv.wait( lock, [&] { return stop || !jobs.empty() ; } ) ;
    if( stop && jobs.empty() )
      return ;
    job = std::move( jobs.front() ) ;
    jobs.pop() ;
    }
    
  try
    {
    LOG( TRACE, "Starting job execution" ) ;
    job() ;
    }
  catch( const std::exception& e )
    {
    LOG( ERROR, "Exception in job execution: {}", e.what() ) ;
    }
  catch( ... )
    {
    LOG( ERROR, "Unknown exception in job execution" ) ;
    }
  }
}

} // namespace gnuworld