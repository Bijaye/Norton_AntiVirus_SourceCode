// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/LDMCRSIG.CPv   1.10   15 Dec 1998 12:11:02   DCHI  $
//
// Description:
//
//  This source file contains routines for loading the macro signature
//  set.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/LDMCRSIG.CPv  $
// 
//    Rev 1.10   15 Dec 1998 12:11:02   DCHI
// Updated for P97 signature set.
// 
//    Rev 1.9   09 Dec 1998 17:45:22   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.8   04 Aug 1998 13:52:06   DCHI
// Changes for Excel heuristics.
// 
//    Rev 1.7   10 Jun 1998 13:20:42   DCHI
// Added W7H2
// 
//    Rev 1.6   11 May 1998 18:17:20   DCHI
// Changes for menu repair and
// for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.5   15 Apr 1998 17:23:38   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.4   04 Nov 1997 17:11:54   DCHI
// Added code to not allocate exclusion memory if there are no
// exclusion sigs.
// 
//    Rev 1.3   15 Oct 1997 18:15:46   DCHI
// Added Word 97 heuristics.
// 
//    Rev 1.2   28 Jul 1997 16:16:58   DCHI
// Added use of DataDirAppendName().
// 
//    Rev 1.1   14 Jul 1997 14:49:44   DCHI
// Added code in DX to check for VIRSCAN6.INI OPTIONAL keyword to specify
// that VIRSCAN6.DAT is optional.
// 
//    Rev 1.0   09 Jul 1997 16:15:24   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef MACRODAT

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"
#include "wdscan15.h"

#include "callback.h"

#if !defined( __MACINTOSH__ )
#include "datadir.h"
#endif

#include "ldmcrsig.h"
#include "w7h2sght.h"
#include "mcrxclud.h"

MACRO_EXCLUSION_SET_T gastMacroExclusionSets[NUM_MACRO_EXCLUSION_SETS] =
{
    { WD7_EXCLUSION_BASE_ID, 0, NULL, NULL },
    { WD8_EXCLUSION_BASE_ID, 0, NULL, NULL },
    { X95_EXCLUSION_BASE_ID, 0, NULL, NULL },
    { X97_EXCLUSION_BASE_ID, 0, NULL, NULL }
};

WD_SIG_SET_T gstWD7SigSet;
WD_SIG_SET_T gstWD8SigSet;
WD_SIG_SET_T gstXL97SigSet;
WD_SIG_SET_T gstXL95SigSet;
WD_SIG_SET_T gstXL4SigSet;
WD_SIG_SET_T gstA2SigSet;
WD_SIG_SET_T gstA97SigSet;
WD_SIG_SET_T gstP97SigSet;

W7H2_SIG_SET_T gstW7H2SigSet;

typedef struct tagMACRO_SIG_SET_INFO
{
    DWORD           dwBaseID;
    LPWD_SIG_SET    lpstSigSet;
    DWORD           dwNumPools;
    LPBYTE FAR *    lplpabyPools;
} MACRO_SIG_SET_INFO_T, FAR *LPMACRO_SIG_SET_INFO;

MACRO_SIG_SET_INFO_T gastMacroSigSetInfo[NUM_MACRO_SIG_SETS] =
{
    {
        WD7_BASE_ID,
        &gstWD7SigSet,
        0,
        NULL
    },
    {
        WD8_BASE_ID,
        &gstWD8SigSet,
        0,
        NULL
    },
    {
        XL97_BASE_ID,
        &gstXL97SigSet,
        0,
        NULL
    },
    {
        XL95_BASE_ID,
        &gstXL95SigSet,
        0,
        NULL
    },
    {
        XL4_BASE_ID,
        &gstXL4SigSet,
        0,
        NULL
    },
    {
        A2_BASE_ID,
        &gstA2SigSet,
        0,
        NULL
    },
    {
        A97_BASE_ID,
        &gstA97SigSet,
        0,
        NULL
    },
    {
        P97_BASE_ID,
        &gstP97SigSet,
        0,
        NULL
    }
};

WD_SIG_INFO_T FAR gstEmptyWD_SIG_INFO_T = { 0, NULL };
WD_NAMED_CRC_SIG_INFO_T FAR gstEmptyWD_NAMED_CRC_SIG_INFO_T =
    { 0x00000000, 0x0000, NULL };
DWORD gdwZero = 0;
WD_VIRUS_SIG_INFO_T FAR gstEmptyWD_VIRUS_SIG_INFO_T = { 0, NULL };
LPF_WD_CUSTOM_SCAN FAR gapfEmptyCustomScan[] = { NULL };
LPF_WD_CUSTOM_REPAIR FAR gapfEmptyCustomRepair[] = { NULL };


//********************************************************************
//
// Function:
//  BOOL VDFLookUp()
//
// Parameters:
//  lpstCallBack        Callbacks to access files
//  hFile               Handle to open data file
//  dwIdent             Identifier of section to look up
//  lpdwOffset          Ptr to DWORD for offset of section
//  lpdwLength          Ptr to DWORD for length of section
//
// Description:
//  This function attempts to locate the specified section in the
//  specified data file.  If it does locate the section, it fills
//  in the DWORDs pointed to by lpdwOffset and lpdwLength the start
//  offset and length of the section, respectively.  If the section
//  is found the function returns TRUE.  Otherwise, the function
//  returns error if the section is not found, there was an error,
//  or the header signature does not match.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL VDFLookUp
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwIdent,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwLength
)
{
    VDF_HEADER_T        stHeader;
    VDF_TABLE_T         stTableEntry;
    WORD                wCurSection;

    // seek to and read the header in so we can locate the section table...

    if (lpstCallBack->FileSeek(hFile,0,SEEK_SET) != 0)
        return(FALSE);

    if (lpstCallBack->FileRead(hFile,&stHeader,sVDF_HEADER) != sVDF_HEADER)
        return(FALSE);

    // endian enable that structure...

    stHeader.dwSignature = DWENDIAN(stHeader.dwSignature);
    stHeader.wNumSections = WENDIAN(stHeader.wNumSections);
    stHeader.dwTableOffset = DWENDIAN(stHeader.dwTableOffset);

    // check the file signature...

    if (stHeader.dwSignature != VDF_SIGNATURE)
        return(FALSE);

    // seek to the data file section table

    if (lpstCallBack->FileSeek(hFile,
                               stHeader.dwTableOffset,
                               SEEK_SET) != stHeader.dwTableOffset)
        return(FALSE);

    // search in the table for the proper ID so we can find where the
    // section is in our data file

    for (wCurSection=0;wCurSection<stHeader.wNumSections;wCurSection++)
    {
        if (lpstCallBack->FileRead(hFile,
                                   &stTableEntry,
                                   sVDF_TABLE) != sVDF_TABLE)
            return(FALSE);

        // found the specified table entry.  report to the user

        if (DWENDIAN(stTableEntry.dwIdent) == dwIdent)
        {
            *lpdwOffset = DWENDIAN(stTableEntry.dwStartOffset);
            *lpdwLength = DWENDIAN(stTableEntry.dwLength);

            return(TRUE);
        }
    }

    return(FALSE);
}


