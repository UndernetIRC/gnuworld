#ifndef __CONSTANTS_H
#define __CONSTANTS_H "$Id: constants.h,v 1.20 2002/10/19 19:53:59 gte Exp $"

/*
 * constants.h
 *
 * 27/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Defines constants used throughout the application.
 *
 * $Id: constants.h,v 1.20 2002/10/19 19:53:59 gte Exp $
 */

namespace gnuworld
{
namespace sql
	{
	/*
	 *  Comma seperated lists of fields for use in retrieving various
	 *  articles of data.
	 */
	const string channel_fields = "id,name,flags,mass_deop_pro,flood_pro,url,description,comment,keywords,registered_ts,channel_ts,channel_mode,userflags,last_updated,limit_offset,limit_period,limit_grace,limit_max";
	const string user_fields = "users.id,users.user_name,users.password,users.url,users.language_id,users.flags,users.last_updated_by,users.last_updated,users.email,users.maxlogins";
	const string level_fields = "channel_id,user_id,access,flags,suspend_expires,suspend_level,suspend_by,added,added_by,last_Modif,last_Modif_By,last_Updated";
	const string ban_fields = "id,channel_id,banmask,set_by,set_ts,level,expires,reason,last_updated";
	}
}

#endif
