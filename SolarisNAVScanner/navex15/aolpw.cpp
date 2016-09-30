//************************************************************************
//
// $Header:   S:/NAVEX/VCS/aolpw.cpv   1.2   12 Oct 1998 10:08:44   CNACHEN  $
//
// Description:
//  AOL password stealer source
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/aolpw.cpv  $
// 
//    Rev 1.2   12 Oct 1998 10:08:44   CNACHEN
// added NLM relinquish control
// 
//    Rev 1.1   05 Aug 1998 13:37:58   CNACHEN
// fixed to use globalalloc/globalfree for w32 platform so we don't bust NAVIEG.
// 
//    Rev 1.0   05 Aug 1998 11:37:20   CNACHEN
// Initial revision.
// 
//    Rev 1.1   03 Aug 1998 16:50:36   CNACHEN
// added code to check for init failure.
// 
//    Rev 1.0   03 Aug 1998 16:08:02   CNACHEN
// Initial revision.
// 
//************************************************************************

#include "platform.h"
#include "callback.h"
#include "n30type.h"
#include "navex.h"
#include "winconst.h"
#include "avendian.h"
#include "navexshr.h"
#include "avdatfil.h"
#include "datadir.h"
#include "aolpw.h"
#include "aolpwsig.h"




BOOL HeaderIsFromPossibleTrojan
(
    LPWININFO       lpstNEInfo              // NE header
)
{
    // check various header fields to reduce scanning on non-infected
    // files

    if (WENDIAN(lpstNEInfo->entryTabLen) != 0x2 &&
        WENDIAN(lpstNEInfo->entryTabLen) != 0x9 &&
        WENDIAN(lpstNEInfo->entryTabLen) != 0x82)
        return(FALSE);

    if (WENDIAN(lpstNEInfo->dataSegNum) != 0x2 &&
        WENDIAN(lpstNEInfo->dataSegNum) != 0x10)
        return(FALSE);

    if (WENDIAN(lpstNEInfo->ip) != 0x0 &&
        WENDIAN(lpstNEInfo->ip) != 0x10 &&
        WENDIAN(lpstNEInfo->ip) != 0x18)
        return(FALSE);

    if (WENDIAN(lpstNEInfo->cs) != 0x1 &&
        WENDIAN(lpstNEInfo->cs) != 0x3)
        return(FALSE);

    if (WENDIAN(lpstNEInfo->nonResTabSize) != 0x0c &&
        WENDIAN(lpstNEInfo->nonResTabSize) != 0x10 &&
        WENDIAN(lpstNEInfo->nonResTabSize) != 0x1c)
        return(FALSE);

    if (WENDIAN(lpstNEInfo->numEntryPoints) != 0x0 &&
        WENDIAN(lpstNEInfo->numEntryPoints) != 0x1 &&
        WENDIAN(lpstNEInfo->numEntryPoints) != 0x15)
        return(FALSE);

    if (WENDIAN(lpstNEInfo->segTabEntries) >= 0x18)
        return(FALSE);

    // passed all the filter tests for the header

    return(TRUE);
}

