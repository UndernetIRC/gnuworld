/* Channel.h */

#ifndef __CHANNEL_H
#define __CHANNEL_H

#include	<string>
#include	<map>
#include	<iostream>
#include	<vector>
#include	<list>

#include	<ctime>

#include	"ChannelUser.h"
#include	"xparameters.h"
#include	"ELog.h"

using std::string ;
using std::map ;
using std::list ;

namespace gnuworld
{

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
	static const modeType	MODE_T = 0x01 ;

	/// Bit representing channel mode +n
	static const modeType	MODE_N = 0x02 ;

	/// Bit representing channel mode +s
	static const modeType	MODE_S = 0x04 ;

	/// Bit representing channel mode +p
	static const modeType	MODE_P = 0x10 ;

	/// Bit representing channel mode +k
	static const modeType	MODE_K = 0x20 ;

	/// Bit representing channel mode +l
	static const modeType	MODE_L = 0x40 ;

	/// Bit representing channel mode +m
	static const modeType	MODE_M = 0x80 ;

	/// Bit representing channel mode +i
	static const modeType	MODE_I = 0x100 ;

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
		{ return (modes & whichMode) ; }

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
	inline bool getUserMode( const ChannelUser::modeType& whichMode,
		iClient* ) const ;

	/**
	 * Remove the given mode from the ChannelUser associated with
	 * the given iClient.
	 */
	inline bool removeUserMode( const ChannelUser::modeType& whichMode,
		iClient* ) ;

	/**
	 * Set the given mode for the ChannelUser associated with the
	 * given iClient.
	 */
	inline bool setUserMode( const ChannelUser::modeType& whichMode,
		iClient* ) ;

	/**
	 * Set a limit on the channel.  This method will set the
	 * channel mode and set the limit.
	 */
	inline void setLimit( const unsigned int& newLimit )
		{ limit = newLimit ; setMode( MODE_L ) ; }

	/**
	 * Set a key on the channel.  This method will set the
	 * channel mode and the key as well.
 	 */
	inline void setKey( const string& newKey )
		{ key = newKey ; setMode( MODE_K ) ; }

	/**
	 * The const_iterator type used to perform a read-only traversal
	 * of the channel ban list.
	 */
	typedef banListType::const_iterator banListConstIterator ;

	/**
	 * Retrieve a const_iterator to the beginning of the ban list.
	 */
	inline banListConstIterator banList_begin() const
		{ return banList.begin() ; }

	/**
	 * Retrieve a const_iterator to the end of the channel ban list.
	 */
	inline banListConstIterator banList_end() const
		{ return banList.end() ; }

	/**
	 * Add a ban to this Channel's ban list.
	 */
	inline void setBan( const string& banMask ) ;

	/**
	 * Remove a ban from this channel's ban list.  This does a lexical
	 * comparison, not a wildcard match.
	 */
	inline void removeBan( const string& banMask ) ;

	/**
	 * Find a ban in the channel's ban list which lexically matches
	 * the given banMask.
	 */
	inline bool findBan( const string& banMask ) const ;

	/**
	 * Find a ban in the channel's ban list which wildcard matches
	 * the given banMask.
	 */
	inline bool matchBan( const string& banMask ) const ;

	/**
	 * Retrieve the current channel modes.
	 */
	inline const modeType& getModes() const
		{ return modes ; }

	inline const string getModeString() const ;

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
	inline bool addUser( ChannelUser* newUser ) ;

	/**
	 * Remove a ChannelUser from this channel's internal
	 * user structure.  Note that the memory associated
	 * with the ChannelUser is NOT deallocated, it is
	 * returned to the caller.
	 */
	inline ChannelUser* removeUser( iClient* theClient ) ;

	/**
	 * Remove a ChannelUser from this channel's internal
	 * user structure.  Note that the memory associated
	 * with the ChannelUser is NOT deallocated, it is
	 * returned to the caller.
	 */
	inline ChannelUser* removeUser( const unsigned int& intYYXXX ) ;

	/**
	 * Return the ChannelUser associated with the given iClient,
	 * NULL if not found.
	 */
	inline ChannelUser* findUser( iClient* theClient ) const ;

	/**
	 * Convenience operator for outputting Channel information
	 * to a C++ output stream.
	 */
	friend ELog& operator<<( ELog& out, const Channel& rhs )
	{
	out	<< "Name: " << rhs.name << endl
		<< "Creation time: " << rhs.creationTime ;
	return out ;
	}

protected:

	virtual void	onModeT( bool ) ;
	virtual void	onModeN( bool ) ;
	virtual void	onModeS( bool ) ;
	virtual void	onModeP( bool ) ;
	virtual void	onModeM( bool ) ;
	virtual void	onModeI( bool ) ;
	virtual void	onModeL( bool, const unsigned int& ) ;
	virtual void	onModeK( bool, const string& ) ;
	virtual void	onModeO( const vector<
				pair< bool, ChannelUser* > >& ) ;
	virtual void	onModeV( const vector<
				pair< bool, ChannelUser* > >& ) ;
	virtual void	onModeB( const vector<
				pair< bool, string > >& ) ;

	/**
	 * Set the creation time of this channel.  This is protected
	 * so that only class xServer may access it externally.
	 */
	inline virtual void	setCreationTime( const time_t& newCT )
		{ creationTime = newCT ; }

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

} ;

} // namespace gnuworld

#endif // __CHANNEL_H
