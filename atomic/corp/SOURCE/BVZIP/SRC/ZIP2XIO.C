/**

    ZZZZZ   IIIII   PPPP     222    X	X   IIIII    OOO	CCCC
       Z      I     P	P   2	2    X X      I     O	O      C
      Z       I     PPPP       2      X       I     O	O      C
     Z	      I     P	      2      X X      I     O	O      C
    ZZZZZ   IIIII   P	    22222   X	X   IIIII    OOO    *	CCCC

    Description: ZIP 2.x bit/buffer read/write routines

    Product: Central Point 'PC TOOLS'

    CONFIDENTIAL and PROPRIETARY
    (c) Copyright 1992,93,94 Quintessence Corporation
    (c) Copyright 1993,94 Central Point Software
    All rights reserved.

    Revision History:

    Version	Date	    Author	Comments
    -------	--------    ------	--------------------------------
    1.00	10-01-92    SJP 	created this file
    2.50	07-05-93    SJP 	changed to fit CPS requirements
    2.75	06-13-94    SJP 	new Skip() and many speed
					improvements
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

#if FLAT32

/*
* table of bit masks 0-32 bits
*/
UINT NEAR bitMask[] = {
    0x00000000,
    0x00000001,
    0x00000003,
    0x00000007,
    0x0000000F,
    0x0000001F,
    0x0000003F,
    0x0000007F,
    0x000000FF,
    0x000001FF,
    0x000003FF,
    0x000007FF,
    0x00000FFF,
    0x00001FFF,
    0x00003FFF,
    0x00007FFF,
    0x0000FFFF,
    0x0001FFFF,
    0x0003FFFF,
    0x0007FFFF,
    0x000FFFFF,
    0x001FFFFF,
    0x003FFFFF,
    0x007FFFFF,
    0x00FFFFFF,
    0x01FFFFFF,
    0x03FFFFFF,
    0x07FFFFFF,
    0x0FFFFFFF,
    0x1FFFFFFF,
    0x3FFFFFFF,
    0x7FFFFFFF,
    0xFFFFFFFF,
};

#else

/*
* table of bit masks 0-16 bits
*/
UINT NEAR bitMask[] = {
    0x0000,
    0x0001,
    0x0003,
    0x0007,
    0x000F,
    0x001F,
    0x003F,
    0x007F,
    0x00FF,
    0x01FF,
    0x03FF,
    0x07FF,
    0x0FFF,
    0x1FFF,
    0x3FFF,
    0x7FFF,
    0xFFFF,
};

#endif

/**
*
* name	    BitReadInit - initialize for bit reading
*
* synopsis  BitReadInit(bi)
*	    struct BITINFO *bi		    bit info structure
*
* purpose   Initializes bit read operations
*
**/

VOID BitReadInit(bi)
struct BITINFO FAR *bi;
{
    /* zero structure */
    FMEMSET(bi, 0, sizeof(struct BITINFO));

    /* signal no bits read */
    bi->bitCnt = UINT_BITS;
}

/**
*
* name	    BitReadSetup - setup bit read buffer
*
* synopsis  size = BitReadSetup(bi, inputBuf, inputSize, eof)
*	    struct BITINFO *bi		    bit info structure
*	    UBYTE *inputBuf		    input buffer
*	    UINT inputSize		    input buffer size
*	    BOOL eof			    input at EOF flag
*
* purpose   Setup for bit read operations with specified buffer
*
* returns   size = used buffer size if 'inputBuf' NULL
*		 = otherwise available input buffer
*
**/

