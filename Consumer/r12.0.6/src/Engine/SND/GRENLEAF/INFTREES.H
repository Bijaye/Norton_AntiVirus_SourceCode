//************************************************************************
//
// $Header:   S:/GRENLEAF/VCS/INFTREES.H_v   1.0   12 Mar 1998 12:12:34   DCHI  $
//
// Description:
//  Greenleaf ArchiveLib 2.0
//
//************************************************************************
// $Log:   S:/GRENLEAF/VCS/INFTREES.H_v  $
// 
//    Rev 1.0   12 Mar 1998 12:12:34   DCHI
// Initial revision.
// 
//************************************************************************

//
// INFTREES.H
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

/* inftrees.h -- header to use inftrees.c
 * Copyright (C) 1995 Mark Adler
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* WARNING: this file should *not* be used by applications. It is
   part of the implementation of the compression library and is
   subject to change. Applications should only use zlib.h.
 */

/* Huffman code lookup table entry--this entry is four bytes for machines
   that have 16-bit pointers (e.g. PC's in the small or medium model). */

typedef struct inflate_huft_s inflate_huft;
struct inflate_huft_s {
  union {
    struct {
      Byte Exop;        /* number of extra bits or operation */
      Byte Bits;        /* number of bits in this code or subcode */
    } what;
    Byte ZL_FAR *pad;          /* pad structure to a power of 2 (4 bytes for */
  } word;               /*  16-bit, 8 bytes for 32-bit machines) */
  union {
    uInt Base;          /* literal, length base, or distance base */
    inflate_huft ZL_FAR *Next; /* pointer to next level of table */
  } more;
};

#ifdef DEBUG
  extern uInt inflate_hufts;
#endif

extern int inflate_trees_bits __P((
    uInt ZL_FAR *,                     /* 19 code lengths */
    uInt ZL_FAR *,                     /* bits tree desired/actual depth */
    inflate_huft ZL_FAR * ZL_FAR *,            /* bits tree result */
    z_stream *));               /* for zalloc, zfree functions */

extern int inflate_trees_dynamic __P((
    uInt,                       /* number of literal/length codes */
    uInt,                       /* number of distance codes */
    uInt ZL_FAR *,                     /* that many (total) code lengths */
    uInt ZL_FAR *,                     /* literal desired/actual bit depth */
    uInt ZL_FAR *,                     /* distance desired/actual bit depth */
    inflate_huft ZL_FAR * ZL_FAR *,            /* literal/length tree result */
    inflate_huft ZL_FAR * ZL_FAR *,            /* distance tree result */
    z_stream *));               /* for zalloc, zfree functions */

extern int inflate_trees_fixed __P((
    uInt ZL_FAR *,                     /* literal desired/actual bit depth */
    uInt ZL_FAR *,                     /* distance desired/actual bit depth */
    inflate_huft ZL_FAR * ZL_FAR *,            /* literal/length tree result */
    inflate_huft ZL_FAR * ZL_FAR *));          /* distance tree result */

extern int inflate_trees_free __P((
    inflate_huft ZL_FAR *,             /* tables to free */
    z_stream *));               /* for zfree function */
