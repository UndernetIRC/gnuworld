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
class Command;

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
	virtual void OnCTCP( iClient* , const string& ,
		const string& , bool );
	virtual void OnPrivateMessage( iClient* , const string& , bool );
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
	virtual cfChannel* getChannel(const string&, bool create = false);


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
	/** Maximum points a user can get. */
	unsigned int confMaxPoints;
	/** Duration of a period in seconds. */
	unsigned short confPeriod;
	/** Duration to wait between linking and counting. */
	unsigned short confStartDelay;


	/***************
	 * T I M E R S *
	 ***************/

	/** Counting timer. */
	xServer::timerID timerCount;
	
	
	/*******************
	 * C O M M A N D S *
	 *******************/
	
	/** Type of the commandMap. */
	typedef std::map< string , Command* , noCaseCompare > commandMapType;
	/** Convenience type when creating a new command pair. */
	typedef commandMapType::value_type commandPairType;
	/** Map holding all available bot commands. */
	commandMapType commandMap;
	/** Register a command. */
	bool RegisterCommand(Command*);
};

} // namespace chanfix

} // namespace gnuworld

#endif
