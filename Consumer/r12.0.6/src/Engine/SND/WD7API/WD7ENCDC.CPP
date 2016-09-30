//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/wd7encdc.cpv   1.4   26 Mar 1998 18:09:14   DCHI  $
//
// Description:
//  Module containing functions for finding encryption key.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/wd7encdc.cpv  $
// 
//    Rev 1.4   26 Mar 1998 18:09:14   DCHI
// Added implementation of WD7EncryptedWriteByte().
// 
//    Rev 1.3   01 Dec 1997 11:31:00   DCHI
// Added looser assumptions.
// 
//    Rev 1.2   13 Nov 1997 14:56:20   DCHI
// Added decryption case where both encrypted bytes == both plaintext bytes.
// 
//    Rev 1.1   04 Nov 1997 12:56:56   DCHI
// Modified DeduceKey to handle byA == byP in case where byB == 0.
// 
//    Rev 1.0   05 May 1997 13:58:30   DCHI
// Initial revision.
// 
//************************************************************************

#include "wd7api.h"

//*************************************************************************
//
// WD7_STATUS WD7DeduceKey()
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
//  WD7_STATUS_OK       If a key was found
//  WD7_STATUS_ERROR    If a key was not found
//
//*************************************************************************

WD7_STATUS WD7DeduceKey
(
    LPWDREVKEY          lpstKey,
    LPBYTE              lpbyBuf,
    LPBYTE              lpabyKey
)
{
    int                 i;
    BYTE                byA, byB;
    BYTE                byP, byQ;

    for (i=0;i<WD7_ENC_KEY_LEN;i++)
    {
        // The encrypted text

        byA = lpbyBuf[i];
        byB = lpbyBuf[i + WD7_ENC_KEY_LEN];

        if (byA == 0 && byB == 0)
        {
            // Impossible to determine key in this case

            return(WD7_STATUS_ERROR);
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

                return(WD7_STATUS_ERROR);
            }

            // byA == 0 == byP at this point

            if (lpstKey->byHashLen > i && byB != byQ)
            {
                // Assume K = B ^ Q

                lpabyKey[i] = byB ^ byQ;
            }
            else
            {
                // Assume K=B, this is iffy when lpstKey->byHashLen <= i

                lpabyKey[i] = byB;
            }
        }
        else
        if (byB == 0)
        {
            if (lpstKey->byHashLen > i)
            {
                if (byQ != 0)
                {
                    // The key is not here

                    return(WD7_STATUS_ERROR);
                }

                if (byA != byP)
                {
                    // Assume K = A ^ P

                    lpabyKey[i] = byA ^ byP;
                }
                else
                {
                    // Assume K=A

                    lpabyKey[i] = byA;
                }
            }
            else
            if (byA == byP)
            {
                // Assume K = A

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

                return(WD7_STATUS_ERROR);
            }

            if (byA == byP && lpstKey->byHashLen > i && byB == byQ)
            {
                // Both encrypted bytes are equal to both plaintext
                //  bytes, so the key must be zero

                lpabyKey[i] = 0;
            }
            else
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

    return(WD7_STATUS_OK);
}


//*************************************************************************
//
// WD7_STATUS WD7CheckRing()
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
//                          WD7_ENC_KEY_LEN.  This is the index of the
//                          first hash byte in the buffer.
//  lpstKey             Key structure to store key if found
//
// Description:
//  Searches the given buffer for a key from the given ring.
//
// Returns:
//  WD7_STATUS_OK       If a key was found
//  WD7_STATUS_ERROR    If a key was not found
//
//*************************************************************************

WD7_STATUS WD7CheckRing
(
    LPWDREVKEYRING      lpstRing,
    LPLPWDREVKEY        lpalpstRevKeys,
    LPBYTE              lpbyBuf,
    LPBYTE              lpbyHash,
    WORD                wStartIdx,
    LPWD7ENCKEY         lpstKey
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
            byHash = lpbyHash[(wStartIdx + i) & (WD7_ENC_KEY_LEN - 1)];
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
                    byQ != lpbyBuf[i+WD7_ENC_KEY_LEN])
                    break;
            }
        }

        if (i == nHashLen)
        {
            // It was a match on the hash

            // Now try to see whether a valid key can be obtained

            if (WD7DeduceKey(lpstRevKey,
                             lpbyBuf,
                             lpstKey->abyKey) == WD7_STATUS_OK)
            {
                // Found a candidate key

                lpstKey->wID = wNextRevKeyIdx;

                return(WD7_STATUS_OK);
            }
        }
    }

    return(WD7_STATUS_ERROR);
}


