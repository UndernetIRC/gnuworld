/**
 * cloner.cc
 * Load fake clones for testing or fun.
 *
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
 *		      Reed Loden <reed@reedloden.com>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * $Id: cloner.cc,v 1.36 2005/01/12 04:36:45 dan_karrels Exp $
 */

#include  <new>
#include  <list>
#include  <vector>
#include  <iostream>
#include  <sstream>
#include  <string>
#include  <algorithm>
#include  <random>

#include  <ctime>
#include  <cstdlib>

#include  "client.h"
#include  "iClient.h"
#include  "cloner.h"
#include  "EConfig.h"
#include  "ip.h"
#include  "Network.h"
#include  "StringTokenizer.h"
#include  "misc.h"
#include  "ELog.h"

namespace gnuworld
{

using std::vector ;
using std::endl ;
using std::stringstream ;
using std::string ;

/*
 *  Exported function used by moduleLoader to gain an
 *  instance of this module.
 */

extern "C" {
xClient* _gnuwinit( const string& args )
  {
  return new cloner( args ) ;
  }
}

cloner::cloner( const string& configFileName )
 : xClient( configFileName )
{
EConfig conf( configFileName ) ;

cloneDescription = conf.Require< string >( "clonedescription" ) ;
cloneMode = conf.Require< string >( "clonemode" ) ;
fakeServerName = conf.Require< string >( "fakeservername" ) ;
fakeServerDescription = conf.Require< string> ( "fakeserverdescription" ) ;
allowOpers = conf.Require< bool >( "allow_opers" ) ;

EConfig::const_iterator ptr = conf.Find( "permit_user" ) ;
while( ptr != conf.end() && ptr->first == "permit_user" )
  {
  allowAccess.push_back( ptr->second ) ;
  ++ptr ;
  }

cloneBurstCount = conf.Require< unsigned int >( "cloneburstcount" ) ;
if( cloneBurstCount < 1 )
  {
  elog	<< "cloner> cloneBurstCount must be at least 1"
    << endl ;
  ::exit( 0 ) ;
  }

ptr = conf.Find( "fakehost" ) ;
while( ptr != conf.end() && ptr->first == "fakehost" )
  {
  hostNames.push_back( ptr->second ) ;
  ++ptr ;
  }

if( hostNames.empty() )
  {
  elog	<< "cloner> Must specify at least one hostname"
    << endl ;
  ::exit( 0 ) ;
  }

ptr = conf.Find( "fakeuser" ) ;
while( ptr != conf.end() && ptr->first == "fakeuser" )
  {
  userNames.push_back( ptr->second ) ;
  ++ptr ;
  }

if( userNames.empty() )
  {
  elog	<< "cloner> Must specify at least one username"
    << endl ;
  ::exit( 0 ) ;
  }

ptr = conf.Find( "fakeaccount" ) ;
bool confError { false } ;
while( ptr != conf.end() && ptr->first == "fakeaccount" )
  {
  StringTokenizer st( ptr->second ) ;
  if( st.size() != 2 )
    {
    confError = true ;
    break ;
    }

  accountNames.push_back( std::make_pair( st[ 0 ], atoi( st[ 1 ] ) ) ) ;
  ++ptr ;
  }

if( accountNames.empty() || confError )
  {
  elog	<< "cloner> Must specify at least one username"
        << endl ;
  ::exit( 0 ) ;
  }

minNickLength = conf.Require< unsigned int >( "minnicklength" ) ;
maxNickLength = conf.Require< unsigned int >( "maxnicklength" ) ;

if( minNickLength < 1 )
  {
  elog	<< "cloner> minNickLength must be at least 1"
        << endl ;
  ::exit( 0 ) ;
  }
if( maxNickLength <= minNickLength )
  {
  elog	<< "cloner> minNickLength must be less than maxNickLength"
    << endl ;
  ::exit( 0 ) ;
  }
}

cloner::~cloner()
{}

void cloner::OnConnect()
{
fakeServer = new (std::nothrow) iServer(
  MyUplink->getIntYY(), // uplinkIntYY
  string( "00]]]" ),
  fakeServerName,
  ::time( nullptr ),
  fakeServerDescription ) ;
assert( fakeServer != nullptr ) ;

MyUplink->AttachServer( fakeServer, this ) ;

xClient::OnConnect() ;
}

void cloner::OnDetach( const string& reason )
{
MyUplink->UnRegisterChannelEvent( xServer::CHANNEL_ALL, this ) ;
MyUplink->UnRegisterTimer( cycleCloneTimer, nullptr ) ;

if( fakeServer && MyUplink->DetachServer( fakeServer ) )
  {
  delete fakeServer ;
  fakeServer = nullptr ;
  }

xClient::OnDetach( reason ) ;
}

void cloner::OnAttach()
{
MyUplink->RegisterChannelEvent( xServer::CHANNEL_ALL, this ) ;
loadCloneTimer = MyUplink->RegisterTimer( ::time( nullptr ) + 1, this, 0 ) ;
}

void cloner::OnNetworkKick( Channel* theChan, iClient*,
  iClient* destClient, const string&, bool )
{
auto pos = std::find( clones.begin() , clones.end() , destClient ) ;
if( pos != clones.end() )
  {
  stringstream s;
  s << destClient->getCharYYXXX()
    << " L "
    << theChan->getName()
    << endl ;

  MyUplink->Write( s ) ;
  }
}

void cloner::OnPrivateMessage( iClient* theClient, const string& Message,
  bool )
{
//elog << "cloner::OnPrivateMessage> " << Message << endl ;

// Get rid of anyone who is not an oper and does not have access
bool userHasAccess = hasAccess( theClient->getAccount() ) ;
if( !userHasAccess && !theClient->isOper() )
  {
  // Normal user
  return ;
  }

if( !userHasAccess )
  {
  // The client must be an oper
  // Are opers allow to use the service?
  if( !allowOpers )
    {
    // Nope
    return ;
    }
  }

StringTokenizer st( Message ) ;
if( st.empty() )
  {
  return ;
  }

string command( string_upper( st[ 0 ] ) ) ;
string topic ;

if( st.size() > 1 )
  {
  topic = string_upper( st[ 1 ] ) ;
  }

//elog	<< "cloner::OnPrivateMessage> command: "
//	<< command
//	<< ", topic: "
//	<< topic
//	<< endl ;

if( command == "SHOWCOMMANDS" )
  {
  if( st.size() < 1 )
    {
    Notice( theClient, "Usage: %s", command.c_str() ) ;
    return ;
    }
  if( st.size() >= 1 )
    {
    Notice( theClient, "_-=[Cloner Commands]=-_" ) ;
    Notice( theClient, "LOADCLONES CYCLE JOIN PART QUIT "
      "KILLALL/QUITALL SAYALL/MSGALL "
      "ACTALL/DOALL/DESCRIBEALL NOTICEALL" ) ;
    Notice( theClient, "_-=[End of Cloner Commands]=-_" ) ;
    }
  }
else if( command == "HELP" )
  {
  if( st.size() < 1 )
    {
    Notice( theClient, "Usage: %s <topic>",
      command.c_str() ) ;
    return ;
    }

  if( topic == "SHOWCOMMANDS" )
    {
    Notice( theClient, "%s - Shows a list of all commands",
      topic.c_str() ) ;
    }
  else if( topic == "HELP" )
    {
    Notice( theClient, "%s <topic> - Gives help on a topic",
      topic.c_str() ) ;
    }
  else if( topic == "LOADCLONES" )
    {
    Notice( theClient, "%s <# of clones> [-iorkx] - Queue creation "
      "  of clone(s)", topic.c_str() ) ;
    Notice( theClient, "-i: the clones will be unidented (~)") ;
    Notice( theClient, "-o: the clones will be IRC operators") ;
    Notice( theClient, "-k: the clones will be network services") ;
    Notice( theClient, "-r: the clones will be logged into an account") ;
    Notice( theClient, "-x: the clones will be +x (hidden host)") ;
    }
    else if( topic == "CYCLE" )
    {
    Notice( theClient, "%s <OFF | time:pst> - Cycling (quitting and reconnecting) "
      "  of clone(s)", topic.c_str() ) ;
    Notice( theClient, "time: the number of seconds to run a cycle") ;
    Notice( theClient, "pst : the percentage of clones to be cycled") ;
    }
    else if( topic == "JOIN" )
    {
    Notice( theClient, "%s <#channel> [# of clones] [-d # of clones per second]- Make clones "
      "/join a #channel. If # is not specified, all loaded clones will /join.", topic.c_str() ) ;
    }
  else if( topic == "PART" )
    {
    Notice( theClient, "%s <#channel> [# of clones] [reason] - Make "
      "clones /part a #channel with an optional "
      "reason. If # is not specified, all loaded clones will /part.", topic.c_str() ) ;
    }
  else if( topic == "KILLALL" || topic == "QUITALL" )
    {
    Notice( theClient, "%s [reason] - Make all "
      "clones /quit with an optional reason",
      topic.c_str() ) ;
    }
  else if( topic == "SAYALL" || topic == "MSGALL" )
    {
    Notice( theClient, "%s <#channel/nickname> "
      "<message> - Make all clones /msg a #channel or "
      "nickname", topic.c_str() ) ;
    }
  else if( topic == "ACTALL" || topic == "DOALL" || topic ==
    "DESCRIBEALL" )
    {
    Notice( theClient, "%s <#channel/nickname> <action> - "
      "Make all clones /me a channel or nickname",
      topic.c_str() ) ;
    }
  else if( topic == "NOTICEALL" )
    {
    Notice( theClient, "%s <#channel/nickname> "
      "<notice> - Make all clones /notice a #channel "
      "or nickname", topic.c_str() ) ;
    }
  } // "HELP"
else if( command == "LOADCLONES" )
  {
  if( st.size() < 2 )
    {
    Notice( theClient, "Usage: %s <# of clones> [-iorkx]",
      command.c_str() ) ;
    return ;
    }

  size_t numClones = static_cast< size_t> ( atoi( st[ 1 ] ) ) ;
  if( numClones < 1 )
    {
    Notice( theClient,
      "LOADCLONES: Invalid number of clones" ) ;
    return ;
    }

  /* Settings. */
  bool cloneIdent = false ;
  bool cloneModeK = false ;
  bool cloneModeO = false ;
  bool cloneModeR = false ;
  bool cloneModeX = false ;

  if( st.size() == 3 && st[2][0] == '-' )
    {
    for( string::size_type charPos = 0 ; charPos < st[ 2 ].size() ; ++charPos )
    {
      switch( st[ 2 ][ charPos ] )
        {
        case 'i':
          cloneIdent = true ;
          break ;
        case 'o':
          cloneModeO = true ;
          break ;
        case 'r':
          cloneModeR = true ;
          break ;
        case 'k':
          cloneModeK = true ;
          break ;
        case 'x':
          cloneModeX = true ;
          break ;
        default:
          break ;
        }
      }
    }

  for( size_t i = 0 ; i < numClones ; ++i )
    {
    cloneSettings settings ;
    settings.ident = cloneIdent ;
    settings.modeO = cloneModeO ;
    settings.modeK = cloneModeK ;
    settings.modeR = cloneModeR ;
    settings.modeX = cloneModeX ;

    cloneQueue.push_back( settings ) ;
    }

  Notice( theClient, "Queuing %d Clones", numClones ) ;
  } // "LOADCLONES"
else if( command == "CYCLE" )
  {
  if( st.size() < 2 )
    {
    Notice( theClient, "Usage: %s <OFF | time:pct>",
      command.c_str() ) ;
    return ;
    }

  if( string_upper( st[ 1 ] ) == "OFF" )
    {
    if( cycleRun )
      {
      cycleRun = false ;
      cycleTime = 0 ;
      cyclePercentage = 0.0 ;
      MyUplink->UnRegisterTimer( cycleCloneTimer, nullptr ) ;
      Notice( theClient, "Cycle stopped." ) ;
      return ;
      }
    else
      {
      Notice( theClient, "Cycle not running." ) ;
      return ;
      }
    }

    StringTokenizer st2( st[ 1 ], ':' ) ;
  if( st2.size() != 2 )
    {
    Notice( theClient, "Usage: %s <OFF | time:pct>",
      command.c_str() ) ;
    return ;
    }

  if( atoi( st2[ 0 ] ) < 1 )
    {
    Notice( theClient, "Cycle time must be greater than 0." ) ;
    return ;
    }

  float tempPst = 0.0 ;
  try {
    tempPst = std::stof( st2[ 1 ] ) ;
  } catch (...) {
    Notice( theClient, "Invalid percentage." ) ;
    return ;
  }

  if( tempPst < 0.0 || tempPst > 100.0 )
    {
    Notice( theClient, "Cycle percentage must be between 0 and 100." ) ;
    return ;
    }

  cycleTime = atoi( st2[ 0 ] ) ;
  cyclePercentage = tempPst ;
  cycleRun = true ;
  cycleCloneTimer = MyUplink->RegisterTimer( ::time( nullptr ) + cycleTime, this, 0 ) ;
  Notice( theClient, "Cycle set to %d:%f", cycleTime, cyclePercentage ) ;
  return ;
  }
else if( command == "JOIN" )
  {
  if( st.size() < 2 )
    {
    Notice( theClient, "Usage: %s <#channel> [# of clones] [-d # of clones per second]",
      command.c_str() ) ;
    return ;
    }

  string chanName( st[ 1 ] ) ;
  if( chanName[ 0 ] != '#' )
    {
    chanName.insert( chanName.begin(), '#' ) ;
    }

  Channel* theChan = Network->findChannel( chanName ) ;
  if( theChan == nullptr )
    {
    Notice( theClient, "Unable to locate that channel." ) ;
    return ;
    }

  size_t clonesTotal { 0 } ;
  size_t clonesPerSec { 0 } ;
  int posParam { 2 } ;

  /* If the first param is a numeric, it is the number of clones. */
  if( st.size() > (size_t)posParam && IsNumeric( st[ posParam ] ) )
    {
    clonesTotal = atoi( st[ posParam ] ) ;
    posParam++ ;
    }

  /* Is the -d param set? */
  if( st.size() > (size_t)posParam && string_lower( st[ posParam ] ) == "-d" )
    {
    clonesPerSec = atoi( st[ posParam + 1 ] ) ;
    }

  if( clonesTotal == 0 || clonesTotal > clones.size() )
    {
    clonesTotal = clones.size() ;
    }

  /* Are we doing a delayed join? */
  if( clonesPerSec > 0 )
    {
    /* Are we already doing a delay join? */
    if( joinCloneCount > 0 )
      {
      Notice( theClient, "Joining of clones ongoing. Please wait." ) ;
      return ;
      }

    /* Store settings. */
    delayChan = theChan ;
    delayCount = clonesPerSec ;

    joinCloneCount = static_cast< size_t >( clonesTotal ) ;
    delayJoinTimer = MyUplink->RegisterTimer( ::time( nullptr ) + 1, this, 0 ) ;

    Notice( theClient, "Queuing %i clones per second (%i in total).",
      delayCount, clonesTotal ) ;
    return ;
    }

  /* Regular join. */
  size_t res = joinClone( clonesTotal, theChan ) ;
  Notice( theClient, "Done. %i clones joined.", res ) ;

  } // JOINALL
else if( command == "PART" )
  {
  if( st.size() < 2 )
    {
    Notice( theClient, "Usage: %s <#channel> [# of clones] [reason]",
      command.c_str() ) ;
    return ;
    }

  string chanName( st[ 1 ] ) ;
  if( chanName[ 0 ] != '#' )
    {
    chanName.insert( chanName.begin(), '#' ) ;
    }

  Channel* theChan = Network->findChannel( chanName ) ;
  if( theChan == nullptr )
    {
    Notice( theClient, "Unable to locate that channel." ) ;
    return ;
    }

  /* Determine number of clones. */
  size_t noClones { 0 } ;
  int posReason { 2 } ;

  if( st.size() > 2 && IsNumeric( st[ 2 ] ) )
    {
    noClones = atoi( st[ 2 ] ) ;
    posReason++ ;
    }
  if( noClones < 1 )
    noClones = clones.size() ;

  /* Do we have a part reason? */
  string partReason { } ;
  if( st.size() > (size_t)posReason )
    partReason = st.assemble(2) ;

  size_t res = partClone( noClones, theChan, partReason ) ;
  Notice( theClient, "Done. %i clones parted.", res ) ;

  } // PARTALL
else if( command == "KILLALL" || command == "QUITALL" )
  {
  if( st.size() < 1 )
    {
    Notice( theClient, "Usage: %s [reason]",
      command.c_str() ) ;
    return ;
    }

  string quitMsg ;
  if( st.size() >= 2 )
    {
    quitMsg = st.assemble( 1 ) ;
    }

  size_t res = quitClone( clones.size(), quitMsg ) ;
  Notice( theClient, "Done. %i clones have been killed.", res ) ;

  } // KILLALL/QUITALL
else if( command == "QUIT" )
  {
  if( st.size() < 2 )
    {
    Notice( theClient, "Usage: %s <#> [reason]",
      command.c_str() ) ;
    return ;
    }

  if( !IsNumeric( st[ 1 ] ) )
    {
    Notice( theClient, "Usage: %s <#> [reason]",
      command.c_str() ) ;
    return ;
    }

  size_t numClones = atoi( st[ 1 ] ) ;
  if( numClones < 1 || numClones > clones.size() )
    {
    Notice( theClient, "Invalid number of clones." ) ;
    return ;
    }

  string quitMsg ;
  if( st.size() >= 3 )
    {
    quitMsg = st.assemble( 2 ) ;
    }

  size_t res = quitClone( numClones, quitMsg ) ;
  Notice( theClient, "Done. %i clones have been killed.", res ) ;

  } // QUIT
else if( command == "SAYALL" || command == "MSGALL" )
  {
  if( st.size() < 3 )
    {
    Notice( theClient, "Usage: %s <#channel/nickname> <message>",
      command.c_str() ) ;
    return ;
    }

  string chanOrNickName( st[ 1 ] ) ;
  string privMsg( st.assemble(2).c_str() ) ;

  if( chanOrNickName[ 0 ] != '#' )
    { // Assume nickname
    iClient* Target = Network->findNick( st[ 1 ] ) ;
    if( nullptr == Target )
      {
      Notice( theClient, "Unable to find nick: %s",
        st[ 1 ].c_str() ) ;
      return ;
      }
    chanOrNickName = Target->getCharYYXXX() ;
    }

  for( cloneVectorType::const_iterator ptr = clones.begin(),
    endPtr = clones.end() ; ptr != endPtr ; ++ptr )
    {
    stringstream s ;
    s	<< (*ptr)->getCharYYXXX()
      << " P "
      << chanOrNickName
      << " :"
      << privMsg ;

    MyUplink->Write( s ) ;
    }
  } // SAYALL/MSGALL
else if( command == "ACTALL" || command == "DOALL" ||
  command == "DESCRIBEALL" )
  {
  if( st.size() < 3 )
    {
    Notice( theClient, "Usage: %s <#channel/nickname> <action>",
      command.c_str() ) ;
    return ;
    }

  string chanOrNickName( st[ 1 ] ) ;
  string action( st.assemble(2).c_str() ) ;

  if( chanOrNickName[ 0 ] != '#' )
    { // Assume nickname
    iClient* Target = Network->findNick( st[ 1 ] ) ;
    if( nullptr == Target )
      {
      Notice( theClient, "Unable to find nick: %s",
        st[ 1 ].c_str() ) ;
      return ;
      }
    chanOrNickName = Target->getCharYYXXX() ;
    }

  for( cloneVectorType::const_iterator ptr = clones.begin(),
    endPtr = clones.end() ; ptr != endPtr ; ++ptr )
    {
    stringstream s ;
    s	<< (*ptr)->getCharYYXXX()
      << " P "
      << chanOrNickName
      << " :\001ACTION "
      << action
      << "\001" ;

    MyUplink->Write( s ) ;
    }
  } // ACTALL/DOALL/DESCRIBEALL
else if( command == "NOTICEALL" )
  {
  if( st.size() < 3 )
    {
    Notice( theClient, "Usage: %s <#channel/nickname> <notice>",
      command.c_str() ) ;
    return ;
    }

  string chanOrNickName( st[ 1 ] ) ;
  string notice( st.assemble(2).c_str() ) ;

  if( chanOrNickName[ 0 ] != '#' )
    { // Assume nickname
    iClient* Target = Network->findNick( st[ 1 ] ) ;
    if( nullptr == Target )
      {
      Notice( theClient, "Unable to find nick: %s",
        st[ 1 ].c_str() ) ;
      return ;
      }
    chanOrNickName = Target->getCharYYXXX() ;
    }

  for( cloneVectorType::const_iterator ptr = clones.begin(),
    endPtr = clones.end() ; ptr != endPtr ; ++ptr )
    {
    stringstream s ;
    s	<< (*ptr)->getCharYYXXX()
      << " O "
      << chanOrNickName
      << " :"
      << notice ;

    MyUplink->Write( s ) ;
    }
  } // NOTICEALL
}

void cloner::OnTimer( const xServer::timerID& timer_id, void* )
{
if( timer_id == cycleCloneTimer )
  {
  //elog << "cloner::OnTimer> cycleCloneTimer" << endl ;

  /* Are we running? */
  if( !cycleRun )
    return ;

  /* The number of clones to cycle. */
  size_t cycleCount = ( clones.size() / 100 ) * cyclePercentage ;

  /* Cycle the clones. */
  for( size_t i = 0 ; i < cycleCount ; ++i )
    {
    iClient* tmpClone = randomClone() ;
    if( tmpClone != nullptr )
      {
      cloneSettings settings ;
      settings.ident = tmpClone->getUserName().find( "~" ) != string::npos ;
      settings.modeO = tmpClone->isModeO() ;
      settings.modeK = tmpClone->isModeK() ;
      settings.modeR = tmpClone->isModeR() ;
      settings.modeX = tmpClone->isModeX() ;

      for( const auto& channel : tmpClone->channels() )
        {
        settings.channels.push_back( channel->getName() ) ;
        }

      cloneQueue.push_back( settings ) ;

      quitClone( tmpClone, "" ) ;
      }
    }

  cycleCloneTimer = MyUplink->RegisterTimer( ::time( nullptr ) + cycleTime, this, 0 ) ;
  }

if( timer_id == loadCloneTimer )
  {
//  elog	<< "cloner::OnTimer> makeCloneCount: "
//  	    << makeCloneCount
//  	    << endl ;

  /* Reset timer. */
  loadCloneTimer = MyUplink->RegisterTimer( ::time( nullptr ) + 1, this, 0 ) ;

  if( !cloneQueue.empty() )
    {
    size_t cloneCount = std::min( cloneQueue.size(), cloneBurstCount ) ;

    for( size_t i = 0 ; i < cloneCount ; ++i )
      {
      addClone() ;
      }

    std::random_device rd ;
    std::shuffle( clones.begin(), clones.end(), rd ) ;
    }
  }

if( timer_id == delayJoinTimer )
  {
//  elog	<< "cloner::OnTimer> joinCloneCount: "
//  	    << joinCloneCount
//  	    << endl ;

  if( joinCloneCount > 0 )
    {
    size_t cloneCount = joinCloneCount ;
    if( cloneCount > delayCount )
      {
      // Too many
      cloneCount = delayCount ;
      }

    joinCloneCount -= cloneCount ;

    /* Join the clones. We only reset the timer if this iteration was successful (>0). */
    if( joinClone( cloneCount, delayChan ) > 0
      && joinCloneCount > 0 )
      {
      delayJoinTimer = MyUplink->RegisterTimer( ::time( nullptr ) + 1, this, 0 ) ;
      }
    else
      {
      joinCloneCount = 0 ;
      }
    }
  }
}

size_t cloner::joinClone( iClient* theClone, Channel* theChan )
{
assert( theClone != nullptr ) ;
assert( theChan != nullptr ) ;

/* Already on the channel? */
if( theClone->findChannel( theChan ) )
  return 0 ;

/* Check channel modes. */
if( theChan->getMode( Channel::MODE_I )
  || theChan->getMode( Channel::MODE_K )
  || ( theChan->getMode( Channel::MODE_L )
  && theChan->getLimit() < theChan->size() ) )
  return 0 ;

/* Chanmode +r? */
if( theChan->getMode( Channel::MODE_R )
  && !theClone->getMode( iClient::MODE_REGISTERED ) )
  return 0 ;

/* Chanmode +Z? */
if( theChan->getMode( Channel::MODE_Z )
  && !theClone->getMode( iClient::MODE_TLS ) )
  return 0 ;

/* Banned? */
if( banMatch( theChan, theClone ) )
  return 0 ;

if( !MyUplink->JoinChannel( theClone, theChan->getName() ) )
  return 0 ;

return 1 ;
}

size_t cloner::joinClone( const size_t n, Channel* theChan )
{
size_t count { 0 } ;

for( cloneVectorType::const_iterator ptr = clones.begin(),
  endPtr = clones.end() ; ptr != endPtr ; ++ptr )
  {
  count += joinClone( *ptr, theChan ) ;

  if( count == n ) break ;
  }

return count ;
}

size_t cloner::partClone( const size_t n, Channel* theChan,
  const string reason )
{
size_t count { 0 } ;

for( cloneVectorType::const_iterator ptr = clones.begin(),
  endPtr = clones.end() ; ptr != endPtr ; ++ptr )
  {
  count += partClone( *ptr, theChan, reason ) ;

  if( count == n ) break ;
  }

return count ;
}

size_t cloner::partClone( iClient* theClone, Channel* theChan,
  const string partMsg )
{
/* On the channel? */
if( !theClone->findChannel( theChan ) )
  return 0 ;

stringstream s ;
s << theClone->getCharYYXXX()
  << " L "
  << theChan->getName() ;

if( partMsg != "" )
s << " :"
  << partMsg ;

MyUplink->Write( s ) ;

ChannelUser* tmpUser = theChan->findUser( theClone ) ;
if( tmpUser != nullptr )
  {
  delete tmpUser ; tmpUser = nullptr ;
  }

theChan->removeUser( theClone ) ;
theClone->removeChannel( theChan ) ;

MyUplink->PostChannelEvent( EVT_PART, theChan,
  static_cast< void* >( theClone ), NULL ) ;

return 1 ;
}

size_t cloner::quitClone( const size_t n, const string quitMsg )
{
size_t count { 0 } ;

for( auto ptr = clones.begin(); ptr != clones.end() ; )
  {
  iClient* theClone = *ptr ;

  if( MyUplink->DetachClient( theClone, quitMsg ) )
    {
    ptr = clones.erase( ptr ) ;

    delete theClone ; theClone = nullptr ;
    ++count ;
    }
  else
    ++ptr ;

  if( count == n ) break ;
  }

return count ;
}

size_t cloner::quitClone( iClient* theClone, const string quitMsg )
{
if( MyUplink->DetachClient( theClone, quitMsg ) )
  {
  auto pos = std::find( clones.begin() , clones.end() , theClone ) ;
  if( pos != clones.end() )
    clones.erase( pos ) ;

  delete theClone ; theClone = nullptr ;
  return 1 ;
  }
return 0 ;
}

void cloner::addClone()
{

if( cloneQueue.empty() )
  {
  elog << "addClone() called, but no clones to add." << endl ;
  return ;
  }

  // The XXX doesn't matter here, the core will choose an
// appropriate value.
string yyxxx( fakeServer->getCharYY() + "]]]" ) ;
string account ;
string ident ;
unsigned int account_id { 0 };

string cloneHost = randomHost() ;
string cloneBase64 ;

/* Check whether the random host is an IP. If it is, lets send the correct Base64 IP. */
unsigned char ipmask_len ;
irc_in_addr ip ;
if( ipmask_parse( cloneHost.c_str(), &ip, &ipmask_len ) )
  cloneBase64 = string( xIP( ip ).GetBase64IP() ) ;
else
  cloneBase64 = randomNick( 6, 6 ) ;

cloneSettings settings = cloneQueue.front() ;

string thisMode = cloneMode ;
if( settings.modeK )
  thisMode += 'k' ;
if( settings.modeO )
  thisMode += 'o' ;
if( settings.modeX )
  thisMode += 'x' ;
if( settings.modeR )
  {
  thisMode +='r' ;
  StringTokenizer st( randomAccount(), ':' ) ;
  account = st[ 0 ] ;
  account_id = atoi( st[ 1 ] ) ;
  }
if( settings.ident )
  ident += "~" ;

iClient* newClient = new iClient(
    fakeServer->getIntYY(),
    yyxxx,
    randomNick( minNickLength, maxNickLength ),
    ident + randomUser(),
    cloneBase64,
    cloneHost,
    cloneHost,
    thisMode,
    account,
    account_id,
    0,
    string(),
    cloneDescription,
    ::time( nullptr ) ) ;
assert( newClient != nullptr ) ;

if( MyUplink->AttachClient( newClient, this ) )
  {
  clones.push_back( newClient ) ;
  cloneQueue.erase( cloneQueue.begin() ) ;
  }

for( const auto& channel : settings.channels )
  {
  Channel* theChan = Network->findChannel( channel ) ;
  if( theChan != nullptr )
    joinClone( newClient, theChan ) ;
  }
}

iClient* cloner::randomClone()
{
if( clones.empty() ) return nullptr ;

srand( ::time( nullptr ) ) ;

cloneVectorType::iterator theClone = clones.begin() ;
std::advance( theClone, rand() % clones.size() ) ;

return *theClone ;
}

string cloner::randomUser()
{
return userNames[ rand() % userNames.size() ] ;
}

string cloner::randomHost()
{
return hostNames[ rand() % hostNames.size() ] ;
}

string cloner::randomAccount()
{
int i = rand() % accountNames.size() ;
return accountNames[ i ].first + ":" + itoa( accountNames[ i ].second ) ;
}

string cloner::randomNick( int minLength, int maxLength )
{
string retMe ;

// Generate a random number between minLength and maxLength
// This will be the length of the nickname
int randomLength = minLength + (rand() % (maxLength - minLength + 1) ) ;

for( int i = 0 ; i < randomLength ; i++ )
  {
  retMe += randomChar() ;
  }

//elog << "randomNick: " << retMe << endl ;
return retMe ;
}

// ascii [65,122]
char cloner::randomChar()
{
char c = ('A' + (rand() % ('z' - 'A')) ) ;
//elog << "char: returning: " << c << endl ;
return c ;

//return( (65 + (rand() % 122) ) ;
//return (char) (1 + (int) (9.0 * rand() / (RAND_MAX + 1.0) ) ) ;
}

bool cloner::banMatch( const Channel* theChan, const iClient* theClient )
{
string authmask = theClient->getNickName() + "!" + theClient->getUserName() ;
authmask += "@" + theClient->getAccount() + theClient->getHiddenHostSuffix() ;
string banMask ;

for( Channel::const_banIterator ptr = theChan->banList_begin() ;
  ptr != theChan->banList_end() ; ++ptr )
  {
  banMask = *ptr ;

  if( !match( banMask, theClient ) )
    return true ;

  if( theClient->isModeR() && !theClient->isModeX() )
    {
    /* client is authed, check our constructed hidden host against them */
    if( match( banMask, authmask ) == 0 )
      return true ;
    }
  }
return false ;
}

bool cloner::hasAccess( const string& accountName ) const
{
for( std::list< string >::const_iterator itr = allowAccess.begin() ;
  itr != allowAccess.end() ; ++itr )
  {
  if( !strcasecmp( accountName, *itr ) )
    {
    return true ;
    }
  }
return false ;
}

} // namespace gnuworld
