#ifndef CONVERT_H
#define CONVERT_H "$Id: Convert.h,v 1.1 2003/07/26 16:47:18 jeekay Exp $"

#include <deque>
#include <sstream>
#include <string>

namespace gnuworld {

typedef std::deque<std::string> assembleType;

class Convert {
public:
	inline static std::string ConvertSecondsToString(time_t _seconds)
	{
		assembleType toAssm;
		char temp[20];

		time_t seconds = _seconds % 60;
		_seconds -= seconds;
		_seconds /= 60;
		
		snprintf(temp, 20, "%us", seconds);
		toAssm.push_front(temp);
		
		if(_seconds <= 0) {
			return Assemble(toAssm);
		}
		
		time_t minutes = _seconds % 60;
		_seconds -= minutes;
		_seconds /= 60;
		
		snprintf(temp, 20, "%um", minutes);
		toAssm.push_front(temp);
		
		if(_seconds <= 0) {
			return Assemble(toAssm);
		}
		
		time_t hours = _seconds % 24;
		_seconds -= hours;
		_seconds /= 24;
		
		snprintf(temp, 20, "%uh", hours);
		toAssm.push_front(temp);
		
		if(_seconds <= 0) {
			return Assemble(toAssm);
		}
		
		time_t days = _seconds;
		
		snprintf(temp, 20, "%ud", days);
		toAssm.push_front(temp);
		
		return Assemble(toAssm);
	}
	
	inline static std::string Assemble(const assembleType& toAssm)
	{
		std::stringstream Return;
		for(assembleType::const_iterator itr = toAssm.begin() ;
		    itr != toAssm.end() ; ++itr) {
			Return << *itr;
		}
		
		return Return.str();	
	}
}; // class Convert

} // namespace gnuworld

#endif
