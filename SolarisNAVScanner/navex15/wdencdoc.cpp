//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WDENCDOC.CPv   1.2   13 Feb 1997 13:34:44   DCHI  $
//
// Description:
//  Module containing functions for finding encryption key.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WDENCDOC.CPv  $
// 
//    Rev 1.2   13 Feb 1997 13:34:44   DCHI
// Modifications to support VBA 5 scanning.
// 
//    Rev 1.1   17 Jan 1997 19:40:16   DCHI
// Loosened restrictions on password cracking.
// 
//    Rev 1.0   17 Jan 1997 11:23:38   DCHI
// Initial revision.
// 
//************************************************************************

#include "storobj.h"
#include "wdencdoc.h"
#include "worddoc.h"
#include "endutils.h"

//*************************************************************************
//
// WORD WordDocDeduceKey()
//
// Parameters:
//  lpstKey             The reverse key from which to deduce the key
//  lpbyBuf             Buffer of bytes to check for a matching key
//  lpabyKey            Array of BYTES to store key if found
//
// Description:
//  Searches the given buffer for a key using the given reverse key.
//
// Returns:
//  WORDDOC_OK          If a key was found
//  WORDDOC_ERR_READ    If a key was not found
//
//*************************************************************************

WORD WordDocDeduceKey
(
    LPWDREVKEY          lpstKey,
    LPBYTE              lpbyBuf,
    LPBYTE              lpabyKey
)
{
    int                 i;
    BYTE                byA, byB;
    BYTE                byP, byQ;

    for (i=0;i<WD_ENC_KEY_LEN;i++)
    {
        // The encrypted text

        byA = lpbyBuf[i];
        byB = lpbyBuf[i + WD_ENC_KEY_LEN];

        if (byA == 0 && byB == 0)
        {
            // Impossible to determine key in this case

            return(WORDDOC_ERR_READ);
        }

        // The plain text to use

        byP = (lpstKey->abyRevKey0[i] - 1);
        if (i < lpstKey->byHashLen)
            byQ = (lpstKey->abyRevKey1[i] - 1);

        if (byA == 0)
        {
            if (byP != 0)
            {
                // Impossible to determine key in this case

                return(WORDDOC_ERR_READ);
            }

            // byA == 0 == byP at this point

            if (lpstKey->byHashLen > i && byB != byQ)
            {
                // This cannot be the right place

                return(WORDDOC_ERR_READ);
            }

            // Assume K=B, this is iffy when lpstKey->byHashLen <= i

            lpabyKey[i] = byB;
        }
        else
        if (byB == 0)
        {
            if (lpstKey->byHashLen > i)
            {
                if (byQ != 0)
                {
                    // The key is not here

                    return(WORDDOC_ERR_READ);
                }

                if (byA != byP)
                {
                    // This cannot be the right place

                    return(WORDDOC_ERR_READ);
                }

                // Assume K=A

                lpabyKey[i] = byA;
            }
            else
            {
                // Assume K = A ^ P

                lpabyKey[i] = byA ^ byP;
            }
        }
        else
        {
            // byA != 0 && byB != 0

            // P must not be 0 and Q must not be 0

            if (byP == 0 || (lpstKey->byHashLen > i && byQ == 0))
            {
                // The key is not here

                return(WORDDOC_ERR_READ);
            }

            if (byA == byB || byA == byP)
            {
                // byP == byQ must be true if hashLen > i
                //  otherwise, iffy

                lpabyKey[i] = byA;
            }
            else
            if (lpstKey->byHashLen > i && byB == byQ)
            {
                lpabyKey[i] = byB;
            }
            else
            {
                // Assume K = A xor P = B xor Q

                lpabyKey[i] = byA ^ byP;
            }
        }
    }

    return(WORDDOC_OK);
}


//*************************************************************************
//
// WORD WordDocCheckRing()
//
// Parameters:
//  lpstRing            The ring of keys to check
//  lpalpstRevKeys      Ptr to the reverse key array
//  lpbyBuf             Buffer of bytes to check for a matching key
//  lpbyHash            The hash that consists of the XOR of the first
//                          sixteen bytes of the buffer with the second
//                          sixteen bytes of the buffer, except for the
//                          special rule that if either bytes is zero,
//                          the corresponding hash byte is zero.
//  wStartIdx           The hash is a circular buffer of size
//                          WD_ENC_KEY_LEN.  This is the index of the
//                          first hash byte in the buffer.
//  lpstKey             Key structure to store key if found
//
// Description:
//  Searches the given buffer for a key from the given ring.
//
// Returns:
//  WORDDOC_OK          If a key was found
//  WORDDOC_ERR_READ    If a key was not found
//
//*************************************************************************

