#ifndef __CONSTANTS_H
#define __CONSTANTS_H "$Id: constants.h,v 1.17 2001/12/27 02:48:08 gte Exp $"

/*
 * constants.h
 *
 * 27/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 *
 * Defines constants used throughout the application.
 *
 * $Id: constants.h,v 1.17 2001/12/27 02:48:08 gte Exp $
 */

namespace gnuworld
{
namespace sql
	{
	/*
	 *  Comma seperated lists of fields for use in retrieving various
	 *  articles of data.
	 */
	const string channel_fields = "id,name,flags,mass_deop_pro,flood_pro,url,description,comment,keywords,registered_ts,channel_ts,channel_mode,userflags,last_updated,limit_offset,limit_period";
	const string user_fields = "id,user_name,password,url,language_id,flags,last_updated_by,last_updated,email";
	const string level_fields = "channel_id,user_id,access,flags,suspend_expires,suspend_level,suspend_by,added,added_by,last_Modif,last_Modif_By,last_Updated";
	const string ban_fields = "id,channel_id,banmask,set_by,set_ts,level,expires,reason,last_updated";
	}
}

#endif
