/**
 * prometheus.cc
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
#include <sstream>
#include <algorithm>
#include <cctype>

#include "ELog.h"
#include "prometheus.h"
#include "threadworker.h"
#include "logger.h"

namespace gnuworld
{

#ifdef HAVE_PROMETHEUS
// Real implementation when Prometheus is available

PrometheusClient::PrometheusClient( xClient* _bot, const std::string& ip, unsigned short port )
  : bot{ _bot }, exposer_{ ip + ":" + std::to_string( port ) }, registry_{ std::make_shared< prometheus::Registry >() }
{ 
  // Register the registry with the exposer to start serving metrics
  // The exposer automatically runs in its own thread
  exposer_.RegisterCollectable( registry_ ) ;
  
  // Set startup timestamp - monitoring tools can calculate uptime from this
  setGauge( "start_time", static_cast< double >( ::time( nullptr ) ) ) ;
  
  elog << "*** [PrometheusClient]: Metrics server started on " << ip << ":" << port << std::endl ;
  elog << "*** [PrometheusClient]: Metrics available at http://" << ip << ":" << port << "/metrics" << std::endl ;
}

std::string PrometheusClient::sanitizeMetricName( const std::string& name )
{
std::string sanitized = bot->getNickName() + "_" + name ;
std::transform( sanitized.begin(), sanitized.end(), sanitized.begin(), ::tolower ) ;

/* Replace invalid characters with underscores. */
for( auto& c : sanitized )
  if( !std::isalnum(c) && c != '_' && c != ':' )
    c = '_' ;

return sanitized ;
}

void PrometheusClient::incrementCounter( const std::string& counterName )
{
#ifdef HAVE_PROMETHEUS
  std::string sanitizedName = sanitizeMetricName( counterName ) ;  
  // Check if counter already exists
  auto it = counters_.find( sanitizedName ) ;
  if( it != counters_.end() )
    {
    // Counter exists, increment it
    it->second->Increment() ;
    }
  else
    {
    // Counter doesn't exist, create it
    auto& counterFamily = prometheus::BuildCounter()
                          .Name( sanitizedName )
                          .Help( "Auto-generated counter for " + counterName )
                          .Register( *registry_ ) ;
                          
    auto& counter = counterFamily.Add( {} ) ;
    counters_[ sanitizedName ] = &counter ;
    
    // Increment the newly created counter
    counter.Increment() ;

    elog << "PrometheusClient: Created new counter '" << sanitizedName << "'" << std::endl ;
    }
#endif
}

void PrometheusClient::incrementCounterBy( const std::string& counterName, double value )
{
  std::string sanitizedName = sanitizeMetricName( counterName ) ;
  
  // Check if counter already exists
  auto it = counters_.find( sanitizedName ) ;
  if( it != counters_.end() )
    {
    // Counter exists, increment it by value
    it->second->Increment( value ) ;
    }
  else
    {
    // Counter doesn't exist, create it
    auto& counterFamily = prometheus::BuildCounter()
                          .Name( sanitizedName )
                          .Help( "Auto-generated counter for " + counterName )
                          .Register( *registry_ ) ;
                          
    auto& counter = counterFamily.Add( {} ) ;
    counters_[ sanitizedName ] = &counter ;
    
    // Increment the newly created counter by value
    counter.Increment( value ) ;
    
    elog << "PrometheusClient: Created new counter '" << sanitizedName << "'" << std::endl ;
    }
}

void PrometheusClient::setGauge( const std::string& gaugeName, double value )
{
  std::string sanitizedName = sanitizeMetricName( gaugeName ) ;
  
  // Check if gauge already exists
  auto it = gauges_.find( sanitizedName ) ;
  if( it != gauges_.end() )
    {
    // Gauge exists, set its value
    it->second->Set( value ) ;
    }
  else
    {
    // Gauge doesn't exist, create it
    auto& gaugeFamily = prometheus::BuildGauge()
                        .Name( sanitizedName )
                        .Help( "Auto-generated gauge for " + gaugeName )
                        .Register( *registry_ ) ;
                        
    auto& gauge = gaugeFamily.Add( {} ) ;
    gauges_[ sanitizedName ] = &gauge ;
    
    // Set the newly created gauge value
    gauge.Set( value ) ;
    
    elog << "PrometheusClient: Created new gauge '" << sanitizedName << "'" << std::endl ;
    }
}

bool PrometheusClient::sendMessage( int level, const std::string )
{
  // Use Logger's getLevelName to get consistent level names
  std::string levelName = string_lower( Logger::levels[ static_cast< Verbosity >( level ) ].name ) ;
  
  // Increment counter for this specific log level
  incrementCounter( "log_" + levelName + "_total" ) ;
  
  // Also increment general log counter
  incrementCounter( "log_messages_total" ) ;

  ++statSuccessful ;
  return true ;
}

#else

// Stub implementation when Prometheus is not available
PrometheusClient::PrometheusClient( xClient* _bot, const std::string&, unsigned short )
  : bot{ _bot }
{ 
  elog << "PrometheusClient: Prometheus support not compiled in (stub mode)" << std::endl ;
}

void PrometheusClient::incrementCounter( const std::string& )
{ }

void PrometheusClient::incrementCounterBy( const std::string&, double )
{ }

void PrometheusClient::setGauge( const std::string&, double )
{ }

bool PrometheusClient::sendMessage( int, const std::string )
{ return true ; }

#endif // HAVE_PROMETHEUS

} // namespace gnuworld
