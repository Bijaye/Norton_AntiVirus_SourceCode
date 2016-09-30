/**

    ZZZZZ   IIIII   PPPP     222    X	X   EEEEE   X	X   PPPP	CCCC
       Z      I     P	P   2	2    X X    E	     X X    P	P      C
      Z       I     PPPP       2      X     EEE       X     PPPP       C
     Z	      I     P	      2      X X    E	     X X    P	       C
    ZZZZZ   IIIII   P	    22222   X	X   EEEEE   X	X   P	    *	CCCC

    Description: Zip 2.x expansion routines (inflate)

    Product: Central Point 'PC TOOLS'

    CONFIDENTIAL and PROPRIETARY
    (c) Copyright 1992,93,94 Quintessence Corporation
    (c) Copyright 1993,94 Central Point Software
    All rights reserved.

    Revision History:

    Version	Date	    Author	Comments
    -------	--------    ------	--------------------------------
    2.10	01-15-93    SJP 	original code for QUINZIP DLL
    2.50	07-05-93    SJP 	rewritten API to CPS specifications
    2.50	08-06-93    SJP 	made HuffBuildDynamicTrees() LOCAL
    2.75	06-21-94    SJP 	better memory allocation, improved
					Decode() improved state switching
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

#define DEBUG 0
#if DEBUG
#include <conio.h>
#endif

/* Private include files. */
#include "pzip2x.h"

/****************************************
 * definitions/enumerations		*
 ****************************************/

#define WINDSIZE 32768U

/*
* expand states
*/
enum {
    EXP_DEFAULT,
    EXP_RDHEADER,
    EXP_RDLENGTH,
    EXP_RDBYTES,
    EXP_WRBYTES,
    EXP_RDTREES,
    EXP_CPEEK8,
    EXP_DECODE,
    EXP_RDXLEN,
    EXP_OPEEK8,
    EXP_DECOFF,
    EXP_RDXOFF,
    EXP_WRSTR,
    EXP_WRCHR,
    EXP_WRLIT,
    EXP_ERROR,
};

/*
* tree construction states
*/
enum {
    TREE_DEFAULT,
    TREE_RDCOUNT,
    TREE_RDLENS,
    TREE_PEEK7,
    TREE_XREAD,
    TREE_ERROR,
};

/*
* decode states
*/
enum {
    DECODE_DEFAULT,
    DECODE_PEEK,
    DECODE_ERROR,
};

/****************************************
* local data structures 		*
*****************************************/

/*
* current expansion state information
*/
struct ZIP2XEXPINFO {

    /*
    * initialized not changed by states
    */
    struct {
	UBYTE FAR *lookBackBuf;
	UBYTE FAR *endBuf;
	struct INFLATE *inflate;
    };

    /*
    * changed between states
    */
    struct {
	UBYTE FAR *toPtr, FAR *fromPtr;
	struct CODETABLE *cte;
	USHORT offset, length;
	USHORT header;
	USHORT value;
	SHORT state;
	UBYTE fixed;
    };

    /*
    * dynamic tree construction states
    */
    struct TREEINFO {
	struct SFTD *sftd;
	SHORT nLitCodes, nDisCodes, nBitCodes;
	SHORT nLitDisCodes, val, bits;
	SHORT state;
	SHORT idx;
    } ti;

    /* bit I/O state information */
    struct BITINFO bi;

    /* buffer I/O state information */
    struct BUFINFO bf;

    BOOL fSaved;
    UINT tmpValSaved;
};

/****************************************
* external data 			*
*****************************************/

/* extra bits needed for lookback */
IMPORT struct CODETABLE NEAR lenExtraTable[];
IMPORT struct CODETABLE NEAR disExtraTable[];

/* bit length code values */
IMPORT UBYTE NEAR bitLenValues[];

/****************************************
 * inline functions			*
 ****************************************/

#include "zip2x.i"

/**
*
* name	    Zip2x_ExpandWorkSize - returns size of expand info structure
*
* synopsis  size = Zip2x_ExpandWorkSize()
*	    USHORT size 		    expand info structure size
*
* purpose   Returns the size of the state info structure required by the
*	    ZIP 2x expansion function.
*
* returns   size = size of expand info structure
*
**/

USHORT FAR PASCAL Zip2x_ExpandWorkSize()
{
    return (sizeof(struct ZIP2XEXPINFO));
}

