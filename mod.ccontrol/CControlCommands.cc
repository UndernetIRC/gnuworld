/* CControlCommands.cc
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
//#include	<netinet/in.h>

#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"Network.h"
#include	"misc.h"
#include	"Gline.h"
#include	"ip.h"
#include	"ELog.h"
#include	"md5hash.h" 

namespace gnuworld
{

namespace ccontrolns
{

using std::string ;

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

bot->Notice( theClient, "Current time: %d", ::time( 0 ) ) ;

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
	bot->Notice( theClient, "Removal of gline succeeded\n" ) ;
	bot->Wallops( "RemGline: %s\n", st[ 1 ].c_str() ) ;
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
bot->Part( chanName ) ;

// Wallops
if( !bot->isOperChan( chanName ) )
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

bool ACCESSCommand::Exec( iClient* theClient, const string& Message)
{
static const char* queryHeader =    "SELECT user_name,access,last_updated_by FROM opers; ";
 
StringTokenizer st( Message ) ;

strstream theQuery;
theQuery	<< queryHeader 
		<< ends;

elog	<< "ACCESS::sqlQuery> "
	<< theQuery.str()
	<< endl; 

ExecStatusType status = bot->SQLDb->Exec( theQuery.str() ) ;
if( PGRES_TUPLES_OK == status )
	{
	for (int i = 0 ; i < bot->SQLDb->Tuples(); i++)
		{
		bot->Notice(theClient, "USER: %s ACCESS: %s MODIFIED BY: %s",
			bot->SQLDb->GetValue(i, 0),
			bot->SQLDb->GetValue(i, 1),
			bot->SQLDb->GetValue(i, 2));
		}
	return true ;
	}
else
	{
	elog	<< "Error in query!"
		<< endl;
	}

return false;    
}

bool LOGINCommand::Exec( iClient* theClient, const string& Message)
{
	StringTokenizer st( Message ) ;
	if( st.size() < 3 )
	{
		Usage(theClient);
		return true;
	}

	AuthInfo* tmpUser = bot->IsAuth(theClient->getCharYYXXX());
	if (tmpUser) {
		bot->Notice(theClient, "You  are already authenticated! ", 
			tmpUser->Name.c_str());
		return false;
	}

	/*
	 *  Find the user record, confirm authorisation and attach the record to this client. 
	 */
 
	User* theUser = bot->GetUser(st[1]);
	if (theUser) { 
		/*
		 *  Compare password with MD5 hash stored in user record.
		 */

		if(!bot->UserGotMask(theUser,theClient->getNickUserHost()))
		{
		    bot->Notice(theClient,"Sorry but your mask doesnt appear in my access list!");
		    return false;
		}

		md5	hash; // MD5 hash algorithm object.
		md5Digest digest; // MD5Digest algorithm object.
		strstream output;
		string salt = theUser->Password.substr(0, 8);
		string md5Part = theUser->Password.substr(8);
		string guess = salt + st.assemble(2);

		// Build a MD5 hash based on our salt + the guessed password.
		hash.update( (unsigned char *)guess.c_str(), strlen( guess.c_str() ));
		hash.report( digest );

		// Convert the digest into an array of int's to output as hex for 
		// comparison with the passwords generated by PHP.
		int data[ MD5_DIGEST_LENGTH ];
		int ii;
		for( ii = 0; ii < MD5_DIGEST_LENGTH; ii++ )
		{
			data[ii] = digest[ii];
		}
		output << hex;
		output.fill('0');
		for( ii = 0; ii < MD5_DIGEST_LENGTH; ii++ ) {
			output << setw(2) << data[ii];
		}
		output << ends;

		if (output.str() != md5Part) // If the MD5 hash's don't match..
		{
			bot->Notice(theClient, "AUTHENTICATION FAILED as %s (Invalid Password).", theUser->UserName.c_str());
			return false;
		}

		theUser->UserName = st[1];
		theUser->Numeric = theClient->getCharYYXXX();
		if(bot->AuthUser(theUser))
		    if(!(theUser->Flags & isSUSPENDED))
			bot->Notice(theClient, "Authentication successful! ",theUser->UserName.c_str()); 
		    else 
			bot->Notice(theClient, "Authentication successful,However you are suspended ",theUser->UserName.c_str()); 
		else
		    bot->Notice(theClient, "Error in authentication ",theUser->UserName.c_str()); 
	delete (theUser);
	} else
	{
		bot->Notice(theClient, "Sorry, I don't know who %s is.", st[1].c_str());
		return false;
	}

	return true; 
} 
		
