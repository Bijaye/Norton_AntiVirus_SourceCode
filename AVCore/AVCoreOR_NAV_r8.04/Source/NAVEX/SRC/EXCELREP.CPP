//************************************************************************
//
// $Header:   S:/NAVEX/VCS/EXCELREP.CPv   1.24   06 May 1997 13:35:30   DCHI  $
//
// Description:
//      Contains Excel macro repair function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/EXCELREP.CPv  $
// 
//    Rev 1.24   06 May 1997 13:35:30   DCHI
// Added #include "macrovid.h".
// 
//    Rev 1.23   06 May 1997 12:10:54   CFORMUL
// Added repair for Laroux.C
// 
// 
//    Rev 1.22   30 Apr 1997 16:25:52   CFORMUL
// Fixed the repair for Robocop
// 
// 
//    Rev 1.21   09 Apr 1997 18:33:02   DCHI
// Changed ordering of repair operations so that Book module is modified
// first so that failure for encrypted documents causes VXD to report failure.
// 
//    Rev 1.20   18 Mar 1997 16:35:22   DCHI
// Added code to not delete VB_PROJECT record if virus has more than one sheet.
// 
//    Rev 1.19   06 Mar 1997 12:57:02   CFORMUL
// Added XM.Robocop.A
// 
// 
//    Rev 1.18   05 Mar 1997 16:52:52   CFORMUL
// Added XM.Legend
// 
// 
//    Rev 1.17   05 Feb 1997 17:02:28   DCHI
// Re-added if statement to ensure correct deleting of VBA_PROJECT descriptor.
// 
//    Rev 1.16   05 Feb 1997 11:46:42   DCHI
// Added SAM modifications.
// 
//    Rev 1.15   09 Jan 1997 18:44:48   CFORMUL
// Added repair for XM.Delta
// 
// 
//    Rev 1.14   26 Dec 1996 15:21:44   AOONWAL
// No change.
// 
//    Rev 1.13   06 Dec 1996 22:01:40   FBARAJA
// Pulled int 3...ooops :)
// 
//    Rev 1.12   06 Dec 1996 21:12:54   CFORMUL
// Fixed repair for Sofa so it repairs re-infected files... the cheese
// 
// 
//    Rev 1.11   06 Dec 1996 15:22:10   CFORMUL
// Added repair info for XM.Sofa.
// Removed an if statement that verifies that the sheet is a VBA module.
// XM.Sofa uses sheets that aren't VBA modules, so this check had to be
// removed in order to repair it.
// 
// 
//    Rev 1.10   02 Dec 1996 14:01:06   AOONWAL
// No change.
// 
//    Rev 1.9   29 Oct 1996 12:59:42   AOONWAL
// No change.
// 
//    Rev 1.8   06 Sep 1996 14:53:38   DCHI
// Added SYM_VXD to pack changes.
// 
//    Rev 1.7   06 Sep 1996 14:21:04   DCHI
// pack changes
// 
//    Rev 1.6   28 Aug 1996 16:09:20   DCHI
// Endianization and packing additions.
// 
//    Rev 1.5   13 Aug 1996 12:27:34   DCHI
// No more deleting the backup file.  Only hide if the directory is
// XLSTART and the file name has a three letter ASCII extension.
// 
//    Rev 1.4   07 Aug 1996 17:24:40   DCHI
// Added deletion of backup file after successful repair.
// 
//    Rev 1.3   07 Aug 1996 11:12:14   DCHI
// Don't unhide the sheet.
// 
//    Rev 1.2   05 Aug 1996 19:09:12   DCHI
// Correction of when selecting sheet in one sheet case.
// 
//    Rev 1.1   05 Aug 1996 18:35:56   DCHI
// Changed repair to make blanked out sheet visible, etc.
// 
//    Rev 1.0   05 Aug 1996 10:40:02   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef SYM_NLM

#include "platform.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
#include "ctsn.h"
#include "callback.h"
#endif

#include "navex.h"

#include "storobj.h"
#include "olewrite.h"

#include "navexshr.h"
#include "macrovid.h"

#include "endutils.h"

#include "excelshr.h"
#include "excelrep.h"

//
// Book record types
//

#define EXCEL_REC_TYPE_BOF                  0x0809
#define EXCEL_REC_TYPE_EOF                  0x0A
#define EXCEL_REC_TYPE_BOUNDSHEET           0x85
#define EXCEL_REC_TYPE_OBPROJ               0xD3
#define EXCEL_REC_TYPE_WINDOW_INFO          0x3D
#define EXCEL_REC_TYPE_SHEET_INFO           0x3E
#define EXCEL_REC_TYPE_SHEET_INFO_02        0x023E

#define EXCEL_REC_TYPE_NOP                  0x12    // Protection type

//
// WINDOW1: Window Information (3Dh)
//

#define EXCEL_WINDOW_INFO_OPTION_HIDDEN_BIT 0x0001

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct tagEXCEL_WINDOW_INFO_T
{
    WORD        wXPos;
    WORD        wYPos;
    WORD        wWidth;
    WORD        wHeight;
    WORD        wOptionFlags;
    WORD        wTabCur;
    WORD        wTabFirst;
    WORD        wCountTabSel;
    WORD        wTabRatio;
} EXCEL_WINDOW_INFO_T;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

//
// WINDOW2: Sheet Window Information (3Eh)
//

#define EXCEL_SHEET_INFO_OPTION_SELECTED_BIT    0x0200

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct tagEXCEL_SHEET_INFO_T
{
    WORD        wOptionFlags;
    WORD        wTopRow;
    WORD        wLeftCol;
    DWORD       dwRGBHdr;
} EXCEL_SHEET_INFO_T;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

//
// BOUNDSHEET: Sheet Information (85h)
//

#define EXCEL_BOUNDSHEET_TYPE_MASK          0x0F

#define EXCEL_BOUNDSHEET_TYPE_WORKSHEET     0x00
#define EXCEL_BOUNDSHEET_TYPE_VB            0x06

#define EXCEL_BOUNDSHEET_HIDDEN_STATE_MASK  0x03
#define EXCEL_BOUNDSHEET_HIDDEN_VISIBLE     0x00
#define EXCEL_BOUNDSHEET_HIDDEN_HIDDEN      0x01
#define EXCEL_BOUNDSHEET_HIDDEN_VERY_HIDDEN 0x02

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "pshpack1.h"
#else
#pragma PACK1
#endif

typedef struct tagEXCEL_BOUNDSHEET_T
{
    DWORD       dwBOFOffset;
    BYTE        byHiddenState;
    BYTE        bySheetType;
    BYTE        bySheetNameLen;
} EXCEL_BOUNDSHEET_T;

#if defined(SYM_WIN32) || defined(SYM_NTK) || defined(SYM_VXD)
#include "poppack.h"
#else
#pragma PACK
#endif

//
// This byte array contains the binary of a blank macro module
//

BYTE gabyBlankExcelModule[] =
{
    0x01, 0x08, 0x01, 0x00, 0xB6, 0x00, 0xFF, 0xFF,
    0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xFF, 0xFF, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x4E, 0x00, 0x00, 0x00, 0x2B, 0x01, 0x00, 0x00, 
    0xEB, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
    0x03, 0x00, 0x00, 0x02, 0x43, 0x01, 0x00, 0x00, 
    0x4E, 0x00, 0x00, 0x00, 0x4E, 0x00, 0x00, 0x00, 
    0x8D, 0x01, 0x00, 0x00, 0x00, 0x00, 0xDF, 0x00, 
    0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x01, 0x01, 
    0x00, 0x00, 0x00, 0x00, 0xE6, 0x25, 0xF9, 0x31, 
    0x07, 0x00, 0xDF, 0x00, 0x00, 0xFF, 0xFF, 0x01, 
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 
    0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 
    0xFF, 0xFF, 0xFF, 0x4D, 0x45, 0x00, 0x00, 0xFF, 
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0xFF, 0xFF, 0x01, 0x01, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 
    0xCA, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x01, 
    0x01, 0x00, 0x00, 0x00, 0x00,
};

