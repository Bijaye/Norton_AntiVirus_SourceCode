//************************************************************************
//
// $Header:   S:/NAVEX/VCS/NEPESCAN.CPv   1.7   26 Dec 1996 15:22:42   AOONWAL  $
//
// Description:
//      Source file for Windows scanning engine.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/NEPESCAN.CPv  $
// 
//    Rev 1.7   26 Dec 1996 15:22:42   AOONWAL
// No change.
// 
//    Rev 1.6   02 Dec 1996 14:00:28   AOONWAL
// No change.
// 
//    Rev 1.5   29 Oct 1996 12:59:26   AOONWAL
// No change.
// 
//    Rev 1.4   28 Aug 1996 16:39:54   DCHI
// Endianized everything.
// 
//    Rev 1.3   12 Jun 1996 19:41:32   JWILBER
// Added function ExtScanFileNELastSeg, to scan the last segment of
// an NE file.
//
//    Rev 1.2   13 Feb 1996 11:14:32   JSULTON
// Uncommented some of Windows NE file scanning functions.
//
//    Rev 1.1   08 Feb 1996 10:30:56   DCHI
// Moved DetermineWindowsType() and GetPESectionInfo() to NEPESHR.CPP.
//
//    Rev 1.0   05 Feb 1996 17:23:34   DCHI
// Initial revision.
//
//************************************************************************

#include "endutils.h"

#include "nepescan.h"
#include "winsig.h"

#include "winconst.h"

#include "nepeshr.h"

EXTSTATUS EXTScanFileNE
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    DWORD           dwNEStart,              // start offset of NE header
    LPWORD         	lpwVID					// Virus ID storage on hit
);

EXTSTATUS EXTScanFilePE
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    DWORD           dwPEStart,              // start offset of PE header
    LPWORD         	lpwVID					// Virus ID storage on hit
);

//********************************************************************
//
// Function:
//	EXTSTATUS EXTScanFileWindows()
//
// Description:
//	Scans a file for the presence of Windows viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************

EXTSTATUS EXTScanFileWindows
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    LPWORD         	lpwVID					// Virus ID storage on hit
)
{
    WORD    wReturn = EXTSTATUS_OK;
	WORD 	wFileType;
    DWORD   dwWinHeaderOffset;

	// Determine type of file {NE,PE}

    wFileType = DetermineWindowsType(lpCallBack,
                                     hFile,
                                     lpbyWorkBuffer,
                                     &dwWinHeaderOffset);

	// Call appropriate scanner

	switch (wFileType)
	{
        case WINDOWS_NE_FORMAT:
			wReturn = EXTScanFileNE(lpCallBack,hFile,lpbyWorkBuffer,
				dwWinHeaderOffset,lpwVID);
			break;
        case WINDOWS_PE_FORMAT:
			wReturn = EXTScanFilePE(lpCallBack,hFile,lpbyWorkBuffer,
				dwWinHeaderOffset,lpwVID);
			break;
		default:
			break;
	}

	return (wReturn);
}