/**
*
* name	    Zip2x_ExpandInit - initializes expand info structure
*
* synopsis  err = Zip2x_ExpandInit(expInfo)
*	    VOID *expInfo		    expand info structure
*
* purpose   Initializes structure that contains state info for the ZIP 2x
*	    expansion function.
*
* returns   err = TRUE if memory allocations fail
*
**/

BOOL FAR PASCAL Zip2x_ExpandInit(expInfo)
VOID FAR *expInfo;
{
    REGISTER struct ZIP2XEXPINFO FAR *ei;
    BOOL err;

    /* assume success */
    err = FALSE;

    /* type compress info structure */
    ei = expInfo;

    /* initialize info structure to zeros */
    FMEMSET(ei, 0, sizeof(struct ZIP2XEXPINFO));

    /*
    * allocate the lookback and miscellanous
    */
    if ((ei->lookBackBuf = ZipMemAllocFar(WINDSIZE)) == NULL ||
	(ei->inflate = ZipMemAllocNear(sizeof(struct INFLATE))) == NULL) {

	    Zip2x_ExpandFree(ei);
	    err = TRUE;

    /*
    * allocation was successful
    */
    } else {
	/*
	* lookback buffer should be zero
	* so seeks past begining of file yield zero
	*/
	FMEMSET(ei->lookBackBuf, 0, WINDSIZE);

	/* initialize end of lookback buffer */
	ei->endBuf = ei->lookBackBuf + WINDSIZE;

	/* initialize lookback pointers */
	ei->toPtr = ei->lookBackBuf;

	/* flag no trees built
	ei->fixed = FALSE;
	*/

	/* initialize bit operations */
	BitReadInit(&ei->bi);

	/* initialize buffer operations */
	BufWriteInit(&ei->bf);
    }

    return (err);
}

/**
*
* name	    Zip2x_ExpandFree - free memory allocated at initialization
*
* synopsis  Zip2x_ExpandFree(expInfo)
*	    struct ZIP2XEXPINFO *expInfo    expand info structure
*
* purpose   Frees memory allocated at initialization
*
**/

VOID FAR PASCAL Zip2x_ExpandFree(expInfo)
VOID FAR *expInfo;
{
    REGISTER struct ZIP2XEXPINFO FAR *ei;

    /* type compress info structure */
    ei = expInfo;

    /*
    * free allocations
    */
    if (ei->inflate != NULL)
	ZipMemFreeNear(ei->inflate);
    if (ei->lookBackBuf != NULL)
	ZipMemFreeFar(ei->lookBackBuf);

    /* initialize info structure to zeros */
    FMEMSET(ei, 0, sizeof(struct ZIP2XEXPINFO));
}

/**
*
* name	    Zip2x_Expand - expand data block using current state
*
* synopsis  done = Zip2x_Expand(inputBuf, inputSize, outputBuf, outputSize, expInfo, eof)
*	    BOOL done			    completed flag
*	    UBYTE *inputBuf		    compressed data buffer
*	    USHORT *inputSize		    address of input buffer size
*	    UBYTE *outputBuf		    uncompressed data buffer
*	    USHORT *outputSize		    address of output buffer size
*	    VOID *expInfo		    expand info structure
*	    BOOL eof			    last block flag
*
* purpose   Expands a block of data using the ZIP 2.x 'inflate' method
*	    with current state info structure.
*
*	    Note: Minimum buffer size is DF_MAXLENGTH bytes
*
* returns   done = TRUE if last block and expansion complete
*
**/

