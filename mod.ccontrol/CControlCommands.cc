/* CControlCommands.cc
 */

#include	<string>
#include	<cstdlib>

//#include	<netinet/in.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"misc.h"
#include	"Gline.h"
#include	"ip.h"
#include	"ELog.h"

namespace ccontrolns
{

using std::string ;
using namespace gnuworld ;

void Command::Usage( iClient* theClient )
{
bot->Notice( theClient, string( "Usage: " ) + ' ' + getInfo() ) ;
}

// help [command]
bool HELPCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

string banner = "--- Help Menu for " ;
banner += bot->getNickName() + " ---" ;

bot->Notice( theClient, banner ) ;

if( 1 == st.size() )
	{
	// Spit out all commands
	for( ccontrol::constCommandIterator ptr = bot->command_begin() ;
		ptr != bot->command_end() ; ++ptr )
		{
		bot->Notice( theClient, ptr->second->getName() ) ;
		}
	}
else
	{
	ccontrol::constCommandIterator ptr =
		bot->findCommand( string_upper( st[ 1 ] ) ) ;
	if( ptr == bot->command_end() )
		{
		bot->Notice( theClient, "Command not found" ) ;
		}
	else
		{
		bot->Notice( theClient, ptr->second->getInfo() ) ;
		}
	}

return true ;
}

bool CHANINFOCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

Channel* theChan = Network->findChannel( st[ 1 ] ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel %s",
		st[ 1 ].c_str() ) ;
	return true ;
	}

bot->Notice( theClient, "Channel %s is mode %s",
	st[ 1 ].c_str(),
	theChan->getModeString().c_str() ) ;
bot->Notice( theClient, "Created at time: %d",
	theChan->getCreationTime() ) ;
bot->Notice( theClient, "Number of channel users: %d",
	theChan->size() ) ;

return true ;
}

// addoperchan #channel
bool ADDOPERCHANCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

string chanName = st[ 1 ] ;
if( '#' != chanName[ 0 ] )
	{
	bot->Notice( theClient, "Invalid channel name" ) ;
	return true ;
	}

if( bot->addOperChan( chanName ) )
	{
	bot->Notice( theClient, "Addition of %s as oper chan SUCCEEDED",
		chanName.c_str() ) ;
	}
else
	{
	bot->Notice( theClient, "Addition of %s as oper chan FAILED",
		chanName.c_str() ) ;
	}
return true ;
}

// remoperchan #channel
bool REMOPERCHANCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

string chanName = st[ 1 ] ;
if( '#' != chanName[ 0 ] )
	{
	bot->Notice( theClient, "Invalid channel name" ) ;
	return true ;
	}

if( bot->removeOperChan( chanName ) )
	{
	bot->Notice( theClient, "Removal of %s as oper chan SUCCEEDED",
		chanName.c_str() ) ;
	}
else
	{
	bot->Notice( theClient, "Removal of %s as oper chan FAILED",
		chanName.c_str() ) ;
	}
return true ;
}

// listoperchans
bool LISTOPERCHANSCommand::Exec( iClient* theClient, const string& Message )
{

bot->Notice( theClient, "There are currently %d IRCoperator only channels",
	bot->operChan_size() ) ;

if( bot->operChan_empty() )
	{
	return true ;
	}

string chanList = "" ;
ccontrol::const_operChanIterator ptr = bot->operChan_begin() ;

while( ptr != bot->operChan_end() )
	{
	if( !chanList.empty() )
		{
		chanList += ", " ;
		chanList += *ptr ;
		}
	else
		{
		chanList = *ptr ;
		}
	++ptr ;
	}

bot->Notice( theClient, chanList ) ;
return true ;
}

// kick #channel nick reason
bool KICKCommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 4 )
	{
	Usage( theClient ) ;
	return true ;
	}

string chanName = st[ 1 ] ;
if( chanName[ 0 ] != '#' )
	{
	chanName.insert( chanName.begin(), '#' ) ;
	}

Channel* theChan = Network->findChannel( chanName ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel: %s",
		chanName.c_str() ) ;
	return true ;
	}

iClient* Target = Network->findNick( st[ 2 ] ) ;
if( NULL == Target )
	{
	bot->Notice( theClient, "Unable to find nick: %s",
		st[ 2 ].c_str() ) ;
	return true ;
	}

bot->Notice( theClient, "Kicking %s from channel %s because %s",
	Target->getNickName().c_str(),
	chanName.c_str(),
	st.assemble( 3 ).c_str() ) ;

