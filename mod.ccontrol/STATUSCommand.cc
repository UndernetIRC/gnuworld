/*
 * STATUSCommand.cc
 *
 * Shows debug status
 *
 */

#include	<string>
#include	<cstdlib>
#include        <iomanip.h>
#include	"ccontrol.h"
#include	"CControlCommands.h"
//#include	"StringTokenizer.h"

const char STATUSCommand_cc_rcsId[] = "$Id: STATUSCommand.cc,v 1.1 2001/11/21 20:54:40 mrbean_ Exp $";

namespace gnuworld
{

using std::string ;

namespace uworld
{

bool STATUSCommand::Exec( iClient* theClient, const string& Message)
{	 

bot->showStatus(theClient);
return true ;
}

}
}

