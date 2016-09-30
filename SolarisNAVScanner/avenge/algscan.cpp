// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/ALGSCAN.CPv   1.0   18 Apr 1997 13:37:46   MKEATIN  $
//
// Description:
//  Functions for applying algorithmic signatures.
//
// Contains:
//  AlgApplyAllSigs()
//
// See Also:
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/ALGSCAN.CPv  $
// 
//    Rev 1.0   18 Apr 1997 13:37:46   MKEATIN
// Initial revision.
// 
//    Rev 1.12   12 Aug 1996 17:32:06   CNACHEN
// Updated to use AVDEREF_WORD macro to retrieve word values from arbitrary
// alignments on unix boxes.
// 
//    Rev 1.11   18 Jul 1996 11:06:30   DCHI
// Modifications supporting relocation of ENG_CACHE_T structure in ALGSCANDATA_T.
// 
//    Rev 1.10   02 Jul 1996 10:28:22   DCHI
// Modified followjump comment.
// 
//    Rev 1.9   01 Jul 1996 18:23:36   DCHI
// Fixed followjmp() after ScanHeader() in COMs.  Fixed buffer boundary
// checks due to cache structure.
// 
//    Rev 1.8   06 Jun 1996 18:08:08   DCHI
// Removed algorithmic signature skipping.
// 
//    Rev 1.7   29 May 1996 18:07:54   DCHI
// Changed lpvFileInfo to lpvInfo for file objects.
// 
//    Rev 1.6   23 May 1996 18:02:44   DCHI
// Added code for caching floating buffers.  Fixed ScanString() bug.
// 
//    Rev 1.5   16 May 1996 16:29:32   CNACHEN
// Cache last VID (properly now) to speed things up.
// 
// 
//    Rev 1.4   16 May 1996 16:04:10   CNACHEN
// Cache last VID to speed things up.
// 
//    Rev 1.3   16 May 1996 15:48:22   DCHI
// Added if wDebugVID == 0 to speed things up.
// 
//    Rev 1.2   16 May 1996 14:12:54   CNACHEN
// Changed to use new AVENGE?.H headers.
// 
//    Rev 1.1   16 May 1996 11:19:32   CNACHEN
// Added debug information...
// 
//    Rev 1.0   13 May 1996 16:29:14   DCHI
// Initial revision.
// 
//************************************************************************

#include "avengel.h"


#ifdef      AVENGE_DEBUG

#include <stdio.h>

BOOL        		gDisplayDebugInfo;

extern  HGENG       gDebughGEng;

extern  WORD        gwDebugVID;

#endif




//********************************************************************
//
// Function:
//  DecryptXORDelta()
//
// Parameters:
//  lpbySignature           Plaintext bytes to search for during decryption
//  lpbyScanThis            Bytes from potentially infected file to decrypt
//
// Description:
//  This function attempts to decrypt the contents of lpbyScanThis into
//  the contents of lpbySignature using a simple XOR/ADD algorithm.  If
//  the lpbyScanThis data looks like it is an encrypted version of the
//  lpbySignature, the function returns TRUE, else it returns FALSE.
//  The first few bytes of the lpbySignature contain flag information to
//  tell this function whether or not to try BYTE or WORD encryption. See
//  below. This handles the following type of encryption scheme (for both
//  BYTE and WORD):
//
//      MOV AL, 12              12 is the BASE key
// TOP: XOR [SI], AL
//      ADD AL, 34              34 is the DELTA
//      INC SI
//      LOOP TOP
//
// Returns:
//  TRUE                    Plaintext bytes have been found in infected file
//  FALSE                   Plaintext bytes not found
//
//********************************************************************

BOOL DecryptXORDelta
(
    LPBYTE  lpbySignature,
    LPBYTE  lpbyScanThis
)
{
    BYTE    byFlags, byEveryNBytes, byKey, byKey2, byDelta, byTemp;
    WORD    wSigData, wFileData, wKey, wKey2, wDelta;

// (Below referred to above) Contents of lpbySignature:
//
// BYTE ControlFlags, BYTE (EveryNBytes)-1, 4 or 8 bytes of sig
//
// Where: ControlFlags: ABCDEFGH
//               A=Byte (0) or Word (1)
//               B=Xor/Delta
//               C=Add/Delta
//               D=Rotate
//               E=Neg
//               F=Not

    // first grab our flags byte and our "skip distance" byte

    byFlags = lpbySignature[0] - 1;
    byEveryNBytes = lpbySignature[1];

    // advance over these two bytes to begin decryption...

    lpbySignature += 2;

    if (byFlags & ALGX_SCAN_WORD_MASK)
    {
        // fetch our sig and file data

        wSigData = (BYTE)(*lpbySignature++ - 1);
		wSigData |= ((WORD)(BYTE)(*lpbySignature++ - 1)) << 8;

        wFileData = AVDEREF_WORD(lpbyScanThis);

        // compute a starting key...

        wKey = wSigData ^ wFileData;

        // fetch our sig and file data

        wSigData = (BYTE)(*lpbySignature++ - 1);
		wSigData |= ((WORD)(BYTE)(*lpbySignature++ - 1)) << 8;

        lpbyScanThis += byEveryNBytes;
        wFileData = AVDEREF_WORD(lpbyScanThis);

        wKey2 = wSigData ^ wFileData;

        // compute our delta value

        wDelta = wKey2 - wKey;

        wKey = wKey2+wDelta;

        // fetch the third word of our file data

        lpbyScanThis += byEveryNBytes;
        wFileData = AVDEREF_WORD(lpbyScanThis);
        wFileData ^= wKey;

        wSigData = (BYTE)(*lpbySignature++ - 1);
		wSigData |= ((WORD)(BYTE)(*lpbySignature++ - 1)) << 8;

		if (wSigData != wFileData)
			return(FALSE);

		// fetch and verify the last word of our file data

        wFileData = AVDEREF_WORD(lpbyScanThis + byEveryNBytes);

		wKey += wDelta;
		wFileData ^= wKey;

		wSigData = (BYTE)(*lpbySignature++ - 1);
		wSigData |= ((WORD)(BYTE)(*lpbySignature - 1)) << 8;

		if (wSigData != wFileData)
			return(FALSE);

        return(TRUE);
    }
    else
    {
        // same as word case...

        byKey = (BYTE)(*lpbySignature++ - 1) ^ *lpbyScanThis;
        byKey2 = (BYTE)(*lpbySignature++ - 1) ^
                *(lpbyScanThis += byEveryNBytes);

        byDelta = byKey2 - byKey;

        byKey = byKey2 + byDelta;

        byTemp = *(lpbyScanThis += byEveryNBytes) ^ byKey;
        if (byTemp != (BYTE)(*lpbySignature++ - 1))
            return(FALSE);

        byKey += byDelta;

        byTemp = *(lpbyScanThis + byEveryNBytes) ^ byKey;
        if (byTemp != (BYTE)(*lpbySignature - 1))
            return(FALSE);

        return(TRUE);
    }
}


//********************************************************************
//
// Function:
//  DecryptAddDelta()
//
// Parameters:
//  lpbySignature           Plaintext bytes to search for during decryption
//  lpbyScanThis            Bytes from potentially infected file to decrypt
//
// Description:
//  This function attempts to decrypt the contents of lpbyScanThis into
//  the contents of lpbySignature using a simple ADD/ADD algorithm.  If
//  the lpbyScanThis data looks like it is an encrypted version of the
//  lpbySignature, the function returns TRUE, else it returns FALSE.
//  The first few bytes of the lpbySignature contain flag information to
//  tell this function whether or not to try BYTE or WORD encryption. See
//  below. This handles the following type of encryption scheme (for both
//  BYTE and WORD):
//
//      MOV AL, 12              12 is the BASE key
// TOP: ADD [SI], AL            (ADD or SUB are OK)
//      ADD AL, 34              34 is the DELTA
//      INC SI
//      LOOP TOP
//
// Returns:
//  TRUE                    Plaintext bytes have been found in infected file
//  FALSE                   Plaintext bytes not found
//
//********************************************************************

