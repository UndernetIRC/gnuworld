/*
 * sqlPendingTraffic Class Header.
 *
 * Stores information about the uniqueness of visitors to
 * pending channels.
 */

#ifndef __SQLPENDINGTRAFFIC_H
#define __SQLPENDINGTRAFFIC_H "$Id: sqlPendingTraffic.h,v 1.2 2001/06/13 20:22:07 gte Exp $"

#include	<string> 
 
using std::string ;

namespace gnuworld
{ 
 
class sqlPendingTraffic
{

public:
	sqlPendingTraffic(PgDatabase*);
	bool insertRecord();
	bool commit();

	unsigned int channel_id; 
	unsigned int ip_number;
	unsigned int join_count; 

	PgDatabase*	SQLDb;
};

}
#endif // __SQLPENDINGTRAFFIC_H 
