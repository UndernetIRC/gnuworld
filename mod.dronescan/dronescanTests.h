#ifndef DRONESCANTESTS_H
#define DRONESCANTESTS_H "$Id: dronescanTests.h,v 1.3 2003/06/15 16:56:15 jeekay Exp $"

namespace gnuworld {

namespace ds {

class dronescan;

class Test {
public:
	Test( dronescan *_bot, const string& _testName, const string& _description, const unsigned short _weight ) :
		bot(_bot), testName(_testName), description(_description), weight(_weight)
		{ } ;
	
	virtual ~Test() { } ;
	
	virtual bool isNormal( const Channel* ) = 0;
	
	virtual inline bool setVariable( const string&, const string& )
		{ return false; }
	
	virtual inline const string& getName() const
		{ return testName; }
	virtual inline const string& getDescription() const
		{ return description; }
	virtual inline unsigned short getWeight() const
		{ return weight; }
	
protected:
	dronescan	*bot;
	string		testName;
	string		description;
	unsigned short	weight;
}; // class Test

/* Big define time */
#define DECLARE_TEST(testName)		\
class testName##Test : public Test {	\
public:					\
	testName##Test(dronescan *_bot, const string& _testName, const string& _description, const unsigned short _weight) :	\
		Test(_bot, _testName, _description, _weight) { } ;	\
	virtual ~testName##Test() { } ;					\
	virtual bool isNormal( const Channel* ) ;

#define DECLARE_TEST_FULL(testName) DECLARE_TEST(testName) };

#define DECLARE_TEST_SET(testName) DECLARE_TEST(testName) \
	virtual bool setVariable( const string&, const string& ); \
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

DECLARE_TEST_FULL( RANGE )

} // namespace ds

} // namespace gnuworld

#endif
