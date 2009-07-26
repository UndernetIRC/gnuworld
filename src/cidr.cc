/**
 * cidr.cc
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
 *			Orlando Bassotto
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
 * $Id: cidr.cc,v 1.3 2009/07/26 18:30:37 mrbean_ Exp $
 */

#include	<string>
#include	<sstream>

#include	<cstdio>
#include	<cstdlib>
#include	<cstring>

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<arpa/inet.h>


#include	"cidr.h"
#include 	"misc.h"
#include	"StringTokenizer.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: cidr.cc,v 1.3 2009/07/26 18:30:37 mrbean_ Exp $" ) ;

namespace gnuworld
{

using std::string ;

xCIDR::xCIDR( const std::string& IP ) : str(IP), prefix(0)
{
	std::size_t pos = IP.find_first_of("/");
	if(pos == string::npos || pos == IP.length()) //Failed to find / or its at the end of the string
	{
		valid = false;
		return;
	}
	if(!(valid = ExtractMask(IP.substr(pos+1))))
	{
		return;
	}
	unsigned int pref;
	
	if(ExtractPrefix(IP.substr(0,pos),mask,pref) == 0)
	{
		valid = false;
		return;
	}
	prefix = xIP(ntohl(pref));
	valid = true;	
}

bool xCIDR::ExtractMask(const std::string& Mask)
{
	if(Mask.length() ==0 || Mask.length() > 2)  //Mask is maximum of 2 chars
	{
		return false;
	}
	if(!IsNumeric(Mask))
	{
		return false;
	}
	mask = atoi(Mask.c_str());
	return mask >=0 && mask <= 32; //Verify its in range
}

bool xCIDR::ExtractPrefix(const std::string& Prefix,const unsigned int& Mask,unsigned int& pref)
{
	StringTokenizer st(Prefix,'.');
	if(st.size() != 4) //Must have 4 parts
	{
		return false;
	}

	in_addr addr;
	
	if(inet_aton(Prefix.c_str(),&addr) > 0) 
	{
		unsigned int tAddr = htonl(addr.s_addr);
		int tMask = 0;
		for(unsigned int i=0; i < 32-Mask;i++) { //Build the mask
			tMask = tMask << 1;
			tMask = tMask + 1;
		}
		pref = tAddr & ~tMask; //extract the prefix
		return true;
	}

	return false;
}

bool xCIDR::matches(const std::string& IP)
{
	if(!valid)
	{
		return false;
	}
	unsigned int clientIp;
	if(ExtractPrefix(IP,mask,clientIp) > 0)
	{
		return clientIp == prefix.GetLongIP();
	}
	return false;
}
} // namespace gnuworld
