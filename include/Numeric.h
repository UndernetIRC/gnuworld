/**
 * Numeric.h
 * This file contains basic routines for converting numerics between
 * integer and char* and std::string formats.
 * The majority of this code is from the Undernet ircu code base.
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
 * $Id: Numeric.h,v 1.7 2009/07/26 18:30:37 mrbean_ Exp $
 */

#ifndef __NUMERIC_H
#define __NUMERIC_H "$Id: Numeric.h,v 1.7 2009/07/26 18:30:37 mrbean_ Exp $"

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

#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>
#include	<arpa/inet.h>

#include	<string.h> /* for DupString()'s strcpy, strlen */
#include	<cstdio>	//for sscanf

///For Test functions
#include	<algorithm>    // std::reverse
///For Test functions

#ifndef INCLUDED_ircd_chattr_h
#include "ircd_chattr.h"
#endif

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

/** Maximum length of a numeric IP (v4 or v6) address.
 * "ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255"
 */
#define SOCKIPLEN 45

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

/*
 * this new faster inet_ntoa was ripped from:
 * From: Thomas Helvey <tomh@inxpress.net>
 */
/** Array of text strings for dotted quads. */
static const char* IpQuadTab[] =
{
    "0",   "1",   "2",   "3",   "4",   "5",   "6",   "7",   "8",   "9",
   "10",  "11",  "12",  "13",  "14",  "15",  "16",  "17",  "18",  "19",
   "20",  "21",  "22",  "23",  "24",  "25",  "26",  "27",  "28",  "29",
   "30",  "31",  "32",  "33",  "34",  "35",  "36",  "37",  "38",  "39",
   "40",  "41",  "42",  "43",  "44",  "45",  "46",  "47",  "48",  "49",
   "50",  "51",  "52",  "53",  "54",  "55",  "56",  "57",  "58",  "59",
   "60",  "61",  "62",  "63",  "64",  "65",  "66",  "67",  "68",  "69",
   "70",  "71",  "72",  "73",  "74",  "75",  "76",  "77",  "78",  "79",
   "80",  "81",  "82",  "83",  "84",  "85",  "86",  "87",  "88",  "89",
   "90",  "91",  "92",  "93",  "94",  "95",  "96",  "97",  "98",  "99",
  "100", "101", "102", "103", "104", "105", "106", "107", "108", "109",
  "110", "111", "112", "113", "114", "115", "116", "117", "118", "119",
  "120", "121", "122", "123", "124", "125", "126", "127", "128", "129",
  "130", "131", "132", "133", "134", "135", "136", "137", "138", "139",
  "140", "141", "142", "143", "144", "145", "146", "147", "148", "149",
  "150", "151", "152", "153", "154", "155", "156", "157", "158", "159",
  "160", "161", "162", "163", "164", "165", "166", "167", "168", "169",
  "170", "171", "172", "173", "174", "175", "176", "177", "178", "179",
  "180", "181", "182", "183", "184", "185", "186", "187", "188", "189",
  "190", "191", "192", "193", "194", "195", "196", "197", "198", "199",
  "200", "201", "202", "203", "204", "205", "206", "207", "208", "209",
  "210", "211", "212", "213", "214", "215", "216", "217", "218", "219",
  "220", "221", "222", "223", "224", "225", "226", "227", "228", "229",
  "230", "231", "232", "233", "234", "235", "236", "237", "238", "239",
  "240", "241", "242", "243", "244", "245", "246", "247", "248", "249",
  "250", "251", "252", "253", "254", "255"
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

inline void splitbase64int( unsigned int intYYXXX,
	unsigned int& intYY,
	unsigned int& intXXX )
{
// Decompose to charYYXXX
char charYYXXX[ 6 ] ;
inttobase64( charYYXXX, intYYXXX, 5 ) ;

// Now split the charYYXXX into two ints
intYY = base64toint( charYYXXX, 2 ) ;
intXXX = base64toint( &charYYXXX[ 2 ], 3 ) ;
}

inline unsigned int combinebase64int( const unsigned int& intYY,
	const unsigned int& intXXX )
{
char charYYXXX[ 6 ] ;

inttobase64( charYYXXX, intYY, 2 ) ;
inttobase64( &charYYXXX[ 2 ], intXXX, 3 ) ;

return base64toint( charYYXXX, 5 ) ;
}

/** Evaluate to non-zero if \a ADDR is a valid address (not all 0s and not all 1s). */
#define irc_in_addr_valid(ADDR) (((ADDR)->in6_16[0] && ~(ADDR)->in6_16[0]) \
                                 || (ADDR)->in6_16[1] != (ADDR)->in6_16[0] \
                                 || (ADDR)->in6_16[2] != (ADDR)->in6_16[0] \
                                 || (ADDR)->in6_16[3] != (ADDR)->in6_16[0] \
                                 || (ADDR)->in6_16[4] != (ADDR)->in6_16[0] \
                                 || (ADDR)->in6_16[5] != (ADDR)->in6_16[0] \
                                 || (ADDR)->in6_16[6] != (ADDR)->in6_16[0] \
                                 || (ADDR)->in6_16[7] != (ADDR)->in6_16[0])
