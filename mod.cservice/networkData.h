/* networkData.h */

#ifndef __NETWORKDATA_H
#define __NETWORKDATA_H "$Id: networkData.h,v 1.2 2001/01/10 01:46:10 gte Exp $"

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
	time_t messageTime;

	// Count of all bytes outputted to this client.
	unsigned int outputCount;

	// Total flood points accumulated by input from this client.
	unsigned int flood_points;

	// Pointer to the sqlUser record of this user (if it exists).
	// Ie: If they're authed.
	sqlUser* currentUser; 

	// True if this user is currently being ignored.
	bool ignored;
} ;

#endif // __NETWORKDATA_H

} // Namespace gnuworld
