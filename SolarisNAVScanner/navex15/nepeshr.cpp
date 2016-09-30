//************************************************************************
//
// $Header:   S:/NAVEX/VCS/nepeshr.cpv   1.11   03 Aug 1998 12:17:14   PDEBATS  $
//
// Description:
//      Shared routines for Windows virus scan/repair.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/nepeshr.cpv  $
// 
//    Rev 1.11   03 Aug 1998 12:17:14   PDEBATS
// Removed if/endif + else for sarc_cst_2242 left else statement commented out
// 
//    Rev 1.10   08 Jul 1998 11:02:16   AOONWAL
// properly removed ifdef SARC_CST_2242 (else part)
// 
//    Rev 1.9   07 Jul 1998 16:58:00   AOONWAL
// commented out ifdef SARC_CST_2242
// 
//    Rev 1.8   30 Jun 1998 17:08:42   DKESSNE
// commented out 0x40 check for Windows files in DetermineWindowsType()
//  -- this check is not valid for PE files
// 
//    Rev 1.7   11 Mar 1998 15:24:24   JWILBER
// Removed comments from "40 check" - this didn't fix the problem.
// Apparently, this is caused by something weird.
//
//    Rev 1.6   11 Mar 1998 13:50:30   JWILBER
// Commented out check for value < 0x40 in first_rel_offset of EXE
// header to determine Windows header type in DetermineWindowsHeader.
//
//    Rev 1.5   26 Dec 1996 15:22:28   AOONWAL
// No change.
//
//    Rev 1.4   02 Dec 1996 14:00:16   AOONWAL
// No change.
//
//    Rev 1.3   29 Oct 1996 12:59:16   AOONWAL
// No change.
//
//    Rev 1.2   28 Aug 1996 16:40:34   DCHI
// Endianized everything.
//
//    Rev 1.1   18 Apr 1996 15:45:24   CNACHEN
// Fixed bug in locating the NE/PE header.  It uses a DWORD offset now from the
// EXE header instead of a WORD offset.
//
//    Rev 1.0   08 Feb 1996 10:39:42   DCHI
// Initial revision.
//
//************************************************************************

#ifdef SARCBOT
#include "config.h"
#endif

#include "endutils.h"

#include "nepeshr.h"

#include "winconst.h"

//********************************************************************
//
// Function:
//  WORD DetermineWindowsType()
//
// Description:
//  Determines the type of a Windows executable.
//
// Returns:
//  Also leaves in lpbyWorkBuffer the first 512 bytes of NE/PE header.
//
//  WINDOWS_NE_FORMAT       Windows NE executable
//  WINDOWS_PE_FORMAT       Windows PE executable
//  WINDOWS_UNKNOWN_FORMAT  Unknown format
//
//********************************************************************

