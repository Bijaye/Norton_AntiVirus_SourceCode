// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/strscan.cpv   1.10   14 Oct 1998 12:00:28   MKEATIN  $
//
// Description:
//      Contains IBM-cloned string scanner
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/ENGINE15/VCS/strscan.cpv  $
// 
//    Rev 1.10   14 Oct 1998 12:00:28   MKEATIN
// Carey's RelinquishControl calls for NLM.
// 
//    Rev 1.9   11 Aug 1998 17:33:42   CNACHEN
// updated to support ibm string scanning at level 2 heuristics.
// 
//    Rev 1.7   10 Jul 1998 11:02:00   CNACHEN
// Changed to require valid match on 3rd byte of sig if there are no
// wildcards elsewhere in the sig.
// 
//    Rev 1.6   09 Jul 1998 15:02:46   CNACHEN
// Added ifdefed debug code for strscan.exe executable.
// 
//    Rev 1.5   10 Jun 1998 16:21:58   CNACHEN
// 
// 
//    Rev 1.4   09 Jun 1998 10:32:00   CNACHEN
// Made sure to decrement slide distance in sigs (which are encrypted). also
// added check to make sure we don't advance past last pool.
// 
//    Rev 1.3   08 Jun 1998 14:18:58   CNACHEN
// Fixed boot scanning check for lpstHash == NULL.
// 
//    Rev 1.2   08 Jun 1998 10:57:48   CNACHEN
// Fixed some file close bugs that Henry found. Also added a check to make
// sure the hash table size and hash mod were proper.
// 
//    Rev 1.1   05 Jun 1998 17:35:32   MKEATIN
// Include stdio.h on NLM.
// 
//    Rev 1.0   05 Jun 1998 15:17:02   CNACHEN
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "callback.h"
#include "avendian.h"
#include "strscan.h"

//#include "ctsn.h"				// NEW_UNIX
#include "n30type.h"				// NEW_UNIX
#include "callback.h"
#include "navex.h"

#include "heurapi.h"

#if defined (SYM_NLM)

   #include "stdio.h"

#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef NAVEX15

#if defined(SYM_DOSX) || defined(SYM_WIN16)

  #include "navheur1.h"
  #include "inifile.h"
  #include "callfake.h"

#endif

#if defined(SYM_DOSX) || defined(SYM_WIN16)

HFILE WINAPI DDFileOpen(LPTSTR lpName, UINT uOpenMode)
{
    return(DFileOpen(lpName,uOpenMode));
}

HFILE WINAPI DDFileClose(HFILE h)
{
    return(DFileClose(h));
}

UINT WINAPI DDFileRead(HFILE hHandle, LPVOID lpBuffer, UINT uBytes)
{
    return(DFileRead(hHandle,lpBuffer,uBytes));
}

DWORD WINAPI DDFileSeek(HFILE hHandle, LONG dwOffset, int nFlag)
{
    return(DFileSeek(hHandle,dwOffset,nFlag));
}

DWORD WINAPI DDFileLength(HFILE hHandle)
{
    return(DFileLength(hHandle));
}

LPVOID WINAPI DDPermMemoryAlloc(DWORD dwSize)
{
    return (PMemoryAlloc(dwSize));
}

void WINAPI DDPermMemoryFree(LPVOID lpvBuffer)
{
    PMemoryFree(lpvBuffer);
}

#endif // #if defined(SYM_DOSX) || defined(SYM_WIN16)

#endif // #ifndef NAVEX15

BOOL VDFLookUp
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwIdent,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwLength
);