bool DEAUTHCommand::Exec( iClient* theClient, const string& Message)
{
    	AuthInfo* tmpUser = bot->IsAuth(theClient->getCharYYXXX());
	if (!tmpUser) 
	{
	    bot->Notice(theClient,"Your are not authenticated");
	    return false;
	}
	if(bot->deAuthUser(theClient->getCharYYXXX()))
	{
	    bot->Notice(theClient,"DeAuthentication successfull");
	    return true;
	}
	else
	{
	    bot->Notice(theClient,"DeAuthentication failed");
	    return false;
	}
}	

bool ADDNEWOPERCommand::Exec( iClient* theClient, const string& Message)
{
 
	StringTokenizer st( Message ) ;
	if( st.size() < 4 )
	{
		Usage(theClient);
		return true;
	}
 
	
	User* theUser = bot->GetUser(st[1]);
	if (theUser) 
	{ 
	    bot->Notice(theClient,"Oper %s already exsits in my db," 
	    "please change the oper handle and try again",theUser->UserName.c_str());
	    return false;
	}	    
	int NewAccess;
	int NewFlags;
	if(!strcasecmp(st[2].c_str(),"coder"))
	{
	    NewAccess = CODER;
	    NewFlags = isCODER;
	}
	else
	if(!strcasecmp(st[2].c_str(),"admin"))
	{
	    NewAccess = ADMIN;
	    NewFlags = isADMIN;
	}
	else
	if(!strcasecmp(st[2].c_str(),"oper"))
	{
	    NewAccess = OPER;
	    NewFlags = isOPER;
	}
	else
	{
	    bot->Notice(theClient,"Illegal oper type, Types are : oper , admin , coder");
	    return false;
	}	     	
	
	AuthInfo *tOper = bot->IsAuth(theClient->getCharYYXXX());

	if(tOper->Access < NewAccess)
	{
	    bot->Notice(theClient,"You can't add an oper with higher access than yours!");
	    return false;
	}	     	
	
 	theUser = new User;
	theUser->UserName = st[1];
	theUser->Password = bot->CryptPass(st[3]);
	theUser->Access=NewAccess;
	theUser->Flags=NewFlags;
	theUser->last_updated_by = theClient->getNickUserHost();
	if(bot->AddOper(theUser) == true)
	    bot->Notice(theClient, "Oper successfully Added.");
	else
	    bot->Notice(theClient, "Error while adding new oper.");
	return true; 
}

bool REMOVEOPERCommand::Exec( iClient* theClient, const string& Message)
{
    	StringTokenizer st( Message ) ;
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}
    	User* theUser = bot->GetUser(st[1]);
	if (!theUser) 
	{ 
	    bot->Notice(theClient,"Oper %s does not exists db," 
	    "check your handle and try again",st[1].c_str());
	    return false;
	}	    
	
	if(bot->DeleteOper(string_lower(st[1])))     
	{    
	    bot->Notice(theClient,"Successfully Deleted Oper %s ",st[1].c_str());
	    AuthInfo *TDeauth = bot->IsAuth(theUser->Id);
	    if(TDeauth)
	    {
		iClient *TClient = Network->findClient(TDeauth->Numeric); 
		if(TClient)
		    bot->Notice(TClient,"You have been removed from my access list");
		bot->deAuthUser(TDeauth->Numeric);
	    }	
	    return true;	
	}
	else
	{    
	    bot->Notice(theClient,"Error While Deleting Oper %s ",st[1].c_str());
	    return false;	
	}
}
	
bool ADDCOMMANDCommand::Exec( iClient* theClient, const string& Message)
{
    	StringTokenizer st( Message ) ;

	if( st.size() < 3 )
	{
		Usage(theClient);
		return true;
	}


    	User* theUser = bot->GetUser(st[1]);
	
	if(!theUser)
	{	
	    bot->Notice(theClient,"I cant find oper %s",st[1].c_str());
	    return false;
	}
	
	int CommandLevel = bot->getCommandLevel(st[2]);
	
	if(CommandLevel < 0 )
	{
	    bot->Notice(theClient,"Command %s does not exists!",st[2].c_str());
	    delete theUser;
	    return false;	        
	}
	
	AuthInfo *AClient = bot->IsAuth(theClient->getCharYYXXX());
	
	if(!(AClient->Access & CommandLevel))
	{
	    bot->Notice(theClient,"You must have access to a command inorder to add it");
	    delete theUser;
	    return false;
	}
	
	else if(theUser->Access & CommandLevel)
	{
	    bot->Notice(theClient,"%s already got access for %s",st[1].c_str(),st[2].c_str());
	    delete theUser;
	    return false;	        
	}	
	
	theUser->Access |= CommandLevel;
	theUser->last_updated_by = theClient->getNickUserHost();
	if(bot->UpdateOper(theUser))
	{
	    bot->Notice(theClient,"Successfully added the command for %s",st[1].c_str());
	    bot->UpdateAuth(theUser->Id);
	    delete theUser;
	    return true;
	}
	else
	{
	    bot->Notice(theClient,"Error while adding command for %s",st[1].c_str());
	    return false;
	}
	
}	

