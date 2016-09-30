// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/HEUR/VCS/sigex.cpv   1.5   02 Dec 1998 19:40:48   MKEATIN  $
//
// Description:
//
//  Contains code for the PAM signature exclusion system.   A signature
//  database of common (uninfected) programs is maintained to allow PAM
//  to skip over common, uninfected files.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/HEUR/VCS/sigex.cpv  $
// 
//    Rev 1.5   02 Dec 1998 19:40:48   MKEATIN
// Check in Melind's fix on a error check.
//
//    Rev 1.4   03 Nov 1997 17:11:46   CNACHEN
// No change.
//
//    Rev 1.3   01 Jul 1997 13:49:28   CNACHEN
// Added heavy hitter support.
//
//    Rev 1.2   23 May 1997 13:10:12   CNACHEN
// Removed printf
//
//    Rev 1.1   23 May 1997 13:01:34   CNACHEN
// Added support for 32-bit crc exclusions
//
//    Rev 1.0   14 May 1997 17:22:34   CNACHEN
// Initial revision.
//
//    Rev 1.8   21 Mar 1997 14:04:20   CNACHEN
// Updated to support BIG endian and picky non-Intel machines...
//
//    Rev 1.7   28 Feb 1996 10:27:14   CNACHEN
// Added an appropriate DFCloseDataFile to PEExcludeInit.
//
//    Rev 1.6   20 Feb 1996 11:27:06   CNACHEN
// Changed all LPSTRs to LPTSTRs.
//
//
//    Rev 1.5   13 Dec 1995 11:58:12   CNACHEN
// All File and Memory functions now use #defined versions with PAM prefixes
//
//    Rev 1.4   19 Oct 1995 18:23:46   CNACHEN
// Initial revision... with comment header :)
//
//************************************************************************

#include "heurapi.h"
#include "crc32.h"

/* code */

PAMSTATUS PExcludeInit(ExcludeContext *hExcludePtr, LPTSTR lpFileName)
{
    HFILE           hStream;
    DATAFILEHANDLE  hData;
	ExcludeContext	pTemp;
	WORD            wCurSig;
    DFSTATUS        status;
    DWORD           dwStartOffset;
    DWORD           dwFinalOffset;
    UINT            uResult;


    /* make sure we don't return something we shouldn't */

    *hExcludePtr = NULL;

    pTemp = (ExcludeContext)PAMMemoryAlloc(sizeof(SigList));
    if (NULL == pTemp)
        return(PAMSTATUS_MEM_ERROR);

    /* now we've got our memory and can go running with it */

    status = DFOpenDataFile(lpFileName,READ_ONLY_FILE,&hData);

    if (DFSTATUS_OK != status)
    {
        /* first free our memory */

        PAMMemoryFree(pTemp);

        if (DFSTATUS_FILE_ERROR == status)
            return(PAMSTATUS_FILE_ERROR);

        return(PAMSTATUS_MEM_ERROR);
    }

    /* now find out where our data is in the data file! */

    status = DFLookUp(hData,
                      ID_BLOOD_SIGNATURE_EXCLUDE,
                      NULL,
                      &dwStartOffset,
                      NULL,
                      NULL,NULL,NULL);

    if (DFSTATUS_OK != status)
    {
        PAMMemoryFree(pTemp);
        DFCloseDataFile(hData);

        return(PAMSTATUS_FILE_ERROR);
    }

    hStream = DFGetHandle(hData);

    dwFinalOffset = PAMFileSeek(hStream,dwStartOffset,SEEK_SET);
    if (dwFinalOffset != dwStartOffset)
    {
        PAMMemoryFree(pTemp);
        DFCloseDataFile(hData);

        return(PAMSTATUS_FILE_ERROR);
    }

    /* now that we're there... read in the good stuff */

    uResult = PAMFileRead(hStream,&pTemp->wNumSigs,sizeof(WORD));
    if (sizeof(WORD) != uResult)
    {
        /* hmm. unable to read in our info.  free memory & close file */

        PAMMemoryFree(pTemp);
        DFCloseDataFile(hData);

        return(PAMSTATUS_FILE_ERROR);
    }

    pTemp->wNumSigs = WENDIAN(pTemp->wNumSigs);

    /* if pTemp->wNumSigs * sizeof(SigType) > 64K-1 then this will fail on
       DOS platforms which aren't running extenders */

    pTemp->lpSigPtr = (SigType *)PAMMemoryAlloc(pTemp->wNumSigs * sizeof(SigType));

    if (NULL == pTemp->lpSigPtr)
    {
        PAMMemoryFree(pTemp);
        DFCloseDataFile(hData);
        return(PAMSTATUS_MEM_ERROR);
    }

    for (wCurSig = 0;wCurSig < pTemp->wNumSigs;wCurSig++)
    {
        uResult = PAMFileRead(hStream,
							 &pTemp->lpSigPtr[wCurSig],
                             sSIGTYPE);

        if (uResult != sSIGTYPE)
        {
            PAMMemoryFree(pTemp->lpSigPtr);
            PAMMemoryFree(pTemp);
            DFCloseDataFile(hData);
            return(PAMSTATUS_MEM_ERROR);
        }

#ifdef BIG_ENDIAN
        {
            WORD            wTemp;

            pTemp->lpSigPtr[wCurSig].wSigLen =
                WENDIAN(pTemp->lpSigPtr[wCurSig].wSigLen);

            for (wTemp=0;wTemp<SIG_EXCLUDE_LEN;wTemp++)
                pTemp->lpSigPtr[wCurSig].wSig[wTemp] =
                    WENDIAN(pTemp->lpSigPtr[wCurSig].wSig[wTemp]);
        }
#endif

    }

    // now load up our CRCs

    /* now find out where our data is in the data file! */

    status = DFLookUp(hData,
                      ID_BLOOD_CRC32,
                      NULL,
                      &dwStartOffset,
                      NULL,
                      NULL,NULL,NULL);

    if (DFSTATUS_OK != status)
    {
        PAMMemoryFree(pTemp->lpSigPtr);
        PAMMemoryFree(pTemp);
        DFCloseDataFile(hData);

        return(PAMSTATUS_FILE_ERROR);
    }

    dwFinalOffset = PAMFileSeek(hStream,dwStartOffset,SEEK_SET);
    if (dwFinalOffset != dwStartOffset)
    {
        PAMMemoryFree(pTemp->lpSigPtr);
        PAMMemoryFree(pTemp);
        DFCloseDataFile(hData);

        return(PAMSTATUS_FILE_ERROR);
    }

    /* now that we're there... read in the good stuff */

    uResult = PAMFileRead(hStream,&pTemp->wNumCRCs,sizeof(WORD));
    if (sizeof(WORD) != uResult)
    {
        /* hmm. unable to read in our info.  free memory & close file */

        PAMMemoryFree(pTemp->lpSigPtr);
        PAMMemoryFree(pTemp);
        DFCloseDataFile(hData);

        return(PAMSTATUS_FILE_ERROR);
    }

    pTemp->wNumCRCs = WENDIAN(pTemp->wNumCRCs);

    if (pTemp->wNumCRCs == 0)
    {
        pTemp->lpdwCRCs = NULL;
        DFCloseDataFile(hData);

	    *hExcludePtr = pTemp;

        return(PAMSTATUS_OK);
    }

    pTemp->lpdwCRCs = (LPDWORD)PAMMemoryAlloc(pTemp->wNumCRCs *
                                              sizeof(DWORD));

    if (NULL == pTemp->lpdwCRCs)
    {
        PAMMemoryFree(pTemp->lpSigPtr);
        PAMMemoryFree(pTemp);
        DFCloseDataFile(hData);
        return(PAMSTATUS_MEM_ERROR);
    }

    uResult = PAMFileRead(hStream,
                          pTemp->lpdwCRCs,
                          (DWORD)pTemp->wNumCRCs * sizeof(DWORD));

    if (uResult != (DWORD)pTemp->wNumCRCs * sizeof(DWORD))
    {
        PAMMemoryFree(pTemp->lpdwCRCs);
        PAMMemoryFree(pTemp->lpSigPtr);
        PAMMemoryFree(pTemp);
        DFCloseDataFile(hData);
        return(PAMSTATUS_MEM_ERROR);
    }

#ifdef BIG_ENDIAN
    {
        WORD            wTemp;

        for (wTemp=0;wTemp<pTemp->wNumCRCs;wTemp++)
            pTemp->lpdwCRCs[wTemp] = DWENDIAN(pTemp->lpdwCRCs[wTemp]);
    }
#endif

    DFCloseDataFile(hData);

    *hExcludePtr = pTemp;

    return(PAMSTATUS_OK);
}

