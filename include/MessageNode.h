/* MessageNode.h
 */

#ifndef __MESSAGENODE_H
#define __MESSAGENODE_H "$Id: MessageNode.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $"

#include	<string>

using std::string ;

namespace gnuworld
{

/**
 * This class is used to store private messages commands
 * during burst time.  At burst time, no private messages
 * may be processed.  Instead, they are stored in a queue
 * of MessageNode's.  When the local burst is complete,
 * the message queue for each services client is processed.
 * This class is immutable.
 */
class MessageNode
{
public:

	/**
	 * Construct a new MessageNode object given the user's
	 * nickUserHost string and the message text.
	 */
	MessageNode( const string& _nickUserHost, const string& _Message )
	: nickUserHost( _nickUserHost ),
	  Message( _Message ) {}

	/**
	 * Construct a new MessageNode object given the user's
	 * iClient object pointer and the message text.
	 */
	MessageNode( const iClient* theClient, const string& _Message )
	: nickUserHost( theClient->getNickName() + '!' +
	   theClient->getUserName() + '@' + theClient->getInsecureHost() ),
	  Message( _Message ) {}

	/**
	 * Copy constructor.
	 */
	MessageNode( const MessageNode& rhs )
	: nickUserHost( rhs.nickUserHost ), Message( rhs.Message ) {}

	/**
	 * Destruct the MessageNode object.  This method doesn't need to
	 * do anything because no streams have been opend, and no
	 * dynamic memory allocated.
	 */
	virtual ~MessageNode() {}

	/**
	 * Return the nickUserHost of the user.
	 */
	inline const string& getNickUserHost() const
		{ return nickUserHost ; }

	/**
	 * Return the message the user sent.
	 */
	inline const string& getMessage() const
		{ return Message ; }

protected:

	/**
	 * The default constructor is implicitly disabled
	 * by the presence of any other constructor.
	 */

	/**
	 * Disable assignment, this method is declared, but NOT defined.
	 */
	MessageNode& operator=( const MessageNode& ) ;

	/**
	 * The user's nickname/username/hostname string.
	 */
	string			nickUserHost ;

	/**
	 * The message sent to the xClient service bot.
	 */
	string			Message ;
 
} ;

} // namespace gnuworld

#endif /* __MESSAGENODE_H */
