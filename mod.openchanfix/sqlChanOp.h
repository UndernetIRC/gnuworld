/**
 * sqlChanOp.h
 *
 * Author: Matthias Crauwels <ultimate_@wol.be>
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
 * $Id: sqlChanOp.h,v 1.2 2006/03/21 23:12:37 buzlip01 Exp $
 */

#ifndef __SQLCHANOP_H
#define __SQLCHANOP_H "$Id: sqlChanOp.h,v 1.2 2006/03/21 23:12:37 buzlip01 Exp $"

#include	<string>
#include	"libpq++.h"
#include	"chanfix_config.h"

namespace gnuworld
{

class sqlManager;

class sqlChanOp {

public:
	sqlChanOp(sqlManager*);
	virtual ~sqlChanOp();


	/*
	 *  Methods to get data attributes.
	 */

	inline const std::string&	getChannel() const
		{ return channel ; }

	inline const std::string&	getAccount() const
		{ return account ; }

	inline const int&	getPoints() const
		{ return points ; }		

	inline const std::string&	getLastSeenAs() const
		{ return nickUserHost ; }

	inline const time_t	getTimeFirstOpped() const
		{ return ts_firstopped ; }

	inline const time_t	getTimeLastOpped() const
		{ return ts_lastopped ; }

	inline const short&	getDay(int _dayval) const
		{ return day[_dayval] ; }

	/*
	 *  Methods to set data attributes.
	 */

	inline void	setChannel(std::string _channel)
		{ channel = _channel ; }

	inline void	setAccount(std::string _account)
		{ account = _account ; }

	inline void	setPoints(short _points)
		{ day[currentDay] = _points; calcTotalPoints(); }

	inline void	addPoint()
		{ day[currentDay]++; calcTotalPoints(); }

	inline void	setLastSeenAs(std::string _nickUserHost)
		{ nickUserHost = _nickUserHost ; }

	inline void     setTimeFirstOpped(time_t _ts_firstopped)
		{ ts_firstopped = _ts_firstopped ; }

	inline void	setTimeLastOpped(time_t _ts_lastopped)
		{ ts_lastopped = _ts_lastopped ; }

	inline void	setDay(int _dayval, short _pointsval)
		{ day[_dayval] = _pointsval ; }

	void setAllMembers(PgDatabase*, int);
	void calcTotalPoints();

private:

	std::string	channel;
	std::string	account;
	std::string	nickUserHost;
	int		points;
	time_t		ts_firstopped;
	time_t		ts_lastopped;
	short		day[DAYSAMPLES];

	sqlManager*	myManager;
}; // class

} // namespace gnuworld


#endif // __SQLCHANOP_H
