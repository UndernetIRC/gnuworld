#ifndef __CONSTANTS_H
#define __CONSTANTS_H "$Id: constants.h,v 1.9 2001/01/28 23:16:33 gte Exp $"

/* 
 * constants.h
 *
 * 27/12/2000 - Greg Sikorski <gte@atomicrevs.demon.co.uk>
 * Initial Version.
 * 
 * Defines constants used throughout the application.
 *
 * $Id: constants.h,v 1.9 2001/01/28 23:16:33 gte Exp $
 */

namespace gnuworld 
{ 

	namespace sql
	{
		/*
		 *  Comma seperated lists of fields for use in retrieving various
		 *  articles of data.
		 */
		const string channel_fields = "id,name,flags,mass_deop_pro,flood_pro,url,description,keywords,registered_ts,channel_ts,channel_mode,userflags,last_updated";
		const string user_fields = "id,user_name,password,email,url,language_id,public_key,flags,last_updated_by,users.last_updated,users_lastseen.last_seen";
		const string level_fields = "channel_id,user_id,access,flags,suspend_expires,suspend_by,added,added_by,last_Modif,last_Modif_By,last_Updated";
		const string ban_fields = "id,channel_id,banmask,set_by,set_ts,level,expires,reason,last_updated";
	}
}

#endif
