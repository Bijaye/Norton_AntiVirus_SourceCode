/**

    PPPP    ZZZZZ   IIIII   PPPP     222    X   X      H   H
    P P      Z      I     P   P   2 2    X X       H   H
    PPPP      Z       I     PPPP       2      X        HHHHH
    P      Z         I     P        2      X X       H   H
    P     ZZZZZ   IIIII   P       22222   X  X   *  H   H

    Description: Header file for ZIP 2.x compression library (private)

    Product: Central Point 'PC TOOLS'

    CONFIDENTIAL and PROPRIETARY
    (c) Copyright 1992,93,94 Quintessence Corporation
    (c) Copyright 1993,94 Central Point Software
    All rights reserved.

    Revision History:

    Version Date      Author  Comments
    ------- --------    ------   --------------------------------
    2.50 07-05-93    SJP   created this file
    2.75 06-06-94    SJP   split into public/private
    2.76 10-16-94    SJP   32 bit FLAT model support

**/

#ifndef PZIP2X_H
#define PZIP2X_H

/* standard includes */
#include <limits.h>

/****************************************
 * flat/segmented dependencies      *
 ****************************************/

#if _M_IX86

#define FLAT32 1
#define UINT_BITS 32

#define __near /**/
#define __far /**/
#define __huge /**/
#define __pascal /**/

#define SEGDATA /**/
#define SEGTEXT /**/
#define SEGSTACK /**/

#define FMEMCPY memcpy
#define FMEMSET memset

#else

#define FLAT32 0
#define UINT_BITS 16

/* no base conversion warnings */
#pragma warning(disable:4120)

#define SEGDATA static
#define SEGTEXT __based(__segname("_CODE"))
#define SEGSTACK __based(__segname("_STACK"))

#define FMEMCPY _fmemcpy
#define FMEMSET _fmemset

#endif

/****************************************
 * special aliases         *
 ****************************************/

#ifndef LOCAL
   #define LOCAL static __fastcall __near
#endif
   
#define PLOCAL static * __fastcall __near
#define FPLOCAL static __far * __fastcall __near
#define GLOBAL /**/
#define IMPORT extern
#define STATIC static
#define REGISTER register
#define CONST const
#define VOLATILE volatile
#define INLINE __inline

#ifndef PASCAL
   #define PASCAL __pascal
#endif

#ifndef CDECL
   #define CDECL __cdecl
#endif

/****************************************
 * data type aliases       *
 ****************************************/

/* type aliases */
#ifndef FAR
   #define FAR __far
#endif

#ifndef NEAR   
   #define NEAR __near
#endif

#define VOID void
#define LONG long

/* type combined */
typedef int BOOL;

/* type aliases */
#define HFAR __huge
#define SHORT short
#define CHAR char
#define INT int

/* type combined */
typedef unsigned char UBYTE;
typedef unsigned short USHORT;
typedef unsigned long ULONG;
typedef unsigned char UCHAR;
typedef unsigned int UINT;

/****************************************
 * definitions/enumerations      *
 ****************************************/

/*
* the infamous NULL
*/
#ifndef NULL
#define NULL ((void *)0)
#endif

/*
* the infamous TRUE/FALSE
*/
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* number of bits in a byte */
#define UBYTE_BITS 8

/*
* deflate constants
*/
#define DF_MAXBITS 15         /* maximum bit length of all codes */
#define DF_LITBITS 15         /* maximum of literal codes */
#define DF_DISBITS 15         /* maximum of distance codes */
#define DF_MAXCODES 288       /* maximum # of all codes */
#define DF_LITCODES 288       /* maximum # of literal codes */
#define DF_LITCODESUSED (DF_LITCODES-2)
#define DF_DISCODES 32        /* maximum # of distance codes */
#define DF_DISCODESUSED (DF_DISCODES-2)
#define DF_BITBITS 7
#define DF_BITCODES 19        /* maximum # of bit length codes */
#define DF_ENDBLOCK 256       /* end of block marker for deflate */
#define DF_MAXLENCODE 285     /* code indicating maximum length */
#define DF_MAXLENGTH 258      /* maximum copy length */
#define DF_STORED 0        /* block is stored */
#define DF_FIXED 1         /* block uses fixed codes */
#define DF_DYNAMIC 2       /* block uses dynamic codes */
#define DF_RESERVED 3         /* reserved - Error */

/****************************************
* miscellaneous data structures  *
*****************************************/

