/**
 * msg_B.cc
 */

#include	<sys/types.h>
#include	<sys/time.h>

#include	<new>
#include	<string>
#include	<vector>
#include	<pair.h>

#include	<cassert>

#include	"server.h"
#include	"xparameters.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"Channel.h"
#include	"ChannelUser.h"
#include	"Network.h"
#include	"iClient.h"

const char msg_B_cc_rcsId[] = "$Id: msg_B.cc,v 1.4 2001/02/05 19:20:25 dan_karrels Exp $" ;

using std::string ;
using std::vector ;
using std::endl ;

namespace gnuworld
{

// MSG_B
// This is the BURST command.
// 0 B #merida 957075177 +tn BAA:o
// 0 B #ValHalla 000031335 +stn CAA:o
// 0 B #linux 801203210 +stn OBL,LLf:ov,MBU:o,OBE,MAg,MAh
// 0 B #mylinux 953234759 +tn OBL,MBU:o,OBE,MAg,MAh
// 0 B #krushnet 000031337 +tn LEM,qAD,OAi,2B6,kA],kA9,qAA,2B2,NCR,kAD,OAC,0DK:o,MAL,zDj
// :%*!*lamer@*lamer.lamer.lamer.lamer11.com *!*lamer@*lamer.lamer.lamer.lamer10.com
// *!*lamer@*lamer.lamer.lamer.lamer9.com *!*lamer@*lamer.lamer.lamer.lamer8.com
// *!*lamer@*lamer.lamer.lamer.lamer7.com *!*lamer@*lamer.lamer.lamer.lamer6.com
// *!*lamer@*lamer.lamer.lamer.lamer5.com *!*lamer@*lamer.lamer.lamer.lamer4.com
// *!*lamer@*lamer.lamer.lamer.lamer3.com *!*lamer@*lamer.lamer.lamer.lamer2.com
//
// Q B #ateneo 848728923 +tnl 2000 r]Q,ZLC,Smt,rGN,gPk,uhy,Z]N,oTL,uem,31b,Znt,
//  3x3,oC0,TvC,3vs,oSo,IP7,oXL,aF2,CW9,sTq,Znw,Is9,gPD,rI1,ToI,ZZK,oGB,$Q
// B #ateneo 848728923 4Qt,LE2,LXJ,3ys,oIG,lwc,TQX,HwR,3iZ,g2D,ZP3,3m2,uPi,Z0n,
//  LTi,oG[,a3N,IH4,T3T,La],goY,geE,sar,oid,o90,35Y,TUL,Z7K,Zx7,TN1,C6$Q
// B #ateneo 848728923 :%*!*@203.145.226.149 *!*@203.177.4.* *!*@203.145.226.134
// *!*@202.8.230.*
//
// Q B #hgsd 933357379 +tn PIs,OfK,OAu,PZl:o,eAA
//
int xServer::MSG_B( xParameters& Param )
{

// Make sure there are at least four arguments supplied:
// servernumeric #channel time_stamp arguments
if( Param.size() < 4 )
	{
	elog	<< "xServer::MSG_B> Invalid number of arguments\n" ;
	return -1 ;
	}

// Attempt to find the channel in the network channel table
Channel* theChan = Network->findChannel( Param[ 1 ] ) ;

// Was the channel found?
if( NULL == theChan )
	{
	// The channel does not yet exist, go ahead and create it.
	theChan = new (nothrow)
		Channel( Param[ 1 ], atoi( Param[ 2 ] ) ) ;
	assert( theChan != 0 ) ;

	// Add the new Channel to the network channel table
	if( !Network->addChannel( theChan ) )
		{
		// The addition of this channel failed, *shrug*
		elog	<< "xServer::MSG_B> Failed to add channel: "
			<< Param[ 1 ] << endl ;

		// Prevent a memory leak by deleting the channel
		delete theChan ;

		// Return error
		return -1 ;
		}
	} // if( NULL == theChan )
else
	{
	// The channel was already found.
	// Make sure the timestamp is accurate, this is an oddity imho.
	time_t newCreationTime =
		static_cast< time_t >( ::atoi( Param[ 2 ] ) ) ;

	// Do the old TS and the new TS match?
	if( newCreationTime != theChan->getCreationTime() )
		{
		// Nope, update the timestamp
		theChan->setCreationTime( newCreationTime ) ;
		// TODO: Clear channel modes etc?
		}
	}

// Parse out the channel state
xParameters::size_type whichToken = 3 ;

// Channel modes will always be the first thing to follow if it's in the burst
if( '+' == Param[ whichToken ][ 0 ] )
	{
	// channel modes
	const char* currentPtr = Param[ whichToken ] ;

	// Skip over the '+'
	++currentPtr ;

	for( ; currentPtr && *currentPtr ; ++currentPtr )
		{
		switch( *currentPtr )
			{
			case 't':
				onChannelModeT( theChan, true, 0 ) ;
				break ;
			case 'n':
				onChannelModeN( theChan, true, 0 ) ;
				break ;
			case 'm':
				onChannelModeM( theChan, true, 0 ) ;
				break ;
			case 'p':
				onChannelModeP( theChan, true, 0 ) ;
				break ;
			case 's':
				onChannelModeS( theChan, true, 0 ) ;
				break ;
			case 'i':
				onChannelModeI( theChan, true, 0 ) ;
				break ;
 			case 'l':
				onChannelModeL( theChan, true, 0,
					::atoi( Param[ whichToken + 1 ] ) ) ;
				whichToken++ ;
				break ;
			case 'k':
				onChannelModeK( theChan, true, 0,
					Param[ whichToken + 1 ] ) ;
				whichToken++ ;
				break ;
			default:
				break ;
			} // switch

		} // for( currentPtr != endPtr )

	// Skip over the modes token
	// whichToken either points to the modes token if no +l/+k
	// was specified, or it points to the last +l/+k argument;
	// skip over this token no matter which.
	whichToken++ ;

	} // if( '+' == Param[ whichToken ][ 0 ]

// Have we reached the end of this burst command?
if( whichToken >= Param.size() )
	{
	return 0 ;
	}

// Parse the remaining tokens
for( ; whichToken < Param.size() ; ++whichToken )
	{
	// Bans will always be the last thing burst, so no users
	// will be burst afterwards.  This is useful because xParameters
	// will only delimit tokens by ':', so the ban string is guaranteed
	// to be caught.
	if( '%' == Param[ whichToken ][ 0 ] )
		{
		// Channel bans
		// Be sure to skip over the '%'
		parseBurstBans( theChan, Param[ whichToken ] + 1 ) ;
		}
	else
		{
		// Userlist
		parseBurstUsers( theChan, Param[ whichToken ] ) ;
		}
	}
return 0 ;
}

// dA1,jBN:ov,C3K:v,jGZ:o,CkU
// ':' indicates a mode state. Eg: ':ov' indicates this and
// all the following numerics are opped and voiced up to the next
// mode state.
// Mode states will always be in the order ov, v, o if present
// at all.
void xServer::parseBurstUsers( Channel* theChan, const char* theUsers )
{
// This is a protected method, so the method arguments are
// guaranteed to be valid

//clog	<< "xServer::parseBurstUsers> Channel: " << theChan->getName()
//	<< ", users: " << theUsers << endl ;

// Parse out users and their modes
StringTokenizer st( theUsers, ',' ) ;

// Used to track op/voice/opvoice mode state switches.
// 1 = op, 2 = voice, 3 = opvoice.
unsigned short int mode_state = 0;

vector< pair< bool, ChannelUser* > > opVector ;
vector< pair< bool, ChannelUser* > > voiceVector ;

for( StringTokenizer::const_iterator ptr = st.begin() ; ptr != st.end() ;
	++ptr )
	{
	// Each token is of the form:
	// abc or abc:modes
	string::size_type pos = (*ptr).find_first_of( ':' ) ;
 
	// Find the client in the client table
	iClient* theClient = Network->findClient( (*ptr).substr( 0, pos ) ) ;

	// Was the search successful?
	if( NULL == theClient )
		{
		// Nope, no such user
		// Log the error
		elog	<< "xServer::parseBurstUsers> ("
			<< theChan->getName() << ")"
			<< ": Unable to find client: "
			<< (*ptr).substr( 0, pos ) << endl ;

		// Skip this user
		continue ;
		}

//	elog	<< "xServer::parseBurstUsers> Adding user " << theClient->getNickName()
//		<< "(" << theClient->getCharYYXXX() << ") to channel "
//		<< theChan->getName() << endl ;

	// Create a ChannelUser object to represent this user's presence
	// in this channel
	ChannelUser* chanUser =
		new (nothrow) ChannelUser( theClient ) ;
	assert( chanUser != 0 ) ;

	// Add this channel to the user's channel structure.
	theClient->addChannel( theChan ) ;

	// Add this user to the channel's database.
	if( !theChan->addUser( chanUser ) )
		{
		// The addition failed
		elog	<< "xServer::parseBurstUsers> Unable to add user "
			<< theClient->getNickName() << " to channel "
			<< theChan->getName() << endl ;

		// Prevent a memory leak by deallocating the unused
		// ChannelUser object
		delete chanUser ;

		continue ;
		}

	// Notify the services clients that a user has
	// joined the channel
	PostChannelEvent( EVT_JOIN, theChan,
		static_cast< void* >( theClient ),
		static_cast< void* >( chanUser ) ) ;

	// Is there a ':' in this client's info?
	if( string::npos == pos )
		{
		// no ':' in this string, add the user with the current
		// MODE state.
		switch( mode_state )
			{
			case 1:
				opVector.push_back(
					opVectorType::value_type(
						true, chanUser ) ) ;
				break;
			case 2:
				voiceVector.push_back(
					voiceVectorType::value_type(
						true, chanUser ) ) ;
				break;
			case 3:
				opVector.push_back(
					opVectorType::value_type(
						true, chanUser ) ) ;
				voiceVector.push_back(
					voiceVectorType::value_type(
						true, chanUser ) ) ;
				break;
			}
 
		// mode_state still 0, not opped or voiced.
		continue ;
		}
 
	// Otherwise, user modes have been specified.
	for( pos++ ; pos < (*ptr).size() ; ++pos )
		{
		switch( (*ptr)[ pos ] )
			{
			case 'o':
				opVector.push_back(
					opVectorType::value_type(
						true, chanUser ) ) ;
				mode_state = 1;
				break ;
			case 'v':
				// Does the user already
				// have mode 'o'?
				if( 1 == mode_state )
					{
					// User has 'o' mode already
					opVector.push_back(
						opVectorType::value_type(
							true, chanUser ) ) ;
					}
				voiceVector.push_back(
					voiceVectorType::value_type(
						true, chanUser ) ) ;
				mode_state = (mode_state == 1) ? 3 : 2;
				break ;
			default:
				elog	<< "xServer::parseBurstUsers> "
					<< "Unknown mode: "
					<< (*ptr)[ pos ] << endl ;
				break ;
			} // switch
		} // for()

	} // while( ptr != st.end() )

// Commit the user modes to the internal tables, and notify
// all listening clients
if( !opVector.empty() )
	{
	onChannelModeO( theChan, 0, opVector ) ;
	}
if( !voiceVector.empty() )
	{
	onChannelModeV( theChan, 0, voiceVector ) ;
	}

}

void xServer::parseBurstBans( Channel* theChan, const char* theBans )
{
// This is a protected method, so the method arguments are
// guaranteed to be valid

//elog	<< "xServer::parseBurstBans> Found bans for channel "
//	<< theChan->getName()
//	<< ": "
//	<< theBans
//	<< endl ;

// Tokenize the ban string
StringTokenizer st( theBans ) ;

banVectorType banVector( st.size() ) ;

// Move through each token and add the ban
for( StringTokenizer::size_type i = 0 ; i < st.size() ; ++i )
	{
	banVector.push_back(
		banVectorType::value_type( true, st[ i ] ) ) ;
	}

if( !banVector.empty() )
	{
	onChannelModeB( theChan, 0, banVector ) ;
	}
}

} // namespace gnuworld