#ifdef BUILD_HEAVY_HITTER

DWORD       gdwLastCRC32;

#endif


PAMSTATUS PExcludeTest
(
    ExcludeContext hExclude,
    LPBYTE lpEntryBuffer
)
{
    long    i, j, lMin, lMax, lLen;
	LPWORD	lpwPtr;
    long    lResult;
    DWORD   dwCRC;

    // first check for wildcard sigs

	for (i=0;i<hExclude->wNumSigs;i++)
	{
		lpwPtr = hExclude->lpSigPtr[i].wSig;
        lLen = hExclude->lpSigPtr[i].wSigLen;
        for (j=0;j<lLen;j++,lpwPtr++)
			if (*lpwPtr != lpEntryBuffer[j] && WILD_WORD != *lpwPtr)
				break;

        if (j == lLen)
            return(PAMSTATUS_NO_SCAN);
    }

#ifdef BUILD_HEAVY_HITTER

    gdwLastCRC32 = ComputeCRC32(lpEntryBuffer,SIG_EXCLUDE_LEN);

#endif

    // then do binary search on CRCs

    if (hExclude->wNumCRCs == 0)
        return(PAMSTATUS_NEED_TO_SCAN);

    lMin = 0;
    lMax = hExclude->wNumCRCs - 1;

    dwCRC = ComputeCRC32(lpEntryBuffer,SIG_EXCLUDE_LEN);

    i = (lMin + lMax) >> 1;             /* divide by 2 */

    while (lMin <= lMax)
    {
        lResult = dwCRC - hExclude->lpdwCRCs[i];

        if (!lResult)
        {
            return(PAMSTATUS_NO_SCAN);
        }

        if (lResult < 0)
            lMax = i - 1;
        else
            lMin = i + 1;

        i = (lMin + lMax) >> 1;
    }

    return(PAMSTATUS_NEED_TO_SCAN);
}


PAMSTATUS PExcludeClose(ExcludeContext hExclude)
{
    PAMMemoryFree(hExclude->lpSigPtr);
    if (hExclude->lpdwCRCs != NULL)
        PAMMemoryFree(hExclude->lpdwCRCs);
    PAMMemoryFree(hExclude);

    return(PAMSTATUS_OK);
}

