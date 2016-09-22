/**
 * misc.cc
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
 * $Id: misc.cc,v 1.6 2007/12/27 20:45:15 kewlio Exp $
 */

#include	<string>
#include	<algorithm>

#include	<cctype>
#include	<cstdio>
#include	<cstdlib>
#include	<cstdarg>
#include	<cstring>

#include	<sstream>

#include	"misc.h"
#include	"StringTokenizer.h"
#include	"ELog.h"

const char rcsId[] = "$Id: misc.cc,v 1.6 2007/12/27 20:45:15 kewlio Exp $" ;

namespace gnuworld
{
/**
 * Create a string and copy into it (Key), but convert to all
 * lower case.
 */
string string_lower( const string& Key )
{
// This variable will be the string to be returned
// Make a copy of the original string to speed up
// allocation.
string retMe( Key ) ;

// Iterate through the new string, converting each character
// to lower case
for( string::size_type i = 0, end = retMe.size() ; i < end ; ++i )
	{
	retMe[ i ] = tolower( retMe[ i ] ) ;
	}

// Return by value the new string (this will create a copy of
// the new string)
return retMe ;
}

/**
 * Create a string and copy into it (Key), but convert to all
 * upper case.
 */
string string_upper( const string& Key )
{

// This variable will be the string to be returned
// Make a copy of the original string to speed up
// allocation.
string retMe( Key ) ;

// Iterate through the new string, converting each character
// to upper case
for( string::size_type i = 0, end = retMe.size() ; i < end ; i++ )
	{
	retMe[ i ] = toupper( retMe[ i ] ) ;
	}

// Return by value the new string (this will create a copy of
// the new string)
return retMe ;
}

/**
 * Convert the given string to all lowercase.  This method
 * mutates the original string, yet is much faster than the
 * equivalent method which receives the string by const
 * reference.
 */
void string_tolower( string& Key )
{
for( string::size_type i = 0, end = Key.size() ; i < end ; i++ )
	{
	Key[ i ] = tolower( Key[ i ] ) ;
	}
}

/**
 * Convert the given string to all uppercase.  This method
 * mutates the original string, yet is much faster than the
 * equivalent method which receives the string by const
 * reference.
 */
void string_toupper( string& Key )
{
for( string::size_type i = 0, end = Key.size() ; i < end ; i++ )
	{
	Key[ i ] = toupper( Key[ i ] ) ;
	}
}

/**
 * Return true if this string consists of all numerical
 * [0,9] characters.
 * Return false otherwise. */
bool IsNumeric( const string& s )
{
for( string::const_iterator ptr = s.begin(), endPtr = s.end() ;
	ptr != endPtr ; ++ptr )
	{
	if( !isdigit( *ptr ) )
		{
		return false ;
		}
	}
return true ;
}

/**
 * Return true if this string consists of a time specification
 * [0123456789SsMmHhDd] - it also must begin with a digit.
 */
bool IsTimeSpec( const string& s )
{
	char c;
	int count = 0;

	for ( string::const_iterator ptr = s.begin(), endPtr = s.end() ;
		ptr != endPtr ; ++ptr )
	{
		c = tolower(*ptr);
		/* if this is the start, it must be a digit */
		if ((ptr == s.begin()) && (!isdigit(c)))
			return false;
		/* check for valid characters (digits, smhd) */
		if (!isdigit(c) && c != 's' && c != 'm' && c != 'h' && c != 'd')
			return false;
		/* keep a count of non-numeric characters */
		if (!isdigit(c))
			count++;
		/* maximum of 1 is allowed */
		if (count > 1)
			return false;
	}
	/* if we reach here, this is a valid time specification */
	return true;
}

int strcasecmp( const string& s1, const string& s2 )
{
return ::strcasecmp( s1.c_str(), s2.c_str() ) ;
}

/**
 * Returns the time which is given as #<d/h/m/s> as seconds */

time_t extractTime( string Length, unsigned int defaultUnits )
{
unsigned int Units;

if (defaultUnits == 0)
	Units = 1;
else
	Units = defaultUnits;

if (!strcasecmp(Length.substr(Length.length()-1).c_str(),"d"))
	{
	Units = (24*3600);
	Length.resize(Length.length()-1);
	}
else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"h"))
        {
        Units = 3600;
        Length.resize(Length.length()-1);
        }
