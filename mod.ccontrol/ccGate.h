
#ifndef __CCGATE_H 
#define __CCGATE_H

#include <string>
#include <pthread.h>

namespace gnuworld
{

namespace uworld
{

class ccGate
{

public:

	ccGate(string _Host = "" , int _Port = 0);
	
	
	virtual ~ccGate();
	
	static const unsigned int statReady = 0x01;
	
	static const unsigned int statConnecting = 0x02;
	
	static const unsigned int statConnected = 0x04;
	
	static const unsigned int statWaitingForReply = 0x08;
	
	static const unsigned int statDisconnecting = 0x10;
	
	static const unsigned int statDone = 0x20;
	
	inline const string& getHost() const
		{ return Host; }
	
	inline const unsigned int getPort() const
		{ return Port; }

	inline const unsigned int getStatus() const
		{ return Status; }
		
	inline const bool getFound() const
		{ return Found; }

	inline const pthread_t getThreadId() const
		{ return threadId; }
		
	inline void setHost( const string _Host)
		{ Host = _Host; }
		
	inline void setPort( const unsigned int _Port)
		{ Port = _Port; }
	
	inline void setStatus( const unsigned int _Status)
		{ Status = _Status; }
	
	inline void setFound( const bool _Found) 
		{ Found = _Found; }
			
	inline void setThreadId( const pthread_t _Id)
		{ threadId = _Id; }			
protected:
    
	string Host;
	
	unsigned int Port;
	
	unsigned int Status;

	bool Found;
	
	pthread_t threadId;
	
};

}

}

#endif	
