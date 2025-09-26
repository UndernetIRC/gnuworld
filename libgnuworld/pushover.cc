/**
 * pushover.cc
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

#include "defs.h"
#ifdef HAVE_LIBCURL
  #include <curl/curl.h>
#endif

#include <iostream>
#include <sstream>
#include <algorithm>

#include "ELog.h"
#include "client.h"
#include "pushover.h"
#include "threadworker.h"
#include "logger.h"

namespace gnuworld
{

PushoverClient::PushoverClient( xClient* _bot, std::string token,
                                pushoverKeysType users
#ifdef USE_THREAD
                               , ThreadWorker* worker
#endif
)
  : bot( _bot ), apiToken( token ), userKeys( users )
#ifdef USE_THREAD
  , threadWorker( worker )
#endif
{}

#ifdef HAVE_LIBCURL
void PushoverClient::initialise_curl()
{
  static bool initialized = false ;
  if( !initialized )
    {
    curl_global_init( CURL_GLOBAL_DEFAULT ) ;
    initialized = true ;
    }
}
#endif

bool PushoverClient::sendMessage( int level, const std::string message )
{
#ifdef HAVE_FORMAT
  return sendMessage( std::format( "[{}] {}", bot->getNickName(), Logger::levels[ level ].name ), message ) ;
#else
  return false ;
#endif
}

bool PushoverClient::sendMessage( const std::string title,
                                  const std::string message )
{
  return sendMessage( title, message, 0, 60, 3600 ) ;
}

bool PushoverClient::sendMessage( const std::string title,
                                  const std::string message,
                                  int priority,
                                  int retry,
                                  int expire )
{
#ifdef USE_THREAD
  if( threadWorker )
    {
    threadWorker->submitJob( [this, title, message, priority, retry, expire]() {
      this->processMessage( title, message, priority, retry, expire ) ;
    } ) ;
    return true; // Assume success for async operations
    }
  else
#endif
    {
    bool ok = processMessage( title, message, priority, retry, expire ) ;
    if( !ok )
      {
      elog << "[PUSHOVER-ERROR] Failed to send message: " << message << std::endl ;
      statErrors++ ;
      }
    return ok ;
    }
}

bool PushoverClient::processMessage( const std::string title,
                                     const std::string message,
                                     int priority,
                                     int retry,
                                     int expire )
{
bool allSucceeded = true ;

for( const auto& user : userKeys )
  {
  bool ok = sendToUser( user, title, message, priority, retry, expire ) ;
  if( !ok )
    {
    elog << "[PUSHOVER-ERROR] Failed to send to user: " << user << std::endl ;
    statErrors++ ;
    allSucceeded = false;
    }
  }

return allSucceeded ;
}
 
bool PushoverClient::sendToUser( [[maybe_unused]] const std::string user,
                                 [[maybe_unused]] const std::string title,
                                 [[maybe_unused]] const std::string message,
                                 [[maybe_unused]] int priority,
                                 [[maybe_unused]] int retry,
                                 [[maybe_unused]] int expire )
{
#ifdef HAVE_LIBCURL
try {
  initialise_curl() ;

  std::string safeTitle = truncate( title, 250 ) ;
  std::string safeMessage = truncate( message, 1024 ) ;

  std::ostringstream postData ;
  postData  << "token=" << apiToken
            << "&user=" << user
            << "&title=" << safeTitle
            << "&message=" << safeMessage
            << "&priority=" << priority ;

  if( priority == 2 )
    {
    retry = std::max( 10, retry ) ;
    expire = std::min( 10800, expire ) ;
    postData << "&retry=" << retry << "&expire=" << expire ;
    }

  std::string url = "https://api.pushover.net/1/messages.json" ;
  std::string body = postData.str() ;

  CURL* curl = curl_easy_init() ;
  if( nullptr == curl )
    throw std::runtime_error( "curl_easy_init failed" ) ;

  struct curl_slist* hdrs = nullptr ;
  hdrs = curl_slist_append( hdrs, "Content-Type: application/x-www-form-urlencoded" ) ;

  curl_easy_setopt( curl, CURLOPT_URL, url.c_str() ) ;
  curl_easy_setopt( curl, CURLOPT_HTTPHEADER, hdrs ) ;
  curl_easy_setopt( curl, CURLOPT_POST, 1L ) ;
  curl_easy_setopt( curl, CURLOPT_COPYPOSTFIELDS, body.c_str() ) ;
  curl_easy_setopt( curl, CURLOPT_CONNECTTIMEOUT, 3L ) ;
  curl_easy_setopt( curl, CURLOPT_TIMEOUT, 5L ) ;
  curl_easy_setopt( curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_1_1 ) ;

  CURLcode rc = curl_easy_perform( curl ) ;
  curl_slist_free_all( hdrs ) ;
  curl_easy_cleanup( curl ) ;
  if( rc != CURLE_OK )
    throw std::runtime_error( curl_easy_strerror( rc ) ) ;

  statSuccessful++ ;
  elog << "[PUSHOVER] Message sent to user: " << user
       << " with title: " << safeTitle
       << " and message: " << safeMessage
       << std::endl ;
  return true ;
} catch( const std::exception& e ) {
  elog << "[PUSHOVER-ERROR] exception: " << e.what() << std::endl ;
} catch( ... ) {
  elog << "[PUSHOVER-ERROR] unknown exception" << std::endl ;
}
#endif
return false ;
}

} // namespace gnuworld
