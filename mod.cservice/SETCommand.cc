/*
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
 * $Id: SETCommand.cc,v 1.41 2001/07/16 19:31:18 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

const char SETCommand_cc_rcsId[] = "$Id: SETCommand.cc,v 1.41 2001/07/16 19:31:18 gte Exp $" ;

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
			theUser->commit();
			bot->Notice(theClient, 
				bot->getResponse(theUser,
					language::invis_on,
					string("Your INVISIBLE setting is now ON.")));
			return true;
		}

		if (value == "OFF")
		{
			theUser->removeFlag(sqlUser::F_INVIS);
			theUser->commit();
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

	if (option == "LANG")
	{ 
//		bot->Notice(theClient, "Coming soon!");
//		return true;

		cservice::languageTableType::iterator ptr = bot->languageTable.find(value);
		if (ptr != bot->languageTable.end()) 
		{
			string lang = ptr->second.second;
			theUser->setLanguageId(ptr->second.first);
			theUser->commit();
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

if (st.size() < 4)
	{
	value = "";
	}
else
	{
 	value = string_upper(st[3]);
	}

	/*
	 * Check the "Locked" status first, so admin's can bypas to turn it OFF :)
	 */

	if(option == "LOCKED")
	{
	    // Check for admin access
	    sqlChannel* admChan = bot->getChannelRecord("*");
	    int admLevel = bot->getAccessLevel(theUser, admChan);
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
		if (admLevel <= level::set::locked)
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
	sqlChannel* admChan = bot->getChannelRecord("*");
	int admLevel = bot->getAccessLevel(theUser, admChan);
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
	    sqlChannel* admChan = bot->getChannelRecord("*");
	    int admLevel = bot->getAccessLevel(theUser, admChan);
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
	    sqlChannel* admChan = bot->getChannelRecord("*");
	    int admLevel = bot->getAccessLevel(theUser, admChan);
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
	    sqlChannel* admChan = bot->getChannelRecord("*");
	    int admLevel = bot->getAccessLevel(theUser, admChan);
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
	    sqlChannel* admChan = bot->getChannelRecord("*");
	    int admLevel = bot->getAccessLevel(theUser, admChan);
	    if(admLevel < level::set::suspend)
	    {
			// No need to tell users about admin commands.
			Usage(theClient);
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
	    sqlChannel* admChan = bot->getChannelRecord("*");
	    int admLevel = bot->getAccessLevel(theUser, admChan);
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
	    sqlChannel* admChan = bot->getChannelRecord("*");
	    int admLevel = bot->getAccessLevel(theUser, admChan);
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
			bot->getUplink()->RegisterChannelEvent( theChan->getName(), bot ) ;
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
			bot->getUplink()->UnRegisterChannelEvent( theChan->getName(), bot ) ;
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

		int setting = atoi(value.c_str());
		if ( (setting < 0) || (setting > 2))
		{
			bot->Notice(theClient, 
				bot->getResponse(theUser,
					language::userflags_syntax,
					string("Invalid USERFLAGS setting. Correct values are 0, 1, 2.")));
			return false;
		}

		theChan->setUserFlags(setting);
		theChan->commit(); 
	    bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::userflags_status,
				string("USERFLAGS for %s is %i")).c_str(),
			theChan->getName().c_str(), setting);
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

	if(option == "FLOODPRO")
	{
	    if(level < level::set::floodpro)
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
					language::floodpro_syntax,
					string("value of FLOODPRO has to be 0-7")));
			return true;
	    }
		    int numValue = atoi(value.c_str());
		    if(numValue > 7 || numValue < 0)
	    {
		bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::floodpro_syntax,
				string("value of FLOODPRO has to be 0-7")));
		return true;
	    }
		theChan->setFloodPro(numValue);
	    theChan->commit(); 
	    bot->Notice(theClient, 
			bot->getResponse(theUser,
				language::floodpro_status,
				string("FLOODPRO for %s is set to %d")).c_str(),
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
	    if(bot->getAdminAccessLevel(theUser) < level::set::comment)
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
 

	bot->Notice(theClient, 
		bot->getResponse(theUser,
			language::mode_invalid,
			string("ERROR: Invalid channel setting.")));	    
	return true ; 
} 

} // namespace gnuworld.

