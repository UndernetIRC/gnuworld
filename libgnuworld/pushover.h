/**
 * pushover.h
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

#include <string>
#include <vector>

#include "gnuworld_config.h"
#include "notifier.h"

namespace gnuworld
{

typedef std::vector< std::string > pushoverKeysType ;
#ifdef USE_THREAD
class ThreadWorker ;
#endif

class PushoverClient : public notifier
{

public:
  PushoverClient( std::string token, pushoverKeysType users
#ifdef USE_THREAD
                , ThreadWorker* worker
#endif
                ) ;

  virtual ~PushoverClient() = default ;

  void setUserKeys( const pushoverKeysType& newUsers )
      { userKeys = newUsers ; }

  void setToken( const std::string& newToken )
      { apiToken = newToken ; }

  [[nodiscard]] size_t getSuccessful() const override
      { return statSuccessful ; }

  [[nodiscard]] size_t getErrors() const override
      { return statErrors ; }

  [[nodiscard]] size_t userKeys_size() const
      { return userKeys.size() ; }

  // Virtual override of base class 2-parameter method
  bool sendMessage( const std::string title,
                    const std::string message ) override ;

  // Legacy method with int parameters for backward compatibility
  bool sendMessage( const std::string title,
                    const std::string message,
                    int priority,
                    int retry = 60,
                    int expire = 3600 ) ;

private:
  std::string       apiToken ;
  pushoverKeysType  userKeys ;
  size_t            statSuccessful = 0 ;
  size_t            statErrors = 0 ;

#ifdef USE_THREAD
  ThreadWorker*     threadWorker = nullptr ;
#endif

#ifdef HAVE_LIBCURL
  void initialise_curl() ;
#endif

  std::string truncate( const std::string input, std::size_t maxLength ) const
      { return input.size() <= maxLength ? input : input.substr( 0, maxLength - 3 ) + "..." ; }

  bool processMessage( const std::string title,
                       const std::string message,
                       int priority,
                       int retry,
                       int expire ) ;

  bool sendToUser( const std::string user,
                   const std::string title,
                   const std::string message,
                   int priority,
                   int retry,
                   int expire ) ;
} ;

} // namespace gnuworld
