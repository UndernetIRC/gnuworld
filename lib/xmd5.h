/* xmd5.h
 */

#ifndef __XMD5_H
#define __XMD5_H "$Id: xmd5.h,v 1.1 2000/06/30 18:46:06 dan_karrels Exp $"

#include	<cstring> 

#include	"xtypes.h"
#include	"misc.h"
#include	"md5.h"

/**
 * This class wraps MD5 functionality.
 */
class xMD5
{
public:
	struct xMD5Context
	{
		xINT32 buf[4];
		xINT32 bits[2];
		xBYTE  in[64];
	};
	
	xMD5()
		{ memset( &md5Ctx, 0, sizeof( xMD5Context ) ) ; }

	void Init()
		{ ::MD5Init((MD5_CTX*)&md5Ctx); }
	void Final()
		{ ::MD5Final((xBYTE*)Digest, (MD5_CTX*)&md5Ctx); }
	
	void Update(void* data, int len)
		{ ::MD5Update((MD5_CTX*)&md5Ctx, (xBYTE*)data, len); }

	xMD5Context& GetMD5Context()
		{ return md5Ctx ; }
	const char* GetDigest() const
		{ return Digest ; }
	
protected:
	char Digest[16];
	xMD5Context md5Ctx;

} ;

#endif // __XMD5_H