bot->Kick( theChan, Target, st.assemble( 3 ) ) ;
return true ;
}

// whois nickname
bool WHOISCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

iClient* Target = Network->findNick( st[ 1 ] ) ;
if( NULL == Target )
	{
	bot->Notice( theClient, "Unable to find nick: %s", st[ 1 ].c_str() ) ;
	return true ;
	}

iServer* targetServer = Network->findServer( Target->getIntYY() ) ;
if( NULL == targetServer )
	{
	elog	<< "WHOISCommand> Unable to find server: "
		<< Target->getIntYY() << endl ;
	return false ;
	}

bot->Notice( theClient, "%s is %s!%s@%s [%s]",
	st[ 1 ].c_str(),
	Target->getNickName().c_str(),
	Target->getUserName().c_str(),
	Target->getInsecureHost().c_str(),
	xIP( htonl( Target->getIP() ) ).GetNumericIP()
	) ;

bot->Notice( theClient, "Numeric: %s, UserModes: %s, Server Numeric: %s (%s)",
	Target->getCharYYXXX().c_str(),
	Target->getCharModes().c_str(),
	targetServer->getCharYY(),
	targetServer->getName().c_str()
	) ;

if( Target->isOper() )
	{
	bot->Notice( theClient, "%s is an IRCoperator",
		st[ 1 ].c_str() ) ;
	}

vector< string > channels ;
for( iClient::const_channelIterator ptr = Target->channels_begin() ;
	ptr != Target->channels_end() ; ++ptr )
	{
	channels.push_back( (*ptr)->getName() ) ;
	}

if( channels.empty() )
	{
	return true ;
	}

string chanNames ;
for( vector< string >::size_type i = 0 ; i < channels.size() ; i++ )
	{
	chanNames += channels[ i ] ;
	if( (i + 1) < channels.size() )
		{
		chanNames += ", " ;
		}
	}

bot->Notice( theClient, "On channels: %s",
	chanNames.c_str() ) ;

return true ;
}

// translate yyxxx
bool TRANSLATECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() != 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

iClient* Target = Network->findClient( st[ 1 ] ) ;
if( NULL == Target )
	{
	bot->Notice( theClient, "Unable to find numeric %s\n",
		st[ 1 ].c_str() ) ;
	return true ;
	}

iServer* theServer = Network->findServer( Target->getIntYY() ) ;
if( NULL == theServer )
	{
	elog	<< "TRANSLATECommand> Unable to find server\n" ;
	return false ;
	}

bot->Notice( theClient, "%s is %s!%s@%s on server %s",
	st[ 1 ].c_str(),
	Target->getNickName().c_str(),
	Target->getUserName().c_str(),
	Target->getInsecureHost().c_str(),
	theServer->getName().c_str() ) ;

return true ;

}

// LDd P AIAAA :mode #krushnet -o DawgSleep
// mode channel modes
bool MODECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage( theClient ) ;
	return true ;
	}

Channel* theChan = Network->findChannel( st[ 1 ] ) ;
if( NULL == theChan )
	{
	bot->Notice( theClient, "Unable to find channel %s\n",
		st[ 1 ].c_str() ) ;
	return true ;
	}

// Define mode to be any mode, such as +abc-def
// Define argument to to be any argument to a particular mode: -o nickArgument

// index into the st object, location of next mode
StringTokenizer::size_type modePos = 2 ;

// index into the st object, location of next argument
StringTokenizer::size_type argPos = 3 ;

// To be used later
iClient* Target = 0 ;

// Store the command to be sent to the network in two strings
// modeString holds the updated modes
string modeString = "" ;

// argString holds the updated arguments, delimited by space (' ')
string argString = "" ;