//*************************************************************************
//
// WD7_STATUS WD7FindKeyInBuf()
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
//  WD7_STATUS_OK       If a key was found
//  WD7_STATUS_ERROR    If a key was not found
//
//*************************************************************************

WD7_STATUS WD7FindKeyInBuf
(
    LPBYTE              lpbyChunk,
    WORD                wChunkSize,
    LPWDREVKEYLOCKER    lpstLocker,
    LPWORD              lpwKeyOffset,
    LPWD7ENCKEY         lpstKey
)
{
    BYTE                byRingIdx;
    WORD                wOffset;
    WORD                wStartIdx;
    BYTE                abyHash[WD7_ENC_KEY_LEN];

    // XOR the first 16 bytes of the first 16 bytes of the buffer
    //  with the second 15 bytes of the second 16 bytes

    for (wOffset=0;wOffset<WD7_ENC_KEY_LEN;wOffset++)
    {
        if (lpbyChunk[wOffset] == 0 ||
            lpbyChunk[wOffset + WD7_ENC_KEY_LEN] == 0)
        {
            // Special case

            abyHash[wOffset] = 0;
        }
        else
        {
            // Normal case

            abyHash[wOffset] = lpbyChunk[wOffset] ^
                               lpbyChunk[wOffset + WD7_ENC_KEY_LEN];
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

            if (WD7CheckRing(lpstLocker->lpastRing + byRingIdx - 1,
                             lpstLocker->lpalpstRevKey,
                             lpbyChunk + wOffset - WD7_ENC_KEY_LEN,
                             abyHash,
                             wStartIdx,
                             lpstKey) == WD7_STATUS_OK)
            {
                // Found a match

                *lpwKeyOffset = wOffset - WD7_ENC_KEY_LEN;

                return(WD7_STATUS_OK);
            }
        }

        if ((wOffset + WD7_ENC_KEY_LEN) > wChunkSize)
        {
            // No match in this chunk

            return(WD7_STATUS_ERROR);
        }

        if (lpbyChunk[wOffset] == 0 ||
            lpbyChunk[wOffset + WD7_ENC_KEY_LEN] == 0)
        {
            // Special case

            abyHash[wStartIdx] = 0;
        }
        else
        {
            // Normal case

            abyHash[wStartIdx] = lpbyChunk[wOffset] ^
                                 lpbyChunk[wOffset + WD7_ENC_KEY_LEN];
        }

        wStartIdx = (wStartIdx + 1) & (WD7_ENC_KEY_LEN - 1);

        ++wOffset;
    }
}


//*************************************************************************
//
// WD7_STATUS WD7FindNextKey()
//
// Parameters:
//  lpstStream          Ptr to the structure of the open stream
//  lpstKey             Key structure to store key if found
//  lpstLocker          Locker of reverse keys for deducing key
//
// Description:
//  The function should not be called if it is not encrypted.
//  The function tries to find the next key,
//
// Returns:
//  WD7_STATUS_OK       If a key was found
//  WD7_STAUTS_ERROR    If a key was not found
//
//*************************************************************************

WD7_STATUS WD7FindNextKey
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWDREVKEYLOCKER    lpstLocker
)
{
    DWORD               dwOffset;
    DWORD               dwStreamLen;
    DWORD               dwBytesRead;
    WORD                wChunkSize;
    WORD                wKeyOffset;
    BYTE                abyWorkBuffer[512];

    // Continue from the last offset

    dwOffset = lpstKey->dwReverseKeyOffset + 1;
    dwStreamLen = SSStreamLen(lpstStream);
    while (dwOffset < dwStreamLen)
    {
        // Read in 512 byte chunks

        if (dwOffset + 512 <= dwStreamLen)
            wChunkSize = 512;
        else
            wChunkSize = (WORD)(dwStreamLen - dwOffset);

        if (wChunkSize < WD7_ENC_KEY_LEN + WD7_ENC_KEY_LEN)
        {
            // Not enough bytes left to search

            lpstKey->dwReverseKeyOffset = dwStreamLen;

            return(WD7_STATUS_ERROR);
        }

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       abyWorkBuffer,
                       wChunkSize,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != wChunkSize)
        {
            // Error reading chunk

            lpstKey->dwReverseKeyOffset = dwStreamLen;

            return(WD7_STATUS_ERROR);
        }

        // Search for key

        if (WD7FindKeyInBuf(abyWorkBuffer,
                            wChunkSize,
                            lpstLocker,
                            &wKeyOffset,
                            lpstKey) == WD7_STATUS_OK)
        {
            int         i;

            // Found one

            lpstKey->dwReverseKeyOffset = dwOffset + wKeyOffset;

            // Rotate key for 16-byte alignment

            for (i=0;i<WD7_ENC_KEY_LEN;i++)
            {
                abyWorkBuffer[(i + lpstKey->dwReverseKeyOffset) %
                    WD7_ENC_KEY_LEN] = lpstKey->abyKey[i];
            }

            // Copy aligned key back into key structure

            for (i=0;i<WD7_ENC_KEY_LEN;i++)
                lpstKey->abyKey[i] = abyWorkBuffer[i];

            return(WD7_STATUS_OK);
        }

        dwOffset += wChunkSize - WD7_ENC_KEY_LEN - WD7_ENC_KEY_LEN + 1;
    }

    // Didn't find any key candidates

    lpstKey->dwReverseKeyOffset = dwStreamLen;

    return(WD7_STATUS_ERROR);
}


