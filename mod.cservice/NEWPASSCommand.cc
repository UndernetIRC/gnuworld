/* NEWPASSCommand.cc */

#include	<string>
#include	<iomanip.h>

#include	"md5hash.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"responses.h"
#include	"networkData.h"
#include	"cservice_config.h"

const char NEWPASSCommand_cc_rcsId[] = "$Id: NEWPASSCommand.cc,v 1.13 2002/03/10 19:50:14 gte Exp $" ;

namespace gnuworld
{
const char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.$*_";

bool NEWPASSCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.NEWPASS");

#ifndef USE_NEWPASS
bot->Notice(theClient, "To change your account password, please use the web interface.");
return true;
#else

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

/*
 *  Fetch the sqlUser record attached to this client. If there isn't one,
 *  they aren't logged in - tell them they should be.
 */

sqlUser* tmpUser = bot->isAuthed(theClient, true);
if (!tmpUser)
	{
	return false;
	}

/* Try and stop people using an invalid syntax.. */
if ( (string_lower(st[1]) == string_lower(tmpUser->getUserName()))
	  || (string_lower(st[1]) == string_lower(theClient->getNickName())) )
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::pass_cant_be_nick,
			string("Your passphrase cannot be your username or current nick - syntax is: NEWPASS <new passphrase>")));
	return false;
	}

if (st.assemble(1).size() > 50)
	{
	bot->Notice(theClient, "Your passphrase cannot exceed 50 characters.");
	return false;
	}

if (st.assemble(1).size() < 6)
	{
	bot->Notice(theClient, "Your passphrase cannot be less than 6 characters.");
	return false;
	}


/* Work out some salt. */
string salt;

// TODO: Why calling srand() here?
srand(clock() * 1000000);

// TODO: What is the significance of '8' here?
// Schema states a fixed 8 bytes of random salt are used in generating the
// passowrd.
for ( unsigned short int i = 0 ; i < 8; i++)
	{
	int randNo = 1+(int) (64.0*rand()/(RAND_MAX+1.0));
	salt += validChars[randNo];
	}

/* Work out a MD5 hash of our salt + password */

md5	hash; // MD5 hash algorithm object.
md5Digest digest; // MD5Digest algorithm object.

// Prepend the salt to the password
string newPass = salt + st.assemble(1);

// Take the md5 hash of this newPass string
hash.update( (const unsigned char *)newPass.c_str(), newPass.size() );
hash.report( digest );

/* Convert to Hex */

int data[ MD5_DIGEST_LENGTH ] = { 0 } ;
for( size_t i = 0 ; i < MD5_DIGEST_LENGTH ; ++i )
	{
	data[ i ] = digest[ i ] ;
	}

strstream output;
output << hex;
output.fill('0');
for( size_t ii = 0; ii < MD5_DIGEST_LENGTH; ii++ )
	{
	output << setw(2) << data[ii];
	}
output << ends;

// Prepend the md5 hash to the salt
string finalPassword = salt + output.str();
tmpUser->setPassword(finalPassword);

if( tmpUser->commit() )
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::pass_changed,
			string("Password successfully changed.")));
	}
else
	{
	// TODO
	bot->Notice( theClient,
		"NEWPASS: Unable to commit to database" ) ;
	}

delete[] output.str() ;
return true;

#endif

}

} // namespace gnuworld.
