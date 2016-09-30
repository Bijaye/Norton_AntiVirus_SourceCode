/**

    ZZZZZ   IIIII   PPPP     222    X	X    CCCC    OOO    M	M	CCCC
       Z      I     P	P   2	2    X X    C	    O	O   MM MM      C
      Z       I     PPPP       2      X     C	    O	O   M M M      C
     Z	      I     P	      2      X X    C	    O	O   M	M      C
    ZZZZZ   IIIII   P	    22222   X	X    CCCC    OOO    M	M   *	CCCC

    Description: ZIP 2.x compression routines (deflate)

    Product: Central Point 'PC TOOLS'

    CONFIDENTIAL and PROPRIETARY
    (c) Copyright 1992,93,94 Quintessence Corporation
    (c) Copyright 1993,94 Central Point Software
    All rights reserved.

    Revision History:

    Version	Date	    Author	Comments
    -------	--------    ------	--------------------------------
    2.20	04-21-93    SJP 	original code for QUINZIP DLL
    2.50	07-12-93    SJP 	API changed to CPS specifications
    2.75	06-06-94    SJP 	lookback scanning faster, RLE
					packing improved, fixed tables
					moved out of DGROUP, better
					hash function, improved state
					switching
    2.76	10-16-94    SJP 	32 bit FLAT model support

**/

/****************************************
 * includes				*
 ****************************************/

// Common includes for Pre-compiled Header
// This should go before any other includes or definitions
#ifdef SYM_BUILD_SM
#include "fsv_pch.h"
#else
#include "..\inc\fsv_pch.h"
#endif
#pragma hdrstop
// Put all other includes after this

//#define DEBUG 0
//#if DEBUG
//#include <conio.h>
//#endif

/* Private include files. */
#include "pzip2x.h"

/****************************************
 * definitions/enumerations		*
 ****************************************/

/* local constants */
#define WINDSIZE 32768U
#define WINDMASK (WINDSIZE-1)
#define REPFLAG (1U<<15)
#define SHADOW (DF_MAXLENGTH-1)

/* maximize REP advance
#define FIRSTREAD ((DF_MAXLENGTH-1)+DF_MAXLENGTH)
*/

/* maximize lookback */
#define FIRSTREAD (DF_MAXLENGTH+2)

/* options */
#if FLAT32
#define VARIANCE_C 1
#else
#define VARIANCE_C 0
#endif
#define HASHTDC 1

/****************************************
 * LITE memory macros/definitions	*
 ****************************************/

#if LITE
#pragma message("zip2xcom.c: compiling for lite memory")
#endif

#if LITE

/* hash table size */
#define STACKSIZE 8192
#define HASHSIZE 4096U

/*
* hash function
*/
#if FLAT32

#define HASHFUNC(a,b,c) (((((USHORT)a << 8) | (USHORT)b) ^ (((USHORT)a >> 4) | ((USHORT)c << 4))) & (HASHSIZE-1))

#else

/* parameters as passed generates better code */
#define HASHFUNC(a,b,c) HashFunc(a,b,c)
USHORT INLINE LOCAL HashFunc(UBYTE a, UBYTE b, UBYTE c)
{
    USHORT u = _rotr((USHORT)a, 4);
    return (((u & 255U) ^ b) ^ ((USHORT)((UBYTE)(u >> 8) ^ c) << 4));
}

#endif

#else

/****************************************
 * NORMAL memory macros/definitions	 *
 ****************************************/

/* hash table size */
#define STACKSIZE 15360
#define HASHSIZE 32768U

#if HASHTDC

/* parameters as passed generates better code */
#define HASHFUNC(x,y,z) HashFunc(x,y,z)
USHORT INLINE HashFunc(UBYTE x, UBYTE y, UBYTE z)
{
    UBYTE h, l, b;

    h = l = (UBYTE)-1;

    b = (UBYTE)l << 5 ^ l;
    l = (((UBYTE)b << 4) ^ ((UBYTE)b >> 3)) ^ h;
    h = (((UBYTE)b >> 4) ^ b) ^ x;

    b = (UBYTE)l << 5 ^ l;
    l = (((UBYTE)b << 4) ^ ((UBYTE)b >> 3)) ^ h;
    h = (((UBYTE)b >> 4) ^ b) ^ y;

    b = (UBYTE)l << 5 ^ l;
    l = (((UBYTE)b << 4) ^ ((UBYTE)b >> 3)) ^ h;
    h = (((UBYTE)b >> 4) ^ b) ^ z;

    return ((USHORT)((USHORT)h << 7 ^ l));
}

#else

/*
* hash function
*/
#if FLAT32

#define HASHFUNC(a,b,c) ((USHORT)(((USHORT)c << 7) ^ ((USHORT)b << 4) ^ a))

#else

/* reversing parameters generates better code */
#define HASHFUNC(a,b,c) HashFunc(c,b,a)
USHORT INLINE LOCAL HashFunc(UBYTE c, UBYTE b, UBYTE a)
{
    return ((((USHORT)c << 8) >> 1) ^ ((USHORT)b << 4) ^ a);
}

#endif

#endif

#endif

/*
* compress states
*/
enum {
    COM_DEFAULT,
    COM_RDSTR,
    COM_RDCHR,
    COM_WRHEADER,
    COM_WRLENGTH,
    COM_WRBYTES,
    COM_WRCOUNT,
    COM_WRCODE1,
    COM_WRCODE2,
    COM_WRCODE3,
    COM_WRLIT1,
    COM_WRLIT2,
    COM_WRLIT3,
    COM_WRLEN,
    COM_WRXLEN,
    COM_WROFF,
    COM_WRXOFF,
    COM_WRCHR,
    COM_WREOB,
};

/****************************************
* local data structures 		*
*****************************************/

/*
* current compression state information
*/
struct ZIP2XCOMPINFO {

    /*
    * initialized not changed by states
    */
    struct {
	USHORT FAR *hashTable, FAR *colTable;
	struct CODESTACK FAR *stack;
	struct ALTCODESTACK FAR *altStack;
	UBYTE FAR *lookBackBuf, FAR *endBuf;
	struct DEFLATE *deflate;
    };

    /*
    * changed between states
    */
    struct {
	/* initialized */
	UBYTE FAR *rdPtr, FAR *toPtr;
	LONG fileSize;
	SHORT length, lastLength;
	SHORT bufLength, bufUsed;
	SHORT sSave;
	USHORT greedyMask;
	SHORT depthSave, altDepth;
	SHORT goodLen, greedy;
	SHORT state;

	/* uninitialized */
	struct CODETABLE *dlTable, *ddTable, *blTable;
	LONG lenSqr, disSqr;
	LONG oldSize, blockSize;
	LONG oldDisVar, oldLenVar;

	USHORT curOffset, curCode;
	SHORT lenSum, lenNum;
	SHORT disSum, disNum;
	SHORT pkSize, curLength, curRep;
	SHORT nLitCodes, nDisCodes, nBitCodes;
	SHORT sSize, dStack, lStack;
	SHORT len;
	UBYTE bits1, bits2, bits3;
	UBYTE method;

	UBYTE FAR FAR *curNextPtr;
	UBYTE FAR *oldPtr;

	struct CODESTACK FAR *sPtr;
	struct ALTCODESTACK FAR *altPtr;
	struct CODETABLE *ct, *bt;
	struct CODETABLE *cte, *bte;
	struct PACKTABLE *pk;
	UBYTE FAR *ptr;
	SHORT idx;
    };

    /* bit I/O state information */
    struct BITINFO bi;

    /* buffer I/O state information */
    struct BUFINFO bf;
};

/*
* structures for code stacking
*/
struct CODESTACK {
    USHORT offset;
    UBYTE idxLen;
    UBYTE idxOff;
};
struct ALTCODESTACK {
    UBYTE lit1;
    UBYTE lit2;
    UBYTE lit3;
    UBYTE length;
};

/****************************************
 * external data			*
 ****************************************/

/* extra bits needed for lookback */
IMPORT struct CODETABLE NEAR lenExtraTable[];
IMPORT struct CODETABLE NEAR disExtraTable[];

/* bit length code values */
IMPORT UBYTE NEAR bitLenValues[];

/****************************************
* local data				*
*****************************************/

/*
* table of indexs in lenExtraTable[] for lengths (3 - 258)
*/
STATIC UBYTE lenExtraIndex[] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 9, 9, 10, 10, 11, 11,
    12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15,
    16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17,
    18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19,
    20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
    22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
    23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
    27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28,
};