//*************************************************************************
//
// WD7_STATUS WD7FindFirstKey()
//
// Parameters:
//  lpstStream          Ptr to the structure of the open stream
//  lpstKey             Key structure to store key if found
//  lpstLocker          Locker of reverse keys for deducing key
//
// Description:
//  Determines whether the document is encrypted.  If encrypted,
//  then tries to find the first key.  The function also checks
//  the magic number at the beginning of the stream to verify
//  that it is a Word 6.0/7.0/95 document.
//
// Returns:
//  WD7_STATUS_OK       If a key was found
//  WD7_STATUS_ERROR    If a key was not found
//
//*************************************************************************

WD7_STATUS WD7FindFirstKey
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWDREVKEYLOCKER    lpstLocker
)
{
    BYTE                abyWorkBuffer[48];
    LPWD7_FIB           lpstFIB;
    DWORD               dwBytesRead;

    // Read the first 48 bytes of the FIB(File Information Block)

    if (SSSeekRead(lpstStream,
                   0,
                   abyWorkBuffer,
                   48,
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != 48)
    {
        return(WD7_STATUS_ERROR);
    }

    // Make sure it is a Word 6.0/7.0/95 document

    if (WD7IsKnownMagicNumber((WORD)abyWorkBuffer[0] |
                              ((WORD)abyWorkBuffer[1] << 8)) == FALSE)
    {
        // Not a Word 6.0/7.0/95 document

        return(WD7_STATUS_ERROR);
    }

    // Is the document encrypted?

    lpstFIB = (LPWD7_FIB)abyWorkBuffer;
    if ((WENDIAN(lpstFIB->wFlags) & WD7_FLAG_ENCRYPTED) == 0)
    {
        // Not encrypted

        lpstKey->bEncrypted = FALSE;
        return(WD7_STATUS_OK);
    }

    lpstKey->bEncrypted = TRUE;

    // Start the search somewhere towards the end of the FIB

    lpstKey->dwReverseKeyOffset = 511;

    return (WD7FindNextKey(lpstStream,
                           lpstKey,
                           lpstLocker));
}


//*************************************************************************
//
// DWORD WD7EncryptedRead()
//
// Parameters:
//  lpstStream          Ptr to the structure of the open stream
//  lpstKey             The key to use
//  dwOffset            Read offset in stream
//  lpbyBuffer          Read buffer
//  dwBytesToRead       Number of bytes to read
//
// Description:
//  The function performs a normal read.  Then if the document is
//  encrypted as determined from lpstKey->bEncrypted, then the
//  function decrypts the contents using the given key.
//
//  If lpstKey is NULL, no attempt is made to decrypt the bytes.
//
// Returns:
//  dwBytesToRead       If successful
//  0xFFFFFFFF          If an error occurred
//
//*************************************************************************

DWORD WD7EncryptedRead
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwOffset,
    LPBYTE              lpbyBuffer,
    DWORD               dwBytesToRead
)
{
    DWORD               dw;
    int                 i;
    DWORD               dwBytesRead;

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   lpbyBuffer,
                   dwBytesToRead,
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != dwBytesToRead)
    {
        // Error reading chunk

        return(0xFFFFFFFF);
    }

    // Don't decrypt if a key isn't provided

    if (lpstKey != NULL)
    {
        if (lpstKey->bEncrypted == FALSE)
        {
            // Not encrypted, so just return

            return(dwBytesToRead);
        }

        // Start no sooner than offset 48

        if (dwOffset < 48)
            dw = 48 - dwOffset;
        else
            dw = 0;

        i = (int)((dwOffset + dw) % WD7_ENC_KEY_LEN);
        for (;dw<dwBytesToRead;dw++)
        {
            if (lpbyBuffer[dw] != 0)
            {
                if (lpbyBuffer[dw] != lpstKey->abyKey[i])
                    lpbyBuffer[dw] ^= lpstKey->abyKey[i];
            }

            if (++i == 16)
                i = 0;
        }
    }

    // Successful decryption

    return(dwBytesToRead);
}


