/**
 * chanfix.h
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
 * $Id: chanfix.h,v 1.4 2004/05/25 21:17:53 jeekay Exp $
 */

#ifndef CF_CHANFIX_H
#define CF_CHANFIX_H

#include <map>

#include <string.h>

#include "client.h"

#include "logging.h"

namespace gnuworld {

namespace chanfix {

class cfChannel;

class chanfix : public xClient {
public:
	/** Constructor taking a configuration filename. */
	chanfix( const std::string& );

	/** Destructor. */
	virtual ~chanfix();


	/*************************************
	 * X C L I E N T   F U N C T I O N S *
	 *************************************/
	virtual void OnAttach();
	virtual void BurstChannels();
	virtual void OnTimer( const TimerHandler::timerID& , void* );


	/***************************
	 * C H A N F I X   M I S C *
	 ***************************/
	virtual void log(const logging::loglevel&, const char*, ... );
	virtual void log(const logging::loglevel&, const string&);
	virtual void setConsoleTopic();


	/***************************
	 * C H A N F I X   C O R E *
	 ***************************/
	virtual void doCountUpdate();
	virtual cfChannel* getChannel(const string&);


protected:
	/***********************
	 * C O N T A I N E R S *
	 ***********************/

	 typedef std::map< string , cfChannel* > mapChannels;
	 mapChannels channels;


	/*************************
	 * C O N F I G   V A R S *
	 *************************/

	/** Name of our console channel. */
	string confConsoleChannel;
	/** Modes of our console channel. */
	string confConsoleModes;

	/** Bitmap of what to log. */
	logging::loglevel confLogLevel;

	/** Points to award a +r op per period. */
	unsigned short confPointsAuth;
	/** Duration of a period in seconds. */
	unsigned short confPeriod;
	/** Duration to wait between linking and counting. */
	unsigned short confStartDelay;


	/***************
	 * T I M E R S *
	 ***************/

	/** Counting timer. */
	xServer::timerID timerCount;
};

} // namespace chanfix

} // namespace gnuworld

#endif