/*
* table of indexs in disExtraTable[] for distances (1 - 32768)
*/
STATIC UBYTE disExtraIndex[] = {
    /* distance 1 - 256 */
    0, 1, 2, 3, 4, 4, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7,
    8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
    11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
    15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,

    /* distance 257 - 32768 */
    0, 0, 16, 17, 18, 18, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21,
    22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
    27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
    27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
};

/****************************************
* data in code segment			*
*****************************************/

ULONG FAR * FAR PASCAL Zip2x_CodeData(VOID)
{
    STATIC ULONG SEGTEXT codeSeq[1] = {5261907};
    return (codeSeq);

    /*
    * trick to force compiler tp place following
    * code inside this procedure
    */
}

/*
* fixed Shannon-Fano codes for literals/lengths (DF_LITCODESUSED)
* <code, bits>
*/
STATIC struct CODETABLE SEGTEXT flTable[] = {
    /* literals */
    {12, 8}, {140, 8}, {76, 8}, {204, 8},
    {44, 8}, {172, 8}, {108, 8}, {236, 8},
    {28, 8}, {156, 8}, {92, 8}, {220, 8},
    {60, 8}, {188, 8}, {124, 8}, {252, 8},
    {2, 8}, {130, 8}, {66, 8}, {194, 8},
    {34, 8}, {162, 8}, {98, 8}, {226, 8},
    {18, 8}, {146, 8}, {82, 8}, {210, 8},
    {50, 8}, {178, 8}, {114, 8}, {242, 8},
    {10, 8}, {138, 8}, {74, 8}, {202, 8},
    {42, 8}, {170, 8}, {106, 8}, {234, 8},
    {26, 8}, {154, 8}, {90, 8}, {218, 8},
    {58, 8}, {186, 8}, {122, 8}, {250, 8},
    {6, 8}, {134, 8}, {70, 8}, {198, 8},
    {38, 8}, {166, 8}, {102, 8}, {230, 8},
    {22, 8}, {150, 8}, {86, 8}, {214, 8},
    {54, 8}, {182, 8}, {118, 8}, {246, 8},
    {14, 8}, {142, 8}, {78, 8}, {206, 8},
    {46, 8}, {174, 8}, {110, 8}, {238, 8},
    {30, 8}, {158, 8}, {94, 8}, {222, 8},
    {62, 8}, {190, 8}, {126, 8}, {254, 8},
    {1, 8}, {129, 8}, {65, 8}, {193, 8},
    {33, 8}, {161, 8}, {97, 8}, {225, 8},
    {17, 8}, {145, 8}, {81, 8}, {209, 8},
    {49, 8}, {177, 8}, {113, 8}, {241, 8},
    {9, 8}, {137, 8}, {73, 8}, {201, 8},
    {41, 8}, {169, 8}, {105, 8}, {233, 8},
    {25, 8}, {153, 8}, {89, 8}, {217, 8},
    {57, 8}, {185, 8}, {121, 8}, {249, 8},
    {5, 8}, {133, 8}, {69, 8}, {197, 8},
    {37, 8}, {165, 8}, {101, 8}, {229, 8},
    {21, 8}, {149, 8}, {85, 8}, {213, 8},
    {53, 8}, {181, 8}, {117, 8}, {245, 8},
    {13, 8}, {141, 8}, {77, 8}, {205, 8},
    {45, 8}, {173, 8}, {109, 8}, {237, 8},
    {29, 8}, {157, 8}, {93, 8}, {221, 8},
    {61, 8}, {189, 8}, {125, 8}, {253, 8},
    {19, 9}, {275, 9}, {147, 9}, {403, 9},
    {83, 9}, {339, 9}, {211, 9}, {467, 9},
    {51, 9}, {307, 9}, {179, 9}, {435, 9},
    {115, 9}, {371, 9}, {243, 9}, {499, 9},
    {11, 9}, {267, 9}, {139, 9}, {395, 9},
    {75, 9}, {331, 9}, {203, 9}, {459, 9},
    {43, 9}, {299, 9}, {171, 9}, {427, 9},
    {107, 9}, {363, 9}, {235, 9}, {491, 9},
    {27, 9}, {283, 9}, {155, 9}, {411, 9},
    {91, 9}, {347, 9}, {219, 9}, {475, 9},
    {59, 9}, {315, 9}, {187, 9}, {443, 9},
    {123, 9}, {379, 9}, {251, 9}, {507, 9},
    {7, 9}, {263, 9}, {135, 9}, {391, 9},
    {71, 9}, {327, 9}, {199, 9}, {455, 9},
    {39, 9}, {295, 9}, {167, 9}, {423, 9},
    {103, 9}, {359, 9}, {231, 9}, {487, 9},
    {23, 9}, {279, 9}, {151, 9}, {407, 9},
    {87, 9}, {343, 9}, {215, 9}, {471, 9},
    {55, 9}, {311, 9}, {183, 9}, {439, 9},
    {119, 9}, {375, 9}, {247, 9}, {503, 9},
    {15, 9}, {271, 9}, {143, 9}, {399, 9},
    {79, 9}, {335, 9}, {207, 9}, {463, 9},
    {47, 9}, {303, 9}, {175, 9}, {431, 9},
    {111, 9}, {367, 9}, {239, 9}, {495, 9},
    {31, 9}, {287, 9}, {159, 9}, {415, 9},
    {95, 9}, {351, 9}, {223, 9}, {479, 9},
    {63, 9}, {319, 9}, {191, 9}, {447, 9},
    {127, 9}, {383, 9}, {255, 9}, {511, 9},

    /* EOB */
    {0, 7},

    /* lengths */
    {64, 7}, {32, 7}, {96, 7},
    {16, 7}, {80, 7}, {48, 7}, {112, 7},
    {8, 7}, {72, 7}, {40, 7}, {104, 7},
    {24, 7}, {88, 7}, {56, 7}, {120, 7},
    {4, 7}, {68, 7}, {36, 7}, {100, 7},
    {20, 7}, {84, 7}, {52, 7}, {116, 7},
    {3, 8}, {131, 8}, {67, 8}, {195, 8},
    {35, 8}, {163, 8},
};

/*
* fixed Shannon-Fano codes for distances (DF_DISCODESUSED)
* <code, bits>
*/
STATIC struct CODETABLE SEGTEXT fdTable[] = {
    {0, 5}, {16, 5}, {8, 5}, {24, 5},
    {4, 5}, {20, 5}, {12, 5}, {28, 5},
    {2, 5}, {18, 5}, {10, 5}, {26, 5},
    {6, 5}, {22, 5}, {14, 5}, {30, 5},
    {1, 5}, {17, 5}, {9, 5}, {25, 5},
    {5, 5}, {21, 5}, {13, 5}, {29, 5},
    {3, 5}, {19, 5}, {11, 5}, {27, 5},
    {7, 5}, {23, 5},
};

/****************************************
 * inline functions			*
 ****************************************/

#include "zip2x.i"

/**
*
* name	    Zip2x_CompressWorkSize - returns size of compress info structure
*
* synopsis  size = Zip2x_CompressWorkSize()
*
* purpose   Returns the size of the state info structure required by the
*	    ZIP 2x compression function.
*
* returns   size = size of compress info structure
*
**/

USHORT FAR PASCAL Zip2x_CompressWorkSize()
{
    return (sizeof(struct ZIP2XCOMPINFO));
}

/**
*
* name	    Zip2x_CompressInit - initializes compress info structure
*
* synopsis  err = Zip2x_CompressInit(compInfo, fileSize, method)
*	    VOID *compInfo		    compress info structure
*	    LONG fileSize		    size of source file
*	    INT method; 		    compression method
*
* purpose   Initializes structure that contains state info for the ZIP 2x
*	    compression function.
*
*	    Notes:  method can be one of the following:
*
*		    ZIP2X_DEFLATN
*		    ZIP2X_DEFLATX
*		    ZIP2X_DEFLATF
*		    ZIP2X_DEFLATS
*
* returns   err = TRUE if memory allocations fail
*
**/

