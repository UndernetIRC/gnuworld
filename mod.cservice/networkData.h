/* networkData.h */

#ifndef __NETWORKDATA_H
#define __NETWORKDATA_H "$Id: networkData.h,v 1.1 2001/01/08 04:13:04 gte Exp $"

#include	<string>
#include	<ctime>

class sqlUser;
 
using std::string ;

namespace gnuworld
{ 
 
class networkData
{

public:

	networkData() ;
	virtual ~networkData() ; 

 	// Timestamp of when last message was recieved from this user.
	unsigned int lastMessage;

	// Count of all bytes outputted to this client.
	unsigned int outputCount;

	// Number of messages recieved since lastMessage.
	unsigned int inputCount;

	// Pointer to the sqlUser record of this user (if it exists).
	// Ie: If they're authed.
	sqlUser* currentUser; 

	// True if this user is currently being ignored.
	bool ignored;
} ;

#endif // __NETWORKDATA_H

} // Namespace gnuworld
