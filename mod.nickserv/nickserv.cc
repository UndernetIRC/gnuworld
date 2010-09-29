/**
 * nickserv.cc
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

#include	<sstream>
#include	<iostream>

#include <cstdarg>

#include "gnuworld_config.h"
#include "Network.h"
#include "StringTokenizer.h"

#include "netData.h"
#include "nickserv.h"

namespace gnuworld
{

namespace ns
{
/*
 *  Exported function used by moduleLoader to gain an
 *  instance of this module.
 */

extern "C"
{
  xClient* _gnuwinit(const string& args)
  {
    return new nickserv( args );
  }

}

/**
 * This constructor calls the base class constructor.  The xClient
 * constructor will open the configuration file given and retrieve
 * basic client info (nick/user/host/etc).
 * Any additional processing must be done here.
 */
nickserv::nickserv( const string& configFileName )
 : xClient( configFileName )
{
/* Load the config file */
nickservConfig = new (std::nothrow) EConfig(configFileName);
assert(nickservConfig != 0);

/* Load in the values we need from the config file */
consoleChannel = nickservConfig->Require("consoleChannel")->second;

/* Load in the processing queue variables */
checkFreq = atoi(nickservConfig->Require("checkFreq")->second.c_str());
startDelay = atoi(nickservConfig->Require("startDelay")->second.c_str());

/* Load debugging variables */
consoleLevel = atoi(nickservConfig->Require("consoleLevel")->second.c_str());

/* Load the DB variables */
string dbHost = nickservConfig->Require("dbHost")->second;
string dbPort = nickservConfig->Require("dbPort")->second;
string dbDb = nickservConfig->Require("dbDb")->second;
string dbUser = nickservConfig->Require("dbUser")->second;
string dbPass = nickservConfig->Require("dbPass")->second;
commitFreq = atoi(nickservConfig->Require("commitFreq")->second.c_str());
int commitCount = atoi(nickservConfig->Require("commitCount")->second.c_str());

string dbString = "host=" + dbHost + " port=" + dbPort + " dbname=" + dbDb
  + " user="+dbUser + " password=" + dbPass;

theManager = sqlManager::getInstance(dbString, commitCount);

/* Precache the users */
precacheUsers();

/* Register the commands we want to use */
RegisterCommand(new INFOCommand(this, "INFO", "<nick>"));
RegisterCommand(new INVITECommand(this, "INVITE", ""));
RegisterCommand(new MODUSERCommand(this, "MODUSER", "<nick> [ACCESS] <level>"));
RegisterCommand(new RECOVERCommand(this, "RECOVER", ""));
RegisterCommand(new REGISTERCommand(this, "REGISTER", ""));
RegisterCommand(new RELEASECommand(this, "RELEASE", ""));
RegisterCommand(new SETCommand(this, "SET", "<property> <value>"));
RegisterCommand(new SHUTDOWNCommand(this, "SHUTDOWN", "<reason>"));
RegisterCommand(new STATSCommand(this, "STATS", "<stat>"));
RegisterCommand(new WHOAMICommand(this, "WHOAMI", ""));

/* Get our Stats instance */
theStats = Stats::getInstance();

/* Get our logger instance */
theLogger = logging::Logger::getInstance();

/* Register ourselves as a logTarget */
theLogger->addLogger(this);
}

nickserv::~nickserv()
{
delete nickservConfig;
}



/**
 * This is the catcher for log messages. It compares the event type to the
 * cached log event receivers. If it matches, send a notice off to that
 * target.
 */
void nickserv::log(const eventType& theEvent, const string& _theMessage)
{
  iClient* theClient;
  sqlUser* theUser;

  string theMessage = "[" + logging::logTarget::getIdent(theEvent) + "] ";
  theMessage += _theMessage;

  for(logUsersType::iterator ptr = logUsers.begin(); ptr != logUsers.end(); ) {
    theClient = *ptr;
    theUser = isAuthed(theClient);
    if(!theUser) {
      // Something very odd is going on
      elog << "*** [nickserv::log] User in queue but not logged in: "
        << theClient->getNickName() << std::endl;
    }

    if(theUser->getLogMask() & theEvent) {
      Notice(theClient, theMessage);
    }

    ptr++;
  }

  if(consoleLevel & theEvent) {
    logAdminMessage("%s", theMessage.c_str());
  }
}



