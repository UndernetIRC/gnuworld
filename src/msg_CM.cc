/*
 * msg_CM. cc
 *
 * $Id: msg_CM.cc,v 1.2 2002/01/12 20:20:12 gte Exp $
 */

#include	"server.h"
#include	"Channel.h"
#include	"ChannelUser.h"
#include	"Network.h"
#include	"iClient.h"
#include	"xparameters.h"

namespace gnuworld
{
/**
 * CLEARMODE message handler.
 * ZZAAA CM #channel obv
 * The above message would remove all ops, bans, and voice modes
 *  from channel #channel.
 */
int xServer::MSG_CM( xParameters& Param )
{

const unsigned int CLEAR_CHANOPS    = 0x00000001 ;
const unsigned int CLEAR_VOICED     = 0x00000002 ;
const unsigned int CLEAR_SECRET     = 0x00000004 ;
const unsigned int CLEAR_MODERATED  = 0x00000008 ;
const unsigned int CLEAR_TOPICLIMIT = 0x00000010 ;
const unsigned int CLEAR_INVITEONLY = 0x00000020 ;
const unsigned int CLEAR_NOPRIVMSGS = 0x00000040 ;
const unsigned int CLEAR_KEY        = 0x00000080 ;
const unsigned int CLEAR_BANS       = 0x00000100 ;
const unsigned int CLEAR_LIMIT      = 0x00000200 ;

Channel* tmpChan = Network->findChannel(Param[1]);

if(!tmpChan)
	{
		// Log Error.
		return 0;
	}

/*
 * First, determine what we are going to clear.
 */

string Modes = Param[2];
unsigned int clearModes = 0;

for( string::size_type i = 0 ; i < Modes.size() ; i++ )
	{
	switch( Modes[ i ] )
		{
		case 'o':
		case 'O':
			clearModes |= CLEAR_CHANOPS ;
			elog << tmpChan->getName() << "Doing CLEAR_CHANOPS" << endl;
			break ;
		case 'v':
		case 'V':
			clearModes |= CLEAR_VOICED ;
			elog << tmpChan->getName() << "Doing CLEAR_VOICED" << endl;
			break ;
		case 's':
		case 'S':
			clearModes |= CLEAR_SECRET ;
			tmpChan->removeMode(Channel::MODE_S);
			elog << tmpChan->getName() << "Doing CLEAR_SECRET" << endl;
			break ;
		case 'm':
		case 'M':
			clearModes |= CLEAR_MODERATED;
			tmpChan->removeMode(Channel::MODE_M);
			elog << tmpChan->getName() << "Doing CLEAR_MODERATED" << endl;
			break ;
		case 't':
		case 'T':
			clearModes |= CLEAR_TOPICLIMIT ;
			tmpChan->removeMode(Channel::MODE_T);
			elog << tmpChan->getName() << "Doing CLEAR_TOPICLIMIT" << endl;
			break ;
		case 'i':
		case 'I':
			clearModes |= CLEAR_INVITEONLY ;
			tmpChan->removeMode(Channel::MODE_I);
			elog << tmpChan->getName() << "Doing CLEAR_INVITEONLY" << endl;
			break ;
		case 'n':
		case 'N':
			clearModes |= CLEAR_NOPRIVMSGS ;
			tmpChan->removeMode(Channel::MODE_N);
			elog << tmpChan->getName() << "Doing CLEAR_NOPRIVMSGS" << endl;
			break ;
		case 'k':
		case 'K':
			clearModes |= CLEAR_KEY ;
			tmpChan->removeMode(Channel::MODE_K);
			tmpChan->setKey("");
			elog << tmpChan->getName() << "Doing CLEAR_KEY" << endl;
			break ;
		case 'b':
		case 'B':
			clearModes |= CLEAR_BANS ;
			tmpChan->removeAllBans();
			elog << tmpChan->getName() << "Doing CLEAR_BANS" << endl;
			break ;
		case 'l':
		case 'L':
			clearModes |= CLEAR_LIMIT ;
			tmpChan->removeMode(Channel::MODE_L);
			tmpChan->setLimit(0);
			elog << tmpChan->getName() << "Doing CLEAR_LIMIT" << endl;
			break ;
		default:
			// Unknown mode
			break ;
		} // switch
	} // for

if (clearModes & (CLEAR_CHANOPS | CLEAR_VOICED))
	{
	/*
	 * Lets loop over everyone in the channel and either deop
	 * or devoice them.
	 */

	for( Channel::const_userIterator ptr = tmpChan->userList_begin();
		ptr != tmpChan->userList_end() ; ++ptr )
		{
			if(clearModes & CLEAR_CHANOPS) ptr->second->removeModeO();
			if(clearModes & CLEAR_VOICED) ptr->second->removeModeV();
		}
	}

/**
 * TODO: Do we wish to dispatch events to the modules now?
 */

return 0 ;
}

} // namespace gnuworld
