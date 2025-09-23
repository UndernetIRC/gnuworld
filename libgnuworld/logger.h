/**
 * logger.h
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
#ifdef HAVE_FORMAT
#include <format>
#endif
#include <map>
#include <array>
#include <memory>
#ifdef USE_THREAD
#include <mutex>
#endif

#include "notifier.h"

/* Logger macro. Passes on the caller function. */
#ifdef HAVE_FORMAT
#define LOG(x, ...)  logger->writeFunc(x, __PRETTY_FUNCTION__, __VA_ARGS__)
#define LOGSQL_ERROR(x)  logger->writeFunc(ERROR, __PRETTY_FUNCTION__, "SQL Error: {}", x->ErrorMessage())
#else
#define LOG(x, ...)  
#define LOGSQL_ERROR(x) elog << "SQL Error: " << x->ErrorMessage() << std::endl ;
#endif

namespace gnuworld
{

class xClient;

/* Report levels */
enum Verbose {
  TRACE = 1,
  DEBUG = 2,
  INFO = 3,
  WARN = 4,
  ERROR = 5,
  FATAL = 6,
  SQL = 99
} ;

class Logger
{
private:
  xClient*                          bot = nullptr ;
  std::string                       debugChan ;
  unsigned short                    chanVerbose = 2 ;
  unsigned short                    logVerbose = 1 ;
  bool                              logSQL = false ;
  std::vector<
    std::pair<
      std::shared_ptr<
        notifier >,
          unsigned short > >        notifiers ;
#ifdef USE_THREAD
  std::mutex                        logMutex ;
#endif

  struct LevelEntry {
    const char* tag ;
    const char* prefix ;
    const char* name ;
  } ;

  static constexpr std::array< LevelEntry, 7 > levels { {
    {"", "", ""},  // Dummy element at index 0
    {"[T]", "TRACE", "TRACE"},    // TRACE = 1
    {"[D]", "DEBUG", "DEBUG"},    // DEBUG = 2
    {"[I]", "INFO ", "INFO"},     // INFO  = 3
    {"[W]", "WARN ", "WARNING"},  // WARN  = 4
    {"[E]", "ERROR", "ERROR"},    // ERROR = 5
    {"[F]", "FATAL", "FATAL"}     // FATAL = 6
  } } ;

  std::string parseFunction( std::string ) ;

  std::string getColour( Verbose v )
    {
    switch ( v )
      {
      case FATAL:
      case ERROR:
        return "\00304" ;
      case WARN:
        return "\00307" ;
      default:
        return "" ;
      }
    }

  std::string getColourReset( Verbose v )
    { return getColour( v ).empty() ? "" : "\003" ; }

  class LoggerStream
    {
    public:
      LoggerStream( Logger& logger, Verbose v )
            : logger( logger ), verbose( v ) {}

      // Overload << operator to accumulate log messages
      template< typename T >
      LoggerStream& operator<<( const T& value )
        {
        messageBuffer << value ;
        return *this ;
        }

      // Handle std::endl and similar manipulators to flush the log
      LoggerStream& operator<<( std::ostream& (*fp)( std::ostream& ) )
        {
        if( fp == static_cast< std::ostream& (*)( std::ostream& )>( std::endl ) )
          { flush(); }
          return *this ;
        }

    private:
      Logger& logger ;
      Verbose verbose ;
      std::ostringstream messageBuffer ;

      // Flush the message to the Logger's write function
      void flush()
        {
        std::string message = messageBuffer.str() ;
        logger.write( verbose, message ) ;
        messageBuffer.str( "" ) ;
        messageBuffer.clear() ;
        }
    } ;

public:

  /* Constructor. */
  Logger( xClient* _bot) : bot( _bot ) {}

  LoggerStream write( Verbose v )
    { return LoggerStream( *this, v ) ; }

  /* Setter functions for Admin and Debug channels */
  inline void setChannel( const std::string& channelName )
    { debugChan = channelName ; }

  inline void setChanVerbose( unsigned short level )
    { chanVerbose = level ; }

  inline void setLogVerbose( unsigned short level )
    { logVerbose = level ; }

  inline void setLogSQL( bool enable )
    { logSQL = enable ; }

  /* Getter functions for Admin and Debug channels */
  inline std::string getChannel() const
    { return debugChan ; }

  /* Enables pushover by storing a shared pointer. */
  inline void addNotifier( std::shared_ptr< notifier > _notifier, unsigned short _verbose )
    { notifiers.push_back( std::make_pair( std::move( _notifier ), _verbose ) ) ; }

  /* Disables pushover by resetting the shared pointer. */
  inline void removeNotifier( std::shared_ptr< notifier > _notifier )
    {
      notifiers.erase( 
        std::remove_if( notifiers.begin(), notifiers.end(), 
          [&_notifier]( const auto& pair ) { return pair.first == _notifier; } ), 
        notifiers.end() ) ;
    }

  /* Update verbosity level for a notifier. */
  inline void updateNotifierVerbose( std::shared_ptr< notifier > _notifier, unsigned short _verbose )
    {
    for( auto& pair : notifiers )
      {
      if( pair.first == _notifier )
        {
        pair.second = _verbose ;
        break ;
        }
      }
    }

  /* Log to the debug and admin channel */
  void write( Verbose, const std::string& ) ;
#ifdef HAVE_FORMAT
  template< typename Format, typename... Args >
  void write( Verbose v, const Format& format, Args&&... args )
  {
  std::string fmtString = std::vformat( format,
    std::make_format_args( args... ) ) ;
  write( v, fmtString ) ;
  }

  /**
   * Logger function that takes caller function as an argument.
   * Called by the LOG macro.
   */
  template< typename Format, typename... Args >
  void writeFunc( Verbose v, const char* func, const Format& format, Args&&... args )
  {
  std::string fmtString = ( v == INFO ? "" : parseFunction( func ) + "> " ) + std::vformat( format,
    std::make_format_args( args... ) ) ;
  write( v, fmtString ) ;
  }
#endif
} ; // class Logger

} // namespace gnuworld
