/* ModeChange.h */

#ifndef __MODECHANGE_H
#define __MODECHANGE_H "$Id: ModeChange.h,v 1.1 2000/12/20 00:49:30 dan_karrels Exp $"

#include	"iClient.h"
#include	"ChannelUser.h"
#include	"Ban.h"

class xServer ;

class ModeChange
{
	friend class xServer ;

public:
	ModeChange()
	 : polarity( false ),
	   whichMode( 0 ),
	   sourceClient( 0 ),
	   targetClient( 0 ),
	   sourceUser( 0 ),
	   targetUser( 0 ),
	   theBan( 0 )
	{}
	ModeChange( const ModeChange& rhs )
	 : polarity( rhs.polarity ),
	   whichMode( rhs.whichMode ),
	   sourceClient( rhs.sourceClient ),
	   targetClient( rhs.targetClient ),
	   sourceUser( rhs.sourceUser ),
	   targetUser( rhs.targetUser ),
	   theBan( rhs.theBan )
	{}
	virtual ~ModeChan()
	{}

	inline bool getPolarity() const
		{ return polarity ; }
	inline char getMode() const
		{ return whichMode ; }
	inline iClient* getSourceClient() const
		{ return sourceClient ; }
	inline iClient* getTargetClient() const
		{ return targetClient ; }
	inline ChannelUser* getSourceUser() const
		{ return sourceUser ; }
	inline ChannelUser* getTargetUser() const
		{ return targetUser ; }
	inline const Ban* getTheBan() const
		{ return theBan ; }

protected:
	bool		polarity ;
	char		whichMode ;

	iClient		*sourceClient,
			*targetClient ;
	ChannelUser	*sourceUser,
			*targetUser ;
	Ban		*theBan ;

} ;

#endif // __MODECHANGE_H
