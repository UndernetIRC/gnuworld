/*
 * CONFIGCommand.cc 
 *
 */

#include	<string>
#include	<cstdlib>
#include	"StringTokenizer.h"
#include	"ccontrol.h"
#include	"CControlCommands.h"

const char CONFIGCommand_cc_rcsId[] = "$Id: CONFIGCommand.cc,v 1.3 2003/02/16 12:14:23 mrbean_ Exp $";

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
	}	
return true ;
}

}

} // namespace gnuworld
