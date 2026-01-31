/*
 * Channel.cc
 * Author: Daniel Karrels (dan@karrels.com)
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
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
 *
 * $Id: Channel.cc,v 1.55 2008/04/16 20:29:37 danielaustin Exp $
 */
#include	<new>
#include	<map>
#include	<string>
#include	<iostream>
#include	<sstream>
#include	"Channel.h"
#include	"iClient.h"
#include	"ChannelUser.h"
#include	"Network.h"
#include	"xparameters.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"match.h"
#include	"server.h"

namespace gnuworld
{
using std::vector ;
using std::string ;
using std::endl ;
using std::stringstream ;

const Channel::modeType Channel::MODE_T	      = 0x00001 ;
const Channel::modeType Channel::MODE_N       = 0x00002 ;
const Channel::modeType Channel::MODE_S       = 0x00004 ;
const Channel::modeType Channel::MODE_P       = 0x00008 ;
const Channel::modeType Channel::MODE_K       = 0x00010 ;
const Channel::modeType Channel::MODE_L       = 0x00020 ;
const Channel::modeType Channel::MODE_M       = 0x00040 ;
const Channel::modeType Channel::MODE_I       = 0x00080 ;
const Channel::modeType Channel::MODE_R       = 0x00100 ;
const Channel::modeType Channel::MODE_D       = 0x00200 ;
const Channel::modeType Channel::MODE_A       = 0x00400 ;
const Channel::modeType Channel::MODE_U       = 0x00800 ;
const Channel::modeType Channel::MODE_REG     = 0x01000 ;
const Channel::modeType Channel::MODE_C       = 0x02000 ;
const Channel::modeType Channel::MODE_CTCP    = 0x04000 ;
const Channel::modeType Channel::MODE_PART    = 0x08000 ;
const Channel::modeType Channel::MODE_MNOREG  = 0x10000 ;
const Channel::modeType Channel::MODE_Z       = 0x20000 ;

Channel::Channel( const string& _name,
	const time_t& _creationTime )
 : name( _name ),
   creationTime( _creationTime ),
   modes( 0 ),
   limit( 0 )
#ifdef TOPIC_TRACK
	 ,topic_ts( 0 )
#endif
{}

Channel::~Channel()
{
// Deallocate all ChannelUser's that are left
userIterator currentPtr = userList_begin() ;
userIterator endPtr = userList_end() ;
for( ; currentPtr != endPtr ; ++currentPtr )
	{
	delete currentPtr->second ;
	}
userList.clear() ;
}

bool Channel::addUser( ChannelUser* newUser )
{
assert( newUser != 0 ) ;

#ifdef USE_THREAD
std::unique_lock< std::shared_mutex > lock( chanMutex ) ;
#endif

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
#ifdef USE_THREAD
std::unique_lock< std::shared_mutex > lock( chanMutex ) ;
#endif

ChannelUser* tmpUser;

// Attempt to find the user in question
userIterator ptr = userList.find( intYYXXX ) ;

// Was the user found?
if( ptr != userList.end() )
	{
	// Yup, go ahead and remove the user from the userList
	tmpUser = ptr->second;

	userList.erase( ptr ) ;

	// Return a pointer to the ChannelUser
	return tmpUser;
	}

// Otherwise, the user was NOT found
// Log the error
// Happens when not handling zombies
//elog	<< "Channel::removeUser> ("
//	<< getName() << ") "
//	<< "Unable to find user: "
//	<< intYYXXX
//	<< std::endl ;

// Return error state
return 0 ;
}

ChannelUser* Channel::findUser( const iClient* theClient ) const
{
assert( theClient != 0 ) ;

const_userIterator ptr = userList.find( theClient->getIntYYXXX() ) ;
if( ptr == userList_end() )
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

bool Channel::removeBan( const string& banMask )
{
for( banIterator ptr = banList_begin(), end = banList_end() ;
	ptr != end ; ++ptr )
	{
	if( !strcasecmp( *ptr, banMask ) )
		{
		banList.erase( ptr ) ;
		// Ban found, return true
		return true ;
		}
	}
// Ban not found
return false ;
}

bool Channel::findBan( const string& banMask ) const
{
for( const_banIterator ptr = banList_begin(),
	end = banList_end() ; ptr != end ; ++ptr )
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
for( const_banIterator ptr = banList_begin(),
	end = banList_end() ; ptr != end ; ++ptr )
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
for( const_banIterator ptr = banList_begin(),
	end = banList_end() ; ptr != end ; ++ptr )
	{
	if( !match( banMask, *ptr ) )
		{
		matchingBan = *ptr ;
		return true ;
		}
	}
return false ;
}

void Channel::onMode(
	const vector< std::pair< bool, Channel::modeType > >& modeVector )
{
typedef vector< std::pair< bool, Channel::modeType > > modeVectorType ;
for( modeVectorType::const_iterator mItr = modeVector.begin() ;
	mItr != modeVector.end() ; ++mItr )
	{
	bool polarity = (*mItr).first ;

	if( polarity )		setMode( (*mItr).second ) ;
	else			removeMode( (*mItr).second ) ;
	} // for()
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
	setKey( string() ) ;
	}
}

void Channel::onModeA( bool polarity, const string& newApass )
{
if( polarity )
	{
	setMode( MODE_A ) ;
	setApass( newApass ) ;
	}
else
	{
	removeMode( MODE_K ) ;
	setApass( string() ) ;
	}
}

void Channel::onModeU( bool polarity, const string& newUpass )
{
if( polarity )
	{
	setMode( MODE_U ) ;
	setUpass( newUpass ) ;
	}
else
	{
	removeMode( MODE_U ) ;
	setUpass( string() ) ;
	}
}

void Channel::onModeO( const vector< std::pair< bool, ChannelUser* > >&
	opVector )
{
typedef vector< std::pair< bool, ChannelUser* > > opVectorType ;
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

void Channel::onModeV( const vector< std::pair< bool, ChannelUser* > >&
	voiceVector )
{
typedef vector< std::pair< bool, ChannelUser* > > voiceVectorType ;
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

if( modes & MODE_T )		modeString += 't' ;
if( modes & MODE_N )		modeString += 'n' ;
if( modes & MODE_S )		modeString += 's' ;
if( modes & MODE_P )		modeString += 'p' ;
if( modes & MODE_M )		modeString += 'm' ;
if( modes & MODE_I )		modeString += 'i' ;
if( modes & MODE_R )		modeString += 'r' ;
if( modes & MODE_REG )		modeString += 'R' ;
if( modes & MODE_D )		modeString += 'D' ;
if( modes & MODE_C )		modeString += 'c';
if( modes & MODE_CTCP )		modeString += 'C';
if( modes & MODE_PART )		modeString += 'u';
if( modes & MODE_MNOREG )	modeString += 'M';
if( modes & MODE_Z )		modeString += 'Z';

if( modes & MODE_K )
	{
	modeString += 'k' ;
	argString += getKey() + ' ' ;
	}

if( modes & MODE_L )
	{
	modeString += 'l' ;

	// Can't put numerical variables into a string
	stringstream s ;
	s	<< getLimit() ;

	argString += s.str() ;
	}

if( modes & MODE_A )
	{
	modeString += 'A' ;
	argString += getApass() + ' ' ;
	}

if( modes & MODE_U )
	{
	modeString += 'U' ;
	argString += getUpass() + ' ' ;
	}


return (modeString + ' ' + argString) ;
}

string Channel::createBan( const iClient* theClient )
{
	assert( theClient != 0 ) ;

	if ((theClient->isModeX()) && (theClient->isModeR()))
		return "*!*@" + theClient->getInsecureHost();
	else
		return createBanMask(theClient->getNickUserHost());
}

void Channel::removeAllModes()
{
for( userIterator uItr = userList_begin() ; uItr != userList_end() ;
	++uItr )
	{
	ChannelUser* theUser = uItr->second ;
	assert( theUser != 0 ) ;

	theUser->removeModeO() ;
	theUser->removeModeV() ;
	}
modes = 0 ;
limit = 0 ;
#if __GNUC__ == 2
key = "";
Apass = "";
Upass = "";
#else
key.clear();
Apass.clear();
Upass.clear();
#endif
}

} // namespace gnuworld
