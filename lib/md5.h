/* md5.h
 */

#ifndef __MD5_H
#define __MD5_h "$Id: md5.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $"

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
*/

#include "global.h"

/**
 * MD5 context.
 */
struct MD5_CTX
{

  /// state (ABCD)
  xUINT32 state[4];

  /// number of bits, modulo 2^64 (lsb first)
  xUINT32 count[2];

  /// input buffer
  unsigned char buffer[64];

} ;

extern "C" {
void MD5Transform (UINT4 [4], unsigned char [64]);
void MD5_memcpy (POINTER, POINTER, unsigned int);
void MD5_memset (POINTER, int, unsigned int);

void Encode(unsigned char *, UINT4 *, unsigned int);
void Decode (UINT4 *, unsigned char *, unsigned int);

void MD5Init(MD5_CTX *);
void MD5Update(MD5_CTX *, unsigned char *, unsigned int);
void MD5Final(unsigned char [16], MD5_CTX *);
}
#endif /* __MD5_H */

