/**
 * dronescan.h
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
 */

#ifndef DRONESCAN_H
#define DRONESCAN_H "$Id: dronescan.h,v 1.38 2009/05/28 10:37:31 hidden1 Exp $"

#include <list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "client.h"

#include "clientData.h"
#include "jfChannel.h"
#include "dbHandle.h"
#include "sqlSpamEvent.h"
#include "sqlSpamRule.h"
#include "sqlSpamAction.h"
#include "sqlSpamRuleAction.h"
#include "sqlSpamExclusion.h"
#include "sqlSpyClient.h"
#include "sqlMonitoredChannel.h"
#include "glineData.h"

// PCRE2 for regex-based spam event matching
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#ifdef ENABLE_LOG4CPLUS
#include <log4cplus/loglevel.h>
#include <log4cplus/configurator.h>
#include <iomanip>

#define JF_GLINED "gnuworld.ds.jf.glined"
#define JF_CSERVICE "gnuworld.ds.jf.cservice"

#endif
#define RecentlyGlinedIpsSize 150

namespace gnuworld {

class EConfig;
class Timer;

namespace ds {

class activeChannel;
class Command;
class sqlFakeClient;
class sqlUser;
class sqlSpamEvent;
class sqlSpamRule;
class sqlSpamAction;
class sqlSpamRuleAction;
class sqlSpamExclusion;
class sqlSpyClient;
class sqlMonitoredChannel;
class Test;

enum DS_STATE { BURST, RUN };

enum LOG_TYPE { DBG, INFO, WARN, ERR };

struct JCFLOODCLIENTS {
    std::list<std::string> log;
    std::list<std::string> chans;
    std::list<std::string> nicks;
    int count;
    int ctime;
};

class dronescan : public xClient {
  public:
    /** Constructor receives a configuration file name. */
    dronescan(const std::string&);

    /** Destructor to clean up after ourselves. */
    inline virtual ~dronescan();

    /***********************************************************
     ** O V E R R I D E N   X C L I E N T   F U N C T I O N S **
     ***********************************************************/

    /** This method is called when we have attached to the xServer. */
    virtual void OnAttach();

    /** This method is called after server connection. */
    virtual void BurstChannels();

    /** This is called when we receive a CTCP */
    virtual void OnCTCP(iClient*, const std::string&, const std::string&, bool);

    /** This is called when a fake client receives a CTCP */
    virtual void OnFakeCTCP(iClient* Sender, iClient* Target, const std::string& CTCP,
                            const std::string& Message, bool Secure);

    /** This is called when we receive a channel CTCP */
    virtual void OnChannelCTCP(iClient* Sender, Channel* theChan,
                               const std::string& CTCPCommand, const std::string& Message);

    /** This is called when a fake client receives a channel CTCP */
    virtual void OnFakeChannelCTCP(iClient* Sender, iClient* Target, Channel* theChan,
                                   const std::string& CTCPCommand, const std::string& Message);

    /** Receive network events. */
    virtual void OnEvent(const eventType&, void*, void*, void*, void*);

    /** Receive channel events. */
    virtual void OnChannelEvent(const channelEventType&, Channel*, void*, void*, void*, void*);

    /** Receive channel messages (PRIVMSG to a channel, main service client only). */
    virtual void OnChannelMessage(iClient* Sender, Channel* theChan,
                                  const std::string& Message);

    /** Receive fake channel messages (PRIVMSG to a channel caught by a fake client). */
    virtual void OnFakeChannelMessage(iClient* Sender, iClient* Target, Channel* theChan,
                                  const std::string& Message);

    /** Receive channel notices (NOTICE to a channel). */
    virtual void OnChannelNotice(iClient* Sender, Channel* theChan,
                                 const std::string& Message);

    /** Receive fake channel notices (NOTICE to a channel caught by a fake client). */
    virtual void OnFakeChannelNotice(iClient* Sender, iClient* Target, Channel* theChan,
                                     const std::string& Message);