EXTSTATUS GetNESectionInfo
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPWININFO       lpstNEInfo,             // NE header
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    DWORD           dwWinHeaderOffset,      // where's the WIN header start?
    WORD            wSecNum,                // what section are we
											// 	interested in (0-base)?
    LPDWORD         lpdwFileOffset,         // where is this section from TOF?
    LPDWORD         lpdwSecLen,             // how long is this section?
    LPWORD          lpwFlags                // segment flags
)
{
    DWORD           dwOffset;
    TBSEGMENT FAR * lpstSegInfo;

    dwOffset = dwWinHeaderOffset +
               WENDIAN(lpstNEInfo->segTabOffset) +
			   wSecNum * sizeof(TBSEGMENT);

    if (dwOffset & 0x80000000UL)
        return EXTSTATUS_FILE_ERROR;

    if (lpCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
        return EXTSTATUS_FILE_ERROR;

    if (lpCallBack->FileRead(hFile,lpbyWorkBuffer,sizeof(TBSEGMENT)) !=
                            sizeof(TBSEGMENT))
        return EXTSTATUS_FILE_ERROR;

	lpstSegInfo = (TBSEGMENT FAR *)lpbyWorkBuffer;

    *lpdwFileOffset = (DWORD)WENDIAN(lpstSegInfo->segDataOffset) <<
        WENDIAN(lpstNEInfo->shiftCount);
    *lpdwSecLen = ((WENDIAN(lpstSegInfo->segLen) == 0) ?
        0x10000UL : WENDIAN(lpstSegInfo->segLen));
    *lpwFlags = WENDIAN(lpstSegInfo->segFlags);

    return(EXTSTATUS_OK);
}


EXTSTATUS   ScanArea
(
    LPCALLBACKREV1  lpCallBack,    // File op callbacks
    HFILE           hFile,         // Handle to file to scan
    LPBYTE          lpbyWorkBuffer,// Work buffer >= 512 bytes
    DWORD           dwOffset,      // where's the WIN header start?
    DWORD           dwScanDist,    // how far to look?
    WORD            wNumSigs,      // # of sigs to scan
    WINSIG_T *      pSigs,         // pointer to our sigs
    DWORD           dwSigFlags,    // bitwise & for flags
    LPWORD          lpwVID         // Virus ID storage on hit
)
{
    DWORD       dwCurOffset;
    long        lBytesLeft;
    WORD        wIndex, wScanIndex, wBytesToScan, wTemp;

    // Make sure scan distance <= 128K

    if (dwScanDist > 0x1FFFEUL)
        return EXTSTATUS_FILE_ERROR;

    lBytesLeft = (long)dwScanDist;
    dwCurOffset = dwOffset;

    if (dwOffset & 0x80000000UL)
        return EXTSTATUS_FILE_ERROR;

    while (lBytesLeft > 0)
    {
        if (lpCallBack->FileSeek(hFile,dwCurOffset,SEEK_SET) != dwCurOffset)
            return EXTSTATUS_FILE_ERROR;

		if (lBytesLeft > SCAN_BUFFER_SIZE)
		{
			wBytesToScan = SCAN_BUFFER_SIZE;
	        lBytesLeft -= SCAN_BUFFER_SIZE - SCAN_OVERLAP_SIZE;
		}
		else
		{
            wBytesToScan = (WORD)lBytesLeft;
			lBytesLeft = 0;
		}

        if (lpCallBack->FileRead(hFile,lpbyWorkBuffer,wBytesToScan) !=
            wBytesToScan)
            return WINDOWS_UNKNOWN_FORMAT;

        for (wScanIndex = 0; wScanIndex < wBytesToScan; wScanIndex++)
        {
            for (wIndex = 0; wIndex < wNumSigs; wIndex++)
            {
                if (pSigs[wIndex].wFirstSigWord ==
                    AVDEREF_WORD(lpbyWorkBuffer + wScanIndex) &&
                    (pSigs[wIndex].dwFlags & dwSigFlags) &&
					(wScanIndex + pSigs[wIndex].wSigLen) <= wBytesToScan)
                {
                    WORD    wLen;
                    WORD    *pSigData;

                    wLen = pSigs[wIndex].wSigLen;
                    pSigData = pSigs[wIndex].pwSig;

                    for (wTemp = 2 ; wTemp < wLen ; wTemp++)
                        if (pSigData[wTemp] !=
                            lpbyWorkBuffer[wScanIndex + wTemp] &&
                            pSigData[wTemp] != WILD_CARD_VALUE)
                            break;

                    if (wTemp == wLen)
                    {
                        *lpwVID = pSigs[wIndex].wVID;

                        return EXTSTATUS_VIRUS_FOUND;
                    }
                }
            }
		}

        dwCurOffset += SCAN_BUFFER_SIZE - SCAN_OVERLAP_SIZE;
    }

    return EXTSTATUS_OK;
}


EXTSTATUS EXTScanFileNEMainEP
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPWININFO       lpstNEInfo,             // NE header
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    DWORD           dwWinHeaderOffset,      // where's the WIN header start?
    LPWORD         	lpwVID					// Virus ID storage on hit
)
{
    WORD            wIndex, wFlags, wEntry, wEntrySubConst;
    DWORD           dwOrigScanDist, dwScanDist, dwSecLen;
    DWORD           dwOffset;
    EXTSTATUS       wResult;

    // compute max distance we're going to want to search for sigs
    // from the main entrypoint

    dwOrigScanDist = 0;
	wEntrySubConst = 0;

    for (wIndex=0;wIndex < gwNumNESigs; wIndex++)
        if (gNESigs[wIndex].dwFlags & WINSIG_SCAN_MAIN_EP)
		{
			wEntrySubConst =
				(wEntrySubConst > gNESigs[wIndex].wEntrySubConst) ?
					wEntrySubConst : gNESigs[wIndex].wEntrySubConst;

            dwOrigScanDist =
				(dwOrigScanDist > gNESigs[wIndex].wDistToSearch) ?
                    dwOrigScanDist : gNESigs[wIndex].wDistToSearch;
		}

    if (!dwOrigScanDist)
        return(EXTSTATUS_OK);

	dwOrigScanDist += wEntrySubConst;

    // first determine section which contains the entrypoint

    //////////////////////////////////////////////////////////////////////////
    // pstNEInfo.cs is ONE based
    //////////////////////////////////////////////////////////////////////////

    if (GetNESectionInfo(lpCallBack,
                         hFile,
                         lpstNEInfo,
                         lpbyWorkBuffer,
                         dwWinHeaderOffset,
                         WENDIAN(lpstNEInfo->cs) - 1,
                         &dwOffset,
                         &dwSecLen,
                         &wFlags) != EXTSTATUS_OK)
        return(EXTSTATUS_FILE_ERROR);

    // constrain our scanning distance to within the segment

	// Check that IP does indeed lie inside segment

    if (dwSecLen <= WENDIAN(lpstNEInfo->ip))
		return WINDOWS_UNKNOWN_FORMAT;

	// Adjust section length to account for entrypoint

    wEntry = WENDIAN(lpstNEInfo->ip);

	if (wEntry >= wEntrySubConst)
		wEntry -= wEntrySubConst;
	else
		wEntry = 0;

	dwSecLen -= wEntry;

    dwScanDist = (dwOrigScanDist > dwSecLen) ? dwSecLen : dwOrigScanDist;

    wResult = ScanArea(lpCallBack,
                       hFile,
                       lpbyWorkBuffer,
                       dwOffset + wEntry,
                       dwScanDist,
                       gwNumNESigs,
                       gNESigs,
                       (DWORD)WINSIG_SCAN_MAIN_EP,
                       lpwVID);

    // lpwVID will have its VID set by ScanArea if a virus was found...

    return(wResult);
}

