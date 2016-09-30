//************************************************************************
//
// $Header:   S:/NAVEX/VCS/neperep.cpv   1.14   02 Apr 1997 19:06:18   AOONWAL  $
//
// Description:
//      Source file for Windows virus repair.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/neperep.cpv  $
// 
//    Rev 1.14   02 Apr 1997 19:06:18   AOONWAL
// Added LPN30 (structure) parameter to all the repair functions
// 
//    Rev 1.13   26 Dec 1996 15:22:18   AOONWAL
// No change.
// 
//    Rev 1.12   10 Dec 1996 13:17:16   AOONWAL
// Added repair for WinTpvo.3783
// 
//    Rev 1.11   02 Dec 1996 14:02:08   AOONWAL
// No change.
// 
//    Rev 1.10   29 Oct 1996 12:58:50   AOONWAL
// No change.
// 
//    Rev 1.9   28 Aug 1996 16:39:00   DCHI
// Endianized everything.
// 
//    Rev 1.8   13 Jun 1996 21:15:44   JWILBER
// Added repair for Tentacle II virus, AKA New Tentacle, VID 1f8d.
//
//    Rev 1.7   15 May 1996 18:18:08   JWILBER
// Fixed bug in Tentacle repair to make it work with large segment
// tables.
//
//    Rev 1.6   15 May 1996 00:31:20   JWILBER
// Added repair for Tentacle.
//
//    Rev 1.5   15 Apr 1996 20:39:44   RSTANEV
// TCHAR support.
//
//    Rev 1.4   09 Feb 1996 15:57:46   CNACHEN
// Modification to BOZA.
//
//    Rev 1.3   09 Feb 1996 10:39:54   DCHI
// Report NO_REPAIR if debug info present.
//
//    Rev 1.2   09 Feb 1996 10:16:34   DCHI
// Corrected BOZA corruption check.
//
//    Rev 1.1   08 Feb 1996 14:09:42   DCHI
// Return EXTSTATUS_NO_REPAIR on open fail.  Corrected corruption check.
//
//    Rev 1.0   08 Feb 1996 10:39:38   DCHI
// Initial revision.
//
//************************************************************************

#ifndef SYM_NLM

#include "endutils.h"

#include "neperep.h"

#include "winconst.h"

#include "nepeshr.h"

//********************************************************************
//
// Function:
//  WORD BozaRepair()
//
// Description:
//  Repairs Boza infection.
//
// Returns:
//  EXTSTATUS_OK            If the repair was successful
//  EXTSTATUS_NO_REPAIR     If the repair was unsuccessful
//
//********************************************************************

BYTE gbyBozaBookmark[] =
{
    0xE8, 0x00, 0x00, 0x00, 0x00, 0x5D, 0x8B, 0xC5, 0x2D
};

BYTE gbyBozaBookmark2[] =
{
    0x2E, 0x76, 0x6C, 0x61, 0x64
};

