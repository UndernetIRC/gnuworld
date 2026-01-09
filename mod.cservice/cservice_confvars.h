/**
 * cservice_confvars.h
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

  #define CONFIG_VAR_LIST \
    CONFIG_VAR( std::string,  confSqlHost,                      "sql_host") \
    CONFIG_VAR( std::string,  confSqlPass,                      "sql_pass") \
    CONFIG_VAR( std::string,  confSqlDb,                        "sql_db") \
    CONFIG_VAR( std::string,  confSqlUser,                      "sql_user") \
    CONFIG_VAR( std::string,  confSqlPort,                      "sql_port") \
    CONFIG_VAR( std::string,  relayChan,                        "relay_channel") \
    CONFIG_VAR( std::string,  privrelayChan,                    "priv_relay_channel") \
    CONFIG_VAR( std::string,  debugChan,                        "debug_channel") \
    CONFIG_VAR( std::string,  coderChan,                        "coder_channel") \
    CONFIG_VAR( std::string,  pendingPageURL,                   "pending_page_url") \
    CONFIG_VAR( std::string,  locMessage,                       "loc_message") \
    CONFIG_VAR( std::string,  ChanfixServerName,                "chanfix_servername") \
    CONFIG_VAR( std::string,  limitJoinAllowedModes,            "limitjoin_allowedmodes") \
    CONFIG_VAR( std::string,  welcomeNewChanMessage,            "welcome_newchan_message") \
    CONFIG_VAR( std::string,  welcomeNewChanTopic,              "welcome_newchan_topic") \
    CONFIG_VAR( int,          updateInterval,                   "update_interval") \
    CONFIG_VAR( int,          expireInterval,                   "expire_interval") \
    CONFIG_VAR( int,          cacheInterval,                    "cache_interval") \
    CONFIG_VAR( int,          webrelayPeriod,                   "webrelay_interval") \
    CONFIG_VAR( int,          flood_duration,                   "flood_duration") \
    CONFIG_VAR( int,          topic_duration,                   "topic_duration") \
    CONFIG_VAR( int,          neverLoggedInUsersExpireTime,     "neverloggedin_users_expiretime") \
    CONFIG_VAR( int,          UsersExpireDBDays,                "users_expire_days") \
    CONFIG_VAR( int,          daySeconds,                       "day_seconds") \
    CONFIG_VAR( int,          pendingChanPeriod,                "pending_duration") \
    CONFIG_VAR( int,          pendingNotifPeriod,               "pending_notif_duration") \
    CONFIG_VAR( unsigned int, input_flood,                      "input_flood") \
    CONFIG_VAR( unsigned int, output_flood,                     "output_flood") \
    CONFIG_VAR( unsigned int, channelsFloodPeriod,              "channels_flood_period") \
    CONFIG_VAR( unsigned int, floodproRelaxTime,                "floodpro_relaxtime") \
    CONFIG_VAR( unsigned int, MAXnotes,                         "max_notes") \
    CONFIG_VAR( unsigned int, RequiredSupporters,               "required_supporters") \
    CONFIG_VAR( unsigned int, JudgeDaySeconds,                  "judge_day_seconds") \
    CONFIG_VAR( unsigned int, NoRegDaysOnNOSupport,             "noreg_days_on_nosupport") \
    CONFIG_VAR( unsigned int, RejectAppOnUserFraud,             "reject_app_on_userfraud") \
    CONFIG_VAR( unsigned int, RequireReview,                    "require_review") \
    CONFIG_VAR( unsigned int, DecideOnObject,                   "decide_on_object") \
    CONFIG_VAR( unsigned int, DecideOnCompleted,                "decide_on_complete") \
    CONFIG_VAR( unsigned int, ReviewsExpireTime,                "reviews_expire_time") \
    CONFIG_VAR( unsigned int, PendingsExpireTime,               "pendings_expire_time") \
    CONFIG_VAR( unsigned int, MaxDays,                          "max_days") \
    CONFIG_VAR( unsigned int, Joins,                            "joins") \
    CONFIG_VAR( unsigned int, UniqueJoins,                      "unique_joins") \
    CONFIG_VAR( unsigned int, MinSupporters,                    "min_supporters") \
    CONFIG_VAR( unsigned int, MinSupportersJoin,                "min_supporters_joins") \
    CONFIG_VAR( unsigned int, NotifyDays,                       "notify_days") \
    CONFIG_VAR( unsigned int, SupportDays,                      "support_days") \
    CONFIG_VAR( unsigned int, ReviewerId,                       "reviewer_id") \
    CONFIG_VAR( unsigned int, LogToAdminConsole,                "log_to_admin_console") \
    CONFIG_VAR( unsigned int, connectCheckFreq,                 "connection_check_frequency") \
    CONFIG_VAR( unsigned int, limitCheckPeriod,                 "limit_check") \
    CONFIG_VAR( unsigned int, limitJoinMaxLowest,               "limitjoin_max_lowest") \
    CONFIG_VAR( unsigned int, limitJoinMaxHighest,              "limitjoin_max_highest") \
    CONFIG_VAR( unsigned int, limitJoinSecsLowest,              "limitjoin_secs_lowest") \
    CONFIG_VAR( unsigned int, limitJoinSecsHighest,             "limitjoin_secs_highest") \
    CONFIG_VAR( unsigned int, limitJoinPeriodHighest,           "limitjoin_period_highest") \
    CONFIG_VAR( unsigned int, loginDelay,                       "login_delay") \
    CONFIG_VAR( unsigned int, noteDuration,                     "note_duration") \
    CONFIG_VAR( unsigned int, noteLimit,                        "note_limit") \
    CONFIG_VAR( unsigned int, preloadUserDays,                  "preload_user_days") \
    CONFIG_VAR( unsigned int, partIdleChan,                     "part_idle_chan") \
    CONFIG_VAR( unsigned int, connectRetry,                     "connection_retry_total") \
    CONFIG_VAR( unsigned int, saslTimeout,                      "sasl_timeout") \
    CONFIG_VAR( unsigned int, maxFingerprints,                  "max_fingerprints") \
    CONFIG_VAR( unsigned int, logVerbosity,                     "log_verbosity") \
    CONFIG_VAR( unsigned int, chanVerbosity,                    "chan_verbosity") \
    CONFIG_VAR( unsigned int, consoleVerbosity,                 "console_verbosity") \
    CONFIG_VAR( bool,         commandLog,                       "log_commands") \
    CONFIG_VAR( bool,         logSQL,                           "log_sql") \
    CONFIG_VAR( bool,         consoleSQL,                       "console_sql") \
    CONFIG_VAR( bool,         pushoverEnable,                   "pushover_enable") \
    CONFIG_VAR( bool,         prometheusEnable,                 "prometheus_enable")