#define BLANK_EXCEL_MODULE_BYTE_SIZE    sizeof(gabyBlankExcelModule)


//*************************************************************************
//
// Function:
//  WORD ExcelBlankOutModule()
//
// Description:
//  Replaces the given stream with the contents of a blank
//  macro module.
//
// Returns:
//  0       On failure
//  1       On success
//
//*************************************************************************

WORD ExcelBlankOutModule
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    LPOLE_FILE_T    lpstOLEFile             // Stream containing module
)
{
    WORD            wBytesWritten;
    WORD            wBytesToWrite;
    DWORD           dwOffset;

    if (lpstOLEFile->siStreamInfo.dwStreamLen < BLANK_EXCEL_MODULE_BYTE_SIZE)
    {
        // Stream size is less than blank module size!

        return 0;
    }

    // Replace the module with a blank module

    wBytesWritten = OLESeekWrite(lpCallBack,
                                 lpstOLEFile,
                                 0,
                                 gabyBlankExcelModule,
                                 BLANK_EXCEL_MODULE_BYTE_SIZE);

    if (wBytesWritten != BLANK_EXCEL_MODULE_BYTE_SIZE)
    {
        // Wrote less than the desired number of bytes

        return 0;
    }

    // Zero out the work buffer

    for (wBytesWritten=0;wBytesWritten<512;wBytesWritten++)
        lpbyWorkBuffer[wBytesWritten] = 0;

    // Fill the rest of the module with zeros

    dwOffset = BLANK_EXCEL_MODULE_BYTE_SIZE;
    while (dwOffset < lpstOLEFile->siStreamInfo.dwStreamLen)
    {
        if (dwOffset + 512 > lpstOLEFile->siStreamInfo.dwStreamLen)
            wBytesToWrite =
                (WORD)(lpstOLEFile->siStreamInfo.dwStreamLen - dwOffset);
        else
            wBytesToWrite = 512;

        wBytesWritten = OLESeekWrite(lpCallBack,
                                     lpstOLEFile,
                                     dwOffset,
                                     lpbyWorkBuffer,
                                     wBytesToWrite);

        if (wBytesWritten != wBytesToWrite)
        {
            // Wrote less than the desired number of bytes

            return 0;
        }

        dwOffset += wBytesWritten;
    }

    return 1;
}


//*************************************************************************
//
// Function:
//  WORD ExcelBlankOutVirusModules()
//
// Description:
//  Looks for all streams with only hexadecimal characters in the
//  name and that are infected with the Excel virus specified
//  by the wVID parameter, replacing each matching stream with
//  the contents of a blank macro module.
//
// Returns:
//  0       On failure
//  > 0     The number of modules virus successfully blanked out
//
//*************************************************************************

WORD ExcelBlankOutVirusModules
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE         	lpbyWorkBuffer,			// Work buffer >= 512 bytes
    WORD            wVID                    // ID of virus
)
{
    WORD            wSigIndex;
    WORD            wMagicNum;
    WORD            wBytesRead;
    WORD            wBytesToRead;
    WORD            wScanIndex;
    WORD            wBlankCount;
    DWORD           dwOffset, dwMaxDist;
    OLE_FILE_T      stOLEFile;              // For storing stream open

    // Blank count is initially zero

    wBlankCount = 0;

    // Find the sig

    wSigIndex = 0;
    while (wSigIndex < gwNumExcelSigs)
    {
        if (gExcelSigs[wSigIndex].wVID == wVID)
            break;

        ++wSigIndex;
    }

    if (wSigIndex == gwNumExcelSigs)
    {
        // No such VID

        return 0;
    }

    // Look for a Hex-Named stream

	wMagicNum = 0;
    while (OLEOpenHexNamedStream(lpCallBack,
                                 hFile,
                                 &stOLEFile,
                                 &wMagicNum) == OLE_OK)
	{
        // Scan for viruses

        // Limit search and read distance

        dwMaxDist = stOLEFile.siStreamInfo.dwStreamLen;
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

            wBytesRead = OLESeekRead(lpCallBack,
                                     &stOLEFile,
                                     dwOffset,
                                     lpbyWorkBuffer,
                                     wBytesToRead);

            if (wBytesRead != wBytesToRead)
            {
                // Read less than the desired number of blocks

                break;
            }

            // Search for the string

            for (wScanIndex = 0; wScanIndex < wBytesRead ; wScanIndex++)
            {
                if (gExcelSigs[wSigIndex].wFirstSigWord ==
                    AVDEREF_WORD(lpbyWorkBuffer + wScanIndex) &&
                    (wScanIndex + gExcelSigs[wSigIndex].wSigLen) <= wBytesRead)
                {
                    WORD    wTemp;
                    WORD    wLen;
                    PWORD   pwSigData;

                    wLen = gExcelSigs[wSigIndex].wSigLen;
                    pwSigData = gExcelSigs[wSigIndex].pwSig;
                    for (wTemp = 2 ; wTemp < wLen ; wTemp++)
                    {
                        if (pwSigData[wTemp] !=
                            lpbyWorkBuffer[wScanIndex + wTemp] &&
                            pwSigData[wTemp] != EXCEL_SIG_WILD_CARD_VALUE)
                            break;
                    }

                    if (wTemp == wLen)
                    {
                        // Found the sucker

                        if (ExcelBlankOutModule(lpCallBack,
                                                lpbyWorkBuffer,
                                                &stOLEFile) != 0)
                        {
                            // Increment blank count

                            ++wBlankCount;
                        }

                        break;
                    }
                }
            }

            dwOffset += 512 - 32;
        }
	}

    return(wBlankCount);
}


//*************************************************************************
//
// Function:
//  WORD OLEStreamZeroOutBytes()
//
// Description:
//  Writes a given number of consecutive zero bytes starting at
//  a given offset in an OLE stream.
//
// Returns:
//  0       On failure
//  1       On success
//
//*************************************************************************

WORD OLEStreamZeroOutBytes
(
    LPCALLBACKREV1      lpCallBack,     // File op callbacks
    LPOLE_FILE_T        lpstOLEFile,    // Stream containing module
    DWORD               dwOffset,       // Starting offset in stream
    WORD                wNumBytes       // # bytes to zero out
)
{
    BYTE byZeroByte = 0;

    // Not fast, but speed isn't that important for repair

    while (wNumBytes-- > 0)
    {
        if (OLESeekWrite(lpCallBack,
                         lpstOLEFile,
                         dwOffset++,
                         &byZeroByte,
                         sizeof(BYTE)) != sizeof(BYTE))
        {
            // Failed write of zero byte

            return 0;
        }
    }

    return 1;
}


//*************************************************************************
//
// Function:
//  WORD ExcelSetTabSelectedState()
//
// Description:
//  Goes through the given Excel Book sheet substream,
//  and sets the tab selected state to the given state.
//
// Returns:
//  0       On failure
//  1       On success
//
//*************************************************************************

