#ifndef CLIENTDATA_H
#define CLIENTDATA_H "$Id: clientData.h,v 1.2 2003/05/06 22:17:33 jeekay Exp $"

namespace gnuworld {

namespace ds {

/** These are the possible states of a client. */
enum CLIENT_STATE {
	UNKNOWN,
	NORMAL,
	ABNORMAL
};

/**
 * This class is a data container for the state of a given iClient.
 * It provides easy access to caches results of checking.
 */
class clientData {
public:
	/** Constructor sets initial state to UNKNOWN. */
	inline clientData()
		{ currentState = UNKNOWN; entropy = 0; }
	
	/**
	 * Accessor for currentState.
	 * @return	the current state of this client.
	 */
	inline CLIENT_STATE getState()
		{ return currentState; }
		
	/**
	 * Mutator for currentState.
	 * @param _currentState	the new state to be set.
	 * @return		the new state.
	 */
	inline CLIENT_STATE setState(CLIENT_STATE _currentState)
		{ return currentState = _currentState; }
	
	
	
	/**
	 * Accessor for the entropy value.
	 * @return	the current entropy value
	 */
	inline double getEntropy()
		{ return entropy; }
	
	/**
	 * Mutator for the entropy value.
	 * @param _entropy	the new entropy value to be set.
	 * @return		the new entropy value.
	 */
	inline double setEntropy(double _entropy)
		{ return entropy = _entropy; }
	
	
	/**
	 * Convenience function to test for state UNKNOWN.
	 * @return	true if currentState == UNKNOWN else false.
	 */
	inline bool isUnknown()
		{ return (currentState == UNKNOWN); }

	/**
	 * Convenience function to test for state NORMAL.
	 * @return	true if currentState == NORMAL else false.
	 */
	inline bool isNormal()
		{ return (currentState == NORMAL); }

	/**
	 * Convenience function to test for state ABNORMAL.
	 * @return	true if currentState == ABNORMAL else false.
	 */
	inline bool isAbnormal()
		{ return (currentState == ABNORMAL); }

protected:
	/** The current state. */
	CLIENT_STATE	currentState;
	
	/** The current entropy value. */
	double		entropy;
}; // class clientData

} // namespace ds

} // namespace gnuworld

#endif