int LoadStringData
(
    LPCALLBACKREV1          lpstCallBack,
    LPTSTR                  lpszStringDataFile,
    DWORD                   dwBase,
    LPSEARCH_INFO           lpstSearchInfo
)
{
	HFILE                   	hFile;
    DWORD                       dwOffset, dwLength, dwPoolSize,dwHashTableBytes;
    WORD                        i,j, wToRead;
	MAIN_STRING_SCAN_HEADER_T   stHeader;
	HASH_STRING_SCAN_HEADER_T   stHashHeader;
    LPBYTE                      lpstHash;

    // open the file

    hFile = lpstCallBack->FileOpen(lpszStringDataFile,0);

    if (hFile == (HFILE)-1)
    {
        lpstSearchInfo->lpstHash = NULL;
        lpstSearchInfo->lplpbyPoolPtrs = NULL;
        return(STRING_LOAD_FILE_NOT_FOUND);
    }

    // locate the section

    if (VDFLookUp(lpstCallBack,
                  hFile,
                  dwBase + IBM_STRING_INCREMENT,
                  &dwOffset,
                  &dwLength) != TRUE)
    {
        lpstCallBack->FileClose(hFile);
        lpstSearchInfo->lpstHash = NULL;
        lpstSearchInfo->lplpbyPoolPtrs = NULL;
        return(STRING_LOAD_GENERAL_ERROR);
    }

    // verify version information

    if (lpstCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
	{
        lpstCallBack->FileClose(hFile);
        lpstSearchInfo->lpstHash = NULL;
        lpstSearchInfo->lplpbyPoolPtrs = NULL;
        return(STRING_LOAD_GENERAL_ERROR);
    }

    if (lpstCallBack->FileRead(hFile,
                               &stHeader,
                               sizeof(stHeader)) != sizeof(stHeader))
    {
        lpstCallBack->FileClose(hFile);
        lpstSearchInfo->lpstHash = NULL;
        lpstSearchInfo->lplpbyPoolPtrs = NULL;
        return(STRING_LOAD_GENERAL_ERROR);
    }

    // endianize, if necessary

	stHeader.dwType = DWENDIAN(stHeader.dwType);
    stHeader.dwVersion = DWENDIAN(stHeader.dwVersion);
    stHeader.wNumPools = WENDIAN(stHeader.wNumPools);

    if (stHeader.dwType != STRING_DATA_FILE_TYPE_VALUE ||
        stHeader.dwVersion != STRING_DATA_FILE_VERSION_VALUE)
    {
        lpstCallBack->FileClose(hFile);
        lpstSearchInfo->lpstHash = NULL;
        lpstSearchInfo->lplpbyPoolPtrs = NULL;
        return(STRING_LOAD_GENERAL_ERROR);
    }

    // allocate a pool pointer array

    lpstSearchInfo->lplpbyPoolPtrs =
		(LPLPBYTE)lpstCallBack->
			PermMemoryAlloc(stHeader.wNumPools * sizeof(LPBYTE));

    if (lpstSearchInfo->lplpbyPoolPtrs == NULL)
    {
        lpstCallBack->FileClose(hFile);
        lpstSearchInfo->lpstHash = NULL;
        lpstSearchInfo->lplpbyPoolPtrs = NULL;
        return(STRING_LOAD_GENERAL_ERROR);
    }

    // NULL out pool ptr table

    for (i=0;i<stHeader.wNumPools;i++)
        lpstSearchInfo->lplpbyPoolPtrs[i] = NULL;

    // load the pools of string data

    for (i=0;i<stHeader.wNumPools;i++)
    {
		// read in pool size in bytes

        if (lpstCallBack->FileRead(hFile,&dwPoolSize,sizeof(dwPoolSize)) !=
            sizeof(dwPoolSize))
        {
            for (j=0;j<i;j++)
                lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs[j]);
            lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs);
            lpstCallBack->FileClose(hFile);
            lpstSearchInfo->lpstHash = NULL;
            lpstSearchInfo->lplpbyPoolPtrs = NULL;
            return(STRING_LOAD_GENERAL_ERROR);
        }

	dwPoolSize = DWENDIAN(dwPoolSize);		// NEW_UNIX

        // allocate memory for pool

        lpstSearchInfo->lplpbyPoolPtrs[i] =
            (LPBYTE)lpstCallBack->PermMemoryAlloc(dwPoolSize);

        if (lpstSearchInfo->lplpbyPoolPtrs[i] == NULL)
        {
            for (j=0;j<i;j++)
                lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs[j]);
            lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs);
            lpstCallBack->FileClose(hFile);
            lpstSearchInfo->lpstHash = NULL;
            lpstSearchInfo->lplpbyPoolPtrs = NULL;
            return(STRING_LOAD_GENERAL_ERROR);
        }

        // read in that pool!

        if (lpstCallBack->FileRead(hFile,
                                   lpstSearchInfo->lplpbyPoolPtrs[i],
                                   (UINT)dwPoolSize) != dwPoolSize)
		{
            for (j=0;j<=i;j++)
                lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs[j]);
            lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs);
            lpstCallBack->FileClose(hFile);
            lpstSearchInfo->lpstHash = NULL;
            lpstSearchInfo->lplpbyPoolPtrs = NULL;
            return(STRING_LOAD_GENERAL_ERROR);
        }

        // success - go onto next pool!
    }
    
    // load information about hash table size, etc.

    if (lpstCallBack->FileRead(hFile,
                               &stHashHeader,
                               sizeof(stHashHeader)) != sizeof(stHashHeader))
    {
        for (i=0;i<stHeader.wNumPools;i++)
            lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs[i]);
        lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs);
        lpstCallBack->FileClose(hFile);
        lpstSearchInfo->lpstHash = NULL;
        lpstSearchInfo->lplpbyPoolPtrs = NULL;
        return(STRING_LOAD_GENERAL_ERROR);
    }

    stHashHeader.wHashTableSize = WENDIAN(stHashHeader.wHashTableSize);
    stHashHeader.wHashTableMask = WENDIAN(stHashHeader.wHashTableMask);

    if ((WORD)(stHashHeader.wHashTableSize-1) != stHashHeader.wHashTableMask)
    {
        for (i=0;i<stHeader.wNumPools;i++)
            lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs[i]);
        lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs);
        lpstCallBack->FileClose(hFile);
        lpstSearchInfo->lpstHash = NULL;
        lpstSearchInfo->lplpbyPoolPtrs = NULL;
        return(STRING_LOAD_GENERAL_ERROR);
    }

    // allocate memory for the hash table

    dwHashTableBytes = (DWORD)stHashHeader.wHashTableSize * sizeof(BUCKET_T);

    lpstSearchInfo->lpstHash =
        (LPBUCKET)lpstCallBack->PermMemoryAlloc(dwHashTableBytes);

    if (lpstSearchInfo->lpstHash == NULL)
    {
        for (i=0;i<stHeader.wNumPools;i++)
            lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs[i]);
        lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs);
        lpstCallBack->FileClose(hFile);
        lpstSearchInfo->lpstHash = NULL;
        lpstSearchInfo->lplpbyPoolPtrs = NULL;
        return(STRING_LOAD_GENERAL_ERROR);
    }

    // load the hash table contents in two passes, just in case its 64K

    lpstHash = (LPBYTE)lpstSearchInfo->lpstHash;

    while (dwHashTableBytes > 0)
    {
        wToRead = dwHashTableBytes > 16384 ? 16384 : (WORD)dwHashTableBytes;

        if (lpstCallBack->FileRead(hFile,
                                   lpstHash,
                                   wToRead) != wToRead)
        {
            lpstCallBack->PermMemoryFree(lpstSearchInfo->lpstHash);
            for (i=0;i<stHeader.wNumPools;i++)
                lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs[i]);
            lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs);
            lpstCallBack->FileClose(hFile);
            lpstSearchInfo->lpstHash = NULL;
            lpstSearchInfo->lplpbyPoolPtrs = NULL;
            return(STRING_LOAD_GENERAL_ERROR);
        }

        lpstHash += wToRead;
        dwHashTableBytes -= wToRead;
    }

    // remember our settings in lpstSearchInfo

    lpstSearchInfo->wHashTableSize = stHashHeader.wHashTableSize;
    lpstSearchInfo->wHashTableMask = stHashHeader.wHashTableMask;
    lpstSearchInfo->wNumPools = stHeader.wNumPools;

    lpstCallBack->FileClose(hFile);         // close that file!

    return(STRING_LOAD_OK);
}

void FreeStringData
(
    LPCALLBACKREV1          lpstCallBack,
    LPSEARCH_INFO           lpstSearchInfo
)
{
    int                     i;

    if (lpstSearchInfo->lpstHash != NULL)
    {
        lpstCallBack->PermMemoryFree(lpstSearchInfo->lpstHash);
    }

    if (lpstSearchInfo->lplpbyPoolPtrs != NULL)
    {
        for (i=0;i<lpstSearchInfo->wNumPools;i++)
            if (lpstSearchInfo->lplpbyPoolPtrs[i] != NULL)
                lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs[i]);

        lpstCallBack->PermMemoryFree(lpstSearchInfo->lplpbyPoolPtrs);
    }
    lpstSearchInfo->lpstHash = NULL;
    lpstSearchInfo->lplpbyPoolPtrs = NULL;
}