BOOL DecryptAddDelta
(
    LPBYTE  lpbySignature,
    LPBYTE  lpbyScanThis
)
{
    BYTE    byFlags, byEveryNBytes, byKey, byKey2, byDelta, byTemp;
    WORD    wSigData, wFileData, wKey, wKey2, wDelta;

// Contents of lpbySignature:
//
// BYTE ControlFlags, BYTE (EveryNBytes)-1, 4 or 8 bytes of sig
//
// Where: ControlFlags: ABCDEFGH
//               A=Byte (0) or Word (1)
//               B=Xor/Delta
//               C=Add/Delta
//               D=Rotate
//               E=Neg
//               F=Not

    // first grab our flags byte and our "skip distance" byte

    byFlags = lpbySignature[0] - 1;
    byEveryNBytes = lpbySignature[1];

    // advance over these two bytes to begin decryption...

    lpbySignature += 2;

    if (byFlags & ALGX_SCAN_WORD_MASK)
    {
        // fetch our sig and file data

        wSigData = (BYTE)(*lpbySignature++ - 1);
		wSigData |= ((WORD)(BYTE)(*lpbySignature++ - 1)) << 8;

        wFileData = AVDEREF_WORD(lpbyScanThis);

		// compute a starting key...

		wKey = wSigData - wFileData;

		// fetch our sig and file data

		wSigData = (BYTE)(*lpbySignature++ - 1);
		wSigData |= ((WORD)(BYTE)(*lpbySignature++ - 1)) << 8;

        lpbyScanThis += byEveryNBytes;
        wFileData = AVDEREF_WORD(lpbyScanThis);

        wKey2 = wSigData - wFileData;

        // compute our delta value

        wDelta = wKey2 - wKey;

        wKey = wKey2+wDelta;

        // fetch the third word of our file data

        lpbyScanThis += byEveryNBytes;
        wFileData = AVDEREF_WORD(lpbyScanThis);
        wFileData += wKey;

        wSigData = (BYTE)(*lpbySignature++ - 1);
		wSigData |= ((WORD)(BYTE)(*lpbySignature++ - 1)) << 8;

		if (wSigData != wFileData)
			return(FALSE);

		// fetch and verify the last word of our file data

        wFileData = AVDEREF_WORD(lpbyScanThis + byEveryNBytes);

		wKey += wDelta;
		wFileData += wKey;

		wSigData = (BYTE)(*lpbySignature++ - 1);
		wSigData |= ((WORD)(BYTE)(*lpbySignature - 1)) << 8;

		if (wSigData != wFileData)
			return(FALSE);

        return(TRUE);
    }
    else
    {
        // same as word case...

        byKey = (BYTE)(*lpbySignature++ - 1) - *lpbyScanThis;
        byKey2 = (BYTE)(*lpbySignature++ - 1) -
                *(lpbyScanThis += byEveryNBytes);

        byDelta = byKey2 - byKey;

        byKey = byKey2 + byDelta;

        byTemp = *(lpbyScanThis += byEveryNBytes) + byKey;
        if (byTemp != (BYTE)(*lpbySignature++ - 1))
			return(FALSE);

        byKey += byDelta;

        byTemp = *(lpbyScanThis + byEveryNBytes) + byKey;
        if (byTemp != (BYTE)(*lpbySignature - 1))
            return(FALSE);

        return(TRUE);
    }
}

//********************************************************************
//
// Function:
//  DecryptRotate()
//
// Parameters:
//  lpbySignature           Plaintext bytes to search for during decryption
//  lpbyScanThis            Bytes from potentially infected file to decrypt
//
// Description:
//  This function attempts to decrypt the contents of lpbyScanThis into
//  the contents of lpbySignature using a simple rotate algorithm.  If
//  the lpbyScanThis data looks like it is an encrypted version of the
//  lpbySignature, the function returns TRUE, else it returns FALSE.
//  The first few bytes of the lpbySignature contain flag information to
//  tell this function whether or not to try BYTE or WORD encryption. See
//  below.  Basically, this function checks whether each byte in lpbySignature
//  could be rotated into the corresponding byte in lpbyScanThis using any
//  rotation value.  It's not picky in any way.  The first byte could be
//  rotated by 3 bits, the second by 4 bits, and the third by 1 bit, etc.
//  This handles the following type of encryption scheme (for both
//  BYTE and WORD):
//
// TOP: ROL BYTE PTR [SI], 03   (03 could be CL or any other combination)
//      INC SI
//      LOOP TOP
//
// Returns:
//  TRUE                    Plaintext bytes have been found in infected file
//  FALSE                   Plaintext bytes not found
//
//********************************************************************

BOOL DecryptRotate
(
    LPBYTE  lpbySignature,
    LPBYTE  lpbyScanThis
)
{
    BYTE    byFlags, byEveryNBytes, bySigData, byFileData, byHighBit;
    WORD    wSigData, wFileData, wHighBit;
	int     i, j;

// Contents of lpbySignature:
//
// BYTE ControlFlags, BYTE (EveryNBytes)-1, 4 or 8 bytes of sig
//
// Where: ControlFlags: ABCDEFGH
//               A=Byte (0) or Word (1)
//               B=Xor/Delta
//               C=Add/Delta
//               D=Rotate
//               E=Neg
//               F=Not
//
// This routine verifies whether each of the four WORDS/BYTES can be rolled
// to match the four WORDS/BYTES in the target file.  NOTE: The roll count does
// not need to be the same for each of the four WORDS/BYTES.

	// first grab our flags byte and our "skip distance" byte

	byFlags = lpbySignature[0] - 1;
	byEveryNBytes = lpbySignature[1];

	// advance over these two bytes to begin decryption...

	lpbySignature += 2;

	if (byFlags & ALGX_SCAN_WORD_MASK)
	{
		// go through four comparisons

		for (i=0;i<4;i++)
		{
			wSigData = (BYTE)(*lpbySignature++ - 1);
			wSigData |= ((WORD)(BYTE)(*lpbySignature++ - 1)) << 8;

            wFileData = AVDEREF_WORD(lpbyScanThis);

            lpbyScanThis += byEveryNBytes;

            for (j=0;j<16;j++)
            {
                if (wSigData & 0x8000)
                    wHighBit = TRUE;
                else
                    wHighBit = FALSE;

                wSigData <<= 1;
                wSigData |= wHighBit;

                if (wSigData == wFileData)
					break;
            }

            if (j == 16)
                break;
        }

        if (i == 4)
            return(TRUE);

        return(FALSE);
    }
    else
    {
        // go through four comparisons

        for (i=0;i<4;i++)
		{
            bySigData = (BYTE)(*lpbySignature++ - 1);
            byFileData = *lpbyScanThis;

            lpbyScanThis += byEveryNBytes;

            for (j=0;j<16;j++)
            {
                if (bySigData & 0x80)
                    byHighBit = TRUE;
                else
                    byHighBit = FALSE;

                bySigData <<= 1;
                bySigData |= byHighBit;

                if (bySigData == byFileData)
                    break;
            }

            if (j == 16)
                break;
        }

        if (i == 4)
            return(TRUE);

        return(FALSE);
    }
}


//********************************************************************
//
// Function:
//  DecryptNot()
//
// Parameters:
//  lpbySignature           Plaintext bytes to search for during decryption
//  lpbyScanThis            Bytes from potentially infected file to decrypt
//
// Description:
//  This function attempts to decrypt the contents of lpbyScanThis into
//  the contents of lpbySignature using a bitwise complement encryption.  If
//  the lpbyScanThis data looks like it is an encrypted version of the
//  lpbySignature, the function returns TRUE, else it returns FALSE.
//  The first few bytes of the lpbySignature contain flag information to
//  tell this function whether or not to try BYTE or WORD encryption. See
//  below.
//  This handles the following type of encryption scheme (for both
//  BYTE and WORD):
//
// TOP: NOT BYTE PTR [SI]
//      INC SI
//      LOOP TOP
//
// Returns:
//  TRUE                    Plaintext bytes have been found in infected file
//  FALSE                   Plaintext bytes not found
//
//********************************************************************

BOOL DecryptNot
(
    LPBYTE  lpbySignature,
    LPBYTE  lpbyScanThis
)
{
	BYTE    byFlags, byEveryNBytes, bySigData, byFileData;
	WORD    wSigData, wFileData;
	int     i;

// Contents of lpbySignature:
//
// BYTE ControlFlags, BYTE (EveryNBytes)-1, 4 or 8 bytes of sig
//
// Where: ControlFlags: ABCDEFGH
//               A=Byte (0) or Word (1)
//               B=Xor/Delta
//               C=Add/Delta
//               D=Rotate
//               E=Neg
//               F=Not
//
// This routine verifies whether each of the four WORDS/BYTES can be rolled
// to match the four WORDS/BYTES in the target file.  NOTE: The roll count does
// not need to be the same for each of the four WORDS/BYTES.

    // first grab our flags byte and our "skip distance" byte

    byFlags = lpbySignature[0] - 1;
    byEveryNBytes = lpbySignature[1];

    // advance over these two bytes to begin decryption...

    lpbySignature += 2;

    if (byFlags & ALGX_SCAN_WORD_MASK)
    {
        // go through four comparisons

        for (i=0;i<4;i++)
		{
            wSigData = (BYTE)(*lpbySignature++ - 1);
			wSigData |= ((WORD)(BYTE)(*lpbySignature++ - 1)) << 8;

            wFileData = AVDEREF_WORD(lpbyScanThis);

            lpbyScanThis += byEveryNBytes;

			if ((WORD)~wSigData != wFileData)
                break;
        }

        if (i == 4)
            return(TRUE);

        return(FALSE);
    }
    else
    {
        // go through four comparisons

        for (i=0;i<4;i++)
        {
            bySigData = (BYTE)(*lpbySignature++ - 1);
            byFileData = *lpbyScanThis;

            lpbyScanThis += byEveryNBytes;

			if ((BYTE)~bySigData != byFileData)
                break;
        }

        if (i == 4)
            return(TRUE);

		return(FALSE);
	}
}

//********************************************************************
//
// Function:
//  DecryptNot()
//
// Parameters:
//  lpbySignature           Plaintext bytes to search for during decryption
//  lpbyScanThis            Bytes from potentially infected file to decrypt
//
// Description:
//  This function attempts to decrypt the contents of lpbyScanThis into
//  the contents of lpbySignature using a 2's complement negation encryption.
//  If the lpbyScanThis data looks like it is an encrypted version of the
//  lpbySignature, the function returns TRUE, else it returns FALSE.
//  The first few bytes of the lpbySignature contain flag information to
//  tell this function whether or not to try BYTE or WORD encryption. See
//  below.
//  This handles the following type of encryption scheme (for both
//  BYTE and WORD):
//
// TOP: NEG BYTE PTR [SI]
//      INC SI
//      LOOP TOP
//
// Returns:
//  TRUE                    Plaintext bytes have been found in infected file
//  FALSE                   Plaintext bytes not found
//
//********************************************************************


