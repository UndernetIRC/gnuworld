/*
 * match.h
 *
 * $Id: match.h,v 1.1 2001/01/12 22:16:21 dan_karrels Exp $
 */
#ifndef INCLUDED_match_h
#define INCLUDED_match_h

#include	"match_table.h"
#include	"ircd_chattr.h"

/*
 * Prototypes
 */

/*
 * XXX - match returns 0 if a match is found. Smelly interface
 * needs to be fixed. --Bleep
 */
int mmatch(const char *old_mask, const char *new_mask);
int match(const char *ma, const char *na);
char *collapse(char *pattern);

int matchcomp(char *cmask, int *minlen, int *charset, const char *mask);
int matchexec(const char *string, const char *cmask, int minlen);
int matchdecomp(char *mask, const char *cmask);
int mmexec(const char *wcm, int wminlen, const char *rcm, int rminlen);

#endif /* INCLUDED_match_h */
