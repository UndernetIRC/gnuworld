/* ChannelUser.cc */

#include	<string>

#include	<cassert>

#include	"iClient.h"
#include	"ChannelUser.h"

const char ChannelUser_h_rcsId[] = __CHANNELUSER_H ;
const char ChannelUser_cc_rcsId[] = "$Id: ChannelUser.cc,v 1.5 2001/07/17 16:58:27 dan_karrels Exp $" ;
const char iClient_h_rcsId[] = __ICLIENT_H ;

namespace gnuworld
{

using std::string ;

const ChannelUser::modeType ChannelUser::MODE_O = 0x01 ;
const ChannelUser::modeType ChannelUser::MODE_V = 0x02 ;
const ChannelUser::modeType ChannelUser::ZOMBIE = 0x04 ;

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

const unsigned int& ChannelUser::getIP() const
{
return theClient->getIP() ;
}

const string ChannelUser::getCharYYXXX() const
{
return theClient->getCharYYXXX() ;
}

const unsigned int& ChannelUser::getIntYY() const
{
return theClient->getIntYY() ;
}

const unsigned int& ChannelUser::getIntXXX() const
{
return theClient->getIntXXX() ;
}

const unsigned int ChannelUser::getIntYYXXX() const
{
return theClient->getIntYYXXX() ;
}

bool ChannelUser::isOper() const
{
return theClient->isOper() ;
}

} //  namespace gnuworld
