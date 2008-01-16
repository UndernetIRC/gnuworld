/**
 * sqlChannel.h
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
 * $Id: sqlChannel.h,v 1.7 2008/01/16 02:03:39 buzlip01 Exp $
 */

#ifndef __SQLCHANNEL_H
#define __SQLCHANNEL_H "$Id: sqlChannel.h,v 1.7 2008/01/16 02:03:39 buzlip01 Exp $"

#include	<string>
#include	<ctime>
#include	"client.h"
#include	"dbHandle.h"

namespace gnuworld
{

namespace cf
{

class sqlcfUser;
class sqlManager;

class sqlChannel
{
public:
	sqlChannel(sqlManager*);
	virtual ~sqlChannel();

	typedef unsigned int	flagType ;

	static const flagType	F_BLOCKED;
	static const flagType	F_ALERT;

	/*
	 *   Channel 'Event' Flags, used in the channelog table.
	 *   These flags are used to filter channel log records.
	 *   in reports.
	 */

	static const int	EV_MISC; /* Uncategorised event */
	static const int	EV_NOTE; /* Miscellaneous notes */
	static const int	EV_CHANFIX; /* Manual chanfixes */
	static const int	EV_SIMULATE; /* Fix simulation */
	static const int	EV_BLOCK; /* Channel block */
	static const int	EV_TEMPBLOCK; /* Temp channel block */
	static const int	EV_UNTEMPBLOCK; /* Temp channel block */
	static const int	EV_UNBLOCK; /* Channel unblock */
	static const int	EV_ALERT; /* Channel alert */
	static const int	EV_UNALERT; /* Channel unalert */
	static const int	EV_REQUESTOP; /* Requestops */

	/*
	 *  Methods to get data atrributes.
	 */


	inline const unsigned int&	getID() const
		{ return id ; }

	inline const std::string&	getChannel() const
		{ return channel ; }

	inline const flagType&	getFlags() const
		{ return flags ; }

	inline bool	getFlag( const flagType& whichFlag ) const
		{ return (flags & whichFlag) ; }

	inline time_t		getLastAttempt() const
		{ return last ; }

	inline time_t		getLastSimAttempt() const
		{ return simlast ; }

	inline time_t		getFixStart() const
		{ return start ; }

	inline time_t		getSimStart() const
		{ return simstart ; }

	inline int	getMaxScore() const
		{ return maxScore ; }

	inline int	getAmountSimOpped() const
		{ return amtopped ; }

	inline int	getTMaxScore() const
		{ return tmaxScore ; }

	inline bool		getModesRemoved() const
		{ return modesRemoved ; }

	inline bool		getSimModesRemoved() const
		{ return simModesRemoved ; }

	inline bool		useSQL() const
		{ return inSQL ; }

	/*
	 *  Methods to set data atrributes.
	 */

        // 'ID' is a primary key and cannot be altered.

	inline void	setID( const unsigned int& _id )
		{ id = _id; }

	inline void 	setChannel(const std::string& _channel)
		{ channel = _channel; }

	inline void	setFlag( const flagType& whichFlag )
		{ flags |= whichFlag; }

	inline void	removeFlag( const flagType& whichFlag )
		{ flags &= ~whichFlag; }

	inline void	clearFlags()
		{ flags = 0; }

	inline void     setLastAttempt (time_t _last)
		{ last = _last; }

	inline void     setLastSimAttempt (time_t _simlast)
		{ simlast = _simlast; }

	inline void	setFixStart(time_t _start)
		{ start = _start; }

	inline void	setSimStart(time_t _simstart)
		{ simstart = _simstart; }

	inline void	setAmountSimOpped(unsigned int _amtopped)
		{ amtopped = _amtopped; }

	inline void	setMaxScore(unsigned int _maxScore)
		{ maxScore = _maxScore; }

	inline void	setTMaxScore(unsigned int _tmaxScore)
		{ tmaxScore = _tmaxScore; }

	inline void	setModesRemoved(bool _modesRemoved)
		{ modesRemoved = _modesRemoved; }

	inline void	setSimModesRemoved(bool _simModesRemoved)
		{ simModesRemoved = _simModesRemoved; }

	inline void	setUseSQL(bool _inSQL)
		{ inSQL = _inSQL; }

	bool Insert(dbHandle*);
	bool Delete(dbHandle*);
	bool commit(dbHandle*);
	void setAllMembers(dbHandle*, int);

	/** Static member for keeping track of max user id */
	static unsigned long int maxUserId;

	void addNote(dbHandle*, unsigned short, iClient*, const std::string&);
	bool deleteNote(dbHandle*, unsigned int);
	bool deleteOldestNote(dbHandle*);
	bool deleteAllNotes(dbHandle*);
	size_t countNotes(dbHandle*, unsigned short);
	const std::string getLastNote(dbHandle*, unsigned short, time_t&);

protected:

	unsigned int	id;
	std::string	channel;
	std::string	user_name;
	time_t		last;
	time_t		simlast;
	time_t		start;
	time_t		simstart;
	unsigned int	amtopped;
	unsigned int	maxScore;
	unsigned int	tmaxScore;
	bool		modesRemoved;
	bool		simModesRemoved;
	flagType	flags;
	bool		inSQL;

	sqlManager*	myManager;
}; // class sqlChannel

} // namespace cf

} // namespace gnuworld

#endif // __SQLCHANNEL_H