BOOL FAR PASCAL Zip2x_CompressInit(compInfo, fileSize, method)
VOID FAR *compInfo;
LONG fileSize;
INT method;
{
    /*
    * compression parameters for each method
    */
    STATIC struct {
	SHORT depth;		    /* collision depth */
	SHORT altDepth; 	    /* alternate depth */
	SHORT goodLen;		    /* good length for alternate */
	USHORT greedyMask;	    /* 0 = greedy, ~0 = 2nd level */
    } compParams[] = {
	{32, 8, 32, (USHORT)~0},    /* ZIP2X_DEFLATN */
	{520, 130, 32, (USHORT)~0}, /* ZIP2X_DEFLATX */
	{3, 0, 0, 0},		    /* ZIP2X_DEFLATF */
	{1, 0, 0, 0},		    /* ZIP2X_DEFLATS */
    };

    REGISTER struct ZIP2XCOMPINFO FAR *ci;
    BOOL err;

    /* assume success */
    err = FALSE;

    /* type compress info structure */
    ci = compInfo;

    /* initialize info structure to zeros */
    FMEMSET(ci, 0, sizeof(struct ZIP2XCOMPINFO));

    /*
    * allocate the hash/collision tables, code stack and lookback
    * allocate the literal, distance, and bit length code tables
    */
    if ((ci->hashTable = ZipMemAllocFar((LONG)HASHSIZE * sizeof(USHORT))) == NULL ||
	(ci->colTable = ZipMemAllocFar((LONG)WINDSIZE * sizeof(USHORT))) == NULL ||
	(ci->stack = ZipMemAllocFar(STACKSIZE * sizeof(struct CODESTACK))) == NULL ||
	(ci->altStack = ZipMemAllocFar(STACKSIZE * sizeof(struct ALTCODESTACK))) == NULL ||
	(ci->lookBackBuf = ZipMemAllocFar(WINDSIZE + SHADOW)) == NULL ||
	(ci->deflate = ZipMemAllocNear(sizeof(struct DEFLATE))) == NULL) {

	    Zip2x_CompressFree(ci);
	    err = TRUE;

    /*
    * initializations
    */
    } else {

	/* zero collision/hash tables */
	FMEMSET(ci->hashTable, 0, HASHSIZE);
	FMEMSET((UBYTE FAR *)ci->hashTable + HASHSIZE, 0, HASHSIZE);
	FMEMSET(ci->colTable, 0, WINDSIZE);
	FMEMSET((UBYTE FAR *)ci->colTable + WINDSIZE, 0, WINDSIZE);

	/* initialize end of lookback buffer */
	ci->endBuf = ci->lookBackBuf + WINDSIZE;

	/* initialize lookback pointers */
	ci->rdPtr = ci->toPtr = ci->lookBackBuf;

	/* size of source file */
	ci->fileSize = fileSize;

	/* initial read (see comments) */
	ci->length = FIRSTREAD;

	/* nothing read from file */
	ci->lastLength = -1;

	/* nothing queued
	ci->bufLength = 0;
	*/

	/* nothing used
	ci->bufUsed = 0;
	*/

	/* nothing unstacked
	ci->sSave = 0;
	*/

	/* start at default state
	ci->state = COM_DEFAULT;
	*/

	/* set compression parameters */
	ci->depthSave = compParams[method].depth;
	ci->altDepth = compParams[method].altDepth;
	ci->goodLen = compParams[method].goodLen;
	ci->greedyMask = compParams[method].greedyMask;

	/* initialize bit operations */
	BitWriteInit(&ci->bi);

	/* initialize buffer operations */
	BufReadInit(&ci->bf);
    }

    return (err);
}

/**
*
* name	    Zip2x_CompressFree - free memory allocated at initialization
*
* synopsis  Zip2x_CompressFree(compInfo)
*	    VOID *compInfo		    compress info structure
*
* purpose   Frees memory allocated at initialization
*
**/

VOID FAR PASCAL Zip2x_CompressFree(compInfo)
VOID FAR *compInfo;
{
    REGISTER struct ZIP2XCOMPINFO FAR *ci;

    /* type compress info structure */
    ci = compInfo;

    /*
    * free allocations
    */
    if (ci->deflate != NULL)
	ZipMemFreeNear(ci->deflate);
    if (ci->lookBackBuf != NULL)
	ZipMemFreeFar(ci->lookBackBuf);
    if (ci->altStack != NULL)
	ZipMemFreeFar(ci->altStack);
    if (ci->stack != NULL)
	ZipMemFreeFar(ci->stack);
    if (ci->colTable != NULL)
	ZipMemFreeFar(ci->colTable);
    if (ci->hashTable != NULL)
	ZipMemFreeFar(ci->hashTable);

    /* initialize info structure to zeros */
    FMEMSET(ci, 0, sizeof(struct ZIP2XCOMPINFO));
}

/**
*
* name	    Zip2x_Compress - compress data block using current state
*
* synopsis  done = Zip2x_Compress(inputBuf, inputSize, outputBuf, outputSize, compInfo, eof)
*	    UBYTE *inputBuf		    uncompressed data buffer
*	    UINT *inputSize		    address of input buffer size
*	    UBYTE *outputBuf		    compressed data buffer
*	    UINT *outputSize		    address of output buffer size
*	    VOID *compInfo		    compress info structure
*	    BOOL eof			    last block flag
*
* purpose   Compresses a block of data using the ZIP 2.x 'deflate' method
*	    with current state info structure.
*
*	    Note: Minimum buffer size is FIRSTREAD bytes
*
* returns   done = TRUE if compression complete
*
**/