    /** Receive private messages. */
    virtual void OnPrivateMessage(iClient*, const std::string&, bool);

    /** Receive fake private messages (PRIVMSG caught by a fake client). */
    virtual void OnFakePrivateMessage(iClient* Sender, iClient* Target,
                                      const std::string& Message, bool secure);

    /** Receive private notices (NOTICE directly to the bot). */
    virtual void OnPrivateNotice(iClient*, const std::string&, bool);

    /** Receive fake private notices (NOTICE caught by a fake client). */
    virtual void OnFakePrivateNotice(iClient* Sender, iClient* Target,
                                     const std::string& Message, bool secure);

    /** Handle kick events for spy client re-join logic. */
    virtual void OnNetworkKick(Channel* theChan, iClient* srcClient,
                               iClient* destClient, const std::string& kickMessage,
                               bool authoritative);

    /** When we are being detached by the xServer */
    virtual void OnDetach(const std::string& = std::string("Server Shutdown"));

    /** Receive our own timed events. */
    virtual void OnTimer(const xServer::timerID&, void*);

    /*****************************************
     ** D R O N E S C A N   T Y P E D E F S **
     *****************************************/

    typedef unsigned short int testEnabledType;

    typedef std::map<std::string, sqlUser*, noCaseCompare> userMapType;
    typedef std::map<unsigned int, sqlFakeClient*> fcMapType;
    typedef std::map<std::string, unsigned int> clientsIPMapType;
    typedef struct JCFLOODCLIENTS jcFloodClients;
    typedef std::map<std::string, jcFloodClients*> clientsIPFloodMapType;
    typedef std::list<std::string> IPJQueueType;
    typedef std::list<glineData*> glineQueueType;
    typedef std::list<std::pair<std::string, int>> recentlyGlinedIpsType;

    /*******************************************
     ** D R O N E S C A N   F U N C T I O N S **
     *******************************************/

    /** Process the join / part floods */
    void processJoinPartChannels();

    /** Process the gline queue */
    void processGlineQueue();

    /** Report a SQL error to the appropriate places. */
    void doSqlError(const std::string&, const std::string&);

    /** Change the current state. */
    void changeState(DS_STATE);

    /** Update the state according to the current network state */
    void updateState();

    /** handles a channel join */
    void handleChannelJoin(Channel*, iClient*);

    /** handles a channel part */
    void handleChannelPart(Channel*, iClient*);

    /** get rounded unix time */
    time_t getRoundedUnixTime(time_t, unsigned int);

    /** addJoin() method */
    void addJoin(jfChannel*, iClient*, unsigned int);

    /** This function handles new clients as they connect. */
    void handleNewClient(iClient*);

    /** This function handles nick changes. */
    void handleNickChange(iClient*);

    /** Calculate global entropy and store it. */
    void calculateEntropy();

    /** Set states on global nicks. */
    void setNickStates();

    /** Convenience function to reset states and check channels. */
    inline void resetAndCheck() {
        setConsoleTopic();
        setNickStates();
        checkChannels();
    }

    /**
     * Check global channels for drones. This is a convenience wrapper
     * for isAbnormal( Channel* ).
     */
    void checkChannels();
    bool checkChannel(const Channel*, const iClient* = 0);

    /** Calculate the entropy of a given string. */
    double calculateEntropy(const std::string&);

    /** Return the entropy of a given client. */
    double calculateEntropy(const iClient*);

    /** Check if a channel is normal. */
    /** Check if an iClient's nick is normal. */
    bool isNormal(const iClient*);

    /** Set a clientData's state depending on the iClient. */
    CLIENT_STATE setClientState(iClient*);

    /** Log a message. */
    void log(LOG_TYPE, const char*, ...);

#ifdef ENABLE_LOG4CPLUS
    void log(const char*, const char*, ...);
#endif

    /** Set the topic of the console channel. */
    void setConsoleTopic();

    /** Reply to a given iClient. */
    void Reply(const iClient*, const char*, ...);

    /** Return a users access */
    sqlUser* getSqlUser(const std::string&);

