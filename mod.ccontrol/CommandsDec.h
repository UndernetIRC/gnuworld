/**
 * CommandsDec.h
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
 * $Id: CommandsDec.h,v 1.22 2009/06/13 06:43:34 hidden1 Exp $
 */

#ifndef __COMMANDSDEC_H
#define __COMMANDSDEC_H "$Id: CommandsDec.h,v 1.22 2009/06/13 06:43:34 hidden1 Exp $"

namespace gnuworld
{

namespace uworld
{
class Command;

#define DECLARE_COMMAND(commName) \
class commName##Command : public Command \
{ \
public: \
	commName##Command( ccontrol* _bot, \
		const string& _commName, \
		const string& _help,  \
		const bool _needDB, \
	        int _flags , bool isDisabled, \
		bool needOp, bool noLog, \
		int minLevel , bool secondAccess ) \
	: Command( _bot, _commName, _needDB, _help,_flags, \
		isDisabled,needOp,noLog,minLevel,secondAccess) \
	{} \
	virtual bool Exec( iClient*, const string&) ; \
	virtual ~commName##Command() {} \
} ;

DECLARE_COMMAND( INVITE )
DECLARE_COMMAND( HELP )
DECLARE_COMMAND( JUPE )
DECLARE_COMMAND( MODE )
DECLARE_COMMAND( GLINE )
DECLARE_COMMAND( SCHANGLINE )
DECLARE_COMMAND( SCANGLINE )
DECLARE_COMMAND( REMGLINE )
DECLARE_COMMAND( TRANSLATE )
DECLARE_COMMAND( WHOIS )
DECLARE_COMMAND( KICK )
DECLARE_COMMAND( ADDOPERCHAN )
DECLARE_COMMAND( REMOPERCHAN )
DECLARE_COMMAND( LISTOPERCHANS )
DECLARE_COMMAND( CHANINFO )
DECLARE_COMMAND( LOGIN )
DECLARE_COMMAND( DEAUTH )
DECLARE_COMMAND( ADDUSER )
DECLARE_COMMAND( REMUSER )
DECLARE_COMMAND( ADDCOMMAND )
DECLARE_COMMAND( REMCOMMAND )
DECLARE_COMMAND( NEWPASS )
DECLARE_COMMAND( SUSPEND )
DECLARE_COMMAND( UNSUSPEND )
DECLARE_COMMAND( MODUSER )
DECLARE_COMMAND( MODERATE )
DECLARE_COMMAND( UNMODERATE )
DECLARE_COMMAND( OP )
DECLARE_COMMAND( DEOP )
DECLARE_COMMAND( LISTHOSTS )
DECLARE_COMMAND( CLEARCHAN )
DECLARE_COMMAND( ADDSERVER )
DECLARE_COMMAND( LEARNNET )
DECLARE_COMMAND( REMSERVER )
DECLARE_COMMAND( CHECKNET )
DECLARE_COMMAND( LASTCOM )
DECLARE_COMMAND( FORCEGLINE )
DECLARE_COMMAND( EXCEPTION )
DECLARE_COMMAND( LISTIGNORES )
DECLARE_COMMAND( REMOVEIGNORE )
DECLARE_COMMAND( LIST )
DECLARE_COMMAND( COMMANDS )
DECLARE_COMMAND( GCHAN )
DECLARE_COMMAND( REMGCHAN )
DECLARE_COMMAND( USERINFO )
DECLARE_COMMAND( STATUS )
DECLARE_COMMAND( SHUTDOWN )
DECLARE_COMMAND( SCAN )
DECLARE_COMMAND( MAXUSERS )
DECLARE_COMMAND( CONFIG )
DECLARE_COMMAND( SAY )
DECLARE_COMMAND( NOMODE )
DECLARE_COMMAND( REOP )
DECLARE_COMMAND( SGLINE )
DECLARE_COMMAND( REMSGLINE )
DECLARE_COMMAND( UNJUPE )
DECLARE_COMMAND ( FORCECHANGLINE )
DECLARE_COMMAND ( SHELLS )
DECLARE_COMMAND ( ANNOUNCE )
DECLARE_COMMAND (LISTUSERS)
} // namespace uworld

} // namespace gnuworld

#endif // __COMMANDSDEC_H