/**
 * Here we decide what channels our xClient needs to burst in to.
 * The only channel of any interest to us is our console channel, which is
 * loaded from the configuration file.
 */
void nickserv::BurstChannels()
{
MyUplink->JoinChannel(this, consoleChannel, nickservConfig->Require("consoleChannelModes")->second);

MyUplink->RegisterChannelEvent(consoleChannel, this);

std::stringstream setTopic;
setTopic << getCharYYXXX() << " T "
         << consoleChannel << " :"
         << "Current NickServ console level: ["
         << logging::logTarget::getIdent(consoleLevel)
         << "] (" << consoleLevel << ")";
Write(setTopic);

return xClient::BurstChannels() ;
}


/**
 * Here we set up the various bits and pieces that we need an xServer
 * reference to be able to do.
 */
void nickserv::OnAttach()
{
for(commandMapType::iterator ptr = commandMap.begin(); ptr != commandMap.end(); ++ptr) {
  ptr->second->setServer(MyUplink);
}

/* Register for all the events we want to see */
MyUplink->RegisterEvent(EVT_ACCOUNT, this);
MyUplink->RegisterEvent(EVT_CHNICK, this);
MyUplink->RegisterEvent(EVT_KILL, this);
MyUplink->RegisterEvent(EVT_NICK, this);
MyUplink->RegisterEvent(EVT_QUIT, this);

/* Register timerID's to process queues */

/* The timer to kick off the processing of the queue
 * Note: We add startDelay here to allow for a longer delay before checking
 * when we first link to the net
 */
time_t theTime = time(NULL) + startDelay;
processQueue_timerID = MyUplink->RegisterTimer(theTime, this, NULL);

xClient::OnAttach() ;
}


/**
 * Here we deal with any channel events we want to listen to.
 */
void nickserv::OnChannelEvent(const channelEventType& theEvent,
	Channel* theChannel,
	void* data1, void*, void*, void*)
{
iClient* theClient = 0;

switch (theEvent) {
  case EVT_JOIN: {
    if(theChannel->getName() != consoleChannel) {
      theLogger->log(logging::events::E_WARNING, "Received a JOIN for channel: %s.",
                     theChannel->getName().c_str());
      return ;
    }

    theClient = static_cast< iClient* > ( data1 );

    sqlUser* theUser = isAuthed(theClient);
    if(theUser && theUser->getLevel() > 0) Op(theChannel, theClient);

    break;
  } // case EVT_JOIN
} // switch (theEvent)

}

/**
 * Here we deal with the various CTCP messages that can get thrown at us.
 */
void nickserv::OnCTCP( iClient* theClient, const string& CTCP,
                      const string& Message, bool Secure)
{
StringTokenizer st(CTCP);

if(st.empty()) return ;

string Command = string_upper(st[0]);

if("DCC" == Command) {
  DoCTCP(theClient, CTCP, "REJECT");
} else if("PING" == Command) {
  DoCTCP(theClient, CTCP, Message);
} else if("VERSION" == Command) {
  DoCTCP(theClient, CTCP, "GNUWorld NickServ v1.0.5");
}

xClient::OnCTCP(theClient, CTCP, Message, Secure);
}


/**
 * Here we deal with any network events that we have asked to listen for.
 * The main jobs done are:
 *  EVT_ACCOUNT : If the account is the same as the nick, we remove it from
                  the warnQueue should it be there.
 *  EVT_NICK    : Instantiate a netData object and assign it to the iClient.
 *                Add the iClient to the process queue.
 *  EVT_CHNICK  : If the new nick is the current account, do nothing.
 *                If the iClient isnt in the process queue, readd it.
 *                We do NOT zero the warning count. This is to prevent someone
 *                  jumping between registered nicks to avoid getting killed.
 *  EVT_KILL    : Delete the netData instance
 *                Remove the iClient from the process queue
 *  EVT_QUIT    : Delete the netData instance
 *                Remove the iClient from the process queue
 */
