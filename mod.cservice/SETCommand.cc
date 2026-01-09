/**
 * SETCommand.cc
 *
 * 28/12/2000 - David Henriksen <david@itwebnet.dk>
 * Initial Version.
 * 01/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Modifications.
 * 10/02/2001 - David Henriksen <david@itwebnet.dk>
 * Minor bug fixes.
 *
 * 20/02/2001 - Gator Robert White <gator@cajun-gator.net>
 * removed AlwaysOp
 * Sets channel options on the specified channel.
 * 2001-03-16 - Perry Lorier <isomer@coders.net>
 * Added 'DESC' as an alias for 'DESCRIPTION'
 * 2001-04-16 - Alex Badea <vampire@go.ro>
 * Changed the implementation for SET LANG, everything is dynamic now.
 *
 * Caveats: None.
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
 * $Id: SETCommand.cc,v 1.64 2008/04/16 20:34:44 danielaustin Exp $
 */

#include	<string>
#include 	<sstream>

#include	"StringTokenizer.h"
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"
#include	"cservice_config.h"
#include <stdio.h>
#ifdef HAVE_LIBOATH
extern "C" {
#include <liboath/oath.h>
}
#endif

namespace gnuworld
{
using namespace level;

bool SETCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;

if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}

/* Is the user authorised? */

sqlUser* theUser = bot->isAuthed(theClient, true);
if(!theUser)
	{
	return false;
	}

/*
 * First, is this a #channel or user set?
 */

if( st[1][0] != '#' ) // Didn't find a hash?
{
	// Look by user then.
	string option = string_upper(st[1]);
	string value = string_upper(st[2]);
	if (option == "INVISIBLE")
	{
		if (value == "ON")
		{
			theUser->setFlag(sqlUser::F_INVIS);
			theUser->commit(theClient);
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::invis_on,
					string("Your INVISIBLE setting is now ON.")));
			return true;
		}

		if (value == "OFF")
		{
			theUser->removeFlag(sqlUser::F_INVIS);
			theUser->commit(theClient);
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::invis_off,
					string("Your INVISIBLE setting is now OFF.")));
			return true;
		}

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
	        return true;
	}

	if (option == "NOADDUSER")
	{
		if (value == "ON")
		{
			theUser->setFlag(sqlUser::F_NOADDUSER);
			theUser->commit(theClient);
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::noadduser_on,
					string("Your NOADDUSER setting is now ON.")));
			return true;
		}

		if (value == "OFF")
		{
			theUser->removeFlag(sqlUser::F_NOADDUSER);
			theUser->commit(theClient);
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::noadduser_off,
					string("Your NOADDUSER setting is now OFF.")));
			return true;
		}

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	}

#ifdef NEW_IRCU_FEATURES
	if (option == "AUTOHIDE")
	{
		if (value == "ON")
		{
			theUser->setFlag(sqlUser::F_AUTOHIDE);
			theUser->commit(theClient);
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::autohide_on,
					string("Your AUTOHIDE setting is now ON.")));
			return true;
		}

		if (value == "OFF")
		{
			theUser->removeFlag(sqlUser::F_AUTOHIDE);
			theUser->commit(theClient);
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::autohide_off,
					string("Your AUTOHIDE setting is now OFF.")));
			return true;
		}

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	}

	if (option == "CERTONLY")
	{
		if (value == "ON")
		{
			if (!bot->hasFP(theUser))
			{
				bot->Notice(theClient,
					bot->getResponse(theUser,
						language::no_fingerprints_registered,
						string("You currently don't have any fingerprints added to your account. For more information, use '/msg X help fingerprint'")));
				return true;
			}
			theUser->setFlag(sqlUser::F_CERTONLY);
			theUser->commit(theClient);
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::certonly_on,
					string("Your CERTONLY setting is now ON.")));
			return true;
		}

		if (value == "OFF")
		{
			theUser->removeFlag(sqlUser::F_CERTONLY);
			theUser->commit(theClient);
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::certonly_off,
					string("Your CERTONLY setting is now OFF.")));
			return true;
		}

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	}
#endif
#ifdef USE_NOTES
	if (option == "NONOTES")
	{
		if (value == "ON")
		{
			theUser->setFlag(sqlUser::F_NONOTES);
			theUser->commit(theClient);
			bot->Notice(theClient,"You are no longer able to receive notes from anyone.");
			return true;
		}

		if (value == "OFF")
		{
			theUser->removeFlag(sqlUser::F_NONOTES);
			theUser->commit(theClient);
			bot->Notice(theClient,"You are now able to receive notes.");
			return true;
		}

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
	        return true;
	}
#endif


	if (option == "DISABLEAUTH")
	{
		if (value == "ON")
		{
			/* only admins can use this command */
			int admLevel = bot->getAdminAccessLevel(theUser);
			if (admLevel > 0)
			{
				theUser->setFlag(sqlUser::F_NOADMIN);
				theUser->commit(theClient);
				bot->Notice(theClient,"You have lost the force :(");
				bot->logAdminMessage("%s (%s) set DISABLEAUTH on!",
					theClient->getNickName().c_str(), theUser->getUserName().c_str());
			} else {
				/* not an admin, return unknown command */
				bot->Notice(theClient,
					bot->getResponse(theUser,
					language::invalid_option,
					string("Invalid option."))); 
			}
			return true;
		}

		if (value == "OFF")
		{
			/* only allow removal if it is set! */
			if (theUser->getFlag(sqlUser::F_NOADMIN))
			{
				theUser->removeFlag(sqlUser::F_NOADMIN);
				theUser->commit(theClient);
				bot->Notice(theClient,"Welcome back, brave Jedi.");
				bot->logAdminMessage("%s (%s) set DISABLEAUTH off!",
					theClient->getNickName().c_str(), theUser->getUserName().c_str());
			} else {
				/* not set?  pretend it's an unknown command */
				bot->Notice(theClient,
					bot->getResponse(theUser,
					language::invalid_option,
					string("Invalid option.")));
			}
			return true;
		}

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
	        return true;
	}

	if (option == "NOPURGE")
	{
		int admLevel = bot->getAdminAccessLevel(theUser);
		if (!admLevel)
		{
			/* not an admin, return unknown command */
			bot->Notice(theClient,
				bot->getResponse(theUser,
				language::invalid_option,
				string("Invalid option.")));
			return true;
		}
		sqlUser* targetUser = theUser;
		if ((value != "ON") && (value != "OFF"))
		{
			targetUser = bot->getUserRecord(st[2]);
			if (!targetUser)
			{
				bot->Notice(theClient,
					bot->getResponse(theUser,
						language::not_registered,
						string("The user %s doesn't appear to be registered.")).c_str(),
					st[2].c_str());
				return true;
			}
			if (st.size() < 4)
			{
				bot->Notice(theClient,"SYNTAX: SET NOPURGE user ON|OFF");
				return false;
			}
			else
				value = string_upper(st[3]);
		}
		if (value == "ON")
		{
			targetUser->setFlag(sqlUser::F_NOPURGE);
			targetUser->commit(theClient);
			bot->Notice(theClient,"NOPURGE setting for %s is now ON", targetUser->getUserName().c_str());
			return true;
		}
		if (value == "OFF")
		{
			targetUser->removeFlag(sqlUser::F_NOPURGE);
			targetUser->commit(theClient);
			bot->Notice(theClient,"NOPURGE setting for %s is now OFF", targetUser->getUserName().c_str());
			return true;
		}
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
	        return true;
	}

