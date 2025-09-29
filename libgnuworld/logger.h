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
#include <fstream>
#ifdef HAVE_FORMAT
 #include <format>
#endif
#ifdef USE_THREAD
  #include <mutex>
#endif
#include <map>
#include <array>
#include <memory>

#include "notifier.h"

/* Logger macro. Passes on the caller function. */
#ifdef HAVE_FORMAT
#define LOG(x, ...)  logger->writeFunc(x, __PRETTY_FUNCTION__, "", __VA_ARGS__)
#define LOGSQL_ERROR(x)  logger->writeFunc(ERROR, __PRETTY_FUNCTION__, "", "SQL Error: {}", x->ErrorMessage())
#else
#define LOG(x, ...) do { } while(0)
#define LOGSQL_ERROR(x) elog << "SQL Error: " << x->ErrorMessage() << std::endl ;
#endif

namespace gnuworld
{

class xClient ;

/* Verbosity levels */
enum Verbosity {
  TRACE = 6,
  DEBUG = 5,
  INFO = 4,
  WARN = 3,
  ERROR = 2,
  FATAL = 1,
  SQL = 99
} ;

class Logger
{
private:
  xClient*                          bot = nullptr ;
  std::string                       debugChan ;
  unsigned short                    chanVerbosity = 2 ;
  unsigned short                    logVerbosity = 1 ;
  bool                              logSQL = false ;
  std::ofstream                     logFile ;
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

  std::string parseFunction( std::string ) ;

  std::string getColour( Verbosity v )
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

  std::string getColourReset( Verbosity v )
    { return getColour( v ).empty() ? "" : "\003" ; }

  class LoggerStream
    {
    public:
      LoggerStream( Logger& logger, Verbosity v )
            : logger( logger ), verbosity( v ) {}

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
      Verbosity verbosity ;
      std::ostringstream messageBuffer ;

      // Flush the message to the Logger's write function
      void flush()
        {
        std::string message = messageBuffer.str() ;
        logger.write( verbosity, message ) ;
        messageBuffer.str( "" ) ;
        messageBuffer.clear() ;
        }
    } ;

public:

  /* Constructor. */
  Logger( xClient* _bot ) ;

  /* Destructor. */
  ~Logger() ;

  LoggerStream write( Verbosity v )
    { return LoggerStream( *this, v ) ; }

  static constexpr std::array< LevelEntry, 7 > levels { {
    {"", "", ""},  // Dummy element at index 0
    {"[F]", "FATAL", "FATAL"},    // FATAL = 1
    {"[E]", "ERROR", "ERROR"},    // ERROR = 2
    {"[W]", "WARN ", "WARNING"},  // WARN  = 3
    {"[I]", "INFO ", "INFO"},     // INFO  = 4
    {"[D]", "DEBUG", "DEBUG"},    // DEBUG = 5
    {"[T]", "TRACE", "TRACE"}     // TRACE = 6
  } } ;

  /* Setter functions for Admin and Debug channels */
  inline void setChannel( const std::string& channelName )
    { debugChan = channelName ; }

  inline void setChanVerbosity( unsigned short level )
    { chanVerbosity = level ; }

  inline void setLogVerbosity( unsigned short level )
    { logVerbosity = level ; }

  inline void setLogSQL( bool enable )
    { logSQL = enable ; }

  /* Getter functions for Admin and Debug channels */
  inline std::string getChannel() const
    { return debugChan ; }

  /* Enables pushover by storing a shared pointer. */
  inline void addNotifier( std::shared_ptr< notifier > _notifier, unsigned short _verbosity )
    { notifiers.push_back( std::make_pair( std::move( _notifier ), _verbosity ) ) ; }

  /* Enables notifier with maximum verbosity level. */
  inline void addNotifier( std::shared_ptr< notifier > _notifier )
    { notifiers.push_back( std::make_pair( std::move( _notifier ), TRACE ) ) ; }

  /* Disables pushover by resetting the shared pointer. */
  inline void removeNotifier( std::shared_ptr< notifier > _notifier )
    {
      notifiers.erase(
        std::remove_if( notifiers.begin(), notifiers.end(),
          [&_notifier]( const auto& pair ) { return pair.first == _notifier; } ),
        notifiers.end() ) ;
    }

  /* Update verbosity level for a notifier. */
  inline void updateNotifierVerbosity( std::shared_ptr< notifier > _notifier, unsigned short _verbosity )
    {
    for( auto& pair : notifiers )
      {
      if( pair.first == _notifier )
        {
        pair.second = _verbosity ;
        break ;
        }
      }
    }

  /* Log to the debug and admin channel */
  void writeFunc( Verbosity, const char*, const std::string&, const std::string& ) ;

  /* Helper function to write to the logfile. */
  void writeLog( Verbosity, const std::string&, const std::string&, const std::string& ) ;

  void write( Verbosity v, const std::string& theMessage )
    { writeFunc( v, "", string(), theMessage ) ; }

  template< typename Format, typename... Args >
  void write( Verbosity v, const Format& format, Args&&... args )
  {
#ifdef HAVE_FORMAT
  std::string fmtString = std::vformat( format,
    std::make_format_args( args... ) ) ;
  writeFunc( v, "", string(), fmtString ) ;
#endif
  }

  /**
   * Logger function that takes caller function as an argument.
   * Called by the LOG macro.
   */
  template< typename Format, typename... Args >
  void writeFunc( Verbosity v, const char* func, const std::string& jsonParams, const Format& format, Args&&... args )
  {
#ifdef HAVE_FORMAT
    std::string fmtString = std::vformat( format,
      std::make_format_args( args... ) ) ;
    writeFunc( v, func, jsonParams, fmtString ) ;
#endif
  }
} ; // class Logger

} // namespace gnuworld
