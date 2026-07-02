/**
 * HELPCommand.cc
 *
 * Displays help information for all available commands in dronescan.
 * Users can request help on all commands or a specific command.
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

#include <string>

#include "StringTokenizer.h"
#include "dronescan.h"
#include "dronescanCommands.h"

namespace gnuworld {

namespace ds {

using std::string;

// ---------------------------------------------------------------------------
// Per-command short descriptions
// ---------------------------------------------------------------------------
static string getCommandDescription(const string& cmdName)
{
    if (cmdName == "ACCESS")               return "View access level of yourself or another user";
    if (cmdName == "ADDUSER")              return "Add a new user with a given access level";
    if (cmdName == "ADDEXCEPTIONALCHANNEL") return "Exempt a channel from drone detection";
    if (cmdName == "ANALYSE")             return "Run a detailed analysis on a channel";
    if (cmdName == "CHECK")               return "Check a channel or user against active tests";
    if (cmdName == "FAKE")                return "Manage fake clients used for detection testing";
    if (cmdName == "HELLO")               return "Authenticate yourself with an account name";
    if (cmdName == "HELP")                return "Show help for all or a specific command";
    if (cmdName == "LIST")                return "List active channels, fake clients, floods, or users";
    if (cmdName == "MODUSER")             return "Modify a user attribute (e.g. access level)";
    if (cmdName == "QUOTE")               return "Send a raw IRC line to the server";
    if (cmdName == "REMEXCEPTIONALCHANNEL") return "Remove a channel from the exemption list";
    if (cmdName == "REMUSER")             return "Remove a user from the system";
    if (cmdName == "RELOAD")              return "Reload configuration and reinitialise the module";
    if (cmdName == "STATUS")              return "Show dronescan uptime, test states, and queue sizes";
    if (cmdName == "SPAM")                return "Manage spam detection events, rules, actions, and exclusions";
    return "";
}

// ---------------------------------------------------------------------------
// Detailed help for individual SPAM sub-objects
// ---------------------------------------------------------------------------
static void helpSpamEvent(dronescan* bot, const iClient* theClient)
{
    bot->Reply(theClient, "SPAM EVENT subcommands:");
    bot->Reply(theClient, "  ADD    <name> <type> <target> <param> <points> <expiry> [max_occ]");
    bot->Reply(theClient, "  DEL    <id>");
    bot->Reply(theClient, "  LIST");
    bot->Reply(theClient, "  SHOW   <id>");
    bot->Reply(theClient, "  SET    <id> <field> <value>");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Event types: TEXT          TEXT_REPEAT");
    bot->Reply(theClient, "             ENTROPY_TEXT  ENTROPY_NICK  JOIN_CHANNEL");
    bot->Reply(theClient, "             USERMODE  KICK_MSG  KICK_COUNT");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "TEXT: event_param is a PCRE2 regex applied to incoming text.");
    bot->Reply(theClient, "  Use the target bitmask to select which text sources to match:");
    bot->Reply(theClient, "  chan_priv=channel PRIVMSGs  chan_not=channel NOTICEs");
    bot->Reply(theClient, "  privmsg=DMs to bot/spy      notice=NOTICEs to bot/spy");
    bot->Reply(theClient, "  part=part reasons           quit=quit msgs (not yet active)");
    bot->Reply(theClient, "  ctcp=CTCP (direct or channel) to bot/spy, e.g. DCC/ACTION - match");
    bot->Reply(theClient, "    DCC specifically with a regex like \"^DCC\" scoped to ctcp");
    bot->Reply(theClient, "  \"chan\" is an alias for chan_priv+chan_not.");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Target (comma-separated or \"all\"): chan_priv chan_not chan privmsg notice part quit ctcp");
    bot->Reply(theClient, "  Bitmask: chan_priv=1 privmsg=2 chan_not=4 part=8 quit=16 notice=32 ctcp=64 all=127");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "SET fields: description  event_param  target  case_sensitive");
    bot->Reply(theClient, "            points  point_expiry  max_occurrence  requires_event_id");
    bot->Reply(theClient, "            enabled  repeat_crossuser");
    bot->Reply(theClient, "            repeat_min_count  repeat_exclusion_regex");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Examples:");
    bot->Reply(theClient, "  SPAM EVENT ADD pill_regex TEXT chan,privmsg \"buy.*cheap.*pills\" 10 60");
    bot->Reply(theClient, "  SPAM EVENT ADD chan_notice_spam TEXT chan_not \"spam.*notice\" 5 60");
    bot->Reply(theClient, "  SPAM EVENT ADD part_flood TEXT part \"bye|cya|leaving\" 5 60");
    bot->Reply(theClient, "  SPAM EVENT ADD direct_spam TEXT privmsg,notice \"click.*here\" 10 60");
    bot->Reply(theClient, "  SPAM EVENT ADD repeat_flood TEXT_REPEAT all . 5 30 3");
    bot->Reply(theClient, "  SPAM EVENT SET 1 enabled yes");
    bot->Reply(theClient, "  SPAM EVENT SET 1 event_param \"new.*regex.*pattern\"");
    bot->Reply(theClient, "  SPAM EVENT SET 1 target all");
    bot->Reply(theClient, "  SPAM EVENT DEL 1");
}

static void helpSpamRule(dronescan* bot, const iClient* theClient)
{
    bot->Reply(theClient, "SPAM RULE subcommands:");
    bot->Reply(theClient, "  ADD        <name> <threshold>");
    bot->Reply(theClient, "  DEL        <id>");
    bot->Reply(theClient, "  LIST");
    bot->Reply(theClient, "  SHOW       <id>");
    bot->Reply(theClient, "  SET        <id> <field> <value>");
    bot->Reply(theClient, "  ADDEVENT   <rule_id> <event_id> [points_override]");
    bot->Reply(theClient, "  REMEVENT   <rule_id> <event_id>");
    bot->Reply(theClient, "  ADDACTION  <rule_id> <action_id> [dur_override] [reason_override] [delay_override]");
    bot->Reply(theClient, "  REMACTION  <spam_rule_action_id>");
    bot->Reply(theClient, "  ADDCHAN    <rule_id> <#channel>");
    bot->Reply(theClient, "  REMCHAN    <rule_id> <#channel>");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "A rule fires its actions when the total points from linked events");
    bot->Reply(theClient, "reach or exceed the threshold within the event expiry windows.");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "SET fields: description  threshold  wait_on_rule_id  enabled");
    bot->Reply(theClient, "            points_per  score_globally  allchans");
    bot->Reply(theClient, "  allchans=1 : rule applies to every channel (use ADDCHAN to exclude).");
    bot->Reply(theClient, "  allchans=0 : rule applies only to channels added with ADDCHAN.");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Examples:");
    bot->Reply(theClient, "  SPAM RULE ADD anti_spam_global 50");
    bot->Reply(theClient, "  SPAM RULE SET 1 allchans yes");
    bot->Reply(theClient, "  SPAM RULE SET 1 threshold 30");
    bot->Reply(theClient, "  SPAM RULE ADDEVENT 1 2");
    bot->Reply(theClient, "  SPAM RULE ADDEVENT 1 3 20     <- use 20 pts instead of event default");
    bot->Reply(theClient, "  SPAM RULE ADDACTION 1 1 3600 \"Spam detected\" 0");
    bot->Reply(theClient, "  SPAM RULE ADDCHAN  1 #watch-me");
    bot->Reply(theClient, "  SPAM RULE REMCHAN  1 #watch-me");
}

static void helpSpamAction(dronescan* bot, const iClient* theClient)
{
    bot->Reply(theClient, "SPAM ACTION subcommands:");
    bot->Reply(theClient, "  ADD    <name> <type> [duration] [reason] [delay]");
    bot->Reply(theClient, "  DEL    <id>");
    bot->Reply(theClient, "  LIST");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Action types: GLINE  KILL  REPORT");
    bot->Reply(theClient, "  duration : seconds for GLINE (ignored for KILL/REPORT)");
    bot->Reply(theClient, "  reason   : gline/kill reason string");
    bot->Reply(theClient, "  delay    : seconds to wait before firing (0 = immediate)");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Examples:");
    bot->Reply(theClient, "  SPAM ACTION ADD gline_1h   GLINE  3600 \"Spam detected\"  0");
    bot->Reply(theClient, "  SPAM ACTION ADD kill_now   KILL   0    \"Spam\"            0");
    bot->Reply(theClient, "  SPAM ACTION ADD report_only REPORT");
    bot->Reply(theClient, "  SPAM ACTION DEL 3");
}

static void helpSpamExclusion(dronescan* bot, const iClient* theClient)
{
    bot->Reply(theClient, "SPAM EXCLUSION subcommands:");
    bot->Reply(theClient, "  ADD    <CHAN|NICK|IP|OPER> <value>");
    bot->Reply(theClient, "  DEL    <id>");
    bot->Reply(theClient, "  LIST");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Exclusion types:");
    bot->Reply(theClient, "  CHAN  - exempt a channel name from spam scanning");
    bot->Reply(theClient, "  NICK - exempt a nickname pattern from spam scoring");
    bot->Reply(theClient, "  IP   - exempt a host/IP from spam scoring");
    bot->Reply(theClient, "  OPER - exempt an oper account from spam scoring");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Examples:");
    bot->Reply(theClient, "  SPAM EXCLUSION ADD CHAN  #trusted-channel");
    bot->Reply(theClient, "  SPAM EXCLUSION ADD OPER  ServiceBot");
    bot->Reply(theClient, "  SPAM EXCLUSION ADD IP    192.168.1.0/24");
    bot->Reply(theClient, "  SPAM EXCLUSION DEL 2");
}

static void helpSpamSpyClient(dronescan* bot, const iClient* theClient)
{
    bot->Reply(theClient, "SPAM SPYCLIENT subcommands:");
    bot->Reply(theClient, "  ADD     <nick> <user> <host> <ip> <realname> [account] [modes]");
    bot->Reply(theClient, "  DEL     <id>");
    bot->Reply(theClient, "  LIST");
    bot->Reply(theClient, "  SHOW    <id>");
    bot->Reply(theClient, "  ENABLE  <id>");
    bot->Reply(theClient, "  DISABLE <id>");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Spy clients join monitored channels to observe traffic for spam detection.");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Examples:");
    bot->Reply(theClient, "  SPAM SPYCLIENT ADD SpyBot spy spy.host.com 1.2.3.4 \"Observer\"");
    bot->Reply(theClient, "  SPAM SPYCLIENT ENABLE  1");
    bot->Reply(theClient, "  SPAM SPYCLIENT DISABLE 1");
    bot->Reply(theClient, "  SPAM SPYCLIENT DEL     1");
}

static void helpSpamMonitorChan(dronescan* bot, const iClient* theClient)
{
    bot->Reply(theClient, "SPAM MONITORCHAN subcommands:");
    bot->Reply(theClient, "  ADD     <#channel> [forcejoin 0|1] [joinasservice 0|1]");
    bot->Reply(theClient, "  DEL     <id>");
    bot->Reply(theClient, "  LIST");
    bot->Reply(theClient, "  SHOW    <id>");
    bot->Reply(theClient, "  ENABLE  <id>");
    bot->Reply(theClient, "  DISABLE <id>");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "  forcejoin=1    : force-join the channel even if not invited");
    bot->Reply(theClient, "  joinasservice=1: join using service mode (+)");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Examples:");
    bot->Reply(theClient, "  SPAM MONITORCHAN ADD #watch-this 1 0");
    bot->Reply(theClient, "  SPAM MONITORCHAN ENABLE  2");
    bot->Reply(theClient, "  SPAM MONITORCHAN DISABLE 2");
    bot->Reply(theClient, "  SPAM MONITORCHAN DEL     2");
}

// ---------------------------------------------------------------------------
// Full SPAM overview (HELP SPAM)
// ---------------------------------------------------------------------------
static void helpSpam(dronescan* bot, const iClient* theClient)
{
    bot->Reply(theClient, "SPAM - Spam detection management");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Top-level syntax: SPAM <object> <verb> [args]");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Objects:");
    bot->Reply(theClient, "  EVENT      - Detection events (what to match and score)");
    bot->Reply(theClient, "  RULE       - Rules that combine events and trigger actions");
    bot->Reply(theClient, "  ACTION     - Actions executed when a rule threshold is reached");
    bot->Reply(theClient, "  EXCLUSION  - Channels/nicks/IPs/opers exempt from scanning");
    bot->Reply(theClient, "  SPYCLIENT  - Fake clients that observe channels");
    bot->Reply(theClient, "  MONITORCHAN- Channels to monitor for spam");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Workflow overview:");
    bot->Reply(theClient, "  1. Create EVENT(s) describing what triggers scoring");
    bot->Reply(theClient, "  2. Create ACTION(s) describing what happens on a hit");
    bot->Reply(theClient, "  3. Create a RULE with a point threshold");
    bot->Reply(theClient, "  4. Link events to the rule (SPAM RULE ADDEVENT)");
    bot->Reply(theClient, "  5. Link actions to the rule (SPAM RULE ADDACTION)");
    bot->Reply(theClient, "  6. Optionally restrict the rule to specific channels (SPAM RULE ADDCHAN)");
    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Type HELP SPAM <object> for details on each section.");
    bot->Reply(theClient, "  e.g.  HELP SPAM EVENT");
    bot->Reply(theClient, "        HELP SPAM RULE");
    bot->Reply(theClient, "        HELP SPAM ACTION");
    bot->Reply(theClient, "        HELP SPAM EXCLUSION");
    bot->Reply(theClient, "        HELP SPAM SPYCLIENT");
    bot->Reply(theClient, "        HELP SPAM MONITORCHAN");
}

// ---------------------------------------------------------------------------
// Main entry point
// ---------------------------------------------------------------------------
void HELPCommand::Exec(const iClient* theClient, const string& Message, const sqlUser*)
{
    StringTokenizer st(Message);

    // HELP [command [subobject]]

    // ---- HELP SPAM <sub> ---------------------------------------------------
    if (st.size() >= 3 && string_upper(st[1]) == "SPAM") {
        const string sub = string_upper(st[2]);
        if      (sub == "EVENT")       helpSpamEvent(bot, theClient);
        else if (sub == "RULE")        helpSpamRule(bot, theClient);
        else if (sub == "ACTION")      helpSpamAction(bot, theClient);
        else if (sub == "EXCLUSION")   helpSpamExclusion(bot, theClient);
        else if (sub == "SPYCLIENT")   helpSpamSpyClient(bot, theClient);
        else if (sub == "MONITORCHAN") helpSpamMonitorChan(bot, theClient);
        else {
            bot->Reply(theClient,
                "Unknown SPAM section '%s'. Use: EVENT RULE ACTION EXCLUSION SPYCLIENT MONITORCHAN",
                st[2].c_str());
        }
        return;
    }

    // ---- HELP <command> ----------------------------------------------------
    if (st.size() == 2) {
        const string targetCmd = string_upper(st[1]);

        // Special top-level SPAM help
        if (targetCmd == "SPAM") {
            helpSpam(bot, theClient);
            return;
        }

        dronescan::commandMapType::const_iterator it = bot->commandMap.find(targetCmd);
        if (it == bot->commandMap.end()) {
            bot->Reply(theClient, "Unknown command '%s'. Type HELP for a list.",
                       targetCmd.c_str());
            return;
        }

        Command* cmd = it->second;
        const string& cmdHelp = cmd->getHelp();
        const string description = getCommandDescription(targetCmd);

        bot->Reply(theClient, "Command    : %s", targetCmd.c_str());
        bot->Reply(theClient, "Syntax     : %s %s", targetCmd.c_str(),
                   cmdHelp.empty() ? "" : cmdHelp.c_str());
        if (!description.empty())
            bot->Reply(theClient, "Description: %s", description.c_str());

        // Extra help per command
        if (targetCmd == "LIST") {
            bot->Reply(theClient, " ");
            bot->Reply(theClient, "Subcommands:");
            bot->Reply(theClient, "  LIST active      - Show channels currently flagged as active");
            bot->Reply(theClient, "  LIST fakeclients - Show all registered fake clients");
            bot->Reply(theClient, "  LIST joinflood   - Show channels with active join flood data");
            bot->Reply(theClient, "  LIST users       - Show all users in the system");
        } else if (targetCmd == "ACCESS") {
            bot->Reply(theClient, " ");
            bot->Reply(theClient, "Examples:");
            bot->Reply(theClient, "  ACCESS           - Show your own access info");
            bot->Reply(theClient, "  ACCESS SomeUser  - Show access info for SomeUser");
        } else if (targetCmd == "MODUSER") {
            bot->Reply(theClient, " ");
            bot->Reply(theClient, "Examples:");
            bot->Reply(theClient, "  MODUSER ACCESS SomeUser 200");
        } else if (targetCmd == "CHECK") {
            bot->Reply(theClient, " ");
            bot->Reply(theClient, "Examples:");
            bot->Reply(theClient, "  CHECK #drones");
            bot->Reply(theClient, "  CHECK SomeNick");
        }
        return;
    }

    // ---- HELP (no args) ? list all commands --------------------------------
    bot->Reply(theClient, "Available commands:");
    bot->Reply(theClient, " ");

    for (dronescan::commandMapType::const_iterator it = bot->commandMap.begin();
         it != bot->commandMap.end(); ++it) {
        Command* cmd = it->second;
        const string& name = cmd->getName();
        const string& usage = cmd->getHelp();
        const string  desc  = getCommandDescription(name);

        if (!usage.empty())
            bot->Reply(theClient, "  %-30s %s",
                       (name + " " + usage).c_str(),
                       desc.empty() ? "" : ("- " + desc).c_str());
        else
            bot->Reply(theClient, "  %-30s %s",
                       name.c_str(),
                       desc.empty() ? "" : ("- " + desc).c_str());
    }

    bot->Reply(theClient, " ");
    bot->Reply(theClient, "Type HELP <command> for more details.  For SPAM: HELP SPAM [section]");

} // HELPCommand::Exec

} // namespace ds

} // namespace gnuworld
