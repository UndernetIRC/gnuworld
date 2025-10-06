/**
 * logger.cc
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
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sstream>

#ifdef HAVE_FORMAT
  #include <format>
#endif

#include "Channel.h"
#include "Network.h"
#include "ELog.h"

#include "logger.h"

namespace gnuworld
{

using std::endl ;
using std::string ;

/**
 * Logger constructor - initializes logging system.
 * Opens log file based on bot's configuration filename.
 * Log file path is derived by replacing .conf extension with .log.
 */
Logger::Logger( xClient* _bot ) : bot( _bot )
{
// Build and save log file path for later use
size_t dotPos = bot->getConfigFileName().find( '.' ) ;
if( dotPos != std::string::npos )
  logFilePath = bot->getConfigFileName().substr( 0, dotPos ) + ".log" ;
else
  logFilePath = bot->getConfigFileName() + ".log" ;

logFile.open( logFilePath, std::ios::app ) ;
if( !logFile.is_open() )
  elog << "Warning: Could not open logfile " << logFilePath << std::endl ;
}

/**
 * Logger destructor - ensures proper cleanup.
 * Closes log file if it was successfully opened.
 */
Logger::~Logger()
{
if( logFile.is_open() )
  logFile.close() ;
}

/**
 * Writes structured JSON log entries to the log file.
 * Creates properly formatted JSON with timestamp, level, function, parameters, and message.
 * All string values are properly escaped for valid JSON output.
 */
void Logger::writeLog( Verbosity v, const std::string& func, const std::string& jsonParams, const std::string& theMessage )
{
if( logFile.is_open() )
  {
  logFile << "{"
          << "\"timestamp\":\"" << getCurrentTimestamp() << "\","
          << "\"level\":\"" << ( v == SQL ? "SQL" : levels[ v ].name ) << "\"" ;
  if( !func.empty() )
    logFile << ",\"function\":\"" << escapeJsonString( func ) << "\"" ;
  if( !jsonParams.empty() )
    logFile << "," << jsonParams ;
  if( !theMessage.empty() )
    logFile << ",\"message\":\"" << escapeJsonString( theMessage ) << "\"" ;

  logFile << "}\n" ;
  logFile.flush() ;
  }
}

/**
 * Main message distribution function for the logging system.
 * Handles thread safety, verbosity filtering, and message routing to:
 * - Log file (JSON format)
 * - IRC debug channel (formatted text)
 * - External notifiers (alerts, webhooks, etc.)
 * Special handling for SQL logging category.
 */
void Logger::writeFunc( Verbosity v, const char* func, const string& jsonParams, const string& theMessage )
{
#ifdef USE_THREAD
/* We don't want multiple threads sharing the same logger object to interfer with each other. */
std::lock_guard< std::mutex > lock( logMutex ) ;
#endif

std::string fmtMessage = ( v == INFO ? "" : parseFunction( func ) + "> " ) + theMessage ;

/* Is this an SQL log? */
if( v == SQL )
  {
  if( logSQL )
    writeLog( v, parseFunction( func ), jsonParams, theMessage ) ;

  if( consoleSQL && elog.getStream() )
    *(elog.getStream()) << elog.getLocalTime()
                        << "[" << bot->getNickName() << "] - "
                        << levels[ v ].prefix << " - "
                        << fmtMessage
                        << endl ;
  return ;
  }

/* Write to logfile. */
if( v <= logVerbosity )
  writeLog( v, parseFunction( func ), jsonParams, theMessage ) ;

/* Write to console. */
if( v <= consoleVerbosity && elog.getStream() )
  *(elog.getStream()) << elog.getLocalTime()
                      << "[" << bot->getNickName() << "] - "
                      << levels[ v ].prefix << " - "
                      << fmtMessage
                      << endl ;

/* Send notification. */
for( const auto& [ notifier, logLevel ] : notifiers )
  {
  if( v <= logLevel )
    {
    notifier->sendMessage( static_cast< int >( v ), fmtMessage ) ;
    }
  }

#ifdef HAVE_FORMAT
if( v <= chanVerbosity && bot->isConnected() && !debugChan.empty() )
  {
  /* Try to locate the channel. */
  Channel* theChan = Network->findChannel( debugChan ) ;
  if( !theChan )
    {
    elog  << "[" << bot->getNickName() << "] - WARN  - write> Unable to locate channel "
          << debugChan
          << " on network!"
          << endl ;
    }
  else
    {
    std::istringstream stream( fmtMessage ) ;
    std::string line ;

    while( std::getline( stream, line ) )
      if( !line.empty() )
          bot->getUplink()->serverNotice( theChan, std::format( "{}[{}] {} {}{}",
            getColour( v ), bot->getNickName(), levels[ v ].tag, line,
            getColourReset( v ) ) ) ;
    }
  }
#endif
}

