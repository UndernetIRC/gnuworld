/**
 * msg_B.cc
 * Author: Daniel Karrels (dan@karrels.com)
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
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
 * $Id: msg_B.cc,v 1.12 2008/04/16 20:29:37 danielaustin Exp $
 */

#include	<sys/types.h>
#include	<sys/time.h>

#include	<new>
#include	<map>
#include	<string>
#include	<vector>
#include	<iostream>
#include	<utility>

#include	<cassert>

#include	"gnuworld_config.h"
#include	"server.h"
#include	"xparameters.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"Channel.h"
#include	"ChannelUser.h"
#include	"Network.h"
#include	"iClient.h"
#include	"ServerCommandHandler.h"


namespace gnuworld
{
using std::pair ;
using std::make_pair ;
using std::string ;
using std::vector ;
using std::endl ;

class msg_B : public ServerCommandHandler
{
public:
	msg_B( xServer* theServer )
	 : ServerCommandHandler( theServer )
	{}
	virtual ~msg_B()
	{}

	virtual bool	Execute( const xParameters& ) ;

protected:
	void	parseBurstUsers( Channel*, const string& ) ;
	void	parseBurstBans( Channel*, const string& ) ;
	
} ;

CREATE_LOADER(msg_B)

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
// Here is a special case, when the only occupant of the channel
// is a zombie:
// BG B #nails 1036089823
// We ignore this case.
//
// With u2.10.12's oplevels, we have to deal with an extra case:
// <numeric> B <channel> <ts> <modes...> <numnick>:<oplevel>
// Ck B #test 1128024142 +tnAU apass upass BBAAA:999
//
bool msg_B::Execute( const xParameters& Param )
{
// Make sure there are at least four arguments supplied:
// servernumeric #channel time_stamp arguments
if( Param.size() < 3 )
	{
	elog	<< "msg_B> Invalid number of arguments: "
		<< Param
		<< endl ;
	return false ;
	}

// Attempt to find the channel in the network channel table
Channel* theChan = Network->findChannel( Param[ 1 ] ) ;

// Was the channel found?
if( NULL == theChan )
	{
	// The channel does not yet exist, go ahead and create it.
	theChan = new (std::nothrow)
		Channel( Param[ 1 ], atoi( Param[ 2 ] ) ) ;
	assert( theChan != 0 ) ;

	// Add the new Channel to the network channel table
	if( !Network->addChannel( theChan ) )
		{
		// The addition of this channel failed, *shrug*
		elog	<< "msg_B> Failed to add channel: "
			<< Param[ 1 ]
			<< endl ;

		// Prevent a memory leak by deleting the channel
		delete theChan ; theChan = 0 ;

		// Return error
		return false ;
		}
	} // if( NULL == theChan )
else
	{
	// The channel was already found.
	// Make sure the timestamp is accurate, this is an oddity imho.
	time_t newCreationTime =
		static_cast< time_t >( ::atoi( Param[ 2 ] ) ) ;

	// Is the old TS greater than the new TS?
	if( theChan->getCreationTime() > newCreationTime )
		{
		// Nope, update the timestamp
		theChan->setCreationTime( newCreationTime ) ;
		theChan->removeAllModes() ;
		theChan->removeAllBans() ;
		}
	}

if( 3 == Param.size() )
	{
	// Zombie in channel.
	// If the user is dezombified, the client will be shown
	// to issue a "J", and the channel will be created anyway.
	// Only difference is the timestamp difference.
	return true ;
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

	xServer::modeVectorType modeVector ;

	for( ; currentPtr && *currentPtr ; ++currentPtr )
		{
		switch( *currentPtr )
			{
			case 't':
				modeVector.push_back( make_pair(
					true, Channel::MODE_T ) ) ;
				break ;
			case 'n':
				modeVector.push_back( make_pair(
					true, Channel::MODE_N ) ) ;
				break ;
			case 'm':
				modeVector.push_back( make_pair(
					true, Channel::MODE_M ) ) ;
				break ;
			case 'p':
				modeVector.push_back( make_pair(
					true, Channel::MODE_P ) ) ;
				break ;
			case 's':
				modeVector.push_back( make_pair(
					true, Channel::MODE_S ) ) ;
				break ;
			case 'i':
				modeVector.push_back( make_pair(
					true, Channel::MODE_I ) ) ;
				break ;
			case 'r':
				modeVector.push_back( make_pair(
					true, Channel::MODE_R ) ) ;
				break ;
			case 'R':
				modeVector.push_back( make_pair(
					true, Channel::MODE_REG ) ) ;
				break ;
			case 'D':
				modeVector.push_back( make_pair(
					true, Channel::MODE_D ) ) ;
				break ;
			case 'c':
				modeVector.push_back( make_pair(
					true, Channel::MODE_C ) ) ;
				break ;
			case 'C':
				modeVector.push_back( make_pair(
					true, Channel::MODE_CTCP ) ) ;
				break ;
			case 'P':
				modeVector.push_back( make_pair(
					true, Channel::MODE_PART ) ) ;
				break ;
			case 'M':
				modeVector.push_back( make_pair(
					true, Channel::MODE_MNOREG ) ) ;
				break ;
 			case 'l':
				theServer->OnChannelModeL( theChan, true, 
					0,
					::atoi( Param[ whichToken + 1 ] ) ) ;
				whichToken++ ;
				break ;
			case 'k':
				theServer->OnChannelModeK( theChan, true, 
					0,
					Param[ whichToken + 1 ] ) ;
				whichToken++ ;
				break ;
			case 'A':
				theServer->OnChannelModeA( theChan, true,
					0,
					Param[ whichToken + 1 ] ) ;
				whichToken++ ;
				break ;
			case 'U':
				theServer->OnChannelModeU( theChan, true,
					0,
					Param[ whichToken + 1 ] ) ;
				whichToken++ ;
				break ;
			default:
				break ;
			} // switch

		} // for( currentPtr != endPtr )

	if( !modeVector.empty() )
		{
		theServer->OnChannelMode( theChan, 0, modeVector ) ;
		}

	// Skip over the modes token
	// whichToken either points to the modes token if no +l/+k
	// was specified, or it points to the last +l/+k argument;
	// skip over this token no matter which.
	whichToken++ ;

	} // if( '+' == Param[ whichToken ][ 0 ]

// Have we reached the end of this burst command?
if( whichToken >= Param.size() )
	{
	return true ;
	}

// Parse the remaining tokens
for( ; whichToken < Param.size() ; ++whichToken )
	{
	// Bans will always be the last thing burst, so no users
	// will be burst afterwards.  This is useful because xParameters
	// will only delimit tokens by ':', so the ban string is guaranteed
	// to be caught.
	if( '~' == Param[ whichToken ][ 0 ] )
		{
		// Channel ban exceptions
		// Be sure to skip over the '%'
		//parseBurstExcepts( theChan, Param[ whichToken ] + 1 ) ;
		}
	else if( '%' == Param[ whichToken ][ 0 ] )
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
return true ;
}

// dA1,jBN:ov,C3K:v,jGZ:o,CkU
// ':' indicates a mode state. Eg: ':ov' indicates this and
// all the following numerics are opped and voiced up to the next
// mode state.
// Mode states will always be in the order ov, v, o if present
// at all.
void msg_B::parseBurstUsers( Channel* theChan, const string& theUsers )
{
// This is a protected method, so the method arguments are
// guaranteed to be valid
string chanName = theChan->getName();  // Added for fixing crash when Channel gets destroyed in the result of PostChannelEvent() below
//elog	<< "msg_B::parseBurstUsers> Channel: " << theChan->getName()
//	<< ", users: " << theUsers << endl ;
// Parse out users and their modes
StringTokenizer st( theUsers, ',' ) ;

// Used to track op/voice/opvoice mode state switches.
// 1 = op, 2 = voice, 3 = opvoice.
unsigned short int mode_state = 0;

typedef xServer::opVectorType opVectorType ;
typedef xServer::voiceVectorType voiceVectorType ;

opVectorType opVector ;
voiceVectorType voiceVector ;

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
		elog	<< "msg_B::parseBurstUsers> ("
			<< theChan->getName() << ")"
			<< ": Unable to find client: "
			<< (*ptr).substr( 0, pos )
			<< endl ;

		// Skip this user
		continue ;
		}

//	elog	<< "msg_B::parseBurstUsers> Adding user "
//		<< theClient->getNickName()
//		<< "(" << theClient->getCharYYXXX() << ") to channel "
//		<< theChan->getName() << endl ;

	// Add this channel to the user's channel structure.
	if( !theClient->addChannel( theChan ) )
		{
		elog	<< "msg_B::parseBurstUsers> Failed to add "
			<< "channel "
			<< *theChan
			<< " to iClient "
			<< *theClient
			<< endl ;

		// Non-fatal error
		continue ;
		}

	// Create a ChannelUser object to represent this user's presence
	// in this channel
	ChannelUser* chanUser =
		new (std::nothrow) ChannelUser( theClient ) ;
	assert( chanUser != 0 ) ;

	// Add this user to the channel's database.
	if( !theChan->addUser( chanUser ) )
		{
		// The addition failed
		elog	<< "msg_B::parseBurstUsers> Unable to add user "
			<< theClient->getNickName()
			<< " to channel "
			<< theChan->getName()
			<< endl ;

		// Prevent a memory leak by deallocating the unused
		// ChannelUser object
		delete chanUser ; chanUser = 0 ;

		// Remove the channel info from the client
		theClient->removeChannel( theChan ) ;

		continue ;
		}

	// Notify the services clients that a user has
	// joined the channel
	theServer->PostChannelEvent( EVT_JOIN, theChan,
		static_cast< void* >( theClient ),
		static_cast< void* >( chanUser ) ) ;

	// Check if the Channel and User both exist after PostChannelEvent()
	// Has to be done to prevent a crash for cases where PostChannelEvent() results in kicking the burst user right in the middle of this function.
	// If the user was alone in the channel, the channel is destroyed and referenced later, causing a segmentation fault.
	Channel* tmpChan = Network->findChannel( chanName );
	if (tmpChan == 0)
		continue;
	if (tmpChan->findUser(theClient) == 0)
		continue;

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
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				/* This is an oplevel, which we are not (currently) interested in.
				 * However, we also don't want it to cause an error!
				 */
				break;
			default:
				elog	<< "msg_B::parseBurstUsers> "
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
	theServer->OnChannelModeO( theChan, 0, opVector ) ;
	}
if( !voiceVector.empty() )
	{
	theServer->OnChannelModeV( theChan, 0, voiceVector ) ;
	}
}

void msg_B::parseBurstBans( Channel* theChan, const string& theBans )
{
// This is a protected method, so the method arguments are
// guaranteed to be valid

//elog	<< "msg_B::parseBurstBans> Found bans for channel "
//	<< theChan->getName()
//	<< ": "
//	<< theBans
//	<< endl ;

// Tokenize the ban string
StringTokenizer st( theBans ) ;

typedef xServer::banVectorType banVectorType ;
banVectorType banVector( st.size() ) ;

// Move through each token and add the ban
for( StringTokenizer::size_type i = 0 ; i < st.size() ; ++i )
	{
	banVector.push_back(
		banVectorType::value_type( true, st[ i ] ) ) ;
	}

if( !banVector.empty() )
	{
	theServer->OnChannelModeB( theChan, 0, banVector ) ;
	}
}

} // namespace gnuworld