// Continue while there are more modes to parse
while( modePos < st.size() )
	{

	// modePos could be equal to argPos if a previous iteration of
	// the while loop found one or more modes with arguments
	if( modePos == argPos )
		{
		argPos++ ;
		}

	// This it the size of the increment to make to modePos.
	// This could be greater than 1 if there are more than one mode
	// arguments at st[ modePos ] that require arguments
	string::size_type modePosIncrement = 1 ;

	// Keep track of the polarity of the mode change.
	bool plus = true ;

	// Iterate through the characters of the mode at st[ modePos ]
	for( string::size_type charPos = 0 ; charPos < st[ modePos ].size() ; ++charPos )
		{
		switch( st[ modePos ][ charPos ] )
			{
			case 'o':
			case 'v':

				// Make sure there is an argument for this mode
				if( argPos >= st.size() )
					{
					Usage( theClient ) ;
					return true ;
					}

				// Add the mode to the modeString
				modeString += st[ modePos ][ charPos ] ;

				// arg needs to be a nickname
				Target = Network->findNick( st[ argPos ] ) ;

				// Is the argument a valid nickname?
				if( NULL == Target )
					{
					bot->Notice( theClient, "Unable to find %s\n",
						st[ argPos ].c_str() ) ;
					return true ;
					}

				// Make sure the user is in this particular channel
				if( NULL == theChan->findUser( Target ) )
					{
					bot->Notice( theClient, "User %s was not found "
						"on channel %s",
						st[ argPos ].c_str(),
						theChan->getName().c_str() ) ;
					return true ;
					}

				// Add this nick's numeric (plus a space) to the end
				// of the current argument string
				argString += Target->getCharYYXXX() + ' ' ;

				// Move to next argument
				argPos++ ;

				// Make sure modePos skips over this argument
				modePosIncrement++ ;

				break ;
			case 'b':
				// Each of these modes needs an argument
				if( argPos >= st.size() )
					{
					Usage( theClient ) ;
					return true ;
					}

				// Add this mode to the current modeString
				modeString += st[ modePos ][ charPos ] ;

				// Add this argument to the current argument string
				argString += st[ argPos ] + ' ' ;

				// Mode to next argument
				argPos++ ;

				// Make sure modePos skips over this argument
				modePosIncrement++ ;

				break ;

			case 'k':
				// Each of these modes needs an argument
				if( argPos >= st.size() )
					{
					Usage( theClient ) ;
					return true ;
					}

				// Add this mode to the current modeString
				modeString += st[ modePos ][ charPos ] ;

				// Add this argument to the current argument string
				argString += st[ argPos ] + ' ' ;

				// Mode to next argument
				argPos++ ;

				// Make sure modePos skips over this argument
				modePosIncrement++ ;

				break ;
			case 'l':

				// Mode -l requires no argument
				if( !plus )
					{
					// No args needed
					modeString += st[ modePos ][ charPos ] ;
					break ;
					}

				// Else, the user has specified +l, need an
				// argument.

				// Each of these modes needs an argument
				if( argPos >= st.size() )
					{
					Usage( theClient ) ;
					return true ;
					}

				// Add this mode to the current modeString
				modeString += st[ modePos ][ charPos ] ;

				// Add this argument to the current argument string
				argString += st[ argPos ] + ' ' ;

				// Mode to next argument
				argPos++ ;

				// Make sure modePos skips over this argument
				modePosIncrement++ ;

				break ;
			case '+':
				if( plus )
					{
					// Already plus
					break ;
					}
				plus = true ;
				modeString += st[ modePos ][ charPos ] ;
				break ;
			case '-':
				if( !plus )
					{
					// Already minus
					break ;
					}
				plus = false ;
				modeString += st[ modePos ][ charPos ] ;
				break ;
			default:
				// Requires no arguments, just add the mode
				// to the modeString
				modeString += st[ modePos ][ charPos ] ;
				break ;

			} // switch()
		} // for()

	modePos += modePosIncrement ;
	} // while( modePos < st.size() )

bot->ModeAsServer( theChan, modeString + ' ' + argString ) ;

// Update internal tables.
// This is a cheat, but it makes things so much easier :)
// TODO
//theChan->OnModeChange( theClient->getCharYYXXX(), modeString, argString
//) ;

return true ;

}

// Input: gline *@blah.net reason
// Input: gline 3600 *@blah.net reason
//
// Output: C GL * +*@lamer.net 3600 :Banned (*@lamer) ...
//
bool GLINECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage( theClient ) ;
	return true ;
	}

StringTokenizer::size_type pos = 1 ;

time_t gLength = bot->getDefaultGlineLength() ;
if( atoi( st[ pos ].c_str() ) > 0 )
	{
	// User has specified a gline length
	if( st.size() < 4 )
		{
		Usage( theClient ) ;
		return true ;
		}

	gLength = atoi( st[ pos ].c_str() ) ;
	pos++ ;
	}

// (pos) is the index of the next token, the user@host mask.

string::size_type atPos = st[ pos ].find_first_of( '@' ) ;
if( string::npos == atPos )
	{
	// User has only specified hostname, not a user name
	bot->Notice( theClient, "GLINE: Please specify gline mask in the "
		"format: user@host" ) ;
	return true ;
	}

