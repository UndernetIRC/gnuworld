/**
 * clientData.h
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 */

#ifndef CLIENTDATA_H
#define CLIENTDATA_H

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
	inline CLIENT_STATE getState() const
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
	inline double getEntropy() const
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
	inline bool isUnknown() const
		{ return (currentState == UNKNOWN); }

	/**
	 * Convenience function to test for state NORMAL.
	 * @return	true if currentState == NORMAL else false.
	 */
	inline bool isNormal() const
		{ return (currentState == NORMAL); }

	/**
	 * Convenience function to test for state ABNORMAL.
	 * @return	true if currentState == ABNORMAL else false.
	 */
	inline bool isAbnormal() const
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