else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"m"))
        {
        Units = 60;
        Length.resize(Length.length()-1);
        }
else if(!strcasecmp(Length.substr(Length.length()-1).c_str(),"s"))
        {
        Units = 1;
        Length.resize(Length.length()-1);
        }

return atoi(Length.c_str()) * Units;
}

int atoi( const std::string& val )
{
return ::atoi( val.c_str() ) ;
}

string itoa(int n)
{
	string Result;
	std::ostringstream convert;
	convert << n;
	Result = convert.str();
	return Result;
}

string extractNick(const string& NickUserHostIP)
{
	StringTokenizer st( NickUserHostIP, '!' ) ;

	// Make sure there are exactly two tokens
	if( st.size() != 2 )
		return string("");

	return st[0];
}

string extractUser(const string& NickUserHostIP)
{
	string NickUser = extractNickUser(NickUserHostIP);

	StringTokenizer st( NickUser, '!' ) ;

	if( st.size() != 2 )
		return string("");

	return st[1];
}

string extractNickUser(const std::string& NickUserHostIP)
{
	StringTokenizer st( NickUserHostIP, '@' ) ;

	if( st.size() != 2 )
		return string("");

	return st[0];
}

string extractHostIP(const string& NickUserHostIP)
{
	StringTokenizer st( NickUserHostIP, '@' ) ;

	if( st.size() != 2 )
		return string("");

	return st[1];
}

bool validUserMask(const string& userMask)
{
	// Check that a '@' exists
	StringTokenizer st( userMask, '@' ) ;

	if (st.size() != 2)
	{
		return false ;
	}

	// Check that a '!' exists
	StringTokenizer st1( st[0], '!' ) ;
	if (st1.size() != 2)
	{
		return false ;
	}

	// Be sure that the hostname is no more than 255 characters
	if( st[ 1 ].size() > 255 )
	{
		return false ;
	}

	// Tests have passed
	return true ;
}

bool validCIDRLength(const string& address)
{
	string hostip = extractHostIP(address);
	irc_in_addr ip;
	unsigned char ipmask_len;
	if (!ipmask_parse(hostip.c_str(), &ip, &ipmask_len))
		return true;
	if ((ipmask_len > 0) && (ipmask_len < 32))
		return false;
	else
		return true;
}

/* If the address is only a hostip/cidr OR an empty string --> will return unchanged
 * If any wildcard or '!' or '@' found will complete to valid form
 */
string fixAddress(const string& address)
{
	if (address.empty())
		return address;

	bool checksafe = false;

	string fixaddr = address;

	if ((fixaddr[0] == '@') || (fixaddr[0] == '!'))
	{
		fixaddr = "*" + fixaddr;
		checksafe = true;
	}
	if (fixaddr[fixaddr.size()-1] == '@')
	{
		fixaddr = fixaddr + "*";
		checksafe = true;
	}

	StringTokenizer st( fixaddr, '@' ) ;
	if (st.size() < 2)
	{
		irc_in_addr ip;
		unsigned char ipmask_len;
		if (!ipmask_parse(address.c_str(), &ip, &ipmask_len))
			return address;
		else
			fixaddr = "*!*@" + address;
	}

	//If a CIDR is specified, help out and calculate the correct address
	string fixIpHost = extractHostIP(fixaddr);
	if (fixIpHost.find('/') != string::npos)
	{
		fixToCIDR64(fixIpHost);
		fixaddr = extractNickUser(fixaddr) + "@" + fixIpHost;
	}

	if (validUserMask(fixaddr))
	{
		if (((fixaddr == "*!*@*") || (fixaddr == "*!~*@*")) && (checksafe))
			return address;
		return fixaddr;
	}

	string nick = extractNick(st[0]);
	if (nick.empty())
	{
		if ((fixaddr[0] != '*') && (fixaddr[0] != '~'))
			fixaddr = "*!*" + fixaddr;
		else
			fixaddr = "*!" + fixaddr;
		checksafe = true;
	}
	if (((fixaddr == "*!*@*") || (fixaddr == "*!~*@*")) && (checksafe))
		return address;
	return fixaddr;
}

bool isUserHost(const string& address)
{
	std::size_t atpos = address.find('@');
	if (atpos == string::npos)
		return false;
	return true;
}

