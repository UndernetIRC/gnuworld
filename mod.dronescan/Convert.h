#ifndef CONVERT_H
#define CONVERT_H "$Id: Convert.h,v 1.5 2003/11/26 23:30:22 dan_karrels Exp $"

#include <deque>
#include <string>

#include <ctime>

namespace gnuworld {

typedef std::deque<std::string> assembleType;

class Convert {
public:
	static std::string ConvertSecondsToString(time_t _seconds);
	
	static std::string Assemble(const assembleType& toAssm);
	
protected:
	/* Don't instantiate a static only class */
	Convert() { } ;
}; // class Convert

} // namespace gnuworld

#endif
