/* NEWPASSCommand.cc */

#include	<string>
#include	<iomanip.h>

#include	"md5hash.h" 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h" 
#include	"responses.h" 
#include	"networkData.h"

const char NEWPASSCommand_cc_rcsId[] = "$Id: NEWPASSCommand.cc,v 1.1 2001/01/19 02:29:31 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool NEWPASSCommand::Exec( iClient* theClient, const string& Message )
{

	const char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.$*_";
 
	StringTokenizer st( Message ) ;
	if( st.size() < 3 )
	{
		Usage(theClient);
		return true;
	}

	/*
	 *  Fetch the sqlUser record attached to this client. If there isn't one,
	 *  they aren't logged in - tell them they should be.
	 */

	sqlUser* tmpUser = bot->isAuthed(theClient, true);
	if (!tmpUser) {
		return false;
	}
 
	/*
	 *  First, check the two supplied passwords match.
	 */

	if (st[1] != st[2])
	{
		bot->Notice(theClient, "Those passwords don't match. However, this proves you are honest and didn't copy and paste.");
		return false;
	}

	/* Work out some salt. */
	string salt;
	int i;

	srand(clock() * 1000000); 
	for ( i=0 ; i < 8; i++) 
	{ 
		int randNo = 1+(int) (64.0*rand()/(RAND_MAX+1.0));
		salt += validChars[randNo]; 
	} 

	/* Work out a MD5 hash of our salt + password */

	md5	hash; // MD5 hash algorithm object.
	md5Digest digest; // MD5Digest algorithm object.
 
	strstream output;
	string newPass;
	newPass = salt + st[1];

	hash.update( (unsigned char *)newPass.c_str(), strlen( newPass.c_str() ));
	hash.report( digest );
	
	/* Convert to Hex */

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
 

 	string finalPassword = salt + output.str();
 	tmpUser->setPassword(finalPassword); 
	tmpUser->commit();
	bot->Notice(theClient, "Password successfully changed.");

	return true; 
} 

} // namespace gnuworld.