BOOL FAR PASCAL Zip2x_Expand(inputBuf, inputSize, outputBuf, outputSize, expInfo, eof)
UBYTE FAR *inputBuf;
unsigned short FAR *inputSize;
UBYTE FAR *outputBuf;
unsigned short FAR *outputSize;
VOID FAR *expInfo;
char eof;
{
    UINT SEGDATA tmpBits;
    USHORT SEGDATA tmpVal;

    /* current state information */
    struct ZIP2XEXPINFO ei;

    ULONG lenChk;
    USHORT readSize;
    USHORT copySize;
    SHORT state;
    BOOL done;

    /* make expand info local */
    ei = *(struct ZIP2XEXPINFO FAR *)expInfo;

    /* setup bit operations */
    readSize = (USHORT)BitReadSetup(&ei.bi, inputBuf, *inputSize, eof);

    /*
    * setup buffer operations
    */
    if ((copySize = (USHORT)BufWriteSetup(&ei.bf, outputBuf, *outputSize)) > readSize)
	copySize = readSize;

    /* resume default state */
    state = ei.state;
    ei.state = EXP_DEFAULT;

    switch (state) {

    default:

	do {
	    /*
	    * read header from input stream
	    */
    case EXP_RDHEADER:
	    if (BitRead(&ei.bi, &tmpBits, 3)) {
		ei.state = EXP_RDHEADER;
		break;
	    }

	    /*
	    * check for unknown block type
	    */
	    ei.header = (USHORT)tmpBits;
	    if ((tmpBits >>= 1) == DF_RESERVED) {
		ei.state = EXP_ERROR;
		break;
	    }

	    /*
	    * if this is a stored block then do a flat copy
	    */
	    if (tmpBits == DF_STORED) {

		#if DEBUG
		cprintf("Stored block\r\n");
		#endif

		/*
		* read length and 1's compliment of length
		*/
    case EXP_RDLENGTH:
		if (BitReadBytes(&ei.bi, (UBYTE FAR *)&lenChk, sizeof(ULONG))) {
		    ei.state = EXP_RDLENGTH;
		    break;
		}

		/*
		* verify the length with the check
		*/
		if ((ei.length = (USHORT)lenChk) != (USHORT)~(USHORT)(lenChk >> 16)) {
		    ei.state = EXP_ERROR;
		    break;
		}

		/*
		* until copy complete
		*/
		while (ei.length) {

		    /*
		    * not more than 'copySize'
		    */
		    if ((tmpVal = (USHORT)(ei.endBuf - ei.toPtr)) > copySize)
			tmpVal = copySize;

		    /*
		    * not more than we can do without wrap-around
		    */
		    if ((ei.offset = ei.length) > tmpVal)
			ei.offset = tmpVal;

		    /*
		    * read/write data
		    */
    case EXP_RDBYTES:
		    if (BitReadBytes(&ei.bi, ei.toPtr, (SHORT)ei.offset)) {
			ei.state = EXP_RDBYTES;
			break;
		    }
    case EXP_WRBYTES:
		    if (BufWriteStr(&ei.bf, ei.toPtr, (SHORT)ei.offset)) {
			ei.state = EXP_WRBYTES;
			break;
		    }

		    /*
		    * wrap around
		    */
		    if ((ei.toPtr += ei.offset) >= ei.endBuf)
			ei.toPtr = ei.lookBackBuf;

		    /* less to do */
		    ei.length -= ei.offset;
		}

	    /*
	    * otherwise loop until end of block code found
	    */
	    } else {

		/*
		* otherwise build the dynamic codes
		*/
		if (tmpBits == DF_DYNAMIC) {

		    #if DEBUG
		    cprintf("Dynamic block\r\n");
		    #endif

		    /*
		    * build dynamic trees
		    */
    case EXP_RDTREES:
		    if (state = BuildDynamicSFTrees(&ei)) {
			ei.state = EXP_RDTREES;
			if (state == TREE_ERROR)
			    ei.state = EXP_ERROR;
			break;
		    }

		/*
		* if using fixed codes
		*/
		} else {

		    #if DEBUG
		    cprintf("Fixed block\r\n");
		    #endif

		    /* build fixed trees */
		    BuildFixedSFTrees(&ei);
		}

		for (;;) {

		    /*
		    * fetch 8 bits from input stream
		    */
    case EXP_CPEEK8:
		    if (BitPeekQuick8(&ei.bi, &tmpBits)) {
			ei.state = EXP_CPEEK8;
			break;
		    }

		    /*
		    * if not in table decode remainder
		    * (note: this if-else structure important)
		    */
		    if ((SHORT)(ei.value = ei.inflate->litTree.vals[tmpBits]) < 0) {
			BitSkip(&ei.bi, 8);
    case EXP_DECODE:
			tmpVal = ei.value;
			if (state = Decode(&ei.bi, ei.inflate->litTree.tree, &tmpVal)) {
			    ei.state = EXP_DECODE;
			    if (state == DECODE_ERROR)
				ei.state = EXP_ERROR;
			    break;
			}

		    /*
		    * skip exact bits for code
		    */
		    } else {
			BitSkipQuick(&ei.bi, ei.inflate->litTree.lens[tmpBits]);
			tmpVal = ei.value;
		    }

		    /*
		    * literal/length
		    */
		    if (tmpVal >= DF_ENDBLOCK) {

			/*
			* stop on ENDBLOCK
			*/
			if (tmpVal == DF_ENDBLOCK)
			    break;

			/*
			* read the required number of extra bits
			* and get the correct length from the table
			*/
			ei.length = (ei.cte = lenExtraTable + tmpVal - 257)->code;
			if (ei.cte->bits) {

			    /*
			    * read extra bits
			    */
    case EXP_RDXLEN:
			    if (BitReadQuick(&ei.bi, &tmpBits, ei.cte->bits)) {
				ei.state = EXP_RDXLEN;
				break;
			    }

			    /* correct length */
			    ei.length += (USHORT)tmpBits;
			}

			/*
			* fetch 8 bits from input stream
			*/
    case EXP_OPEEK8:
			if (BitPeekQuick8(&ei.bi, &tmpBits)) {
			    ei.state = EXP_OPEEK8;
			    break;
			}

			/*
			* if not in table decode remainder
			* (note: this if-else structure important)
			*/
			if ((SHORT)(ei.value = ei.inflate->disTree.vals[tmpBits]) < 0) {
			    BitSkip(&ei.bi, 8);
    case EXP_DECOFF:
			    tmpVal = ei.value;
			    if (state = Decode(&ei.bi, ei.inflate->disTree.tree, &tmpVal)) {
				ei.state = EXP_DECOFF;
				if (state == DECODE_ERROR)
				    ei.state = EXP_ERROR;
				break;
			    }

			/*
			* skip exact bits for code
			*/
			} else {
			    BitSkipQuick(&ei.bi, ei.inflate->disTree.lens[tmpBits]);
			    tmpVal = ei.value;
			}

			/*
			* read the required number of extra bits
			* and get the correct offset from the table
			*/
			ei.offset = (ei.cte = disExtraTable + tmpVal)->code;
			if (ei.cte->bits) {

			    /*
			    * read extra bits
			    */
    case EXP_RDXOFF:
			    if (BitReadQuick(&ei.bi, &tmpBits, ei.cte->bits)) {
				ei.state = EXP_RDXOFF;
				break;
			    }

			    /* correct offset */
			    ei.offset += (USHORT)tmpBits;
			}

			#if DEBUG
			cprintf("%u/%u ", ei.offset, ei.length);
			#endif

			/*
			* Go back 'offset' bytes in Look Back Buffer and copy 'length'
			* bytes into a temporary buffer, and push them back onto queue
			*/
			ei.fromPtr = ei.toPtr - ei.offset;
			if ((USHORT)(ei.toPtr - ei.lookBackBuf) < ei.offset)
			    ei.fromPtr += WINDSIZE;

			#if DEBUG
			if (*ei.fromPtr > 32 && *ei.fromPtr < 127)
			    cprintf("(%c)\r\n", *ei.fromPtr);
			else
			    cprintf("(%d)\r\n", *ei.fromPtr);
			#endif

			/*
			* do fast copy/write if no overlap or wrap-around
			*/
			if ((ei.fromPtr < ei.toPtr) ? ((USHORT)(ei.endBuf - ei.toPtr) > ei.length && ei.fromPtr + ei.length <= ei.toPtr) : ((USHORT)(ei.endBuf - ei.fromPtr) >= ei.length && ei.toPtr + ei.length <= ei.fromPtr)) {

			    /*
			    * write string
			    */
    case EXP_WRSTR:
			    if (BufWriteStrQuick(&ei.bf, ei.fromPtr, ei.length)) {
				ei.state = EXP_WRSTR;
				break;
			    }

			    FMEMCPY(ei.toPtr, ei.fromPtr, ei.length);
			    ei.toPtr += ei.length;

			/*
			* must copy/write 1 byte at a time
			*/
			} else {
			    /*
			    * copy/write 1 byte at a time
			    */
			    do {
				/*
				* write out byte
				*/
    case EXP_WRCHR:
				if (BufWriteQuick(&ei.bf, ei.fromPtr)) {
				    ei.state = EXP_WRCHR;
				    break;
				}

				/*
				* add byte to the look-Back Buffer
				*/
				*ei.toPtr = *ei.fromPtr;
				if (++ei.toPtr >= ei.endBuf)
				    ei.toPtr = ei.lookBackBuf;

				/*
				* next byte
				*/
				if (++ei.fromPtr >= ei.endBuf)
				    ei.fromPtr = ei.lookBackBuf;

			    } while (--ei.length);

			    if (ei.state)
				break;
			}

		    /*
		    * literal character
		    */
		    } else {
			/* state save value */
			ei.value = tmpVal;

			#if DEBUG
			if (tmpVal > 32 && tmpVal < 127)
			    cprintf("%c\r\n", tmpVal);
			else
			    cprintf("%d\r\n", tmpVal);
			#endif

			/*
			* Write out 1 byte
			*/
    case EXP_WRLIT:
			if (BufWriteQuick(&ei.bf, (UBYTE FAR *)&ei.value)) {
			    ei.state = EXP_WRLIT;
			    break;
			}

			/*
			* copy literal to output stream
			*/
			*ei.toPtr = (UBYTE)ei.value;
			if (++ei.toPtr >= ei.endBuf)
			    ei.toPtr = ei.lookBackBuf;
		    }
		}
	    }

	    /*
	    * stop on errors
	    */
	    if (ei.state)
		break;

	/*
	* continue while 1st bit of the block header is zero
	*/
	} while (!(ei.header & 1));
    }

    /* we are done if an error occured or all input read */
    done = ((ei.state == EXP_ERROR) | !ei.state);

    /* how much input/output was processed */
    *inputSize = BitReadSetup(&ei.bi, NULL, (UINT)done, 0);
    *outputSize = BufWriteSetup(&ei.bf, NULL, 0);

    if (done)
	Zip2x_ExpandFree(expInfo);
    else {
	/* save state */
	*(struct ZIP2XEXPINFO FAR *)expInfo = ei;
    }

    return (done);
}