WORD ExcelSetTabSelectedState
(
    LPCALLBACKREV1      lpCallBack,     // File op callbacks
    LPOLE_FILE_T        lpstOLEFile,    // The Book stream
    DWORD               dwOffset,       // The offset of the substream
    WORD                wSelected       // Non-zero if selected
)
{
    WORD                wRecordType;
    WORD                wRecordLen;
    WORD                wReturnValue;
    WORD                wDepth;
    EXCEL_SHEET_INFO_T  stSheetInfo;

    // wReturnValue becomes 1 if we see a SHEET_INFO_T structure

    wReturnValue = 0;

    // A worksheet can have charts, so wDepth keeps track of the
    //  current recursive depth level

    wDepth = 0;

    // Set selected fields of all sheet information records
    //  in the substream

    while (dwOffset < lpstOLEFile->siStreamInfo.dwStreamLen)
    {
        // Read a record type and its length

        if (OLESeekRead(lpCallBack,
                        lpstOLEFile,
                        dwOffset,
                        (LPBYTE)&wRecordType,
                        sizeof(WORD)) != sizeof(WORD) ||
            OLESeekRead(lpCallBack,
                        lpstOLEFile,
                        dwOffset + sizeof(WORD),
                        (LPBYTE)&wRecordLen,
                        sizeof(WORD)) != sizeof(WORD))
        {
            return 0;
        }

        // Endianize

        wRecordType = ConvertEndianShort(wRecordType);
        wRecordLen = ConvertEndianShort(wRecordLen);

        if (wRecordType == EXCEL_REC_TYPE_BOF)
        {
            // Increase the depth

            ++wDepth;
        }
        else
        if (wRecordType == EXCEL_REC_TYPE_EOF)
        {
            if (wDepth == 1)
                break;
            else
                --wDepth;
        }
        else
        if (wDepth == 1 &&  // We only want to modify top level sheet info
            wRecordLen >= sizeof(EXCEL_SHEET_INFO_T) &&
            (wRecordType == EXCEL_REC_TYPE_SHEET_INFO ||
             wRecordType == EXCEL_REC_TYPE_SHEET_INFO_02))
        {
            // Read SHEET_INFO structure

            if (OLESeekRead(lpCallBack,
                            lpstOLEFile,
                            dwOffset + sizeof(WORD) + sizeof(WORD),
                            (LPBYTE)&stSheetInfo,
                            sizeof(EXCEL_SHEET_INFO_T)) !=
                sizeof(EXCEL_SHEET_INFO_T))
            {
                // Failed read of SHEET_INFO_T

                return 0;
            }

            // Set the selected state

            // Endianize the option flags

            stSheetInfo.wOptionFlags =
                ConvertEndianShort(stSheetInfo.wOptionFlags);

            if (wSelected == 0)
            {
                // Unselected

                stSheetInfo.wOptionFlags &=
                    ~(EXCEL_SHEET_INFO_OPTION_SELECTED_BIT);
            }
            else
            {
                // Selected

                stSheetInfo.wOptionFlags |=
                    EXCEL_SHEET_INFO_OPTION_SELECTED_BIT;
            }

            // Re-Endianize the option flags

            stSheetInfo.wOptionFlags =
                ConvertEndianShort(stSheetInfo.wOptionFlags);

            // Write back the updated sheet info

            if (OLESeekWrite(lpCallBack,
                             lpstOLEFile,
                             dwOffset + sizeof(WORD) + sizeof(WORD),
                             (LPBYTE)&stSheetInfo,
                             sizeof(EXCEL_SHEET_INFO_T)) !=
                sizeof(EXCEL_SHEET_INFO_T))
            {
                // Failed write of SHEET_INFO_T

                return 0;
            }

            // Indicate success

            wReturnValue = 1;
        }

        dwOffset += sizeof(WORD) + sizeof(WORD) + wRecordLen;
    }

    return(wReturnValue);
}


//*************************************************************************
//
// Function:
//  WORD ExcelSetOneTabSelectedOnly()
//
// Description:
//  Goes through the given Excel Book stream, selecting the tab
//  of the first stream and deselecting all others.
//
// Returns:
//  0       On failure
//  > 0     On success
//
//*************************************************************************

WORD ExcelSetOneTabSelectedOnly
(
    LPCALLBACKREV1      lpCallBack,     // File op callbacks
    LPOLE_FILE_T        lpstOLEFile,    // The Book stream
    WORD                wIndexToSelect  // Index of sheet to select
)
{
    DWORD               dwOffset;
    WORD                wSheetIndex;
    WORD                wRecordType;
    WORD                wRecordLen;
    EXCEL_BOUNDSHEET_T  stBoundSheet;

    dwOffset = 0;
    wSheetIndex = 0;
    while (dwOffset < lpstOLEFile->siStreamInfo.dwStreamLen)
    {
        // Read a record type and its length

        if (OLESeekRead(lpCallBack,
                        lpstOLEFile,
                        dwOffset,
                        (LPBYTE)&wRecordType,
                        sizeof(WORD)) != sizeof(WORD) ||
            OLESeekRead(lpCallBack,
                        lpstOLEFile,
                        dwOffset + sizeof(WORD),
                        (LPBYTE)&wRecordLen,
                        sizeof(WORD)) != sizeof(WORD))
        {
            return 0;
        }

        // Endianize

        wRecordType = ConvertEndianShort(wRecordType);
        wRecordLen = ConvertEndianShort(wRecordLen);

        // Is it the EOF record

        if (wRecordType == EXCEL_REC_TYPE_EOF)
            break;

        if (wRecordLen >= sizeof(EXCEL_BOUNDSHEET_T) &&
            wRecordType == EXCEL_REC_TYPE_BOUNDSHEET)
        {
            // Read BOUNDSHEET structure

            if (OLESeekRead(lpCallBack,
                            lpstOLEFile,
                            dwOffset + sizeof(WORD) + sizeof(WORD),
                            (LPBYTE)&stBoundSheet,
                            sizeof(EXCEL_BOUNDSHEET_T)) !=
                sizeof(EXCEL_BOUNDSHEET_T))
            {
                // Failed read of BOUNDSHEET_T

                return 0;
            }

            if (wSheetIndex++ == wIndexToSelect)
            {
                if (ExcelSetTabSelectedState(lpCallBack,
                                             lpstOLEFile,
                                             ConvertEndianLong(stBoundSheet.dwBOFOffset),
                                             1) == 0)
                {
                    // Failed to set tab as selected

                    return 0;
                }
            }
            else
            {
                if (ExcelSetTabSelectedState(lpCallBack,
                                             lpstOLEFile,
                                             ConvertEndianLong(stBoundSheet.dwBOFOffset),
                                             0) == 0)
                {
                    // Failed to set tab as deselected

                    return 0;
                }
            }
        }

        dwOffset += sizeof(WORD) + sizeof(WORD) + wRecordLen;
    }

    return(wSheetIndex);
}


//*************************************************************************
//
// Function:
//  WORD ExcelFindFirstVisibleNonNAVSheetIndex()
//
// Description:
//  Finds the first sheet substream with a name other than
//  a NAV name.  If none exists, then it returns index 0.
//
// Returns:
//  0xFFFF  On failure
//  n       The first sheet with a non-NAV name, or the first
//            NAV index if no non-NAV ones exist
//
//*************************************************************************

