/**
 * LOGINCommand.cc
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
 * $Id: LOGINCommand.cc,v 1.67 2009/06/09 15:40:29 mrbean_ Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>
#include	<iomanip>
#include	<inttypes.h>
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"md5hash.h"
#include	"responses.h"
#include	"levels.h"
#include	"networkData.h"
#include	"cservice_config.h"
#include	"Network.h"
#include	"ip.h"

namespace gnuworld
{

bool LOGINCommand::Exec( iClient* theClient, const string& Message )
{
StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

/*
 * Check theClient isn't already logged in, if so, tell
 * them they shouldn't be.
 */

sqlUser* tmpUser = bot->isAuthed(theClient, false);
if (tmpUser)
	{
	bot->Notice(theClient,
		bot->getResponse(tmpUser, language::already_authed).c_str(),
		tmpUser->getUserName().c_str());
	return false;
	}

cservice::AuthStruct auth = {
	AuthType::LOGIN, 				// auth type
	cservice::AUTH_ERROR,				// result (placeholder)
	st[ 1 ],					// username
	st.assemble( 2 ),				// password/token
	theClient->getUserName(),			// ident
	xIP( theClient->getIP() ).GetNumericIP(),	// ip
	theClient->getTlsFingerprint(),			// tls fingerprint
	nullptr,					// sqlUser (placeholder)
	theClient,					// iClient
	{}						// no sasl
} ;

cservice::AuthResult auth_res = bot->authenticateUser( auth ) ;
auth.result = auth_res ;
bot->processAuthentication( auth ) ;

return true;
}

} // namespace gnuworld.

