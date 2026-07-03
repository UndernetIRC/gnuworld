/**
 * dronescan.cc
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
#include <vector>
#include <sstream>
#include <iostream>
#include <list>
#include <cstdarg> /* va_list */
#include <cstdio>  /* *printf() */
#include <cstdlib> /* rand(), srand() */
#include <ctime>   /* time() */
#include "dbHandle.h"

#include "gnuworld_config.h"
#include "EConfig.h"
#include "Network.h"
#include "server.h"
#include "StringTokenizer.h"

#include "activeChannel.h"
#include "clientData.h"
#include "constants.h"
#include "dronescan.h"
#include "dronescanCommands.h"
#include "dronescanTests.h"
#include "sqlFakeClient.h"
#include "sqlUser.h"
#include "sqlSpyClient.h"
#include "sqlMonitoredChannel.h"
#include "Timer.h"
#include "ip.h"
#include "constants.h"

#ifdef ENABLE_LOG4CPLUS
#include <log4cplus/logger.h>
#endif

namespace gnuworld {

namespace ds {

using std::endl;
using std::string;

/*
 * Exported function to be used by moduleLoader to gain an
 * instance of this module.
 */
extern "C" {
xClient* _gnuwinit(const string& args) { return new dronescan(args); }
}

/***************************
 ** C O N S T R U C T O R **
 ***************************/

/**
 * This constructor calls the base class constructor.  The xClient
 * constructor will open the configuration file given and retrieve
 * basic client info (nick/user/host/etc).
 * Any additional processing must be done here.
 */
dronescan::dronescan(const string& configFileName) : xClient(configFileName) {
    /* Load the config file */
    dronescanConfig = new (std::nothrow) EConfig(configFileName);
    assert(dronescanConfig != 0);

    /* Do config file processing here */
    consoleChannel = dronescanConfig->Require("consoleChannel")->second;

    /* Set our initial state */
    // elog << "dronescan.start> getUplink()->getStartTime()) = " << getUplink()->getStartTime() <<
    // endl;
    int i = 0;
    for (xNetwork::serverIterator Itr = Network->servers_begin(); Itr != Network->servers_end();
         Itr++) {
        i++;
    }
    elog << "droneScan.start> i = " << i << endl;
    if (i == 1) // if i == 1, it means i'm not connectd to a hub. If i > 1, it means the RELOAD
                // command was sent
        currentState = BURST;
    averageEntropy = 0;
    totalNicks = 0;

    /* What is the voting cutoff? */
    voteCutoff = atoi(dronescanConfig->Require("voteCutoff")->second.c_str());

    /*
     * Set up initial margins.
     * These, particularly nickMargin,  will require tweaking to make them work well.
     * It is probably wise to not run these too close to false positive since
     * the average entropy and such will vary over time. Two decimal places is
     * probably the furthest I would push it for now.
     */
    channelMargin = atof(dronescanConfig->Require("channelMargin")->second.c_str());
    nickMargin = atof(dronescanConfig->Require("nickMargin")->second.c_str());
    channelCutoff = atoi(dronescanConfig->Require("channelCutoff")->second.c_str());

    RegisterTest(
        new ABNORMALSTest(this, "ABNORMALS", "Checks for the percentage of normal clients.", 10));
    RegisterTest(new COMMONREALTest(this, "COMMONREAL", "Checks for common realnames", 10));
    RegisterTest(new HASALLOPTest(this, "HASALLOP", "Checks if a channel has all ops.", 10));
    RegisterTest(new HASOPTest(this, "HASOP", "Checks if a channel has no ops.", 10));
    RegisterTest(
        new MAXCHANSTest(this, "MAXCHANS", "Checks the max channel membership of a channel.", 10));
    RegisterTest(new RANGETest(this, "RANGE", "Checks the entropy range.", 10));

    /* Set up core options */
    dcInterval = atoi(dronescanConfig->Require("dcInterval")->second.c_str());
    consoleLevel = atoi(dronescanConfig->Require("consoleLevel")->second.c_str());
    jcInterval = atoi(dronescanConfig->Require("jcInterval")->second.c_str());
    jcCutoff = atoi(dronescanConfig->Require("jcCutoff")->second.c_str());
    // pcCutoff = atoi(dronescanConfig->Require("pcCutoff")->second.c_str());
    ncInterval = atoi(dronescanConfig->Require("ncInterval")->second.c_str());
    ncCutoff = atoi(dronescanConfig->Require("ncCutoff")->second.c_str());
    jcMinJoinToGline = atoi(dronescanConfig->Require("jcMinJoinToGline")->second.c_str());
    jcGracePeriodBurstOrSplit =
        atoi(dronescanConfig->Require("jcGracePeriodBurstOrSplit")->second.c_str());
    jcMinJoinToGlineJOnly = atoi(dronescanConfig->Require("jcMinJoinToGlineJOnly")->second.c_str());
    jcMinJoinsPerIPToGline =
        atoi(dronescanConfig->Require("jcMinJoinsPerIPToGline")->second.c_str());
    jcJoinsPerIPTime = atoi(dronescanConfig->Require("jcJoinsPerIPTime")->second.c_str());
    jcMinJFSizeToGline = atoi(dronescanConfig->Require("jcMinJFSizeToGline")->second.c_str());
    jcMinJFJOnlySizeToGline =
        atoi(dronescanConfig->Require("jcMinJFJOnlySizeToGline")->second.c_str());
    jcIgnoreJoinFloodLag = atoi(dronescanConfig->Require("jcIgnoreJoinFloodLag")->second.c_str());
    jcIgnoreJoinFloodLagTS =
        atoi(dronescanConfig->Require("jcIgnoreJoinFloodLagTS")->second.c_str());
    jcGlineEnable =
        atoi(dronescanConfig->Require("jcGlineEnable")->second.c_str()) == 1 ? true : false;
    jcGlineEnableConf = jcGlineEnable;
    jcGlineReason = dronescanConfig->Require("jcGlineReason")->second.c_str();
    jcGlineLength = atoi(dronescanConfig->Require("jcGlineLength")->second.c_str());
    gbCount = atoi(dronescanConfig->Require("gbCount")->second.c_str());
    gbInterval = atoi(dronescanConfig->Require("gbInterval")->second.c_str());
    /* Set up variables that our tests will need */
    typedef std::vector<string> testVarsType;
    testVarsType testVars;

    /* Cycle over tests to get configuration variable name */
    for (testMapType::const_iterator itr = testMap.begin(); itr != testMap.end(); ++itr) {
        testVars.push_back(itr->second->getVariable());
    }

    for (testVarsType::const_iterator itr = testVars.begin(); itr != testVars.end(); ++itr) {
        if (*itr == "")
            continue;

        string theValue = dronescanConfig->Require(*itr)->second;

        if (Test* theTest = setTestVariable(string_upper(*itr), theValue)) {
            elog << "dronescan> Test " << theTest->getName() << " accepted parameter " << *itr
                 << " @ " << theValue << std::endl;
        } else {
            elog << "dronescan> No test accepted variable: " << *itr << " @ " << theValue
                 << std::endl;
            ::exit(1);
        }
    }

    /* Set up database */
    string sqlHost(dronescanConfig->Require("sqlHost")->second);
    string sqlPort(dronescanConfig->Require("sqlPort")->second);
    string sqlDB(dronescanConfig->Require("sqlDB")->second);
    string sqlUsername(dronescanConfig->Require("sqlUser")->second);
    string sqlPass(dronescanConfig->Require("sqlPass")->second);

    std::stringstream connectString;
    connectString << "host=" << sqlHost << " "
                  << "port=" << sqlPort << " "
                  << "dbname=" << sqlDB << " "
                  << "user=" << sqlUsername << " "
                  << "password=" << sqlPass;
    elog << "dronescan::dronescan> Connecting to SQL Server." << std::endl;

    // SQLDb = new PgDatabase(connectString.str().c_str());
    SQLDb = new dbHandle(this, sqlHost, atoi(sqlPort), sqlDB, sqlUsername, sqlPass);
    if (SQLDb->ConnectionBad()) {
        elog << "dronescan> Failed to connect to SQL server: " << SQLDb->ErrorMessage()
             << std::endl;
        ::exit(0);
    }

    elog << "dronescan> Established connection to SQL server." << std::endl;

    // Verify database migrations
    if (!checkMigrationsAfterDBConnect("dronescan", SQLDb)) {
        ::exit(1);
    }

    /* Preload caches */
    preloadFakeClientCache();
    preloadUserCache();
    if (preloadExceptionalChannels() == false) {
        // If exceptional channels can't be loaded, we don't want glines to be turned on
        jcGlineEnable = false;
    }
    refreshSpamCaches();

    /* Initialise statistics */
    customDataCounter = 0;

    /* Setup fake oper record if necessary */
    unsigned short fakeOperLevel = atoi(dronescanConfig->Require("fakeOperLevel")->second.c_str());
    if (fakeOperLevel > 0) {
        /* Create a fake record for opers to use */
        fakeOperUser = new sqlUser(0);
        fakeOperUser->setAccess(fakeOperLevel);
        fakeOperUser->setFlags(0);
        fakeOperUser->setLastUpdated(::time(0));
        fakeOperUser->setLastUpdatedBy("Internal");
    } else {
        fakeOperUser = 0;
    }

    /* Load optional enableHello configuration */
    EConfig::const_iterator helloIt = dronescanConfig->Find("enableHello");
    if (helloIt != dronescanConfig->end() && atoi(helloIt->second.c_str()) == 1) {
        enableHello = true;
    } else {
        enableHello = false;
    }

    /* Load optional spyClientsChannel configuration. If unset, spy clients
     * don't join any channel on introduction. */
    EConfig::const_iterator spyChanIt = dronescanConfig->Find("spyClientsChannel");
    if (spyChanIt != dronescanConfig->end())
        spyClientsChannel = spyChanIt->second;

    /* Set up our timer. */
    theTimer = new Timer();

    /* Register commands available to users */
    RegisterCommand(new ACCESSCommand(this, "ACCESS", "() (<user>)"));
    RegisterCommand(new ADDUSERCommand(this, "ADDUSER", "<user> <access>"));
    RegisterCommand(
        new ADDEXCEPTIONALCHANNELCommand(this, "ADDEXCEPTIONALCHANNEL", "<channel name>"));
    RegisterCommand(new ANALYSECommand(this, "ANALYSE", "<#channel>"));
    RegisterCommand(new CHECKCommand(this, "CHECK", "(<#channel>) (<user>)"));
    RegisterCommand(new FAKECommand(this, "FAKE", "(activate)"));
    RegisterCommand(new HELLOCommand(this, "HELLO", "<account>"));
    RegisterCommand(new HELPCommand(this, "HELP", "(<command>)"));
    RegisterCommand(new LISTCommand(this, "LIST", "(active|fakeclients|joinflood|users)"));
    RegisterCommand(new MODUSERCommand(this, "MODUSER", "(ACCESS) <user> <level>"));
    RegisterCommand(new QUOTECommand(this, "QUOTE", "<string>"));
    RegisterCommand(
        new REMEXCEPTIONALCHANNELCommand(this, "REMEXCEPTIONALCHANNEL", "<channel name>"));
    RegisterCommand(new REMUSERCommand(this, "REMUSER", "<user>"));
    RegisterCommand(new STATUSCommand(this, "STATUS", ""));
    RegisterCommand(new RELOADCommand(this, "RELOAD", ""));
    RegisterCommand(new REHASHCommand(this, "REHASH", ""));
    RegisterCommand(new SPAMCommand(this, "SPAM",
        "(EVENT|RULE|ACTION|EXCLUSION) (ADD|DEL|LIST|SHOW|ADDEVENT|REMEVENT|ADDACTION|REMACTION) ..."));

    // in case RELOAD command is used, have to iterate the userlist to know how many clients per IP
    // are online for the gline count.
    clientsIPMap.clear();
    for (xNetwork::const_clientIterator cItr = Network->clients_begin();
         cItr != Network->clients_end(); ++cItr) {
        string IP = xIP(cItr->second->getIP()).GetNumericIP();
        /* Store usercount per IPs to a map */
        if (clientsIPMap.find(IP) == clientsIPMap.end())
            clientsIPMap.insert(std::make_pair(IP, 1));
        else
            clientsIPMap[IP]++;
    }

} // dronescan::dronescan(const string&)

/*************************
 ** D E S T R U C T O R **
 *************************/
dronescan::~dronescan() {}

/*****************************************************
 ** X C L I E N T   M E M B E R   F U N C T I O N S **
 *****************************************************/

/**
 * Here we run whatever needs doing as soon as we have attached to
 * the xServer. We are not attached to the network at this point.
 */
void dronescan::OnAttach() {
    /* Register for global network events */
    MyUplink->RegisterEvent(EVT_BURST_CMPLT, this);
    MyUplink->RegisterEvent(EVT_NETJOIN, this);
    MyUplink->RegisterEvent(EVT_NETBREAK, this);
    MyUplink->RegisterEvent(EVT_NICK, this);
    MyUplink->RegisterEvent(EVT_KILL, this);
    MyUplink->RegisterEvent(EVT_QUIT, this);

    /* Register for all channel events */
    MyUplink->RegisterChannelEvent(xServer::CHANNEL_ALL, this);

    /* Set up clearing active channels */
    time_t theTime = time(0) + dcInterval;
    tidClearActiveList = MyUplink->RegisterTimer(theTime, this, 0);

    /* Set up our JC counter */
    theTime = time(0) + jcInterval;
    tidClearJoinCounter = MyUplink->RegisterTimer(theTime, this, 0);

    theTime = time(0) + ncInterval;
    tidClearNickCounter = MyUplink->RegisterTimer(theTime, this, 0);

    theTime = time(0) + gbInterval;
    tidGlineQueue = MyUplink->RegisterTimer(theTime, this, 0);

    /* Periodic GC of expired TEXT_REPEAT tracking entries (fixed 60s) */
    theTime = time(0) + 60;
    tidRepeatGC = MyUplink->RegisterTimer(theTime, this, 0);

    xClient::OnAttach();
} // dronescan::OnAttach()

/**
 * Here we decide what channels our xClient needs to burst in to.
 * The only channel of any interest to us is our console channel, which is
 * loaded from the configuration file.
 */
void dronescan::BurstChannels() {
    MyUplink->JoinChannel(this, consoleChannel,
                          dronescanConfig->Require("consoleChannelModes")->second);

    /* We need to assign a clientData to all xClients on this xServer */
    xNetwork::const_localClientIterator itr = Network->localClient_begin();
    for (; itr != Network->localClient_end(); ++itr) {
        xClient* theXClient = itr->second;
        iClient* theClient = theXClient->getInstance();

        clientData* newData = new clientData();
        newData->setState(NORMAL);

        theClient->setCustomData(this, newData);
        customDataCounter++;
    }

    /* Set the topic */
    setConsoleTopic();

    /* Introduce all enabled spy clients to the network */
    srand(static_cast<unsigned int>(::time(nullptr)));
    introduceAllSpyClients();

    /* Schedule spy client joins for all enabled monitored channels */
    for (monitoredChannelsMapType::const_iterator it = monitoredChannelsMap.begin();
         it != monitoredChannelsMap.end(); ++it) {
        sqlMonitoredChannel* mc = it->second;
        if (!mc->isEnabled()) continue;

        if (mc->isJoinAsService()) {
            MyUplink->JoinChannel(this, mc->getName(), "");
        } else {
            if (findBestSpyClient(mc->getName(), mc->isForceJoin()) >= 0)
                scheduleSpyClientJoin(mc->getName(), mc->isForceJoin(), 0, 10);
            else
                Message(consoleChannel,
                        "[SpyClient] No available spy client for %s at burst.",
                        mc->getName().c_str());
        }
    }

    return xClient::BurstChannels();
} // dronescan::BurstChannels()

void dronescan::OnCTCP(iClient* theClient, const string& CTCP, const string& Message, bool Secure) {
    if (theClient && currentState == RUN)
        processSpamText(theClient, CTCP, spam_target::CTCP, "");

    StringTokenizer st(CTCP);

    if (st.empty())
        return;

    string Command = string_upper(st[0]);

    if ("DCC" == Command) {
        DoCTCP(theClient, CTCP, "REJECT");
    } else if ("PING" == Command) {
        DoCTCP(theClient, CTCP, Message);
    } else if ("VERSION" == Command) {
        DoCTCP(theClient, CTCP, "GNUWorld DroneScan v0.0.9.2");
    }

    xClient::OnCTCP(theClient, CTCP, Message, Secure);
}

/**
 * OnFakeCTCP: called when an iClient sends a CTCP directly to a fake
 * (spy) client. Feeds the CTCP command line into the spam detection
 * pipeline; spy clients do not auto-reply to CTCP.
 */
void dronescan::OnFakeCTCP(iClient* Sender, iClient* Target, const std::string& CTCP,
                           const std::string& Message, bool Secure)
{
    if (!Sender || currentState != RUN)
        return;
    processSpamText(Sender, CTCP, spam_target::CTCP, "");
    xClient::OnFakeCTCP(Sender, Target, CTCP, Message, Secure);
}

/**
 * OnChannelCTCP: called when an iClient sends a CTCP to a channel
 * (e.g. CTCP ACTION / "/me"). Feeds the CTCP command line into the
 * spam detection pipeline.
 */
void dronescan::OnChannelCTCP(iClient* Sender, Channel* theChan,
                              const std::string& CTCPCommand, const std::string& Message)
{
    if (!Sender || !theChan || currentState != RUN)
        return;
    processSpamText(Sender, CTCPCommand, spam_target::CTCP, theChan->getName());
    xClient::OnChannelCTCP(Sender, theChan, CTCPCommand, Message);
}

/**
 * OnFakeChannelCTCP: called when an iClient sends a channel CTCP caught
 * by a fake (spy) client. Feeds the CTCP command line into the spam
 * detection pipeline.
 */
void dronescan::OnFakeChannelCTCP(iClient* Sender, iClient* Target, Channel* theChan,
                                  const std::string& CTCPCommand, const std::string& Message)
{
    if (!Sender || !theChan || currentState != RUN)
        return;
    processSpamText(Sender, CTCPCommand, spam_target::CTCP, theChan->getName());
    xClient::OnFakeChannelCTCP(Sender, Target, theChan, CTCPCommand, Message);
}

/**
 * Here we receive network events that we are registered for.
 */
void dronescan::OnEvent(const eventType& theEvent, void* Data1, void* Data2, void* Data3,
                        void* Data4) {
    switch (theEvent) {
    case EVT_BURST_CMPLT: {
        log(DBG, "Caught EOB. Resetting frequencies.");
        updateState();
        break;
    } // EVT_BURST_CMPLT
    case EVT_NETJOIN: {
        changeState(BURST);
        break;
    }
    case EVT_NETBREAK: {
        lastSplitTime = ::time(0);
        updateState();
        break;
    }
    case EVT_NICK: {
        handleNewClient(static_cast<iClient*>(Data1));
        break;
    } // EVT_NICK
    case EVT_CHNICK: {
        handleNickChange(static_cast<iClient*>(Data1));
        break;
    }

    case EVT_KILL: /* Intentional drop through */
    case EVT_QUIT: {
        iClient* theClient = static_cast<iClient*>(theEvent == EVT_KILL ? Data2 : Data1);

        // QUIT-only: feed the quit reason into per-channel spam scoring for
        // every monitored channel the user was on. msg_Q.cc passes the quit
        // reason (Data2) and a channel-name snapshot (Data3) captured before
        // the client was removed from its channels. The client is still a
        // valid iClient* here (deleted only after PostEvent returns).
        if (theEvent == EVT_QUIT && currentState == RUN) {
            const std::string* quitReason = static_cast<const std::string*>(Data2);
            const std::vector<std::string>* quitChans =
                static_cast<const std::vector<std::string>*>(Data3);
            if (quitReason && quitChans && !quitReason->empty()) {
                for (std::vector<std::string>::const_iterator ci = quitChans->begin();
                     ci != quitChans->end(); ++ci) {
                    if (monitoredChannelsMap.count(string_lower(*ci)))
                        processSpamText(theClient, *quitReason, spam_target::QUIT, *ci);
                }
            }
        }

        /* Store usercount per IPs to a map */
        string IP = xIP(theClient->getIP()).GetNumericIP();
        if (clientsIPMap[IP] <= 1)
            clientsIPMap.erase(IP);
        else
            clientsIPMap[IP]--;

        clientData* theData = static_cast<clientData*>(theClient->removeCustomData(this));

        delete (theData);
        --customDataCounter;
        break;
    }
    } // switch( theEvent )

    xClient::OnEvent(theEvent, Data1, Data2, Data3, Data4);
}

/** Receive channel events. */
void dronescan::OnChannelEvent(const channelEventType& theEvent, Channel* theChannel, void* Data1,
                               void* Data2, void* Data3, void* Data4) {
    /* If we are bursting, we don't want to be checking joins. */
    if (currentState == BURST)
        return;

    /* If this is not a join and not part, we don't care. */
    if (theEvent != EVT_JOIN && theEvent != EVT_PART)
        return;

    iClient* theClient = static_cast<iClient*>(Data1);
    if (theEvent == EVT_JOIN) {
        // Auto-voice spy clients and auto-op opers in the console channel
        if (!strcasecmp(theChannel->getName().c_str(), consoleChannel.c_str())) {
            if (isSpyClient(theClient))
                voiceSpyClientInConsole(theClient);
            else if (theClient->isOper())
                opInConsole(theClient);
        }
        handleChannelJoin(theChannel, theClient);
    } else if (theEvent == EVT_PART) {
        handleChannelPart(theChannel, theClient);
        // Feed part reason into spam detection (TEXT events with PART target)
        const string partReason = (Data2 && currentState == RUN)
            ? *static_cast<string*>(Data2) : string();
        if (!partReason.empty())
            processSpamText(theClient, partReason,
                            spam_target::PART, theChannel->getName());
    }
    xClient::OnChannelEvent(theEvent, theChannel, Data1, Data2, Data3, Data4);
}

/**
 * Here we receive private messages from iClients.
 */
void dronescan::OnPrivateMessage(iClient* theClient, const string& Message, bool) {
    if (theClient && currentState == RUN)
        processSpamText(theClient, Message, spam_target::PRIVMSG, "");

    sqlUser* theUser = getSqlUser(theClient->getAccount());

    /* If the client is opered, we might have a fake account */
    if (!theUser && theClient->isOper() && fakeOperUser) {
        theUser = fakeOperUser;

        fakeOperUser->setUserName(theClient->getNickName());
        fakeOperUser->setCreated(::time(0));
        fakeOperUser->setLastSeen(::time(0));
    }

    if (!theClient->isOper())
        return;
    StringTokenizer st(Message);

    if (st.size() < 1)
        return;

    string Command = string_upper(st[0]);

    if (!theUser && Command != "HELLO") {
        //log(INFO, "Oper %s is not authenticated for using command %s", theClient->getNickName().c_str(), Command.c_str());
        elog << "Oper " << theClient->getNickName() << " is not authenticated for using command "
             << Command << endl;
        return;
    }

    /* We have now seen this user! */
    if (theUser) {
        theUser->setLastSeen(::time(0));
        theUser->commit();
    }

    /* If we are currently in BURST, we don't accept commands */
    /*if(BURST == currentState) {
            Reply(theClient, "Sorry, I do not accept commands during a burst.");
            return ;
    }*/

    commandMapType::iterator commandHandler = commandMap.find(Command);

    if (commandHandler != commandMap.end()) {
        commandHandler->second->Exec(theClient, Message, theUser);
        return;
    }

    if ("INVITE" == Command) {
        Invite(theClient, consoleChannel);
        return;
    }

    /* This is commented out because it doesn't work at the moment */
    /*	if("RELOAD" == Command)
                    {
                    getUplink()->UnloadClient(this, "Reloading...");
                    getUplink()->LoadClient("libdronescan.la", getConfigFileName());
                    return ;
                    }
    */
    if ("STATS" == Command) {
        Reply(theClient, "Allocated custom data: %d", customDataCounter);

        Reply(theClient, "CM/NM/CC: %0.2lf/%0.2lf/%u", channelMargin, nickMargin, channelCutoff);

        Reply(theClient, "jcI/jcC : %u/%u", jcInterval, jcCutoff);
        Reply(theClient, "ncI/ncC : %u/%u", ncInterval, ncCutoff);
        return;
    }

    if ("RESET" == Command) {
        resetAndCheck();
        return;
    }

    if (st.size() < 2)
        return;

    if ("INFO" == Command) {
        string nick = st[1];
        iClient* targetClient = Network->findNick(nick);
        if (!targetClient)
            return;
        Reply(theClient, "Status of %s: %s", targetClient->getNickName().c_str(),
              (isNormal(targetClient) ? "Normal" : "Abnormal"));
        return;
    }

    if (st.size() < 3)
        return;

    if ("SET" == Command) {
        string Option = string_upper(st[1]);

        // Glines on/off
        if ("GL" == Option) {
            if ("ON" == string_upper(st[2])) {
                if (jcGlineEnable) {
                    Reply(theClient, "I am already setting glines - no change");
                    return;
                } else {
                    if (jcGlineEnableConf) {
                        jcGlineEnable = true;
                        Reply(theClient, "Glines activated");
                        log(INFO, "%s is activating auto-glines", theClient->getNickName().c_str());
                    } else {
                        Reply(theClient, "You have to enable auto-glines in the conf first");
                    }
                    return;
                }
            } else if ("OFF" == string_upper(st[2])) {
                if (!jcGlineEnable) {
                    Reply(theClient, "I'm not setting glines - no change");
                    return;
                } else {
                    jcGlineEnable = false;
                    Reply(theClient, "Glines deactivated");
                    log(INFO, "%s is deactivating auto-glines", theClient->getNickName().c_str());
                    return;
                }
            } else {
                Reply(theClient, "Syntax: set GL <ON|OFF>");
                return;
            }
        }

        /* Global entropy options */
        if ("CC" == Option) {
            unsigned int newCC = atoi(st[2].c_str());
            channelCutoff = newCC;
            resetAndCheck();
            return;
        }
        if ("CM" == Option) {
            double newCM = atof(st[2].c_str());
            if (newCM < 0 || newCM > 1)
                return;
            channelMargin = newCM;
            resetAndCheck();
            return;
        }
        if ("NM" == Option) {
            double newNM = atof(st[2].c_str());
            if (newNM < 0 || newNM > 1)
                return;
            nickMargin = newNM;
            resetAndCheck();
            return;
        }

        /* None of the hardcoded options have hit. Try dynamic. */
        if (Test* theTest = setTestVariable(Option, st[2])) {
            log(INFO, "%s set %s to %s in %s", theClient->getNickName().c_str(), Option.c_str(),
                st[2].c_str(), theTest->getName().c_str());

            resetAndCheck();
            return;
        } else {
            Reply(theClient, "No test accepted the variable %s", Option.c_str());
            return;
        }
    }
}

/**
 * OnFakePrivateMessage: called when an iClient sends a PRIVMSG directly to
 * a fake (spy) client. Feeds the message into the spam detection pipeline.
 * Unlike OnPrivateMessage, no command dispatch happens here - that logic
 * is specific to real authenticated clients issuing bot commands.
 */
void dronescan::OnFakePrivateMessage(iClient* Sender, iClient* Target,
                                     const std::string& Message, bool secure)
{
    if (!Sender || currentState != RUN)
        return;
    processSpamText(Sender, Message, spam_target::PRIVMSG, "");
    xClient::OnFakePrivateMessage(Sender, Target, Message, secure);
}

/**
 * OnChannelMessage: called when an iClient sends a PRIVMSG to a channel
 * the main service client is a member of (e.g. joinasservice channels).
 * Feeds the message into the spam detection pipeline.
 */
void dronescan::OnChannelMessage(iClient* Sender, Channel* theChan,
                                 const std::string& Message)
{
    if (!Sender || !theChan || currentState != RUN)
        return;
    processSpamText(Sender, Message,
                    spam_target::CHAN_PRIV, theChan->getName());
    xClient::OnChannelMessage(Sender, theChan, Message);
}

/**
 * OnFakeChannelMessage: called when an iClient sends a PRIVMSG to a channel.
 * Feeds the message into the spam detection pipeline.
 */
void dronescan::OnFakeChannelMessage(iClient* Sender, iClient* Target, Channel* theChan,
                                 const std::string& Message)
{
    if (!Sender || !theChan || currentState != RUN)
        return;
    processSpamText(Sender, Message,
                    spam_target::CHAN_PRIV, theChan->getName());
    xClient::OnFakeChannelMessage(Sender, Target, theChan, Message);
}

/**
 * OnChannelNotice: called when an iClient sends a NOTICE to a channel.
 * Feeds the notice into the spam detection pipeline with the CHAN_NOT bit.
 */
void dronescan::OnChannelNotice(iClient* Sender, Channel* theChan,
                                const std::string& Message)
{
    if (!Sender || !theChan || currentState != RUN)
        return;
    processSpamText(Sender, Message,
                    spam_target::CHAN_NOT, theChan->getName());
    xClient::OnChannelNotice(Sender, theChan, Message);
}

/**
 * OnFakeChannelNotice: called when an iClient sends a NOTICE to a channel
 * caught by a fake (spy) client. Feeds the notice into the spam detection
 * pipeline with the CHAN_NOT bit.
 */
void dronescan::OnFakeChannelNotice(iClient* Sender, iClient* Target, Channel* theChan,
                                    const std::string& Message)
{
    if (!Sender || !theChan || currentState != RUN)
        return;
    processSpamText(Sender, Message,
                    spam_target::CHAN_NOT, theChan->getName());
    xClient::OnFakeChannelNotice(Sender, Target, theChan, Message);
}

/**
 * OnPrivateNotice: called when an iClient sends a NOTICE directly to the bot.
 * Feeds the notice into the spam detection pipeline with the NOTICE bit.
 */
void dronescan::OnPrivateNotice(iClient* Sender, const std::string& Message, bool secure)
{
    if (!Sender || currentState != RUN)
        return;
    processSpamText(Sender, Message, spam_target::NOTICE, "");
    xClient::OnPrivateNotice(Sender, Message, secure);
}

/**
 * OnFakePrivateNotice: called when an iClient sends a NOTICE directly to a
 * fake (spy) client. Feeds the notice into the spam detection pipeline
 * with the NOTICE bit.
 */
void dronescan::OnFakePrivateNotice(iClient* Sender, iClient* Target,
                                    const std::string& Message, bool secure)
{
    if (!Sender || currentState != RUN)
        return;
    processSpamText(Sender, Message, spam_target::NOTICE, "");
    xClient::OnFakePrivateNotice(Sender, Target, Message, secure);
}

/**
 * OnNetworkKick: called when any client is kicked from a channel.
 * If the kicked client is one of our spy clients:
 *  - Send a PART to resync GNUWorld's internal channel state.
 *  - Report the kick to the console channel.
 *  - Track kick count per channel in a 24-hour window.
 *  - If 2+ kicks in 24h: stop monitoring that channel.
 *  - Otherwise: schedule another spy client to rejoin after 300-1500s.
 */
void dronescan::OnNetworkKick(Channel* theChan, iClient* srcClient,
                              iClient* destClient, const string& kickMessage,
                              bool authoritative)
{
    int scId = getSpyClientId(destClient);
    if (scId < 0) {
        xClient::OnNetworkKick(theChan, srcClient, destClient, kickMessage, authoritative);
        return;
    }

    // Resync GNUWorld channel state (same pattern as cloner.cc)
    std::stringstream partMsg;
    partMsg << destClient->getCharYYXXX() << " L " << theChan->getName() << endl;
    MyUplink->Write(partMsg);

    const string chanKey = string_lower(theChan->getName());

    // Update our tracking maps - only this channel; the spy client may
    // still be covering others.
    chanActiveSpyMap.erase(chanKey);
    {
        spyClientChanMapType::iterator sit = spyClientChanMap.find(scId);
        if (sit != spyClientChanMap.end()) {
            sit->second.erase(chanKey);
            if (sit->second.empty())
                spyClientChanMap.erase(sit);
        }
    }

    // Report to console
    const string kickerStr = srcClient ? srcClient->getNickName() : "<server>";
    Message(consoleChannel,
            "[SpyClient] %s was kicked from %s by %s (%s).",
            destClient->getNickName().c_str(),
            theChan->getName().c_str(),
            kickerStr.c_str(),
            kickMessage.c_str());

    // Kick tracking ? 24-hour window
    time_t now = ::time(nullptr);
    ChanKickTrack& track = chanKickTrackMap[chanKey];
    if (track.count == 0 || (now - track.firstKickTime) > 86400) {
        // Start a new window
        track.count        = 1;
        track.firstKickTime = now;
    } else {
        ++track.count;
    }

    if (track.count >= 2) {
        kickStoppedChannels.insert(chanKey);
        Message(consoleChannel,
                "[SpyClient] Stopping monitoring of %s after %d spy client kicks in 24h.",
                theChan->getName().c_str(), track.count);
        xClient::OnNetworkKick(theChan, srcClient, destClient, kickMessage, authoritative);
        return;
    }

    // Schedule a replacement ? use a different spy client if possible
    monitoredChannelsMapType::const_iterator mc = monitoredChannelsMap.find(chanKey);
    bool forcejoin = (mc != monitoredChannelsMap.end()) ? mc->second->isForceJoin() : false;

    if (findBestSpyClient(theChan->getName(), forcejoin) >= 0) {
        scheduleSpyClientJoin(theChan->getName(), forcejoin, 300, 1500);
    } else {
        Message(consoleChannel,
                "[SpyClient] No available spy client to replace %s in %s.",
                destClient->getNickName().c_str(), theChan->getName().c_str());
    }

    xClient::OnNetworkKick(theChan, srcClient, destClient, kickMessage, authoritative);
}

/** Clean up after ourselves */
void dronescan::OnDetach(const std::string& message) {
    /* We need to delete() anything we have new()d
     * Currently this is:
     *  dronescanConfig
     *  Tests
     *  Commands
     *  clientData for every client on the net
     *  theTimer
     *
     * It is important this is kept up to date so that reload() does not leak.
     * We also need to unregister any timers we have so GNUworld doesn't segfault :)
     */

    /* Delete our config */
    delete dronescanConfig;

    /* Delete our timer */
    delete theTimer;

    /* Delete commands */
    for (commandMapType::iterator itr = commandMap.begin(); itr != commandMap.end(); ++itr) {
        delete itr->second;
    }
    commandMap.clear();

    /* Delete tests */
    for (testMapType::iterator itr = testMap.begin(); itr != testMap.end(); ++itr) {
        delete itr->second;
    }
    testMap.clear();

    /* Iterate over clients to delete clientData */
    for (xNetwork::const_clientIterator ptr = Network->clients_begin();
         ptr != Network->clients_end(); ++ptr) {
        iClient* theClient = ptr->second;
        clientData* theData = static_cast<clientData*>(theClient->removeCustomData(this));
        delete theData;
    }

    /* Unregister the join counting timer */
    if (!MyUplink->UnRegisterTimer(tidClearJoinCounter, 0) ||
        !MyUplink->UnRegisterTimer(tidClearNickCounter, 0) ||
        !MyUplink->UnRegisterTimer(tidClearActiveList, 0)) {
        elog << "dronescan::~dronescan> "
             << "Could not unregister timer. Expect problems shortly." << std::endl;
    }

    /* Cancel pending spy client join timers */
    for (pendingJoinTimersType::iterator it = pendingJoinTimers.begin();
         it != pendingJoinTimers.end(); ++it) {
        MyUplink->UnRegisterTimer(it->first, nullptr);
    }
    pendingJoinTimers.clear();

    /* Detach all live spy clients */
    for (liveSpyClientsMapType::iterator it = liveSpyClientsMap.begin();
         it != liveSpyClientsMap.end(); ++it) {
        MyUplink->DetachClient(it->second, "Dronescan detach");
    }
    liveSpyClientsMap.clear();
    chanActiveSpyMap.clear();
    spyClientChanMap.clear();

    /* Stop the repeat-tracking GC timer and drop tracking state */
    MyUplink->UnRegisterTimer(tidRepeatGC, nullptr);
    repeatTrackMap.clear();

    /* Done! */
    xClient::OnDetach(message);
}

/** Receive our own timed events. */
void dronescan::OnTimer(const xServer::timerID& theTimer, void*) {
    time_t theTime;

    /* Handle pending spy client join timers */
    {
        pendingJoinTimersType::iterator pit = pendingJoinTimers.find(theTimer);
        if (pit != pendingJoinTimers.end()) {
            bool forcejoin       = pit->second.first;
            string chanName      = pit->second.second;
            pendingJoinTimers.erase(pit);
            doSpyClientJoin(chanName, forcejoin);
            return;
        }
    }

    if (theTimer == tidClearActiveList) {
        droneChannelsType::iterator dcitr, next_dcitr;
        time_t joinSince = ::time(0) - dcInterval;

        for (dcitr = droneChannels.begin(); dcitr != droneChannels.end(); dcitr = next_dcitr) {
            next_dcitr = dcitr;
            ++next_dcitr;

            if (dcitr->second->getLastJoin() <= joinSince) {
                log(DBG, "Removing %s.", dcitr->first.c_str());

                delete dcitr->second;
                droneChannels.erase(dcitr);
            }
        }

        theTime = time(0) + dcInterval;
        tidClearActiveList = MyUplink->RegisterTimer(theTime, this, 0);
    }

    if (theTimer == tidClearJoinCounter) {
        processJoinPartChannels();
    }

    if (theTimer == tidClearNickCounter) {
        for (ncChanMapType::const_iterator itr = ncChanMap.begin(); itr != ncChanMap.end(); ++itr) {
            Channel* theChan = Network->findChannel(itr->first);
            if (0 == theChan) {
                elog << "dronescan::OnTimer> Unable "
                     << "to find channel: " << itr->first << std::endl;
                continue;
            }

            if (itr->second >= ncCutoff)
                log(WARN, "Nick flood over in %s. Total joins: %u. Total size: %d",
                    itr->first.c_str(), itr->second, theChan->size());
        }

        log(DBG, "Clearing %u records from the join counter.", ncChanMap.size());
        ncChanMap.clear();

        theTime = time(0) + ncInterval;
        tidClearNickCounter = MyUplink->RegisterTimer(theTime, this, 0);
    }

    if (theTimer == tidGlineQueue) {
        processGlineQueue();

        theTime = time(0) + gbInterval;
        tidGlineQueue = MyUplink->RegisterTimer(theTime, this, 0);
    }

    if (theTimer == tidRepeatGC) {
        // Reclaim expired TEXT_REPEAT tracking entries
        time_t nowGC = ::time(0);
        for (repeatTrackMapType::iterator rit = repeatTrackMap.begin();
             rit != repeatTrackMap.end(); ) {
            if (nowGC > rit->second.expires_at)
                rit = repeatTrackMap.erase(rit);
            else
                ++rit;
        }

        theTime = time(0) + 60;
        tidRepeatGC = MyUplink->RegisterTimer(theTime, this, 0);
    }
}

/*******************************************
 ** D R O N E S C A N   F U N C T I O N S **
 *******************************************/

void dronescan::processJoinPartChannels() {
    static time_t jcFCInterval = ::time(0);
    for (jcChanMapType::const_iterator itr = jcChanMap.begin(); itr != jcChanMap.end(); ++itr) {
        Channel* theChan = Network->findChannel(itr->first);
        if (0 == theChan) {
            elog << "dronescan::OnTimer> Unable "
                 << "to find channel: " << itr->first << std::endl;
            /* The join/flood is now reported/glined even if the channel is empty.
               We now have to make sure theChan != 0 below.
            */
        }
        jfChannel* jChannel = itr->second;

        if (jChannel->getNumOfJoins() >= jcCutoff) {
            log(WARN, "Join flood over in %s. Total joins: %u. Total parts: %u. Total size: %d",
                itr->first.c_str(), jChannel->getNumOfJoins(), jChannel->getNumOfParts(),
                (theChan == 0 ? 0 : theChan->size()));
            std::list<std::pair<glineData*, std::list<string>>> glined;
            jfChannel::joinPartMapIterator joinPartIt = jChannel->joinPartBegin();
            jfChannel::joinPartMapIterator joinPartEnd = jChannel->joinPartEnd();
            std::stringstream names;
            std::stringstream excluded;
            std::stringstream tempNames;
            int clientcount = 0;

            bool isoktogline = false;
            bool isoktogline2 = false;
            for (; joinPartIt != joinPartEnd; ++joinPartIt) {
                isoktogline = ((::time(0) - lastBurstTime) >= (jcGracePeriodBurstOrSplit / 2) &&
                               (::time(0) - lastSplitTime) >= (jcGracePeriodBurstOrSplit / 2) &&
                               jcGlineEnable && jChannel->getNumOfJoins() > jcMinJFSizeToGline &&
                               (jChannel->getNumOfParts() > jcMinJFSizeToGline ||
                                (joinPartIt->second.numOfJoins >= jcMinJoinToGlineJOnly &&
                                 jChannel->getNumOfJoins() >= jcMinJFJOnlySizeToGline)))
                                  ? true
                                  : false;
                if (isoktogline)
                    isoktogline2 = true;
#ifdef ENABLE_LOG4CPLUS
                int numOfUsernames = 0;
#endif
                if ((joinPartIt->second.numOfJoins >= jcMinJoinToGline &&
                     joinPartIt->second.numOfParts >= jcMinJoinToGline) ||
                    (joinPartIt->second.numOfJoins >= jcMinJoinToGlineJOnly &&
                     jChannel->getNumOfJoins() >= jcMinJFJOnlySizeToGline)) {
                    if (!joinPartIt->second.seenOper && !joinPartIt->second.seenLoggedInUser) {
                        std::list<string>::const_iterator numericsIt =
                            joinPartIt->second.numerics.begin();
                        tempNames << joinPartIt->first.c_str() << std::string("[")
                                  << joinPartIt->second.numOfJoins << std::string("]<");
                        std::list<std::string> clients;
                        int nickCount = 0;
                        for (; numericsIt != joinPartIt->second.numerics.end(); ++numericsIt) {
                            iClient* theClient = Network->findClient(*numericsIt);
                            if (theClient && !strcmp(xIP(theClient->getIP()).GetNumericIP().c_str(),
                                                     joinPartIt->first.c_str())) {
                                nickCount++;
                                if (nickCount > 1)
                                    tempNames << ",";
                                tempNames << theClient->getNickName();
                                if (isoktogline == true) {
                                    clients.push_back(theClient->getNickName() + "!" +
                                                      theClient->getUserName() + "@" +
                                                      xIP(theClient->getIP()).GetNumericIP(true) +
                                                      " " + theClient->getDescription());
                                }
                            }
                        }
                        tempNames << ">";
                        if (names.str().size() + tempNames.str().size() > 400) {
                            outputNames(itr->first, names, false, isoktogline);
                        }
                        names << " " << tempNames.str();
                        tempNames.str("");
                        if (isoktogline == true) {
                            glineData* theGline = new (std::nothrow)
                                glineData("*@" + joinPartIt->first, jcGlineReason, jcGlineLength);
                            assert(theGline != 0);
                            glined.push_back(
                                std::pair<glineData*, std::list<std::string>>(theGline, clients));
                            clientcount += clients.size();
                        }
                    } else {
                        excluded << joinPartIt->first.c_str() << "[";
                        if (joinPartIt->second.seenOper) {
                            excluded << "O";
                        }
                        if (joinPartIt->second.seenLoggedInUser) {
                            excluded << "L";
                        }
                        excluded << "],";
                        if (excluded.str().size() > 400) {
                            outputNames(itr->first, excluded, true, isoktogline);
                        }

#ifdef ENABLE_LOG4CPLUS
                        std::list<std::pair<std::string, std::string>>::const_iterator userNamesIt =
                            joinPartIt->second.userNames.begin();
                        for (; userNamesIt != joinPartIt->second.userNames.end(); ++userNamesIt) {
                            iClient* theClient = Network->findClient(userNamesIt->first);
                            if (theClient && !strcmp(xIP(theClient->getIP()).GetNumericIP().c_str(),
                                                     joinPartIt->first.c_str())) {
                                if (numOfUsernames++ == 0) {
                                    log(JF_CSERVICE,
                                        "Join flood over in %s. Total joins: %u. Total parts: %u. "
                                        "Total size: %d",
                                        itr->first.c_str(), jChannel->getNumOfJoins(),
                                        jChannel->getNumOfParts(),
                                        (theChan == 0 ? 0 : theChan->size()));
                                }
                                /*log(JF_CSERVICE,"%s %s!%s@%s %s",userNamesIt->second.c_str(),
                                        theClient->getNickName().c_str(),
                                        theClient->getUserName().c_str(),
                                        xIP(theClient->getIP()).GetNumericIP().c_str(),
                                        theClient->getDescription().c_str());*/
                                log(JF_CSERVICE, "(%s) %s!%s@%s.users.undernet.org %s",
                                    xIP(theClient->getIP()).GetNumericIP().c_str(),
                                    theClient->getNickName().c_str(),
                                    theClient->getUserName().c_str(), userNamesIt->second.c_str(),
                                    theClient->getDescription().c_str());
                            }
                        }
#endif
                    }
                }
            }

            isoktogline = isoktogline2;

            if (names.str().size() > 0) {
                outputNames(itr->first, names, false, isoktogline);
            }
            if (excluded.str().size() > 0) {
                outputNames(itr->first, excluded, true, isoktogline);
            }
            if (isoktogline == true) {
                /* Deactivating the gline abortion if too few clients flooding found */
                // if ((glined.size() < 3) && (clientcount < 8))
                //	{
                //		if (glined.size() != 0) {
                //		log(WARN,"Aborting glines for channel %s because only %d flooding
                // clients from %d addresses were found",
                //			itr->first.c_str(),clientcount,glined.size());
                //		}
                //	}
#ifdef ENABLE_LOG4CPLUS
                //	else {
                log(JF_GLINED,
                    "Join flood over in %s. Total joins: %u. Total parts: %u. Total size: %d. "
                    "Total addresses glined %d.",
                    itr->first.c_str(), jChannel->getNumOfJoins(), jChannel->getNumOfParts(),
                    (theChan == 0 ? 0 : theChan->size()), glined.size());
                //	}
#endif
            }
            std::list<std::pair<glineData*, std::list<string>>>::iterator glinesIt = glined.begin();
            for (; glinesIt != glined.end(); ++glinesIt) {
                glineData* curGline = glinesIt->first;
                /* Deactivating the gline abortion if too few clients flooding found */
                // if ((glined.size() >= 3) || (clientcount >= 8))
                //	{
#ifdef ENABLE_LOG4CPLUS
                std::list<string>::iterator clientsIt = glinesIt->second.begin();
                for (; clientsIt != glinesIt->second.end(); ++clientsIt) {
                    log(JF_GLINED, (*clientsIt).c_str());
                }
#endif
                // Checking if the gline is already present in the queue
                glineQueueType::iterator git = glineQueue.begin();
                bool found_gline_in_queue = false;
                for (; git != glineQueue.end(); ++git) {
                    if (strcasecmp((*git)->getHost().c_str(), curGline->getHost().c_str()) == 0)
                        found_gline_in_queue = true;
                }
                // skipping findGline() check, cpu expensive.
                // if ((MyUplink->findGline(curGline->getHost()) == 0) && (found_gline_in_queue ==
                // false)) {
                if (found_gline_in_queue == false) {
                    glineQueue.push_back(curGline);
                }
                //}
                else {
                    delete curGline;
                }
            }

            /* Deactivating the gline abortion if too few clients flooding found */
            // if ((glined.size() >= 3) || (clientcount >= 8))
            log(WARN, "Glining %d floodbots from %d different ips", clientcount, glined.size());
        }

        delete itr->second;
    }

    IPJQueueType::iterator Itr2 = IPJQueue.begin();
    std::stringstream tempNames;
    int clientcount = 0;
    for (; Itr2 != IPJQueue.end(); Itr2++) {
        glineQueueType::iterator git = glineQueue.begin();
        bool found_gline_in_queue = false;
        for (; git != glineQueue.end(); ++git) {
            std::string gmask = "*@" + *Itr2;
            if (strcasecmp((*git)->getHost().c_str(), gmask.c_str()) == 0)
                found_gline_in_queue = true;
        }
        if (found_gline_in_queue == true) {
            continue;
        }
#ifdef ENABLE_LOG4CPLUS
        if (clientcount == 0)
            log(JF_GLINED, "--- Multiple join floods without parts ---");
#endif
        clientcount++;
        if (tempNames.str().size() > 380) {
            std::stringstream s;
            s << "Glining the following clients:" << tempNames.str();
            log(WARN, "%s", s.str().c_str());
            tempNames.str("");
        }
        tempNames << " ";
        jcFloodClients* jcFC = clientsIPFloodMap[*Itr2];
        std::list<string>::const_iterator Itr3 = jcFC->nicks.begin();
        tempNames << *Itr2 << std::string("[") << jcFC->count << std::string("]<");
        int Count = 0;
        for (; Itr3 != jcFC->nicks.end(); Itr3++) {
            Count++;
            if (Count > 1)
                tempNames << ",";
            tempNames << *Itr3;
        }
        tempNames << ">(";
        Itr3 = jcFC->chans.begin();
        Count = 0;
        for (; Itr3 != jcFC->chans.end(); Itr3++) {
            Count++;
            if (Count > 1)
                tempNames << ",";
            tempNames << *Itr3;
        }
        tempNames << ")";

        glineData* theGline =
            new (std::nothrow) glineData("*@" + *Itr2, jcGlineReason, jcGlineLength);
        assert(theGline != 0);
        glineQueue.push_back(theGline);
#ifdef ENABLE_LOG4CPLUS
        Itr3 = jcFC->log.begin();
        for (; Itr3 != jcFC->log.end(); Itr3++) {
            log(JF_GLINED, "%s", (*Itr3).c_str());
        }
#endif
    }
    if (clientcount > 0) {
#ifdef ENABLE_LOG4CPLUS
        log(JF_GLINED, "--- End of multiple join floods without parts ---");
#endif
        std::string astr = "Glining the following clients:" + tempNames.str();
        log(WARN, "%s", astr.c_str());
        log(WARN, "Glining %d different ips for multiple joins during join floods", clientcount);
    }
    if (IPJQueue.size() > 0)
        IPJQueue.clear();
    if ((unsigned int)(jcFCInterval - ::time(0)) >= jcJoinsPerIPTime) {
        jcFloodClients* jcFC;
        jcFCInterval = ::time(0);
        clientsIPFloodMapType::iterator anItr = clientsIPFloodMap.begin();
        while (anItr != clientsIPFloodMap.end()) {
            jcFC = anItr->second;
            if ((unsigned int)(::time(0) - jcFC->ctime) > jcJoinsPerIPTime) {
                anItr = clientsIPFloodMap.erase(anItr);
                delete jcFC;
            } else {
                anItr++;
            }
        }
    }

    log(DBG, "Clearing %u records from the join counter.", jcChanMap.size());
    jcChanMap.clear();

    time_t theTime = time(0) + jcInterval;
    tidClearJoinCounter = MyUplink->RegisterTimer(theTime, this, 0);
}

void dronescan::outputNames(const std::string& chanName, std::stringstream& names, bool exclude,
                            bool isoktogline) {
    names << "\r\n";
    // std::string gString = exclude ? "Excluding" : jcGlineEnable ? "Glining" : "Suppose to gline";
    std::string gString = exclude ? "Excluding" : isoktogline ? "Glining" : "Suppose to gline";
    log(WARN, "%s the following clients from %s: %s", gString.c_str(), chanName.c_str(),
        names.str().c_str());
    names.str("");
}

void dronescan::processGlineQueue() {
    if (glineQueue.size() > 0) {
        unsigned int count = 0;
        int userCount;
        glineData* curGline;
        char us[100];
        log(DBG, "Processing gline queue\r\n");
        for (; count < gbCount && glineQueue.size() > 0;) {
            curGline = glineQueue.front();
            glineQueue.pop_front();
            // userCount = Network->countMatchingRealUserHost(curGline->getHost());
            StringTokenizer st(curGline->getHost(), '@');

            recentlyGlinedIpsType::iterator rItr = recentlyGlinedIps.begin();
            bool alreadyGlined = false;
            for (; rItr != recentlyGlinedIps.end(); rItr++) {
                if ((rItr->first == st[1]) && ((::time(0) - rItr->second) < 120)) {
                    alreadyGlined = true;
                }
            }
            if (alreadyGlined) {
                delete curGline;
                continue;
            }

            userCount = clientsIPMap[st[1]];
            us[0] = '\0';
            sprintf(us, "%d", userCount);
            std::string glineReason = string("AUTO [") + us + string("] ") + curGline->getReason();
            MyUplink->setGline(nickName, curGline->getHost(), glineReason, curGline->getExpires(),
                               ::time(0), this);

            recentlyGlinedIps.push_front(std::pair<std::string, int>(st[1], ::time(0)));
            if (recentlyGlinedIps.size() > RecentlyGlinedIpsSize) {
                recentlyGlinedIps.pop_back();
            }
            delete curGline;
            count++;
        }

        log(DBG, "Processed %d glines from the gline queue, %d glines are left in the queue", count,
            glineQueue.size());
    }
}
/** Report a SQL error as necessary. */
void dronescan::doSqlError(const string& theQuery, const string& theError) {
    /* First, log it to error out */
    elog << "SQL> Whilst executing: " << theQuery << std::endl;
    elog << "SQL> " << theError << std::endl;
}

/** This function allows us to change our current state. */
void dronescan::changeState(DS_STATE newState) {
    if (currentState == newState)
        return;

    /* Instantiate our own timer so we don't interfere with anyone elses */
    Timer stateTimer;
    stateTimer.Start();

    /* First, do what we need to exit our current state */
    switch (currentState) {
    case BURST: {
        calculateEntropy();
        setNickStates();
        elog << "*** DroneScan: Exiting state BURST" << std::endl;
        lastBurstTime = ::time(0);
        break;
    }
    case RUN: {
        elog << "*** DroneScan: Exiting state RUN" << std::endl;
    }
    }

    currentState = newState;

    switch (currentState) {
    case BURST: {
        charMap.clear();
        elog << "*** DroneScan: Entering state BURST" << std::endl;
        break;
    }
    case RUN: {
        elog << "*** DroneScan: Entering state RUN" << std::endl;
        checkChannels();
        break;
    }
    }

    log(INFO, "Changed state in: %u ms", stateTimer.stopTimeMS());
}

void dronescan::updateState() {
    std::list<iServer*> burstingServers = Network->getAllBurstingServers();
    std::list<iServer*>::iterator ptr = burstingServers.begin();
    for (; ptr != burstingServers.end(); ++ptr) {
        log(DBG, "Server %s is bursting", (*ptr)->getName().c_str());
    }
    changeState(burstingServers.size() == 0 ? RUN : BURST);
}

void dronescan::handleChannelJoin(Channel* theChannel, iClient* theClient) {
    /* If this channel is too small, don't test it. */
    if (theChannel->size() < channelCutoff)
        return;

    /* If this is an exceptional channel, dont test it. */
    if (isExceptionalChannel(theChannel->getName()))
        return;

    /* Iterate over our available tests, checking this channel */
    if (droneChannels.find(theChannel->getName()) == droneChannels.end()) {
        /* This channel is not currently listed as active */
        // Check the channel for abnormalities, if enough
        // are found then it will be added to the
        // droneChannels structure by checkChannel()
        checkChannel(theChannel);
    }

    /* Reset lastjoin on the active channel */
    droneChannelsType::iterator droneChanItr = droneChannels.find(theChannel->getName());

    // If the channel is still not in the droneChannels
    // structure then it is a "normal" channel
    if (droneChanItr != droneChannels.end()) {
        droneChanItr->second->setLastJoin(::time(0));
    }

    /* Do join count processing if applicable */
    if ((::time(0) - lastBurstTime) < jcGracePeriodBurstOrSplit)
        return; /* Don't report join/floods right after a burst */
    const iServer* theClientServer = theClient->getServer();
    if ((theClientServer->getLag() > jcIgnoreJoinFloodLag) &&
        ((::time(0) - theClientServer->getLastLagTS()) < jcIgnoreJoinFloodLagTS))
        return; /* Don't report join/floods if server is lagged */

    const string& channelName = theChannel->getName();
    jcChanMapIterator jcChanIt = jcChanMap.find(channelName);
    jfChannel* channel;
    if (jcChanIt != jcChanMap.end()) {
        channel = jcChanIt->second;
    } else {
        channel = new (std::nothrow) jfChannel(channelName);
        assert(channel != NULL);
        jcChanMap[channelName] = channel;
    }
    unsigned int joinCount = channel->advanceChannelJoin(theClient);

    if (joinCount == jcCutoff) {
        log(WARN, "%s is being join flooded.", channelName.c_str());
        // if the channel was not in flooded state, clear its join record
        channel->setJoinFlooded(true);
    }
    if (channel->getJoinFlooded()) {
        if (joinCount == jcCutoff) {
            for (jfChannel::jClientsVectorIterator jItr = channel->jClientsBegin();
                 jItr != channel->jClientsEnd(); jItr++) {
                iClient* tmpClient = Network->findClient(*jItr);
                if (tmpClient == 0)
                    continue;
                addJoin(channel, tmpClient, joinCount);
            }
        } else
            addJoin(channel, theClient, joinCount);
    }
}

time_t dronescan::getRoundedUnixTime(time_t ts, unsigned int power) {
    ts >>= power;
    ts <<= power;
    return ts;
}

void dronescan::addJoin(jfChannel* channel, iClient* theClient, unsigned int joinCount) {
    std::string channelName = channel->getName();
    channel->addJoin(theClient);
    if (joinCount >= jcCutoff) {
        string IP = xIP(theClient->getIP()).GetNumericIP();
        jcFloodClients* jcFC;
        if ((::time(0) - lastBurstTime) >= jcGracePeriodBurstOrSplit && jcGlineEnable) {
            clientsIPFloodMapType::const_iterator Itr = clientsIPFloodMap.find(IP);
            if (Itr != clientsIPFloodMap.end()) {
                jcFC = Itr->second;
                if ((unsigned int)(::time(0) - jcFC->ctime) > jcJoinsPerIPTime) {
                    jcFC->ctime = ::time(0);
                    jcFC->count = 0;
                    jcFC->chans.clear();
                    jcFC->nicks.clear();
                    jcFC->log.clear();
                }
            } else {
                jcFC = new (std::nothrow) jcFloodClients;
                assert(jcFC != NULL);
                jcFC->count = 0;
                jcFC->ctime = ::time(0);
                clientsIPFloodMap[IP] = jcFC;
            }
            jcFC->count++;
            std::list<string>::iterator sItr;
            bool isMatchFound = false;
            for (sItr = jcFC->chans.begin(); sItr != jcFC->chans.end(); sItr++) {
                if (*sItr == channelName) {
                    isMatchFound = true;
                    break;
                }
            }
            if (!isMatchFound)
                jcFC->chans.push_back(channelName);
            isMatchFound = false;
            for (sItr = jcFC->nicks.begin(); sItr != jcFC->nicks.end(); sItr++) {
                if (*sItr == theClient->getNickName()) {
                    isMatchFound = true;
                    break;
                }
            }
            if (!isMatchFound)
                jcFC->nicks.push_back(theClient->getNickName());

            std::stringstream s;
            s << theClient->getNickName() << "!" << theClient->getUserName() << "@"
              << xIP(theClient->getIP()).GetNumericIP() << " " << theClient->getDescription() << " "
              << channelName << " " << ::time(0);
            jcFC->log.push_back(s.str());

            if ((unsigned int)jcFC->count >= jcMinJoinsPerIPToGline) {
                IPJQueue.push_back(IP);
            }
        }
    }
}

void dronescan::handleChannelPart(Channel* theChan, iClient* theClient) {

    /* If this is an exceptional channel, dont test it. */
    if (isExceptionalChannel(theChan->getName()))
        return;

    jcChanMapIterator jcChanIt = jcChanMap.find(theChan->getName());
    if (jcChanIt != jcChanMap.end() && jcChanIt->second->getJoinFlooded()) {
        jcChanIt->second->advanceChannelParts();
        //	if(partCount == pcCutOff)
        //		{
        //		log(DEBUG,"%s is being part flooded.",
        //		    channelName.c_str());
        //		jcChanIt->second->setPartFlooded(true);
        //		}
        //	if(jcChanIt->second->getPartFlooded())
        //		{
        jcChanIt->second->addPart(theClient);
        //		}
    }
}

/** Here we handle new clients as they connect to the network. */
void dronescan::handleNewClient(iClient* theClient) {
    /* First, all new clients must be assigned a clientData */
    clientData* theData = new clientData();
    assert(theClient->setCustomData(this, theData));
    ++customDataCounter;

    string IP = xIP(theClient->getIP()).GetNumericIP();
    /* Store usercount per IPs to a map */
    if (clientsIPMap.find(IP) == clientsIPMap.end())
        clientsIPMap.insert(std::make_pair(IP, 1));
    else
        clientsIPMap[IP]++;

    /* If we are still bursting, calculate letter frequencies */
    if (currentState == BURST) {
        /* Do nothing - this will all get done later */
    } else if (currentState == RUN) {
        setClientState(theClient);
    }
}

void dronescan::handleNickChange(iClient* theClient) {
    for (iClient::const_channelIterator it = theClient->channels_begin();
         it != theClient->channels_end(); ++it) {
        Channel* theChannel = *it;
        /* If this channel is too small, don't test it. */
        if (theChannel->size() < channelCutoff)
            return;

        /* Iterate over our available tests, checking this channel */
        if (droneChannels.find(theChannel->getName()) == droneChannels.end()) {
            /* This channel is not currently listed as active */
            // Check the channel for abnormalities, if enough
            // are found then it will be added to the
            // droneChannels structure by checkChannel()
            checkChannel(theChannel);
        }

        /* Reset lastnick on the active channel */
        droneChannelsType::iterator droneChanItr = droneChannels.find(theChannel->getName());

        // If the channel is still not in the droneChannels
        // structure then it is a "normal" channel
        if (droneChanItr != droneChannels.end()) {
            droneChanItr->second->setLastNick(::time(0));
        }

        /* Do nick count processing if applicable */
        const string& channelName = theChannel->getName();

        ncChanMap[channelName]++;

        unsigned int nickCount = ncChanMap[channelName];

        if (nickCount == ncCutoff) {
            log(WARN, "%s is being nick flooded.", channelName.c_str());
        }
    }
}

/** Calculate the global entropy. */
void dronescan::calculateEntropy() {
    /* Zero out everything */
    charMap.clear();
    averageEntropy = 0;
    totalNicks = 0;

    theTimer->Start();

    elog << "dronescan::calculateEntropy> Calculating frequencies." << std::endl;
    /* First, learn the entropy from all nicks */
    for (xNetwork::const_clientIterator ptr = Network->clients_begin();
         ptr != Network->clients_end(); ++ptr) {
        const char* nickPtr = ptr->second->getNickName().c_str();
        for (; *nickPtr; ++nickPtr) {
            charMap[*nickPtr]++;
        }
        ++totalNicks;
    }

    log(DBG, "Calculated frequencies in: %u ms", theTimer->stopTimeMS());

    theTimer->Start();

    elog << "dronescan::calculateEntropy> Normalising frequencies." << std::endl;
    for (charMapType::iterator itr = charMap.begin(); itr != charMap.end(); ++itr)
        itr->second /= totalNicks;

    log(DBG, "Normalised frequencies in: %u ms", theTimer->stopTimeMS());

    elog << "dronescan::calculateEntropy> Calculating average entropy." << std::endl;

    double totalEntropy = 0;

    theTimer->Start();

    for (xNetwork::const_clientIterator ptr = Network->clients_begin();
         ptr != Network->clients_end(); ++ptr) {
        if (ptr->second->isModeK())
            continue;
        totalEntropy += calculateEntropy(ptr->second->getNickName());
    }

    log(DBG, "Total entropy  : %lf", totalEntropy);
    log(DBG, "Total nicks    : %u", totalNicks);

    averageEntropy = totalEntropy / totalNicks;
    log(DBG, "Average entropy: %lf ", averageEntropy);

    log(DBG, "Found entropy in: %u ms", theTimer->stopTimeMS());
}

/** Return the entropy of a given client. */
double dronescan::calculateEntropy(const iClient* theClient) {
    clientData* theData = static_cast<clientData*>(theClient->getCustomData(this));
    if (0 == theData)
        return 0.0;

    return theData->getEntropy();
}

/** Calculate state of all nicks. */
void dronescan::setNickStates() {
    elog << "dronescan::setNickStates> Finding states of all nicks." << std::endl;

    theTimer->Start();

    /* Now we must assign a state to each nick we see */
    for (xNetwork::const_clientIterator ptr = Network->clients_begin();
         ptr != Network->clients_end(); ++ptr) {
        setClientState(ptr->second);
    }

    log(DBG, "Set all nick states in: %u ms", theTimer->stopTimeMS());
}

/** Check global channels for drones. */
void dronescan::checkChannels() {
    log(INFO, "Checking channels for drones:");

    unsigned int noChannels = 0;
    unsigned int noDrones = 0;
    unsigned int failure = 0;

    theTimer->Start();

    xNetwork::const_channelIterator ptr = Network->channels_begin();

    for (; ptr != Network->channels_end(); ++ptr) {
        ++noChannels;

        if (ptr->second->size() < channelCutoff)
            continue;

        if (!checkChannel(ptr->second)) {
            ++failure;
            noDrones += ptr->second->size();
        }
    }

    log(INFO,
        "Finished checking %u channels. %u/%u total possible channels/clients. Duration: %u ms",
        noChannels, failure, noDrones, theTimer->stopTimeMS());
}

/** Check a channel for drones. */
bool dronescan::checkChannel(const Channel* theChannel, const iClient* theClient) {
    unsigned short int failed = 0;

    /* Iterate over the tests. */
    for (testMapType::iterator testItr = testMap.begin(); testItr != testMap.end(); ++testItr) {
        bool hasPassed = testItr->second->isNormal(theChannel);

        if (theClient) {
            Reply(theClient, "%20s: %s", testItr->second->getName().c_str(),
                  hasPassed ? "PASSED" : "FAILED");
        }

        if (!hasPassed)
            failed += testItr->second->getWeight();
    }

    /* If we were checking for a client, don't output to console. */
    if (theClient)
        return true;

    /* If the failure count is over or equal to the vote cutoff
     * report this channel as abnormal. */

    if (failed >= voteCutoff) {
        /* This channel is voted abnormal. */
        std::stringstream chanStat, chanParams;
        if (theChannel->getMode(Channel::MODE_I))
            chanStat << "i";
        if (theChannel->getMode(Channel::MODE_R))
            chanStat << "r";

        if (theChannel->getMode(Channel::MODE_K)) {
            chanStat << "k";
            chanParams << theChannel->getKey();
        }
        if (theChannel->getMode(Channel::MODE_L)) {
            chanStat << "l";
            if (theChannel->getMode(Channel::MODE_K))
                chanParams << " ";
            chanParams << theChannel->getLimit();
        }

        std::stringstream s;
        char buf[512];

        // s << "[" << failed << "] ("
        //	<<
        snprintf(buf, 511, "[%u] (%4u) %s +%s %s", failed, (unsigned int)theChannel->size(),
                 theChannel->getName().c_str(), chanStat.str().c_str(), chanParams.str().c_str());
        buf[511] = 0;
        s << buf;
        int spaces = 40 - s.str().size();
        for (int i = 0; i < spaces; i++) {
            s << " ";
        }
        s << "  (" << Ago(theChannel->getCreationTime()) << ")";
        log(WARN, s.str().c_str());

        /*
        log(WARN, "[%u] (%4u) %s +%s %s    (%s old)",
                failed,
                theChannel->size(),
                theChannel->getName().c_str(),
                chanStat.str().c_str(),
                chanParams.str().c_str(),
                Ago(theChannel->getCreationTime())
                );
        */
        /* Add this channel to the actives list */
        activeChannel* newActive = new activeChannel(theChannel->getName(), ::time(0));
        droneChannels[theChannel->getName()] = newActive;

        return false;
    } else {
        /* This channel is voted normal. */
        return true;
    }
}

/** Calculate the entropy of a given string. */
double dronescan::calculateEntropy(const string& theString) {
    /* If we have not yet calibrated, we cannot calculate an entropy */
    assert(!charMap.empty());

    double entropy = 0;
    const char* ptr = theString.c_str();

    for (; *ptr; ++ptr)
        entropy += charMap[*ptr];

    return entropy / theString.length();
}

/** Check whether an iClient's nick is `normal'. */
bool dronescan::isNormal(const iClient* theClient) {
    /* We should never see this called during BURST */
    assert(currentState != BURST);

    clientData* theData = static_cast<clientData*>(theClient->getCustomData(this));
    assert(theData != 0);

    /* We should never see an unknown state for an initialised client */
    assert(!theData->isUnknown());

    return theData->isNormal();
}

/** Set the iClient's state depending on certain features. */
CLIENT_STATE dronescan::setClientState(iClient* theClient) {
    clientData* theData = static_cast<clientData*>(theClient->getCustomData(this));
    assert(theData != 0);

    double userEntropy = calculateEntropy(theClient->getNickName());
    theData->setEntropy(userEntropy);

    /* Make sure our services don't skew anything */
    if (theClient->isModeK())
        return theData->setState(NORMAL);

    /*
     * First, check if the first 8 chars of the realname are the same as the
     * first 8 chars of the nickname.
     */
    if (strncmp(theClient->getNickName().c_str(), theClient->getDescription().c_str(), 8) == 0) {
        return theData->setState(ABNORMAL);
    }

    /*
     * Second, check whether the nickname itself is abnormal.
     */
    if (userEntropy > averageEntropy * (1 + nickMargin) ||
        userEntropy < averageEntropy * (1 - nickMargin))
        return theData->setState(ABNORMAL);

    /*
     * It has passed the checks. It is therefore normal.
     */
    return theData->setState(NORMAL);
}

/** Log a message. */
void dronescan::log(LOG_TYPE logType, const char* format, ...) {
    if (logType < consoleLevel)
        return;

    std::stringstream newMessage;

    switch (logType) {
    case DBG:
        newMessage << "[D] ";
        break;
    case INFO:
        newMessage << "[I] ";
        break;
    case WARN:
        newMessage << "[W] ";
        break;
    case ERR:
        newMessage << "[E] ";
        break;
    default:
        newMessage << "[U] ";
        break;
    }

    char buffer[512] = {0};
    va_list _list;

    va_start(_list, format);
    vsnprintf(buffer, 512, format, _list);
    va_end(_list);

    newMessage << buffer;

    Message(consoleChannel, "%s", newMessage.str().c_str());
}

#ifdef ENABLE_LOG4CPLUS
void dronescan::log(const char* cat, const char* format, ...) {
    char buffer[512] = {0};
    va_list _list;

    va_start(_list, format);
    vsnprintf(buffer, 512, format, _list);
    va_end(_list);

    log4cplus::Logger::getInstance(cat).log(log4cplus::INFO_LOG_LEVEL, buffer);
}
#endif

/** Set the topic of the console channel. */
void dronescan::setConsoleTopic() {
    std::stringstream setTopic;
    setTopic << getCharYYXXX() << " T " << consoleChannel << " :";

    setTopic << "  ||"
             << "  channelMargin: " << channelMargin << "  nickMargin: " << nickMargin
             << "  channelCutoff: " << channelCutoff;

    setTopic << "  ||"
             << "  jcInterval: " << jcInterval << "  jcCutoff: " << jcCutoff
             << "  ncInterval: " << ncInterval << "  ncCutoff: " << ncCutoff;

    Write(setTopic);
}

/** Reply to an iClient. */
void dronescan::Reply(const iClient* theClient, const char* format, ...) {
    char buffer[512] = {0};
    va_list _list;

    va_start(_list, format);
    vsnprintf(buffer, 512, format, _list);
    va_end(_list);

    Message(theClient, buffer);
}

sqlUser* dronescan::getSqlUser(const string& theNick) {
    userMapType::const_iterator itr = userMap.find(theNick);

    if (itr != userMap.end()) {
        return itr->second;
    }

    return 0;
}

/** Are we due an update? */
bool dronescan::updateDue(string _table) {
    std::stringstream check;
    check << "SELECT max(last_updated) FROM " << _table;

    if (!SQLDb->Exec(check, true))
    //	if( PGRES_TUPLES_OK != status )
    {
        doSqlError(check.str(), SQLDb->ErrorMessage());
        return false;
    }

    time_t maxUpdated = atoi(SQLDb->GetValue(0, 0));

    if (maxUpdated > lastUpdated[_table])
        return true;

    return false;
}

/** Preload the fake clients cache */
void dronescan::preloadFakeClientCache() {
    /* Are we due to update? */
    if (!updateDue("FAKECLIENTS"))
        return;

    std::stringstream theQuery;
    theQuery << sql::fakeclients;

    if (!SQLDb->Exec(theQuery, true)) {
        doSqlError(theQuery.str(), SQLDb->ErrorMessage());
        return;
    }

    for (fcMapType::iterator itr = fakeClients.begin(); itr != fakeClients.end(); ++itr) {
        delete itr->second;
    }

    fakeClients.clear();

    /*
            string yyxxx( MyUplink->getCharYY() + "]]]" );
    */

    for (unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
        sqlFakeClient* newFake = new sqlFakeClient(SQLDb);
        assert(newFake != 0);

        newFake->setAllMembers(i);
        fakeClients.insert(fcMapType::value_type(newFake->getId(), newFake));

        /*
                        iClient *fakeClient = new iClient(
                                MyUplink->getIntYY(),
                                yyxxx,
                                newFake->getNickName(),
                                newFake->getUserName(),
                                "AKAQEK",
                                newFake->getHostName(),
                                newFake->getHostName(),
                                "+i",
                                string(),
                                0,
                                newFake->getRealName(),
                                ::time(0)
                                );

                        assert( fakeClient != 0 );

                        MyUplink->AttachClient( fakeClient );
        */
    }

    elog << "dronescan::preloadFakeClientCache> Loaded " << fakeClients.size() << " fake clients."
         << std::endl;
    log(INFO, "Loaded %u fake clients.", fakeClients.size());
}

/** Preload the users cache */
void dronescan::preloadUserCache() {
    /* Are we due to update? */
    if (!updateDue("USERS"))
        return;

    std::stringstream theQuery;
    theQuery << "SELECT user_name,last_seen,last_updated_by,last_updated,flags,access,created "
             << "FROM users";

    if (SQLDb->Exec(theQuery, true))
    //	if(PGRES_TUPLES_OK == status)
    {
        /* First we need to clear the current cache. */
        for (userMapType::iterator itr = userMap.begin(); itr != userMap.end(); ++itr) {
            delete itr->second;
        }
        userMap.clear();

        for (unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
            sqlUser* newUser = new sqlUser(SQLDb);
            assert(newUser != 0);

            newUser->setAllMembers(i);
            userMap.insert(userMapType::value_type(newUser->getUserName(), newUser));
        }
    } else {
        elog << "dronescan::preloadUserCache> " << SQLDb->ErrorMessage();
    }

    elog << "dronescan::preloadUserCache> Loaded " << userMap.size() << " users." << std::endl;
}

bool dronescan::preloadExceptionalChannels() {
    std::stringstream theQuery;
    theQuery << "SELECT name FROM exceptionalChannels";

    // if(PGRES_TUPLES_OK == status) {
    if (SQLDb->Exec(theQuery, true)) {
        /* First we need to clear the current cache. */
        exceptionalChannels.clear();
        for (unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
            exceptionalChannels.push_back(SQLDb->GetValue(i, 0));
        }
    } else {
        elog << "dronescan::preloadExceptionalChannels> " << SQLDb->ErrorMessage();
        return false;
    }

    elog << "dronescan::preloadExceptionalChannels> Loaded " << exceptionalChannels.size()
         << " exceptional channels." << std::endl;
    return true;
}

/** Reload all spam detection caches. */
void dronescan::refreshSpamCaches()
{
    // Free compiled PCRE2 regexes before reloading events
    for (spamRegexCacheType::iterator it = spamRegexCache.begin();
         it != spamRegexCache.end(); ++it) {
        if (it->second)
            pcre2_code_free(it->second);
    }
    spamRegexCache.clear();

    // Free compiled repeat_exclusion_regex patterns
    for (spamRegexCacheType::iterator it = spamRepeatExclusionCache.begin();
         it != spamRepeatExclusionCache.end(); ++it) {
        if (it->second)
            pcre2_code_free(it->second);
    }
    spamRepeatExclusionCache.clear();

    // Drop repeat-tracking state so it doesn't reference stale events
    repeatTrackMap.clear();

    preloadSpamEvents();
    preloadSpamRules();
    preloadSpamActions();
    preloadSpamRuleEvents();
    preloadSpamRuleActions();
    preloadSpamExclusions();
    preloadSpyClients();
    preloadMonitoredChannels();
    preloadSpamRuleChannels();

    relinkSpamGraph();

    // Resync live spy clients against the freshly loaded DB caches.
    // MyUplink is NULL during the constructor ? only sync after network attach.
    if (MyUplink)
        resyncSpyClients();
}

/**
 * relinkSpamGraph: rebuild the pointer-linked spam object graph from the
 * currently loaded id-keyed maps (spamRuleEventsMap, spamRuleActionsMap)
 * and object maps (spamEventsMap, spamRulesMap, spamActionsMap).
 *
 * Full rebuild rather than incremental link/unlink: cheap (bounded by the
 * number of event/rule/action bindings, which is admin-managed and small),
 * and self-healing - any id in spamRuleEventsMap/spamRuleActionsMap that no
 * longer resolves in the corresponding object map (e.g. the referenced
 * event/rule/action was just deleted) is simply skipped, so no stale
 * pointers survive a rebuild.
 *
 * Must be called after any structural change to the graph: on load (via
 * refreshSpamCaches()), and after EVENT/RULE/ACTION ADD or DEL and
 * ADDEVENT/REMEVENT/ADDACTION/REMACTION in SPAMCommand.cc. Deliberately NOT
 * called for SET enabled - enabled state is checked live via isEnabled() by
 * callers of getRules()/getEvents()/getActions(), so toggling it needs no
 * relink.
 */
void dronescan::relinkSpamGraph()
{
    for (spamEventsMapType::const_iterator it = spamEventsMap.begin();
         it != spamEventsMap.end(); ++it)
        it->second->clearRules();

    for (spamRulesMapType::const_iterator it = spamRulesMap.begin();
         it != spamRulesMap.end(); ++it) {
        it->second->clearEvents();
        it->second->clearActions();
    }

    // spam_rule_events: link rule<->event both ways, unconditional on
    // enabled state.
    for (spamRuleEventsMapType::const_iterator rit = spamRuleEventsMap.begin();
         rit != spamRuleEventsMap.end(); ++rit) {
        spamRulesMapType::const_iterator ruleIt = spamRulesMap.find(rit->first);
        if (ruleIt == spamRulesMap.end())
            continue;
        sqlSpamRule* rule = ruleIt->second;

        const std::vector<std::pair<int,int>>& bindings = rit->second;
        for (size_t i = 0; i < bindings.size(); ++i) {
            const int event_id       = bindings[i].first;
            const int points_override = bindings[i].second;

            spamEventsMapType::const_iterator eventIt = spamEventsMap.find(event_id);
            if (eventIt == spamEventsMap.end())
                continue;
            sqlSpamEvent* event = eventIt->second;

            rule->addEvent(event, points_override);
            event->addRule(rule);
        }
    }

    // spam_rule_actions: resolve each binding's action pointer and link it
    // to its rule, unconditional on enabled state.
    for (spamRuleActionsMapType::const_iterator rit = spamRuleActionsMap.begin();
         rit != spamRuleActionsMap.end(); ++rit) {
        spamRulesMapType::const_iterator ruleIt = spamRulesMap.find(rit->first);
        if (ruleIt == spamRulesMap.end())
            continue;
        sqlSpamRule* rule = ruleIt->second;

        const std::vector<sqlSpamRuleAction*>& bindings = rit->second;
        for (size_t i = 0; i < bindings.size(); ++i) {
            sqlSpamRuleAction* ra = bindings[i];

            spamActionsMapType::const_iterator actionIt = spamActionsMap.find(ra->getActionId());
            if (actionIt == spamActionsMap.end()) {
                ra->setAction(nullptr);
                continue;
            }
            ra->setAction(actionIt->second);
            rule->addAction(ra);
        }
    }

    // Flat vector of every loaded event, for hot-path iteration.
    spamEventsList.clear();
    spamEventsList.reserve(spamEventsMap.size());
    for (spamEventsMapType::const_iterator it = spamEventsMap.begin();
         it != spamEventsMap.end(); ++it)
        spamEventsList.push_back(it->second);
}

/**
 * compileEventRegex: (re)compile the TEXT event_param regex for ev into
 * spamRegexCache, freeing any previously compiled pattern for this event
 * id first. No-op (after freeing) for non-TEXT events or an empty param.
 * Called on load, on EVENT ADD, and on EVENT SET (event_param or
 * case_sensitive) so the compiled pattern never goes stale.
 */
void dronescan::compileEventRegex(sqlSpamEvent* ev)
{
    spamRegexCacheType::iterator ci = spamRegexCache.find(ev->getId());
    if (ci != spamRegexCache.end()) {
        pcre2_code_free(ci->second);
        spamRegexCache.erase(ci);
    }

    if (ev->getEventType() != "TEXT" || ev->getEventParam().empty())
        return;

    int errcode;
    PCRE2_SIZE erroffset;
    uint32_t flags = ev->isCaseSensitive() ? 0 : PCRE2_CASELESS;
    pcre2_code* re = pcre2_compile(
        reinterpret_cast<PCRE2_SPTR>(ev->getEventParam().c_str()),
        PCRE2_ZERO_TERMINATED, flags, &errcode, &erroffset, nullptr);
    if (re)
        spamRegexCache[ev->getId()] = re;
    else
        elog << "dronescan::compileEventRegex> PCRE2 compile failed for event "
             << ev->getId() << " at offset " << erroffset << std::endl;
}

/**
 * compileRepeatExclusionRegex: same as compileEventRegex, but for the
 * TEXT_REPEAT repeat_exclusion_regex / spamRepeatExclusionCache pair.
 */
void dronescan::compileRepeatExclusionRegex(sqlSpamEvent* ev)
{
    spamRegexCacheType::iterator ci = spamRepeatExclusionCache.find(ev->getId());
    if (ci != spamRepeatExclusionCache.end()) {
        pcre2_code_free(ci->second);
        spamRepeatExclusionCache.erase(ci);
    }

    if (ev->getEventType() != "TEXT_REPEAT" || ev->getRepeatExclusionRegex().empty())
        return;

    int errcode;
    PCRE2_SIZE erroffset;
    uint32_t flags = ev->isCaseSensitive() ? 0 : PCRE2_CASELESS;
    pcre2_code* re = pcre2_compile(
        reinterpret_cast<PCRE2_SPTR>(ev->getRepeatExclusionRegex().c_str()),
        PCRE2_ZERO_TERMINATED, flags, &errcode, &erroffset, nullptr);
    if (re)
        spamRepeatExclusionCache[ev->getId()] = re;
    else
        elog << "dronescan::compileRepeatExclusionRegex> repeat_exclusion_regex compile "
                "failed for event " << ev->getId() << " at offset " << erroffset << std::endl;
}

/**
 * freeEventRegexes: free and erase any compiled regex cache entries for
 * event_id. Called on EVENT DEL so deleted events don't leak pcre2_code*.
 */
void dronescan::freeEventRegexes(int event_id)
{
    spamRegexCacheType::iterator ci = spamRegexCache.find(event_id);
    if (ci != spamRegexCache.end()) {
        pcre2_code_free(ci->second);
        spamRegexCache.erase(ci);
    }

    spamRegexCacheType::iterator xi = spamRepeatExclusionCache.find(event_id);
    if (xi != spamRepeatExclusionCache.end()) {
        pcre2_code_free(xi->second);
        spamRepeatExclusionCache.erase(xi);
    }
}

void dronescan::preloadSpamEvents()
{
    for (spamEventsMapType::iterator it = spamEventsMap.begin(); it != spamEventsMap.end(); ++it)
        delete it->second;
    spamEventsMap.clear();

    std::stringstream q;
    q << "SELECT id, name, description, event_type, event_param, target, "
      << "case_sensitive, points, point_expiry, max_occurrence, "
      << "requires_event_id, enabled, "
      << "repeat_crossuser, repeat_min_count, repeat_exclusion_regex, "
      << "created_ts, modified_ts, modified_by "
      << "FROM spam_events ORDER BY id";

    if (!SQLDb->Exec(q, true)) {
        doSqlError(q.str(), SQLDb->ErrorMessage());
        return;
    }
    for (unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
        sqlSpamEvent* ev = new sqlSpamEvent(SQLDb);
        ev->setAllMembers(i);
        spamEventsMap[ev->getId()] = ev;

        compileEventRegex(ev);
        compileRepeatExclusionRegex(ev);
    }
    elog << "dronescan::preloadSpamEvents> Loaded " << spamEventsMap.size()
         << " spam events." << std::endl;
}

void dronescan::preloadSpamRules()
{
    for (spamRulesMapType::iterator it = spamRulesMap.begin(); it != spamRulesMap.end(); ++it)
        delete it->second;
    spamRulesMap.clear();

    std::stringstream q;
    q << "SELECT id, name, description, threshold, wait_on_rule_id, "
      << "allchans, points_per, score_globally, "
      << "enabled, created_ts, modified_ts, modified_by "
      << "FROM spam_rules ORDER BY id";

    if (!SQLDb->Exec(q, true)) {
        doSqlError(q.str(), SQLDb->ErrorMessage());
        return;
    }
    for (unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
        sqlSpamRule* rule = new sqlSpamRule(SQLDb);
        rule->setAllMembers(i);
        spamRulesMap[rule->getId()] = rule;
    }
    elog << "dronescan::preloadSpamRules> Loaded " << spamRulesMap.size()
         << " spam rules." << std::endl;
}

void dronescan::preloadSpamActions()
{
    for (spamActionsMapType::iterator it = spamActionsMap.begin(); it != spamActionsMap.end(); ++it)
        delete it->second;
    spamActionsMap.clear();

    std::stringstream q;
    q << "SELECT id, name, action_type, duration, reason, delay, rand_min, rand_max, "
      << "enabled, created_ts, modified_ts, modified_by "
      << "FROM spam_actions ORDER BY id";

    if (!SQLDb->Exec(q, true)) {
        doSqlError(q.str(), SQLDb->ErrorMessage());
        return;
    }
    for (unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
        sqlSpamAction* act = new sqlSpamAction(SQLDb);
        act->setAllMembers(i);
        spamActionsMap[act->getId()] = act;
    }
    elog << "dronescan::preloadSpamActions> Loaded " << spamActionsMap.size()
         << " spam actions." << std::endl;
}

void dronescan::preloadSpamRuleEvents()
{
    spamRuleEventsMap.clear();

    std::stringstream q;
    q << "SELECT rule_id, event_id, points_override "
      << "FROM spam_rule_events ORDER BY rule_id, event_id";

    if (!SQLDb->Exec(q, true)) {
        doSqlError(q.str(), SQLDb->ErrorMessage());
        return;
    }
    unsigned int total = 0;
    for (unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
        int rule_id  = atoi(SQLDb->GetValue(i, 0));
        int event_id = atoi(SQLDb->GetValue(i, 1));
        const string po = SQLDb->GetValue(i, 2);
        int points_override = !po.empty() ? atoi(po.c_str()) : -1;
        spamRuleEventsMap[rule_id].push_back(std::make_pair(event_id, points_override));
        ++total;
    }
    elog << "dronescan::preloadSpamRuleEvents> Loaded " << total
         << " rule-event links." << std::endl;
}

void dronescan::preloadSpamRuleActions()
{
    for (spamRuleActionsMapType::iterator it = spamRuleActionsMap.begin();
         it != spamRuleActionsMap.end(); ++it) {
        for (size_t i = 0; i < it->second.size(); ++i)
            delete it->second[i];
    }
    spamRuleActionsMap.clear();

    std::stringstream q;
    q << "SELECT id, rule_id, action_id, action_type, "
      << "action_duration_override, action_reason_override, delay_override "
      << "FROM spam_rule_actions ORDER BY rule_id, id";

    if (!SQLDb->Exec(q, true)) {
        doSqlError(q.str(), SQLDb->ErrorMessage());
        return;
    }
    unsigned int total = 0;
    for (unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
        sqlSpamRuleAction* ra = new sqlSpamRuleAction(SQLDb);
        ra->setAllMembers(i);
        spamRuleActionsMap[ra->getRuleId()].push_back(ra);
        ++total;
    }
    elog << "dronescan::preloadSpamRuleActions> Loaded " << total
         << " rule-action links." << std::endl;
}

void dronescan::preloadSpamExclusions()
{
    for (spamExclusionsListType::iterator it = spamExclusionsList.begin();
         it != spamExclusionsList.end(); ++it)
        delete *it;
    spamExclusionsList.clear();

    std::stringstream q;
    q << "SELECT id, exclusion_type, value, created_ts, modified_ts, modified_by "
      << "FROM spam_exclusions ORDER BY id";

    if (!SQLDb->Exec(q, true)) {
        doSqlError(q.str(), SQLDb->ErrorMessage());
        return;
    }
    for (unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
        sqlSpamExclusion* ex = new sqlSpamExclusion(SQLDb);
        ex->setAllMembers(i);
        spamExclusionsList.push_back(ex);
    }
    elog << "dronescan::preloadSpamExclusions> Loaded " << spamExclusionsList.size()
         << " spam exclusions." << std::endl;
}

void dronescan::preloadSpyClients()
{
    for (spyClientsMapType::iterator it = spyClientsMap.begin();
         it != spyClientsMap.end(); ++it)
        delete it->second;
    spyClientsMap.clear();

    std::stringstream q;
    q << "SELECT id, nickname, username, hostname, ip, realname, "
      << "account, account_id, modes, enabled, "
      << "created_by, created_ts, modified_ts, modified_by "
      << "FROM spyclients ORDER BY id";

    if (!SQLDb->Exec(q, true)) {
        doSqlError(q.str(), SQLDb->ErrorMessage());
        return;
    }
    for (unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
        sqlSpyClient* sc = new sqlSpyClient(SQLDb);
        sc->setAllMembers(i);
        spyClientsMap[sc->getId()] = sc;
    }
    elog << "dronescan::preloadSpyClients> Loaded " << spyClientsMap.size()
         << " spy clients." << std::endl;
}

void dronescan::preloadMonitoredChannels()
{
    for (monitoredChannelsMapType::iterator it = monitoredChannelsMap.begin();
         it != monitoredChannelsMap.end(); ++it)
        delete it->second;
    monitoredChannelsMap.clear();

    std::stringstream q;
    q << "SELECT id, name, forcejoin, joinasservice, enabled, "
      << "created_ts, modified_ts, modified_by "
      << "FROM monitored_channels ORDER BY id";

    if (!SQLDb->Exec(q, true)) {
        doSqlError(q.str(), SQLDb->ErrorMessage());
        return;
    }
    for (unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
        sqlMonitoredChannel* mc = new sqlMonitoredChannel(SQLDb);
        mc->setAllMembers(i);
        // Key by lowercase channel name for case-insensitive lookups
        string key = string_lower(mc->getName());
        monitoredChannelsMap[key] = mc;
    }
    elog << "dronescan::preloadMonitoredChannels> Loaded " << monitoredChannelsMap.size()
         << " monitored channels." << std::endl;
}

void dronescan::preloadSpamRuleChannels()
{
    spamRuleChannelsMap.clear();

    std::stringstream q;
    q << "SELECT rule_id, channel_name FROM spam_rule_channels ORDER BY rule_id, channel_name";

    if (!SQLDb->Exec(q, true)) {
        doSqlError(q.str(), SQLDb->ErrorMessage());
        return;
    }
    unsigned int total = 0;
    for (unsigned int i = 0; i < SQLDb->Tuples(); ++i) {
        int rule_id           = atoi(SQLDb->GetValue(i, 0).c_str());
        string channel_name   = SQLDb->GetValue(i, 1);
        spamRuleChannelsMap[rule_id].push_back(channel_name);
        ++total;
    }
    elog << "dronescan::preloadSpamRuleChannels> Loaded " << total
         << " rule-channel entries." << std::endl;
}

// ---------------------------------------------------------------------------
// Spam processing helpers
// ---------------------------------------------------------------------------

/**
 * makeActor: snapshot a client's identity so scoring/actions still work even
 * if the client later quits (needed for crossuser TEXT_REPEAT). host is the
 * real host, not the +x hidden host.
 */
dronescan::SpamActor dronescan::makeActor(iClient* theClient) const
{
    SpamActor a;
    a.numeric = theClient->getCharYYXXX();
    a.ip      = xIP(theClient->getIP()).GetNumericIP();
    a.nick    = theClient->getNickName();
    a.user    = theClient->getUserName();
    a.host    = theClient->getRealInsecureHost();
    return a;
}

/**
 * scoreEvent: award one occurrence of a matched event to an actor, updating
 * the per-rule scoring buckets. Shared by TEXT and TEXT_REPEAT.
 */
void dronescan::scoreEvent(sqlSpamEvent* ev, const SpamActor& actor,
                           const std::string& channel_name, time_t now,
                           const std::string& text)
{
    const std::vector<sqlSpamRule*>& rules = ev->getRules();
    for (size_t i = 0; i < rules.size(); ++i) {
        sqlSpamRule* rule = rules[i];
        if (!rule->isEnabled())
            continue;

        const string key = buildScoringKey(rule, actor, channel_name);
        SpamScore& score = spamScoreMap[key][ev->getId()];
        if (now - score.window_start > ev->getPointExpiry()) {
            score.count        = 0;
            score.window_start = now;
        }
        elog << "dronescan::scoreEvent> Updating score for event " << ev->getId()
             << " rule " << rule->getId()
             << " (count=" << score.count << ", window_start=" << score.window_start
             << ", point_expiry=" << ev->getPointExpiry() << ")" << std::endl;
        int maxOcc = ev->getMaxOccurrence();
        if (maxOcc < 0 || score.count < maxOcc)
            ++score.count;
        score.last_text    = text;
        score.last_text_ts = now;
    }
}

/**
 * processRepeatEvent: TEXT_REPEAT detection. Tracks identical text per
 * channel/target scope; fires (scores) once repeat_min_count is reached and
 * keeps firing for every subsequent repeat within the window. The tracking
 * entry is NOT erased on fire, so later repeaters keep being caught.
 * For crossuser events, every involved participant is awarded on each fire,
 * and each participant's identity is retained so actions still work after a
 * participant quits.
 */
void dronescan::processRepeatEvent(sqlSpamEvent* ev, const SpamActor& actor,
                                   const std::string& text, const std::string& channel_name,
                                   time_t now, std::map<std::string, SpamActor>& actorsToEvaluate)
{
    // repeat_exclusion_regex: never track text matching this pattern
    spamRegexCacheType::const_iterator xit = spamRepeatExclusionCache.find(ev->getId());
    if (xit != spamRepeatExclusionCache.end() && xit->second) {
        pcre2_match_data* mdata = pcre2_match_data_create_from_pattern(xit->second, nullptr);
        int rc = pcre2_match(xit->second,
                             reinterpret_cast<PCRE2_SPTR>(text.c_str()),
                             text.size(), 0, 0, mdata, nullptr);
        pcre2_match_data_free(mdata);
        if (rc >= 0)
            return;  // excluded from repeat tracking
    }

    const bool   crossuser = ev->isRepeatCrossUser();
    const string cmp       = ev->isCaseSensitive() ? text : string_lower(text);
    const string scope     = channel_name.empty() ? string("privmsg")
                                                   : string_lower(channel_name);

    const char SEP = '\x1f';
    string key = std::to_string(ev->getId()) + SEP + scope + SEP;
    if (!crossuser)
        key += actor.numeric + SEP;
    key += cmp;

    RepeatEntry& e = repeatTrackMap[key];
    if (e.window_start == 0 || (now - e.window_start) > ev->getPointExpiry()) {
        e.count        = 0;
        e.window_start = now;
        e.participants.clear();
    }
    e.expires_at = now + ev->getPointExpiry();
    ++e.count;
    if (crossuser)
        e.participants[actor.numeric] = actor;

    if (e.count < ev->getRepeatMinCount())
        return;  // not enough repeats yet

    // Fire: award points, but leave the entry in place for further repeats.
    if (crossuser) {
        for (std::map<std::string, SpamActor>::const_iterator pit = e.participants.begin();
             pit != e.participants.end(); ++pit) {
            scoreEvent(ev, pit->second, channel_name, now, text);
            actorsToEvaluate[pit->first] = pit->second;
        }
    } else {
        scoreEvent(ev, actor, channel_name, now, text);
        // actor is already in actorsToEvaluate (seeded by processSpamText)
    }
}

/**
 * processSpamText: called whenever text arrives that should be checked for
 * spam events. target_bit is the bitmask value for the traffic source
 * (e.g. spam_target::CHAN_PRIV). Handles TEXT (regex) and TEXT_REPEAT
 * (repetition) events; other event types are silently skipped.
 */
void dronescan::processSpamText(iClient* theClient, const std::string& text,
                                int target_bit, const std::string& channel_name)
{
    if (!theClient || currentState != RUN)
        return;

    //elog << "dronescan::processSpamText> Checking text for spam events: " << text << std::endl;
    const time_t now = ::time(0);

    const SpamActor actor = makeActor(theClient);
    // Actors whose rules should be evaluated after scoring. The triggering
    // actor is always evaluated; crossuser repeats add other participants.
    std::map<std::string, SpamActor> actorsToEvaluate;
    actorsToEvaluate[actor.numeric] = actor;

    for (size_t i = 0; i < spamEventsList.size(); ++i)
    {
        sqlSpamEvent* ev = spamEventsList[i];
        if (!ev->isEnabled())
            continue;
        if (!(ev->getTarget() & target_bit))
            continue;

        if (ev->getEventType() == "TEXT_REPEAT") {
            processRepeatEvent(ev, actor, text, channel_name, now, actorsToEvaluate);
            continue;
        }
        if (ev->getEventType() != "TEXT")
            continue;  // other event types implemented in later phases

        //elog << "dronescan::processSpamText> Checking cache" << std::endl;
        spamRegexCacheType::const_iterator rit = spamRegexCache.find(ev->getId());
        if (rit == spamRegexCache.end() || !rit->second)
            continue;

        pcre2_match_data* mdata = pcre2_match_data_create_from_pattern(rit->second, nullptr);
        int rc = pcre2_match(rit->second,
                             reinterpret_cast<PCRE2_SPTR>(text.c_str()),
                             text.size(), 0, 0, mdata, nullptr);
        pcre2_match_data_free(mdata);

        if (rc < 0)
            continue;  // no match (or error)

        scoreEvent(ev, actor, channel_name, now, text);
    }

    for (std::map<std::string, SpamActor>::const_iterator ait = actorsToEvaluate.begin();
         ait != actorsToEvaluate.end(); ++ait)
        evaluateSpamRules(ait->second, channel_name);
}

/**
 * buildScoringKey: the in-memory scoring bucket for a (rule, actor, channel)
 * triple. Format is "rule_id.channel_or_privmsg.unit", or "rule_id.unit" when
 * the rule scores globally (channel segment omitted). unit is the client's
 * numeric nick, unless the rule's points_per is "IP".
 */
std::string dronescan::buildScoringKey(sqlSpamRule* rule, const SpamActor& actor,
                                       const std::string& channel_name) const
{
    const string unit = (rule->getPointsPer() == "IP") ? actor.ip : actor.numeric;

    string key = std::to_string(rule->getId()) + ".";
    if (!rule->isScoreGlobally())
        key += (channel_name.empty() ? "privmsg" : string_lower(channel_name)) + ".";
    key += unit;
    return key;
}

/**
 * evaluateSpamRules: after scores have been updated for an actor, check
 * whether any enabled rule has crossed its threshold.
 */
void dronescan::evaluateSpamRules(const SpamActor& actor, const std::string& channel_name)
{
    const string lcChan     = string_lower(channel_name);
    const time_t now        = ::time(0);

    // Only process channels we are actually monitoring
    if (!channel_name.empty() &&
        monitoredChannelsMap.find(lcChan) == monitoredChannelsMap.end())
        return;

    for (spamRulesMapType::const_iterator rit = spamRulesMap.begin();
         rit != spamRulesMap.end(); ++rit)
    {
        sqlSpamRule* rule = rit->second;
        if (!rule->isEnabled())
            continue;

        // Channel scope check based on allchans flag
        if (!channel_name.empty()) {
            spamRuleChannelsMapType::const_iterator rcit =
                spamRuleChannelsMap.find(rule->getId());
            bool inList = false;
            if (rcit != spamRuleChannelsMap.end()) {
                const std::vector<string>& chans = rcit->second;
                for (size_t i = 0; i < chans.size(); ++i) {
                    if (string_lower(chans[i]) == lcChan) {
                        inList = true;
                        break;
                    }
                }
            }
            if (rule->isAllChans()) {
                // exclusion mode: skip if channel is explicitly excluded
                if (inList)
                    continue;
            } else {
                // inclusion mode: skip if channel is NOT in the list
                if (!inList)
                    continue;
            }
        }

        // Compute total score for this scoring key against this rule
        const std::vector<RuleEventLink>& links = rule->getEvents();

        const string scoringKey = buildScoringKey(rule, actor, channel_name);

        int totalScore = 0;
        std::string triggerText;
        time_t      triggerTs = 0;
        for (size_t i = 0; i < links.size(); ++i) {
            sqlSpamEvent* ev = links[i].event;
            int points_override = links[i].pointsOverride;
            int event_id = ev->getId();

            spamScoreMapType::const_iterator ski = spamScoreMap.find(scoringKey);
            if (ski == spamScoreMap.end())
                continue;
            std::map<int, SpamScore>::const_iterator sii = ski->second.find(event_id);
            if (sii == ski->second.end())
                continue;

            const SpamScore& sc = sii->second;
            // Check if the scoring window has expired
            if ((now - sc.window_start) > ev->getPointExpiry())
                continue;

            int pts = (points_override >= 0) ? points_override : ev->getPoints();
            totalScore += sc.count * pts;

            // Track the most recently matched text among contributing
            // events, to report the line that likely tipped the threshold.
            if (sc.count > 0 && sc.last_text_ts >= triggerTs) {
                triggerTs   = sc.last_text_ts;
                triggerText = sc.last_text;
            }
        }

        if (totalScore >= rule->getThreshold()) {
            fireRuleActions(rule, actor, channel_name, triggerText);

            // Reset scores for all events linked to this rule to prevent
            // immediate re-triggering
            for (size_t i = 0; i < links.size(); ++i) {
                spamScoreMapType::iterator ski = spamScoreMap.find(scoringKey);
                if (ski != spamScoreMap.end())
                    ski->second.erase(links[i].event->getId());
            }
        }
    }
}

namespace {

// Strip control bytes (including IRC formatting codes, all below 0x20) and
// cap length before embedding matched text in a REPORT message. ISO-8859-1
// is single-byte, so no multi-byte handling is required.
std::string sanitizeSpamTextForReport(const std::string& text)
{
    const size_t kMaxLen = 200;
    std::string out;
    out.reserve(std::min(text.size(), kMaxLen));
    for (size_t i = 0; i < text.size() && out.size() < kMaxLen; ++i) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        out += (c < 0x20 || c == 0x7F || (c >= 0x80 && c <= 0x9F)) ? ' ' : static_cast<char>(c);
    }
    if (text.size() > kMaxLen)
        out += "...";
    return out;
}

} // anonymous namespace

/**
 * fireRuleActions: execute all actions linked to the given rule for
 * the offending actor. Handles REPORT and GLINE; KILL is deferred.
 * Uses the captured SpamActor identity so actions still work even if the
 * offender has since quit (crossuser TEXT_REPEAT).
 */
void dronescan::fireRuleActions(sqlSpamRule* rule, const SpamActor& actor,
                                const std::string& channel_name,
                                const std::string& triggerText)
{
    if (!rule)
        return;

    const std::vector<sqlSpamRuleAction*>& actions = rule->getActions();

    const string& nick = actor.nick;
    const string& user = actor.user;
    const string& host = actor.host;
    const string& ip   = actor.ip;

    for (size_t i = 0; i < actions.size(); ++i) {
        sqlSpamRuleAction* ra = actions[i];

        // relinkSpamGraph() only adds a binding to rule->getActions() once
        // its action pointer is resolved, so getAction() is never null here.
        sqlSpamAction* act = ra->getAction();
        if (!act->isEnabled())
            continue;

        const string actionType = ra->getActionType();

        // Determine effective reason
        string reason = ra->getActionReasonOverride().empty()
                        ? act->getReason()
                        : ra->getActionReasonOverride();
        if (reason.empty())
            reason = "Spam detected";

        if (actionType == "REPORT") {
            const std::string sanitizedText = sanitizeSpamTextForReport(triggerText);
            char buf[512];
            if (sanitizedText.empty()) {
                snprintf(buf, sizeof(buf),
                    "SPAM[REPORT] %s!%s@%s (%s) triggered rule '%s' in %s",
                    nick.c_str(), user.c_str(), host.c_str(), ip.c_str(),
                    rule->getName().c_str(),
                    channel_name.empty() ? "(no channel)" : channel_name.c_str());
            } else {
                snprintf(buf, sizeof(buf),
                    "SPAM[REPORT] %s!%s@%s (%s) triggered rule '%s' in %s - text: \"%s\"",
                    nick.c_str(), user.c_str(), host.c_str(), ip.c_str(),
                    rule->getName().c_str(),
                    channel_name.empty() ? "(no channel)" : channel_name.c_str(),
                    sanitizedText.c_str());
            }
            Message(consoleChannel, "%s", buf);

        } else if (actionType == "GLINE") {
            // Determine effective duration
            int duration = (ra->getActionDurationOverride() >= 0)
                           ? ra->getActionDurationOverride()
                           : act->getDuration();
            if (duration < 0)
                duration = 3600;

            glineData* gd = new (std::nothrow)
                glineData("*@" + ip, reason, duration);
            assert(gd != 0);
            glineQueue.push_back(gd);

            char buf[512];
            snprintf(buf, sizeof(buf),
                "SPAM[GLINE] Queued GLINE for %s!%s@%s (%s) ? rule '%s' ? reason: %s",
                nick.c_str(), user.c_str(), host.c_str(), ip.c_str(),
                rule->getName().c_str(), reason.c_str());
            Message(consoleChannel, "%s", buf);
        }
        // KILL deferred to a later phase
    }
}

// ---------------------------------------------------------------------------
// Spy client live management
// ---------------------------------------------------------------------------

/** Returns true if the given iClient is one of our live spy clients. */
bool dronescan::isSpyClient(const iClient* ic) const
{
    for (liveSpyClientsMapType::const_iterator it = liveSpyClientsMap.begin();
         it != liveSpyClientsMap.end(); ++it) {
        if (it->second == ic)
            return true;
    }
    return false;
}

/** Returns the spy client id for the given iClient, or -1. */
int dronescan::getSpyClientId(const iClient* ic) const
{
    for (liveSpyClientsMapType::const_iterator it = liveSpyClientsMap.begin();
         it != liveSpyClientsMap.end(); ++it) {
        if (it->second == ic)
            return it->first;
    }
    return -1;
}

/** Voice a spy client in the console channel. */
void dronescan::voiceSpyClientInConsole(iClient* ic)
{
    Channel* consoleChan = Network->findChannel(consoleChannel);
    if (!consoleChan)
        return;
    Mode(consoleChan, "+v", ic->getCharYYXXX());
}

/** Op a client in the console channel. */
void dronescan::opInConsole(iClient* ic)
{
    Channel* consoleChan = Network->findChannel(consoleChannel);
    if (!consoleChan)
        return;
    Mode(consoleChan, "+o", ic->getCharYYXXX());
}

/**
 * Introduce a single spy client to the network.
 * Handles nick conflicts by trying a random numeric suffix.
 * Returns the iClient* on success, nullptr on failure.
 */
iClient* dronescan::introduceSpyClient(sqlSpyClient* sc)
{
    if (!sc || !sc->isEnabled())
        return nullptr;

    // Already live
    if (liveSpyClientsMap.count(sc->getId()))
        return liveSpyClientsMap[sc->getId()];

    // Determine nick ? fall back to nick+random if taken
    string nick = sc->getNickname();
    if (Network->findClient(nick) != nullptr) {
        // Try to find another spy client's nick that is free
        bool found = false;
        for (spyClientsMapType::const_iterator it = spyClientsMap.begin();
             it != spyClientsMap.end(); ++it) {
            if (it->second == sc) continue;
            if (it->second->isEnabled() && !liveSpyClientsMap.count(it->second->getId())
                && Network->findClient(it->second->getNickname()) == nullptr) {
                // Prefer the other client; the caller (introduceAllSpyClients) will
                // reach it in turn. Skip this one for now.
                found = true;
                break;
            }
        }
        // Try nick+random suffix until an available one is found
        unsigned int attempts = 0;
        while (Network->findClient(nick) != nullptr && attempts < 200) {
            std::ostringstream suffix;
            suffix << sc->getNickname() << (1 + rand() % 99);
            nick = suffix.str();
            ++attempts;
        }
        if (attempts >= 200) {
            Message(consoleChannel,
                    "[SpyClient] Could not find a free nick for spy client id %d (%s) after 200 attempts.",
                    sc->getId(), sc->getNickname().c_str());
            return nullptr;
        }
        if (!found)
            Message(consoleChannel,
                    "[SpyClient] Nick collision for %s ? using %s instead.",
                    sc->getNickname().c_str(), nick.c_str());
    }

    // Compute base64 IP from the stored string
    string base64ip;
    unsigned char ipmask_len;
    irc_in_addr ip;
    if (ipmask_parse(sc->getIp().c_str(), &ip, &ipmask_len)) {
        base64ip = string(xIP(ip).GetBase64IP());
    } else {
        // Not a valid IP ? use a deterministic fallback
        base64ip = "AAAAAA";
        elog << "dronescan::introduceSpyClient> Invalid IP string '"
             << sc->getIp() << "' for spy client " << sc->getId() << endl;
    }

    // YYXXX: the core reassigns the numeric via AttachClient
    string yyxxx(MyUplink->getCharYY() + "]]]");

    iClient* ic = new iClient(
        MyUplink->getIntYY(),
        yyxxx,
        nick,
        sc->getUsername(),
        base64ip,
        sc->getHostname(),
        sc->getHostname(),
        sc->getModes(),
        string(),       // account - set via UserLogin() if needed
        0,              // account_id - set via UserLogin() if needed
        0,              // account_flags
        string(),       // tls_fingerprint
        sc->getRealname(),
        ::time(nullptr));

    if (!MyUplink->AttachClient(ic, this)) {
        delete ic;
        elog << "dronescan::introduceSpyClient> AttachClient failed for spy client "
             << sc->getId() << endl;
        return nullptr;
    }

    if (!sc->getAccount().empty()) {
        MyUplink->UserLogin(ic, sc->getAccount(),
                            static_cast<unsigned int>(sc->getAccountId()), 0, this);
    }

    liveSpyClientsMap[sc->getId()] = ic;

    // Join the configured spy clients channel, if any
    if (!spyClientsChannel.empty())
        MyUplink->JoinChannel(ic, spyClientsChannel);

    elog << "dronescan::introduceSpyClient> Introduced " << nick
         << " (id=" << sc->getId() << ") to the network." << endl;
    return ic;
}

/** Introduce all enabled spy clients that are not yet live. */
void dronescan::introduceAllSpyClients()
{
    srand(static_cast<unsigned int>(::time(nullptr)));
    for (spyClientsMapType::const_iterator it = spyClientsMap.begin();
         it != spyClientsMap.end(); ++it) {
        sqlSpyClient* sc = it->second;
        if (!sc->isEnabled()) continue;
        if (liveSpyClientsMap.count(sc->getId())) continue;
        introduceSpyClient(sc);
    }
}

/** Part a spy client from a monitored channel and update tracking maps. */
void dronescan::partSpyClientFromChannel(int scId, const std::string& chanName)
{
    liveSpyClientsMapType::iterator lit = liveSpyClientsMap.find(scId);
    if (lit == liveSpyClientsMap.end())
        return;

    iClient* ic = lit->second;
    const string chanKey = string_lower(chanName);

    Channel* theChan = Network->findChannel(chanName);
    if (theChan && theChan->findUser(ic))
        MyUplink->PartChannel(ic, chanName, "");

    // Clean up tracking - only this channel; the spy client may still be
    // covering others.
    chanActiveSpyMap.erase(chanKey);
    spyClientChanMapType::iterator sit = spyClientChanMap.find(scId);
    if (sit != spyClientChanMap.end()) {
        sit->second.erase(chanKey);
        if (sit->second.empty())
            spyClientChanMap.erase(sit);
    }
}

/** Detach a live spy client from the network and clean up all maps. */
void dronescan::detachSpyClient(int scId)
{
    liveSpyClientsMapType::iterator lit = liveSpyClientsMap.find(scId);
    if (lit == liveSpyClientsMap.end())
        return;

    iClient* ic = lit->second;

    // Part it from every channel it is monitoring
    spyClientChanMapType::iterator sit = spyClientChanMap.find(scId);
    if (sit != spyClientChanMap.end()) {
        for (std::set<string>::const_iterator cit = sit->second.begin();
             cit != sit->second.end(); ++cit) {
            const string& chanName = *cit;
            Channel* theChan = Network->findChannel(chanName);
            if (theChan && theChan->findUser(ic))
                MyUplink->PartChannel(ic, chanName, "");
            chanActiveSpyMap.erase(chanName);
        }
        spyClientChanMap.erase(sit);
    }

    MyUplink->DetachClient(ic, "Spy client disabled");
    liveSpyClientsMap.erase(lit);
}

/**
 * Find the best available spy client for the given channel.
 * Prefers an idle spy client (covering no channels yet). If none is idle,
 * falls back to the live spy client already covering the fewest channels,
 * so a spy client can end up covering more than one channel once the pool
 * is exhausted rather than leaving the channel unmonitored.
 * Returns spy client id, or -1 only if there is no eligible live spy client
 * at all.
 */
int dronescan::findBestSpyClient(const std::string& chanName, bool forcejoin)
{
    Channel* theChan = Network->findChannel(chanName);

    int    bestBusyId    = -1;
    size_t bestBusyCount = 0;

    for (spyClientsMapType::const_iterator it = spyClientsMap.begin();
         it != spyClientsMap.end(); ++it) {
        sqlSpyClient* sc = it->second;
        if (!sc->isEnabled()) continue;

        // Must be live
        liveSpyClientsMapType::const_iterator lit = liveSpyClientsMap.find(sc->getId());
        if (lit == liveSpyClientsMap.end()) continue;
        iClient* ic = lit->second;

        if (!forcejoin && theChan) {
            // Skip +i (invite-only)
            if (theChan->getMode(Channel::MODE_I)) continue;
            // Skip +k (keyed)
            if (theChan->getMode(Channel::MODE_K)) continue;
            // Skip +l if channel is at capacity
            if (theChan->getMode(Channel::MODE_L) &&
                theChan->size() >= theChan->getLimit()) continue;
            // Skip +r (channel requires an identified user) if this spy
            // client has no services account
            if (theChan->getMode(Channel::MODE_R) && sc->getAccount().empty()) continue;
            // Skip if spy client host is banned
            if (theChan->matchBan(ic->getNickUserHost())) continue;
        }

        spyClientChanMapType::const_iterator cit = spyClientChanMap.find(sc->getId());
        size_t chanCount = (cit != spyClientChanMap.end()) ? cit->second.size() : 0;

        // Idle spy client - take it immediately
        if (chanCount == 0)
            return sc->getId();

        // Otherwise remember the least-loaded busy candidate as a fallback
        if (bestBusyId < 0 || chanCount < bestBusyCount) {
            bestBusyId    = sc->getId();
            bestBusyCount = chanCount;
        }
    }

    // No idle spy client - reuse the least-loaded one already assigned
    // elsewhere, or -1 if there is no eligible live spy client at all.
    return bestBusyId;
}

/**
 * Select the best spy client for the given channel and make it join
 * immediately. Selection happens here (not at schedule time) so it always
 * sees up-to-date per-client channel load.
 */
void dronescan::doSpyClientJoin(const std::string& chanName, bool forcejoin)
{
    // Respect kick-stopped channels
    const string chanKey = string_lower(chanName);
    if (kickStoppedChannels.count(chanKey))
        return;

    // The channel may have been removed/disabled from monitoring since this
    // join was scheduled (e.g. via SPAM MONITORCHAN DEL/DISABLE, or dropped
    // on reload) - treat a stale timer firing for it as a no-op.
    monitoredChannelsMapType::const_iterator mcit = monitoredChannelsMap.find(chanKey);
    if (mcit == monitoredChannelsMap.end() || !mcit->second->isEnabled())
        return;

    int scId = findBestSpyClient(chanName, forcejoin);
    if (scId < 0) {
        Message(consoleChannel,
                "[SpyClient] No available spy client for %s.", chanName.c_str());
        return;
    }

    liveSpyClientsMapType::iterator lit = liveSpyClientsMap.find(scId);
    if (lit == liveSpyClientsMap.end())
        return;

    // Already covering this exact channel - nothing to do. A spy client
    // may otherwise already be covering other channels; that's fine.
    spyClientChanMapType::iterator sit = spyClientChanMap.find(scId);
    if (sit != spyClientChanMap.end() && sit->second.count(chanKey))
        return;

    iClient* ic = lit->second;
    MyUplink->JoinChannel(ic, chanName);
    chanActiveSpyMap[chanKey] = scId;
    spyClientChanMap[scId].insert(chanKey);
}

/**
 * Schedule a spy client join with a random delay in [minDelay, maxDelay] seconds.
 */
void dronescan::scheduleSpyClientJoin(const std::string& chanName, bool forcejoin,
                                      int minDelay, int maxDelay)
{
    if (kickStoppedChannels.count(string_lower(chanName)))
        return;

    int delay = minDelay;
    if (maxDelay > minDelay)
        delay += rand() % (maxDelay - minDelay + 1);

    time_t fireAt = ::time(nullptr) + static_cast<time_t>(delay);
    xServer::timerID tid = MyUplink->RegisterTimer(fireAt, this, nullptr);
    pendingJoinTimers[tid] = std::make_pair(forcejoin, string_lower(chanName));
}

/** Cancel any pending spy-client-join timer(s) scheduled for the given channel. */
void dronescan::cancelPendingJoinTimers(const std::string& chanName)
{
    const string chanKey = string_lower(chanName);
    for (pendingJoinTimersType::iterator it = pendingJoinTimers.begin();
         it != pendingJoinTimers.end(); ) {
        if (it->second.second == chanKey) {
            MyUplink->UnRegisterTimer(it->first, nullptr);
            pendingJoinTimers.erase(it++);
        } else {
            ++it;
        }
    }
}

/**
 * Resync live spy clients against the DB caches after a refresh.
 * - Detach clients that no longer exist in the DB or are disabled.
 * - Introduce newly added / re-enabled clients.
 * - If a removed/disabled client was monitoring a channel, schedule a replacement.
 */
void dronescan::resyncSpyClients()
{
    // Step 1: find live clients that are gone or disabled in DB
    std::vector<int> toDetach;
    std::vector<string> orphanedChans; // channels that lost their spy client

    for (liveSpyClientsMapType::const_iterator it = liveSpyClientsMap.begin();
         it != liveSpyClientsMap.end(); ++it) {
        int scId = it->first;
        bool stillValid = false;
        spyClientsMapType::const_iterator sit = spyClientsMap.find(scId);
        if (sit != spyClientsMap.end() && sit->second->isEnabled())
            stillValid = true;

        if (!stillValid) {
            toDetach.push_back(scId);
            // Record every channel this client was monitoring
            spyClientChanMapType::const_iterator cit = spyClientChanMap.find(scId);
            if (cit != spyClientChanMap.end()) {
                for (std::set<string>::const_iterator sIt = cit->second.begin();
                     sIt != cit->second.end(); ++sIt)
                    orphanedChans.push_back(*sIt);
            }
        }
    }

    for (std::vector<int>::iterator it = toDetach.begin(); it != toDetach.end(); ++it) {
        detachSpyClient(*it);
        Message(consoleChannel, "[SpyClient] Detached spy client id %d (removed or disabled in DB).", *it);
    }

    // Step 2: introduce clients that exist in DB but are not live yet
    for (spyClientsMapType::const_iterator it = spyClientsMap.begin();
         it != spyClientsMap.end(); ++it) {
        if (!it->second->isEnabled()) continue;
        if (liveSpyClientsMap.count(it->first)) continue;
        introduceSpyClient(it->second);
    }

    // Step 3: schedule replacement joins for orphaned channels
    for (std::vector<string>::iterator it = orphanedChans.begin();
         it != orphanedChans.end(); ++it) {
        const string& chanName = *it;
        if (kickStoppedChannels.count(chanName)) continue;

        // Find the monitored channel config
        monitoredChannelsMapType::const_iterator mc = monitoredChannelsMap.find(chanName);
        if (mc == monitoredChannelsMap.end()) continue;
        if (!mc->second->isEnabled()) continue;
        if (mc->second->isJoinAsService()) continue;

        if (findBestSpyClient(chanName, mc->second->isForceJoin()) >= 0)
            scheduleSpyClientJoin(chanName, mc->second->isForceJoin(), 0, 10);
        else
            Message(consoleChannel,
                    "[SpyClient] No available spy client to take over %s after removal.",
                    chanName.c_str());
    }
}

/** Register a new command. */
bool dronescan::RegisterCommand(Command* theCommand) {
    return commandMap.insert(commandPairType(theCommand->getName(), theCommand)).second;
}

/** Register a new test. */
bool dronescan::RegisterTest(Test* theTest) {
    return testMap.insert(testPairType(theTest->getName(), theTest)).second;
}

/** Unregister a test. */
bool dronescan::UnRegisterTest(const string& testName) {
    testMapType::iterator ptr = testMap.find(testName);
    if (ptr == testMap.end())
        return false;

    delete ptr->second;
    testMap.erase(ptr);

    return true;
}

/** Set a test variable. */
Test* dronescan::setTestVariable(const string& var, const string& value) {
    for (testMapType::iterator testItr = testMap.begin(); testItr != testMap.end(); ++testItr) {
        if (testItr->second->setVariable(var, value))
            return testItr->second;
    }

    return 0;
}

bool dronescan::isExceptionalChannel(const string& chanName) {
    exceptionalChannelsType::iterator it = exceptionalChannels.begin();
    for (; it != exceptionalChannels.end(); ++it) {
        if (!strcasecmp((*it).c_str(), chanName.c_str())) {
            return true;
        }
    }
    return false;
}

bool dronescan::addExceptionalChannel(const string& chanName) {
    std::stringstream insertQ;
    insertQ << "INSERT into exceptionalChannels(name) VALUES('" << escapeSQLChars(chanName) << "');"
            << std::ends;

    // ExecStatusType status = SQLDb->Exec(insertQ.str().c_str());
    if (!SQLDb->Exec(insertQ))
    // if (PGRES_COMMAND_OK != status)
    {
        elog << "ERROR while adding exceptionalChannel: " << SQLDb->ErrorMessage() << std::endl;
        return false;
    }
    exceptionalChannels.push_back(chanName);
    return true;
}

bool dronescan::remExceptionalChannel(const string& chanName) {
    std::stringstream insertQ;
    insertQ << "DELETE from exceptionalChannels where name='" << escapeSQLChars(chanName) << "';"
            << std::ends;

    // ExecStatusType status = SQLDb->Exec(insertQ.str().c_str());
    if (!SQLDb->Exec(insertQ))
    // if (PGRES_COMMAND_OK != status)
    {
        elog << "ERROR while removing exceptionalChannel: " << SQLDb->ErrorMessage() << std::endl;
        return false;
    }
    exceptionalChannelsType::iterator it = exceptionalChannels.begin();
    for (; it != exceptionalChannels.end(); ++it) {
        if (!strcasecmp((*it).c_str(), chanName.c_str())) {
            exceptionalChannels.erase(it);
            return true;
        }
    }
    return true;
}

char* dronescan::Duration(long ts) {
    /* express duration in human readable format */
    long duration;
    int days, hours, mins = 0;
    char tmp[16];
    static char ago[250];

    ago[0] = '\0';

    duration = ts;

    days = (duration / 86400);
    duration %= 86400;
    hours = (duration / 3600);
    duration %= 3600;
    mins = (duration / 60);
    duration %= 60;

    if (days > 0) {
        sprintf(tmp, "%dd", days);
        strcat(ago, tmp);
    }
    if (hours > 0) {
        sprintf(tmp, "%dh", hours);
        strcat(ago, tmp);
    }
    if (mins > 0) {
        sprintf(tmp, "%dm", mins);
        strcat(ago, tmp);
    }
    /* only show seconds if we have any (or no other units */
    if ((duration > 0) || (strlen(ago) == 0)) {
        sprintf(tmp, "%ds", (int)duration);
        strcat(ago, tmp);
    }

    return ago;
}

char* dronescan::Ago(long ts) {
    /* express a a timestamp in human readable format */
    long duration;

    duration = (time(NULL) - ts);

    return (Duration(duration));
}

/** Return usage information for a client */
void Command::Usage(const iClient* theClient) {
    bot->Reply(theClient, "SYNTAX: %s", getInfo().c_str());
}

} // namespace ds

} // namespace gnuworld