BOOL FAR PASCAL Zip2x_Compress(inputBuf, inputSize, outputBuf, outputSize, compInfo, eof)
UBYTE FAR *inputBuf;
unsigned short FAR *inputSize;
UBYTE FAR *outputBuf;
unsigned short FAR *outputSize;
VOID FAR *compInfo;
char eof;
{
    UINT SEGDATA tmpBits;
    INT SEGDATA rdLen;

    /* current state information */
    struct ZIP2XCOMPINFO ci;

    LONG dynSize, fixSize;
    LONG disVar, lenVar;
    ULONG lenChk;
    USHORT offset, lastOff, off;
    USHORT code, u, ru, ou, xu;
    USHORT copySize;
    USHORT tmpVal;
    SHORT length;
    SHORT bufLen, n;
    SHORT rep, maxLen;
    SHORT depth, greedy;
    SHORT state;
    BOOL done;

    UBYTE FAR *nextPtr, FAR *repPtr;

    REGISTER UBYTE FAR *tp, FAR *hp;
    REGISTER UBYTE FAR *toPtr, FAR *rdPtr;
    REGISTER struct PACKTABLE *pk;
    REGISTER struct CODETABLE *tt;
    REGISTER UBYTE FAR *p;
    REGISTER SHORT i;

    /* make compress info local */
    ci = *(struct ZIP2XCOMPINFO FAR *)compInfo;

    /* setup buffer operations */
    BufReadSetup(&ci.bf, inputBuf, *inputSize, eof);

    /* setup bit operations */
    copySize = (USHORT)BitWriteSetup(&ci.bi, outputBuf, *outputSize);

    /* keep these handy */
    toPtr = ci.toPtr;
    rdPtr = ci.rdPtr;
    length = ci.length;

    /* resume default state */
    state = ci.state;
    ci.state = COM_DEFAULT;

    switch (state) {

    default:

	/*
	* while not EOF - compress in STACKSIZE blocks
	*/
	do {
	   /*************************************************
	    * Phase 1 - pop, compress, tally, push	    *
	    *************************************************/

	    /* reset the dynamic counts */
	    memset(ci.deflate->lenProbCount, 0, sizeof(ci.deflate->lenProbCount));
	    memset(ci.deflate->disProbCount, 0, sizeof(ci.deflate->disProbCount));

	    /* set the end-of-block count (only happens once) */
	    ci.deflate->lenProbCount[DF_ENDBLOCK] = 1;
	    ci.lenSqr = ci.lenNum = ci.lenSum = 1;
	    ci.oldLenVar = 1;

	    /* reset count totals */
	    ci.disSqr = ci.disNum = ci.disSum = 0;
	    ci.oldDisVar = 0;

	    /* stack empty */
	    ci.dStack = ci.lStack = 0;

	    /*
	    * stack saved codes if any
	    */
	    if (ci.sSave) {
		ci.blockSize = StackCodes(&ci, ci.sSize, n = (SHORT)(ci.sSave - ci.sSize));
		ci.fileSize -= ci.blockSize;
		ci.oldPtr = ci.lookBackBuf + (((USHORT)(toPtr - ci.lookBackBuf) - ci.blockSize) & WINDMASK);
		ci.oldSize = ci.blockSize;
		ci.sSave = 0;
	    } else {
		ci.oldPtr = toPtr;
		ci.oldSize = 0;
		n = 0;
	    }

	    /* initialize stack pointers */
	    ci.sPtr = ci.stack + (ci.sSize = n);
	    ci.altPtr = ci.altStack + ci.sSize;

	    /* no current string */
	    ci.curLength = -1;

	    /*
	    * while there is still data to read from the inFile, keep going
	    */
	    do {
		/*
		* if the last read was 0 bytes then we are at EOF
		*/
		if (ci.lastLength != 0) {

		    /*
		    * wrap if needed
		    */
		    if (rdPtr >= ci.endBuf)
			rdPtr = ci.lookBackBuf;

		    /*
		    * large read
		    */
		    if ((ci.lastLength = length) != 1) {

			/*
			* large read
			*/
    case COM_RDSTR:
			if (BufReadStrQuick(&ci.bf, rdPtr, length, &rdLen)) {

			    /*
			    * check for EOF
			    */
			    if ((UBYTE)eof)
				ci.lastLength = 0;
			    else {
				ci.state = COM_RDSTR;
				break;
			    }

			/*
			* something read
			*/
			} else {
			    /* distance from window start */
			    tmpVal = (USHORT)(rdPtr - ci.lookBackBuf);

			    /*
			    * copy shadow to first data bytes
			    */
			    n = (SHORT)rdLen;
			    if (tmpVal + n > WINDSIZE) {

				/* no more than what was read to shadow */
				FMEMCPY(ci.lookBackBuf, ci.endBuf, length = (SHORT)((tmpVal + n) - WINDSIZE));
				rdPtr = ci.lookBackBuf + length;

			    /*
			    * copy data bytes to shadow
			    */
			    } else {
				/*
				* if within first SHADOW bytes
				*/
				if (tmpVal < SHADOW) {

				    /*
				    * no more than what was read
				    */
				    if ((length = SHADOW - (SHORT)tmpVal) > n)
					length = n;

				    /* copy to shadow */
				    FMEMCPY(ci.endBuf + tmpVal, rdPtr, length);
				}

				/* next */
				rdPtr += n;
			    }

			    /* next */
			    ci.bufLength += n;
			}

		    /*
		    * small read
		    */
		    } else {
			/*
			* read character
			*/
    case COM_RDCHR:
			if (BufReadQuick(&ci.bf, rdPtr)) {

			    /*
			    * check for EOF
			    */
			    if ((UBYTE)eof)
				ci.lastLength = 0;
			    else {
				ci.state = COM_RDCHR;
				break;
			    }

			/*
			* character read
			*/
			} else {
			    /* distance from window start */
			    tmpVal = (USHORT)(rdPtr - ci.lookBackBuf);

			    /*
			    * copy to shadow if within first SHADOW bytes
			    */
			    if (tmpVal < SHADOW)
				*(ci.endBuf + tmpVal) = *rdPtr;

			    /* next */
			    rdPtr++;
			    ci.bufLength++;
			}
		    }
		}

		/*
		* need at least 3 for string lookback
		*/
		if (ci.bufLength >= 3) {

		    /* local copies */
		    bufLen = ci.bufLength;
		    tp = toPtr;

		    /* last hashed character */
		    repPtr = tp;

		    for (;;) {

			/* assume nothing */
			length = -1;

			/*
			* if current string complete do next
			*/
			if (ci.curLength != -1) {

			    ou = ci.hashTable[ci.curCode];
			    ci.hashTable[ci.curCode] = (u = (USHORT)(repPtr - ci.lookBackBuf));

			    /*
			    * if not already there (REP advance)
			    */
			    if (u != (ou & (USHORT)~REPFLAG))
				ci.colTable[u] = ou;
			    else
				ou = ci.colTable[u];

			    #if DEBUG > 2
				cprintf("1:hashCode %u set to loc %u, last was %u\r\n", ci.curCode, u, ou);
			    #endif

			    /*
			    * if repeated prefix
			    */
			    if (ci.curRep >= 3) {

				/* have a repeat */
				ci.hashTable[ci.curCode] |= REPFLAG;

				/*
				* first code already computed
				* hash two less than REP count
				*/
				i = (ci.curRep -= 2);
				while (--i > 0) {

				    if (++repPtr >= ci.endBuf)
					repPtr = ci.lookBackBuf;

				    ci.hashTable[ci.curCode] = (u = (USHORT)(repPtr - ci.lookBackBuf)) | REPFLAG;

				    #if DEBUG > 2
					cprintf("2:hashCode %u set to loc %u (REP)\r\n", ci.curCode, u);
				    #endif

				    /*
				    * if not already there (REP advance)
				    */
				    if (u != (ou & (USHORT)~REPFLAG))
					ci.colTable[u] = ou;
				    else
					ou = ci.colTable[u];
				}
			    }

			    /*
			    * need at least 3 for a string
			    * need more than current for another
			    */
			    if (ci.curLength < 3 || ci.curLength >= greedy || --bufLen < ci.curLength)
				break;

			    /*
			    * bump to next string
			    */
			    if (++tp >= ci.endBuf)
				tp = ci.lookBackBuf;
			}

			/*
			* determine maximum string length
			*/
			if ((maxLen = bufLen) > DF_MAXLENGTH)
			    maxLen = DF_MAXLENGTH;

			/*
			* greedy at zero does only 1 string
			* how far we will travel collision chains
			* don't look so hard if we have a good string
			*/
			greedy = (SHORT)((USHORT)maxLen & ci.greedyMask);
			depth = ci.depthSave;
			if (ci.curLength >= ci.goodLen)
			    depth = ci.altDepth;

			/*
			* check for repeated prefix
			*/
			ru = 0;
			rep = 1;
			u = *(p = tp);
			if (*++p == (UBYTE)u && *++p == (UBYTE)u) {
			    ru = REPFLAG;
			    rep = 3 - 1;
			    while (++rep < maxLen && *++p == (UBYTE)u)
				;
			}

			/* determine offset for end of new stuff */
			tmpVal = (USHORT)(rdPtr - tp) & WINDMASK;

			/* determine offset from buffer start */
			xu = (USHORT)(tp - ci.lookBackBuf);

			/* previous location of current string */
			u = ci.hashTable[code = HASHFUNC(tp[0], tp[1], tp[2])];

			/* no last string */
			lastOff = USHRT_MAX;

			/*
			* scan lookback buffer
			*/
			for (;;) {

			    /* determine hash pointer offset */
			    off = (USHORT)(((ou = u) & (USHORT)~REPFLAG) - xu) & WINDMASK;

			    /*
			    * if hash and string have repeated prefix
			    */
			    if (ou & ru) {

				/*
				* backup to match string REP if we
				* end at current string then previous
				* character was probably repeated
				*/
				if (!(off -= rep - (SHORT)3))
				    off = WINDMASK;
				if ((u = WINDSIZE - ci.bufUsed) > off)
				    off = u;

				/*
				* stop looking if hash entry invalid
				*/
				if (off < tmpVal)
				    break;

				/* determine hash pointer offset */
				u = (xu + off) & WINDMASK;

				/*
				* if a repeated prefix crosses string
				* then we need to move prefix start
				* just prior to string
				*/
				if (((off + rep) & WINDMASK) < tmpVal) {
				    u = (xu - 1) & WINDMASK;
				    off = WINDMASK;
				}

			    /*
			    * stop looking if hash entry invalid
			    */
			    } else if (off < tmpVal)
				break;

			    /*
			    * stop looking if not further away
			    */
			    if (off >= lastOff)
				break;

			    /*
			    * if xth and 1st character matches
			    */
			    hp = ci.lookBackBuf + (u & (USHORT)~REPFLAG);
			    if (length < 3 || *(USHORT FAR *)(hp + length - 1) == *(USHORT FAR *)(tp + length - 1)) {

				/*
				* must have at least three characters
				*/
				if (*(USHORT FAR *)(hp + 1) == *(USHORT FAR *)(tp + 1) && *hp == *tp) {

				    /* we have a match of 3 */
				    p = tp + 3;
				    hp += 3;

				    /*
				    * match in steps of four
				    * (big savings with 32 bit compilers)
				    */
				    n = maxLen - (SHORT)3;
				    while (n >= sizeof(ULONG) && !(*(ULONG FAR *)hp ^ *(ULONG FAR *)p)) {
					p += sizeof(ULONG);
					hp += sizeof(ULONG);
					n -= sizeof(ULONG);
				    }

				    /*
				    * attempt two more
				    */
				    if (n >= sizeof(USHORT) && *(USHORT FAR *)hp == *(USHORT FAR *)p) {
					p += sizeof(USHORT);
					hp += sizeof(USHORT);
					n -= sizeof(USHORT);
				    }

				    /*
				    * attempt one more
				    */
				    if (n > 0 && *hp == *p) {
					p++;
					n--;
				    }

				    /*
				    * better string found
				    */
				    if ((n = maxLen - n) > length) {
					offset = WINDSIZE - off;
					if ((nextPtr = p) >= ci.endBuf)
					    nextPtr -= WINDSIZE;
					if ((length = n) == maxLen)
					    break;
				    }

				} else {

				    /*
				    * penalty if hash entry invalid
				    */
				    if (!--depth)
					break;

				}
			    }

			    #if DEBUG > 2
			    cprintf(" code = %u, loc = %u, len = %d, off = %u\r\n", code, ou, len, off);
			    #endif

			    /*
			    * stop looking if depth reached
			    */
			    if (!--depth)
				break;

			    /* next */
			    u = ci.colTable[ou & (USHORT)~REPFLAG];
			    lastOff = off;
			}

			/*
			* if we did two passes
			*/
			if (ci.curLength != -1)
			    break;

			/*
			* use anything found
			*/
			ci.curCode = code;
			ci.curNextPtr = nextPtr;
			ci.curRep = rep;
			ci.curOffset = offset;

			/*
			* nothing (-1) should be (0)
			*/
			if ((ci.curLength = length) < 0)
			    ci.curLength = 0;
		    }

		    #if DEBUG > 2
		    cprintf(" curLength = %d, curOffset = %u, length = %d, offset = %u\r\n", ci.curLength, ci.curOffset, length, offset);
		    #endif

		    /*
		    * curLength, curOffset is 1st string
		    * length, offset is 2nd string
		    */

		    /*
		    * need at least three for a string
		    */
		    if (ci.curLength >= 3 && (ci.curLength > length || (ci.curLength == length && ci.curOffset <= offset))) {

			/* stop at buffer length minus already done */
			i = ci.bufLength - ci.curRep - 2;

			/* stop at string length minus already done */
			n = i - ci.curLength + ci.curRep;

			/* repeats already done */
			ci.curRep = (DF_MAXLENGTH-2);

			/*
			* generate hash codes for remaining string
			*/
			while (--i >= 0) {

			    if (++repPtr >= ci.endBuf)
				repPtr = ci.lookBackBuf;

			    /*
			    * prefix repeat
			    */
			    u = (USHORT)(repPtr - ci.lookBackBuf);
			    if (ci.curRep < (DF_MAXLENGTH-2) && repPtr[2] == repPtr[0]) {

				ci.hashTable[code] = u | REPFLAG;

				#if DEBUG > 2
				cprintf("4:hashCode %u set to loc %u (REP)\r\n", code, u);
				#endif

				/*
				* if not already there (REP advance)
				*/
				if (u != (ou & (USHORT)~REPFLAG))
				    ci.colTable[u] = ou;
				else
				    ou = ci.colTable[u];

				ci.curRep++;

			    /*
			    * normal code
			    */
			    } else if (i >= n) {

				ou = ci.hashTable[code = HASHFUNC(repPtr[0], repPtr[1], repPtr[2])];
				ci.hashTable[code] = u;

				/*
				* if not already there (REP advance)
				*/
				if (u != (ou & (USHORT)~REPFLAG))
				    ci.colTable[u] = ou;
				else
				    ou = ci.colTable[u];

				#if DEBUG > 2
				cprintf("3:hashCode %u set to loc %u, last was %u\r\n", code, u, ou);
				#endif

				ci.curRep = (DF_MAXLENGTH-2);
				if (repPtr[1] == repPtr[0] && repPtr[2] == repPtr[0]) {
				    ci.hashTable[code] |= REPFLAG;
				    ci.curRep = 1;
				}

			    /*
			    * end of string
			    */
			    } else
				break;
			}

			#if DEBUG > 1
			cprintf("%u/%u\r\n", ci.curOffset, ci.curLength);
			#endif

			/*
			* push length and short string on stack
			*/
			if (!(ci.altPtr->length = (UBYTE)(ci.curLength - 3))) {
			    ci.altPtr->lit1 = toPtr[0];
			    ci.altPtr->lit2 = toPtr[1];
			    ci.altPtr->lit3 = toPtr[2];
			}
			ci.altPtr++;

			/* look up the index for this length */
			i = lenExtraIndex[ci.curLength-3];

			/* update length count */
			ci.lenNum += !ci.deflate->lenProbCount[i+257];
			ci.lenSqr += ((USHORT)ci.deflate->lenProbCount[i+257]++ << 1) + 1;
			ci.lenSum++;

			/* save length index */
			ci.sPtr->idxLen = (UBYTE)i;

			/*
			* look up the index for this offset
			*/
			if ((u = (USHORT)(ci.curOffset - 1)) >= 256) u = (u >> 7) + 256;
			i = disExtraIndex[u];

			/* update offset count */
			ci.disNum += !ci.deflate->disProbCount[i];
			ci.disSqr += ((USHORT)ci.deflate->disProbCount[i]++ << 1) + 1;
			ci.disSum++;

			/* save offset index */
			ci.sPtr->idxOff = (UBYTE)i;

			/*
			* push offset on stack
			*/
			ci.sPtr->offset = ci.curOffset;
			ci.sPtr++;
			ci.sSize++;

			/* next position */
			toPtr = ci.curNextPtr;

			/* less in buffer/file */
			ci.fileSize -= ci.curLength;
			ci.bufLength -= ci.curLength;
			ci.oldSize += ci.curLength;

			/*
			* more in lookback buffer
			*/
			if (ci.bufUsed < WINDSIZE && (ci.bufUsed += ci.curLength) > WINDSIZE)
			    ci.bufUsed = WINDSIZE;

			/* string length */
			length = ci.curLength;
			ci.curLength = -1;

			/* compute variance for distances */
			disVar = ComputeVar(ci.disNum, ci.disSum, ci.disSqr);

			/*
			* if variance wider
			*/
			if (disVar > ci.oldDisVar || disVar < 1000) {
			    ci.oldDisVar = disVar;
			    ci.dStack = ci.sSize;

			/*
			* if variance within 1.56% of last and not at EOF
			*/
			} else if (disVar + (disVar >> 6) < ci.oldDisVar && ci.fileSize) {

			    /* unstack to widest variance */
			    ci.blockSize = UnStackCodes(&ci, (SHORT)(ci.sSize - ci.dStack));

			    /* correct sizes */
			    ci.fileSize += ci.blockSize;
			    ci.oldSize -= ci.blockSize;
			    ci.sSave = ci.sSize;
			    ci.sSize = ci.dStack;
			    break;
			}

			/* skip literal push */
			goto skipliteral;
		    }

		/*
		* if nothing to do stop!
		*/
		} else if (!ci.bufLength)
		    break;

		#if DEBUG > 1
		if (*toPtr > 32 && *toPtr < 127)
		    cprintf("%c\r\n", *toPtr);
		else
		    cprintf("%d\r\n", *toPtr);
		#endif

		/*
		* push literal onto the stack
		*/
		ci.sPtr->offset = 0;
		ci.sPtr->idxLen = *toPtr;
		ci.sPtr++;
		ci.altPtr++;
		ci.sSize++;

		/* update literal count */
		ci.lenNum += !ci.deflate->lenProbCount[*toPtr];
		ci.lenSqr += ((USHORT)ci.deflate->lenProbCount[*toPtr]++ << 1) + 1;
		ci.lenSum++;

		/*
		* next position
		*/
		if (++toPtr >= ci.endBuf)
		    toPtr = ci.lookBackBuf;

		/* less in buffer/file */
		ci.fileSize--;
		ci.bufLength--;
		ci.oldSize++;

		/*
		* more in lookBack buffer
		*/
		if (ci.bufUsed < WINDSIZE)
		    ci.bufUsed++;

		/* current string is next string */
		ci.curLength = length;
		ci.curRep = rep;
		ci.curOffset = offset;
		ci.curNextPtr = nextPtr;
		ci.curCode = code;

		/* one literal character */
		length = 1;

	       /****************************
		* string match enters here *
		****************************/

		skipliteral:

		/* compute variance for literals/lengths */
		lenVar = ComputeVar(ci.lenNum, ci.lenSum, ci.lenSqr);

		/*
		* if variance wider
		*/
		if (lenVar > ci.oldLenVar || lenVar < 1000) {
		    ci.oldLenVar = lenVar;
		    ci.lStack = ci.sSize;

		/*
		* if variance within 12.5% of last and not at EOF
		*/
		} else if (lenVar + (lenVar >> 3) < ci.oldLenVar && ci.fileSize) {

		    /* unstack to widest variance */
		    ci.blockSize = UnStackCodes(&ci, (SHORT)(ci.sSize - ci.lStack));

		    /* correct sizes */
		    ci.fileSize += ci.blockSize;
		    ci.oldSize -= ci.blockSize;
		    ci.sSave = ci.sSize;
		    ci.sSize = ci.lStack;
		    break;
		}

	    /*
	    * stop at end of stack
	    */
	    } while (ci.fileSize && ci.sSize < STACKSIZE);

	    /*
	    * stop on error
	    */
	    if (ci.state)
		break;

	   /*************************************************
	    * Phase 2 - write headers, generate, pop, write *
	    *************************************************/

	    /* calculate how big the block is using fixed codes */
	    fixSize = CalcBlockSize(flTable, fdTable, ci.stack, ci.sSize);

	    /* zero Huffman tables */
	    memset(ci.dlTable = ci.deflate->dlTable, 0, sizeof(ci.deflate->dlTable));

	    /*
	    * determine last literal/length code used
	    * we know code DF_ENDCODE used
	    */
	    ci.nLitCodes = DF_LITCODESUSED;
	    while (!ci.deflate->lenProbCount[ci.nLitCodes-1] && --ci.nLitCodes)
		;

	    /* build dynamic literal Huffman codes */
	    BuildHuffCodes(ci.dlTable, ci.deflate->lenProbCount, &ci.deflate->tmpTree, ci.nLitCodes, DF_LITBITS);

	    /*
	    * determine last distance code used
	    * may be none!
	    */
	    ci.nDisCodes = DF_DISCODESUSED;
	    while (!ci.deflate->disProbCount[ci.nDisCodes-1] && --ci.nDisCodes)
		;

	    /* build dynamic distance Huffman codes */
	    BuildHuffCodes(ci.ddTable = ci.dlTable + ci.nLitCodes, ci.deflate->disProbCount, &ci.deflate->tmpTree, ci.nDisCodes, DF_DISBITS);

	    /*
	    * force 1 distance code (0 length)
	    * pack literal/length and distance tables at once
	    */
	    ci.nDisCodes += !ci.nDisCodes;
	    ci.pkSize = PackLengths(ci.deflate->pkTable, ci.dlTable, (SHORT)(ci.nLitCodes + ci.nDisCodes));

	    /* re-use for bit-length counts */
	    memset(ci.deflate->lenProbCount, 0, DF_BITCODES * sizeof(SHORT));

	    /*
	    * count all the bit length codes (0-18)
	    * determine last length code used
	    */
	    pk = ci.deflate->pkTable;
	    i = ci.pkSize;
	    n = 0;
	    do {
		if (pk->bits >= n) n = pk->bits + 1;
		ci.deflate->lenProbCount[pk->bits]++;
		pk++;
	    } while (--i);

	    /*
	    * determine bit length codes used
	    * in required order (16 17 18 0 ...)
	    */
	    ci.nBitCodes = DF_BITCODES;
	    while (!ci.deflate->lenProbCount[bitLenValues[ci.nBitCodes-1]] && --ci.nBitCodes > 4)
		;

	    /* build the Huffman codes for the packed lengths of the Huffman codes */
	    BuildHuffCodes(ci.blTable = ci.ddTable + ci.nDisCodes, ci.deflate->lenProbCount, &ci.deflate->tmpTree, n, DF_BITBITS);

	    /* calculate the compressed size to see if the file got larger */
	    dynSize = CalcBlockSize(ci.dlTable, ci.ddTable, ci.stack, ci.sSize) +
		      CalcHeadSize(ci.deflate->pkTable, ci.blTable, ci.pkSize, ci.nBitCodes);

	    /* assume storing :-( */
	    ci.method = DF_STORED;

	    /*
	    * use dynamic codes if smaller than fixed
	    */
	    if (dynSize < fixSize) {

		/*
		* must be smaller than storing
		*/
		if (dynSize < ci.oldSize + (INT)sizeof(ULONG))
		    ci.method = DF_DYNAMIC;

	    /*
	    * fixed codes smaller than dynamic
	    */
	    } else {
		/*
		* must be smaller than storing
		*/
		if (fixSize < ci.oldSize + (INT)sizeof(ULONG)) {

		    /* replace dynamic tables with fixed */
		    FMEMCPY(ci.dlTable, flTable, sizeof(flTable));
		    FMEMCPY(ci.ddTable = ci.dlTable + DF_LITCODESUSED, fdTable, sizeof(fdTable));

		    /* fixed codes */
		    ci.method = DF_FIXED;
		}
	    }

	    /*
	    * write out the 3 bit block header:
	    *
	    *	*--2--*--1--*--0--*
	    *	   |	 |     |
	    *	   +-----+     last block if set
	    *	      |
	    *	      0 = stored
	    *	      1 = fixed
	    *	      2 = dynamic
	    *	      3 = reserved
	    */
    case COM_WRHEADER:
	    tmpBits = ((USHORT)ci.method << 1);
	    if (!ci.fileSize)
		tmpBits |= 1;
	    if (BitWrite(&ci.bi, &tmpBits, 3)) {
		ci.state = COM_WRHEADER;
		break;
	    }

	    /*
	    * if the block got bigger then store it
	    */
	    if (ci.method == DF_STORED) {

		/*
		* write out the block length and the one's compliment of the block length
		*/
    case COM_WRLENGTH:
		lenChk = (USHORT)(ci.len = (SHORT)ci.oldSize) | ((ULONG)~(USHORT)ci.oldSize << 16);
		if (BitWriteBytes(&ci.bi, (UBYTE FAR *)&lenChk, sizeof(ULONG))) {
		    ci.state = COM_WRLENGTH;
		    break;
		}

		/* last position */
		ci.ptr = ci.oldPtr;

		/*
		* while there is still data to write
		*/
		while (ci.len) {

		    /*
		    * not more than output buffer size
		    */
    case COM_WRBYTES:
		    if ((tmpVal = (USHORT)(ci.endBuf - ci.ptr)) > copySize)
			tmpVal = copySize;

		    /*
		    * not more than we can do without wrap-around
		    */
		    if ((USHORT)(n = ci.len) > tmpVal)
			n = (SHORT)tmpVal;

		    /*
		    * write data
		    */
		    if (BitWriteBytes(&ci.bi, ci.ptr, n)) {
			ci.state = COM_WRBYTES;
			break;
		    }

		    /*
		    * wrap around
		    */
		    if ((ci.ptr += n) >= ci.endBuf)
			ci.ptr = ci.lookBackBuf;

		    /* less to do */
		    ci.len -= n;
		    ci.oldSize -= n;
		}

	    /*
	    * block is compressed
	    */
	    } else {
		/*
		* if we are using dynamic codes then write out the Huffman
		* codes header and the bit length codes
		*/
		if (ci.method == DF_DYNAMIC) {

		    /*
		    * write out the number of literal codes to be sent (5 bits)
		    * write out the number of distance codes to be sent (5 bits)
		    * write out the number of bit length codes (4 bits)
		    */
    case COM_WRCOUNT:
		    tmpBits = (ci.nLitCodes - 257) | ((USHORT)(ci.nDisCodes - 1) << 5) | ((USHORT)(ci.nBitCodes - 4) << 10);
		    if (BitWrite(&ci.bi, &tmpBits, 5 + 5 + 4)) {
			ci.state = COM_WRCOUNT;
			break;
		    }

		    /*
		    * write out that many 3 bit bit length codes
		    */
		    ci.idx = 0;
		    do {
			/*
			* codes must be written in a certain order:
			* 16 17 18 0 8 7 9 6 10 5 11 4 12 3 13 2 14 1 15
			*/
    case COM_WRCODE1:
			tmpBits = (ci.blTable + bitLenValues[ci.idx])->bits;
			if (BitWriteQuick(&ci.bi, &tmpBits, 3)) {
			    ci.state = COM_WRCODE1;
			    break;
			}

		    } while (++ci.idx < ci.nBitCodes);

		    /*
		    * stop on error
		    */
		    if (ci.state)
			break;

		    /*
		    * write out all of the packed bit lengths
		    */
		    ci.pk = ci.deflate->pkTable;
		    ci.idx = ci.pkSize;
		    do {
			/*
			* write out bit lengths
			*/
    case COM_WRCODE2:
			tmpBits = (tt = ci.blTable + ci.pk->bits)->code;
			if (BitWriteQuick(&ci.bi, &tmpBits, tt->bits)) {
			    ci.state = COM_WRCODE2;
			    break;
			}

			/*
			* write out any extra bits needed
			*/
			if (ci.pk->extraBits) {
    case COM_WRCODE3:
			    tmpBits = ci.pk->extraLen;
			    if (BitWriteQuick(&ci.bi, &tmpBits, ci.pk->extraBits)) {
				ci.state = COM_WRCODE3;
				break;
			    }
			}

			/* next */
			ci.pk++;

		    } while (--ci.idx);

		    /*
		    * stop on error
		    */
		    if (ci.state)
			break;
		}

		/* reset the stack pointer */
		ci.altPtr = ci.altStack;
		ci.sPtr = ci.stack;
		ci.idx = ci.sSize;

		/*
		* while the stack has info in it
		*/
		while (--ci.idx >= 0) {

		    /* assume literal */
		    ci.ct = ci.dlTable + ci.sPtr->idxLen;

		    /*
		    * stacked record is offset/distance
		    */
		    if (ci.sPtr->offset) {

			ci.cte = lenExtraTable + ci.sPtr->idxLen;
			ci.ct += 257;

			ci.bte = disExtraTable + ci.sPtr->idxOff;
			ci.bt = ci.ddTable + ci.sPtr->idxOff;

			ci.len = ci.altPtr->length + 3;

			/*
			* three characters costs less than lookback
			*/
			if (ci.len == 3 &&
			    (ci.bits1 = ci.dlTable[ci.altPtr->lit1].bits) &&
			    (ci.bits2 = ci.dlTable[ci.altPtr->lit2].bits) &&
			    (ci.bits3 = ci.dlTable[ci.altPtr->lit3].bits) &&
			    (ci.bits1 + ci.bits2 + ci.bits3) <= (ci.ct->bits + ci.cte->bits + ci.bt->bits + ci.bte->bits)) {

			    /*
			    * write out each code in this string
			    */
    case COM_WRLIT1:
			    tmpBits = ci.dlTable[ci.altPtr->lit1].code;
			    if (BitWriteQuick(&ci.bi, &tmpBits, ci.bits1)) {
				ci.state = COM_WRLIT1;
				break;
			    }
    case COM_WRLIT2:
			    tmpBits = ci.dlTable[ci.altPtr->lit2].code;
			    if (BitWriteQuick(&ci.bi, &tmpBits, ci.bits2)) {
				ci.state = COM_WRLIT2;
				break;
			    }
    case COM_WRLIT3:
			    tmpBits = ci.dlTable[ci.altPtr->lit3].code;
			    if (BitWriteQuick(&ci.bi, &tmpBits, ci.bits3)) {
				ci.state = COM_WRLIT3;
				break;
			    }

			/*
			* lookback costs less than three characters
			*/
			} else {
			    /*
			    * write out length code
			    */
    case COM_WRLEN:
			    tmpBits = ci.ct->code;
			    if (BitWriteQuick(&ci.bi, &tmpBits, ci.ct->bits)) {
				ci.state = COM_WRLEN;
				break;
			    }

			    /*
			    * write out any extra bits needed to encode this length
			    */
			    if (ci.cte->bits) {
    case COM_WRXLEN:
				tmpBits = ci.len - ci.cte->code;
				if (BitWriteQuick(&ci.bi, &tmpBits, ci.cte->bits)) {
				    ci.state = COM_WRXLEN;
				    break;
				}
			    }

			    /*
			    * write out distance code
			    */
    case COM_WROFF:
			    tmpBits = ci.bt->code;
			    if (BitWriteQuick(&ci.bi, &tmpBits, ci.bt->bits)) {
				ci.state = COM_WROFF;
				break;
			    }

			    /*
			    * write out any extra bits needed to encode this offset
			    */
			    if (ci.bte->bits) {
    case COM_WRXOFF:
				tmpBits = ci.sPtr->offset - ci.bte->code;
				if (BitWriteQuick(&ci.bi, &tmpBits, ci.bte->bits)) {
				    ci.state = COM_WRXOFF;
				    break;
				}
			    }
			}

		    /*
		    * stacked record is a literal
		    */
		    } else {
			ci.len = 1;

			/*
			* write out literal
			*/
    case COM_WRCHR:
			tmpBits = ci.ct->code;
			if (BitWriteQuick(&ci.bi, &tmpBits, ci.ct->bits)) {
			    ci.state = COM_WRCHR;
			    break;
			}
		    }

		    /* less to do */
		    ci.oldSize -= ci.len;

		    ci.sPtr++;
		    ci.altPtr++;
		}

		/*
		* write out the End-Of-Block Code
		*/
		if (!ci.state) {
    case COM_WREOB:
		    tmpBits = ci.dlTable[DF_ENDBLOCK].code;
		    if (BitWrite(&ci.bi, &tmpBits, ci.dlTable[DF_ENDBLOCK].bits)) {
			ci.state = COM_WREOB;
			break;
		    }
		}
	    }

	    /*
	    * stop on error
	    */
	    if (ci.state)
		break;

	} while (ci.fileSize);

    }

    /* we are done if an error occured or all input read */
    done = !ci.state;

    /* how much input/output was processed */
    *inputSize = BufReadSetup(&ci.bf, NULL, 0, 0);
    *outputSize = BitWriteSetup(&ci.bi, NULL, (UINT)done);

    if (done)
	Zip2x_CompressFree(compInfo);

    else {
	/* restore the handys */
	ci.toPtr = toPtr;
	ci.rdPtr = rdPtr;
	ci.length = length;

	/* save state */
	*(struct ZIP2XCOMPINFO FAR *)compInfo = ci;
    }

    return (done);
}

