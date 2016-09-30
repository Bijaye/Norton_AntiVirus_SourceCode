// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/EMULATOR/VCS/sigex.cpv   1.7   28 Feb 1996 10:27:14   CNACHEN  $
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
// $Log:   S:/EMULATOR/VCS/sigex.cpv  $
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

#include "avendian.h"
#include "pamapi.h"
#include "ident.h"

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
                      ID_SIGNATURE_EXCLUDE,
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

    DFCloseDataFile(hData);

    *hExcludePtr = pTemp;

    return(PAMSTATUS_OK);
}


PAMSTATUS PExcludeTest(ExcludeContext hExclude,
                     LPBYTE lpEntryBuffer)
{
	WORD    i,j, wLen;
	LPWORD	lpwPtr;

	for (i=0;i<hExclude->wNumSigs;i++)
	{
		lpwPtr = hExclude->lpSigPtr[i].wSig;
		wLen = hExclude->lpSigPtr[i].wSigLen;
		for (j=0;j<wLen;j++,lpwPtr++)
			if (*lpwPtr != lpEntryBuffer[j] && WILD_WORD != *lpwPtr)
				break;

		if (j == wLen)
            return(PAMSTATUS_NO_SCAN);
    }

    return(PAMSTATUS_NEED_TO_SCAN);
}


PAMSTATUS PExcludeClose(ExcludeContext hExclude)
{
    PAMMemoryFree(hExclude->lpSigPtr);
    PAMMemoryFree(hExclude);

    return(PAMSTATUS_OK);
}