EXTSTATUS EXTScanFileNELastSeg              // This scans the last segment in
(                                           //  the segment table
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPWININFO       lpstNEInfo,             // NE header
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    DWORD           dwWinHeaderOffset,      // where's the WIN header start?
    LPWORD         	lpwVID					// Virus ID storage on hit
)
{
    WORD            wIndex, wFlags;
    DWORD           dwOrigScanDist, dwScanDist, dwSecLen;
    DWORD           dwOffset;
    EXTSTATUS       wResult;

    // compute max distance we're going to want to search for sigs
    // from the beginning of the segment

    dwOrigScanDist = 0;

    for (wIndex=0; wIndex < gwNumNESigs; wIndex++)
        if (gNESigs[wIndex].dwFlags & WINSIG_SCAN_LAST_SEG)
            dwOrigScanDist =
				(dwOrigScanDist > gNESigs[wIndex].wDistToSearch) ?
                    dwOrigScanDist : gNESigs[wIndex].wDistToSearch;

    if (!dwOrigScanDist)
        return(EXTSTATUS_OK);

    //////////////////////////////////////////////////////////////////////////
    // pstNEInfo.cs is ONE based
    //////////////////////////////////////////////////////////////////////////

    if (GetNESectionInfo(lpCallBack,            // Get info on last segment
                         hFile,
                         lpstNEInfo,
                         lpbyWorkBuffer,
                         dwWinHeaderOffset,
                         WENDIAN(lpstNEInfo->segTabEntries) - 1,
                         &dwOffset,
                         &dwSecLen,
                         &wFlags) != EXTSTATUS_OK)
        return(EXTSTATUS_FILE_ERROR);

    // Make sure we don't scan past the end of the sector
 
    dwScanDist = (dwOrigScanDist > dwSecLen) ? dwSecLen : dwOrigScanDist;

    wResult = ScanArea(lpCallBack,
                       hFile,
                       lpbyWorkBuffer,
                       dwOffset,
                       dwScanDist,
                       gwNumNESigs,
                       gNESigs,
                       (DWORD)WINSIG_SCAN_LAST_SEG,
                       lpwVID);

    // lpwVID will have its VID set by ScanArea if a virus was found...

    return(wResult);
}

#if 0

