/**

    ZZZZZ   IIIII   PPPP     222    X	X    CCCC    OOO    DDDD	CCCC
       Z      I     P	P   2	2    X X    C	    O	O   D	D      C
      Z       I     PPPP       2      X     C	    O	O   D	D      C
     Z	      I     P	      2      X X    C	    O	O   D	D      C
    ZZZZZ   IIIII   P	    22222   X	X    CCCC    OOO    DDDD    *	CCCC

    Description: ZIP 2.x compression support (code generation)

    Product: Central Point 'PC TOOLS'

    CONFIDENTIAL and PROPRIETARY
    (c) Copyright 1992,93,94 Quintessence Corporation
    (c) Copyright 1993,94 Central Point Software
    All rights reserved.

    Revision History:

    Version	Date	    Author	Comments
    -------	--------    ------	--------------------------------
    2.20	04-21-93    SJP 	created this file
    2.50	07-05-93    SJP 	changed to fit CPS requirements
    2.75	06-08-94    SJP 	completly re-written, fixed
					scaling bug, and M-V Huffman
					also new method for S-F codes
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

#if FLAT32
#define SCALE_C 1
#else
#define SCALE_C 0
#endif

/**
*
* name	    BuildHuffCodes - build Huffman codes given the frequencies
*
* synopsis  BuildHuffCodes(codeTable, probCounts, tmpTree, size, bits)
*	    struct CODETABLE *codeTable     table of Huffman codes that is built
*	    SHORT *probCounts		    probability counts
*	    struct DTMPTREE *tmpTree	    temporary tree storage
*	    SHORT size			    number of counts
*	    SHORT bits			    maximum bit length
*
* purpose   Builds Huffman codes given the frequencies
*
**/

VOID BuildHuffCodes(codeTable, probCounts, tmpTree, size, bits)
struct CODETABLE *codeTable;
SHORT *probCounts;
struct DTMPTREE *tmpTree;
SHORT size;
SHORT bits;
{
    /*
    * modified Fibonacci sequence for minimum variability Huffman
    * 1,1,1,3,4,7... vs 1,1,1,2,3...
    *
    * sums are 1 less than next in series and we need to stay
    * 1 less than that
    */
    STATIC SHORT fib[] = {3-2,4-2,7-2,11-2,18-2,29-2,47-2,76-2,123-2,199-2,322-2,521-2,843-2,1364-2,2207-2,3571-2};

    SHORT codeLenCounts[DF_MAXBITS];
    SHORT heapSize;
    SHORT scale, sum;
    SHORT newSum;

    REGISTER SHORT i, j, k, l, t;

    /*
    * if something to do
    */
    if (size) {

	/* start without scaling */
	scale = sum = 1;

	/*
	* this may take two passes
	*/
	for (;;) {

	    /*
	    * until we fit the scale
	    */
	    k = -1;
	    for (;;) {

		/*
		* scale counts and initialize heap index
		*/
		newSum = heapSize = 0;
		l = 0;
		do {
		    if (probCounts[l]) {
			newSum += (tmpTree->count[heapSize] = ComputeScale(probCounts[l], scale, sum));
			tmpTree->depth[heapSize] = 0;
			tmpTree->heap[heapSize] = heapSize++;
			if (k < 0)
			    k = l;
		    }
		} while (++l < size);

		/*
		* stop if not scaling or we now fit
		*/
		if (scale == 1 || (newSum -= fib[bits]) <= 0)
		    break;

		/*
		* correct scale for arithmetic round-up
		* first attempt to subtract overflow from scale
		* if wild overflow set scale to fib/size
		*/
		t = (SHORT)(fib[bits] / heapSize);
		if ((scale -= newSum + (newSum >> 1)) < t)
		    scale = t;

	    }

	    /*
	    * if something to do
	    */
	    if (heapSize) {

		/*
		* need at least two entries
		*/
		if (heapSize == 1)
		    codeTable[k].bits = 1;

		else {
		    /*
		    * initialize heap
		    */
		    l = (heapSize >> 1) + 1;  /* odd sizes */
		    while (--l >= 0)
			MoveDownHeap(tmpTree, l, heapSize);

		    /*
		    * while queue has at least two entries
		    */
		    i = heapSize + (l = heapSize);
		    do {
			/* save least-freq entry */
			j = tmpTree->heap[0];

			/* take out least-freq entry */
			tmpTree->heap[0] = tmpTree->heap[--l];
			MoveDownHeap(tmpTree, 0, l);

			/* next least-freq entry */
			k = tmpTree->heap[0];

			/* get larger depth */
			if ((t = tmpTree->depth[j]) < tmpTree->depth[k]) t = tmpTree->depth[k];

			/* generate new node */
			tmpTree->depth[--i] = t + 1;
			tmpTree->count[i] = tmpTree->count[j] + tmpTree->count[k];
			tmpTree->dad[j] = tmpTree->dad[k] = i;

			/* put back new entry */
			tmpTree->heap[0] = i;
			MoveDownHeap(tmpTree, 0, l);

		    } while (l > 1);
		    tmpTree->dad[i] = 0;

		    /* zero code length counts */
		    FMEMSET(codeLenCounts, 0, sizeof(codeLenCounts));

		    /*
		    * generate code lengths for values
		    */
		    l = k = 0;
		    do {
			if (probCounts[l]) {

			    /*
			    * walk tree to determine lengths
			    */
			    t = tmpTree->dad[k++];
			    i = 0;
			    do {
				i++;
			    } while (t = tmpTree->dad[t]);

			    /*
			    * if we overflow then frequencies
			    * must be a Fibonacci sequence
			    */
			    if (i > bits)
				break;

			    codeTable[l].bits = (UBYTE)i;
			    codeLenCounts[i-1]++;
			}
		    } while (++l < size);

		    /*
		    * if we overflow then frequencies
		    * must be a Fibonacci sequence
		    */
		    if (i > bits) {

			/* scale frequencies */
			scale = fib[bits];
			sum = newSum;

			/* do this all over again */
			continue;
		    }

		    /* generate Shannon-Fanno codes from lengths */
		    BuildSFCodes(codeLenCounts, codeTable, size);
		}
	    }

	    /* exit by default */
	    break;
	}
    }
}

