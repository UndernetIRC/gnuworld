/*
 * Channel.h
 * Author: Daniel Karrels (dan@karerls.com)
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
 * $Id: Channel.h,v 1.27 2003/06/14 19:35:26 jeekay Exp $
 */

#ifndef __CHANNEL_H
#define __CHANNEL_H "$Id: Channel.h,v 1.27 2003/06/14 19:35:26 jeekay Exp $"

#include	<string>
#include	<map>
#include	<iostream>
#include	<vector>
#include	<list>
#include	<utility>

#include	<ctime>

#include	"ChannelUser.h"
#include	"xparameters.h"
#include	"ELog.h"

namespace gnuworld
{

using std::string ;
using std::map ;
using std::list ;
using std::vector ;
using std::endl ;
using std::pair ;

/// Forward declaration of class iClient.
class iClient ;

/**
 * This class represents a single network channel.
 * Channel users are also maintained in this structure.
 */
class Channel
{

protected:

	/**
	 * The type used to hold this channel's users.
	 */
	typedef map< unsigned int, ChannelUser* > userListType ;

	/**
	 * The type to be used to store channel bans.
	 */
	typedef list< string > banListType ;

	/// Make class xServer a friend of this class.
	friend class xServer ;

public:

	/**
	 * The type used to store this channel's current channel
	 * modes.
	 */
	typedef unsigned short int modeType ;

	/// Bit representing channel mode +t
	static const modeType	MODE_T ;

	/// Bit representing channel mode +n
	static const modeType	MODE_N ;

	/// Bit representing channel mode +s
	static const modeType	MODE_S ;

	/// Bit representing channel mode +p
	static const modeType	MODE_P ;

	/// Bit representing channel mode +k
	static const modeType	MODE_K ;

	/// Bit representing channel mode +l
	static const modeType	MODE_L ;

	/// Bit representing channel mode +m
	static const modeType	MODE_M ;

	/// Bit representing channel mode +i
	static const modeType	MODE_I ;

	/// Bit representing channel mode +r
	static const modeType	MODE_R ;

	/// Type used to store number of clients in channel
	typedef userListType::size_type size_type ;

	/**
	 * The type used for mutable iteration through this
	 * channel's user structure.
	 */
	typedef userListType::iterator userIterator ;

	/**
	 * The type used for immutable iteration through this
	 * channel's user structure.
	 */
	typedef userListType::const_iterator const_userIterator ;

	/**
	 * The type used for immutable iteration through this
	 * channel's ban structure.
	 */
	typedef banListType::const_iterator const_banIterator ;

	/**
	 * The type used for mutable iteration through this
	 * channel's ban structure.
	 */
	typedef banListType::iterator banIterator ;

	/**
	 * Construct a channel of the given name, constructed at the
	 * given creation time.
	 */
	Channel( const string& _name,
		const time_t& _creationTime ) ;

	/**
	 * Destroy this channel.
	 */
	virtual ~Channel() ;

	/**
	 * Test if a given channel mode is set.
	 */
	inline bool getMode( const modeType& whichMode ) const
		{ return (whichMode == (modes & whichMode)) ; }

	/**
	 * Set the given channel mode.
	 */
	inline void setMode( const modeType& whichMode )
		{ modes |= whichMode ; }

	/**
	 * Remove the given channel mode.
	 */
	inline void removeMode( const modeType& whichMode )
		{ modes &= ~whichMode ; }

	/**
	 * Return true if the given mode is set for the the ChannelUser
	 * corresponding to the given iClient.
	 */
	bool getUserMode( const ChannelUser::modeType& whichMode,
		iClient* ) const ;

	/**
	 * Remove the given mode from the ChannelUser associated with
	 * the given iClient.
	 */
	bool removeUserMode( const ChannelUser::modeType& whichMode,
		iClient* ) ;

	/**
	 * Set the given mode for the ChannelUser associated with the
	 * given iClient.
	 */
	bool setUserMode( const ChannelUser::modeType& whichMode,
		iClient* ) ;

	/**
	 * Set a limit on the channel.  This method will set the
	 * channel mode and set the limit.
	 */
	inline void setLimit( const unsigned int& newLimit )
		{ limit = newLimit ; }

	/**
	 * Set a key on the channel.  This method will set the
	 * channel mode and the key as well.
 	 */
	inline void setKey( const string& newKey )
		{ key = newKey ; }