EXTSTATUS EXTScanFileNEAllCodeData
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPWININFO       lpstNEInfo,             // NE header
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    DWORD           dwWinHeaderOffset,      // where's the WIN header start?
    DWORD           dwData,                 // dwData = TRUE if scanning data
    LPWORD         	lpwVID					// Virus ID storage on hit
)
{
    WORD            wIndex, wFlags, wFlagAnd;
    DWORD           dwOffset, dwOrigScanDist, dwScanDist, dwSecLen;
    EXTSTATUS       wResult = EXTSTATUS_OK;

    // compute max distance we're going to want to search for sigs
    // from the main entrypoint

    dwOrigScanDist = 0;

    wFlagAnd = (dwData == WINSIG_SCAN_ALL_CODE) ? 0 : NE_SEG_FLAG_DATA_SEG;

    for (wIndex=0;wIndex < gwNumNESigs; wIndex++)
        if (gNESigs[wIndex].dwFlags & dwData)
            dwOrigScanDist = (dwOrigScanDist > gNESigs[wIndex].wDistToSearch) ?
                        dwOrigScanDist : gNESigs[wIndex].wDistToSearch;

    if (!dwOrigScanDist)
        return(EXTSTATUS_OK);

    // first determine section which contains the entrypoint

    for (wIndex = 0; wIndex < WENDIAN(pstNEInfo->segTabEntries) ; wIndex++)
    {
        if (GetNESectionInfo(lpCallBack,
                             hFile,
                             pstNEInfo,
                             lpbyWorkBuffer,
                             dwWinHeaderOffset,
                             wIndex,
                             &dwOffset,
                             &dwSecLen,
                             &wFlags) != EXTSTATUS_OK)
        	return(EXTSTATUS_FILE_ERROR);

        // are we dealing with a code or a data section?

        if ((wFlags & NE_SEG_FLAG_DATA_SEG) == wFlagAnd)
        {
            // constrain our scanning distance to within this segment

            dwScanDist = dwOrigScanDist > dwSecLen ? dwSecLen : dwOrigScanDist;

            wResult = ScanArea(lpCallBack,
                               hFile,
                               lpbyWorkBuffer,
                               dwOffset,
                               dwScanDist,
                               gwNumNESigs,
                               gNESigs,
                               (DWORD)WINSIG_SCAN_ALL_CODE,
                               lpwVID);

            if (wResult != EXTSTATUS_OK)
                return(wResult);
        }
    }

    // lpwVID will have its VID set by ScanArea if a virus was found...

    return(wResult);
}


