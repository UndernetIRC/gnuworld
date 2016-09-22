/**
 * match.h
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
 * $Id: match.h,v 1.4 2003/12/29 23:59:36 dan_karrels Exp $
 */

#ifndef __MATCH_H
#define __MATCH_H "$Id: match.h,v 1.4 2003/12/29 23:59:36 dan_karrels Exp $"

#include	<string>

#include	"match_table.h"
#include	"ircd_chattr.h"
#include	"Numeric.h"
#include	"iClient.h"

namespace gnuworld
{
using std::string;
/*
 * Prototypes
 */

/*
 * XXX - match returns 0 if a match is found. Smelly interface
 * needs to be fixed. --Bleep
 */
int match( const string&, const string& ) ;
int match(const string&, const iClient*);
int match( const char *ma, const char *na);
int smatch( const char *mask, const char *name );
int casematch( const char *ma, const char *na);
int casematch( const std::string& s1, const std::string& s2 );
int mmatch( const char *old_mask, const char *new_mask);
int ipmask_check(const struct irc_in_addr *addr, const struct irc_in_addr *mask, unsigned char bits);

char *collapse(char *pattern);

int matchcomp(char *cmask, int *minlen, int *charset, const char *mask);
int matchexec(const char *string, const char *cmask, int minlen);
int matchdecomp(char *mask, const char *cmask);
int mmexec(const char *wcm, int wminlen, const char *rcm, int rminlen);
//Given mask matches with every mask
int matchall(const std::string&);
int cidrmatch(const char *mask1, const char *mask2);
int cidrmatch(const string&, const string&);

} // namespace gnuworld

#endif // __MATCH_H