#ifdef USE_SETMAXLOGINS
	if (option == "MAXLOGINS")
		{
		unsigned int maxlogins = atoi(value.c_str());
		if (maxlogins > 3 || maxlogins <= 0)
			{
				bot->Notice(theClient, "Max Logins cannot be greater than 3 or less than 1");
				return false;
			}

		theUser->setMaxLogins(maxlogins);
		theUser->commit(theClient);

		bot->Notice(theClient, "Max Logins now set to %i", maxlogins);
		return true;
		}
#endif

	if (option == "LANG")
	{
		cservice::languageTableType::iterator ptr = bot->languageTable.find(value);
		if (ptr != bot->languageTable.end())
		{
			string lang = ptr->second.second;
			theUser->setLanguageId(ptr->second.first);
			theUser->commit(theClient);
			bot->Notice(theClient,
			    bot->getResponse(theUser,
			    	    language::lang_set_to,
				    string("Language is set to %s.")).c_str(), lang.c_str());
			return true;
		}

		bot->Notice(theClient,
        		"ERROR: Invalid language selection.");
		return true;
	}

/* Require management of TOTP via website
#ifdef	TOTP_AUTH_ENABLED
	if (option == "TOTP")
	{	
		int admin = bot->getAdminAccessLevel(theUser);
		if((admin > 0) || (theUser->getFlag(sqlUser::F_OPER)) || (theClient->isOper())) {
			if(value == "ON") {
				if(theUser->getFlag(sqlUser::F_TOTP_ENABLED)) {
					bot->Notice(theClient,"TOTP is already enabled for your account");
					return true;
				}
				//Create a random hex string 168bit long
				char str_key2[20];
				char hex_key[41];
				srand(clock()*745+time(NULL));
				hex_key[0] = 0;
				for(int i=0; i < 20; i++) {
					str_key2[i]=((rand() %95) + 32);
					sprintf(hex_key+(i*2),"%02x",str_key2[i] & 0xFF);
				}	
				char* key;
				int res = oath_base32_encode(str_key2,20,&key,NULL);
				if(res != OATH_OK) {
					bot->Notice(theClient,"Failed to enable TOTP authentication, please contact a cservice representitive");
					return true;
				}
				theUser->setTotpKey(string(key));
				if(!theUser->commit(theClient)) {
					bot->Notice(theClient,"Failed to enable TOTP authentication, please contact a cservice representitive");
					free(key);
					return true;
				}

				bot->Notice(theClient,"TOTP key set.  Your base 32 encoded secret key is: %s",key);
				bot->Notice(theClient,"Your key in hex string: %s",hex_key);
				bot->Notice(theClient,"For QR representation of your key, visit : https://cservice.undernet.org/genqr.php?code=%s&name=UnderNet",key);
				bot->Notice(theClient,"Please note, this key will never be presented to you again.  NEVER GIVE YOUR KEY TO ANYONE!");			
				bot->Notice(theClient,"To confirm TOTP activation please configure your device with the above key and type  /msg %s set TOTP confirm <token>",bot->getNickName().c_str());
				free(key);
				return true;
			} else if(value == "CONFIRM") {
				 if(theUser->getFlag(sqlUser::F_TOTP_ENABLED)) {
                                        bot->Notice(theClient,"TOTP is already enabled for your account");
                                        return true;
                                }
				if(st.size() == 3) {
					bot->Notice(theClient,"Usage: /msg %s set totp CONFIRM <token>",bot->getNickName().c_str());
					return true;
				}
				if(theUser->getTotpKey() == "") {
					bot->Notice(theClient,"Before confirming TOTP it must be enabled using /msg %s set TOTP on",bot->getNickName().c_str());
					return true;
				}
				char* key;
			        size_t len;
			        int res  = oath_base32_decode(theUser->getTotpKey().c_str(),theUser->getTotpKey().size(),&key,&len);
			        if(res != OATH_OK) {
                			bot->Notice(theClient,"TOTP key validation failed due to an error, please contact CService represetitive");
                			elog << "ERROR while decoding base32 (" << st[st.size()-1].c_str() << ") " << oath_strerror(res) << "\n";
                			return false;
        			}
        			res=oath_totp_validate(key,len,time(NULL),30,0,1,st[3].c_str());
        			free(key);
			        if(res < 0 ) {
					bot->Notice(theClient,"TOTP validation failed, invalid key, please make sure youre device is configured properly with the correct key");
					return false;
				}
				if(st.size() ==4) {
					bot->Notice(theClient,"WARNING:  This will enable time-based OTP (one time passwords).  Once enabled, in order to login you will require a device to generate the OTP token which has the stored secret key.  If you are sure, type: /msg %s set totp CONFIRM <token> -force",bot->getNickName().c_str()); 
					return true;
				}
				if(string_upper(st[4]) == "-FORCE") {
				 	theUser->setFlag(sqlUser::F_TOTP_ENABLED);
                                        if(!theUser->commit(theClient)) {
                                                bot->Notice(theClient,"Failed to enable TOTP authentication, please contact a cservice representitive");
                                                return true;
                                        }
					bot->Notice(theClient,"TOTP Authentication is ENABLED");
					return true;
				} else {
					bot->Notice(theClient,"Invalid option %s",st[4].c_str());
					return true;
				}
			}
		} 
	}
#endif
*/

	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::invalid_option,
			string("Invalid option.")));
	return true;
}

Channel* tmpChan = Network->findChannel(st[1]);

/* Is the channel registered? */

sqlChannel* theChan = bot->getChannelRecord(st[1]);
if(!theChan)
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::chan_not_reg,
			string("Sorry, %s isn't registered with me.")).c_str(),
		st[1].c_str());
	return false;
	}

// Check level.

int level = bot->getEffectiveAccessLevel(theUser, theChan, false);
string option = string_upper(st[2]);
string value;
string reason;

if (st.size() < 4)
	{
	value = "";
	}
