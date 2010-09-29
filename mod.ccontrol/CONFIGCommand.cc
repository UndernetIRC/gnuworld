/**
 * CONFIGCommand.cc 
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
 * $Id: CONFIGCommand.cc,v 1.14 2008/01/03 04:26:22 kewlio Exp $
 */

#include	<string>

#include	"StringTokenizer.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"
#include	"gnuworld_config.h"
#include	"ccontrol_generic.h"	/* for Duration() and Ago() */

RCSTAG( "$Id: CONFIGCommand.cc,v 1.14 2008/01/03 04:26:22 kewlio Exp $" ) ;

namespace gnuworld
{

using std::string ;
using std::endl ;
using std::ends ;

namespace uworld
{

bool CONFIGCommand::Exec( iClient* theClient, const string& Message)
{
StringTokenizer st(Message);

if(st.size() < 3)
	{
	Usage(theClient);
	return true;
	}

bot->MsgChanLog("CONFIG %s",st.assemble(1).c_str());
	
for(unsigned int  pos =1; pos < st.size() ;)
	{
	if(!strcasecmp(st[pos],"-VClones"))
		{
		if(st.size() < pos +2)
			{
			bot->Notice(theClient,"-VClones must get the number of virtual clones");
			return true;
			}
		if(!bot->updateMisc("VClones",atoi(st[pos+1].c_str())))
			{
			bot->MsgChanLog("Error while updating the max virtual clones in the db!\n");
			}
		else
			{
			bot->Notice(theClient,"%s was successfully updated to %s",
				    st[pos].c_str(),st[pos+1].c_str());
			}
		pos+=2;
		}
	else if(!strcasecmp(st[pos],"-IClones"))
		{
		if(st.size() < pos +2)
			{
			bot->Notice(theClient,"-IClones must get the number of ident clones");
			return true;
			}
		if(!bot->updateMisc("IClones",atoi(st[pos+1].c_str())))
			{
			bot->MsgChanLog("Error while updating the max ident clones in the db!\n");
			}
		else
			{
			bot->Notice(theClient,"%s was successfully updated to %s",
				st[pos].c_str(),st[pos+1].c_str());
			}
		pos+=2;
		}
        else if(!strcasecmp(st[pos],"-CClonesCIDR"))
                {
                if(st.size() < pos +2)
                        {
                        bot->Notice(theClient,"-CClonesCIDR must get the CIDR Size");
                        return true;
                        }
                if((atoi(st[pos+1].c_str())<16) || (atoi(st[pos+1].c_str())>31))
                        {
                        bot->Notice(theClient,"-CClonesCIDR must have a CIDR Size in the range 16-32");
                        return true;
                        }
                if(!bot->updateMisc("CClonesCIDR",atoi(st[pos+1].c_str())))
                        {
                        bot->MsgChanLog("Error while updating the CIDR size for clones in the db!\n");
                        }
                else
                        {
                        bot->Notice(theClient,"%s was successfully updated to %s",
                                st[pos].c_str(),st[pos+1].c_str());
                        bot->Notice(theClient,"This change requires you to restart the service IMMEDIATELY or "
                                "unexpected results will occur!");
                        }
                pos+=2;
                }
        else if(!strcasecmp(st[pos],"-CClonesTime"))
  	        {
  	        if(st.size() < pos +2)
  	                 {
  	                 bot->Notice(theClient,"-CClonesTime must get the duration between announcements per netblock.");
  	                 return true;
  	                 }
		if (!IsTimeSpec(st[pos+1]))
			{
			bot->Notice(theClient,"-CClonesTime must have a valid time specified, e.g. 60s or 1m");
			return true;
			}
		int g = extractTime(st[pos+1], 1);
		if (g < 0 || g > 300)
  	                 {
  	                 bot->Notice(theClient,"-CClonesTime value must be between 0s and 5m.");
  	                 return true;
  	                 }
  	        if(!bot->updateMisc("CClonesTime",g))
  	                 {
  	                 bot->MsgChanLog("Error while updating the Duration time.\n");
  	                 }
  	        else
  	                 {
  	                 bot->Notice(theClient,"%s was successfully updated to %s",
  	                 st[pos].c_str(),Duration((long)g));
  	                 }
                 pos+=2;
                 }
	else if(!strcasecmp(st[pos],"-CClonesGTime"))
		{
			if (st.size() < pos +2)
			{
				bot->Notice(theClient,"-CClonesGTime must get the duration of the gline in seconds.");
				return true;
			}
			if (!IsTimeSpec(st[pos+1]))
			{
				bot->Notice(theClient,"Invalid CIDR Clones Gline Time specified.");
				return true;
			}
			int g = extractTime(st[pos+1], 1);
			if (g < 1800 || g > 172800)
			{
				bot->Notice(theClient,"-CClonesGTime value must be between 1800 and 172800 seconds (30 mins - 2 days).");
				return true;
			}
			if (!bot->updateMisc("CClonesGTime",g))
			{
				bot->MsgChanLog("Error while updating the CIDR Clones Gline Time.\n");
			} else {
				bot->Notice(theClient,"%s was successfully updated to %s",
					st[pos].c_str(),Duration((long)g));
			}
			pos += 2;
		}
        else if(!strcasecmp(st[pos],"-CClones"))
                {
                if(st.size() < pos +2)
                        {
                        bot->Notice(theClient,"-CClones must be the number of CIDR clones");
                        return true;
                        }
                if(!bot->updateMisc("CClones",atoi(st[pos+1].c_str())))
                        {
                        bot->MsgChanLog("Error while updated the max CIDR clones in the db!\n");
                        }
                else
                        {
                        bot->Notice(theClient,"%s was successfully updated to %s",
                                st[pos].c_str(),st[pos+1].c_str());
                        }
                pos+=2;
                }
	else if(!strcasecmp(st[pos],"-Clones"))
		{
		if(st.size() < pos +2)
			{
			bot->Notice(theClient,"-Clones must get the number of virtual clones");
			return true;
			}
		if(!bot->updateMisc("Clones",atoi(st[pos+1].c_str())))
			{
			bot->MsgChanLog("Error while updating the max clones in the db!\n");
			}
		else
			{
			bot->Notice(theClient,"%s was successfully updated to %s",
				    st[pos].c_str(),st[pos+1].c_str());
			}
		pos+=2;			
		}
	else if(!strcasecmp(st[pos],"-GBCount"))
		{
		if(st.size() < pos +2)
			{
			bot->Notice(theClient,"-GBCount must get the number glines to burst");
			return true;
			}
		if(!bot->updateMisc("GlineBurstCount",atoi(st[pos+1].c_str())))
			{
			bot->MsgChanLog("Error while updating the gline burst count in the db!\n");
			}
		else
			{
			bot->Notice(theClient,"%s was successfully updated to %s",
				    st[pos].c_str(),st[pos+1].c_str());
			}
		pos+=2;			
		}
	else if(!strcasecmp(st[pos],"-GBInterval"))
		{
		if(st.size() < pos +2)
			{
			bot->Notice(theClient,"-GBInterval must get the number of seconds between each burst");
			return true;
			}
		if (!IsTimeSpec(st[pos+1]))
			{
			bot->Notice(theClient,"-GBInterval must get the number of seconds between each burst - e.g. 3 or 3s");
			return true;
			}
		int g = extractTime(st[pos+1], 1);
		if (g < 0 || g > 10)
			{
			bot->Notice(theClient,"-GBInterval must be between 0 and 10 seconds inclusive.");
			return true;
			}
		if(!bot->updateMisc("GlineBurstInterval",g))
			{
			bot->MsgChanLog("Error while updating the gline burst interval in the db!\n");
			}
		else
			{
			bot->Notice(theClient,"%s was successfully updated to %s",
				    st[pos].c_str(),Duration((long)g));
			}
		pos+=2;			
		}
	else if(!strcasecmp(st[pos],"-GTime"))
		{
		if(st.size() < pos +2)
			{
			bot->Notice(theClient,"-GTime must get the duration for the excessive connections gline - e.g. 900s, 30m, 1h or 1d.");
			return true;
			}
		if (!IsTimeSpec(st[pos+1]))
			{
			bot->Notice(theClient,"-GTime must get a valid duration - e.g. 900s, 30m, 1h or 1d.");
			return true;
			}
		int g = extractTime(st[pos+1], 1);
		if (g < 300 || g > 172800)
			{
			bot->Notice(theClient,"-GTime must get a duration between 300s and 2d");
			return true;
			}
		if(!bot->updateMisc("GTime",g))
			{
			bot->MsgChanLog("Error while updating the gline duration in the db!\n");
			}
		else
			{
			bot->Notice(theClient,"%s was successfully updated to %s",
				    st[pos].c_str(),Duration((long)g));
			}

		pos+=2;			
		}

	else if(!strcasecmp(st[pos],"-SGline"))
		{
		if(st.size() < pos +2)
			{
			bot->Notice(theClient,"-SGline must get a yes/no answer indicating whether or not to save glines");
			return true;
			}
		if(!bot->updateMisc("SGLine",(strcasecmp(st[pos+1],"YES") == 0) ? 1 : 0))
			{
			bot->MsgChanLog("Error while updating the save gline flag in the db\n");
			}
		else
			{
			bot->Notice(theClient,"%s was successfully updated to %s",
				    st[pos].c_str(),st[pos+1].c_str());
			}
		pos+=2;			
		}
        else if(!strcasecmp(st[pos],"-CClonesGline"))
                {
                if(st.size() < pos +2)
                        {
                        bot->Notice(theClient,"-CClonesGline must get a yes/no answer indicating whether or not to auto-gline");
                        return true;
                        }
                if(!bot->updateMisc("CClonesGline",(strcasecmp(st[pos+1],"YES") == 0) ? 1 : 0))
                        {
                        bot->MsgChanLog("Error while updating the CIDR auto-gline flag in the db\n");
                        }
                else
                        {
                        bot->Notice(theClient,"%s was successfully updated to %s",
                                st[pos].c_str(),st[pos+1].c_str());
                        }
                pos+=2;
		}
	else if(!strcasecmp(st[pos],"-IClonesGline"))
		{
		if(st.size() < pos +2)
			{
			bot->Notice(theClient,"-IClonesGline must get a yes/no answer indicating whether or not to auto-gline");
			return true;
			}
		if(!bot->updateMisc("IClonesGline",(strcasecmp(st[pos+1],"YES") == 0) ? 1 : 0))
			{
			bot->MsgChanLog("Error while updating the CIDR ident auto-gline flag in the db\n");
			}
		else
			{
			bot->Notice(theClient,"%s was successfully updated to %s",
				st[pos].c_str(),st[pos+1].c_str());
			}
		pos+=2;
		}
	else
		{
		bot->Notice(theClient,"Sorry, i am not familiar with option %s",st[pos].c_str());
		++pos;
		}
	}	
return true ;
}

}

} // namespace gnuworld