bool REMOVECOMMANDCommand::Exec( iClient* theClient, const string& Message)
{
    	StringTokenizer st( Message ) ;

	if( st.size() < 3 )
	{
		Usage(theClient);
		return true;
	}


    	User* theUser = bot->GetUser(st[1]);
	
	if(!theUser)
	{	
	    bot->Notice(theClient,"I cant find oper %s",st[1].c_str());
	    return false;
	}
	
	int CommandLevel = bot->getCommandLevel(st[2]);
	if(CommandLevel < 0 )
	{
	    bot->Notice(theClient,"Command %s does not exists!",st[2].c_str());
	    delete theUser;
	    return false;	        
	}
	
	else if(!(theUser->Access & CommandLevel))
	{
	    bot->Notice(theClient,"%s doest have access for %s",st[1].c_str(),st[2].c_str());
	    delete theUser;
	    return false;	        
	}	
	
	theUser->Access &= ~CommandLevel;
	theUser->last_updated_by = theClient->getNickUserHost();
	if(bot->UpdateOper(theUser))
	{
	    bot->Notice(theClient,"Successfully removed the command from %s",st[1].c_str());
	    bot->UpdateAuth(theUser->Id);
	    delete theUser;
	    return true;
	}
	else
	{
	    bot->Notice(theClient,"Error while removing command from %s",st[1].c_str());
	    delete theUser;
	    return false;
	}
	
}	

bool NEWPASSCommand::Exec( iClient* theClient, const string& Message)
{
    	StringTokenizer st( Message ) ;
	
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}

	AuthInfo *tmpUser = bot->IsAuth(theClient->getCharYYXXX());
	
	if(!tmpUser)
	{
	    bot->Notice(theClient,"You have to be logged in to use this command");
	    return false;
	}
	
	User* theUser = bot->GetUser(tmpUser->Name);

	theUser->Password = bot->CryptPass(st[1]);
	if(bot->UpdateOper(theUser))
	{
	    bot->Notice(theClient,"Password changed!");
	    delete tmpUser;
	    return true;
	}
	else
	{
	    bot->Notice(theClient,"Error while changing password");
	    delete tmpUser;
	    return true;
	}
}	

bool SUSPENDOPERCommand::Exec( iClient* theClient, const string& Message)
{
    	StringTokenizer st( Message ) ;
	
	if( st.size() < 4 )
	{
		Usage(theClient);
		return true;
	}
	
	User *tmpUser = bot->GetUser(st[1]);
	if(!tmpUser)
	{
	    bot->Notice(theClient,"%s isnt on my access list",st[1].c_str());
	    return false;
	}
	
	if(tmpUser->Flags & isSUSPENDED)
	{
	    bot->Notice(theClient,"%s is already suspended",st[1].c_str());
	    delete tmpUser;
	    return false;
	}
	
	if(!strcasecmp(st[3].c_str(),"s"))
	    tmpUser->SuspendExpires = atoi(st[2].c_str());
	else if(!strcasecmp(st[3].c_str(),"m"))
	    tmpUser->SuspendExpires = atoi(st[2].c_str()) * 60;
	else if(!strcasecmp(st[3].c_str(),"h"))
	    tmpUser->SuspendExpires = atoi(st[2].c_str()) * 3600;
	else if(!strcasecmp(st[3].c_str(),"d"))
	    tmpUser->SuspendExpires = atoi(st[2].c_str()) * 3600*24;
	else
	{
	    bot->Notice(theClient,"%s is not a proper time refrence");
	    delete tmpUser;
	}
	tmpUser->SuspendExpires += time( 0 );
	tmpUser->SuspendedBy = theClient->getNickUserHost();	    
	tmpUser->Flags |= isSUSPENDED;
	
	if(bot->UpdateOper(tmpUser))
	{
	    bot->Notice(theClient,"%s has been suspended",st[1].c_str());
	    bot->UpdateAuth(tmpUser->Id);
	    delete tmpUser;
	    return true;
	}
	else
	{
	    bot->Notice(theClient,"Error while suspendeding %s",st[1].c_str());
	    delete tmpUser;
	    return false;
	}

}