void nickserv::OnEvent( const eventType& event,
	void* Data1, void* Data2, void* Data3, void* Data4)
{
/* The target user of the event */
iClient* theClient = static_cast< iClient* >( Data1 );

switch( event ) {
  case EVT_KILL: theClient = static_cast< iClient* >( Data2 );
  case EVT_QUIT: {
    netData* theData = static_cast< netData* >( theClient->removeCustomData(this) );
    delete theData;
    removeFromQueue(theClient);

    logUsersType::iterator ptr = find(logUsers.begin(), logUsers.end(), theClient);
    if(ptr != logUsers.end()) { logUsers.erase(ptr); }

    return ;
    break;
  } // case EVT_KILL/QUIT

  case EVT_NICK: {
    netData* theData = new netData();
    theClient->setCustomData(this, theData);

    /* If this user has umode +r */
    if(theClient->isModeR()) {
      /* Find the sqlUser for their +r and assign it to this iClient */
      theData->authedUser = isRegistered(theClient->getAccount());
      if(theData->authedUser) {
        if(theData->authedUser->getLogMask()) { logUsers.push_back(theClient); }
        theData->authedUser->commitLastSeen();
      }
    }

    addToQueue(theClient);

    return ;
    break;
  } // case EVT_NICK

  case EVT_CHNICK: {
    addToQueue(theClient);

    return ;
    break;
  } // case EVT_CHNICK

  case EVT_ACCOUNT: {
    netData* theData = static_cast< netData* > (theClient->getCustomData(this));

    theData->authedUser = isRegistered(theClient->getAccount());
    if(theData->authedUser) {
      if(theData->authedUser->getLogMask()) { logUsers.push_back(theClient); }
      theData->authedUser->commitLastSeen();
    }

    return ;
    break;
  } // case EVT_ACCOUNT
} // switch( event )

xClient::OnEvent( event, Data1, Data2, Data3, Data4 ) ;
}

/**
 * Here we deal with incoming communications from network clients.
 */
void nickserv::OnPrivateMessage( iClient* theClient,
	const string& Message, bool )
{
if(!theClient->isModeR()) {
  Notice(theClient, "You must be logged in before attempting to use any commands.");
  return ;
}

StringTokenizer st(Message);
if(st.empty()) {
  Notice(theClient, "Incomplete command");
  return ;
}

string Command = string_upper(st[0]);
commandMapType::iterator commHandler = commandMap.find(Command);

if(commHandler == commandMap.end()) {
  return ;
}

commHandler->second->Exec(theClient, Message);
}

/**
 * When a timer expires, this function is called. It allows for periodic
 * processing of data.
 */
void nickserv::OnTimer(const xServer::timerID& theTimer, void* )
{
if(theTimer == processQueue_timerID) {
  processQueue();

  time_t theTime = time(NULL) + checkFreq;
  processQueue_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
  } // if(theTimer == processQueue_timerID)

} // nickserv::OnTimer(xServer::timerID, void*)

/**
 * This is where we register a command so that users can interact
 * with the module.
 */
bool nickserv::RegisterCommand( Command* theCommand )
{
return commandMap.insert( commandPairType(theCommand->getName(), theCommand)).second;
}

/**
 * This function empties the current user cache and reloads all information from
 * the database. It is only called at startup, as the cache is authoritative.
 */
void nickserv::precacheUsers()
{
elog << "*** [NickServ:precacheUsers] Precaching users." << std::endl;

/* Get a connection instance to our backend */
dbHandle* cacheCon = theManager->getConnection();

/* Retrieve the list of registered users */
std::stringstream cacheQuery;
cacheQuery << "SELECT id,name,flags,level,lastseen_ts,registered_ts,logmask"
  << " FROM users";

if(cacheCon->Exec(cacheQuery,true)) {
  for(unsigned int i = 0; i < cacheCon->Tuples(); i++) {
    sqlUser* tmpUser = new sqlUser(theManager);
    assert(tmpUser != 0);

    tmpUser->setAllMembers(cacheCon, i);
    sqlUserCache.insert(sqlUserHashType::value_type(tmpUser->getName(), tmpUser));
  }
} else {
  elog << "*** [NickServ:precacheUsers] Error executing query: "
    << cacheCon->ErrorMessage()
    << std::endl;
  ::exit(0);
}

elog << "*** [NickServ:precacheUsers] Done. Loaded a total of "
  << sqlUserCache.size() << " user records."
  << std::endl;

/* Dispose of our connection instance */
theManager->removeConnection(cacheCon);
}