/** Evaluate to non-zero if \a ADDR (of type struct irc_in_addr) is an IPv4 address. */
#define irc_in_addr_is_ipv4(ADDR) (!(ADDR)->in6_16[0] && !(ADDR)->in6_16[1] && !(ADDR)->in6_16[2] \
                                   && !(ADDR)->in6_16[3] && !(ADDR)->in6_16[4] \
                                   && ((!(ADDR)->in6_16[5] && (ADDR)->in6_16[6]) \
                                       || (ADDR)->in6_16[5] == 65535))
/** Evaluate to non-zero if \a A is a different IP than \a B. */
#define irc_in_addr_cmp(A,B) (irc_in_addr_is_ipv4(A) ? ((A)->in6_16[6] != (B)->in6_16[6] \
                                  || (A)->in6_16[7] != (B)->in6_16[7] || !irc_in_addr_is_ipv4(B)) \
                              : memcmp((A), (B), sizeof(struct irc_in_addr)))

/** Structure to store an IP address. */
struct irc_in_addr
{
  unsigned short in6_16[8]; /**< IPv6 encoded parts, little-endian. */
};

/** Encode an IP address in the base64 used by numnicks.
 * For IPv4 addresses (including IPv4-mapped and IPv4-compatible IPv6
 * addresses), the 32-bit host address is encoded directly as six
 * characters.
 *
 * For IPv6 addresses, each 16-bit address segment is encoded as three
 * characters, but the longest run of zero segments is encoded using an
 * underscore.
 * @param[out] buf Output buffer to write to.
 * @param[in] addr IP address to encode.
 * @param[in] count Number of bytes writable to \a buf.
 * @param[in] v6_ok If non-zero, peer understands base-64 encoded IPv6 addresses.
 */