/*
* bit I/O state information
*/
struct BITINFO {
    UBYTE FAR *buffer;        /* buffer address */
    UBYTE FAR *bufPtr;        /* bit buffer */
    UBYTE FAR *bufEnd;        /* end bit buffer */
    UINT last;          /* last word shifted down */
    UBYTE bitCnt;       /* bit offset in bit buffer */
    UBYTE eof;          /* EOF flag */
};

/*
* buffer I/O state information
*/
struct BUFINFO {
    UBYTE FAR *buffer;        /* buffer address */
    UBYTE FAR *bufPtr;        /* pos within buffer */
    UBYTE FAR *bufEnd;        /* end of buffer */
    UBYTE eof;          /* EOF flag */
};

/*
* Shannon-Fano codes with Huffman computed lengths
* and lookback distances/lengths extra bits
*/
struct CODETABLE {
    USHORT code;        /* S-F code or dis/len */
    UBYTE bits;         /* code size or extra len */
};

/*
* RLE compressed Shannon-Fano CODETABLE
*/
#pragma warning(disable:4103)
#pragma pack(1)
struct PACKTABLE {
    union {
    struct {
   UBYTE bits;       /* code size */
   UBYTE extraBits;     /* extra bits for run-len */
   UBYTE extraLen;      /* run-length */
    };
    struct {
   USHORT bitsExtraBits;      /* word bits/extraBits */
    };
    struct {
   UBYTE pad1;
   USHORT extraBitsLen;    /* word extraBits/extraLen */
    };
    };
};
#pragma pack()

/*
* Shannon-Fano tree descriptor
*/
struct SFTD {
    struct SFDH {
   USHORT codeLenCounts[DF_MAXBITS];
    };
    UBYTE codeLens[0];
};

/*
* Shannon-Fano table/tree
*/
struct SFTT {
    struct SFTH {
   USHORT vals[(1<<UBYTE_BITS)];
   UBYTE lens[(1<<UBYTE_BITS)];
    };
    USHORT tree[0];
};

/****************************************
* miscellaneous temporary data      *
*****************************************/

/*
* deflate
*/
struct DEFLATE {

    /* table of S-F codes */
    struct CODETABLE dlTable[DF_LITCODESUSED+DF_DISCODESUSED+DF_BITCODES];
    struct PACKTABLE pkTable[DF_LITCODESUSED+DF_DISCODESUSED];

    /* length/literal probability counts */
    SHORT lenProbCount[DF_LITCODESUSED];

    /* distance probability counts */
    SHORT disProbCount[DF_DISCODESUSED];

    /*
    * temporary tree construction
    */
    struct DTMPTREE {
   UBYTE depth[DF_MAXCODES*2];
   SHORT count[DF_MAXCODES*2];
   SHORT heap[DF_MAXCODES*2];
   SHORT dad[DF_MAXCODES*2];
    } tmpTree;
};


/* inflate
*/
struct INFLATE {

    /* S-F literal tree */
    struct {
   struct SFTH;
   USHORT tree[DF_LITCODES * 2];
    } litTree;

    /* S-F distance tree */
    struct {
   struct SFTH;
   USHORT tree[DF_DISCODES * 2];
    } disTree;

    /*
    * temporary tree construction
    */
    struct ITMPTREE {
   /* temporary S-F bit lengths tree */
   struct {
       struct SFTH;
   } bitTree;

   /* temporary bit/literal tree descriptors */
   union {
   struct {
       struct SFDH;
       UBYTE codeLens[DF_BITCODES];
   } bitTemp;
   struct {
       struct SFDH;
       UBYTE codeLens[DF_LITCODES];
   } litTemp;
   };

   /* temporary distance tree descriptor */
   struct {
       struct SFDH;
       UBYTE codeLens[DF_DISCODES];
   } disTemp;
    } tmpTree;

    /*
    * S-F temporary tree construction
    */
    struct TMPSFT {
   USHORT tempCodes[DF_MAXCODES];
    } tmpSFT;
};

/****************************************
* function declarations       *
*****************************************/

/* ZIP2XCOM.C */

USHORT FAR PASCAL Zip2x_CompressWorkSize(void);
BOOL FAR PASCAL Zip2x_CompressInit(VOID FAR *, LONG, INT);
VOID FAR PASCAL Zip2x_CompressFree(VOID FAR *);
BOOL FAR PASCAL Zip2x_Compress(UBYTE FAR *, unsigned short FAR *, UBYTE FAR *, unsigned short FAR *, VOID FAR *, char);
LONG LOCAL StackCodes(struct ZIP2XCOMPINFO SEGSTACK *, SHORT, SHORT);
LONG LOCAL UnStackCodes(struct ZIP2XCOMPINFO SEGSTACK *, SHORT);
SHORT LOCAL PackLengths(struct PACKTABLE *, struct CODETABLE *, SHORT);
SHORT LOCAL CalcHeadSize(struct PACKTABLE *, struct CODETABLE *, SHORT, SHORT);
LONG LOCAL CalcBlockSize(struct CODETABLE FAR *, struct CODETABLE FAR *, struct CODESTACK FAR *, SHORT);
LONG INLINE LOCAL ComputeVar(SHORT, SHORT, LONG);

