#ifndef DRONESCANTESTS_H
#define DRONESCANTESTS_H "$Id: dronescanTests.h,v 1.2 2003/05/08 11:42:53 jeekay Exp $"

namespace gnuworld {

namespace ds {

class dronescan;

class Test {
public:
	Test( dronescan *_bot, const string& _testName, const string& _description ) :
		bot(_bot), testName(_testName), description(_description)
		{ } ;
	
	virtual ~Test() { } ;
	
	virtual bool isNormal( const Channel* ) = 0;
	
	virtual inline string& getName()
		{ return testName; }
	
protected:
	dronescan	*bot;
	string		testName;
	string		description;
}; // class Test

/* Big define time */
#define DECLARE_TEST(testName)		\
class testName##Test : public Test {	\
public:					\
	testName##Test(dronescan *_bot, const string& _testName, const string& _description) :	\
		Test(_bot, _testName, _description) { } ;	\
	virtual ~testName##Test() { } ;					\
	virtual bool isNormal( const Channel* ) ;		\
};

DECLARE_TEST( ABNORMALS )
DECLARE_TEST( HASOP )
DECLARE_TEST( MAXCHANS )
DECLARE_TEST( RANGE )

} // namespace ds

} // namespace gnuworld

#endif