BOOL AOLStringFoundInSegments
(
    LPCALLBACKREV1  lpCallBack,
    HFILE           hFile,
    LPWININFO       lpstNEInfo,             // NE header
    DWORD           dwWinHeaderOffset,
    LPBYTE          lpbyWorkBuffer
)
{
    WORD            wIndex, wMaxIndex, wFlags, i, wSecLen;
    DWORD           dwOffset, dwSecLen;

    // verify that AOL is within 1024 bytes of the start of sections 3,4,5 or 6
    // or find WINSOCK in the module table...

    wMaxIndex = WENDIAN(lpstNEInfo->segTabEntries) < 6 ?
                WENDIAN(lpstNEInfo->segTabEntries) : 6;

    for (wIndex = 3; wIndex <= wMaxIndex ; wIndex++)
	{
        // locate the section...

        EXTSTATUS GetNESectionInfo
        (
            LPCALLBACKREV1  lpCallBack,             // File op callbacks
            HFILE           hFile,                  // Handle to file to scan
            LPWININFO       lpstNEInfo,             // NE header
            LPBYTE          lpbyWorkBuffer,         // Work buffer >= 512 bytes
            DWORD           dwWinHeaderOffset,      // where's the WIN header start?
            WORD            wSecNum,                // what section are we
                                                    //  interested in (0-base)?
            LPDWORD         lpdwFileOffset,         // where is this section from TOF?
            LPDWORD         lpdwSecLen,             // how long is this section?
            LPWORD          lpwFlags                // segment flags
        );


        if (GetNESectionInfo(lpCallBack,
                             hFile,
							 lpstNEInfo,
							 lpbyWorkBuffer,
							 dwWinHeaderOffset,
                             wIndex,
                             &dwOffset,
							 &dwSecLen,
                             &wFlags) != EXTSTATUS_OK)
            return(FALSE);

        // read the section

        if (dwSecLen > 1024)
            wSecLen = 1024;
        else
            wSecLen = (WORD)dwSecLen;

        // make sure we have enough room!

        if (wSecLen < 3)
            return(FALSE);

        if (lpCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
        {
            return(FALSE);
        }

        if (lpCallBack->FileRead(hFile,lpbyWorkBuffer,wSecLen) != wSecLen)
        {
            return(FALSE);
        }

        // subtract out 3 from our scan distance since we don't want to
        // exceed our buffer length

        wSecLen -= 3;

        // see if we have "AOL" in the first dwSecLenBytes

        for (i=0;i<wSecLen;i++)
        {
            if (lpbyWorkBuffer[i] == 'A' &&
                lpbyWorkBuffer[i+1] == 'O' &&
                lpbyWorkBuffer[i+2] == 'L')
            {
                // hit!

                return(TRUE);
            }
        }
    }

    // no hit on AOL... try to find "WINSOCK" in module table

    if (lpCallBack->FileSeek(hFile,
                             dwWinHeaderOffset +
                                WENDIAN(lpstNEInfo->modTabOffset),
                             SEEK_SET) != dwWinHeaderOffset +
											WENDIAN(lpstNEInfo->modTabOffset))
    {
        return(FALSE);
    }

    if (lpCallBack->FileRead(hFile,lpbyWorkBuffer,MODULE_TABLE_SIZE) !=
        MODULE_TABLE_SIZE)
    {
        return(FALSE);
    }

    for (i=0;i<MODULE_TABLE_SIZE;i++)
    {
        if (lpbyWorkBuffer[i] == 'N' &&
            lpbyWorkBuffer[i+1] == 'S' &&
            lpbyWorkBuffer[i+2] == 'O' &&
            lpbyWorkBuffer[i+3] == 'C' &&
            lpbyWorkBuffer[i+4] == 'K')
        {
            return(TRUE);
        }
    }

    // nope!

    return(FALSE);
}

#ifdef NAVEX15

EXTSTATUS LoadAOLData
(
    LPCALLBACKREV1  lpCallBack,
    LPTSTR          lpszNAVEXDataDir
)
{
    HFILE                                   hFile;
    TCHAR                                   szDatFileName[SYM_MAX_PATH];
    DWORD                                   dwOffset, dwLength;
    AOLPW_HEADER_T                          stHeader;
    DWORD                                   i, j;

    BOOL VDFLookUp(LPCALLBACKREV1 lpCallBack,HFILE hFile,DWORD dwIdent,LPDWORD lpdwOffset,LPDWORD lpdwLength);

    // zero out global data first in case we fail to load

    gstAOLPWInfo.dwNumStrings = 0;
    gstAOLPWInfo.dwNumProfiles = 0;
    gstAOLPWInfo.lpstSigArray = NULL;
    gstAOLPWInfo.lpstProfileArray = NULL;

    // virscan5.dat contains our sig data...

#if defined(SYM_UNIX)
    DataDirAppendName(lpszNAVEXDataDir,_T("virscan5.dat"),szDatFileName);
#else
    DataDirAppendName(lpszNAVEXDataDir,_T("VIRSCAN5.DAT"),szDatFileName);
#endif

    hFile = lpCallBack->FileOpen(szDatFileName,0);

    if (hFile == (HFILE)-1)
        return(EXTSTATUS_FILE_ERROR);

    // look up our section!

    if (VDFLookUp(lpCallBack,
                  hFile,
                  AOLPW_SECTION_ID,
                  &dwOffset,
                  &dwLength) != TRUE)
    {
        lpCallBack->FileClose(hFile);
        return(EXTSTATUS_FILE_ERROR);
    }

    // seek to it and read it!

    if (lpCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
    {
        lpCallBack->FileClose(hFile);
        return(EXTSTATUS_FILE_ERROR);
    }

    if (lpCallBack->FileRead(hFile,
                               &stHeader,
                               sizeof(stHeader)) != sizeof(stHeader))
    {
        lpCallBack->FileClose(hFile);
        return(EXTSTATUS_FILE_ERROR);
    }

    // endianize!

    stHeader.dwVersion = DWENDIAN(stHeader.dwVersion);
    stHeader.dwNumStrings = DWENDIAN(stHeader.dwNumStrings);
    stHeader.dwNumProfiles = DWENDIAN(stHeader.dwNumProfiles);

    if (stHeader.dwVersion != 1)
    {
        lpCallBack->FileClose(hFile);
        return(EXTSTATUS_FILE_ERROR);
    }

    // now allocate memory and read the strings into memory

    if (stHeader.dwNumStrings > MAX_AOLPW_STRINGS ||
        stHeader.dwNumProfiles > MAX_AOLPW_PROFILES)
    {
        // this is not within reason! error!

        lpCallBack->FileClose(hFile);
        return(EXTSTATUS_FILE_ERROR);
    }

    gstAOLPWInfo.lpstSigArray =
        (LPAOLPW_SIG)lpCallBack->PermMemoryAlloc(sizeof(AOLPW_SIG_T) *
                                                 stHeader.dwNumStrings);

    if (gstAOLPWInfo.lpstSigArray == NULL)
    {
        lpCallBack->FileClose(hFile);
        return(EXTSTATUS_FILE_ERROR);
    }

    // read those strings

    for (i=0;i<stHeader.dwNumStrings;i++)
    {
        if (lpCallBack->FileRead(hFile,
                                   gstAOLPWInfo.lpstSigArray+i,
                                   sizeof(AOLPW_SIG_T)) != sizeof(AOLPW_SIG_T))
        {
            lpCallBack->PermMemoryFree(gstAOLPWInfo.lpstSigArray);
            gstAOLPWInfo.lpstSigArray = NULL;
            lpCallBack->FileClose(hFile);
            return(EXTSTATUS_FILE_ERROR);
        }

        // endianize

        gstAOLPWInfo.lpstSigArray[i].wEncryptedSigLen =
            WENDIAN(gstAOLPWInfo.lpstSigArray[i].wEncryptedSigLen);

        // no need to endianize the wFirstWORD since the byte ordering will
        // be the same in the data file and in the executables we're scanning
    }

    // now allocate memory and  read in the profiles

    gstAOLPWInfo.lpstProfileArray =
        (LPAOLPW_PROFILE)lpCallBack->PermMemoryAlloc(sizeof(AOLPW_PROFILE_T) *
                                                     stHeader.dwNumProfiles);

    if (gstAOLPWInfo.lpstProfileArray == NULL)
    {
        lpCallBack->PermMemoryFree(gstAOLPWInfo.lpstSigArray);
        gstAOLPWInfo.lpstSigArray = NULL;
        lpCallBack->FileClose(hFile);
        return(EXTSTATUS_FILE_ERROR);
    }

    // read those profiles

    for (i=0;i<stHeader.dwNumProfiles;i++)
    {
        if (lpCallBack->FileRead(hFile,
                                   gstAOLPWInfo.lpstProfileArray+i,
                                   sizeof(AOLPW_PROFILE_T)) !=
            sizeof(AOLPW_PROFILE_T))
        {
            lpCallBack->PermMemoryFree(gstAOLPWInfo.lpstSigArray);
            gstAOLPWInfo.lpstSigArray = NULL;
            lpCallBack->PermMemoryFree(gstAOLPWInfo.lpstProfileArray);
            gstAOLPWInfo.lpstProfileArray = NULL;
            lpCallBack->FileClose(hFile);
            return(EXTSTATUS_FILE_ERROR);
        }

        // endianize

        gstAOLPWInfo.lpstProfileArray[i].wNumStringsToMatch =
            WENDIAN(gstAOLPWInfo.lpstProfileArray[i].wNumStringsToMatch);

        gstAOLPWInfo.lpstProfileArray[i].wVID =
            WENDIAN(gstAOLPWInfo.lpstProfileArray[i].wVID);

        for (j=0;j<gstAOLPWInfo.lpstProfileArray[i].wNumStringsToMatch;j++)
        {
            gstAOLPWInfo.lpstProfileArray[i].wIndexList[j] =
                WENDIAN(gstAOLPWInfo.lpstProfileArray[i].wIndexList[j]);
        }
    }

    // success - close our file and update our global structure to reflect
    // our loaded data!

    lpCallBack->FileClose(hFile);

    gstAOLPWInfo.dwNumStrings = stHeader.dwNumStrings;
    gstAOLPWInfo.dwNumProfiles = stHeader.dwNumProfiles;

    return(EXTSTATUS_OK);
}

EXTSTATUS FreeAOLData
(
    LPCALLBACKREV1  lpCallBack
)
{
    // free sig and profile arrays

    if (gstAOLPWInfo.lpstSigArray != NULL)
    {
        lpCallBack->PermMemoryFree(gstAOLPWInfo.lpstSigArray);
        gstAOLPWInfo.lpstSigArray = NULL;
    }

    if (gstAOLPWInfo.lpstProfileArray != NULL)
    {

        lpCallBack->PermMemoryFree(gstAOLPWInfo.lpstProfileArray);
        gstAOLPWInfo.lpstProfileArray = NULL;
    }

    gstAOLPWInfo.dwNumStrings = 0;
    gstAOLPWInfo.dwNumProfiles = 0;

    return(EXTSTATUS_OK);
}

EXTSTATUS ZeroAOLData
(
    void
)
{
    gstAOLPWInfo.lpstSigArray = NULL;
    gstAOLPWInfo.lpstProfileArray = NULL;

    gstAOLPWInfo.dwNumStrings = 0;
    gstAOLPWInfo.dwNumProfiles = 0;

    return(EXTSTATUS_OK);
}

#endif // #ifdef NAVEX15

EXTSTATUS AOLFindStringsInFile
(
    LPCALLBACKREV1  lpCallBack,
    HFILE           hFile,
    LPBYTE          lpbyScanBuffer,
    LPBYTE          lpbyStringMatches,
    LPBOOL          lpbAbort
)
{
    UINT            uLen, uStop;
    int             i,j,k,nStrings;
    LPAOLPW_SIG     lpstSigArray;
    BYTE            byUpper;
    WORD            wUpperHash;
    long            lMaxScan = 256000L;

    *lpbAbort = FALSE;                  // no exclusions found yet

    nStrings = (int)gstAOLPWInfo.dwNumStrings;

    // seek to the TOF

    if (lpCallBack->FileSeek(hFile,0,SEEK_SET) != 0)
    {
        return(EXTSTATUS_FILE_ERROR);
    }

    // grunt scan


    do
    {
#ifdef SYM_NLM
        lpCallBack->Progress(0,0);      // for NLM
#endif

        uLen = lpCallBack->FileRead(hFile,lpbyScanBuffer,AOLPW_BUFFER_SIZE);
        if (uLen == (UINT)-1)
        {
            return(EXTSTATUS_FILE_ERROR);
        }

        if (uLen >= sizeof(WORD))
        {
            uStop = uLen - sizeof(WORD);

            for (i=0;i<uStop;i++)
            {
                lpstSigArray = gstAOLPWInfo.lpstSigArray;

                if (lpbyScanBuffer[i] >= 'a' && lpbyScanBuffer[i] <= 'z')
                    wUpperHash = (BYTE)(lpbyScanBuffer[i] - 0x20);
                else
                    wUpperHash = lpbyScanBuffer[i];

                if (lpbyScanBuffer[i+1] >= 'a' && lpbyScanBuffer[i+1] <= 'z')
                    wUpperHash |= ((WORD)(BYTE)(lpbyScanBuffer[i+1] - 0x20)) << 8;
                else
                    wUpperHash |= ((WORD)lpbyScanBuffer[i+1]) << 8;

                // got a hit on first two bytes, look further

                for (j=0;j<nStrings;j++,lpstSigArray++)
                {
                    if (wUpperHash == lpstSigArray->wFirstWORD &&
                        uLen - i - sizeof(WORD) >=
                            lpstSigArray->wEncryptedSigLen)
                    {
                        for (k=0;k<lpstSigArray->wEncryptedSigLen;k++)
                        {
                            // +1 encryption on sigs

                            byUpper = lpbyScanBuffer[i+k+sizeof(WORD)];
                            if (byUpper >= 'a' && byUpper <= 'z')
                                byUpper -= 0x20;

                            if (byUpper !=
                                (BYTE)(lpstSigArray->byEncryptedSig[k]-1))
                                break;
                        }

                        if (k == lpstSigArray->wEncryptedSigLen)
                        {
                            // hit! record that we found a string

                            if (lpstSigArray->bCleanFile == TRUE)
                            {
                                // hit exclusion sig!  no virus!

                                *lpbAbort = TRUE;

                                return(EXTSTATUS_OK);
                            }

                            lpbyStringMatches[j] = TRUE;
                        }
                    }
                }
            }
        }

        // make sure to overlap our buffer reads so we don't miss
        // anything

        if (uLen == AOLPW_BUFFER_SIZE)
        {
            // seek back to take care of overlap!

            if (lpCallBack->FileSeek(hFile,
                                     -(int)AOLPW_TOTAL_SIG_LEN,
                                     SEEK_CUR) ==
                (DWORD)-1)
            {
                return(EXTSTATUS_FILE_ERROR);
            }
        }

        // scan a max of 256K

        lMaxScan -= uLen;
    }
    while (lMaxScan > 0 && uLen == AOLPW_BUFFER_SIZE);

    return(EXTSTATUS_OK);
}

EXTSTATUS AOLMatchPWProfiles
(
    LPCALLBACKREV1  lpCallBack,
    HFILE           hFile,
    LPWININFO       lpstNEInfo,
    DWORD           dwWinHeaderOffset,
    LPWORD          lpwVID
)
{
    LPBYTE          lpbyStringMatches, lpbyBuffer;
    int             i, j;
    LPAOLPW_PROFILE lpstProfileArray;
    BOOL            bAbort;

    // allocate a buffer of bytes to store our string statuses and init to 0

#ifndef SYM_WIN32
    lpbyStringMatches =
        (LPBYTE)lpCallBack->PermMemoryAlloc(gstAOLPWInfo.dwNumStrings);
#else
    lpbyStringMatches =
        (LPBYTE)GlobalAlloc(GMEM_FIXED,gstAOLPWInfo.dwNumStrings);
#endif

    if (lpbyStringMatches == NULL)
        return(EXTSTATUS_MEM_ERROR);

    for (i=0;i<gstAOLPWInfo.dwNumStrings;i++)
        lpbyStringMatches[i] = FALSE;

    // allocate a buffer for big disk reads

#ifndef SYM_WIN32
    lpbyBuffer = (LPBYTE)lpCallBack->PermMemoryAlloc(AOLPW_BUFFER_SIZE);
#else
    lpbyBuffer = (LPBYTE)GlobalAlloc(GMEM_FIXED,AOLPW_BUFFER_SIZE);
#endif

    if (lpbyBuffer == NULL)
    {
#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyStringMatches);
#else
        GlobalFree(lpbyStringMatches);
#endif
        return(EXTSTATUS_MEM_ERROR);
    }

    // seek to start of file

    if (lpCallBack->FileSeek(hFile,0,SEEK_SET) != 0)
    {

#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyStringMatches);
        lpCallBack->PermMemoryFree(lpbyBuffer);
#else
        GlobalFree(lpbyStringMatches);
        GlobalFree(lpbyBuffer);
#endif

        return(EXTSTATUS_FILE_ERROR);
    }

    // start blowing through the file

    if (AOLFindStringsInFile(lpCallBack,
                             hFile,
                             lpbyBuffer,
                             lpbyStringMatches,
                             &bAbort) !=
            EXTSTATUS_OK)
    {

#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyStringMatches);
        lpCallBack->PermMemoryFree(lpbyBuffer);
#else
        GlobalFree(lpbyStringMatches);
        GlobalFree(lpbyBuffer);
#endif

        return(EXTSTATUS_FILE_ERROR);
    }

    if (bAbort == TRUE)
    {
#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyStringMatches);
        lpCallBack->PermMemoryFree(lpbyBuffer);
#else
        GlobalFree(lpbyStringMatches);
        GlobalFree(lpbyBuffer);
#endif


        return(EXTSTATUS_OK);
    }

    // now we've blown through the entire file - determine whether or not
    // any of our profiles match...

    lpstProfileArray = gstAOLPWInfo.lpstProfileArray;

    for (i=0;i<gstAOLPWInfo.dwNumProfiles;i++,lpstProfileArray++)
    {
        for (j=0;j<lpstProfileArray->wNumStringsToMatch;j++)
        {
            if (lpbyStringMatches[lpstProfileArray->wIndexList[j]] != TRUE)
                break;
        }

        if (j == lpstProfileArray->wNumStringsToMatch)
        {
            // we have a hit!

#ifndef SYM_WIN32
            lpCallBack->PermMemoryFree(lpbyStringMatches);
            lpCallBack->PermMemoryFree(lpbyBuffer);
#else
            GlobalFree(lpbyStringMatches);
            GlobalFree(lpbyBuffer);
#endif

            *lpwVID = lpstProfileArray->wVID;
            return(EXTSTATUS_VIRUS_FOUND);
        }
    }

#ifndef SYM_WIN32
    lpCallBack->PermMemoryFree(lpbyStringMatches);
    lpCallBack->PermMemoryFree(lpbyBuffer);
#else
    GlobalFree(lpbyStringMatches);
    GlobalFree(lpbyBuffer);
#endif

    return(EXTSTATUS_OK);
}


EXTSTATUS AOLPWBloodhound
(
    LPCALLBACKREV1  lpCallBack,
    HFILE           hFile,
    LPWININFO       lpstNEInfo,             // NE header
    DWORD           dwWinHeaderOffset,
    LPBYTE          lpbyWorkBuffer,
    LPWORD          lpwVID
)
{
    if (gstAOLPWInfo.lpstSigArray == NULL ||
        gstAOLPWInfo.lpstProfileArray == NULL)
    {
        // init failure! no worries

        return(EXTSTATUS_OK);
    }

    // filter based on header

    if (HeaderIsFromPossibleTrojan(lpstNEInfo) == FALSE)
        return(EXTSTATUS_OK);

    // filter by finding a few strings in known places

    if (AOLStringFoundInSegments(lpCallBack,
                                 hFile,
                                 lpstNEInfo,             // NE header
                                 dwWinHeaderOffset,
                                 lpbyWorkBuffer) == FALSE)
        return(EXTSTATUS_OK);

    // blast through first 256K or so of file for strings...

    return(AOLMatchPWProfiles(lpCallBack,
                              hFile,
                              lpstNEInfo,
                              dwWinHeaderOffset,
                              lpwVID));
}