/**
*
* name	    BuildDynamicSFTrees - Build dynamic Shannon-Fano trees
*
* synopsis  state = BuildDynamicSFTrees(ei)
*	    struct ZIP2XEXPINFO *ei;	    expand info structure
*
* purpose   Reads/creates Shannon-Fano tree description and passes
*	    it to tree builder
*
* returns   state = TREE_DEFAULT for success
*		  = TREE_ERROR if error in data stream
*		  = TREE_NNNNN if within a state
*
**/

SHORT LOCAL BuildDynamicSFTrees(ei)
struct ZIP2XEXPINFO SEGSTACK *ei;
{
    /*
    * bits/base for codes 16,17,18
    */
    STATIC struct {
	UBYTE extra;
	UBYTE base;
    } lenTable[] = {
	{2, 3},
	{3, 3},
	{7, 11},
    };

    UINT SEGDATA tmpVal;

    /* current state information */
    struct TREEINFO ti;
    USHORT u;
    SHORT state;

    REGISTER struct ITMPTREE *tmpTree;

    /* temporary tree storage */
    tmpTree = &ei->inflate->tmpTree;

    /* resume default state */
    state = ei->ti.state;
    ei->ti.state = ti.state = TREE_DEFAULT;

    /*
    * if state saved
    */
    if (state)
	ti = ei->ti;

