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

namespace gnuworld
{

/**
 * A simple proof-of-concept client to test the feasibility of
 * snooping public messages from particular clients with 'hidden'
 * clients (fake clients spawned with a fake username and hostname).
 * Took about 2.5 hours to get the spawning and snooping working.
 * This will probably be migrated into dronescan at some point, but
 * perhaps not.
 */
class snoop : public xClient
{
public:
	/**
	 * Standard xClient constructor, receives the name of the
	 * configuration file.
	 */
	snoop( const std::string& confFileName ) ;

	/**
	 * Destructor performs the cleanup.
	 */
	virtual ~snoop() ;

	/**
	 * Request that the client join the admin and relay channels.
	 * This method is called by the xServer after being fully
	 * connected.
	 */
	virtual void	BurstChannels() ;

	/**
	 * The channel message handler, used to accept public channel
	 * commands from registered users (or opers) in the admin
	 * channel.
	 */
	virtual void	OnChannelMessage( iClient*,
				Channel*,
				const std::string& ) ;

	/**
	 * This method invoked when a fake client is to see a
	 * public channel message.  This occurs in all channels in
	 * which a fake clients resides (not just the admin or snooped
	 * channels).
	 */
	virtual void	OnFakeChannelMessage( iClient*,
				iClient*,
				Channel*,
				const std::string& ) ;

protected:

	/**
	 * Return true if the given string represents a valid
	 * nickname.  This is rather trivial, and probably not
	 * entirely correct, but hopefully it will avoid some
	 * simple typos and types of abuse.
	 */
	bool		validNickname( const std::string& ) const ;

	/**
	 * Notify the client of the proper usage of the given
	 * command identified by the string.
	 */
	void		usage( iClient*, const std::string& ) ;

	/**
	 * Spawn a client for the given iClient.
	 * This should be replaced soon.
	 */
	void		handleSpawnClient( iClient*,
				Channel*,
				const StringTokenizer& ) ;

	/**
	 * Have a spawned client join a channel.
	 * This should be replaced soon.
	 */
	void		handleSpawnJoin( iClient*,
				Channel*,
				const StringTokenizer& ) ;

	/**
	 * Have a spawned client part a channel.
	 * This should be replaced soon.
	 */
	void		handleSpawnPart( iClient*,
				Channel*,
				const StringTokenizer& ) ;

	/**
	 * Have a spawned client quit.
	 * This should be replaced soon.
	 */
	void		handleSpawnQuit( iClient*,
				const StringTokenizer& ) ;

	/**
	 * The command char (string) to which the bot responds
	 * in the admin channel.
	 */
	std::string	cmdchar ;

	/**
	 * The name of the admin channel.  Channel commands are only
	 * accepted from this channel.
	 * This channel is auto joined.
	 */
	std::string	adminChanName ;

	/**
	 * The channel to which to relay snooped messages
	 * This channel is auto joined.
	 */
	std::string	relayChanName ;

	/**
	 * The default quit message for spawned clients.
	 */
	std::string	defaultQuitMessage ;

	/**
	 * The maximum nickname length for the network on which this
	 * client runs.  This is used in determining if a nickname
	 * is valid.
	 */
	size_t		maxnicklen ;

} ;

} // namespace gnuworld

#endif // __SNOOP_H
