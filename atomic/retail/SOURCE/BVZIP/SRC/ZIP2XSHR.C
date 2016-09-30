/**

    ZZZZZ   IIIII   PPPP     222    X	X    SSSS   H	H   RRRR	CCCC
       Z      I     P	P   2	2    X X    S	    H	H   R	R      C
      Z       I     PPPP       2      X      SSS    HHHHH   RRRR       C
     Z	      I     P	      2      X X	S   H	H   R	R      C
    ZZZZZ   IIIII   P	    22222   X	X   SSSS    H	H   R	R   *	CCCC

    Description: ZIP 2.x shared functions and data

    Product: Central Point 'PC TOOLS'

    CONFIDENTIAL and PROPRIETARY
    (c) Copyright 94, Quintessence Corporation
    (c) Copyright 94, Central Point Software
    All rights reserved.

    Revision History:

    Version	Date	    Author	Comments
    -------	--------    ------	--------------------------------
    2.75	06-21-94    SJP 	created this file
    2.76	10-16-94    SJP 	32 bit FLAT model support

**/

// Common includes for Pre-compiled Header
// This should go before any other includes or definitions
#ifdef SYM_BUILD_SM
#include "fsv_pch.h"
#else
#include "..\inc\fsv_pch.h"
#endif
#pragma hdrstop
// Put all other includes after this

// Private include files
#include "pzip2x.h"

/****************************************
* shared data				*
*****************************************/

/*
* table of extra bits needed for lengths
*/
GLOBAL struct CODETABLE lenExtraTable[] = {
    {3, 0}, {4, 0}, {5, 0},
    {6, 0}, {7, 0}, {8, 0}, {9, 0},
    {10, 0}, {11, 1}, {13, 1}, {15, 1},
    {17, 1}, {19, 2}, {23, 2}, {27, 2},
    {31, 2}, {35, 3}, {43, 3}, {51, 3},
    {59, 3}, {67, 4}, {83, 4}, {99, 4},
    {115, 4}, {131, 5}, {163, 5}, {195, 5},
    {227, 5}, {258, 0},
};

/*
* table of extra bits needed for distances
*/
GLOBAL struct CODETABLE disExtraTable[] = {
    {1, 0}, {2, 0}, {3, 0}, {4, 0},
    {5, 1}, {7, 1}, {9, 2}, {13, 2},
    {17, 3}, {25, 3}, {33, 4}, {49, 4},
    {65, 5}, {97, 5}, {129, 6}, {193, 6},
    {257, 7}, {385, 7}, {513, 8}, {769, 8},
    {1025, 9}, {1537, 9}, {2049, 10}, {3073, 10},
    {4097, 11}, {6145, 11}, {8193, 12}, {12289, 12},
    {16385, 13}, {24577, 13},
};

/*
* table of bit length code values
*/
GLOBAL UBYTE bitLenValues[] = {
    16, 17, 18, 0, 8, 7,
    9, 6, 10, 5, 11, 4,
    12, 3, 13, 2, 14, 1,
    15
};

#pragma warning(disable:4035)
#pragma optimize("egl", off)

/**
*
* name	    ZipMemAllocFar - allocate FAR memory
*
* synopsis  memPtr = ZipMemAllocFar(size)
*	    ULONG size; 		    allocation size
*
* purpose   Allocates FAR memory for buffers and/or tables
*
* returns   memPtr = pointer to allocated memory or NULL if error
*
**/

VOID FAR * FAR ZipMemAllocFar(size)
ULONG size;
{

#if WIN

#if WIN > 1
#define GMEM_FLAGS (GMEM_MOVEABLE | GMEM_SHARE)
#else
#define GMEM_FLAGS (GMEM_MOVEABLE)
#endif

    VOID FAR *memPtr;
    HGLOBAL hGlobal;

    /*
    * allocate global memory
    */
    memPtr = NULL;
    if ((hGlobal = GlobalAlloc(GMEM_FLAGS, size)) != (HGLOBAL)0)
	memPtr = GlobalLock(hGlobal);

    return (memPtr);

#else

#if FLAT32

    return (malloc(size));

#else

    USHORT paras;

    paras = (USHORT)((size + 15) >> 4);

    _asm
    {
	mov	bx,[paras]		; paragraphs needed
	mov	ah,48h
	int	21h			; allocate memory
	mov	dx,ax			; dx = segment
	mov	ax,0			; dx:ax = memory location
	jnc	ok			; allocation seccessful
	mov	dx,ax			; dx:ax = NULL
    ok:
    }

#endif

#endif

}

/**
*
* name	    ZipMemFreeFar - free FAR memory
*
* synopsis  ZipMemFreeFar(memPtr)
*	    VOID FAR *memPtr;		    allocation size
*
* purpose   Frees previously allocated memory
*
**/

VOID FAR ZipMemFreeFar(memPtr)
VOID FAR *memPtr;
{

#if WIN

    HGLOBAL hGlobal;

    hGlobal = (HGLOBAL)LOWORD(GlobalHandle(SELECTOROF(memPtr)));

    /* unlock/free global memory */
    GlobalUnlock(hGlobal);
    GlobalFree(hGlobal);

#else

#if FLAT32

    free(memPtr);

#else

    _asm
    {
	mov	es,word ptr [memPtr+2]
	mov	ah,49h
	int	21h
    }

#endif

#endif

}

#pragma optimize("", on)

/**
*
* name	    ZipMemAllocNear - allocate NEAR memory
*
* synopsis  memPtr = ZipMemAllocNear(size)
*	    UINT size;			    allocation size
*
* purpose   Allocates NEAR memory for buffers and/or tables
*
* returns   memPtr = pointer to allocated memory or NULL if error
*
**/

VOID NEAR * FAR ZipMemAllocNear(size)
UINT size;
{

#if FLAT32
    return (malloc(size));
#else
    return (_nmalloc(size));
#endif

}

/**
*
* name	    ZipMemFreeNear - free NEAR memory
*
* synopsis  ZipMemFreeNear(memPtr)
*	    VOID NEAR *memPtr;		    memory to free
*
* purpose   Frees previously allocated memory
*
**/

VOID FAR ZipMemFreeNear(memPtr)
VOID NEAR *memPtr;
{

#if FLAT32
    free(memPtr);
#else
    _nfree(memPtr);
#endif

}