EXTSTATUS EXTScanFileNEAllEP
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPWININFO       lpstNEInfo,             // NE header
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    DWORD           dwWinHeaderOffset,      // where's the WIN header start?
    LPWORD         	lpwVID					// Virus ID storage on hit
)
{
    DWORD           dwOffset, dwSecOffset, dwSecLen;
	DWORD			dwScanDist, dwOrigScanDist;
    WORD            wIndex, wSegNum, wEntry, wFlags, wEntrySubConst;
    BUNDLEHEADER    stBundleHeader;
    MOVEABLEBUNDLE  stMB;
    FIXEDBUNDLE     stFB;
	WORD			wResult;

	dwOrigScanDist = 0;
	wEntrySubConst = 0;

    for (wIndex=0;wIndex < gwNumNESigs; wIndex++)
        if (gNESigs[wIndex].dwFlags & WINSIG_SCAN_ALL_EP)
		{
			wEntrySubConst =
				(wEntrySubConst > gNESigs[wIndex].wEntrySubConst) ?
					wEntrySubConst : gNESigs[wIndex].wEntrySubConst;

            dwOrigScanDist =
				(dwOrigScanDist > gNESigs[wIndex].wDistToSearch) ?
                    dwOrigScanDist : gNESigs[wIndex].wDistToSearch;
		}

    if (!dwOrigScanDist)
        return(EXTSTATUS_OK);

	dwOrigScanDist += wEntrySubConst;

    // seek to the entry point table

    dwOffset = dwWinHeaderOffset + WENDIAN(lpstNEInfo->entryTabOffset);

    do
    {
	    if (lpCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
    	    return EXTSTATUS_FILE_ERROR;

        if (lpCallBack->FileRead(hFile,
                                 (LPBYTE)&stBundleHeader,
                                 sizeof(stBundleHeader)) !=
								 sizeof(stBundleHeader))
            return WINDOWS_UNKNOWN_FORMAT;

        // Endianize the structure

		dwOffset += sizeof(stBundleHeader);

        for (wIndex = 0; wIndex < stBundleHeader.byBundleCount; wIndex++)
        {
            switch (stBundleHeader.byBundleType)
            {
				case NE_BUNDLE_NULL:
					stBundleHeader.byBundleCount = 0;
					continue;

                case NE_BUNDLE_MOVEABLE:
                    // moveable segments

				    if (lpCallBack->FileSeek(hFile,dwOffset,SEEK_SET) !=
						dwOffset)
    	    			return EXTSTATUS_FILE_ERROR;

                    if (lpCallBack->FileRead(hFile,
											 (LPBYTE)&stMB,
											 sizeof(stMB)) !=
                        					 sizeof(stMB))
                        return WINDOWS_UNKNOWN_FORMAT;

					dwOffset += sizeof(stMB);

                    wSegNum = stMB.bySegNum; // This is actually a BYTE
                    wEntry = WENDIAN(stMB.wEntry);

                    break;

                case NE_BUNDLE_CONSTANT_MODULE:

					// Skip over it all

					dwOffset += stBundleHeader.byBundleCount * sizeof(stFB);
					stBundleHeader.byBundleCount = 0;
					continue;

                default:
                    // fixed segment number

				    if (lpCallBack->FileSeek(hFile,dwOffset,SEEK_SET) !=
						dwOffset)
    	    			return EXTSTATUS_FILE_ERROR;

                    if (lpCallBack->FileRead(hFile,
											 (LPBYTE)&stFB,
											 sizeof(stFB)) !=
					                         sizeof(stFB))
                        return WINDOWS_UNKNOWN_FORMAT;

					dwOffset += sizeof(stFB);

                    // in this case, the byBundleType = the seg #

                    wSegNum = stBundleHeader.byBundleType;
                    wEntry = WENDIAN(stFB.wEntry);

                    break;
            }

            // now wSegNum and wEntry have the appropriate values.

            if (GetNESectionInfo(lpCallBack,
                                 hFile,
                                 pstNEInfo,
                                 lpbyWorkBuffer,
                                 dwWinHeaderOffset,
                                 wSegNum-1,
                                 &dwSecOffset,
                                 &dwSecLen,
                                 &wFlags) != EXTSTATUS_OK)
            	return(EXTSTATUS_FILE_ERROR);

			// Make sure the entry lies within the segment

            if (dwSecLen <= wEntry)
                continue;

			// Adjust section length

			if (wEntry >= wEntrySubConst)
				wEntry -= wEntrySubConst;
			else
				wEntry = 0;

			dwSecLen -= wEntry;

            dwScanDist = (dwOrigScanDist > dwSecLen) ?
				dwSecLen : dwOrigScanDist;

            wResult = ScanArea(lpCallBack,
                               hFile,
                               lpbyWorkBuffer,
                               dwSecOffset + wEntry,
                               dwScanDist,
                               gwNumNESigs,
                               gNESigs,
                               (DWORD)WINSIG_SCAN_ALL_EP,
                               lpwVID);

            if (wResult != EXTSTATUS_OK)
                return(wResult);
        }
    }
	while ( stBundleHeader.byBundleCount );

	return EXTSTATUS_OK;
}

#endif

//********************************************************************
//
// Function:
//	EXTSTATUS EXTScanFileNE()
//
// Description:
//	Scans a file for the presence of Windows NE viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************


EXTSTATUS EXTScanFileNE
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    DWORD           dwNEStart,              // start offset of NE header
    LPWORD         	lpwVID					// Virus ID storage on hit
)
{
    WORD    wReturn = EXTSTATUS_OK;
    WININFO stNEInfo;

    *lpwVID = 0;

    stNEInfo = *((WININFO FAR *)lpbyWorkBuffer);

    wReturn = EXTScanFileNEMainEP(lpCallBack,
                                  hFile,
                                  &stNEInfo,
                                  lpbyWorkBuffer,
                                  dwNEStart,
                                  lpwVID);

    if (EXTSTATUS_OK == wReturn)
        wReturn = EXTScanFileNELastSeg(lpCallBack,
                                        hFile,
                                        &stNEInfo,
                                        lpbyWorkBuffer,
                                        dwNEStart,
                                        lpwVID);

#if 0

    if (EXTSTATUS_OK == wReturn)
        wReturn = EXTScanFileNEAllEP(lpCallBack,
                                     hFile,
                                     &stNEInfo,
                                     lpbyWorkBuffer,
                                     dwNEStart,
                                     lpwVID);


    // all code

    if (EXTSTATUS_OK == wReturn)
        wReturn = EXTScanFileNEAllCodeData(lpCallBack,
                                           hFile,
                                           &stNEInfo,
                                           lpbyWorkBuffer,
                                           dwNEStart,
                                           (DWORD)WINSIG_SCAN_ALL_CODE,
                                           lpwVID);

    // all data

    if (EXTSTATUS_OK == wReturn)
        wReturn = EXTScanFileNEAllCodeData(lpCallBack,
                                           hFile,
                                           &stNEInfo,
                                           lpbyWorkBuffer,
                                           dwNEStart,
                                           (DWORD)WINSIG_SCAN_ALL_DATA,
                                           lpwVID);
#endif

	return (wReturn);
}


// Scan PE

EXTSTATUS EXTPELocateSectionWithRVA
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPNAVEX_IMAGE_NT_HEADERS lpstPEInfo,             // PE header
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    DWORD           dwWinHeaderOffset,      // where's the WIN header start?
	DWORD			dwRVA,					// RVA to search for
    LPDWORD         lpdwOffset,         	// where is this section from TOF?
    LPDWORD         lpdwSecLen,             // how long is this section?
    LPDWORD         lpdwFlags,              // section flags
	LPDWORD			lpdwRVABase             // RVA in memory
)
{
	DWORD			i;

    // first determine section which contains the entrypoint

    for (i=0;i<WENDIAN(lpstPEInfo->FileHeader.NumberOfSections);i++)
	{
    	if (GetPESectionInfo(lpCallBack,
                         	 hFile,
                             lpbyWorkBuffer,
                             dwWinHeaderOffset,
							 i,
                         	 lpdwOffset,
                         	 lpdwSecLen,
                         	 lpdwFlags,
							 lpdwRVABase) != EXTSTATUS_OK)
        	return(EXTSTATUS_FILE_ERROR);

		if (*lpdwRVABase <= dwRVA &&
			dwRVA < *lpdwRVABase + *lpdwSecLen)
			return(EXTSTATUS_OK);
	}

	return(EXTSTATUS_FILE_ERROR);
}

