/* Channel.cc */

#include	<new>
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
#include	"server.h"

const char Channel_h_rcsId[] = __CHANNEL_H ;
const char Channel_cc_rcsId[] = "$Id: Channel.cc,v 1.29 2002/02/24 21:36:41 mrbean_ Exp $" ;
const char iClient_h_rcsId[] = __ICLIENT_H ;
const char ChannelUser_h_rcsId[] = __CHANNELUSER_H ;
const char Network_h_rcsId[] = __NETWORK_H ;
const char xParameters_h_rcsId[] = __XPARAMETERS_H ;
const char StringTokenizer_h_rcsId[] = __STRINGTOKENIZER_H ;
const char ELog_h_rcsId[] = __ELOG_H ;
const char match_h_rcsId[] = __MATCH_H ;
const char server_h_rcsId[] = __SERVER_H ;

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::strstream ;
using std::ends ;

const Channel::modeType Channel::MODE_T = 0x01 ;
const Channel::modeType Channel::MODE_N = 0x02 ;
const Channel::modeType Channel::MODE_S = 0x04 ;
const Channel::modeType Channel::MODE_P = 0x08 ;
const Channel::modeType Channel::MODE_K = 0x10 ;
const Channel::modeType Channel::MODE_L = 0x20 ;
const Channel::modeType Channel::MODE_M = 0x40 ;
const Channel::modeType Channel::MODE_I = 0x80 ;

Channel::Channel( const string& _name,
	const time_t& _creationTime )
 : name( _name ),
   creationTime( _creationTime ),
   modes( 0 ),
   limit( 0 )
{
#ifdef TOPIC_TRACK
topic = "";
#endif
}

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
assert( newUser != 0 ) ;

//elog	<< "Channel::addUser> ("
//	<< getName()
//	<< ") Number of users: "
//	<< userList.size()
//	<< endl ;

if( !userList.insert(
	userListType::value_type( newUser->getIntYYXXX(), newUser ) ).second )
	{
	elog	<< "Channel::addUser> ("
		<< getName()
		<< "): "
		<< "Unable to add user: "
		<< *newUser
		<< endl ;
	return false ;
	}

//elog	<< "Channel::addUser> " << name << " added user: "
//	<< *newUser << endl ;

return true ;
}

bool Channel::addUser( iClient* theClient )
{
ChannelUser* addMe = new (std::nothrow) ChannelUser( theClient ) ;

// The signature of addUser() here will verify the pointer
return addUser( addMe ) ;
}

ChannelUser* Channel::removeUser( ChannelUser* theUser )
{
assert( theUser != 0 ) ;

return removeUser( theUser->getClient()->getIntYYXXX() ) ;
}

ChannelUser* Channel::removeUser( iClient* theClient )
{
// This method is public, so the pointer must be validated
assert( theClient != 0 ) ;

return removeUser( theClient->getIntYYXXX() ) ;
}

ChannelUser* Channel::removeUser( const unsigned int& intYYXXX )
{

// Attempt to find the user in question
userListType::iterator ptr = userList.find( intYYXXX ) ;

// Was the user found?
if( ptr != userList.end() )
	{
	// Yup, go ahead and remove the user from the userList
	userList.erase( ptr ) ;

	// Return a pointer to the ChannelUser
	return ptr->second ;
	}

// Otherwise, the user was NOT found
// Log the error
elog	<< "Channel::removeUser> ("
	<< getName() << ") "
	<< "Unable to find user: "
	<< intYYXXX
	<< std::endl ;

// Return error state
return 0 ;
}

ChannelUser* Channel::findUser( iClient* theClient ) const
{
assert( theClient != 0 ) ;

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
	elog	<< "Channel::removeUserMode> ("
		<< getName()
		<< ") "
		<< "Unable to find user: "
		<< theClient->getCharYYXXX()
		<< endl ;
	return false ;
	}
theChanUser->removeMode( whichMode ) ;
return true ;
}

bool Channel::setUserMode( const ChannelUser::modeType& whichMode,
	iClient* theClient )
{
// findUser() is also public, and so will verify theClient's pointer
ChannelUser* theChanUser = findUser( theClient ) ;
if( NULL == theChanUser )
	{
	elog	<< "Channel::setUserMode> ("
		<< getName()
		<< ") "
		<< "Unable to find user: "
		<< theClient->getCharYYXXX()
		<< endl ;
	return false ;
	}
theChanUser->setMode( whichMode ) ;
return true ;
}

