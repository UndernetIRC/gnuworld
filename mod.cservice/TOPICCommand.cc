/* 
 * TOPICCommand.cc 
 *
 * 26/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * 30/12/2000 - David Henriksen <david@itwebnet.dk>
 * Wrote TOPIC Command.
 *
 * Sets a topic in the channel.
 *
 * Caveats: None
 *
 * $Id: TOPICCommand.cc,v 1.2 2000/12/30 23:32:34 gte Exp $
 */

#include	<string>
 
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"

const char TOPICCommand_cc_rcsId[] = "$Id: TOPICCommand.cc,v 1.2 2000/12/30 23:32:34 gte Exp $" ;

namespace gnuworld
{

using namespace gnuworld;
 
bool TOPICCommand::Exec( iClient* theClient, const string& Message )
{ 
	StringTokenizer st( Message ) ;
	if( st.size() < 3 )
	{
		Usage(theClient);
		return true;
	}
	
	sqlUser* theUser = bot->isAuthed(theClient, true);
	if(!theUser)
	{
		return false;
	}
	
	sqlChannel* theChan = bot->getChannelRecord(st[1]);
	if(!theChan)
	{
		bot->Notice(theClient, bot->getResponse(theUser, language::chan_not_reg).c_str(),
			st[1].c_str());
		return false;
	}
	
	int level = bot->getAccessLevel(theUser, theChan);
	if(level < level::topic)
	{
		bot->Notice(theClient, bot->getResponse(theUser, language::insuf_access).c_str(),
			st[1].c_str());
		return false;
	}
	
	// Cannot set topic, if E hasn't joined.
	
	Channel* tmpChan = Network->findChannel(theChan->getName());
	if(!tmpChan)
	{
		bot->Notice(theClient, bot->getResponse(theUser, language::chan_is_empty).c_str(),
			st[1].c_str());
		return false;
	}
	
	// Done with the checking.
	
	string topic = st.assemble(2);
	
	if(strlen(topic.c_str()) > 160) // Default ircu TOPICLEN.
	{
	    bot->Notice(theClient, "ERROR: Your defined topic is exceeding 160 chars!");
	    return false;
        }
        
	strstream s;
	s << bot->getCharYYXXX() << " T "
	<< st[1] << " :" << topic << " (" << theUser->getUserName() << ")" << ends;
        
	bot->Write( s );

	delete[] s.str(); 
        
	return true ; 
} 

} // namespace gnuworld.