unsigned char fixToCIDR64(string& strIP)
{
	irc_in_addr ip;
	unsigned char ipmask_len;
	if (!ipmask_parse(strIP.c_str(), &ip, &ipmask_len))
		return 0;

	bool IsIPv4 = irc_in_addr_is_ipv4(&ip);
	string::size_type pos = strIP.find('/');
	strIP = strIP.substr(0, pos);

	// Re-enable this, if want to force to always /64
	//if ((ipmask_len > 64) && (!IsIPv4))
	//	ipmask_len = 64;

	//Instead of the above rule, if we have an ipv6 address, and no '/' present,
	//then the address is default truncated to /64
	if (((ipmask_len > 64) && (!IsIPv4)) && (pos == string::npos))
		ipmask_len = 64;

	if (((ipmask_len >= 16) && (!IsIPv4)) || ((ipmask_len >= 96) && (IsIPv4)))
	{
		strIP = IPCIDRMinIP(strIP.c_str(), unsigned(ipmask_len));
		if (IsIPv4)
		{ //adjust to 32 bit, if we have a IPv4/32 we don't want to show /32
			if (ipmask_len < 128)
				strIP += '/' + itoa(unsigned(ipmask_len) - 96);
		}
		else
		{ //also, if ipv6/128, don't show the /128
			if (ipmask_len < 128)
				strIP += '/' + itoa(unsigned(ipmask_len));
		}
	}
	return ipmask_len;
}

string fixToCIDR64(const string& strIP)
{
	string fixIP = strIP;
	fixToCIDR64(fixIP);
	return fixIP;
}

string createBanMask(const string& address)
{
	if (!isUserHost(address))
		return address;

	string nick = extractNick(address);
	string ident = extractUser(address);
	string hostip = extractHostIP(address);

	if (!nick.empty()) nick = "*!";

	if ('~' == ident[0])
		ident = "~*";

	if (hostip.find(':') != string::npos)
		hostip = createClass(hostip);

	return nick + ident +'@'+ hostip;
}

string createClass(const string& address, bool wildcard)
{
	string fixaddr;
	bool isUserAddr = isUserHost(address);
	if (isUserAddr)
		fixaddr = extractHostIP(address);
	else
		fixaddr = address;
	unsigned char ipmask_len;
	irc_in_addr ip;
	if (ipmask_parse(fixaddr.c_str(), &ip, &ipmask_len))
	{
		if (irc_in_addr_is_ipv4(&ip))
		{
			if (wildcard)
			{
				StringTokenizer st24(fixaddr, '.');
				fixaddr  = st24[0] + '.';
				fixaddr += st24[1] + '.';
				fixaddr += st24[2] + ".*";
			}
			else
				fixaddr += "/24";
			if (isUserAddr)
				fixaddr = extractNickUser(address) + '@' + fixaddr;
			return fixaddr;
		}
		else
		{
			fixaddr = fixToCIDR64(fixaddr.c_str());
			if (isUserAddr)
				fixaddr = extractNickUser(address) + '@' + fixaddr;
			return fixaddr;
		}
	}
	else //host address case
	{
		StringTokenizer st(fixaddr, '.');
		if (st.size() < 2)
			return address;
		fixaddr = "*." + st.assemble(1);
		if (isUserAddr)
			fixaddr = extractNickUser(address) + '@' + fixaddr;
		return fixaddr;
	}
}

const string prettyDuration( int duration )
{
	if (duration == 0)
		return "Never";

// Pretty format a 'duration' in seconds to
// x day(s), xx:xx:xx.

char tmpBuf[ 64 ] = {0};

int	res = ::time(NULL) - duration,
	secs = res % 60,
	mins = (res / 60) % 60,
	hours = (res / 3600) % 24,
	days = (res / 86400) ;

sprintf(tmpBuf, "%i day%s, %02d:%02d:%02d",
	days,
	(days == 1 ? "" : "s"),
	hours,
	mins,
	secs );

return string( tmpBuf ) ;
}

const string TokenStringsParams(const char* format,...)
{
	char buf[ 1024 ] = { 0 } ;
	va_list _list ;

	va_start( _list, format ) ;
	vsnprintf( buf, 1024, format, _list ) ;
	va_end( _list ) ;
	return string(buf);
}

} // namespace gnuworld
