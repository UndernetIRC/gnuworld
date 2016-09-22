/**
 * LOGINCommand.cc
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
 * $Id: LOGINCommand.cc,v 1.67 2009/06/09 15:40:29 mrbean_ Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>
#include	<iomanip>
#include	<inttypes.h>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"md5hash.h"
#include	"responses.h"
#include	"levels.h"
#include	"networkData.h"
#include	"cservice_config.h"
#include	"Network.h"
#include	"ip.h"

const char LOGINCommand_cc_rcsId[] = "$Id: LOGINCommand.cc,v 1.67 2009/06/09 15:40:29 mrbean_ Exp $" ;

namespace gnuworld
{
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;
using namespace gnuworld;

bool LOGINCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.LOGIN");

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}

/*
 * Check theClient isn't already logged in, if so, tell
 * them they shouldn't be.
 */

sqlUser* tmpUser = bot->isAuthed(theClient, false);
if (tmpUser)
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser, language::already_authed).c_str(),
		tmpUser->getUserName().c_str());
	return false;
	}
unsigned int maxFailedLogins = bot->getConfigVar("MAX_FAILED_LOGINS")->asInt();
unsigned int failedLogins = bot->getFailedLogins(theClient);
if ((maxFailedLogins > 0) && (failedLogins >= maxFailedLogins))
{
        /* exceeded maximum failed logins */
        bot->Notice(theClient,
                bot->getResponse(tmpUser,
                language::max_failed_logins,
                string("AUTHENTICATION FAILED as %s (Exceeded maximum login failures for this session)")).c_str(),
                st[1].c_str());
        return false;
}

sqlUser* theUser;
int auth_res =  bot->authenticateUser(st[1],st.assemble(2),theClient,&theUser);
unsigned int loginTime = bot->getUplink()->getStartTime() + bot->loginDelay;
unsigned int max_failed_logins = bot->getConfigVar("FAILED_LOGINS")->asInt();
unsigned int failed_login_rate = bot->getConfigVar("FAILED_LOGINS_RATE")->asInt();
string clientList;

