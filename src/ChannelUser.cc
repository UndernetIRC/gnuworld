/* ChannelUser.cc */

#include	<string>

#include	"iClient.h"
#include	"ChannelUser.h"

using std::string ;

namespace gnuworld
{

ChannelUser::ChannelUser( iClient* _theClient )
 : theClient( _theClient ),
   modes( 0 )
{}

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