bool Channel::getUserMode( const ChannelUser::modeType& whichMode,
	iClient* theClient ) const
{

// findUser() is also public, and so will verify theClient's pointer
ChannelUser* theChanUser = findUser( theClient ) ;
if( NULL == theChanUser )
	{
	elog	<< "Channel::getUserMode> ("
		<< getName()
		<< ") "
		<< "Unable to find user: "
		<< theClient->getCharYYXXX()
		<< endl ;
	return false ;
	}
return theChanUser->getMode( whichMode ) ;
}

void Channel::setBan( const string& newBan )
{
// xServer will worry about removing conflicting bans
banList.push_front( newBan ) ;
}

void Channel::removeBan( const string& banMask )
{
for( banListType::iterator ptr = banList.begin(), end = banList.end() ;
	ptr != end ; ++ptr )
	{
	if( !strcasecmp( *ptr, banMask ) )
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
	if( !strcasecmp( *ptr, banMask ) )
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
	if( !match( banMask, *ptr ) )
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
	if( !match( banMask, *ptr ) )
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
		ptr->second->setModeO() ;
		}
	else
		{
		ptr->second->removeModeO() ;
		}
	}
}

void Channel::onModeV( const vector< pair< bool, ChannelUser* > >&
	voiceVector )
{
typedef vector< pair< bool, ChannelUser* > > voiceVectorType ;
for( voiceVectorType::const_iterator ptr = voiceVector.begin() ;
	ptr != voiceVector.end() ; ++ptr )
	{
	if( ptr->first )
		{
		ptr->second->setModeV() ;
		}
	else
		{
		ptr->second->removeModeV() ;
		}
	}
}

/**
 * The banVector passed to this method will be updated to
 * include any bans that have been removed as a result
 * of overlapping bans being added.
 * The order of these additions will be as expected:
 *  an overlapping ban will be put into the banVector,
 *  followed by all bans that it overrides.
 */
void Channel::onModeB( xServer::banVectorType& banVector )
{
typedef xServer::banVectorType banVectorType ;

banVectorType origBans( banVector ) ;
banVector.clear() ;

// Walk through the list of bans being removed/added
for( banVectorType::const_iterator newBanPtr = origBans.begin() ;
	newBanPtr != origBans.end() ; ++newBanPtr )
	{
	banVector.push_back( *newBanPtr ) ;

	// Is the ban being set or removed?
	if( !newBanPtr->first )
		{
		// Removing a ban
		removeBan( newBanPtr->second ) ;
		continue ;
		}

	// Setting a ban
	// Need to check the list of current bans for overlaps
	// This is grossly inefficient, Im open to suggestions

	// Next, search for overlaps
	banIterator currentBanPtr = banList_begin() ;
	for( ; currentBanPtr != banList_end() ; )
		{
		if( !match( newBanPtr->second, *currentBanPtr ) )
			{
			// Add the removed ban to the banVector
			// so that the caller can notify the rest
			// of the system of the removal
			banVector.push_back( banVectorType::value_type(
				false, *currentBanPtr ) ) ;

//			elog	<< "Channel::onModeB> Removing "
//				<< "overlapping ban: "
//				<< *currentBanPtr
//				<< endl ;

			// Overlap, remove the old ban
			currentBanPtr = banList.erase( currentBanPtr ) ;

			}
		else
			{
			// Update the iterator
			++currentBanPtr ;
			}
		} // inner for()

	// Now set the new ban
	// Setting this ban will add the ban into
	// later comparisons, but not this comparison,
	// which is what we want.
	setBan( newBanPtr->second ) ;

	} // outer for()
}

const string Channel::getModeString() const
{

string modeString( "+" ) ;
string argString ;

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
	s	<< getLimit()
		<< ends ;

	argString += s.str() ;

	delete[] s.str() ;
	}

return (modeString + ' ' + argString) ;

}

string Channel::createBan( const iClient* theClient )
{
assert( theClient != 0 ) ;

string theBan = "*!*" ;

// Don't include the '~'
if( (theClient->getUserName().size() >= 2) &&
	('~' == theClient->getUserName()[ 0 ]) )
	{
	theBan += theClient->getUserName().c_str() + 1 ;
	}
else if( !theClient->getUserName().empty() )
	{
	theBan += theClient->getUserName() ;
	}

theBan += '@' ;

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
