/**
 * AI.cc
 * Artificial Intelligence unit.
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
 * $Id: AI.cc,v 1.2 2003/06/28 01:21:20 dan_karrels Exp $
 */

#include	<universe.h>
#include	"sqlUser.h"

const char AICommand_cc_rcsId[] = "$Id: AI.cc,v 1.2 2003/06/28 01:21:20 dan_karrels Exp $" ;

namespace gnuworld
{

bool AICommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.AI");

/*
 *  a2b57e6118e750f3b8cde7fa8d7c4114
 */

return true ;
}

} // namespace gnuworld.
