/*
 * IRC - Internet Relay Chat, common/match.c
 * Copyright (C) 1990 Jarkko Oikarinen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: match.cc,v 1.10 2007/09/13 02:00:45 dan_karrels Exp $
 */

#include	<string>

#include	<cstdio>
#include	<cstdlib>

#include	"match.h"
#include	"ConnectionManager.h"

const char rcsId[] = "$Id: match.cc,v 1.10 2007/09/13 02:00:45 dan_karrels Exp $" ;

namespace gnuworld
{

int match( const std::string& s1, const std::string& s2 )
{
if( s1.empty() || s2.empty() )
	{
	return 1 ;
	}
return match( s1.c_str(), s2.c_str() ) ;
}

/*
 * Compare if a given string (name) matches the given
 * mask (which can contain wild cards: '*' - match any
 * number of chars, '?' - match any single character.
 *
 * return  0, if match
 *         1, if no match
 */

/*
 * match
 *
 * Rewritten by Andrea Cocito (Nemesi), November 1998.
 *
 */

/****************** Nemesi's match() ***************/

int match(const char *mask, const char *string)
{
  const char *m = mask, *s = string;
  char ch;
  const char *bm, *bs;          /* Will be reg anyway on a decent CPU/compiler */
  bool isIP = true, isCIDR = false;
  char CIDRip[16];
  int i = 0, CIDRmask = 0, dots = 0;
  int client_addr[4] = { 0 };
  unsigned long mask_ip, client_ip;

  /* check if the mask is a CIDR mask (also if it's an IP) */
  while ((ch = *m++))
  {
     if (ch == '.')
     {
	dots++;
	if (dots > 3)
	{
		isIP = false;	/* more than 3 dots, can't be an IP */
		break;		/* no point continuing the check as we have the info we want */
	}
     }
     if (ch == '/')
     {
        isCIDR = true;
        break;			/* break, preserving location of the slash in m */
     }
     if (i <= 15)
     {
        CIDRip[i] = ch;
        i++;
     }
     if (isIP && ((ch > '9') || (ch < '0')) && (ch != '.'))
     {
	/* not an IP */
	isIP = false;
	isCIDR = false;
	break;
     }
  }

  if (!isIP)
    isCIDR = false;		/* if it's not an IP, it can't be a CIDR mask! */
 
  if (isCIDR)
  {
     /* we have a CIDR mask, deal with it as such */
     /* we can only match CIDR masks against IPs - check for an IP */
     if (ConnectionManager::isIpAddress(string))
     {
        /* ok, it's an IP - compute masks etc */
        CIDRip[i] = '\0';
        CIDRmask = atoi(m);
        /* convert IP into integer */
        i = sscanf(CIDRip, "%d.%d.%d.%d", &client_addr[0], &client_addr[1], &client_addr[2], &client_addr[3]);
        mask_ip = ntohl((client_addr[0]) | (client_addr[1] << 8) | (client_addr[2] << 16) | (client_addr[3] << 24));
        i = sscanf(string, "%d.%d.%d.%d", &client_addr[0], &client_addr[1], &client_addr[2], &client_addr[3]);
        client_ip = ntohl((client_addr[0]) | (client_addr[1] << 8) | (client_addr[2] << 16) | (client_addr[3] << 24));
        /* time to compare them */
        for (i = 0; i < (32 - CIDRmask); i++)
        {
           /* right shift  to drop off the insignificant bits */
           mask_ip >>= 1;
           client_ip >>= 1;
        }
        for (i = 0; i < (32 - CIDRmask); i++)
        {
           /* left shift to restore the ip, but with insignificant bits = 0 */
           mask_ip <<= 1;
           client_ip <<= 1;
        }
        if (client_ip == mask_ip)
        {
           /* cidr matches */
           return 0;
        } else {
           /* cidr doesnt match */
           return 1;
        }
        return 1;
     }
  }

  m = mask;

  /* Process the "head" of the mask, if any */
  while ((ch = *m++) && (ch != '*'))
    switch (ch)
    {
      case '\\':
        if (*m == '?' || *m == '*')
          ch = *m++;
      default:
        if (ToLower(*s) != ToLower(ch))
          return 1;
      case '?':
        if (!*s++)
          return 1;
    };
  if (!ch)
    return *s;

  /* We got a star: quickly find if/where we match the next char */
got_star:
  bm = m;                       /* Next try rollback here */
  while ((ch = *m++))
    switch (ch)
    {
      case '?':
        if (!*s++)
          return 1;
      case '*':
        bm = m;
        continue;               /* while */
      case '\\':
        if (*m == '?' || *m == '*')
          ch = *m++;
      default:
        goto break_while;       /* C is structured ? */
    };
break_while:
  if (!ch)
    return 0;                   /* mask ends with '*', we got it */
  ch = ToLower(ch);
  while (ToLower(*s++) != ch)
    if (!*s)
      return 1;
  bs = s;                       /* Next try start from here */

  /* Check the rest of the "chunk" */
  while ((ch = *m++))
  {
    switch (ch)
    {
      case '*':
        goto got_star;
      case '\\':
        if (*m == '?' || *m == '*')
          ch = *m++;
      default:
        if (ToLower(*s) != ToLower(ch))
        {
          m = bm;
          s = bs;
          goto got_star;
        };
      case '?':
        if (!*s++)
          return 1;
    };
  };
  if (*s)
  {
    m = bm;
    s = bs;
    goto got_star;
  };
  return 0;
}

/*
 * collapse()
 * Collapse a pattern string into minimal components.
 * This particular version is "in place", so that it changes the pattern
 * which is to be reduced to a "minimal" size.
 *
 * (C) Carlo Wood - 6 Oct 1998
 * Speedup rewrite by Andrea Cocito, December 1998.
 * Note that this new optimized alghoritm can *only* work in place.
 */

char *collapse(char *mask)
{
  bool star = false ;
  char *m = mask;
  char *b;

  if (m)
  {
    do
    {
      if ((*m == '*') && ((m[1] == '*') || (m[1] == '?')))
      {
        b = m;
        do
        {
          if (*m == '*')
            star = true;
          else
          {
            if (star && (*m != '?'))
            {
              *b++ = '*';
              star = false;
            };
            *b++ = *m;
            if ((*m == '\\') && ((m[1] == '*') || (m[1] == '?')))
              *b++ = *++m;
          };
        }
        while (*m++);
        break;
      }
      else
      {
        if ((*m == '\\') && ((m[1] == '*') || (m[1] == '?')))
          m++;
      };
    }
    while (*m++);
  };
  return mask;
}

/*
 ***************** Nemesi's matchcomp() / matchexec() **************
 */

/* These functions allow the use of "compiled" masks, you compile a mask
 * by means of matchcomp() that gets the plain text mask as input and writes
 * its result in the memory locations addressed by the 3 parameters:
 * - *cmask will contain the text of the compiled mask
 * - *minlen will contain the lenght of the shortest string that can match 
 *   the mask
 * - *charset will contain the minimal set of chars needed to match the mask
 * You can pass NULL as *charset and it will be simply not returned, but you
 * MUST pass valid pointers for *minlen and *cmask (wich must be big enough 
 * to contain the compiled mask text that is in the worst case as long as the 
 * text of the mask itself in plaintext format) and the return value of 
 * matchcomp() will be the number of chars actually written there (excluded 
 * the trailing zero). cmask can be == mask, matchcomp() can work in place.
 * The {cmask, minlen} couple of values make the real compiled mask and
 * need to be passed to the functions that use the compiled mask, if you pass
 * the wrong minlen or something wrong in cmask to one of these expect a
 * coredump. This means that when you record a compiled mask you must store
 * *both* these values.
 * Once compiled the mask can be used to match a string by means of 
 * matchexec(), it can be printed back to human-readable format by means
 * of sprintmatch() or it can be compared to another compiled mask by means
 * of mmexec() that will tell if it completely overrides that mask (a lot like
 * what mmatch() does for plain text masks).
 * You can gain a lot of speed in many situations avoiding to matchexec() when:
 * - The maximum lenght of the field you are about to match() the mask to is
 *   shorter than minlen, in example when matching abc*def*ghil with a nick:
 *   It just cannot match since a nick is at most 9 chars long and the mask
 *   needs at least 10 chars (10 will be the value returned in minlen).
 * - The charset allowed for the field you are about to match to doesn't
 *   "contain" the charset returned by matchcomp(), in example when you
 *   have *.* as mask it makes no sense to try to match it against a nick
 *   because, again, a nick can't contain a '.', you can check this with
 *   a simple (charset & NTL_IRCNK) in this case.
 * - As a special case, since compiled masks are forced to lowercase,
 *   it would make no sense to use the NTL_LOWER and NTL_UPPER on a compiled
 *   mask, thus they are reused as follows: if the NTL_LOWER bit of charset
 *   is set it means that the mask contains only non-wilds chars (i.e. you can
 *   use strCasecmp() to match it or a direct hash lookup), if the NTL_UPPER
 *   bit is set it means that it contains only wild chars (and you can
 *   match it with strlen(field)>=minlen).
 * Do these optimizations ONLY when the data you are about to pass to
 * matchexec() are *known* to be invalid in advance, using strChattr() 
 * or strlen() on the text would be slower than calling matchexec() directly
 * and let it fail.
 * Internally a compiled mask contain in the *cmask area the text of
 * the plain text form of the mask itself with applied the following hacks:
 * - All characters are forced to lowercase (so that uppercase letters and
 *   specifically the symbols 'A' and 'Z' are reserved for special use)
 * - All non-escaped stars '*' are replaced by the letter 'Z'
 * - All non-escaped question marks '?' are replaced by the letter 'A' 
 * - All escape characters are removed, the wilds escaped by them are
 *   then passed by without the escape since they don't collide anymore
 *   with the real wilds (encoded as A/Z) 
 * - Finally the part of the mask that follows the last asterisk is
 *   reversed (byte order mirroring) and moved right after the first
 *   asterisk.
 * After all this a mask like:   Head*CHUNK1*chu\*nK2*ch??k3*TaIl 
 *               .... becomes:   headZliatZchunk1Zchu*nk2ZchAAk3
 * This can still be printed on a console, more or less understood by an
 * human and handled with the usual str*() library functions.
 * When you store somewhere the compiled mask you can avoid storing the
 * textform of it since it can be "decompiled" by means of sprintmatch(),
 * but at that time the following things are changed in the mask:
 * - All chars have been forced to lowercase.
 * - The mask is collapsed.
 * The balance point of using compiled masks in terms of CPU is when you expect
 * to use matchexec() instead of match() at least 20 times on the same mask
 * or when you expect to use mmexec() instead of mmatch() 3 times.
 */