/**
*
* name	    StackCodes - addes specified number of codes removed from stack
*
* synopsis  size = StackCodes(ci, offset, count)
*	    struct ZIP2XCOMPINFO *ci	    compress info structure
*	    SHORT offset		    offset to top of stack
*	    SHORT count 		    number of codes
*
* purpose   Stacks codes which would be better used in the next block
*
* returns   size = raw data size of stacked codes
*
**/

LONG LOCAL StackCodes(ci, offset, count)
struct ZIP2XCOMPINFO SEGSTACK *ci;
SHORT offset;
SHORT count;
{
    REGISTER struct CODESTACK FAR *sPtr;
    REGISTER struct ALTCODESTACK FAR *altPtr;
    REGISTER SHORT i;
    REGISTER LONG size;

    /* local copies */
    sPtr = ci->stack;
    altPtr = ci->altStack;

    /* stacked size */
    size = 0;

    /*
    * stack specified items
    */
    while (--count >= 0) {

	/* copy to stack bottom */
	*altPtr = *(altPtr + offset);
	*sPtr = *(sPtr + offset);

	/* assume literal index */
	i = sPtr->idxLen;

	/*
	* if length/offset pair
	*/
	if (sPtr->offset) {

	    /* correct offset amount */
	    ci->disNum += !ci->deflate->disProbCount[sPtr->idxOff];
	    ci->disSqr += ((USHORT)ci->deflate->disProbCount[sPtr->idxOff]++ << 1) + 1;
	    ci->disSum++;

	    /* correct size */
	    size += altPtr->length + (3-1);

	    /* get length index */
	    i += 257;
	}

	ci->lenNum += !ci->deflate->lenProbCount[i];
	ci->lenSqr += ((USHORT)ci->deflate->lenProbCount[i]++ << 1) + 1;
	ci->lenSum++;

	/* correct size */
	size++;

	/* next */
	sPtr++;
	altPtr++;
    }

    return (size);
}

