// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/CRCSCAN.CPv   1.0   18 Apr 1997 13:37:26   MKEATIN  $
//
// Description:
//  Functions supporting scanning with CRC definitions.
//
// Contains:
//  CRCApplySigs()
//
// See Also:
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/CRCSCAN.CPv  $
// 
//    Rev 1.0   18 Apr 1997 13:37:26   MKEATIN
// Initial revision.
// 
//    Rev 1.1   16 May 1996 14:12:42   CNACHEN
// Changed to use new AVENGE?.H headers.
// 
//    Rev 1.0   13 May 1996 16:28:54   DCHI
// Initial revision.
// 
//************************************************************************

#include "avengel.h"

//*************************************************************************
//
// Function:
//  CRCCompute()
//
// Parameters:
//  wSeed           CRC seed value
//  lpbyBuf         Pointer to buffer
//  wCount          Number of bytes in buffer
//
// Description:
//  Calculates a 16-bit CRC on the wCount bytes at lpbyBuf.
//
// Returns:
//  WORD            16-bit CRC
//
//*************************************************************************

WORD CRCCompute
(
    WORD    wSeed,
    LPBYTE  lpbyBuf,
    WORD    wCount
)
{
    while (wCount-- > 0)
    {
        wSeed = ((wSeed << 8) + (wSeed >> 8)) ^
            ((short)(*(signed char *)lpbyBuf++));
        wSeed ^= (wSeed & 0x00ff) >> 4;
        wSeed ^= (wSeed << 12) ^ ((wSeed & 0x00ff) << 5);
    }

    return (wSeed);
}


//*************************************************************************
//
// Function:
//  CRCApplySigs()
//
// Parameters:
//  lpFStartData    Pointer to FStart Data
//  lpFStartInfo    Pointer to FStart Info
//  lpCRCScanInfo   Pointer to CRC data to scan with
//  lpbFoundSig     Pointer to BOOL to store virus found status
//  lphVirus        Pointer to HVIRUS to store virus handle if virus found
//
// Description:
//  Determines whether the FSTART buffer matches any CRC sigs.
//  Sets *lpbFoundSig to TRUE if a match is found.  Otherwise,
//  *lpbFoundSig is set to FALSE.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS CRCApplySigs
(
    LPFSTARTDATA        lpFStartData,
    LPFSTARTINFO        lpFStartInfo,
    LPCRC_SCAN_INFO     lpCRCScanInfo,
    LPBOOL              lpbFoundSig,
    LPHVIRUS            lphVirus
)
{
    int                 i;
    long                lLow, lHigh, lMid, lIndex;
    WORD                wCRC;
    LPCRC_SIG_DATA      lpCRCSigData;

    // reset our found status first...

    *lpbFoundSig = FALSE;

    // Make sure there are enough bytes to compute a CRC

    if (lpFStartInfo->wFStartBufferLen < CRC_ON_N_BYTES)
        return(ENGSTATUS_OK);

    // Compute CRC

    wCRC = CRCCompute(0,lpFStartData->byFStartBuffer,CRC_ON_N_BYTES);

    // CRC of zero is invalid

    if (wCRC == 0)
        return(ENGSTATUS_OK);

    ///////////////////////////////////////////////////////////////////
    //
    // Binary search to find a matching CRC sig
    //
    ///////////////////////////////////////////////////////////////////

    lLow = 0;
    lHigh = lpCRCScanInfo->wNumSigs - 1;
    while (lLow <= lHigh)
    {
        lMid = (lLow + lHigh) / 2;

		lpCRCSigData = lpCRCScanInfo->CRCPoolArray[(WORD)(lMid >> CRC_POOL_SHIFT)].
			lpCRCSigData + (WORD)(lMid & CRC_POOL_MASK);

		// Compare with first sig in this pool

		if (wCRC < lpCRCSigData->wCRC)
		{
			// In lower half

			lHigh = lMid - 1;
		}
		else
		if (wCRC > lpCRCSigData->wCRC)
		{
			// In upper half

			lLow = lMid + 1;
		}
		else
		{
			// Found a match

			break;
		}
	}

	// Is it within range?

	if (lLow > lHigh)
	{
		// No match

		return(ENGSTATUS_OK);
	}

    ///////////////////////////////////////////////////////////////////
    //
    // Compare verification bytes for all matching CRC sigs
    //
    ///////////////////////////////////////////////////////////////////

    // Search backward for first sig with given CRC

    lIndex = lMid;
    while (--lIndex >= 0)
    {
		lpCRCSigData = lpCRCScanInfo->CRCPoolArray[(WORD)(lIndex >> CRC_POOL_SHIFT)].
			lpCRCSigData + (WORD)(lIndex & CRC_POOL_MASK);

        // Also a sig with the given CRC?

        if (wCRC != lpCRCSigData->wCRC)
            break;
    }

    // Search forward checking verification bytes for matching CRC sigs

    while (++lIndex < (int)lpCRCScanInfo->wNumSigs)
    {
		lpCRCSigData = lpCRCScanInfo->CRCPoolArray[(WORD)(lIndex >> CRC_POOL_SHIFT)].
			lpCRCSigData + (WORD)(lIndex & CRC_POOL_MASK);

        // If deleted CRC sig, then skip it

        if (lpCRCSigData->wVirusRecordIndex == INVALID_VIRUS_HANDLE)
            continue;

        // Only check verification bytes with same CRC

        if (wCRC != lpCRCSigData->wCRC)
            break;

        // Compare verification bytes

        for (i=0;i<lpCRCSigData->byVerifyLen;i++)
            if (lpFStartData->byFStartBuffer[i] !=
                (BYTE)(lpCRCSigData->byVerifyData[i] - 1))
                break;

        if (i == lpCRCSigData->byVerifyLen)
        {
            // Found a match

            *lpbFoundSig = TRUE;
            *lphVirus = (HVIRUS)lpCRCSigData->wVirusRecordIndex;

            return(ENGSTATUS_OK);
        }
    }

    // No match

    return(ENGSTATUS_OK);
}