    /* Preloaders */
    bool updateDue(std::string);
    void preloadFakeClientCache();
    void preloadUserCache();
    bool preloadExceptionalChannels();

    /* Spam detection cache loaders */
    void preloadSpamEvents();
    void preloadSpamRules();
    void preloadSpamActions();
    void preloadSpamRuleEvents();
    void preloadSpamRuleActions();
    void preloadSpamExclusions();
    void preloadSpyClients();
    void preloadMonitoredChannels();
    void preloadSpamRuleChannels();
    void refreshSpamCaches();

    /* Spam detection processing helpers */
    void processSpamText(iClient* theClient, const std::string& text,
                         int target_bit, const std::string& channel_name);
    void evaluateSpamRules(iClient* theClient, const std::string& channel_name);
    void fireRuleActions(sqlSpamRule* rule, iClient* theClient,
                         const std::string& channel_name);
    // Scoring key: rule_id.channel_or_privmsg.unit, or rule_id.unit when
    // rule->isScoreGlobally() is true (channel segment omitted). unit is the
    // client's numeric nick, or its IP when rule->getPointsPer() == "IP".
    std::string buildScoringKey(sqlSpamRule* rule, iClient* theClient,
                                const std::string& channel_name) const;

    /* Spy client live management */

    /** Introduce all enabled spy clients to the network. */
    void introduceAllSpyClients();

    /**
     * Introduce a single spy client to the network.
     * Returns the iClient* on success, nullptr on failure.
     * If the desired nick is taken, tries other spy clients first,
     * then falls back to nick + random suffix.
     */
    iClient* introduceSpyClient(sqlSpyClient* sc);

    /** Detach a live spy client from the network and clean up maps. */
    void detachSpyClient(int scId);

    /**
     * Find the best available spy client for the given channel.
     * If forcejoin is false, skips channels that are +i, +k, +l(full),
     * or where the spy client host matches a ban.
     * Returns spy client id or -1 if none available.
     */
    int findBestSpyClient(const std::string& chanName, bool forcejoin);

    /** Make a live spy client join the given channel immediately. */
    void doSpyClientJoin(int scId, const std::string& chanName);

    /**
     * Schedule a spy client join with a random delay in [minDelay, maxDelay] seconds.
     * minDelay=0, maxDelay=300 for new channels; 300/1500 after a kick.
     */
    void scheduleSpyClientJoin(int scId, const std::string& chanName,
                               int minDelay, int maxDelay);

    /** Voice a spy client in the console channel. */
    void voiceSpyClientInConsole(iClient* ic);

    /** Op a client in the console channel. */
    void opInConsole(iClient* ic);

    /** Returns true if the given iClient is one of our live spy clients. */
    bool isSpyClient(const iClient* ic) const;

    /** Returns the spy client id for the given iClient, or -1. */
    int getSpyClientId(const iClient* ic) const;

    /** Part a spy client from a monitored channel and update tracking maps. */
    void partSpyClientFromChannel(int scId, const std::string& chanName);

    /**
     * Resync live spy clients against the DB caches:
     * - detach clients that were removed from the DB
     * - introduce newly added / re-enabled clients
     * - if a removed/disabled client was monitoring a channel,
     *   schedule another client to take over
     */
    void resyncSpyClients();

    /* Allow commands access to the database pointer */
    inline dbHandle* getSqlDb() { return SQLDb; }

    inline DS_STATE getCurrentState() { return currentState; }

    inline int GetGlineQueueSize() { return glineQueue.size(); }

    inline bool isHelloEnabled() { return enableHello; }

    /** Internal variables */
    userMapType userMap;
    fcMapType fakeClients;