UINT BitReadSetup(bi, inputBuf, inputSize, eof)
struct BITINFO SEGSTACK *bi;
UBYTE FAR *inputBuf;
UINT inputSize;
BOOL eof;
{
    UINT size;

    /*
    * setup I/O for new buffer
    */
    size = inputSize;
    if (inputBuf != NULL) {
	bi->bufEnd = (bi->bufPtr = bi->buffer = inputBuf) + size;
	bi->eof = (UBYTE)eof;

    /*
    * amount of buffer we used
    */
    } else {
	/* size in multiples of a word */
	size = bi->bufPtr - bi->buffer;

	/*
	* if 'inputSize' flag then we need to be exact
	*/
	if ((BOOL)inputSize && bi->bitCnt != UINT_BITS) {

	    /*
	    * if not within high bytes
	    */
	    #if FLAT32
	    if (bi->bitCnt <= (UBYTE_BITS*3))
		size--;
	    if (bi->bitCnt <= (UBYTE_BITS*2))
		size--;
	    #endif
	    if (bi->bitCnt <= UBYTE_BITS)
		size--;
	}
    }

    return (size);
}

/**
*
* name	    BitSkip - Skip 'bitCnt' bits on input stream
*
* synopsis  BitSkip(bi, bitCnt)
*	    struct BITINFO *bi		    bit info structure
*	    UBYTE bitCnt		    how many bits to skip
*
* purpose   Skips specified number of bits
*
* returns   errCode = error result code
*
**/

VOID BitSkip(bi, bitCnt)
struct BITINFO SEGSTACK *bi;
UBYTE bitCnt;
{
    /* var used for better optimiztion */
    REGISTER INT newCnt;

    /*
    * if not enough read-ahead
    */
    newCnt = bitCnt;
    if ((bi->bitCnt += bitCnt) > UINT_BITS) {
	bi->last = *(UINT FAR *)bi->bufPtr;
	bi->bufPtr += sizeof(UINT);
	newCnt = (bi->bitCnt -= UINT_BITS);
    }

    /* less read-ahead */
    bi->last >>= newCnt;
}

/**
*
* name	    BitRead - Read 'bitCnt' bits into 'bitPtr'
*
* synopsis  err = BitRead(bi, bitPtr, bitCnt)
*	    struct BITINFO *bi		    bit info structure
*	    UINT *bitPtr		    buffer that will contain the read in bits
*	    UBYTE bitCnt		    how many bits to read
*
* purpose   Read specified number of bits
*
* returns   err = TRUE if out of data
*
**/

BOOL BitRead(bi, bitPtr, bitCnt)
struct BITINFO SEGSTACK *bi;
UINT *bitPtr;
UBYTE bitCnt;
{
    BOOL err;

    /* var used for better optimiztion */
    REGISTER INT newCnt;

    /* assume success */
    err = FALSE;

    /*
    * if not enough read-ahead
    */
    newCnt = bitCnt;
    if ((bi->bitCnt += bitCnt) > UINT_BITS) {

	/*
	* if more queued
	*/
	if (bi->bufPtr >= bi->bufEnd) {
	    bi->bitCnt -= bitCnt;
	    err = TRUE;
	} else {
	    *bitPtr = ((*(UINT FAR *)bi->bufPtr << (UBYTE)(newCnt - bi->bitCnt + UINT_BITS)) | bi->last) & bitMask[newCnt];
	    bi->last = *(UINT FAR *)bi->bufPtr >> (bi->bitCnt -= UINT_BITS);
	    bi->bufPtr += sizeof(UINT);
	}

    /*
    * mask read-ahead bits
    */
    } else {
	*bitPtr = bi->last & bitMask[newCnt];
	bi->last >>= newCnt;
    }

    return (err);
}

/**
*
* name	    BitReadBytes - read bytes after byte aligning
*
* synopsis  err = BitReadBytes(bi, buffer, size)
*	    struct BITINFO *bi		    bit info structure
*	    UBYTE *buffer		    buffer to put input bytes
*	    INT size			    number of bytes to read
*
* purpose   To read 'size' bytes into 'buffer' from the input file
*
* returns   err = TRUE if out of data
*
**/

