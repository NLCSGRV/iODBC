/*
 *  unicode.c
 *
 *  $Id$
 *
 *  ODBC unicode functions
 *
 *  The iODBC driver manager.
 *
 *  Copyright (C) 1996-2003 by OpenLink Software <iodbc@openlinksw.com>
 *  All Rights Reserved.
 *
 *  This software is released under the terms of either of the following
 *  licenses:
 *
 *      - GNU Library General Public License (see LICENSE.LGPL)
 *      - The BSD License (see LICENSE.BSD).
 *
 *  While not mandated by the BSD license, any patches you make to the
 *  iODBC source code may be contributed back into the iODBC project
 *  at your discretion. Contributions will benefit the Open Source and
 *  Data Access community as a whole. Submissions may be made at:
 *
 *      http://www.iodbc.org
 *
 *
 *  GNU Library Generic Public License Version 2
 *  ============================================
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 *  The BSD License
 *  ===============
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *  3. Neither the name of OpenLink Software Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL OPENLINK OR
 *  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#define UNICODE

#include <iodbc.h>

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>

#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <ansiapi.h>
#include <mapinls.h>
#endif

#include "unicode.h"

#if !defined(HAVE_WCSLEN)
size_t
wcslen (wchar_t * wcs)
{
  size_t len = 0;

  while (*wcs++ != L'\0')
    len++;

  return len;
}
#endif


#if !defined(HAVE_WCSCPY)
wchar_t *
wcscpy (wchar_t * wcd, const wchar_t * wcs)
{
  wchar_t *dst = wcd;

  while ((*dst++ = *wcs++) != L'\0')
    ;

  return wcd;
}
#endif


#if !defined (HAVE_WCSNCPY)
wchar_t *
wcsncpy (wchar_t * wcd, const wchar_t * wcs, size_t n)
{
  wchar_t *dst = wcd;
  size_t len = 0;

  while ( len < n && (*dst++ = *wcs++) != L'\0')
    len++;

  for (; len < n; len++)
    *dst++ = L'\0';

  return wcd;
}
#endif


SQLCHAR *
dm_SQL_W2A (SQLWCHAR * inStr, int size)
{
  SQLCHAR *outStr = NULL;
  int len;

  if (inStr == NULL)
    return NULL;

  if (size == SQL_NTS)
    len = wcslen (inStr);
  else
    len = size;

  if (len < 0)
    return NULL;

  if ((outStr = (SQLCHAR *) malloc (len + 1)) != NULL)
    {
      if (len > 0)
	OPL_W2A (inStr, outStr, len);
      outStr[len] = '\0';
    }

  return outStr;
}


SQLWCHAR *
dm_SQL_A2W (SQLCHAR * inStr, int size)
{
  SQLWCHAR *outStr = NULL;
  int len;

  if (inStr == NULL)
    return NULL;

  if (size == SQL_NTS)
    len = strlen ((char *) inStr);
  else
    len = size;

  if (len < 0)
    return NULL;

  if ((outStr = (SQLWCHAR *) calloc (len + 1, sizeof (SQLWCHAR))) != NULL)
    {
      if (len > 0)
	OPL_A2W (inStr, outStr, len);
      outStr[len] = L'\0';
    }

  return outStr;
}


int
dm_StrCopyOut2_A2W (
  SQLCHAR	* inStr,
  SQLWCHAR	* outStr,
  SQLSMALLINT	  size,
  SQLSMALLINT	* result)
{
  int length;

  if (!inStr)
    return -1;

  length = strlen ((char *) inStr);

  if (result)
    *result = length;

  if (!outStr)
    return 0;

  if (size >= length + 1)
    {
      if (length > 0)
	OPL_A2W (inStr, outStr, length);
      outStr[length] = L'\0';
      return 0;
    }
  if (size > 0)
    {
      OPL_A2W (inStr, outStr, size);
      outStr[--size] = L'\0';
    }
  return -1;
}


int
dm_StrCopyOut2_W2A (
  SQLWCHAR	* inStr,
  SQLCHAR	* outStr,
  SQLSMALLINT	  size,
  SQLSMALLINT	* result)
{
  int length;

  if (!inStr)
    return -1;

  length = wcslen (inStr);

  if (result)
    *result = length;

  if (!outStr)
    return 0;

  if (size >= length + 1)
    {
      if (length > 0)
	OPL_W2A (inStr, outStr, length);
      outStr[length] = '\0';
      return 0;
    }
  if (size > 0)
    {
      OPL_W2A (inStr, outStr, size);
      outStr[--size] = '\0';
    }
  return -1;
}


SQLWCHAR *
dm_strcpy_A2W (SQLWCHAR * destStr, SQLCHAR * sourStr)
{
  size_t length;

  if (!sourStr || !destStr)
    return destStr;

  length = strlen ((char *) sourStr);
  if (length > 0)
    OPL_A2W (sourStr, destStr, length);
  destStr[length] = L'\0';
  return destStr;
}


SQLCHAR *
dm_strcpy_W2A (SQLCHAR * destStr, SQLWCHAR * sourStr)
{
  size_t length;

  if (!sourStr || !destStr)
    return destStr;

  length = wcslen (sourStr);
  if (length > 0)
    OPL_W2A (sourStr, destStr, length);
  destStr[length] = '\0';
  return destStr;
}


static int
calc_len_for_utf8 (SQLWCHAR * str, int size)
{
  int len = 0;
  SQLWCHAR c;

  if (!str)
    return len;

  if (size == SQL_NTS)
    {
      while ((c = *str))
	{
	  if (c < 0x80)
	    len += 1;
	  else if (c < 0x800)
	    len += 2;
	  else if (c < 0x10000)
	    len += 3;
	  else if (c < 0x200000)
	    len += 4;
	  else
	    len += 1;

	  str++;
	}
    }
  else
    {
      while (size > 0)
	{
	  c = *str;
	  if (c < 0x80)
	    len += 1;
	  else if (c < 0x800)
	    len += 2;
	  else if (c < 0x10000)
	    len += 3;
	  else if (c < 0x200000)
	    len += 4;
	  else
	    len += 1;

	  str++;
	  size--;
	}
    }
  return len;
}


static int
utf8_len (SQLCHAR * p, int size)
{
  int len = 0;

  if (!*p)
    return 0;

  if (size == SQL_NTS)
    while (*p)
      {
	for (p++; (*p & 0xC0) == 0x80; p++)
	  ;
	len++;
      }
  else
    while (size > 0)
      {
	for (p++, size--; (size > 0) && ((*p & 0xC0) == 0x80); p++, size--)
	  ;
	len++;
      }
  return len;
}


/*
 *  size      - size of buffer for output utf8 string in bytes
 *  return    - length of output utf8 string
 */