//*************************************************************************
//
// DWORD WD7EncryptedWrite()
//
// Parameters:
//  lpstStream          Ptr to the structure of the open stream
//  lpstKey             The key to use
//  dwOffset            Write offset in stream
//  lpbyBuffer          Write buffer
//  dwBytesToWrite      Number of bytes to write
//
// Description:
//  If the document is encrypted as determined from lpstKey->bEncrypted,
//  then the function first encrypts the contents using the given key.
//  The function then performs a normal write.
//
//  If the document is encrypted, the buffer will return with its
//  contents encrypted.
//
//  If lpstKey is NULL, the output is not encrypted.
//
// Returns:
//  dwBytesToWrite      If successful
//  0xFFFFFFFF          If an error occurred
//
//*************************************************************************

DWORD WD7EncryptedWrite
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwOffset,
    LPBYTE              lpbyBuffer,
    DWORD               dwBytesToWrite
)
{
    DWORD               dw;
    int                 i;
    DWORD               dwBytesWritten;

    // Don't encrypt if a key isn't provided.

    if (lpstKey != NULL)
    {
        if (lpstKey->bEncrypted == TRUE)
        {
            // Start no sooner than offset 48

            if (dwOffset < 48)
                dw = 48 - dwOffset;
            else
                dw = 0;

            i = (int)((dwOffset + dw) % WD7_ENC_KEY_LEN);
            for (;dw<dwBytesToWrite;dw++)
            {
                if (lpbyBuffer[dw] != 0)
                {
                    if (lpbyBuffer[dw] != lpstKey->abyKey[i])
                        lpbyBuffer[dw] ^= lpstKey->abyKey[i];
                }

                if (++i == 16)
                    i = 0;
            }
        }
    }

    // Write out the buffer

    if (SSSeekWrite(lpstStream,
                    dwOffset,
                    lpbyBuffer,
                    dwBytesToWrite,
                    &dwBytesWritten) != SS_STATUS_OK ||
        dwBytesWritten != dwBytesToWrite)
    {
        // Error writing buffer

        return(0xFFFFFFFF);
    }

    // Successful write

    return(dwBytesToWrite);
}


//*************************************************************************
//
// DWORD WD7EncryptedWriteByte()
//
// Parameters:
//  lpstStream          Ptr to the structure of the open stream
//  lpstKey             The key to use
//  dwOffset            Write offset in stream
//  byByte              Byte to write
//  dwBytesToWrite      Number of times to repeat write of given byte
//
// Description:
//  The function performs a repeated encrypted write of the given byte.
//
//  If lpstKey is NULL, the output is not encrypted.
//
// Returns:
//  dwBytesToWrite      If successful
//  0xFFFFFFFF          If an error occurred
//
//*************************************************************************

DWORD WD7EncryptedWriteByte
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwOffset,
    BYTE                byByte,
    DWORD               dwBytesToWrite
)
{
    BYTE                abyBuf[256];
    DWORD               dwOrigCount;
    DWORD               dwCount;
    int                 i;

    for (i=0;i<sizeof(abyBuf);i++)
        abyBuf[i] = byByte;

    dwOrigCount = dwBytesToWrite;
    dwCount = sizeof(abyBuf);
    while (dwBytesToWrite != 0)
    {
        if (dwBytesToWrite < dwCount)
            dwCount = dwBytesToWrite;

        if (WD7EncryptedWrite(lpstStream,
                              lpstKey,
                              dwOffset,
                              abyBuf,
                              dwCount) != dwCount)
            return(FALSE);

        dwBytesToWrite -= dwCount;
        dwOffset += dwCount;
    }

    return(dwOrigCount);
}