WORD ExcelFindFirstVisibleNonNAVSheetIndex
(
    LPCALLBACKREV1      lpCallBack,     // File op callbacks
    LPOLE_FILE_T        lpstOLEFile,    // The Book stream
    LPBYTE				lpbyWorkBuffer	// Work buffer >= 512 bytes
)
{
    WORD                wCurIndex;
    WORD                wRecordType;
    WORD                wRecordLen;
    DWORD               dwOffset;       // The offset of the substream
    EXCEL_BOUNDSHEET_T  stBoundSheet;

    wCurIndex = 0;
    dwOffset = 0;
    while (dwOffset < lpstOLEFile->siStreamInfo.dwStreamLen)
    {
        // Read a record type and its length

        if (OLESeekRead(lpCallBack,
                        lpstOLEFile,
                        dwOffset,
                        (LPBYTE)&wRecordType,
                        sizeof(WORD)) != sizeof(WORD) ||
            OLESeekRead(lpCallBack,
                        lpstOLEFile,
                        dwOffset + sizeof(WORD),
                        (LPBYTE)&wRecordLen,
                        sizeof(WORD)) != sizeof(WORD))
        {
            return(0xFFFF);
        }

        // Endianize

        wRecordType = ConvertEndianShort(wRecordType);
        wRecordLen = ConvertEndianShort(wRecordLen);

        if (wRecordType == EXCEL_REC_TYPE_EOF)
            break;

        // If it is a BOUNDSHEET and the length of the name
        //  is greater than 3

        if (wRecordType == EXCEL_REC_TYPE_BOUNDSHEET)
        {
            // Read BOUNDSHEET structure

            if (OLESeekRead(lpCallBack,
                            lpstOLEFile,
                            dwOffset + sizeof(WORD) + sizeof(WORD),
                            (LPBYTE)&stBoundSheet,
                            sizeof(EXCEL_BOUNDSHEET_T)) !=
                sizeof(EXCEL_BOUNDSHEET_T))
            {
                // Failed read of BOUNDSHEET_T

                return(0xFFFF);
            }

            // Read the sheet name

            if (OLESeekRead(lpCallBack,
                            lpstOLEFile,
                            dwOffset + sizeof(WORD) + sizeof(WORD) +
                                sizeof(EXCEL_BOUNDSHEET_T),
                            lpbyWorkBuffer,
                            stBoundSheet.bySheetNameLen) !=
                stBoundSheet.bySheetNameLen)
            {
                // Failed read of sheet name

                return(0xFFFF);
            }

            // Is it NAV?

            if (stBoundSheet.bySheetNameLen > 3 &&
                lpbyWorkBuffer[0] == 'N' &&
                lpbyWorkBuffer[1] == 'A' &&
                lpbyWorkBuffer[2] == 'V' &&
                '0' <= lpbyWorkBuffer[3] && lpbyWorkBuffer[3] <= '9')
            {
                // This is a NAV name

                ++wCurIndex;
            }
            else
            if ((stBoundSheet.byHiddenState &
                 EXCEL_BOUNDSHEET_HIDDEN_STATE_MASK) ==
                EXCEL_BOUNDSHEET_HIDDEN_VISIBLE)
            {
                // Found a non-NAV visible sheet name

                return(wCurIndex);
            }
        }

        dwOffset += sizeof(WORD) + sizeof(WORD) + wRecordLen;
    }

    return(0);
}


//*************************************************************************
//
// Function:
//  WORD ExcelGetNextNAVTabIndex()
//
// Description:
//  Goes through the given Excel Book sheet main substream,
//  and returns the next index to use for the NAV tab.
//
// Returns:
//  0xFFFF  On failure
//  n       The next tab index to use
//
//*************************************************************************

WORD ExcelGetNextNAVTabIndex
(
    LPCALLBACKREV1      lpCallBack,     // File op callbacks
    LPOLE_FILE_T        lpstOLEFile,    // The Book stream
    LPBYTE              lpbyWorkBuffer  // Work buffer >= 512 bytes
)
{
    WORD                wNextIndex;
    WORD                wRecordType;
    WORD                wRecordLen;
    DWORD               dwOffset;       // The offset of the substream
    EXCEL_BOUNDSHEET_T  stBoundSheet;

    wNextIndex = 0;
    dwOffset = 0;
    while (dwOffset < lpstOLEFile->siStreamInfo.dwStreamLen)
    {
        // Read a record type and its length

        if (OLESeekRead(lpCallBack,
                        lpstOLEFile,
                        dwOffset,
                        (LPBYTE)&wRecordType,
                        sizeof(WORD)) != sizeof(WORD) ||
            OLESeekRead(lpCallBack,
                        lpstOLEFile,
                        dwOffset + sizeof(WORD),
                        (LPBYTE)&wRecordLen,
                        sizeof(WORD)) != sizeof(WORD))
        {
            return(0xFFFF);
        }

        // Endianize

        wRecordType = ConvertEndianShort(wRecordType);
        wRecordLen = ConvertEndianShort(wRecordLen);

        if (wRecordType == EXCEL_REC_TYPE_EOF)
            break;

        // If it is a BOUNDSHEET and the length of the name
        //  is greater than 3

        if (wRecordType == EXCEL_REC_TYPE_BOUNDSHEET &&
            wRecordLen >= (sizeof(EXCEL_BOUNDSHEET_T) + 3))
        {
            // Read BOUNDSHEET structure

            if (OLESeekRead(lpCallBack,
                            lpstOLEFile,
                            dwOffset + sizeof(WORD) + sizeof(WORD),
                            (LPBYTE)&stBoundSheet,
                            sizeof(EXCEL_BOUNDSHEET_T)) !=
                sizeof(EXCEL_BOUNDSHEET_T))
            {
                // Failed read of BOUNDSHEET_T

                return(0xFFFF);
            }

            // Read the sheet name

            if (OLESeekRead(lpCallBack,
                            lpstOLEFile,
                            dwOffset + sizeof(WORD) + sizeof(WORD) +
                                sizeof(EXCEL_BOUNDSHEET_T),
                            lpbyWorkBuffer,
                            stBoundSheet.bySheetNameLen) !=
                stBoundSheet.bySheetNameLen)
            {
                // Failed read of sheet name

                return(0xFFFF);
            }

            // Is it NAV?

            if (lpbyWorkBuffer[0] == 'N' &&
                lpbyWorkBuffer[1] == 'A' &&
                lpbyWorkBuffer[2] == 'V' &&
                '0' <= lpbyWorkBuffer[3] && lpbyWorkBuffer[3] <= '9')
            {
                int i;
                WORD wIndex;

                // Check the number

                wIndex = 0;
                for (i=3;i<stBoundSheet.bySheetNameLen;i++)
                {
                    if ('0' <= lpbyWorkBuffer[i] && lpbyWorkBuffer[i] <= '9')
                    {
                        wIndex = wIndex * 10 +
                            lpbyWorkBuffer[i] - '0';
                    }
                    else
                    {
                        // End of number

                        break;
                    }
                }

                if (wIndex >= 0xFFFE)
                {
                    // File infected 0xFFFE or more times?

                    return(0xFFFF);
                }

                if (wIndex >= wNextIndex)
                    wNextIndex = wIndex + 1;
            }
        }

        dwOffset += sizeof(WORD) + sizeof(WORD) + wRecordLen;
    }

    return(wNextIndex);
}


//*************************************************************************
//
// Function:
//  WORD ExcelBooksRemoveModule()
//
// Description:
//  Looks for all streams named "Book".  For each such stream,
//  the function looks for a sheet with the name lppsModuleName.
//  The module name for the sheet is replaced with NAV### where
//  ### is a number that makes the NAV### unique in the sheet
//  namespace.  The BOUNDSHEET record for this sheet is modified
//  so that the sheet is a normal worksheet and is visible.
//
//  If the book contains only one VB project module (the viral one),
//  then the OBPROJ record is set to NOP and zeroed out because there
//  are no more modules after we delete the viral one.  Otherwise, the
//  OBPROJ record is left alone.
//
//  If the book contains only one sheet (the viral one), the rest
//  of the fields after the option flags field in the sheet's substream
//  SHEET WINDOW INFORMATION record are zeroed out.  The option
//  flags field is modified so that the sheet's tab is selected.
//
//  The WINDOW INFORMATION record is modified so that the fHidden bit
//  field is set to 0 (unhidden).  In addition the following fields
//  of the record are also modified:
//      - itabCur = n       Select the first non-NAV### named workbook tab
//      - itabFirst = n     First visible workbook tab
//      - ctabSel = 1       Number of workbook tabs selected
//
//  The last task is to select the first non-NAV### sheet tab and
//  deselecting all other tabs by modifiying the SHEET WINDOW INFORMATION
//  for each sheet substream.
//
// Returns:
//  0       On failure
//  > 0     The number of books successfully modified
//
//*************************************************************************

BYTE gabyBookEntry[] = { 'B', 0, 'o', 0, 'o', 0, 'k', 0, 0, 0 };

BYTE gszVBAProject[] = "_VBA_PROJECT";

