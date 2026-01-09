/**
 * NEWPASSCommand.cc
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
 * $Id: NEWPASSCommand.cc,v 1.17 2005/11/17 22:20:57 kewlio Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>
#include	<iomanip>
#include	<inttypes.h>

#include	"md5hash.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"responses.h"
#include	"networkData.h"
#include	"cservice_config.h"

namespace gnuworld
{
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

const char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.$*_";

bool NEWPASSCommand::Exec( iClient* theClient, const string& Message )
{
#ifndef USE_NEWPASS
bot->Notice(theClient, "To change your account password, please use the web interface.");
(void)Message; (void)validChars;
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

stringstream output;
output << std::hex;
output.fill('0');
for( size_t ii = 0; ii < MD5_DIGEST_LENGTH; ii++ )
	{
	output << std::setw(2) << data[ii];
	}
output << ends;

// Prepend the md5 hash to the salt
string finalPassword = salt + output.str().c_str();
tmpUser->setPassword(finalPassword);
std::string err ;
//	elog << "Generating SCRAM record for user " << theUser->getUserName() << " and password: [" << st.assemble( 0, pass_end ) << "]\n" ;
auto recOpt = make_scram_sha256_record( st.assemble( 1 ), &err ) ;

if( !recOpt )
	{
	LOG( ERROR, "SCRAM generation error: {}", err ) ;
	}
else
	{
	std::string scram_record = *recOpt ;
	tmpUser->setScramRecord( scram_record ) ;
	}

if( tmpUser->commit(theClient) )
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser,
			language::pass_changed,
			string("Password successfully changed.")));

	bot->InsertUserHistory(theClient, "NEWPASS");
	}
else
	{
	// TODO
	bot->Notice( theClient,
		"NEWPASS: Unable to commit to database" ) ;
	}

return true;

#endif

}

} // namespace gnuworld.
