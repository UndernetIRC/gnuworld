/**
 * msg_JU.cc
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
 * $Id: msg_JU.cc,v 1.9 2010/09/20 17:36:25 denspike Exp $
 */

#include	<string>

#include	"gnuworld_config.h"
#include	"server.h"
#include 	"Network.h"
#include	"xparameters.h"
#include	"ServerCommandHandler.h"

RCSTAG( "$Id: msg_JU.cc,v 1.9 2010/09/20 17:36:25 denspike Exp $" ) ;

namespace gnuworld
{

using std::endl;

CREATE_HANDLER(msg_JU)

/**
 * JUPE message handler.
 */
bool msg_JU::Execute( const xParameters& Param)
{

if (Param.size() < 6)
	{
	elog 	<< "msg_JU: Invalid number of arguments" 
		<< endl;
	return false;
	}
if(Param[2][0] == '+') 
	{
	/*
	 * A new jupe is interduced, need to create an iServer for it
	 * and notify all the modules
	 */
	std::string Reason = Param.assemble(5);
	std::string SName =  Param[2];
	SName = SName.substr(1);
	std::string CTime = Param[4];
	unsigned int intYY = 0;
	char *temp = new char[2];
	if (!Network->allocateServerNumeric(intYY))
		{
		elog << "msg_JU> Error while allocating server numeric!"
		     << endl;
		return false;
		}
	const char* temp2 = inttobase64(temp,intYY,2);
	
	if (Reason[0] == ':')
		{
		Reason = Reason.substr(1);
		};
	iServer* jupeServer = new (std::nothrow) iServer(
	base64toint(Param[0]),
	temp2,
	SName,
	atoi(CTime.c_str()),
	Reason
	);
	assert (jupeServer != 0);
	jupeServer->setJupe();
	if (!Network->addServer(jupeServer))
		{
		elog << "msg_JU> error while adding new server :(" << endl;
		return false;
		}
	theServer->PostEvent(EVT_NETJOIN, //TODO add EVT_JUPE
	static_cast< void* >(jupeServer),
	NULL);

	}
else
	{ // its a removal..
	std::string SName =  Param[2];
	SName = SName.substr(1);
	iServer* jupeServer = Network->findServerName(SName);
	if(!jupeServer)
		{
		elog << "msg_JU> Cant find server for removal" << endl;
		return false;
		}
	if(jupeServer->getCharYY() == theServer->getCharYY())
		{
		elog << "msg_JU> Let's not even try to remove ourself" << endl;
		return false;
		}
	// BUG: Nothing here?
	if(!Network->removeServer(jupeServer->getIntYY(),true))
		{}
	}	

// TODO
return false ;
}

} // namespace gnuworld
