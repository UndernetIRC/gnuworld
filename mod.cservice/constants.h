/**
 * constants.h
 *
 * 27/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Defines constants used throughout the application.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: constants.h,v 1.23 2007/03/25 16:42:24 kewlio Exp $
 */

#ifndef __CONSTANTS_H
#define __CONSTANTS_H "$Id: constants.h,v 1.23 2007/03/25 16:42:24 kewlio Exp $"

#include	<string>

namespace gnuworld
{
namespace sql
	{
	/*
	 *  Comma seperated lists of fields for use in retrieving various
	 *  articles of data.
	 */
	const std::string channel_fields = "id,name,flags,mass_deop_pro,flood_pro,url,channels.description,comment,keywords,registered_ts,channel_ts,channel_mode,userflags,channels.last_updated,limit_offset,limit_period,limit_grace,limit_max,max_bans,no_take,welcome,limit_joinmax,limit_joinsecs,limit_joinperiod,limit_joinmode";
	const std::string user_fields = "users.id,users.user_name,users.password,users.url,users.language_id,users.flags,users.last_updated_by,users.last_updated,users.signup_ts,users.email,users.maxlogins,users.verificationdata,users.totp_key,users.scram_record";
	const std::string level_fields = "channel_id,user_id,access,flags,suspend_expires,suspend_level,suspend_by,added,added_by,last_Modif,last_Modif_By,last_Updated,suspend_reason";
	const std::string ban_fields = "id,channel_id,banmask,set_by,set_ts,level,expires,reason,last_updated";
	}
}

#endif
