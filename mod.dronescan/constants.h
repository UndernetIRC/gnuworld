/**
 * constants.h
 *
 * Contains things that are used more than once across
 * the module but which need to remain uniform.
 *
 * $Id: constants.h,v 1.2 2003/12/29 23:59:37 dan_karrels Exp $
 */

#ifndef CONSTANTS_H
#define CONSTANTS_H "$Id: constants.h,v 1.2 2003/12/29 23:59:37 dan_karrels Exp $"

#include	<string>

namespace gnuworld {

namespace ds {

namespace sql {

	const std::string fakeclients = "SELECT fc.id, fc.nickname, "
		"fc.username, fc.hostname, fc.realname, u.user_name, "
		"fc.created_by, fc.created_on, fc.last_updated, fc.flags "
		"FROM fakeclients AS fc JOIN users AS u ON "
		"fc.created_by=u.id";

} // namespace sql

} // namespace ds

} // namespace gnuworld

#endif
