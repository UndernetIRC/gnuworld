/**
 * LASTCOMCommand.cc
 *
 * 18/04/2006 - Neil Spierling <sirvulcan@gmail.com>
 * Based on mod.ccontrol LASTCOM command.
 *
 * Sends a list of the last X commands.
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
 * $Id: LASTCOMCommand.cc,v 1.4 2008/01/16 02:03:37 buzlip01 Exp $
 */

#include	<sstream>
#include	<string>
#include	<iostream>
#include        <iomanip>

#include	<cstdlib>

#include	"chanfix.h"
#include	"chanfixCommands.h"
#include        "defs.h"
#include        "responses.h"
#include        "sqlChanOp.h"
#include        "sqlChannel.h"
#include        "sqlcfUser.h"

#include        "Network.h"
#include	"iClient.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"gnuworld_config.h"

RCSTAG( "$Id: LASTCOMCommand.cc,v 1.4 2008/01/16 02:03:37 buzlip01 Exp $" ) ;

namespace gnuworld
{

namespace cf
{

void LASTCOMCommand::Exec(iClient* theClient, sqlcfUser* theUser, const std::string& Message)
{
StringTokenizer st( Message ) ;
unsigned int NumOfCom;
unsigned int Days = 0;
std::stringstream theQuery;

dbHandle* cacheCon = bot->getLocalDBHandle();

if (st.size() == 1 ) {
  NumOfCom = 20;
  static const char* queryHeader = "SELECT * FROM comlog ";
  theQuery 	<< queryHeader 
		<< " ORDER BY ts DESC"
		<< " LIMIT " << NumOfCom
		<< std::ends;
} else 	{
  NumOfCom = atoi(st[1].c_str());
  if (st.size() > 2) {
    Days = atoi(st[2].c_str());
    if (Days > 365) {
      bot->SendTo(theClient,
                  bot->getResponse(theUser,
                                   language::lastcom_too_far_back,
                                   std::string("You cannot see logs more than one year ago.")).c_str());
      return;
    }
    static const char* queryHeader = "SELECT * FROM comlog where ts >";
    theQuery 	<< queryHeader 
		<< (::time(0) - Days * 24 * 3600)
		<< " ORDER BY ts ASC"
		<< " LIMIT " << NumOfCom
		<< std::ends;
  } else {	
    static const char* queryHeader = "SELECT * FROM comlog ";
    theQuery 	<< queryHeader 
		<< " ORDER BY ts DESC"
		<< " LIMIT " << NumOfCom
		<< std::ends;
  }
}

elog	<< "chanfix::LASTCOM> " 
	<< theQuery.str().c_str() 
	<< std::endl;
	
if (!cacheCon->Exec(theQuery.str(),true)) {
  elog	<< "chanfix::LASTCOM> SQL Error: "
	<< cacheCon->ErrorMessage()
	<< std::endl ;
  return;
}

// SQL Query succeeded
bot->SendTo(theClient,
            bot->getResponse(theUser,
                             language::lastcom_listing_messages,
                             std::string("Listing last %d messages from day %d.")).c_str(),
			     NumOfCom,Days);

// TODO: Fix this warning
for (int i = (cacheCon->Tuples() - 1) ; i >= 0; i--) {
  bot->SendTo(theClient,"[ %s - %s ] %s",
	bot->convertToAscTime(atoi(cacheCon->GetValue(i, 0))),
	cacheCon->GetValue(i,1).c_str(),
	cacheCon->GetValue(i,2).c_str());
}

bot->SendTo(theClient,
            bot->getResponse(theUser,
                             language::end_of_log,
                             std::string("End of LASTCOM report.")).c_str());

/* Dispose of our connection instance */
//bot->theManager->removeConnection(cacheCon);

bot->logLastComMessage(theClient, Message);

return;
}

}
}