    /*
    * resume interrupted state
    */
    switch (state) {

    default:
	/* flag dynamic trees */
	ei->fixed = FALSE;

	/*
	* read 5 bits for # of literal codes
	* read 5 bits for # of distance code
	* read 4 bits for # of bit length codes
	*/
    case TREE_RDCOUNT:
	if (BitRead(&ei->bi, &tmpVal, (5 + 5 + 4)))
	    ti.state = TREE_RDCOUNT;

	else {
	    /* literal codes range from 257 to 288 */
	    ti.nLitCodes = (SHORT)((tmpVal & 31) + 257);

	    /* distance codes range from 1 to 32 */
	    ti.nDisCodes = (SHORT)(((tmpVal >>= 5) & 31) + 1);

	    /* bit length codes range from 4 to 19 */
	    ti.nBitCodes = (SHORT)(((tmpVal >> 5) & 15) + 4);

	    /* zero SF tree descriptor */
	    memset(&tmpTree->bitTemp, 0, sizeof(tmpTree->bitTemp));

	    /*
	    * read the bit lengths for the bit length codes
	    */
	    ti.idx = 0;
	    do {
		/*
		* read bit length
		*/
    case TREE_RDLENS:
		if (BitReadQuick(&ei->bi, &tmpVal, 3)) {
		    ti.state = TREE_RDLENS;
		    break;
		}

		/*
		* only non-zero lengths
		*/
		if (tmpVal) {

		    /*
		    * codes are in a certain order:
		    * 16 17 18 0 8 7 9 6 10 5 11 4 12 3 13 2 14 1 15
		    */

		    /* set bit length for value */
		    tmpTree->bitTemp.codeLens[bitLenValues[ti.idx]] = (UBYTE)tmpVal;

		    /* count number of codes for each length */
		    tmpTree->bitTemp.codeLenCounts[(SHORT)tmpVal-1]++;
		}

	    } while (++ti.idx < ti.nBitCodes);
	}

	/*
	* build the bit length code tree to decode the
	* lengths for the other trees
	*/
	if (!ti.state) {
	    if (BuildSFTree((struct SFTD *)&tmpTree->bitTemp, (struct SFTT *)&tmpTree->bitTree, &ei->inflate->tmpSFT, DF_BITCODES))
		ti.state = TREE_ERROR;
	}

	/*
	* no errors thus far
	*/
	if (!ti.state) {

	    /* zero both SF tree descriptors */
	    memset(&tmpTree->litTemp, 0, sizeof(tmpTree->litTemp));
	    memset(&tmpTree->disTemp, 0, sizeof(tmpTree->disTemp));

	    /* init tempTree, value and last bit length */
	    ti.sftd = (struct SFTD *)&tmpTree->litTemp;
	    ti.val = ti.bits = 0;

	    /*
	    * must decode both trees at once since its
	    * packed that way
	    */
	    ti.nLitDisCodes = (SHORT)(ti.nLitCodes + ti.nDisCodes);
	    ti.idx = 0;
	    do {
		/*
		* fetch 7 bits from input stream
		*/
    case TREE_PEEK7:
		if (BitPeekQuick7(&ei->bi, &tmpVal)) {
		    ti.state = TREE_PEEK7;
		    break;
		}

		/* skip exact bits for this code */
		BitSkipQuick(&ei->bi, tmpTree->bitTree.lens[u = (USHORT)tmpVal & 0x7F]);

		/* value must be in table */
		tmpVal = tmpTree->bitTree.vals[u];

		/*
		* nothing unusual
		*/
		if (tmpVal < 16) {
		    ti.bits = (SHORT)tmpVal;
		    tmpVal = 1;

		/*
		* RLE code 16,17,18
		*/
		} else {
		    /*
		    * read extra bits for run
		    */
    case TREE_XREAD:
          if (ei->fSaved)
          {
             ei->fSaved = FALSE;
             tmpVal = ei->tmpValSaved;
          }
          if (BitReadQuick(&ei->bi, &tmpVal, lenTable[u = tmpVal - 16].extra)) 
          {
             ei->fSaved = TRUE;
             ei->tmpValSaved = tmpVal;
             ti.state = TREE_XREAD;
   	       break;
          }

		    /* correct the run */
		    tmpVal += lenTable[u].base;

		    /*
		    * run of zeros for 16 otherwise last
		    */
		    if (u)
			    ti.bits = 0;
		}

		/*
		* zero length code (skipped)
		*/
		if (!ti.bits) {

		    ti.val += (SHORT)tmpVal;
		    ti.idx += (SHORT)tmpVal;

		    /*
		    * change to distance tree if necessary
		    */
		    if (ti.idx >= ti.nLitCodes && ti.sftd != (struct SFTD *)&tmpTree->disTemp) {
			ti.sftd = (struct SFTD *)&tmpTree->disTemp;
			ti.val -= ti.nLitCodes;
		    }

		/*
		* non-zero length
		*/
		} else {

		    do {
			/* set bit length for value */
			ti.sftd->codeLens[ti.val] = (UBYTE)ti.bits;

			/* count number of codes for each length */
			ti.sftd->codeLenCounts[ti.bits-1]++;

			ti.val++, ti.idx++;

			/*
			* change to distance tree if necessary
			*/
			if (ti.idx == ti.nLitCodes && ti.sftd != (struct SFTD *)&tmpTree->disTemp) {
			    ti.sftd = (struct SFTD *)&tmpTree->disTemp;
			    ti.val = 0;
			}

		    } while (--tmpVal);
		}

	    } while (ti.idx < ti.nLitDisCodes);
	}

	/*
	* build the literal/distance trees
	*/
	if (!ti.state) {
	    if (BuildSFTree((struct SFTD *)&tmpTree->litTemp, (struct SFTT *)&ei->inflate->litTree, &ei->inflate->tmpSFT, ti.nLitCodes))
		ti.state = TREE_ERROR;
	}
	if (!ti.state) {
	    if (BuildSFTree((struct SFTD *)&tmpTree->disTemp, (struct SFTT *)&ei->inflate->disTree, &ei->inflate->tmpSFT, ti.nDisCodes))
		ti.state = TREE_ERROR;
	}
    }

