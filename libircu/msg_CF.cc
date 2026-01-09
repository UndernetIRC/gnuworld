/**
 * msg_CF.cc
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

 #include	<string>
 #include	<iostream>
 
 #include	"gnuworld_config.h"
 #include	"ServerCommandHandler.h"
 #include	"server.h"
 #include	"xparameters.h"
 #include	"Channel.h"
 #include	"Network.h"
 #include	"iClient.h"
 #include	"ELog.h"
 
 namespace gnuworld
 {
 
 CREATE_HANDLER(msg_CF)
 
 /**
  * CONFIGURATION message handler.
  * 
  * This message is used to propagate network configuration
  * source CF <timestamp> <key> :<value>
  */
 bool msg_CF::Execute( const xParameters& Param )
 {
 if( Param.size() < 3 )
     {
     elog	<< "msg_CF> Invalid number of parameters"
         << std::endl ;
     return false ;
     }
 
 iServer* sourceServer = Network->findServer( Param[ 0 ] ) ;
 if( NULL == sourceServer )
     {
     elog	<< "msg_CF> Unable to find source server: "
         << Param[ 0 ] << std::endl ;
     return false ;
     }

time_t timestamp = atoi( Param[ 1 ] ) ;
std::string key( Param[ 2 ] ) ;
std::string value( Param.assemble( 3 ) ) ;

auto netConf = Network->findNetConf( key ) ;
if( netConf && netConf->second > timestamp )
    {
    // This should never happen.
    elog	<< "msg_CF> Netconf variable already exists and is newer: "
            << key
            << " (timestamp on file: " << netConf->second << ", timestamp on network: " << timestamp << ")"
            << std::endl ;
    return false ;
    }
 
 Network->addNetConf( key, value, timestamp ) ;

 elog	<< "msg_CF> Adding netconf variable: "
        << key
        << " (value: " << value << ")"
        << " (timestamp: " << timestamp << ")"
        << std::endl ;

 // Post event to listening clients
 theServer->PostEvent( EVT_NETCONF, static_cast< void* >( sourceServer ), 
     static_cast< void* >( &key ) ) ;
 
 // Return success
 return true ;
 }
 
 } // namespace gnuworld