//********************************************************************
//
// Function:
//  BOOL LoadWORDValue()
//
// Parameters:
//  lpstLoad            Load structure
//  lpwValue            Ptr to WORD storage
//
// Description:
//  This function retrieves the next WORD value from the info
//  stream.  The info data is buffered in chunks of
//  LOAD_MACRO_SIG_INFO_BUF_SIZE bytes.  So that reads to disk
//  are only done at those intervals.  Otherwise, the WORD value
//  is taken directly from the load structure's internal buffer.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL LoadWORDValue
(
    LPLOAD_MACRO_SIG    lpstLoad,
    LPWORD              lpwValue
)
{
    if (lpstLoad->dwNumInfoBufBytes == 0)
    {
        // Refill the buffer

        if (lpstLoad->lpstCallBack->
            FileSeek(lpstLoad->hFile,
                     lpstLoad->dwInfoOffset,
                     SEEK_SET) != lpstLoad->dwInfoOffset)
            return(FALSE);

        if (lpstLoad->dwInfoLength < LOAD_MACRO_SIG_INFO_BUF_SIZE)
            lpstLoad->dwNumInfoBufBytes = lpstLoad->dwInfoLength;
        else
            lpstLoad->dwNumInfoBufBytes = LOAD_MACRO_SIG_INFO_BUF_SIZE;

        // Update next offset and length left

        lpstLoad->dwInfoOffset += lpstLoad->dwNumInfoBufBytes;
        lpstLoad->dwInfoLength -= lpstLoad->dwNumInfoBufBytes;

        if (lpstLoad->lpstCallBack->
            FileRead(lpstLoad->hFile,
                     lpstLoad->abyInfoBuf,
                     (UINT)lpstLoad->dwNumInfoBufBytes) !=
            lpstLoad->dwNumInfoBufBytes)
            return(FALSE);

        // Start at beginning of buffer

        lpstLoad->dwInfoBufIndex = 0;
    }

    if (lpstLoad->dwNumInfoBufBytes < 2)
    {
        lpstLoad->dwNumInfoBufBytes = 0;
        return(FALSE);
    }

    *lpwValue = (WORD)(lpstLoad->abyInfoBuf[lpstLoad->dwInfoBufIndex]) |
        (((WORD)lpstLoad->abyInfoBuf[lpstLoad->dwInfoBufIndex + 1]) << 8);

    lpstLoad->dwNumInfoBufBytes -= sizeof(WORD);
    lpstLoad->dwInfoBufIndex += sizeof(WORD);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL LoadPoolInfo()
//
// Parameters:
//  lpstLoad            Load structure
//  lplpbyPool          Ptr to storage for pool pointer
//
// Description:
//  This function retrieves two WORD values from the info stream,
//  consisting of the pool number and the offset in the pool.
//  *lplpbyPool is set to point to the beginning of the area
//  specified by the pool number and offset from the info stream.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL LoadPoolInfo
(
    LPLOAD_MACRO_SIG    lpstLoad,
    LPBYTE FAR *        lplpbyPool
)
{
    WORD                wPoolNum;
    WORD                wPoolOffset;

    // Get signature pool number and offset

    if (LoadWORDValue(lpstLoad,
                      &wPoolNum) == FALSE)
        return(FALSE);

    if (LoadWORDValue(lpstLoad,
                      &wPoolOffset) == FALSE)
        return(FALSE);

    if (wPoolNum > lpstLoad->wNumPools)
        return(FALSE);

    *lplpbyPool = lpstLoad->lpalpbyPools[wPoolNum] + wPoolOffset;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL LoadWORDandPoolInfo()
//
// Parameters:
//  lpstLoad            Load structure
//  lpwValue            Ptr to WORD for first retrieved WORD value
//  lplpbyPool          Ptr to storage for pool pointer
//
// Description:
//  The function first retrieves a WORD value from the info stream.
//  This function then retrieves two WORD values from the info stream,
//  consisting of the pool number and the offset in the pool.
//  *lplpbyPool is set to point to the beginning of the area
//  specified by the pool number and offset from the info stream.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL LoadWORDandPoolInfo
(
    LPLOAD_MACRO_SIG    lpstLoad,
    LPWORD              lpwValue,
    LPBYTE FAR *        lplpbyPool
)
{
    WORD                wPoolNum;
    WORD                wPoolOffset;

    // Get WORD value

    if (LoadWORDValue(lpstLoad,
                      lpwValue) == FALSE)
        return(FALSE);

    // Get pool number and offset

    if (LoadWORDValue(lpstLoad,
                      &wPoolNum) == FALSE)
        return(FALSE);

    if (LoadWORDValue(lpstLoad,
                      &wPoolOffset) == FALSE)
        return(FALSE);

    if (wPoolNum > lpstLoad->wNumPools)
        return(FALSE);

    *lplpbyPool = lpstLoad->lpalpbyPools[wPoolNum] + wPoolOffset;

    return(TRUE);
}


//********************************************************************
//
// BOOL LoadMacroSigSet()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//  hFile               Handle to open data file
//  dwBaseID            Base ID of signature set data
//  lpstSigSet          Ptr to signature structure to fill
//  lpdwPoolCount       Ptr to DWORD for number of allocated pools
//  lplpalpbyPools      Ptr to ptr for array of pool ptrs
//
// Description:
//  Loads the data for a signature set.
//
//  Buffers required:
//   alpabyPools[wNumPools]
//   abyPools[wNumPools][]
//   astRawNameSigIndex[]
//   astNamedCRCSigInfo[]
//   astRawMacroSigIndex[]
//   abyCRCPool[]
//   astVirusSigInfo[]
//
//  Total Number of Pools excluding pool array buffer:
//      wNumPools + 5
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL LoadMacroSigSet
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwBaseID,
    LPWD_SIG_SET        lpstSigSet,
    LPDWORD             lpdwPoolCount,
    LPBYTE FAR * FAR *  lplpalpbyPools
)
{
    LOAD_MACRO_SIG_T    stLoad;
    DWORD               dwDataOffset;
    DWORD               dwDataLength;
    WORD                w;
    WORD                wPoolSize;
    WORD                wCount;

    stLoad.lpstCallBack = lpstCallBack;
    stLoad.hFile = hFile;
    stLoad.dwNumInfoBufBytes = 0;

    // Look up the info section

    if (VDFLookUp(lpstCallBack,
                  hFile,
                  dwBaseID + ID_OFFSET_INFO_SECTION,
                  &stLoad.dwInfoOffset,
                  &stLoad.dwInfoLength) == FALSE)
        return(FALSE);

    // Look up the raw data section

    if (VDFLookUp(lpstCallBack,
                  hFile,
                  dwBaseID + ID_OFFSET_DATA_SECTION,
                  &dwDataOffset,
                  &dwDataLength) == FALSE)
        return(FALSE);


    /////////////////////////////////////////////////////////////
    // Get pool data
    /////////////////////////////////////////////////////////////

    // Get the number of Pools

    if (LoadWORDValue(&stLoad,&stLoad.wNumPools) == FALSE)
        return(FALSE);

    // Assume a limit of 1024 pools

    if (stLoad.wNumPools > 1024)
        return(FALSE);

    stLoad.lpalpbyPools = (LPBYTE FAR *)lpstCallBack->
        PermMemoryAlloc((stLoad.wNumPools + 5) * sizeof(LPBYTE));

    if (stLoad.lpalpbyPools == NULL)
        return(FALSE);

    *lpdwPoolCount = stLoad.wNumPools + 5;
    *lplpalpbyPools = stLoad.lpalpbyPools;

    // Set pointers to NULL

    for (w=0;w<(stLoad.wNumPools+5);w++)
        stLoad.lpalpbyPools[w] = NULL;

    // Now read pool sizes, allocate pools, and read pool data

    for (w=0;w<stLoad.wNumPools;w++)
    {
        // Get pool size

        if (LoadWORDValue(&stLoad,&wPoolSize) == FALSE)
            return(FALSE);

        // Allocate pool memory

        stLoad.lpalpbyPools[w] = (LPBYTE)lpstCallBack->
            PermMemoryAlloc(wPoolSize * sizeof(BYTE));

        if (stLoad.lpalpbyPools[w] == NULL)
            return(FALSE);

        // Load pool data

        if (lpstCallBack->FileSeek(hFile,
                                   dwDataOffset,
                                   SEEK_SET) != dwDataOffset)
            return(FALSE);

        if (lpstCallBack->FileRead(hFile,
                                   stLoad.lpalpbyPools[w],
                                   wPoolSize) != wPoolSize)
            return(FALSE);

        dwDataOffset += wPoolSize;
    }


    /////////////////////////////////////////////////////////////
    // Get name signature data
    /////////////////////////////////////////////////////////////

    // Get total number of name signatures

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->lNumNameSigs = wCount;

    ////////////////////////////////////////////////////////
    // Name set and end offset signatures

    // Name set offset signatures

    if (LoadWORDandPoolInfo(&stLoad,
                            &wCount,
                            &lpstSigSet->lpabyNameSetOffsetSigs) == FALSE)
        return(FALSE);

    lpstSigSet->lNumNameSetOffsetSigs = wCount;

    // Name end offset signatures

    if (LoadWORDandPoolInfo(&stLoad,
                            &wCount,
                            &lpstSigSet->lpabyNameEndOffsetSigs) == FALSE)
        return(FALSE);

    lpstSigSet->lNumNameEndOffsetSigs = wCount;

    ////////////////////////////////////////////////////////
    // Name raw signatures

    // Get number of name raw signature sets

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    if (wCount == 0)
    {
        lpstSigSet->lpastRawNameSigIndex = &gstEmptyWD_SIG_INFO_T;
    }
    else
    {
        // Allocate memory for raw name signature set index

        stLoad.lpalpbyPools[stLoad.wNumPools + POOL_RAW_NAME_SIG_INDEX] =
            (LPBYTE)lpstCallBack->
                PermMemoryAlloc(wCount * sizeof(WD_SIG_INFO_T));

        if (stLoad.lpalpbyPools[stLoad.wNumPools + POOL_RAW_NAME_SIG_INDEX] ==
            NULL)
            return(FALSE);

        lpstSigSet->lpastRawNameSigIndex = (LPWD_SIG_INFO)
            stLoad.lpalpbyPools[stLoad.wNumPools + POOL_RAW_NAME_SIG_INDEX];

        // Get raw name signature sets

        for (w=0;w<wCount;w++)
        {
            if (LoadWORDandPoolInfo(&stLoad,
                                    &lpstSigSet->lpastRawNameSigIndex[w].
                                        wBaseID,
                                    &lpstSigSet->lpastRawNameSigIndex[w].
                                        lpabySigs) == FALSE)
                return(FALSE);
        }
    }

    // Get name signature raw byte hit array

    if (LoadPoolInfo(&stLoad,
                     &lpstSigSet->lpabyNameSigRawByteHit) == FALSE)
        return(FALSE);


    /////////////////////////////////////////////////////////////
    // Named CRC signatures
    /////////////////////////////////////////////////////////////

    // Get number of named CRC signatures

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->lNumNamedCRCSigs = wCount;

    // Get number of name raw signature sets

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->lNumNamedCRCSigSets = wCount;

    if (wCount == 0)
    {
        lpstSigSet->lpastNamedCRCSigInfo = &gstEmptyWD_NAMED_CRC_SIG_INFO_T;
    }
    else
    {
        // Allocate memory for named CRC signature sets

        stLoad.lpalpbyPools[stLoad.wNumPools + POOL_NAMED_CRC_SIG_INFO] =
            (LPBYTE)lpstCallBack->
                PermMemoryAlloc(wCount * sizeof(WD_NAMED_CRC_SIG_INFO_T));

        if (stLoad.lpalpbyPools[stLoad.wNumPools + POOL_NAMED_CRC_SIG_INFO] ==
            NULL)
            return(FALSE);

        lpstSigSet->lpastNamedCRCSigInfo = (LPWD_NAMED_CRC_SIG_INFO)
            stLoad.lpalpbyPools[stLoad.wNumPools + POOL_NAMED_CRC_SIG_INFO];

        // Get named CRC signature sets

        for (w=0;w<wCount;w++)
        {
            // Get CRC index

            if (LoadWORDValue(&stLoad,
                              &lpstSigSet->lpastNamedCRCSigInfo[w].
                                   wCRCIndex) == FALSE)
                return(FALSE);

            // Get base ID and signature set

            if (LoadWORDandPoolInfo(&stLoad,
                                    &lpstSigSet->lpastNamedCRCSigInfo[w].
                                        wBaseID,
                                    &lpstSigSet->lpastNamedCRCSigInfo[w].
                                        lpabySigs) == FALSE)
                return(FALSE);
        }
    }

    // Get named CRC full set info

    if (LoadWORDandPoolInfo(&stLoad,
                            &wCount,
                            &lpstSigSet->lpabyNamedCRCSigFullSet) == FALSE)
        return(FALSE);

    lpstSigSet->lNamedCRCSigFullSetLen = wCount;


    /////////////////////////////////////////////////////////////
    // Get macro signature data
    /////////////////////////////////////////////////////////////

    // Get total number of macro signatures

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->lNumMacroSigs = wCount;

    ////////////////////////////////////////////////////////
    // Macro set and end offset signatures

    // Macro set offset signatures

    if (LoadWORDandPoolInfo(&stLoad,
                            &wCount,
                            &lpstSigSet->lpabyMacroSetOffsetSigs) == FALSE)
        return(FALSE);

    lpstSigSet->lNumMacroSetOffsetSigs = wCount;

    // Name end offset signatures

    if (LoadWORDandPoolInfo(&stLoad,
                            &wCount,
                            &lpstSigSet->lpabyMacroEndOffsetSigs) == FALSE)
        return(FALSE);

    lpstSigSet->lNumMacroEndOffsetSigs = wCount;

    ////////////////////////////////////////////////////////
    // Macro raw signatures

    // Get number of macro raw signature sets

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    if (wCount == 0)
    {
        lpstSigSet->lpastRawMacroSigIndex = &gstEmptyWD_SIG_INFO_T;
    }
    else
    {
        // Allocate memory for raw macro signature set index

        stLoad.lpalpbyPools[stLoad.wNumPools + POOL_RAW_MACRO_SIG_INDEX] =
            (LPBYTE)lpstCallBack->
                PermMemoryAlloc(wCount * sizeof(WD_SIG_INFO_T));

        if (stLoad.lpalpbyPools[stLoad.wNumPools + POOL_RAW_MACRO_SIG_INDEX] ==
            NULL)
            return(FALSE);

        lpstSigSet->lpastRawMacroSigIndex = (LPWD_SIG_INFO)
            stLoad.lpalpbyPools[stLoad.wNumPools + POOL_RAW_MACRO_SIG_INDEX];

        // Get raw macro signature sets

        for (w=0;w<wCount;w++)
        {
            if (LoadWORDandPoolInfo(&stLoad,
                                    &lpstSigSet->lpastRawMacroSigIndex[w].
                                        wBaseID,
                                    &lpstSigSet->lpastRawMacroSigIndex[w].
                                        lpabySigs) == FALSE)
                return(FALSE);
        }
    }

    // Get macro signature full set info

    if (LoadWORDandPoolInfo(&stLoad,
                            &wCount,
                            &lpstSigSet->lpabyMacroSigFullSet) == FALSE)
        return(FALSE);

    lpstSigSet->lMacroSigFullSetLen = wCount;

    // Get macro signature raw byte hit array

    if (LoadPoolInfo(&stLoad,
                     &lpstSigSet->lpabyMacroSigRawByteHit) == FALSE)
        return(FALSE);


    /////////////////////////////////////////////////////////////
    // Get menu repair data
    /////////////////////////////////////////////////////////////

    // Get the size of the menu repair signature array

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->lMenuRepairSigArraySize = wCount;

    if (LoadPoolInfo(&stLoad,
                     &lpstSigSet->lpabyMenuRepairSigs) == FALSE)
        return(FALSE);


    /////////////////////////////////////////////////////////////
    // Get CRC data
    /////////////////////////////////////////////////////////////

    // Get number of CRCs

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->lNumCRCs = wCount;

    if (wCount == 0)
    {
        lpstSigSet->lpadwCRCs = &gdwZero;
    }
    else
    {
        // Look up the CRC section

        if (VDFLookUp(lpstCallBack,
                      hFile,
                      dwBaseID + ID_OFFSET_CRC_SECTION,
                      &dwDataOffset,
                      &dwDataLength) == FALSE)
            return(FALSE);

        // Limit to 16000 CRCs

        if (wCount > 16000 || wCount * sizeof(DWORD) != dwDataLength)
            return(FALSE);

        // Allocate memory for CRCs

        stLoad.lpalpbyPools[stLoad.wNumPools + POOL_CRC_POOL] =
            (LPBYTE)lpstCallBack->
                PermMemoryAlloc(wCount * sizeof(DWORD));

        if (stLoad.lpalpbyPools[stLoad.wNumPools + POOL_CRC_POOL] == NULL)
            return(FALSE);

        lpstSigSet->lpadwCRCs = (LPDWORD)
            stLoad.lpalpbyPools[stLoad.wNumPools + POOL_CRC_POOL];

        // Read CRCs

        if (lpstCallBack->FileSeek(hFile,
                                   dwDataOffset,
                                   SEEK_SET) != dwDataOffset)
            return(FALSE);

        if (lpstCallBack->FileRead(hFile,
                                   lpstSigSet->lpadwCRCs,
                                   (UINT)dwDataLength) != dwDataLength)
            return(FALSE);

        // Endianize CRCs

        for (w=0;w<wCount;w++)
            lpstSigSet->lpadwCRCs[w] = DWENDIAN(lpstSigSet->lpadwCRCs[w]);
    }


    /////////////////////////////////////////////////////////////
    // Get virus signature data
    /////////////////////////////////////////////////////////////

    // Get number of virus signatures

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->lNumVirusSigs = wCount;

    if (wCount == 0)
    {
        lpstSigSet->lpastVirusSigInfo = &gstEmptyWD_VIRUS_SIG_INFO_T;
    }
    else
    {
        // Allocate memory for virus signatures

        stLoad.lpalpbyPools[stLoad.wNumPools + POOL_VIRUS_SIG_INFO] =
            (LPBYTE)lpstCallBack->
                PermMemoryAlloc(wCount * sizeof(WD_VIRUS_SIG_INFO_T));

        if (stLoad.lpalpbyPools[stLoad.wNumPools + POOL_VIRUS_SIG_INFO] ==
            NULL)
            return(FALSE);

        lpstSigSet->lpastVirusSigInfo = (LPWD_VIRUS_SIG_INFO)
            stLoad.lpalpbyPools[stLoad.wNumPools + POOL_VIRUS_SIG_INFO];

        // Get virus signatures

        for (w=0;w<wCount;w++)
        {
            if (LoadWORDandPoolInfo(&stLoad,
                                    &lpstSigSet->lpastVirusSigInfo[w].
                                        wID,
                                    &lpstSigSet->lpastVirusSigInfo[w].
                                        lpabySig) == FALSE)
                return(FALSE);
        }
    }


    /////////////////////////////////////////////////////////////
    // Set empty custom scan/repair
    /////////////////////////////////////////////////////////////

    lpstSigSet->lpapfCustomScan = gapfEmptyCustomScan;
    lpstSigSet->lpapfCustomRepair = gapfEmptyCustomRepair;

    return(TRUE);
}


#ifdef MACROHEU

//********************************************************************
//
// BOOL LoadExclusionSigSet()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//  hFile               Handle to open data file
//  lpstSet             Ptr to exclusion set structure
//
// Description:
//  Loads the data for an exclusion signature set.
//  lpstSet->dwCount, lpstSet->lpadwCRCs,
//  and lpstSet->lpadwCRCByteCounts will only
//  be set if the function is successful.
//  Otherwise the values are left as is.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL LoadExclusionSigSet
(
    LPCALLBACKREV1          lpstCallBack,
    HFILE                   hFile,
    LPMACRO_EXCLUSION_SET   lpstSet
)
{
    DWORD                   dwDataOffset;
    DWORD                   dwDataLength;
    DWORD                   dwSigCount;
    LPDWORD                 lpdwData;

    // Look up the section

    if (VDFLookUp(lpstCallBack,
                  hFile,
                  lpstSet->dwBaseID,
                  &dwDataOffset,
                  &dwDataLength) == FALSE)
        return(FALSE);

    // Get exclusion sig count

    if (lpstCallBack->FileSeek(hFile,
                               dwDataOffset,
                               SEEK_SET) != dwDataOffset)
        return(FALSE);

    if (lpstCallBack->FileRead(hFile,
                               &dwSigCount,
                               sizeof(DWORD)) != sizeof(DWORD))
        return(FALSE);

    dwSigCount = DWENDIAN(dwSigCount);

    if (dwSigCount == 0)
        return(TRUE);

    // Limit to 4096 exclusion signatures

    if (dwSigCount > 4096)
        return(FALSE);

    // Make sure there is enough data

    if (dwDataLength < sizeof(DWORD) + dwSigCount * 2 * sizeof(DWORD))
        return(FALSE);

    // Allocate a buffer

    lpdwData = (LPDWORD)lpstCallBack->
        PermMemoryAlloc(dwSigCount * 2 * sizeof(DWORD));

    if (lpdwData == NULL)
        return(FALSE);

    // Read the data

    if (lpstCallBack->FileRead(hFile,
                               lpdwData,
                               (UINT)(dwSigCount * 2 * sizeof(DWORD))) !=
        (UINT)(dwSigCount * 2 * sizeof(DWORD)))
    {
        // Error reading, first free the allocated memory

        lpstCallBack->PermMemoryFree(lpdwData);
        return(FALSE);
    }

    // Success

    lpstSet->dwCount = dwSigCount;
    lpstSet->lpadwCRCs = lpdwData;
    lpstSet->lpadwCRCByteCounts = lpdwData + dwSigCount;

    return(TRUE);
}


//********************************************************************
//
// BOOL LoadW7H2SigSet()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//  hFile               Handle to open data file
//  dwBaseID            Base ID of signature set data
//  lpstSigSet          Ptr to signature structure to fill
//
// Description:
//  Loads the data for a W7H2 signature set.
//
//  Assumes that the sig set structure is initialized to zero.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL LoadW7H2SigSet
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwBaseID,
    LPW7H2_SIG_SET      lpstSigSet
)
{
    LOAD_MACRO_SIG_T    stLoad;
    DWORD               dwDataOffset;
    DWORD               dwDataLength;
    LPBYTE              lpabyDat;
    WORD                w;
    WORD                wCount;
    WORD                wOffset;

    stLoad.lpstCallBack = lpstCallBack;
    stLoad.hFile = hFile;
    stLoad.dwNumInfoBufBytes = 0;

    // Look up the info section

    if (VDFLookUp(lpstCallBack,
                  hFile,
                  dwBaseID + ID_OFFSET_INFO_SECTION,
                  &stLoad.dwInfoOffset,
                  &stLoad.dwInfoLength) == FALSE)
        return(FALSE);

    // Look up the raw data section

    if (VDFLookUp(lpstCallBack,
                  hFile,
                  dwBaseID + ID_OFFSET_DATA_SECTION,
                  &dwDataOffset,
                  &dwDataLength) == FALSE)
        return(FALSE);

    // Allocate pool memory

    lpabyDat = (LPBYTE)lpstCallBack->
        PermMemoryAlloc(dwDataLength * sizeof(BYTE));

    if (lpabyDat == NULL)
        return(FALSE);

    // Set the Cmd array ptr to the allocated pool

    lpstSigSet->lpawCmd = (LPWORD)lpabyDat;

    // Load pool data

    if (lpstCallBack->FileSeek(hFile,
                               dwDataOffset,
                               SEEK_SET) != dwDataOffset)
        return(FALSE);

    if (lpstCallBack->FileRead(hFile,
                               lpabyDat,
                               dwDataLength) != dwDataLength)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // Get referenced cmds
    /////////////////////////////////////////////////////////////

    // Get number of referenced cmds

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->nNumCmds = (int)wCount;

    // Get pool offset

    if (LoadWORDValue(&stLoad,&wOffset) == FALSE)
        return(FALSE);

    // The cmds must start at the beginning of the data pool

    if (wOffset != 0)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // Get referenced dlgs
    /////////////////////////////////////////////////////////////

    // Get number of referenced dlgs

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->nNumDlgs = (int)wCount;

    // Get pool offset

    if (LoadWORDValue(&stLoad,&wOffset) == FALSE)
        return(FALSE);

    if (wOffset > dwDataLength)
        return(FALSE);

    // Set the pointer to the referenced dlgs

    lpstSigSet->lpawDlg = (LPWORD)(lpabyDat + wOffset);

    /////////////////////////////////////////////////////////////
    // Get str signatures
    /////////////////////////////////////////////////////////////

    // Get number of str signatures

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->nNumStrSigs = (int)wCount;

    // Allocate memory for the str sig ptrs

    lpstSigSet->lpalpabyStrSigs = (LPLPBYTE)lpstCallBack->
        PermMemoryAlloc(wCount * sizeof(LPBYTE));

    if (lpstSigSet->lpalpabyStrSigs == NULL)
        return(FALSE);

    // Get pool offsets for str signatures

    for (w=0;w<wCount;w++)
    {
        if (LoadWORDValue(&stLoad,&wOffset) == FALSE)
        return(FALSE);

        if (wOffset > dwDataLength)
            return(FALSE);

        // Set the pointer

        lpstSigSet->lpalpabyStrSigs[w] = lpabyDat + wOffset;
    }

    // Get pool offset for str sig idx

    if (LoadWORDValue(&stLoad,&wOffset) == FALSE)
        return(FALSE);

    if (wOffset > dwDataLength)
        return(FALSE);

    // Set the pointer to the str sig idx

    lpstSigSet->lpawStrSigIdx = (LPWORD)(lpabyDat + wOffset);

    // Get pool offset for str sig cat scores

    if (LoadWORDValue(&stLoad,&wOffset) == FALSE)
        return(FALSE);

    if (wOffset > dwDataLength)
        return(FALSE);

    // Set the pointer to the str sig cat scores

    lpstSigSet->lpabyStrSigCatScores = lpabyDat + wOffset;

    /////////////////////////////////////////////////////////////
    // Get line signatures
    /////////////////////////////////////////////////////////////

    // Get number of line signatures

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->nNumLineSigs = (int)wCount;

    // Allocate memory for the line sig ptrs

    lpstSigSet->lpalpabyLineSigs = (LPLPBYTE)lpstCallBack->
        PermMemoryAlloc(wCount * sizeof(LPBYTE));

    if (lpstSigSet->lpalpabyLineSigs == NULL)
        return(FALSE);

    // Get pool offsets for line signatures

    for (w=0;w<wCount;w++)
    {
        if (LoadWORDValue(&stLoad,&wOffset) == FALSE)
            return(FALSE);

        if (wOffset > dwDataLength)
            return(FALSE);

        // Set the pointer

        lpstSigSet->lpalpabyLineSigs[w] = lpabyDat + wOffset;
    }

    // Get pool offset for line sig cat scores

    if (LoadWORDValue(&stLoad,&wOffset) == FALSE)
        return(FALSE);

    if (wOffset > dwDataLength)
        return(FALSE);

    // Set the pointer to the line sig cat scores

    lpstSigSet->lpabyLineSigCatScores = lpabyDat + wOffset;

    /////////////////////////////////////////////////////////////
    // Get func signatures
    /////////////////////////////////////////////////////////////

    // Get number of func signatures

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->nNumFuncSigs = (int)wCount;

    // Allocate memory for the func sig ptrs

    lpstSigSet->lpalpabyFuncSigs = (LPLPBYTE)lpstCallBack->
        PermMemoryAlloc(wCount * sizeof(LPBYTE));

    if (lpstSigSet->lpalpabyFuncSigs == NULL)
        return(FALSE);

    // Get pool offsets for func signatures

    for (w=0;w<wCount;w++)
    {
        if (LoadWORDValue(&stLoad,&wOffset) == FALSE)
            return(FALSE);

        if (wOffset > dwDataLength)
            return(FALSE);

        // Set the pointer

        lpstSigSet->lpalpabyFuncSigs[w] = lpabyDat + wOffset;
    }

    // Get pool offset for func sig cat scores

    if (LoadWORDValue(&stLoad,&wOffset) == FALSE)
        return(FALSE);

    if (wOffset > dwDataLength)
        return(FALSE);

    // Set the pointer to the func sig cat scores

    lpstSigSet->lpabyFuncSigCatScores = lpabyDat + wOffset;

    /////////////////////////////////////////////////////////////
    // Get set signatures
    /////////////////////////////////////////////////////////////

    // Get number of set signatures

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->nNumSetSigs = (int)wCount;

    // Allocate memory for the set sig ptrs

    lpstSigSet->lpalpabySetSigs = (LPLPBYTE)lpstCallBack->
        PermMemoryAlloc(wCount * sizeof(LPBYTE));

    if (lpstSigSet->lpalpabySetSigs == NULL)
        return(FALSE);

    // Get pool offsets for set signatures

    for (w=0;w<wCount;w++)
    {
        if (LoadWORDValue(&stLoad,&wOffset) == FALSE)
            return(FALSE);

        if (wOffset > dwDataLength)
            return(FALSE);

        // Set the pointer

        lpstSigSet->lpalpabySetSigs[w] = lpabyDat + wOffset;
    }

    // Get pool offset for set sig VIDs

    if (LoadWORDValue(&stLoad,&wOffset) == FALSE)
        return(FALSE);

    if (wOffset > dwDataLength)
        return(FALSE);

    // Set the pointer to the set sig VIDs

    lpstSigSet->lpawVID = (LPWORD)(lpabyDat + wOffset);

    // Get the number of categories

    if (LoadWORDValue(&stLoad,&wCount) == FALSE)
        return(FALSE);

    lpstSigSet->nNumCats = (int)wCount;

    return(TRUE);
}