inline const char* iptobase64(char* buf, const struct irc_in_addr* addr, unsigned int count, int v6_ok)
{
  if (irc_in_addr_is_ipv4(addr)) {
    assert(count >= 6);
    inttobase64(buf, (ntohs(addr->in6_16[6]) << 16) | ntohs(addr->in6_16[7]), 6);
  } else if (!v6_ok) {
    assert(count >= 6);
    if (addr->in6_16[0] == htons(0x2002))
        inttobase64(buf, (ntohs(addr->in6_16[1]) << 16) | ntohs(addr->in6_16[2]), 6);
    else
        strcpy(buf, "AAAAAA");
  } else {
    unsigned int max_start, max_zeros, curr_zeros, zero, ii;
    char *output = buf;

    assert(count >= 25);
    /* Can start by printing out the leading non-zero parts. */
    for (ii = 0; (addr->in6_16[ii]) && (ii < 8); ++ii) {
      inttobase64(output, ntohs(addr->in6_16[ii]), 3);
      output += 3;
    }
    /* Find the longest run of zeros. */
    for (max_start = zero = ii, max_zeros = curr_zeros = 0; ii < 8; ++ii) {
      if (!addr->in6_16[ii])
        curr_zeros++;
      else if (curr_zeros > max_zeros) {
        max_start = ii - curr_zeros;
        max_zeros = curr_zeros;
        curr_zeros = 0;
      }
    }
    if (curr_zeros > max_zeros) {
      max_start = ii - curr_zeros;
      max_zeros = curr_zeros;
      curr_zeros = 0;
    }
    /* Print the rest of the address */
    for (ii = zero; ii < 8; ) {
      if ((ii == max_start) && max_zeros) {
        *output++ = '_';
        ii += max_zeros;
      } else {
        inttobase64(output, ntohs(addr->in6_16[ii]), 3);
        output += 3;
        ii++;
      }
    }
    *output = '\0';
  }
  return buf;
}

/** Decode an IP address from base64.
 * @param[in] input Input buffer to decode.
 * @param[out] addr IP address structure to populate.
 */
inline void base64toip(const char* input, struct irc_in_addr* addr)
{
  memset(addr, 0, sizeof(*addr));
  if (strlen(input) == 6) {
    unsigned int in = base64toint(input);
    /* An all-zero address should stay that way. */
    if (in) {
      addr->in6_16[5] = htons(65535);
      addr->in6_16[6] = htons(in >> 16);
      addr->in6_16[7] = htons(in & 65535);
    }
  } else {
    unsigned int pos = 0;
    do {
      if (*input == '_') {
        unsigned int left;
        for (left = (25 - strlen(input)) / 3 - pos; left; left--)
          addr->in6_16[pos++] = 0;
        input++;
      } else {
        unsigned short accum = convert2n[(unsigned char)*input++];
        accum = (accum << NUMNICKLOG) | convert2n[(unsigned char)*input++];
        accum = (accum << NUMNICKLOG) | convert2n[(unsigned char)*input++];
        addr->in6_16[pos++] = ntohs(accum);
      }
    } while (pos < 8);
  }
}

/**ircu's ip_registry_canonicalize function:
 * Convert IP addresses to canonical form for comparison.  IPv4
 * addresses are translated into 6to4 form; IPv6 addresses are left
 * alone.
 */
inline irc_in_addr convert_ipv4_to_ipv6_form (const struct irc_in_addr in)
{
    if (irc_in_addr_is_ipv4(&in)) {
    	irc_in_addr out;
        out.in6_16[0] = htons(0x2002);
        out.in6_16[1] = in.in6_16[6];
        out.in6_16[2] = in.in6_16[7];
        out.in6_16[3] = out.in6_16[4] = out.in6_16[5] = 0;
        out.in6_16[6] = out.in6_16[7] = 0;
        return out;
    } else
    	return in;
}

/** Attempt to parse an IPv4 address into a network-endian form.
 * @param[in] input Input string.
 * @param[out] output Network-endian representation of the address.
 * @param[out] pbits Number of bits found in pbits.
 * @return Number of characters used from \a input, or 0 if the parse failed.
 */