	/**
	 * Add a ban to this Channel's ban list.
	 */
	void setBan( const string& banMask ) ;

	/**
	 * Remove a ban from this channel's ban list.  This does a lexical
	 * comparison, not a wildcard match.
	 */
	void removeBan( const string& banMask ) ;

	/**
	 * Remove all the bans in the channels ban list.
	 */
	inline void removeAllBans()
		{ banList.clear(); }

	/**
	 * Find a ban in the channel's ban list which lexically matches
	 * the given banMask.
	 */
	bool findBan( const string& banMask ) const ;

	/**
	 * Find a ban in the channel's ban list which wildcard matches
	 * the given banMask.
	 */
	bool matchBan( const string& banMask ) const ;

	/**
	 * Search for a ban that matches the mask in (banMask).
	 * If a matching ban is found, it is stored in (matchingBan),
	 * and true is returned.
	 * Otherwise, (matchingBan) is unmodified, and false is
	 * returned.
	 */
	inline bool getMatchingBan( const string& banMask,
		string& matchingBan ) const ;

	/**
	 * Retrieve the current channel modes.
	 */
	inline const modeType& getModes() const
		{ return modes ; }

	/**
	 * Retrieve a string of the current channel modes.
	 */
	const string getModeString() const ;

	/**
	 * Type used to store the size of the banlist.
	 */
	typedef banListType::size_type banListSizeType ;

	/**
	 * Retrieve the number of elements in the ban list.
	 */
	inline banListSizeType banList_size() const
		{ return banList.size() ; }

	/**
	 * Retrieve the name of this channel.
	 */
	inline const string& getName() const
		{ return name ; }

	/**
	 * Retrieve the creation time of this channel.
	 */
	inline const time_t& getCreationTime() const
		{ return creationTime ; }

	/**
	 * Set the creation time of this channel.  This is protected
	 * so that only class xServer may access it externally.
	 */
	inline virtual void	setCreationTime( const time_t& newCT )
		{ creationTime = newCT ; }

	/**
	 * Retrieve this channel's key.  Note that the
	 * existence of a key does not mean that channel
	 * mode +k is set.
	 */
	inline const string& getKey() const
		{ return key ; }

	/**
	 * Retrieve this channel's limit.  Note that the
	 * existence of a limit here does not mean that
	 * channel mode +l is set.
	 */
	inline const unsigned int& getLimit() const
		{ return limit ; }

	/**
	 * Return true if no clients remain in this channel,
	 * false otherwise.
	 */
	inline bool empty() const
		{ return userList.empty() ; }

	/**
	 * Return the number of clients in this channel.
	 */
	inline size_type size() const
		{ return userList.size() ; }

	/**
	 * Retrieve a const_iterator to the beginning of the ban list.
	 */
	inline const_banIterator banList_begin() const
		{ return banList.begin() ; }

	/**
	 * Retrieve a const_iterator to the end of the channel ban list.
	 */
	inline const_banIterator banList_end() const
		{ return banList.end() ; }

	/**
	 * Retrieve an iterator to the beginning of the ban list.
	 */
	inline banIterator banList_begin()
		{ return banList.begin() ; }

	/**
	 * Retrieve an iterator to the end of the channel ban list.
	 */
	inline banIterator banList_end()
		{ return banList.end() ; }

	/**
	 * Retrieve an iterator to the beginning of
	 * this channel's user structure.
	 */
	inline userIterator userList_begin()
		{ return userList.begin() ; }

	/**
	 * Retrieve an iterator to the end of
	 * this channel's user structure.
	 */
	inline userIterator userList_end()
		{ return userList.end() ; }

	/**
	 * Retrieve a const iterator to the beginning of
	 * this channel's user structure.
	 */
	inline const_userIterator userList_begin() const
		{ return userList.begin() ; }

	/**
	 * Retrieve a const iterator to the end of this
	 * channel's user structure.
	 */
	inline const_userIterator userList_end() const
		{ return userList.end() ; }

	/**
	 * Add a ChannelUser to this channel's internal user
	 * structure.
	 */
	bool addUser( ChannelUser* newUser ) ;

	/**
	 * Add an iClient to this chanenl's internal user
	 * structure.
	 */
	bool addUser( iClient* theClient ) ;

	/**
	 * Remove a ChannelUser from this channel's internal
	 * user structure.  Note that the memory associated
	 * with the ChannelUser is NOT deallocated, it is
	 * returned to the caller.
	 */
	ChannelUser* removeUser( iClient* theClient ) ;