#endif // #ifdef MACROHEU


#ifdef SYM_DOSX

//********************************************************************
//
// BOOL SetupEmptyMacroSigSet()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//  lpstSigSet          Ptr to signature structure to fill
//  lpdwPoolCount       Ptr to DWORD for number of allocated pools
//  lplpalpbyPools      Ptr to ptr for array of pool ptrs
//
// Description:
//  Sets up the data for an empty signature set.
//
//  Buffers required:
//   alpabyPools[0]
//  Total Number of Pools excluding pool array buffer: 1
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL SetupEmptyMacroSigSet
(
    LPCALLBACKREV1      lpstCallBack,
    LPWD_SIG_SET        lpstSigSet,
    LPDWORD             lpdwPoolCount,
    LPBYTE FAR * FAR *  lplpalpbyPools
)
{
    WORD                w;
    LPBYTE FAR *        lpalpbyPools;

    /////////////////////////////////////////////////////////////
    // Allocate a pool of 256 zeroes
    /////////////////////////////////////////////////////////////

    lpalpbyPools = (LPBYTE FAR *)lpstCallBack->
        PermMemoryAlloc(sizeof(LPBYTE));

    if (lpalpbyPools == NULL)
        return(FALSE);

    *lpdwPoolCount = 1;
    *lplpalpbyPools = lpalpbyPools;

    // Set pointers to NULL

    lpalpbyPools[0] = NULL;

    // Allocate pool memory

    lpalpbyPools[0] = (LPBYTE)lpstCallBack->
        PermMemoryAlloc(256 * sizeof(BYTE));

    if (lpalpbyPools[0] == NULL)
        return(FALSE);

    // Set pool to zeroes

    for (w=0;w<256;w++)
        lpalpbyPools[0][w] = 0;


    /////////////////////////////////////////////////////////////
    // Get name signature data
    /////////////////////////////////////////////////////////////

    lpstSigSet->lNumNameSigs = 0;

    ////////////////////////////////////////////////////////
    // Name set and end offset signatures

    // Name set offset signatures

    lpstSigSet->lpabyNameSetOffsetSigs = lpalpbyPools[0];
    lpstSigSet->lNumNameSetOffsetSigs = 0;

    // Name end offset signatures

    lpstSigSet->lpabyNameEndOffsetSigs = lpalpbyPools[0];
    lpstSigSet->lNumNameEndOffsetSigs = 0;

    ////////////////////////////////////////////////////////
    // Name raw signatures

    // Get number of name raw signature sets

    lpstSigSet->lpastRawNameSigIndex = &gstEmptyWD_SIG_INFO_T;

    // Get name signature raw byte hit array

    lpstSigSet->lpabyNameSigRawByteHit = lpalpbyPools[0];


    /////////////////////////////////////////////////////////////
    // Named CRC signatures
    /////////////////////////////////////////////////////////////

    // Get number of named CRC signatures

    lpstSigSet->lNumNamedCRCSigs = 0;

    // Get number of name raw signature sets

    lpstSigSet->lNumNamedCRCSigSets = 0;

    lpstSigSet->lpastNamedCRCSigInfo = &gstEmptyWD_NAMED_CRC_SIG_INFO_T;

    // Get named CRC full set info

    lpstSigSet->lpabyNamedCRCSigFullSet = lpalpbyPools[0];
    lpstSigSet->lNamedCRCSigFullSetLen = 0;


    /////////////////////////////////////////////////////////////
    // Get macro signature data
    /////////////////////////////////////////////////////////////

    // Get total number of macro signatures

    lpstSigSet->lNumMacroSigs = 0;

    ////////////////////////////////////////////////////////
    // Macro set and end offset signatures

    // Macro set offset signatures

    lpstSigSet->lpabyMacroSetOffsetSigs = lpalpbyPools[0];
    lpstSigSet->lNumMacroSetOffsetSigs = 0;

    // Name end offset signatures

    lpstSigSet->lpabyMacroEndOffsetSigs = lpalpbyPools[0];
    lpstSigSet->lNumMacroEndOffsetSigs = 0;

    ////////////////////////////////////////////////////////
    // Macro raw signatures

    // Get number of macro raw signature sets

    lpstSigSet->lpastRawMacroSigIndex = &gstEmptyWD_SIG_INFO_T;

    // Get macro signature full set info

    lpstSigSet->lpabyMacroSigFullSet = lpalpbyPools[0];
    lpstSigSet->lMacroSigFullSetLen = 0;

    // Get macro signature raw byte hit array

    lpstSigSet->lpabyMacroSigRawByteHit = lpalpbyPools[0];


    /////////////////////////////////////////////////////////////
    // Get CRC data
    /////////////////////////////////////////////////////////////

    // Get number of CRCs

    lpstSigSet->lNumCRCs = 0;
    lpstSigSet->lpadwCRCs = &gdwZero;


    /////////////////////////////////////////////////////////////
    // Get virus signature data
    /////////////////////////////////////////////////////////////

    // Get number of virus signatures

    lpstSigSet->lNumVirusSigs = 0;
    lpstSigSet->lpastVirusSigInfo = &gstEmptyWD_VIRUS_SIG_INFO_T;


    /////////////////////////////////////////////////////////////
    // Set empty custom scan/repair
    /////////////////////////////////////////////////////////////

    lpstSigSet->lpapfCustomScan = gapfEmptyCustomScan;
    lpstSigSet->lpapfCustomRepair = gapfEmptyCustomRepair;

    return(TRUE);
}

