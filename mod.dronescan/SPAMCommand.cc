/**
 * SPAMCommand.cc
 *
 * Top-level SPAM command dispatcher for dronescan.
 * Syntax:
 *   SPAM EVENT     ADD    <name> <type> <target> <param> <points> <expiry> [max_occ]
 *                         [-rule <rule_name>] [-repeat_count <n>]
 *   SPAM EVENT     DEL    <name>
 *   SPAM EVENT     LIST
 *   SPAM EVENT     SHOW   <name>
 *   SPAM EVENT     SET    <name> <field> <value>
 *   SPAM RULE      ADD    <name> <threshold> [-action <action_name>]
 *   SPAM RULE      DEL    <name>
 *   SPAM RULE      LIST
 *   SPAM RULE      SHOW   <name>
 *   SPAM RULE      SET    <name> <field> <value>
 *   SPAM RULE      ADDEVENT   <rule_name> <event_name> [points_override]
 *   SPAM RULE      REMEVENT   <rule_name> <event_name>
 *   SPAM RULE      ADDACTION  <rule_name> <action_name> [dur_override] [reason_override] [delay_override]
 *   SPAM RULE      REMACTION  <rule_name> <action_name>
 *   SPAM RULE      ADDCHAN    <rule_name> <#channel>
 *   SPAM RULE      REMCHAN    <rule_name> <#channel>
 *   SPAM ACTION    ADD    <name> <type> [duration] [reason] [delay]
 *   SPAM ACTION    DEL    <name>
 *   SPAM ACTION    LIST
 *   SPAM ACTION    SET    <name> <field> <value>
 *   SPAM EXCLUSION ADD    <CHAN|NICK|IP|OPER> <value>
 *   SPAM EXCLUSION DEL    <id>
 *   SPAM EXCLUSION LIST
 *   SPAM EXCLUSION SET    <id> <field> <value>
 *   SPAM SPYCLIENT ADD    <nick> <user> <host> <ip> <realname> [account] [modes]
 *   SPAM SPYCLIENT DEL    <id|nick>
 *   SPAM SPYCLIENT LIST
 *   SPAM SPYCLIENT SHOW   <id|nick>
 *   SPAM SPYCLIENT SET    <id|nick> <field> <value>
 *   SPAM SPYCLIENT ENABLE <id|nick>
 *   SPAM SPYCLIENT DISABLE <id|nick>
 *   SPAM CHAN      ADD    <#channel> [forcejoin 0|1] [joinasservice 0|1]
 *   SPAM CHAN      DEL    <#channel>
 *   SPAM CHAN      LIST
 *   SPAM CHAN      SHOW   <#channel>
 *   SPAM CHAN      SET    <#channel> <field> <value>
 *   SPAM CHAN      ENABLE <#channel>
 *   SPAM CHAN      DISABLE <#channel>
 *   SPAM CHAN      ADDSPY <#channel> <nick>
 *   SPAM CHAN      REMSPY <#channel> <nick>
 *
 * target bitmask: chan_priv=1, privmsg=2, chan_not=4, part=8, quit=16, notice=32,
 *                  ctcp=64, all=127
 *                  "chan" is an alias for chan_priv|chan_not (=5)
 *
 * Rule/event/action names are matched case-insensitively and must be
 * unique within their table. RULE SET wait_on_rule_id and EVENT SET
 * requires_event_id take a rule/event name as their value, or "none"
 * (or an empty value) to clear the reference.
 *
 * EVENT ADD's "-rule" option may be repeated to link the new event to
 * several rules in one call; "-repeat_count" is mandatory when <type> is
 * TEXT_REPEAT (it sets repeat_min_count) and invalid for any other type.
 * RULE ADD's "-action" option may likewise be repeated to link the new
 * rule to several existing actions in one call (no override values -
 * use RULE ADDACTION afterward for those).
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#include <cctype>
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

// ---------------------------------------------------------------------------
// Helpers: case-insensitive name -> object lookups
// ---------------------------------------------------------------------------
static sqlSpamRule* findRuleByName(dronescan* bot, const string& name)
{
    const string lname = string_lower(name);
    for (dronescan::spamRulesMapType::const_iterator it = bot->spamRulesMap.begin();
         it != bot->spamRulesMap.end(); ++it) {
        if (string_lower(it->second->getName()) == lname)
            return it->second;
    }
    return nullptr;
}

static sqlSpamEvent* findEventByName(dronescan* bot, const string& name)
{
    const string lname = string_lower(name);
    for (dronescan::spamEventsMapType::const_iterator it = bot->spamEventsMap.begin();
         it != bot->spamEventsMap.end(); ++it) {
        if (string_lower(it->second->getName()) == lname)
            return it->second;
    }
    return nullptr;
}

static sqlSpamAction* findActionByName(dronescan* bot, const string& name)
{
    const string lname = string_lower(name);
    for (dronescan::spamActionsMapType::const_iterator it = bot->spamActionsMap.begin();
         it != bot->spamActionsMap.end(); ++it) {
        if (string_lower(it->second->getName()) == lname)
            return it->second;
    }
    return nullptr;
}

static sqlSpyClient* findSpyClientByNick(dronescan* bot, const string& nick)
{
    const string lnick = string_lower(nick);
    for (dronescan::spyClientsMapType::const_iterator it = bot->spyClientsMap.begin();
         it != bot->spyClientsMap.end(); ++it) {
        if (string_lower(it->second->getNickname()) == lnick)
            return it->second;
    }
    return nullptr;
}

// Numeric argument -> id lookup (spyClientsMap is keyed by id); otherwise
// nickname lookup. IRC nicknames can't start with a digit, so a purely
// numeric argument is unambiguous.
static sqlSpyClient* findSpyClientByIdOrNick(dronescan* bot, const string& arg)
{
    bool numeric = !arg.empty();
    for (size_t i = 0; numeric && i < arg.size(); ++i)
        if (!isdigit(static_cast<unsigned char>(arg[i])))
            numeric = false;

    if (numeric) {
        dronescan::spyClientsMapType::const_iterator it =
            bot->spyClientsMap.find(atoi(arg.c_str()));
        return (it != bot->spyClientsMap.end()) ? it->second : nullptr;
    }
    return findSpyClientByNick(bot, arg);
}

// Comma-separated nicknames of the spy clients restricted to channelId, or
// an empty string if the channel has no restriction (any spy client may
// join it).
static string spyClientNickListForChannel(dronescan* bot, int channelId)
{
    dronescan::monitoredChannelSpyClientsMapType::const_iterator it =
        bot->monitoredChannelSpyClientsMap.find(channelId);
    if (it == bot->monitoredChannelSpyClientsMap.end() || it->second.empty())
        return string();

    string out;
    for (size_t i = 0; i < it->second.size(); ++i) {
        dronescan::spyClientsMapType::const_iterator scit =
            bot->spyClientsMap.find(it->second[i]);
        if (scit == bot->spyClientsMap.end())
            continue;  // stale id
        if (!out.empty())
            out += ", ";
        out += scit->second->getNickname();
    }
    return out;
}

// Link an event to a rule via spam_rule_events (INSERT + in-memory map
// update). Shared by "RULE ADDEVENT" and the "-rule" flag on "EVENT ADD".
// Caller is responsible for calling bot->relinkSpamGraph() afterward.
static bool linkEventToRule(dronescan* bot, sqlSpamRule* rule, sqlSpamEvent* ev,
                            int pointsOverride)
{
    const int rule_id  = rule->getId();
    const int event_id = ev->getId();

    stringstream q;
    q << "INSERT INTO spam_rule_events (rule_id, event_id, points_override) VALUES ("
      << rule_id << ", " << event_id << ", "
      << (pointsOverride >= 0 ? std::to_string(pointsOverride) : "NULL")
      << ")";
    if (!bot->getSqlDb()->Exec(q))
        return false;
    bot->spamRuleEventsMap[rule_id].push_back(std::make_pair(event_id, pointsOverride));
    return true;
}

// Link an action to a rule via spam_rule_actions (duplicate-binding check +
// INSERT + in-memory map update). Shared by "RULE ADDACTION" and the
// "-action" flag on "RULE ADD". durOverride/delayOverride use -1 and
// reasonOverride an empty string as "not provided" sentinels, matching
// spam_rule_actions' own NULL-means-default convention.
// Sets *outAlreadyLinked and returns nullptr if the rule already has this
// action bound; returns nullptr on insert failure; caller is responsible
// for calling bot->relinkSpamGraph() afterward on success.
static sqlSpamRuleAction* linkActionToRule(dronescan* bot, sqlSpamRule* rule, sqlSpamAction* action,
                                           int durOverride, const string& reasonOverride,
                                           int delayOverride, bool* outAlreadyLinked)
{
    *outAlreadyLinked = false;
    const int rule_id   = rule->getId();
    const int action_id = action->getId();

    // A rule may only bind a given action once (spam_rule_actions has a
    // UNIQUE(rule_id, action_id) constraint).
    dronescan::spamRuleActionsMapType::const_iterator existingIt =
        bot->spamRuleActionsMap.find(rule_id);
    if (existingIt != bot->spamRuleActionsMap.end()) {
        for (size_t i = 0; i < existingIt->second.size(); ++i) {
            if (existingIt->second[i]->getActionId() == action_id) {
                *outAlreadyLinked = true;
                return nullptr;
            }
        }
    }

    sqlSpamRuleAction* ra = new sqlSpamRuleAction(bot->getSqlDb());
    ra->setRuleId(rule_id);
    ra->setActionId(action_id);
    ra->setActionType(action->getActionType());
    if (durOverride >= 0) ra->setActionDurationOverride(durOverride);
    if (!reasonOverride.empty()) ra->setActionReasonOverride(reasonOverride);
    if (delayOverride >= 0) ra->setDelayOverride(delayOverride);

    if (!ra->insert()) {
        delete ra;
        return nullptr;
    }
    bot->spamRuleActionsMap[rule_id].push_back(ra);
    return ra;
}

// Parse comma-separated target names to an integer bitmask.
// Returns -1 on any invalid token.
// e.g. "chan_priv,privmsg" -> 3,  "all" -> 127,  "chan" -> 5 (chan_priv|chan_not)
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
        else if (p == "ctcp")      mask |= spam_target::CTCP;
        else if (p == "all")       mask |= spam_target::ALL;
        else return -1;
    }
    return (mask > 0) ? mask : -1;
}

// Decode an integer bitmask to a human-readable comma-separated string.
// e.g. 127 -> "all",  5 -> "chan_priv,chan_not"
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
    if (mask & spam_target::CTCP)      { if (!s.empty()) s += ","; s += "ctcp";     }
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
            "Usage: SPAM EVENT <ADD|DEL|LIST|SHOW|SET> ...");
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
            if (ev->getEventType() == "TEXT_REPEAT") {
                bot->Reply(theClient,
                           "     Param: %-32s  CaseSens: %s",
                           ev->getParam().empty() ? "(none)" : ev->getParam().c_str(),
                           ev->isCaseSensitive() ? "yes" : "no");
                bot->Reply(theClient,
                    "     CrossUser: %-3s  MinCount: %-3d  ExclRegex: %s",
                    ev->isRepeatCrossUser() ? "yes" : "no",
                    ev->getRepeatMinCount(),
                    ev->getRepeatExclusionRegex().empty()
                        ? "(none)" : ev->getRepeatExclusionRegex().c_str());
            } else {
                bot->Reply(theClient,
                           "     Param: %s",
                           ev->getParam().empty() ? "(none)" : ev->getParam().c_str());
            }
        }
        bot->Reply(theClient, "--- %zu event(s) ---", bot->spamEventsMap.size());
        return;
    }

    // -- SHOW ----------------------------------------------------------------
    if (verb == "SHOW") {
        if (!checkAccess(theClient, theUser, bot, level::spam_read)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM EVENT SHOW <name>"); return; }

        sqlSpamEvent* ev = findEventByName(bot, st[3]);
        if (!ev) {
            bot->Reply(theClient, "Event '%s' not found.", st[3].c_str());
            return;
        }
        bot->Reply(theClient, "ID      : %d", ev->getId());
        bot->Reply(theClient, "Name    : %s", ev->getName().c_str());
        bot->Reply(theClient, "Type    : %s", ev->getEventType().c_str());
        bot->Reply(theClient, "Param   : %s", ev->getParam().c_str());
        bot->Reply(theClient, "Target  : %s (mask=%d)",
                   targetBitmaskToString(ev->getTarget()).c_str(), ev->getTarget());
        if (ev->getEventType() == "TEXT_REPEAT")
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
        //                [-rule <rule_name>] [-repeat_count <n>]
        if (st.size() < 9) {
            bot->Reply(theClient,
                "Usage: SPAM EVENT ADD <name> <type> <target> <param> <points> <expiry> "
                "[max_occ] [-rule <rule_name>] [-repeat_count <n>]");
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
                "Invalid target '%s'. Use comma-separated: chan,privmsg,notice,part,quit,ctcp,all",
                st[5].c_str());
            return;
        }

        // [max_occ] is an optional trailing positional before the flags; it's
        // present only if the next token doesn't look like a "-flag".
        size_t pos = 9;
        bool hasMaxOcc = false;
        int maxOcc = -1;
        if (pos < st.size() && !st[pos].empty() && st[pos][0] != '-') {
            maxOcc = atoi(st[pos].c_str());
            hasMaxOcc = true;
            ++pos;
        }

        // Scan remaining tokens for -rule (repeatable) and -repeat_count.
        std::vector<string> ruleNames;
        bool hasRepeatCount = false;
        int repeatCount = 0;
        while (pos < st.size()) {
            const string flag = string_lower(st[pos]);
            if (flag == "-rule") {
                if (pos + 1 >= st.size()) {
                    bot->Reply(theClient, "-rule requires a rule name.");
                    return;
                }
                ruleNames.push_back(st[pos + 1]);
                pos += 2;
            } else if (flag == "-repeat_count") {
                if (pos + 1 >= st.size()) {
                    bot->Reply(theClient, "-repeat_count requires a value.");
                    return;
                }
                repeatCount = atoi(st[pos + 1].c_str());
                hasRepeatCount = true;
                pos += 2;
            } else {
                bot->Reply(theClient,
                    "Unknown option '%s'. Valid options: -rule <rule_name>, -repeat_count <n>.",
                    st[pos].c_str());
                return;
            }
        }

        if (evType == "TEXT_REPEAT" && !hasRepeatCount) {
            bot->Reply(theClient, "TEXT_REPEAT events require -repeat_count <n>.");
            return;
        }
        if (evType != "TEXT_REPEAT" && hasRepeatCount) {
            bot->Reply(theClient, "-repeat_count is only valid for TEXT_REPEAT events.");
            return;
        }

        // Resolve every -rule name up front so ADD fails fast on a typo
        // instead of leaving a half-linked event behind.
        std::vector<sqlSpamRule*> rulesToLink;
        for (size_t i = 0; i < ruleNames.size(); ++i) {
            sqlSpamRule* r = findRuleByName(bot, ruleNames[i]);
            if (!r) {
                bot->Reply(theClient, "Rule '%s' not found.", ruleNames[i].c_str());
                return;
            }
            rulesToLink.push_back(r);
        }

        sqlSpamEvent* ev = new sqlSpamEvent(bot->getSqlDb());
        ev->setName(st[3]);
        ev->setEventType(evType);
        ev->setTarget(targetMask);
        ev->setParam(st[6]);
        ev->setPoints(atoi(st[7].c_str()));
        ev->setPointExpiry(atoi(st[8].c_str()));
        if (hasMaxOcc)
            ev->setMaxOccurrence(maxOcc);
        if (hasRepeatCount)
            ev->setRepeatMinCount(repeatCount);
        ev->setCreatedTs(::time(0));
        ev->setModifiedTs(::time(0));
        ev->setModifiedBy(0);

        if (!ev->insert()) {
            bot->Reply(theClient, "Failed to add spam event (duplicate name?).");
            delete ev;
            return;
        }
        bot->spamEventsMap[ev->getId()] = ev;
        bot->compileEventRegex(ev);
        bot->compileRepeatExclusionRegex(ev);

        string linkedNames;
        for (size_t i = 0; i < rulesToLink.size(); ++i) {
            if (!linkEventToRule(bot, rulesToLink[i], ev, -1)) {
                bot->Reply(theClient, "Warning: failed to link event to rule '%s'.",
                           rulesToLink[i]->getName().c_str());
                continue;
            }
            if (!linkedNames.empty()) linkedNames += ", ";
            linkedNames += rulesToLink[i]->getName();
        }
        bot->relinkSpamGraph();

        bot->Reply(theClient, "Spam event '%s' added with ID %d (target: %s).",
                   ev->getName().c_str(), ev->getId(),
                   targetBitmaskToString(ev->getTarget()).c_str());
        if (!linkedNames.empty())
            bot->Reply(theClient, "Linked to rule(s): %s.", linkedNames.c_str());
        return;
    }

    // -- DEL -----------------------------------------------------------------
    if (verb == "DEL") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM EVENT DEL <name>"); return; }

        sqlSpamEvent* ev = findEventByName(bot, st[3]);
        if (!ev) {
            bot->Reply(theClient, "Event '%s' not found.", st[3].c_str());
            return;
        }
        const int id = ev->getId();
        const string evName = ev->getName();
        if (!ev->remove()) {
            bot->Reply(theClient, "Failed to delete spam event '%s'.", evName.c_str());
            return;
        }
        delete ev;
        bot->spamEventsMap.erase(id);
        bot->freeEventRegexes(id);
        bot->relinkSpamGraph();
        bot->Reply(theClient, "Spam event '%s' deleted.", evName.c_str());
        return;
    }

    // -- SET -----------------------------------------------------------------
    if (verb == "SET") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM EVENT SET <name> <field> <value>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM EVENT SET <name> <field> <value>");
            return;
        }
        sqlSpamEvent* ev = findEventByName(bot, st[3]);
        if (!ev) {
            bot->Reply(theClient, "Event '%s' not found.", st[3].c_str());
            return;
        }
        const int id = ev->getId();
        const string field = string_lower(st[4]);
        const string value = (st.size() >= 6) ? st[5] : string();

        if (field == "name") {
            ev->setName(value);
        } else if (field == "description") {
            ev->setDescription(value);
        } else if (field == "param") {
            ev->setParam(value);
            bot->compileEventRegex(ev);
            if (ev->getEventType() == "TEXT" && !value.empty() &&
                bot->spamRegexCache.find(id) == bot->spamRegexCache.end())
                bot->Reply(theClient, "Warning: regex compile failed.");
        } else if (field == "target") {
            int mask = parseTargetBitmask(value);
            if (mask < 0) {
                bot->Reply(theClient,
                    "Invalid target '%s'. Use comma-separated: chan,privmsg,notice,part,quit,ctcp,all",
                    value.c_str());
                return;
            }
            ev->setTarget(mask);
        } else if (field == "case_sensitive") {
            if (ev->getEventType() != "TEXT_REPEAT") {
                bot->Reply(theClient,
                    "case_sensitive only affects TEXT_REPEAT dedup matching. "
                    "For TEXT event regex matching, prefix your pattern with "
                    "'(?i)' instead to make it case-insensitive.");
                return;
            }
            ev->setCaseSensitive(value == "1" || value == "yes" || value == "true");
        } else if (field == "points") {
            ev->setPoints(atoi(value.c_str()));
        } else if (field == "point_expiry") {
            ev->setPointExpiry(atoi(value.c_str()));
        } else if (field == "max_occurrence") {
            ev->setMaxOccurrence(atoi(value.c_str()));
        } else if (field == "requires_event_id") {
            if (value.empty() || string_lower(value) == "none") {
                ev->setRequiresEventId(0);
            } else {
                sqlSpamEvent* reqEv = findEventByName(bot, value);
                if (!reqEv) {
                    bot->Reply(theClient, "Event '%s' not found.", value.c_str());
                    return;
                }
                ev->setRequiresEventId(reqEv->getId());
            }
        } else if (field == "enabled") {
            ev->setEnabled(value == "1" || value == "yes" || value == "true");
        } else if (field == "repeat_crossuser") {
            ev->setRepeatCrossUser(value == "1" || value == "yes" || value == "true");
        } else if (field == "repeat_min_count") {
            ev->setRepeatMinCount(atoi(value.c_str()));
        } else if (field == "repeat_exclusion_regex") {
            ev->setRepeatExclusionRegex(value);
            bot->compileRepeatExclusionRegex(ev);
            if (ev->getEventType() == "TEXT_REPEAT" && !value.empty() &&
                bot->spamRepeatExclusionCache.find(id) == bot->spamRepeatExclusionCache.end())
                bot->Reply(theClient, "Warning: exclusion regex compile failed.");
        } else {
            bot->Reply(theClient,
                "Unknown field '%s'. Valid: name, description, param, target, case_sensitive, "
                "points, point_expiry, max_occurrence, requires_event_id, enabled, "
                "repeat_crossuser, repeat_min_count, repeat_exclusion_regex",
                field.c_str());
            return;
        }

        ev->setModifiedTs(::time(0));
        ev->setModifiedBy(0);
        if (!ev->commit()) {
            bot->Reply(theClient, "Failed to update event %d%s.", id,
                       field == "name" ? " (duplicate name?)" : "");
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
            "Usage: SPAM RULE <ADD|DEL|LIST|SHOW|SET|ADDEVENT|REMEVENT|"
            "ADDACTION|REMACTION|ADDCHAN|REMCHAN> ...");
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
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM RULE SHOW <name>"); return; }

        sqlSpamRule* rule = findRuleByName(bot, st[3]);
        if (!rule) {
            bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
            return;
        }
        const int id = rule->getId();
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
        // SPAM RULE ADD <name> <threshold> [-action <action_name>]
        if (st.size() < 5) {
            bot->Reply(theClient,
                "Usage: SPAM RULE ADD <name> <threshold> [-action <action_name>]");
            return;
        }

        // Scan remaining tokens for -action (repeatable).
        std::vector<string> actionNames;
        size_t pos = 5;
        while (pos < st.size()) {
            const string flag = string_lower(st[pos]);
            if (flag == "-action") {
                if (pos + 1 >= st.size()) {
                    bot->Reply(theClient, "-action requires an action name.");
                    return;
                }
                actionNames.push_back(st[pos + 1]);
                pos += 2;
            } else {
                bot->Reply(theClient,
                    "Unknown option '%s'. Valid options: -action <action_name>.",
                    st[pos].c_str());
                return;
            }
        }

        // Resolve every -action name up front so ADD fails fast on a typo
        // instead of leaving a half-linked rule behind.
        std::vector<sqlSpamAction*> actionsToLink;
        for (size_t i = 0; i < actionNames.size(); ++i) {
            sqlSpamAction* a = findActionByName(bot, actionNames[i]);
            if (!a) {
                bot->Reply(theClient, "Action '%s' not found.", actionNames[i].c_str());
                return;
            }
            actionsToLink.push_back(a);
        }

        sqlSpamRule* rule = new sqlSpamRule(bot->getSqlDb());
        rule->setName(st[3]);
        rule->setThreshold(atoi(st[4].c_str()));
        rule->setCreatedTs(::time(0));
        rule->setModifiedTs(::time(0));

        if (!rule->insert()) {
            bot->Reply(theClient, "Failed to add spam rule (duplicate name?).");
            delete rule;
            return;
        }
        bot->spamRulesMap[rule->getId()] = rule;

        string linkedNames;
        for (size_t i = 0; i < actionsToLink.size(); ++i) {
            bool alreadyLinked = false;
            sqlSpamRuleAction* ra = linkActionToRule(bot, rule, actionsToLink[i],
                                                      -1, string(), -1, &alreadyLinked);
            if (!ra) {
                bot->Reply(theClient, "Warning: failed to link action '%s'%s.",
                           actionsToLink[i]->getName().c_str(),
                           alreadyLinked ? " (already linked)" : "");
                continue;
            }
            if (!linkedNames.empty()) linkedNames += ", ";
            linkedNames += actionsToLink[i]->getName();
        }
        bot->relinkSpamGraph();

        bot->Reply(theClient, "Spam rule '%s' added with ID %d.",
                   rule->getName().c_str(), rule->getId());
        if (!linkedNames.empty())
            bot->Reply(theClient, "Linked to action(s): %s.", linkedNames.c_str());
        return;
    }

    // -- DEL -----------------------------------------------------------------
    if (verb == "DEL") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM RULE DEL <name>"); return; }

        sqlSpamRule* rule = findRuleByName(bot, st[3]);
        if (!rule) {
            bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
            return;
        }
        const int id = rule->getId();
        const string ruleName = rule->getName();
        if (!rule->remove()) {
            bot->Reply(theClient, "Failed to delete spam rule '%s'.", ruleName.c_str());
            return;
        }
        delete rule;
        bot->spamRulesMap.erase(id);
        // Clean up in-memory rule-event and rule-action entries too
        bot->spamRuleEventsMap.erase(id);
        dronescan::spamRuleActionsMapType::iterator rai = bot->spamRuleActionsMap.find(id);
        if (rai != bot->spamRuleActionsMap.end()) {
            for (size_t i = 0; i < rai->second.size(); ++i)
                delete rai->second[i];
            bot->spamRuleActionsMap.erase(rai);
        }
        bot->relinkSpamGraph();
        bot->Reply(theClient, "Spam rule '%s' deleted.", ruleName.c_str());
        return;
    }

    // -- ADDEVENT ------------------------------------------------------------
    if (verb == "ADDEVENT") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE ADDEVENT <rule_name> <event_name> [points_override]
        if (st.size() < 5) {
            bot->Reply(theClient,
                "Usage: SPAM RULE ADDEVENT <rule_name> <event_name> [points_override]");
            return;
        }
        sqlSpamRule* rule = findRuleByName(bot, st[3]);
        if (!rule) {
            bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
            return;
        }
        sqlSpamEvent* ev = findEventByName(bot, st[4]);
        if (!ev) {
            bot->Reply(theClient, "Event '%s' not found.", st[4].c_str());
            return;
        }
        int po = (st.size() >= 6) ? atoi(st[5].c_str()) : -1;

        if (!linkEventToRule(bot, rule, ev, po)) {
            bot->Reply(theClient, "Failed to link event %d to rule %d.",
                       ev->getId(), rule->getId());
            return;
        }
        bot->relinkSpamGraph();
        bot->Reply(theClient, "Event '%s' linked to rule '%s'%s.",
                   ev->getName().c_str(), rule->getName().c_str(),
                   po >= 0 ? (" (points override: " + std::to_string(po) + ")").c_str() : "");
        return;
    }

    // -- REMEVENT ------------------------------------------------------------
    if (verb == "REMEVENT") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM RULE REMEVENT <rule_name> <event_name>");
            return;
        }
        sqlSpamRule* rule = findRuleByName(bot, st[3]);
        if (!rule) {
            bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
            return;
        }
        sqlSpamEvent* ev = findEventByName(bot, st[4]);
        if (!ev) {
            bot->Reply(theClient, "Event '%s' not found.", st[4].c_str());
            return;
        }
        const int rule_id  = rule->getId();
        const int event_id = ev->getId();

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
        bot->relinkSpamGraph();
        bot->Reply(theClient, "Event '%s' unlinked from rule '%s'.",
                   ev->getName().c_str(), rule->getName().c_str());
        return;
    }

    // -- ADDACTION -----------------------------------------------------------
    if (verb == "ADDACTION") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE ADDACTION <rule_name> <action_name> [dur_override] [reason_override] [delay_override]
        if (st.size() < 5) {
            bot->Reply(theClient,
                "Usage: SPAM RULE ADDACTION <rule_name> <action_name> "
                "[dur_override] [reason_override] [delay_override]");
            return;
        }
        sqlSpamRule* rule = findRuleByName(bot, st[3]);
        if (!rule) {
            bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
            return;
        }
        sqlSpamAction* action = findActionByName(bot, st[4]);
        if (!action) {
            bot->Reply(theClient, "Action '%s' not found.", st[4].c_str());
            return;
        }
        const int durOverride    = (st.size() >= 6) ? atoi(st[5].c_str()) : -1;
        const string reasonOverride = (st.size() >= 7) ? st[6] : string();
        const int delayOverride  = (st.size() >= 8) ? atoi(st[7].c_str()) : -1;

        // A rule may only bind a given action once (spam_rule_actions has a
        // UNIQUE(rule_id, action_id) constraint) so REMACTION can identify a
        // binding unambiguously by <rule_name> <action_name>.
        bool alreadyLinked = false;
        sqlSpamRuleAction* ra = linkActionToRule(bot, rule, action,
                                                  durOverride, reasonOverride, delayOverride,
                                                  &alreadyLinked);
        if (!ra) {
            if (alreadyLinked)
                bot->Reply(theClient,
                    "Action '%s' is already linked to rule '%s'.",
                    action->getName().c_str(), rule->getName().c_str());
            else
                bot->Reply(theClient, "Failed to link action %d to rule %d.",
                           action->getId(), rule->getId());
            return;
        }
        bot->relinkSpamGraph();
        bot->Reply(theClient, "Action '%s' linked to rule '%s' (sra id: %d).",
                   action->getName().c_str(), rule->getName().c_str(), ra->getId());
        return;
    }

    // -- REMACTION -----------------------------------------------------------
    if (verb == "REMACTION") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE REMACTION <rule_name> <action_name>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM RULE REMACTION <rule_name> <action_name>");
            return;
        }
        sqlSpamRule* rule = findRuleByName(bot, st[3]);
        if (!rule) {
            bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
            return;
        }
        sqlSpamAction* action = findActionByName(bot, st[4]);
        if (!action) {
            bot->Reply(theClient, "Action '%s' not found.", st[4].c_str());
            return;
        }

        dronescan::spamRuleActionsMapType::iterator rit =
            bot->spamRuleActionsMap.find(rule->getId());
        if (rit != bot->spamRuleActionsMap.end()) {
            std::vector<sqlSpamRuleAction*>& vec = rit->second;
            for (std::vector<sqlSpamRuleAction*>::iterator vi = vec.begin();
                 vi != vec.end(); ++vi) {
                if ((*vi)->getActionId() == action->getId()) {
                    if (!(*vi)->remove()) {
                        bot->Reply(theClient, "Failed to remove rule-action binding.");
                        return;
                    }
                    delete *vi;
                    vec.erase(vi);
                    bot->relinkSpamGraph();
                    bot->Reply(theClient, "Action '%s' unlinked from rule '%s'.",
                               action->getName().c_str(), rule->getName().c_str());
                    return;
                }
            }
        }
        bot->Reply(theClient, "Action '%s' is not linked to rule '%s'.",
                   action->getName().c_str(), rule->getName().c_str());
        return;
    }

    // -- SET -----------------------------------------------------------------
    if (verb == "SET") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE SET <name> <field> <value>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM RULE SET <name> <field> <value>");
            return;
        }
        sqlSpamRule* rule = findRuleByName(bot, st[3]);
        if (!rule) {
            bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
            return;
        }
        const int id = rule->getId();
        const string field = string_lower(st[4]);
        const string value = (st.size() >= 6) ? st[5] : string();

        if (field == "name") {
            rule->setName(value);
        } else if (field == "description") {
            rule->setDescription(value);
        } else if (field == "threshold") {
            rule->setThreshold(atoi(value.c_str()));
        } else if (field == "wait_on_rule_id") {
            if (value.empty() || string_lower(value) == "none") {
                rule->setWaitOnRuleId(0);
            } else {
                sqlSpamRule* waitRule = findRuleByName(bot, value);
                if (!waitRule) {
                    bot->Reply(theClient, "Rule '%s' not found.", value.c_str());
                    return;
                }
                rule->setWaitOnRuleId(waitRule->getId());
            }
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
                "Unknown field '%s'. Valid: name, description, threshold, wait_on_rule_id, "
                "enabled, points_per, score_globally, allchans",
                field.c_str());
            return;
        }

        rule->setModifiedTs(::time(0));
        rule->setModifiedBy(0);
        if (!rule->commit()) {
            bot->Reply(theClient, "Failed to update rule %d%s.", id,
                       field == "name" ? " (duplicate name?)" : "");
            return;
        }
        bot->Reply(theClient, "Rule %d: %s set to '%s'.", id, field.c_str(), value.c_str());
        return;
    }

    // -- ADDCHAN -------------------------------------------------------------
    if (verb == "ADDCHAN") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE ADDCHAN <rule_name> <#channel>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM RULE ADDCHAN <rule_name> <#channel>");
            return;
        }
        sqlSpamRule* rule = findRuleByName(bot, st[3]);
        if (!rule) {
            bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
            return;
        }
        const int rule_id = rule->getId();
        const string chanName = string_lower(st[4]);

        // Check for duplicate in in-memory map
        dronescan::spamRuleChannelsMapType::const_iterator rci =
            bot->spamRuleChannelsMap.find(rule_id);
        if (rci != bot->spamRuleChannelsMap.end()) {
            for (size_t i = 0; i < rci->second.size(); ++i) {
                if (rci->second[i] == chanName) {
                    bot->Reply(theClient,
                        "Channel '%s' is already listed for rule '%s'.",
                        chanName.c_str(), rule->getName().c_str());
                    return;
                }
            }
        }

        stringstream iq;
        iq << "INSERT INTO spam_rule_channels (rule_id, channel_name) VALUES ("
           << rule_id << ", '" << escapeSQLChars(chanName) << "')";
        if (!bot->getSqlDb()->Exec(iq)) {
            bot->Reply(theClient,
                "Failed to add channel '%s' to rule '%s' (duplicate?).",
                chanName.c_str(), rule->getName().c_str());
            return;
        }
        bot->spamRuleChannelsMap[rule_id].push_back(chanName);
        bot->Reply(theClient, "Channel '%s' added to rule '%s'.",
                   chanName.c_str(), rule->getName().c_str());
        return;
    }

    // -- REMCHAN -------------------------------------------------------------
    if (verb == "REMCHAN") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM RULE REMCHAN <rule_name> <#channel>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM RULE REMCHAN <rule_name> <#channel>");
            return;
        }
        sqlSpamRule* rule = findRuleByName(bot, st[3]);
        if (!rule) {
            bot->Reply(theClient, "Rule '%s' not found.", st[3].c_str());
            return;
        }
        const int rule_id = rule->getId();
        const string chanName = string_lower(st[4]);

        stringstream dq;
        dq << "DELETE FROM spam_rule_channels WHERE rule_id = " << rule_id
           << " AND channel_name = '" << escapeSQLChars(chanName) << "'";
        if (!bot->getSqlDb()->Exec(dq)) {
            bot->Reply(theClient,
                "Failed to remove channel '%s' from rule '%s'.",
                chanName.c_str(), rule->getName().c_str());
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
                "Channel '%s' was not found in rule '%s's list.",
                chanName.c_str(), rule->getName().c_str());
            return;
        }
        bot->Reply(theClient, "Channel '%s' removed from rule '%s'.",
                   chanName.c_str(), rule->getName().c_str());
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
        bot->Reply(theClient, "Usage: SPAM ACTION <ADD|DEL|LIST|SET> ...");
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
            bot->Reply(theClient, "Failed to add spam action (duplicate name?).");
            delete act;
            return;
        }
        bot->spamActionsMap[act->getId()] = act;
        bot->relinkSpamGraph();
        bot->Reply(theClient, "Spam action '%s' added with ID %d.",
                   act->getName().c_str(), act->getId());
        return;
    }

    // -- DEL -----------------------------------------------------------------
    if (verb == "DEL") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM ACTION DEL <name>"); return; }

        sqlSpamAction* act = findActionByName(bot, st[3]);
        if (!act) {
            bot->Reply(theClient, "Action '%s' not found.", st[3].c_str());
            return;
        }
        const int id = act->getId();
        const string actName = act->getName();
        if (!act->remove()) {
            bot->Reply(theClient, "Failed to delete spam action '%s'.", actName.c_str());
            return;
        }
        delete act;
        bot->spamActionsMap.erase(id);
        bot->relinkSpamGraph();
        bot->Reply(theClient, "Spam action '%s' deleted.", actName.c_str());
        return;
    }

    // -- SET -----------------------------------------------------------------
    if (verb == "SET") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM ACTION SET <name> <field> <value>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM ACTION SET <name> <field> <value>");
            return;
        }
        sqlSpamAction* act = findActionByName(bot, st[3]);
        if (!act) {
            bot->Reply(theClient, "Action '%s' not found.", st[3].c_str());
            return;
        }
        const int id = act->getId();
        const string field = string_lower(st[4]);
        const string value = (st.size() >= 6) ? st[5] : string();

        if (field == "name") {
            act->setName(value);
        } else if (field == "action_type") {
            const string atype = string_upper(value);
            if (!isValidActionType(atype)) {
                bot->Reply(theClient, "Invalid action type. Use: GLINE, KILL, REPORT");
                return;
            }
            act->setActionType(atype);
        } else if (field == "duration") {
            act->setDuration(atoi(value.c_str()));
        } else if (field == "reason") {
            act->setReason(value);
        } else if (field == "delay") {
            act->setDelay(atoi(value.c_str()));
        } else if (field == "rand_min") {
            act->setRandMin(atoi(value.c_str()));
        } else if (field == "rand_max") {
            act->setRandMax(atoi(value.c_str()));
        } else if (field == "enabled") {
            act->setEnabled(value == "1" || value == "yes" || value == "true");
        } else {
            bot->Reply(theClient,
                "Unknown field '%s'. Valid: name, action_type, duration, reason, delay, "
                "rand_min, rand_max, enabled",
                field.c_str());
            return;
        }

        act->setModifiedTs(::time(0));
        act->setModifiedBy(0);
        if (!act->commit()) {
            bot->Reply(theClient, "Failed to update action %d%s.", id,
                       field == "name" ? " (duplicate name?)" : "");
            return;
        }
        bot->relinkSpamGraph();
        bot->Reply(theClient, "Action %d: %s set to '%s'.", id, field.c_str(), value.c_str());
        return;
    }

    bot->Reply(theClient, "Unknown verb '%s'. Use ADD, DEL, LIST, SET.", st[2].c_str());
}

// ---------------------------------------------------------------------------
// EXCLUSION subcommands
// ---------------------------------------------------------------------------
static void handleExclusion(dronescan* bot, const iClient* theClient,
                             const sqlUser* theUser, const StringTokenizer& st)
{
    if (st.size() < 3) {
        bot->Reply(theClient, "Usage: SPAM EXCLUSION <ADD|DEL|LIST|SET> ...");
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

    // -- SET -----------------------------------------------------------------
    if (verb == "SET") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM EXCLUSION SET <id> <field> <value>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM EXCLUSION SET <id> <field> <value>");
            return;
        }
        const int id = atoi(st[3].c_str());
        sqlSpamExclusion* ex = nullptr;
        for (dronescan::spamExclusionsListType::const_iterator it =
                 bot->spamExclusionsList.begin();
             it != bot->spamExclusionsList.end(); ++it) {
            if ((*it)->getId() == id) { ex = *it; break; }
        }
        if (!ex) {
            bot->Reply(theClient, "Exclusion %d not found.", id);
            return;
        }
        const string field = string_lower(st[4]);
        const string value = (st.size() >= 6) ? st[5] : string();

        if (field == "exclusion_type") {
            const string etype = string_upper(value);
            if (!isValidExclusionType(etype)) {
                bot->Reply(theClient, "Invalid exclusion type. Use: CHAN, NICK, IP, OPER");
                return;
            }
            ex->setExclusionType(etype);
        } else if (field == "value") {
            ex->setValue(value);
        } else {
            bot->Reply(theClient,
                "Unknown field '%s'. Valid: exclusion_type, value", field.c_str());
            return;
        }

        ex->setModifiedTs(::time(0));
        ex->setModifiedBy(0);
        if (!ex->commit()) {
            bot->Reply(theClient, "Failed to update exclusion %d.", id);
            return;
        }
        bot->Reply(theClient, "Exclusion %d: %s set to '%s'.", id, field.c_str(), value.c_str());
        return;
    }

    bot->Reply(theClient, "Unknown verb '%s'. Use ADD, DEL, LIST, SET.", st[2].c_str());
}

// ---------------------------------------------------------------------------
// SPYCLIENT subcommands
// ---------------------------------------------------------------------------
static void handleSpyClient(dronescan* bot, const iClient* theClient,
                             const sqlUser* theUser, const StringTokenizer& st)
{
    if (st.size() < 3) {
        bot->Reply(theClient,
            "Usage: SPAM SPYCLIENT <ADD|DEL|LIST|SHOW|SET|ENABLE|DISABLE> ...");
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
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM SPYCLIENT SHOW <id|nick>"); return; }

        sqlSpyClient* sc = findSpyClientByIdOrNick(bot, st[3]);
        if (!sc) {
            bot->Reply(theClient, "Spy client '%s' not found.", st[3].c_str());
            return;
        }
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
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM SPYCLIENT DEL <id|nick>"); return; }

        sqlSpyClient* sc = findSpyClientByIdOrNick(bot, st[3]);
        if (!sc) {
            bot->Reply(theClient, "Spy client '%s' not found.", st[3].c_str());
            return;
        }
        const int id = sc->getId();
        const string nick = sc->getNickname();
        // Detach from network before removing from DB
        bot->detachSpyClient(id);
        if (!sc->remove()) {
            bot->Reply(theClient, "Failed to delete spy client '%s'.", nick.c_str());
            return;
        }
        delete sc;
        bot->spyClientsMap.erase(id);
        bot->Reply(theClient, "Spy client '%s' deleted.", nick.c_str());
        return;
    }

    // -- ENABLE / DISABLE ----------------------------------------------------
    if (verb == "ENABLE" || verb == "DISABLE") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 4) {
            bot->Reply(theClient, "Usage: SPAM SPYCLIENT %s <id|nick>", verb.c_str());
            return;
        }
        sqlSpyClient* sc = findSpyClientByIdOrNick(bot, st[3]);
        if (!sc) {
            bot->Reply(theClient, "Spy client '%s' not found.", st[3].c_str());
            return;
        }
        sc->setEnabled(verb == "ENABLE");
        sc->setModifiedTs(::time(0));
        sc->setModifiedBy(0);
        if (!sc->commit()) {
            bot->Reply(theClient, "Failed to update spy client '%s'.", sc->getNickname().c_str());
            return;
        }
        // Apply live change
        if (verb == "ENABLE")
            bot->introduceSpyClient(sc);
        else
            bot->detachSpyClient(sc->getId());
        bot->Reply(theClient, "Spy client '%s' %s.",
                   sc->getNickname().c_str(), verb == "ENABLE" ? "enabled" : "disabled");
        return;
    }

    // -- SET -----------------------------------------------------------------
    if (verb == "SET") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM SPYCLIENT SET <id|nick> <field> <value>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM SPYCLIENT SET <id|nick> <field> <value>");
            return;
        }
        sqlSpyClient* sc = findSpyClientByIdOrNick(bot, st[3]);
        if (!sc) {
            bot->Reply(theClient, "Spy client '%s' not found.", st[3].c_str());
            return;
        }
        const int id = sc->getId();
        const string field = string_lower(st[4]);
        const string value = (st.size() >= 6) ? st[5] : string();
        bool wasEnabled = sc->isEnabled();

        if (field == "nick") {
            sc->setNickname(value);
        } else if (field == "user") {
            sc->setUsername(value);
        } else if (field == "host") {
            sc->setHostname(value);
        } else if (field == "ip") {
            sc->setIp(value);
        } else if (field == "realname") {
            sc->setRealname(value);
        } else if (field == "account") {
            sc->setAccount(value);
        } else if (field == "modes") {
            sc->setModes(value);
        } else if (field == "enabled") {
            sc->setEnabled(value == "1" || value == "yes" || value == "true");
        } else {
            bot->Reply(theClient,
                "Unknown field '%s'. Valid: nick, user, host, ip, realname, "
                "account, modes, enabled",
                field.c_str());
            return;
        }

        sc->setModifiedTs(::time(0));
        sc->setModifiedBy(0);
        if (!sc->commit()) {
            bot->Reply(theClient, "Failed to update spy client %d.", id);
            return;
        }
        if (field == "enabled" && wasEnabled != sc->isEnabled()) {
            if (sc->isEnabled())
                bot->introduceSpyClient(sc);
            else
                bot->detachSpyClient(id);
        }
        bot->Reply(theClient, "Spy client %d: %s set to '%s'.", id, field.c_str(), value.c_str());
        return;
    }

    bot->Reply(theClient,
        "Unknown verb '%s'. Use ADD, DEL, LIST, SHOW, SET, ENABLE, DISABLE.", st[2].c_str());
}

// ---------------------------------------------------------------------------
// CHAN subcommands
// ---------------------------------------------------------------------------
static void handleChan(dronescan* bot, const iClient* theClient,
                        const sqlUser* theUser, const StringTokenizer& st)
{
    if (st.size() < 3) {
        bot->Reply(theClient,
            "Usage: SPAM CHAN <ADD|DEL|LIST|SHOW|SET|ENABLE|DISABLE|ADDSPY|REMSPY> ...");
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
        bot->Reply(theClient, "%-4s %-32s %-8s %-11s %-7s %-10s %s",
                   "ID", "Channel", "ForceJoin", "JoinAsSvc", "Enabled", "SpyClnts", "LastTrig");
        for (dronescan::monitoredChannelsMapType::const_iterator it =
                 bot->monitoredChannelsMap.begin();
             it != bot->monitoredChannelsMap.end(); ++it) {
            sqlMonitoredChannel* mc = it->second;
            dronescan::monitoredChannelSpyClientsMapType::const_iterator sit =
                bot->monitoredChannelSpyClientsMap.find(mc->getId());
            const string spySummary = (sit != bot->monitoredChannelSpyClientsMap.end() &&
                                        !sit->second.empty())
                                       ? std::to_string(sit->second.size()) : "any";
            const char* lastTrig = (mc->getLastTriggeredTs() > 0)
                                   ? bot->Ago(mc->getLastTriggeredTs()) : "never";
            bot->Reply(theClient, "%-4d %-32s %-8s %-11s %-7s %-10s %s",
                       mc->getId(),
                       mc->getName().c_str(),
                       mc->isForceJoin() ? "yes" : "no",
                       mc->isJoinAsService() ? "yes" : "no",
                       mc->isEnabled() ? "yes" : "no",
                       spySummary.c_str(),
                       lastTrig);
        }
        bot->Reply(theClient, "--- %zu channel(s) ---", bot->monitoredChannelsMap.size());
        return;
    }

    // -- SHOW ----------------------------------------------------------------
    if (verb == "SHOW") {
        if (!checkAccess(theClient, theUser, bot, level::spam_read)) return;
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM CHAN SHOW <#channel>"); return; }

        dronescan::monitoredChannelsMapType::const_iterator it =
            bot->monitoredChannelsMap.find(string_lower(st[3]));
        if (it == bot->monitoredChannelsMap.end()) {
            bot->Reply(theClient, "Channel '%s' is not monitored.", st[3].c_str());
            return;
        }
        sqlMonitoredChannel* found = it->second;
        const string spyList = spyClientNickListForChannel(bot, found->getId());
        bot->Reply(theClient, "ID          : %d", found->getId());
        bot->Reply(theClient, "Channel     : %s", found->getName().c_str());
        bot->Reply(theClient, "ForceJoin   : %s", found->isForceJoin() ? "yes" : "no");
        bot->Reply(theClient, "JoinAsService: %s", found->isJoinAsService() ? "yes" : "no");
        bot->Reply(theClient, "Enabled     : %s", found->isEnabled() ? "yes" : "no");
        bot->Reply(theClient, "SpyClients  : %s", spyList.empty() ? "any" : spyList.c_str());
        if (found->getLastTriggeredTs() > 0)
            bot->Reply(theClient, "LastTriggered: %s ago (rule '%s')",
                       bot->Ago(found->getLastTriggeredTs()),
                       found->getLastTriggeredRule().c_str());
        else
            bot->Reply(theClient, "LastTriggered: never");
        return;
    }

    // -- ADD -----------------------------------------------------------------
    if (verb == "ADD") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM CHAN ADD <#channel> [forcejoin 0|1] [joinasservice 0|1]
        if (st.size() < 4) {
            bot->Reply(theClient,
                "Usage: SPAM CHAN ADD <#channel> [forcejoin 0|1] [joinasservice 0|1]");
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
            if (bot->findBestSpyClient(chanName, mc->isForceJoin()) >= 0)
                bot->scheduleSpyClientJoin(chanName, mc->isForceJoin(), 0, 300);
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
        if (st.size() < 4) { bot->Reply(theClient, "Usage: SPAM CHAN DEL <#channel>"); return; }

        dronescan::monitoredChannelsMapType::iterator found =
            bot->monitoredChannelsMap.find(string_lower(st[3]));
        if (found == bot->monitoredChannelsMap.end()) {
            bot->Reply(theClient, "Channel '%s' is not monitored.", st[3].c_str());
            return;
        }
        const string chanName = found->second->getName();
        // Part any spy client currently in this channel, and cancel any
        // spy-client join that was scheduled but hasn't fired yet
        {
            const string chanKey = found->first;
            dronescan::chanActiveSpyMapType::iterator sit = bot->chanActiveSpyMap.find(chanKey);
            if (sit != bot->chanActiveSpyMap.end())
                bot->partSpyClientFromChannel(sit->second, chanKey);
            bot->cancelPendingJoinTimers(chanKey);
        }
        if (!found->second->remove()) {
            bot->Reply(theClient, "Failed to delete monitored channel '%s'.", chanName.c_str());
            return;
        }
        delete found->second;
        bot->monitoredChannelsMap.erase(found);
        bot->Reply(theClient, "Monitored channel '%s' deleted.", chanName.c_str());
        return;
    }

    // -- ENABLE / DISABLE ----------------------------------------------------
    if (verb == "ENABLE" || verb == "DISABLE") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        if (st.size() < 4) {
            bot->Reply(theClient, "Usage: SPAM CHAN %s <#channel>", verb.c_str());
            return;
        }
        dronescan::monitoredChannelsMapType::iterator it =
            bot->monitoredChannelsMap.find(string_lower(st[3]));
        if (it == bot->monitoredChannelsMap.end()) {
            bot->Reply(theClient, "Channel '%s' is not monitored.", st[3].c_str());
            return;
        }
        sqlMonitoredChannel* mc = it->second;
        mc->setEnabled(verb == "ENABLE");
        mc->setModifiedTs(::time(0));
        mc->setModifiedBy(0);
        if (!mc->commit()) {
            bot->Reply(theClient, "Failed to update monitored channel '%s'.", mc->getName().c_str());
            return;
        }
        const string chanKey = string_lower(mc->getName());
        if (verb == "DISABLE") {
            // Part any active spy client from this channel, and cancel any
            // spy-client join that was scheduled but hasn't fired yet
            dronescan::chanActiveSpyMapType::iterator sit = bot->chanActiveSpyMap.find(chanKey);
            if (sit != bot->chanActiveSpyMap.end())
                bot->partSpyClientFromChannel(sit->second, chanKey);
            bot->cancelPendingJoinTimers(chanKey);
        } else {
            // Enable: schedule a spy client join
            if (!mc->isJoinAsService()) {
                if (bot->findBestSpyClient(mc->getName(), mc->isForceJoin()) >= 0)
                    bot->scheduleSpyClientJoin(mc->getName(), mc->isForceJoin(), 0, 300);
                else
                    bot->Reply(theClient, "Warning: no available spy client for %s.",
                               mc->getName().c_str());
            } else {
                bot->getUplink()->JoinChannel(bot, mc->getName(), "");
            }
        }
        bot->Reply(theClient, "Monitored channel '%s' %s.",
                   mc->getName().c_str(), verb == "ENABLE" ? "enabled" : "disabled");
        return;
    }

    // -- SET -----------------------------------------------------------------
    if (verb == "SET") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM CHAN SET <#channel> <field> <value>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM CHAN SET <#channel> <field> <value>");
            return;
        }
        dronescan::monitoredChannelsMapType::iterator it =
            bot->monitoredChannelsMap.find(string_lower(st[3]));
        if (it == bot->monitoredChannelsMap.end()) {
            bot->Reply(theClient, "Channel '%s' is not monitored.", st[3].c_str());
            return;
        }
        sqlMonitoredChannel* mc = it->second;
        const string field = string_lower(st[4]);
        const string value = (st.size() >= 6) ? st[5] : string();
        const bool wasEnabled = mc->isEnabled();

        if (field == "forcejoin") {
            mc->setForceJoin(value == "1" || value == "yes" || value == "true");
        } else if (field == "joinasservice") {
            mc->setJoinAsService(value == "1" || value == "yes" || value == "true");
        } else if (field == "enabled") {
            mc->setEnabled(value == "1" || value == "yes" || value == "true");
        } else {
            bot->Reply(theClient,
                "Unknown field '%s'. Valid: forcejoin, joinasservice, enabled",
                field.c_str());
            return;
        }

        mc->setModifiedTs(::time(0));
        mc->setModifiedBy(0);
        if (!mc->commit()) {
            bot->Reply(theClient, "Failed to update monitored channel '%s'.", mc->getName().c_str());
            return;
        }
        if (field == "enabled" && wasEnabled != mc->isEnabled()) {
            const string chanKey = string_lower(mc->getName());
            if (!mc->isEnabled()) {
                dronescan::chanActiveSpyMapType::iterator sit = bot->chanActiveSpyMap.find(chanKey);
                if (sit != bot->chanActiveSpyMap.end())
                    bot->partSpyClientFromChannel(sit->second, chanKey);
                bot->cancelPendingJoinTimers(chanKey);
            } else {
                if (!mc->isJoinAsService()) {
                    if (bot->findBestSpyClient(mc->getName(), mc->isForceJoin()) >= 0)
                        bot->scheduleSpyClientJoin(mc->getName(), mc->isForceJoin(), 0, 300);
                    else
                        bot->Reply(theClient, "Warning: no available spy client for %s.",
                                   mc->getName().c_str());
                } else {
                    bot->getUplink()->JoinChannel(bot, mc->getName(), "");
                }
            }
        }
        bot->Reply(theClient, "Monitored channel '%s': %s set to '%s'.",
                   mc->getName().c_str(), field.c_str(), value.c_str());
        return;
    }

    // -- ADDSPY ----------------------------------------------------------
    if (verb == "ADDSPY") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM CHAN ADDSPY <#channel> <nick>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM CHAN ADDSPY <#channel> <nick>");
            return;
        }
        const string chanName = string_lower(st[3]);
        dronescan::monitoredChannelsMapType::const_iterator mcit =
            bot->monitoredChannelsMap.find(chanName);
        if (mcit == bot->monitoredChannelsMap.end()) {
            bot->Reply(theClient, "Channel '%s' is not monitored.", chanName.c_str());
            return;
        }
        sqlMonitoredChannel* mc = mcit->second;

        sqlSpyClient* sc = findSpyClientByNick(bot, st[4]);
        if (!sc) {
            bot->Reply(theClient,
                "Spy client '%s' not found. Use SPAM SPYCLIENT LIST to see valid nicks.",
                st[4].c_str());
            return;
        }

        std::vector<int>& allowed = bot->monitoredChannelSpyClientsMap[mc->getId()];
        for (size_t i = 0; i < allowed.size(); ++i) {
            if (allowed[i] == sc->getId()) {
                bot->Reply(theClient, "Spy client '%s' is already allowed on '%s'.",
                           sc->getNickname().c_str(), mc->getName().c_str());
                return;
            }
        }

        stringstream iq;
        iq << "INSERT INTO monitored_channel_spyclients (channel_id, spyclient_id) VALUES ("
           << mc->getId() << ", " << sc->getId() << ")";
        if (!bot->getSqlDb()->Exec(iq)) {
            bot->Reply(theClient, "Failed to add spy client '%s' to '%s' (duplicate?).",
                       sc->getNickname().c_str(), mc->getName().c_str());
            return;
        }
        allowed.push_back(sc->getId());
        bot->Reply(theClient, "Spy client '%s' added to '%s'.",
                   sc->getNickname().c_str(), mc->getName().c_str());
        return;
    }

    // -- REMSPY ------------------------------------------------------------
    if (verb == "REMSPY") {
        if (!checkAccess(theClient, theUser, bot, level::spam_write)) return;
        // SPAM CHAN REMSPY <#channel> <nick>
        if (st.size() < 5) {
            bot->Reply(theClient, "Usage: SPAM CHAN REMSPY <#channel> <nick>");
            return;
        }
        const string chanName = string_lower(st[3]);
        dronescan::monitoredChannelsMapType::const_iterator mcit =
            bot->monitoredChannelsMap.find(chanName);
        if (mcit == bot->monitoredChannelsMap.end()) {
            bot->Reply(theClient, "Channel '%s' is not monitored.", chanName.c_str());
            return;
        }
        sqlMonitoredChannel* mc = mcit->second;

        sqlSpyClient* sc = findSpyClientByNick(bot, st[4]);
        if (!sc) {
            bot->Reply(theClient, "Spy client '%s' not found.", st[4].c_str());
            return;
        }

        stringstream dq;
        dq << "DELETE FROM monitored_channel_spyclients WHERE channel_id = " << mc->getId()
           << " AND spyclient_id = " << sc->getId();
        if (!bot->getSqlDb()->Exec(dq)) {
            bot->Reply(theClient, "Failed to remove spy client '%s' from '%s'.",
                       sc->getNickname().c_str(), mc->getName().c_str());
            return;
        }

        bool found = false;
        dronescan::monitoredChannelSpyClientsMapType::iterator ait =
            bot->monitoredChannelSpyClientsMap.find(mc->getId());
        if (ait != bot->monitoredChannelSpyClientsMap.end()) {
            std::vector<int>& vec = ait->second;
            for (std::vector<int>::iterator vi = vec.begin(); vi != vec.end(); ++vi) {
                if (*vi == sc->getId()) {
                    vec.erase(vi);
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            bot->Reply(theClient,
                "Spy client '%s' was not in the allowed list for '%s'.",
                sc->getNickname().c_str(), mc->getName().c_str());
            return;
        }
        bot->Reply(theClient, "Spy client '%s' removed from '%s'.",
                   sc->getNickname().c_str(), mc->getName().c_str());
        return;
    }

    bot->Reply(theClient,
        "Unknown verb '%s'. Use ADD, DEL, LIST, SHOW, SET, ENABLE, DISABLE, "
        "ADDSPY, REMSPY.", st[2].c_str());
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
    if (obj == "CHAN")        { handleChan(bot, theClient, theUser, st);        return; }

    bot->Reply(theClient,
        "Usage: SPAM <EVENT|RULE|ACTION|EXCLUSION|SPYCLIENT|CHAN> <verb> ...");
}

} // namespace ds
} // namespace gnuworld