inline static unsigned int
ircd_aton_ip4(const char *input, unsigned int *output, unsigned char *pbits)
{
  unsigned int dots = 0, pos = 0, part = 0, ip = 0, bits;

  /* Intentionally no support for bizarre IPv4 formats (plain
   * integers, octal or hex components) -- only vanilla dotted
   * decimal quads.
   */
  if (input[0] == '.')
    return 0;
  bits = 32;
  while (1) switch (input[pos]) {
  case '\0':
    if (dots < 3)
      return 0;
  out:
    ip |= part << (24 - 8 * dots);
    *output = htonl(ip);
    if (pbits)
      *pbits = bits;
    return pos;
  case '.':
    if (++dots > 3)
      return 0;
    if (input[++pos] == '.')
      return 0;
    ip |= part << (32 - 8 * dots);
    part = 0;
    if (input[pos] == '*') {
      while (input[++pos] == '*' || input[pos] == '.') ;
      if (input[pos] != '\0')
        return 0;
      if (pbits)
        *pbits = dots * 8;
      *output = htonl(ip);
      return pos;
    }
    break;
  case '/':
    if (!pbits || !IsDigit(input[pos + 1]))
      return 0;
    for (bits = 0; IsDigit(input[++pos]); )
      bits = bits * 10 + input[pos] - '0';
    if (bits > 32)
      return 0;
    goto out;
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
    part = part * 10 + input[pos++] - '0';
    if (part > 255)
      return 0;
    break;
  default:
    return 0;
  }
}

/** Parse a numeric IPv4 or IPv6 address into an irc_in_addr.
 * @param[in] input Input buffer.
 * @param[out] ip Receives parsed IP address.
 * @param[out] pbits If non-NULL, receives number of bits specified in address mask.
 * @return Number of characters used from \a input, or 0 if the
 * address was unparseable or malformed.
 */
inline int ipmask_parse(const char *input, struct irc_in_addr *ip, unsigned char *pbits)
{
  char *colon;
  char *dot;

  assert(ip);
  assert(input);
  memset(ip, 0, sizeof(*ip));
  colon = (char*)strchr(input, ':');
  dot = (char*)strchr(input, '.');

  if (colon && (!dot || (dot > colon))) {
    unsigned int part = 0, pos = 0, ii = 0, colon = 8;
    const char *part_start = NULL;

    /* Parse IPv6, possibly like ::127.0.0.1.
     * This is pretty straightforward; the only trick is borrowed
     * from Paul Vixie (BIND): when it sees a "::" continue as if
     * it were a single ":", but note where it happened, and fill
     * with zeros afterward.
     */
    if (input[pos] == ':') {
      if ((input[pos+1] != ':') || (input[pos+2] == ':'))
        return 0;
      colon = 0;
      pos += 2;
      part_start = input + pos;
    }
    while (ii < 8) switch (input[pos]) {
      unsigned char chval;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
      chval = input[pos] - '0';
    use_chval:
      part = (part << 4) | chval;
      if (part > 0xffff)
        return 0;
      pos++;
      break;
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      chval = input[pos] - 'A' + 10;
      goto use_chval;
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
      chval = input[pos] - 'a' + 10;
      goto use_chval;
    case ':':
      part_start = input + ++pos;
      if (input[pos] == '.')
        return 0;
      if (input[pos] == '\0')
      	return 0;
      ip->in6_16[ii++] = htons(part);
      part = 0;
      if (input[pos] == ':') {
        if (colon < 8)
          return 0;
        if (ii == 8)
          return 0;
        colon = ii;
        pos++;
      }
      break;
    case '.': {
      uint32_t ip4;
      unsigned int len;
      len = ircd_aton_ip4(part_start, &ip4, pbits);
      if (!len || (ii > 6))
        return 0;
      ip->in6_16[ii++] = htons(ntohl(ip4) >> 16);
      ip->in6_16[ii++] = htons(ntohl(ip4) & 65535);
      if (pbits)
        *pbits += 96;
      pos = part_start + len - input;
      goto finish;
    }
    case '/':
      if (!pbits || !IsDigit(input[pos + 1]))
        return 0;
      ip->in6_16[ii++] = htons(part);
      for (part = 0; IsDigit(input[++pos]); )
        part = part * 10 + input[pos] - '0';
      if (part > 128)
        return 0;
      *pbits = part;
      goto finish;
    case '*':
      while (input[++pos] == '*' || input[pos] == ':') ;
      if (input[pos] != '\0' || colon < 8)
        return 0;
      if (part && ii < 8)
          ip->in6_16[ii++] = htons(part);
      if (pbits)
        *pbits = ii * 16;
      return pos;
    case '\0':
      ip->in6_16[ii++] = htons(part);
      if (colon == 8 && ii < 8)
        return 0;
      if (pbits)
        *pbits = 128;
      goto finish;
    default:
      return 0;
    }
    if (input[pos] != '\0')
      return 0;
  finish:
    if (colon < 8) {
      unsigned int jj;
      /* Shift stuff after "::" up and fill middle with zeros. */
      for (jj = 0; jj < ii - colon; jj++)
        ip->in6_16[7 - jj] = ip->in6_16[ii - jj - 1];
      for (jj = 0; jj < 8 - ii; jj++)
        ip->in6_16[colon + jj] = 0;
    }
    return pos;
  } else if (dot || strchr(input, '/')) {
    unsigned int addr;
    int len = ircd_aton_ip4(input, &addr, pbits);
    if (len) {
      ip->in6_16[5] = htons(65535);
      ip->in6_16[6] = htons(ntohl(addr) >> 16);
      ip->in6_16[7] = htons(ntohl(addr) & 65535);
      if (pbits)
        *pbits += 96;
    }
    return len;
  } else if (input[0] == '*') {
    unsigned int pos = 0;
    while (input[++pos] == '*') ;
    if (input[pos] != '\0')
      return 0;
    if (pbits)
      *pbits = 0;
    return pos;
  } else return 0; /* parse failed */
}

