/* iClient.cc
 */

#include	<string>
#include	<iostream>
#include	<ctime>
#include	<map>

#include	<cassert>

#include	"iClient.h"
#include	"client.h"
#include	"Numeric.h"
#include	"ip.h"

const char iClient_h_rcsId[] = __ICLIENT_H ;
const char iClient_cc_rcsId[] = "$Id: iClient.cc,v 1.6 2000/12/23 15:01:58 dan_karrels Exp $" ;

using std::string ;
using std::map ;

namespace gnuworld
{

iClient::iClient( const unsigned int& _uplink,
	const string& _yxx,
	const string& _nickName,
	const string& _userName,
	const string& _hostBase64,
	const string& _insecureHost,
	const string& _mode,
	const string& _description,
	const time_t& _connectTime )
: intYY( _uplink ),
	nickName( _nickName ),
	userName( _userName ),
	IP( xIP( _hostBase64, true ).GetLongIP() ),
	insecureHost( _insecureHost ),
#ifdef CLIENT_DESC
	description( _description),
#endif
	connectTime( _connectTime )
{
if( 5 == _yxx.size() )
	{
	// n2k, yyxxx
	intXXX = base64toint( _yxx.c_str() + 2, 3 ) ;
	intYYXXX = base64toint( _yxx.c_str(), 5 ) ;

	charYY[ 0 ] = _yxx[ 0 ] ;
	charYY[ 1 ] = _yxx[ 1 ] ;
	charYY[ 2 ] = 0 ;
	charXXX[ 0 ] = _yxx[ 2 ] ;
	charXXX[ 1 ] = _yxx[ 3 ] ;
	charXXX[ 2 ] = _yxx[ 4 ] ;
	charXXX[ 3 ] = 0 ;
	}
else
	{
	// yxx
	intXXX = base64toint( _yxx.c_str() + 1, 2 ) ;
	intYYXXX = base64toint( _yxx.c_str(), 3 ) ;

	charYY[ 0 ] = _yxx[ 0 ] ;
	charYY[ 1 ] = 0 ;
	charXXX[ 0 ] = _yxx[ 1 ] ;
	charXXX[ 1 ] = _yxx[ 2 ] ;
	charXXX[ 2 ] = 0 ;
	}

mode = 0 ;
setModes( _mode ) ;
}

iClient::~iClient()
{
for( customDataMapType::iterator ptr = customDataMap.begin() ;
	ptr != customDataMap.end() ; ++ptr )
	{
	ptr->first->deleteCustomData( this, ptr->second ) ;
	}
}

void iClient::setModes( const string& newModes )
{
for( string::size_type i = 0 ; i < newModes.size() ; i++ )
	{
	switch( newModes[ i ] )
		{
		case 'o':
		case 'O':
			mode |= MODE_OPER ;
			break ;
		case 'i':
		case 'I':
			mode |= MODE_INVISIBLE ;
			break ;
		case 'w':
		case 'W':
			mode |= MODE_WALLOPS ;
			break ;
		case 'k':
		case 'K':
			mode |= MODE_SERVICES ;
			break ;
		case 'd':
		case 'D':
			mode |= MODE_DEAF ;
			break ;
		default:
			// Unknown mode
			break ;
		} // switch
	} // for
} // setModes()

void iClient::removeChannel( Channel* theChan )
{
for( channelListType::iterator ptr = channelList.begin() ; ptr != channelList.end() ;
	++ptr )
	{
	if( *ptr == theChan )
		{
		channelList.erase( ptr ) ;
		break ;
		}
	}
}

const string iClient::getCharModes() const
{
string retMe = "+" ;

if( mode & MODE_OPER )		retMe += 'o' ;
if( mode & MODE_WALLOPS )	retMe += 'w' ;
if( mode & MODE_INVISIBLE )	retMe += 'i' ;
if( mode & MODE_DEAF )		retMe += 'd' ;
if( mode & MODE_SERVICES )	retMe += 'k' ;

return retMe ;
}

bool iClient::setCustomData( xClient* theClient, void* data )
{
#ifndef NDEBUG
  assert( theClient != 0 ) ;
#endif

if( customDataMap.find( theClient ) != customDataMap.end() )
	{
	return false ;
	}

return customDataMap.insert( customDataMapType::value_type(
	theClient, data ) ).second ;
}

void* iClient::getCustomData( xClient* theClient ) const
{
#ifndef NDEBUG
  assert( theClient != 0 ) ;
#endif

customDataMapType::const_iterator ptr = customDataMap.find( theClient ) ;
if( ptr == customDataMap.end() )
	{
	return 0 ;
	}
return ptr->second ;
}

void* iClient::removeCustomData( xClient* theClient )
{
#ifndef NDEBUG
  assert( theClient != 0 ) ;
#endif

customDataMapType::iterator ptr = customDataMap.find( theClient ) ;
if( ptr == customDataMap.end() )
	{
	return 0 ;
	}
customDataMap.erase( ptr ) ;
return ptr->second ;
}

} // namespace gnuworld