/**
*
* name	    UnStackCodes - removes specified number of codes from stack
*
* synopsis  size = UnStackCodes(ci, count)
*	    struct ZIP2XCOMPINFO *ci	    compress info structure
*	    SHORT count;		    number of codes
*
* purpose   Unstacks codes which would be better used in the next block
*
* returns   size = raw data size of unstacked codes
*
**/

LONG LOCAL UnStackCodes(ci, count)
struct ZIP2XCOMPINFO SEGSTACK *ci;
SHORT count;
{
    REGISTER struct CODESTACK FAR *sPtr;
    REGISTER struct ALTCODESTACK FAR *altPtr;
    REGISTER SHORT i;
    REGISTER LONG size;

    /* local copies */
    sPtr = ci->sPtr;
    altPtr = ci->altPtr;

    /* unstacked size */
    size = 0;

    /*
    * unstack specified items
    */
    while (--count >= 0) {

	/* previous */
	altPtr--;
	sPtr--;

	/* assume literal index */
	i = sPtr->idxLen;

	/*
	* if length/offset pair
	*/
	if (sPtr->offset) {

	    /* correct offset count */
	    ci->deflate->disProbCount[sPtr->idxOff]--;
	    ci->disSum--;

	    /* correct size */
	    size += altPtr->length + (3-1);

	    /* get length index */
	    i += 257;
	}

	/* correct literal/length count */
	ci->deflate->lenProbCount[i]--;
	ci->lenSum--;

	/* correct size */
	size++;
    }

    return (size);
}

