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
#include <functional>
#include <typeindex>
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
#include "ip.h"

/**
 * Main logging macro for simple formatted messages.
 * Automatically passes the current function name and supports format strings.
 * Usage: LOG(INFO, "User {} connected", username);
 */
#ifdef HAVE_FORMAT
#define LOG(x, ...)  logger->writeFunc(x, __PRETTY_FUNCTION__, "", __VA_ARGS__)

/**
 * SQL error logging macro for database-related errors.
 * Automatically formats SQL error messages from database objects.
 */
#define LOGSQL_ERROR(x)  logger->writeFunc(ERROR, __PRETTY_FUNCTION__, "", "SQL Error: {}", x->ErrorMessage())
#else
#define LOG(x, ...) do { } while(0)
#define LOGSQL_ERROR(x) elog << "SQL Error: " << x->ErrorMessage() << std::endl ;
#endif

/**
 * Structured logging macro with template support and field extraction.
 * Allows mixing format arguments with named placeholders and structured fields.
 * Usage: LOG_MSG(INFO, "User {} joined {channel}", username).with("channel", chanPtr).logStructured();
 */
#define LOG_MSG(level, template_msg, ...) \
    logger->createMessage(level, __PRETTY_FUNCTION__, template_msg, ##__VA_ARGS__)

namespace gnuworld
{

class xClient ;

/**
 * Verbosity levels for logging system.
 * Higher numbers indicate more verbose logging.
 * SQL is a special category for database query logging.
 */
enum Verbosity {
  TRACE = 6,  // Most verbose - detailed execution traces
  DEBUG = 5,  // Debug information for development
  INFO = 4,   // General informational messages
  WARN = 3,   // Warning messages for potential issues
  ERROR = 2,  // Error messages for failures
  FATAL = 1,  // Critical errors that may cause shutdown
  SQL = 99    // Special category for SQL query logging
} ;

/**
 * Main logging system for GNUWorld services.
 * Provides structured JSON logging, IRC channel output, file logging,
 * and extensible notification systems. Supports both simple text logging
 * and rich structured logging with custom object handlers.
 */
class Logger
{
private:
  xClient*                          bot = nullptr ;
  std::string                       logFilePath ;
  std::string                       debugChan ;
  unsigned short                    chanVerbosity = 4 ;
  unsigned short                    logVerbosity = 6 ;
  unsigned short                    consoleVerbosity = 6 ;
  bool                              logSQL = false ;
  bool                              consoleSQL = false ;
  std::ofstream                     logFile ;
  std::vector<
    std::pair<
      std::shared_ptr<
        notifier >,
          unsigned short > >        notifiers ;

  /**
   * Registry for custom object handlers that modules can register.
   * Maps type_index to handler functions for structured logging of custom objects.
   */
  std::map<
    std::type_index, std::function<
      bool( std::map<
          std::string, std::string >&,
          const std::string&, const void* ) > > customHandlers ;

#ifdef USE_THREAD
  std::mutex                        logMutex ;
#endif

  /**
   * Structure defining display properties for each verbosity level.
   * Contains IRC tag, prefix, and full name for each logging level.
   */
  struct LevelEntry {
    const char* tag ;     // Short tag for IRC display
    const char* prefix ;  // Prefix for log entries
    const char* name ;    // Full name for JSON logging
  } ;

  /**
   * Parses __PRETTY_FUNCTION__ to extract readable function names.
   * Removes template parameters and keeps class::function format.
   */
  std::string parseFunction( std::string ) ;

  /**
   * Returns IRC color code for the given verbosity level.
   * Used to colorize messages in IRC channels.
   */
  std::string getColour( Verbosity v )
    {
    switch( v )
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

  /**
   * Returns IRC color reset code for the given verbosity level.
   * Resets color formatting after colored messages.
   */
  std::string getColourReset( Verbosity v )
    { return getColour( v ).empty() ? "" : "\003" ; }

  /**
   * Stream-based logging interface for << operator usage.
   * Accumulates messages in a buffer and flushes on std::endl.
   * Provides a familiar iostream-style interface for logging.
   */
  class LoggerStream
    {
    public:
      /**
       * Constructor for LoggerStream.
       * Associates the stream with a logger instance and verbosity level.
       */
      LoggerStream( Logger& logger, Verbosity v )
            : logger( logger ), verbosity( v ) {}

      /**
       * Template operator<< for accumulating log message content.
       * Stores all streamed values in an internal buffer.
       */
      template< typename T >
      LoggerStream& operator<<( const T& value )
        {
        messageBuffer << value ;
        return *this ;
        }

      /**
       * Special operator<< for stream manipulators like std::endl.
       * Flushes the accumulated message when std::endl is encountered.
       */
      LoggerStream& operator<<( std::ostream& (*fp)( std::ostream& ) )
        {
        if( fp == static_cast< std::ostream& (*)( std::ostream& )>( std::endl ) )
          { flush() ; }
          return *this ;
        }

    private:
      Logger& logger ;
      Verbosity verbosity ;
      std::ostringstream messageBuffer ;

      /**
       * Flushes the accumulated message buffer to the logger.
       * Clears the buffer after sending the message.
       */
      void flush()
        {
        std::string message = messageBuffer.str() ;
        logger.write( verbosity, message ) ;
        messageBuffer.str( "" ) ;
        messageBuffer.clear() ;
        }
    } ;

public:

  /**
   * Constructor - initializes logger with bot instance.
   * Opens log file based on bot's config filename.
   */
  Logger( xClient* _bot ) ;

  /**
   * Destructor - ensures log file is properly closed.
   */
  ~Logger() ;

  /**
   * Template-based structured logging class.
   * Supports format arguments, named placeholders, and custom object serialization.
   * Provides both human-readable messages and machine-parseable JSON output.
   */
  class MessageTemplate {
  private:
      std::string template_str ;                      // Original template string
      std::map< std::string, std::string > fields ;   // Key-value pairs for JSON
      Verbosity level ;                               // Log level
      const char* func ;                              // Function name
      Logger* logger_instance ;                       // Logger reference
      std::vector< std::string > format_args ;        // Positional format arguments

      /**
       * Converts format arguments to strings for template replacement.
       * Handles different types and ensures proper string conversion.
       */
      template< typename T >
      std::string formatArgumentToString( T&& arg ) const
        {
        if constexpr( std::is_same_v< std::decay_t<T>, std::string > )
            return arg ;
        else if constexpr( std::is_same_v< std::decay_t<T>, const char* > )
            return std::string( arg ) ;
        else
            return std::to_string( arg ) ;
        }

      /**
       * Builds JSON string from the fields map.
       * Handles proper JSON escaping and type formatting.
       */
      std::string buildJsonFromMap() const ;

  public:
      /**
       * Constructor for MessageTemplate with optional format arguments.
       * Stores template string and converts format arguments for later use.
       */
      template< typename... FormatArgs >
      MessageTemplate( Logger* logger, Verbosity lvl, const char* f, const std::string& tmpl, FormatArgs&&... args )
          : template_str( tmpl ), level( lvl ), func( f ), logger_instance( logger )
        {
        if constexpr( sizeof...( args ) > 0 )
          {
          // Store format arguments for later formatting
          ( format_args.push_back( formatArgumentToString( std::forward<FormatArgs>( args ) ) ), ... ) ;
          }
        }

      /**
       * Generic with() method for adding structured data fields.
       * Attempts custom object handlers first, then falls back to built-in types.
       * Supports basic types, IRC objects, and module-registered custom types.
       */
      template< typename T >
      MessageTemplate& with( const std::string& key, T&& value )
        {
        auto handlerIt = logger_instance->customHandlers.find( std::type_index( typeid( std::decay_t<T> ) ) ) ;
        if( handlerIt != logger_instance->customHandlers.end() )
          {
          if constexpr( std::is_pointer_v< std::decay_t<T> > )
            {
            if (handlerIt->second(fields, key, static_cast<const void*>(value)))
                return *this ;
            }
          else
            {
            if (handlerIt->second(fields, key, &value))
                return *this ;
            }
          }
        else if constexpr( std::is_same_v< std::decay_t<T>, std::string > )
          fields[ key ] = value ;
        else if constexpr( std::is_same_v< std::decay_t<T>, const char* > )
          fields[ key ] = std::string( value ) ;
        else if constexpr( std::is_same_v< std::decay_t<T>, bool > )
          fields[ key ] = value ? "true" : "false" ;
        else if constexpr( std::is_same_v< std::decay_t<T>, iClient* > )
          return withClient( key, value ) ;
        else if constexpr( std::is_same_v< std::decay_t<T>, Channel* > )
          return withChannel( key, value ) ;
        else if constexpr( std::is_same_v< std::decay_t<T>, iServer* > )
          return withServer( key, value ) ;
        else if constexpr( std::is_same_v< std::decay_t<T>, ChannelUser* > )
          return withChannelUser( key, value ) ;
        else if constexpr( std::is_pointer_v< std::decay_t<T> > )
          {
          if( value )
            fields[ key ] = "0x" + std::to_string( reinterpret_cast<uintptr_t>( value ) ) ;
          else
            fields[ key ] = "nullptr" ;
          }
        else
          fields[ key ] = std::to_string( value ) ;

        return *this ;
        }

      /**
       * Specialized handler for iClient* objects.
       * Extracts comprehensive client information including nick, host, auth status, etc.
       */
      MessageTemplate& withClient( const std::string& prefix, iClient* client )
        {
        if( !client )
          {
          fields[ prefix + "_nick" ] = "nullptr" ;
          return *this ;
          }

        fields[ prefix + "_nick" ] = client->getNickName() ;
        fields[ prefix + "_userhost" ] = client->getNickUserHost() ;
        //fields[ prefix + "_host" ] = client->getInsecureHost() ;
        //fields[ prefix + "_realhost" ] = client->getRealInsecureHost() ;
        fields[ prefix + "_ip" ] = xIP( client->getIP() ).GetNumericIP() ;
        fields[ prefix + "_numeric" ] = client->getCharYYXXX() ;

        // Add authentication info if available
        if( client->isModeR() )
          {
          fields[ prefix + "_account" ] = client->getAccount() ;
          fields[ prefix + "_account_id" ] = std::to_string( client->getAccountID() ) ;
          }

        fields[ prefix + "_is_oper" ] = client->isOper() ? "true" : "false" ;
        //fields[ prefix + "_is_hidden" ] = client->isModeX() ? "true" : "false" ;

        return *this ;
        }

      /**
       * Specialized handler for iServer* objects.
       * Extracts server information including name, numeric, uplink, and status flags.
       */
      MessageTemplate& withServer( const std::string& prefix, iServer* server )
        {
        if( !server )
          {
          fields[ prefix + "_name" ] = "nullptr" ;
          return *this ;
          }

        fields[ prefix + "_name" ] = server->getName() ;
        fields[ prefix + "_numeric" ] = server->getCharYY() ;
        fields[ prefix + "_uplink" ] = server->getUplinkIntYY() ;

        // Add server flags
        //fields[ prefix + "_is_service" ] = server->isService() ? "true" : "false" ;
        //fields[ prefix + "_is_hub" ] = server->isHub() ? "true" : "false" ;
        fields[ prefix + "_is_bursting" ] = server->isBursting() ? "true" : "false" ;

        return *this ;
        }

      /**
       * Specialized handler for Channel* objects.
       * Extracts channel information including name, creation time, modes, etc.
       */
      MessageTemplate& withChannel( const std::string& prefix, Channel* channel )
        {
        if( !channel )
          {
          fields[ prefix + "_name" ] = "nullptr" ;
          return *this ;
          }

        fields[ prefix + "_name" ] = channel->getName() ;
        //fields[ prefix + "_creation_ts" ] = std::to_string( channel->getCreationTime() ) ;
        fields[ prefix + "_modes" ] = channel->getModeString() ;

        return *this ;
        }

      /**
       * Specialized handler for ChannelUser* objects.
       * Extracts channel-specific user information like op/voice status and modes.
       */
      MessageTemplate& withChannelUser( const std::string& prefix, ChannelUser* theUser )
        {
        if( !theUser )
          return *this ;

        fields[ prefix + "_is_op" ] = theUser->getMode( ChannelUser::MODE_O ) ? "true" : "false" ;
        fields[ prefix + "_is_voice" ] = theUser->getMode( ChannelUser::MODE_V ) ? "true" : "false" ;
        //fields[ prefix + "_channel_modes" ] = theUser->getModeString() ;

        return *this ;
        }

      /**
       * Replaces both positional {} placeholders and named {field} placeholders.
       */
      std::string format() const ;

      /**
       * Executes the structured logging operation.
       * Sends both formatted message and JSON fields to the logging system.
       */
      void logStructured() const ;
  } ;

  /**
   * Factory method to create a MessageTemplate with no format arguments.
   * Used by the LOG_MSG macro for template-based structured logging.
   */
  MessageTemplate createMessage( Verbosity level, const char* func, const std::string& templateStr )
    { return MessageTemplate( this, level, func, templateStr ) ; }

  /**
   * Template factory method to create a MessageTemplate with format arguments.
   * Supports variadic templates for flexible format argument handling.
   */
  template< typename... FormatArgs >
  MessageTemplate createMessage( Verbosity level, const char* func, const std::string& templateStr, FormatArgs&&... args )
    { return MessageTemplate( this, level, func, templateStr, std::forward<FormatArgs>( args )... ) ; }

  /**
   * Creates a LoggerStream for iostream-style logging.
   * Allows usage like: logger.write(INFO) << "Message" << std::endl;
   */
  LoggerStream write( Verbosity v )
    { return LoggerStream( *this, v ) ; }

  /**
   * Static array defining properties for each verbosity level.
   * Contains tag, prefix, and name for consistent level handling.
   */
  static constexpr std::array< LevelEntry, 7 > levels { {
    {"", "", ""},  // Dummy element at index 0
    {"[F]", "FATAL", "FATAL"},    // FATAL = 1
    {"[E]", "ERROR", "ERROR"},    // ERROR = 2
    {"[W]", "WARN ", "WARNING"},  // WARN  = 3
    {"[I]", "INFO ", "INFO"},     // INFO  = 4
    {"[D]", "DEBUG", "DEBUG"},    // DEBUG = 5
    {"[T]", "TRACE", "TRACE"}     // TRACE = 6
  } } ;

  /**
   * Sets the IRC channel name for debug output.
   * Messages will be sent to this channel based on chanVerbosity setting.
   */
  inline void setChannel( const std::string& channelName )
    { debugChan = channelName ; }

  /**
   * Sets the verbosity level for IRC channel output.
   * Only messages at or below this level will be sent to the debug channel.
   */
  inline void setChanVerbosity( unsigned short level )
    { chanVerbosity = level ; }

  /**
   * Sets the verbosity level for log file output.
   * Only messages at or below this level will be written to the log file.
   */
  inline void setLogVerbosity( unsigned short level )
    { logVerbosity = level ; }

  /**
   * Enables or disables SQL query logging.
   * When enabled, SQL queries will be logged to the file.
   */
  inline void setLogSQL( bool enable )
    { logSQL = enable ; }

  /**
   * Sets the verbosity level for console output.
   * Only log messages at or below this level will be displayed on the console.
   */
  inline void setConsoleVerbosity( unsigned short level )
    { consoleVerbosity = level ; }

  /**
   * Enables or disables SQL query logging to the console.
   * When enabled, SQL-related log messages will be displayed on the console.
   */
  inline void setConsoleSQL( bool enable )
    { consoleSQL = enable ; }

  /**
   * Returns the currently configured debug channel name.
   */
  inline std::string getChannel() const
    { return debugChan ; }

  /**
   * Adds a notifier with specific verbosity level.
   * Notifiers receive log messages and can send them to external services.
   */
  inline void addNotifier( std::shared_ptr< notifier > _notifier, unsigned short _verbosity )
    { notifiers.push_back( std::make_pair( std::move( _notifier ), _verbosity ) ) ; }

  /**
   * Adds a notifier with maximum verbosity level (receives all messages).
   */
  inline void addNotifier( std::shared_ptr< notifier > _notifier )
    { notifiers.push_back( std::make_pair( std::move( _notifier ), TRACE ) ) ; }

  /**
   * Removes a notifier from the notification list.
   */
  inline void removeNotifier( std::shared_ptr< notifier > _notifier )
    {
      notifiers.erase(
        std::remove_if( notifiers.begin(), notifiers.end(),
          [&_notifier]( const auto& pair ) { return pair.first == _notifier ; } ),
        notifiers.end() ) ;
    }

  /**
   * Updates the verbosity level for an existing notifier.
   */
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

  /**
   * Checks if a message should be processed based on verbosity settings.
   * Returns true if the message should be logged to any output destination.
   */
  inline bool shouldLog( Verbosity v ) const
    {
    // Special case for SQL logging
    if( v == SQL )
      return logSQL ;

    // Check if message meets any output threshold
    if( v <= logVerbosity )
      return true ;

    if( v <= chanVerbosity )
      return true ;

    // Check notifiers
    for( const auto& [ notifier, logLevel ] : notifiers )
      {
      if( v <= logLevel )
        return true ;
      }

    return false ;
    }

  /**
   * Registers a custom object handler for structured logging.
   * Allows modules to register handlers for their own object types.
   * Handler function receives fields map, key, and object pointer.
   */
  template< typename T >
  inline void registerObjectHandler( std::function< bool( std::map< std::string, std::string >&, const std::string&, T* ) > handler )
    {
    // Register with the pointer type since that's what with() will look for
    customHandlers[ std::type_index( typeid( T* ) ) ] =
        [ handler ]( std::map< std::string, std::string >& fields, const std::string& key, const void* obj ) -> bool {
            return handler( fields, key, static_cast<T*>( const_cast<void*>( obj ) ) ) ;
            } ;
    }

  /**
   * Core logging function that handles message distribution.
   * Sends messages to log file, IRC channel, and notifiers based on verbosity levels.
   */
  void writeFunc( Verbosity, const char*, const std::string&, const std::string& ) ;

  /**
   * Helper function for writing JSON-formatted entries to the log file.
   * Creates structured log entries with timestamp, level, function, and message.
   */
  void writeLog( Verbosity, const std::string&, const std::string&, const std::string& ) ;

  /**
   * Simple logging function without function name or JSON parameters.
   */
  void write( Verbosity v, const std::string& theMessage )
    { writeFunc( v, "", string(), theMessage ) ; }

  /**
   * Template logging function with format string support.
   * Provides printf-style formatting for log messages.
   */
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
   * Main logging function with caller function information.
   * Called by the LOG macro to provide formatted logging with function context.
   * Supports format strings and JSON parameter injection.
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

  /**
   * Closes and reopens the log file for external log rotation support.
   * Called from xServer::rotateLogs() when a SIGHUP is received.
   */
  void rotateLogs() ;
} ; // class Logger

} // namespace gnuworld
