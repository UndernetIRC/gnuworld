/**
 * msg_P.cc
 */

#include	<string>

#include	"server.h"
#include	"Network.h"
#include	"iClient.h"
#include	"client.h"
#include	"ELog.h"

const char msg_P_cc_rcsId[] = "$Id: msg_P.cc,v 1.2 2001/03/04 11:41:36 isomer Exp $" ;

using std::string ;
using std::endl ;

namespace gnuworld
{

/**
 * A nick has sent a private message
 * QBg P PAA :help
 * QBg: Source nickname's numeric
 * P: PRIVMSG
 * PAA: Destination nickname's numeric
 * :help: Message
 *
 * QAE P PAA :translate xaa
 * QAE P AAPAA :translate xaa
 */
int xServer::MSG_P( xParameters& Param )
{

if( Param.size() < 3 )
	{
	elog	<< "xServer::MSG_P> Invalid number of arguments\n" ;
	return -1 ;
	}

char* Sender	= Param[ 0 ] ;
char* Receiver	= Param[ 1 ] ;

// Is the PRIVMSG being sent to a channel?
if( ('#' == *Receiver) || ('+' == *Receiver))
	{
	// It's a channel message, just ignore it
	return 0 ;
	}

char		*Server		= NULL,
		*Pos		= NULL,
		*Command	= NULL ;

bool		CTCP		= false ;
bool		secure		= false ;

xClient		*Client		= NULL ;

// Search for user@host in the receiver string
Pos = strchr( Receiver, '@' ) ;

// Was there a '@' in the Receiver string?
if( NULL != Pos )
	{
	// Yup, nickname specified
	Server = Receiver + (Pos - Receiver) + 1 ;
	Receiver[ Pos - Receiver ] = 0 ;
	Client = Network->findLocalNick( Receiver ) ;
	secure = true ;
	}
else if( Receiver[ 0 ] == charYY[ 0 ]
	&& Receiver[ 1 ] == charYY[ 1 ] )
	{
	// It's mine
	Client = Network->findLocalClient( Receiver ) ;
	}
else
	{
	return -1 ;
	}

char* Message = Param[ 2 ] ;

// Is it a CTCP message?
if( Message[ 0 ] == 1 && Message[ strlen( Message ) - 1 ] == 1 )
	{
	Message++ ;
	CTCP = true ;
	Message[ strlen( Message ) - 1 ] = 0 ;

	// TODO: Get rid of this hideous method call
	// strtok() is a pos
	Command = strtok( Message, " " ) ;
	char* Msg = strtok( NULL, "\r" ) ; 
	Message = Msg ;

	// Message == 0 will cause std::string() constructor to crash.
	if( NULL == Message )
		{
		Message = "" ;
		}
	// Same reason as above. DOH!
	if( NULL == Command )
		{
		Command = "" ;
		}
	}

// :Sender PRIVMSG YXX :Message
// :Sender PRIVMSG YXX :\001Command\001
// :Sender PRIVMSG YXX :\001\Command\001 Message

if( NULL == Client )
	{
	elog	<< "xServer::MSG_P: Local client not found: "
		<< Receiver << endl ;
	return -1 ;
	}

iClient* Target = Network->findClient( Sender ) ;
if( NULL == Target )
	{
	elog	<< "xServer::MSG_P> Unable to find Sender: "
		<< Sender << endl ;
	return -1 ;
	}

if( CTCP )
	{
	return Client->OnCTCP( Target, Command, Message, secure ) ;
	}
else
	{
	return Client->OnPrivateMessage( Target, Message, secure ) ;
	}
}

} // namespace gnuworld