WORD EXTRepairFileBoza
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    LPN30           lpsN30,                 // Pointer to N30 structure
    LPTSTR          lpszFileName,           // Infected file's name
    LPBYTE          lpbyWorkBuffer          // Work buffer >= 512 bytes
)
{
    int     i;
    HFILE   hFile;
    UINT    uAttr;
    DWORD   dwWinHeaderOffset, dwEndOfSectionTable;
    DWORD   dwOffset, dwSecLen, dwFlags, dwRVABase, dwTemp;
    DWORD   dwEPAdjustValue, dwVirusSectionOffset;
    NAVEX_IMAGE_NT_HEADERS stPEInfo;
    NAVEX_IMAGE_SECTION_HEADER  stSectionHeader = { 0 };

    if (RepairFileOpen(lpCallBack,lpszFileName,&hFile,&uAttr) != 0)
        return (EXTSTATUS_NO_REPAIR);

    // Get header

    if (DetermineWindowsType(lpCallBack,
                             hFile,
                             lpbyWorkBuffer,
                             &dwWinHeaderOffset) != WINDOWS_PE_FORMAT)
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    stPEInfo = *((LPNAVEX_IMAGE_NT_HEADERS)lpbyWorkBuffer);

    // Check for corruption

    dwEndOfSectionTable = dwWinHeaderOffset +
        sizeof(NAVEX_IMAGE_NT_HEADERS) +
        WENDIAN(stPEInfo.FileHeader.NumberOfSections) *
        sizeof(NAVEX_IMAGE_SECTION_HEADER);

    if (DWENDIAN(stPEInfo.OptionalHeader.FileAlignment) == 0 ||
        DWENDIAN(stPEInfo.OptionalHeader.SectionAlignment) == 0)
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    // Corruption also if debug info present

    if (DWENDIAN(stPEInfo.OptionalHeader.
        DataDirectory[NAVEX_IMAGE_DIRECTORY_ENTRY_DEBUG].Size) != 0)
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    if ((dwEndOfSectionTable % DWENDIAN(stPEInfo.OptionalHeader.FileAlignment))
        <= sizeof(NAVEX_IMAGE_SECTION_HEADER) &&
        (dwEndOfSectionTable % DWENDIAN(stPEInfo.OptionalHeader.FileAlignment))
        != 0)
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    // Get segment entry

    if (GetPESectionInfo(lpCallBack,
                         hFile,
                         lpbyWorkBuffer,
                         dwWinHeaderOffset,
                         WENDIAN(stPEInfo.FileHeader.NumberOfSections) - 1,
                         &dwOffset,
                         &dwSecLen,
                         &dwFlags,
                         &dwRVABase) != EXTSTATUS_OK)
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    dwVirusSectionOffset = dwOffset;

    // Read/bookmark virus

    if (lpCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    if (lpCallBack->FileRead(hFile,lpbyWorkBuffer,16) != 16)
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    dwEPAdjustValue = AVDEREF_DWORD(lpbyWorkBuffer + 9) - 5;

    // Verify bookmark

    for (i=0;i<sizeof(gbyBozaBookmark);i++)
    {
        if (gbyBozaBookmark[i] != lpbyWorkBuffer[i])
        {
            RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
            return (EXTSTATUS_NO_REPAIR);
        }
    }

//    // Read secondary bookmark
//
//    dwOffset += 2552;
//    if (lpCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
//    {
//        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
//        return (EXTSTATUS_NO_REPAIR);
//    }
//
//    if (lpCallBack->FileRead(hFile,lpbyWorkBuffer,16) != 16)
//    {
//        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
//        return (EXTSTATUS_NO_REPAIR);
//    }
//
//    // Verify bookmark2
//
//    for (i=0;i<sizeof(gbyBozaBookmark2);i++)
//    {
//        if (gbyBozaBookmark2[i] != lpbyWorkBuffer[i])
//        {
//            RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
//            return (EXTSTATUS_NO_REPAIR);
//        }
//    }

    // Alter header
    //  # of sections -= 1
    //  EP = RVA base of virus section + 5 - (DWORD)virus[9]
    //  Reserved = 0
    //  Image size -= (1A78 / SectionAlignment + 1) * SectionAlignment

    // This is the code before endianization:
    //
    // stPEInfo.FileHeader.NumberOfSections--;
    // stPEInfo.OptionalHeader.AddressOfEntryPoint -= dwEPAdjustValue;
    // stPEInfo.OptionalHeader.Reserved1 = 0;
    // stPEInfo.OptionalHeader.SizeOfImage -=
    //     (0x1A78 / stPEInfo.OptionalHeader.SectionAlignment + 1) *
    //     stPEInfo.OptionalHeader.SectionAlignment;

    stPEInfo.FileHeader.NumberOfSections =
        WENDIAN(WENDIAN(stPEInfo.FileHeader.NumberOfSections) - 1);
    stPEInfo.OptionalHeader.AddressOfEntryPoint =
        DWENDIAN(DWENDIAN(stPEInfo.OptionalHeader.AddressOfEntryPoint) -
            dwEPAdjustValue);
    stPEInfo.OptionalHeader.Reserved1 = 0;
    stPEInfo.OptionalHeader.SizeOfImage = DWENDIAN(
        DWENDIAN(stPEInfo.OptionalHeader.SizeOfImage) -
            (0x1A78 / DWENDIAN(stPEInfo.OptionalHeader.SectionAlignment) + 1) *
                DWENDIAN(stPEInfo.OptionalHeader.SectionAlignment));

    // Write header

    if (lpCallBack->FileSeek(hFile,dwWinHeaderOffset,SEEK_SET) !=
        dwWinHeaderOffset)
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    if (lpCallBack->FileWrite(hFile,(LPBYTE)&stPEInfo,sizeof(stPEInfo)) !=
        sizeof(stPEInfo))
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    // Zero segment

    dwOffset = dwEndOfSectionTable - sizeof(NAVEX_IMAGE_SECTION_HEADER);
    if (lpCallBack->FileSeek(hFile,dwOffset,SEEK_SET) != dwOffset)
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    if (lpCallBack->FileWrite(hFile,(LPBYTE)&stSectionHeader,
        sizeof(stSectionHeader)) != sizeof(stSectionHeader))
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    // Zero out virus body

    if (lpCallBack->FileSeek(hFile,dwVirusSectionOffset,SEEK_SET) !=
        dwVirusSectionOffset)
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    dwTemp = 0;
    for (i=0;i<(0xA78 / 4 + 1);i++)
        if (lpCallBack->FileWrite(hFile,(LPBYTE)&dwTemp,sizeof(DWORD)) !=
            sizeof(DWORD))
        {
            RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
            return (EXTSTATUS_NO_REPAIR);
        }

    // Truncate file

    if (lpCallBack->FileSeek(hFile,dwVirusSectionOffset,SEEK_SET) !=
        dwVirusSectionOffset)
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    if (lpCallBack->FileWrite(hFile,lpbyWorkBuffer,0) != 0)
    {
        RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);
        return (EXTSTATUS_NO_REPAIR);
    }

    RepairFileClose(lpCallBack,lpszFileName,hFile,uAttr);

    return (EXTSTATUS_OK);
}

// Function:
//  WORD EXTRepairFileTent()
//
// Purpose: Removes the reference to the segment containing the virus
//          from the NE header, restores the header to its original state,
//          and then removes the segment with the virus from the file
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error

#define TENTMOVE    0x400               // Move this many bytes at a time
#define TENTLEN     1958                // Tentacle adds this much onto file