#define EXCEL_OBPROJ_VBA_PROJECT_RECLEN     (sizeof(gszVBAProject)-1)

WORD ExcelBooksRemoveModule
(
    LPCALLBACKREV1      lpCallBack,             // File op callbacks
    HFILE               hFile,                  // Handle to file to scan
    LPBYTE              lpbyWorkBuffer,         // Work buffer >= 1024 bytes
    LPBYTE              lppsModuleName,         // Pascal string of module
    BOOL                bSingleSheetCase        // Single sheet case?
)
{
    WORD                wMagicNum;
    WORD                wRecordType;
    WORD                wRecordLen;
    WORD                wDeleteCount;

    DWORD               dwWindowInfoRecordOffset;
    WORD                wWindowInfoRecordLen;

    DWORD               dwViralModuleSectionOffset;

    DWORD               dwViralSheetRecordOffset;

    DWORD               dwObProjRecordOffset;
    WORD                wObProjRecordLen;

    DWORD               dwTotalSheetCount;
    DWORD               dwVBModuleSheetCount;

    OLE_FILE_T          stOLEFile;              // For storing stream open
    DWORD               dwOffset;
    int                 i;
    EXCEL_BOUNDSHEET_T  stBoundSheet;
    BYTE                byInfectedSheetType;
    LPBYTE              lpbyStreamBATCache;

    lpbyStreamBATCache = lpbyWorkBuffer;
    lpbyWorkBuffer += 512;

    // Set delete count to zero

    wDeleteCount = 0;

    // Look for all Book streams

	wMagicNum = 0;
    while (OLEOpenStream(lpCallBack,
                         hFile,
                         &stOLEFile,
                         (LPWORD)gabyBookEntry,
                         lpbyStreamBATCache,
                         &wMagicNum) == OLE_OK)
    {
        // Set total number of sheets and number of module sheets seen to zero

        dwTotalSheetCount = dwVBModuleSheetCount = 0;

        // Set important record offsets to zero

        dwWindowInfoRecordOffset = 0;
        dwViralSheetRecordOffset = 0;
        dwObProjRecordOffset = 0;

        // Start record parsing

        dwOffset = 0;
        while (dwOffset < stOLEFile.siStreamInfo.dwStreamLen)
        {
            // Read a record type and its length

            if (OLESeekRead(lpCallBack,
                            &stOLEFile,
                            dwOffset,
                            (LPBYTE)&wRecordType,
                            sizeof(WORD)) != sizeof(WORD) ||
                OLESeekRead(lpCallBack,
                            &stOLEFile,
                            dwOffset + sizeof(WORD),
                            (LPBYTE)&wRecordLen,
                            sizeof(WORD)) != sizeof(WORD))
            {
                return 0;
            }

            // Endianize

            wRecordType = ConvertEndianShort(wRecordType);
            wRecordLen = ConvertEndianShort(wRecordLen);

            // Is it the EOF record

            if (wRecordType == EXCEL_REC_TYPE_EOF)
                break;

            if (wRecordType == EXCEL_REC_TYPE_OBPROJ &&
                wRecordLen == EXCEL_OBPROJ_VBA_PROJECT_RECLEN)
            {
                if (OLESeekRead(lpCallBack,
                                &stOLEFile,
                                dwOffset + sizeof(WORD) + sizeof(WORD),
                                lpbyWorkBuffer,
                                EXCEL_OBPROJ_VBA_PROJECT_RECLEN) !=
                    EXCEL_OBPROJ_VBA_PROJECT_RECLEN)
                {
                    // Failed read of _VBA_PROJECT string

                    return 0;
                }

                for (i=0;i<EXCEL_OBPROJ_VBA_PROJECT_RECLEN;i++)
                    if (gszVBAProject[i] != lpbyWorkBuffer[i])
                        break;

                if (i == EXCEL_OBPROJ_VBA_PROJECT_RECLEN)
                {
                    // Note the location of the _VBA_PROJECT record

                    dwObProjRecordOffset = dwOffset;
                    wObProjRecordLen = wRecordLen;
                }
            }
            else
            if (wRecordType == EXCEL_REC_TYPE_BOUNDSHEET)
            {
                ++dwTotalSheetCount;

                // Read BOUNDSHEET structure

                if (OLESeekRead(lpCallBack,
                                &stOLEFile,
                                dwOffset+sizeof(WORD)+sizeof(WORD),
                                (LPBYTE)&stBoundSheet,
                                sizeof(EXCEL_BOUNDSHEET_T)) !=
                    sizeof(EXCEL_BOUNDSHEET_T))
                {
                    // Failed read of BOUNDSHEET_T

                    return 0;
                }

                // Is it a Visual Basic module?

                if ((stBoundSheet.bySheetType & EXCEL_BOUNDSHEET_TYPE_MASK) ==
                    EXCEL_BOUNDSHEET_TYPE_VB)
                    ++dwVBModuleSheetCount;

                // Read the Pascal string

                if (OLESeekRead(lpCallBack,
                                &stOLEFile,
                                dwOffset + sizeof(WORD) + sizeof(WORD) +
                                    sizeof(EXCEL_BOUNDSHEET_T),
                                lpbyWorkBuffer,
                                stBoundSheet.bySheetNameLen) !=
                    stBoundSheet.bySheetNameLen)
                {
                    // Failed read

                    return 0;
                }

                // Is it the viral sheet?

                if (lppsModuleName[0] == stBoundSheet.bySheetNameLen)
                {
                    for (i=0;i<lppsModuleName[0];i++)
                        if (lpbyWorkBuffer[i] != lppsModuleName[i+1])
                            break;

                    if (i == lppsModuleName[0])
                    {
                        WORD    wNAVIndex;
                        WORD    wHighestIndex;

                        wNAVIndex = ExcelGetNextNAVTabIndex(lpCallBack,
                            &stOLEFile,lpbyWorkBuffer);

                        if (wNAVIndex == 0xFFFF)
                        {
                            // Failed to get valid NAV index

                            return 0;
                        }

                        // Start with "NAV"

                        lpbyWorkBuffer[0] = 'N';
                        lpbyWorkBuffer[1] = 'A';
                        lpbyWorkBuffer[2] = 'V';

                        // Zeroes for the name

                        for (i=3;i<lppsModuleName[0];i++)
                            lpbyWorkBuffer[i] = '0';

                        // How much space do we have for a number?

                        switch (lppsModuleName[0])
                        {
                            case 4:
                                wHighestIndex = 9;
                                break;
                            case 5:
                                wHighestIndex = 99;
                                break;
                            case 6:
                                wHighestIndex = 999;
                                break;
                            case 7:
                                wHighestIndex = 9999;
                                break;
                            default:
                                wHighestIndex = 65535;
                                break;
                        }

                        // Enough space?

                        if (wNAVIndex > wHighestIndex)
                        {
                            // Exceeded range

                            return 0;
                        }

                        // Fill in the zero-filled number

                        for (i=lppsModuleName[0]-1;i>2;i--)
                        {
                            lpbyWorkBuffer[i] = '0' +
                                (BYTE)(wNAVIndex % 10);

                            wNAVIndex /= 10;
                        }

                        // Write out the replacement name

                        if (OLESeekWrite(lpCallBack,
                                         &stOLEFile,
                                         dwOffset +
                                            sizeof(WORD) + sizeof(WORD) +
                                            sizeof(EXCEL_BOUNDSHEET_T),
                                         lpbyWorkBuffer,
                                         lppsModuleName[0]) !=
                            lppsModuleName[0])
                        {
                            // Failed write

                            return 0;
                        }

///////////////////////////////////////////////////////////////////////////
//                          // Unhide the sheet
//
//                          stBoundSheet.byHiddenState =
//                              (stBoundSheet.byHiddenState &
//                               ~(EXCEL_BOUNDSHEET_HIDDEN_STATE_MASK)) |
//                              EXCEL_BOUNDSHEET_HIDDEN_VISIBLE;
///////////////////////////////////////////////////////////////////////////

                        if (OLESeekWrite(lpCallBack,
                                         &stOLEFile,
                                         dwOffset +
                                             sizeof(WORD) + sizeof(WORD),
                                         (LPBYTE)&stBoundSheet,
                                         sizeof(EXCEL_BOUNDSHEET_T)) !=
                            sizeof(EXCEL_BOUNDSHEET_T))
                        {
                            // Failed write of visible state

                            return 0;
                        }

                        // Success

                        dwViralSheetRecordOffset = dwOffset;
                        byInfectedSheetType = stBoundSheet.bySheetType;
                        dwViralModuleSectionOffset =
                            ConvertEndianLong(stBoundSheet.dwBOFOffset);
                    }
                }
            }
            else
            if (wRecordType == EXCEL_REC_TYPE_WINDOW_INFO)
            {
                // Remember where this record is

                dwWindowInfoRecordOffset = dwOffset;
                wWindowInfoRecordLen = wRecordLen;
            }

            dwOffset += sizeof(WORD) + sizeof(WORD) + wRecordLen;
        }

        // Only if there exists the named viral sheet and there
        //  is more than one sheet in the book do we eliminate
        //  this sheet, otherwise Excel crashes on opening it.

        if (dwViralSheetRecordOffset > 0)
        {
            // Set the sheet type to worksheet from module
            //  Excel needs at least one sheet in the document

            byInfectedSheetType =
                (byInfectedSheetType & (~EXCEL_BOUNDSHEET_TYPE_MASK)) |
                EXCEL_BOUNDSHEET_TYPE_WORKSHEET;

            if (OLESeekWrite(lpCallBack,
                             &stOLEFile,
                             dwViralSheetRecordOffset +
                                sizeof(WORD) +      // record type field
                                sizeof(WORD) +      // record length field
                                sizeof(DWORD) +     // BOF offset
                                sizeof(BYTE),       // Hide status
                             (LPBYTE)&byInfectedSheetType,
                             sizeof(BYTE)) != sizeof(BYTE))
            {
                // Failed write of worksheet type to BOUNDSHEET record

                return 0;
            }

            // Set the record type of record containing _VBA_PROJECT string
            //  to zero if there is only one VB module

            if (dwVBModuleSheetCount == 1 && dwObProjRecordOffset != 0 &&
                bSingleSheetCase == TRUE)
            {
                // Set the record type to zero

                wRecordType = ConvertEndianShort(EXCEL_REC_TYPE_NOP);
                if (OLESeekWrite(lpCallBack,
                                 &stOLEFile,
                                 dwObProjRecordOffset,
                                 (LPBYTE)&wRecordType,
                                 sizeof(WORD)) != sizeof(WORD))
                {
                    // Failed write of 0 record type to BOUNDSHEET

                    return 0;
                }

                // Zero out the record

                if (OLEStreamZeroOutBytes(lpCallBack,
                                          &stOLEFile,
                                          dwObProjRecordOffset +
                                            sizeof(WORD) + sizeof(WORD),
                                          wObProjRecordLen) == 0)
                {
                    // Failed to zero out record

                    return 0;
                }
            }

            // Modify the SHEET INFO record

            dwOffset = dwViralModuleSectionOffset;
            while (dwOffset < stOLEFile.siStreamInfo.dwStreamLen)
            {
                // Read a record type and its length

                if (OLESeekRead(lpCallBack,
                                &stOLEFile,
                                dwOffset,
                                (LPBYTE)&wRecordType,
                                sizeof(WORD)) != sizeof(WORD) ||
                    OLESeekRead(lpCallBack,
                                &stOLEFile,
                                dwOffset + sizeof(WORD),
                                (LPBYTE)&wRecordLen,
                                sizeof(WORD)) != sizeof(WORD))
                {
                    return 0;
                }

                // Endianize

                wRecordType = ConvertEndianShort(wRecordType);
                wRecordLen = ConvertEndianShort(wRecordLen);

                // Make sure the first record is BOF

                if (dwOffset == dwViralModuleSectionOffset)
                {
                    if (wRecordType != EXCEL_REC_TYPE_BOF)
                    {
                        // The first record was not BOF!

                        return 0;
                    }
                }
                else
                if (wRecordType == EXCEL_REC_TYPE_BOF)
                {
                    // Saw another BOF before EOF!

                    return 0;
                }
                else
                if (wRecordType == EXCEL_REC_TYPE_EOF)
                {
                    // EOF! success

                    break;
                }
                else
                if (wRecordType == EXCEL_REC_TYPE_SHEET_INFO ||
                    wRecordType == EXCEL_REC_TYPE_SHEET_INFO_02)
                {
                    // Low byte: Set the sheet to display
                    //  - 0x02 gridlines
                    //  - 0x04 row and column headings
                    //  - 0x10 zero values
                    //  - 0x20 default header
                    //  - 0x80 display outline symbols
                    //  = 0xB6
                    //
                    // High byte: Set the sheet to
                    //  - 0x02 tab selected
                    //  - 0x04 displayed in the window
                    //  = 0x06

                    WORD wOptionFlags = ConvertEndianShort(0x06B6);

                    if (OLESeekWrite(lpCallBack,
                                     &stOLEFile,
                                     dwOffset + sizeof(WORD) + sizeof(WORD),
                                     (LPBYTE)&wOptionFlags,
                                     sizeof(WORD)) != sizeof(WORD))
                    {
                        // Failed write of option bytes to SHEET_INFO

                        return 0;
                    }

                    // Zero out the rest of the bytes in this record
                    //  e.g., Topmost row visible = 0,
                    //        Leftmost column visible = 0,
                    //        etc.

                    if (wRecordLen > sizeof(WORD))
                    {
                        // And then zero out the rest of the bytes

                        if (OLEStreamZeroOutBytes(lpCallBack,
                            &stOLEFile,
                            dwOffset + sizeof(WORD) + sizeof(WORD) +
                                sizeof(WORD),   // wOptionFlags
                            wRecordLen - sizeof(WORD)) == 0)
                        {
                            // Failed to zero out record

                            return 0;
                        }
                    }
                }

                dwOffset += sizeof(WORD) + sizeof(WORD) + wRecordLen;
            }

            if (dwWindowInfoRecordOffset > 0 &&
                wWindowInfoRecordLen >= sizeof(EXCEL_WINDOW_INFO_T))
            {
                EXCEL_WINDOW_INFO_T stWindowInfo;
                WORD                wFirstNonNAVSheetIndex;

                // Read the Window Information record

                if (OLESeekRead(lpCallBack,
                                &stOLEFile,
                                dwWindowInfoRecordOffset +
                                    sizeof(WORD) + sizeof(WORD),
                                (LPBYTE)&stWindowInfo,
                                sizeof(EXCEL_WINDOW_INFO_T)) !=
                    sizeof(EXCEL_WINDOW_INFO_T))
                {
                    // Failed read of Window Information record

                    return 0;
                }

                // Find the first non-NAV repaired sheet

                wFirstNonNAVSheetIndex =
                    ExcelFindFirstVisibleNonNAVSheetIndex(lpCallBack,
                                                          &stOLEFile,
                                                          lpbyWorkBuffer);

                if (wFirstNonNAVSheetIndex == 0xFFFF)
                {
                    // Failed

                    return 0;
                }

                // Unhide, set the first visible and selected tab
                //  to the first non-viral sheet, and set the
                //  number of selected tabs to one

///////////////////////////////////////////////////////////////////////////
//              stWindowInfo.wOptionFlags &=
//                  ~(EXCEL_WINDOW_INFO_OPTION_HIDDEN_BIT);
///////////////////////////////////////////////////////////////////////////

                stWindowInfo.wTabCur =
                    ConvertEndianShort(wFirstNonNAVSheetIndex);
                stWindowInfo.wTabFirst =
                    ConvertEndianShort(wFirstNonNAVSheetIndex);
                stWindowInfo.wCountTabSel = ConvertEndianShort(1);

                // Write out the modified Window Information record

                if (OLESeekWrite(lpCallBack,
                                 &stOLEFile,
                                 dwWindowInfoRecordOffset +
                                     sizeof(WORD) + sizeof(WORD),
                                 (LPBYTE)&stWindowInfo,
                                 sizeof(EXCEL_WINDOW_INFO_T)) !=
                    sizeof(EXCEL_WINDOW_INFO_T))
                {
                    // Failed write of Window Information record

                    return 0;
                }

                // Select the first sheet tab and deselect all others

                if (ExcelSetOneTabSelectedOnly(lpCallBack,
                                               &stOLEFile,
                                               wFirstNonNAVSheetIndex) == 0)
                {
                    // Failed to select only the first tab

                    return 0;
                }
            }
            else
            {
                // No valid Window Information record!?!

                return 0;
            }

            // Increment delete count

            ++wDeleteCount;
        }
	}

    return(wDeleteCount);
}