static int
wcstoutf8 (SQLWCHAR * wstr, SQLCHAR * ustr, int size)
{
  int len;
  SQLWCHAR c;
  int first, i;
  int count = 0;

  if (!wstr)
    return 0;

  while ((c = *wstr) && count < size)
    {
      if (c < 0x80)
	{
	  len = 1;
	  first = 0;
	}
      else if (c < 0x800)
	{
	  len = 2;
	  first = 0xC0;
	}
      else if (c < 0x10000)
	{
	  len = 3;
	  first = 0xE0;
	}
      else if (c < 0x200000)
	{
	  len = 4;
	  first = 0xf0;
	}
      else
	{
	  len = 1;
	  first = 0;
	  c = '?';
	}

      if (size - count < len)
	{
	  return count;
	}

      for (i = len - 1; i > 0; --i)
	{
	  ustr[i] = (c & 0x3f) | 0x80;
	  c >>= 6;
	}
      ustr[0] = c | first;

      ustr += len;
      count += len;
      wstr++;
    }
  return count;
}


/*
 *  wlen      - length of input *wstr string in symbols
 *  size     - size of buffer ( *ustr string) in bytes
 *  converted - number of converted symbols from *wstr
 *
 *  Return    - length of output utf8 string
 */
static int
wcsntoutf8 (
  SQLWCHAR	* wstr,
  SQLCHAR	* ustr,
  int		  wlen,
  int		  size,
  int		* converted)
{
  int len;
  SQLWCHAR c;
  int first, i;
  int count = 0;
  int _converted = 0;

  if (!wstr)
    return 0;

  while (_converted < wlen && count < size)
    {
      c = *wstr;
      if (c < 0x80)
	{
	  len = 1;
	  first = 0;
	}
      else if (c < 0x800)
	{
	  len = 2;
	  first = 0xC0;
	}
      else if (c < 0x10000)
	{
	  len = 3;
	  first = 0xE0;
	}
      else if (c < 0x200000)
	{
	  len = 4;
	  first = 0xf0;
	}
      else
	{
	  len = 1;
	  first = 0;
	  c = '?';
	}

      if (size - count < len)
	{
	  if (converted)
	    *converted = _converted;
	  return count;
	}

      for (i = len - 1; i > 0; --i)
	{
	  ustr[i] = (c & 0x3f) | 0x80;
	  c >>= 6;
	}
      ustr[0] = c | first;

      ustr += len;
      count += len;
      wstr++;
      _converted++;
    }
  if (converted)
    *converted = _converted;
  return count;
}


static SQLCHAR *
strdup_WtoU8 (SQLWCHAR * str)
{
  SQLCHAR *ret;
  int len;

  if (!str)
    return NULL;

  len = calc_len_for_utf8 (str, SQL_NTS);
  if ((ret = (SQLCHAR *) malloc (len + 1)) == NULL)
    return NULL;

  len = wcstoutf8 (str, ret, len);
  ret[len] = '\0';

  return ret;
}