else
	{
 	value = string_upper(st[3]);
	if (st.size() < 5)
		reason = "";
	else
		reason = st.assemble(4);
	}

	/*
	 * Check the "Locked" status first, so admin's can bypas to turn it OFF :)
	 */

	if(option == "LOCKED")
	{
	    // Check for admin access
	    int admLevel = bot->getAdminAccessLevel(theUser);
	    if(admLevel < level::set::locked)
	    {
			// No need to tell users about admin commands.
			Usage(theClient);
			return true;
	    }
	    if(value == "ON") theChan->setFlag(sqlChannel::F_LOCKED);
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_LOCKED);
	    else
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	    }
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_LOCKED) ? "ON" : "OFF");

	    return true;
	}

	/*
	 *  Check if the channel is "Locked", if so, only allow admins to change settings.
	 */

	if(theChan->getFlag(sqlChannel::F_LOCKED))
	{
		int admLevel = bot->getAdminAccessLevel(theUser);
		if (admLevel < level::set::locked)
			{
			bot->Notice(theClient, "The channel settings for %s have been locked by a cservice"
				" administrator and cannot be changed.", theChan->getName().c_str());
			return(true);
			}
	}

	if(option == "CAUTION")
	{
    /* Check for admin access */
    if(bot->getAdminAccessLevel(theUser) < level::set::caution)
	{
	/* No need to tell users about admin commands. */
	Usage(theClient);
	return true;
	}

    if(value == "ON") theChan->setFlag(sqlChannel::F_CAUTION);
    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_CAUTION);
    else
	{
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::set_cmd_syntax_on_off,
			string("value of %s must be ON or OFF")).c_str(),
		option.c_str());
	return true;
	}

	theChan->commit();
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::set_cmd_status,
			string("%s for %s is %s")).c_str(),
	option.c_str(),
		theChan->getName().c_str(),
		theChan->getFlag(sqlChannel::F_CAUTION) ? "ON" : "OFF");
	return true;
	}

	if(option == "NOREG")
	{
	// Check for admin access
	int admLevel = bot->getAdminAccessLevel(theUser);
	if(admLevel < level::set::noreg)
		{
		// No need to tell users about admin commands.
		Usage(theClient);
		return true;
		}

	if(value == "ON") theChan->setFlag(sqlChannel::F_NOREG);
	else if(value == "OFF") theChan->removeFlag(sqlChannel::F_NOREG);
	else
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
		}
	theChan->commit();
	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::set_cmd_status,
			string("%s for %s is %s")).c_str(),
		option.c_str(),
		theChan->getName().c_str(),
		theChan->getFlag(sqlChannel::F_NOREG) ? "ON" : "OFF");
	return true;
	}

	if(option == "SPECIAL")
	{
	    // Check for admin access
	    int admLevel = bot->getAdminAccessLevel(theUser);
	    if(admLevel < level::set::special)
	    {
			// No need to tell users about admin commands.
			Usage(theClient);
			return true;
	    }
	    if(value == "ON") theChan->setFlag(sqlChannel::F_SPECIAL);
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_SPECIAL);
	    else
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	    }
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_SPECIAL) ? "ON" : "OFF");

	    return true;
	}


	if(option == "NEVERREG")
	{
	    // Check for admin access
	    int admLevel = bot->getAdminAccessLevel(theUser);
	    if(admLevel < level::set::neverreg)
	    {
			// No need to tell users about admin commands.
			Usage(theClient);
			return true;
	    }
	    if(value == "ON") theChan->setFlag(sqlChannel::F_NEVREG);
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_NEVREG);
	    else
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	    }
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_NEVREG) ? "ON" : "OFF");

	    return true;
	}

	if(option == "NOPURGE")
	{
	    // Check for admin access
	    int admLevel = bot->getAdminAccessLevel(theUser);
	    if(admLevel < level::set::nopurge)
	    {
			// No need to tell users about admin commands.
			Usage(theClient);
			return true;
	    }
	    if(value == "ON") theChan->setFlag(sqlChannel::F_NOPURGE);
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_NOPURGE);
	    else
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	    }
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_NOPURGE) ? "ON" : "OFF");
	    return true;
	}

	if(option == "SUSPEND")
	{
	    // Check for admin access
	    int admLevel = bot->getAdminAccessLevel(theUser);
	    if(admLevel < level::set::suspend)
	    {
			// No need to tell users about admin commands.
			Usage(theClient);
			return true;
	    }
            if (reason == "")
            {
                bot->Notice(theClient, "No reason given!  You must specify a reason after ON/OFF");
                return true;
            }
	    if(value == "ON") theChan->setFlag(sqlChannel::F_SUSPEND);
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_SUSPEND);
	    else
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	    }
	    theChan->commit();
            /* write a channel log entry */
            string logmsg;
            if (!theChan->getFlag(sqlChannel::F_SUSPEND))
                logmsg += "un";
            logmsg += "suspend reason: ";
            logmsg += reason;
            if (theChan->getFlag(sqlChannel::F_SUSPEND))
            {
                bot->writeChannelLog(theChan, theClient, sqlChannel::EV_SUSPEND, logmsg);
		/* inform admin channel */
		bot->logAdminMessage("%s (%s) has suspended %s",
			theClient->getNickName().c_str(), theUser->getUserName().c_str(),
			theChan->getName().c_str());
		if (tmpChan) bot->deopAllOnChan(tmpChan); // Deop everyone. :)
            } else {
                bot->writeChannelLog(theChan, theClient, sqlChannel::EV_UNSUSPEND, logmsg);
		/* inform admin channel */
		bot->logAdminMessage("%s (%s) has unsuspended %s",
			theClient->getNickName().c_str(), theUser->getUserName().c_str(),
			theChan->getName().c_str());
            }
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_SUSPEND) ? "ON" : "OFF");
	    return true;
	}

	if(option == "TEMPMAN")
	{
	    // Check for admin access
	    int admLevel = bot->getAdminAccessLevel(theUser);
	    if(admLevel < level::set::tempman)
	    {
			// No need to tell users about admin commands.
			Usage(theClient);
			return true;
	    }

	    if(value == "ON") theChan->setFlag(sqlChannel::F_TEMP);
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_TEMP);
	    else
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	    }
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_TEMP) ? "ON" : "OFF");
	    return true;
	}

	if(option == "VACATION")
	{
	    // Check for admin access
	    int admLevel = bot->getAdminAccessLevel(theUser);
	    if(admLevel < level::set::vacation)
	    {
			// No need to tell users about admin commands.
			Usage(theClient);
			return true;
	    }
	    if(value == "ON") theChan->setFlag(sqlChannel::F_VACATION);
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_VACATION);
	    else
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	    }
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_VACATION) ? "ON" : "OFF");
	    return true;
	}

	/*
	* Check the "NOFORCE" status first, so admin's can bypas to turn it OFF :)
	*/

	if (option == "NOFORCE")
	{
		// Check for admin access
		int admLevel = bot->getAdminAccessLevel(theUser);
		if (admLevel == 0) {
			// No need to tell users about admin commands.
			Usage(theClient);
			return true;
		}
		if (admLevel < level::set::noforce)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::insuf_access,
					string("Sorry, you have insufficient access to perform that command.")));
			return false;
		}
		if (value == "ON") theChan->setFlag(sqlChannel::F_NOFORCE);
		else if (value == "OFF") theChan->removeFlag(sqlChannel::F_NOFORCE);
		else
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::set_cmd_syntax_on_off,
					string("value of %s must be ON or OFF")).c_str(),
				option.c_str());
			return true;
		}
		theChan->commit();
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_NOFORCE) ? "ON" : "OFF");

		//TODO?
		//bot->writeChannelLog(theChan, theClient, sqlChannel::EV_NOFORCE, "");

		if (value == "ON") {
			for (sqlChannel::forceMapType::const_iterator ptr = theChan->forceMap.begin();
				ptr != theChan->forceMap.end(); ++ptr)
			{
				// Look up this username in the cache.
				sqlUser* AdminUser = bot->getUserRecord(ptr->second.second);
				int ForceLevel = ptr->second.first;

				//Now Remove force access who is not privileged :)
				if (ForceLevel < level::immune::noforce)
				{
					bot->noticeAllAuthedClients(AdminUser,
						bot->getResponse(AdminUser,
							language::set_cmd_status,
							string("%s for %s is %s")).c_str(),
						option.c_str(),
						theChan->getName().c_str(), "ON");

					theChan->forceMap.erase(AdminUser->getID());

					bot->noticeAllAuthedClients(AdminUser,
						bot->getResponse(AdminUser,
							language::rem_temp_access,
							string("Removed your temporary access of %i from channel %s")).c_str(),
						ForceLevel, theChan->getName().c_str());
				}
			} //for cycle
		} //if (value == ON)

		return true;
	}

    if(option == "MIA")
    {
        // Check for admin access
        int admLevel = bot->getAdminAccessLevel(theUser);
        if(admLevel < level::set::mia)
        {
                    // No need to tell users about admin commands.
                    Usage(theClient);
                    return true;
        }
        if(value == "ON") theChan->setFlag(sqlChannel::F_MIA);
        else if(value == "OFF") theChan->removeFlag(sqlChannel::F_MIA);
        else
        {
            bot->Notice(theClient,
                    bot->getResponse(theUser,
                            language::set_cmd_syntax_on_off,
                            string("value of %s must be ON or OFF")).c_str(),
                    option.c_str());
            return true;
        }
        theChan->commit();
        bot->Notice(theClient,
                    bot->getResponse(theUser,
                            language::set_cmd_status,
                            string("%s for %s is %s")).c_str(),
                    option.c_str(),
                    theChan->getName().c_str(),
                    theChan->getFlag(sqlChannel::F_MIA) ? "ON" : "OFF");
        return true;
    }

	if (option == "OPLOG")
	{
		if (level < level::set::oplog)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::insuf_access,
					string("You do not have enough access!")));
			return true;
		}
		if (value == "ON") theChan->setFlag(sqlChannel::F_OPLOG);
		else if (value == "OFF") theChan->removeFlag(sqlChannel::F_OPLOG);
		else
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::set_cmd_syntax_on_off,
					string("value of %s must be ON or OFF")).c_str(),
				option.c_str());
			return true;
		}
		theChan->commit();
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_OPLOG) ? "ON" : "OFF");
		return true;
	}

	if(option == "NOOP")
	{
	    if(level < level::set::noop)
	    {
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::insuf_access,
					string("You do not have enough access!")));
			return true;
	    }
	    if(value == "ON")
	    {
			theChan->setFlag(sqlChannel::F_NOOP);
			if (tmpChan) bot->deopAllOnChan(tmpChan); // Deop everyone. :)
	    }
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_NOOP);
	    else
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	    }
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_NOOP) ? "ON" : "OFF");
	    return true;
	}
