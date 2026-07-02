/**
 * SPAMCommand.cc
 *
 * Top-level SPAM command dispatcher for dronescan.
 * Syntax:
 *   SPAM EVENT     ADD    <name> <type> <target> <param> <points> <expiry> [max_occ]
 *   SPAM EVENT     DEL    <id>
 *   SPAM EVENT     LIST
 *   SPAM EVENT     SHOW   <id>
 *   SPAM EVENT     SET    <id> <field> <value>
 *   SPAM RULE      ADD    <name> <threshold>
 *   SPAM RULE      DEL    <id>
 *   SPAM RULE      LIST
 *   SPAM RULE      SHOW   <id>
 *   SPAM RULE      SET    <id> <field> <value>
 *   SPAM RULE      ADDEVENT   <rule_id> <event_id> [points_override]
 *   SPAM RULE      REMEVENT   <rule_id> <event_id>
 *   SPAM RULE      ADDACTION  <rule_id> <action_id> [dur_override] [reason_override] [delay_override]
 *   SPAM RULE      REMACTION  <spam_rule_action_id>
 *   SPAM RULE      ADDCHAN    <rule_id> <#channel>
 *   SPAM RULE      REMCHAN    <rule_id> <#channel>
 *   SPAM ACTION    ADD    <name> <type> [duration] [reason] [delay]
 *   SPAM ACTION    DEL    <id>
 *   SPAM ACTION    LIST
 *   SPAM EXCLUSION ADD    <CHAN|NICK|IP|OPER> <value>
 *   SPAM EXCLUSION DEL    <id>
 *   SPAM EXCLUSION LIST
 *   SPAM SPYCLIENT ADD    <nick> <user> <host> <ip> <realname> [account] [modes]
 *   SPAM SPYCLIENT DEL    <id>
 *   SPAM SPYCLIENT LIST
 *   SPAM SPYCLIENT SHOW   <id>
 *   SPAM SPYCLIENT ENABLE <id>
 *   SPAM SPYCLIENT DISABLE <id>
 *   SPAM MONITORCHAN ADD    <#channel> [forcejoin 0|1] [joinasservice 0|1]
 *   SPAM MONITORCHAN DEL    <id>
 *   SPAM MONITORCHAN LIST
 *   SPAM MONITORCHAN SHOW   <id>
 *   SPAM MONITORCHAN ENABLE <id>
 *   SPAM MONITORCHAN DISABLE <id>
 *
 * target bitmask: chan_priv=1, privmsg=2, chan_not=4, part=8, quit=16, notice=32, all=63
 *                  "chan" is an alias for chan_priv|chan_not (=5)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <sstream>
#include <string>

#include "StringTokenizer.h"
#include "levels.h"
#include "dronescan.h"
#include "dronescanCommands.h"
#include "sqlUser.h"
#include "sqlSpamEvent.h"
#include "sqlSpamRule.h"
#include "sqlSpamAction.h"
#include "sqlSpamRuleAction.h"
#include "sqlSpamExclusion.h"
#include "sqlSpyClient.h"
#include "sqlMonitoredChannel.h"
#include "constants.h"

namespace gnuworld {
namespace ds {

using std::string;
using std::stringstream;

// ---------------------------------------------------------------------------
// Helper: check minimum access
// ---------------------------------------------------------------------------
static bool checkAccess(const iClient* theClient, const sqlUser* theUser,
                        dronescan* bot, unsigned short required)
{
    if (theUser->getAccess() < required) {
        bot->Reply(theClient, "Access denied.");
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Helpers: valid event types / action types
// ---------------------------------------------------------------------------
static bool isValidEventType(const string& t)
{
    static const char* valid[] = {
        "TEXT", "TEXT_REPEAT",
        "ENTROPY_TEXT", "ENTROPY_NICK", "JOIN_CHANNEL",
        "USERMODE", "KICK_MSG", "KICK_COUNT", nullptr
    };
    for (int i = 0; valid[i]; ++i)
        if (t == valid[i]) return true;
    return false;
}

static bool isValidActionType(const string& t)
{
    return (t == "GLINE" || t == "KILL" || t == "REPORT");
}

static bool isValidExclusionType(const string& t)
{
    return (t == "CHAN" || t == "NICK" || t == "IP" || t == "OPER");
}

// Parse comma-separated target names to an integer bitmask.
// Returns -1 on any invalid token.
// e.g. "chan_priv,privmsg" -> 3,  "all" -> 63,  "chan" -> 5 (chan_priv|chan_not)
static int parseTargetBitmask(const string& s)
{
    int mask = 0;
    string tok;
    std::istringstream ss(s);
    while (std::getline(ss, tok, ',')) {
        if (tok.empty()) continue;
        const string p = string_lower(tok);
        if      (p == "chan_priv") mask |= spam_target::CHAN_PRIV;
        else if (p == "chan_not")  mask |= spam_target::CHAN_NOT;
        else if (p == "chan")      mask |= spam_target::CHAN_PRIV | spam_target::CHAN_NOT;
        else if (p == "privmsg")   mask |= spam_target::PRIVMSG;
        else if (p == "notice")    mask |= spam_target::NOTICE;
        else if (p == "part")      mask |= spam_target::PART;
        else if (p == "quit")      mask |= spam_target::QUIT;
        else if (p == "all")       mask |= spam_target::ALL;
        else return -1;
    }
    return (mask > 0) ? mask : -1;
}

// Decode an integer bitmask to a human-readable comma-separated string.
// e.g. 63 -> "all",  5 -> "chan_priv,chan_not"
static string targetBitmaskToString(int mask)
{
    if ((mask & spam_target::ALL) == spam_target::ALL)
        return "all";
    string s;
    if (mask & spam_target::CHAN_PRIV) { if (!s.empty()) s += ","; s += "chan_priv"; }
    if (mask & spam_target::CHAN_NOT)  { if (!s.empty()) s += ","; s += "chan_not";  }
    if (mask & spam_target::PRIVMSG)   { if (!s.empty()) s += ","; s += "privmsg";  }
    if (mask & spam_target::NOTICE)    { if (!s.empty()) s += ","; s += "notice";   }
    if (mask & spam_target::PART)      { if (!s.empty()) s += ","; s += "part";     }
    if (mask & spam_target::QUIT)      { if (!s.empty()) s += ","; s += "quit";     }
    return s.empty() ? "(none)" : s;
}

// ---------------------------------------------------------------------------
// EVENT subcommands
// ---------------------------------------------------------------------------
static void handleEvent(dronescan* bot, const iClient* theClient,
                        const sqlUser* theUser, const StringTokenizer& st)
{
    if (st.size() < 3) {
        bot->Reply(theClient,
            "Usage: SPAM EVENT <ADD|DEL|LIST|SHOW> ...");
        return;
    }

    const string verb = string_upper(st[2]);

    // -- LIST ----------------------------------------------------------------
    if (verb == "LIST") {
        if (!checkAccess(theClient, theUser, bot, level::spam_read)) return;

        if (bot->spamEventsMap.empty()) {
            bot->Reply(theClient, "No spam events defined.");
            return;
        }
        bot->Reply(theClient, "=== Spam Events (%zu) ===",
                   bot->spamEventsMap.size());
        for (dronescan::spamEventsMapType::const_iterator it = bot->spamEventsMap.begin();
             it != bot->spamEventsMap.end(); ++it) {
            sqlSpamEvent* ev = it->second;
            bot->Reply(theClient, "[%d] %-22s  Type: %-14s  Target: %-14s  Enabled: %s",
                       ev->getId(),
                       ev->getName().c_str(),
                       ev->getEventType().c_str(),
                       targetBitmaskToString(ev->getTarget()).c_str(),
                       ev->isEnabled() ? "yes" : "no");
            bot->Reply(theClient,
                       "     Pts: %-5d  Expiry: %-5ds  MaxOcc: %-10s  ReqEvent: %s",
                       ev->getPoints(),
                       ev->getPointExpiry(),
                       ev->getMaxOccurrence() >= 0
                           ? std::to_string(ev->getMaxOccurrence()).c_str() : "unlimited",
                       ev->getRequiresEventId() > 0
                           ? std::to_string(ev->getRequiresEventId()).c_str() : "none");
            bot->Reply(theClient,
                       "     Param: %-32s  CaseSens: %s",
                       ev->getEventParam().empty() ? "(none)" : ev->getEventParam().c_str(),
                       ev->isCaseSensitive() ? "yes" : "no");
            if (ev->getEventType() == "TEXT_REPEAT") {
                bot->Reply(theClient,
                    "     CrossUser: %-3s  MinCount: %-3d  ExclRegex: %s",
                    ev->isRepeatCrossUser() ? "yes" : "no",
                    ev->getRepeatMinCount(),
                    ev->getRepeatExclusionRegex().empty()
                        ? "(none)" : ev->getRepeatExclusionRegex().c_str());
            }
        }
        bot->Reply(theClient, "--- %zu event(s) ---", bot->spamEventsMap.size());
        return;
    }

    // -- SHOW ----------------------------------------------------------------
    if (verb == "SHOW") {
        if (!checkAccess(theClient, theUser, bot, level::spam_read)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM EVENT SHOW <id>"); return; }

        int id = atoi(st[3].c_str());
        dronescan::spamEventsMapType::const_iterator it = bot->spamEventsMap.find(id);
        if (it == bot->spamEventsMap.end()) {
            bot->Reply(theClient, "Event %d not found.", id);
            return;
        }
        sqlSpamEvent* ev = it->second;
        bot->Reply(theClient, "ID      : %d", ev->getId());
        bot->Reply(theClient, "Name    : %s", ev->getName().c_str());
        bot->Reply(theClient, "Type    : %s", ev->getEventType().c_str());
        bot->Reply(theClient, "Param   : %s", ev->getEventParam().c_str());
        bot->Reply(theClient, "Target  : %s (mask=%d)",
                   targetBitmaskToString(ev->getTarget()).c_str(), ev->getTarget());
        bot->Reply(theClient, "CaseSens: %s", ev->isCaseSensitive() ? "yes" : "no");
        bot->Reply(theClient, "Points  : %d  Expiry: %ds",
                   ev->getPoints(), ev->getPointExpiry());
        bot->Reply(theClient, "MaxOcc  : %s",
                   ev->getMaxOccurrence() >= 0
                       ? std::to_string(ev->getMaxOccurrence()).c_str() : "unlimited");
        bot->Reply(theClient, "Requires: event %d%s",
                   ev->getRequiresEventId(),
                   ev->getRequiresEventId() > 0 ? "" : " (none)");
        bot->Reply(theClient, "Enabled : %s", ev->isEnabled() ? "yes" : "no");
        if (ev->getEventType() == "TEXT_REPEAT") {
            bot->Reply(theClient, "Repeat cross-user   : %s",
                       ev->isRepeatCrossUser() ? "yes" : "no");
            bot->Reply(theClient, "Repeat min count    : %d", ev->getRepeatMinCount());
            bot->Reply(theClient, "Repeat excl regex   : %s",
                       ev->getRepeatExclusionRegex().empty()
                           ? "(none)" : ev->getRepeatExclusionRegex().c_str());
        }
        return;
    }

    // -- ADD -----------------------------------------------------------------
    if (verb == "ADD") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM EVENT ADD <name> <type> <target> <param> <points> <expiry> [max_occ]
        if (st.size() < 9) {
            bot->Reply(theClient,
                "Usage: SPAM EVENT ADD <name> <type> <target> <param> <points> <expiry> [max_occ]");
            return;
        }

        const string evType = string_upper(st[4]);
        if (!isValidEventType(evType)) {
            bot->Reply(theClient, "Invalid event type: %s", evType.c_str());
            return;
        }
        int targetMask = parseTargetBitmask(st[5]);
        if (targetMask < 0) {
            bot->Reply(theClient,
                "Invalid target '%s'. Use comma-separated: chan,privmsg,notice,part,quit,all",
                st[5].c_str());
            return;
        }

        sqlSpamEvent* ev = new sqlSpamEvent(bot->getSqlDb());
        ev->setName(st[3]);
        ev->setEventType(evType);
        ev->setTarget(targetMask);
        ev->setEventParam(st[6]);
        ev->setPoints(atoi(st[7].c_str()));
        ev->setPointExpiry(atoi(st[8].c_str()));
        if (st.size() >= 10)
            ev->setMaxOccurrence(atoi(st[9].c_str()));
        ev->setCreatedTs(::time(0));
        ev->setModifiedTs(::time(0));
        ev->setModifiedBy(0);

        if (!ev->insert()) {
            bot->Reply(theClient, "Failed to add spam event.");
            delete ev;
            return;
        }
        bot->spamEventsMap[ev->getId()] = ev;
        bot->Reply(theClient, "Spam event '%s' added with ID %d (target: %s).",
                   ev->getName().c_str(), ev->getId(),
                   targetBitmaskToString(ev->getTarget()).c_str());
        return;
    }

    // -- DEL -----------------------------------------------------------------
    if (verb == "DEL") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM EVENT DEL <id>"); return; }

        int id = atoi(st[3].c_str());
        dronescan::spamEventsMapType::iterator it = bot->spamEventsMap.find(id);
        if (it == bot->spamEventsMap.end()) {
            bot->Reply(theClient, "Event %d not found.", id);
            return;
        }
        if (!it->second->remove()) {
            bot->Reply(theClient, "Failed to delete spam event %d.", id);
            return;
        }
        delete it->second;
        bot->spamEventsMap.erase(it);
        bot->Reply(theClient, "Spam event %d deleted.", id);
        return;
    }

    // -- SET -----------------------------------------------------------------
    if (verb == "SET") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM EVENT SET <id> <field> <value>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM EVENT SET <id> <field> <value>");
            return;
        }
        int id = atoi(st[3].c_str());
        dronescan::spamEventsMapType::iterator it = bot->spamEventsMap.find(id);
        if (it == bot->spamEventsMap.end()) {
            bot->Reply(theClient, "Event %d not found.", id);
            return;
        }
        sqlSpamEvent* ev = it->second;
        const string field = string_lower(st[4]);
        const string value = (st.size() >= 6) ? st[5] : string();

        if (field == "description") {
            ev->setDescription(value);
        } else if (field == "event_param") {
            ev->setEventParam(value);
            // If TEXT event, recompile the regex cache
            if (ev->getEventType() == "TEXT") {
                dronescan::spamRegexCacheType::iterator ci =
                    bot->spamRegexCache.find(id);
                if (ci != bot->spamRegexCache.end()) {
                    pcre2_code_free(ci->second);
                    bot->spamRegexCache.erase(ci);
                }
                if (!value.empty()) {
                    int errcode;
                    PCRE2_SIZE erroffset;
                    uint32_t opts = PCRE2_UTF;
                    if (!ev->isCaseSensitive()) opts |= PCRE2_CASELESS;
                    pcre2_code* re = pcre2_compile(
                        (PCRE2_SPTR)value.c_str(), PCRE2_ZERO_TERMINATED,
                        opts, &errcode, &erroffset, nullptr);
                    if (re)
                        bot->spamRegexCache[id] = re;
                    else
                        bot->Reply(theClient,
                            "Warning: regex compile failed at offset %zu.", erroffset);
                }
            }
        } else if (field == "target") {
            int mask = parseTargetBitmask(value);
            if (mask < 0) {
                bot->Reply(theClient,
                    "Invalid target '%s'. Use comma-separated: chan,privmsg,notice,part,quit,all",
                    value.c_str());
                return;
            }
            ev->setTarget(mask);
        } else if (field == "case_sensitive") {
            ev->setCaseSensitive(value == "1" || value == "yes" || value == "true");
        } else if (field == "points") {
            ev->setPoints(atoi(value.c_str()));
        } else if (field == "point_expiry") {
            ev->setPointExpiry(atoi(value.c_str()));
        } else if (field == "max_occurrence") {
            ev->setMaxOccurrence(atoi(value.c_str()));
        } else if (field == "requires_event_id") {
            ev->setRequiresEventId(atoi(value.c_str()));
        } else if (field == "enabled") {
            ev->setEnabled(value == "1" || value == "yes" || value == "true");
        } else if (field == "repeat_crossuser") {
            ev->setRepeatCrossUser(value == "1" || value == "yes" || value == "true");
        } else if (field == "repeat_min_count") {
            ev->setRepeatMinCount(atoi(value.c_str()));
        } else if (field == "repeat_exclusion_regex") {
            ev->setRepeatExclusionRegex(value);
        } else {
            bot->Reply(theClient,
                "Unknown field '%s'. Valid: description, event_param, target, case_sensitive, "
                "points, point_expiry, max_occurrence, requires_event_id, enabled, "
                "repeat_crossuser, repeat_min_count, repeat_exclusion_regex",
                field.c_str());
            return;
        }

        ev->setModifiedTs(::time(0));
        ev->setModifiedBy(0);
        if (!ev->commit()) {
            bot->Reply(theClient, "Failed to update event %d.", id);
            return;
        }
        bot->Reply(theClient, "Event %d: %s set to '%s'.", id, field.c_str(), value.c_str());
        return;
    }

    bot->Reply(theClient, "Unknown verb '%s'. Use ADD, DEL, LIST, SHOW, SET.", st[2].c_str());
}

// ---------------------------------------------------------------------------
// RULE subcommands
// ---------------------------------------------------------------------------
static void handleRule(dronescan* bot, const iClient* theClient,
                       const sqlUser* theUser, const StringTokenizer& st)
{
    if (st.size() < 3) {
        bot->Reply(theClient,
            "Usage: SPAM RULE <ADD|DEL|LIST|SHOW|ADDEVENT|REMEVENT|ADDACTION|REMACTION> ...");
        return;
    }

    const string verb = string_upper(st[2]);

    // -- LIST ----------------------------------------------------------------
    if (verb == "LIST") {
        if (!checkAccess(theClient, theUser, bot, level::spam_read)) return;

        if (bot->spamRulesMap.empty()) {
            bot->Reply(theClient, "No spam rules defined.");
            return;
        }
        bot->Reply(theClient, "=== Spam Rules (%zu) ===",
                   bot->spamRulesMap.size());
        for (dronescan::spamRulesMapType::const_iterator it = bot->spamRulesMap.begin();
             it != bot->spamRulesMap.end(); ++it) {
            sqlSpamRule* rule = it->second;
            bot->Reply(theClient,
                       "[%d] %-25s  Threshold: %-5d  AllChans: %-3s  WaitOnRule: %-4s  PointsPer: %-8s  Global: %-3s  Enabled: %s",
                       rule->getId(),
                       rule->getName().c_str(),
                       rule->getThreshold(),
                       rule->isAllChans() ? "yes" : "no",
                       rule->getWaitOnRuleId() > 0
                           ? std::to_string(rule->getWaitOnRuleId()).c_str() : "none",
                       rule->getPointsPer().c_str(),
                       rule->isScoreGlobally() ? "yes" : "no",
                       rule->isEnabled() ? "yes" : "no");

            // Linked events
            dronescan::spamRuleEventsMapType::const_iterator rei =
                bot->spamRuleEventsMap.find(rule->getId());
            if (rei != bot->spamRuleEventsMap.end() && !rei->second.empty()) {
                string evLine = "     Events:";
                for (size_t i = 0; i < rei->second.size(); ++i) {
                    int eid = rei->second[i].first;
                    int po  = rei->second[i].second;
                    dronescan::spamEventsMapType::const_iterator ei =
                        bot->spamEventsMap.find(eid);
                    const char* ename = (ei != bot->spamEventsMap.end())
                                      ? ei->second->getName().c_str() : "?";
                    evLine += " [" + std::to_string(eid) + "]" + ename;
                    if (po >= 0)
                        evLine += "(pts_ovr:" + std::to_string(po) + ")";
                }
                bot->Reply(theClient, "%s", evLine.c_str());
            } else {
                bot->Reply(theClient, "     Events: (none)");
            }

            // Linked actions
            dronescan::spamRuleActionsMapType::const_iterator rai =
                bot->spamRuleActionsMap.find(rule->getId());
            if (rai != bot->spamRuleActionsMap.end() && !rai->second.empty()) {
                string actLine = "     Actions:";
                for (size_t i = 0; i < rai->second.size(); ++i) {
                    sqlSpamRuleAction* ra = rai->second[i];
                    dronescan::spamActionsMapType::const_iterator ai =
                        bot->spamActionsMap.find(ra->getActionId());
                    const char* aname = (ai != bot->spamActionsMap.end())
                                      ? ai->second->getName().c_str() : "?";
                    actLine += " [sra:" + std::to_string(ra->getId()) + "]" + aname
                             + "(" + ra->getActionType() + " dur:"
                             + (ra->getActionDurationOverride() >= 0
                                 ? std::to_string(ra->getActionDurationOverride()) : "def")
                             + " delay:"
                             + (ra->getDelayOverride() >= 0
                                 ? std::to_string(ra->getDelayOverride()) : "def")
                             + ")";
                }
                bot->Reply(theClient, "%s", actLine.c_str());
            } else {
                bot->Reply(theClient, "     Actions: (none)");
            }

            // Channel inclusions/exclusions
            dronescan::spamRuleChannelsMapType::const_iterator rci =
                bot->spamRuleChannelsMap.find(rule->getId());
            if (rci != bot->spamRuleChannelsMap.end() && !rci->second.empty()) {
                const char* chanLabel = rule->isAllChans() ? "     Excl-Chans:" : "     Incl-Chans:";
                string chanLine = chanLabel;
                for (size_t i = 0; i < rci->second.size(); ++i)
                    chanLine += " " + rci->second[i];
                bot->Reply(theClient, "%s", chanLine.c_str());
            }
        }
        bot->Reply(theClient, "--- %zu rule(s) ---", bot->spamRulesMap.size());
        return;
    }

    // -- SHOW ----------------------------------------------------------------
    if (verb == "SHOW") {
        if (!checkAccess(theClient, theUser, bot, level::spam_read)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM RULE SHOW <id>"); return; }

        int id = atoi(st[3].c_str());
        dronescan::spamRulesMapType::const_iterator it = bot->spamRulesMap.find(id);
        if (it == bot->spamRulesMap.end()) {
            bot->Reply(theClient, "Rule %d not found.", id);
            return;
        }
        sqlSpamRule* rule = it->second;
        bot->Reply(theClient, "ID          : %d", rule->getId());
        bot->Reply(theClient, "Name        : %s", rule->getName().c_str());
        bot->Reply(theClient, "Threshold   : %d", rule->getThreshold());
        bot->Reply(theClient, "WaitOnRule  : %d%s",
                   rule->getWaitOnRuleId(),
                   rule->getWaitOnRuleId() > 0 ? "" : " (none)");
        bot->Reply(theClient, "AllChans    : %s", rule->isAllChans() ? "yes" : "no");
        bot->Reply(theClient, "PointsPer   : %s", rule->getPointsPer().c_str());
        bot->Reply(theClient, "Global      : %s", rule->isScoreGlobally() ? "yes" : "no");
        bot->Reply(theClient, "Enabled     : %s", rule->isEnabled() ? "yes" : "no");

        // Channel inclusion/exclusion list
        dronescan::spamRuleChannelsMapType::const_iterator rci =
            bot->spamRuleChannelsMap.find(id);
        if (rci != bot->spamRuleChannelsMap.end() && !rci->second.empty()) {
            const char* listLabel = rule->isAllChans()
                ? "Chan exclusions:" : "Chan inclusions:";
            bot->Reply(theClient, "%s", listLabel);
            for (size_t i = 0; i < rci->second.size(); ++i)
                bot->Reply(theClient, "  %s", rci->second[i].c_str());
        }

        // Linked events
        dronescan::spamRuleEventsMapType::const_iterator rei =
            bot->spamRuleEventsMap.find(id);
        if (rei != bot->spamRuleEventsMap.end() && !rei->second.empty()) {
            bot->Reply(theClient, "Events:");
            for (size_t i = 0; i < rei->second.size(); ++i) {
                int eid = rei->second[i].first;
                int po  = rei->second[i].second;
                dronescan::spamEventsMapType::const_iterator ei =
                    bot->spamEventsMap.find(eid);
                const char* ename = (ei != bot->spamEventsMap.end())
                                  ? ei->second->getName().c_str() : "?";
                if (po >= 0)
                    bot->Reply(theClient, "  [%d] %s (points override: %d)", eid, ename, po);
                else
                    bot->Reply(theClient, "  [%d] %s", eid, ename);
            }
        } else {
            bot->Reply(theClient, "Events    : (none)");
        }

        // Linked actions
        dronescan::spamRuleActionsMapType::const_iterator rai =
            bot->spamRuleActionsMap.find(id);
        if (rai != bot->spamRuleActionsMap.end() && !rai->second.empty()) {
            bot->Reply(theClient, "Actions:");
            for (size_t i = 0; i < rai->second.size(); ++i) {
                sqlSpamRuleAction* ra = rai->second[i];
                dronescan::spamActionsMapType::const_iterator ai =
                    bot->spamActionsMap.find(ra->getActionId());
                const char* aname = (ai != bot->spamActionsMap.end())
                                  ? ai->second->getName().c_str() : "?";
                bot->Reply(theClient, "  [sra:%d] action:%d (%s) type:%s dur:%s reason:%s delay:%s",
                           ra->getId(), ra->getActionId(), aname,
                           ra->getActionType().c_str(),
                           ra->getActionDurationOverride() >= 0
                               ? std::to_string(ra->getActionDurationOverride()).c_str() : "default",
                           ra->getActionReasonOverride().empty() ? "default"
                               : ra->getActionReasonOverride().c_str(),
                           ra->getDelayOverride() >= 0
                               ? std::to_string(ra->getDelayOverride()).c_str() : "default");
            }
        } else {
            bot->Reply(theClient, "Actions   : (none)");
        }
        return;
    }

    // -- ADD -----------------------------------------------------------------
    if (verb == "ADD") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE ADD <name> <threshold>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM RULE ADD <name> <threshold>");
            return;
        }

        sqlSpamRule* rule = new sqlSpamRule(bot->getSqlDb());
        rule->setName(st[3]);
        rule->setThreshold(atoi(st[4].c_str()));
        rule->setCreatedTs(::time(0));
        rule->setModifiedTs(::time(0));

        if (!rule->insert()) {
            bot->Reply(theClient, "Failed to add spam rule.");
            delete rule;
            return;
        }
        bot->spamRulesMap[rule->getId()] = rule;
        bot->Reply(theClient, "Spam rule '%s' added with ID %d.",
                   rule->getName().c_str(), rule->getId());
        return;
    }

    // -- DEL -----------------------------------------------------------------
    if (verb == "DEL") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM RULE DEL <id>"); return; }

        int id = atoi(st[3].c_str());
        dronescan::spamRulesMapType::iterator it = bot->spamRulesMap.find(id);
        if (it == bot->spamRulesMap.end()) {
            bot->Reply(theClient, "Rule %d not found.", id);
            return;
        }
        if (!it->second->remove()) {
            bot->Reply(theClient, "Failed to delete spam rule %d.", id);
            return;
        }
        delete it->second;
        bot->spamRulesMap.erase(it);
        // Clean up in-memory rule-event and rule-action entries too
        bot->spamRuleEventsMap.erase(id);
        dronescan::spamRuleActionsMapType::iterator rai = bot->spamRuleActionsMap.find(id);
        if (rai != bot->spamRuleActionsMap.end()) {
            for (size_t i = 0; i < rai->second.size(); ++i)
                delete rai->second[i];
            bot->spamRuleActionsMap.erase(rai);
        }
        bot->Reply(theClient, "Spam rule %d deleted.", id);
        return;
    }

    // -- ADDEVENT ------------------------------------------------------------
    if (verb == "ADDEVENT") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE ADDEVENT <rule_id> <event_id> [points_override]
        if (st.size() < 5) {
            bot->Reply(theClient,
                "Usage: SPAM RULE ADDEVENT <rule_id> <event_id> [points_override]");
            return;
        }
        int rule_id  = atoi(st[3].c_str());
        int event_id = atoi(st[4].c_str());

        if (bot->spamRulesMap.find(rule_id) == bot->spamRulesMap.end()) {
            bot->Reply(theClient, "Rule %d not found.", rule_id);
            return;
        }
        if (bot->spamEventsMap.find(event_id) == bot->spamEventsMap.end()) {
            bot->Reply(theClient, "Event %d not found.", event_id);
            return;
        }
        int po = (st.size() >= 6) ? atoi(st[5].c_str()) : -1;

        stringstream q;
        q << "INSERT INTO spam_rule_events (rule_id, event_id, points_override) VALUES ("
          << rule_id << ", " << event_id << ", "
          << (po >= 0 ? std::to_string(po) : "NULL")
          << ")";
        if (!bot->getSqlDb()->Exec(q)) {
            bot->Reply(theClient, "Failed to link event %d to rule %d.", event_id, rule_id);
            return;
        }
        bot->spamRuleEventsMap[rule_id].push_back(std::make_pair(event_id, po));
        bot->Reply(theClient, "Event %d linked to rule %d%s.", event_id, rule_id,
                   po >= 0 ? (" (points override: " + std::to_string(po) + ")").c_str() : "");
        return;
    }

    // -- REMEVENT ------------------------------------------------------------
    if (verb == "REMEVENT") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM RULE REMEVENT <rule_id> <event_id>");
            return;
        }
        int rule_id  = atoi(st[3].c_str());
        int event_id = atoi(st[4].c_str());

        stringstream q;
        q << "DELETE FROM spam_rule_events WHERE rule_id = " << rule_id
          << " AND event_id = " << event_id;
        if (!bot->getSqlDb()->Exec(q)) {
            bot->Reply(theClient, "Failed to unlink event %d from rule %d.", event_id, rule_id);
            return;
        }

        // Remove from in-memory map
        dronescan::spamRuleEventsMapType::iterator rei =
            bot->spamRuleEventsMap.find(rule_id);
        if (rei != bot->spamRuleEventsMap.end()) {
            std::vector<std::pair<int,int>>& vec = rei->second;
            for (std::vector<std::pair<int,int>>::iterator vi = vec.begin();
                 vi != vec.end(); ++vi) {
                if (vi->first == event_id) {
                    vec.erase(vi);
                    break;
                }
            }
        }
        bot->Reply(theClient, "Event %d unlinked from rule %d.", event_id, rule_id);
        return;
    }

    // -- ADDACTION -----------------------------------------------------------
    if (verb == "ADDACTION") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE ADDACTION <rule_id> <action_id> [dur_override] [reason_override] [delay_override]
        if (st.size() < 5) {
            bot->Reply(theClient,
                "Usage: SPAM RULE ADDACTION <rule_id> <action_id> "
                "[dur_override] [reason_override] [delay_override]");
            return;
        }
        int rule_id   = atoi(st[3].c_str());
        int action_id = atoi(st[4].c_str());

        if (bot->spamRulesMap.find(rule_id) == bot->spamRulesMap.end()) {
            bot->Reply(theClient, "Rule %d not found.", rule_id);
            return;
        }
        dronescan::spamActionsMapType::const_iterator ait =
            bot->spamActionsMap.find(action_id);
        if (ait == bot->spamActionsMap.end()) {
            bot->Reply(theClient, "Action %d not found.", action_id);
            return;
        }

        sqlSpamRuleAction* ra = new sqlSpamRuleAction(bot->getSqlDb());
        ra->setRuleId(rule_id);
        ra->setActionId(action_id);
        ra->setActionType(ait->second->getActionType());
        if (st.size() >= 6) ra->setActionDurationOverride(atoi(st[5].c_str()));
        if (st.size() >= 7) ra->setActionReasonOverride(st[6]);
        if (st.size() >= 8) ra->setDelayOverride(atoi(st[7].c_str()));

        if (!ra->insert()) {
            bot->Reply(theClient, "Failed to link action %d to rule %d.", action_id, rule_id);
            delete ra;
            return;
        }
        bot->spamRuleActionsMap[rule_id].push_back(ra);
        bot->Reply(theClient, "Action %d linked to rule %d (sra id: %d).",
                   action_id, rule_id, ra->getId());
        return;
    }

    // -- REMACTION -----------------------------------------------------------
    if (verb == "REMACTION") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE REMACTION <spam_rule_action_id>
        if (st.size() < 4) {
            bot->Reply(theClient, "Usage: SPAM RULE REMACTION <spam_rule_action_id>");
            return;
        }
        int sra_id = atoi(st[3].c_str());

        // Find it across all rule buckets
        bool found = false;
        for (dronescan::spamRuleActionsMapType::iterator rit =
                 bot->spamRuleActionsMap.begin();
             rit != bot->spamRuleActionsMap.end(); ++rit) {
            std::vector<sqlSpamRuleAction*>& vec = rit->second;
            for (std::vector<sqlSpamRuleAction*>::iterator vi = vec.begin();
                 vi != vec.end(); ++vi) {
                if ((*vi)->getId() == sra_id) {
                    if (!(*vi)->remove()) {
                        bot->Reply(theClient, "Failed to remove rule-action %d.", sra_id);
                        return;
                    }
                    delete *vi;
                    vec.erase(vi);
                    found = true;
                    break;
                }
            }
            if (found) break;
        }
        if (!found) {
            bot->Reply(theClient, "Rule-action %d not found.", sra_id);
            return;
        }
        bot->Reply(theClient, "Rule-action %d removed.", sra_id);
        return;
    }

    // -- SET -----------------------------------------------------------------
    if (verb == "SET") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE SET <id> <field> <value>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM RULE SET <id> <field> <value>");
            return;
        }
        int id = atoi(st[3].c_str());
        dronescan::spamRulesMapType::iterator it = bot->spamRulesMap.find(id);
        if (it == bot->spamRulesMap.end()) {
            bot->Reply(theClient, "Rule %d not found.", id);
            return;
        }
        sqlSpamRule* rule = it->second;
        const string field = string_lower(st[4]);
        const string value = (st.size() >= 6) ? st[5] : string();

        if (field == "description") {
            rule->setDescription(value);
        } else if (field == "threshold") {
            rule->setThreshold(atoi(value.c_str()));
        } else if (field == "wait_on_rule_id") {
            rule->setWaitOnRuleId(atoi(value.c_str()));
        } else if (field == "enabled") {
            rule->setEnabled(value == "1" || value == "yes" || value == "true");
        } else if (field == "points_per") {
            rule->setPointsPer(string_upper(value));
        } else if (field == "score_globally") {
            rule->setScoreGlobally(value == "1" || value == "yes" || value == "true");
        } else if (field == "allchans") {
            bool newVal = (value == "1" || value == "yes" || value == "true");
            // Changing allchans resets the channel list
            stringstream dq;
            dq << "DELETE FROM spam_rule_channels WHERE rule_id = " << id;
            bot->getSqlDb()->Exec(dq);
            bot->spamRuleChannelsMap.erase(id);
            rule->setAllChans(newVal);
        } else {
            bot->Reply(theClient,
                "Unknown field '%s'. Valid: description, threshold, wait_on_rule_id, "
                "enabled, points_per, score_globally, allchans",
                field.c_str());
            return;
        }

        rule->setModifiedTs(::time(0));
        rule->setModifiedBy(0);
        if (!rule->commit()) {
            bot->Reply(theClient, "Failed to update rule %d.", id);
            return;
        }
        bot->Reply(theClient, "Rule %d: %s set to '%s'.", id, field.c_str(), value.c_str());
        return;
    }

    // -- ADDCHAN -------------------------------------------------------------
    if (verb == "ADDCHAN") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE ADDCHAN <rule_id> <#channel>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM RULE ADDCHAN <rule_id> <#channel>");
            return;
        }
        int rule_id = atoi(st[3].c_str());
        if (bot->spamRulesMap.find(rule_id) == bot->spamRulesMap.end()) {
            bot->Reply(theClient, "Rule %d not found.", rule_id);
            return;
        }
        const string chanName = string_lower(st[4]);

        // Check for duplicate in in-memory map
        dronescan::spamRuleChannelsMapType::const_iterator rci =
            bot->spamRuleChannelsMap.find(rule_id);
        if (rci != bot->spamRuleChannelsMap.end()) {
            for (size_t i = 0; i < rci->second.size(); ++i) {
                if (rci->second[i] == chanName) {
                    bot->Reply(theClient,
                        "Channel '%s' is already listed for rule %d.",
                        chanName.c_str(), rule_id);
                    return;
                }
            }
        }

        stringstream iq;
        iq << "INSERT INTO spam_rule_channels (rule_id, channel_name) VALUES ("
           << rule_id << ", '" << escapeSQLChars(chanName) << "')";
        if (!bot->getSqlDb()->Exec(iq)) {
            bot->Reply(theClient,
                "Failed to add channel '%s' to rule %d (duplicate?).",
                chanName.c_str(), rule_id);
            return;
        }
        bot->spamRuleChannelsMap[rule_id].push_back(chanName);
        bot->Reply(theClient, "Channel '%s' added to rule %d.", chanName.c_str(), rule_id);
        return;
    }

    // -- REMCHAN -------------------------------------------------------------
    if (verb == "REMCHAN") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE REMCHAN <rule_id> <#channel>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM RULE REMCHAN <rule_id> <#channel>");
            return;
        }
        int rule_id = atoi(st[3].c_str());
        const string chanName = string_lower(st[4]);

        stringstream dq;
        dq << "DELETE FROM spam_rule_channels WHERE rule_id = " << rule_id
           << " AND channel_name = '" << escapeSQLChars(chanName) << "'";
        if (!bot->getSqlDb()->Exec(dq)) {
            bot->Reply(theClient,
                "Failed to remove channel '%s' from rule %d.", chanName.c_str(), rule_id);
            return;
        }

        // Remove from in-memory map
        bool found = false;
        dronescan::spamRuleChannelsMapType::iterator rci =
            bot->spamRuleChannelsMap.find(rule_id);
        if (rci != bot->spamRuleChannelsMap.end()) {
            std::vector<string>& vec = rci->second;
            for (std::vector<string>::iterator vi = vec.begin(); vi != vec.end(); ++vi) {
                if (*vi == chanName) {
                    vec.erase(vi);
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            bot->Reply(theClient,
                "Channel '%s' was not found in rule %d's list.", chanName.c_str(), rule_id);
            return;
        }
        bot->Reply(theClient, "Channel '%s' removed from rule %d.", chanName.c_str(), rule_id);
        return;
    }

    bot->Reply(theClient,
        "Unknown verb '%s'. Use ADD, DEL, LIST, SHOW, SET, "
        "ADDEVENT, REMEVENT, ADDACTION, REMACTION, ADDCHAN, REMCHAN.", st[2].c_str());
}

// ---------------------------------------------------------------------------
// ACTION subcommands
// ---------------------------------------------------------------------------
static void handleAction(dronescan* bot, const iClient* theClient,
                         const sqlUser* theUser, const StringTokenizer& st)
{
    if (st.size() < 3) {
        bot->Reply(theClient, "Usage: SPAM ACTION <ADD|DEL|LIST> ...");
        return;
    }

    const string verb = string_upper(st[2]);

    // -- LIST ----------------------------------------------------------------
    if (verb == "LIST") {
        if (!checkAccess(theClient, theUser, bot, level::spam_read)) return;

        if (bot->spamActionsMap.empty()) {
            bot->Reply(theClient, "No spam actions defined.");
            return;
        }
        bot->Reply(theClient, "=== Spam Actions (%zu) ===",
                   bot->spamActionsMap.size());
        for (dronescan::spamActionsMapType::const_iterator it = bot->spamActionsMap.begin();
             it != bot->spamActionsMap.end(); ++it) {
            sqlSpamAction* act = it->second;
            bot->Reply(theClient,
                       "[%d] %-20s  Type: %-8s  Dur: %-6s  Delay: %-5d  Enabled: %s",
                       act->getId(),
                       act->getName().c_str(),
                       act->getActionType().c_str(),
                       act->getDuration() >= 0
                           ? std::to_string(act->getDuration()).c_str() : "N/A",
                       act->getDelay(),
                       act->isEnabled() ? "yes" : "no");
            bot->Reply(theClient, "     Reason: %s",
                       act->getReason().empty() ? "(none)" : act->getReason().c_str());
        }
        bot->Reply(theClient, "--- %zu action(s) ---", bot->spamActionsMap.size());
        return;
    }

    // -- ADD -----------------------------------------------------------------
    if (verb == "ADD") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM ACTION ADD <name> <type> [duration] [reason] [delay]
        if (st.size() < 5) {
            bot->Reply(theClient,
                "Usage: SPAM ACTION ADD <name> <type> [duration] [reason] [delay]");
            return;
        }
        const string atype = string_upper(st[4]);
        if (!isValidActionType(atype)) {
            bot->Reply(theClient, "Invalid action type. Use: GLINE, KILL, REPORT");
            return;
        }

        sqlSpamAction* act = new sqlSpamAction(bot->getSqlDb());
        act->setName(st[3]);
        act->setActionType(atype);
        if (st.size() >= 6) act->setDuration(atoi(st[5].c_str()));
        if (st.size() >= 7) act->setReason(st[6]);
        if (st.size() >= 8) act->setDelay(atoi(st[7].c_str()));
        act->setCreatedTs(::time(0));
        act->setModifiedTs(::time(0));

        if (!act->insert()) {
            bot->Reply(theClient, "Failed to add spam action.");
            delete act;
            return;
        }
        bot->spamActionsMap[act->getId()] = act;
        bot->Reply(theClient, "Spam action '%s' added with ID %d.",
                   act->getName().c_str(), act->getId());
        return;
    }

    // -- DEL -----------------------------------------------------------------
    if (verb == "DEL") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM ACTION DEL <id>"); return; }

        int id = atoi(st[3].c_str());
        dronescan::spamActionsMapType::iterator it = bot->spamActionsMap.find(id);
        if (it == bot->spamActionsMap.end()) {
            bot->Reply(theClient, "Action %d not found.", id);
            return;
        }
        if (!it->second->remove()) {
            bot->Reply(theClient, "Failed to delete spam action %d.", id);
            return;
        }
        delete it->second;
        bot->spamActionsMap.erase(it);
        bot->Reply(theClient, "Spam action %d deleted.", id);
        return;
    }

    bot->Reply(theClient, "Unknown verb '%s'. Use ADD, DEL, LIST.", st[2].c_str());
}

// ---------------------------------------------------------------------------
// EXCLUSION subcommands
// ---------------------------------------------------------------------------
static void handleExclusion(dronescan* bot, const iClient* theClient,
                             const sqlUser* theUser, const StringTokenizer& st)
{
    if (st.size() < 3) {
        bot->Reply(theClient, "Usage: SPAM EXCLUSION <ADD|DEL|LIST> ...");
        return;
    }

    const string verb = string_upper(st[2]);

    // -- LIST ----------------------------------------------------------------
    if (verb == "LIST") {
        if (!checkAccess(theClient, theUser, bot, level::spam_read)) return;

        if (bot->spamExclusionsList.empty()) {
            bot->Reply(theClient, "No spam exclusions defined.");
            return;
        }
        bot->Reply(theClient, "=== Spam Exclusions (%zu) ===",
                   bot->spamExclusionsList.size());
        bot->Reply(theClient, "%-4s %-8s %s", "ID", "Type", "Value");
        for (dronescan::spamExclusionsListType::const_iterator it =
                 bot->spamExclusionsList.begin();
             it != bot->spamExclusionsList.end(); ++it) {
            bot->Reply(theClient, "%-4d %-8s %s",
                       (*it)->getId(),
                       (*it)->getExclusionType().c_str(),
                       (*it)->getValue().c_str());
        }
        bot->Reply(theClient, "--- %zu exclusion(s) ---", bot->spamExclusionsList.size());
        return;
    }

    // -- ADD -----------------------------------------------------------------
    if (verb == "ADD") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM EXCLUSION ADD <CHAN|NICK|IP|OPER> <value>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM EXCLUSION ADD <CHAN|NICK|IP|OPER> <value>");
            return;
        }
        const string etype = string_upper(st[3]);
        if (!isValidExclusionType(etype)) {
            bot->Reply(theClient, "Invalid exclusion type. Use: CHAN, NICK, IP, OPER");
            return;
        }

        sqlSpamExclusion* ex = new sqlSpamExclusion(bot->getSqlDb());
        ex->setExclusionType(etype);
        ex->setValue(st[4]);
        ex->setCreatedTs(::time(0));
        ex->setModifiedTs(::time(0));

        if (!ex->insert()) {
            bot->Reply(theClient, "Failed to add exclusion (duplicate?).");
            delete ex;
            return;
        }
        bot->spamExclusionsList.push_back(ex);
        bot->Reply(theClient, "Exclusion added: %s %s (id: %d).",
                   etype.c_str(), st[4].c_str(), ex->getId());
        return;
    }

    // -- DEL -----------------------------------------------------------------
    if (verb == "DEL") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM EXCLUSION DEL <id>"); return; }

        int id = atoi(st[3].c_str());
        bool found = false;
        for (dronescan::spamExclusionsListType::iterator it = bot->spamExclusionsList.begin();
             it != bot->spamExclusionsList.end(); ++it) {
            if ((*it)->getId() == id) {
                if (!(*it)->remove()) {
                    bot->Reply(theClient, "Failed to delete exclusion %d.", id);
                    return;
                }
                delete *it;
                bot->spamExclusionsList.erase(it);
                found = true;
                break;
            }
        }
        if (!found) {
            bot->Reply(theClient, "Exclusion %d not found.", id);
            return;
        }
        bot->Reply(theClient, "Exclusion %d deleted.", id);
        return;
    }

    bot->Reply(theClient, "Unknown verb '%s'. Use ADD, DEL, LIST.", st[2].c_str());
}

// ---------------------------------------------------------------------------
// SPYCLIENT subcommands
// ---------------------------------------------------------------------------
static void handleSpyClient(dronescan* bot, const iClient* theClient,
                             const sqlUser* theUser, const StringTokenizer& st)
{
    if (st.size() < 3) {
        bot->Reply(theClient,
            "Usage: SPAM SPYCLIENT <ADD|DEL|LIST|SHOW|ENABLE|DISABLE> ...");
        return;
    }

    const string verb = string_upper(st[2]);

    // -- LIST ----------------------------------------------------------------
    if (verb == "LIST") {
        if (!checkAccess(theClient, theUser, bot, level::spam_read)) return;
        if (bot->spyClientsMap.empty()) {
            bot->Reply(theClient, "No spy clients defined.");
            return;
        }
        bot->Reply(theClient, "=== Spy Clients (%zu) ===",
                   bot->spyClientsMap.size());
        for (dronescan::spyClientsMapType::const_iterator it = bot->spyClientsMap.begin();
             it != bot->spyClientsMap.end(); ++it) {
            sqlSpyClient* sc = it->second;
            bot->Reply(theClient,
                       "[%d] %-12s  User: %-10s  Host: %-24s  IP: %-16s  Enabled: %s",
                       sc->getId(),
                       sc->getNickname().c_str(),
                       sc->getUsername().c_str(),
                       sc->getHostname().c_str(),
                       sc->getIp().empty() ? "(none)" : sc->getIp().c_str(),
                       sc->isEnabled() ? "yes" : "no");
            bot->Reply(theClient,
                       "     Realname: %-24s  Account: %-16s  Modes: %s",
                       sc->getRealname().empty() ? "(none)" : sc->getRealname().c_str(),
                       sc->getAccount().empty() ? "(none)" : sc->getAccount().c_str(),
                       sc->getModes().empty() ? "(none)" : sc->getModes().c_str());
        }
        bot->Reply(theClient, "--- %zu spy client(s) ---", bot->spyClientsMap.size());
        return;
    }

    // -- SHOW ----------------------------------------------------------------
    if (verb == "SHOW") {
        if (!checkAccess(theClient, theUser, bot, level::spam_read)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM SPYCLIENT SHOW <id>"); return; }

        int id = atoi(st[3].c_str());
        dronescan::spyClientsMapType::const_iterator it = bot->spyClientsMap.find(id);
        if (it == bot->spyClientsMap.end()) {
            bot->Reply(theClient, "Spy client %d not found.", id);
            return;
        }
        sqlSpyClient* sc = it->second;
        bot->Reply(theClient, "ID       : %d", sc->getId());
        bot->Reply(theClient, "Nick     : %s", sc->getNickname().c_str());
        bot->Reply(theClient, "User     : %s", sc->getUsername().c_str());
        bot->Reply(theClient, "Host     : %s", sc->getHostname().c_str());
        bot->Reply(theClient, "IP       : %s", sc->getIp().c_str());
        bot->Reply(theClient, "Realname : %s", sc->getRealname().c_str());
        bot->Reply(theClient, "Account  : %s",
                   sc->getAccount().empty() ? "(none)" : sc->getAccount().c_str());
        bot->Reply(theClient, "Modes    : %s",
                   sc->getModes().empty() ? "(none)" : sc->getModes().c_str());
        bot->Reply(theClient, "Enabled  : %s", sc->isEnabled() ? "yes" : "no");
        return;
    }

    // -- ADD -----------------------------------------------------------------
    if (verb == "ADD") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM SPYCLIENT ADD <nick> <user> <host> <ip> <realname> [account] [modes]
        if (st.size() < 8) {
            bot->Reply(theClient,
                "Usage: SPAM SPYCLIENT ADD <nick> <user> <host> <ip> <realname> [account] [modes]");
            return;
        }

        sqlSpyClient* sc = new sqlSpyClient(bot->getSqlDb());
        sc->setNickname(st[3]);
        sc->setUsername(st[4]);
        sc->setHostname(st[5]);
        sc->setIp(st[6]);
        sc->setRealname(st[7]);
        if (st.size() >= 9) sc->setAccount(st[8]);
        if (st.size() >= 10) sc->setModes(st[9]);
        sc->setEnabled(true);
        sc->setCreatedTs(::time(0));
        sc->setModifiedTs(::time(0));
        sc->setModifiedBy(0);

        if (!sc->insert()) {
            bot->Reply(theClient, "Failed to add spy client.");
            delete sc;
            return;
        }
        bot->spyClientsMap[sc->getId()] = sc;
        // Introduce the new spy client to the network
        bot->introduceSpyClient(sc);
        bot->Reply(theClient, "Spy client '%s' added with ID %d.",
                   sc->getNickname().c_str(), sc->getId());
        return;
    }

    // -- DEL -----------------------------------------------------------------
    if (verb == "DEL") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM SPYCLIENT DEL <id>"); return; }

        int id = atoi(st[3].c_str());
        dronescan::spyClientsMapType::iterator it = bot->spyClientsMap.find(id);
        if (it == bot->spyClientsMap.end()) {
            bot->Reply(theClient, "Spy client %d not found.", id);
            return;
        }
        // Detach from network before removing from DB
        bot->detachSpyClient(id);
        if (!it->second->remove()) {
            bot->Reply(theClient, "Failed to delete spy client %d.", id);
            return;
        }
        delete it->second;
        bot->spyClientsMap.erase(it);
        bot->Reply(theClient, "Spy client %d deleted.", id);
        return;
    }

    // -- ENABLE / DISABLE ----------------------------------------------------
    if (verb == "ENABLE" || verb == "DISABLE") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 4) {
            bot->Reply(theClient, "Usage: SPAM SPYCLIENT %s <id>", verb.c_str());
            return;
        }
        int id = atoi(st[3].c_str());
        dronescan::spyClientsMapType::iterator it = bot->spyClientsMap.find(id);
        if (it == bot->spyClientsMap.end()) {
            bot->Reply(theClient, "Spy client %d not found.", id);
            return;
        }
        sqlSpyClient* sc = it->second;
        sc->setEnabled(verb == "ENABLE");
        sc->setModifiedTs(::time(0));
        sc->setModifiedBy(0);
        if (!sc->commit()) {
            bot->Reply(theClient, "Failed to update spy client %d.", id);
            return;
        }
        // Apply live change
        if (verb == "ENABLE")
            bot->introduceSpyClient(sc);
        else
            bot->detachSpyClient(id);
        bot->Reply(theClient, "Spy client %d %s.",
                   id, verb == "ENABLE" ? "enabled" : "disabled");
        return;
    }

    bot->Reply(theClient,
        "Unknown verb '%s'. Use ADD, DEL, LIST, SHOW, ENABLE, DISABLE.", st[2].c_str());
}

// ---------------------------------------------------------------------------
// MONITORCHAN subcommands
// ---------------------------------------------------------------------------
static void handleMonitorChan(dronescan* bot, const iClient* theClient,
                               const sqlUser* theUser, const StringTokenizer& st)
{
    if (st.size() < 3) {
        bot->Reply(theClient,
            "Usage: SPAM MONITORCHAN <ADD|DEL|LIST|SHOW|ENABLE|DISABLE> ...");
        return;
    }

    const string verb = string_upper(st[2]);

    // -- LIST ----------------------------------------------------------------
    if (verb == "LIST") {
        if (!checkAccess(theClient, theUser, bot, level::spam_read)) return;
        if (bot->monitoredChannelsMap.empty()) {
            bot->Reply(theClient, "No monitored channels defined.");
            return;
        }
        bot->Reply(theClient, "=== Monitored Channels (%zu) ===",
                   bot->monitoredChannelsMap.size());
        bot->Reply(theClient, "%-4s %-32s %-8s %-11s %s",
                   "ID", "Channel", "ForceJoin", "JoinAsSvc", "Enabled");
        for (dronescan::monitoredChannelsMapType::const_iterator it =
                 bot->monitoredChannelsMap.begin();
             it != bot->monitoredChannelsMap.end(); ++it) {
            sqlMonitoredChannel* mc = it->second;
            bot->Reply(theClient, "%-4d %-32s %-8s %-11s %s",
                       mc->getId(),
                       mc->getName().c_str(),
                       mc->isForceJoin() ? "yes" : "no",
                       mc->isJoinAsService() ? "yes" : "no",
                       mc->isEnabled() ? "yes" : "no");
        }
        bot->Reply(theClient, "--- %zu channel(s) ---", bot->monitoredChannelsMap.size());
        return;
    }

    // -- SHOW ----------------------------------------------------------------
    if (verb == "SHOW") {
        if (!checkAccess(theClient, theUser, bot, level::spam_read)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM MONITORCHAN SHOW <id>"); return; }

        int id = atoi(st[3].c_str());
        sqlMonitoredChannel* found = nullptr;
        for (dronescan::monitoredChannelsMapType::const_iterator it =
                 bot->monitoredChannelsMap.begin();
             it != bot->monitoredChannelsMap.end(); ++it) {
            if (it->second->getId() == id) {
                found = it->second;
                break;
            }
        }
        if (!found) {
            bot->Reply(theClient, "Monitored channel %d not found.", id);
            return;
        }
        bot->Reply(theClient, "ID          : %d", found->getId());
        bot->Reply(theClient, "Channel     : %s", found->getName().c_str());
        bot->Reply(theClient, "ForceJoin   : %s", found->isForceJoin() ? "yes" : "no");
        bot->Reply(theClient, "JoinAsService: %s", found->isJoinAsService() ? "yes" : "no");
        bot->Reply(theClient, "Enabled     : %s", found->isEnabled() ? "yes" : "no");
        return;
    }

    // -- ADD -----------------------------------------------------------------
    if (verb == "ADD") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM MONITORCHAN ADD <#channel> [forcejoin 0|1] [joinasservice 0|1]
        if (st.size() < 4) {
            bot->Reply(theClient,
                "Usage: SPAM MONITORCHAN ADD <#channel> [forcejoin 0|1] [joinasservice 0|1]");
            return;
        }
        const string chanName = string_lower(st[3]);
        if (bot->monitoredChannelsMap.count(chanName)) {
            bot->Reply(theClient, "Channel '%s' is already monitored.", chanName.c_str());
            return;
        }

        sqlMonitoredChannel* mc = new sqlMonitoredChannel(bot->getSqlDb());
        mc->setName(chanName);
        mc->setForceJoin(st.size() >= 5 && (st[4] == "1" || st[4] == "yes"));
        mc->setJoinAsService(st.size() >= 6 && (st[5] == "1" || st[5] == "yes"));
        mc->setEnabled(true);
        mc->setCreatedTs(::time(0));
        mc->setModifiedTs(::time(0));
        mc->setModifiedBy(0);

        if (!mc->insert()) {
            bot->Reply(theClient, "Failed to add monitored channel.");
            delete mc;
            return;
        }
        bot->monitoredChannelsMap[chanName] = mc;
        // Start monitoring: schedule a spy client join
        if (!mc->isJoinAsService()) {
            int scId = bot->findBestSpyClient(chanName, mc->isForceJoin());
            if (scId >= 0)
                bot->scheduleSpyClientJoin(scId, chanName, 0, 300);
            else
                bot->Reply(theClient, "Warning: no available spy client for %s.", chanName.c_str());
        } else {
            bot->getUplink()->JoinChannel(bot, chanName, "");
        }
        bot->Reply(theClient, "Monitored channel '%s' added with ID %d.",
                   chanName.c_str(), mc->getId());
        return;
    }

    // -- DEL -----------------------------------------------------------------
    if (verb == "DEL") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM MONITORCHAN DEL <id>"); return; }

        int id = atoi(st[3].c_str());
        dronescan::monitoredChannelsMapType::iterator found = bot->monitoredChannelsMap.end();
        for (dronescan::monitoredChannelsMapType::iterator it =
                 bot->monitoredChannelsMap.begin();
             it != bot->monitoredChannelsMap.end(); ++it) {
            if (it->second->getId() == id) {
                found = it;
                break;
            }
        }
        if (found == bot->monitoredChannelsMap.end()) {
            bot->Reply(theClient, "Monitored channel %d not found.", id);
            return;
        }
        // Part any spy client currently in this channel
        {
            const string chanKey = found->first;
            dronescan::chanActiveSpyMapType::iterator sit = bot->chanActiveSpyMap.find(chanKey);
            if (sit != bot->chanActiveSpyMap.end())
                bot->partSpyClientFromChannel(sit->second, chanKey);
        }
        if (!found->second->remove()) {
            bot->Reply(theClient, "Failed to delete monitored channel %d.", id);
            return;
        }
        delete found->second;
        bot->monitoredChannelsMap.erase(found);
        bot->Reply(theClient, "Monitored channel %d deleted.", id);
        return;
    }

    // -- ENABLE / DISABLE ----------------------------------------------------
    if (verb == "ENABLE" || verb == "DISABLE") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 4) {
            bot->Reply(theClient, "Usage: SPAM MONITORCHAN %s <id>", verb.c_str());
            return;
        }
        int id = atoi(st[3].c_str());
        sqlMonitoredChannel* mc = nullptr;
        for (dronescan::monitoredChannelsMapType::iterator it =
                 bot->monitoredChannelsMap.begin();
             it != bot->monitoredChannelsMap.end(); ++it) {
            if (it->second->getId() == id) {
                mc = it->second;
                break;
            }
        }
        if (!mc) {
            bot->Reply(theClient, "Monitored channel %d not found.", id);
            return;
        }
        mc->setEnabled(verb == "ENABLE");
        mc->setModifiedTs(::time(0));
        mc->setModifiedBy(0);
        if (!mc->commit()) {
            bot->Reply(theClient, "Failed to update monitored channel %d.", id);
            return;
        }
        const string chanKey = string_lower(mc->getName());
        if (verb == "DISABLE") {
            // Part any active spy client from this channel
            dronescan::chanActiveSpyMapType::iterator sit = bot->chanActiveSpyMap.find(chanKey);
            if (sit != bot->chanActiveSpyMap.end())
                bot->partSpyClientFromChannel(sit->second, chanKey);
        } else {
            // Enable: schedule a spy client join
            if (!mc->isJoinAsService()) {
                int scId = bot->findBestSpyClient(mc->getName(), mc->isForceJoin());
                if (scId >= 0)
                    bot->scheduleSpyClientJoin(scId, mc->getName(), 0, 300);
                else
                    bot->Reply(theClient, "Warning: no available spy client for %s.",
                               mc->getName().c_str());
            } else {
                bot->getUplink()->JoinChannel(bot, mc->getName(), "");
            }
        }
        bot->Reply(theClient, "Monitored channel %d %s.",
                   id, verb == "ENABLE" ? "enabled" : "disabled");
        return;
    }

    bot->Reply(theClient,
        "Unknown verb '%s'. Use ADD, DEL, LIST, SHOW, ENABLE, DISABLE.", st[2].c_str());
}

// ---------------------------------------------------------------------------
// SPAMCommand::Exec ? top-level dispatcher
// ---------------------------------------------------------------------------
void SPAMCommand::Exec(const iClient* theClient, const string& Message, const sqlUser* theUser)
{
    StringTokenizer st(Message);

    if (st.size() < 2) {
        Usage(theClient);
        return;
    }

    const string obj = string_upper(st[1]);

    if (obj == "EVENT")       { handleEvent(bot, theClient, theUser, st);       return; }
    if (obj == "RULE")        { handleRule(bot, theClient, theUser, st);        return; }
    if (obj == "ACTION")      { handleAction(bot, theClient, theUser, st);      return; }
    if (obj == "EXCLUSION")   { handleExclusion(bot, theClient, theUser, st);   return; }
    if (obj == "SPYCLIENT")   { handleSpyClient(bot, theClient, theUser, st);   return; }
    if (obj == "MONITORCHAN") { handleMonitorChan(bot, theClient, theUser, st); return; }

    bot->Reply(theClient,
        "Usage: SPAM <EVENT|RULE|ACTION|EXCLUSION|SPYCLIENT|MONITORCHAN> <verb> ...");
}

} // namespace ds
} // namespace gnuworld
