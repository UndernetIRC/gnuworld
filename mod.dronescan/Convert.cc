#include <sstream>
#include <string>

#include	<cstdio>

#include "Convert.h"

namespace gnuworld {

using std::string ;
using std::stringstream ;

string Convert::ConvertSecondsToString(time_t _seconds)
{
	assembleType toAssm;
	char temp[20];

	time_t seconds = _seconds % 60;
	_seconds -= seconds;
	_seconds /= 60;

	snprintf(temp, 20, "%lis", seconds);
	toAssm.push_front(temp);

	if(_seconds <= 0) {
		return Assemble(toAssm);
	}

	time_t minutes = _seconds % 60;
	_seconds -= minutes;
	_seconds /= 60;

	snprintf(temp, 20, "%lim", minutes);
	toAssm.push_front(temp);

	if(_seconds <= 0) {
		return Assemble(toAssm);
	}

	time_t hours = _seconds % 24;
	_seconds -= hours;
	_seconds /= 24;

	snprintf(temp, 20, "%lih", hours);
	toAssm.push_front(temp);

	if(_seconds <= 0) {
		return Assemble(toAssm);
	}

	time_t days = _seconds;

	snprintf(temp, 20, "%lid", days);
	toAssm.push_front(temp);

	return Assemble(toAssm);
}

string Convert::Assemble(const assembleType& toAssm)
{
	stringstream Return;

	for(assembleType::const_iterator itr = toAssm.begin() ;
	    itr != toAssm.end() ; ++itr) {
		Return << *itr;
	}

	return Return.str();
}

} // namespace gnuworld
