#ifndef __RESPONSES_H
#define __RESPONSES_H "$Id: responses.h,v 1.3 2000/12/31 05:06:27 gte Exp $"

/* 
 * responses.h
 *
 * 28/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 * 
 * Defines 'response' ID's, unique references to textual responses
 * from the bot.
 *
 * $Id: responses.h,v 1.3 2000/12/31 05:06:27 gte Exp $
 */

namespace gnuworld { 

	namespace language {
		const int already_authed =     1;
		const int auth_success =       2;
		const int insuf_access =       3;
		const int chan_is_empty =      4;
		const int dont_see_them =      5;
		const int cant_find_on_chan =  6;
		const int chan_not_reg =       7;
		const int youre_opped_by =     8;
		const int youre_voiced_by =    9;
		const int youre_not_in_chan = 10;
		const int already_opped =     11;
	}
}

#endif