BOOL DecryptNeg
(
    LPBYTE  lpbySignature,
    LPBYTE  lpbyScanThis
)
{
	BYTE    byFlags, byEveryNBytes, bySigData, byFileData;
	WORD    wSigData, wFileData;
	int     i;

// Contents of lpbySignature:
//
// BYTE ControlFlags, BYTE (EveryNBytes)-1, 4 or 8 bytes of sig
//
// Where: ControlFlags: ABCDEFGH
//               A=Byte (0) or Word (1)
//               B=Xor/Delta
//               C=Add/Delta
//               D=Rotate
//               E=Neg
//               F=Not
//
// This routine verifies whether each of the four WORDS/BYTES can be rolled
// to match the four WORDS/BYTES in the target file.  NOTE: The roll count does
// not need to be the same for each of the four WORDS/BYTES.

    // first grab our flags byte and our "skip distance" byte

    byFlags = lpbySignature[0] - 1;
    byEveryNBytes = lpbySignature[1];

    // advance over these two bytes to begin decryption...

    lpbySignature += 2;

    if (byFlags & ALGX_SCAN_WORD_MASK)
    {
        // go through four comparisons

        for (i=0;i<4;i++)
        {
            wSigData = (BYTE)(*lpbySignature++ - 1);
			wSigData |= ((WORD)(BYTE)(*lpbySignature++ - 1)) << 8;

            wFileData = AVDEREF_WORD(lpbyScanThis);

            lpbyScanThis += byEveryNBytes;

            if ((WORD)-(short)wSigData != wFileData)
                break;
        }

        if (i == 4)
            return(TRUE);

        return(FALSE);
    }
    else
    {
        // go through four comparisons

        for (i=0;i<4;i++)
        {
            bySigData = (BYTE)(*lpbySignature++ - 1);
            byFileData = *lpbyScanThis;

            lpbyScanThis += byEveryNBytes;

            if ((BYTE)-bySigData != byFileData)
                break;
        }

        if (i == 4)
            return(TRUE);

        return(FALSE);
    }
}

//********************************************************************
//
// Function:
//  FillFloatingBuffer()
//
// Parameters:
//  lpFileObject            A file object for the file to be scanned
//  lpEngCache              Cache structure
//  lpAlgScanThis           Buffers used by the scanner, to be filled
//                          with data from the target file
//  lplpbyFStartPtr         A pointer to a pointer that points to the
//                          current buffer where signatures are being applied.
//                          After the function reads the specified data into
//                          the floating FStart buffer, it will set
//                          *lplpbyFStartPtr to point to the start of the
//                          floating buffer.  Subsequent signature matching
//                          will be done from the floating buffer unless the
//                          signature pointer is reset using SeekTop() or
//                          SeekHeader, etc.
//  dwSeekOffset            The offset relative to the top of the file
//                          from which to fill the buffer.
//  lpdwCurOffset           On success, *lpdwCurOffset is set to dwSeekOffset.
//
// Description:
//  This function fills up the FStart floating buffer with 256 bytes of data
//  so the scanner can look for virus signatures.  The file must already be
//  open.
//
//  :Details of operation:
//  - If the seek is past the end of the file, the function returns
//      a pre-initialized buffer of zeroes.
//  - If the seek is at or past the FileLength - ENG_EOF_CACHE_SIZE,
//      the function first determines whether the ENG_EOF_CACHE_SIZE
//      bytes at the end of the file have been cached.  If not, then
//      the ENG_EOF_CACHE_SIZE bytes from the end of the file are
//      read into the case.  The function returns the appropriate bytes
//      from the cache.
//  - If the seek is to a location that has been cached, the contents
//      of the cached buffer are returned.  Otherwise, the data is
//      read into the next available buffer and returned.  The
//      cache implements a FIFO policy.
//
// Returns:
//  ENGSTATUS_OK            On successful buffer fill.
//  ENGSTATUS_ERROR         On file error
//
//********************************************************************

ENGSTATUS FillFloatingBuffer
(
	LPFILEOBJECT     lpFileObject,
    LPENG_CACHE      lpEngCache,
	LPALGSCANDATA    lpAlgScanThis,
    LPLPBYTE         lplpbyFStartPtr,
    DWORD            dwSeekOffset,
    LPDWORD          lpdwCurOffset
)
{
    DWORD dwNumBytes;

    // See if it's in the cache

	if (dwSeekOffset >= lpEngCache->dwFileLength)
    {
        // Just immediately return with a buffer of zeroes if a seek
        //  is attempted past the end of the file

        lpAlgScanThis->lpbyFloatingBuffer =
            lpEngCache->byEOFCache + ENG_EOF_CACHE_SIZE;
    }
    else
    if (dwSeekOffset >= lpEngCache->dwEOFCacheStartOffset)
    {
        // Has the EOF cache been filled yet?

        if (lpEngCache->wNumBytesInEOFCache == 0)
        {
            // Fill the cache first

            if (AVFileSeek(lpFileObject->lpvInfo,
                           lpEngCache->dwEOFCacheStartOffset,
                           SEEK_SET,
                           lpdwCurOffset) == CBSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            // fill our cache buffer with data...

            if (AVFileRead(lpFileObject->lpvInfo,
                           lpEngCache->byEOFCache,
                           ENG_EOF_CACHE_SIZE,
                           &dwNumBytes) == CBSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            lpEngCache->wNumBytesInEOFCache = (WORD)dwNumBytes;

            // Set the remaining bytes to zero

            MEMSET(lpEngCache->byEOFCache + (WORD)dwNumBytes,
                   0,
                   ENG_EOF_CACHE_SIZE-(WORD)dwNumBytes);
        }

        // Set pointer

        lpAlgScanThis->lpbyFloatingBuffer =
            lpEngCache->byEOFCache +
            (WORD)(dwSeekOffset - lpEngCache->dwEOFCacheStartOffset);
    }
    else
    {
        int i;

        // Search other cache

        for (i=0;i<lpEngCache->wNumOtherFilled;i++)
        {
            if (lpEngCache->dwOtherOffset[i] == dwSeekOffset)
                break;
        }

        if (i == lpEngCache->wNumOtherFilled)
        {
            // Couldn't find it, so read one in

            // Use FIFO

            i = lpEngCache->wNextFreeEntry;

            // Now read it in

            // seek to the proper location and set up our lpdwCurOffset

            if (AVFileSeek(lpFileObject->lpvInfo,
                           dwSeekOffset,
                           SEEK_SET,
                           lpdwCurOffset) == CBSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            // fill our floating buffer cache entry with data...

            if (AVFileRead(lpFileObject->lpvInfo,
                           lpEngCache->byOtherCache[i],
                           FSTART_BUFFER_SIZE,
                           &dwNumBytes) == CBSTATUS_ERROR ||
                dwNumBytes != FSTART_BUFFER_SIZE)
            {
                // Must have been a fatal error

                lpEngCache->dwOtherOffset[i] = (DWORD)-1;
                return(ENGSTATUS_ERROR);
            }

            // Update offset of entry

            lpEngCache->dwOtherOffset[i] = dwSeekOffset;

            // Update next free entry

            if (++lpEngCache->wNextFreeEntry >= ENG_NUM_CACHE_BUFFERS)
                lpEngCache->wNextFreeEntry = 0;

            if (lpEngCache->wNumOtherFilled < ENG_NUM_CACHE_BUFFERS)
                ++lpEngCache->wNumOtherFilled;
        }

        lpAlgScanThis->lpbyFloatingBuffer =
            lpEngCache->byOtherCache[i];
    }

    *lpdwCurOffset = dwSeekOffset;
    *lplpbyFStartPtr = lpAlgScanThis->lpbyFloatingBuffer;

#ifdef      AVENGE_DEBUG
    if (gDisplayDebugInfo)
	{
		int i;

        printf("\tFLOAT (Offset %ld): ", *lpdwCurOffset);
        for (i=0;i<16;i++)
            printf("%02X ",
                    lpAlgScanThis->lpbyFloatingBuffer[i]);
        printf("\n");
    }
#endif

    return(ENGSTATUS_OK);
}

//********************************************************************
//
// Function:
//  AlgMatchWild()
//
// Parameters:
//  lpFileObject            A file object for the file to be scanned
//  lpAlgSigData            Algorithmic signature data (pool pointers and hash
//                          tables)
//  byWildByte              Value of the 0xD? function which invoked the
//                          algorithmic interpreter.
//  lpAlgScanThis           Contains data read from the target file.  This data
//                          will be scruitinzed or updated by the various
//                          algoritmhic functions below.
//  lpFStartInfo            Information from FSTART on the file type,
//                          first landing, etc.
//  lpnCurSigIndex          Pointer to an integer which specifies our offset in
//                          our alg signature.
//  lplpbyFStartPtr         A pointer to a pointer that points to the
//                          current byte in the scan buffers where signatures
//                          are being applied.
//  lpdwCurOffset           Pointer to a DWORD which contains the offset in
//                          the file from where our scan buffer was last filled.
//                          This is used to compute IP's for followjumps.
//  lpbMatch                Did this algorithmic function match or fail?
//
// Description:
//  This function applies a single algorithmic function from a signature to
//  the provided scan data.  It may refill the floating FStart buffer (if
//  a function requests a new scan buffer from the target file).  It will
//  also modify the *lplpbyFStartPtr with almost all successful alg functions.
//  On entry to this function, lpnCurSigIndex indexes the start of the
//  alg function to be executed, and is updated to index the end of the
//  cur alg function after it has completed matching (if it is successful).
//  See NEVIR:VIRSCAN.DAT README.SRC for information on each of the
//  algorithmic functions.
//
// Returns:
//  ENGSTATUS_OK            If no errors were encountered during the
//                          application of the current function.
//  ENGSTATUS_ERROR         If a file error occured during application
//
//********************************************************************

ENGSTATUS AlgMatchWild
(
    LPFILEOBJECT    lpFileObject,
    LPALG_SIG_DATA  lpAlgSigData,
    BYTE            byWildByte,
    LPALGSCANDATA   lpAlgScanThis,
    LPFSTARTINFO    lpFStartInfo,
    LPINT           lpnCurSigIndex,
    LPLPBYTE        lplpbyFStartPtr,
    LPDWORD         lpdwCurOffset,
    LPBOOL          lpbMatch
)
{
    BYTE            byDecodedSigByte, byTemp, byScanByte, byScanMask;
	int             nCurSigIndex, i, j, nStringLen, nSearchDist;
    unsigned int    uSlideCount;
    WORD            wTemp, wBufferOffset, wIP, wNumPages, wMod512, wLow, wHi;
    DWORD           dwTemp, dwImageSize, dwEntryDist;
	LPBYTE          lpbySigPtr, lpbyScanThis;

	// speed things up by using a pointer direct to our signature and to what
    // we're scanning

    lpbySigPtr = lpAlgSigData->bySigData;
    lpbyScanThis = *lplpbyFStartPtr;

    // all processing will use nCurSigIndex.  Before returning, *lpnCurSigIndex
    // should be set to nCurSigIndex's value.

    nCurSigIndex = *lpnCurSigIndex;

    // check to see if we're doing a 0xDF opcode. If so, adjust indices and
    // our byWildByte accordingly...

    if (byWildByte == ALG_DF_FUNC)
    {
        // decode the next byte. it's a sub-function number between 0 and
        // 17 (currently).  This is added to the ALGX_BASE to determine the
        // appropriate function number in the switch below.

        byWildByte = lpbySigPtr[++nCurSigIndex] - 1 + ALGX_BASE;
    }
    else
        byWildByte &= MASK_LOW_NIBBLE;


	// determine which function we're executing.

	switch (byWildByte)
    {

///////////////////////////////////////////////////////////////////////////////

        case ALG_FUNC_QUICK_SLIDE:

            // quick slide up to 16 bytes for the specified signature byte...

            byDecodedSigByte = lpbySigPtr[++nCurSigIndex] - 1;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tQuickSlide(0x%02X): ",byDecodedSigByte);
            }
#endif

			for (i=0;i<QUICK_SLIDE_DISTANCE;i++)
                if (*lpbyScanThis++ == byDecodedSigByte)
                {
                    *lplpbyFStartPtr = lpbyScanThis;
                    *lpnCurSigIndex = nCurSigIndex;
                    *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
                    if (gDisplayDebugInfo)
                    {
                        int j;

                        printf("MATCH (After sliding %d bytes)\n", i);

                        printf("\tFSTART: ");
                        for (j=0;j<16;j++)
                            printf("%02X ", lpbyScanThis[j]);
                        printf("\n");
                    }
#endif

                    return(ENGSTATUS_OK);
                }

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("FAIL\n");
            }
#endif

            *lpbMatch = FALSE;
            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALG_FUNC_SKIP_1:
        case ALG_FUNC_SKIP_2:
        case ALG_FUNC_SKIP_3:
        case ALG_FUNC_SKIP_4:
        case ALG_FUNC_SKIP_5:
        case ALG_FUNC_SKIP_6:
        case ALG_FUNC_SKIP_7:
        case ALG_FUNC_SKIP_8:
        case ALG_FUNC_SKIP_9:

            // skip between 1 and 9 bytes forward...  1 <= byWildByte <= 9

            *lplpbyFStartPtr = lpbyScanThis + byWildByte;
            *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                int i;

				printf("\tSkip(%d): MATCH\n",byWildByte);

                printf("\tFSTART: ");
                for (i=0;i<16;i++)
                    printf("%02X ", lpbyScanThis[i + byWildByte]);
                printf("\n");
            }
#endif

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALG_FUNC_SLIDE:

            // quick slide up to specified # of bytes for the specified
            // signature byte...

            uSlideCount = (BYTE)(lpbySigPtr[++nCurSigIndex] - 1);
            byDecodedSigByte = lpbySigPtr[++nCurSigIndex] - 1;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tScanSlide(%d,0x%02X): ",
                        uSlideCount,
                        byDecodedSigByte);
            }