    /*
    * save state if any
    */
    if (ti.state && ti.state != TREE_ERROR)
	ei->ti = ti;

    return (ti.state);
}

/**
*
* name	    BuildFixedSFTrees - Build fixed Shannon-Fano trees
*
* synopsis  BuildFixedSFTrees(ei)
*	    struct ZIP2XEXPINFO *ei;	    expand info structure
*
* purpose   Passes fixed Shannon-Fano tree description to tree builder
*
**/

VOID LOCAL BuildFixedSFTrees(ei)
struct ZIP2XEXPINFO SEGSTACK *ei;
{
    SHORT bits;

    REGISTER struct ITMPTREE *tmpTree;
    REGISTER UBYTE *codeLens;
    REGISTER SHORT i;

    /*
    * if tree not already built
    */
    if (!ei->fixed) {

	/* flag fixed trees */
	ei->fixed = TRUE;

	/* temporary tree storage */
	tmpTree = &ei->inflate->tmpTree;

	/* zero both SF tree descriptors */
	memset(&tmpTree->litTemp, 0, sizeof(tmpTree->litTemp));
	memset(&tmpTree->disTemp, 0, sizeof(tmpTree->disTemp));

	/* codes 280 to 287 are 8 bits */
	bits = 8;

	/*
	* prepare the fixed literal tree
	*/
	codeLens = tmpTree->litTemp.codeLens + DF_LITCODES;
	i = DF_LITCODES;
	do {
	    /*
	    * codes 256 to 279 are 7 bits
	    */
	    if (i == 279+1)
		bits = 7;

	    /*
	    * codes 144 to 255 are 9 bits
	    */
	    else if (i == 255+1)
		bits = 9;

	    /*
	    * codes 0 to 143 are 8 bits
	    */
	    else if (i == 143+1)
		bits = 8;

	    /* set bit length for value */
	    *--codeLens = (UBYTE)bits;

	    /* count number of codes for each length */
	    tmpTree->litTemp.codeLenCounts[bits-1]++;

	} while (--i);

	/*
	* prepare the fixed distance tree
	* all codes are 5 bits
	*/
	tmpTree->disTemp.codeLenCounts[5-1] = (i = DF_DISCODES);
	codeLens = tmpTree->disTemp.codeLens + DF_DISCODES;
	do {
	    *--codeLens = 5;
	} while (--i);

	/* build the literal/distance trees */
	BuildSFTree((struct SFTD *)&tmpTree->litTemp, (struct SFTT *)&ei->inflate->litTree, &ei->inflate->tmpSFT, DF_LITCODES);
	BuildSFTree((struct SFTD *)&tmpTree->disTemp, (struct SFTT *)&ei->inflate->disTree, &ei->inflate->tmpSFT, DF_DISCODES);
    }
}