 /* 
    * matchcomp()
    *
    * Compiles a mask into a form suitable for using in matchexec().
  */

int matchcomp(char *cmask, int *minlen, int *charset, const char *mask)
{
  const char *m = mask;
  char *b = cmask;
  char *fs = 0;
  char *ls = 0;
  char *x1, *x2;
  int l1, l2, lmin, loop, sign;
  bool star = false;
  int cnt = 0;
  char ch;
  int chset = ~0;
  int chset2 = (NTL_LOWER | NTL_UPPER);

  if (m)
    while ((ch = *m++))
      switch (ch)
      {
        case '*':
          star = true;
          break;
        case '?':
          cnt++;
          *b++ = 'A';
          chset2 &= ~NTL_LOWER;
          break;
        case '\\':
          if ((*m == '?') || (*m == '*'))
            ch = *m++;
        default:
          if (star)
          {
            ls = b;
            fs = fs ? fs : b;
            *b++ = 'Z';
            chset2 &= ~NTL_LOWER;
            star = false;
          };
          cnt++;
          *b = ToLower(ch);
          chset &= IRCD_CharAttrTab[*b++ - CHAR_MIN];
          chset2 &= ~NTL_UPPER;
      };

  if (charset)
    *charset = (chset | chset2);

  if (star)
  {
    ls = b;
    fs = (fs ? fs : b);
    *b++ = 'Z';
  };

  if (ls)
  {
    for (x1 = ls + 1, x2 = (b - 1); x1 < x2; x1++, x2--)
    {
      ch = *x1;
      *x1 = *x2;
      *x2 = ch;
    };
    l1 = (ls - fs);
    l2 = (b - ls);
    x1 = fs;
    while ((lmin = (l1 < l2) ? l1 : l2))
    {
      x2 = x1 + l1;
      for (loop = 0; loop < lmin; loop++)
      {
        ch = x1[loop];
        x1[loop] = x2[loop];
        x2[loop] = ch;
      };
      x1 += lmin;
      sign = l1 - l2;
      l1 -= (sign < 0) ? 0 : lmin;
      l2 -= (sign > 0) ? 0 : lmin;
    };
  };

  *b = '\0';
  *minlen = cnt;
  return (b - cmask);

}

/*
 * matchexec()
 *
 * Executes a match with a mask previosuly compiled with matchcomp()
 * Note 1: If the mask isn't correctly produced by matchcomp() I will core
 * Note 2: 'min' MUST be the value returned by matchcomp on that mask,
 *         or.... I will core even faster :-)
 * Note 3: This piece of code is not intended to be nice but efficient.
 */

int matchexec(const char *string, const char *cmask, int minlen)
{
  const char *s = string - 1;
  const char *b = cmask - 1;
  int trash;
  const char *bb, *bs;
  char ch;

tryhead:
  while ((ToLower(*++s) == *++b) && *s) { /* noop */ };
  if (!*s)
    return ((*b != '\0') && ((*b++ != 'Z') || (*b != '\0')));
  if (*b != 'Z')
  {
    if (*b == 'A')
      goto tryhead;
    return 1;
  };

  bs = s;
  while (*++s) { /* noop */ };

  if ((trash = (s - string - minlen)) < 0)
    return 2;

trytail:
  while ((ToLower(*--s) == *++b) && *b && (ToLower(*--s) == *++b) && *b
      && (ToLower(*--s) == *++b) && *b && (ToLower(*--s) == *++b) && *b) { /* noop */ };
  if (*b != 'Z')
  {
    if (*b == 'A')
      goto trytail;
    return (*b != '\0');
  };

  s = --bs;
  bb = b;

  while ((ch = *++b))
  {
    while ((ToLower(*++s) != ch))
      if (--trash < 0)
        return 4;
    bs = s;

trychunk:
    while ((ToLower(*++s) == *++b) && *b) { /* noop */ };
    if (!*b)
      return 0;
    if (*b == 'Z')
    {
      bs = --s;
      bb = b;
      continue;
    };
    if (*b == 'A')
      goto trychunk;

    b = bb;
    s = bs;
    if (--trash < 0)
      return 5;
  };

  return 0;
}

/*
 * matchdecomp()
 * Prints the human readable version of *cmask into *mask, (decompiles
 * cmask).
 * The area pointed by *mask MUST be big enough (the mask might be up to
 * twice the size of its compiled form if it's made all of \? or \*, and
 * this function can NOT work in place since it might enflate the mask)
 * The printed mask is not identical to the one that was compiled to cmask,
 * infact it is 1) forced to all lowercase, 2) collapsed, both things
 * are supposed to NOT change it's meaning.
 * It returns the number of chars actually written to *mask;
 */

int matchdecomp(char *mask, const char *cmask)
{
  char *rtb = mask;
  const char *rcm = cmask;
  const char *begtail, *endtail;

  if (rtb ==0)
    return (-1);

  if (rcm == 0)
    return (-2);

  for (; (*rcm != 'Z'); rcm++, rtb++)
  {
    if ((*rcm == '?') || (*rcm == '*'))
      *rtb++ = '\\';
    if (!((*rtb = ((*rcm == 'A') ? '?' : *rcm))))
      return (rtb - mask);
  };

  begtail = rcm++;
  *rtb++ = '*';

  while (*rcm && (*rcm != 'Z'))
    rcm++;

  endtail = rcm;

  if (*rcm)
  {
    while (*++rcm)
      switch (*rcm)
      {
        case 'A':
          *rtb++ = '?';
          break;
        case 'Z':
          *rtb++ = '*';
          break;
        case '*':
        case '?':
          *rtb++ = '\\';
        default:
          *rtb++ = *rcm;
      };
    *rtb++ = '*';
  };

  for (rcm = endtail; (--rcm) > begtail; *rtb++ = ((*rcm == 'A') ? '?' : *rcm))
    if ((*rcm == '?') || (*rcm == '*'))
      *rtb++ = '\\';

  *rtb = '\0';
  return (rtb - mask);
}

/*
 * mmexec()
 * Checks if a wider compiled mask (wcm/wminlen) completely overrides
 * a more restrict one (rcm/rminlen), basically what mmatch() does for
 * non-compiled masks, returns 0 if the override is true (like mmatch()).
 * "the wider overrides the restrict" means that any string that matches
 * the restrict one _will_ also match the wider one, always. 
 * In this we behave differently from mmatch() because in example we return 
 * true for " a?*cd overrides a*bcd " for wich the override happens for how 
 * we literally defined it, here mmatch() would have returned false.
 * The original concepts and the base alghoritm are copied from mmatch() 
 * written by Run (Carlo Wood), this function is written by
 * Nemesi (Andrea Cocito)
 */

int mmexec(const char *wcm, int wminlen, const char *rcm, int rminlen)
{
  const char *w, *r, *br, *bw, *rx, *rz;
  int eat, trash;

  /* First of all rm must have enough non-stars to 'contain' wm */
  if ((trash = rminlen - wminlen) < 0)
    return 1;
  w = wcm;
  r = rcm;
  eat = 0;

  /* Let's start the game, remember that '*' is mapped to 'Z', '?'
     is mapped to 'A' and that head?*??*?chunk*???*tail becomes
     headAAAAZliatAAAZchunk for compiled masks */

  /* Match the head of wm with the head of rm */
  for (; (*r) && (*r != 'Z') && ((*w == *r) || (*w == 'A')); r++, w++) { /* noop */ };
  if (*r == 'Z')
    while (*w == 'A')           /* Eat extra '?' before '*' in wm if got '*' in rm */
      w++, eat++;
  if (*w != 'Z')                /* head1<any>.. can't match head2<any>.. */
    return ((*w) || (*r)) ? 1 : 0;      /* and head<nul> matches only head<nul> */
  if (!*++w)
    return 0;                   /* headZ<nul> matches head<anything>    */

  /* Does rm have any stars in it ? let's check */
  for (rx = r; *r && (*r != 'Z'); r++) { /* noop */ };
  if (!*r)
  {
    /* rm has no stars and thus isn't a mask but it's just a flat
       string: special handling occurs here, note that eat must be 0 here */

    /* match the tail */
    if (*w != 'Z')
    {
      for (; r--, (*w) && ((*w == *r) || (*w == 'A')); w++) { /* noop */ };
      if (*w != 'Z')            /* headZliat1<any> fails on head<any>2tail  */
        return (*w) ? 1 : 0;    /* but headZliat<nul> matches head<any>tail */
    }

    /* match the chunks */
    while (1)
    {                           /* This loop can't break but only return   */

      for (bw = w++; (*w != *rx); rx++) /* Seek the 1st char of the chunk */
        if (--trash < 0)        /* See if we can trash one more char of rm */
          return 1;             /* If not we can only fail of course       */
      for (r = ++rx, w++; (*w) && ((*w == *r) || (*w == 'A')); r++, w++) { /* noop */ };
      if (!*w)                  /* Did last loop match the rest of chunk ? */
        return 0;               /* ... Yes, end of wm, matched !           */
      if (*w != 'Z')
      {                         /* ... No, hitted non-star                 */
        w = bw;                 /* Rollback at beginning of chunk          */
        if (--trash < 0)        /* Trashed the char where this try started */
          return 1;             /* if we can't trash more chars fail       */
      }
      else
      {
        rx = r;                 /* Successfully matched a chunk, move rx   */
      }                 /* and go on with the next one             */
    }
  }

  /* rm has at least one '*' and thus is a 'real' mask */
  rz = r++;                     /* rx = unused of head, rz = beg-tail */

  /* Match the tail of wm (if any) against the tail of rm */
  if (*w != 'Z')
  {
    for (; (*w) && (*r != 'Z') && ((*w == *r) || (*w == 'A')); w++, r++) { /* noop */ };
    if (*r == 'Z')              /* extra '?' before tail are fluff, just flush 'em */
      while (*w == 'A')
        w++;
    if (*w != 'Z')              /* We aren't matching a chunk, can't rollback      */
      return (*w) ? 1 : 0;
  }

  /* Match the chunks of wm against what remains of the head of rm */
  while (1)
  {
    bw = w;
    for (bw++; (rx < rz) && (*bw != *rx); rx++) /* Seek the first           */
      if (--trash < 0)          /* waste some trash reserve */
        return 1;
    if (!(rx < rz))             /* head finished            */
      break;
    for (bw++, (br = ++rx);
        (br < rz) && (*bw) && ((*bw == *br) || (*bw == 'A')); br++, bw++) { /* noop */ };
    if (!(br < rz))             /* Note that we didn't use any 'eat' char yet, if  */
      while (*bw == 'A')        /* there were eat-en chars the head would be over  */
        bw++, eat++;            /* Happens only at end of head, and eat is still 0 */
    if (!*bw)
      return 0;
    if (*bw != 'Z')
    {
      eat = 0;
      if (!(br < rz))
      {                         /* If we failed because we got the end of head */
        trash -= (br - rx);     /* it makes no sense to rollback, just trash   */
        if (--trash < 0)        /* all the rest of the head wich isn't long    */
          return 1;             /* enough for this chunk and go out of this    */
        break;                  /* loop, then we try with the chunks of rm     */
      };
      if (--trash < 0)
        return 1;
    }
    else
    {
      w = bw;
      rx = br;
    }
  }

  /* Match the unused chunks of wm against the chunks of rm */
  rx = r;
  for (; *r && (*r != 'Z'); r++) { /* noop */ };
  rz = r;
  if (*r++)
  {
    while (*r)
    {
      bw = w;
      while (eat && *r)         /* the '?' we had eated make us skip as many chars */
        if (*r++ != 'Z')        /* here, but can't skip stars or trailing zero     */
          eat--;
      for (bw++; (*r) && (*bw != *r); r++)
        if ((*r != 'Z') && (--trash < 0))
          return 1;
      if (!*r)
        break;
      for ((br = ++r), bw++;
          (*br) && (*br != 'Z') && ((*bw == *br) || (*bw == 'A')); br++, bw++) { /* noop */ };
      if (*br == 'Z')
        while (*bw == 'A')
          bw++, eat++;
      if (!*bw)
        return 0;
      if (*bw != 'Z')
      {
        eat = 0;
        if ((!*br) || (*r == 'Z'))
        {                       /* If we hit the end of rm or a star in it */
          trash -= (br - r);    /* makes no sense to rollback within this  */
          if (trash < 0)        /* same chunk of br, skip it all and then  */
            return 1;           /* either rollback or break this loop if   */
          if (!*br)             /* it was the end of rm                    */
            break;
          r = br;
        }
        if (--trash < 0)
          return 1;
      }
      else
      {
        r = br;
        w = bw;
      }
    }
  }

  /* match the remaining chunks of wm against what remains of the tail of rm */
  r = rz - eat - 1;             /* can't have <nul> or 'Z'within the tail, so just move r */
  while (r >= rx)
  {
    bw = w;
    for (bw++; (*bw != *r); r--)
      if (--trash < 0)
        return 1;
    if (!(r >= rx))
      return 1;
    for ((br = --r), bw++;
        (*bw) && (br >= rx) && ((*bw == *br) || (*bw == 'A')); br--, bw++) { /* noop */ };
    if (!*bw)
      return 0;
    if (!(br >= rx))
      return 1;
    if (*bw != 'Z')
    {
      if (--trash < 0)
        return 1;
    }
    else
    {
      r = br;
      w = bw;
    }
  }
  return 1;                     /* Auch... something left out ? Fail */
}

} // namespace gnuworld
