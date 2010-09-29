/*************************************************************************
 * $Workfile: MD5.CPP $
 * $Revision: 1.3 $
 *  $Modtime: 1/08/97 6:35p $
 *
 * PURPOSE:
 * MD5 Message digest class derived from the RSA Data Security, Inc. 
 * MD5 Message-Digest Algorithm as published in RFC 1321, April 1992.
 * 
 * COPYRIGHT:
 * Copyright (c) 1995, 1996, 1997 Tree Frog Software, All rights reserved.
 * This source code and the binaries that result may be freely distributed,
 * used and modified as long as the above copyright notice remains intact.
 * 
 * WARRANTY:
 * The author of md5.cpp (hereafter referred to as "the author") makes no
 * warranty of any kind, expressed or implied, including without limitation, 
 * any warranties of merchantability and/or fitness for a particular purpose.
 * The author shall not be liable for any damages, whether direct, indirect,
 * special, or consequential arising from a failure of this program to 
 * operate in the manner desired by the user.  The author shall not be liable
 * for any damage to data or property which may be caused directly or 
 * indirectly by use of the program.
 *
 * In no event will the author be liable to the user for any damages,
 * including any lost profits, lost savings, or other incidental or 
 * consequential damages arising out of the use or inability to use the 
 * program, or for any claim by any other party.
 * 
 * -------------------- MD5 RSA copyright follows: -------------------------
 *
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 * 
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 * 
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 * 
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 * 
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 * 
 * So there! 
 ************************************************************************/
#include	<iostream>   // Needed for ostream and istream.
#include	<cstring>     // Needed for memcpy() and memset().
#include	<inttypes.h>
#include	"md5hash.h"

const char rcsId[] = "$Id: md5hash.cc,v 1.3 2005/11/16 21:40:37 kewlio Exp $" ;