#ifdef USE_NOVOICE
	if (option == "NOVOICE")
	{
	    if(level < level::set::novoice)
	    {
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::insuf_access,
					string("You do not have enough access!")));
			return true;
	    }
	    if(value == "ON")
	    {
			theChan->setFlag(sqlChannel::F_NOVOICE);
			if (tmpChan) bot->deVoiceAllOnChan(tmpChan); // DeVoice everyone. :)
	    }
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_NOVOICE);
	    else
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	    }
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_NOVOICE) ? "ON" : "OFF");
	    return true;
	}
#endif
	if(option == "STRICTOP")
	{
	    if(level < level::set::strictop)
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::insuf_access,
				string("You do not have enough access!")));
		return true;
	    }
	    if(value == "ON")
	    {
	    	theChan->setFlag(sqlChannel::F_STRICTOP);
			if (tmpChan) bot->deopAllUnAuthedOnChan(tmpChan);
		}
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_STRICTOP);
	    else
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	    }
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_STRICTOP) ? "ON" : "OFF");
	    return true;
	}
#ifdef USE_NOTAKE
	if (option == "NOTAKE")
	{
		if(level < level::set::notake)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
						language::insuf_access,
				string("You do not have enough access!")));
			return true;
		}
		if(value == "ON")
		{
			theChan->setFlag(sqlChannel::F_NOTAKE);
			theChan->setNoTake(1); //default revenge is IGNORE
		}
		else if(value == "OFF") theChan->removeFlag(sqlChannel::F_NOTAKE);
		else
		{
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
					string("value of %s must be ON or OFF")).c_str(),
				option.c_str());
			return true;
		}
		theChan->commit();
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_NOTAKE) ? "ON" : "OFF");
		return true;
	}
	if (option == "TAKEREVENGE")
	{
		if(level < level::set::notake)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
						language::insuf_access,
						string("You do not have enough access!")));
			return false;
		}
		int setting;
		if (value != "")
		{
			if (!IsNumeric(value))
			{
				if (value=="NONE")
					setting = 1;
				else if (value=="BAN")
					setting = 2;
				else if (value=="SUSPEND")
					setting = 3;
				else
					setting = 4;		/* dummy value to cause failure */
			} else {
				setting = atoi(value.c_str());
			}
			if ((setting < 1) || (setting > 3))
			{
				bot->Notice(theClient, "Invalid TAKEREVENGE setting. Correct values are NONE, BAN or SUSPEND.");
				return false;
			}
			theChan->setNoTake(setting);
			theChan->commit();
		} else {
			setting = theChan->getNoTake();
		}
		/* set value to textual description */
		switch (setting) {
			default:	break;
			case 1:		value = "NONE";	break;
			case 2:		value = "BAN";		break;
			case 3:		value = "SUSPEND";	break;
		}
		bot->Notice(theClient,"TAKEREVENGE for %s is %s",
			theChan->getName().c_str(), value.c_str());
		return true;
	}