/**
 * Parses __PRETTY_FUNCTION__ output to extract readable function names.
 * Removes template parameters and return types, keeping class::function format.
 * Handles both C++ member functions and standalone functions.
 */
std::string Logger::parseFunction( std::string pretty )
{
auto paren = pretty.find( '(' ) ;
if( paren != std::string::npos )
  pretty.erase( paren ) ;

auto lastColons = pretty.rfind( "::" ) ;
if( lastColons != std::string::npos )
  {
  // Find the second-to-last "::" to get class::function
  auto secondLastColons = pretty.rfind( "::", lastColons - 1 ) ;
  if( secondLastColons != std::string::npos )
    return pretty.substr( secondLastColons + 2 ) ;
  else
    return pretty ;
  }
return pretty ;
}

/**
 * Builds JSON field string from the MessageTemplate's field map.
 * Properly formats different data types (strings, numbers, booleans).
 * Handles JSON escaping and type detection for valid JSON output.
 */
std::string Logger::MessageTemplate::buildJsonFromMap() const
{
if( fields.empty() )
  return "" ;

std::string result ;
bool first = true ;
for( const auto& [k, v] : fields )
  {
  if( !first )
    result += "," ;

  // Determine if value should be quoted
  bool isNumeric = !v.empty() && ( std::isdigit( v[ 0 ] ) ||
                  ( v[ 0 ] == '-' && v.length() > 1 && std::isdigit( v[ 1 ] ) ) ) ;
  bool isBool = ( v == "true" || v == "false" ) ;

  result += "\"" + escapeJsonString( k ) + "\":" ;
  if( isNumeric || isBool )
    result += v ;
  else
    result += "\"" + escapeJsonString( v ) + "\"" ;

  first = false ;
  }
return result ;
}

/**
 * Formats the template string for human-readable output.
 * Two-phase replacement:
 * 1. Replaces positional {} placeholders with format arguments
 * 2. Replaces named {field} placeholders with values from .with() calls
 * Produces clean, readable log messages.
 */
std::string Logger::MessageTemplate::format() const
{
std::string result = template_str ;

// First, apply format arguments to {} placeholders
if( !format_args.empty() )
  {
  size_t arg_index = 0 ;
  size_t pos = 0 ;
  while( ( pos = result.find( "{}", pos ) ) != std::string::npos && arg_index < format_args.size() )
    {
    result.replace( pos, 2, format_args[ arg_index ] ) ;
    pos += format_args[ arg_index ].length() ;
    arg_index++ ;
    }
  }

// Then, replace named placeholders from .with() fields
for( const auto& [k, v] : fields )
  {
  std::string placeholder = "{" + k + "}" ;
  size_t pos = result.find( placeholder ) ;
  if( pos != std::string::npos )
    {
    result.replace( pos, placeholder.length(), v ) ;
    }
  }

return result ;
}

/**
 * Executes the structured logging operation.
 * Combines human-readable message formatting with structured JSON field logging.
 * Calls the main logging system with both formatted message and JSON parameters.
 */
void Logger::MessageTemplate::logStructured() const
{
// Early exit if message won't be logged anywhere
if( !logger_instance->shouldLog( level ) )
  return ;

// Create clean message (replace placeholders with actual values)
std::string clean_msg = format() ;

// Build JSON fields string
std::string jsonFields = buildJsonFromMap() ;

// Call the lower-level writeFunc directly
logger_instance->writeFunc( level, func, jsonFields, clean_msg ) ;
}

/**
 * Closes and reopens the log file for external log rotation support.
 * This function should be called after external log rotation (e.g., via logrotate)
 * to ensure the logger writes to the new log file instead of the rotated one.
 * Thread-safe when USE_THREAD is enabled.
 */
void Logger::rotateLogs()
{
#ifdef USE_THREAD
std::lock_guard< std::mutex > lock( logMutex ) ;
#endif

// Close existing log file if open
if( logFile.is_open() )
  logFile.close() ;

// Reopen log file using saved path
logFile.open( logFilePath, std::ios::app ) ;
if( !logFile.is_open() )
  {
  elog << "Warning: Could not reopen logfile " << logFilePath << " after rotation" << std::endl ;
  }
}
} // namespace gnuworld
