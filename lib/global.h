/* GLOBAL.H - RSAREF types and constants
 */

#ifndef __GLOBAL_H
#define __GLOBAL_H "$Id: global.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $"

/**
 * This defines a generic pointer type used for MD5.
 */
typedef unsigned char* POINTER ;

/**
 * This defines a two byte word, used for MD5.
 */
typedef unsigned short int UINT2;

/**
 * This defines a 32 bit word, used for MD5.
 */
#define xUINT32 unsigned int

/**
 * This defines a four byte word, used for MD5.
 */
typedef unsigned long int UINT4;

#endif /* __GLOBAL_H */