/**
 * This function simply takes a name/sqlUser* pair and adds them to the cache
 */
void nickserv::addUserToCache(string username, sqlUser* theUser)
{
  sqlUserCache.insert(sqlUserHashType::value_type(username, theUser));
}


/**
 * This function attempts to add a given iClient to the processing queue.
 * It will return 1 if the client did not already exist in the queue, else 0.
 * It is safe to ignore the return value.
 */
int nickserv::addToQueue(iClient* theClient)
{
QueueType::iterator queuePos =
  find(warnQueue.begin(), warnQueue.end(), theClient);
if(queuePos != warnQueue.end()) {
  return 0;
} else {
  warnQueue.push_back(theClient);
  return 1;
}
} // nickserv::addToQueue(iClient*)


/**
 * This function attempts to remove a given iClient from the processing queue.
 * It will 1 if the client was actually found, else 0.
 * It is safe to ignore the return value.
 */
int nickserv::removeFromQueue(iClient* theClient)
{
QueueType::iterator queuePos =
  find(warnQueue.begin(), warnQueue.end(), theClient);
if(queuePos != warnQueue.end()) {
  warnQueue.erase(queuePos);
  return 1;
} else {
  return 0;
}
} // nickserv::removeFromQueue(iClient*)


/**
 * This function iterates through the processing queue and takes any
 * action as appropriate.
 */
void nickserv::processQueue()
{
theStats->incStat("NS.PROCESS");
theLogger->log(logging::events::E_DEBUG, "Processing queue - %u entr%s.",
               warnQueue.size(), (warnQueue.size() == 1) ? ("y") : ("ies"));

/* A number of things can happen here.
 * Firstly, the warnQueue can be empty. If this is the case, return immediately.
 */

if(warnQueue.size() == 0) { return; }

QueueType killQueue;

vector<string> jupeQueue;

/* Now we start to process the queue. The rules are as follows for a given nick:
 *  Is the cached user record set to autokill?
 *    No  - remove entry from warnQueue, continue processing next element
 *  Has the user already been warned?
 *    Yes - add user to killQueue, remove from warnQueue
 *    No  - warn user, increment warnings
 *
 * NOTE: The for() does NOT automatically increment queuePos so we must take care
 *       to do it manually when required. This is to enable us to delete from the
 *       warnQueue while we are iterating over it.
 */

for(QueueType::iterator queuePos = warnQueue.begin(); queuePos != warnQueue.end(); ) {
  iClient* theClient = *queuePos;
  netData* theData = static_cast< netData* >( theClient->getCustomData(this) );

  /* Is this a juped nick? */
  /* TODO: There should be an iClient call for this */
  if(theClient->isFake()) {
    /* This is a juped nick. Let's not try to kill it eh. */
    queuePos = warnQueue.erase(queuePos);
    continue;
  }

  /* Is this nick registered? */
  sqlUser* regUser = isRegistered(theClient->getNickName());
  if(!regUser) {
    theData->warned = 0;
    queuePos = warnQueue.erase(queuePos);
    continue;
  }

  /* Does the regUser have autokill set? */
  if(!regUser->hasFlag(sqlUser::F_AUTOKILL)) {
    theData->warned = 0;
    queuePos = warnQueue.erase(queuePos);
    continue;
  }

  /* User is registered and record has autokill set.
   * See if this iClient and the sqlUser match */
  if(isAccountMatch(theClient, regUser)) {
    theData->warned = 0;
    queuePos = warnQueue.erase(queuePos);
    continue;
  }

  /* We now know that the user is NOT logged in as the user, and the user
   * has AUTOKILL set. Warn them. */

  if(theData->warned) {
    killQueue.push_back(theClient);
    queuePos = warnQueue.erase(queuePos);
    continue;
  } else {
    Notice(theClient, "You are using a registered nickname. Please login or you will be disconnected.");
    theData->warned++;
    theStats->incStat("NS.WARN");
    theLogger->log(logging::events::E_INFO, "Warned: (" +
      theClient->getCharYYXXX() + ") " + theClient->getNickName());
    queuePos++;
    continue;
  } // if(theData->warned)
} // iterate over warnQueue

if(killQueue.empty()) { return; }

for(QueueType::iterator queuePos = killQueue.begin(); queuePos != killQueue.end(); queuePos++) {
  iClient* theClient = *queuePos;

  /* Add the nickname to the juping queue */
  jupeQueue.push_back(theClient->getNickName());

  netData* theData = static_cast<netData*>(theClient->removeCustomData(this));
  delete(theData);
  theStats->incStat("NS.KILL");
  theLogger->log(logging::events::E_INFO, "Killed: (" +
    theClient->getCharYYXXX() + ") " + theClient->getNickName());
  Kill(theClient, "[NickServ] AutoKill");
} // iterate over killQueue

/* Iterate over jupeQueue and do appropriate things */
string fakenumeric(MyUplink->getCharYY() + "]]]");

for( vector<string>::iterator itr = jupeQueue.begin();
     itr != jupeQueue.end() ;
     ++itr ) {

    string theNick = *itr;

    theLogger->log( logging::events::E_DEBUG, "Juping " +
                    theNick
                  );


    /* Set up a fake iClient to represent the new jupe.
     * TODO: The connect time should probably be less so that we win any
     * collision races resulting from a lagging network.
     */
    iClient *fakeClient = new iClient(
      MyUplink->getIntYY(),
      fakenumeric,
      theNick,
      "juped",
      "AAAAAA",
      "nick.name",
      "nick.name",
      "+ikd",
      string(),
      0,
      "Juped Nick",
      ::time( 0 )
    );

    assert( fakeClient != 0 );

    if( ! MyUplink->AttachClient( fakeClient, this ) ) {
      theLogger->log(logging::events::E_INFO, "Unable to jupe: " + theNick);
    }

}

} // nickserv::processQueue()