WORD EXTRepairFileTent(LPCALLBACKREV1   lpCallBack, // File op callbacks
                        LPN30           lpsN30, // Pointer to N30 structure
                        LPTSTR          lpszFileName,   // Infected filename
                        LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE       hFile;
    UINT        uAttr;
    WORD        i, j, x;                // Miscellaneous variables
    WORD        wFirstSeg, wDelSeg;     // First segment in file, delete seg
    WORD        wNumSegs, wSegsInBuf;   // Number of segments, Segments/Buffer
    WORD        wMove;                  // Amount of data to move per pass
    WORD        wCSIP[2];               // Place to store original CS & IP
    DWORD       dwx, dwWinHd;           // Misc variable, NE Header location
    DWORD       dwDataBeg, dwDataEnd;   // File offsets for data to move
    DWORD       dwWrite, dwToMove;      // Write offset, bytes to move
    EXEHEADER   FileHead;               // Place to hold fix bytes
    NEHEADER    FAR *WinHead;           // Windows NE header structure
    NESEGMENT_TABLE FAR *WinSeg;        // Windows segment table entry

    // _asm    int 3                    // Remove after debugging! &&T
                                        // Open file
    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);
                                        // Read EXE header for later fix
    if (sEXELARGE != FILEREAD(((LPBYTE) &FileHead), sEXELARGE))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (WINDOWS_NE_FORMAT != DetermineWindowsType(lpCallBack,
                                                    hFile,
                                                    lpbyWorkBuffer,
                                                    &dwWinHd))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    WinHead = (NEHEADER FAR *) lpbyWorkBuffer;  // This avoids lots of casts

    lpCallBack->FileSeek(hFile, -4, SEEK_END);  // Goto last 4 bytes
 
    if (4 != FILEREAD((LPBYTE) wCSIP, 4))       // Read original CS:IP
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = wCSIP[0];                               // Swap CS & IP for fix
    wCSIP[0] = wCSIP[1];                        // below
    wCSIP[1] = x;

    // Endianize the necessary fields to modify

    WinHead->neEntryOffset = WENDIAN(WinHead->neEntryOffset);
    WinHead->neSegTabNumb = WENDIAN(WinHead->neSegTabNumb);
    WinHead->neResourceOff = WENDIAN(WinHead->neResourceOff);
    WinHead->neResidentOff = WENDIAN(WinHead->neResidentOff);
    WinHead->neModRefOff = WENDIAN(WinHead->neModRefOff);
    WinHead->neImportOff = WENDIAN(WinHead->neImportOff);
    WinHead->neNonresOff = DWENDIAN(WinHead->neNonresOff);
 
    WinHead->neEntryOffset -= sNESEGMENT_TABLE; // Account for deletion of
    WinHead->neCSIP = *((LPDWORD) wCSIP);       // entry in segment table
    wDelSeg = WinHead->neSegTabNumb--;          // This is entry to delete
    WinHead->neResourceOff -= sNESEGMENT_TABLE; // Change offsets because
    WinHead->neResidentOff -= sNESEGMENT_TABLE; // everything will be moved
    WinHead->neModRefOff -= sNESEGMENT_TABLE;   // back after entry is
    WinHead->neImportOff -= sNESEGMENT_TABLE;   // deleted
    WinHead->neNonresOff -= sNESEGMENT_TABLE;
    WinHead->neExeFlags2 ^= 0x08;               // Clear this bit

    // Re-Endianize the necessary fields modified

    WinHead->neEntryOffset = WENDIAN(WinHead->neEntryOffset);
    WinHead->neSegTabNumb = WENDIAN(WinHead->neSegTabNumb);
    WinHead->neResourceOff = WENDIAN(WinHead->neResourceOff);
    WinHead->neResidentOff = WENDIAN(WinHead->neResidentOff);
    WinHead->neModRefOff = WENDIAN(WinHead->neModRefOff);
    WinHead->neImportOff = WENDIAN(WinHead->neImportOff);
    WinHead->neNonresOff = DWENDIAN(WinHead->neNonresOff);
 
    // Endianize

    FileHead.exMaxAlloc = WENDIAN(FileHead.exMaxAlloc);

    FileHead.exMaxAlloc++;                      // Tentacle decrements this
                                                // in the EXE header
    // Re-endianize

    FileHead.exMaxAlloc = WENDIAN(FileHead.exMaxAlloc);

    // The windows header gets written here so we can use the buffer to
    // load the segment table.   We also write the fixed DOS EXE header
    // while we're at it.

    if (0 != SEEK_TOF())                        // Goto TOF
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                                // Write fixed header
    if (sEXELARGE != FILEWRITE(((LPBYTE) &FileHead), sEXELARGE))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (dwWinHd != FILESEEK(dwWinHd))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                                // Write fixed Win header
    if (sNEHEADER != FILEWRITE(lpbyWorkBuffer, sNEHEADER))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    x = WENDIAN(WinHead->neShift);              // Save this for later
 
    wNumSegs = wDelSeg;                         // Setup to read segment
    dwDataBeg = dwWinHd + WENDIAN(WinHead->neSegTabOff); // table
    wSegsInBuf = TENTMOVE / sNESEGMENT_TABLE;
    wFirstSeg = 0xffff;

    if (dwDataBeg != FILESEEK(dwDataBeg))       // Seek to begin of seg table
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
 
    do
    {
        if (wNumSegs < wSegsInBuf)  // If segments left < Segments in buffer
        {
            wMove = wNumSegs * sNESEGMENT_TABLE;
            j = wNumSegs;
        }
        else
        {
            wMove = TENTMOVE;
            j = wSegsInBuf;
        }

        if (wMove != FILEREAD(lpbyWorkBuffer, wMove))   // Read a bufferload
            CLOSEOUT(EXTSTATUS_FILE_ERROR);             // of data
 
        WinSeg = (NESEGMENT_TABLE FAR *) lpbyWorkBuffer;    // Avoid casting
                                                            // and reset ptr
        for (i = j; i > 0; i--)
        {
            if ((WENDIAN(WinSeg->neSegDataSec) < wFirstSeg) &&
                                                (0 != WENDIAN(WinSeg->neSegDataSec)))
                wFirstSeg = WENDIAN(WinSeg->neSegDataSec);   // Find 1st seg in file

            WinSeg++;
        }

        wNumSegs -= j;
    } while (0 != wNumSegs);

    dwx = ((DWORD) wDelSeg) * sNESEGMENT_TABLE;
    dwDataBeg += dwx;                   // Find where data is to be moved from
    dwWrite = dwDataBeg - sNESEGMENT_TABLE;         // Data is written here

    dwDataEnd = ((DWORD) wFirstSeg) << x;           // X was WinHead->neShift
    dwToMove = dwDataEnd - dwDataBeg;

    do                      // Move data over deleted segment table entry
    {
        wMove = (TENTMOVE < dwToMove) ? TENTMOVE : (WORD) dwToMove;
 
        if (dwDataBeg != FILESEEK(dwDataBeg))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        if (wMove != FILEREAD(lpbyWorkBuffer, wMove))   // Read data to move
            CLOSEOUT(EXTSTATUS_FILE_ERROR);
 
        if (dwWrite != FILESEEK(dwWrite))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        if (wMove != FILEWRITE(lpbyWorkBuffer, wMove))  // Write data
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        dwToMove -= wMove;                              // Update pointers
        dwDataBeg += wMove;                             // and variables
        dwWrite += wMove;
    } while (0 != dwToMove);

    x = (0 == FILETRUNCATE(TENTLEN)) ? EXTSTATUS_OK : EXTSTATUS_FILE_ERROR;

    CLOSEOUT(x);
}   // EXTRepairFileTent

