/*
 * nickserv.cc
 */

#include "libpq++.h"

#include "StringTokenizer.h"

#include "netData.h"
#include "nickserv.h"

const char NickServ_cc_rcsId[] = "$Id: nickserv.cc,v 1.3 2002/08/15 20:46:45 jeekay Exp $";

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

string dbHost = nickservConfig->Require("dbHost")->second;
string dbPort = nickservConfig->Require("dbPort")->second;
string dbDb = nickservConfig->Require("dbDb")->second;
string dbUser = nickservConfig->Require("dbUser")->second;
string dbPass = nickservConfig->Require("dbPass")->second;

string dbString = "host=" + dbHost + " port=" + dbPort + " dbname=" + dbDb
  + " user="+dbUser + " password=" + dbPass;

theManager = sqlManager::getInstance(dbString);

/* Precache the users */
precacheUsers();
}

nickserv::~nickserv()
{
delete nickservConfig;
}



/**
 * Here we decide what channels our xClient needs to burst in to.
 * The only channel of any interest to us is our console channel, which is
 * loaded from the configuration file.
 */
int nickserv::BurstChannels()
{
MyUplink->JoinChannel(this, consoleChannel, nickservConfig->Require("consoleChannelModes")->second);

return xClient::BurstChannels() ;
}


/**
 * Here we set up the various bits and pieces that we need an xServer
 * reference to be able to do.
 */