//*************************************************************************
//
// Function:
//  WORD ExcelTStringNCmpI()
//
// Description:
//  Compares the two strings up to N characters with case insensitivity.
//
// Returns:
//  0       If both strings are the same up to N characters
//  1       if the strings differ
//
//*************************************************************************

WORD ExcelTStringNCmpI
(
    LPTSTR          lptstr0,
    LPTSTR          lptstr1,
    int             n
)
{
    TCHAR           tch0, tch1;

    tch0 = *lptstr0;
    tch1 = *lptstr1;
    while (tch0 && tch1 && n > 0)
    {
        // Convert to upper case

        if ('a' <= tch0 && tch0 <= 'z')
            tch0 = (tch0 - 'a') + 'A';

        if ('a' <= tch1 && tch1 <= 'z')
            tch1 = (tch1 - 'a') + 'A';

        if (tch0 != tch1)
            return 1;

        tch0 = *++lptstr0;
        tch1 = *++lptstr1;
        --n;
    }

    if (n == 0)
    {
        // Equal

        return 0;
    }

    // Unequal

    return 1;
}

#ifdef __MACINTOSH__

// prototypes
Boolean ExcelMoveStartupCopy( HFILE fileRef );
Boolean EqualSubstr( uchar *str1, uchar *str2, uchar len );


