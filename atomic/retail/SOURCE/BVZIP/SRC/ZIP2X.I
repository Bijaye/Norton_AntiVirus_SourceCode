/**

    ZZZZZ   IIIII   PPPP     222    X	X      IIIII
       Z      I     P	P   2	2    X X	 I
      Z       I     PPPP       2      X 	 I
     Z	      I     P	      2      X X	 I
    ZZZZZ   IIIII   P	    22222   X	X   *  IIIII

    Description: Inline functions for ZIP 2.x compression library

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
    2.75	06-13-94    SJP 	new Peek()/Skip() and many speed
					improvments
    2.76	10-16-94    SJP 	32 bit FLAT model support

**/

/* bit mask table */
IMPORT UINT NEAR bitMask[];

/**
*
* name	    BitPeekQuick8 - Peeks 8 bits on input stream
*
* synopsis  err = BitPeekQuick8(bi, bitPtr)
*	    struct BITINFO *bi		    bit info structure
*	    UINT *bitPtr		    buffer that will contain the read in bits
*
* purpose   Peeks 8 bits quickly
*
* returns   err = TRUE if out of data
*
**/

BOOL INLINE BitPeekQuick8(bi, bitPtr)
struct BITINFO SEGSTACK *bi;
UINT *bitPtr;
{
    BOOL err;

    /* assume success */
    err = FALSE;

    /*
    * queue more bits if necessary
    */
    *bitPtr = (UBYTE)bi->last;
    if (bi->bitCnt > (UINT_BITS-8)) {
	if (bi->bufPtr >= bi->bufEnd)
	    err = !bi->eof;
	else
	    *bitPtr |= (UBYTE)(*bi->bufPtr << (UINT_BITS - bi->bitCnt));
    }

    return (err);
}

/**
*
* name	    BitPeekQuick7 - Peeks 7 bits on input stream
*
* synopsis  err = BitPeekQuick7(bi, bitPtr)
*	    struct BITINFO *bi		    bit info structure
*	    UINT *bitPtr		    buffer that will contain the read in bits
*
* purpose   Peeks 7 bits quickly (db8 garbage)
*
* returns   err = TRUE if out of data
*
**/

BOOL INLINE BitPeekQuick7(bi, bitPtr)
struct BITINFO SEGSTACK *bi;
UINT *bitPtr;
{
    BOOL err;

    /* assume success */
    err = FALSE;

    /*
    * queue more bits if necessary
    */
    *bitPtr = (UBYTE)bi->last;
    if (bi->bitCnt > (UINT_BITS-7)) {
	if (bi->bufPtr >= bi->bufEnd)
	    err = !bi->eof;
	else
	    *bitPtr |= (UBYTE)(*bi->bufPtr << (UINT_BITS - bi->bitCnt));
    }

    return (err);
}

/**
*
* name	    BitSkipQuick - Skip 'bitCnt' bits on input stream
*
* synopsis  BitSkipQuick(bi, bitCnt)
*	    struct BITINFO *bi		    bit info structure
*	    UBYTE bitCnt		    how many bits to skip
*
* purpose   Skips specified number of bits quickly
*
**/

VOID INLINE BitSkipQuick(bi, bitCnt)
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
* name	    BitReadQuick - Read 'bitCnt' bits into 'bitPtr'
*
* synopsis  err = BitReadQuick(bi, bitPtr, bitCnt)
*	    struct BITINFO *bi		    bit info structure
*	    UINT *bitPtr		    buffer that will contain the read in bits
*	    UBYTE bitCnt		    how many bits to read
*
* purpose   Read specified number of bits quickly
*
* returns   err = TRUE if out of data
*
**/

BOOL INLINE BitReadQuick(bi, bitPtr, bitCnt)
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
* name	    BitWriteQuick - Write 'bitCnt' bits from 'bitPtr'
*
* synopsis  err = BitWriteQuick(bi, bitPtr, bitCnt)
*	    struct BITINFO *bi		    bit info structure
*	    UINT *bitPtr;		    source buffer
*	    UBYTE bitCnt		    bit count
*
* purpose   Write specified number of bits
*
* returns   err = TRUE if end of buffer
*
**/

