/* xtypes.h
 */

#ifndef __XTYPES__
#define __XTYPES__ "$Id: xtypes.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $"

#ifdef __alpha
typedef int                xINT32;
typedef int                xLONG32;
typedef long               xINT64;
typedef long               xLONG64;
typedef unsigned int       xUINT32;
typedef unsigned int       xULONG32;
typedef unsigned long      xUINT64;
typedef unsigned long      xULONG64;
#else
typedef int                xINT32;
typedef long               xLONG32;
typedef long long          xINT64;
typedef long long          xLONG64;
typedef unsigned int       xUINT32;
typedef unsigned long      xULONG32;
typedef unsigned long long xUINT64;
typedef unsigned long long xULONG64;
#endif

#define xINT   xINT32
#define xLONG  xLONG32
#define xUINT  xUINT32
#define xULONG xULONG32

typedef char               xCHAR;
typedef char               xINT8;
typedef unsigned char      xUCHAR;
typedef unsigned char      xUINT8;
typedef unsigned char      xBYTE;

typedef short              xSHORT;
typedef short              xINT16;
typedef unsigned short     xUSHORT;
typedef unsigned short     xUINT16;
typedef unsigned short     xWORD;

#endif // __XTYPES__