// Function:
//  WORD EXTRepairFileNewTent()
//
// Purpose: Removes the segment table entry for the segment containing the
//          virus, deletes the entries for the virus in the Module Reference
//          Table and Imported Name Table, and then updates all the other
//          offsets in the NE header.  Finally, this fixes the relocation
//          data in the segment containing the entry point.
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error

#define NEWTENTLEN  10634               // New Tentacle adds this much on file
#define SHELLSIZE   6                   // Size of 'SHELL' entry in ImpNmTbl
#define DATABLOCKS  3                   // Blocks of file data to move

WORD EXTRepairFileNewTent(LPCALLBACKREV1    lpCallBack, // File op callbacks
                            LPN30           lpsN30, // Pointer to N30 structure
                            LPTSTR          lpszFileName,   // Infected file
                            LPBYTE          lpbyWorkBuffer) // size >= 512
{
    HFILE       hFile;
    UINT        uAttr;
    WORD        i, j, x;                // Miscellaneous variables
    WORD        wFirstSeg, wDelSeg;     // First segment in file, delete seg
    WORD        wDelMod;                // Module Ref table entry to delete
    WORD        wNumSegs, wSegsInBuf;   // Number of segments, Segments/Buffer
    WORD        wModRef, wImpNmTbl;     // Offsets for ModRefTbl, ImpNmTbl
    WORD        wEntTblOff;             // Entry table offset
    WORD        wAlign;                 // Segment sector size for file
    WORD        wMove;                  // Amount of data to move per pass
    WORD        wSegFun[2];             // Segment/Function from fixup data
    DWORD       dwEntryFix;             // Offset of fixup data in entry seg
    DWORD       dwx, dwWinHd;           // Misc variable, NE Header location
    DWORD       dwDataBeg[DATABLOCKS];  // File offsets for three blocks of
    DWORD       dwDataEnd[DATABLOCKS];  // data to move
    DWORD       dwWrite[DATABLOCKS];    // File offset to write
    DWORD       dwToMove;               // Bytes to move
    EXEHEADER   FileHead;               // Place to hold fix bytes
    NEHEADER    FAR *WinHead;           // Windows NE header structure
    NESEGMENT_TABLE FAR *WinSeg;        // Windows segment table entry
    NEREL_IMP_NAME FAR  *WinFix;        // Windows (NE) fixup table entry

    // _asm    int 3                    // Remove after debugging!
                                        // Open file
    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);
                                        // Read EXE header for later fix
    if (sEXELARGE != FILEREAD(((LPBYTE) &FileHead), sEXELARGE))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (WINDOWS_NE_FORMAT != DetermineWindowsType(lpCallBack,
                                                    hFile,
                                                    lpbyWorkBuffer,
                                                    &dwWinHd))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    WinHead = (NEHEADER FAR *) lpbyWorkBuffer;  // This avoids lots of casts

    // Endianize the necessary fields before modifying

    WinHead->neEntryOffset = WENDIAN(WinHead->neEntryOffset);
    WinHead->neSegTabNumb = WENDIAN(WinHead->neSegTabNumb);
    WinHead->neModRefNumb = WENDIAN(WinHead->neModRefNumb);
    WinHead->neResourceOff = WENDIAN(WinHead->neResourceOff);
    WinHead->neResidentOff = WENDIAN(WinHead->neResidentOff);
    WinHead->neModRefOff = WENDIAN(WinHead->neModRefOff);
    WinHead->neImportOff = WENDIAN(WinHead->neImportOff);
    WinHead->neNonresOff = DWENDIAN(WinHead->neNonresOff);

    wEntTblOff = WinHead->neEntryOffset;        // Account for deletion of
    WinHead->neEntryOffset -= (sNESEGMENT_TABLE + sizeof(WORD) + SHELLSIZE);
    wDelSeg = WinHead->neSegTabNumb--;          // entries in segment, module
    wDelMod = WinHead->neModRefNumb--;          // reference, and import name
    WinHead->neResourceOff -= sNESEGMENT_TABLE; // tables.
    WinHead->neResidentOff -= sNESEGMENT_TABLE; // Change offsets because
    wModRef = WinHead->neModRefOff;             // everything will be moved
    WinHead->neModRefOff -= sNESEGMENT_TABLE;   // back after entry is
    wImpNmTbl = WinHead->neImportOff;           // deleted
    WinHead->neImportOff -= (sNESEGMENT_TABLE + sizeof(WORD));
    WinHead->neNonresOff -= (sNESEGMENT_TABLE + sizeof(WORD) + SHELLSIZE);
    wAlign = WENDIAN(WinHead->neShift);
    WinHead->neExeFlags2 ^= 0x08;               // Clear this bit

    // Re-Endianize the necessary fields after modifying

    WinHead->neEntryOffset = WENDIAN(WinHead->neEntryOffset);
    WinHead->neSegTabNumb = WENDIAN(WinHead->neSegTabNumb);
    WinHead->neModRefNumb = WENDIAN(WinHead->neModRefNumb);
    WinHead->neResourceOff = WENDIAN(WinHead->neResourceOff);
    WinHead->neResidentOff = WENDIAN(WinHead->neResidentOff);
    WinHead->neModRefOff = WENDIAN(WinHead->neModRefOff);
    WinHead->neImportOff = WENDIAN(WinHead->neImportOff);
    WinHead->neNonresOff = DWENDIAN(WinHead->neNonresOff);

    // Endianize

    FileHead.exMaxAlloc = WENDIAN(FileHead.exMaxAlloc);

    FileHead.exMaxAlloc++;                      // Tentacle decrements this
                                                // in the EXE header
    // Re-endianize

    FileHead.exMaxAlloc = WENDIAN(FileHead.exMaxAlloc);

    // The windows header gets written here so we can use the buffer to
    // load the segment table.   We also write the fixed DOS EXE header
    // while we're at it.

    if (0 != SEEK_TOF())                        // Goto TOF
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                                // Write fixed header
    if (sEXELARGE != FILEWRITE(((LPBYTE) &FileHead), sEXELARGE))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (dwWinHd != FILESEEK(dwWinHd))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                                // Write fixed Win header
    if (sNEHEADER != FILEWRITE(lpbyWorkBuffer, sNEHEADER))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    dwDataBeg[0] = (DWENDIAN(WinHead->neCSIP) >> 16) - 1; // Get entry pt seg offset
    dwDataBeg[0] *= sNESEGMENT_TABLE;           // into segment table

    dwx = dwWinHd + WENDIAN(WinHead->neSegTabOff);    // Get seg table offset
    dwDataBeg[0] += dwx;

    if (dwDataBeg[0] != FILESEEK(dwDataBeg[0])) // Seek to seg table entry
        CLOSEOUT(EXTSTATUS_FILE_ERROR);         //  for entry point
 
    // Endian-dependency here!!!!
    // We read a value into the first 16 bits of a DWORD, and assume this
    // is the least-significant half.
 
    dwEntryFix = 0;
 
    if (sizeof(WORD) != FILEREAD((LPBYTE) &dwEntryFix, sizeof(WORD)))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);         // Read offset to entry seg

    // Endianize

    dwEntryFix = DWENDIAN(dwEntryFix);

    dwEntryFix <<= wAlign;                      // Convert to byte offset

    if (sizeof(WORD) != FILEREAD((LPBYTE) &x, sizeof(WORD)))    // Get bytes
       CLOSEOUT(EXTSTATUS_FILE_ERROR);                  // of code in seg

    dwEntryFix += WENDIAN(x);                   // Point to end of code
 
    wNumSegs = wDelSeg;                         // Setup to read segment
    dwDataBeg[0] = dwx;                         // table (restore saved value)
    wSegsInBuf = TENTMOVE / sNESEGMENT_TABLE;
    wFirstSeg = 0xffff;

    if (dwDataBeg[0] != FILESEEK(dwDataBeg[0])) // Seek to begin of seg table
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
 
    do
    {
        if (wNumSegs < wSegsInBuf)  // If segments left < Segments in buffer
        {
            wMove = wNumSegs * sNESEGMENT_TABLE;
            j = wNumSegs;
        }
        else
        {
            wMove = TENTMOVE;
            j = wSegsInBuf;
        }

        if (wMove != FILEREAD(lpbyWorkBuffer, wMove))   // Read a bufferload
            CLOSEOUT(EXTSTATUS_FILE_ERROR);             // of data
 
        WinSeg = (NESEGMENT_TABLE FAR *) lpbyWorkBuffer;    // Avoid casting
                                                            // and reset ptr
        for (i = j; i > 0; i--)
        {
            if ((WENDIAN(WinSeg->neSegDataSec) < wFirstSeg) &&
                                                (0 != WENDIAN(WinSeg->neSegDataSec)))
                wFirstSeg = WENDIAN(WinSeg->neSegDataSec);   // Find 1st seg in file

            WinSeg++;
        }

        wNumSegs -= j;
    } while (0 != wNumSegs);

    // This loop moves data back in three blocks to compensate for the
    // deletion of an entry in the segment table (8 bytes), an entry in the
    // module reference table (2 bytes), and an entry in the non-resident
    // name table (6 bytes).  Moving the data back overwrites the entries
    // in the appropriate table, thereby deleting them.
 
    // Setup to move data over the deleted segment table entry
    // Move data back by sNESEGMENT_TABLE bytes (currently 8)
 
    dwx = ((DWORD) wDelSeg) * sNESEGMENT_TABLE; // Compute size of seg table
    dwDataBeg[0] += dwx;            // DataBeg now points to end of seg table
 
    dwx = sNESEGMENT_TABLE;         // dwx holds number of bytes to move back
    dwWrite[0] = dwDataBeg[0] - dwx;            // Data is written here

    // This moves data back by (sNESEGMENT_TABLE + sizeof(WORD)) bytes
    // (currently 10) because of the segment table entry deletion, and the
    // deletion of the last entry in the module reference table.  Data from
    // the beginning of the imported name table to 16 bytes before the non-
    // resident name table are moved.
 
    dwDataBeg[1] = dwWinHd + wImpNmTbl;
    dwDataEnd[0] = dwDataBeg[1] - sizeof(WORD);

    dwx += sizeof(WORD);            // Ends at next-to-last ModRefTbl entry
    dwWrite[1] = dwDataBeg[1] - dwx;    // Write at end of previous data
                                        // block
    // This moves data back by 16 bytes, from the beginning of the non-
    // resident name table to the beginning of the first segment in the
    // file.  This overwrites the last entry in the imported name table,
    // which takes up 6 bytes.
 
    dwDataBeg[2] = dwWinHd + wEntTblOff;
    dwDataEnd[1] = dwDataBeg[2] - SHELLSIZE;
 
    dwx += SHELLSIZE;
    dwWrite[2] = dwDataBeg[2] - dwx;
 
    dwDataEnd[2] = ((DWORD) wFirstSeg) << wAlign;   // End at beginning
 
    for (i = 0; i < DATABLOCKS; i++)
    {
        dwToMove = dwDataEnd[i] - dwDataBeg[i];

        while (0 != dwToMove)   // Move data over deleted segment table entry
        {
            wMove = (TENTMOVE < dwToMove) ? TENTMOVE : (WORD) dwToMove;
 
            if (dwDataBeg[i] != FILESEEK(dwDataBeg[i]))
                CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                                        // Read data to move
            if (wMove != FILEREAD(lpbyWorkBuffer, wMove))
                CLOSEOUT(EXTSTATUS_FILE_ERROR);
 
            if (dwWrite[i] != FILESEEK(dwWrite[i]))
                CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                                        // Write data
            if (wMove != FILEWRITE(lpbyWorkBuffer, wMove))
                CLOSEOUT(EXTSTATUS_FILE_ERROR);

            dwToMove -= wMove;                          // Update pointers
            dwDataBeg[i] += wMove;                      // and variables
            dwWrite[i] += wMove;
        }
    }

    lpCallBack->FileSeek(hFile, -4, SEEK_END);  // Goto last 4 bytes
 
    if (4 != FILEREAD((LPBYTE) wSegFun, 4))     // Read original fixup data
        CLOSEOUT(EXTSTATUS_NO_REPAIR);          // 1st = seg / 2nd = function

    x = EXTSTATUS_OK;                                   // Save for later
 
    // This stuff looks for the relocation/fixup table entry that points
    // to the viral code
 
    if (dwEntryFix != FILESEEK(dwEntryFix))     // Seek to fixup data for
        CLOSEOUT(EXTSTATUS_FILE_ERROR);         //  entry segment

    // Variables from the segment table searching are reused here.  Their
    // functions are the same, but their names are a little funky here.
 
    if (sizeof(WORD) != FILEREAD((LPBYTE) &wNumSegs, sizeof(WORD)))
       CLOSEOUT(EXTSTATUS_FILE_ERROR);          // Get number of fixup items

    wNumSegs = WENDIAN(wNumSegs);

    dwEntryFix += sizeof(WORD);                 // Keep track of current pos
    wSegsInBuf = TENTMOVE / sNEREL_IMP_NAME;

    do
    {
        if (wNumSegs < wSegsInBuf)  // If fixups left < fixups in buffer
        {
            wMove = wNumSegs * sNEREL_IMP_NAME;
            j = wNumSegs;
        }
        else
        {
            wMove = TENTMOVE;
            j = wSegsInBuf;
        }

        if (wMove != FILEREAD(lpbyWorkBuffer, wMove))   // Read a bufferload
            CLOSEOUT(EXTSTATUS_FILE_ERROR);             // of data
 
        WinFix = (NEREL_IMP_NAME FAR *) lpbyWorkBuffer;     // Avoid casting
                                                            // and reset ptr
        i = j;
 
        do
        {                   // Find fixup entry that points to viral code seg
            if ((wDelSeg == WENDIAN(WinFix->neIndexModule)) &&
                (0 == WinFix->neRelType) &&
                (3 == WinFix->neAddressType))
            {
                WinFix->neRelType++;        // Change from 0 to 1

                WinFix->neIndexModule = wSegFun[0]; // Get original fixup
                WinFix->neOrdalNumber = wSegFun[1]; // data from EOF

                if (dwEntryFix != FILESEEK(dwEntryFix)) // Seek to rewrite
                    CLOSEOUT(EXTSTATUS_FILE_ERROR);     // this entry

                if (sNEREL_IMP_NAME !=                  // Rewrite this entry
                                FILEWRITE((LPBYTE) WinFix, sNEREL_IMP_NAME))
                    CLOSEOUT(EXTSTATUS_FILE_ERROR);

                i = 1;                                  // This makes us quit
                wNumSegs = j;
                WinFix = NULL;                          // This flags success
            }
            else
            {
                WinFix++;
                dwEntryFix += sNEREL_IMP_NAME;
            }
        } while (0 != --i);

        wNumSegs -= j;
    } while (0 != wNumSegs);

    if (NULL != WinFix)                         // x is already EXTSTATUS_OK
        x = EXTSTATUS_NO_REPAIR;
    else if (0 != FILETRUNCATE(NEWTENTLEN))
        x = EXTSTATUS_FILE_ERROR;

    CLOSEOUT(x);
}   // EXTRepairFileNewTent