#endif

            for (i=0;i<(int)uSlideCount;i++)
                if (*lpbyScanThis++ == byDecodedSigByte)
                {
                    *lplpbyFStartPtr = lpbyScanThis;

                    *lpnCurSigIndex = nCurSigIndex;
                    *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
                    if (gDisplayDebugInfo)
                    {
                        int j;

                        printf("MATCH (After sliding %d bytes)\n", i);
                        printf("\tFSTART: ");
                        for (j=0;j<16;j++)
                            printf("%02X ", lpbyScanThis[j]);
                        printf("\n");
                    }
#endif

                    return(ENGSTATUS_OK);
                }

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("FAIL\n");
            }
#endif

            *lpbMatch = FALSE;
            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALG_FUNC_SEEK_TOP:

            // reset our scanthis pointer to the top of the FSTART buffer

			*lplpbyFStartPtr = lpAlgScanThis->stFStartData.byFStartBuffer;
            *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tSeekTop(): MATCH\n");
            }
#endif

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALG_FUNC_SKIP_FORWARD_N:

            // jump ahead N bytes in the FSTART buffer

            byTemp = lpbySigPtr[++nCurSigIndex] - 1;

            *lpnCurSigIndex = nCurSigIndex;
            *lplpbyFStartPtr += byTemp;
            *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                int i;

                printf("\tSeekForward(%d): MATCH\n",byTemp);
                printf("\tFSTART: ");
                for (i=0;i<16;i++)
                    printf("%02X ", lpbyScanThis[i+byTemp]);
                printf("\n");
            }
#endif

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALG_FUNC_SKIP_BACK_N:

            // jump back N bytes in the FSTART buffer

            byTemp = lpbySigPtr[++nCurSigIndex] - 1;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tSeekBackward(%d): ",byTemp);
            }
#endif

            *lpnCurSigIndex = nCurSigIndex;
            *lplpbyFStartPtr -= byTemp;

            // see if we've seeked before the start of our FSTART buffer
            // note: this does not properly check whether we seek too far back
            // when looking at byTOFBuffer or lpbyFloatingBuffer,
            // but is a direct port from the old ASM code...

			if (*lplpbyFStartPtr < lpAlgScanThis->stFStartData.byFStartBuffer)
            {
				*lpbMatch = FALSE;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("FAIL (Seeked before start of FSTART buffer)\n");
                }
#endif
            }
            else
            {
                *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    int i;

                    printf("MATCH\n");
                    printf("\tFSTART: ");
                    for (i=0;i<16;i++)
                        printf("%02X ", lpbyScanThis[i-byTemp]);
                    printf("\n");
                }
#endif
            }

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALG_FUNC_MATCH_HIGH_NIBBLE:

            // first get our nibble byte from the signature...

            byTemp = lpbySigPtr[++nCurSigIndex] - 1;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("\tHighNibble(0x%02X): ",byTemp);
                }
#endif

            // check to see if we have a match...

            if ((*lpbyScanThis & MASK_HIGH_NIBBLE) == byTemp)
            {
                *lplpbyFStartPtr = lpbyScanThis+1;

                *lpnCurSigIndex = nCurSigIndex;
                *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("MATCH (Actual value is 0x%02X)\n",*lpbyScanThis);
                }
#endif

                return(ENGSTATUS_OK);
            }

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("FAIL (Actual value is 0x%02X)\n",*lpbyScanThis);
                }
#endif

            *lpbMatch = FALSE;

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_SCAN_HEADER:

            // reposition the scanthis pointer to the start of the TOF buffer

			*lplpbyFStartPtr = lpAlgScanThis->stFStartData.byTOFBuffer;

            *lpnCurSigIndex = nCurSigIndex;
            *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                int i;

                printf("\tScanHeader(): MATCH\n");
                printf("\tTOF: ");
                for (i=0;i<16;i++)
                    printf("%02X ",
                            lpAlgScanThis->stFStartData.byTOFBuffer[i]);
                printf("\n");
            }
#endif

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_FOLLOWJMP2ABS:

            // use a WORD from the scan-this data as an absolute address and
            // jump there.  NOTE: this only works in COM files!

            wTemp = AVDEREF_WORD(lpbyScanThis) - COM_IP_SUB_CONST;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tFollowJump2(): MATCH (Dest IP = 0x%04X)\n",
                        wTemp + COM_IP_SUB_CONST);
            }
