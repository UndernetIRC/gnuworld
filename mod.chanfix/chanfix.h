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
 * $Id: chanfix.h,v 1.2 2004/05/18 20:55:02 jeekay Exp $
 */

#ifndef CHANFIX_H
#define CHANFIX_H

#include <string.h>

#include "client.h"

#include "logging.h"

namespace gnuworld {

class EConfig;

namespace chanfix {

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
	virtual void log(const logging::loglevel&, const string&);
	virtual void setConsoleTopic();
		
	
protected:
	/*************************
	 * C O N F I G   V A R S *
	 *************************/
	
	/** Name of our console channel. */
	string confConsoleChannel;
	/** Modes of our console channel. */
	string confConsoleModes;
	
	/** Points to award a +r op per period. */
	unsigned short confOpAuth;
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
