/**
 * dronescanTests.h
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

#ifndef DRONESCANTESTS_H
#define DRONESCANTESTS_H

#include	<string>

namespace gnuworld {

namespace ds {

class dronescan;

class Test {
public:
	Test( dronescan *_bot, const std::string& _testName,
		const std::string& _description,
		const unsigned short _weight ) :
		bot(_bot), testName(_testName), description(_description), weight(_weight)
		{ } ;

	virtual ~Test() { } ;

	virtual bool isNormal( const Channel* ) = 0;

	virtual inline bool setVariable( const std::string&,
			const std::string& )
		{ return false; }

	virtual inline const std::string& getName() const
		{ return testName; }
	virtual inline const std::string& getDescription() const
		{ return description; }
	virtual inline unsigned short getWeight() const
		{ return weight; }

	virtual const std::string getVariable() const
		{ return std::string(); }

	virtual inline const std::string getStatus() const
		{ return std::string(); }

protected:
	dronescan	*bot;
	std::string	testName;
	std::string	description;
	unsigned short	weight;
}; // class Test

/* Big define time */
#define DECLARE_TEST(testName)		\
class testName##Test : public Test {	\
public:					\
	testName##Test(dronescan *_bot, const std::string& _testName, \
	const std::string& _description, const unsigned short _weight) : \
		Test(_bot, _testName, _description, _weight) { } ;	\
	virtual ~testName##Test() { } ;					\
	virtual bool isNormal( const Channel* ) ;

#define DECLARE_TEST_FULL(testName) DECLARE_TEST(testName) };

#define DECLARE_TEST_SET(testName) DECLARE_TEST(testName) \
	virtual bool setVariable( const std::string&, const std::string& ); \
	virtual const std::string getVariable() const; \
	virtual const std::string getStatus() const; \
protected:

DECLARE_TEST_FULL( ABNORMALS )

DECLARE_TEST_SET( COMMONREAL )
	unsigned short realCutoff;
};

DECLARE_TEST_FULL( HASALLOP )

DECLARE_TEST_FULL( HASOP )

DECLARE_TEST_SET( MAXCHANS )
	unsigned short maxChans;
};

DECLARE_TEST_SET( RANGE )
	double channelRange;
};

} // namespace ds

} // namespace gnuworld

#endif
