#ifndef __RESPONSES_H
#define __RESPONSES_H "$Id: responses.h,v 1.6 2001/02/12 05:42:55 isomer Exp $"

/* 
 * responses.h
 *
 * 28/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 * 
 * Defines 'response' ID's, unique references to textual responses
 * from the bot.
 *
 * $Id: responses.h,v 1.6 2001/02/12 05:42:55 isomer Exp $
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
		const int already_voiced =    12;
		const int youre_deopped_by =  13;
		const int youre_devoiced_by = 14;
		const int not_opped =         15;
		const int not_voiced =        16;
		const int not_registered =    17;
		const int i_am_not_on_chan =  18;
		const int ban_level_range =   19;
		const int ban_duration =      20;
		const int motd = 21;
	}
}

#endif
