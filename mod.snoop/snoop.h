/**
 * snoop.h
 */

#ifndef __SNOOP_H
#define __SNOOP_H

#include	<string>

#include	"client.h"
#include	"Channel.h"
#include	"iClient.h"
#include	"EConfig.h"
#include	"StringTokenizer.h"

using std::string ;

namespace gnuworld
{

class snoop : public xClient
{
public:
	snoop( const string& confFileName ) ;
	virtual ~snoop() ;

	virtual bool	BurstChannels() ;
	virtual void	OnChannelMessage( iClient*,
				Channel*,
				const string& ) ;
	virtual void	OnFakeChannelMessage( iClient*,
				iClient*,
				Channel*,
				const string& ) ;

protected:
	void		usage( iClient*, const string& ) ;
	void		handleSpawnClient( iClient*,
				Channel*,
				const StringTokenizer& ) ;
	void		handleSpawnJoin( iClient*,
				Channel*,
				const StringTokenizer& ) ;
	void		handleSpawnPart( iClient*,
				Channel*,
				const StringTokenizer& ) ;
	void		handleSpawnQuit( iClient*,
				const StringTokenizer& ) ;

	string		cmdchar ;
	string		adminChanName ;
	string		relayChanName ;
	string		defaultQuitMessage ;

} ;

} // namespace gnuworld

#endif // __SNOOP_H