/**
*
* name	    MoveDownHeap - move heap down until node smaller
*
* synopsis  MoveDownHeap(tmpTree, k, hs)
*	    struct DTMPTREE *tmpTree	    temporary tree storage
*	    SHORT k;			    starting node
*	    SHORT hs;			    heap size
*
* purpose   This procedure moves down the heap, exchanging the
*	    node at position k with the smaller of its two
*	    children if necessary and stopping when the node
*	    at k is smaller than both its children or the bottom
*	    is reached. Tree depth is used as a tie-breaker so
*	    minimum variability Huffman coding is established.
*
**/

VOID LOCAL MoveDownHeap(tmpTree, k, hs)
struct DTMPTREE *tmpTree;
SHORT k;
SHORT hs;
{
    REGISTER SHORT j, v, t, tt, n;

    /*
    * if something to do
    */
    if ((j = k * 2 + 1) < hs) {

	v = tmpTree->heap[k];

	do {
	    t = tmpTree->heap[j];

	    if (j+1 < hs && ((n = tmpTree->count[tt = tmpTree->heap[j+1]] - tmpTree->count[t]) < 0 || (!n && tmpTree->depth[tt] < tmpTree->depth[t])))
		t = tt, j++;
	    if (((n = tmpTree->count[t] - tmpTree->count[v]) > 0 || (!n && tmpTree->depth[t] >= tmpTree->depth[v])))
		break;

	    tmpTree->heap[k] = t;
	    k = j;

	    j *= 2;
	    j++;

	} while (j < hs);

	tmpTree->heap[k] = v;
    }
}

/**
* name	    BuildSFCodes - build Shannon/Fanno codes from Huffman lengths
*
* synopsis  BuildSFCodes(codeLenCounts, codeTable, size)
*	    SHORT *codeLenCounts	    Huffman code length counts
*	    struct CODETABLE *codeTable     shannon-fano code table
*	    SHORT size			    number of codes
*
* purpose   Builds Shannon/Fanno codes from Huffman code lengths
*
**/

VOID LOCAL BuildSFCodes(codeLenCounts, codeTable, size)
SHORT SEGSTACK *codeLenCounts;
struct CODETABLE *codeTable;
SHORT size;
{
    USHORT nextCode[DF_MAXBITS];
    USHORT SEGSTACK *nc;
    USHORT code1, code2;

    REGISTER SHORT i;

    /* start with zero */
    *(nc = (USHORT SEGSTACK *)nextCode) = 0;

    /*
    * compute Shannon-Fano codes for each length
    * lengths converted from 1-15 to 0-14
    */
    i = DF_MAXBITS-1;
    do {
	*(nc+1) = (USHORT)(*nc + *codeLenCounts) << 1;
	nc++;
	codeLenCounts++;
    } while (--i);

    /*
    * increment/reverse codes within each length
    */
    do {
	/*
	* non-zero codes only
	*/
	if (i = codeTable->bits) {

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

	    codeTable->code = code2;
	}

	codeTable++;

    } while (--size);
}

#if SCALE_C

/*
* ComputeScale - scale frequencies for Huffman encoding
*/
SHORT INLINE LOCAL ComputeScale(num1, num2, num3)
SHORT num1;
SHORT num2;
SHORT num3;
{
    /* this uses 32 bit divide/multiply helper functions */
    return ((SHORT)(((LONG)num1 * num2 + (num3 - 1)) / num3));
}

#else

#pragma warning(disable:4035)
#pragma optimize("egl", off)

/*
* ComputeScale - scale frequencies for Huffman encoding
*/
SHORT INLINE LOCAL ComputeScale(num1, num2, num3)
SHORT num1;
SHORT num2;
SHORT num3;
{
    _asm
    {
	mov	ax,word ptr [num1]	; 1st number
	mov	si,word ptr [num2]	; 2nd number
	mul	si			; dx:ax = num1 * num2
	mov	si,word ptr [num3]	; 3rd number
	dec	si			; num3 - 1
	add	ax,si
	adc	dx,0			; dx:ax = (num1 * num2) + (num3 - 1)
	inc	si
	div	si			; dx:ax = result
    }
}

#pragma optimize("", on)

#endif