//*************************************************************************
//
// Function:
//  WORD ExcelMoveStartupCopy()
//
// Description:
//  Tries to move any backup copy of infected file out of Excel Startup folder
//
// Returns:
//  false       On failure
//  true        On success or no backup found moveable
//
//*************************************************************************

Boolean ExcelMoveStartupCopy( HFILE fileRef )
{

	FCBPBRec	FCBInfo;
	Str31		repFileName;
	Boolean 	fileOK = true;
	uchar		startupFolderName[] = "\pExcel Startup Folder"; //make resource for int'l?
	
	// get file info for repaired file

	FCBInfo.ioFCBIndx = 0;  // info for this file
	FCBInfo.ioRefNum = fileRef;
	FCBInfo.ioNamePtr = repFileName;
	
	if (PBGetFCBInfoSync( &FCBInfo ) == noErr)
		{
		OSErr		err;
		CInfoPBRec	cpb;
		Str31		name;
		long		dirID = FCBInfo.ioFCBParID;
		short		vRef = FCBInfo.ioFCBVRefNum;
		short		repNameLen = *repFileName;
		
		// get the directory name
		name[0] = 0;
		cpb.hFileInfo.ioCompletion = nil;
		cpb.dirInfo.ioNamePtr = name;
		cpb.dirInfo.ioVRefNum = vRef;
		cpb.dirInfo.ioDrDirID =  dirID;
		cpb.hFileInfo.ioFDirIndex = -1;  // get info on dir		
		err = PBGetCatInfoSync( &cpb );
		
		if (err == noErr)
			{
			// if directory name starts with "Excel Startup Folder" 
			if (*name >= *startupFolderName && EqualSubstr( name+1, startupFolderName+1, *startupFolderName ))
				{
				uchar	ellipsis = (FontScript() == smJapanese) ? 0xFF : 'É';
				short 	dirIndex = 1;
				
				// walk the directory
				do
					{
					name[0] = 0;
					cpb.hFileInfo.ioCompletion = nil;
					cpb.hFileInfo.ioNamePtr = name;
					cpb.hFileInfo.ioVRefNum = vRef;
					cpb.hFileInfo.ioFDirIndex = dirIndex;
					cpb.hFileInfo.ioDirID = dirID;
					
					err = PBGetCatInfoSync( &cpb );
					
					if ( err == noErr )
						{
						// if not a directory
						if ( (cpb.hFileInfo.ioFlAttrib & ioDirMask) == 0 )
							{
							short 	nameLen = *name;
							Boolean	needMove = false;
							
							// if end of file name is same as repaired file's, move it
							if ( (repNameLen < nameLen) && 
									EqualSubstr( repFileName+1, name + nameLen - repNameLen + 1, repNameLen ))
								needMove = true;
							
							// also move if the file name was ellipsized (copy name too long)
							//  and it's not the same as the repaired one
							else if ( (nameLen == 31) && (name[nameLen] == ellipsis)		
									 && ( (repNameLen != 31) || !EqualSubstr( repFileName+1, name+1, 31 ) ))
								needMove = true;
								
							if (needMove)									
								{
								OSErr 		moveErr;
								CMovePBRec	pb;

								// attempt to move file to root dir of this drive
								
								pb.ioCompletion = nil;
								pb.ioNamePtr = name;
								pb.ioVRefNum = vRef;
								pb.ioDirID = dirID;
								pb.ioNewDirID = fsRtDirID;
								pb.ioNewName = nil;
								
								moveErr = PBCatMoveSync( &pb );
								
								// flag any failure but continue attempt to move others
								if (moveErr != noErr)
									fileOK = false;  
								else
									// decrement index so we don't skip next file (move decrements # files in dir)
									dirIndex--;
								}
							}
							
						dirIndex++;
						}
							
					} while ( err == noErr );
									
				}	
			}
		else // can't get dir name
			fileOK = false;
		}
	else
		fileOK = false;

	return fileOK;
}

// compare two memory blocks for a specified length
Boolean EqualSubstr( uchar *str1, uchar *str2, uchar len )
{
	short index;
	
	for ( index = 0; index < len; index++ )
		if (str1[index] != str2[index])
			break;
			
	return (index >= len);
}

#else

//*************************************************************************
//
// Function:
//  void ExcelTryHideName()
//
// Description:
//  Tries a candidate name to hide.
//
// Returns:
//  Nothing.
//
//*************************************************************************

void ExcelTryHideName
(
    LPCALLBACKREV1  lpCallBack,         // File op callbacks
    LPTSTR          lptstrFileName,     // Infected file's name
    LPTSTR          lptstrHideName,     // Candidate name buffer
    int             nStrLen             // Length of name
)
{
    if (ExcelTStringNCmpI(lptstrFileName,
                          lptstrHideName,
                          nStrLen) == 0)
    {
        // Strings are equal, can't use it

        return;
    }

    // Try changing attributes to hidden

    lpCallBack->FileSetAttr(lptstrHideName,2);

    return;
}


//*************************************************************************
//
// Function:
//  WORD ExcelXLStartBackup()
//
// Description:
//  Tries to hide the NAV backup of the specified file if the
//  directory it is in is XLSTART
//
// Returns:
//  0       On failure
//  1       On success or no backup found hideable
//
//*************************************************************************

TCHAR gtchXLSTART[] = { 'X', 'L', 'S', 'T', 'A', 'R', 'T' };

