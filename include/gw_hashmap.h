#ifndef GW_HASHMAP_H
#define GW_HASHMAP_H

#include "defs.h"

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