#endif
#ifdef USE_FLOODPRO
	if (option == "FLOODPRO")
	{
	    if(level < level::set::floodpro)
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::insuf_access,
				string("You do not have enough access!")));
		return true;
	    }
		sqlChannel::FloodProLevel prevFN = theChan->getFloodproLevel();
	    if ((value == "ON") || (value == "KICK"))
	    {
	    	theChan->setFlag(sqlChannel::F_FLOODPRO);
	    	if (!theChan->getFloodPro())
	    		theChan->setDefaultFloodproValues();
	    	theChan->setFloodproLevel(sqlChannel::FLOODPRO_KICK);
	    	theChan->setManualFloodproLevel(sqlChannel::FLOODPRO_KICK);
	    }
	    else if (value == "OFF")
	    {
	    	theChan->setFloodproLevel(sqlChannel::FLOODPRO_NONE);
	    	//theChan->setFloodPro(0);
	    	theChan->removeFlag(sqlChannel::F_FLOODPRO);
	    	theChan->setFloodproLevel(sqlChannel::FLOODPRO_NONE);
			theChan->setManualFloodproLevel(sqlChannel::FLOODPRO_NONE);
	    }
		else if (value == "BAN")
		{
			theChan->setFlag(sqlChannel::F_FLOODPRO);
			if (!theChan->getFloodPro())
				theChan->setDefaultFloodproValues();
			theChan->setFloodproLevel(sqlChannel::FLOODPRO_BAN);
			theChan->setManualFloodproLevel(sqlChannel::FLOODPRO_BAN);
		}
	    else if ((value == "DEFAULT") || (value == "DEFAULTS"))
	    {
	    	theChan->setDefaultFloodproValues();
	    	theChan->setFlag(sqlChannel::F_FLOODPRO);
			theChan->setFloodproLevel(sqlChannel::FLOODPRO_KICK);
			theChan->setManualFloodproLevel(sqlChannel::FLOODPRO_KICK);
	    	bot->Notice(theClient, "Default FLOODPRO values have been set for channel %s", theChan->getName().c_str());
	    }
	    else
	    {
	    	bot->Notice(theClient, "Value of %s must be ON (KICK), BAN, DEFAULT, or OFF", option.c_str());
	    	return true;
	    }

	    theChan->commit();
		bot->Notice(theClient, "%s punishment level on %s is %s",
				option.c_str(), theChan->getName().c_str(), sqlChannel::getFloodLevelName(theChan->getFloodproLevel()).c_str());
		if (theChan->getFloodproLevel() > prevFN)
		{
			if (tmpChan)
			{
				bot->NoticeChannelOps(tmpChan, "Increased %s punishment level on %s to %s by %s",
				option.c_str(),
				theChan->getName().c_str(),
				value.c_str(),
				theUser->getUserName().c_str());
			}
		}
		else if (theChan->getFloodproLevel() < prevFN)
		{
			if (tmpChan)
			{
				bot->NoticeChannelOps(tmpChan, "Decreased %s punishment level on %s to %s by %s",
					option.c_str(),
					theChan->getName().c_str(),
					value.c_str(),
					theUser->getUserName().c_str());
			}
		}
	    return true;
	}
	if (option == "FLOODPROGLINE")
	{
		int admLevel = (int)bot->getAdminAccessLevel(theUser);
		if ((admLevel < level::set::floodpro_gline) && (level < level::set::floodpro_gline))
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::insuf_access,
				string("You do not have enough access!")));
			return true;
		}
		sqlChannel::FloodProLevel prevFN = theChan->getFloodproLevel();
		if (value == "ON")
		{
			theChan->setFlag(sqlChannel::F_FLOODPROGLINE);
			if (!theChan->getFlag(sqlChannel::F_FLOODPRO))
			{
				theChan->setFlag(sqlChannel::F_FLOODPRO);
				if (!theChan->getFloodPro())
					theChan->setDefaultFloodproValues();
			}
		}
		else if (value == "OFF")
			theChan->removeFlag(sqlChannel::F_FLOODPROGLINE);
	    else
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("Value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	    }

		theChan->commit();
		bot->Notice(theClient, "%s punishment level on %s is %s",
			option.c_str(),	theChan->getName().c_str(),	value.c_str());
		if (theChan->getFloodproLevel() > prevFN)
		{
			if (tmpChan)
			{
				bot->NoticeChannelOps(tmpChan, "Increased %s punishment level on %s to %s by %s",
					option.c_str(),
					theChan->getName().c_str(),
					value.c_str(),
					theUser->getUserName().c_str());
			}
		}
		else if (theChan->getFloodproLevel() < prevFN)
		{
			if (tmpChan)
			{
				bot->NoticeChannelOps(tmpChan, "Decreased %s punishment level on %s to %s by %s",
					option.c_str(),
					theChan->getName().c_str(),
					value.c_str(),
					theUser->getUserName().c_str());
			}
		}

	    return true;
	}
	if ((option == "MSGFLOOD") || (option == "MESSAGEFLOOD") || (option == "PRIVMSGFLOOD"))
	{
	    if(level < level::set::floodpro)
	    {
	    	bot->Notice(theClient,
	    			bot->getResponse(theUser,
	    					language::insuf_access,
	    					string("You do not have enough access!")));
	    	return true;
	    }
	    if (!IsNumeric(value) || value.empty())
	    {
			bot->Notice(theClient, "Value of MESSAGEFLOOD must be 0 or 2-255");
			return true;
	    }
	    int numValue = atoi(value.c_str());
	    if(numValue > 255 || numValue < 0)
	    {
			bot->Notice(theClient, "Value of MESSAGEFLOOD must be 0 or 2-255");
			return true;
	    }
	    if (numValue == 1)
	    {
			bot->Notice(theClient, "Value of MESSAGEFLOOD cannot be 1");
			return true;
	    }
		theChan->setFloodMsg(numValue);
	    theChan->commit();
	    bot->Notice(theClient,"MESSAGEFLOOD for %s is set to %d",
			theChan->getName().c_str(), numValue);
	    return true;
	}
	if (option == "NOTICEFLOOD")
	{
	    if(level < level::set::floodpro)
	    {
	    	bot->Notice(theClient,
	    			bot->getResponse(theUser,
	    					language::insuf_access,
	    					string("You do not have enough access!")));
	    	return true;
	    }
	    if (!IsNumeric(value) || value.empty())
	    {
			bot->Notice(theClient, "Value of NOTICEFLOOD has to be 0-15");
			return true;
	    }
	    int numValue = atoi(value.c_str());
	    if (numValue > 15 || numValue < 0)
	    {
			bot->Notice(theClient, "Value of NOTICEFLOOD has to be 0-15");
			return true;
	    }
		theChan->setFloodNotice(numValue);
	    theChan->commit();
	    bot->Notice(theClient,"NOTICEFLOOD for %s is set to %d",
			theChan->getName().c_str(), numValue);
	    return true;
	}
	if (option == "CTCPFLOOD")
	{
	    if(level < level::set::floodpro)
	    {
	    	bot->Notice(theClient,
	    			bot->getResponse(theUser,
	    					language::insuf_access,
	    					string("You do not have enough access!")));
	    	return true;
	    }
	    if (!IsNumeric(value) || value.empty())
	    {
			bot->Notice(theClient, "Value of CTCPFLOOD has to be 0-15");
			return true;
	    }
	    int numValue = atoi(value.c_str());
	    if (numValue > 15 || numValue < 0)
	    {
			bot->Notice(theClient, "Value of CTCPFLOOD has to be 0-15");
			return true;
	    }
		theChan->setFloodCTCP(numValue);
	    theChan->commit();
	    bot->Notice(theClient,"CTCPFLOOD for %s is set to %d",
			theChan->getName().c_str(), numValue);
	    return true;
	}
	if ((option == "FLOODPERIOD") || (option == "FLOODPRD"))
	{
        if (level < level::set::floodpro)
	    {
	    	bot->Notice(theClient,
	    			bot->getResponse(theUser,
	    					language::insuf_access,
	    					string("You do not have enough access!")));
	    	return true;
	    }
	    if (!IsNumeric(value) || value.empty())
	    {
			bot->Notice(theClient, "Value of FLOODPERIOD has to be 0-15");
			return true;
	    }
	    int numValue = atoi(value.c_str());
        if (numValue > 15 || numValue < 0)
	    {
			bot->Notice(theClient, "Value of FLOODPERIOD has to be 0-15");
			return true;
	    }
		theChan->setFloodPeriod(numValue);
	    theChan->commit();
	    bot->Notice(theClient,"FLOODPERIOD for %s is set to %d",
			theChan->getName().c_str(), numValue);
	    if (numValue == 0)
	    {
	    	theChan->removeFlag(sqlChannel::F_FLOODPRO);
	    	theChan->setFloodproLevel(sqlChannel::FLOODPRO_NONE);
	    	theChan->commit();
	    	bot->Notice(theClient,
	    			bot->getResponse(theUser,
	    				language::set_cmd_status,
	    				string("%s for %s is %s")).c_str(),
	    			"FLOODPRO",
	    			theChan->getName().c_str(),
	    			theChan->getFlag(sqlChannel::F_FLOODPRO) ? "ON" : "OFF");
	    }
	    return true;
	}
    if ((option == "REPEATFLOOD") || (option == "REPFLOOD"))
	{
        if (level < level::set::floodpro)
	    {
	    	bot->Notice(theClient,
	    			bot->getResponse(theUser,
	    					language::insuf_access,
	    					string("You do not have enough access!")));
	    	return true;
	    }
        if (!IsNumeric(value) || value.empty())
        {
            bot->Notice(theClient, "Value of REPEATFLOOD must be be 0 or 2-15");
            return true;
        }
        int numValue = atoi(value.c_str());
        if (numValue > 15 || numValue < 0)
        {
            bot->Notice(theClient, "Value of REPEATFLOOD must be be 0 or 2-15");
            return true;
        }
	    if (numValue == 1)
	    {
			bot->Notice(theClient, "Value of REPEATFLOOD cannot be 1");
			return true;
	    }
		theChan->setRepeatCount(numValue);
		theChan->commit();
        bot->Notice(theClient,"REPEATFLOOD for %s is %s",
			theChan->getName().c_str(), value.c_str());
	    return true;
	}