BOOL BitReadBytes(bi, buffer, size)
struct BITINFO SEGSTACK *bi;
UBYTE FAR *buffer;
INT size;
{
    UINT uSize;
    BOOL err;

    /* assume success */
    err = FALSE;

    /*
    * byte align
    */
    if (bi->bitCnt != UINT_BITS) {

	/*
	* if not within high bytes
	*/
	#if FLAT32
	if (bi->bitCnt <= (UBYTE_BITS*3))
	    bi->bufPtr--;
	if (bi->bitCnt <= (UBYTE_BITS*2))
	    bi->bufPtr--;
	#endif
	if (bi->bitCnt <= UBYTE_BITS)
	    bi->bufPtr--;


	/* no bits read */
	bi->bitCnt = UINT_BITS;
	bi->last = 0;
    }

    /*
    * if not enough to fill request
    */
    if ((UINT)(bi->bufEnd - bi->bufPtr) < (UINT)size)
	err = TRUE;

    /*
    * copy data
    */
    else {
	/* copy from buffer */
	FMEMCPY(buffer, bi->bufPtr, size);
	bi->bufPtr += size;

	/*
	* correct for odd sizes
	*/
	if ((uSize = ~(bi->bufPtr - bi->buffer - 1)) & 1) {
	    bi->last = *bi->bufPtr++;
	    bi->bitCnt -= UBYTE_BITS;
	}
	#if FLAT32
	if (uSize & 2) {
	    bi->last |= *((USHORT *)bi->bufPtr)++ << (UBYTE)(UINT_BITS - bi->bitCnt);
	    bi->bitCnt -= (UBYTE_BITS*2);
	}
	#endif
    }

    return (err);
}

/**
*
* name	    BitWriteInit - initialize for bit writing
*
* synopsis  BitWriteInit(bi)
*	    struct BITINFO *bi		    bit info structure
*
* purpose   Initializes bit write operations
*
**/

VOID BitWriteInit(bi)
struct BITINFO FAR *bi;
{
    /* initialize structure */
    FMEMSET(bi, 0, sizeof(struct BITINFO));
}

/**
*
* name	    BitWriteSetup - setup bit write buffer
*
* synopsis  size = BitWriteSetup(bi, outputBuf, outputSize)
*	    struct BITINFO *bi		    bit info structure
*	    UBYTE *outputBuf		    output buffer
*	    UINT outputSize		    output buffer size
*
* purpose   Setup for bit write operations with specified buffer
*
* returns   size = used buffer size if 'outputBuf' NULL
*		 = otherwise available output buffer
*
**/

UINT BitWriteSetup(bi, outputBuf, outputSize)
struct BITINFO SEGSTACK *bi;
UBYTE FAR *outputBuf;
UINT outputSize;
{
    UINT size;

    /*
    * setup I/O for new buffer
    * must keep 1 word spare
    */
    size = outputSize - sizeof(UINT);
    if (outputBuf != NULL)
	bi->bufEnd = (bi->bufPtr = bi->buffer = outputBuf) + size;

    /*
    * amount of buffer we used
    */
    else {
	/* size in multiples of a word */
	size = bi->bufPtr - bi->buffer;

	/*
	* if 'outputSize' flag then we need to be exact
	*/
	if ((BOOL)outputSize && bi->bitCnt) {

	    /*
	    * move last word to output
	    * output buffer needs to be 1 word larger
	    */
	    *(UINT FAR *)bi->bufPtr = bi->last;

	    /*
	    * at least one, more if within high bytes
	    */
	    size++;
	    #if FLAT32
	    if (bi->bitCnt > (UBYTE_BITS*3))
		size++;
	    if (bi->bitCnt > (UBYTE_BITS*2))
		size++;
	    #endif
	    if (bi->bitCnt > UBYTE_BITS)
		size++;
	}
    }

    return (size);
}

