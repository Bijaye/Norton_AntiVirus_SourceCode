//************************************************************************
//
// $Header:   S:/NAVEX/VCS/xl97rpr.cpv   1.3   09 Dec 1998 17:45:40   DCHI  $
//
// Description:
//  Contains Excel 97 macro repair function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/xl97rpr.cpv  $
// 
//    Rev 1.3   09 Dec 1998 17:45:40   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.2   08 Dec 1998 13:00:58   DCHI
// Changes for MSX.
// 
//    Rev 1.1   09 Nov 1998 13:58:48   DCHI
// Added #pragma data_seg()'s for global FAR data for SYM_WIN16.
// 
//    Rev 1.0   08 Sep 1998 17:09:16   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#if defined(NAVEX15) || !defined(SYM_NLM)

#include "platform.h"
#include "ctsn.h"
#include "navex.h"
#include "callback.h"

#include "wd7api.h"
#include "o97api.h"
#include "xl5api.h"
#include "acsapi.h"

#include "wdscan15.h"
#include "wdrepair.h"

#include "offcsc15.h"
#include "o97scn15.h"
#include "o97rpr15.h"
#include "xl97rpr.h"

#include "mcrsgutl.h"

#include "mvp.h"

#include "nvx15inf.h"

// Assume XL97 encrypted repair is enabled

int gnXL97EncryptedRepair = 1;

//********************************************************************
//
// Function:
//  BOOL XL97RepairModule()
//
// Description:
//  Repairs a module of an Excel 97 virus.  If the module should
//  be deleted, then *lpbDelete is set to TRUE before returning.
//
// Returns:
//  TRUE        If the repair was successful
//  FALSE       If the repair was unsuccessful
//
//********************************************************************