/** Convert an IP address to printable ASCII form.
 * @param[out] buf Output buffer to write to.
 * @param[in] in Address to format.
 * @return Pointer to the output buffer \a buf.
 */
inline const char* ircd_ntoa_r(char* buf, const struct irc_in_addr* in, bool unfolded = false)
{
    assert(buf != NULL);
    assert(in != NULL);

    if (irc_in_addr_is_ipv4(in)) {
      unsigned int pos, len;
      unsigned char *pch;

      pch = (unsigned char*)&in->in6_16[6];
      len = strlen(IpQuadTab[*pch]);
      memcpy(buf, IpQuadTab[*pch++], len);
      pos = len;
      buf[pos++] = '.';
      len = strlen(IpQuadTab[*pch]);
      memcpy(buf+pos, IpQuadTab[*pch++], len);
      pos += len;
      buf[pos++] = '.';
      len = strlen(IpQuadTab[*pch]);
      memcpy(buf+pos, IpQuadTab[*pch++], len);
      pos += len;
      buf[pos++] = '.';
      len = strlen(IpQuadTab[*pch]);
      memcpy(buf+pos, IpQuadTab[*pch++], len);
      buf[pos + len] = '\0';
      return buf;
    } else {
      static const char hexdigits[] = "0123456789abcdef";
      unsigned int pos, part, max_start, max_zeros, curr_zeros, ii;

      /* Find longest run of zeros. */
      for (max_start = ii = 1, max_zeros = curr_zeros = 0; ii < 8; ++ii) {
        if (!in->in6_16[ii])
          curr_zeros++;
        else if (curr_zeros > max_zeros) {
          max_start = ii - curr_zeros;
          max_zeros = curr_zeros;
          curr_zeros = 0;
        }
      }
      if (curr_zeros > max_zeros) {
        max_start = ii - curr_zeros;
        max_zeros = curr_zeros;
      }

      /* Print out address. */
/** Append \a CH to the output buffer. */
#define APPEND(CH) do { buf[pos++] = (CH); } while (0)
      for (pos = ii = 0; (ii < 8); ++ii) {
    	if (!unfolded)
        if ((max_zeros > 0) && (ii == max_start)) {
          APPEND(':');
          ii += max_zeros - 1;
          continue;
        }
        part = ntohs(in->in6_16[ii]);
        if ((part >= 0x1000) || (unfolded))
          APPEND(hexdigits[part >> 12]);
        if ((part >= 0x100) || (unfolded))
          APPEND(hexdigits[(part >> 8) & 15]);
        if ((part >= 0x10) || (unfolded))
          APPEND(hexdigits[(part >> 4) & 15]);
        APPEND(hexdigits[part & 15]);
        if (ii < 7)
          APPEND(':');
      }
#undef APPEND

      /* Nul terminate and return number of characters used. */
      buf[pos++] = '\0';
      return buf;
    }
}