/**
*
* name	    PackLengths - packs bit length table
*
* synopsis  pkSize = PackLengths(pkTable, srcTable, srcSize,)
*	    struct PACKTABLE *pkTable	    compressed bit length table
*	    struct CODETABLE *srcTable	    bit length codes table
*	    SHORT srcSize		    size of table
*
* purpose   Pack bit length table using run-length encoding
*
* returns   pkSize = packed table size
*
**/

SHORT LOCAL PackLengths(pkTable, srcTable, srcSize)
struct PACKTABLE *pkTable;
struct CODETABLE *srcTable;
SHORT srcSize;
{
    SHORT pkSize, length, limit;
    SHORT last;

    /* start with nothing */
    pkSize = 0;
    last = UCHAR_MAX;

    /*
    * until end-of-table
    */
    do {
	/* must add 1 */
	length = 1;

	/*
	* if a new code
	*/
	if ((SHORT)srcTable->bits != last) {

	    /* assume a zero */
	    limit = 138;

	    /*
	    * if non-zero
	    */
	    if (last = srcTable->bits) {
		limit = 6;
		length--;
		pkTable->bits = (UBYTE)last;
		pkTable->extraBitsLen = 0;
		pkTable++;
		pkSize++;
	    }
	}

	/*
	* continue while we keep getting the same characters
	* if the run is not zeros then it can only have a length of 6 or less
	* if the run is zeros then it can have a length up to 138
	*/
	if (--srcSize) {
	    do {
		if ((SHORT)(++srcTable)->bits == last && length < limit)
		    length++;
		else
		    break;
	    } while (--srcSize);
	}

	/*
	* 3 repeats for RLE
	*/
	if (length >= 3) {

	    /*
	    * RLE for non-zeros use code 16
	    */
	    if (last) {
		pkTable->extraLen = (UBYTE)length - (UBYTE)3;
		pkTable->bitsExtraBits = 16 | (2 << 8);

	    /*
	    * if the RLE under 11 use code 17
	    */
	    } else if (length < 11) {
		pkTable->extraLen = (UBYTE)length - (UBYTE)3;
		pkTable->bitsExtraBits = 17 | (3 << 8);

	    /*
	    * otherwise RLE is 11 to 138 so use code 18
	    */
	    } else {
		pkTable->extraLen = (UBYTE)length - (UBYTE)11;
		pkTable->bitsExtraBits = 18 | (7 << 8);
	    }

	    pkTable++;
	    pkSize++;

	/*
	* non RLE
	*/
	} else if (length) {

	    /*
	    * add to table
	    */
	    do {
		pkTable->bits = (UBYTE)last;
		pkTable->extraBitsLen = 0;
		pkTable++;
		pkSize++;
	    } while (--length);
	}

    } while (srcSize);

    return (pkSize);
}

