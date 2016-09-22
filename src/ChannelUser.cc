/**
 * ChannelUser.cc
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
 * $Id: ChannelUser.cc,v 1.10 2009/07/26 18:30:37 mrbean_ Exp $
 */

#include	<string>
#include	<vector>

#include	<cassert>

#include	"iClient.h"
#include	"ChannelUser.h"

RCSTAG("$Id: ChannelUser.cc,v 1.10 2009/07/26 18:30:37 mrbean_ Exp $" ) ;

namespace gnuworld
{

using std::string ;

const ChannelUser::modeType ChannelUser::MODE_O = 0x01 ;
const ChannelUser::modeType ChannelUser::MODE_V = 0x02 ;
//const ChannelUser::modeType ChannelUser::ZOMBIE = 0x04 ;

ChannelUser::ChannelUser( iClient* _theClient )
 : theClient( _theClient ),
   modes( 0 )
{
assert( theClient != 0 ) ;
}

ChannelUser::~ChannelUser()
{}

const string& ChannelUser::getNickName() const
{
return theClient->getNickName() ;
}

const string& ChannelUser::getUserName() const
{
return theClient->getUserName() ;
}

const string& ChannelUser::getHostName() const
{
return theClient->getInsecureHost() ;
}

const irc_in_addr& ChannelUser::getIP() const
{
return theClient->getIP() ;
}

const string ChannelUser::getCharYYXXX() const
{
return theClient->getCharYYXXX() ;
}

unsigned int ChannelUser::getIntYY() const
{
return theClient->getIntYY() ;
}

unsigned int ChannelUser::getIntXXX() const
{
return theClient->getIntXXX() ;
}

unsigned int ChannelUser::getIntYYXXX() const
{
return theClient->getIntYYXXX() ;
}

bool ChannelUser::isOper() const
{
return theClient->isOper() ;
}

string ChannelUser::getModeString() const
{
std::vector< string > modes ;	

if( isOper() )
	{
	modes.push_back( "oper" ) ;
	}
if( isModeV() )
	{
	modes.push_back( "voice" ) ;
	}
if( isModeO() )
	{
	modes.push_back( "op" ) ;
	}

string retMe ;
for( std::vector< string >::size_type i = 0 ; i < modes.size() ; ++i )
	{
	retMe += modes[ i ] ;
	if( (i + 1) < modes.size() )
		{
		retMe += "," ;
		}
	}
return retMe ;
}

} //  namespace gnuworld
