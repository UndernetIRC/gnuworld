/**********************************************************************
 * $Workfile: MD5.H $
 * $Revision: 1.4 $
 *  $Modtime: 1/08/97 6:35p $
 *
 * PURPOSE:
 * MD5 Message-Digest class derived from the RSA Data Security, Inc. 
 * MD5 Message-Digest Algorithm as published in RFC 1321, April 1992.
 * 
 * ASSUMES:
 * size_t as unsigned.
 *
 * NOTE:
 * Currently the md5::update length is limited to unsigned int, which may
 * be less than unit32_t.
 * 
 * COPYRIGHT:
 * Copyright (c) 1995, 1996, 1997 Tree Frog Software, All rights reserved.
 * This source code and the binaries that result may be freely distributed,
 * used and modified as long as the above copyright notice remains intact.
 * 
 * WARRANTY:
 * The author of md5.h (hereafter referred to as "the author") makes no
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
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 * 
 * So there! 
 **********************************************************************/
#ifndef MD5_H
#define MD5_H "$Id: md5hash.h,v 1.4 2005/11/16 21:40:37 kewlio Exp $"

#include <iostream>
 
#define MD5_DIGEST_LENGTH 16

namespace gnuworld
{

class md5Digest;     // Forward declaration.

class md5 {
public:
   md5(  void );                       // Automatically calls md5::init().
   virtual ~md5( void );               // Automatically calls md5::clear().
   void update( const unsigned char *input, size_t length );  // Process data.
   void report( md5Digest &digest );   // Used to generate the digest.
   void init(  void );                 // Used for reinitialization.
   void clear( void );                 // Used to wipe clean interal data.
protected:
   uint32_t  m_state[4];              // State (ABCD)
   uint32_t  m_count[2];              // Number of bits, modulo 2^64 (lsb first).
   unsigned char  m_buffer[64];       // Input buffer.
   void transform( const unsigned char block[64] );
   void encode( unsigned char  *output, const uint32_t  *input, int32_t len );
   void encode( md5Digest &digest );
   void decode( uint32_t  *output, const unsigned char  *input, int32_t len );
};

class md5Digest {
   friend class md5 ;
public:
   md5Digest(  void );              // Automatically calls md5Digest::clear().
   ~md5Digest( void );              // Automatically calls md5Digest::clear().
   unsigned char &operator[]( size_t ii );        // lvalue.
   unsigned char  operator[]( size_t ii ) const;  // rvalue.
   void   clear( void );

private:
   unsigned char m_data[ MD5_DIGEST_LENGTH ];

   friend int32_t      operator==( const md5Digest &lhs, const md5Digest &rhs );
   friend int32_t      operator!=( const md5Digest &lhs, const md5Digest &rhs );
   friend std::istream& operator>>( std::istream& stream,
				md5Digest& digest );
   friend std::ostream& operator<<( std::ostream& stream,
				const md5Digest& digest );
//   friend void     md5::encode( md5Digest &digest );
};

} // namespace gnuworld

#endif // MD5_H
