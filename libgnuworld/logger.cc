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

/* Function for writing messages to the admin or debug channels. */
void Logger::write( Verbose v, const string& theMessage )
{
#ifdef USE_THREAD
/* We don't want multiple threads sharing the same logger object to interfer with each other. */
std::lock_guard< std::mutex > lock( logMutex ) ;
#endif

/* Is this an SQL log? */
if( v == SQL )
  {
  if( logSQL )
    {
    elog  << "[" << bot->getNickName() << "] - SQL   - "
          << theMessage
          << endl ;
    }

  return ;
  }

/* Write to elog. */
if( v >= logVerbose )
  {
  elog  << "[" << bot->getNickName() << "] - "
        << levels[ v ].prefix << " - "
        << theMessage
        << endl ;
  }

#ifdef HAVE_FORMAT
/* Send notification. */
for( const auto& [ notifier, logLevel ] : notifiers )
  {
  if( v >= logLevel )
    {
    notifier->sendMessage( std::format( "[{}] {}", bot->getNickName(), levels[ v ].name ),
      theMessage ) ;
    }
  }

if( v >= chanVerbose && bot->isConnected() && !debugChan.empty() )
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
    std::istringstream stream( theMessage ) ;
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
