
/*
 $Id: ccBadChannel.h,v 1.1 2002/05/25 15:03:58 mrbean_ Exp $
 */
 
#ifndef __CCBADCHANNEL_H_
#define __CCBADCHANNEL_H_

#include <libpq++.h>
#include <string>

using namespace std;

namespace gnuworld
{

namespace uworld
{

class ccBadChannel
{

public:
	ccBadChannel() {}
	
	ccBadChannel(PgDatabase* , unsigned int);
	
	ccBadChannel(const string& _Name, const string& _Reason , 
		     const string& _AddedBy) : Name(_Name),
		     Reason(_Reason),AddedBy(_AddedBy) {};
	
	~ccBadChannel(){}
	
	bool Update(PgDatabase*);
	
	bool Insert(PgDatabase*);
	
	bool Delete(PgDatabase*);
	
	const string& getName() const
	{ return Name; }
	
	const string& getReason() const
	{ return Reason; }

	const string& getAddedBy() const
	{ return AddedBy; }
	
	void setName(const string& _Name)
	{ Name = _Name; }
	
	void setReason(const string& _Reason)
	{ Reason = _Reason; }
	
	void setAddedBy(const string& _AddedBy)
	{ AddedBy = _AddedBy; }
	
private:
	
	string Name;
	
	string Reason;
	
	string AddedBy;
	
	
};	

}

}

#endif 