/**
*
* name	    BuildSFTree - Build Shannon-Fano tree from descriptor
*
* synopsis  BuildSFTree(sftd, sftt, tmpsft, numCodes)
*	    struct SFTD *sftd;		    tree description (lengths)
*	    struct SFTT *sftt;		    tree storage
*	    struct TMPSFT *tmpsft;	    temporary storage
*	    SHORT numCodes;		    number of codes
*
* purpose   Builds a lookup table for all lengths 1-8 and a decode
*	    tree for lengths 9-15.
*
* returns   err = TRUE if not enough data available
*
**/

BOOL LOCAL BuildSFTree(sftd, sftt, tmpsft, numCodes)
struct SFTD *sftd;
struct SFTT *sftt;
struct TMPSFT *tmpsft;
SHORT numCodes;
{
    USHORT nextCode[DF_MAXBITS+1];
    USHORT SEGSTACK *nc;

    USHORT code1, code2, step;
    SHORT n;

    USHORT *vals, *valEnd;
    USHORT *lenCounts, *tree;
    UBYTE *codeLens, *lens;
    USHORT *tempCodes;
    BOOL err;

    REGISTER SHORT i;

    /* assume success */
    err = FALSE;

    /*
    * need at least one code
    */
    if (--numCodes >= 0) {

	/* start with zero */
	*(nc = (USHORT SEGSTACK *)nextCode) = 0;

	/*
	* compute Shannon-Fano codes for each length
	* lengths converted from 1-15 to 0-14, 16(15) tested
	*/
	lenCounts = sftd->codeLenCounts;
	i = DF_MAXBITS;
	do {
	    *(nc+1) = (USHORT)(*nc + *lenCounts) << 1;
	    nc++;
	    lenCounts++;
	} while (--i);

	do {
	    /*
	    * if we don't overflow to zero at 16 bits then
	    * the lengths are wrong unless its a single code
	    */
	    if (*nc) {

		i = DF_MAXBITS;
		n = 0;
		do {
		    n += *--lenCounts;
		} while (--i);

		if (n > 1) {
		    err = TRUE;
		    continue;
		}
	    }

	    /* zero values table */
	    memset(sftt->vals, 0, sizeof(sftt->vals));
	    valEnd = sftt->vals + (1<<UBYTE_BITS);

	    /*
	    * we initialize lens table to 8 bits in case
	    * the compressed data stream gets corrupt
	    */
	    memset(sftt->lens, UBYTE_BITS, sizeof(sftt->lens));

	    /*
	    * increment/reverse codes within each length
	    */
	    tempCodes = tmpsft->tempCodes;
	    codeLens = sftd->codeLens;
	    n = numCodes;
	    do {
		/*
		* non-zero codes only
		*/
		if (i = *codeLens++) {

		    /* convert 1-15 to 0-14 and finish code */
		    code1 = nextCode[--i]++;
		    code2 = 0;

		    /*
		    * reverse code
		    */
		    do {
			code2 <<= 1;
			code2 |= code1 & 1;
			code1 >>= 1;
		    } while (--i >= 0);

		    *tempCodes++ = code2;
		}

	    } while (--n >= 0);

	    tree = sftt->tree;
	    do {
		/*
		* non-zero codes only
		*/
		if (i = *--codeLens) {

		    code2 = *--tempCodes;

		    /*
		    * codes up to 8 bits fit lookup table
		    */
		    vals = sftt->vals + (UBYTE)code2;
		    if (i < 9) {

			lens = sftt->lens + code2;
			step = ((USHORT)1 << i);

			/*
			* add value to table for code and all other
			* codes with the same prefix (up to 8 bits)
			* this allows a byte lookup with extraneous bits
			*/
			do {
			    /* value and length */
			    *lens = (UBYTE)i;
			    lens += step;
			    *vals = numCodes;
			    vals += step;

			} while (vals < valEnd);

		    /*
		    * codes 9 bits and larger
		    */
		    } else {

			i -= 8;
			do {
			    if (*vals)
				vals = sftt->tree + (USHORT)~(*vals);
			    else {
				*vals = (USHORT)~(USHORT)(tree - sftt->tree);
				vals = tree;
				*tree++ = 0;
				*tree++ = 0;
			    }

			    if ((code2 >>= 1) & 0x80)
				vals++;

			} while (--i);

			*vals = numCodes;
		    }
		}

	    } while (--numCodes >= 0);

	} while (0);
    }

    return (err);
}

