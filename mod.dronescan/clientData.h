#ifndef CLIENTDATA_H
#define CLIENTDATA_H "$Id: clientData.h,v 1.1 2003/05/02 11:54:54 jeekay Exp $"

namespace gnuworld {

namespace ds {

enum CLIENT_STATE {
	UNKNOWN,
	NORMAL,
	ABNORMAL
};

class clientData {
public:
	inline clientData()
		{ currentState = UNKNOWN; }
	
	inline CLIENT_STATE getState()
		{ return currentState; }
	inline CLIENT_STATE setState(CLIENT_STATE _currentState)
		{ return currentState = _currentState; }
	
	/** Convenience function. */
	inline bool isUnknown()
		{ return (currentState == UNKNOWN); }
	inline bool isNormal()
		{ return (currentState == NORMAL); }
	inline bool isAbnormal()
		{ return (currentState == ABNORMAL); }

protected:
	CLIENT_STATE currentState;
}; // class clientData

} // namespace ds

} // namespace gnuworld

#endif