#endif
	if(option == "AUTOTOPIC")
	{
	    if(level < level::set::autotopic)
	    {
		bot->Notice(theClient,
				bot->getResponse(theUser,
				language::insuf_access,
				string("You do not have enough access!")));
		return true;
	    }
	    if(value == "ON")
	    {
	    	theChan->setFlag(sqlChannel::F_AUTOTOPIC);
			bot->doAutoTopic(theChan);
		}
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_AUTOTOPIC);
	    else
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	    }
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_AUTOTOPIC) ? "ON" : "OFF");
	    return true;
	}

	if(option == "AUTOJOIN")
	{
	    if (level < level::set::autojoin)
	    {
	    	bot->Notice(theClient,
	    		bot->getResponse(theUser,
	    			language::insuf_access,
					string("You do not have enough access!")));

	    	return true;
	    }
	    if (value == "ON")
	    {
	    	theChan->setFlag(sqlChannel::F_AUTOJOIN);
			theChan->setInChan(true);
			bot->Join(theChan->getName(), "+R",
				theChan->getChannelTS(), true);
			bot->incrementJoinCount();
		    bot->Notice(theClient,
				bot->getResponse(theUser,
					language::set_cmd_status,
					string("%s for %s is %s")).c_str(),
				option.c_str(),
				theChan->getName().c_str(),
				theChan->getFlag(sqlChannel::F_AUTOJOIN) ? "ON" : "OFF");
		}
	    else if (value == "OFF")
	    {
	    	theChan->removeFlag(sqlChannel::F_AUTOJOIN);
		    bot->Notice(theClient,
				bot->getResponse(theUser,
					language::set_cmd_status,
					string("%s for %s is %s")).c_str(),
				option.c_str(),
				theChan->getName().c_str(),
				theChan->getFlag(sqlChannel::F_AUTOJOIN) ? "ON" : "OFF");
			theChan->setInChan(false);
			bot->decrementJoinCount();
			bot->Part(theChan->getName());
		}
	    else
	    {
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::set_cmd_syntax_on_off,
					string("value of %s must be ON or OFF")).c_str(),
				option.c_str());
			return true;
	    }
	    theChan->commit();
	    return true;
	}

	if(option == "USERFLAGS")
	{
	    if(level < level::set::userflag)
	    {
                bot->Notice(theClient,
                                bot->getResponse(theUser,
                                language::insuf_access,
                                string("You do not have enough access!")));
		return false;
	    }

		int setting;
		if (value != "")
		{
			if (!IsNumeric(value))
			{
				if (value=="NONE")
					setting = 0;
				else if (value=="OP")
					setting = 1;
				else if (value=="VOICE")
					setting = 2;
				else
					setting = 3;		/* dummy value to cause failure */
			} else {
				setting = atoi(value.c_str());
			}
			if ( (setting < 0) || (setting > 2))
			{
				bot->Notice(theClient,
					bot->getResponse(theUser,
						language::userflags_syntax,
						string("Invalid USERFLAGS setting. Correct values are NONE, OP or VOICE.")));
				return false;
			}

			theChan->setUserFlags(setting);
			theChan->commit();
		} else {
			setting = theChan->getUserFlags();
		}
		/* set value to textual description */
		switch (setting) {
			default:	break;
			case 0:		value = "NONE";		break;
			case 1:		value = "OP";		break;
			case 2:		value = "VOICE";	break;
		}
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::userflags_status,
				string("USERFLAGS for %s is %s")).c_str(),
			theChan->getName().c_str(), value.c_str());
	    return true;
	}

	if(option == "MASSDEOPPRO")
	{
	    if(level < level::set::massdeoppro)
	    {
	                bot->Notice(theClient,
        	                        bot->getResponse(theUser,
                	                language::insuf_access,
                        	        string("You do not have enough access!")));
			return true;
	    }
	    // Temporary MASSDEOPPRO range! 0-7.. is this correct?
	    if(!IsNumeric(value))
	    {
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::massdeoppro_syntax,
					string("value of MASSDEOPPRO has to be 0-7")));
			return true;
	    }
	    int numValue = atoi(value.c_str());
	    if(numValue > 7 || numValue < 0)
	    {
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::massdeoppro_syntax,
					string("value of MASSDEOPPRO has to be 0-7")));
			return true;
	    }
		theChan->setMassDeopPro(numValue);
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::massdeoppro_status,
				string("MASSDEOPPRO for %s is set to %d")).c_str(),
			theChan->getName().c_str(), numValue);
	    return true;
	}

	if(option == "DESCRIPTION" || option == "DESC")
	{
		string desc = st.assemble(3);
	    if(level < level::set::desc)
	    {
                bot->Notice(theClient,
                                  bot->getResponse(theUser,
                                  language::insuf_access,
                                  string("You do not have enough access!")));
		return true;
	    }
	    if (strlen(desc.c_str()) > 380)
	    {
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::desc_max_len,
					string("The DESCRIPTION can be a maximum of %i chars!")).c_str(),380);
			return true;
	    }
		theChan->setDescription(desc);
	    theChan->commit();

		if(desc == "")
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::desc_cleared,
					string("DESCRIPTION for %s is cleared.")).c_str(),
				theChan->getName().c_str());
		} else
		{
	    	bot->Notice(theClient,
			bot->getResponse(theUser,
				language::desc_status,
				string("DESCRIPTION for %s is: %s")).c_str(),
			theChan->getName().c_str(),
			desc.c_str());
		}

		if (theChan->getFlag(sqlChannel::F_AUTOTOPIC))
		{
			bot->doAutoTopic(theChan);
		}

	    return true;
	}

	if(option == "URL")
	{
		string url = st.assemble(3);
	    if(level < level::set::url)
	    {
                        bot->Notice(theClient,
                                        bot->getResponse(theUser,
                                        language::insuf_access,
                                        string("You do not have enough access!")));
			return true;
	    }
	    if (strlen(url.c_str()) > 128) // URL max now 128
	    {
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::url_max_len,
					string("The URL can be a maximum of %i chars!")).c_str(),128);
			return true;
	    }
		theChan->setURL(url);
	    theChan->commit();

		if(url == "")
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::url_cleared,
					string("URL for %s is cleared.")).c_str(),
				theChan->getName().c_str());
		} else
		{
	    	bot->Notice(theClient,
			bot->getResponse(theUser,
				language::url_status,
				string("URL for %s is: %s")).c_str(),
			theChan->getName().c_str(),
			url.c_str());
		}

		if (theChan->getFlag(sqlChannel::F_AUTOTOPIC))
		{
			bot->doAutoTopic(theChan);
		}

	    return true;
	}

	if(option == "KEYWORDS")
	{
	    /* Keywords are being processed as a long string. */
	    string keywords = st.assemble(3);
	    if(level < level::set::keywords)
	    {
                bot->Notice(theClient,
                               bot->getResponse(theUser,
                               language::insuf_access,
                               string("You do not have enough access!")));
		return true;
	    }
	    if(strlen(keywords.c_str()) > 300) // keywords now 300
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::keywords_max_len,
				string("The string of keywords cannot exceed %i chars!")).c_str(), 300);
		return true;
	    }
	    theChan->setKeywords(keywords);
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::keywords_status,
				string("KEYWORDS for %s are: %s")).c_str(),
			theChan->getName().c_str(),
			keywords.c_str());
	    return true;
	}

