// Dec2TAR.h : Decomposer 2 Engine for TAR files
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2001, 2005 by Symantec Corporation.  All rights reserved.

#if !defined(DEC2TAR_H)
#define DEC2TAR_H

#if defined(_WINDOWS)
#pragma pack(push, 1)
#elif defined(IBMCXX)
#pragma option align=packed
#else
#pragma pack(1)
#endif

/*
 * Standard Archive Format - Standard TAR - USTAR
 */
#define	RECORDSIZE	512
#define	NAMSIZ		100
#define	TUNMLEN	 32
#define	TGNMLEN	 32

typedef struct tagTARHEADER
{
	char	name[NAMSIZ];
	char	mode[8];
	char	uid[8];
	char	gid[8];
	char	size[12];
	char	mtime[12];
	char	chksum[8];
	char	linkflag;
	char	linkname[NAMSIZ];
	char	magic[8];
	char	uname[TUNMLEN];
	char	gname[TGNMLEN];
	char	devmajor[8];
	char	devminor[8];
	char	filler[RECORDSIZE-NAMSIZ-8-8-8-12-12-8-1-NAMSIZ-8-TUNMLEN-TGNMLEN-8-8];
#if defined(__GNUC__)
} __attribute__ ((packed)) TARHEADER, *PTARHEADER;
#else
} TARHEADER, *PTARHEADER;
#endif

#if defined(_WINDOWS)
#pragma pack(pop)
#elif defined(IBMCXX)
#pragma option align=reset
#else 
#pragma pack()
#endif

/* The checksum field is filled with this while the checksum is computed. */
#define	  CHKBLANKS	"        "        /* 8 blanks, no null */

/* The magic field is filled with this if uname and gname are valid. */
#define	  TMAGIC	"\x75\x73\x74\x61\x72\x20\x20"    /* 7 chars and a null, string equivalent to "ustar  " */
#define	  TMAGIC_NO_TRAILING_SPACES	"\x75\x73\x74\x61\x72"    /* 5 chars and a null, string equivalent to "ustar" */

/* The magic field is filled with this if this is a GNU format dump entry */
#define	  GNUMAGIC	"\x47\x4e\x55\x74\x61\x72\x20"    /* 7 chars and a null, string equivalent to "GNUtar " */

/* The linkflag defines the type of file */
#define	LF_OLDNORMAL '\x00'           /* Normal disk file, Unix compatible */
#define	LF_NORMAL	 '\x30'           /* Normal disk file */
#define	LF_LINK	 '\x31'           /* Link to previously dumped file */
#define	LF_SYMLINK	 '\x32'           /* Symbolic link */
#define	LF_CHR		 '\x33'           /* Character special file */
#define	LF_BLK		 '\x34'           /* Block special file */
#define	LF_DIR		 '\x35'           /* Directory */
#define	LF_FIFO	 '\x36'           /* FIFO special file */
#define	LF_CONTIG	 '\x37'           /* Contiguous file */

/* Further link types may be defined later. */

/* Bits used in the mode field - values in octal */
#define	TSUID	 04000		  /* Set UID on execution */
#define	TSGID	 02000		  /* Set GID on execution */
#define	TSVTX	 01000		  /* Save text (sticky bit) */

/* File permissions */
#define	TUREAD	 00400		  /* read by owner */
#define	TUWRITE  00200		  /* write by owner */
#define	TUEXEC	 00100		  /* execute/search by owner */
#define	TGREAD	 00040		  /* read by group */
#define	TGWRITE  00020		  /* write by group */
#define	TGEXEC	 00010		  /* execute/search by group */
#define	TOREAD	 00004		  /* read by other */
#define	TOWRITE  00002		  /* write by other */
#define	TOEXEC	 00001		  /* execute/search by other */


/////////////////////////////////////////////////////////////////////////////
// Global Constants

/////////////////////////////////////////////////////////////////////////////
// Global Functions
#if defined(_WINDOWS)
  #ifdef DEC2TAR_CPP
	#define DECTARLINKAGE __declspec(dllexport) 
  #else
	#define DECTARLINKAGE __declspec(dllimport) 
  #endif
#else
  #define DECTARLINKAGE EXTERN_C
#endif

DECTARLINKAGE DECRESULT DecNewTAREngine(IDecEngine** ppEngine);
DECTARLINKAGE DECRESULT DecDeleteTAREngine(IDecEngine* pEngine);

#endif	// DEC2TAR_H
