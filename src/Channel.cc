/* Channel.cc */

#include	<string>
#include	<iostream>
#include	<strstream>

#include	"Channel.h"
#include	"iClient.h"
#include	"ChannelUser.h"
#include	"Network.h"
#include	"xparameters.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"match.h"

using std::string ;
using std::endl ;
using std::strstream ;

namespace gnuworld
{

Channel::Channel( const string& _name,
	const time_t& _creationTime )
 : name( _name ),
   creationTime( _creationTime ),
   modes( 0 ),
   limit( 0 )
{}

Channel::~Channel()
{
// Deallocate all ChannelUser's that are left
userListType::iterator currentPtr = userList.begin() ;
userListType::iterator endPtr = userList.end() ;
for( ; currentPtr != endPtr ; ++currentPtr )
	{
	delete currentPtr->second ;
	}
userList.clear() ;
}

bool Channel::addUser( ChannelUser* newUser )
{
if( !userList.insert(
	userListType::value_type( newUser->getIntYYXXX(), newUser ) ).second )
	{
//	elog	<< "Channel::addUser> (" << getName() << "): "
//		<< "Unable to add user: " << *newUser << endl ;
	return false ;
	}

//elog	<< "Channel::addUser> " << name << " added user: "
//	<< *newUser << endl ;

return true ;
}

ChannelUser* Channel::removeUser( iClient* theClient )
{
#ifndef NDEBUG
  assert( theClient != 0 ) ;
#endif

return removeUser( theClient->getIntYYXXX() ) ;
}

ChannelUser* Channel::removeUser( const unsigned int& intYYXXX )
{
userListType::iterator ptr = userList.find( intYYXXX ) ;
if( ptr != userList.end() )
	{
	userList.erase( ptr ) ;
	return ptr->second ;
	}

//elog		<< "Channel::removeUser> (" << getName() << ") "
//		<< "Unable to find user: " << intYYXXX << std::endl ;

return 0 ;
}

ChannelUser* Channel::findUser( iClient* theClient ) const
{
#ifndef NDEBUG
  assert( theClient != 0 ) ;
#endif

userListType::const_iterator ptr = userList.find( theClient->getIntYYXXX() ) ;
if( ptr == userList.end() )
	{
	// User not found
	return 0 ;
	}
return ptr->second ;
}

bool Channel::removeUserMode( const ChannelUser::modeType& whichMode,
	iClient* theClient )
{
ChannelUser* theChanUser = findUser( theClient ) ;
if( NULL == theChanUser )
	{
//	elog	<< "Channel::removeUserMode> (" << getName() << ") "
//		<< "Unable to find user\n" ;
	return false ;
	}
theChanUser->removeMode( whichMode ) ;
return true ;
}

bool Channel::setUserMode( const ChannelUser::modeType& whichMode,
	iClient* theClient )
{
ChannelUser* theChanUser = findUser( theClient ) ;
if( NULL == theChanUser )
	{
//	elog	<< "Channel::setUserMode> (" << getName() << ") "
//		<< "Unable to find user\n" ;
	return false ;
	}
theChanUser->setMode( whichMode ) ;
return true ;
}

bool Channel::getUserMode( const ChannelUser::modeType& whichMode,
	iClient* theClient ) const
{
ChannelUser* theChanUser = findUser( theClient ) ;
if( NULL == theChanUser )
	{
//	elog	<< "Channel::getUserMode> (" << getName() << ") "
//		<< "Unable to find user\n" ;
	return false ;
	}
return theChanUser->getMode( whichMode ) ;
}

void Channel::setBan( const string& newBan )
{
// TODO: Remove ambiguous bans, although this should be in
// the xServer class somewhere so that events can
// be posted withouth increasing coupling
banList.push_front( newBan ) ;
}

void Channel::removeBan( const string& banMask )
{
for( banListType::iterator ptr = banList.begin(), end = banList.end() ;
	ptr != end ; ++ptr )
	{
	if( !strcasecmp( ptr->c_str(), banMask.c_str() ) )
		{
		banList.erase( ptr ) ;
		return ;
		}
	}
}

bool Channel::findBan( const string& banMask ) const
{
for( banListType::const_iterator ptr = banList.begin(),
	end = banList.end() ; ptr != end ; ++ptr )
	{
	if( !strcasecmp( ptr->c_str(), banMask.c_str() ) )
		{
		return true ;
		}
	}
return false ;
}

bool Channel::matchBan( const string& banMask ) const
{
for( banListType::const_iterator ptr = banList.begin(),
	end = banList.end() ; ptr != end ; ++ptr )
	{
	if( !match( banMask.c_str(), ptr->c_str() ) )
		{
		// Found a match
		return true ;
		}
	}
return false ;
}

bool Channel::getMatchingBan( const string& banMask,
	string& matchingBan ) const
{
for( banListType::const_iterator ptr = banList.begin(),
	end = banList.end() ; ptr != end ; ++ptr )
	{
	if( !match( banMask.c_str(), ptr->c_str() ) )
		{
		matchingBan = *ptr ;
		return true ;
		}
	}
return false ;
}

void Channel::onModeT( bool polarity )
{
if( polarity )	setMode( MODE_T ) ;
else		removeMode( MODE_T ) ;
}

void Channel::onModeN( bool polarity )
{
if( polarity )	setMode( MODE_N ) ;
else		removeMode( MODE_N ) ;
}

void Channel::onModeS( bool polarity )
{
if( polarity )	setMode( MODE_S ) ;
else		removeMode( MODE_S ) ;
}

void Channel::onModeP( bool polarity )
{
if( polarity )	setMode( MODE_P ) ;
else		removeMode( MODE_P ) ;
}

void Channel::onModeM( bool polarity )
{
if( polarity )	setMode( MODE_M ) ;
else		removeMode( MODE_M ) ;
}

void Channel::onModeI( bool polarity )
{
if( polarity )	setMode( MODE_I ) ;
else		removeMode( MODE_I ) ;
}

void Channel::onModeL( bool polarity, const unsigned int& newLimit )
{
if( polarity )
	{
	setMode( MODE_L ) ;
	setLimit( newLimit ) ;
	}
else
	{
	removeMode( MODE_L ) ;
	setLimit( 0 ) ;
	}
}

void Channel::onModeK( bool polarity, const string& newKey )
{
if( polarity )
	{
	setMode( MODE_K ) ;
	setKey( newKey ) ;
	}
else
	{
	removeMode( MODE_K ) ;
	setKey( "" ) ;
	}
}

void Channel::onModeO( const vector< pair< bool, ChannelUser* > >&
	opVector )
{
typedef vector< pair< bool, ChannelUser* > > opVectorType ;
for( opVectorType::const_iterator ptr = opVector.begin() ;
	ptr != opVector.end() ; ++ptr )
	{
	if( ptr->first )
		{
		ptr->second->setMode( ChannelUser::MODE_O ) ;
		}
	else
		{
		ptr->second->removeMode( ChannelUser::MODE_O ) ;
		}
	}
}

void Channel::onModeV( const vector< pair< bool, ChannelUser* > >&
	voiceVector )
{
typedef vector< const pair< bool, ChannelUser* > > voiceVectorType ;
for( voiceVectorType::const_iterator ptr = voiceVector.begin() ;
	ptr != voiceVector.end() ; ++ptr )
	{
	if( ptr->first )
		{
		ptr->second->setMode( ChannelUser::MODE_V ) ;
		}
	else
		{
		ptr->second->removeMode( ChannelUser::MODE_V  ) ;
		}
	}
}

void Channel::onModeB( const vector< pair< bool, string > >&
	banVector )
{
typedef vector< const pair< bool, string > > banVectorType ;
for( banVectorType::const_iterator ptr = banVector.begin() ;
	ptr != banVector.end() ; ++ptr )
	{
	if( ptr->first )	setBan( ptr->second ) ;
	else			removeBan( ptr->second ) ;
	}
}

const string Channel::getModeString() const
{

string modeString = "+" ;
string argString = "" ;

if( modes & MODE_T )	modeString += 't' ;
if( modes & MODE_N )	modeString += 'n' ;
if( modes & MODE_S )	modeString += 's' ;
if( modes & MODE_P )	modeString += 'p' ;
if( modes & MODE_M )	modeString += 'm' ;
if( modes & MODE_I )	modeString += 'i' ;

if( modes & MODE_K )
	{
	modeString += 'k' ;
	argString += getKey() + ' ' ;
	}

if( modes & MODE_L )
	{
	modeString += 'l' ;

	// Can't put numerical variables into a string
	strstream s ;
	s << getLimit() << ends ;

	argString += s.str() ;

	delete[] s.str() ;
	}

return (modeString + ' ' + argString) ;

}

string Channel::createBan( const iClient* theClient )
{
#ifndef NDEBUG
  assert( theClient != 0 ) ;
#endif

string theBan = "*!*" ;

theBan += theClient->getUserName() + '@' ;

StringTokenizer st( theClient->getInsecureHost(), '.' ) ;
if( Socket::isIPAddress( theClient->getInsecureHost() ) )
	{
	theBan += st[ 0 ] + '.' ;
	theBan += st[ 1 ] + '.' ;
	theBan += st[ 2 ] + ".*" ;
	}
else
	{
	if( 2 == st.size() )
		{
		theBan += theClient->getInsecureHost() ;
		}
	else
		{
		theBan += "*." + st.assemble( 1 ) ;
		}
	}

return theBan ;
}

} // namespace gnuworld