WORD ExcelHideXLStartBackup
(
    LPCALLBACKREV1  lpCallBack,         // File op callbacks
    LPTSTR          lpszFileName,       // Infected file's name
    LPBYTE          lpbyWorkBuffer      // Work buffer >= 2048 bytes
)
{
    int             nStrLen;
    int             i;
    LPTSTR          lptstrCur;
    LPTSTR          lptstrEnd;
    LPBYTE          lpby;

    if (lpszFileName == NULL || *lpszFileName == 0)
    {
        // Don't know what to hide

        return 0;
    }

    //////////////////////////////////////////////////////////////////
    // Look for XLStart
    //////////////////////////////////////////////////////////////////

    lptstrCur = lpszFileName;
    while (*lptstrCur != 0)
    {
        if (ExcelTStringNCmpI(lptstrCur,gtchXLSTART,7) == 0)
            break;

        ++lptstrCur;
    }

    if (*lptstrCur == 0)
    {
        // Not XLSTART

        return 1;
    }

    //////////////////////////////////////////////////////////////////
    // Find the location of the extension and set up buffer
    //  with filename to hide
    //////////////////////////////////////////////////////////////////

    // Find the length of the string

    nStrLen = 0;
    lptstrCur = lpszFileName;
    while (lptstrCur[nStrLen] != 0)
        ++nStrLen;

    // Don't do it if the extension is not a three character
    //  ASCII extension or if the length is greater than 1024 chars

    if (nStrLen < 4)
        return 1;

    lptstrEnd = lpszFileName + nStrLen - 4;

    if (lptstrEnd[0] != '.' || 
        lptstrEnd[1] < 1 || lptstrEnd[1] > 127 ||
        lptstrEnd[2] < 1 || lptstrEnd[2] > 127 || 
        lptstrEnd[3] < 1 || lptstrEnd[3] > 127 ||
        ((nStrLen + 1) * sizeof(TCHAR)) > 1024)
        return 1;

    // Make a copy of the filename

    lpby = (LPBYTE)lpszFileName;
    for (i=0;i<=(nStrLen*sizeof(TCHAR));i++)
        lpbyWorkBuffer[i] = lpby[i];

    // Point to the character after the period

    lptstrEnd = (LPTSTR)(lpbyWorkBuffer +
        ((LPBYTE)lptstrEnd - (LPBYTE)lpszFileName)) + 1;

    //////////////////////////////////////////////////////////////////
    // Try to hide all VIR and VI{0-9} extensions
    //////////////////////////////////////////////////////////////////

    // Initialize with .VIR and try it

    lptstrEnd[0] = 'V';
    lptstrEnd[1] = 'I';
    lptstrEnd[2] = 'R';

    ExcelTryHideName(lpCallBack,
                     lpszFileName,
                     (LPTSTR)lpbyWorkBuffer,
                     nStrLen);

    // Try .VI0 through .VI9

    for (i=0;i<10;i++)
    {
        lptstrEnd[2] = '0' + i;

        ExcelTryHideName(lpCallBack,
                         lpszFileName,
                         (LPTSTR)lpbyWorkBuffer,
                         nStrLen);
    }

    return 1;
}

#endif  //__MACINTOSH__

//*************************************************************************
//
// Function:
//  WORD ExcelMacroVirusRepair()
//
// Description:
//  Repairs the first stream found that is infected with the Excel
//  macro virus identified by VID.
//
// Returns:
//	EXTSTATUS_OK			If the repair was successful
//	EXTSTATUS_NO_REPAIR		If the repair was unsuccessful
//
//*************************************************************************

BYTE gapsLaroux[] = { 6, 'l', 'a', 'r', 'o', 'u', 'x' , 0 };
BYTE gapsSofa1[] = {0x0C,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0};
BYTE gapsSofa2[] = {0x0B,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0};
BYTE gapsDelta[] = { 7, 'M', 'o', 'd', 'u', 'l', 'e', '1', 0 };
BYTE gapsLegend[] = { 6, 'L', 'e', 'g', 'e', 'n', 'd', 0 };
BYTE gapsRobocop1[] = { 4, 'R', 'O', 'B', 'O', 0};
BYTE gapsRobocop2[] = { 3, 'C', 'O', 'P', 0};
BYTE gapsRobocop3[] = { 7, 'M', 'o', 'd', 'u', 'l', 'e', '1', 0 };
BYTE gapsRobocop4[] = { 7, 'M', 'o', 'd', 'u', 'l', 'e', '2', 0 };

EXTSTATUS ExcelMacroVirusRepair
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
	WORD 			wVID,					// ID of virus to repair
    LPTSTR          lpszFileName,           // Infected file's name
#ifdef __MACINTOSH__
    HFILE           hFile,                  // Handle to file to repair
#endif
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 1024 bytes
)
{
    WORD            wResult = 0;

#ifndef __MACINTOSH__
    HFILE           hFile;
    UINT            uAttr;
#endif


#ifndef __MACINTOSH__
    if (RepairFileOpen(lpCallBack,lpszFileName,&hFile,&uAttr) != 0)
		return 0;
#endif

    switch (wVID)
    {
       case VID_LAROUX:
       case VID_LAROUXB:
       case VID_LAROUXC:
          wResult = ExcelBooksRemoveModule(lpCallBack,
                                           hFile,
                                           lpbyWorkBuffer,
                                           gapsLaroux,
                                           TRUE);
          break;

       case VID_SOFA:
          wResult = ExcelBooksRemoveModule(lpCallBack,
                                           hFile,
                                           lpbyWorkBuffer,
                                           gapsSofa1,
                                           FALSE) ||
                    ExcelBooksRemoveModule(lpCallBack,
                                           hFile,
                                           lpbyWorkBuffer,
                                           gapsSofa2,
                                           FALSE);
          break;

       case VID_DELTA:
          wResult = ExcelBooksRemoveModule(lpCallBack,
                                            hFile,
                                            lpbyWorkBuffer,
                                            gapsDelta,
                                            TRUE);
          break;

       case VID_LEGEND:
          wResult = ExcelBooksRemoveModule(lpCallBack,
                                            hFile,
                                            lpbyWorkBuffer,
                                            gapsLegend,
                                            TRUE);
          break;

       case VID_ROBOCOPA:
          wResult = ExcelBooksRemoveModule(lpCallBack,
                                            hFile,
                                            lpbyWorkBuffer,
                                            gapsRobocop1,
                                            FALSE) &&
                    ExcelBooksRemoveModule(lpCallBack,
                                            hFile,
                                            lpbyWorkBuffer,
                                            gapsRobocop2,
                                            FALSE);

          //Also try to kill these two sheets
          ExcelBooksRemoveModule(lpCallBack,
                                 hFile,
                                 lpbyWorkBuffer,
                                 gapsRobocop3,
                                 FALSE);
          ExcelBooksRemoveModule(lpCallBack,
                                 hFile,
                                 lpbyWorkBuffer,
                                 gapsRobocop4,
                                 FALSE);

          break;
    }

    if (wResult == 0)
    {
#ifndef __MACINTOSH__
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
#endif

        return(EXTSTATUS_NO_REPAIR);
	}

    if (ExcelBlankOutVirusModules(lpCallBack,
                                  hFile,
                                  lpbyWorkBuffer,
                                  wVID) == 0)
    {
#ifndef __MACINTOSH__
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
#endif

        return(EXTSTATUS_NO_REPAIR);
    }

#ifndef __MACINTOSH__
    RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);

    // Delete the backup file

    if (ExcelHideXLStartBackup(lpCallBack,
                               lpszFileName,
                               lpbyWorkBuffer) == 0)
#else
	if (!ExcelMoveStartupCopy( hFile ))                               
#endif
    {
        // Failed to hide the backup

        return(EXTSTATUS_NO_REPAIR);
    }

    return(EXTSTATUS_OK);
}

#endif  // #ifndef SYM_NLM