#endif // #ifdef SYM_DOSX


//********************************************************************
//
// BOOL LoadMacroSigs()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//
// Description:
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL LoadMacroSigs
(
    LPCALLBACKREV1          lpstCallBack,
#if !defined( __MACINTOSH__ )
    LPTSTR                  lpszDataDir
#else
    FSSpecPtr               lpVirscan6Spec
#endif
)
{
    BOOL                    bResult = TRUE;
    BOOL                    bOptional;
    HFILE                   hFile;
    int                     i;
    LPMACRO_SIG_SET_INFO    lpstInfo;

#if !defined( __MACINTOSH__ )
#if defined(SYM_VXD)
    char        szDatFileName[2 * SYM_MAX_PATH * sizeof(WCHAR)];
#elif defined(SYM_NTK)
    TCHAR       szDatFileName[2 * SYM_MAX_PATH];
#elif defined(SYM_WIN)
    TCHAR       szDatFileName[2 * SYM_MAX_PATH];
#else
    char        szDatFileName[SYM_MAX_PATH];
#endif
#endif // __MACINTOSH__

    // VIRSCAN6.DAT is by default not optional

    bOptional = FALSE;

//////////////////////////////////////////////////////////////////////
#if defined(SYM_DOSX)

    /////////////////////////////////////////////////////////////
    // Check for VIRSCAN6.INI

#if !defined( __MACINTOSH__ )
    DataDirAppendName(lpszDataDir,_T("VIRSCAN6.INI"),szDatFileName);

    hFile = lpstCallBack->FileOpen(szDatFileName,0);
#else
    hFile = lpstCallBack->FileOpen(lpVirscan6Spec,0);
#endif

    if (hFile != (HFILE)-1)
    {
        BYTE abyOPTIONAL[8];

        // Check to see if the first eight characters are "OPTIONAL"

        if (lpstCallBack->FileRead(hFile,abyOPTIONAL,8) == 8)
        {
            if (abyOPTIONAL[0] == 'O' &&
                abyOPTIONAL[1] == 'P' &&
                abyOPTIONAL[2] == 'T' &&
                abyOPTIONAL[3] == 'I' &&
                abyOPTIONAL[4] == 'O' &&
                abyOPTIONAL[5] == 'N' &&
                abyOPTIONAL[6] == 'A' &&
                abyOPTIONAL[7] == 'L')
                bOptional = TRUE;
        }

        lpstCallBack->FileClose(hFile);
    }

#endif // #if defined(SYM_DOSX)
//////////////////////////////////////////////////////////////////////


    /////////////////////////////////////////////////////////////
    // Open data file

#if !defined( __MACINTOSH__ )
#if defined(SYM_UNIX)
    DataDirAppendName(lpszDataDir,_T("virscan6.dat"),szDatFileName);
#else
    DataDirAppendName(lpszDataDir,_T("VIRSCAN6.DAT"),szDatFileName);
#endif

    hFile = lpstCallBack->FileOpen(szDatFileName,0);
#else
    hFile = lpstCallBack->FileOpen(lpVirscan6Spec,0);
#endif

    if (hFile == (HFILE)-1)
    {
        if (bOptional == FALSE)
            return(FALSE);

#if defined(SYM_DOSX)

        // Create empty signature set

        for (i=0;i<NUM_MACRO_SIG_SETS;i++)
        {
            lpstInfo = gastMacroSigSetInfo + i;

            if (SetupEmptyMacroSigSet(lpstCallBack,
                                      lpstInfo->lpstSigSet,
                                      &lpstInfo->dwNumPools,
                                      &lpstInfo->lplpabyPools) == FALSE)
            {
                bResult = FALSE;
                break;
            }
        }

#endif // #if defined(SYM_DOSX)
    }
    else
    {
        for (i=0;i<NUM_MACRO_SIG_SETS;i++)
        {
            lpstInfo = gastMacroSigSetInfo + i;

            if (LoadMacroSigSet(lpstCallBack,
                                hFile,
                                lpstInfo->dwBaseID,
                                lpstInfo->lpstSigSet,
                                &lpstInfo->dwNumPools,
                                &lpstInfo->lplpabyPools) == FALSE)
            {
                bResult = FALSE;
                break;
            }
        }

#ifdef MACROHEU
        // Ignore errors loading exclusion signature sets

        for (i=0;i<NUM_MACRO_EXCLUSION_SETS;i++)
        {
            LoadExclusionSigSet(lpstCallBack,
                                hFile,
                                gastMacroExclusionSets + i);
        }

        // Ignore errors loading W7H2 signature set

        LoadW7H2SigSet(lpstCallBack,
                       hFile,
                       W7H2_BASE_ID,
                       &gstW7H2SigSet);

#endif // #ifdef MACROHEU

        // Close data file

#if 0  // This return value check seems to be incorrect with respect to NAVAPI
        if (lpstCallBack->FileClose(hFile) != 0)
            bResult = FALSE;
#endif
        lpstCallBack->FileClose(hFile);  // NEW_UNIX

    }

    if (bResult == FALSE)
        UnloadMacroSigs(lpstCallBack);

    return(bResult);
}