WORD WordDocCheckRing
(
    LPWDREVKEYRING      lpstRing,
    LPLPWDREVKEY        lpalpstRevKeys,
    LPBYTE              lpbyBuf,
    LPBYTE              lpbyHash,
    WORD                wStartIdx,
    LPWDENCKEY          lpstKey
)
{
    WORD                w;
    WORD                wNextRevKeyIdx;
    int                 i;
    LPWDREVKEY          lpstRevKey;
    int                 nHashLen;
    LPBYTE              lpbyTargetP;
    LPBYTE              lpbyTargetQ;
    BYTE                byHash, byP, byQ;

    // For now, just do a linear search

    wNextRevKeyIdx = lpstRing->wFirstRevKeyIdx;
    for (w=0;w<lpstRing->wKeyCount;w++,wNextRevKeyIdx++)
    {
        lpstRevKey = lpalpstRevKeys[wNextRevKeyIdx];
        nHashLen = lpstRevKey->byHashLen;
        lpbyTargetP = lpstRevKey->abyRevKey0 + 1;
        lpbyTargetQ = lpstRevKey->abyRevKey1 + 1;
        for (i=1;i<nHashLen;i++)
        {
            byHash = lpbyHash[(wStartIdx + i) & (WD_ENC_KEY_LEN - 1)];
            byP = *lpbyTargetP++ - 1;
            byQ = *lpbyTargetQ++ - 1;
            if (byP == 0 || byQ == 0)
            {
                if (byHash != 0)
                    break;
            }
            else
            {
                if (byHash != (byP ^ byQ) &&
                    byP != lpbyBuf[i] &&
                    byQ != lpbyBuf[i+WD_ENC_KEY_LEN])
                    break;
            }
        }

        if (i == nHashLen)
        {
            // It was a match on the hash

            // Now try to see whether a valid key can be obtained

            if (WordDocDeduceKey(lpstRevKey,
                                 lpbyBuf,
                                 lpstKey->abyKey) == WORDDOC_OK)
            {
                // Found a candidate key

                lpstKey->wID = wNextRevKeyIdx;

                return(WORDDOC_OK);
            }
        }
    }

    return(WORDDOC_ERR_READ);
}


//*************************************************************************
//
// WORD WordDocFindKeyInBuf()
//
// Parameters:
//  lpbyChunk           The chunk to check
//  wChunkSize          Size of the chunk (must be >= 32)
//  lpstLocker          The locker of reverse key rings
//  lpwKeyOffset        If key is found, *lpwKeyOffset is set
//                          to the offset of the key in the chunk
//  lpstKey             Key structure to store key if found
//
// Description:
//  Searches the given buffer for a key from the given locker.
//
// Returns:
//  WORDDOC_OK          If a key was found
//  WORDDOC_ERR_READ    If a key was not found
//
//*************************************************************************

WORD WordDocFindKeyInBuf
(
    LPBYTE              lpbyChunk,
    WORD                wChunkSize,
    LPWDREVKEYLOCKER    lpstLocker,
    LPWORD              lpwKeyOffset,
    LPWDENCKEY          lpstKey
)
{
    BYTE                byRingIdx;
    WORD                wOffset;
    WORD                wStartIdx;
    BYTE                abyHash[WD_ENC_KEY_LEN];

    // XOR the first 16 bytes of the first 16 bytes of the buffer
    //  with the second 15 bytes of the second 16 bytes

    for (wOffset=0;wOffset<WD_ENC_KEY_LEN;wOffset++)
    {
        if (lpbyChunk[wOffset] == 0 ||
            lpbyChunk[wOffset + WD_ENC_KEY_LEN] == 0)
        {
            // Special case

            abyHash[wOffset] = 0;
        }
        else
        {
            // Normal case

            abyHash[wOffset] = lpbyChunk[wOffset] ^
                               lpbyChunk[wOffset + WD_ENC_KEY_LEN];
        }
    }

    wStartIdx = 0;
    while (1)
    {
        // See whether there is such a ring

        byRingIdx = lpstLocker->abyRevKeyIdx[abyHash[wStartIdx]];

        if (byRingIdx != 0)
        {
            // Check the ring

            if (WordDocCheckRing(lpstLocker->lpastRing + byRingIdx - 1,
                                 lpstLocker->lpalpstRevKey,
                                 lpbyChunk + wOffset - WD_ENC_KEY_LEN,
                                 abyHash,
                                 wStartIdx,
                                 lpstKey) == WORDDOC_OK)
            {
                // Found a match

                *lpwKeyOffset = wOffset - WD_ENC_KEY_LEN;

                return(WORDDOC_OK);
            }
        }

        if ((wOffset + WD_ENC_KEY_LEN) > wChunkSize)
        {
            // No match in this chunk

            return(WORDDOC_ERR_READ);
        }

        abyHash[wStartIdx] = lpbyChunk[wOffset] ^
                             lpbyChunk[wOffset + WD_ENC_KEY_LEN];

        wStartIdx = (wStartIdx + 1) & (WD_ENC_KEY_LEN - 1);

        ++wOffset;
    }
}