/* ZIP2XCOD.C */

VOID BuildHuffCodes(struct CODETABLE *, SHORT *, struct DTMPTREE *, SHORT, SHORT);
VOID LOCAL MoveDownHeap(struct DTMPTREE *, SHORT, SHORT);
VOID LOCAL BuildSFCodes(SHORT SEGSTACK *, struct CODETABLE *, SHORT);
SHORT INLINE LOCAL ComputeScale(SHORT, SHORT, SHORT);

/* ZIP2XEXP.C */

USHORT FAR PASCAL Zip2x_ExpandWorkSize(void);
BOOL FAR PASCAL Zip2x_ExpandInit(VOID FAR *);
VOID FAR PASCAL Zip2x_ExpandFree(VOID FAR *);
BOOL FAR PASCAL Zip2x_Expand(UBYTE FAR *, unsigned short FAR *, UBYTE FAR *, unsigned short FAR *, VOID FAR *, char);
SHORT LOCAL BuildDynamicSFTrees(struct ZIP2XEXPINFO SEGSTACK *);
VOID LOCAL BuildFixedSFTrees(struct ZIP2XEXPINFO SEGSTACK *);
BOOL LOCAL BuildSFTree(struct SFTD *, struct SFTT *, struct TMPSFT *, SHORT);
SHORT LOCAL Decode(struct BITINFO SEGSTACK *, USHORT *, USHORT *);

/* ZIP2XIO.C */

VOID BitReadInit(struct BITINFO FAR *);
UINT BitReadSetup(struct BITINFO SEGSTACK *, UBYTE FAR *, UINT, BOOL);
VOID BitSkip(struct BITINFO SEGSTACK *, UBYTE);
BOOL BitRead(struct BITINFO SEGSTACK *, UINT *, UBYTE);
BOOL BitReadBytes(struct BITINFO SEGSTACK *, UBYTE FAR *, INT);
VOID BitWriteInit(struct BITINFO FAR *);
UINT BitWriteSetup(struct BITINFO SEGSTACK *, UBYTE FAR *, UINT);
BOOL BitWrite(struct BITINFO SEGSTACK *, UINT *, UBYTE);
BOOL BitWriteBytes(struct BITINFO SEGSTACK *, UBYTE FAR *, INT);
VOID BufReadInit(struct BUFINFO FAR *);
UINT BufReadSetup(struct BUFINFO SEGSTACK *, UBYTE FAR *, UINT, BOOL);
VOID BufWriteInit(struct BUFINFO FAR *);
UINT BufWriteSetup(struct BUFINFO SEGSTACK *, UBYTE FAR *, UINT);
BOOL BufWriteStr(struct BUFINFO SEGSTACK *, UBYTE FAR *, INT);

/* ZIP2XSHR.C */

VOID FAR * FAR ZipMemAllocFar(ULONG);
VOID FAR ZipMemFreeFar(VOID FAR *);
VOID NEAR * FAR ZipMemAllocNear(UINT);
VOID FAR ZipMemFreeNear(VOID NEAR *);

/* ZIP2X.I */

BOOL INLINE BitPeekQuick8(struct BITINFO SEGSTACK *, UINT *);
BOOL INLINE BitPeekQuick7(struct BITINFO SEGSTACK *, UINT *);
VOID INLINE BitSkipQuick(struct BITINFO SEGSTACK *, UBYTE);
BOOL INLINE BitReadQuick(struct BITINFO SEGSTACK *, UINT *, UBYTE);
BOOL INLINE BitWriteQuick(struct BITINFO SEGSTACK *, UINT *, UBYTE);
BOOL INLINE BufReadQuick(struct BUFINFO SEGSTACK *, UBYTE FAR *);
BOOL INLINE BufReadStrQuick(struct BUFINFO SEGSTACK *, UBYTE FAR *, INT, INT *);
BOOL INLINE BufWriteQuick(struct BUFINFO SEGSTACK *, UBYTE FAR *);
BOOL INLINE BufWriteStrQuick(struct BUFINFO SEGSTACK *, UBYTE FAR *, INT);

#endif