namespace gnuworld
{

// F, G, H and I are basic MD5 functions.
inline uint32_t F( uint32_t xx, uint32_t yy, uint32_t zz ) 
{  return (( xx & yy ) | (~xx & zz ));  }

inline uint32_t G( uint32_t xx, uint32_t yy, uint32_t zz ) 
{  return (( xx & zz ) | ( yy & ~zz )); }

inline uint32_t H( uint32_t xx, uint32_t yy, uint32_t zz )
{  return xx ^ yy ^ zz; }

inline uint32_t I( uint32_t xx, uint32_t yy, uint32_t zz ) 
{  return yy ^ ( xx | ~zz ); }

// ROTATE_LEFT rotates x left n bits.
inline uint32_t ROTATE_LEFT( uint32_t xx, int32_t nn )
{  return ( xx << nn ) | ( xx >> (32-nn )); }

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
// Rotation is separate from addition to prevent recomputation.
inline void 
FF(uint32_t &aa, uint32_t bb, uint32_t cc, uint32_t dd, uint32_t xx, int32_t ss, uint32_t ac){ 
   aa += F( bb, cc, dd ) + xx + ac;
   aa = ROTATE_LEFT( aa, ss );
   aa += bb;
   return;
}

inline void
GG(uint32_t &aa, uint32_t bb, uint32_t cc, uint32_t dd, uint32_t xx, int32_t ss, uint32_t ac){ 
   aa += G( bb, cc, dd ) + xx + ac;
   aa = ROTATE_LEFT( aa, ss );
   aa += bb;
   return; 
}

inline void 
HH(uint32_t &aa, uint32_t bb, uint32_t cc, uint32_t dd, uint32_t xx, int32_t ss, uint32_t ac){ 
   aa += H( bb, cc, dd ) + xx + ac;
   aa = ROTATE_LEFT( aa, ss );
   aa += bb;
   return; 
}

inline void
II(uint32_t &aa, uint32_t bb, uint32_t cc, uint32_t dd, uint32_t xx, int32_t ss, uint32_t ac){ 
   aa += I( bb, cc, dd ) + xx + ac;
   aa = ROTATE_LEFT( aa, ss );
   aa += bb;
   return; 
}

md5Digest::md5Digest(  void )
{ 
   clear();
}

md5Digest::~md5Digest( void )
{
   clear();
}

void
md5Digest::clear( void )
{
   memset( m_data, 0, sizeof( m_data ));
   return;
}

unsigned char&
md5Digest::operator[]( size_t ii )  // lvalue operator[].
{
   static unsigned char val = 0;
   if( ii >= MD5_DIGEST_LENGTH ) {  // Note that size_t is unsigned.
      val = 0;
      return val;                   // Benign, don't throw an exception.
   }
   return m_data[ ii ];
}

unsigned char
md5Digest::operator[]( size_t ii ) const  // rvalue operator[].
{
   if( ii >= MD5_DIGEST_LENGTH ) {  // Note that size_t is unsigned.
      return 0;                     // Benign, don't throw an exception.
   }
   return m_data[ ii ];
}

int32_t 
operator==( const md5Digest &lhs, const md5Digest &rhs )
{
   int32_t result = 1;   // Assume true to begin with.
   for( int32_t ii = 0; result && ii < MD5_DIGEST_LENGTH; ii++ ) {
      result = lhs.m_data[ii] == rhs.m_data[ii];
   }
   return result;
}

int32_t
operator!=( const md5Digest &lhs, const md5Digest &rhs )
{
   return !( lhs == rhs );
}

std::istream& 
operator>>( std::istream& stream, md5Digest& digest )
{
   stream.read( reinterpret_cast< char* >( digest.m_data ),
	MD5_DIGEST_LENGTH );
//   for( int32_t ii = 0; ii < MD5_DIGEST_LENGTH; ii++ ) {
//      stream >> digest.m_data[ii];
//   }
   return stream;
}

std::ostream& 
operator<<( std::ostream& stream, const md5Digest& digest )
{
   stream.write( reinterpret_cast< const char* >( digest.m_data ),
	MD5_DIGEST_LENGTH );
//   for( int32_t ii = 0; ii < MD5_DIGEST_LENGTH; ii++ ) {
//      stream << digest.m_data[ii];
//   }
   return stream;
}

md5::md5( void )
{
   init();  // Set up the initial state values.
}

md5::~md5( void )
{
   clear(); // Wipe the class data clean.
}

void
md5::init( void )
{  // Load magic initialization constants.
   m_state[0] = 0x67452301L;  // These values are only used here.
   m_state[1] = 0xefcdab89L;
   m_state[2] = 0x98badcfeL;
   m_state[3] = 0x10325476L;
   m_count[0] = m_count[1] = 0;
   memset( m_buffer, 0, sizeof( m_buffer ));
   return;
}

void 
md5::clear( void )
{  // Zero the data memebers of this class.
   m_state[0] = m_state[1] = m_state[2] = m_state[3] = 0;
   m_count[0] = m_count[1] = 0;
   memset( m_buffer, 0, sizeof( m_buffer ));
   return;
}

void
md5::update( const unsigned char *input, size_t inputLen )
{  // MD5 block update operation. Continues an MD5 message-digest
   // operation, processing another message block, and updating the
   // context.

   // Compute number of bytes mod 64.
   size_t index = (size_t)((m_count[0] >> 3) & 0x3F);

   // Update number of bits.
   if(( m_count[0] += ((uint32_t)inputLen << 3)) < ((uint32_t)inputLen << 3)) {
      m_count[1]++;
   }
   m_count[1] += ((uint32_t)inputLen >> 29);

   size_t partLen = 64 - index;

   // Transform as many times as possible.
   size_t ii = 0;
   if( inputLen >= partLen ) {
      memcpy( &m_buffer[index], input, partLen);
      transform( m_buffer );

      for( ii = partLen; ii + 63 < inputLen; ii += 64 ) {
         transform( &input[ii] );
      }
      index = 0;
   }
   if( inputLen - ii ) {
      // Buffer remaining input.
      memcpy( &m_buffer[index],&input[ii],inputLen-ii );
   }
   return;
}

void 
md5::report( md5Digest &digest )
{  // MD5 finalization. Ends an MD5 message-digest operation, writing the
   // the message digest and zeroizing the context.
   static unsigned char padding[64] = {
      0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
   unsigned char bits[8];

   encode( bits, m_count, 8 );    // Save number of bits

   // Pad out to 56 mod 64.
   size_t index  = (size_t)((m_count[0] >> 3) & 0x3f);
   size_t padLen = (index < 56) ? (56 - index) : (120 - index);
   
   update( padding, padLen );
   update( bits,    8 );         // Append length (before padding).

   encode( digest );             // Store state in digest.
   clear();                      // Zero sensitive information.
   return;
}

void
md5::transform( const unsigned char block[64] )
{  // MD5 basic transformation. Transforms state based on block.
   const int32_t S11 =  7;
   const int32_t S12 = 12;
   const int32_t S13 = 17;
   const int32_t S14 = 22;
   const int32_t S21 =  5;
   const int32_t S22 =  9;
   const int32_t S23 = 14;
   const int32_t S24 = 20;
   const int32_t S31 =  4;
   const int32_t S32 = 11;
   const int32_t S33 = 16;
   const int32_t S34 = 23;
   const int32_t S41 =  6;
   const int32_t S42 = 10;
   const int32_t S43 = 15;
   const int32_t S44 = 21;

   uint32_t aa = m_state[0], bb = m_state[1], cc = m_state[2], dd = m_state[3];
   uint32_t xx[16];

   decode( xx, block, 64 );

   /* Round 1 */
   FF( aa, bb, cc, dd, xx[ 0], S11, 0xd76aa478L); /* 1 */
   FF( dd, aa, bb, cc, xx[ 1], S12, 0xe8c7b756L); /* 2 */
   FF( cc, dd, aa, bb, xx[ 2], S13, 0x242070dbL); /* 3 */
   FF( bb, cc, dd, aa, xx[ 3], S14, 0xc1bdceeeL); /* 4 */
   FF( aa, bb, cc, dd, xx[ 4], S11, 0xf57c0fafL); /* 5 */
   FF( dd, aa, bb, cc, xx[ 5], S12, 0x4787c62aL); /* 6 */
   FF( cc, dd, aa, bb, xx[ 6], S13, 0xa8304613L); /* 7 */
   FF( bb, cc, dd, aa, xx[ 7], S14, 0xfd469501L); /* 8 */
   FF( aa, bb, cc, dd, xx[ 8], S11, 0x698098d8L); /* 9 */
   FF( dd, aa, bb, cc, xx[ 9], S12, 0x8b44f7afL); /* 10 */
   FF( cc, dd, aa, bb, xx[10], S13, 0xffff5bb1L); /* 11 */
   FF( bb, cc, dd, aa, xx[11], S14, 0x895cd7beL); /* 12 */
   FF( aa, bb, cc, dd, xx[12], S11, 0x6b901122L); /* 13 */
   FF( dd, aa, bb, cc, xx[13], S12, 0xfd987193L); /* 14 */
   FF( cc, dd, aa, bb, xx[14], S13, 0xa679438eL); /* 15 */
   FF( bb, cc, dd, aa, xx[15], S14, 0x49b40821L); /* 16 */

   /* Round 2 */
   GG( aa, bb, cc, dd, xx[ 1], S21, 0xf61e2562L); /* 17 */
   GG( dd, aa, bb, cc, xx[ 6], S22, 0xc040b340L); /* 18 */
   GG( cc, dd, aa, bb, xx[11], S23, 0x265e5a51L); /* 19 */
   GG( bb, cc, dd, aa, xx[ 0], S24, 0xe9b6c7aaL); /* 20 */
   GG( aa, bb, cc, dd, xx[ 5], S21, 0xd62f105dL); /* 21 */
   GG( dd, aa, bb, cc, xx[10], S22, 0x02441453L); /* 22 */
   GG( cc, dd, aa, bb, xx[15], S23, 0xd8a1e681L); /* 23 */
   GG( bb, cc, dd, aa, xx[ 4], S24, 0xe7d3fbc8L); /* 24 */
   GG( aa, bb, cc, dd, xx[ 9], S21, 0x21e1cde6L); /* 25 */
   GG( dd, aa, bb, cc, xx[14], S22, 0xc33707d6L); /* 26 */
   GG( cc, dd, aa, bb, xx[ 3], S23, 0xf4d50d87L); /* 27 */
   GG( bb, cc, dd, aa, xx[ 8], S24, 0x455a14edL); /* 28 */
   GG( aa, bb, cc, dd, xx[13], S21, 0xa9e3e905L); /* 29 */
   GG( dd, aa, bb, cc, xx[ 2], S22, 0xfcefa3f8L); /* 30 */
   GG( cc, dd, aa, bb, xx[ 7], S23, 0x676f02d9L); /* 31 */
   GG( bb, cc, dd, aa, xx[12], S24, 0x8d2a4c8aL); /* 32 */
 
   /* Round 3 */
   HH( aa, bb, cc, dd, xx[ 5], S31, 0xfffa3942L); /* 33 */
   HH( dd, aa, bb, cc, xx[ 8], S32, 0x8771f681L); /* 34 */
   HH( cc, dd, aa, bb, xx[11], S33, 0x6d9d6122L); /* 35 */
   HH( bb, cc, dd, aa, xx[14], S34, 0xfde5380cL); /* 36 */
   HH( aa, bb, cc, dd, xx[ 1], S31, 0xa4beea44L); /* 37 */
   HH( dd, aa, bb, cc, xx[ 4], S32, 0x4bdecfa9L); /* 38 */
   HH( cc, dd, aa, bb, xx[ 7], S33, 0xf6bb4b60L); /* 39 */
   HH( bb, cc, dd, aa, xx[10], S34, 0xbebfbc70L); /* 40 */
   HH( aa, bb, cc, dd, xx[13], S31, 0x289b7ec6L); /* 41 */
   HH( dd, aa, bb, cc, xx[ 0], S32, 0xeaa127faL); /* 42 */
   HH( cc, dd, aa, bb, xx[ 3], S33, 0xd4ef3085L); /* 43 */
   HH( bb, cc, dd, aa, xx[ 6], S34, 0x04881d05L); /* 44 */
   HH( aa, bb, cc, dd, xx[ 9], S31, 0xd9d4d039L); /* 45 */
   HH( dd, aa, bb, cc, xx[12], S32, 0xe6db99e5L); /* 46 */
   HH( cc, dd, aa, bb, xx[15], S33, 0x1fa27cf8L); /* 47 */
   HH( bb, cc, dd, aa, xx[ 2], S34, 0xc4ac5665L); /* 48 */
 
   /* Round 4 */
   II( aa, bb, cc, dd, xx[ 0], S41, 0xf4292244L); /* 49 */
   II( dd, aa, bb, cc, xx[ 7], S42, 0x432aff97L); /* 50 */
   II( cc, dd, aa, bb, xx[14], S43, 0xab9423a7L); /* 51 */
   II( bb, cc, dd, aa, xx[ 5], S44, 0xfc93a039L); /* 52 */
   II( aa, bb, cc, dd, xx[12], S41, 0x655b59c3L); /* 53 */
   II( dd, aa, bb, cc, xx[ 3], S42, 0x8f0ccc92L); /* 54 */
   II( cc, dd, aa, bb, xx[10], S43, 0xffeff47dL); /* 55 */
   II( bb, cc, dd, aa, xx[ 1], S44, 0x85845dd1L); /* 56 */
   II( aa, bb, cc, dd, xx[ 8], S41, 0x6fa87e4fL); /* 57 */
   II( dd, aa, bb, cc, xx[15], S42, 0xfe2ce6e0L); /* 58 */
   II( cc, dd, aa, bb, xx[ 6], S43, 0xa3014314L); /* 59 */
   II( bb, cc, dd, aa, xx[13], S44, 0x4e0811a1L); /* 60 */
   II( aa, bb, cc, dd, xx[ 4], S41, 0xf7537e82L); /* 61 */
   II( dd, aa, bb, cc, xx[11], S42, 0xbd3af235L); /* 62 */
   II( cc, dd, aa, bb, xx[ 2], S43, 0x2ad7d2bbL); /* 63 */
   II( bb, cc, dd, aa, xx[ 9], S44, 0xeb86d391L); /* 64 */
 
   m_state[0] += aa;
   m_state[1] += bb;
   m_state[2] += cc;
   m_state[3] += dd;

   memset((unsigned char*)xx, 0, sizeof (xx));  // Clear sensitive information.
   return;
}

void
md5::encode( unsigned char  *output, const uint32_t *input, int32_t len )
{  // Encodes input (uint32_t) into output (unsigned char).
   // Assumes len is a multiple of 4.
  int32_t ii, jj;
  for( ii = 0, jj = 0; jj < len; ii++, jj += 4 ) {
     output[jj]   = (unsigned char)(  input[ii]         & 0xFF );
     output[jj+1] = (unsigned char)(( input[ii] >>  8 ) & 0xFF );
     output[jj+2] = (unsigned char)(( input[ii] >> 16 ) & 0xFF );
     output[jj+3] = (unsigned char)(( input[ii] >> 24 ) & 0xFF );
  }
  return;
}

void
md5::encode( md5Digest &digest )
{  // Encodes m_state into digest.
  int32_t ii, jj;
  for( ii = 0, jj = 0; jj < MD5_DIGEST_LENGTH; ii++, jj += 4 ) {
     digest.m_data[jj]   = (unsigned char)(  m_state[ii]         & 0xFF );
     digest.m_data[jj+1] = (unsigned char)(( m_state[ii] >>  8 ) & 0xFF );
     digest.m_data[jj+2] = (unsigned char)(( m_state[ii] >> 16 ) & 0xFF );
     digest.m_data[jj+3] = (unsigned char)(( m_state[ii] >> 24 ) & 0xFF );
  }
  return;
}

void
md5::decode( uint32_t *output, const unsigned char  *input, int32_t len )
{  // Decodes input (unsigned char) into output (uint32_t). 
   // Assumes len is a multiple of 4.
  int32_t ii, jj;
  for( ii = 0, jj = 0; jj < len; ii++, jj += 4 ) {
     output[ii] = ((uint32_t)input[jj]) | 
                 (((uint32_t)input[jj+1]) <<  8 ) |
                 (((uint32_t)input[jj+2]) << 16 ) | 
                 (((uint32_t)input[jj+3]) << 24 );
  }
  return;
}


} // Namespace