BOOL XL97RepairModule
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    WORD                wModuleCount,       // Count of modules
    LPBYTE              abyModuleName,      // Zero terminated module name
    LPBOOL              lpbDelete,          // Ptr to BOOL to request delete
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo    // Virus sig info
)
{
    int                 i;
    LPWD_SCAN           lpstScan;
    LPLZNT              lpstLZNT;

    lpstScan = lpstOffcScan->lpstScan;
    lpstLZNT = lpstScan->uScan.stVBA5.lpstLZNT;

    /////////////////////////////////////////////////////////////
    // Initialize the scan structure
    /////////////////////////////////////////////////////////////

    // Assign hit memory

    if (AssignMacroSigHitMem(lpstOffcScan,
                             lpstScan) == FALSE)
    {
        return(FALSE);
    }

    lpstScan->wMacroCount = wModuleCount;

    lpstScan->lpstStream = lpstOffcScan->lpstStream;
    lpstScan->wScanType = WD_SCAN_TYPE_VBA5;

    // Copy the name

    i = 0;
    while (1)
    {
        lpstScan->abyName[i] = abyModuleName[i];
        if (abyModuleName[i++] == 0)
            break;
    }

    WDInitHitBitArrays(&gstXL97SigSet,lpstOffcScan->lpstScan);

    /////////////////////////////////////////////////////////////
    // Scan and apply repair to module
    /////////////////////////////////////////////////////////////

    // Scan module

    VBA5ScanModule(&gstXL97SigSet,lpstScan);

    // Apply repair signature

    *lpbDelete = FALSE;

    if (MVPCheck(MVP_XL97,
                 lpstScan->abyName,
                 lpstScan->dwMVPCRC) == FALSE &&
        O97LZNTStreamAttributeOnly(lpstLZNT) == FALSE)
    {
        // Found a non-approved macro

        *lpbDelete = TRUE;
    }
    else
    if (lpstVirusSigInfo->wID != VID_MVP)
    {
        // Apply repair signature

        if (WDApplyRepair(&gstXL97SigSet,
                          lpstScan,
                          lpbDelete,
                          lpstVirusSigInfo -
                              gstXL97SigSet.lpastVirusSigInfo,
                          0) ==
            WD_STATUS_ERROR)
        {
            // Error applying repair signature

            return(FALSE);
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL97EncryptedRepairCheck()
//
// Parameters:
//  lpstOffcScan        Ptr to MSOFFICE_SCAN_T structure
//  lpbEncrypted        Ptr to BOOL for result
//
// Description:
//  Determines whether or not to proceed with repair and returns
//  the encrypted state of the document.
//
//  *lpbEncrypted is set to TRUE if the Workbook stream is encrypted
//  and set to FALSE otherwise.
//
// Returns:
//  TRUE                If repair should proceed
//  FALSE               If repair should not proceed
//
//********************************************************************

BOOL XL97EncryptedRepairCheck
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPBOOL              lpbEncrypted
)
{
#if !defined(NAVEX15) && (defined(SYM_WIN16) || defined(SYM_DOSX))

    // See if we have loaded up XL97 encrypted repair enabled flag yet

    if (gbCheckedXL97EncRepEnableState == FALSE)
        GetXL97EncRepEnableState((LPCALLBACKREV1)lpstOffcScan->lpstRoot->
                                     lpvRootCookie);

#endif // #if defined(SYM_DOSX) || defined(SYM_WIN16)

    if (O97XLFindAssociatedStreams(lpstOffcScan->lpstRoot,
                                   &lpstOffcScan->u.stO97) == FALSE)
    {
        // Could not find associated streams

        return(FALSE);
    }

    // Initialize for access to the Workbook stream

    if (SSOpenStreamAtIndex(lpstOffcScan->lpstStream,
                            lpstOffcScan->u.stO97.
                                u.stXL97.dwWorkbookEntry) != SS_STATUS_OK)
    {
        // Failed to open Workbook stream

        return(FALSE);
    }

    // Is the Workbook stream encrypted?

    if (XL97WorkbookIsEncrypted(lpstOffcScan->lpstStream,
                                lpbEncrypted) == FALSE)
    {
        // Failed to determine encrypted state

        return(FALSE);
    }

    // If the document is encrypted and we are not doing
    //  XL97 encrypted repair, return NO_REPAIR

    if (*lpbEncrypted != FALSE && gnXL97EncryptedRepair == 0)
        return(FALSE);

    // The document is either not encrypted or
    //  it is and we are doing XL97 encrypted repair

    return(TRUE);
}


//********************************************************************
//
// gabyXLLZNTBlankMsg[]     LZNT compressed blank msg
// gabyXLLZNTNAVMsg[]       LZNT compressed NAV msg
//
//********************************************************************

// '''''''''''''''''''''''''''''''''''''''''''''''''''''''''
// ' During repair, Norton AntiVirus replaced the contents '
// ' of this Visual Basic module with the message you are  '
// ' now reading.  The module could not be removed at the  '
// ' time of repair because the document was password      '
// ' protected.                                            '
// '                                                       '
// ' If you are using Excel 97 or greater, you can delete  '
// ' this module by following these steps:                 '
// '   1. Click in the Window with this message so that    '
// '      the cursor is blinking within it.                '
// '   2. From the File menu, select the menu item whose   '
// '      text begins with Remove.                         '
// '   3. Click the No button of the dialog.               '
// '                                                       '
// ' If you are using Excel 5.0/95, you can delete this    '
// ' sheet by following these steps:                       '
// '   1. From the Edit menu, select Delete Sheet.         '
// '   2. Confirm that you want to delete the sheet        '
// '      by clicking the OK button of the dialog.         '
// '''''''''''''''''''''''''''''''''''''''''''''''''''''''''

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyXLLZNTBlankMsg[] =
{
    0x01, 0x22, 0xB0, 0x00, 0x41, 0x74, 0x74, 0x72,
    0x69, 0x62, 0x75, 0x74, 0x00, 0x65, 0x20, 0x56,
    0x42, 0x5F, 0x4E, 0x61, 0x6D, 0x00, 0x65, 0x20,
    0x3D, 0x20, 0x22, 0x4D, 0x6F, 0x64, 0x00, 0x75,
    0x6C, 0x65, 0x31, 0x22, 0x0D, 0x0A
};

BYTE FAR gabyXLLZNTNAVMsg[] =
{
    0x01, 0x80, 0xB2, 0x00, 0x41, 0x74, 0x74, 0x72,
    0x69, 0x62, 0x75, 0x74, 0x00, 0x65, 0x20, 0x56,
    0x42, 0x5F, 0x4E, 0x61, 0x6D, 0x00, 0x65, 0x20,
    0x3D, 0x20, 0x22, 0x4D, 0x6F, 0x64, 0x00, 0x75,
    0x6C, 0x65, 0x31, 0x22, 0x0D, 0x0A, 0x27, 0x03,
    0x35, 0x00, 0x00, 0x74, 0x20, 0x44, 0x75, 0x72,
    0x69, 0x6E, 0x00, 0x67, 0x20, 0x72, 0x65, 0x70,
    0x61, 0x69, 0x72, 0x00, 0x2C, 0x20, 0x4E, 0x6F,
    0x72, 0x74, 0x6F, 0x6E, 0x00, 0x20, 0x41, 0x6E,
    0x74, 0x69, 0x56, 0x69, 0x72, 0x04, 0x75, 0x73,
    0x01, 0x30, 0x6C, 0x61, 0x63, 0x65, 0x64, 0x00,
    0x20, 0x74, 0x68, 0x65, 0x20, 0x63, 0x6F, 0x6E,
    0x80, 0x74, 0x65, 0x6E, 0x74, 0x73, 0x20, 0x27,
    0x01, 0x3A, 0x04, 0x6F, 0x66, 0x00, 0x14, 0x69,
    0x73, 0x20, 0x56, 0x69, 0x00, 0x73, 0x75, 0x61,
    0x6C, 0x20, 0x42, 0x61, 0x73, 0x10, 0x69, 0x63,
    0x20, 0x6D, 0x02, 0x96, 0x20, 0x77, 0x69, 0x04,
    0x74, 0x68, 0x02, 0x32, 0x6D, 0x65, 0x73, 0x73,
    0x61, 0x00, 0x67, 0x65, 0x20, 0x79, 0x6F, 0x75,
    0x20, 0x61, 0x88, 0x72, 0x65, 0x20, 0x03, 0x3A,
    0x6E, 0x6F, 0x77, 0x00, 0x59, 0x84, 0x61, 0x64,
    0x00, 0x7A, 0x2E, 0x20, 0x20, 0x54, 0x00, 0x5A,
    0x01, 0x04, 0x37, 0x63, 0x6F, 0x75, 0x6C, 0x64,
    0x20, 0x6E, 0x20, 0x6F, 0x74, 0x20, 0x62, 0x65,
    0x00, 0x21, 0x6D, 0x6F, 0x32, 0x76, 0x00, 0x7A,
    0x61, 0x74, 0x02, 0x25, 0x03, 0x1D, 0x74, 0x69,
    0x0F, 0x80, 0x7F, 0x00, 0x3D, 0x83, 0x58, 0x00,
    0x13, 0x63, 0x61, 0x75, 0x73, 0x82, 0x65, 0x02,
    0x10, 0x64, 0x6F, 0x63, 0x75, 0x6D, 0x80, 0x4F,
    0x00, 0x20, 0x77, 0x61, 0x73, 0x20, 0x70, 0x61,
    0x73, 0xC0, 0x73, 0x77, 0x6F, 0x72, 0x64, 0x20,
    0x02, 0x00, 0x02, 0x58, 0x00, 0x70, 0x72, 0x6F,
    0x74, 0x65, 0x63, 0x74, 0x65, 0xFE, 0x64, 0x80,
    0x39, 0x03, 0x0B, 0xA1, 0x02, 0x02, 0x1D, 0x27,
    0x17, 0x89, 0x14, 0x02, 0x1D, 0x24, 0x49, 0x66,
    0x06, 0x7E, 0x75, 0x73, 0x81, 0xB5, 0x45, 0x78,
    0x00, 0x63, 0x65, 0x6C, 0x20, 0x39, 0x37, 0x20,
    0x6F, 0x00, 0x72, 0x20, 0x67, 0x72, 0x65, 0x61,
    0x74, 0x65, 0x03, 0x00, 0xBC, 0x81, 0x8F, 0x63,
    0x61, 0x6E, 0x20, 0x64, 0x65, 0x6C, 0x6C, 0x65,
    0x00, 0xF4, 0x84, 0x75, 0x68, 0x00, 0xAF, 0x84,
    0x8C, 0x62, 0x00, 0x79, 0x20, 0x66, 0x6F, 0x6C,
    0x6C, 0x6F, 0x77, 0x07, 0x81, 0x10, 0x00, 0x63,
    0xC0, 0x3C, 0x73, 0x74, 0x65, 0x70, 0x73, 0x06,
    0x3A, 0x8E, 0x26, 0x04, 0x2C, 0x31, 0x2E, 0x20,
    0x43, 0x6C, 0x40, 0x69, 0x63, 0x6B, 0x20, 0x69,
    0x6E, 0x42, 0x47, 0x57, 0x78, 0x69, 0x6E, 0x64,
    0x80, 0x5D, 0x44, 0x66, 0x01, 0x16, 0x84, 0x66,
    0x73, 0x7E, 0x6F, 0xC0, 0x07, 0xC0, 0x58, 0x00,
    0x12, 0x84, 0x0E, 0x40, 0x02, 0x82, 0x7B, 0x75,
    0x0C, 0x72, 0x73, 0xC0, 0x28, 0x80, 0x0A, 0x62,
    0x6C, 0x69, 0x6E, 0xCE, 0x6B, 0x01, 0x1E, 0x41,
    0x0F, 0xC0, 0x13, 0x69, 0x74, 0x8E, 0x50, 0x84,
    0x0E, 0x80, 0x32, 0x2E, 0x20, 0x46, 0x72, 0x6F,
    0x6D, 0x42, 0x1C, 0x20, 0x46, 0x69, 0x6C, 0x65,
    0x20, 0x00, 0x64, 0x75, 0x2C, 0x34, 0x20, 0x73,
    0x80, 0x35, 0x63, 0x83, 0x71, 0x01, 0x04, 0x20,
    0x69, 0x80, 0x74, 0x65, 0x6D, 0x20, 0x77, 0x68,
    0x6F, 0x40, 0x31, 0x83, 0x84, 0x84, 0xC0, 0x1C,
    0x20, 0x20, 0x74, 0x65, 0x78, 0x41, 0x7E, 0x90,
    0x67, 0x69, 0x6E, 0x73, 0x83, 0x90, 0x52, 0x65,
    0x81, 0x80, 0x1B, 0xD7, 0x6B, 0x44, 0x1D, 0x33,
    0xC5, 0x3A, 0x81, 0x2D, 0x4E, 0x6F, 0x20, 0x07,
    0x00, 0xCB, 0x81, 0xB1, 0x42, 0xA8, 0x65, 0x20,
    0x64, 0x69, 0x61, 0xF8, 0x6C, 0x6F, 0x67, 0x0D,
    0x11, 0x84, 0x0E, 0xC2, 0x1D, 0x2C, 0x01, 0x82,
    0x0E, 0x01, 0xD4, 0x75, 0x35, 0x2E, 0x30, 0x2F,
    0x39, 0x35, 0x2C, 0x3B, 0xC2, 0x7C, 0x08, 0x74,
    0x74, 0x81, 0x72, 0x80, 0x19, 0x82, 0x0E, 0x73,
    0x68, 0x3C, 0x65, 0x65, 0xC0, 0x39, 0x66, 0x74,
    0xC3, 0x13, 0x42, 0x07, 0x20, 0x20, 0x43, 0xE0,
    0x3A, 0x26, 0x2C, 0x45, 0x64, 0x69, 0x74, 0x2B,
    0x2C, 0x44, 0x3D, 0xE3, 0x47, 0x53, 0x21, 0x0C,
    0x27, 0x1E, 0x44, 0x07, 0x80, 0x33, 0x43, 0x6F,
    0x70, 0x6E, 0x66, 0x69, 0x72, 0xE1, 0x33, 0x20,
    0x3E, 0x81, 0x4E, 0x77, 0xF4, 0x61, 0x6E, 0xE0,
    0x32, 0x6F, 0x05, 0x4F, 0x01, 0x28, 0xC3, 0x13,
    0x83, 0x0E, 0x37, 0x03, 0x51, 0x62, 0x01, 0x20,
    0x50, 0x63, 0x01, 0x4A, 0x04, 0x50, 0x20, 0x4F,
    0x1E, 0x4B, 0xFC, 0x2C, 0xA1, 0x0E, 0x7F, 0x93,
    0x33, 0x04, 0x0D, 0x0A
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif


//********************************************************************
//
// Function:
//  BOOL XL97ReplaceVBASrcWithMsg()
//
// Parameters:
//  lpstStream          Ptr to stream to replace
//  lpstLZNT            Ptr to LZNT structure
//
// Description:
//  Replaces the compressed source of the given stream with either
//  the entirely blank message or the one with a message.  The one
//  used is the largest one that fits.  The remainder of the stream
//  is zeroed out.  The stack-based binary is also zeroed out.
//
//  If the compressed source of the given stream is smaller than
//  both of the replacement messages, the function returns error.
//
// Returns:
//  TRUE                If replacement was successful
//  FALSE               On error
//
//********************************************************************

BOOL XL97ReplaceVBASrcWithMsg
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT
)
{
    int                 nVersion;
    BOOL                bLitEnd;

    DWORD               dwLZNTOffset;
    DWORD               dwStreamLen;
    LPBYTE              lpabyReplacement;
    DWORD               dwZeroesToWrite;
    DWORD               dwBytesToWrite;
    DWORD               dwCount;

    DWORD               dwLineTableOffset;
    DWORD               dwNumLines;
    DWORD               dwBinaryOffset;

    // Determine module type

    if (O97GetModuleVersionEndian(lpstStream,
                                  &nVersion,
                                  &bLitEnd) == FALSE)
        return(FALSE);

    if (VBA5LZNTModuleInit(lpstStream,
                           lpstLZNT) == FALSE)
        return(FALSE);

    dwLZNTOffset = lpstLZNT->dwFirstChunkOffset - 1;

    dwStreamLen = SSStreamLen(lpstStream);
    if (dwLZNTOffset >= dwStreamLen)
        return(FALSE);

    /////////////////////////////////////////////////////////////
    // Determine which message to use

    dwBytesToWrite = dwStreamLen - dwLZNTOffset;
    if (dwBytesToWrite < sizeof(gabyXLLZNTBlankMsg))
        return(FALSE);

    if (dwBytesToWrite >= sizeof(gabyXLLZNTNAVMsg))
    {
        // Use the full message

        lpabyReplacement = gabyXLLZNTNAVMsg;
        dwZeroesToWrite = dwBytesToWrite - sizeof(gabyXLLZNTNAVMsg);
        dwBytesToWrite = sizeof(gabyXLLZNTNAVMsg);
    }
    else
    {
        // Use the blank message

        lpabyReplacement = gabyXLLZNTBlankMsg;
        dwZeroesToWrite = dwBytesToWrite - sizeof(gabyXLLZNTBlankMsg);
        dwBytesToWrite = sizeof(gabyXLLZNTBlankMsg);
    }

    /////////////////////////////////////////////////////////////
    // Replace the module source

    if (SSSeekWrite(lpstStream,
                    dwLZNTOffset,
                    lpabyReplacement,
                    dwBytesToWrite,
                    &dwCount) != SS_STATUS_OK ||
        dwCount != dwBytesToWrite)
    {
        // Wrote less than the desired number of bytes

        return(FALSE);
    }

    // Zero out the remainder of the stream

    if (SSWriteZeroes(lpstStream,
                      dwLZNTOffset + dwBytesToWrite,
                      dwZeroesToWrite) != SS_STATUS_OK)
    {
        // Failed to zero out rest of stream

        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Zero out the stack-based code

    if (O97GetLineBinaryOffsets(lpstStream,
                                &dwLineTableOffset,
                                &dwNumLines,
                                &dwBinaryOffset) == FALSE)
        return(FALSE);

    // Validate the binary offset

    if (dwBinaryOffset > dwLZNTOffset || dwBinaryOffset < 10)
        return(FALSE);

    // Get the size of the stack based code

    if (SSSeekRead(lpstStream,
                   dwBinaryOffset - 4,
                   &dwZeroesToWrite,
                   sizeof(DWORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(DWORD))
        return(FALSE);

    dwZeroesToWrite = SSEndianDWORD((LPBYTE)&dwZeroesToWrite,bLitEnd);

    // Validate the size

    if (dwZeroesToWrite >= dwStreamLen ||
        dwBinaryOffset + dwZeroesToWrite > dwLZNTOffset)
        return(FALSE);

    // Zero out the stack-based code

    if (SSWriteZeroes(lpstStream,
                      dwBinaryOffset,
                      dwZeroesToWrite) != SS_STATUS_OK)
    {
        // Failed to zero out rest of stream

        return(FALSE);
    }

    // Truncate the stream

    if (SSSetStreamLen(lpstStream->lpstRoot,
                       SSStreamID(lpstStream),
                       dwLZNTOffset + dwBytesToWrite) != SS_STATUS_OK)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL XL97RepairEpilogue()
//
// Parameters:
//  lpstOffcScan        Ptr to MSOFFICE_SCAN_T structure
//  lpstLZNT            Ptr to LZNT_T structure
//  bEncrypted          Specifies whether Workbook is encrypted
//
// Description:
//  Completes repair on an Excel 97 repair by completing the
//  following steps:
//
//  - Updates for recompilation.
//  - If the Workbook is not encrypted, performs the following:
//      - Changes all VB sheets to very hidden
//      - Determines whether to delete the <Macros> storage
//      - Determines whether to remove the Visual Basic record
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL XL97RepairEpilogue
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPLZNT              lpstLZNT,
    BOOL                bEncrypted
)
{
    // Delete __SRP_ streams and increment second ushort
    //  of _VBA_PROJECT stream

    if (O97UpdateForRecompilation(lpstOffcScan->lpstRoot,
                                  lpstOffcScan->lpstStream,
                                  &lpstOffcScan->u.stO97) == FALSE)
    {
        // Error updating for recompilation

        return(FALSE);
    }

    if (bEncrypted == FALSE)
    {
        // Set VB sheets to very hidden

        if (O97XLWorkbookVBtoVHWS(lpstOffcScan->lpstStream,
                                  lpstOffcScan->u.stO97.u.stXL97.
                                      dwWorkbookEntry) == FALSE)
        {
            // Error setting VB sheets to very hidden

            return(FALSE);
        }

        // Determine whether to get rid of the _VBA_PROJECT_CUR storage

        if (O97XLCheckMacrosDeletable(lpstOffcScan->lpstRoot,
                                      lpstOffcScan->lpstStream,
                                      lpstLZNT,
                                      &lpstOffcScan->u.stO97) == TRUE)
        {
            // Remove Visual Basic Project record from Workbook stream

            if (O97XLWorkbookRemoveVBP(lpstOffcScan->lpstStream,
                                       lpstOffcScan->u.stO97.u.stXL97.
                                           dwWorkbookEntry) == FALSE)
            {
                // Failed to remove Visual Basic Project record

                return(FALSE);
            }

            // Delete the _VBA_PROJECT_CUR storage

            if (SSUnlinkEntry(lpstOffcScan->lpstRoot,
                              lpstOffcScan->u.stO97.u.stXL97.
                                  dwParentStorageEntry,
                              lpstOffcScan->u.stO97.u.stXL97.
                                  dw_VBA_PROJECT_CUREntry) != SS_STATUS_OK)
            {
                // Failed to delete Macros storage

                return(FALSE);
            }
        }
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  EXTSTATUS XL97MacroVirusRepair()
//
// Description:
//  Repairs a file of Excel 97 macro viruses.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS XL97MacroVirusRepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPBYTE              lpbyWorkBuffer,
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo
)
{
    BYTE                abyModuleName[SS_MAX_NAME_LEN+1];
    WORD                wModuleCount;

    BOOL                bDelete;
    BOOL                bMore;
    BOOL                bEncrypted;
    LPSS_ENUM_SIBS      lpstSibs;
    LPWD_SCAN           lpstScan;

    // Check to see whether we should continue with repair
    //  based on the encrypted state of the document and
    //  whether we are doing XL97 encrypted repair

    if (XL97EncryptedRepairCheck(lpstOffcScan,
                                 &bEncrypted) == FALSE)
    {
        // Do not proceed with repair

        return(EXTSTATUS_NO_REPAIR);
    }

    lpstScan = lpstOffcScan->lpstScan;

    // Allocate LZNT structure

    if (LZNTAllocStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                        &lpstScan->uScan.stVBA5.lpstLZNT) != TRUE)
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    // Count number of non-empty modules

    if (O97CountNonEmptyModules(lpstOffcScan->lpstRoot,
                                lpstOffcScan->lpstStream,
                                lpstScan->uScan.stVBA5.lpstLZNT,
                                lpstOffcScan->u.stO97.dwVBAChildEntry,
                                &wModuleCount) == FALSE)
    {
        // Failed to get module count

        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        return(EXTSTATUS_NO_REPAIR);
    }

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstOffcScan->lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        return(EXTSTATUS_NO_REPAIR);
    }

    // Iterate through the modules to delete

    bMore = TRUE;
    while (bMore)
    {
        SSInitEnumSibsStruct(lpstSibs,
                             lpstOffcScan->u.stO97.dwVBAChildEntry);

        while (1)
        {
            if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                                       O97OpenVBA5ModuleCB,
                                       abyModuleName,
                                       lpstSibs,
                                       lpstOffcScan->lpstStream) ==
                SS_STATUS_OK)
            {
                // Scan module

                if (XL97RepairModule(lpstOffcScan,
                                     wModuleCount,
                                     abyModuleName,
                                     &bDelete,
                                     lpstVirusSigInfo) == FALSE)
                {
                    // Error repairing this module

                    LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                   lpstScan->uScan.stVBA5.lpstLZNT);

                    SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,
                                         lpstSibs);
                    return(EXTSTATUS_NO_REPAIR);
                }

                if (bDelete == TRUE)
                {
                    if (bEncrypted == FALSE)
                    {
                        // Delete the module

                        if (O97XLDeleteModule(lpstOffcScan->lpstRoot,
                                              lpstOffcScan->lpstStream,
                                              lpstScan->uScan.stVBA5.lpstLZNT,
                                              abyModuleName,
                                              &lpstOffcScan->u.stO97,
                                              lpbyWorkBuffer) == FALSE)
                        {
                            // Error deleting the module

                            LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                           lpstScan->uScan.stVBA5.lpstLZNT);

                            SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,
                                                 lpstSibs);
                            return(EXTSTATUS_NO_REPAIR);
                        }

                        if (SSGetChildOfStorage(lpstOffcScan->lpstRoot,
                                                lpstOffcScan->u.stO97.dwVBAEntry,
                                                &lpstOffcScan->u.stO97.dwVBAChildEntry) !=
                            SS_STATUS_OK)
                        {
                            // Error getting the child of the VBA storage

                            LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                           lpstScan->uScan.stVBA5.lpstLZNT);

                            SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,
                                                 lpstSibs);
                            return(EXTSTATUS_NO_REPAIR);
                        }

                        // Need to resynchronize

                        // Resynchronization assumes that repairs without
                        //  deletions will not do a blind repair.

                        break;
                    }
                    else
                    {
                        // Just replace the module with a blank stream

                        if (XL97ReplaceVBASrcWithMsg(lpstOffcScan->
                                                         lpstStream,
                                                     lpstScan->uScan.stVBA5.
                                                         lpstLZNT) == FALSE)
                        {
                            // Error replacing stream

                            LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                           lpstScan->uScan.stVBA5.lpstLZNT);

                            SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,
                                                 lpstSibs);
                            return(EXTSTATUS_NO_REPAIR);
                        }
                    }
                }
            }
            else
            {
                // No more candidate modules

                bMore = FALSE;
                break;
            }
        }
    }

    SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);

    if (XL97RepairEpilogue(lpstOffcScan,
                           lpstScan->uScan.stVBA5.lpstLZNT,
                           bEncrypted) == FALSE)
    {
        // Error in epilogue

        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        return(EXTSTATUS_NO_REPAIR);
    }

    LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                   lpstScan->uScan.stVBA5.lpstLZNT);

    return(EXTSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EXTSTATUS XL97DoFullSetRepair()
//
// Description:
//  Performs a full set repair on an Excel 97 document.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

EXTSTATUS XL97DoFullSetRepair
(
    LPMSOFFICE_SCAN     lpstOffcScan,
    LPBYTE              lpbyWorkBuffer
)
{
    BOOL                bMore;
    BOOL                bEncrypted;
    LPWD_SCAN           lpstScan;
    LPSS_ENUM_SIBS      lpstSibs;

    // Check to see whether we should continue with repair
    //  based on the encrypted state of the document and
    //  whether we are doing XL97 encrypted repair

    if (XL97EncryptedRepairCheck(lpstOffcScan,
                                 &bEncrypted) == FALSE)
    {
        // Do not proceed with repair

        return(EXTSTATUS_NO_REPAIR);
    }

    lpstScan = lpstOffcScan->lpstScan;

    // Allocate LZNT structure

    if (LZNTAllocStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                        &lpstScan->uScan.stVBA5.lpstLZNT) != TRUE)
    {
        return(EXTSTATUS_NO_REPAIR);
    }

    // Allocate a sibling enumeration structure

    if (SSAllocEnumSibsStruct(lpstOffcScan->lpstRoot,
                              &lpstSibs,
                              SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
    {
        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        return(EXTSTATUS_NO_REPAIR);
    }

    // Assign hit memory

    if (AssignMacroSigHitMem(lpstOffcScan,
                             lpstScan) == FALSE)
    {
        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_NO_REPAIR);
    }


    /////////////////////////////////////////////////////////////
    // Initialize the scan structure
    /////////////////////////////////////////////////////////////

    lpstScan->lpstStream = lpstOffcScan->lpstStream;
    lpstScan->wScanType = WD_SCAN_TYPE_VBA5;

    // Check to see if it is a full set

    if (VBA5IsFullSet(lpstOffcScan->lpstRoot,
                      lpstOffcScan->lpstStream,
                      &gstXL97SigSet,
                      lpstScan,
                      lpstOffcScan->u.stO97.dwVBAChildEntry,
                      lpstSibs) == FALSE)
    {
        // The set of modules is not a full set

        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_OK);
    }

    // At this point, all the modules have been verified to be part
    //  of the full set, so delete them all

    if (O97XLFindAssociatedStreams(lpstOffcScan->lpstRoot,
                                   &lpstOffcScan->u.stO97) == FALSE)
    {
        // Could not find associated streams

        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                       lpstScan->uScan.stVBA5.lpstLZNT);

        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
        return(EXTSTATUS_NO_REPAIR);
    }

    // Iterate through the modules to delete

    bMore = TRUE;
    while (bMore)
    {
        SSInitEnumSibsStruct(lpstSibs,
                             lpstOffcScan->u.stO97.dwVBAChildEntry);

        while (1)
        {
            if (SSEnumSiblingEntriesCB(lpstOffcScan->lpstRoot,
                                       O97OpenVBA5ModuleCB,
                                       lpstScan->abyName,
                                       lpstSibs,
                                       lpstOffcScan->lpstStream) ==
                SS_STATUS_OK)
            {
                if (bEncrypted == FALSE)
                {
                    // Delete the module

                    if (O97XLDeleteModule(lpstOffcScan->lpstRoot,
                                          lpstOffcScan->lpstStream,
                                          lpstScan->uScan.stVBA5.lpstLZNT,
                                          lpstScan->abyName,
                                          &lpstOffcScan->u.stO97,
                                          lpbyWorkBuffer) == FALSE)
                    {
                        // Error deleting the module

                        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                       lpstScan->uScan.stVBA5.lpstLZNT);

                        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
                        return(EXTSTATUS_NO_REPAIR);
                    }

                    if (SSGetChildOfStorage(lpstOffcScan->lpstRoot,
                                            lpstOffcScan->u.stO97.dwVBAEntry,
                                            &lpstOffcScan->u.stO97.dwVBAChildEntry) !=
                        SS_STATUS_OK)
                    {
                        // Error getting the child of the VBA storage

                        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                       lpstScan->uScan.stVBA5.lpstLZNT);

                        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
                        return(EXTSTATUS_NO_REPAIR);
                    }

                    // Need to resynchronize

                    // Resynchronization assumes that repairs without
                    //  deletions will not do a blind repair.

                    break;
                }
                else
                {
                    // Just replace the module with a blank stream

                    if (XL97ReplaceVBASrcWithMsg(lpstOffcScan->
                                                     lpstStream,
                                                 lpstScan->uScan.stVBA5.
                                                     lpstLZNT) == FALSE)
                    {
                        // Error replacing stream

                        LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                                       lpstScan->uScan.stVBA5.lpstLZNT);

                        SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,
                                             lpstSibs);
                        return(EXTSTATUS_NO_REPAIR);
                    }
                }
            }
            else
            {
                // No more candidate modules

                bMore = FALSE;
                break;
            }
        }
    }

    LZNTFreeStruct(lpstOffcScan->lpstRoot->lpvRootCookie,
                   lpstScan->uScan.stVBA5.lpstLZNT);

    SSFreeEnumSibsStruct(lpstOffcScan->lpstRoot,lpstSibs);
    return(EXTSTATUS_OK);
}

#endif  // #ifndef SYM_NLM

