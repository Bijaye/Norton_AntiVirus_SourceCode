//************************************************************************
//
// $Header:   S:/NAVEX/VCS/nepescan.cpv   1.34   17 Dec 1998 15:09:18   relniti  $
//
// Description:
//      Source file for Windows scanning engine.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/nepescan.cpv  $
// 
//    Rev 1.34   17 Dec 1998 15:09:18   relniti
// REMOVING #ifdef DKZOO
// 
//    Rev 1.33   09 Dec 1998 13:03:48   MMAGEE
// removed #ifdef/endif for SARC_CUST_7982
// removed #ifdef/endif for SARC_CUST_7930
// removed #ifdef/endif for SARC_CUST_7557
// 
//    Rev 1.32   24 Nov 1998 12:47:38   DKESSNE
// added call to special scan function for W95.Parvo
// 
//    Rev 1.31   18 Nov 1998 15:36:50   DKESSNE
// modified EXTScanFilePEMainEP to calculate file offset correctly when
// EP is found in a section, but EP < BaseOfCode.  This was causing NAV to
// miss a W95.Marburg sample (CI 7390).
// 
//    Rev 1.30   17 Nov 1998 15:01:14   DKESSNE
// added EXTScanFilePELastSegStart, to scan PE files from the beginning
// of the last section (for W95.Padania, CI 7557)
// 
//    Rev 1.29   17 Nov 1998 09:59:22   AOONWAL
// Removed #ifdef SARC_CUST_6689
// 
//    Rev 1.28   12 Nov 1998 19:06:26   DKESSNE
// added scan for W95.HPS (CI 6689)
// 
//    Rev 1.27   23 Oct 1998 13:07:38   DKESSNE
// Fixed bug in ScanArea():  The function had been returning if it read past
// the end of the file, rather than scanning what it did read.
// 
//    Rev 1.26   22 Oct 1998 17:53:12   DKESSNE
// added call to ScanLibertine from ScanAreaSpecial
// 
//    Rev 1.25   21 Oct 1998 16:20:22   DKESSNE
// removed ifdefs around the call to ScanMemorial
// 
//    Rev 1.24   14 Oct 1998 12:04:56   MMAGEE
// removed #ifdef/endif for REDTEAMKRNL, VID 0x1ad7
// 
//    Rev 1.23   12 Oct 1998 10:59:42   CNACHEN
// Added support for AOL pws heuristics.
// 
// 
//    Rev 1.22   10 Oct 1998 16:41:44   DKESSNE
// added call to ScanMemorial in ScanAreaSpecial
// 
//    Rev 1.21   23 Sep 1998 16:20:34   MMAGEE
// reverting to rev 1.19 to restore red team kernel ifdef
// 
//    Rev 1.19   15 Sep 1998 01:34:54   JWILBER
// Added function EXTScanFileRedTeamKrnl to scan for RedTeam in
// Krnl386.EXE.  #ifdef'd with REDTEAMKRNL.
//
//    Rev 1.18   27 Aug 1998 14:10:32   JWILBER
// Removed SARC_ZOO #ifdefs.
//
//    Rev 1.17   25 Aug 1998 12:26:40   CNACHEN
// added w95.inca detection, commented out with #ifdef SARC_ZOO_DARRENK
//
//    Rev 1.16   19 Aug 1998 12:46:00   CNACHEN
// Updated windows engine to scan last section of PE files, added
// W95.Cabanas.E
//
//    Rev 1.15   03 Aug 1998 13:31:54   MMAGEE
// removed ifdef/endif for sarc_marburg;  VID 0x2566;  CI 2291
//
//    Rev 1.14   03 Aug 1998 11:58:14   PDEBATS
// Removed sarc_cst_pecih if/else/endif statements
//
//    Rev 1.13   27 Jul 1998 12:05:38   DKESSNE
// added ScanAreaSpecialPEMainEP for Win95 viruses that cannot be detected
// using the ScanArea function.  Virus specific scan functions will go in
// the file PESPSCAN.CPP.   Also added #include "pespscan.h".
// Currently #ifdef'd with SARC_MARBURG
//
//    Rev 1.12   08 Jul 1998 11:02:44   AOONWAL
// properly removed ifdef SARC_CST_PECIH (else part)
//
//    Rev 1.11   07 Jul 1998 16:09:56   AOONWAL
// commented out ifdef SARC_CST_PECIH
//
//    Rev 1.10   30 Jun 1998 17:08:22   DKESSNE
// forgot to endianize
//
//    Rev 1.9   30 Jun 1998 15:50:16   DKESSNE
// slight modification to EP check in EXTScanFilePEMainEP
// (set dwScanDist for EP < BaseOfCode)
//
//    Rev 1.8   26 Jun 1998 13:21:50   DKESSNE
// added #ifdef SARCBOT include "config.h"
// added check for EntryPoint < BaseOfCode in EXTScanFilePEMainEP
//   (W95.CIH has viral code at the end of the PE Header, not in any segment)
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

