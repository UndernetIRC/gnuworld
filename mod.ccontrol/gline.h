
#ifndef __GLINECONSTS_H
#define __GLINECONSTS_H "$Id: gline.h,v 1.5 2001/08/26 22:22:55 dan_karrels Exp $"


namespace gnuworld
{

namespace uworld
{

namespace gline
	{

	const unsigned int GLINE_OK =0x01;
	const unsigned int FORCE_NEEDED_HOST = 0x02;
	const unsigned int FORCE_NEEDED_TIME = 0x04;
	const unsigned int FU_NEEDED_USERS = 0x08;
	const unsigned int HUH_NO_HOST = 0x10;
	const unsigned int HUH_NO_USERS = 0x20; 
	const unsigned int BAD_HOST = 0x40;
	const unsigned int BAD_TIME = 0x80;
	const unsigned int FORCE_NEEDED_WILDTIME = 0x100;
	const unsigned int FU_NEEDED_TIME = 0x200;
	const unsigned int NEG_TIME = 0x400;
	
	const unsigned int MGLINE_TIME = 24*3600; 
	const unsigned int MGLINE_WILD_TIME = 10*60;
	const unsigned int MFGLINE_USERS = 255;
	const unsigned int MFGLINE_TIME = 14*3600*24;
	const unsigned int PERM_TIME = 730*3600*24; 
	const unsigned int MFU_TIME = 100*3600*24;
	}

} // namespace uworld

} // namespace gnuworld

#endif // __GLINE_H

