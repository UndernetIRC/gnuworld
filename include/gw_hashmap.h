/**
 * gw_hashmap.h
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
 * $Id: gw_hashmap.h,v 1.2 2004/08/24 23:49:34 dan_karrels Exp $
 */

#ifndef GW_HASHMAP_H
#define GW_HASHMAP_H

#include	<string>

#include	"defs.h"

/* If we have ext/hash_map, it is probably the GNU GCC extension.
 * That lives in the extension namespace of __gnu_cxx.
 *
 * If no hash_map implementations are available, default to map.
 */

#if defined HAVE_EXT_HASH_MAP
# include <ext/hash_map>
# define HASHMAP ::__gnu_cxx::hash_map
# define HASHMAP_TYPE "::__gnu_cxx::hash_map"
/* Unfortunate hack as the 'standard' extension does not define a
 * hashing function for ::std::string.
 * We simply pass the value of .c_str() on to the char* hasher.
 */
namespace __gnu_cxx {
	template <> struct hash<std::string> {
		size_t operator()(const std::string& h) const {
			return __stl_hash_string(h.c_str());
		}
	};
}
#elif defined HAVE_MAP
# include <map>
# define HASHMAP ::std::map
# define HASHMAP_TYPE "::std::map"
#else
# error "Unable to find a usable map implementation"
#endif

#endif
