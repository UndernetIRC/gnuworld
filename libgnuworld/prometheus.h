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

/**
 * @class PrometheusClient
 * @brief Prometheus metrics client that exposes gnuworld statistics via HTTP
 *
 * The PrometheusClient integrates with the Prometheus monitoring system by:
 * - Exposing metrics via HTTP endpoint (typically /metrics)
 * - Implementing the notifier interface for log-based metrics
 * - Managing counters and gauges for application statistics
 * - Automatically sanitizing metric names to comply with Prometheus standards
 *
 * Metric Types:
 * - Counters: Monotonically increasing values (e.g., connection count, errors)
 * - Gauges: Values that can go up/down (e.g., active users, memory usage)
 *
 * Thread Safety:
 * - All public methods are thread-safe
 * - Uses internal synchronization for metric registration
 *
 * Example Usage:
 * @code
 * try {
 *   auto prometheus = std::make_unique<PrometheusClient>(this, "0.0.0.0", 9090);
 *   prometheus->incrementCounter("user_connections");
 *   prometheus->setGauge("active_channels", 150);
 * } catch (const std::exception& e) {
 *   // Handle initialization failure gracefully
 * }
 * @endcode
 */
class PrometheusClient : public notifier
{

public:
  /**
   * @brief Construct a new Prometheus Client
   *
   * Initializes the Prometheus exposer and registry, starts the HTTP server
   * to serve metrics on the specified IP and port.
   *
   * @param bot Pointer to the xClient instance (for logging and identification)
   * @param ip IP address to bind the HTTP server (e.g., "0.0.0.0", "127.0.0.1")
   * @param port Port number for the HTTP server (e.g., 9090)
   *
   * @throws std::runtime_error If the HTTP server cannot bind to the specified address/port
   * @throws std::invalid_argument If parameters are invalid
   *
   * @note The constructor will throw if the port is already in use or if
   *       the Prometheus library cannot initialize properly
   */
  PrometheusClient( xClient* bot, const std::string& ip, unsigned short port ) ;

  /**
   * @brief Destroy the Prometheus Client
   *
   * Automatically stops the HTTP server and cleans up all metrics.
   */
  virtual ~PrometheusClient() = default ;

  /**
   * @return size_t Count of successful metric updates (counters + gauges)
   */
  [[nodiscard]] size_t getSuccessful() const override
      { return statSuccessful ; }

  /**
   * @return size_t Count of failed metric operations (due to invalid names, etc.)
   */
  [[nodiscard]] size_t getErrors() const override
      { return statErrors ; }

  /**
   * @brief This method is called automatically by the logging system
   *        when this client is registered as a notifier
   *        Increments counters based on log severity levels.
   */
  bool sendMessage( int level, const std::string message ) override ;

  /**
   * @brief Increment a counter by 1
   *
   * Creates the counter if it doesn't exist, then increments by 1.
   * Counter names are automatically sanitized to comply with Prometheus naming rules.
   * The counter name will be prefixed with the bot's nickname to avoid conflicts.
   *
   * @param counterName Name of the counter (will be sanitized)
   *
   * @note Counters are monotonically increasing - use for events like:
   *       - Connection attempts
   *       - Messages processed
   *       - Errors encountered
   *
   * @warning This method is thread-safe but metric creation is not atomic.
   *          Multiple threads creating the same metric simultaneously may cause issues.
   */
  void incrementCounter( const std::string& counterName ) ;

  /**
   * @brief Increment a counter by a specific value
   *
   * Creates the counter if it doesn't exist, then increments by the specified amount.
   * Useful for batch operations or weighted metrics.
   *
   * @param counterName Name of the counter (will be sanitized)
   * @param value Amount to increment (must be >= 0 for counters)
   *
   * @note If value is negative, it will be treated as 0 (counters cannot decrease)
   */
  void incrementCounterBy( const std::string& counterName, double value ) ;

  /**
   * @brief Set a gauge to a specific value
   *
   * Creates the gauge if it doesn't exist, then sets it to the specified value.
   * Gauges can increase or decrease and represent current state.
   * The gauge name will be prefixed with the bot's nickname to avoid conflicts.
   *
   * @param gaugeName Name of the gauge (will be sanitized)
   * @param value Value to set the gauge to (can be any double)
   *
   * @note Gauges are for values that can go up/down - use for metrics like:
   *       - Active user count
   *       - Memory usage
   *       - Queue lengths
   *       - Temperature readings
   */
  void setGauge( const std::string& gaugeName, double value ) ;

private:
  xClient* bot = nullptr ;

  /**
   * @brief Sanitize metric names to comply with Prometheus standards
   *
   * Prometheus metric names must match: [a-zA-Z_:][a-zA-Z0-9_:]*
   * This function:
   * - Converts to lowercase
   * - Replaces invalid characters with underscores
   * - Ensures the first character is valid
   * - Adds bot name prefix to avoid conflicts
   *
   * @param name Original metric name
   * @return std::string Sanitized metric name safe for Prometheus
   *
   * @note Examples:
   *       - "user-count" → "botname_user_count"
   *       - "123invalid" → "botname_123invalid"
   *       - "SASL.SUCCESS" → "botname_sasl_success"
   */
  std::string sanitizeMetricName( const std::string& name ) ;

#ifdef HAVE_PROMETHEUS
  /** @brief Prometheus HTTP server for exposing metrics */
  prometheus::Exposer exposer_ ;

  /** @brief Registry for all metrics - shared pointer for thread safety */
  std::shared_ptr< prometheus::Registry > registry_ ;

  /** @brief Map of counter names to counter instances for fast lookup */
  std::unordered_map< std::string, prometheus::Counter* > counters_ ;

  /** @brief Map of gauge names to gauge instances for fast lookup */
  std::unordered_map< std::string, prometheus::Gauge* > gauges_ ;
#endif

  /** @brief Count of successful metric operations */
  size_t statSuccessful = 0 ;

  /** @brief Count of failed metric operations */
  size_t statErrors = 0 ;

} ;

} // namespace gnuworld