    /* Spam detection caches */
    typedef std::map<int, sqlSpamEvent*>                               spamEventsMapType;
    typedef std::map<int, sqlSpamRule*>                                spamRulesMapType;
    typedef std::map<int, sqlSpamAction*>                              spamActionsMapType;
    // rule_id -> list of (event_id, points_override); -1 points_override means use event default
    typedef std::map<int, std::vector<std::pair<int,int>>>             spamRuleEventsMapType;
    // reverse of spamRuleEventsMap: event_id -> list of rule ids that include it
    typedef std::map<int, std::vector<int>>                            spamEventRulesMapType;
    // rule_id -> list of bound rule-actions
    typedef std::map<int, std::vector<sqlSpamRuleAction*>>             spamRuleActionsMapType;
    typedef std::list<sqlSpamExclusion*>                               spamExclusionsListType;
    // spyclients: id -> sqlSpyClient*
    typedef std::map<int, sqlSpyClient*>                               spyClientsMapType;
    // monitored_channels: lowercase channel name -> sqlMonitoredChannel*
    typedef std::map<std::string, sqlMonitoredChannel*>                monitoredChannelsMapType;
    // spam_rule_channels: rule_id -> list of channel names
    typedef std::map<int, std::vector<std::string>>                    spamRuleChannelsMapType;

    spamEventsMapType       spamEventsMap;
    spamRulesMapType        spamRulesMap;
    spamActionsMapType      spamActionsMap;
    spamRuleEventsMapType   spamRuleEventsMap;
    spamEventRulesMapType   spamEventRulesMap;
    spamRuleActionsMapType  spamRuleActionsMap;
    spamExclusionsListType  spamExclusionsList;
    spyClientsMapType       spyClientsMap;
    monitoredChannelsMapType monitoredChannelsMap;
    spamRuleChannelsMapType spamRuleChannelsMap;

    /* Live spy client state */

    // spy client id -> live iClient* on the network
    typedef std::map<int, iClient*>                                    liveSpyClientsMapType;
    liveSpyClientsMapType liveSpyClientsMap;

    // lowercase channel name -> spy client id currently assigned to it
    typedef std::map<std::string, int>                                 chanActiveSpyMapType;
    chanActiveSpyMapType chanActiveSpyMap;

    // reverse map: spy client id -> lowercase channel name it is in (empty if not in a channel)
    typedef std::map<int, std::string>                                 spyClientChanMapType;
    spyClientChanMapType spyClientChanMap;

    // Per-channel kick tracking: how many spy client kicks happened within 24h
    struct ChanKickTrack {
        int    count;
        time_t firstKickTime;
        ChanKickTrack() : count(0), firstKickTime(0) {}
    };
    typedef std::map<std::string, ChanKickTrack>                       chanKickTrackMapType;
    chanKickTrackMapType chanKickTrackMap;

    // Channels where monitoring has been stopped due to repeated kicks
    typedef std::set<std::string>                                      kickStoppedChannelsType;
    kickStoppedChannelsType kickStoppedChannels;

    // Pending join timers: timerID -> {spy client id, channel name}
    typedef std::map<xServer::timerID, std::pair<int, std::string>>   pendingJoinTimersType;
    pendingJoinTimersType pendingJoinTimers;

    /* PCRE2 regex cache: event_id -> compiled regex (PRIVMSG_REGEX events only) */
    typedef std::map<int, pcre2_code*>                                 spamRegexCacheType;
    spamRegexCacheType spamRegexCache;

    /* In-memory spam scoring: scoringKey -> (event_id -> SpamScore) */
    struct SpamScore {
        int    count;        // occurrences within the current window
        time_t window_start; // when the current window began
        SpamScore() : count(0), window_start(0) {}
    };
    typedef std::map<std::string, std::map<int, SpamScore>>            spamScoreMapType;
    spamScoreMapType spamScoreMap;
    clientsIPMapType clientsIPMap;
    clientsIPFloodMapType clientsIPFloodMap;
    int lastBurstTime;
    int lastSplitTime;

    /** Typedef of currently seen drone channels */
    typedef std::map<std::string, activeChannel*> droneChannelsType;
    droneChannelsType droneChannels;

    /** Typedef of channels which are not checked for join/part floods */
    typedef std::list<std::string> exceptionalChannelsType;
    exceptionalChannelsType exceptionalChannels;