/**
*
* name	    CalcHeadSize - calculate dynamic code header size
*
* synopsis  size = CalcHeadSize(pkTable, blTable, pkSize, nBitCodes)
*	    struct PACKTABLE *pkTable	    compressed bit length table
*	    struct CODETABLE *blTable	    bit length codes table
*	    SHORT pkSize		    compressed table size
*	    SHORT nBitCodes		    number of bit length codes
*
* purpose   Determines size of code header + bit lengths header
*
* returns   size = total size of header in bytes
*
**/

SHORT LOCAL CalcHeadSize(pkTable, blTable, pkSize, nBitCodes)
struct PACKTABLE *pkTable;
struct CODETABLE *blTable;
SHORT pkSize;
SHORT nBitCodes;
{
    REGISTER struct PACKTABLE *pk;
    LONG size;

    /* literal codes + distance codes + bit length codes + 3 bits per */
    size = (5 + 5 + 4) + nBitCodes * 3;

    /*
    * total the packed bit lengths
    */
    pk = pkTable;
    do {
	size += blTable[pk->bits].bits + pk->extraBits;
	pk++;
    } while (--pkSize);

    size = (size + 7) >> 3;
    return ((SHORT)size);
}

/**
*
* name	    CalcBlockSize - calculate compressed data size
*
* synopsis  newSize = CalcBlockSize(lenTable, disTable, sPtr, size)
*	    struct CODETABLE *lenTable	    table of literal/length codes
*	    struct CODETABLE *disTable	    table of distance codes
*	    struct CODESTACK *sPtr	    block of compressed data
*	    SHORT size			    size of the data block
*
* purpose   Determines size of a data block after compression
*
* returns   newSize = total data size after compression
*
**/

LONG LOCAL CalcBlockSize(lenTable, disTable, sPtr, sSize)
struct CODETABLE FAR *lenTable;
struct CODETABLE FAR *disTable;
struct CODESTACK FAR *sPtr;
SHORT sSize;
{
    REGISTER LONG size;

    /* start with nothing */
    size = 0;

    while (--sSize >= 0) {

	/*
	* if it was a literal
	*/
	if (!sPtr->offset)
	    size += lenTable[sPtr->idxLen].bits;

	/*
	* otherwise it was a length/offset
	*/
	else {
	    /* update the number of bits needed for this length */
	    size += lenTable[sPtr->idxLen+257].bits + lenExtraTable[sPtr->idxLen].bits;

	    /* update the number of bits needed for this offset */
	    size += disTable[sPtr->idxOff].bits + disExtraTable[sPtr->idxOff].bits;
	}

	sPtr++;
    }

    size = (size + 7) >> 3;
    return (size);
}

#if VARIANCE_C

/*
* ComputeVar - compute variance for number sequence
*/
LONG INLINE LOCAL ComputeVar(num, sum, sqr)
SHORT num;
SHORT sum;
LONG sqr;
{
    /* this uses slow 32 bit divide/multiply helper functions */
    return ((sqr - (LONG)sum * sum / num) / num);
}

#else

#pragma warning(disable:4035)
#pragma optimize("egl", off)

/*
* ComputeVar - compute variance for number sequence
*/
LONG INLINE LOCAL ComputeVar(num, sum, sqr)
SHORT num;
SHORT sum;
LONG sqr;
{
    _asm
    {
;	compute sum * sum

	mov	si,word ptr [sum]
	mov	ax,si
	mul	si			; dx:ax = sum * sum

;	compute sum * sum / num

	mov	si,word ptr [num]
	mov	cx,ax			; save low
	mov	ax,dx			;
	sub	dx,dx			; dx:ax = high
	div	si			; dx:ax = high / num
	xchg	cx,ax			; dx:ax = quotient + low
	div	si			; ax = (quotient + low) / num

;	compute sqr - sum * sum / num

	mov	bx,word ptr [sqr+0];
	sub	bx,ax
	mov	ax,word ptr [sqr+2];
	sbb	ax,cx			; ax:bx = result

;	compute (sqr - sum * sum / num) / num

	sub	dx,dx			; dx:ax = high
	div	si			; dx:ax = high / num
	xchg	bx,ax			; dx:ax = quotient + low
	div	si			; ax = (quotient + low) / num
	mov	dx,bx
    }
}

#pragma optimize("", on)

#endif