#endif

            // now seek to the specified location and read the data into
            // the floating buffer..

            if (FillFloatingBuffer(lpFileObject,
                                   &(lpAlgScanThis->stEngCache),
                                   lpAlgScanThis,
                                   lplpbyFStartPtr,
                                   wTemp,
                                   lpdwCurOffset) == ENGSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            *lpnCurSigIndex = nCurSigIndex;
            *lpbMatch = TRUE;

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_DIRECTJMP2ABS:

            // jump to a specified offset in the file...  this offset is
            // specified directly in the signature...  the offset is not
            // adjusted in any way (e.g. by a subtraction constant of 0x100)

            // first get the low byte, then the high byte...

            wTemp = (BYTE)(lpbySigPtr[++nCurSigIndex] - 1);
			wTemp |= ((WORD)(BYTE)(lpbySigPtr[++nCurSigIndex] - 1)) << 8;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tJumpTOF(%u): MATCH\n", wTemp);
            }
#endif

            // now seek to the specified location and read the data into
            // the floating buffer..

            if (FillFloatingBuffer(lpFileObject,
                                   &(lpAlgScanThis->stEngCache),
                                   lpAlgScanThis,
                                   lplpbyFStartPtr,
                                   wTemp,
                                   lpdwCurOffset) == ENGSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            *lpnCurSigIndex = nCurSigIndex;
            *lpbMatch = TRUE;

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_SEEKBACK:

            // determine where we want to seek to, relative to the EOF...

            wTemp = (BYTE)(lpbySigPtr[++nCurSigIndex] - 1);
			wTemp |= ((WORD)(BYTE)(lpbySigPtr[++nCurSigIndex] - 1)) << 8;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tJumpEOF(%u): ", wTemp);
            }
#endif
            dwTemp = lpAlgScanThis->stEngCache.dwFileLength;

            // are we trying to seek before the TOF?  If so, this file
            // could not be infected...

            if (dwTemp < wTemp)
            {
                *lpbMatch = FALSE;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("FAIL (Seek before TOF)\n", wTemp);
                }
#endif

                return(ENGSTATUS_OK);
            }

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("MATCH\n");
            }
#endif

            if (FillFloatingBuffer(lpFileObject,
                                   &(lpAlgScanThis->stEngCache),
                                   lpAlgScanThis,
                                   lplpbyFStartPtr,
                                   dwTemp - wTemp,
                                   lpdwCurOffset) == ENGSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            *lpnCurSigIndex = nCurSigIndex;
            *lpbMatch = TRUE;

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_FOLLOWJMPHEADERE9:

            // this function is used to follow a JMP or CALL which is found
            // while the signature is examining the top of a COM file. The
            // assumption is that the lpbyScanThis points within the TOFBuffer.
            // This function assumes we have already matched the E9/E8 byte
            // of the JMP/CALL instruction, and we are on top of the
            // destination operand... (which is why we're adding 2 instead of
            // 3 in the destination address computation below).

            wTemp = AVDEREF_WORD(lpbyScanThis) + 2;
			wTemp += (lpbyScanThis - lpAlgScanThis->stFStartData.byTOFBuffer);

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tFollowJmpE9Header(): MATCH (Dest IP = 0x%04X",
                        wTemp);
            }
#endif
            // now seek to the specified location and read the data into
            // the floating buffer..

            if (FillFloatingBuffer(lpFileObject,
                                   &(lpAlgScanThis->stEngCache),
                                   lpAlgScanThis,
                                   lplpbyFStartPtr,
                                   wTemp,
                                   lpdwCurOffset) == ENGSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            *lpnCurSigIndex = nCurSigIndex;
            *lpbMatch = TRUE;

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_STRINGSEARCH:

            // search for the specified string.

            nSearchDist = (BYTE)(lpbySigPtr[++nCurSigIndex] - 1);
            nStringLen = (BYTE)(lpbySigPtr[++nCurSigIndex] - 1);

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                int i;

                printf("\tScanString(%d,%d) (String = ",nSearchDist,nStringLen);
                for (i=0;i<nStringLen;i++)
					if ((BYTE)(lpbySigPtr[nCurSigIndex+i+1]-1) == 0xD1)
						printf("?? ");
					else
						printf("%02X ",(BYTE)(lpbySigPtr[nCurSigIndex+i+1]-1));
                printf("): ");
            }
#endif

            // retrieve the first byte of our search string and look for it...

            byTemp = lpbySigPtr[++nCurSigIndex] - 1;

            // if we find the first byte then see if we have a match on the
			// rest of the string...
			// note: a D1 byte (encoded to D2) in the string designates a
			// wild byte in the string...

			for (i=0;i<nSearchDist;i++)
			{
				if (byTemp == *lpbyScanThis)
				{
					for (j=1;j<nStringLen;j++)
						if (lpbySigPtr[nCurSigIndex + j] !=
							(BYTE)(lpbyScanThis[j]+1) &&
							lpbySigPtr[nCurSigIndex + j] != STRING_SEARCH_SKIP_1_BYTE)
							break;

					if (j == nStringLen)
					{
						// set our signature index to point to the last byte of
						// the string we were searching for.  it will be advanced
						// automatically one level up.

                        *lpnCurSigIndex = nCurSigIndex + nStringLen - 1;

						// make sure we advance our scan-this pointer so the next
						// byte we match is just after the string we found..

                        *lplpbyFStartPtr += nStringLen + i; // removed +1

						*lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
                        if (gDisplayDebugInfo)
                        {
                            int j;

                            printf("MATCH (After searching %d bytes)\n", i);
                            printf("\tFSTART: ");
                            for (j=0;j<16;j++)
                                printf("%02X ", lpbyScanThis[nStringLen + j]);
                            printf("\n");
                        }
#endif
                        return(ENGSTATUS_OK);
					}
				}

				lpbyScanThis++;
			}

			*lpbMatch = FALSE;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("FAIL\n");
            }
#endif
            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

		case ALGX_FUNC_UNUSED_06:
        case ALGX_FUNC_UNUSED_07:
        case ALGX_FUNC_UNUSED_08:
		case ALGX_FUNC_UNUSED_0E:
        case ALGX_FUNC_UNUSED_0F:
        case ALGX_FUNC_UNUSED_11:
        case ALGX_FUNC_UNUSED_12:

            *lpbMatch = FALSE;

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_MATCH_LOW_NIBBLE:


            // first get our nibble byte from the signature...

            byTemp = lpbySigPtr[++nCurSigIndex] - 1;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("\tLowNibble(0x%02X): ",byTemp);
                }
#endif

            // check to see if we have a match...

            if ((*lpbyScanThis & MASK_LOW_NIBBLE) == byTemp)
            {
                *lplpbyFStartPtr = lpbyScanThis+1;

                *lpnCurSigIndex = nCurSigIndex;
                *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("MATCH (Actual value is 0x%02X)\n",*lpbyScanThis);
                }
#endif
                return(ENGSTATUS_OK);
            }

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("FAIL (Actual value is 0x%02X)\n",*lpbyScanThis);
            }
#endif

            *lpbMatch = FALSE;

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_DETECT_DX:

            // fetch the 0xD? byte we want to match...

            byTemp = lpbySigPtr[++nCurSigIndex] - 1;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tVerifyDByte(0x%02X): ",byTemp);
            }
#endif

            // check if we have a hit?

            if (*lpbyScanThis == byTemp)
            {
                *lplpbyFStartPtr = lpbyScanThis+1;

                *lpnCurSigIndex = nCurSigIndex;
                *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("MATCH (Actual value is 0x%02X)\n",*lpbyScanThis);
                }
#endif

                return(ENGSTATUS_OK);
            }

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("FAIL (Actual value is 0x%02X)\n",*lpbyScanThis);
            }
#endif

            *lpbMatch = FALSE;

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_FOLLOWJMP1:

            // update our location in our buffer based on the 1-byte
            // displacement operand..  Make sure we add an extra 1 to skip
            // over the JMP operand.

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tFollowJump1(): ");
            }
#endif
            lpbyScanThis += ((signed short)(signed char)*lpbyScanThis) + 1;

            // Make sure we are within the buffer limits

            if (!((lpbyScanThis >= lpAlgScanThis->stFStartData.
                    byFStartBuffer &&
                   lpbyScanThis <= lpAlgScanThis->stFStartData.
                    byTOFBuffer + FSTART_BUFFER_SIZE - 1) ||
                  (lpbyScanThis >= lpAlgScanThis->lpbyFloatingBuffer &&
                   lpbyScanThis <= lpAlgScanThis->
                    lpbyFloatingBuffer + FSTART_BUFFER_SIZE - 1)))
            {
                *lpbMatch = FALSE;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("FAIL (Displacement 0x%02X outside of buffers)\n",
                            *lpbyScanThis);
                }
#endif

                return(ENGSTATUS_OK);
            }

            *lplpbyFStartPtr = lpbyScanThis;
            *lpnCurSigIndex = nCurSigIndex;
            *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("MATCH (Displacement is 0x%02X)\n",
                        *lpbyScanThis);
            }
#endif

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_EXEC:

            // obtain the NAVEX ID to post to the NAVEX buffer...

            wTemp = (BYTE)(lpbySigPtr[++nCurSigIndex] - 1);
            wTemp |= ((WORD)(BYTE)(lpbySigPtr[++nCurSigIndex] - 1)) << 8;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
				printf("\tExecute(0x%04X): FAIL (Intentional)\n",wTemp);
            }