int LoadOffsetData
(
    LPCALLBACKREV1          lpstCallBack,
    LPTSTR                  lpszOffsetDataFile,
    DWORD                   dwBase,
    LPSEARCH_INFO           lpstSearchInfo
)
{
    HFILE                   	hFile;
    DWORD                       dwOffset, dwLength;
    MAIN_OFFSET_SCAN_HEADER_T   stHeader;

    // open the file

    hFile = lpstCallBack->FileOpen(lpszOffsetDataFile,0);

    if (hFile == (HFILE)-1)
    {
        lpstSearchInfo->lpbyOffsetSigBuffer = NULL;
        return(STRING_LOAD_FILE_NOT_FOUND);
    }

    // locate the section

    if (VDFLookUp(lpstCallBack,
                  hFile,
                  dwBase + IBM_OFFSET_INCREMENT,
                  &dwOffset,
                  &dwLength) != TRUE)
    {
        lpstCallBack->FileClose(hFile);
        lpstSearchInfo->lpbyOffsetSigBuffer = NULL;
        return(STRING_LOAD_GENERAL_ERROR);
	}

    // verify version information

    if (lpstCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
    {
        lpstCallBack->FileClose(hFile);
        lpstSearchInfo->lpbyOffsetSigBuffer = NULL;
        return(STRING_LOAD_GENERAL_ERROR);
    }

    if (lpstCallBack->FileRead(hFile,
                               &stHeader,
                               sizeof(stHeader)) != sizeof(stHeader))
    {
		lpstCallBack->FileClose(hFile);
        lpstSearchInfo->lpbyOffsetSigBuffer = NULL;
        return(STRING_LOAD_GENERAL_ERROR);
    }

    // endianize, if necessary

    stHeader.dwType = DWENDIAN(stHeader.dwType);
    stHeader.dwVersion = DWENDIAN(stHeader.dwVersion);
    stHeader.dwTotalSize = DWENDIAN(stHeader.dwTotalSize);
    stHeader.wNumSigs = WENDIAN(stHeader.wNumSigs);

    if (stHeader.dwType != STRING_DATA_FILE_TYPE_VALUE ||
        stHeader.dwVersion != STRING_DATA_FILE_VERSION_VALUE ||
        stHeader.dwTotalSize >= 0xFFFFU)        // one segment worth!
    {
        lpstCallBack->FileClose(hFile);
        lpstSearchInfo->lpbyOffsetSigBuffer = NULL;
        return(STRING_LOAD_GENERAL_ERROR);
    }

    // allocate a pool pointer array

    lpstSearchInfo->lpbyOffsetSigBuffer =
		(LPBYTE)lpstCallBack->PermMemoryAlloc(stHeader.dwTotalSize);

    if (lpstSearchInfo->lpbyOffsetSigBuffer == NULL)
    {
        lpstCallBack->FileClose(hFile);
        return(STRING_LOAD_GENERAL_ERROR);
    }

    if (lpstCallBack->FileRead(hFile,
                               lpstSearchInfo->lpbyOffsetSigBuffer,
                               (UINT)stHeader.dwTotalSize) !=
          stHeader.dwTotalSize)
	{
        lpstCallBack->PermMemoryFree(lpstSearchInfo->lpbyOffsetSigBuffer);
        lpstCallBack->FileClose(hFile);
        lpstSearchInfo->lpbyOffsetSigBuffer = NULL;
        return(STRING_LOAD_GENERAL_ERROR);
    }

    // remember our settings in lpstSearchInfo

    lpstSearchInfo->wNumOffsetSigs = stHeader.wNumSigs;

    lpstCallBack->FileClose(hFile);         // close that file!

    return(STRING_LOAD_OK);
}

void FreeOffsetData
(
    LPCALLBACKREV1          lpstCallBack,
    LPSEARCH_INFO           lpstSearchInfo
)
{
	if (lpstSearchInfo->lpbyOffsetSigBuffer != NULL)
        lpstCallBack->PermMemoryFree(lpstSearchInfo->lpbyOffsetSigBuffer);

    lpstSearchInfo->lpbyOffsetSigBuffer = NULL;
}



BOOL VerifyRestOfString
(
    LPBYTE                  lpbySigPtr,
    int                     nSigLength,
    LPBYTE                  lpbyScanMe,
    int                     nBufferLen
)
{
	int                     i;
	WORD                    wSlideDist;
	WORD                    wIndex, wSlideIndex;
        BYTE                    bySigValue;

	// lpbySigPtr points to a WORD scan distance to slide on entry to this fn

    wSlideDist = (BYTE)(lpbySigPtr[0]-1) |
                    ((WORD)(BYTE)(lpbySigPtr[1]-1) << 8);

	// skip over length word...

	lpbySigPtr += 2;
	nSigLength -= 2;

    // apply the rest of the signature over wSlideDist bytes.

	for (wSlideIndex=0;wSlideIndex<=wSlideDist;wSlideIndex++)
	{
#ifdef SYM_NLM
        // relinquish control on NLM

        if ((wSlideIndex & 0x003f) == 0)
            ThreadSwitch();
#endif

        wIndex = wSlideIndex;           // start looking here

        // match the string

		for (i=0;i<nSigLength && wIndex < nBufferLen;i++)
		{
            bySigValue = lpbySigPtr[i]-1;     // for efficiency (decrypt)

            // check for wildcard:  E9 %5 ?? B8 etc is OK, for example

			if (bySigValue == SIG_VALUE_WILDCARD)
                wIndex++;
            else if (bySigValue == SIG_VALUE_SLIDE)
            {
                // recurse for more slides

                if (VerifyRestOfString(lpbySigPtr + i + 1,
                                       nSigLength - i - 1,
                                       lpbyScanMe + wIndex,
                                       nBufferLen - wIndex) == TRUE)
                {
                    // good to go!

                    return(TRUE);
                }
                else
                {
                    // mismatch

					break;
                }
            }
            else
			{
                // see if we're looking for a FD, FE or FF
                // byte...

                if (bySigValue == SIG_VALUE_LITERAL)
                    i++;

                // make sure we match - no mismatches are allowed

                if ((BYTE)(lpbySigPtr[i]-1) != lpbyScanMe[wIndex])
                    break;

                wIndex++;
            }
        }

        if (i == nSigLength)
            return(TRUE);               // got one!
    }

    return(FALSE);
}


#ifdef STRSCAN_DEBUG

int gnDebugLength;
int gnDebugMismatches;
LPBYTE glpbyDebugSigPtr;
WORD gwDebugOffset;
BOOL gbDebugHashed;

#endif // #ifdef STRSCAN_DEBUG

int StringScan
(
    LPSEARCH_INFO       lpstSearchInfo,
    LPBYTE              lpbyScanMe,
    int                 nBufferLen
)
{
    int                 i, nSearchLen, k;
    WORD                wCur, wPoolNum;
    int                 l, nSigLength, nMismatches;
    LPBYTE              lpbySigPtr;
    WORD                wModMask;
	LPBUCKET            lpstBucket;
	BYTE				bySigValue;
    BOOL                bMismatchOn3, bWildUsed;


//    LPBYTE              lpbyDarren; int nDarrenLen;

    // for speed, determine our modulo for our hash table - we're going to
    // and with it instead of doing a %...

    wModMask = lpstSearchInfo->wHashTableMask;  // modulo mask, e.g. 00001111b
												// means table size of 16

    if (nBufferLen < sizeof(WORD))
        return(STRING_SCAN_NO_STRING);          // buffer is way to small

    nSearchLen = nBufferLen - sizeof(WORD);     // how much to scan

    // look for strings

    for (i=0;i <= nSearchLen ;i++,lpbyScanMe++)
    {

#ifdef SYM_NLM
        // relinquish control on NLM

        if ((i & 0x003f) == 0)
            ThreadSwitch();
#endif
        // use next two bytes in file to compute hash value

        wCur = *lpbyScanMe | ((WORD)*(lpbyScanMe+1) << 8);

        // find the bucket

		lpstBucket = &lpstSearchInfo->lpstHash[wCur & wModMask];

        // find the starting pool where sigs are bucketed off

        wPoolNum = WENDIAN(lpstBucket->wPoolNum);

        // if there are sigs off this bucket, then wPoolNum won't be equal to
        // INVALID_POOL_NUM

        if (wPoolNum != INVALID_POOL_NUM)
        {
            // get a ptr to the pool that contains at least some of
            // our signatures... the rest may be in zero or more following
            // pools

            lpbySigPtr = lpstSearchInfo->lplpbyPoolPtrs[wPoolNum] +
                         WENDIAN(lpstBucket->wOffsetInPool);

           
            // look at the length byte of each sig until we find an invalid
            // length of 0xFF, which indicates we've hit the end of all
            // the signatures bucketed under this hash.

            while ((BYTE)*lpbySigPtr != SIG_LENGTH_END_OF_SIGS)
            {
                // if we find a SIG_LENGTH_NEXT_POOL length byte, we're
                // going to advance to the next pool and start looking
                // for the strings in there.

//                lpbyDarren = lpbySigPtr;
//                nDarrenLen = *lpbySigPtr;

                if (*lpbySigPtr == SIG_LENGTH_NEXT_POOL)
                {
                    if (++wPoolNum < lpstSearchInfo->wNumPools)
                        lpbySigPtr = lpstSearchInfo->lplpbyPoolPtrs[wPoolNum];
                    else
                    {
                        // should never happen!

                        return(STRING_SCAN_NO_STRING);
                    }
                }

                // if our signature length is SIG_LENGTH_END_OF_SIGS
                // we're done with all bucketed sigs.

                if (*lpbySigPtr != SIG_LENGTH_END_OF_SIGS)
                {
                    // start comparing from 3nd byte (bytes 0 and 1 already
                    // verified after if statement below)

                    l = 2;
                    k = 0;

                    // adjust string length to skip over first two header
                    // bytes and then first two bytes of signature

					nSigLength = (WORD)*lpbySigPtr++ - 2;
					nMismatches = (WORD)*lpbySigPtr++;

#ifdef STRSCAN_DEBUG

                    gnDebugLength = nSigLength;
                    gnDebugMismatches = nMismatches;
                    glpbyDebugSigPtr = lpbySigPtr;
                    gbDebugHashed = TRUE;

#endif // #ifdef STRSCAN_DEBUG

                    // make sure we match perfectly on the first 2 - just
                    // because we hashed doesn't mean we match.
                    // first two bytes of hashed sigs are NEVER encrypted,
                    // rest of bytes are...

                    if (AVDEREF_WORD(lpbyScanMe) == AVDEREF_WORD(lpbySigPtr))	// NEW_UNIX
					{
                        bMismatchOn3 = FALSE;
                        bWildUsed = FALSE;

						lpbySigPtr += 2;    // skip over first 2 bytes which
											// were already verified
                        nSigLength -= 2;    // ditto

                        for (;k<nSigLength && l+i < nBufferLen;k++)
                        {
                            // use local variable for speed

                            bySigValue = lpbySigPtr[k] - 1;

                            // check for wildcard

                            if (bySigValue == SIG_VALUE_WILDCARD)
                            {
                                l++;
                                bWildUsed = TRUE;
                            }
                            else if (bySigValue == SIG_VALUE_SLIDE)
                            {
                                // check for slide

                                if (VerifyRestOfString(lpbySigPtr + k + 1,
                                                       nSigLength - k - 1,
                                                       lpbyScanMe + l,
                                                       nBufferLen - i - l
                                                      ) == TRUE)
                                {
                                    /*printf("slide found\n");
                                    int p;
                                    for (p=0;p<nDarrenLen;p++)
                                        printf("%02X ",lpbyDarren[p]);
                                    printf("\n"); */
                                    return(STRING_SCAN_FOUND_STRING);
                                }
                                else
                                {
                                    // not found - advance to next string

                                    break;
                                }
                            }
                            else
                            {
                                // see if we're looking for a FD, FE or FF
                                // byte...

                                if (bySigValue == SIG_VALUE_LITERAL)
                                    k++;

                                // make sure we match - if not its ok as
                                // long as we have mismatches left...

                                if ((BYTE)(lpbySigPtr[k]-1) != lpbyScanMe[l])
                                {
                                    nMismatches--;
                                    if (nMismatches < 0)
                                        break;

                                    if (l == 2) // on 3rd byte
                                    {
                                        bMismatchOn3 = TRUE;
                                    }
                                }

                                l++;
                            }
                        }
                    }

                    // see if we caught one!

                    if (k == nSigLength)
                    {
/*                        printf("regular found\n");
                        int p;
                        for (p=0;p<nDarrenLen;p++)
                            printf("%02X ",lpbyDarren[p]);
                        printf("\n");
                        for (p=0;p<nDarrenLen;p++)
                            printf("%02X ",lpbyScanMe[p]);
                        printf("\n");
                        printf("i=%d\n",i); */

                        if (bWildUsed == TRUE ||
                            (/*bWildUsed == FALSE &&*/ bMismatchOn3 == FALSE))
                        {
                            return(STRING_SCAN_FOUND_STRING);
                        }
                    }

                    // advance to next sig

                    lpbySigPtr += nSigLength;
                }
            }
        }
    }

    return(STRING_SCAN_NO_STRING);
}



int OffsetScan
(
	LPSEARCH_INFO       lpstSearchInfo,
	LPBYTE              lpbyTOFBuffer,
	int                 nTOFLen,
	LPBYTE              lpbyEPBuffer,
    int                 nEPLen
)
{
    int                 i, k, l, nBufferLen;
    int                 nSigLength, nMismatches;
    LPBYTE              lpbySigPtr, lpbyMatchBuffer;
	BYTE				bySigValue;
	WORD                wNumSigs, wOffset;
    BOOL                bMismatchOn3, bWildUsed;

    
    // point to start of string buffer and determine how many we have to
    // look at

    wNumSigs = lpstSearchInfo->wNumOffsetSigs;
    lpbySigPtr = lpstSearchInfo->lpbyOffsetSigBuffer;

    // searching right at the EP until we find out otherwise

    lpbyMatchBuffer = lpbyEPBuffer;
    nBufferLen = nEPLen;                // how many bytes after EP


    for (i=0;i < wNumSigs;i++)
    {
#ifdef SYM_NLM
        // relinquish control on NLM

        if ((i & 0x001f) == 0)
            ThreadSwitch();
#endif

        // determine offset in file where we expect to find the string
        // encoded in little endian form

        wOffset = *lpbySigPtr | ((WORD)*(lpbySigPtr+1) << 8);

        // if its not FFFF than we've got an offset that specifies where
        // in the first 16K we can find our string.

        if (wOffset != ENTRYPOINT_SIG_OFFSET)
        {
            // strings sorted by offset from TOF go off end of file -
            // no possible match

            if (wOffset >= nTOFLen)
                return(STRING_SCAN_NO_STRING);

            lpbyMatchBuffer = lpbyTOFBuffer + wOffset;
            nBufferLen = nTOFLen;

            // offset is properly set now
        }
        else
		{
            wOffset = 0;        // right at EP

            // make sure the EP is valid and in the file! if not, skip over
            // the signature...

            if (nEPLen < 0)
            {
                lpbySigPtr += sizeof(WORD);             // skip over offset
                lpbySigPtr += (WORD)*lpbySigPtr;        // skip over rest of sig

                continue;
            }
        }

        // ok, look for the string

        lpbySigPtr += sizeof(WORD);             // skip over offset
        nSigLength = (WORD)*lpbySigPtr++ - 2;   // get string length
        nMismatches = (WORD)*lpbySigPtr++;      // get allowed mismatches

#ifdef STRSCAN_DEBUG

            gnDebugLength = nSigLength;
            gnDebugMismatches = nMismatches;
            glpbyDebugSigPtr = lpbySigPtr;
            gwDebugOffset = wOffset;
            gbDebugHashed = FALSE;

#endif // #ifdef STRSCAN_DEBUG

        // match the string

        bWildUsed = FALSE;
        bMismatchOn3 = FALSE;

        for (k=0,l=0;k<nSigLength && l + wOffset < nBufferLen;k++)
        {
            // get in a local byte for speed

            bySigValue = lpbySigPtr[k] - 1;

            // check for a wildcard

            if (bySigValue == SIG_VALUE_WILDCARD)
            {
                l++;
                bWildUsed = TRUE;
            }
            else if (bySigValue == SIG_VALUE_SLIDE)
            {
                // check for slide

                if (VerifyRestOfString(lpbySigPtr + k + 1,
                                       nSigLength - k - 1,
                                       lpbyMatchBuffer + l,
                                       nBufferLen - wOffset - l
                                      ) == TRUE)
                {
                    // got one

                    return(STRING_SCAN_FOUND_STRING);
                }
                else
                {
                    // mismatch! go to next string

                    break;
                }
            }
            else
            {
                // encoded special byte (FD, FE or FF)? if so advance to it.

                if (bySigValue == SIG_VALUE_LITERAL)
                    k++;

                // match next byte as a literal

                if ((BYTE)(lpbySigPtr[k]-1) != lpbyMatchBuffer[l])
                {
                    // mismatch? ok if we haven't had too many.

					nMismatches--;
                    if (nMismatches < 0)
                        break;                  // too many

                    if (l == 2)
                    {
                        // 3rd byte matched
                        bMismatchOn3 = TRUE;
                    }
                }

                l++;                            // advance to next byte in
                                                // file being scanned
            }
        }

        // see if we caught one!

        if (k == nSigLength)
        {
            if (bWildUsed == TRUE ||
                (/*bWildUsed == FALSE &&*/ bMismatchOn3 == FALSE))
            {
                return(STRING_SCAN_FOUND_STRING);
            }
        }

        // advance to next signature

        lpbySigPtr += nSigLength;
    }

    return(STRING_SCAN_NO_STRING);
}


// From the source code, for file viruses,
// - We check one or two entrypoints, e.g. the DOS stub and the Windows epoint for NE/PE, device and strategy for DOS device drivers.
//
// - If file begins with MZ/ZM, EXE entrypoint (and possibly NE/PE entrypoint) is checked.
//
// - E9 and E8 initial jumps are followed.
//
// That's it. No additional following is done. The full emulator does the tracing for complicated viruses like Commander Bomber.
//
//
// For boot viruses,
// - An initial E9 or E8 is followed.
//
// We don't use EPoint only for file viruses.

BOOL DetermineEPOffset
(
	LPBYTE				lpbyTOF,
	int					nTOFLen,
    LPDWORD             lpdwOffset
)
{
	LPIBMEXEHEADER      lpEXEHeaderPtr;
    DWORD               dwSegBase;
    WORD                wCurIP;

    if (nTOFLen < 2)            // must have at least 3 bytes!
    {
        return(FALSE);
    }

    // check for EXE file

    if (*(LPWORD)lpbyTOF == 0x4d5a ||
		*(LPWORD)lpbyTOF == 0x5a4d)
    {
        // dealing with EXE

        if (nTOFLen < MIN_EXE_FILE_LENGTH)
            return(FALSE);

		lpEXEHeaderPtr = (LPIBMEXEHEADER)lpbyTOF;

        // compute offset of EP in EXE files

        dwSegBase = (DWORD)((WORD)(WENDIAN(lpEXEHeaderPtr->cs) +
                                   WENDIAN(lpEXEHeaderPtr->header_size))) << 4;

        // set the IP

        wCurIP = WENDIAN(lpEXEHeaderPtr->ip);
        *lpdwOffset = (DWORD)(dwSegBase + wCurIP) & EXE_ADDRESS_MASK;

        return(TRUE);
    }
    else
    {
        // dealing with COM... follow only E9 or E8 at TOF

        if (*lpbyTOF == 0xE8 || *lpbyTOF == 0xE9)
        {
            if (nTOFLen < 3)
                return(FALSE);      // must have at least 3 bytes for call/jmp

            // 3 added in below is for sizeof JMP/CALL

            *lpdwOffset = (DWORD)(*(lpbyTOF+1) | (WORD)*(lpbyTOF+2) << 8) + 3;

            return(TRUE);
        }

        // offset is at TOF

        *lpdwOffset = 0;
        return(TRUE);
    }
}


BOOL GetBuffers
(
    LPCALLBACKREV1          lpstCallBack,
    HFILE                   hFile,
    LPBYTE                  lpbyTOF,
    LPINT                   lpnTOFLen,
    LPBYTE                  lpbyEOF,
    LPINT                   lpnEOFLen,
    LPBYTE                  lpbyEPSandwich,
    LPINT                   lpnEPLen,
    LPINT                   lpnEPOffsetInSandwich,
    BOOL                    bUseWimpyMode
)
{
    DWORD               dwTOFEnd, dwFileSize, dwOffset, dwLen;
    DWORD               dwTargetTOF, dwTargetEOF, dwTargetEP;

    if (bUseWimpyMode == TRUE)
    {
        dwTargetTOF = WIMPY_TOF_SCAN_SIZE;
        dwTargetEOF = WIMPY_EOF_SCAN_SIZE;
        dwTargetEP = WIMPY_EP_SANDWICH_SIZE;
    }
    else
    {
        dwTargetTOF = TOF_SCAN_SIZE;
        dwTargetEOF = EOF_SCAN_SIZE;
        dwTargetEP = EP_SANDWICH_SIZE;
    }

    // get file size

    dwFileSize = lpstCallBack->FileSize(hFile);
    if (dwFileSize == 0xFFFFFFFFUL)
        return(FALSE);

    // determine how much of the TOF to read in - either 16K or as much as
    // we can.

    if (dwFileSize < dwTargetTOF)
        dwTOFEnd = dwFileSize;
    else
        dwTOFEnd = dwTargetTOF;

    // seek and read it in!

    if (lpstCallBack->FileSeek(hFile,0,SEEK_SET) != 0)
        return(FALSE);

    if (lpstCallBack->FileRead(hFile,
                               lpbyTOF,
                               (UINT)dwTOFEnd) != dwTOFEnd)
        return(FALSE);

    // set length (< 16K)

    *lpnTOFLen = (int)dwTOFEnd;

    // now read the EOF buffer if its entirely contained in first 16K of file.
    // if it is at all non-overlapping with the TOF, then read the whole darn
    // thing

    if (dwFileSize > dwTargetTOF)
    {
        // need to read it!

        if (lpstCallBack->FileSeek(hFile,
                                   dwFileSize - dwTargetEOF,
                                   SEEK_SET) != dwFileSize - dwTargetEOF)
            return(FALSE);

        if (lpstCallBack->FileRead(hFile,
                                   lpbyEOF,
                                   (UINT)dwTargetEOF) != (UINT)dwTargetEOF)
        return(FALSE);

        *lpnEOFLen = (int)dwTargetEOF;
    }
    else
    {
        *lpnEOFLen = 0;             // don't bother scanning the EOF buffer!
	}

    // now determine the EP of the file...

    if (DetermineEPOffset(lpbyTOF,
                          *lpnTOFLen,
                          &dwOffset) == FALSE)
    {
        *lpnEPLen = 0;
        *lpnEPOffsetInSandwich = 0;
    }
    else
    {
        // read in that EP

        if (dwOffset >= (dwTargetEP/2))
        {
            *lpnEPOffsetInSandwich = (int)(dwTargetEP/2);
            dwOffset -= (dwTargetEP/2);

        }
        else
        {
            *lpnEPOffsetInSandwich = (int)dwOffset;
            dwOffset = 0;
        }

        // seek & read

        if (lpstCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
            return(FALSE);

        dwLen = lpstCallBack->FileRead(hFile,lpbyEPSandwich,(UINT)dwTargetEP);
        if (dwLen == (UINT)-1)
            return(FALSE);

        *lpnEPLen = (int)dwLen;
    }

	return(TRUE);
}

int LoadAllStringData
(
    LPCALLBACKREV1          lpstCallBack,
    LPTSTR                  lpszStringDataFile,
    DWORD                   dwBase,
    LPSEARCH_INFO           lpstSearchInfo
)
{
    int                     nResult;

    nResult = LoadStringData(lpstCallBack,
                             lpszStringDataFile,
                             dwBase,
                             lpstSearchInfo);

    if (nResult != STRING_LOAD_OK)
        return(nResult);

    nResult = LoadOffsetData(lpstCallBack,
                             lpszStringDataFile,
                             dwBase,
                             lpstSearchInfo);

    if (nResult != STRING_LOAD_OK)
    {
        FreeStringData(lpstCallBack,lpstSearchInfo);
    }

    return(nResult);
}

#ifndef NAVEX15

#ifdef SYM_DOSX
void LoadIBMStringData
(
    LPSTR               lpszProgramPath
)
{
    extern NAVEX_HEUR_T         gstHeur;
    extern SEARCH_INFO_T        gstIBMFile;
//    extern SEARCH_INFO_T        gstIBMBoot;
    LPSTR               lpszPtr, lpszLast;
    char                szPath[SYM_MAX_PATH], szDataFile[SYM_MAX_PATH];
    DWORD               dwSize;
    CALLBACKREV1        stCB = {DDFileOpen,
                                DDFileClose,
                                DDFileRead,
                                NULL,
                                DDFileSeek,
                                DDFileLength,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                DDPermMemoryAlloc,
                                DDPermMemoryFree};


    // set up pointers for filename processing

    lpszLast = NULL;
    strcpy(szPath,lpszProgramPath);
    strupr(szPath);
    lpszPtr = szPath;

    // find trailing backslash.

    lpszLast = strrchr(szPath,'\\');

    if (lpszLast != NULL)
    {
        lpszLast++;
        *(lpszLast) = 0;
    }

#if defined(SYM_UNIX)
    sprintf(szDataFile,"%s%s",szPath,"virscan8.dat");
#else
    sprintf(szDataFile,"%s%s",szPath,"VIRSCAN8.DAT");
#endif

    // THIS MUST BE DONE AFTER HEURISTICS ARE LOADED!

    if (gstHeur.nHeurLevel == 3)
    {
        if (LoadAllStringData(&stCB,
                              szDataFile,
                              IBM_FILE_BASE,
                              &gstIBMFile) != EXTSTATUS_OK)
        {
            return;
        }
    }
/*
    if (LoadAllStringData(&stCB,
                          szDataFile,
                          IBM_BOOT_BASE,
                          &gstIBMBoot) != EXTSTATUS_OK)
    {
        // its OK to free even if we didn't load, since stuff will be
        // zeroed in this case

        FreeAllStringData(&stCB,&gstIBMFile);
    }
*/
}
#endif // SYM_DOS

#ifdef SYM_WIN16
void LoadIBMStringData
(
    LPVOID              lpvExtra
)
{
    extern NAVEX_HEUR_T gstHeur;
    DWORD               dwSize, dw;
    char                szPath[SYM_MAX_PATH], szDataFile[SYM_MAX_PATH];
    CALLBACKREV1        stCB = {DDFileOpen,
                                DDFileClose,
                                DDFileRead,
                                NULL,
                                DDFileSeek,
                                DDFileLength,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                DDPermMemoryAlloc,
                                DDPermMemoryFree};


    dwSize = GetModuleFileName((HINSTANCE)lpvExtra,
                               szPath,
                               SYM_MAX_PATH);

    if (dwSize == 0)
        return;

    // Search for the preceding backslash (no double byte since we're only
    // skipping backward over the NAVEX.DLL name which has no double byte
    // chars)

    dw = dwSize - 1;
    while (dw != 0)
    {
        if (szPath[dw] == '\\')
            break;

        --dw;
    }

    if (szPath[dw] != '\\')
        szPath[dw] = '\\';

    szPath[dw+1] = 0;

    mystrcpy(szDataFile,szPath);
#if defined(SYM_UNIX)
    mystrcat(szDataFile,"virscan8.dat");
#else
    mystrcat(szDataFile,"VIRSCAN8.DAT");
#endif

    // THIS MUST BE DONE AFTER HEURISTICS ARE LOADED!

    if (gstHeur.nHeurLevel == 3)
    {
        if (LoadAllStringData(&stCB,
                              szDataFile,
                              IBM_FILE_BASE,
                              &gstIBMFile) != EXTSTATUS_OK)
        {
            return;
        }
    }
/*
    if (LoadAllStringData(&stCB,
                          szDataFile,
                          IBM_BOOT_BASE,
                          &gstIBMBoot) != EXTSTATUS_OK)
    {
        // its OK to free even if we didn't load, since stuff will be
        // zeroed in this case

        FreeAllStringData(&stCB,&gstIBMFile);
    }
*/
}
#endif // SYM_WIN16

#if defined(SYM_DOSX) || defined(SYM_WIN16)

void FreeAllStringData
(
    LPSEARCH_INFO           lpstSearchInfo
)
{
    CALLBACKREV1        stCB = {DDFileOpen,
                                DDFileClose,
                                DDFileRead,
                                NULL,
                                DDFileSeek,
                                DDFileLength,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                NULL,
                                DDPermMemoryAlloc,
                                DDPermMemoryFree};

    FreeStringData(&stCB,lpstSearchInfo);
    FreeOffsetData(&stCB,lpstSearchInfo);
}

#endif // #if defined(SYM_DOSX) || defined(SYM_WIN16)

#endif // #ifndef NAVEX15

void FreeAllStringData
(
    LPCALLBACKREV1          lpstCallBack,
    LPSEARCH_INFO           lpstSearchInfo
)
{
    FreeStringData(lpstCallBack,lpstSearchInfo);
    FreeOffsetData(lpstCallBack,lpstSearchInfo);
}


int IBMStringScanFile
(
    LPCALLBACKREV1          lpstCallBack,
    LPSEARCH_INFO           lpstSearchInfo,
    HFILE                   hFile,
    LPBYTE                  lpbyDecryptedBuffer,
    int                     nDecryptedBufferLen,
    BOOL                    bUseWimpyMode
)
{
    LPBYTE                  lpbyTOF, lpbyEOF, lpbyEPSandwich;
    int                     nTOFLen, nEOFLen, nEPLen, nEPOffsetInSandwich;

    lpbyTOF = (LPBYTE)lpstCallBack->PermMemoryAlloc(TOF_SCAN_SIZE);
	if (lpbyTOF == NULL)
	{
		return(STRING_SCAN_NO_STRING);
	}

	lpbyEOF = (LPBYTE)lpstCallBack->PermMemoryAlloc(EOF_SCAN_SIZE);
	if (lpbyEOF == NULL)
	{
		lpstCallBack->PermMemoryFree(lpbyTOF);

		return(STRING_SCAN_NO_STRING);
	}

	lpbyEPSandwich = (LPBYTE)lpstCallBack->PermMemoryAlloc(EP_SANDWICH_SIZE);
    if (lpbyEPSandwich == NULL)
    {
        lpstCallBack->PermMemoryFree(lpbyTOF);
        lpstCallBack->PermMemoryFree(lpbyEOF);

		return(STRING_SCAN_NO_STRING);
    }

    if (GetBuffers(lpstCallBack,
                   hFile,
                   lpbyTOF,
                   &nTOFLen,
                   lpbyEOF,
                   &nEOFLen,
                   lpbyEPSandwich,
                   &nEPLen,
                   &nEPOffsetInSandwich,
                   bUseWimpyMode) != TRUE)
    {
        lpstCallBack->PermMemoryFree(lpbyTOF);
        lpstCallBack->PermMemoryFree(lpbyEOF);
        lpstCallBack->PermMemoryFree(lpbyEPSandwich);

        return(STRING_SCAN_NO_STRING);
    }

    // scan those buffers!

    // do TOF, then EOF, then sandwich first

	if (StringScan(lpstSearchInfo,
                   lpbyTOF,
                   nTOFLen) == STRING_SCAN_FOUND_STRING)
    {
        lpstCallBack->PermMemoryFree(lpbyTOF);
        lpstCallBack->PermMemoryFree(lpbyEOF);
        lpstCallBack->PermMemoryFree(lpbyEPSandwich);

        return(STRING_SCAN_FOUND_STRING);
    }

	if (StringScan(lpstSearchInfo,
                   lpbyEOF,
                   nEOFLen) == STRING_SCAN_FOUND_STRING)
    {
        lpstCallBack->PermMemoryFree(lpbyTOF);
        lpstCallBack->PermMemoryFree(lpbyEOF);
        lpstCallBack->PermMemoryFree(lpbyEPSandwich);

        return(STRING_SCAN_FOUND_STRING);
    }

    if (StringScan(lpstSearchInfo,
                   lpbyEPSandwich,
                   nEPLen) == STRING_SCAN_FOUND_STRING)
    {
        lpstCallBack->PermMemoryFree(lpbyTOF);
        lpstCallBack->PermMemoryFree(lpbyEOF);
        lpstCallBack->PermMemoryFree(lpbyEPSandwich);

        return(STRING_SCAN_FOUND_STRING);
    }

	if (lpbyDecryptedBuffer != NULL &&
		StringScan(lpstSearchInfo,
				   lpbyDecryptedBuffer,
                   nDecryptedBufferLen) == STRING_SCAN_FOUND_STRING)
    {
        lpstCallBack->PermMemoryFree(lpbyTOF);
        lpstCallBack->PermMemoryFree(lpbyEOF);
        lpstCallBack->PermMemoryFree(lpbyEPSandwich);

        return(STRING_SCAN_FOUND_STRING);
    }

    // now do EPoint/TOF offset-based scan

	if (OffsetScan(lpstSearchInfo,
                   lpbyTOF,
                   nTOFLen,
                   lpbyEPSandwich + nEPOffsetInSandwich,
                   nEPLen - nEPOffsetInSandwich) == STRING_SCAN_FOUND_STRING)
    {
        lpstCallBack->PermMemoryFree(lpbyTOF);
        lpstCallBack->PermMemoryFree(lpbyEOF);
        lpstCallBack->PermMemoryFree(lpbyEPSandwich);

        return(STRING_SCAN_FOUND_STRING);
    }

    lpstCallBack->PermMemoryFree(lpbyTOF);
    lpstCallBack->PermMemoryFree(lpbyEOF);
    lpstCallBack->PermMemoryFree(lpbyEPSandwich);

    return(STRING_SCAN_NO_STRING);
}

BOOL DetermineBootEPOffset
(
    LPBYTE                  lpbyBOOBuffer,
    LPDWORD                 lpdwEPOffset
)
{
    // dealing with boot... follow only E9 or E8 at TOF

    if (*lpbyBOOBuffer == 0xE8 || *lpbyBOOBuffer == 0xE9)
    {
        // 3 added in below is for sizeof JMP/CALL

        *lpdwEPOffset = (DWORD)(*(lpbyBOOBuffer+1) |
                            (WORD)*(lpbyBOOBuffer+2) << 8) + 3;

        return(TRUE);
    }

    *lpdwEPOffset = 0;

    return(TRUE);
}


int IBMStringScanBoot
(
    LPCALLBACKREV1          lpstCallBack,
    LPSEARCH_INFO           lpstBootInfo,
    LPSEARCH_INFO           lpstFileInfo,
    LPBYTE                  lpbyBOOBuffer           // 512
)
{
    DWORD                   dwEPOffset;

    // scan those buffers!

    // do full sector

    if (StringScan(lpstBootInfo,
                   lpbyBOOBuffer,
                   512) == STRING_SCAN_FOUND_STRING)
    {
        return(STRING_SCAN_FOUND_STRING);
    }

    if (lpstFileInfo->lpstHash != NULL)
    {
        // only use if its loaded...

        if (StringScan(lpstFileInfo,
                       lpbyBOOBuffer,
                       512) == STRING_SCAN_FOUND_STRING)
        {
            return(STRING_SCAN_FOUND_STRING);
        }
    }

    // now do EPoint offset-based scan

    if (DetermineBootEPOffset(lpbyBOOBuffer,&dwEPOffset) != TRUE)
    {
        return(STRING_SCAN_NO_STRING);
    }

    if (dwEPOffset >= 512)
        return(STRING_SCAN_NO_STRING);      // EP is out of bounds...

    if (OffsetScan(lpstBootInfo,
                   lpbyBOOBuffer,
                   512,
                   lpbyBOOBuffer + dwEPOffset,
                   512 - dwEPOffset) == STRING_SCAN_FOUND_STRING)
    {
        return(STRING_SCAN_FOUND_STRING);
    }

    return(STRING_SCAN_NO_STRING);
}





