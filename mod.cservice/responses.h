#ifndef __RESPONSES_H
#define __RESPONSES_H "$Id: responses.h,v 1.2 2000/12/30 05:47:29 gte Exp $"

/* 
 * responses.h
 *
 * 28/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 * 
 * Defines 'response' ID's, unique references to textual responses
 * from the bot.
 *
 * $Id: responses.h,v 1.2 2000/12/30 05:47:29 gte Exp $
 */

namespace gnuworld { 

	namespace language {
		const int already_authed =    1;
		const int auth_success =      2;
		const int insuf_access =      3;
		const int chan_is_empty =     4;
		const int dont_see_them =     5;
		const int cant_find_on_chan = 6;
		const int chan_not_reg =      7;
		const int youre_opped_by =    8;
		const int youre_voiced_by =   9;
		const int invited_by =       10;
	}
}

#endif