EXTSTATUS EXTScanFilePEMainEP
(
    LPCALLBACKREV1  	lpCallBack,         // File op callbacks
    HFILE          		hFile,				// Handle to file to scan
    LPNAVEX_IMAGE_NT_HEADERS  lpstPEInfo,         // PE header
    LPBYTE         		lpbyWorkBuffer,		// Work buffer >= 512 bytes
    DWORD           	dwWinHeaderOffset,  // where's the WIN header start?
    LPWORD         		lpwVID				// Virus ID storage on hit
)
{
    WORD            wIndex;
    DWORD           dwOrigScanDist, dwScanDist, dwSecLen, dwRVABase;
    DWORD           dwOffset, dwOffsetInSection, dwEntrySubConst;
	DWORD			dwFlags;
    EXTSTATUS       wResult;

    // compute max distance we're going to want to search for sigs
    // from the main entrypoint

    dwOrigScanDist = 0;
	dwEntrySubConst = 0;

    for (wIndex=0;wIndex < gwNumPESigs; wIndex++)
        if (gPESigs[wIndex].dwFlags & WINSIG_SCAN_MAIN_EP)
		{
			dwEntrySubConst =
				(dwEntrySubConst > gPESigs[wIndex].wEntrySubConst) ?
					dwEntrySubConst : gPESigs[wIndex].wEntrySubConst;

            dwOrigScanDist =
				(dwOrigScanDist > gPESigs[wIndex].wDistToSearch) ?
                    dwOrigScanDist : gPESigs[wIndex].wDistToSearch;
		}

    if (!dwOrigScanDist)
        return(EXTSTATUS_OK);

	dwOrigScanDist += dwEntrySubConst;

	if (EXTPELocateSectionWithRVA(lpCallBack,
								  hFile,
								  lpstPEInfo,
    							  lpbyWorkBuffer,
								  dwWinHeaderOffset,
                                  DWENDIAN(lpstPEInfo->OptionalHeader.
                                    AddressOfEntryPoint),
								  &dwOffset,
								  &dwSecLen,
								  &dwFlags,
								  &dwRVABase) != EXTSTATUS_OK)
		return(EXTSTATUS_OK);

    // constrain our scanning distance to within the segment

    dwOffsetInSection = DWENDIAN(lpstPEInfo->OptionalHeader.
        AddressOfEntryPoint) - dwRVABase;

	if (dwOffsetInSection >= dwEntrySubConst)
		dwOffsetInSection -= dwEntrySubConst;
	else
		dwOffsetInSection = 0;

	dwOffset +=	dwOffsetInSection;

    dwScanDist = dwSecLen - dwOffsetInSection;
	if (dwScanDist > dwOrigScanDist)
		dwScanDist = dwOrigScanDist;

    wResult = ScanArea(lpCallBack,
                       hFile,
                       lpbyWorkBuffer,
                       dwOffset,
                       dwScanDist,
                       gwNumPESigs,
                       gPESigs,
                       (DWORD)WINSIG_SCAN_MAIN_EP,
                       lpwVID);

    // lpwVID will have its VID set by ScanArea if a virus was found...

    return(wResult);
}

#if 0

