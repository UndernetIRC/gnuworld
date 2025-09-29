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

#include "logger.h"

namespace gnuworld
{

using std::endl ;
using std::string ;

/* Constructor - open logfile */
Logger::Logger( xClient* _bot ) : bot( _bot )
{
std::string logFilePath ;
size_t dotPos = bot->getConfigFileName().find('.');
if( dotPos != std::string::npos )
  logFilePath = bot->getConfigFileName().substr( 0, dotPos ) + ".log" ;
else
  logFilePath = bot->getConfigFileName() + ".log" ;

logFile.open( logFilePath, std::ios::app ) ;
if( !logFile.is_open() )
  elog << "Warning: Could not open logfile " << logFilePath << std::endl ;
}

/* Destructor - close logfile */
Logger::~Logger()
{
if( logFile.is_open() )
  logFile.close() ;
}

void Logger::writeLog( Verbosity v, const std::string& func, const std::string& jsonParams, const std::string& theMessage )
{
if( logFile.is_open() )
  {
  logFile << "{"
          << "\"timestamp\":\"" << getCurrentTimestamp() << "\","
          << "\"level\":\"" << ( v == SQL ? "SQL" : levels[ v ].name ) << "\"," ;
  if( !func.empty() )
    logFile << "\"function\":\"" << escapeJsonString( func ) << "\"," ;
  if( !jsonParams.empty() )
    logFile << jsonParams << "," ;

  logFile << "\"message\":\"" << escapeJsonString( theMessage ) << "\"}\n";
  logFile.flush();
  }
}

/* Function for writing messages to the logfile and debug channel. Accepting json params and message. */
void Logger::writeFunc( Verbosity v, const char* func, const string& jsonParams, const string& theMessage )
{
#ifdef USE_THREAD
/* We don't want multiple threads sharing the same logger object to interfer with each other. */
std::lock_guard< std::mutex > lock( logMutex ) ;
#endif

/* Is this an SQL log? */
if( v == SQL )
  {
  if( logSQL )
    writeLog( v, parseFunction( func ), jsonParams, theMessage ) ;

  return ;
  }

/* Write to logfile. */
if( v <= logVerbosity )
  {
  /* Write to logfile */
  writeLog( v, parseFunction( func ), jsonParams, theMessage ) ;

  /* Keeping elog enabled for now... */
  elog  << "[" << bot->getNickName() << "] - "
        << levels[ v ].prefix << " - "
        << theMessage
        << endl ;
  }

std::string fmtMessage = ( v == INFO ? "" : parseFunction( func ) + "> " ) + theMessage ;

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

} // namespace gnuworld
