/* Numeric.h
 * This file contains basic routines for converting numerics between
 * integer and char* and std::string formats.
 * The majority of this code is from the Undernet ircu code base.
 */

#ifndef __NUMERIC_H
#define __NUMERIC_H "$Id: Numeric.h,v 1.3 2002/04/27 14:54:49 dan_karrels Exp $"

/*
 * Numeric nicks are new as of version ircu2.10.00beta1.
 *
 * The idea is as follows:
 * In most messages (for protocol 10+) the original nick will be
 * replaced by a 3 character string: YXX
 * Where 'Y' represents the server, and 'XX' the nick on that server.
 *
 * 'YXX' should not interfer with the input parser, and therefore is
 * not allowed to contain spaces or a ':'.
 * Also, 'Y' can't start with a '+' because of m_server().
 *
 * We keep the characters printable for debugging reasons too.
 *
 * The 'XX' value can be larger then the maximum number of clients
 * per server, we use a mask (struct Server::nn_mask) to get the real
 * client numeric. The overhead is used to have some redundancy so
 * just-disconnected-client aren't confused with just-connected ones.
 */

/* These must be the same on ALL servers ! Do not change ! */
#include	<cassert>

namespace gnuworld
{

/// The length of the character numerics, 5 for n2k
#define P10_NUMNICKLEN 5
#define NUMNICKLOG 6
#define NUMNICKMAXCHAR 'z'      /* See convert2n[] */
#define NUMNICKBASE 64          /* (2 << NUMNICKLOG) */
#define NUMNICKMASK 63          /* (NUMNICKBASE-1) */
#define NN_MAX_SERVER 4096      /* (NUMNICKBASE * NUMNICKBASE) */

/*
 * The internal counter for the 'XX' of local clients
 */

/**
 * convert2y[] converts a numeric to the corresponding character.
 * The following characters are currently known to be forbidden:
 *
 * '\0' : Because we use '\0' as end of line.
 *
 * ' '  : Because parse_*() uses this as parameter seperator.
 * ':'  : Because parse_server() uses this to detect if a prefix is a
 *        numeric or a name.
 * '+'  : Because m_nick() uses this to determine if parv[6] is a
 *        umode or not.
 * '&', '#', '+', '$', '@' and '%' :
 *        Because m_message() matches these characters to detect special cases.
 */
static const char convert2y[] = {
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
  'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
  'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
  'w','x','y','z','0','1','2','3','4','5','6','7','8','9','[',']'
};

/**
 * convert2n[] ocnverts a character to the corresponding integer.
 */
static const unsigned int convert2n[] = {
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  52,53,54,55,56,57,58,59,60,61, 0, 0, 0, 0, 0, 0, 
   0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
  15,16,17,18,19,20,21,22,23,24,25,62, 0,63, 0, 0,
   0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
  41,42,43,44,45,46,47,48,49,50,51, 0, 0, 0, 0, 0,

   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * This method converts a base64 character array to
 * an unsigned integer.
 */
inline unsigned int base64toint( const char* s )
{
unsigned int i = convert2n[ static_cast< unsigned char >( *s++ ) ] ;
while( *s )
	{
	i <<= NUMNICKLOG;
	i += convert2n[ static_cast< unsigned char >( *s++ ) ] ;
	}
return i ;
}

/**
 * This method converts only the first (count) characters
 * of the string (s) to unsigned integer format.
 */
inline unsigned int base64toint( const char* s, size_t count )
{
assert( s != 0 ) ;

unsigned int i = convert2n[ static_cast< unsigned char >( *s++ ) ] ;
while (*s && --count > 0)
	{
	i <<= NUMNICKLOG;
	i += convert2n[ static_cast< unsigned char >( *s++ ) ] ;
	}

return i;
}

/**
 * This method converts an unsigned integer (v) to a base64
 * character array of length (count).
 * Be sure that buf is allocated and large enough to hold
 * (count) characters (excluding null terminator).
 * This method returns a pointer to the base 64 buffer.
 */
inline const char* inttobase64( char* buf, unsigned int v, size_t count )
{
assert( buf != 0 ) ;

buf[count] = '\0';
while (count > 0)
	{
	buf[ --count ] = convert2y[(v & NUMNICKMASK)];
	v >>= NUMNICKLOG;
	}

return buf;
}

} // namespace gnuworld

#endif // __NUMERIC_H