#endif

            // if there are any more NAVEX slots open, insert the request
            // and bump up the count of requests

            if (lpAlgScanThis->wNAVEXRequestCount < MAX_NAVEX_REQUESTS)
            {
                lpAlgScanThis->
                    wNAVEXRequestBuffer[lpAlgScanThis->wNAVEXRequestCount] =
                        wTemp;
				lpAlgScanThis->wNAVEXRequestCount++;
            }

            // fail the current signature...

            *lpbMatch = FALSE;

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_RET_FALSE:

            // no match...

            *lpbMatch = FALSE;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tRetFalse(): FAIL (Intentional)\n");
            }
#endif

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_ENCRYPTED_SEARCH:

            // it does not appear that our scan this pointer is updated if we
            // have a match here...

            byTemp = lpbySigPtr[++nCurSigIndex] - 1;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
			{
				int i;

                printf("\tEncryptedSearch(");

				if (byTemp & ALGX_SCAN_XOR_DELTA_MASK)
					printf("X");
				if (byTemp & ALGX_SCAN_ADD_DELTA_MASK)
					printf("+");
				if (byTemp & ALGX_SCAN_ROTATE_MASK)
					printf("R");
				if (byTemp & ALGX_SCAN_NEG_MASK)
					printf("-");
				if (byTemp & ALGX_SCAN_NOT_MASK)
					printf("~");
				printf(",");
				if (byTemp & ALGX_SCAN_WORD_MASK)
					printf("WORD,%d)\n",lpbySigPtr[nCurSigIndex+1]);
				else
					printf("BYTE,%d)\n",lpbySigPtr[nCurSigIndex+1]);

				printf("\tBytes to match: ");
				for (i=0;i<8;i++)
					printf("%02X ", lpbySigPtr[i+2]);
				printf("\n");

				printf("\tBytes from file: ");
				for (i=0;i<8;i++)
					printf("%02X ",
							lpbyScanThis[i*lpbySigPtr[nCurSigIndex+1]]);
				printf("\n");
				printf("\tEncryptedSearch status: ");
			}
#endif


			if (byTemp & ALGX_SCAN_XOR_DELTA_MASK)
			{
				if (DecryptXORDelta(lpbySigPtr + nCurSigIndex,lpbyScanThis))
				{
					if (byTemp & ALGX_SCAN_WORD_MASK)
						*lpnCurSigIndex += ALGX_ENCRYPTED_WORD_SIG_SKIP;
					else
						*lpnCurSigIndex += ALGX_ENCRYPTED_BYTE_SIG_SKIP;

					*lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
					if (gDisplayDebugInfo)
					{
						printf("MATCH (XOR/DELTA)\n");
					}
#endif
					return(ENGSTATUS_OK);
				}
			}

			if (byTemp & ALGX_SCAN_ADD_DELTA_MASK)
			{
				if (DecryptAddDelta(lpbySigPtr + nCurSigIndex,lpbyScanThis))
				{
					if (byTemp & ALGX_SCAN_WORD_MASK)
						*lpnCurSigIndex += ALGX_ENCRYPTED_WORD_SIG_SKIP;
                    else
                        *lpnCurSigIndex += ALGX_ENCRYPTED_BYTE_SIG_SKIP;

                    *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
                    if (gDisplayDebugInfo)
                    {
                        printf("MATCH (ADD/DELTA)\n");
                    }
#endif
                    return(ENGSTATUS_OK);
                }
            }

            if (byTemp & ALGX_SCAN_ROTATE_MASK)
            {
                if (DecryptRotate(lpbySigPtr + nCurSigIndex,lpbyScanThis))
                {
                    if (byTemp & ALGX_SCAN_WORD_MASK)
                        *lpnCurSigIndex += ALGX_ENCRYPTED_WORD_SIG_SKIP;
                    else
                        *lpnCurSigIndex += ALGX_ENCRYPTED_BYTE_SIG_SKIP;

                    *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
                    if (gDisplayDebugInfo)
                    {
                        printf("MATCH (ROTATE)\n");
                    }
#endif

                    return(ENGSTATUS_OK);
                }
            }

            if (byTemp & ALGX_SCAN_NEG_MASK)
            {
                if (DecryptNeg(lpbySigPtr + nCurSigIndex,lpbyScanThis))
                {
                    if (byTemp & ALGX_SCAN_WORD_MASK)
                        *lpnCurSigIndex += ALGX_ENCRYPTED_WORD_SIG_SKIP;
                    else
                        *lpnCurSigIndex += ALGX_ENCRYPTED_BYTE_SIG_SKIP;

                    *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
                    if (gDisplayDebugInfo)
                    {
                        printf("MATCH (NEG)\n");
                    }
#endif

                    return(ENGSTATUS_OK);
                }
            }

            if (byTemp & ALGX_SCAN_NOT_MASK)
            {
                if (DecryptNot(lpbySigPtr + nCurSigIndex,lpbyScanThis))
                {
                    if (byTemp & ALGX_SCAN_WORD_MASK)
                        *lpnCurSigIndex += ALGX_ENCRYPTED_WORD_SIG_SKIP;
                    else
                        *lpnCurSigIndex += ALGX_ENCRYPTED_BYTE_SIG_SKIP;

                    *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
                    if (gDisplayDebugInfo)
                    {
                        printf("MATCH (NOT)\n");
                    }
#endif

                    return(ENGSTATUS_OK);
                }
            }

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("FAIL\n");
            }
#endif

            *lpbMatch = FALSE;

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_FOLLOWJUMP:

            // this function can not be used after a ScanHeader,
            //  although it will work in cases where it is a COM file
            //  and the FSTART buffer begins at the TOF (e.g., VID 0x2188)

            // the determine the type of the jump (1 or 2 byte) and compute
            // the relative displacement...

            if (*(lpbyScanThis-1) == JUMP_1_BYTE ||
                (*(lpbyScanThis-1) & MASK_HIGH_NIBBLE) ==
                    JUMP_CONDITIONAL_HIGH_NIBBLE)
            {
                wTemp = (signed short)(signed char)*lpbyScanThis++;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("\tFollowJump(): MATCH (one byte jump 0x%02X detected, displacement is %02x)\n",
                            *(lpbyScanThis-1),
                            *lpbyScanThis);
                }
#endif
            }
            else
            {
                wTemp = AVDEREF_WORD(lpbyScanThis);
                lpbyScanThis += 2;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("\tFollowJump(): MATCH (two byte jump assumed, displacement is %04x)\n",
                            wTemp);
                }
#endif
            }

            // determine what our offset is in our FStart or Floating buffer

            if (lpbyScanThis >= lpAlgScanThis->stFStartData.
                    byFStartBuffer &&
                lpbyScanThis <= lpAlgScanThis->stFStartData.
                    byFStartBuffer + FSTART_BUFFER_SIZE - 1)
            {
                wBufferOffset = lpbyScanThis -
                    lpAlgScanThis->stFStartData.byFStartBuffer;
            }
            else
            if (lpbyScanThis >= lpAlgScanThis->stFStartData.
                    byTOFBuffer &&
                lpbyScanThis <= lpAlgScanThis->stFStartData.
                    byTOFBuffer + FSTART_BUFFER_SIZE - 1)
            {
                wBufferOffset = lpbyScanThis -
                    lpAlgScanThis->stFStartData.byTOFBuffer;
            }
            else
            {
                wBufferOffset = lpbyScanThis -
                    lpAlgScanThis->lpbyFloatingBuffer;
            }

            // compute the current IP

            wIP = (WORD)(*lpdwCurOffset - lpFStartInfo->dwSegmentBase);
			wIP += wBufferOffset;

            // compute the destination IP

            wIP += wTemp;

            // compute the location of this IP in the entire file

            dwTemp = lpFStartInfo->dwSegmentBase + wIP;

            // and re-fill our floating buffer...

            if (FillFloatingBuffer(lpFileObject,
                                   &(lpAlgScanThis->stEngCache),
                                   lpAlgScanThis,
                                   lplpbyFStartPtr,
                                   dwTemp,
                                   lpdwCurOffset) == ENGSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            *lpnCurSigIndex = nCurSigIndex;
            *lpbMatch = TRUE;

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_SCANMASK:

            // determine how far we're going to slide for our scanmask...

            nSearchDist = (unsigned int)(BYTE)(lpbySigPtr[++nCurSigIndex] - 1);

            byScanByte = (lpbySigPtr[++nCurSigIndex] - 1);
            byScanMask = (lpbySigPtr[++nCurSigIndex] - 1);

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tScanMask(%d, %02X, %02X): ",
                        nSearchDist,
                        byScanByte,
                        byScanMask);
            }
#endif

            for (i=0;i<nSearchDist;i++)
                if ((*lpbyScanThis++ & byScanMask) == byScanByte)
                {
                    *lplpbyFStartPtr = lpbyScanThis;
                    *lpnCurSigIndex = nCurSigIndex;

                    *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
                    if (gDisplayDebugInfo)
                    {
                        int j;

                        printf("MATCH (Found 0x%02X after sliding %d bytes)\n",
                                *(lpbyScanThis-1),
                                i);

                        printf("\tFSTART: ");
                        for (j=0;j<16;j++)
                            printf("%02X ", lpbyScanThis[j]);
                        printf("\n");
                    }
#endif
                    return(ENGSTATUS_OK);
                }

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("FAIL\n");
            }