/* decode */
#define UTF8_COMPUTE(Char, Mask, Len)					      \
  if (Char < 128)							      \
    {									      \
      Len = 1;								      \
      Mask = 0x7f;							      \
    }									      \
  else if ((Char & 0xe0) == 0xc0)					      \
    {									      \
      Len = 2;								      \
      Mask = 0x1f;							      \
    }									      \
  else if ((Char & 0xf0) == 0xe0)					      \
    {									      \
      Len = 3;								      \
      Mask = 0x0f;							      \
    }									      \
  else if ((Char & 0xf8) == 0xf0)					      \
    {									      \
      Len = 4;								      \
      Mask = 0x07;							      \
    }									      \
  else									      \
    Len = -1;



/*
 *  size      - size of buffer for output string in symbols (SQLWCHAR)
 *  return    - length of output SQLWCHAR string
 */
static int
utf8towcs (SQLCHAR * ustr, SQLWCHAR * wstr, int size)
{
  int i, mask = 0, len;
  unsigned char c;
  SQLWCHAR wc;
  int count = 0;

  if (!ustr)
    return 0;

  while ((c = (unsigned char) *ustr) && count < size)
    {
      UTF8_COMPUTE (c, mask, len);
      if (len == -1)
	return count;

      wc = c & mask;
      for (i = 1; i < len; i++)
	{
	  if ((ustr[i] & 0xC0) != 0x80)
	    return count;
	  wc <<= 6;
	  wc |= (ustr[i] & 0x3F);
	}
      *wstr = wc;
      ustr += len;
      wstr++;
      count++;
    }
  return count;
}


/*
 *  ulen      - length of input *ustr string in bytes
 *  size      - size of buffer ( *wstr string) in symbols
 *  converted - number of converted bytes from *ustr
 *
 *  Return    - length of output wcs string
 */
static int
utf8ntowcs (
  SQLCHAR	* ustr,
  SQLWCHAR	* wstr,
  int		  ulen,
  int		  size,
  int		* converted)
{
  int i, mask = 0, len;
  unsigned char c;
  SQLWCHAR wc;
  int count = 0;
  int _converted = 0;

  if (!ustr)
    return 0;

  while ((_converted < ulen) && (count < size))
    {
      c = (unsigned char) *ustr;
      UTF8_COMPUTE (c, mask, len);
      if ((len == -1) || (_converted + len > ulen))
	{
	  if (converted)
	    *converted = _converted;
	  return count;
	}

      wc = c & mask;
      for (i = 1; i < len; i++)
	{
	  if ((ustr[i] & 0xC0) != 0x80)
	    {
	      if (converted)
		*converted = _converted;
	      return count;
	    }
	  wc <<= 6;
	  wc |= (ustr[i] & 0x3F);
	}
      *wstr = wc;
      ustr += len;
      wstr++;
      count++;
      _converted += len;
    }
  if (converted)
    *converted = _converted;
  return count;
}


static SQLWCHAR *
strdup_U8toW (SQLCHAR * str)
{
  SQLWCHAR *ret;
  int len;

  if (!str)
    return NULL;

  len = utf8_len (str, SQL_NTS);
  if ((ret = (SQLWCHAR *) malloc ((len + 1) * sizeof (SQLWCHAR))) == NULL)
    return NULL;

  len = utf8towcs (str, ret, len);
  ret[len] = L'\0';

  return ret;
}


SQLCHAR *
dm_SQL_WtoU8 (SQLWCHAR * inStr, int size)
{
  SQLCHAR *outStr = NULL;
  int len;

  if (inStr == NULL)
    return NULL;

  if (size == SQL_NTS)
    {
      outStr = strdup_WtoU8 (inStr);
    }
  else
    {
      len = calc_len_for_utf8 (inStr, size);
      if ((outStr = (SQLCHAR *) malloc (len + 1)) != NULL)
	{
	  len = wcsntoutf8 (inStr, outStr, size, len, NULL);
	  outStr[len] = '\0';
	}
    }

  return outStr;
}


SQLWCHAR *
dm_SQL_U8toW (SQLCHAR * inStr, SQLSMALLINT size)
{
  SQLWCHAR *outStr = NULL;
  int len;

  if (inStr == NULL)
    return NULL;

  if (size == SQL_NTS)
    {
      outStr = strdup_U8toW (inStr);
    }
  else
    {
      len = utf8_len (inStr, size);
      if ((outStr = (SQLWCHAR *) calloc (len + 1, sizeof (SQLWCHAR))) != NULL)
	utf8ntowcs (inStr, outStr, size, len, NULL);
    }

  return outStr;
}


int
dm_StrCopyOut2_U8toW (
  SQLCHAR	* inStr,
  SQLWCHAR	* outStr,
  SQLSMALLINT	  size,
  SQLSMALLINT	* result)
{
  int length;

  if (!inStr)
    return -1;

  length = utf8_len (inStr, SQL_NTS);

  if (result)
    *result = length;

  if (!outStr)
    return 0;

  if (size >= length + 1)
    {
      length = utf8towcs (inStr, outStr, size);
      outStr[length] = L'\0';
      return 0;
    }
  if (size > 0)
    {
      length = utf8towcs (inStr, outStr, size - 1);
      outStr[length] = L'\0';
    }
  return -1;
}