/** Convert an IP address to printable ASCII form.
 * This is generally deprecated in favor of ircd_ntoa_r().
 * @param[in] in Address to convert.
 * @return Pointer to a static buffer containing the readable form.
 */
inline const char* ircd_ntoa(const struct irc_in_addr* in, bool unfolded = false)
{
  static char buf[SOCKIPLEN];
  return ircd_ntoa_r(buf, in, unfolded);
}

///Temporary (removable) functions
inline irc_in_addr NullIPv6()
{
	irc_in_addr res;
	for (unsigned short i = 0; i < 8; i ++)
		res.in6_16[i] = 0;
	return res;
}
///////////end/////////////////

/////// Test functions ////////
inline std::string DecToBin(unsigned short number)
{
    std::string result = "";
    unsigned short count = 0;
    short space = 0;
    const size_t size = sizeof(number) * 8;

    do
    {
        if ((number & 1) == 0 )
            result += "0";
        else
            result += "1";

        space++;
        count++;

        if ((space == 4) && (count < size))
        {
        	result += ' ';
        	space = 0;
        }
        number >>= 1;

    } while ( number );

   	for ( ; count < size; count++)
   	{
        if (space == 4)
        {
        	result += ' ';
        	space = 0;
        }
   		space++;
        result += '0';
   	}

    std::reverse(result.begin(), result.end());
    return result;
}

//Legacy function
inline unsigned int convertStrIPtoInt(std::string IP)
{
	int client_addr[4] = { 0 };
	sscanf(IP.c_str(), "%d.%d.%d.%d", &client_addr[0], &client_addr[1], &client_addr[2], &client_addr[3]);
	return ntohl((client_addr[0]) | (client_addr[1] << 8) | (client_addr[2] << 16) | (client_addr[3] << 24));
}

//Legacy function
inline std::string convertIntToStrIP(unsigned IP)
{
    unsigned mask_ip = htonl(IP);
    struct in_addr tmp_ip;
    tmp_ip.s_addr = mask_ip;
    char* client_ip;
    client_ip = inet_ntoa(tmp_ip);
    return (std::string)client_ip;
}

inline std::string print_irc_in_addr(const irc_in_addr& IP)
{
	std::string res = DecToBin(ntohs(IP.in6_16[0]));
	for (unsigned short i = 1; i < 8; i++)
		res += "|" + DecToBin(ntohs(IP.in6_16[i]));
	return res;
}
/////// Test functions end ////////

inline irc_in_addr irc_IPCIDRMinIP(const std::string& IP, unsigned CClonesCIDR = 120)
{
	unsigned char maskbits;
	irc_in_addr ircip;
	ipmask_parse(IP.c_str(), &ircip, &maskbits);
	unsigned int quot = (128 - CClonesCIDR)/16;
	unsigned int rem = (128 - CClonesCIDR) % 16;
	unsigned int i;
	for (i = 0; i < quot; i++)
		ircip.in6_16[7-i] = 0;
	unsigned short ip16 = ntohs(ircip.in6_16[7-i]);
	ip16 >>= rem;
	ip16 <<= rem;
	ircip.in6_16[7-i] = htons(ip16);
	return ircip;
}

inline std::string IPCIDRMinIP(const std::string& IP, unsigned CClonesCIDR = 120)
{
	irc_in_addr ip6 = irc_IPCIDRMinIP(IP, CClonesCIDR);
	return (std::string)ircd_ntoa(&ip6);
}

} // namespace gnuworld

#endif // __NUMERIC_H