//********************************************************************
//
// BOOL UnloadMacroSigs()
//
// Parameters:
//  lpstCallBack        Ptr to callback structure
//
// Description:
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL UnloadMacroSigs
(
    LPCALLBACKREV1          lpstCallBack
)
{
    DWORD                   dw;
    int                     i;
    LPMACRO_SIG_SET_INFO    lpstInfo;

    for (i=0;i<NUM_MACRO_SIG_SETS;i++)
    {
        lpstInfo = gastMacroSigSetInfo + i;
        if (lpstInfo->dwNumPools != 0)
        {
            for (dw=0;dw<lpstInfo->dwNumPools;dw++)
            {
                if (lpstInfo->lplpabyPools[dw] != NULL)
                    lpstCallBack->
                        PermMemoryFree(lpstInfo->lplpabyPools[dw]);
            }

            lpstCallBack->PermMemoryFree(lpstInfo->lplpabyPools);

            lpstInfo->dwNumPools = 0;
        }
    }

#ifdef MACROHEU
    for (i=0;i<NUM_MACRO_EXCLUSION_SETS;i++)
    {
        if (gastMacroExclusionSets[i].dwCount != 0)
        {
            lpstCallBack->PermMemoryFree(gastMacroExclusionSets[i].lpadwCRCs);
            gastMacroExclusionSets[i].dwCount = 0;
        }
    }

    if (gstW7H2SigSet.lpawCmd != NULL)
        lpstCallBack->PermMemoryFree(gstW7H2SigSet.lpawCmd);

    if (gstW7H2SigSet.lpalpabyStrSigs != NULL)
        lpstCallBack->PermMemoryFree(gstW7H2SigSet.lpalpabyStrSigs);

    if (gstW7H2SigSet.lpalpabyLineSigs != NULL)
        lpstCallBack->PermMemoryFree(gstW7H2SigSet.lpalpabyLineSigs);

    if (gstW7H2SigSet.lpalpabyFuncSigs != NULL)
        lpstCallBack->PermMemoryFree(gstW7H2SigSet.lpalpabyFuncSigs);

    if (gstW7H2SigSet.lpalpabySetSigs != NULL)
        lpstCallBack->PermMemoryFree(gstW7H2SigSet.lpalpabySetSigs);

#endif // #ifdef MACROHEU

    return(TRUE);
}

#endif // #ifdef MACRODAT