WORD DetermineWindowsType
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    HFILE           hFile,                  // Handle to file to scan
    LPBYTE          lpbyWorkBuffer,         // Work buffer >= 512 bytes
    LPDWORD         lpdwWinHeaderOffset     // Offset of WIN header from TOF
)
{
    LPDWORD         lpdwNEOffsetPtr;

    // Read first sector of file

    if (lpCallBack->FileSeek(hFile,0,SEEK_SET) != 0)
        return WINDOWS_UNKNOWN_FORMAT;

    if (lpCallBack->FileRead(hFile,lpbyWorkBuffer,512) != 512)
        return WINDOWS_UNKNOWN_FORMAT;

    // Check for 0x40 at first_rel_offset commented out by jjw 11 Mar 98
    // Some Windows files are missed w/ this check.
    // Added comments back in - this didn't fix the problem

    // Value of 0x40 or greater indicates another header

//    if (WENDIAN(((LPEXEHEADER_T)lpbyWorkBuffer)->first_rel_offset) < 0x40)
//        return WINDOWS_UNKNOWN_FORMAT;

    // Read Windows header

    // use the DWORD starting at the w2ndHdrOffset field in the EXE
    // header.  This is the offset in the file to the NE/PE header...

    lpdwNEOffsetPtr = (LPDWORD)&((LPEXEHEADER_T)lpbyWorkBuffer)->w2ndHdrOffset;

    // obtain the NE/PE header offset.

    *lpdwWinHeaderOffset = AVDEREF_DWORD(lpdwNEOffsetPtr);

    if (*lpdwWinHeaderOffset & 0x80000000UL)
        return WINDOWS_UNKNOWN_FORMAT;

    if (lpCallBack->FileSeek(hFile,
                             *lpdwWinHeaderOffset,
                             SEEK_SET) != *lpdwWinHeaderOffset)
        return WINDOWS_UNKNOWN_FORMAT;

    if (lpCallBack->FileRead(hFile,lpbyWorkBuffer,512) != 512)
        return WINDOWS_UNKNOWN_FORMAT;

    switch ((WORD)DWENDIAN(((LPNAVEX_IMAGE_NT_HEADERS)lpbyWorkBuffer)->
        Signature))
    {
        case WINDOWS_NE_SIGNATURE:

            // First validate fields

            if (((LPWININFO)lpbyWorkBuffer)->targetOS > 2)
                return WINDOWS_UNKNOWN_FORMAT;

            if (WENDIAN(((LPWININFO)lpbyWorkBuffer)->cs) >
                WENDIAN(((LPWININFO)lpbyWorkBuffer)->segTabEntries))
                return WINDOWS_UNKNOWN_FORMAT;

            if ((DWENDIAN(((LPWININFO)lpbyWorkBuffer)->ss_sp) >> 16) >
                WENDIAN(((LPWININFO)lpbyWorkBuffer)->segTabEntries))
                return WINDOWS_UNKNOWN_FORMAT;

            return WINDOWS_NE_FORMAT;

        case WINDOWS_PE_SIGNATURE:

            if (((LPNAVEX_IMAGE_NT_HEADERS)lpbyWorkBuffer)->FileHeader.Machine !=
                WENDIAN(0x14C))
            {
                // Not IntelI386

                return WINDOWS_UNKNOWN_FORMAT;
            }

            return WINDOWS_PE_FORMAT;

        default:
            break;
    }

    return WINDOWS_UNKNOWN_FORMAT;
}

//********************************************************************
//
// Function:
//  WORD GetPESectionInfo()
//
// Description:
//  Gets the file offset, section length, section flags,
//  and section RVA base for a numbered section.
//
// Returns:
//  Returns the values in the dereferenced DWORDSs passed in.
//
//  EXTSTATUS_FILE_ERROR    On error
//  EXTSTATUS_OK            On success
//
//********************************************************************

EXTSTATUS GetPESectionInfo
(
    LPCALLBACKREV1      lpCallBack,             // File op callbacks
    HFILE               hFile,                  // Handle to file to scan
    LPBYTE              lpbyWorkBuffer,         // Work buffer >= 512 bytes
    DWORD               dwWinHeaderOffset,      // where's the WIN header start?
    DWORD               dwSecNum,               // what section are we
                                                //  interested in (0-base)?
    LPDWORD             lpdwFileOffset,         // where is this section from TOF?
    LPDWORD             lpdwSecLen,             // how long is this section?
    LPDWORD             lpdwFlags,              // section flags
    LPDWORD             lpdwRVABase             // RVA in memory
)
{
    DWORD                       dwOffset;
    NAVEX_IMAGE_SECTION_HEADER FAR *  lpstSegInfo;

    dwOffset = dwWinHeaderOffset +
                sizeof(NAVEX_IMAGE_NT_HEADERS) +
                dwSecNum * sizeof(NAVEX_IMAGE_SECTION_HEADER);

    if (dwOffset & 0x80000000UL)
        return EXTSTATUS_FILE_ERROR;

    if (lpCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
        return EXTSTATUS_FILE_ERROR;

    if (lpCallBack->FileRead(hFile,lpbyWorkBuffer,
        sizeof(NAVEX_IMAGE_SECTION_HEADER)) != sizeof(NAVEX_IMAGE_SECTION_HEADER))
        return EXTSTATUS_FILE_ERROR;

    lpstSegInfo = (NAVEX_IMAGE_SECTION_HEADER FAR *)lpbyWorkBuffer;

    *lpdwFileOffset = DWENDIAN(lpstSegInfo->PointerToRawData);
    *lpdwSecLen = DWENDIAN(lpstSegInfo->SizeOfRawData);
    *lpdwFlags = DWENDIAN(lpstSegInfo->Characteristics);
    *lpdwRVABase = DWENDIAN(lpstSegInfo->VirtualAddress);

    return(EXTSTATUS_OK);
}
