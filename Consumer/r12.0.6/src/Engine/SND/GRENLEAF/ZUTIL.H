//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ZUTIL.H_v   1.0   12 Mar 1998 12:13:34   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ZUTIL.H_v  $
// 
//    Rev 1.0   12 Mar 1998 12:13:34   DCHI
// Initial revision.
// 
//************************************************************************

//
// ZUTIL.H
//
//  Source file for ArchiveLib 2.0
//
//  No Copyright claimed by Greenleaf Software!
//
// DESCRIPTION
//
//  This is one of the ZLIB source files, with as few changes as possible.
//
// REVISION HISTORY
//
//   February 14, 1996  2.0A : New release
/* zutil.h -- internal interface and configuration of the compression library
 * Copyright (C) 1995 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* $Id: zutil.h,v 1.9 1995/05/03 17:27:12 jloup Exp $ */

#ifndef _Z_UTIL_H
#define _Z_UTIL_H

#if !defined( AL_FLAT_MODEL )
#define ZL_FAR _far
#else
#define ZL_FAR
#endif

#include "zlib.h"

/* #ifndef __GNUC__   disable inline for now */
#  define inline
/* #endif */

/*
 * #ifdef MSDOS
 * #   include <stddef.h>
 * #   include <errno.h>
 * #else
 *    extern int errno;
 *
 * #endif
 */
#include <errno.h>
#include <string.h>

#ifndef local
#  define local static
#endif
/* compile with -Dlocal if your debugger can't find static symbols */

typedef unsigned char  uch;
typedef unsigned short ush;
typedef unsigned long  ulg;

//
// Under msc, I get a warning if it doesn't know the length of this
// array.  Under Watcom, I get an error if I do know the length!
//
#if defined( AL_MICROSOFT )
extern char *z_errmsg[8]; /* indexed by 1-zlib_error */
#else
extern char *z_errmsg[8]; /* indexed by 1-zlib_error */
#endif

#define ERR_RETURN(strm,err) return (strm->msg=z_errmsg[1-err], err)
/* To be used only when the state is known to be valid */

        /* common constants */

#define DEFLATED   8

#define DEF_WBITS 15
/* default windowBits for decompression. MAX_WBITS is for compression only */

#if MAX_MEM_LEVEL >= 8
#  define DEF_MEM_LEVEL 8
#else
#  define DEF_MEM_LEVEL  MAX_MEM_LEVEL
#endif
/* default memLevel */

#define STORED_BLOCK 0
#define STATIC_TREES 1
#define DYN_TREES    2
/* The three kinds of block type */

#define MIN_MATCH  3
#define MAX_MATCH  258
/* The minimum and maximum match lengths */

        /* target dependencies */

//#if defined( AL_WINDOWS ) || defined( MSDOS )
#  ifdef __TURBOC__
#    include <alloc.h>
#  else /* MSC */
#    include <malloc.h>
#  endif
//#endif

#ifndef FOPEN
#  define FOPEN(name, mode) fopen((name), (mode))
#endif

         /* functions */

#ifdef HAVE_STRERROR
   extern char *strerror __P((int));
#  define zstrerror(errnum) strerror(errnum)
#else
#  define zstrerror(errnum) ""
#endif

#if defined(pyr) && !defined(NO_MEMCPY)
#  define NO_MEMCPY
#endif
#if defined(STDC) && !defined(HAVE_MEMCPY) && !defined(NO_MEMCPY)
#  define HAVE_MEMCPY
#endif
#ifdef HAVE_MEMCPY
#  if defined( AL_FLAT_MODEL )
#    define zmemcpy memcpy
#    define zmemzero(dest, len) memset(dest, 0, len)
#  else
#    define zmemcpy _fmemcpy
#    define zmemzero(dest, len) _fmemset(dest, 0, len)
#  endif
#else
   extern void zmemcpy  __P((Byte* dest, Byte* source, uInt len));
   extern void zmemzero __P((Byte* dest, uInt len));
#endif

/* Diagnostic functions */
#ifdef DEBUG
#  include <stdio.h>
#  ifndef verbose
#    define verbose 0
#  endif
#  define Assert(cond,msg) {if(!(cond)) z_error(msg);}
#  define Trace(x) fprintf x
#  define Tracev(x) {if (verbose) fprintf x ;}
#  define Tracevv(x) {if (verbose>1) fprintf x ;}
#  define Tracec(c,x) {if (verbose && (c)) fprintf x ;}
#  define Tracecv(c,x) {if (verbose>1 && (c)) fprintf x ;}
#else
#  define Assert(cond,msg)
#  define Trace(x)
#  define Tracev(x)
#  define Tracevv(x)
#  define Tracec(c,x)
#  define Tracecv(c,x)
#endif


typedef uLong (*check_func) __P((uLong check, Byte ZL_FAR *buf, uInt len));

extern void z_error    __P((char *m));

voidp zcalloc __P((voidp opaque, unsigned items, unsigned size));
void  zcfree  __P((voidp opaque, voidp ptr));

#define ZALLOC(strm, items, size) \
           (*((strm)->zalloc))((strm)->opaque, (items), (size))
#define ZFREE(strm, addr)  (*((strm)->zfree))((strm)->opaque, (voidp)(addr))
#define TRY_FREE(s, p) {if (p) ZFREE(s, p);}

#endif /* _Z_UTIL_H */
