/**
 * match.h
 *
 * $Id: match.h,v 1.2 2003/06/17 15:13:53 dan_karrels Exp $
 */
#ifndef __MATCH_H
#define __MATCH_H "$Id: match.h,v 1.2 2003/06/17 15:13:53 dan_karrels Exp $"

#include	<string>

#include	"match_table.h"
#include	"ircd_chattr.h"

namespace gnuworld
{

using std::string ;
/*
 * Prototypes
 */

/*
 * XXX - match returns 0 if a match is found. Smelly interface
 * needs to be fixed. --Bleep
 */
int match( const string&, const string& ) ;
//int mmatch( const char *old_mask, const char *new_mask);
int match( const char *ma, const char *na);
char *collapse(char *pattern);

int matchcomp(char *cmask, int *minlen, int *charset, const char *mask);
int matchexec(const char *string, const char *cmask, int minlen);
int matchdecomp(char *mask, const char *cmask);
int mmexec(const char *wcm, int wminlen, const char *rcm, int rminlen);

} // namespace gnuworld

#endif // __MATCH_H
