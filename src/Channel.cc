/* Channel.cc */

#include	<string>
#include	<iostream>
#include	<strstream>

#include	"Channel.h"
#include	"iClient.h"
#include	"ChannelUser.h"
#include	"Network.h"
#include	"xparameters.h"
#include	"StringTokenizer.h"
#include	"ELog.h"

using std::string ;
using std::endl ;
using std::strstream ;

namespace gnuworld
{

Channel::Channel( const string& _name,
	const time_t& _creationTime )
 : name( _name ),
   creationTime( _creationTime ),
   modes( 0 ),
   limit( 0 )
{}

Channel::~Channel()
{
// Deallocate all ChannelUser's that are left
userListType::iterator currentPtr = userList.begin() ;
userListType::iterator endPtr = userList.end() ;
for( ; currentPtr != endPtr ; ++currentPtr )
	{
	delete currentPtr->second ;
	}
userList.clear() ;
}

bool Channel::addUser( ChannelUser* newUser )
{
if( !userList.insert(
	userListType::value_type( newUser->getIntYYXXX(), newUser ) ).second )
	{
//	elog	<< "Channel::addUser> (" << getName() << "): "
//		<< "Unable to add user: " << *newUser << endl ;
	return false ;
	}

//elog	<< "Channel::addUser> " << name << " added user: "
//	<< *newUser << endl ;

return true ;
}

ChannelUser* Channel::removeUser( iClient* theClient )
{
return removeUser( theClient->getIntYYXXX() ) ;
}

ChannelUser* Channel::removeUser( const unsigned int& intYYXXX )
{
userListType::iterator ptr = userList.find( intYYXXX ) ;
if( ptr != userList.end() )
	{
	userList.erase( ptr ) ;
	return ptr->second ;
	}

//elog		<< "Channel::removeUser> (" << getName() << ") "
//		<< "Unable to find user: " << intYYXXX << std::endl ;

return 0 ;
}

ChannelUser* Channel::findUser( iClient* theClient ) const
{
return userList.find( theClient->getIntYYXXX() )->second ;
}

bool Channel::removeUserMode( const ChannelUser::modeType& whichMode,
	iClient* theClient )
{
ChannelUser* theChanUser = findUser( theClient ) ;
if( NULL == theChanUser )
	{
//	elog	<< "Channel::removeUserMode> (" << getName() << ") "
//		<< "Unable to find user\n" ;
	return false ;
	}
theChanUser->removeMode( whichMode ) ;
return true ;
}

bool Channel::setUserMode( const ChannelUser::modeType& whichMode,
	iClient* theClient )
{
ChannelUser* theChanUser = findUser( theClient ) ;
if( NULL == theChanUser )
	{
//	elog	<< "Channel::setUserMode> (" << getName() << ") "
//		<< "Unable to find user\n" ;
	return false ;
	}
theChanUser->setMode( whichMode ) ;
return true ;
}

bool Channel::getUserMode( const ChannelUser::modeType& whichMode,
	iClient* theClient ) const
{
ChannelUser* theChanUser = findUser( theClient ) ;
if( NULL == theChanUser )
	{
//	elog	<< "Channel::getUserMode> (" << getName() << ") "
//		<< "Unable to find user\n" ;
	return false ;
	}
return theChanUser->getMode( whichMode ) ;
}

void Channel::setBan( const string& banMask )
{
banList.push_back( banMask ) ;
}

void Channel::removeBan( const string& banMask )
{
for( banListType::iterator ptr = banList.begin(), end = banList.end() ;
	ptr != end ; ++ptr )
	{
	// TODO: Case insensitive search?
	if( *ptr == banMask )
		{
		banList.erase( ptr ) ;
		return ;
		}
	}
}

bool Channel::findBan( const string& banMask ) const
{
return std::find( banList.begin(), banList.end(), banMask )
	!= banList.end() ;
}

bool Channel::matchBan( const string& banMask ) const
{
// TODO
return findBan( banMask ) ;
}

// ABCDE M #channel <modes>
void Channel::OnModeChange( iClient*, const xParameters& Param )
{
if( Param.size() < 3 )
	{
	elog	<< "Channel::OnModeChange> Invalid number of arguments\n" ;
	return ;
	}

bool plus = true ;
xParameters::size_type argPos = 3 ;

iClient* theClient = 0 ;
ChannelUser* theUser = 0 ;

for( const char* modePtr = Param[ 2 ] ; *modePtr ; ++modePtr )
	{
	switch( *modePtr )
		{
		case '+':
			plus = true ;
			break ;
		case '-':
			plus = false ;
			break ;
		case 'i':
			if( plus )	setMode( MODE_I ) ;
			else		removeMode( MODE_I ) ;
			break ;
		case 't':
			if( plus )	setMode( MODE_T ) ;
			else		removeMode( MODE_T ) ;
			break ;
		case 'n':
			if( plus )	setMode( MODE_N ) ;
			else		removeMode( MODE_N ) ;
			break ;
		case 's':
			if( plus )	setMode( MODE_S ) ;
			else		removeMode( MODE_S ) ;
			break ;
		case 'p':
			if( plus )	setMode( MODE_P ) ;
			else		removeMode( MODE_P ) ;
			break ;
		case 'm':
			if( plus )	setMode( MODE_M ) ;
			else		removeMode( MODE_M ) ;
			break ;
		case 'b':
			if( argPos >= Param.size() )
				{
				elog	<< "Channel::OnModeChange> Missing "
					<< "argument (mode b)\n" ;
				break ;
				}
			if( plus )	setBan( Param[ argPos ] ) ;
			else		removeBan( Param[ argPos ] ) ;
			argPos++ ;
			break ;
		case 'l':
			if( plus )
				{
				setLimit( atoi( Param[ argPos ] ) ) ;
				// argument only needed for +l
				argPos++ ;
				}
			else
				{
				removeMode( MODE_L ) ;
				// no arg needed for -l
				}
			break ;
		case 'k':
			if( argPos >= Param.size() )
				{
				elog	<< "Channel::OnModeChange> Missing "
					<< "argument (mode k)\n" ;
				break ;
				}
			if( plus )
				{
				setKey( Param[ argPos ] ) ;
				}
			else
				{
				removeMode( MODE_K ) ;
				}
			argPos++ ;
			break ;
		case 'v':
			if( argPos >= Param.size() )
				{
				elog	<< "Channel::OnModeChange> Missing "
					<< "argument (mode v)\n" ;
				break ;
				}
			theClient = Network->findClient( Param[ argPos ] ) ;
			if( NULL == theClient )
				{
				elog	<< "Channel::OnModeChange> Unable to find "
					<< "client: " << Param[ argPos ]
					<< endl ;
				break ;
				}
			theUser = findUser( theClient ) ;
			if( NULL == theUser )
				{
				elog	<< "Channel::OnModeChange> (mode v) Unable "
					<< "to find ChannelUser\n" ;
				break ;
				}
			if( plus )	theUser->setMode( ChannelUser::MODE_V ) ;
			else		theUser->removeMode( ChannelUser::MODE_V ) ;

			argPos++ ;
			break ;
		case 'o':
			if( argPos >= Param.size() )
				{
				elog	<< "Channel::OnModeChange> Missing "
					<< "argument (mode o)\n" ;
				break ;
				}
			theClient = Network->findClient( Param[ argPos ] ) ;
			if( NULL == theClient )
				{
				elog	<< "Channel:OnModeChange> Unable to find "
					<< "client: " << Param[ argPos ]
					<< endl ;
				break ;
				}
			theUser = findUser( theClient ) ;
			if( NULL == theUser )
				{
				elog	<< "Channel::OnModeChange> (mode o) Unable "
					<< "to find ChannelUser\n" ;
				break ;
				}
			if( plus )	theUser->setMode( ChannelUser::MODE_O ) ;
			else		theUser->removeMode( ChannelUser::MODE_O ) ;

			argPos++ ;
			break ;

		default:
			elog	<< "Channel::OnModeChange> Found unexpected mode: "
				<< *modePtr << endl ;
			break ;

		} // switch()

	} // for()

}

// NOTE: No error checking is done in this method, the mode/arg strings
// are assumed to be in proper order, and all clients are assumed to
// exist.
void Channel::OnModeChange( const string yyxxx,
	const string& modes, const string& argString )
{

StringTokenizer args( argString ) ;

bool plus = true ;
StringTokenizer::size_type	argPos = 0 ;

iClient* theClient = 0 ;
ChannelUser* theUser = 0 ;

for( string::size_type modePos = 0 ; modePos < modes.size() ; modePos++ )
	{
	switch( modes[ modePos ] )
		{
		case '+':
			plus = true ;
			break ;
		case '-':
			plus = false ;
			break ;
		case 'i':
			if( plus )		setMode( MODE_I ) ;
			else			removeMode( MODE_I ) ;
			break ;
		case 'n':
			if( plus )		setMode( MODE_N ) ;
			else			removeMode( MODE_N ) ;
			break ;
		case 'm':
			if( plus )		setMode( MODE_M ) ;
			else			removeMode( MODE_M ) ;
			break ;
		case 's':
			if( plus )		setMode( MODE_S ) ;
			else			removeMode( MODE_S ) ;
			break ;
		case 'p':
			if( plus )		setMode( MODE_P ) ;
			else			removeMode( MODE_P ) ;
			break ;
		case 't':
			if( plus )		setMode( MODE_T ) ;
			else			removeMode( MODE_T ) ;
			break ;
		case 'l':
			if( !plus )
				{
				removeMode( MODE_L ) ;
				break ;
				}

			setLimit( atoi( args[ argPos++ ].c_str() ) ) ;
			break ;
		case 'k':
			if( plus )		setKey( args[ argPos ] ) ;
			else			removeMode( MODE_K ) ;
			argPos++ ;
			break ;
		case 'b':
			if( plus )		setBan( args[ argPos ] ) ;
			else			removeBan( args[ argPos ] ) ;
			argPos++ ;
			break ;
		case 'o':
			theClient = Network->findClient( args[ argPos ] ) ;
			argPos++ ;
			theUser = findUser( theClient ) ;
			if( NULL == theUser )
				{
				elog	<< "Channel::OnModeChange (o)> Unable "
					<< "to find client: " << args[ argPos - 1 ]
					<< endl ;
				continue ;
				}
			if( plus )	theUser->setMode( ChannelUser::MODE_O ) ;
			else		theUser->removeMode( ChannelUser::MODE_O ) ;
			break ;
		case 'v':
			theClient = Network->findClient( args[ argPos ] ) ;
			argPos++ ;
			theUser = findUser( theClient ) ;
			if( NULL == theUser )
				{
				elog	<< "Channel::OnModeChange(v)> Unable "
					<< "to find client: " << args[ argPos - 1 ]
					<< endl ;
				continue ;
				}
			if( plus )	theUser->setMode( ChannelUser::MODE_V ) ;
			else		theUser->removeMode( ChannelUser::MODE_V ) ;
			break ;
		default:
			elog	<< "Channel::OnModeChange> Found unexpected mode: "
				<< modes[ modePos ] << endl ;
		} // switch()
	} // for()

}

const string Channel::getModeString() const
{

string modeString = "+" ;
string argString = "" ;

if( modes & MODE_T )	modeString += 't' ;
if( modes & MODE_N )	modeString += 'n' ;
if( modes & MODE_S )	modeString += 's' ;
if( modes & MODE_P )	modeString += 'p' ;
if( modes & MODE_M )	modeString += 'm' ;
if( modes & MODE_I )	modeString += 'i' ;

if( modes & MODE_K )
	{
	modeString += 'k' ;
	argString += getKey() + ' ' ;
	}

if( modes & MODE_L )
	{
	modeString += 'l' ;

	// Can't put numerical variables into a string
	strstream s ;
	s << getLimit() << ends ;

	argString += s.str() ;

	delete[] s.str() ;
	}

return (modeString + ' ' + argString) ;

}

} // namespace gnuworld