//*************************************************************************
//
// WORD WordDocFindNextKey()
//
// Parameters:
//  lpstCallBack        File op callbacks
//  lpstOLEFile         WordDoc stream OLE file info
//  lpbyWorkBuffer      Work buffer >= 512 bytes
//  lpstKey             Key structure to store key if found
//  lpstLocker          Locker of reverse keys for deducing key
//
// Description:
//  The function should not be called if it is not encrypted.
//  The function tries to find the next key,
//
// Returns:
//  WORDDOC_OK          If a key was found
//  WORDDOC_ERR_READ    If a key was not found
//
//*************************************************************************

WORD WordDocFindNextKey
(
    LPCALLBACKREV1      lpstCallBack,
    LPOLE_FILE_T        lpstOLEFile,
    LPBYTE              lpbyWorkBuffer,
    LPWDENCKEY          lpstKey,
    LPWDREVKEYLOCKER    lpstLocker
)
{
    DWORD               dwOffset;
    WORD                wChunkSize;
    WORD                wKeyOffset;

    // Continue from the last offset

    dwOffset = lpstKey->dwReverseKeyOffset + 1;
    while (dwOffset < lpstOLEFile->siStreamInfo.dwStreamLen)
    {
        // Read in 512 byte chunks

        if (dwOffset + 512 <= lpstOLEFile->siStreamInfo.dwStreamLen)
            wChunkSize = 512;
        else
            wChunkSize = (WORD)(lpstOLEFile->siStreamInfo.dwStreamLen -
                dwOffset);

        if (wChunkSize < WD_ENC_KEY_LEN + WD_ENC_KEY_LEN)
        {
            // Not enough bytes left to search

            lpstKey->dwReverseKeyOffset =
                lpstOLEFile->siStreamInfo.dwStreamLen;

            return(WORDDOC_ERR_READ);
        }

        if (OLESeekRead(lpstCallBack,
                        lpstOLEFile,
                        dwOffset,
                        lpbyWorkBuffer,
                        wChunkSize) != wChunkSize)
        {
            // Error reading chunk

            lpstKey->dwReverseKeyOffset =
                lpstOLEFile->siStreamInfo.dwStreamLen;

            return(WORDDOC_ERR_READ);
        }

        // Search for key

        if (WordDocFindKeyInBuf(lpbyWorkBuffer,
                                wChunkSize,
                                lpstLocker,
                                &wKeyOffset,
                                lpstKey) == WORDDOC_OK)
        {
            int         i;

            // Found one

            lpstKey->dwReverseKeyOffset = dwOffset + wKeyOffset;

            // Rotate key for 16-byte alignment

            for (i=0;i<WD_ENC_KEY_LEN;i++)
            {
                lpbyWorkBuffer[(i + lpstKey->dwReverseKeyOffset) %
                    WD_ENC_KEY_LEN] = lpstKey->abyKey[i];
            }

            // Copy aligned key back into key structure

            for (i=0;i<WD_ENC_KEY_LEN;i++)
                lpstKey->abyKey[i] = lpbyWorkBuffer[i];

            return(WORDDOC_OK);
        }

        dwOffset += wChunkSize - WD_ENC_KEY_LEN - WD_ENC_KEY_LEN + 1;
    }

    // Didn't find any key candidates

    lpstKey->dwReverseKeyOffset = lpstOLEFile->siStreamInfo.dwStreamLen;

    return(WORDDOC_ERR_READ);
}


//*************************************************************************
//
// WORD WordDocFindFirstKey()
//
// Parameters:
//  lpstCallBack        File op callbacks
//  lpstOLEFile         WordDoc stream OLE file info
//  lpbyWorkBuffer      Work buffer >= 512 bytes
//  lpstKey             Key structure to store key if found
//  lpstLocker          Locker of reverse keys for deducing key
//
// Description:
//  Determines whether the document is encrypted.  If encrypted,
//  then tries to find the first key.
//
// Returns:
//  WORDDOC_OK          If a key was found
//  WORDDOC_ERR_READ    If a key was not found
//
//*************************************************************************

WORD WordDocFindFirstKey
(
    LPCALLBACKREV1      lpstCallBack,
    LPOLE_FILE_T        lpstOLEFile,
    LPBYTE              lpbyWorkBuffer,
    LPWDENCKEY          lpstKey,
    LPWDREVKEYLOCKER    lpstLocker
)
{
    LPWD_FIB            lpstFIB;

    // Read the first 48 bytes of the FIB(File Information Block)

    if (OLESeekRead(lpstCallBack,lpstOLEFile,0,lpbyWorkBuffer,48) != 48)
        return(WORDDOC_ERR_READ);

    lpstFIB = (LPWD_FIB)lpbyWorkBuffer;
    if ((ConvertEndianShort(lpstFIB->wFlags) & WD_FLAG_ENCRYPTED) == 0)
    {
        // Not encrypted

        lpstKey->bEncrypted = FALSE;
        return(WORDDOC_OK);
    }

    lpstKey->bEncrypted = TRUE;

    // Start the search somewhere towards the end of the FIB

    lpstKey->dwReverseKeyOffset = 511;

    return (WordDocFindNextKey(lpstCallBack,
                               lpstOLEFile,
                               lpbyWorkBuffer,
                               lpstKey,
                               lpstLocker));
}