EXTSTATUS EXTScanFilePEAllCodeData
(
    LPCALLBACKREV1  	lpCallBack,         // File op callbacks
    HFILE          		hFile,				// Handle to file to scan
    LPNAVEX_IMAGE_NT_HEADERS  lpstPEInfo,         // PE header
    LPBYTE         		lpbyWorkBuffer,		// Work buffer >= 512 bytes
    DWORD           	dwWinHeaderOffset,  // where's the WIN header start?
    DWORD           	dwData,             // dwData = TRUE if scanning data
    LPWORD         		lpwVID				// Virus ID storage on hit
)
{
    DWORD           dwIndex;
	DWORD			dwFlags, dwFlagAnd;
    DWORD           dwOffset, dwOrigScanDist, dwScanDist, dwSecLen, dwRVABase;
    EXTSTATUS       wResult = EXTSTATUS_OK;

    // compute max distance we're going to want to search for sigs
    // from the main entrypoint

    dwOrigScanDist = 0;

	if (dwData == WINSIG_SCAN_ALL_CODE)
        dwFlagAnd = NAVEX_IMAGE_SCN_CNT_CODE | NAVEX_IMAGE_SCN_MEM_EXECUTE;
	else
        dwFlagAnd = NAVEX_IMAGE_SCN_CNT_INITIALIZED_DATA;

    for (dwIndex=0;dwIndex < gwNumPESigs; dwIndex++)
        if (gPESigs[dwIndex].dwFlags & dwData)
            dwOrigScanDist = (dwOrigScanDist > gPESigs[dwIndex].wDistToSearch) ?
                        dwOrigScanDist : gPESigs[dwIndex].wDistToSearch;

    if (!dwOrigScanDist)
        return(EXTSTATUS_OK);

    for (dwIndex = 0;
        dwIndex < WENDIAN(lpstPEInfo->FileHeader.NumberOfSections);
        dwIndex++)
    {
        if (GetPESectionInfo(lpCallBack,
                             hFile,
                             lpbyWorkBuffer,
                             dwWinHeaderOffset,
                             dwIndex,
                             &dwOffset,
                             &dwSecLen,
                             &dwFlags,
							 &dwRVABase) != EXTSTATUS_OK)
        	return(EXTSTATUS_FILE_ERROR);

        // are we dealing with a code or a data section?

        if (dwFlags & dwFlagAnd)
        {
            // constrain our scanning distance to within this segment

            dwScanDist = dwOrigScanDist > dwSecLen ? dwSecLen : dwOrigScanDist;

            wResult = ScanArea(lpCallBack,
                               hFile,
                               lpbyWorkBuffer,
                               dwOffset,
                               dwScanDist,
                               gwNumPESigs,
                               gPESigs,
                               (DWORD)WINSIG_SCAN_ALL_CODE,
                               lpwVID);

            if (wResult != EXTSTATUS_OK)
                return(wResult);
        }
    }

    // lpwVID will have its VID set by ScanArea if a virus was found...

    return(wResult);
}