void nickserv::ImplementServer( xServer* theServer )
{
/* Register the commands we want to use */
//RegisterCommand(new WHOAMICommand(this, "WHOAMI", ""));

/* Register for all the events we want to see */
theServer->RegisterEvent(EVT_ACCOUNT, this);
theServer->RegisterEvent(EVT_CHNICK, this);
theServer->RegisterEvent(EVT_KILL, this);
theServer->RegisterEvent(EVT_NICK, this);
theServer->RegisterEvent(EVT_QUIT, this);

/* Register timerID's to process queues */

/* The timer to kick off the processing of the queue
 * Note: We add startDelay here to allow for a longer delay before checking
 * when we first link to the net
 */
time_t theTime = time(NULL) + startDelay;
processQueue_timerID = theServer->RegisterTimer(theTime, this, NULL);

xClient::ImplementServer( theServer );
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
int nickserv::OnEvent( const eventType& event, void* Data1, void* Data2, void* Data3, void* Data4)
{

elog << "Got an Event!" << endl;

/* The target user of the event */
iClient* theClient = static_cast< iClient* >( Data1 );

switch( event ) {
  case EVT_KILL: theClient = static_cast< iClient* >( Data2 );
  case EVT_QUIT: {
    netData* theData = static_cast< netData* >( theClient->removeCustomData(this) );
    delete theData;
    removeFromQueue(theClient);
    
    return 1;
    break;
  } // case EVT_KILL/QUIT
  
  case EVT_NICK: {
    netData* theData = new netData();
    theClient->setCustomData(this, theData);
    if(theClient->isModeR() && (string_lower(theClient->getNickName()) == string_lower(theClient->getAccount()))) {
      return 1;
      break;
    }
    addToQueue(theClient);
    
    return 1;
    break;
  } // case EVT_NICK
  
  case EVT_CHNICK: {
    if(string_lower(theClient->getNickName()) == string_lower(theClient->getAccount())) {
      removeFromQueue(theClient);
    } else {
      addToQueue(theClient);
    }
    
    return 1;
    break;
  } // case EVT_CHNICK
  
  case EVT_ACCOUNT: {
    elog << "Got account. iClient nick: " << theClient->getNickName() << ", AC: "
      << theClient->getAccount() << endl;
    if(string_lower(theClient->getNickName()) == string_lower(theClient->getAccount())) {
      removeFromQueue(theClient);
    }
    
    return 1;
    break;
  } // case EVT_ACCOUNT
} // switch( event )

return 0;
}


/**
 * Here we deal with incoming communications from network clients.
 */
int nickserv::OnPrivateMessage( iClient* theClient,
	const string& Message, bool )
{
if(!theClient->isModeR()) {
  Notice(theClient, "You must be logged in before attempting to use any commands.");
  return 1;
}

StringTokenizer st(Message);
if(st.empty()) {
  Notice(theClient, "Incomplete command");
  return 1;
}

string Command = string_upper(st[0]);
commandMapType::iterator commHandler = commandMap.find(Command);

if(commHandler == commandMap.end()) {
  return 1;
}

commHandler->second->Exec(theClient, Message);

return 1 ;
}


/**
 * When a timer expires, this function is called. It allows for periodic
 * processing of data.
 */
int nickserv::OnTimer(xServer::timerID theTimer, void* theData)
{
if(theTimer == processQueue_timerID) {
  processQueue();
  
  time_t theTime = time(NULL) + checkFreq;
  processQueue_timerID = MyUplink->RegisterTimer(theTime, this, NULL);
  return 1;
} // if(theTimer == processQueue_timerID)

return 0;
} // nickserv::OnTimer(xServer::timerID, void*)



/**
 * This is where we register a command so that users can interact
 * with the module.
 */
bool nickserv::RegisterCommand( Command* theCommand )
{
//return commandMap.insert( commandPairType(theCommand->getName(), theCommand)).second;
}


/**
 * This function empties the current user cache and reloads all information from
 * the database. It is only called at startup, as the cache is authoritative.
 */
void nickserv::precacheUsers()
{
elog << "*** [NickServ:precacheUsers] Precaching users." << endl;

/* Get a connection instance to our backend */
PgDatabase* cacheCon = theManager->getConnection();

/* Retrieve the list of registered users */
string cacheQuery = "SELECT id,name,flags,level FROM users";
if(cacheCon->ExecTuplesOk(cacheQuery.c_str())) {
  for(int i = 0; i < cacheCon->Tuples(); i++) {
    sqlUser* tmpUser = new sqlUser();
    tmpUser->setAllMembers(cacheCon, i);
    sqlUserCache.insert(sqlUserHashType::value_type(tmpUser->getName(), tmpUser));
  }
} else {
  elog << "*** [NickServ:precacheUsers] Error executing query: "
    << cacheCon->ErrorMessage()
    << endl;
  ::exit(0);
}

elog << "*** [NickServ:precacheUsers] Done. Loaded a total of "
  << sqlUserCache.size() << " user records."
  << endl;

/* Dispose of our connection instance */
theManager->removeConnection(cacheCon);
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

/* A number of things can happen here.
 * Firstly, the warnQueue can be empty. If this is the case, return immediately.
 */

elog << "warnQueue size: " << warnQueue.size() << endl;

if(warnQueue.size() == 0) { return; }

QueueType killQueue;

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
  
  /* First things first - is this nick set to autokill? */
  sqlUserHashType::const_iterator cachedUser = sqlUserCache.find(theClient->getNickName());
  if(cachedUser == sqlUserCache.end() || !(cachedUser->second)->getFlag(sqlUser::F_AUTOKILL)) {
    theData->warned = 0;
    queuePos = warnQueue.erase(queuePos);
    continue;
  }
  
  if(theData->warned) {
    killQueue.push_back(theClient);
    queuePos = warnQueue.erase(queuePos);
    continue;
  } else {
    Notice(theClient, "You are using a registered nickname. Please login or you will be disconnected.");
    theData->warned++;
    queuePos++;
    continue;
  } // if(theData->warned)
} // iterate over warnQueue

elog << "killQueue size: " << killQueue.size() << endl;

if(killQueue.size() == 0) { return; }

for(QueueType::iterator queuePos = killQueue.begin(); queuePos != killQueue.end(); queuePos++) {
  iClient* theClient = *queuePos;
  netData* theData = static_cast<netData*>(theClient->removeCustomData(this));
  delete(theData);
  Kill(theClient, "[NickServ] AutoKill");
} // iterate over killQueue

} // nickserv::processQueue()


} // namespace ns

} // namespace gnuworld
