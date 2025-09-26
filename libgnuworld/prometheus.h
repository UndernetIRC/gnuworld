/**
 * prometheus.h
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
#include <memory>
#include <unordered_map>
#include "defs.h"

#ifdef HAVE_PROMETHEUS
  #include <prometheus/exposer.h>
  #include <prometheus/registry.h>
  #include <prometheus/counter.h>
  #include <prometheus/gauge.h>
#endif

#include "gnuworld_config.h"
#include "notifier.h"
#include "client.h"

namespace gnuworld
{

class PrometheusClient : public notifier
{

public:
  PrometheusClient( xClient*, const std::string& ip, unsigned short port ) ;

  virtual ~PrometheusClient() = default ;

  [[nodiscard]] size_t getSuccessful() const override
      { return statSuccessful ; }

  [[nodiscard]] size_t getErrors() const override
      { return statErrors ; }

  bool sendMessage( int level, const std::string message ) override ;

  // Increment a counter by name, creating it if it doesn't exist
  void incrementCounter( const std::string& counterName ) ;

  // Increment a counter by a specific value
  void incrementCounterBy( const std::string& counterName, double value ) ;

  // Set a gauge value (for metrics that can go up and down)
  void setGauge( const std::string& gaugeName, double value ) ;

private:
  xClient* bot = nullptr ;

  inline std::string sanitizeMetricName( const std::string& name )
  {
    std::string sanitized = bot->getNickName() + "_" + name ;
    std::replace( sanitized.begin(), sanitized.end(), '.', '_' ) ;
    std::transform( sanitized.begin(), sanitized.end(), sanitized.begin(), ::tolower ) ;
    return sanitized ;
  }

#ifdef HAVE_PROMETHEUS
  prometheus::Exposer exposer_ ;
  std::shared_ptr< prometheus::Registry > registry_ ;
  
  // Map to store counters by name
  std::unordered_map< std::string, prometheus::Counter* > counters_ ;
  std::unordered_map< std::string, prometheus::Gauge* > gauges_ ;
#endif
  
  size_t            statSuccessful = 0 ;
  size_t            statErrors = 0 ;

} ;

} // namespace gnuworld
