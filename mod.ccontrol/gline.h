
#ifndef __GLINECONSTS_H
#define __GLINECONSTS_H "$Id: gline.h,v 1.1 2001/07/23 10:33:59 mrbean_ Exp $"


namespace gnuworld
{

namespace uworld
{

namespace gline
	{

	const unsigned int GLINE_OK =0x01;
	const unsigned int FORCE_NEEDED_HOST = 0x02;
	const unsigned int FORCE_NEEDED_TIME = 0x03;
	const unsigned int FORCE_NEEDED_USERS = 0x04;
	const unsigned int  HUH_NO_HOST = 0x05;
	const unsigned int HUH_NO_USERS = 0x06; 
	const unsigned int BAD_HOST = 0x07;

	}

}
}

#endif // __GLINE_H