bool UNSUSPENDOPERCommand::Exec( iClient* theClient, const string& Message)
{
    	StringTokenizer st( Message ) ;
	
	if( st.size() < 2 )
	{
		Usage(theClient);
		return true;
	}
	
	User *tmpUser = bot->GetUser(st[1]);
	if(!tmpUser)
	{
	    bot->Notice(theClient,"%s isnt on my access list",st[1].c_str());
	    return false;
	}
	
	if(!(tmpUser->Flags & isSUSPENDED))
	{
	    bot->Notice(theClient,"%s is not suspended",st[1].c_str());
	    return false;
	}
	
	tmpUser->SuspendExpires = 0;
	tmpUser->Flags &= ~isSUSPENDED;
	tmpUser->SuspendedBy ="";
	
	if(bot->UpdateOper(tmpUser))
	{
	    bot->Notice(theClient,"%s has been unsuspended",st[1].c_str());
	    bot->UpdateAuth(tmpUser->Id);
	    delete tmpUser;
	    return true;
	}
	else
	{
	    bot->Notice(theClient,"Error while unsuspendeding %s",st[1].c_str());
	    return false;
	}

}

bool MODOPERCommand::Exec( iClient* theClient, const string& Message)
{	 
    	StringTokenizer st( Message ) ;
	
	if( st.size() < 4 )
	{
		Usage(theClient);
		return true;
	}

	User *tmpUser = bot->GetUser(st[1]);
	if(!tmpUser)
	{
	    bot->Notice(theClient,"%s isnt on my access list",st[1].c_str());
	    return false;
	}
	
	AuthInfo* tmpAuth = bot->IsAuth(theClient->getCharYYXXX());
	if(tmpAuth->Flags < tmpUser->Flags)
	{
	    bot->Notice(theClient,"You cant modify a user who got higher level than yours");
	    delete tmpUser;
	    return false;
	}
	else if(!strcasecmp(st[2].c_str(),"newpass"))
	{
	    tmpUser->Password = bot->CryptPass(st[3]);
	    tmpUser->last_updated_by = theClient->getNickUserHost();
	    if(bot->UpdateOper(tmpUser))
	    {
		bot->Notice(theClient,"Password for %s Changed to %s",st[1].c_str(),st[2].c_str());
	    }
	    else
	    {
		bot->Notice(theClient,"Error while changing password for %s",st[1].c_str());
	    }
	}
	else if(!strcasecmp(st[2].c_str(),"addhost"))
	{
	    if(!bot->validUserMask(st[3]))
	    {
		bot->Notice(theClient,"Mask %s isnt not a valid mask in the form of *!*@*",st[3].c_str());
	    }
	    else if(bot->UserGotHost(tmpUser,st[3]) || bot->UserGotMask(tmpUser,st[3]))
	    {
		bot->Notice(theClient,"%s already got the host %s covered by other mask",st[1].c_str(),st[3].c_str());
	    }
	    else if(bot->AddHost(tmpUser,st[3]))
	    {
		bot->Notice(theClient,"Mask %s added for %s",st[3].c_str(),st[1].c_str());
	    }
	    else
	    {
		bot->Notice(theClient,"Error while adding mask %s  for %s",st[3].c_str(),st[1].c_str());
	    }
	}
	else if(!strcasecmp(st[2].c_str(),"delhost"))
	{
	    if(!bot->UserGotHost(tmpUser,st[3]))
	    {
		bot->Notice(theClient,"%s doesnt havee the host %s in my access list",st[1].c_str(),st[3].c_str());
	    }
	    else if(bot->DelHost(tmpUser,st[3]))
	    {
		bot->Notice(theClient,"Mask %s was deleted from %s access list",st[3].c_str(),st[1].c_str());
	    }
	    else
	    {
		bot->Notice(theClient,"Error while deleting mask %s from %s access list",st[3].c_str(),st[1].c_str());
	    }
	}	    
    	else
	{
	    bot->Notice(theClient,"Unknown option",st[3].c_str(),st[1].c_str());
	}
	
	delete tmpUser;
	return true;
}		    				

} // close namespace ccontrolns

} // namespace gnuworld
