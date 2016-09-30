//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/ADLER32.CPv   1.0   12 Mar 1998 12:14:58   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/ADLER32.CPv  $
// 
//    Rev 1.0   12 Mar 1998 12:14:58   DCHI
// Initial revision.
// 
//************************************************************************

//
// ADLER32.CPP
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

#include "arclib.h"
#if !defined( AL_IBM )
#pragma hdrstop
#endif
#if defined( AL_BORLAND )
#pragma option -w-
#endif

/* adler32.c -- compute the Adler-32 checksum of a data stream
 * Copyright (C) 1995 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h 
 */

/* $Id: adler32.c,v 1.6 1995/05/03 17:27:08 jloup Exp $ */

#include "zutil.h"

#define BASE 65521 /* largest prime smaller than 65536 */
#define NMAX 5552
/* NMAX is the largest n such that 255n(n+1)/2 + (n+1)(BASE-1) <= 2^32-1 */

#define DO1(buf)  {s1 += *buf++; s2 += s1;}
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);
#define DO16(buf) DO8(buf); DO8(buf);

/* ========================================================================= */
uLong adler32(
    uLong adler,
    Byte ZL_FAR *buf,
    uInt len )
{
    unsigned long s1 = adler & 0xffff;
    unsigned long s2 = (adler >> 16) & 0xffff;
    int k;

    if (buf == Z_NULL) return 1L;

    while (len > 0) {
        k = len < NMAX ? len : NMAX;
        len -= k;
        while (k >= 16) {
            DO16(buf);
            k -= 16;
        }
        if (k != 0) do {
            DO1(buf);
        } while (--k);
        s1 %= BASE;
        s2 %= BASE;
    }
    return (s2 << 16) | s1;
}