#ifdef SARCBOT
#include "config.h"
#endif

#include "endutils.h"

#include "nepescan.h"
#include "winsig.h"

#include "winconst.h"

#include "nepeshr.h"

#include "pespscan.h"

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
    LPWORD          lpwVID,                 // Virus ID storage on hit
    LPWININFO       lpstNEHeader,           // Filled in for NE files
    LPDWORD         lpdwNEHeaderOffset      // 0 for non-NE, non-zero for valid NE
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

    *lpdwNEHeaderOffset = 0;

	switch (wFileType)
	{
        case WINDOWS_NE_FORMAT:

            *lpstNEHeader = *(LPWININFO)lpbyWorkBuffer;

			wReturn = EXTScanFileNE(lpCallBack,hFile,lpbyWorkBuffer,
				dwWinHeaderOffset,lpwVID);

            // remember our NE offset

            *lpdwNEHeaderOffset = dwWinHeaderOffset;

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

        wBytesToScan = lpCallBack->FileRead(hFile,lpbyWorkBuffer,wBytesToScan);

        if (wBytesToScan == UERROR)
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


#define KRNLNAMELEN 7   // Number of bytes to check in Resident Name Table
#define INITTASKLEN 9   // Number of bytes to check in Non-Res Name Table
#define REDTEAMLEN  4766    // Length of RedTeam virus

EXTSTATUS EXTScanFileNERedTeamKrnl          // Check KRNL386.EXE for RedTeam
(                                           //  infection
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPWININFO       lpstNEInfo,             // NE header
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    DWORD           dwWinHeaderOffset,      // where's the WIN header start?
    LPWORD         	lpwVID					// Virus ID storage on hit
)
{
    BYTE            byKrnlName[] =  { 0x06, 0x4b, 0x45, 0x52,
                                      0x4e, 0x45, 0x4c };   // 6, 'KERNEL'
    BYTE            byInitTask[] =  { 0x08, 0x49, 0x4e, 0x49, 0x54, 0x54,
                                      0x41, 0x53, 0x4b };   // 8, 'INITTASK'
    WORD            wCount, wIndex, wFlags, wEntry, wEntryOff, wSegNum;
    DWORD           dwOffset, dwSecLen;
    EXTSTATUS       wResult;
    BUNDLEHEADER    stBundleHeader;
    FIXEDBUNDLE     stFixedBundle;
    MOVEABLEBUNDLE  stMoveBundle;

    // _asm    int 3            // Remove when done debugging   &&RTK
 
    wResult = EXTSTATUS_FILE_ERROR;
                                // Compute location of Resident Name Table
    dwOffset = dwWinHeaderOffset +
                            (DWORD) WENDIAN(lpstNEInfo->resNameTabOffset);

    if (dwOffset != FILESEEK(dwOffset)) // Goto Resident Name Table
        return(wResult);
                                        // Read first entry in table
    if (KRNLNAMELEN != FILEREAD(lpbyWorkBuffer, KRNLNAMELEN))
        return(wResult);

    wCount = 0;                         // See if entry is 'KERNEL'
    wIndex = 0;

    do
    {
        if (byKrnlName[wIndex] == lpbyWorkBuffer[wIndex])
            wCount++;

        wIndex++;
    } while ((wIndex == wCount) && (wIndex < KRNLNAMELEN));

    if (KRNLNAMELEN != wCount)          // If not, no RedTeam
        return(EXTSTATUS_OK);
                                        // Compute location of Non-res Name
    dwOffset = (DWORD) WENDIAN(lpstNEInfo->nonResTabOffset);    // Table

    wCount = 0;
 
    do                      // Look for 'INITTASK' in the non-res name table
    {
        if (dwOffset != FILESEEK(dwOffset))     // Seek to next read
            return(wResult);

        if (1 != FILEREAD(lpbyWorkBuffer, 1))   // Read length of name entry
            return(wResult);

        dwOffset++;         // Adjust position after reading a byte

        if (0 != lpbyWorkBuffer[0])             // If it's not zero
        {
            if ((INITTASKLEN - 1) != lpbyWorkBuffer[0]) // If it's not the
            {                                           //  right length
                dwOffset += (DWORD) lpbyWorkBuffer[0];      // Compute offset
                dwOffset += 2;                              //  of next entry
            }
            else                                    // If it is the right
            {                                       //  length
                if ((INITTASKLEN - 1) != FILEREAD((lpbyWorkBuffer + 1),
                                                        (INITTASKLEN - 1)))
                    return(wResult);            // Read the entry
                                                // Adjust for length of read
                dwOffset += (INITTASKLEN + 1);  // (INITTASK - 1) + following
                                                // word (2) = INITTASK + 1
                wCount = 0;                 // See if this is 'INITTASK'
                wIndex = 0;

                do
                {
                    if (byInitTask[wIndex] == lpbyWorkBuffer[wIndex])
                        wCount++;   // wCount = INITTASKLEN if 'INITTASK'
                                    //  was found
                    wIndex++;
                } while ((wIndex == wCount) && (wIndex < INITTASKLEN));
            }
        }
    } while ((0 != lpbyWorkBuffer[0]) && (INITTASKLEN != wCount));

    if (INITTASKLEN != wCount)      // Bail if 'INITTASK' not found
        return(EXTSTATUS_OK);
                        // If found, file ptr is at Entry table offset value
    if (sizeof(WORD) != FILEREAD(&wEntry, sizeof(WORD)))
        return(wResult);        // Read entry table offset

    wEntry = WENDIAN(wEntry);       // Endianize
                                    // Compute location of Entry Table
    dwOffset = dwWinHeaderOffset +
                            (DWORD) WENDIAN(lpstNEInfo->entryTabOffset);
    wCount = 0;     // When wCount == wEntry, we've found the correct entry
    do              // Find entry n in the entry table, where n = wEntry
    {
	    if (dwOffset != FILESEEK(dwOffset))
    	    return(wResult);

        if (sBUNDLEHEADER != FILEREAD((LPBYTE) &stBundleHeader,
                                                            sBUNDLEHEADER))
            return(WINDOWS_UNKNOWN_FORMAT);

		dwOffset += sBUNDLEHEADER;      // Update position for header read

        switch (stBundleHeader.byBundleType)
        {
            case NE_BUNDLE_MOVEABLE:
                if ((wCount + stBundleHeader.byBundleCount) < wEntry)
                {           // If this bundle doesn't have the entry we seek
                    dwOffset += stBundleHeader.byBundleCount *
                                                            sMOVEABLEBUNDLE;
                    wCount += stBundleHeader.byBundleCount;
                }
                else                    // If this bundle contains INITTASK
                {                       // compute location of entry we need
                    wIndex = wEntry - wCount - 1;
                    wCount = wEntry;

                    dwOffset += wIndex * sMOVEABLEBUNDLE;

                    if (dwOffset != FILESEEK(dwOffset)) // Seek to it
                        return(wResult);
                                                        // Read it
                    if (sMOVEABLEBUNDLE !=
                                    FILEREAD(&stMoveBundle, sMOVEABLEBUNDLE))
                        return(wResult);
                                                        // Bundle type = seg #
                    wSegNum = (WORD) stMoveBundle.bySegNum;
                    wEntryOff = WENDIAN(stMoveBundle.wEntry);
                }

                break;

            case NE_BUNDLE_CONSTANT_MODULE:
                dwOffset += stBundleHeader.byBundleCount * sFIXEDBUNDLE;

			case NE_BUNDLE_NULL:
                wCount += stBundleHeader.byBundleCount;

                break;

            default:                            // Fixed segment number
                if ((wCount + stBundleHeader.byBundleCount) < wEntry)
                {   // If this bundle doesn't have the entry we seek
                    dwOffset += stBundleHeader.byBundleCount *
                                                            sFIXEDBUNDLE;
                    wCount += stBundleHeader.byBundleCount;
                }
                else            // If this bundle contains INITTASK
                {               // Compute location of entry we need
                    wIndex = wEntry - wCount - 1;
                    wCount = wEntry;
                    dwOffset += wIndex * sFIXEDBUNDLE;

                    if (dwOffset != FILESEEK(dwOffset)) // Seek to it
                        return(wResult);
                                                        // Read it
                    if (sFIXEDBUNDLE !=
                                    FILEREAD(&stFixedBundle, sFIXEDBUNDLE))
                        return(wResult);
                                                        // Bundle type = seg #
                    wSegNum = stBundleHeader.byBundleType;
                    wEntryOff = WENDIAN(stFixedBundle.wEntry);
                }

                break;
        }   // switch
    } while (wCount < wEntry);

    if (wCount != wEntry)
        return(EXTSTATUS_OK);
                                    // Use wSegNum from do-while loop above
    if (EXTSTATUS_OK != GetNESectionInfo(lpCallBack,
                                         hFile,
                                         lpstNEInfo,
                                         lpbyWorkBuffer,
                                         dwWinHeaderOffset,
                                         (wSegNum - 1),
                                         &dwOffset,
                                         &dwSecLen,
                                         &wFlags))
        return(wResult);
                                    // Make sure entry lies within segment
    if (dwSecLen <= (DWORD) (wEntryOff + REDTEAMSIGLEN))
        return(EXTSTATUS_OK);
                                    // Compute offset of RedTeam entrypoint
    dwOffset += dwSecLen - REDTEAMLEN + REDTEAMSUBCON;

    if (dwOffset != FILESEEK(dwOffset))
        return(wResult);

    if (REDTEAMSIGLEN != FILEREAD(lpbyWorkBuffer, REDTEAMSIGLEN))
        return(wResult);

    wCount = 0;
    wIndex = 0;

    do          // Compare buffer w/ RedTeam sig
    {
        if (gpwWinRedTeamSig[wIndex] == (WORD) lpbyWorkBuffer[wIndex])
            wCount++;
        else if (0xffff == gpwWinRedTeamSig[wIndex])
            wCount++;

        ++wIndex;
    } while ((wIndex == wCount) && (wIndex < REDTEAMSIGLEN));

    if (wIndex != wCount)               // If it doesn't match
        wResult = EXTSTATUS_OK;
    else
    {
        wResult = EXTSTATUS_VIRUS_FOUND;    // If it does
        *lpwVID = VID_REDTEAMKRNL;
    }

    return(wResult);
}   // EXTScanFileNERedTeamKrnl


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
                               // Checks KRNL386.EXE for RedTeam infection
    if (EXTSTATUS_OK == wReturn)
        wReturn = EXTScanFileNERedTeamKrnl(lpCallBack,
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




EXTSTATUS   ScanAreaSpecialPEMainEP
(
    LPCALLBACKREV1              lpCallBack,         // File op callbacks
    HFILE                       hFile,              // Handle to file to scan
    LPBYTE                      lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEInfo,         // PE header
    DWORD                       dwPEHeaderOffset,   // Offset of PE header
    DWORD                       dwSectionOffset,    // Start of section
    DWORD                       dwSegLength,        // Length of section
    DWORD                       dwEPOffset,         // Entry Point offset
    LPWORD                      lpwVID              // Virus ID storage on hit
)
{
    WORD    wResult;

//    _asm int 3h

    wResult = ScanMarburg(lpCallBack,
                          hFile,
                          lpbyWorkBuffer,
                          lpstPEInfo,
                          dwPEHeaderOffset,
                          dwSectionOffset,
                          dwSegLength,
                          dwEPOffset,
                          lpwVID);

    if (wResult == EXTSTATUS_OK)
        wResult = ScanInca(lpCallBack,
                           hFile,
                           lpbyWorkBuffer,
                           lpstPEInfo,
                           dwPEHeaderOffset,
                           dwSectionOffset,
                           dwSegLength,
                           dwEPOffset,
                           lpwVID);


    if (wResult == EXTSTATUS_OK)
        wResult = ScanMemorial(lpCallBack,
                               hFile,
                               lpbyWorkBuffer,
                               lpstPEInfo,
                               dwPEHeaderOffset,
                               dwSectionOffset,
                               dwSegLength,
                               dwEPOffset,
                               lpwVID);

    if (wResult == EXTSTATUS_OK)
        wResult = ScanLibertine(lpCallBack,
                               hFile,
                               lpbyWorkBuffer,
                               lpstPEInfo,
                               dwPEHeaderOffset,
                               dwSectionOffset,
                               dwSegLength,
                               dwEPOffset,
                               lpwVID);

    if (wResult == EXTSTATUS_OK)
        wResult = ScanHPS(lpCallBack,
                          hFile,
                          lpbyWorkBuffer,
                          lpstPEInfo,
                          dwPEHeaderOffset,
                          dwSectionOffset,
                          dwSegLength,
                          dwEPOffset,
                          lpwVID);

    if (wResult == EXTSTATUS_OK)
        wResult = ScanParvo(lpCallBack,
                            hFile,
                            lpbyWorkBuffer,
                            lpstPEInfo,
                            dwPEHeaderOffset,
                            dwSectionOffset,
                            dwSegLength,
                            dwEPOffset,
                            lpwVID);


/*
    if (wResult != EXTSTATUS_OK)
        wResult = ScanFoo(

    if (wResult != EXTSTATUS_OK)
        wResult = ScanbAR(

*/



    return wResult;
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
    DWORD           dwFlags;
    DWORD           dwSectionOffset, dwEPOffset;
    EXTSTATUS       wResult;

    WORD            wEPFoundInSection = 0;

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
    {
        //if EntryPoint < BaseOfCode, we still want to scan
        if ( DWENDIAN(lpstPEInfo->OptionalHeader.AddressOfEntryPoint) >=
             DWENDIAN(lpstPEInfo->OptionalHeader.BaseOfCode) )

        {
            return(EXTSTATUS_OK);
        }
    }
    else
    {
        wEPFoundInSection = 1;
    }


    dwSectionOffset = dwOffset;

    // constrain our scanning distance to within the segment

    dwOffsetInSection = DWENDIAN(lpstPEInfo->OptionalHeader.
        AddressOfEntryPoint) - dwRVABase;

    dwEPOffset = dwSectionOffset + dwOffsetInSection;

	if (dwOffsetInSection >= dwEntrySubConst)
		dwOffsetInSection -= dwEntrySubConst;
	else
		dwOffsetInSection = 0;

	dwOffset +=	dwOffsetInSection;

    dwScanDist = dwSecLen - dwOffsetInSection;

    //if EntryPoint < BaseOfCode
    //  dwOffset   = EntryPoint
    //  dwScanDist = BaseOfCode - EntryPoint

    if ( (!wEPFoundInSection) &&
         (DWENDIAN(lpstPEInfo->OptionalHeader.AddressOfEntryPoint) <
            DWENDIAN(lpstPEInfo->OptionalHeader.BaseOfCode)) )
    {
        dwOffset =

            dwSectionOffset =
                dwEPOffset =

                    DWENDIAN(lpstPEInfo->OptionalHeader.AddressOfEntryPoint);
        dwScanDist = DWENDIAN(lpstPEInfo->OptionalHeader.BaseOfCode) -
                     DWENDIAN(lpstPEInfo->OptionalHeader.AddressOfEntryPoint);
    }

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


    if (wResult == EXTSTATUS_OK)
    {
        wResult = ScanAreaSpecialPEMainEP(lpCallBack,
                                           hFile,
                                           lpbyWorkBuffer,
                                           lpstPEInfo,
                                           dwWinHeaderOffset,
                                           dwSectionOffset,
                                           dwSecLen,
                                           dwEPOffset,
                                           lpwVID);

    }


    // lpwVID will have its VID set by ScanArea if a virus was found...

    return(wResult);
}

EXTSTATUS EXTScanFilePELastSeg                          // This scans the last segment in
(                                                       //  the segment table
    LPCALLBACKREV1              lpCallBack,             // File op callbacks
    HFILE                       hFile,                  // Handle to file to scan
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEInfo,             // PE header
    LPBYTE                      lpbyWorkBuffer,         // Work buffer >= 512 bytes
    DWORD                       dwWinHeaderOffset,      // where's the WIN header start?
    LPWORD                      lpwVID                  // Virus ID storage on hit
)
{
    WORD            wIndex;
    DWORD           dwOffset, dwSecLen, dwFlags, dwRVABase, dwScanDist;
    EXTSTATUS       wResult;

    // compute max distance we're going to want to search for sigs
    // from the beginning of the segment

    dwScanDist = 0;

    for (wIndex=0;wIndex < gwNumPESigs; wIndex++)
        if (gPESigs[wIndex].dwFlags & WINSIG_SCAN_LAST_SEG)
		{
            dwScanDist =
                (dwScanDist > gPESigs[wIndex].wDistToSearch) ?
                    dwScanDist : gPESigs[wIndex].wDistToSearch;
		}

    if (!dwScanDist)
        return(EXTSTATUS_OK);

    // distance is from END of last section...

    // get offset and size of last segment

    if (GetPESectionInfo(lpCallBack,
                         hFile,
                         lpbyWorkBuffer,
                         dwWinHeaderOffset,
                         WENDIAN(lpstPEInfo->FileHeader.NumberOfSections)-1,
                         &dwOffset,
                         &dwSecLen,
                         &dwFlags,
                         &dwRVABase) != EXTSTATUS_OK)
        return(EXTSTATUS_FILE_ERROR);

    if (dwScanDist > dwSecLen)
        dwScanDist = dwSecLen;

    // update our starting location for searching to point to the end
    // of the last segment - the max scan distance (dwScanDist)...

    dwOffset += dwSecLen - dwScanDist;

    // scan entire segment

    wResult = ScanArea(lpCallBack,
                       hFile,
                       lpbyWorkBuffer,
                       dwOffset,
                       dwScanDist,
                       gwNumPESigs,
                       gPESigs,
                       (DWORD)WINSIG_SCAN_LAST_SEG,
                       lpwVID);

    // lpwVID will have its VID set by ScanArea if a virus was found...

    return(wResult);
}


EXTSTATUS EXTScanFilePELastSegStart
(
    LPCALLBACKREV1              lpCallBack,             // File op callbacks
    HFILE                       hFile,                  // Handle to file to scan
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEInfo,             // PE header
    LPBYTE                      lpbyWorkBuffer,         // Work buffer >= 512 bytes
    DWORD                       dwWinHeaderOffset,      // where's the WIN header start?
    LPWORD                      lpwVID                  // Virus ID storage on hit
)
// This scans the last section in the section table, from the beginning.
{
    WORD            wIndex;
    DWORD           dwOffset, dwSecLen, dwFlags, dwRVABase, dwScanDist;
    EXTSTATUS       wResult;

    // compute max distance we're going to want to search for sigs
    // from the beginning of the segment

    dwScanDist = 0;

    for (wIndex=0;wIndex < gwNumPESigs; wIndex++)
        if (gPESigs[wIndex].dwFlags & WINSIG_SCAN_LAST_SEG_START)
		{
            dwScanDist =
                (dwScanDist > gPESigs[wIndex].wDistToSearch) ?
                    dwScanDist : gPESigs[wIndex].wDistToSearch;
		}

    if (!dwScanDist)
        return(EXTSTATUS_OK);


    // get offset and size of last segment

    if (GetPESectionInfo(lpCallBack,
                         hFile,
                         lpbyWorkBuffer,
                         dwWinHeaderOffset,
                         WENDIAN(lpstPEInfo->FileHeader.NumberOfSections)-1,
                         &dwOffset,
                         &dwSecLen,
                         &dwFlags,
                         &dwRVABase) != EXTSTATUS_OK)
        return(EXTSTATUS_FILE_ERROR);

    if (dwScanDist > dwSecLen)
        dwScanDist = dwSecLen;

    // scan dwScanDist bytes,  starting at dwOffset
    wResult = ScanArea(lpCallBack,
                       hFile,
                       lpbyWorkBuffer,
                       dwOffset,
                       dwScanDist,
                       gwNumPESigs,
                       gPESigs,
                       (DWORD)WINSIG_SCAN_LAST_SEG_START,
                       lpwVID);

    // lpwVID will have its VID set by ScanArea if a virus was found...

    return(wResult);

}//EXTScanFilePELastSegStart


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

    if (EXTSTATUS_OK == wReturn)
        wReturn = EXTScanFilePELastSeg(lpCallBack,
                                       hFile,
                                       &stPEInfo,
                                       lpbyWorkBuffer,
                                       dwPEStart,
                                       lpwVID);

    if (EXTSTATUS_OK == wReturn)
        wReturn = EXTScanFilePELastSegStart(lpCallBack,
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