	/**
	 * Remove the given ChannelUser from this Channel.
	 */
	ChannelUser* removeUser( ChannelUser* theUser ) ;

	/**
	 * Remove a ChannelUser from this channel's internal
	 * user structure.  Note that the memory associated
	 * with the ChannelUser is NOT deallocated, it is
	 * returned to the caller.
	 */
	ChannelUser* removeUser( const unsigned int& intYYXXX ) ;

	/**
	 * Return the ChannelUser associated with the given iClient,
	 * NULL if not found.
	 */
	ChannelUser* findUser( const iClient* theClient ) const ;

#ifdef TOPIC_TRACK	
	
	/**
	 * Returns the channel topic (if TOPIC_TRACK is defined)
	 */
    	const string& getTopic() const
		{ return topic; }

	/**
	 * Sets this channel's topic value to the value passed in.
	 * This method exists only if TOPIC_TRACK is defined.
	 */
	void setTopic(const string& _Topic)
		{ topic = _Topic; }
	
#endif	
	
	/**
	 * Convenience operator for outputting Channel information
	 * to a C++ output stream.
	 */
	friend ELog& operator<<( ELog& out, const Channel& rhs )
		{
		out	<< "Name: " << rhs.name
			<< ", creation time: " << rhs.creationTime ;
		return out ;
		}

	/**
	 * Return a level 2 ban for the given user.
	 */
	static string createBan( const iClient* ) ;

protected:

	/**
	 * This method is called when channel mode 't' is set
	 * or unset.
	 */
	virtual void	onModeT( bool ) ;

	/**
	 * This method is called when channel mode 'n' is set
	 * or unset.
	 */
	virtual void	onModeN( bool ) ;

	/**
	 * This method is called when channel mode 's' is set
	 * or unset.
	 */
	virtual void	onModeS( bool ) ;

	/**
	 * This method is called when channel mode 'p' is set
	 * or unset.
	 */
	virtual void	onModeP( bool ) ;

	/**
	 * This method is called when channel mode 'm' is set
	 * or unset.
	 */
	virtual void	onModeM( bool ) ;

	/**
	 * This method is called when channel mode 'i' is set
	 * or unset.
	 */
	virtual void	onModeI( bool ) ;

	/**
	 * This method is called when channel mode 'r' is set
	 * or unset.
	 */
	virtual void	onModeR( bool ) ;

	/**
	 * This method is called when channel mode 'l' is set
	 * or unset.
	 */
	virtual void	onModeL( bool, const unsigned int& ) ;

	/**
	 * This method is called when channel mode 'k' is set
	 * or unset.
	 */
	virtual void	onModeK( bool, const string& ) ;

	/**
	 * This method is called when one or more channel
	 * mode (t)'s is/are set or unset.
	 */
	virtual void	onModeO( const vector<
				pair< bool, ChannelUser* > >& ) ;

	/**
	 * This method is called when one or more channel
	 * mode (v)'s is/are set or unset.
	 */
	virtual void	onModeV( const vector<
				pair< bool, ChannelUser* > >& ) ;

	/**
	 * This method is called when one or more channel
	 * mode (b)'s is/are set or unset.
	 * This method will add to the vector passed to it any
	 * bans that have been removed as a result of newly added
	 * overlapping bans.
	 */
	virtual void	onModeB( vector< pair< bool, string > >& ) ;

	/**
	 * The name of this channel.
	 */
	string		name ;

	/**
	 * The time at which this channel was created.
	 */
	time_t		creationTime ;

	/**
	 * This channel's current modes.
	 */
	modeType	modes ;

	/**
	 * The limit associated with this channel.
	 * Note that this variable may hold values
	 * even when channel mode +l is NOT set.
	 */
	unsigned int	limit ;

	/**
	 * The key associated with this channel.
	 * Note that this variable may hold values
	 * even when channel mode +k is NOT set.
	 */
	string		key ;

	/**
	 * The structure used to hold the ChannelUser
	 * instances.
	 */
	userListType	userList ;

	/**
	 * The structure used to store the channel bans.
	 */
	banListType	banList ;
	
#ifdef TOPIC_TRACK
	/**
	 * This channel's topic, only if TOPIC_TRACK is defined.
	 */
	string		topic;
#endif

} ;

} // namespace gnuworld

#endif // __CHANNEL_H
