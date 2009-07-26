/**
 * CControlCommands.h
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
 * $Id: CControlCommands.h,v 1.34 2009/07/26 18:30:37 mrbean_ Exp $
 */

#ifndef __CCONTROLCOMMANDS_H
#define __CCONTROLCOMMANDS_H "$Id: CControlCommands.h,v 1.34 2009/07/26 18:30:37 mrbean_ Exp $"

#include	<string>

#include	"iClient.h"

using std::string ;

namespace gnuworld
{

class xServer ;

namespace uworld
{

class ccontrol ;

class Command
{

public:
	Command( ccontrol* _bot, const string& _commName,
		const bool _needsDB,
		const string& _help, const unsigned long _flags,
		const bool _disabled, const bool _needOp,
		const bool _noLog,const int unsigned _minLevel,
		const bool _secondAccess)
	 : bot( _bot ),
	   server( 0 ),
	   commName( _commName ),
	   commRealName( _commName ),
	   needDB ( _needsDB ),
	   help( _help ),
	   flags ( _flags ),
	   isDisabled ( _disabled ),
	   needOp ( _needOp),
	   noLog ( _noLog ),
	   minLevel ( _minLevel ),
	   secondAccess ( _secondAccess )
	{}
	virtual ~Command() {}

	/// Exec returns true if the command was successfully
	/// executed, false otherwise.
	virtual bool Exec( iClient*, const string&) = 0 ;

	void	setServer( xServer* _server )
		{ server = _server ; }
	virtual string getInfo() const
		{ return commName + ' ' + help ; }
	virtual void Usage( iClient* theClient ) ;

	void	setName( const string& _name )
		{ commName = string_upper( _name ); }
	
	void	Disable()
		{ isDisabled = true ; }
	
	void 	Enable()
		{ isDisabled = false ; }
		
	void 	setNeedOp ( const bool _needOp )
		{ needOp = _needOp ; }

	void 	setNoLog ( const bool _noLog )
		{ noLog = _noLog ; }
		
	void	setMinLevel ( const int unsigned _minLevel )
		{ minLevel = _minLevel ; }
		
	inline const string& getName() const
		{ return commName ; }
	
	inline const string& getRealName() const
		{ return commRealName ; }
	
	inline const bool& needsDB() const
		{ return needDB ; }

	inline const string& getHelp() const
		{ return help ; }

    inline const unsigned long& getFlags() const
	    { return flags ; }	

	inline const bool& getIsDisabled() const
		{ return isDisabled ; }

	inline const bool& getNeedOp() const
		{ return needOp ; }

	inline const bool& getNoLog() const
		{ return noLog ; }

	inline const unsigned int& getMinLevel() const
		{ return minLevel ; }

	inline const bool& getSecondAccess() const
		{ return secondAccess ; }				
protected:
	ccontrol*	bot ;
	xServer*	server ;
	string		commName ;
	string		commRealName ;
	bool		needDB;
	string		help ;
	unsigned long   flags;
	bool		isDisabled;
	bool		needOp;
	bool		noLog;
	unsigned int	minLevel;
	bool		secondAccess;
} ;
}

} // namespace gnuworld

#endif // __CCONTROLCOMMANDS_H