// Function:
//  WORD EXTRepairFileTpvo()
//
// Purpose: Removes the reference to the segment containing the virus
//          from the NE header, restores the header to its original state,
//          and then removes the segment with the virus from the file
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_NO_REPAIR   if unable to repair
//                        EXTSTATUS_FILE_ERROR  if file or disk error

#define TPVOMOVE    0x400               // Move this many bytes at a time
#define TPVOLEN      3783               // Tvpo adds this much onto file

WORD EXTRepairFileTpvo(LPCALLBACKREV1   lpCallBack, // File op callbacks
                        LPN30           lpsN30, // Pointer to N30 structure
                        LPTSTR          lpszFileName,   // Infected filename
                        LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE       hFile;
    UINT        uAttr;
    WORD        i, j, x;                // Miscellaneous variables
    WORD        wFirstSeg, wDelSeg;     // First segment in file, delete seg
    WORD        wNumSegs, wSegsInBuf;   // Number of segments, Segments/Buffer
    WORD        wMove, wToMove, wSegNum;// Amount of data to move per pass
    WORD        wCSIP[2];               // Place to store original CS & IP
    DWORD       dwx, dwWinHd;           // Misc variable, NE Header location
    DWORD       dwDataBeg;              // File offsets for data to move
    DWORD       dwWrite;                // Write offset, bytes to move
    EXEHEADER   FileHead;               // Place to hold fix bytes
    NEHEADER    FAR *WinHead;           // Windows NE header structure
    NESEGMENT_TABLE FAR *WinSeg;        // Windows segment table entry


    // _asm    int 3                    // Remove after debugging! &&T

                                        // Open file
    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    lpCallBack->FileSeek(hFile, -64, SEEK_END);  // Goto last 64 bytes

                                        // Read EXE header for later fix
    if (sEXELARGE != FILEREAD(((LPBYTE) &FileHead), sEXELARGE))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (WINDOWS_NE_FORMAT != DetermineWindowsType(lpCallBack,
                                                    hFile,
                                                    lpbyWorkBuffer,
                                                    &dwWinHd))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);


    WinHead = (NEHEADER FAR *) lpbyWorkBuffer;  // This avoids lots of casts

    lpCallBack->FileSeek(hFile, -68, SEEK_END);  // Goto last 68 bytes
 
    if (4 != FILEREAD((LPBYTE) wCSIP, 4))       // Read original CS:IP
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = wCSIP[0];                               // Swap CS & IP for fix
    wCSIP[0] = wCSIP[1];                        // below
    wCSIP[1] = x;


    // Endianize the necessary fields to modify
    WinHead->neEntryOffset = WENDIAN(WinHead->neEntryOffset);
    WinHead->neSegTabNumb = WENDIAN(WinHead->neSegTabNumb);
    WinHead->neResourceOff = WENDIAN(WinHead->neResourceOff);
    WinHead->neResidentOff = WENDIAN(WinHead->neResidentOff);
    WinHead->neModRefOff = WENDIAN(WinHead->neModRefOff);
    WinHead->neImportOff = WENDIAN(WinHead->neImportOff);
 
    WinHead->neEntryOffset -= sNESEGMENT_TABLE;
    WinHead->neCSIP = *((LPDWORD) wCSIP);
    wDelSeg = WinHead->neSegTabNumb--;
    WinHead->neResourceOff -= sNESEGMENT_TABLE;
    WinHead->neResidentOff -= sNESEGMENT_TABLE;
    WinHead->neModRefOff -= sNESEGMENT_TABLE;
    WinHead->neImportOff -= sNESEGMENT_TABLE;

    // Re-Endianize the necessary fields modified

    WinHead->neEntryOffset = WENDIAN(WinHead->neEntryOffset);
    WinHead->neSegTabNumb = WENDIAN(WinHead->neSegTabNumb);
    WinHead->neResourceOff = WENDIAN(WinHead->neResourceOff);
    WinHead->neResidentOff = WENDIAN(WinHead->neResidentOff);
    WinHead->neModRefOff = WENDIAN(WinHead->neModRefOff);
    WinHead->neImportOff = WENDIAN(WinHead->neImportOff);
 

    // The windows header gets written here so we can use the buffer to
    // load the segment table.   We also write the fixed DOS EXE header
    // while we're at it.

    if (0 != SEEK_TOF())                        // Goto TOF
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                                // Write fixed header
    if (sEXELARGE != FILEWRITE(((LPBYTE) &FileHead), sEXELARGE))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (dwWinHd != FILESEEK(dwWinHd))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (sNEHEADER != FILEWRITE(lpbyWorkBuffer, sNEHEADER))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    x = WENDIAN(WinHead->neShift);              // Save this for later

    wNumSegs = wDelSeg;                         // Setup to read segment
    dwDataBeg = dwWinHd + WENDIAN(WinHead->neSegTabOff); // table
    wSegsInBuf = TPVOMOVE / sNESEGMENT_TABLE;
    wFirstSeg = 0xffff;

    if (dwDataBeg != FILESEEK(dwDataBeg))       // Seek to begin of seg table
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    do
    {
        if (wNumSegs < wSegsInBuf)  // If segments left < Segments in buffer
        {
            wMove = wNumSegs * sNESEGMENT_TABLE;
            j = wNumSegs;
        }
        else
        {
            wMove = TPVOMOVE;
            j = wSegsInBuf;
        }

        if (wMove != FILEREAD(lpbyWorkBuffer, wMove))   // Read a bufferload
            CLOSEOUT(EXTSTATUS_FILE_ERROR);             // of data

        WinSeg = (NESEGMENT_TABLE FAR *) lpbyWorkBuffer;    // Avoid casting
                                                            // and reset ptr
        for (i = j; i > 0; i--)
        {
            if ((WENDIAN(WinSeg->neSegDataSec) < wFirstSeg) &&
                                                (0 != WENDIAN(WinSeg->neSegDataSec)))
                wFirstSeg = WENDIAN(WinSeg->neSegDataSec);   // Find 1st seg in file

            WinSeg++;
        }

        wNumSegs -= j;
    } while (0 != wNumSegs);


                        // Find data to move over deleted segment table entry
    dwWinHd = dwWinHd + 0x22;

    if (dwWinHd != FILESEEK(dwWinHd))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (2 != FILEREAD((LPBYTE) &wToMove, 2))       // Read Seg Table offset
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    dwWinHd = dwWinHd - 0x6;

    if (dwWinHd != FILESEEK(dwWinHd))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (2 != FILEREAD((LPBYTE) &wSegNum, 2))       // Read Entries in Seg Table
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    dwWinHd = dwWinHd - 0x1C;

    dwx = 0;
    dwx = wToMove + (wSegNum * 8);
    wToMove = wToMove + (wSegNum * 8);
    dwDataBeg = dwWinHd + wToMove;             // Find where data is to be moved from

    if (dwx != wToMove)                        // If more than a Word value
        CLOSEOUT(EXTSTATUS_FILE_ERROR);        // don't repair. Rare possibility.

    do                      // Move data over deleted segment table entry
    {
        wMove = (TENTMOVE < wToMove) ? TENTMOVE : wToMove;
        dwDataBeg = dwDataBeg - wMove;
        dwWrite = dwDataBeg + 8;

        if (dwDataBeg != FILESEEK(dwDataBeg))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        if (wMove != FILEREAD(lpbyWorkBuffer, wMove))   // Read data to move
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        if (dwWrite != FILESEEK(dwWrite))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        if (wMove != FILEWRITE(lpbyWorkBuffer, wMove))  // Write data
            CLOSEOUT(EXTSTATUS_FILE_ERROR);


        wToMove = wToMove - wMove;                              // Update pointers
    } while (0 != wToMove);

    x = (0 == FILETRUNCATE(TPVOLEN)) ? EXTSTATUS_OK : EXTSTATUS_FILE_ERROR;

    CLOSEOUT(x);
}   // EXTRepairFileTpvo

#endif  // #ifndef SYM_NLM
