/*
 * CONFIGCommand.cc 
 *
 */

#include	<string>
#include	<cstdlib>
#include	"StringTokenizer.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"

const char CONFIGCommand_cc_rcsId[] = "$Id: CONFIGCommand.cc,v 1.5 2003/03/06 12:34:13 mrbean_ Exp $";

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
		if(!bot->updateMisc("GlineBurstInterval",atoi(st[pos+1].c_str())))
			{
			bot->MsgChanLog("Error while updating the gline burst interval in the db!\n");
			}
		else
			{
			bot->Notice(theClient,"%s was successfully updated to %s",
				    st[pos].c_str(),st[pos+1].c_str());
			}
		pos+=2;			
		}
	else if(!strcasecmp(st[pos],"-GTime"))
		{
		if(st.size() < pos +2)
			{
			bot->Notice(theClient,"-GTime must get the duration in seconds for the excessive connections gline");
			return true;
			}
		if(!bot->updateMisc("GTime",atoi(st[pos+1].c_str())))
			{
			bot->MsgChanLog("Error while updating the gline duration in the db!\n");
			}
		else
			{
			bot->Notice(theClient,"%s was successfully updated to %s",
				    st[pos].c_str(),st[pos+1].c_str());
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
