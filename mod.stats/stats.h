/**
 * stats.h
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
 *                    Orlando Bassotto
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
 * $Id: stats.h,v 1.4 2002/08/08 18:32:34 dan_karrels Exp $
 */

#ifndef __STATS_H
#define __STATS_H "$Id: stats.h,v 1.4 2002/08/08 18:32:34 dan_karrels Exp $"

#include	<fstream>
#include	<string>
#include	<map>

#include	"client.h"
#include	"iClient.h"
#include	"misc.h" // noCaseCompare
#include	"server.h"

namespace gnuworld
{

using std::ofstream ;
using std::string ;
using std::map ;

class stats : public xClient
{

protected:
	typedef map< string, ofstream*, noCaseCompare > fileTableType ;

public:
	stats( const string& ) ;
	virtual ~stats() ;

	typedef fileTableType::iterator fileIterator ;
	typedef fileTableType::const_iterator constFileIterator ;

	virtual void ImplementServer( xServer* ) ;
	virtual int OnPrivateMessage( iClient*, const string&,
		bool = false ) ;
	virtual int OnEvent( const eventType&,
		void* = 0, void* = 0, void* = 0, void* = 0 ) ;
        virtual int OnChannelEvent( const channelEventType&,
                Channel*,  
                void* Data1 = NULL, void* Data2 = NULL,
                void* Data3 = NULL, void* Data4 = NULL ) ;

protected:
	void			WriteLog( const string& file,
					const string& line = string() ) ;

	string			data_path ;
	fileTableType		fileTable ;

	bool			logDuringBurst ;

} ;

} // namespace gnuworld

#endif // __STATS_h
