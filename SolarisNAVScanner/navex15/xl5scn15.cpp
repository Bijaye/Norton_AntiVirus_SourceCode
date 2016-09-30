//************************************************************************
//
// $Header:   S:/NAVEX/VCS/xl5scn15.cpv   1.1   07 Nov 1997 10:50:58   DCHI  $
//
// Description:
//      Contains Excel macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/xl5scn15.cpv  $
// 
//    Rev 1.1   07 Nov 1997 10:50:58   DCHI
// Sped up scanning by beginning at offset given by XL5GetMacroOffsetSize().
// 
//    Rev 1.0   09 Jul 1997 16:16:52   DCHI
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

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"

#include "wdscan15.h"
#include "offcsc15.h"

#include "xl5scn15.h"
#include "excelshr.h"

//********************************************************************
//
// Function:
//  EXTSTATUS XL5ScanBuffer()
//
// Description:
//  Searches the buffer for Excel virus signatures.
//
// Returns:
//  EXTSTATUS_OK            No virus
//  EXTSTATUS_VIRUS_FOUND   Virus signature found
//
//********************************************************************

EXTSTATUS XL5ScanBuffer
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
                WENDIAN(*(LPWORD)(lpbyBuffer+dwScanIndex)) &&
                (dwScanIndex + gExcelSigs[wSigIndex].wSigLen) <= dwBufferSize)
            {
                WORD    wTemp;
                WORD    wLen;
                LPWORD  lpwSigData;

                wLen = gExcelSigs[wSigIndex].wSigLen;
                lpwSigData = gExcelSigs[wSigIndex].lpwSig;
                for (wTemp = 2 ; wTemp < wLen ; wTemp++)
                {
                    if (lpwSigData[wTemp] !=
                        lpbyBuffer[dwScanIndex + wTemp] &&
                        lpwSigData[wTemp] != EXCEL_SIG_WILD_CARD_VALUE)
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
//  Scans a file for the presence of Excel viruses.
//
//  If a virus was found, the function sets
//  lpstOffcScan->u.stXL5.wVID to the VID of the virus.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//********************************************************************

EXTSTATUS XL5Scan
(
    LPMSOFFICE_SCAN         lpstOffcScan
)
{
    DWORD           dwBytesRead;
    DWORD           dwBytesToRead;
    DWORD           dwOffset, dwSize, dwMaxDist;
    BYTE            abyWorkBuffer[512];

    if (XL5GetMacroOffsetSize(lpstOffcScan->lpstStream,
                              &dwOffset,
                              &dwSize) == FALSE)
    {
        dwOffset = 0;

        // Limit search and read distance

        dwMaxDist = SSStreamLen(lpstOffcScan->lpstStream);
        if (dwMaxDist > 10000000lu)
            dwMaxDist = 10000000lu;
    }
    else
        dwMaxDist = dwOffset + dwSize;

    // Scan in blocks

    dwBytesToRead = 512;
    while (dwOffset < dwMaxDist)
    {
        if (dwMaxDist - dwOffset < 512)
            dwBytesToRead = dwMaxDist - dwOffset;

        if (SSSeekRead(lpstOffcScan->lpstStream,
                       dwOffset,
                       abyWorkBuffer,
                       dwBytesToRead,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != dwBytesToRead)
        {
            // Error reading

            return(EXTSTATUS_OK);
        }

        // Search for the string

        if (XL5ScanBuffer(abyWorkBuffer,
                          dwBytesRead,
                          &lpstOffcScan->u.stXL5.wVID) ==
            EXTSTATUS_VIRUS_FOUND)
            return(EXTSTATUS_VIRUS_FOUND);

        dwOffset += 512 - 32;
    }

    // No virus

    return(EXTSTATUS_OK);
}