    /** The gline queue */
    glineQueueType glineQueue;
    IPJQueueType IPJQueue;
    recentlyGlinedIpsType recentlyGlinedIps;

    bool isExceptionalChannel(const std::string&);

    bool addExceptionalChannel(const std::string&);

    bool remExceptionalChannel(const std::string&);

    virtual char* Ago(long);
    virtual char* Duration(long);

    /** Join counter config options */
    unsigned int jcInterval;
    unsigned int jcCutoff;
    unsigned int jcGracePeriodBurstOrSplit;
    unsigned int jcMinJoinToGline;
    unsigned int jcMinJoinToGlineJOnly;
    unsigned int jcMinJoinsPerIPToGline;
    unsigned int jcJoinsPerIPTime;
    unsigned int jcMinJFSizeToGline;
    unsigned int jcMinJFJOnlySizeToGline;
    unsigned int jcIgnoreJoinFloodLag;
    unsigned int jcIgnoreJoinFloodLagTS;
    bool jcGlineEnable;
    bool jcGlineEnableConf;
    std::string jcGlineReason;
    unsigned int jcGlineLength;
    unsigned int pcCutoff;
    unsigned int ncInterval;
    unsigned int ncCutoff;
    typedef std::map<std::string, jfChannel*, noCaseCompare> jcChanMapType;
    typedef jcChanMapType::const_iterator jcChanMapIterator;
    jcChanMapType jcChanMap;
    typedef std::map<std::string, unsigned int, noCaseCompare> ncChanMapType;
    ncChanMapType ncChanMap;

    /** Gline queue config options */
    unsigned int gbCount;
    unsigned int gbInterval;

    /* Test control */
    /** Test map type. */
    typedef std::map<std::string, Test*> testMapType;
    typedef testMapType::value_type testPairType;
    bool RegisterTest(Test*);
    bool UnRegisterTest(const std::string&);

    /** Tests map */
    testMapType testMap;

    /** Set a variable in one of the tests. */
    Test* setTestVariable(const std::string&, const std::string&);

    /* Configuration variables used by tests. */

    /** Global options */
    unsigned int voteCutoff;

    /** Abnormals options */
    double channelMargin;

  protected:
    void outputNames(const std::string&, std::stringstream&, bool, bool);
    /** Configuration file. */
    EConfig* dronescanConfig;

    /** Our database instance */
    dbHandle* SQLDb;

    /** Configuration variables. */
    std::string consoleChannel;
    std::string consoleChannelModes;

    /** State variable. */
    DS_STATE currentState;

    /** Character occurance frequencies. */
    typedef std::map<char, double> charMapType;
    charMapType charMap;

    /** Average nickname entropy. */
    double averageEntropy;
    unsigned int totalNicks;

    /** Margins. */
    double nickMargin;
    unsigned int channelCutoff;

    /** How often to show drone channels on join test */
    unsigned int dcInterval;
    xServer::timerID tidClearActiveList;

    /** What level to log to the console. */
    int consoleLevel;

    /** Stats. */
    int customDataCounter;

    /** Internally used timer. */
    Timer* theTimer;

    /** Timers for GNUWorld triggered events. */
    xServer::timerID tidClearJoinCounter;
    xServer::timerID tidClearNickCounter;
    xServer::timerID tidRefreshCaches;
    xServer::timerID tidGlineQueue;

    /** Command map type. */
    typedef std::map<std::string, Command*, noCaseCompare> commandMapType;
    typedef commandMapType::value_type commandPairType;
    commandMapType commandMap;

    friend class HELPCommand;

    /** Command configuration options */
    bool enableHello;
    bool RegisterCommand(Command*);

    /** Time of the last cache. */
    std::map<std::string, time_t> lastUpdated;

    /** How often to refresh caches. */
    unsigned int rcInterval;

    /** Fake sqlUser record for opered clients without accounts. */
    sqlUser* fakeOperUser;

}; // class dronescan

} // namespace ds

} // namespace gnuworld

#endif