switch(auth_res)
	{
	case cservice::TOO_EARLY_TOLOGIN:
		bot->Notice(theClient, "AUTHENTICATION FAILED as %s. (Unable "
                	"to login during reconnection, please try again in "
	                "%i seconds)",
        	        st[1].c_str(), (loginTime - bot->currentTime()));
		return false;
		break;
	case cservice::AUTH_FAILED:
		bot->setFailedLogins(theClient, failedLogins+1);
	        bot->Notice(theClient, "AUTHENTICATION FAILED as %s.", st[1].c_str());
        	return false;
		break;
	case cservice::AUTH_UNKNOWN_USER:
	        bot->setFailedLogins(theClient, failedLogins+1);
	        bot->Notice(theClient,
        	        bot->getResponse(tmpUser,
                        language::not_registered,
                        string("AUTHENTICATION FAILED as %s.")).c_str(),
                st[1].c_str());
	        return false;
		break;
	case cservice::AUTH_SUSPENDED_USER:
	        bot->setFailedLogins(theClient, failedLogins+1);
		bot->Notice(theClient, "AUTHENTICATION FAILED as %s. (Suspended)",
        		st[1].c_str());
	        return false;
		break;
	case cservice::AUTH_NO_TOKEN:
		bot->setFailedLogins(theClient, failedLogins+1);
		bot->Notice(theClient,"AUTHENTICATION FAILED as %s. (Missing TOTP token)",st[1].c_str());
                return false;
		break;
	case cservice::AUTH_INVALID_PASS:
		if (failed_login_rate==0)
		        failed_login_rate = 900;
		      bot->setFailedLogins(theClient, failedLogins+1);
	        bot->Notice(theClient,
                bot->getResponse(theUser,
                        language::auth_failed,
                        string("AUTHENTICATION FAILED as %s.")).c_str(),
                theUser->getUserName().c_str());
        	/* increment failed logins counter */
	        theUser->incFailedLogins();
        	if ((max_failed_logins > 0) && (theUser->getFailedLogins() > max_failed_logins) &&
                	(theUser->getLastFailedLoginTS() < (time(NULL) - failed_login_rate)))
		        {
	                /* we have exceeded our maximum - alert relay channel */
        	        /* work out a checksum for the password.  Yes, I could have
                	 * just used a checksum of the original password, but this
	                 * means it's harder to 'fool' the check digit with a real
        	         * password - create MD5 from original salt stored */
                	unsigned char   checksum;
	                md5             hash;
        	        md5Digest       digest;

                	if (theUser->getPassword().size() < 9)
                	{
                        	checksum = 0;
	                } else {
        	                string salt = theUser->getPassword().substr(0, 8);
                	        string guess = salt + st.assemble(2);

                        	hash.update( (const unsigned char *)guess.c_str(), guess.size() );
	                        hash.report( digest );

        	                checksum = 0;
                	        for (size_t i = 0; i < MD5_DIGEST_LENGTH; i++)
                        	{
	                                /* add ascii value to check digit */
        	                        checksum += digest[i];
                	        }
                	}

	                theUser->setLastFailedLoginTS(time(NULL));
        	        bot->logPrivAdminMessage("%d failed logins for %s (last attempt by %s, checksum %d).",
                	        theUser->getFailedLogins(),
                        	theUser->getUserName().c_str(),
	                        theClient->getRealNickUserHost().c_str(),
        	                checksum);
	        }
        	return false;
		break;
	case cservice::AUTH_ERROR:
		bot->Notice(theClient,"AUTHENTICATION FAILED as %s due to an error, please contact CService represetitive",st[1].c_str());
                return false;
		break;
	case cservice::AUTH_INVALID_TOKEN:
		 bot->setFailedLogins(theClient, failedLogins+1);
	         bot->Notice(theClient,
                 bot->getResponse(theUser,
                 	language::auth_failed_token,
                        string("AUTHENTICATION FAILED as %s. (Invalid Token)")).c_str(),
                        theUser->getUserName().c_str());
                /* increment failed logins counter */
                theUser->incFailedLogins();
                return false;
		break;
	case cservice::AUTH_FAILED_IPR:
		 bot->setFailedLogins(theClient, failedLogins+1);
                bot->Notice(theClient, "AUTHENTICATION FAILED as %s. (IPR)",
                        st[1].c_str());
                /* notify the relay channel */
                bot->logAdminMessage("%s (%s) failed IPR check.",
                        theClient->getNickName().c_str(),
                        st[1].c_str());
                /* increment failed logins counter */
                theUser->incFailedLogins();
                if ((max_failed_logins > 0) && (theUser->getFailedLogins() > max_failed_logins) &&
                        (theUser->getLastFailedLoginTS() < (time(NULL) - failed_login_rate)))
                {
                        /* we have exceeded our maximum - alert relay channel */
                        theUser->setLastFailedLoginTS(time(NULL));
                        bot->logPrivAdminMessage("%d failed logins for %s (last attempt by %s).",
                                theUser->getFailedLogins(),
                                theUser->getUserName().c_str(),
                                theClient->getRealNickUserHost().c_str());
                }
                return false;
		break;
	case cservice::AUTH_ML_EXCEEDED:
		bot->setFailedLogins(theClient, failedLogins+1);
                bot->Notice(theClient, "AUTHENTICATION FAILED as %s. (Maximum "
                        "concurrent logins exceeded).",
                        theUser->getUserName().c_str());

                for( sqlUser::networkClientListType::iterator ptr = theUser->networkClientList.begin() ;
                        ptr != theUser->networkClientList.end() ; )
                        {
                        clientList += (*ptr)->getNickUserHost();
                        ++ptr;
                        if (ptr != theUser->networkClientList.end())
                                {
                                clientList += ", ";
                                }
                        } // for()

                bot->Notice(theClient, "Current Sessions: %s", clientList.c_str());
                return false;
		break;
	case cservice::AUTH_SUCCEEDED:
		break;
	defult:
		//Should never get here!
		elog << "Response " << auth_res << " while authenticating!\n";
		bot->Notice(theClient,"AUTHENTICATION FAILED as %s (due to an error)\n",st[1].c_str());
		return false;
		break;
	}

return bot->doCommonAuth(theClient, theUser->getUserName());

}

} // namespace gnuworld.