/**
*
* name	    Decode - extract value from Shannon-Fano tree
*
* synopsis  state = Decode(bi, tree, rValue)
*	    struct BITINFO *bi; 	    bit info structure
*	    USHORT *tree;		    Shannon-Fano coding tree
*	    USHORT *rValue;		    code/storage for value
*
* purpose   Walks coding tree until next code found. Returns
*	    value represented by that code.
*
* returns   state = DECODE_DEFAULT if successful
*		  = DECODE_PEEK if out of data
*		  = DECODE_ERROR otherwise
*
**/

SHORT LOCAL Decode(bi, tree, rValue)
struct BITINFO SEGSTACK *bi;
USHORT *tree;
USHORT *rValue;
{
    UINT SEGDATA tmpVal;
    SHORT state;

    REGISTER USHORT bits;
    REGISTER USHORT code;
    REGISTER UBYTE bitCnt;

    /* assumed state */
    state = DECODE_PEEK;

    /* fetch code prefix */
    code = *rValue;

    /*
    * fetch 7 bits from input stream
    */
    if (!BitPeekQuick7(bi, &tmpVal)) {

	/* assume success */
	state = DECODE_DEFAULT;

	bits = (USHORT)tmpVal | 0x80;
	bitCnt = 0;

	for (;;) {

	    /* one more bit */
	    bitCnt++;

	    /*
	    * follow left/right branches
	    */
	    if ((SHORT)(code = tree[(USHORT)~(code | 1) | (bits & 1)]) >= 0) {
		*rValue = code;
		BitSkipQuick(bi, bitCnt);
		break;
	    }

	    /*
	    * bad data if more than 7 bits read
	    */
	    if ((bits >>= 1) == 1) {
		state = DECODE_ERROR;
		break;
	    }
	}
    }

    return (state);
}

