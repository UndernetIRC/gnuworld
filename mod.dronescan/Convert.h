#ifndef CONVERT_H
#define CONVERT_H "$Id: Convert.h,v 1.4 2003/10/12 16:22:53 jeekay Exp $"

#include <deque>
#include <string>

#include <time.h>

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