#endif
            *lpbMatch = FALSE;

            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        case ALGX_FUNC_ENTRYPOINT_IMAGE:

            // determine whether or not the entry-point is within the specified
            // range based on the load image size of the program...  This only
            // applies for EXE files.  The function is the same as
            // ALGX_FUNC_ENTRYPOINT_FILESIZE for COM files.

            if (lpFStartInfo->wFileType == FSTART_EXE_FILE_TYPE)
            {
                wNumPages = AVDEREF_WORD(lpAlgScanThis->stFStartData.
												byTOFBuffer +
                                                OFFSET_OF_SIZE_IN_PAGES);
                wMod512 = AVDEREF_WORD(lpAlgScanThis->stFStartData.
												byTOFBuffer +
                                                OFFSET_OF_MOD_512);

                // the number of pages count includes any leftover bytes that
                // do not comprise a full 512 byte page.  To calculate the
                // actual load image size, we do the following:
                // 1. check if wMod512 is non zero.  If so, the file has a
                //    load image which is (wNumPages-1)*512 + wMod512 bytes
                // 2. if wMod512 is zero, then the file has a load image which
                //    is wNumPages * 512 + wMod512.

                if (wMod512 != 0)
                    wNumPages--;

                dwImageSize = (DWORD)wNumPages * (DWORD)512 + (DWORD)wMod512;
            }
            else
                dwImageSize = lpAlgScanThis->stEngCache.dwFileLength;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tCheckEntryptImage( ");
            }
#endif
            // now fall through to the comparison against the bound values

///////////////////////////////////////////////////////////////////////////////

		case ALGX_FUNC_ENTRYPOINT_FILESIZE:

			if (byWildByte == ALGX_FUNC_ENTRYPOINT_FILESIZE)
            {
                dwImageSize = lpAlgScanThis->stEngCache.dwFileLength;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("\tCheckEntryptFilesize( ");
                }
#endif
            }

            // our program entry-point has already been computed.  use it to
            // determine the distance from entry-point to EOF (or EOImage)...

			dwEntryDist = dwImageSize - lpFStartInfo->dwFirstLandingOffset;

            // get the lower entrypoint bound...
            wLow = (BYTE)(lpbySigPtr[++nCurSigIndex] - 1);
            wLow |= ((WORD)(BYTE)(lpbySigPtr[++nCurSigIndex] - 1)) << 8;

            // get the upper entrypoint bound...

            wHi = (BYTE)(lpbySigPtr[++nCurSigIndex] - 1);
            wHi |= ((WORD)(BYTE)(lpbySigPtr[++nCurSigIndex] - 1)) << 8;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
				printf("%u,%u): ",wLow,wHi);
            }
#endif

            if (dwEntryDist < wLow)
            {
                *lpbMatch = FALSE;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("FAIL (Entrypoint distance of %lu is less than lower bound)\n",
                            dwEntryDist);
                }
#endif
                return(ENGSTATUS_OK);
            }

            if (dwEntryDist > wHi)
            {
                *lpbMatch = FALSE;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("FAIL (Entrypoint distance of %lu is greater than upper bound)\n",
                            dwEntryDist);
                }
#endif
                return(ENGSTATUS_OK);
            }

            *lpnCurSigIndex = nCurSigIndex;

            *lpbMatch = TRUE;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("MATCH (Entrypoint distance of %lu falls in bounds)\n",
                            dwEntryDist);
            }
#endif
            return(ENGSTATUS_OK);

///////////////////////////////////////////////////////////////////////////////

        default:

            *lpbMatch = FALSE;

#ifdef      AVENGE_DEBUG
            if (gDisplayDebugInfo)
            {
                printf("\tUnsupportedFunction(): FAIL\n");
            }
#endif
            return(ENGSTATUS_OK);
    }
}





//********************************************************************
//
// Function:
//  AlgMatchString()
//
// Parameters:
//  lpFileObject            A file object for the file to be scanned
//  lpAlgSigData            Algorithmic signature data (pool pointers and hash
//                          tables)
//  lpAlgScanThis           Contains data read from the target file.  This data
//                          will be scruitinzed or updated by the various
//                          algoritmhic functions.
//  lpFStartInfo            Information from FSTART on the file type,
//                          first landing, etc.
//  lpbFoundSig             Did we match our entire signature?
//  lpnPartsMatched         How many atomic components of our signature did
//                          we match?  Non-alg bytes are counted 1 part per
//                          part.  Alg bytes are counted 1 part per function.
//
// Description:
//
//  This function applies an entire single signature to the scan data of the
//  target file.  It updates the lpnPartsMatched to reflect how many components
//  of the signature actually matched with the file scan data.  This info.
//  is used to skip over signatures with the same prefix content.  The value
//  *lpbFoundSig will be set to TRUE if the sig matched or FALSE otherwise.
//
// Returns:
//  ENGSTATUS_OK            If no errors were encountered during the
//                          application of the current signature.
//  ENGSTATUS_ERROR         If a file error occured during application
//
//********************************************************************

ENGSTATUS AlgMatchString
(
    LPFILEOBJECT    lpFileObject,
    LPALG_SIG_DATA  lpAlgSigData,
    LPALGSCANDATA   lpAlgScanThis,
    LPFSTARTINFO    lpFStartInfo,
    LPBOOL          lpbFoundSig,
    LPINT           lpnPartsMatched
)
{
    int             nSigLen, nCurSigIndex, nPartsMatched;
    BYTE            byDecodedSigByte;
    DWORD           dwCurOffset;
    LPBYTE          lpbyFStartPtr;
    BOOL            bMatch;

    // so far no matches..

#ifdef      AVENGE_DEBUG

	{
        int                             i;
        BOOL                            bDebug;
        static  VIRUS_USER_INFO_T       stVirusUserInfo;
        static  WORD                    wVirusRecordIndex = 0xFFFF;

        if (gwDebugVID != 0)
        {
            if (wVirusRecordIndex == 0xFFFF)
            {
                EngGetUserInfoHVIRUS(gDebughGEng,
                                     lpAlgSigData->wVirusRecordIndex,
                                     &stVirusUserInfo);

                if (stVirusUserInfo.wVirusID == gwDebugVID)
                {
                    // remember where the record with the matching VID is.

                    wVirusRecordIndex = lpAlgSigData->wVirusRecordIndex;
                }
            }

            if (lpAlgSigData->wVirusRecordIndex == wVirusRecordIndex)
                bDebug = TRUE;
            else
                bDebug = FALSE;

            if (gwDebugVID == 0xFFFF)
                bDebug = TRUE;

            if (bDebug)
            {
                gDisplayDebugInfo = TRUE;

                printf("\nApplying ALG Signature 0x%04X (%s)\n",
                        stVirusUserInfo.wVirusID,
                        stVirusUserInfo.sVirusName);

                printf("FSTART (Offset %ld): ",lpFStartInfo->dwFStartBufferOffset);
                for (i=0;i<16;i++)
                    printf("%02X ",lpAlgScanThis->stFStartData.byFStartBuffer[i]);
                printf("\n");
            }
            else
                gDisplayDebugInfo = FALSE;
        }
	}

#endif

	bMatch = FALSE;
	nCurSigIndex = nPartsMatched = 0;

	// point our lpbyFStartPtr at our file data (provided by FSTART)

	lpbyFStartPtr = lpAlgScanThis->stFStartData.byFStartBuffer;

	// note: the signature length is NOT encoded like the sigs.

	nSigLen = (int)(BYTE)lpAlgSigData->bySigLength;

	// remember where our current buffer is from so we can do followjumps...

	dwCurOffset = lpFStartInfo->dwFStartBufferOffset;

	// Note: the new engine does not require there to be an extra 0xDF
	// at the top of the signature to perform an algorithmic function
	// immediately.

	for (;nCurSigIndex<nSigLen;nCurSigIndex++)
	{
		byDecodedSigByte = lpAlgSigData->bySigData[nCurSigIndex] - 1;

		if ((byDecodedSigByte & MASK_HIGH_NIBBLE) == WILDCARD_HIGH_NIBBLE)
		{
			if (AlgMatchWild(lpFileObject,
							 lpAlgSigData,
							 byDecodedSigByte,
							 lpAlgScanThis,
							 lpFStartInfo,
							 &nCurSigIndex,
							 &lpbyFStartPtr,
							 &dwCurOffset,
							 &bMatch) == ENGSTATUS_ERROR)
				return(ENGSTATUS_ERROR);

			if (bMatch == FALSE)
			{
				*lpbFoundSig = FALSE;
				*lpnPartsMatched = nPartsMatched;

				return(ENGSTATUS_OK);
			}
		}
		else
		{
#ifdef      AVENGE_DEBUG
			if (gDisplayDebugInfo)
			{
				printf("\tSig:0x%02X==Host:0x%02X: ",
						byDecodedSigByte,
						*lpbyFStartPtr);
			}
#endif

			if (byDecodedSigByte == *lpbyFStartPtr)
			{
				lpbyFStartPtr++;

#ifdef      AVENGE_DEBUG
			if (gDisplayDebugInfo)
			{
				printf("MATCH\n");
			}
#endif

			}
			else
			{
                *lpbFoundSig = FALSE;
                *lpnPartsMatched = nPartsMatched;

#ifdef      AVENGE_DEBUG
                if (gDisplayDebugInfo)
                {
                    printf("FAIL\n");
                }
#endif

                return(ENGSTATUS_OK);
            }
        }

        // if we've got this far we've matched the function/byte and are
        // going on to the next function/byte

        nPartsMatched++;
    }

	// no need to return # matched parts if we match the whole thing...

	*lpbFoundSig = TRUE;

    return(ENGSTATUS_OK);
}


