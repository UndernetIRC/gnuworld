/*
 * SETCommand.cc
 *
 * 28/12/2000 - David Henriksen <david@itwebnet.dk>
 * Initial Version.
 * 01/01/2001 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Modifications.
 *
 * Sets channel options on the specified channel.
 *
 * Caveats: SET LANG is still under consideration.
 *
 * $Id: SETCommand.cc,v 1.10 2001/01/20 22:01:01 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h" 
#include	"cservice.h"
#include	"Network.h"
#include	"levels.h"
#include	"responses.h"

const char SETCommand_cc_rcsId[] = "$Id: SETCommand.cc,v 1.10 2001/01/20 22:01:01 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
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
	if(!theUser) return false; 

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
				bot->Notice(theClient, "Your INVISIBLE setting is now ON.");
				return true;
			}

			if (value == "OFF")
			{
				theUser->removeFlag(sqlUser::F_INVIS);
				theUser->commit();
				bot->Notice(theClient, "Your INVISIBLE setting is now OFF.");
				return true;
			}

		}

	}

    Channel* tmpChan = Network->findChannel(st[1]); 

	/* Is the channel registered? */
	
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if(!theChan)
	{
	    bot->Notice(theClient, "Sorry, %s isn't registered with me.", st[1].c_str());
	    return false;
	} 
 
	// Check level.

	int level = bot->getEffectiveAccessLevel(theUser, theChan, false); 
	string option = string_upper(st[2]);
	string value; 

	if (st.size() < 4)
	{
		value = "";
	} else {
	 	value = string_upper(st[3]);
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
		bot->Notice(theClient, "value of CAUTION must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "CAUTION for %s is %s",
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
		bot->Notice(theClient, "value of NOREG must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "NOREG for %s is %s",
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
		bot->Notice(theClient, "value of SPECIAL must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "SPECIAL for %s is %s",
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
		bot->Notice(theClient, "value of NEVERREG must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "NEVERREG for %s is %s",
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
		bot->Notice(theClient, "value of NOPURGE must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "NOPURGE for %s is %s",
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
		bot->Notice(theClient, "value of SUSPEND must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "SUSPEND for %s is %s",
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
		bot->Notice(theClient, "value of TEMPMAN must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "TEMPMAN for %s is %s",
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
	    if(value == "ON") theChan->setFlag(sqlChannel::F_TEMP);
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_TEMP);
	    else
	    {
		bot->Notice(theClient, "value of VACATION must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "VACATION for %s is %s",
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_VACATION) ? "ON" : "OFF");
	    return true;
	}

	if(option == "NOOP")
	{
	    if(level < level::set::noop)
	    {
			bot->Notice(theClient, "NOOP: You do not have enough access!");
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
		bot->Notice(theClient, "value of NOOP must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "NOOP for %s is %s",
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_NOOP) ? "ON" : "OFF");
	    return true;
	}

	if(option == "ALWAYSOP")
	{
	    if(level < level::set::alwaysop)
	    {
		bot->Notice(theClient, "ALWAYSOP: You do not have enough access!");
		return true;
	    }
	    if(value == "ON") theChan->setFlag(sqlChannel::F_ALWAYSOP);
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_ALWAYSOP);
	    else
	    {
		bot->Notice(theClient, "value of ALWAYSOP must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "ALWAYSOP for %s is %s",
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_ALWAYSOP) ? "ON" : "OFF");
	    return true;
	}

	if(option == "OPONLY")
	{
	    if(level < level::set::oponly)
	    {
		bot->Notice(theClient, "OPONLY: You do not have enough access!");
		return true;
	    }
	    if(value == "ON") theChan->setFlag(sqlChannel::F_OPONLY);
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_OPONLY);
	    else
	    {
		bot->Notice(theClient, "value of OPONLY must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "OPONLY for %s is %s",
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_OPONLY) ? "ON" : "OFF");
	    return true;
	}

	if(option == "STRICTOP")
	{
	    if(level < level::set::strictop)
	    {
		bot->Notice(theClient, "STRICTOP: You do not have enough access!");
		return true;
	    }
	    if(value == "ON") theChan->setFlag(sqlChannel::F_STRICTOP);
	    else if(value == "OFF") theChan->removeFlag(sqlChannel::F_STRICTOP);
	    else
	    {
		bot->Notice(theClient, "value of STRICTOP must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "STRICTOP for %s is %s",
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_STRICTOP) ? "ON" : "OFF");
	    return true;
	}

	if(option == "AUTOTOPIC")
	{
	    if(level < level::set::autotopic)
	    {
		bot->Notice(theClient, "AUTOTOPIC: You do not have enough access!");
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
		bot->Notice(theClient, "value of AUTOTOPIC must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "AUTOTOPIC for %s is %s",
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_AUTOTOPIC) ? "ON" : "OFF");
	    return true;
	}

	if(option == "AUTOJOIN")
	{
	    if(level < level::set::autojoin)
	    {
		bot->Notice(theClient, "AUTOJOIN: You do not have enough access!");
		return true;
	    }
	    if(value == "ON") 
	    { 
	    	theChan->setFlag(sqlChannel::F_AUTOJOIN);
			theChan->setInChan(true);
			bot->getUplink()->RegisterChannelEvent( theChan->getName(), bot ) ;
			bot->Join(theChan->getName(), theChan->getChannelMode(), 
				theChan->getChannelTS(), true);
		}
	    else if(value == "OFF") 
	    { 
	    	theChan->removeFlag(sqlChannel::F_AUTOJOIN);
			theChan->setInChan(false);
			bot->getUplink()->UnRegisterChannelEvent( theChan->getName(), bot ) ;
			bot->Part(theChan->getName());
		}
	    else
	    {
		bot->Notice(theClient, "value of AUTOJOIN must be ON or OFF");
		return true;
	    }
	    theChan->commit(); 
	    bot->Notice(theClient, "AUTOJOIN for %s is %s",
			theChan->getName().c_str(),
			theChan->getFlag(sqlChannel::F_AUTOJOIN) ? "ON" : "OFF");
	    return true;
	}

//	if(option == "USERFLAGS")
//	{
//	    /* There is no F_USERFLAGS flag? --Plexus */
//	    
//	    if(level < level::set::userflag)
//	    {
//		bot->Notice(theClient, "USERFLAGS: You do not have enough access!");
//		return true;
//	    }
//	    if(value == "1") theChan->flags |= F_USERFLAGS;
//	    else if(value == "0") theChan->flags &= ~F_USERFLAGS;
//	    else
//	    {
//		bot->Notice(theClient, "value of USERFLAGS must be 1 or 0");
//		return true;
//	    }
//	    // theChan->commit(); 
//	    bot->Notice(theClient, "USERFLAGS for %s is %s",
//			theChan->name.c_str(),
//			theChan->flags & F_USERFLAGS ? "1" : "0");
//	    return true;
//	}

	if(option == "MASSDEOPPRO")
	{
	    if(level < level::set::massdeoppro)
	    {
			bot->Notice(theClient, "MASSDEOPPRO: You do not have enough access!");
			return true;
	    }
	    // Temporary MASSDEOPPRO range! 0-7.. is this correct?
	    if(!IsNumeric(value))
	    {
			bot->Notice(theClient, "value of MASSDEOPPRO has to be 0-7");
			return true;
	    }
	    int numValue = atoi(value.c_str());
	    if(numValue > 7 || numValue < 0)
	    {
			bot->Notice(theClient, "value of MASSDEOPPRO has to be 0-7");
			return true;
	    }
		theChan->setMassDeopPro(numValue);
	    theChan->commit(); 
	    bot->Notice(theClient, "MASSDEOPPRO for %s is set to %d",
			theChan->getName().c_str(),
			numValue);
	    return true;
	}

	if(option == "FLOODPRO")
	{
	    if(level < level::set::floodpro)
	    {
			bot->Notice(theClient, "FLOODPRO: You do not have enough access!");
			return true;
	    }
	    // Temporary MASSDEOPPRO range! 0-7.. is this correct?
	    if(!IsNumeric(value))
	    {
			bot->Notice(theClient, "value of FLOODPRO has to be 0-7");
			return true;
	    }
		    int numValue = atoi(value.c_str());
		    if(numValue > 7 || numValue < 0)
	    {
		bot->Notice(theClient, "value of FLOODPRO has to be 0-7");
		return true;
	    }
		theChan->setFloodPro(numValue);
	    theChan->commit(); 
	    bot->Notice(theClient, "FLOODPRO for %s is set to %d",
			theChan->getName().c_str(),
			numValue);
	    return true;
	}

	if(option == "DESCRIPTION")
	{
		string desc = st.assemble(3);
	    if(level < level::set::desc)
	    {
			bot->Notice(theClient, "DESCRIPTION: You do not have enough access!");
		return true;
	    }
	    if(strlen(desc.c_str()) > 80)
	    {
			bot->Notice(theClient, "The DESCRIPTION can be a maximum of 80 chars!");
			return true;
	    }
		theChan->setDescription(desc);
	    theChan->commit(); 
	    bot->Notice(theClient, "DESCRIPTION for %s is: %s",
			theChan->getName().c_str(),
			desc.c_str());
		if (theChan->getFlag(sqlChannel::F_AUTOTOPIC))
		{
			bot->doAutoTopic(theChan);
		}

	    return true;
	}

	if(option == "URL")
	{
		string url = st[3];
	    if(level < level::set::url)
	    {
			bot->Notice(theClient, "URL: You do not have enough access!");
			return true;
	    }
	    if(strlen(value.c_str()) > 80) // is 80 ok as an max url length?
	    {
			bot->Notice(theClient, "The URL can be a maximum of 80 chars!");
			return true;
	    }
		theChan->setURL(url);
	    theChan->commit(); 
	    bot->Notice(theClient, "URL for %s is: %s",
			theChan->getName().c_str(),
			url.c_str());
		if (theChan->getFlag(sqlChannel::F_AUTOTOPIC))
		{
			bot->doAutoTopic(theChan);
		}

	    return true;
	}

	if(option == "KEYWORDS")
	{
	    /* Keywords are being processed as a long string. */
	    if(level < level::set::keywords)
	    {
		bot->Notice(theClient, "KEYWORDS: You do not have enough access!");
		return true;
	    }
	    if(strlen(value.c_str()) > 80) // is 80 ok as an max keywords length?
	    {
		bot->Notice(theClient, "The string of keywords cannot exceed 80 chars!");
		return true;
	    }
		theChan->setKeywords(value);
	    theChan->commit(); 
	    bot->Notice(theClient, "KEYWORDS for %s are: %s",
			theChan->getName().c_str(),
			value.c_str());
	    return true;
	}

	if(option == "LANG")
	{
	    if(level < level::set::lang)
	    {
		bot->Notice(theClient, "LANG: You do not have enough access!");
		return true;
	    }
	    string lang = string_upper(value);
	    if(lang == "EN") // English.
	    {
		theChan->commit(); 
		bot->Notice(theClient, "Language for channel %s is set to English.",
			    theChan->getName().c_str());
		return true;
	    }
	    /* Insert the rest of the languages here.. */
	    bot->Notice(theClient, "ERROR: Invalid language selection.");
	    return true;
	}

	if(option == "MODE")
	{
	    if(level < level::set::mode)
	    {
		bot->Notice(theClient, "MODE: You do not have enough access!");
		return true;
	    }
	    string modes = ""; // Chan modes.
	    string args = ""; // key or limit. 
	    
	    StringTokenizer::size_type mpos = 4;
	    StringTokenizer::size_type apos = 5;
	    bool plus;
	    while(mpos < st.size())
	    {
		if(mpos == apos) apos++;

		string::size_type mposInc = 1;
		
		plus = true;
		
		for(string::size_type cpos = 0; cpos < st[mpos].size(); ++cpos)
		{
		    switch(st[mpos][cpos])
		    {
			case 'k':
				if(apos >= st.size())
				{
				    bot->Notice(theClient, "ERROR: You have specified a KEY mode(+k), without arguments!");
				    return true;
				}
				modes += st[mpos][cpos];
				args += st[apos] + ' ';
				apos++;
				mposInc++;
				break;
			case 'l':
				if(!plus) // -l doesn't need any arguments.
				{
				    modes += st[mpos][cpos];
				    break;
				}
				if(apos >= st.size())
				{
				    bot->Notice(theClient, "ERROR: You have specified a LIMIT mode(+l), without arguments!");
				    return true;
				}
				modes += st[mpos][cpos];
				args += st[apos] + ' ';
				apos++;
				mposInc++;
				break;
			
			/*
			 * The +sntipm mode checking isn't really
			 * required, but is added so cmaster can
			 * tell the user if he/she is using invalid
			 * channel modes. aka userfriendly :p
			 */
			 
			case 's':
				modes += st[mpos][cpos];
				break;
			case 'p':
				modes += st[mpos][cpos];
				break;
			case 'n':
				modes += st[mpos][cpos];
				break;
			case 't':
				modes += st[mpos][cpos];
				break;
			case 'i':
				modes += st[mpos][cpos];
				break;
			case 'm':
				modes += st[mpos][cpos];
				break;
			case '+':
				if(plus) break;
				plus = true;
				modes += st[mpos][cpos];
				break;
			case '-':
				if(!plus) break;
				plus = false;
				modes += st[mpos][cpos];
				break;
			default:
				bot->Notice(theClient, "ERROR: You have defined one or more, invalid channel modes!");
				return true;
				break;
		    }
		}
		mpos += mposInc;
	}
 
	strstream s;
	
	s << bot->getCharYYXXX() << " M " << theChan->getName()
	  << " " << modes + ' ' << args << endl << ends;
	
	bot->Write( s.str() );
	
	delete[] s.str();

	// updates internal tables, idea taken from ccontrol.
//	tmpChan->OnModeChange(theClient->getCharYYXXX(), modes, args);

	string finalMode = modes + ' ' + args;	
	theChan->setChannelMode(finalMode);
	theChan->commit();
	    return true;
	}

	bot->Notice(theClient, "ERROR: Invalid channel setting.");	    
	return true ; 
} 

} // namespace gnuworld.