#ifdef USE_WELCOME
	if (option == "WELCOME")
	{
		string welcome;
		if (st.size() > 3)
			welcome = st.assemble(3);
		if (level < level::set::welcome)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::insuf_access,
					string("You do not have enough access!")));
			return true;
		}
		if (strlen(welcome.c_str()) > 300)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::welcome_max_len,
					string("The WELCOME can be a maximum of 300 chars!")));
			return true;
		}

		if ((string_upper(welcome) == "OFF") || (welcome == ""))
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::welcome_cleared,
					string("WELCOME for %s is cleared.")).c_str(),
				theChan->getName().c_str());
			theChan->setWelcome("");
			theChan->commit();
		}
		else
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::welcome_status,
					string("WELCOME for %s is: %s")).c_str(),
				theChan->getName().c_str(),
				welcome.c_str());
			theChan->setWelcome(welcome);
			theChan->commit();
		}
		return true;
	}
#endif

	if(option == "MODE")
	{
	    if(level < level::set::mode)
	    {
                bot->Notice(theClient,
                               bot->getResponse(theUser,
                               language::insuf_access,
                               string("You do not have enough access!")));
		return true;
	    }
		if (!tmpChan)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::no_such_chan,
					string("Can't locate channel %s on the network!")).c_str(),
				st[1].c_str());
			return false;
		}

		/* If FLOATLIM is activated, we don't store the +l. */
		string tempModes = tmpChan->getModeString();
		if (theChan->getFlag(sqlChannel::F_FLOATLIM))
			stripModes(tempModes, "l");

	    theChan->setChannelMode(tempModes);
	    theChan->commit();

	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getChannelMode().c_str());
	    return true;
	}


	if(option == "COMMENT")
	{
	    /* Check for admin access */
	    int admLevel = bot->getAdminAccessLevel(theUser);
	    if(admLevel < level::set::comment)
			{
			/* No need to tell users about admin commands. */
			Usage(theClient);
			return true;
			}

		string comment = st.assemble(3);

		if(comment.size() > 200)
	    {
			bot->Notice(theClient, "The COMMENT can be a maximum of 200 chars!");
			return true;
	    }

		theChan->setComment(comment);
	    theChan->commit();

		if(comment.empty())
		{
			bot->Notice(theClient, "COMMENT for %s is cleared.",
				theChan->getName().c_str());
		} else
		{
	    	bot->Notice(theClient, "COMMENT for %s is: %s",
				theChan->getName().c_str(), comment.c_str());
		}

	    return true;
	}

	// ********* JOIN LIMITATION - prevents unidented and unauthed users from sneaking into the channel in preparation for a flood **********
	if (option == "JOINLIM")
	{
		if (level < level::set::joinlim)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::insuf_access,
					string("You do not have enough access!")));
			return true;
		}

		if (value == "ON")
		{
			theChan->setFlag(sqlChannel::F_JOINLIM);
			theChan->setLimitJoinTime(0);
			theChan->setLimitJoinCount(0);
		}
		else if (value == "OFF")
		{
			theChan->removeFlag(sqlChannel::F_JOINLIM);
			if (theChan->getLimitJoinActive())
			{
				bot->stopTimer(theChan->getLimitJoinTimer());
				bot->undoJoinLimits(theChan);
			}
		}
		else
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::set_cmd_syntax_on_off,
					string("value of %s must be ON or OFF")).c_str(),
					option.c_str());
			return true;
		}
		theChan->commit();

		// Check for default
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
				option.c_str(),
				theChan->getName().c_str(),
				theChan->getFlag(sqlChannel::F_JOINLIM) ? "ON" : "OFF");
		return true;
	}

	if (option == "JOINMAX")
	{
		if (level < level::set::joinlim)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::insuf_access,
					string("You do not have enough access!")));
			return true;
		}

		std::istringstream ss(value.c_str());

		int limit_joins = -1, limit_secs;
		char colon;

		// Extract numbers based on colon if present
		if (ss >> limit_joins >> colon >> limit_secs && colon == ':')
		{
			if (limit_joins < (int)bot->getConflimitJoinMaxLowest() || limit_joins > (int)bot->getConflimitJoinMaxHighest())
			{
				bot->Notice(theClient,
					bot->getResponse(theUser,
						language::set_joinmax_joinrange,
						string("Value of joins has to be %i-%i")).c_str(),
						(int)bot->getConflimitJoinMaxLowest(), (int)bot->getConflimitJoinMaxHighest());
				return true;
			}

			if (limit_secs < (int)bot->getConflimitJoinSecsLowest() || limit_secs > (int)bot->getConflimitJoinSecsHighest())
			{
				bot->Notice(theClient,
					bot->getResponse(theUser,
						language::set_joinmax_secrange,
						string("Value of seconds has to be %i-%i")).c_str(),
						(int)bot->getConflimitJoinSecsLowest(), (int)bot->getConflimitJoinSecsHighest());
				return true;
			}

			theChan->setLimitJoinMax(limit_joins);
			theChan->setLimitJoinSecs(limit_secs);
			theChan->setLimitJoinTime(0);
			theChan->setLimitJoinCount(0);
			theChan->commit();

			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::set_joinmax,
					string("JOINMAX for %s is now %i:%i (JOINS:SECS)")).c_str(),
					theChan->getName().c_str(), limit_joins, limit_secs);

			return true;
		}
		else
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::syntax,
					string("SYNTAX: %s")).c_str(),
					string("SET <#channel> JOINMAX <joins:secs>").c_str());
			return false;
		}
		return true;
	}

	if (option == "JOINMODE")
	{
		if (level < level::set::joinlim)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::insuf_access,
					string("You do not have enough access!")));
			return true;
		}

		if (st.size() < 4)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::syntax,
					string("SYNTAX: %s")).c_str(),
					string("SET <#channel> JOINMODE <modes> [parameters]").c_str());
			return false;
		}

		std::string allowedModes = bot->getConflimitJoinAllowedModes();
		std::string resultModes = "";

		StringTokenizer::size_type numParam = 0;
		bool invalidMode = false;
		bool invalidParam = false;

		for (char c : st[3])
		{
			if (c == '+')
				continue;

			if (allowedModes.find(c) != std::string::npos)
			{
				if (c == 'k')
					numParam++; // Ignore this parm
				if (c == 'b')
				{
					numParam++;
					if (st.size() < numParam + 4)
					{
						invalidParam = true;
						break;
					}
					std::string banMask = fixAddress(st[numParam + 3]);
					if (!validUserMask(banMask))
					{
						bot->Notice(theClient,
							bot->getResponse(theUser,
								language::ban_invalid,
								string("The banmask '%s' is not valid")).c_str(),
								st[numParam + 3].c_str());
						return true;
					}
				}
				resultModes += c;
			}
			else
			{
				invalidMode = true;
				break;
			}
		}

		if (invalidMode || resultModes.empty())
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::set_joinmode_allowed,
					string("The allowed JOINMODE modes are: %s")).c_str(),
					bot->getConflimitJoinAllowedModes().c_str());
			return true;
		}

		if (invalidParam || numParam != (st.size() - 4))
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::set_joinmode_params,
					string("Invalid number of JOINMODE parameters: %i modes and %i parameters provided")).c_str(),
					numParam, st.size() - 4);
			return true;
		}

		std::string modeString = "+";
		modeString += resultModes;
		if (numParam > 0)
		{
			modeString += " ";
			for (int i = 0; i < (int)numParam; i++)
			{
				modeString += st[i + 4];
				if (i < (int)(numParam - 1))
					modeString += " ";
			}
		}
		theChan->setLimitJoinMode(modeString);
		theChan->commit();

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_joinmode,
				string("JOINMODE for %s is now: %s")).c_str(),
				theChan->getName().c_str(), modeString.c_str());
		return true;
	}

	if (option == "JOINPERIOD")
	{
		if (level < level::set::joinlim)
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::insuf_access,
					string("You do not have enough access!")));
			return true;
		}

		unsigned int limit_period = atoi(value.c_str());
		if ((limit_period < 1) || (limit_period > bot->getConflimitJoinPeriodHighest()))
		{
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::set_joinperiod_range,
					string("Value of JOINPERIOD has to be 1-%i")).c_str(),
					(int) bot->getConflimitJoinPeriodHighest());
			return true;
		}

		/* Is JOINPERIOD changes while an active timer is running? */
		if (theChan->getLimitJoinActive())
		{
			time_t lapTime = ::time(nullptr) - theChan->getLimitJoinTime();
			time_t expTime = ::time(nullptr) - lapTime + limit_period;

			/**
			 * If the time lapsed under the existing timer is more than
			 * the new limit_period, kill the timer and undo modes.
			 */
			if (lapTime >= limit_period)
			{
				bot->stopTimer(theChan->getLimitJoinTimer());
				bot->undoJoinLimits(theChan);
				theChan->setLimitJoinTimer(0);
				theChan->setLimitJoinTimeExpire(0);
			}

			/**
			 * if the time lapsed under the existing timer is less than
			 * the new limit_period, reset timer to new expire time.
			 */
			else if (lapTime < limit_period)
			{
				bot->stopTimer(theChan->getLimitJoinTimer());
				xServer::timerID newTimer = this->server->RegisterTimer(expTime, bot, theChan);
				theChan->setLimitJoinTimer(newTimer);
				theChan->setLimitJoinTimeExpire(expTime);
			}
		}

		theChan->setLimitJoinPeriod(limit_period);
		theChan->commit();

		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_joinperiod,
				string("JOINPERIOD for %s is now: %i seconds")).c_str(),
				theChan->getName().c_str(), limit_period);
		return true;
	}

	// ********** FLOAT LIMITATION - prevents hordes of clones to rapidly join the channel **********
	if(option == "FLOATLIM")
	{
	    if(level < level::set::floatlim)
	    {
		bot->Notice(theClient,
				bot->getResponse(theUser,
				language::insuf_access,
				string("You do not have enough access!")));
		return true;
	    }
	    if(value == "ON")
	    {
	    	theChan->setFlag(sqlChannel::F_FLOATLIM);
		}
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_FLOATLIM);
	    else
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_syntax_on_off,
				string("value of %s must be ON or OFF")).c_str(),
			option.c_str());
		return true;
	    }
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_FLOATLIM) ? "ON" : "OFF");
	    return true;
	}

	if(option == "FLOATMARGIN")
	{
	    if(level < level::set::floatlim)
	    {
		bot->Notice(theClient,
				bot->getResponse(theUser,
				language::insuf_access,
				string("You do not have enough access!")));
		return true;
	    }

	unsigned int limit_offset = atoi(value.c_str());

	if ((limit_offset <= 1) | (limit_offset > 20))
		{
			bot->Notice(theClient, "Invalid floating-limit Margin (2-20 Allowed).");
			return true;
		}

	if (limit_offset <= theChan->getLimitGrace())
		{
			bot->Notice(theClient, "FLOATMARGIN cannot be less than or equal to FLOATGRACE.");
			return true;
		}

	theChan->setLimitOffset(limit_offset);
	theChan->commit();

	bot->Notice(theClient, "Floating-limit Margin now set to %i", limit_offset);
	return true;
	}

	if(option == "FLOATPERIOD")
	{
	    if(level < level::set::floatlim)
	    {
		bot->Notice(theClient,
				bot->getResponse(theUser,
				language::insuf_access,
				string("You do not have enough access!")));
		return true;
	    }

	unsigned int limit_period = atoi(value.c_str());

	if ((limit_period < 20) | (limit_period > 200))
		{
			bot->Notice(theClient, "Invalid floating-limit period (20-200 Allowed).");
			return true;
		}

	theChan->setLimitPeriod(limit_period);
	theChan->commit();

	bot->Notice(theClient, "Floating-limit period now set to %i", limit_period);
	return true;
	}

	if(option == "FLOATGRACE")
	{
	    if(level < level::set::floatlim)
	    {
		bot->Notice(theClient,
				bot->getResponse(theUser,
				language::insuf_access,
				string("You do not have enough access!")));
		return true;
	    }

	unsigned int limit_grace = atoi(value.c_str());

	if (limit_grace > 19)
		{
			bot->Notice(theClient, "Invalid floating-grace setting (0-19 Allowed).");
			return true;
		}

	if (limit_grace > theChan->getLimitOffset())
		{
			bot->Notice(theClient, "FLOATGRACE cannot be greater than FLOATMARGIN.");
			return true;
		}

	theChan->setLimitGrace(limit_grace);
	theChan->commit();

	bot->Notice(theClient, "Floating-limit grace now set to %i", limit_grace);
	return true;
	}

	if(option == "FLOATMAX")
	{
	    if(level < level::set::floatlim)
	    {
		bot->Notice(theClient,
				bot->getResponse(theUser,
				language::insuf_access,
				string("You do not have enough access!")));
		return true;
	    }

	unsigned int limit_max = atoi(value.c_str());

	if (limit_max > 65536)
		{
			bot->Notice(theClient, "Invalid floating-limit max (0-65536 Allowed).");
			return true;
		}


	theChan->setLimitMax(limit_max);
	theChan->commit();

	if (!limit_max)
	{
		bot->Notice(theClient, "Floating-limit MAX setting has now been disabled.");
	} else {
		bot->Notice(theClient, "Floating-limit max now set to %i", limit_max);
	}
	return true;
	}

	bot->Notice(theClient,
		bot->getResponse(theUser,
			language::mode_invalid,
			string("ERROR: Invalid channel setting.")));
	return true ;
}

} // namespace gnuworld.