/**
*
* name	    BitWrite - Write 'bitCnt' bits from 'bitPtr'
*
* synopsis  err = BitWrite(bi, bitPtr, bitCnt)
*	    struct BITINFO *bi		    bit info structure
*	    UINT *bitPtr;		    source buffer
*	    UBYTE bitCnt;		    bit count
*
* purpose   Writes specified number of bits
*
* returns   err = TRUE if no more room
*
**/

BOOL BitWrite(bi, bitPtr, bitCnt)
struct BITINFO SEGSTACK *bi;
UINT *bitPtr;
UBYTE bitCnt;
{
    #if FLAT32
    INT n;	/* 32 bit compiler bug */
    #endif
    BOOL err;

    /* var used for better optimiztion */
    REGISTER INT oldCnt;

    /* assume success */
    err = FALSE;

    oldCnt = bi->bitCnt;

    /* avoid 32 bit compiler bug */
    #if FLAT32
    n = bi->bitCnt + bitCnt;
    bi->bitCnt = (UBYTE)n;
    if (n >= UINT_BITS) {
    #else

    /*
    * if not enough write-ahead
    */
    if ((bi->bitCnt += bitCnt) >= UINT_BITS) {
    #endif

	/*
	* if more can be buffered
	*/
	if (bi->bufPtr >= bi->bufEnd) {
	    bi->bitCnt = (UBYTE)oldCnt;
	    err = TRUE;
	} else {
	    *(UINT FAR *)bi->bufPtr = bi->last | (*bitPtr << oldCnt);
	    bi->last = (*bitPtr >> (UBYTE)(UINT_BITS - oldCnt));
	    bi->bufPtr += sizeof(UINT);
	    bi->bitCnt -= UINT_BITS;
	}

    /*
    * less write-ahead
    */
    } else
	bi->last |= *bitPtr << oldCnt;

    return (err);
}

/**
*
* name	    BitWriteBytes - write bytes after byte aligning
*
* synopsis  err = BufWriteBytes(bi, buffer, size)
*	    struct BITINFO *bi		    bit info structure
*	    UBYTE *buffer		    buffer of bytes to output
*	    INT size			    number of bytes to write
*
* purpose   To write 'size' bytes from 'buffer' to the output file
*
* returns   err = TRUE if no more room
*
**/

BOOL BitWriteBytes(bi, buffer, size)
struct BITINFO SEGSTACK *bi;
UBYTE FAR *buffer;
INT size;
{
    UINT uSize;
    BOOL err;

    /* assume success */
    err = FALSE;

    /*
    * byte align
    */
    if (bi->bitCnt) {

	/*
	* move last word to output
	* output buffer needs to be 1 word larger
	*/
	*(UINT FAR *)bi->bufPtr = bi->last;

	/*
	* at least one, more if within high bytes
	*/
	bi->bufPtr++;
	#if FLAT32
	if (bi->bitCnt > (UBYTE_BITS*3))
	    bi->bufPtr++;
	if (bi->bitCnt > (UBYTE_BITS*2))
	    bi->bufPtr++;
	#endif
	if (bi->bitCnt > UBYTE_BITS)
	    bi->bufPtr++;

	/* no bits written */
	bi->last = bi->bitCnt = 0;
    }

    /*
    * if not enough room
    */
    if ((UINT)(bi->bufEnd - bi->bufPtr) < (UINT)size)
	err = TRUE;

    /*
    * copy data
    */
    else {
	/* copy bytes to output buffer */
	FMEMCPY(bi->bufPtr, buffer, size);
	bi->bufPtr += size;

	/*
	* correct for odd sizes
	*/
	if ((uSize = bi->bufPtr - bi->buffer) & 1) {
	    bi->last = *--bi->bufPtr;
	    bi->bitCnt += UBYTE_BITS;
	}
	#if FLAT32
	if (uSize & 2) {
	    bi->last = (bi->last << 16) | *--((USHORT *)bi->bufPtr);
	    bi->bitCnt += (UBYTE_BITS*2);
	}
	#endif
    }

    return (err);
}

/**
*
* name	    BufReadInit - initialize for buffered reading
*
* synopsis  err = BufReadInit(bi)
*	    struct BUFINFO *bf		    buffer info structure
*
* purpose   Initializes buffered read operations
*
**/

VOID BufReadInit(bf)
struct BUFINFO FAR *bf;
{
    /* initialize structure */
    FMEMSET(bf, 0, sizeof(struct BUFINFO));
}

/**
*
* name	    BufReadSetup - setup buffer for reading
*
* synopsis  size = BufReadSetup(bf, inputBuf, inputSize, eof)
*	    struct BUFINFO *bf		    buffer info structure
*	    UBYTE *inputBuf		    input buffer
*	    UINT inputSize		  input buffer size
*	    UBYTE eof			    input at EOF flag
*
* purpose   Setup for read operations with specified buffer
*
* returns   size = used buffer size if 'inputBuf' NULL
*		 = otherwise available input buffer
*
**/

UINT BufReadSetup(bf, inputBuf, inputSize, eof)
struct BUFINFO SEGSTACK *bf;
UBYTE FAR *inputBuf;
UINT inputSize;
BOOL eof;
{
    UINT size;

    /*
    * setup I/O for new buffer
    */
    size = inputSize;
    if (inputBuf != NULL) {
	bf->bufEnd = (bf->bufPtr = bf->buffer = inputBuf) + size;
	bf->eof = (UBYTE)eof;

    /*
    * amount of buffer we used
    */
    } else
	size = bf->bufPtr - bf->buffer;

    return (size);
}

/**
*
* name	    BufWriteInit - initialize for buffered writing
*
* synopsis  err = BufWriteInit(bi)
*	    struct BUFINFO *bf		    buffer info structure
*
* purpose   Initializes buffered write operations
*
**/

VOID BufWriteInit(bf)
struct BUFINFO FAR *bf;
{
    /* initialize structure */
    FMEMSET(bf, 0, sizeof(struct BUFINFO));
}

/**
*
* name	    BufWriteSetup - setup buffer for writing
*
* synopsis  size = BufWriteSetup(bf, outputBuf, outputSize)
*	    struct BUFINFO *bf		    buffer info structure
*	    UBYTE *outputBuf		    output buffer
*	    UINT outputSize		    output buffer size
*
* purpose   Setup for write operations with specified buffer
*
* returns   size = used buffer size if 'outputBuf' NULL
*		 = otherwise available output buffer
*
**/

UINT BufWriteSetup(bf, outputBuf, outputSize)
struct BUFINFO SEGSTACK *bf;
UBYTE FAR *outputBuf;
UINT outputSize;
{
    UINT size;

    /*
    * setup I/O for new buffer
    */
    size = outputSize;
    if (outputBuf != NULL)
	bf->bufEnd = (bf->bufPtr = bf->buffer = outputBuf) + size;

    /*
    * amount of buffer we used
    */
    else
	size = bf->bufPtr - bf->buffer;

    return (size);
}

/**
*
* name	    BufWriteStr - move 'size' bytes to output buffer
*
* synopsis  err = BufWriteStr(bf, buffer, size)
*	    struct BUFINFO *bf		    buffer info structure
*	    CHAR *buffer		    output buffer
*	    INT size			    how many bytes to move
*
* purpose   Move specified number of bytes to output buffer
*
* returns   err = TRUE if end of buffer encountered
*
**/

BOOL BufWriteStr(bf, buffer, size)
struct BUFINFO SEGSTACK *bf;
UBYTE FAR *buffer;
INT size;
{
    BOOL err;

    /* assume success */
    err = FALSE;

    if ((UINT)(bf->bufEnd - bf->bufPtr) < (UINT)size)
	err = TRUE;
    else {
	FMEMCPY(bf->bufPtr, buffer, size);
	bf->bufPtr += size;
    }

    return (err);
}