/**
 * This function simply writes a given message to the console channel.
 */
void nickserv::logAdminMessage(const char* format, ... )
{

char buf[1024] = {0};
va_list _list;

va_start(_list, format);
vsnprintf(buf, 1024, format, _list);
va_end(_list);

Channel* logChannel = Network->findChannel(consoleChannel);
if(!logChannel) return;

Message(logChannel, buf);

return;

}


/**
 * This allows a command to modify the console level
 */
void nickserv::setConsoleLevel(logging::events::eventType& newMask) {
  if(newMask < logging::events::E_MIN || newMask > logging::events::E_MAX) {
    theLogger->log(logging::events::E_WARNING, "An attempt was made to set the"
                    " console level to %u.", newMask);
    return;
  }

  consoleLevel = newMask;

  Write("%s T %s :Current NickServ console level: %u",
        getCharYYXXX().c_str(), consoleChannel.c_str(), newMask);
}


/**
 * This function compares an iClient and a sqlUser and returns true if they
 * match, ie iClient->getNickName() == sqlUser->getName()
 */
bool nickserv::isAccountMatch(iClient* theClient, sqlUser* theUser)
{
string lowerNickName = string_lower(theClient->getAccount());
string lowerName = string_lower(theUser->getName());

if(lowerNickName == lowerName) return true;

return false;
}


/**
 * This function simply returns iClient->netData->authedUser.
 * It is purely a convenience method.
 */
sqlUser* nickserv::isAuthed(iClient* theClient)
{
netData* theData = static_cast< netData* >( theClient->getCustomData(this) );
return (theData ? theData->authedUser : 0) ;
}


/**
 * This function simply checks if a given nick is registered, and if so then returns
 * the associated sqlUser* from the cache.
 */
sqlUser* nickserv::isRegistered(string theNick)
{
sqlUserHashType::iterator cachedUser = sqlUserCache.find(theNick);
if(cachedUser == sqlUserCache.end()) return 0;
return cachedUser->second;
}


void Command::Usage(iClient* theClient)
{
bot->Notice(theClient, string("SYNTAX: ")  + getInfo());
}

} // namespace ns

} // namespace gnuworld