string userName = st[ pos ].substr( 0, pos ) ;
string hostName = st[ pos ].substr( pos + 1 ) ;

if( hostName.find_first_of( '*' ) != string::npos )
	{
	// Wildcard in hostname, do nothing for now.
	}

// Avoid passing a reference to a temporary variable.
string nickUserHost = theClient->getNickUserHost() ;

server->setGline( nickUserHost,
	st[ pos ],
	st.assemble( pos + 1 ),
	gLength ) ;

strstream s ;
s	<< server->getCharYY() << " WA :"
	<< theClient->getCharYYXXX()
	<< " is adding gline for: "
	<< st[ pos ]
	<< ", expires at " << (time( 0 ) + gLength)
	<< " because: " << st.assemble( pos + 1 )
	<< ends ;
bot->Write( s ) ;
delete[] s.str() ;

return true ;
}

// scangline *user@*host
bool SCANGLINECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

vector< const Gline* > glines = server->matchGline( st[ 1 ] ) ;

bot->Notice( theClient, "Found %d matches", glines.size() ) ;
if( glines.empty() )
	{
	return true ;
	}

bot->Notice( theClient, "Current time: %d", time( 0 ) ) ;

for( vector< const Gline* >::const_iterator ptr = glines.begin() ;
	ptr != glines.end() ; ++ptr )
	{
	bot->Notice( theClient, "%s expires at %d, set by %s because %s",
		(*ptr)->getUserHost().c_str(),
		(*ptr)->getExpiration(),
		(*ptr)->getSetBy().c_str(),
		(*ptr)->getReason().c_str() ) ;
	}

return true ;
}

// remgline user@host
bool REMGLINECommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

if( !server->removeGline( st[ 1 ] ) )
	{
	// Remove failed
	bot->Notice( theClient, "Removal of gline failed" ) ;
	}
else
	{
	// Removal succeeded
	bot->Notice( theClient, "Removal of gline succeeded" ) ;
	bot->Wallops( "RemGline: %s", st[ 1 ].c_str() ) ;
	}

return true ;
}

// invite #channel
bool INVITECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;

if( st.size() == 1 )
	{
	// send help
	Usage( theClient ) ;
	return false ;
	}

string chanName = st[ 1 ] ;
if( chanName[ 0 ] != '#' )
	{
	chanName.insert( chanName.begin(), '#' ) ;
	}

char buf[ 512 ] = { 0 } ;

// Invite buffer
sprintf( buf, "%s I %s :%s\n",
	bot->getCharYYXXX().c_str(),
	theClient->getNickName().c_str(),
	chanName.c_str() ) ;

if( bot->isOperChan( chanName ) )
	{
	// No problem

	// Send the invitation
	bot->QuoteAsServer( buf ) ;
	return true ;
	}

// Else, this is a user channel
// Note that this assumes that the bot
// is in no channels other than oper
// channels.

// Join
bot->Join( chanName, string(), 0, true ) ;

// Invite
bot->QuoteAsServer( buf ) ;

// Part
bot->Part( st[ 1 ] ) ;

// Wallops
if( !bot->isOperChan( st[ 1 ] ) )
	{
	string wallopMe = theClient->getCharYYXXX() ;
	wallopMe += " is cordially invited to channel "
		+ chanName ;
	bot->Wallops( wallopMe ) ;
	}

return true ;
}

// jupe servername reason
bool JUPECommand::Exec( iClient* theClient, const string& Message )
{

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage( theClient ) ;
	return false ;
	}

// The server name to be juped must have at least 1 '.'
if( string::npos == st[ 1 ].find_first_of( '.' ) )
	{
	bot->Notice( theClient, "Bogus server name" ) ;
	return false ;
	}

string writeMe( theClient->getCharYYXXX() ) ;
writeMe += " is asking me to jupe " ;
writeMe += st[ 1 ] + " because: " + st.assemble( 2 ) ;
bot->Wallops( writeMe ) ;

// This will squit the server, if it exists or not, or if it is
// already juped.
server->SquitServer( st[ 1 ], "Prepare to be juped" ) ;

iServer* jupeServer = new iServer(
	0, // uplinkIntYY
	"", // charYYXXX
	st[ 1 ],
	time( 0 ),
	time( 0 ),
	10 ) ;

// Attach the new (fake) server.
server->AttachServer( jupeServer, st.assemble( 2 ) ) ;

return true ;

}

} // close namespace ccontrolns