EXTSTATUS EXTScanFilePEAllEP
(
    LPCALLBACKREV1  	lpCallBack,         // File op callbacks
    HFILE          		hFile,				// Handle to file to scan
    LPNAVEX_IMAGE_NT_HEADERS  lpstPEInfo,         // PE header
    LPBYTE         		lpbyWorkBuffer,		// Work buffer >= 512 bytes
    DWORD           	dwWinHeaderOffset,  // where's the WIN header start?
    LPWORD         		lpwVID				// Virus ID storage on hit
)
{
    DWORD           		dwOffset, dwSecOffset, dwSecLen, dwRVABase;
	DWORD					i, dwScanDist, dwOrigScanDist;
	DWORD					dwEntrySubConst, dwFlags, dwFuncRVA;
	DWORD					dwFunctionTableOffset, dwOffsetInSection;
    WORD            		wIndex, wSegNum, wEntry;
    BUNDLEHEADER    		stBundleHeader;
    MOVEABLEBUNDLE  		stMB;
    FIXEDBUNDLE     		stFB;
	WORD					wResult;
    NAVEX_IMAGE_EXPORT_DIRECTORY  stImageExportDir;

	dwOrigScanDist = 0;
	dwEntrySubConst = 0;

    for (wIndex=0;wIndex < gwNumPESigs; wIndex++)
        if (gPESigs[wIndex].dwFlags & WINSIG_SCAN_ALL_EP)
		{
			dwEntrySubConst =
				(dwEntrySubConst > gPESigs[wIndex].wEntrySubConst) ?
					dwEntrySubConst : gPESigs[wIndex].wEntrySubConst;

            dwOrigScanDist =
				(dwOrigScanDist > gPESigs[wIndex].wDistToSearch) ?
                    dwOrigScanDist : gPESigs[wIndex].wDistToSearch;
		}

    if (!dwOrigScanDist)
        return(EXTSTATUS_OK);

	dwOrigScanDist += dwEntrySubConst;

    // seek to the export table and read it

	if (EXTPELocateSectionWithRVA(lpCallBack,
		hFile,
		lpstPEInfo,
    	lpbyWorkBuffer,
		dwWinHeaderOffset,
        DWENDIAN(lpstPEInfo->OptionalHeader.
            DataDirectory[NAVEX_IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress),
		&dwOffset,
		&dwSecLen,
		&dwFlags,
		&dwRVABase) != EXTSTATUS_OK)
		return(EXTSTATUS_OK);

    dwOffset += DWENDIAN(lpstPEInfo->OptionalHeader.
        DataDirectory[NAVEX_IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress) -
		dwRVABase;

    if (lpCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
   	    return EXTSTATUS_FILE_ERROR;

    if (lpCallBack->FileRead(hFile,
                             (LPBYTE)&stImageExportDir,
                             sizeof(stImageExportDir)) !=
		 					 sizeof(stImageExportDir))
        return EXTSTATUS_FILE_ERROR;

	// Find location of AddressOfFunctions table

	if (EXTPELocateSectionWithRVA(lpCallBack,
		hFile,
		lpstPEInfo,
    	lpbyWorkBuffer,
		dwWinHeaderOffset,
        (DWORD)DWENDIAN(stImageExportDir.AddressOfFunctions),
		&dwOffset,
		&dwSecLen,
		&dwFlags,
		&dwRVABase) != EXTSTATUS_OK)
		return(EXTSTATUS_OK);

	dwFunctionTableOffset = dwOffset +
        (DWORD)DWENDIAN(stImageExportDir.AddressOfFunctions) - dwRVABase;

    for (i=0;i<DWENDIAN(stImageExportDir.NumberOfFunctions);i++)
	{
	    if (lpCallBack->FileSeek(hFile,dwFunctionTableOffset,SEEK_SET) !=
			dwFunctionTableOffset)
    	    return EXTSTATUS_FILE_ERROR;

        if (lpCallBack->FileRead(hFile,
                                 (LPBYTE)&dwFuncRVA,
                                 sizeof(dwFuncRVA)) !=
								 sizeof(dwFuncRVA))
            return EXTSTATUS_FILE_ERROR;

		dwFunctionTableOffset += sizeof(dwFuncRVA);

		if (EXTPELocateSectionWithRVA(lpCallBack,
			hFile,
			lpstPEInfo,
	    	lpbyWorkBuffer,
			dwWinHeaderOffset,
			dwFuncRVA,
			&dwOffset,
			&dwSecLen,
			&dwFlags,
			&dwRVABase) != EXTSTATUS_OK)
			return(EXTSTATUS_OK);

		dwOffsetInSection = dwFuncRVA - dwRVABase;

		if (dwOffsetInSection >= dwEntrySubConst)
			dwOffsetInSection -= dwEntrySubConst;
		else
			dwOffsetInSection = 0;

		dwOffset += dwOffsetInSection;

	    dwScanDist = dwSecLen - dwOffsetInSection;
		if (dwScanDist > dwOrigScanDist)
			dwScanDist = dwOrigScanDist;

        wResult = ScanArea(lpCallBack,
                           hFile,
                           lpbyWorkBuffer,
                           dwOffset,
                           dwScanDist,
                           gwNumPESigs,
                           gPESigs,
                           (DWORD)WINSIG_SCAN_ALL_EP,
                           lpwVID);

        if (wResult != EXTSTATUS_OK)
            return(wResult);
    }

	return EXTSTATUS_OK;
}

#endif

//********************************************************************
//
// Function:
//	EXTSTATUS EXTScanFilePE()
//
// Description:
//	Scans a file for the presence of Windows PE viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************

EXTSTATUS EXTScanFilePE
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    DWORD           dwPEStart,              // start offset of PE header
    LPWORD         	lpwVID					// Virus ID storage on hit
)
{
    WORD    wReturn = EXTSTATUS_OK;
    NAVEX_IMAGE_NT_HEADERS stPEInfo;

    *lpwVID = 0;

    stPEInfo = *((LPNAVEX_IMAGE_NT_HEADERS)lpbyWorkBuffer);

    wReturn = EXTScanFilePEMainEP(lpCallBack,
                                  hFile,
                                  &stPEInfo,
                                  lpbyWorkBuffer,
                                  dwPEStart,
                                  lpwVID);

#if 0

    if (EXTSTATUS_OK == wReturn)
        wReturn = EXTScanFilePEAllEP(lpCallBack,
                                     hFile,
                                     &stPEInfo,
                                     lpbyWorkBuffer,
                                     dwPEStart,
                                     lpwVID);


    // all code

    if (EXTSTATUS_OK == wReturn)
        wReturn = EXTScanFilePEAllCodeData(lpCallBack,
                                           hFile,
                                           &stPEInfo,
                                           lpbyWorkBuffer,
                                           dwPEStart,
                                           (DWORD)WINSIG_SCAN_ALL_CODE,
                                           lpwVID);

    // all data

    if (EXTSTATUS_OK == wReturn)
        wReturn = EXTScanFilePEAllCodeData(lpCallBack,
                                           hFile,
                                           &stPEInfo,
                                           lpbyWorkBuffer,
                                           dwPEStart,
                                           (DWORD)WINSIG_SCAN_ALL_DATA,
                                           lpwVID);
#endif

	return (wReturn);
}
