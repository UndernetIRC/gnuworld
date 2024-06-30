/**
 * LISTCommand.cc
 * Gives list of all kind of stuff to the oper
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
 * $Id: LISTCommand.cc,v 1.24 2008/08/06 19:35:58 hidden1 Exp $
 */

#include	<string>
#include	<iomanip>

#include	<cstdlib>

#include	"ccontrol.h"
#include	"Network.h"
#include	"Channel.h"
#include	"CControlCommands.h"
#include	"StringTokenizer.h"
#include	"gnuworld_config.h"

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool LISTCommand::Exec( iClient* theClient, const string& Message)
{	 
typedef list<const Channel*> channelListT;
const Channel* curChannel;
channelListT channelList;
unsigned int count = 0;
const char *c;
char ch;
string Mask;
bool isValid = false;

StringTokenizer st( Message ) ;
if(st.size() < 2)
	{
	Usage(theClient);
	return true;
	}

bot->MsgChanLog("LIST %s\n",st.assemble(1).c_str());
if(!strcasecmp(st[1].c_str(),"glines"))
	{
	bot->listGlines(theClient);
	}
else if(!strcasecmp(st[1].c_str(),"suspended"))
	{
	bot->listSuspended(theClient);
	}
else if(!strcasecmp(st[1].c_str(),"servers"))
	{
	bot->listServers(theClient);
	}
else if(!strcasecmp(st[1].c_str(),"badchannels"))
	{
	bot->Notice(theClient,"Please use LIST nomodechannels to get a list of the channels which are marked as no mode");
	}
else if(!strcasecmp(st[1].c_str(),"nomodechannels"))
	{
	bot->listBadChannels(theClient);
	}
else if(!strcasecmp(st[1].c_str(),"limits"))
	{
	bot->listIpLExceptions(theClient);
	}
else if(!strcasecmp(st[1].c_str(),"channels"))
	{
		/* new "list channels" command */
		if (st.size() < 3)
		{
			/* not enough parameters */
#ifdef TOPIC_TRACK
			bot->Notice(theClient, "'list channels' requires 'key', 'modes', 'topic' or 'topicby'");
#else
			bot->Notice(theClient, "'list channels' requires 'key' or 'modes'");
#endif
			return false;
		}
		if (!strcasecmp(st[2].c_str(),"key"))
		{
			if (st.size() < 4)
			{
				bot->Notice(theClient, "for 'list channels key', you must supply the key to search for!");
				return false;
			}
			/* ok, we have the key to search for in st[3] */
			channelList = Network->getChannelsWithKey(st[3]);

			if (channelList.empty())
			{
				bot->Notice(theClient, "I'm sorry, no channels have a key matching '%s'", st[3].c_str());
				return false;
			}

			bot->Notice(theClient, "--==[ Channels matching key '%s' %s]==--", st[3].c_str(),
				(channelList.size() > 15)?"- showing first 15 only":"");

			for (channelListT::iterator cptr = channelList.begin(); (cptr != channelList.end()) && (count < 15); cptr++)
			{
				curChannel = *cptr;
				bot->Notice(theClient, "Channel: [%4d] %s", 
					curChannel->size(),
					curChannel->getName().c_str());
				count++;
			}
			bot->Notice(theClient, "--==[ End of 'list channels key %s' - %d found ]==--",
				st[3].c_str(), channelList.size());
		}
		else if (!strcasecmp(st[2].c_str(),"modes"))
		{
			if (st.size() < 4)
			{
				bot->Notice(theClient, "for 'list channels modes', you must supply the modes to match against!");
				bot->Notice(theClient, "e.g. 'list channels modes +tns-m' to require +tns and require not to have +m");
				bot->Notice(theClient, "(other modes may be set unless explicitly not required as above)");
				return false;
			}
			if (st.size() > 4)
			{
				bot->Notice(theClient, "You supplied too many arguments.  arguments such as +k or +l here may not contain the key or limit!");
				return false;
			}
			/* ok, we have the modes to search for in st[3] - validate them */
			if ((!match("+", st[3].substr(0,1))) && (!match("-", st[3].substr(0,1))))
			{
				bot->Notice(theClient, "The modes must start with + or - (e.g. +tns or -s)");
				return false;
			}
			isValid = true;
			c = st[3].c_str();
			while ((ch = *c++))
			{
				switch (ch) {
					case '+':
					case '-':
					case 'k':
					case 'l':
					case 'i':
					case 'm':
					case 'n':
					case 'p':
					case 's':
					case 't':
					case 'r':
					case 'D':	/* acceptable modes */
							break;
					default:	/* unacceptable modes */
							isValid = false;
							break;
				}
			}
			if (!isValid)
			{
				bot->Notice(theClient, "The modes given are invalid (you supplied '%s', valid modes are klimnpstrD)",
					st[3].c_str());
				return false;
			}
			/* ok, validated them */
			channelList = Network->getChannelsWithModes(st[3]);

			if (channelList.empty())
			{
				bot->Notice(theClient, "I'm sorry, no channels have modes matching '%s'", st[3].c_str());
				return false;
			}

			bot->Notice(theClient, "--==[ Channels matching modes '%s' %s]==--", st[3].c_str(),
				(channelList.size() > 15)?"- showing first 15 only":"");

			for (channelListT::iterator cptr = channelList.begin(); (cptr != channelList.end()) && (count < 16); cptr++)
			{
				curChannel = *cptr;
				bot->Notice(theClient, "Channel: [%4d] %s",
					curChannel->size(),
					curChannel->getName().c_str());
				count++;
			}
			bot->Notice(theClient, "--==[ End of 'list channels modes %s' - %d found ]==--",
				st[3].c_str(), channelList.size());
		}
#ifdef TOPIC_TRACK
		else if (!strcasecmp(st[2].c_str(),"topic"))
		{
			if (st.size() < 4)
			{
				bot->Notice(theClient, "for 'list channels topic', you must supply the topic to search for!");
				return false;
			}
			/* ok, we have the topic to search for in st[3] */
			channelList = Network->getChannelsWithTopic(st[3]);

			if (channelList.empty())
			{
				bot->Notice(theClient, "I'm sorry, no channels have a topic matching '%s'", st[3].c_str());
				return false;
			}

			bot->Notice(theClient, "--==[ Channels matching topic '%s' %s]==--", st[3].c_str(),
				(channelList.size() > 15)?"- showing first 15 only":"");

			for (channelListT::iterator cptr = channelList.begin(); (cptr != channelList.end()) && (count < 15); cptr++)
			{
				curChannel = *cptr;
				bot->Notice(theClient, "Channel: [%4d] %s",
					curChannel->size(),
					curChannel->getName().c_str());
				count++;
			}
			bot->Notice(theClient, "--==[ End of 'list channels topic '%s' - %d found ]==--",
				st[3].c_str(), channelList.size());
		}
		else if (!strcasecmp(st[2].c_str(),"topicby"))
		{
			if (st.size() < 4)
			{
				bot->Notice(theClient, "for 'list channels topicby', you must supply the nick!user@host mask to search for!");
				return false;
			}
			/* validate the nick!user@host */
			string::size_type atPos = st[3].find_first_of('!');
			if (string::npos == atPos)
			{
				/* not in nick!user@host format */
				/* assume they want to check nick only, but tell them just in case */
				Mask = "";
				Mask += st[3].c_str();
				Mask += "!*@*";
				bot->Notice(theClient, "Mask was not in nick!user@host format, re-writing mask to '%s'.  This may not be what you want!",
					Mask.c_str());
			} else {
				/* we have nick!..., check for user@host! */
				string::size_type atPos2 = st[3].find_first_of('@');
				if ((string::npos == atPos2) || (atPos2 < atPos))
				{
					/* either no '@' or it's before the first '!' */
					/* assume they want to check nick!user only, but tell them just in case */
					Mask = "";
					Mask += st[3].c_str();
					/* if the '!' is at the end of the line, we need to add a * */
					if (atPos == (st[3].length()-1))
						Mask += "*";
					Mask += "@*";
					bot->Notice(theClient, "Mask was not in nick!user@host format, re-writing mask to '%s'.  This may not be what you want!",
						Mask.c_str());
				} else {
					/* we have a full nick!user@host - we dont need to alter it */
					Mask = st[3].c_str();
				}
			}
			/* ok, we have a valid topicby to search for in Mask */
			channelList = Network->getChannelsWithTopicBy(Mask);

			if (channelList.empty())
			{
				bot->Notice(theClient, "I'm sorry, no channels have a topic matching '%s'", Mask.c_str());
				return false;
			}

			bot->Notice(theClient, "--==[ Channels with topics set by '%s' %s]==--", Mask.c_str(),
				(channelList.size() > 15)?"- showing first 15 only":"");

			for (channelListT::iterator cptr = channelList.begin(); (cptr != channelList.end()) && (count < 15); cptr++)
			{
				curChannel = *cptr;
				bot->Notice(theClient, "Channel: [%4d] %s",
					curChannel->size(),
					curChannel->getName().c_str());
				count++;
			}
			bot->Notice(theClient, "--==[ End of 'list channels topicby '%s' - %d found ]==--",
				Mask.c_str(), channelList.size());
		} else {
			bot->Notice(theClient,"'list channels' requires 'key', 'modes', 'topic' or 'topicby'");
			return false;
#else
		else {
			bot->Notice(theClient,"'list channels' requires 'key' or 'modes'");
			return false;
#endif
		}
	}
else
	{
	bot->Notice(theClient,"Unknown list type - see HELP");
	}

return true ;
}

}
}