BOOL INLINE BitWriteQuick(bi, bitPtr, bitCnt)
struct BITINFO SEGSTACK *bi;
UINT *bitPtr;
UBYTE bitCnt;
{
    BOOL err;

    /* var used for better optimiztion */
    REGISTER INT oldCnt;

    /* assume success */
    err = FALSE;

    /*
    * if not enough write-ahead
    */
    oldCnt = bi->bitCnt;
    if ((bi->bitCnt += bitCnt) >= UINT_BITS) {

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
* name	    BufReadQuick - read character from input buffer
*
* synopsis  err = BufReadQuick(bf, data)
*	    struct BUFINFO *bf		    buffer info structure
*	    UBYTE *data 		    storage for character
*
* purpose   Moves character from input buffer
*
* returns   err = TRUE if no more data available
*
**/

BOOL INLINE BufReadQuick(bf, data)
struct BUFINFO SEGSTACK *bf;
UBYTE FAR *data;
{
    BOOL err;

    /* assume success */
    err = FALSE;

    if (bf->bufPtr >= bf->bufEnd)
	err = TRUE;
    else {
	*data = *bf->bufPtr;
	bf->bufPtr++;
    }

    return (err);
}

/**
*
* name	    BufReadStrQuick - read string from input buffer
*
* synopsis  err = BufReadStrQuick(bf, str, len, rLen)
*	    struct BUFINFO *bf		    buffer info structure
*	    UBYTE *str			    storage for string
*	    INT len			    requested string length
*	    INT *rLen			    storage for actual length
*
* purpose   Moves string from input buffer
*
* returns   err = TRUE if no more data available
*
**/

BOOL INLINE BufReadStrQuick(bf, str, len, rLen)
struct BUFINFO SEGSTACK *bf;
UBYTE FAR *str;
INT len;
INT *rLen;
{
    UINT size;
    BOOL err;

    /* assume success */
    err = FALSE;

    /*
    * if not enough to read
    */
    if ((size = bf->bufEnd - bf->bufPtr) < (UINT)len && !bf->eof)
	err = TRUE;

    else {
	/*
	* read smaller of remaining and requested
	*/
	if (size > (UINT)len)
	    size = (UINT)len;

	FMEMCPY(str, bf->bufPtr, size);
	bf->bufPtr += size;

	/* return length read */
	*rLen = (INT)size;
    }

    return (err);
}

/**
*
* name	    BufWriteQuick - write character to output buffer
*
* synopsis  err = BufWriteQuick(bf, data)
*	    struct BUFINFO *bf		    buffer info structure
*	    UBYTE *data 		    address of output character
*
* purpose   Moves character to output buffer
*
* returns   err = TRUE if end of buffer
*
**/

BOOL INLINE BufWriteQuick(bf, data)
struct BUFINFO SEGSTACK *bf;
UBYTE FAR *data;
{
    BOOL err;

    /* assume success */
    err = FALSE;

    if (bf->bufPtr >= bf->bufEnd)
	err = TRUE;
    else {
	*bf->bufPtr = *data;
	bf->bufPtr++;
    }

    return (err);
}

/**
*
* name	    BufWriteStrQuick - write string to output buffer
*
* synopsis  err = BufWriteStrQuick(bf, str, len)
*	    struct BUFINFO *bf		    buffer info structure
*	    UBYTE *str			    string to output
*	    INT len			    string length
*
* purpose   Moves string to output buffer
*
* returns   err = TRUE if end of buffer
*
**/

BOOL INLINE BufWriteStrQuick(bf, str, len)
struct BUFINFO SEGSTACK *bf;
UBYTE FAR *str;
INT len;
{
    BOOL err;

    /* assume success */
    err = FALSE;

    if ((UINT)(bf->bufEnd - bf->bufPtr) < (UINT)len)
	err = TRUE;
    else {
	FMEMCPY(bf->bufPtr, str, len);
	bf->bufPtr += len;
    }

    return (err);
}

