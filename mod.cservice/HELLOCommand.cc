/* HELLOCommand.cc */

#include	<string>

#include	"cservice_config.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"Network.h"
#include	"cservice.h"

const char HELLOCommand_cc_rcsId[] = "$Id: HELLOCommand.cc,v 1.1 2005/04/03 22:11:42 dan_karrels Exp $" ;

namespace gnuworld
{
using std::string ;

bool HELLOCommand::Exec( iClient* theClient, const string& Message )
{
#ifdef ALLOW_HELLO
bot->incStat("COMMANDS.HELLO");

StringTokenizer st( Message ) ;
if( st.size() < 4 )
	{
	Usage(theClient);
	return true;
	}

const char validChars[] 
	= "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

sqlUser* theUser = bot->isAuthed(theClient, false);
if (theUser)
        {
	bot->Notice(theClient, "You can't create another "
		"account when you already have one!");
        return false;
        }

/*
 * Check this IP hasn't already tried in the last 48 hours.
 */

cservice::helloIPListType::iterator itr =
	bot->helloIPList.find( theClient->getIP() ) ;

if (itr != bot->helloIPList.end())
	{
	/*
	 * Has it been 48hours already?
	 */
	if (bot->currentTime() < itr->second)
		{
		bot->Notice(theClient, "Sorry, an account has "
			"already been registered from this IP within "
			"the last %i hours. "
			"Please wait and try again, or contact cservice.", 
			bot->helloBlockPeriod / 3600);

		return false;
		}
	}

sqlUser* newUser = bot->getUserRecord(st[1].c_str());
if(newUser)
	{
	bot->Notice(theClient, "This username already exists!");
	return false;
	}

/*
 * Check the username contains valid characters/correct 
length.
 */
if (  (st[1].size() < 2) || (st[1].size() > 12)  )
	{
	bot->Notice(theClient, "Invalid username.");
	return false;
	}

string theUserName = st[1];
bool badName = false;

for( string::const_iterator ptr = theUserName.begin() ;
	ptr != theUserName.end() ; ++ptr )
	{
	/*
	 * 62 entries in the table. 26 + 26 + 10 digits.
	 */

	bool found = false;
	for (int f = 0; f < 62; f++)
		{
		if(*ptr == validChars[f])
			{
			found = true;
			}
		}
	if (!found)
		{
		badName = true;
		}
	}

if (badName)
	{
	bot->Notice(theClient, "Invalid username.");
	return false;
	}

/*
 * Do some basic validation of the email address.
 */
if (st[2].size() > 128)
	{
	bot->Notice(theClient, "E-mail address is too long.");
	return false;
	}

if( string::npos == st[2].find_first_of('@') )
	{
	/*
	 * No @?
	 */
	bot->Notice(theClient, "Invalid e-mail address.");
	return false;
	}

/*
 * Check if they've at least vaugly attempted to specify a 
 * proper domain structure
 * and we have the right number of @'s.
 */
StringTokenizer emailCheck( st[2], '@' ) ;

if (  (emailCheck.size() != 2) ||
	(string::npos == emailCheck[1].find_first_of('.')) )
	{
	bot->Notice(theClient, "Invalid e-mail address.");
	return false;
	}

if(strcasecmp(st[2], st[3]))
	{
        bot->Notice(theClient, "E-mail addresses don't match!");
        return false;
	}

/*
 * TODO: Check this email address doesn't already exist in 
 * the database!
 */

/*
 * We need to give this user a password
 */
string plainpass;

for ( unsigned short int i = 0 ; i < 8 ; i++ )
        {
        int randNo = 1+(int) (62.0*rand()/(RAND_MAX+1.0));
        plainpass += validChars[randNo];
        }

string cryptpass = bot->CryptPass(plainpass);
string updatedBy = "HELLO used by: ";
updatedBy += theClient->getNickUserHost().c_str();

newUser = new (std::nothrow) sqlUser(bot->SQLDb);
newUser->setUserName(escapeSQLChars(st[1].c_str()));
newUser->setEmail(escapeSQLChars(st[2]));
newUser->setPassword(cryptpass.c_str());
newUser->setLastUpdatedBy(updatedBy);
newUser->Insert();

bot->Notice(theClient, "I generated this password for you: \002%s\002",
	plainpass.c_str());
bot->Notice(theClient, "Login using \002/msg %s@%s LOGIN %s %s\002",
	bot->getNickName().c_str(),
	bot->getUplinkName().c_str(),
	st[1].c_str(),
	plainpass.c_str());
bot->Notice(theClient, "Then change your password using \002/msg "
	"%s@%s NEWPASS <new_password>\002",
	bot->getNickName().c_str(),
	bot->getUplinkName().c_str());

bot->helloIPList.erase(theClient->getIP());
bot->helloIPList.insert(
	std::make_pair(theClient->getIP(),
		bot->currentTime() + bot->helloBlockPeriod) );

delete (newUser);
#endif // ALLOW_HELLO

return true ;
}

} // namespace gnuworld.