//********************************************************************
//
// Function:
//  AlgApplyBucketedSigs()
//
// Parameters:
//  lpFileObject            A file object for the file to be scanned
//  lpAlgScanThis           Contains data read from the target file.  This data
//                          will be scruitinzed or updated by the various
//                          algoritmhic functions.
//  lpFStartInfo            Information from FSTART on the file type,
//                          first landing, etc.
//  lpAlgScanData           Alg scanning data.  This data structure points to
//                          the pools of alg sigs as well as the alg hash
//                          tables.
//  lpAlgHashEntry          This is a pointer to a bucket from one of the
//                          hash tables which specifies which pool contains
//                          the applicable ALG signatures, where in the pool
//                          those signatures are, and how many signatures there
//                          are to be applied.
//  lpbFoundSig             Was a signature found? (*lpbFoundSig = TRUE/FALSE)
//  lphVirus                This points to an hVirus that is set to a virus
//                          index if a virus is indeed detected.
//
//
// Description:
//
//  This function applies all signatures that are chained off a given hash
//  table bucket to the target file.  If it finds a signature, it will set
//  *lpbFoundSig to TRUE, set *lphVirus to the virus's index in the
//  VIRSCAN1.DAT and return ENGSTATUS_OK.  If no viruses are located,
//  *lpbFoundSig will be set to FALSE.
//
// Returns:
//  ENGSTATUS_OK            If no errors were encountered during the
//                          application of the signatures.
//  ENGSTATUS_ERROR         If a file error occured during application
//
//********************************************************************

ENGSTATUS AlgApplyBucketedSigs
(
    LPFILEOBJECT        lpFileObject,
    LPALGSCANDATA       lpAlgScanThis,
    LPFSTARTINFO        lpFStartInfo,
    LPALG_SCAN_INFO     lpAlgScanData,
    LPALG_HASH_ENTRY    lpAlgHashEntry,
    LPBOOL              lpbFoundSig,
    LPHVIRUS            lphVirus
)
{
    LPALG_BUFF_ENTRY    lpPoolInfo;
    LPALG_SIG_DATA      lpAlgSigData;
    LPBYTE              lpbyPoolPtr, lpbyEndOfPool;
    WORD                wNumSigs;
    int                 nPartsMatched, nCurPoolNum;

	// determine which pool contains our first signature...

	nCurPoolNum = (int)(WORD)lpAlgHashEntry->byPoolNum;

	// are there any signatures off this hash cell?

	wNumSigs = lpAlgHashEntry->wNumSigs;

	if (wNumSigs != 0 && nCurPoolNum != (int)(WORD)INVALID_ALG_POOL_NUM)
	{
		// get a quick pointer to our pool info

		lpPoolInfo = &lpAlgScanData->lpPoolInfoStruct->
						AlgPoolArray[nCurPoolNum];

		// get a pointer to the start of the pool

		lpbyPoolPtr = lpPoolInfo->lpbyAlgPoolPtr;

		// point to our first signature (at wPoolOffset in the pool)

		lpAlgSigData = (LPALG_SIG_DATA)(lpbyPoolPtr+
										lpAlgHashEntry->wPoolOffset);

		// find where the end of the current pool is

		lpbyEndOfPool = lpbyPoolPtr + lpPoolInfo->wBuffSize;

		do
		{
            // don't scan with deleted signatures...

            if (lpAlgSigData->wVirusRecordIndex !=
                INVALID_VIRUS_HANDLE)
            {
                // apply that signature...

                if (AlgMatchString(lpFileObject,
                                   lpAlgSigData,
                                   lpAlgScanThis,
                                   lpFStartInfo,
                                   lpbFoundSig,
                                   &nPartsMatched) == ENGSTATUS_ERROR)
                    return(ENGSTATUS_ERROR);

                if (*lpbFoundSig == TRUE)
                {
                    *lphVirus = (HVIRUS)lpAlgSigData->wVirusRecordIndex;

                    return(ENGSTATUS_OK);
                }
            }

			// advance to the next signature in the pool...

            lpAlgSigData = (LPALG_SIG_DATA)
                             ((LPBYTE)lpAlgSigData +
                                (int)(BYTE)lpAlgSigData->bySigLength +
                                sizeof(ALG_SIG_HDR_T));

			// see if we've hit the end of the current pool...

            if ((LPBYTE)lpAlgSigData >= lpbyEndOfPool)
			{
				// advance to the next pool...

				lpPoolInfo++;

                // update our lpAlgSigData to point to the start of the next
                // pool. also compute the end of this pool.

				lpAlgSigData = (LPALG_SIG_DATA)lpPoolInfo->lpbyAlgPoolPtr;
				lpbyEndOfPool = (LPBYTE)lpAlgSigData + lpPoolInfo->wBuffSize;
			}
        } while (--wNumSigs > 0);
	}

	// no virus found...

    return(ENGSTATUS_OK);
}



//********************************************************************
//
// Function:
//  AlgApplyAllSigs()
//
// Parameters:
//  lpFileObject            A file object for the file to be scanned
//  lpAlgScanThis           Contains data read from the target file.  This data
//                          will be scruitinzed or updated by the various
//                          algoritmhic functions.
//  lpFStartInfo            Information from FSTART on the file type,
//                          first landing, etc.
//  lpAlgScanData           Alg scanning data.  This data structure points to
//                          the pools of alg sigs as well as the alg hash
//                          tables.
//  lpbFoundSig             Was a signature found? (*lpbFoundSig = TRUE/FALSE)
//  lphVirus                This points to an hVirus that is set to a virus
//                          index if a virus is indeed detected.
//
//
// Description:
//
//  This function applies all applicable signatures specified by lpAlgScanThis
//  to the target file. If it finds a signature, it will set *lpbFoundSig to
//  TRUE, set *lphVirus to the virus's index in the VIRSCAN1.DAT and return
//  ENGSTATUS_OK.  If no viruses are located, *lpbFoundSig will be set to
//  FALSE.
//
// Returns:
//  ENGSTATUS_OK            If no errors were encountered during the
//                          application of the signatures
//  ENGSTATUS_ERROR         If a file error occured during application
//
//********************************************************************

ENGSTATUS AlgApplyAllSigs
(
    LPFILEOBJECT        lpFileObject,
    LPALGSCANDATA       lpAlgScanThis,
    LPFSTARTINFO        lpFStartInfo,
    LPALG_SCAN_INFO     lpAlgScanData,
    LPBOOL              lpbFoundSig,
    LPHVIRUS            lphVirus
)
{
    DWORD               dwHashValue;
    WORD                wHashValue;
    BYTE                byHashValue;

    // Set initial floating buffer pointer

    lpAlgScanThis->lpbyFloatingBuffer =
        lpAlgScanThis->stEngCache.byEOFCache + ENG_EOF_CACHE_SIZE;

    // reset our found status first...

	*lpbFoundSig = FALSE;

	// There are four stages to alg scanning: 4-byte hash sigs, 2-byte hash
	// sigs, 1-byte hash sigs and no-hash sigs.

	// 4-byte hash

	dwHashValue = DWENDIAN(*(LPDWORD)
						lpAlgScanThis->stFStartData.byFStartBuffer) %
						lpAlgScanData->wFourByteHashSize;

	if (AlgApplyBucketedSigs(lpFileObject,
							 lpAlgScanThis,
							 lpFStartInfo,
							 lpAlgScanData,
							 &lpAlgScanData->
								lpFourByteTable[(WORD)dwHashValue],
							 lpbFoundSig,
                             lphVirus) == ENGSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

	if (*lpbFoundSig == TRUE)
        return(ENGSTATUS_OK);

	// 2-byte hash

	wHashValue = WENDIAN(*(LPWORD)
						lpAlgScanThis->stFStartData.byFStartBuffer) %
						lpAlgScanData->wTwoByteHashSize;

	if (AlgApplyBucketedSigs(lpFileObject,
							 lpAlgScanThis,
							 lpFStartInfo,
							 lpAlgScanData,
							 &lpAlgScanData->
								lpTwoByteTable[wHashValue],
							 lpbFoundSig,
                             lphVirus) == ENGSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

	if (*lpbFoundSig == TRUE)
        return(ENGSTATUS_OK);

	// 1-byte hash

    byHashValue = (BYTE)
        ((WORD)lpAlgScanThis->stFStartData.byFStartBuffer[0] %
                        lpAlgScanData->wOneByteHashSize);

	if (AlgApplyBucketedSigs(lpFileObject,
							 lpAlgScanThis,
							 lpFStartInfo,
							 lpAlgScanData,
							 &lpAlgScanData->
								lpOneByteTable[byHashValue],
							 lpbFoundSig,
                             lphVirus) == ENGSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

	if (*lpbFoundSig == TRUE)
        return(ENGSTATUS_OK);

	// no hash signatures

	if (AlgApplyBucketedSigs(lpFileObject,
							 lpAlgScanThis,
							 lpFStartInfo,
							 lpAlgScanData,
							 &lpAlgScanData->stNoHashInfo,
							 lpbFoundSig,
                             lphVirus) == ENGSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

	// successful scan...

    return(ENGSTATUS_OK);
}

