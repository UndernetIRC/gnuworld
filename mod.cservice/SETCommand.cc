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

#include	"StringTokenizer.h"
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"
#include	"cservice_config.h"

const char SETCommand_cc_rcsId[] = "$Id: SETCommand.cc,v 1.64 2008/04/16 20:34:44 danielaustin Exp $" ;

namespace gnuworld
{
using namespace level;

bool SETCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.SET");

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
	    if(level < level::set::autojoin)
	    {
                bot->Notice(theClient,
                                bot->getResponse(theUser,
                                language::insuf_access,
                                string("You do not have enough access!")));

		return true;
	    }
	    if(value == "ON")
	    {
	    	theChan->setFlag(sqlChannel::F_AUTOJOIN);
			theChan->setInChan(true);
			bot->Join(theChan->getName(), theChan->getChannelMode(),
				theChan->getChannelTS(), false);
			bot->joinCount++;
			bot->reopQ.insert(cservice::reopQType::value_type(theChan->getName(), bot->currentTime() + 15) );
		if (tmpChan)
			{
			if(theChan->getFlag(sqlChannel::F_NOOP)) bot->deopAllOnChan(tmpChan);
			if(theChan->getFlag(sqlChannel::F_STRICTOP)) bot->deopAllUnAuthedOnChan(tmpChan);
			}
		}
	    else if(value == "OFF")
	    {
	    	theChan->removeFlag(sqlChannel::F_AUTOJOIN);
			theChan->setInChan(false);
			bot->joinCount--;
			bot->Part(theChan->getName());
		}
	    else
	    {
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_AUTOJOIN) ? "ON" : "OFF");
		return true;
	    }
	    theChan->commit();
	    bot->Notice(theClient,
			bot->getResponse(theUser,
				language::set_cmd_status,
				string("%s for %s is %s")).c_str(),
			option.c_str(),
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_AUTOJOIN) ? "ON" : "OFF");
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
	    if(strlen(desc.c_str()) > 80)
	    {
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::desc_max_len,
					string("The DESCRIPTION can be a maximum of 80 chars!")));
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
	    if(strlen(url.c_str()) > 75) // Gator - changed to 75
	    {
			bot->Notice(theClient,
				bot->getResponse(theUser,
					language::url_max_len,
					string("The URL can be a maximum of 75 chars!")));
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
	    if(strlen(value.c_str()) > 80) // is 80 ok as an max keywords length?
	    {
		bot->Notice(theClient,
			bot->getResponse(theUser,
				language::keywords_max_len,
				string("The string of keywords cannot exceed 80 chars!")));
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

	    theChan->setChannelMode(tmpChan->getModeString());
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
	    if(level < level::set::comment)
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
