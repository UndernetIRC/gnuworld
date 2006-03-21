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
 * $Id: sqlChannel.h,v 1.2 2006/03/21 23:12:37 buzlip01 Exp $
 */

#ifndef __SQLCHANNEL_H
#define __SQLCHANNEL_H "$Id: sqlChannel.h,v 1.2 2006/03/21 23:12:37 buzlip01 Exp $"

#include	<string>
#include	<ctime>
#include	"libpq++.h"

namespace gnuworld
{

class sqlUser;
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
	static const int	EV_BLOCK; /* Channel block */
	static const int	EV_UNBLOCK; /* Channel unblock */
	static const int	EV_ALERT; /* Channel alert */
	static const int	EV_UNALERT; /* Channel unalert */

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

	inline time_t		getFixStart() const
		{ return start ; }

	inline unsigned int	getMaxScore() const
		{ return maxScore ; }

	inline bool		getModesRemoved() const
		{ return modesRemoved ; }

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

	inline void	setFixStart(time_t _start)
		{ start = _start; }

	inline void	setMaxScore(unsigned int _maxScore)
		{ maxScore = _maxScore; }

	inline void	setModesRemoved(bool _modesRemoved)
		{ modesRemoved = _modesRemoved; }

	inline void	setUseSQL(bool _inSQL)
		{ inSQL = _inSQL; }

	bool Insert();
	bool Delete();
	bool commit();
	void setAllMembers(PgDatabase*, int);

	/** Static member for keeping track of max user id */
	static unsigned long int maxUserId;

	void addNote(unsigned short, sqlUser*, const std::string&);
	bool deleteNote(unsigned int);
	bool deleteOldestNote();
	bool deleteAllNotes();
	size_t countNotes(unsigned short);
	const std::string getLastNote(unsigned short, time_t&);

protected:

	unsigned int	id;
	std::string	channel;
	time_t		last;
	time_t		start;
	unsigned int	maxScore;
	bool		modesRemoved;
	flagType	flags;
	bool		inSQL;

	sqlManager*	myManager;
}; // class sqlChannel

} // namespace gnuworld

#endif // __SQLCHANNEL_H
