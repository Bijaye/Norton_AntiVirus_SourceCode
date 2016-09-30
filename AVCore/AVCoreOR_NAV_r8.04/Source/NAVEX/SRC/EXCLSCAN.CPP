//************************************************************************
//
// $Header:   S:/NAVEX/VCS/exclscan.cpv   1.8   07 Apr 1997 18:45:52   DCHI  $
//
// Description:
//      Contains Excel macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/exclscan.cpv  $
// 
//    Rev 1.8   07 Apr 1997 18:45:52   DCHI
// Added code to speed up excel scanning by being more selective.
// 
//    Rev 1.7   05 Feb 1997 11:46:58   DCHI
// Added SAM modifications.
// 
//    Rev 1.6   26 Dec 1996 15:22:00   AOONWAL
// No change.
// 
//    Rev 1.5   02 Dec 1996 14:01:34   AOONWAL
// No change.
// 
//    Rev 1.4   29 Oct 1996 12:58:30   AOONWAL
// No change.
// 
//    Rev 1.3   28 Aug 1996 16:12:24   DCHI
// Use AVDEREF_WORD() for dereferencing WORD value.
// 
//    Rev 1.2   05 Aug 1996 10:38:16   DCHI
// Moved OLEOpenHexNamedStream to excelshr.cpp.
// 
//    Rev 1.1   26 Jul 1996 18:00:32   DCHI
// Make sure we don't scan sections that have 0-length directory entries in
// the ole file.
// 
//    Rev 1.0   25 Jul 1996 18:16:20   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
#include "ctsn.h"
#include "callback.h"
#endif

#include "navex.h"

#include "storobj.h"

#include "navexshr.h"

#include "endutils.h"

#include "excelshr.h"
#include "exclscan.h"

//********************************************************************
//
// Function:
//  EXTSTATUS EXTScanBufferExcel()
//
// Description:
//  Searches the buffer for Excel virus signatures.
//
// Returns:
//  EXTSTATUS_OK            No virus
//  EXTSTATUS_VIRUS_FOUND   Virus signature found
//
//********************************************************************

EXTSTATUS EXTScanBufferExcel
(
    LPBYTE          lpbyBuffer,
    DWORD           dwBufferSize,
    LPWORD          lpwVID
)
{
    DWORD           dwScanIndex;
    WORD			wSigIndex;

    for (dwScanIndex = 0; dwScanIndex < dwBufferSize ; dwScanIndex++)
    {
        for (wSigIndex = 0; wSigIndex < gwNumExcelSigs; wSigIndex++)
        {
            if (gExcelSigs[wSigIndex].wFirstSigWord ==
                AVDEREF_WORD(lpbyBuffer + dwScanIndex) &&
                (dwScanIndex + gExcelSigs[wSigIndex].wSigLen) <= dwBufferSize)
            {
                WORD    wTemp;
                WORD    wLen;
                PWORD   pwSigData;

                wLen = gExcelSigs[wSigIndex].wSigLen;
                pwSigData = gExcelSigs[wSigIndex].pwSig;
                for (wTemp = 2 ; wTemp < wLen ; wTemp++)
                {
                    if (pwSigData[wTemp] !=
                        lpbyBuffer[dwScanIndex + wTemp] &&
                        pwSigData[wTemp] != EXCEL_SIG_WILD_CARD_VALUE)
                        break;
                }

                if (wTemp == wLen)
                {
                    *lpwVID = gExcelSigs[wSigIndex].wVID;

#ifdef __MACINTOSH__    // get the signature back to the calling routine
				BlockMoveData( &lpbyBuffer[dwScanIndex], &lpbyBuffer[1], wLen );
                lpbyBuffer[0] = (uchar)wLen;
#endif
                    return(EXTSTATUS_VIRUS_FOUND);
                }
            }
        }
    }

    return(EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EXTSTATUS XL5Scan()
//
// Description:
//  Scans a stream the presence of Excel viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************

EXTSTATUS XL5Scan
(
    LPCALLBACKREV1  lpstCallBack,       // File op callbacks
    LPOLE_FILE      lpstOLEFile,        // OLE file structure of hex stream
    LPBYTE          lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPWORD          lpwVID              // Virus ID storage on hit
)
{

    WORD            wBytesRead;
    WORD            wBytesToRead;
    DWORD           dwOffset, dwMaxDist;

    // Limit search and read distance

    dwMaxDist = lpstOLEFile->siStreamInfo.dwStreamLen;
    if (dwMaxDist > 10000000lu)
        dwMaxDist = 10000000lu;

    // Scan in blocks

    dwOffset = 0;
    while (dwOffset < dwMaxDist)
    {
        if (dwOffset + 512 > dwMaxDist)
            wBytesToRead = (WORD)(dwMaxDist - dwOffset);
        else
            wBytesToRead = 512;

        wBytesRead = OLESeekRead(lpstCallBack,
                                 lpstOLEFile,
                                 dwOffset,
                                 lpbyWorkBuffer,
                                 wBytesToRead);

        if (wBytesRead != wBytesToRead)
        {
            // Read less than the desired number of blocks

            // Just use wBytesRead and break

            break;
        }

        // Search for the string

        if (EXTScanBufferExcel(lpbyWorkBuffer,
                               wBytesRead,
                               lpwVID) == EXTSTATUS_VIRUS_FOUND)
            return(EXTSTATUS_VIRUS_FOUND);

        dwOffset += 512 - 32;
    }

    // No virus

    return(EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//	EXTSTATUS EXTScanFileMacro()
//
// Description:
//  Scans a file for the presence of Excel viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************

EXTSTATUS EXTScanFileExcelMacro
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    LPWORD         	lpwVID					// Virus ID storage on hit
)
{
    OLE_FILE_T      stOLEFile;
    WORD            wMagicNum;

    // Looked for a Hex-Named stream

	wMagicNum = 0;
    while (OLEOpenHexNamedStream(lpstCallBack,
                                 hFile,
                                 &stOLEFile,
                                 &wMagicNum) == OLE_OK)
	{
        // Scan for viruses

        if (XL5Scan(lpstCallBack,
                    &stOLEFile,
                    lpbyWorkBuffer,
                    lpwVID) == EXTSTATUS_VIRUS_FOUND)
            return(EXTSTATUS_VIRUS_FOUND);
	}

	// No virus

    return(EXTSTATUS_OK);
}



