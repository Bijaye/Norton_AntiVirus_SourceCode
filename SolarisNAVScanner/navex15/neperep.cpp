//************************************************************************
//
// $Header:   S:/NAVEX/VCS/neperep.cpv   1.59   14 Jan 1999 18:05:08   DKESSNE  $
//
// Description:
//      Source file for Windows virus repair.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/neperep.cpv  $
// 
//    Rev 1.59   14 Jan 1999 18:05:08   DKESSNE
// added repair for W95.Memorial, CI 11108
// 
//    Rev 1.58   28 Dec 1998 11:21:24   relniti
// REMOVE #ifdef SARC_REMOTEEXPLORER
// 
//    Rev 1.57   24 Dec 1998 23:05:52   DCHI
// Added EXTRepairFileRemoteXCorrupt() implementation.
// 
//    Rev 1.56   24 Dec 1998 05:56:40   relniti
// W32.RemoteExplore repair code
// 
//    Rev 1.55   17 Dec 1998 14:52:38   MMAGEE
// removed #ifdef/endif for SARC_CUST_9029;  W95.K32;  vid 0x25d1
// 
//    Rev 1.54   11 Dec 1998 13:25:42   DKESSNE
// added repair for W95.K32
// 
//    Rev 1.53   09 Dec 1998 12:58:44   MMAGEE
// removed #ifdef/endif for SARC_CUST_7557
// removed #ifdef/endif for SARC_CUST_7982
// 
//    Rev 1.52   24 Nov 1998 12:48:00   DKESSNE
// added repair function for W95.Parvo
// 
//    Rev 1.51   17 Nov 1998 18:54:34   DKESSNE
// added repair function for W95.Padania 0x25be, CI 7557
// 
//    Rev 1.50   17 Nov 1998 09:59:48   AOONWAL
// Removed #ifdef SARC_CUST_6689
// 
//    Rev 1.49   12 Nov 1998 19:07:12   DKESSNE
// added repair function for W95.HPS (CI 6689)
// 
//    Rev 1.48   11 Nov 1998 11:35:22   MMAGEE
// removed #ifdef/endif for SARC_CST_5650
// 
//    Rev 1.47   14 Oct 1998 15:09:00   DKESSNE
// added repair function for W95.Cabanas
// 
//    Rev 1.46   14 Oct 1998 12:19:06   MMAGEE
// removed #ifdef/endif for SARC_CST_4989; vid 258b (W95.Inca)
// 
//    Rev 1.45   30 Sep 1998 17:30:26   DKESSNE
// added Inca repair for CI 4989
// 
//    Rev 1.44   10 Sep 1998 14:54:28   MMAGEE
// removed #ifdef/endif for SARC_CUST_4213
// 
//    Rev 1.43   09 Sep 1998 20:52:00   JWILBER
// Added repair for RedTeam, VID 6130, for CI 4213.
//
//    Rev 1.42   09 Sep 1998 14:46:22   JWILBER
// Removed #defines for MARKJ repair for Maryl and Abid.
//
//    Rev 1.41   09 Sep 1998 11:54:34   AOONWAL
// Modified Redteam repair to initialized dwFileLen variable before using it.
//
//    Rev 1.40   09 Sep 1998 04:32:54   JWILBER
// Added repair for RedTeam, VID 6130, for CI 4213.
//
//    Rev 1.39   01 Sep 1998 13:08:26   DKESSNE
// added W95.MarkJ repair function
//
//    Rev 1.38   13 Aug 1998 19:43:48   PDEBATS
// emoved ifdef/endif, ifdef: else/endif block, for SARC_CST_MARBURGB VID61A6
//
//    Rev 1.37   13 Aug 1998 13:35:28   jwilber
// Made modification to Marburg repair to also repair Marburg.B.
// Mods are #ifdef'd with SARC_CST_MARBURGB.
//
//    Rev 1.36   05 Aug 1998 15:08:50   DKESSNE
// fix for lack of callback->memalloc in Pyro
//
//    Rev 1.35   03 Aug 1998 13:26:00   PDEBATS
// removed if/endif for sarc_marburg cst2291 vid 2566
//
//    Rev 1.34   27 Jul 1998 14:46:26   DKESSNE
// deleted some declarations to avoid errors in w32
//
//    Rev 1.33   27 Jul 1998 14:20:50   DKESSNE
// ifdef'd Marburg repair function
//
//    Rev 1.32   27 Jul 1998 12:34:52   DKESSNE
// changed order of ifdef'd includes: config.h and pespscan.h
//
//    Rev 1.31   27 Jul 1998 12:20:12   DKESSNE
// added repair function for W95.Marburg
//
//    Rev 1.30   08 Jul 1998 11:09:58   DKESSNE
// added check for overwriting lpbyWorkBuffer in case of corrupted file
//
//    Rev 1.29   01 Jul 1998 16:30:42   DKESSNE
// modified repair for W95.CIH, to adjust for variable virus data size
//
//    Rev 1.28   30 Jun 1998 11:06:46   DKESSNE
// changed repair loop in W95.CIH to enable repair of a second variant of
// the virus, added some comments
//
//    Rev 1.27   29 Jun 1998 15:01:34   DKESSNE
// added repair function for W95.CIH 0x617e
//
//    Rev 1.26   19 Mar 1998 16:42:00   FBARAJA
// Removed #ifdef SARC_CST_1490
//
//    Rev 1.25   19 Mar 2000 11:24:54   hjaland
// Added 1490 for further testing.
//
//    Rev 1.24   18 Mar 2000 11:14:08   hjaland
// Removed #ifdef SARC_CST_1490
//
//    Rev 1.23   12 Mar 1998 20:19:10   RELNITI
// ADD EXTFileRepKlunky for 1490
//
//    Rev 1.22   11 Mar 1998 20:51:12   FBARAJA
// Removed #ifdef SARC_CST_1462 (HJ)
//
//    Rev 1.21   11 Mar 1998 13:48:12   JWILBER
// Uncommented #ifdef SARC_CST_1462 statements around EXTFileRepairAppar,
// and added #ifdef SARCBOT \ #include "config.h" for QA builds.
//
//    Rev 1.20   10 Mar 1998 18:06:10   JWILBER
// Added repair for Apparition.89021, VID 1ca3, CI 1462.
// Function name is EXTRepairAppar.
//
//    Rev 1.19   10 Nov 1997 13:26:30   JWILBER
// Added IsEinstein32() function, with a call to it from
// EXTRepairFilePoppy(), because Einstein32 files get mangled
// by Poppy.1536.
//
//    Rev 1.18   04 Nov 1997 20:52:38   JWILBER
// Changed a PNAVEX_IMAGE_SECTION_HEADER cast to a
// LPNAVEX_IMAGE_SECTION_HEADER cast to make Win16 happy with
// the repair for Poppy.1536.
//
//    Rev 1.17   04 Nov 1997 14:00:56   JWILBER
// Added EXTRepairFilePoppy for real - no stub.
//
//    Rev 1.16   04 Nov 1997 10:52:22   JWILBER
// Added stub for EXTRepairFilePoppy, since NAVEXRF.CPP refers
// to this function, and it's still under construction.
//
//    Rev 1.15   10 Jul 1997 17:43:38   DDREW
// Turn on NLM repairs for NAVEX15
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

#if defined(NAVEX15) || !defined(SYM_NLM)

#include "endutils.h"

#include "neperep.h"

#include "winconst.h"

#include "nepeshr.h"

#include "pespscan.h"


#ifdef SARCBOT
#include "config.h"
#endif

#include "remotex.h"

WORD IsEinstein32(LPNAVEX_IMAGE_NT_HEADERS,
                    DWORD,
                    LPCALLBACKREV1,
                    HFILE,
                    LPBYTE,
                    WORD);

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

// Function:
//  WORD EXTRepairFilePoppy()
//
// Purpose: Fixes up the PE header as much as possible to restore its pre-
//          infection state.  Since the virus infects that last segment in
//          the segment table, this is also fixed up.  Finally the virus is
//          overwritten and truncated as needed from the file.
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

#define POPVSIZE    0x1000      // Increase in virtual size of Poppy segment
#define POPRSIZE    0x0600      // Increase in raw data size
#define POPEP_OFF   0xac        // Offset of original EP from entry point
#define POPBOOKMK   0xba600137  // Bookmark for Poppy
#define POPWRTBLK   0x100       // Size of block of zeros to overwrite virus

WORD EXTRepairFilePoppy(LPCALLBACKREV1  lpCallBack, // File op callbacks
                        LPN30           lpsN30,     // Ptr to N30 structure
                        LPTSTR          lpszFileName,   // Infected filename
                        LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE                       hFile;
    UINT                        uAttr;
    WORD                        x;          // Misc variable
    WORD                        wSegs;      // Number of Segments in File
    DWORD                       dwx, dwy;   // Misc variables
    DWORD                       dwTrunc;    // Bytes to truncate from file
    DWORD                       dwVirSeg;   // Offset of Virus Seg Tab Entry
    DWORD                       dwLastSeg;  // Offset of Last Seg
    DWORD                       dwEndSegTab;    // Offset of end of seg table
    DWORD                       dwBookMk;   // Offset of Entry Point
    DWORD                       dwSecLen;   // Length of Last Segment
    DWORD                       dwWinHd;    // PE Hdr loc
    DWORD                       dwFixBook[2];   // Fix Data & Bookmark
    LPBYTE                      lpbyTmp;    // Temporary pointer
    NAVEX_IMAGE_NT_HEADERS      stPEInfo;   // PE Header Info
    NAVEX_IMAGE_SECTION_HEADER  stPESeg;    // Virus Segment Info

    // _asm    int 3            // &&P

    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    // Get header

    if (WINDOWS_PE_FORMAT != DetermineWindowsType(lpCallBack,
                                                    hFile,
                                                    lpbyWorkBuffer,
                                                    &dwWinHd))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    stPEInfo = *((LPNAVEX_IMAGE_NT_HEADERS) lpbyWorkBuffer);

    // Endianize header for computations

    stPEInfo.OptionalHeader.AddressOfEntryPoint =
                        DWENDIAN(stPEInfo.OptionalHeader.AddressOfEntryPoint);
    stPEInfo.OptionalHeader.ImageBase =
                        DWENDIAN(stPEInfo.OptionalHeader.ImageBase);
    stPEInfo.OptionalHeader.SectionAlignment =
                        DWENDIAN(stPEInfo.OptionalHeader.SectionAlignment);
    stPEInfo.OptionalHeader.FileAlignment =
                        DWENDIAN(stPEInfo.OptionalHeader.FileAlignment);
    stPEInfo.OptionalHeader.SizeOfImage =
                        DWENDIAN(stPEInfo.OptionalHeader.SizeOfImage);

    // Check for corruption

    if (stPEInfo.OptionalHeader.FileAlignment == 0 ||
                                stPEInfo.OptionalHeader.SectionAlignment == 0)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    // Corruption also if debug info present
                // No endianization on this because we're checking for zero!
    if (stPEInfo.OptionalHeader.
                DataDirectory[NAVEX_IMAGE_DIRECTORY_ENTRY_DEBUG].Size != 0)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    wSegs = WENDIAN(stPEInfo.FileHeader.NumberOfSections);

    dwEndSegTab = dwWinHd + sizeof(NAVEX_IMAGE_NT_HEADERS) +
                                wSegs * sizeof(NAVEX_IMAGE_SECTION_HEADER);

    if ((dwEndSegTab % stPEInfo.OptionalHeader.FileAlignment)
                    <= sizeof(NAVEX_IMAGE_SECTION_HEADER) &&
                (dwEndSegTab % stPEInfo.OptionalHeader.FileAlignment) != 0)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    // Check to see if this is an Einstein32 file

    x = IsEinstein32(&stPEInfo,
                        dwWinHd,
                        lpCallBack,
                        hFile,
                        lpbyWorkBuffer,
                        wSegs--);

    // Note the '--' after wSegs above!!!!  Converts from number of segments
    //  to zero-based segment number for last segment.

    if (0 != x)
    {
        FILECLOSE();

        return((1 == x) ? EXTSTATUS_NO_REPAIR : EXTSTATUS_FILE_ERROR);
    }

    // Get segment table data for last segment, where the virus is.

    if (EXTSTATUS_OK != GetPESectionInfo(lpCallBack,
                            hFile,
                            lpbyWorkBuffer,
                            dwWinHd,
                            wSegs,
                            &dwLastSeg,
                            &dwSecLen,
                            &dwx,
                            &dwy))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    stPESeg = *((LPNAVEX_IMAGE_SECTION_HEADER) lpbyWorkBuffer);

    // Endianize segment table entries for computations

    stPESeg.Misc.VirtualSize = DWENDIAN(stPESeg.Misc.VirtualSize);
    stPESeg.SizeOfRawData = DWENDIAN(stPESeg.SizeOfRawData);

    // Compute file offset of last segment table entry

    dwVirSeg = dwWinHd + sizeof(NAVEX_IMAGE_NT_HEADERS) +
                                wSegs * sizeof(NAVEX_IMAGE_SECTION_HEADER);

    dwBookMk = dwLastSeg + stPESeg.Misc.VirtualSize - POPVSIZE + POPEP_OFF;

    // Read/bookmark virus

    if (dwBookMk != FILESEEK(dwBookMk))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = 2 * sizeof(DWORD);

    if (x != FILEREAD((LPBYTE) dwFixBook, x))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    dwFixBook[0] = DWENDIAN(dwFixBook[0]);
    dwFixBook[1] = DWENDIAN(dwFixBook[1]);

    if (POPBOOKMK != dwFixBook[1])
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    // Compute and store the original entry point - subtract the image base

    stPEInfo.OptionalHeader.AddressOfEntryPoint =
                            dwFixBook[0] - stPEInfo.OptionalHeader.ImageBase;

    // Fix up virtual, raw, and image sizes

    dwTrunc = stPESeg.SizeOfRawData;    // Use to compute truncation length

    stPESeg.Misc.VirtualSize -= POPVSIZE;

    dwx = stPESeg.Misc.VirtualSize % stPEInfo.OptionalHeader.FileAlignment;

    stPESeg.SizeOfRawData = stPESeg.Misc.VirtualSize;

    if (0 != dwx)
        stPESeg.SizeOfRawData +=
                                (stPEInfo.OptionalHeader.FileAlignment - dwx);

    dwTrunc -= stPESeg.SizeOfRawData;   // Use difference between old and new
                                        //  values to find amount to truncate
    dwx = stPEInfo.OptionalHeader.SizeOfImage %
                                    stPEInfo.OptionalHeader.SectionAlignment;
    if (POPVSIZE < dwx)
        dwx -= POPVSIZE;

    stPEInfo.OptionalHeader.SizeOfImage -= dwx;

    // Fix up segment name - Poppy changes last char of name to 0xff

    if (0xff == stPESeg.Name[NAVEX_IMAGE_SIZEOF_SHORT_NAME - 1])
        stPESeg.Name[NAVEX_IMAGE_SIZEOF_SHORT_NAME - 1] = 0;

    // Unendianize for storing back to disk

    stPEInfo.OptionalHeader.AddressOfEntryPoint =
                        DWENDIAN(stPEInfo.OptionalHeader.AddressOfEntryPoint);
    stPEInfo.OptionalHeader.ImageBase =
                        DWENDIAN(stPEInfo.OptionalHeader.ImageBase);
    stPEInfo.OptionalHeader.SectionAlignment =
                        DWENDIAN(stPEInfo.OptionalHeader.SectionAlignment);
    stPEInfo.OptionalHeader.FileAlignment =
                        DWENDIAN(stPEInfo.OptionalHeader.FileAlignment);
    stPEInfo.OptionalHeader.SizeOfImage =
                        DWENDIAN(stPEInfo.OptionalHeader.SizeOfImage);

    stPESeg.Misc.VirtualSize = DWENDIAN(stPESeg.Misc.VirtualSize);
    stPESeg.SizeOfRawData = DWENDIAN(stPESeg.SizeOfRawData);

    // Write fixes back to disk

    if (dwWinHd != FILESEEK(dwWinHd))       // Write new PE header
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (sizeof(NAVEX_IMAGE_NT_HEADERS) !=
                        FILEWRITE(&stPEInfo, sizeof(NAVEX_IMAGE_NT_HEADERS)))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (dwVirSeg != FILESEEK(dwVirSeg))     // Write new segment table entry
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (sizeof(NAVEX_IMAGE_SECTION_HEADER) !=
                    FILEWRITE(&stPESeg, sizeof(NAVEX_IMAGE_SECTION_HEADER)))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    // Re-endianize for ops below

    stPESeg.Misc.VirtualSize = DWENDIAN(stPESeg.Misc.VirtualSize);
    stPESeg.SizeOfRawData = DWENDIAN(stPESeg.SizeOfRawData);

    // Overwrite virus code w/ zeros

    dwx = stPESeg.SizeOfRawData - stPESeg.Misc.VirtualSize; // Number of zeros
                                                            //  to write
    lpbyTmp = lpbyWorkBuffer;   // Make string of zeros to minimize FILEWRITE
                                //  calls
    dwy = (dwx > POPWRTBLK) ? POPWRTBLK : dwx;

    for (x = 0; x < dwy; x++)
        *lpbyTmp++ = 0;

    dwLastSeg += stPESeg.Misc.VirtualSize;      // Where to write

    if (dwLastSeg != FILESEEK(dwLastSeg))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    do
    {
        dwy = (dwx > POPWRTBLK) ? POPWRTBLK : dwx;

        if ((UINT) dwy != FILEWRITE(lpbyWorkBuffer, (UINT) dwy))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        dwx -= dwy;
    } while (0 != dwx);

    x = (0 == FILETRUNCATE(dwTrunc)) ? EXTSTATUS_OK : EXTSTATUS_FILE_ERROR;

    CLOSEOUT(x);
} // ExtRepairFilePoppy



// EXTRepairFileAppar arguments:                    &&AP
//
// Purpose: Apparition prepends 89021 bytes to the beginning of the host, and
//          changes the bytes "4d 4d" in headers to "cd 83".  This routine
//          removes the prepending, and changes the cd83 back to 4d4d.
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
//

#define APPARLEN    89021       // Length of virus
#define WINHDRLOC   0x1c        // Location of WinHeader (in EXEHeader)
#define APPSTRSZ    10          // Length of string to search for
#define APPREPL     0x4d4d      // Replace cd83 w/ this
#define SRCHBUFSZ   REPMOVBUFSZ // Search buffer size - search for cd 83

WORD EXTRepairFileAppar(LPCALLBACKREV1  lpCallBack,
                        LPN30           lpsN30,
                        LPTSTR          lpszFileName,
                        LPBYTE          lpbyWorkBuffer)
{
    HFILE   hFile;
    BYTE    bSrch[] =   {0xcd, 0x83, 0x8b, 0xe5, 0x1f,
                         0x5d, 0x4d, 0xca, 0x0a, 0x00};
    WORD    wRetVal;                    // Value to return to caller
    WORD    i;                          // Miscellaneous variable
    UINT    uAttr;                      // Attribute
    DWORD   dwFileSz, dwReadPos, dwWritePos;
    DWORD   dwSrchLen;                  // Number of bytes to search

    // _asm    int 3                    // Remove after debugging! &&AP

    (void)lpsN30;

    wRetVal = EXTSTATUS_FILE_ERROR;

    if (0 != FILEOPEN())                // Open file
        return(wRetVal);

    if (APPARLEN != FILESEEK(APPARLEN)) // Go to beginning of host
        CLOSEOUT(wRetVal);

    if (sizeof(WORD) != FILEREAD(&i, sizeof(WORD)))
        CLOSEOUT(wRetVal);              // Read MZ (we hope) from host

    if (!ISEXEHDR(&i))                  // Make sure we read an MZ
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    dwReadPos = APPARLEN + WINHDRLOC;

    if (dwReadPos != FILESEEK(dwReadPos))
        CLOSEOUT(wRetVal);

    if (sizeof(WORD) != FILEREAD(&i, sizeof(WORD)))
        CLOSEOUT(wRetVal);              // Read WinHeader Offset

    dwReadPos = APPARLEN + WENDIAN(i);  // Start searching at WinHeader

    dwFileSz = FILESIZE();              // Compute number of bytes to
                                        // search
    if (DWERROR == dwFileSz)
        CLOSEOUT(wRetVal);

    dwSrchLen = dwFileSz - dwReadPos;

    i = APPREPL;                    // Value to replace cd 83 with
                                    // APPREPL == 0x4d4d, so endianness isn't
    do                              // a problem.
    {                               // Search for cd 83 to replace w/ 4d 4d
        dwWritePos = SearchFileForString(lpCallBack, hFile, lpbyWorkBuffer,
                                            dwReadPos, dwSrchLen,
                                            (LPBYTE) bSrch, APPSTRSZ);

        if (DWERROR != dwWritePos)
        {
            if (dwWritePos != FILESEEK(dwWritePos)) // Goto cd 83
                CLOSEOUT(wRetVal);

            if (sizeof(WORD) != FILEWRITE(&i, sizeof(WORD)))    // Replace it
                CLOSEOUT(wRetVal);

            dwReadPos = dwWritePos + APPSTRSZ;

            dwSrchLen = dwFileSz - dwReadPos;
        }
    } while(DWERROR != dwWritePos);

    dwSrchLen = dwFileSz - APPARLEN;

    i = RepairFileMoveBytes(lpCallBack, hFile, lpbyWorkBuffer, APPARLEN, 0L,
                            dwSrchLen);

    if (0 == i)
        i = FILETRUNCATE(APPARLEN);

    wRetVal = (0 == i) ?  EXTSTATUS_OK : EXTSTATUS_FILE_ERROR;

    CLOSEOUT(wRetVal);
}   // RepairFileAppar



// Function:
//  WORD EXTRepairFileKlunky()
//
// Purpose: Fixes up the PE header EP & ImageSize.
//          Finally the virus is truncated from the file.
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

#define KLUNKYEP_OFF       7423        // Offset of delta EP from EOF
#define KLUNKYVSIZE        7939        // infection size
#define KLUNKYIMAGESIZE    8939        // IMAGE size increase

WORD EXTRepairFileKlunky(LPCALLBACKREV1  lpCallBack, // File op callbacks
                         LPN30           lpsN30,     // Ptr to N30 structure
                         LPTSTR          lpszFileName,   // Infected filename
                         LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE                       hFile;
    UINT                        uAttr;
    WORD                        x;          // Misc variable
    DWORD                       dwFileSize; // FileSize
    DWORD                       dwEPdeltaOff;
    DWORD                       dwEPdelta;  // Delta EP stored at KLUNKYEP_OFF
                                            //  from EOF
    DWORD                       dwWinHd;    // PE Hdr loc
    NAVEX_IMAGE_NT_HEADERS      stPEInfo;   // PE Header Info

    // _asm    int 3            // &&P

    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    // Get PE header
    if (WINDOWS_PE_FORMAT != DetermineWindowsType(lpCallBack,
                                                   hFile,
                                                   lpbyWorkBuffer,
                                                   &dwWinHd))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    stPEInfo = *((LPNAVEX_IMAGE_NT_HEADERS) lpbyWorkBuffer);

    // Endianize header for computations
    stPEInfo.OptionalHeader.AddressOfEntryPoint =
                        DWENDIAN(stPEInfo.OptionalHeader.AddressOfEntryPoint);
    stPEInfo.OptionalHeader.SizeOfImage =
                        DWENDIAN(stPEInfo.OptionalHeader.SizeOfImage);
    stPEInfo.FileHeader.NumberOfSections =
                        WENDIAN(stPEInfo.FileHeader.NumberOfSections);


    //Get Filesize
    dwFileSize = FILESIZE();
    dwEPdeltaOff = dwFileSize - KLUNKYEP_OFF;

    //Get Delta EP
    if (dwEPdeltaOff != FILESEEK(dwEPdeltaOff))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);
    x = sizeof(DWORD);
    if (x != FILEREAD((LPBYTE) &dwEPdelta, x))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    dwEPdelta = DWENDIAN(dwEPdelta);

    // Compute and store the original EP
    stPEInfo.OptionalHeader.AddressOfEntryPoint =
                            stPEInfo.OptionalHeader.AddressOfEntryPoint
                             + 7 - dwEPdelta;

    // Decrease Number of Sections
    stPEInfo.FileHeader.NumberOfSections --;

    // Compute original image sizes
    stPEInfo.OptionalHeader.SizeOfImage -= KLUNKYIMAGESIZE;

    // Un-endianize for storing back to disk
    stPEInfo.OptionalHeader.AddressOfEntryPoint =
                        DWENDIAN(stPEInfo.OptionalHeader.AddressOfEntryPoint);
    stPEInfo.OptionalHeader.SizeOfImage =
                        DWENDIAN(stPEInfo.OptionalHeader.SizeOfImage);
    stPEInfo.FileHeader.NumberOfSections =
                        WENDIAN(stPEInfo.FileHeader.NumberOfSections);

    // Write new PE header
    if (dwWinHd != FILESEEK(dwWinHd))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    if (sizeof(NAVEX_IMAGE_NT_HEADERS) !=
                        FILEWRITE(&stPEInfo, sizeof(NAVEX_IMAGE_NT_HEADERS)))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    // truncate the file By infection size
    x = (0 == FILETRUNCATE(KLUNKYVSIZE)) ? EXTSTATUS_OK : EXTSTATUS_FILE_ERROR;

    CLOSEOUT(x);
} // ExtRepairFileKlunky


// Function:
// WORD EXTRepairFileCIH() -- Darren K. 06/29/1998 -- Repair for W95.CIH 0x617e
//
// Purpose: Fixes up the PE header EP & ImageSize.
//          Finally the virus is truncated from the file.
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


WORD EXTRepairFileCIH(LPCALLBACKREV1  lpCallBack, // File op callbacks
                      LPN30           lpsN30,     // Ptr to N30 structure
                      LPTSTR          lpszFileName,   // Infected filename
                      LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE                       hFile;      //assigned in FILEOPEN
    UINT                        uAttr;      //assigned in FILEOPEN

    WORD                        x;          // Misc variable
    DWORD                       dwx;        // Misc Variable

    NAVEX_IMAGE_NT_HEADERS      stPEInfo;   // PE Header Info
    DWORD                       dwWinHd;    // PE Hdr loc

    DWORD                       adwVirusData[14];
    DWORD                       dwOffsetOfVirusData;
    DWORD                       dwSizeOfVirusData;

    DWORD                       dwOriginalEP;
    DWORD                       dwOffsetOfOriginalEP;

    BYTE                        bZero = 0;  // zero byte for writing
    DWORD                       i, j;

    NAVEX_IMAGE_SECTION_HEADER  stSectionHeader;
    DWORD                       dwOffsetSectionHeader;
    WORD                        wSectionNumber;


    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    // Get PE header
    if (WINDOWS_PE_FORMAT != DetermineWindowsType(lpCallBack,
                                                   hFile,
                                                   lpbyWorkBuffer,
                                                   &dwWinHd))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    stPEInfo = *((LPNAVEX_IMAGE_NT_HEADERS) lpbyWorkBuffer);

    // Endianize header for computations
    stPEInfo.OptionalHeader.AddressOfEntryPoint =
                        DWENDIAN(stPEInfo.OptionalHeader.AddressOfEntryPoint);
    stPEInfo.OptionalHeader.SizeOfImage =
                        DWENDIAN(stPEInfo.OptionalHeader.SizeOfImage);
    stPEInfo.OptionalHeader.ImageBase =
                        DWENDIAN(stPEInfo.OptionalHeader.ImageBase);
    stPEInfo.FileHeader.NumberOfSections =
                        WENDIAN(stPEInfo.FileHeader.NumberOfSections);


    //get viral data
    //  starts 56 bytes before EntryPoint
    //  array of DWORDs, going backward from EntryPoint
    //  first DWORD (from EP) gives length of viral code in the PE header,
    //    excluding these 56 bytes of viral data
    //  subsequent pairs of DWORDs give absolute virtual address and length
    //    of the next portion of viral code

    dwOffsetOfVirusData = stPEInfo.OptionalHeader.AddressOfEntryPoint - 56;

    if (dwOffsetOfVirusData != FILESEEK(dwOffsetOfVirusData))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = (sizeof(DWORD) * 14);
    if ( x != FILEREAD( (LPBYTE) adwVirusData, x) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    for (i=0;i<14;i++)
        adwVirusData[i] = DWENDIAN(adwVirusData[i]);        //endianize


    //get original EP (stored at EP + 0x5e, as absolute virtual address)

    dwOffsetOfOriginalEP = stPEInfo.OptionalHeader.AddressOfEntryPoint + 0x5e;

    if (dwOffsetOfOriginalEP != FILESEEK(dwOffsetOfOriginalEP))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = sizeof(DWORD);
    if ( x != FILEREAD( (LPBYTE) &dwOriginalEP, x) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    dwOriginalEP -= stPEInfo.OptionalHeader.ImageBase;


    //zero out viral code at the end of the PE Header


    //grr -- size of virus data may not be 56
    //may need to fix more later

    dwSizeOfVirusData =  stPEInfo.OptionalHeader.AddressOfEntryPoint -
                        (dwWinHd +
                         sizeof(NAVEX_IMAGE_NT_HEADERS) +
                         (stPEInfo.FileHeader.NumberOfSections *
                          sizeof(NAVEX_IMAGE_SECTION_HEADER) ) );

    dwOffsetOfVirusData = stPEInfo.OptionalHeader.AddressOfEntryPoint -
                          dwSizeOfVirusData;

    if (dwOffsetOfVirusData != FILESEEK(dwOffsetOfVirusData))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    //to include the viral data before the EP
    adwVirusData[13] += dwSizeOfVirusData;

    //make sure we don't overwrite the lpbyWorkBuffer in case of corrupted file
    if (adwVirusData[13] >= 2048) adwVirusData[13] = 2048;

    for (i=0;i<adwVirusData[13];i++)
        lpbyWorkBuffer[i] = 0;

    if ( adwVirusData[13] !=
        (DWORD) FILEWRITE( (LPBYTE)lpbyWorkBuffer, (UINT) adwVirusData[13] ) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //virus puts itself in caves at the end of sections
    //zero out viral code in each section, and repair section header

    i=12;
    while ( (adwVirusData[i] != 0) && (i>=2) )
    {
        //adwVirusData[i]   = absolute virtual address of next portion of virus
        //adwVirusData[i-1] = size of next portion of virus

        //convert address of next portion of virus to RVA
        adwVirusData[i] -= stPEInfo.OptionalHeader.ImageBase;

        //find section with this RVA and read in section info

        dwOffsetSectionHeader = dwWinHd + sizeof(NAVEX_IMAGE_NT_HEADERS);
        if (dwOffsetSectionHeader != FILESEEK(dwOffsetSectionHeader))
            CLOSEOUT(EXTSTATUS_NO_REPAIR);

        for (j=0;j<stPEInfo.FileHeader.NumberOfSections;j++)
        {
            //read in section header
            x = sizeof(NAVEX_IMAGE_SECTION_HEADER);
            if ( x != FILEREAD( (LPBYTE) &stSectionHeader, x) )
                CLOSEOUT(EXTSTATUS_FILE_ERROR);

            //check to see if it's the right one
            if (stSectionHeader.VirtualAddress <= adwVirusData[i] &&
                adwVirusData[i] <= (stSectionHeader.VirtualAddress +
                                   stSectionHeader.Misc.VirtualSize) )
            {
                wSectionNumber = (WORD) j;
                break;
            }

            dwOffsetSectionHeader += sizeof(NAVEX_IMAGE_SECTION_HEADER);
        }

        if (j >= stPEInfo.FileHeader.NumberOfSections) //didn't find the section
            CLOSEOUT(EXTSTATUS_NO_REPAIR);


        //write new Section info

        stSectionHeader.Misc.VirtualSize =
            adwVirusData[i] - DWENDIAN(stSectionHeader.VirtualAddress);

        stSectionHeader.Misc.VirtualSize =
            DWENDIAN(stSectionHeader.Misc.VirtualSize);

        if (dwOffsetSectionHeader != FILESEEK(dwOffsetSectionHeader))
            CLOSEOUT(EXTSTATUS_NO_REPAIR);

        x = sizeof(NAVEX_IMAGE_SECTION_HEADER);
        if (x != FILEWRITE( (LPBYTE)&stSectionHeader, x ) )
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        //zero out viral code at the end of section

        dwx = DWENDIAN(stSectionHeader.PointerToRawData) +
              DWENDIAN(stSectionHeader.Misc.VirtualSize);
        if (dwx != FILESEEK(dwx))
            CLOSEOUT(EXTSTATUS_NO_REPAIR);

        for (j=0;j<adwVirusData[i-1];j++)
        {
            if (1 != FILEWRITE(&bZero,1))
                CLOSEOUT(EXTSTATUS_NO_REPAIR);
        }

        //new conditions to compare
        i -= 2;

    }//while

    //write original EntryPoint

    dwx = dwWinHd + 0x28;  //0x28 is offset of AddressOfEntryPoint in PE Header
    if (dwx != FILESEEK(dwx))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = sizeof(DWORD);
    if (x != FILEWRITE( (LPBYTE)&dwOriginalEP, x ) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    CLOSEOUT(EXTSTATUS_OK);

} // ExtRepairFileCIH


// Function:
// WORD EXTRepairFileMarburg()
// -- Darren K. 07/24/1998 -- Repair for W95.Marburg 0x2566
//
// Purpose: Fixes up the PE header and last Section header.
//          Decrypts virus to find original host bytes.
//          Finally the virus is truncated from the file.
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


WORD EXTRepairFileMarburg
(
    LPCALLBACKREV1  lpCallBack, // File op callbacks
    LPN30           lpsN30,     // Ptr to N30 structure
    LPTSTR          lpszFileName,   // Infected filename
    LPBYTE          lpbyWorkBuffer // Buffer >= 512 bytes
)
{
    HFILE                       hFile;      //assigned in FILEOPEN
    UINT                        uAttr;      //assigned in FILEOPEN

    WORD                        x;          // Misc variable
    WORD                        wResult;

    NAVEX_IMAGE_NT_HEADERS      stPEHeader;         // PE Header Info
    DWORD                       dwPEHeaderOffset;   // PE hdr loc
    DWORD                       dwEPOffset;         // file offset of EP
    NAVEX_IMAGE_SECTION_HEADER  stLastSectionHeader;

    //Marburg infection info
    DWORD                       dwVirusOffset;
    DWORD                       dwDecryptKey;
    BYTE                        byDecryptFlags;
    WORD                        wVirusBytesAtEP;
    DWORD                       dwLastSectionHeaderOffset;

    LPBYTE                      lpbyVirusBuffer;

    WORD                        wVID;
    WORD                        wMarIdx;

    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    // Get PE header
    if (WINDOWS_PE_FORMAT != DetermineWindowsType(lpCallBack,
                                                   hFile,
                                                   lpbyWorkBuffer,
                                                   &dwPEHeaderOffset) )
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    stPEHeader = *((LPNAVEX_IMAGE_NT_HEADERS) lpbyWorkBuffer);


    //figure out where the file offset of EP is

    if (RVAToFileOffset(lpCallBack,
                        hFile,
                        &stPEHeader,
                        lpbyWorkBuffer,
                        dwPEHeaderOffset,
                        DWENDIAN(stPEHeader.OptionalHeader.AddressOfEntryPoint),
                        &dwEPOffset)
                        != EXTSTATUS_OK)
        return EXTSTATUS_FILE_ERROR;

    // _asm    int 3

    //get Marburg info
    wResult = LocateMarburgStart(lpCallBack,
                                   hFile,
                                   lpbyWorkBuffer,
                                   &stPEHeader,
                                   dwPEHeaderOffset,
                                   dwEPOffset,
                                   &dwVirusOffset,
                                   &dwDecryptKey,
                                   &byDecryptFlags,
                                   &wVirusBytesAtEP,
                                   &wVID,
                                   &dwLastSectionHeaderOffset);

    if (wResult != EXTSTATUS_VIRUS_FOUND)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    for(wMarIdx = 0;
            (wMarIdx < wNumMarburgs) && (MarbTab[wMarIdx].wVID != wVID);
            wMarIdx++)
        ;           // Find index into Marburg data

    if (wMarIdx == wNumMarburgs)        // Made sure we actually found a
        CLOSEOUT(EXTSTATUS_NO_REPAIR);  //  Marburg entry in the table


    //read encrypted virus into lpbyVirusBuffer
    //                                  (virus size = MarbTab[wMarIdx].dwSize)

#ifndef SYM_WIN32
    lpbyVirusBuffer = (LPBYTE) lpCallBack->PermMemoryAlloc(0x1700);
#else
    lpbyVirusBuffer = (LPBYTE) GlobalAlloc(GMEM_FIXED, 0x1700);
#endif

    if (lpbyVirusBuffer == NULL)
    {

#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyVirusBuffer);
#else
        GlobalFree(lpbyVirusBuffer);
#endif

        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    }


    if (dwVirusOffset != FILESEEK(dwVirusOffset))
    {

#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyVirusBuffer);
#else
        GlobalFree(lpbyVirusBuffer);
#endif

        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    }
                        // Read VirusSize bytes into buffer
    if (MarbTab[wMarIdx].dwSize != FILEREAD(lpbyVirusBuffer,
                                                    MarbTab[wMarIdx].dwSize))

    {

#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyVirusBuffer);
#else
        GlobalFree(lpbyVirusBuffer);
#endif

        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    }


    //decrypt the virus
    if (EXTSTATUS_OK != DecryptBufferWithKey(lpbyVirusBuffer,
                                             0x1700,
                                             byDecryptFlags,
                                             dwDecryptKey,
                                             0) )
    {

#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyVirusBuffer);
#else
        GlobalFree(lpbyVirusBuffer);
#endif

        CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }


    //write original host bytes

    if (dwEPOffset != FILESEEK(dwEPOffset))
    {

#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyVirusBuffer);
#else
        GlobalFree(lpbyVirusBuffer);
#endif

        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    }


    if (wVirusBytesAtEP !=
        (DWORD) FILEWRITE(lpbyVirusBuffer+MarbTab[wMarIdx].dwSize-0x104,
        (UINT) wVirusBytesAtEP))

    {

#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyVirusBuffer);
#else
        GlobalFree(lpbyVirusBuffer);
#endif

        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    }


/*
    //can't do this -- virus doesn't always calculate the new SizeOfImage
    //correctly

    //write original SizeOfImage in PE header

    dwOriginalSizeOfImage = DWENDIAN(stPEHeader.OptionalHeader.SizeOfImage)
                                - 0x3000;

    dwx = dwPEHeaderOffset + 0x50;  //offset of SizeOfImage in PE Header
    if (dwx != FILESEEK(dwx))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = sizeof(DWORD);
    if (x != FILEWRITE( (LPBYTE)&dwOriginalSizeOfImage, x ) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

*/

    //fix the last section header

    if (dwLastSectionHeaderOffset != FILESEEK(dwLastSectionHeaderOffset))
    {

#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyVirusBuffer);
#else
        GlobalFree(lpbyVirusBuffer);
#endif

        CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }

    x = sizeof(NAVEX_IMAGE_SECTION_HEADER);
    if (x != FILEREAD(&stLastSectionHeader, x))
    {

#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyVirusBuffer);
#else
        GlobalFree(lpbyVirusBuffer);
#endif

        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    }

    stLastSectionHeader.SizeOfRawData =
    stLastSectionHeader.Misc.VirtualSize =
        dwVirusOffset - DWENDIAN(stLastSectionHeader.PointerToRawData);

    stLastSectionHeader.SizeOfRawData =
        DWENDIAN(stLastSectionHeader.SizeOfRawData);
    stLastSectionHeader.Misc.VirtualSize =
        DWENDIAN(stLastSectionHeader.Misc.VirtualSize);

    if (dwLastSectionHeaderOffset != FILESEEK(dwLastSectionHeaderOffset))
    {

#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyVirusBuffer);
#else
        GlobalFree(lpbyVirusBuffer);
#endif

        CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }

    x = sizeof(NAVEX_IMAGE_SECTION_HEADER);
    if (x != FILEWRITE( (LPBYTE)&stLastSectionHeader, x ) )
    {

#ifndef SYM_WIN32
        lpCallBack->PermMemoryFree(lpbyVirusBuffer);
#else
        GlobalFree(lpbyVirusBuffer);
#endif

        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    }


                    // Free up memory here to ease error-handling
#ifndef SYM_WIN32   // Not needed to truncate files, anyway
    lpCallBack->PermMemoryFree(lpbyVirusBuffer);
#else
    GlobalFree(lpbyVirusBuffer);
#endif

    //truncate
 
    x = EXTSTATUS_FILE_ERROR;
 
    if (dwVirusOffset == FILESEEK(dwVirusOffset))
        if (0 == FILEWRITE( (LPBYTE)&x, 0 ) )
            x = EXTSTATUS_OK;

    CLOSEOUT(x);

} // ExtRepairFileMarburg


// Function:
//  WORD EXTRepairFileRedTeam()                             &RT
//
// Purpose: This removes the RedTeam virus from infected NE files by doing the
//          following:
//          Fixes IP in NE header.
//          Fixes changes to the segment table - EP segment is made longer
//          Fixes the relocation table in the EP segment
//          Fixes the offsets in the resource table
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

#define REDTEAMLEN  4766                // RedTeam adds this much to EP seg
#define REDRELOCS   14                  // This many reloc entries added

WORD EXTRepairFileRedTeam(LPCALLBACKREV1    lpCallBack, // File op callbacks
                            LPN30           lpsN30, // Pointer to N30 structure
                            LPTSTR          lpszFileName,   // Infected file
                            LPBYTE          lpbyWorkBuffer) // size >= 512
{
    HFILE       hFile;
    UINT        uAttr;
    WORD        i, j, k, x;             // Miscellaneous variables
    WORD        wCS, wIP;               // CS seg, IP offset
    WORD        wFixVal, wSegFix;       // Generic fixup value, Segment to fix
    WORD        wEPSector;              // EP sector offset
    WORD        wNumSegs;               // Number of segments
    WORD        wAlign;                 // Segment sector size for file
    WORD        wResAlign;              // Segment sector size for resources
    WORD        wOrigSegLen;            // Original length of EP sector
    WORD        wResFixVal;             // Fixup resource by this amount
    DWORD       dwFix;                  // Offset of data to fix
    DWORD       dwx, dwWinHd;           // Misc variable, NE Header location
    DWORD       dwSegTab, dwResTab;     // Segment, resource table offsets
    DWORD       dwEPSeg, dwEP;          // File offset of EP segment
    DWORD       dwRelRead;              // Read reloc table from this offset
    DWORD       dwFileLen;              // Length of file
    LPWININFO   WinHead;                // Windows NE header structure
    LPTBSEGMENT WinSeg;                 // Windows segment table entry
    LPINTERNALFIXED WinFix;             // Windows (NE) fixup table entries

    // _asm    int 3                    // Remove after debugging!
                                        // Open file
    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    if (WINDOWS_NE_FORMAT != DetermineWindowsType(lpCallBack,
                                                    hFile,
                                                    lpbyWorkBuffer,
                                                    &dwWinHd))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    dwFileLen = FILESIZE();                 // Save filesize for later

    WinHead = (LPWININFO) lpbyWorkBuffer;   // This avoids lots of casts

    // Endianize the necessary fields before extracting data

    wCS = WENDIAN(WinHead->cs);
    wIP = WENDIAN(WinHead->ip);

    wOrigSegLen = wIP;          // Save the original segment length for later

    wNumSegs = WENDIAN(WinHead->segTabEntries); // Number of seg tab entries
    dwSegTab = (DWORD) WENDIAN(WinHead->segTabOffset);  // SegTab offset
    dwResTab = (DWORD) WENDIAN(WinHead->resTabOffset);  // ResTab offset
    wAlign = WENDIAN(WinHead->shiftCount);      // Logical sector alignment
    WinHead->fastLoadSize = WENDIAN(WinHead->fastLoadSize);

    wFixVal = ((REDTEAMLEN + (REDRELOCS * sINTERNALFIXED)) >> wAlign) + 1;
                                // Adjust sector offsets by wFixVal if needed
 
    dwSegTab += dwWinHd;                        // Add NE Header offset
    dwResTab += dwWinHd;

    // Compute the file offset of the entry point
                            // Compute file offset of EP segment table entry
    dwEPSeg = dwSegTab + (DWORD) (sTBSEGMENT * (wCS - 1));
 
    if (dwEPSeg != FILESEEK(dwEPSeg)) // dwEPSeg is now file offset of segment
        CLOSEOUT(EXTSTATUS_FILE_ERROR);     // table entry for EP

    if (sizeof(WORD) != FILEREAD(&wEPSector, sizeof(WORD))) // Read file
        CLOSEOUT(EXTSTATUS_FILE_ERROR);             // offset for EP segment

    dwEPSeg = (DWORD) WENDIAN(wEPSector);
    dwEPSeg <<= wAlign;     // Now we point to the beginning of the EP segment

    dwEP = dwEPSeg + wIP + 1;   // At IP+1 is original IP (preceded by PUSH)

    if (dwEP != FILESEEK(dwEP))     // dwEP is now file offset of EP + 1
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (sizeof(WORD) != FILEREAD(&wIP, sizeof(WORD)))   // Read original IP
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    WinHead->ip = WENDIAN(wIP);                 // Restore original IP
 
    WinHead->fastLoadSize = WENDIAN(WinHead->fastLoadSize - wFixVal);

    // The windows header gets written here so we can use the buffer to
    // load the segment table.

    if (dwWinHd != FILESEEK(dwWinHd))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                                // Write fixed Win header
    if (sWININFO != FILEWRITE(lpbyWorkBuffer, sWININFO))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    // Read segment table entries and adjust for removal of virus
 
    dwFix = dwSegTab;

    if (dwFix != FILESEEK(dwFix))           // Go to segment table beginning
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
 
    WinSeg = (LPTBSEGMENT) lpbyWorkBuffer;  // Avoid casts

    wSegFix = 0xffff;
    for (i = 0; i < wNumSegs; i++)
    {
        if (sTBSEGMENT != FILEREAD((LPBYTE) WinSeg, sTBSEGMENT))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);

        x = WENDIAN(WinSeg->segDataOffset);  // Read sector offset

        if (x >= wEPSector)
        {
            if (x > wEPSector)  // Adjust offset of segments located after
            {                   // the virus code
                if (x < wSegFix)    // Save location of segment closest to TOF
                    wSegFix = x;    // that must be moved.  This segment, and
                                    // everything after it must be moved
                WinSeg->segDataOffset = WENDIAN(x - wFixVal);
            }
            else                // This is for when x == wEPSector
            {                   // Adjust parameters for segment w/ virus
                j = WENDIAN(WinSeg->segLen);        // Fix segment length
                WinSeg->segLen = WENDIAN(j - REDTEAMLEN);

                j = WENDIAN(WinSeg->segMinSize);    // Fix min allocation
                WinSeg->segMinSize = WENDIAN(j - REDTEAMLEN);
            }
 
            if (dwFix != FILESEEK(dwFix))   // Backup to what was just read
                CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                            // Write it again
            if (sTBSEGMENT != FILEWRITE((LPBYTE) WinSeg, sTBSEGMENT))
                CLOSEOUT(EXTSTATUS_FILE_ERROR);
        }

        dwFix += sTBSEGMENT;            // Update current position pointer
    }

    // Fix up resource table
 
    if (dwResTab != FILESEEK(dwResTab)) // Go to resource table beginning
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
 
    if (sizeof(WORD) != FILEREAD((LPBYTE) &wResAlign, sizeof(WORD)))
        CLOSEOUT(EXTSTATUS_FILE_ERROR); // Read the alignment byte

    wResAlign = WENDIAN(wResAlign);

    wResFixVal =
            ((REDTEAMLEN + (REDRELOCS * sINTERNALFIXED)) >> wResAlign) + 1;
 
    if (sizeof(WORD) != FILEREAD((LPBYTE) &x, sizeof(WORD)))
        CLOSEOUT(EXTSTATUS_FILE_ERROR); // Read the resource type

    while (0 != x)      // While the resource type isn't 0
    {
        if (sizeof(WORD) != FILEREAD((LPBYTE) &i, sizeof(WORD)))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);     // Read the count of entries

        i = WENDIAN(i);

        dwx = lpCallBack->FileSeek(hFile, 4, SEEK_CUR); // Skip reserved bytes

        for (j = 0; j < i; j++)         // Update the offset for each entry
        {
            if (sizeof(WORD) != FILEREAD((LPBYTE) &k, sizeof(WORD)))
                CLOSEOUT(EXTSTATUS_FILE_ERROR);     // Read the offset

            k = WENDIAN(k);                     // Fix the offset

            k = WENDIAN(k - wResFixVal);
                                                // Re-write the offset
            dwx = lpCallBack->FileSeek(hFile, -2, SEEK_CUR);

            if (sizeof(WORD) != FILEWRITE((LPBYTE) &k, sizeof(WORD)))
                CLOSEOUT(EXTSTATUS_FILE_ERROR);     // Write offset

            dwx = lpCallBack->FileSeek(hFile, 10, SEEK_CUR);    // Skip stuff
        }

        if (sizeof(WORD) != FILEREAD((LPBYTE) &x, sizeof(WORD)))
            CLOSEOUT(EXTSTATUS_FILE_ERROR); // Read the resource type
    }   // while (0 != x)

    // Fixup the relocation table in the EP segment

    dwEP--;     // Point to EP instead of EP + 1
    dwRelRead = dwEP + REDTEAMLEN;  // Point to end of virus code / begin of
                                    // reloc table
    if (dwRelRead != FILESEEK(dwRelRead))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (sizeof(WORD) != FILEREAD((LPBYTE) &x, sizeof(WORD)))
        CLOSEOUT(EXTSTATUS_FILE_ERROR); // Read the number of reloc entries

    x = WENDIAN(x);     // Fix up number of reloc entries

    i = WENDIAN(x - REDRELOCS);

    dwx = lpCallBack->FileSeek(hFile, -2, SEEK_CUR);    // Backup one word

    if (sizeof(WORD) != FILEWRITE((LPBYTE) &i, sizeof(WORD)))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);     // Write fixed number of reloc
                                            // entries
    WinFix = (LPINTERNALFIXED) lpbyWorkBuffer;

    dwx += sizeof(WORD);
    dwFix = dwx;            // Save location of relocation table beginning

    dwx += (sINTERNALFIXED * (x - 1));  // Goto last reloc table entry

    if (dwx != FILESEEK(dwx))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
                                        // Read last reloc table entry
    if (sINTERNALFIXED != FILEREAD(lpbyWorkBuffer, sINTERNALFIXED))
        CLOSEOUT(EXTSTATUS_FILE_ERROR)

    j = WinFix->bySegNumber;    // Use this data to fix an entry in while
    k = WinFix->byEmpty;        // loop below

    x = i;                  // Save number of reloc entries for later
    dwx = dwFix;            // Save location of reloc table beginning

    if (dwFix != FILESEEK(dwFix))   // Go back to beginning of reloc table
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    while (0 < i)   // Look for a reloc entry that points into viral code
    {
        if (sINTERNALFIXED != FILEREAD(lpbyWorkBuffer, sINTERNALFIXED))
            CLOSEOUT(EXTSTATUS_FILE_ERROR);     // Read a reloc entry

        if ((3 == WinFix->byAddType) && (0 == WinFix->byRelType))
        {           // Make sure it's the right type of entry
            if ((wCS == WinFix->bySegNumber) &&
                (WENDIAN(WinFix->wSegOffset) > wOrigSegLen))
            {           // Make sure it points into viral code
                WinFix->byRelType = 1;              // Modify this to point to
                WinFix->bySegNumber = (BYTE) j;     // Kernel!Inittask
                WinFix->byEmpty = (BYTE) k;         // j & k are upper & lower
                WinFix->wSegOffset = 0x5b;          // halves of Module
                                                    // Reference table offset
                WinFix->wSegOffset = WENDIAN(WinFix->wSegOffset);

                if (dwFix != FILESEEK(dwFix))       // Backup to the modified
                    CLOSEOUT(EXTSTATUS_FILE_ERROR); // entry and rewrite it

                if (sINTERNALFIXED !=
                                FILEWRITE(lpbyWorkBuffer, sINTERNALFIXED))
                    CLOSEOUT(EXTSTATUS_FILE_ERROR);

                i = 1;
            }
        }

        dwFix += sINTERNALFIXED;
        i--;
    }

    dwx += (x * sINTERNALFIXED);    // Compute location of viral reloc
                                    // entries
    if (dwx != FILESEEK(dwx))       // Goto viral reloc entries
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    for (i = 0; i < sINTERNALFIXED; i++)    // Fill buffer w/ some zeros
        lpbyWorkBuffer[i] = 0;

    dwFix = (DWORD) wSegFix;        // Compute beginning of next sector
    dwFix <<= wAlign;
                                    // Compute number of bytes from current
    dwx = dwFix - dwx;              // location to beginning of next sector

    i = sINTERNALFIXED;             // Number of bytes / write

    while (0 != dwx)
    {
        if (dwx < (DWORD) i)        // Check to see if partial write is
            i = (WORD) dwx;         // needed

        if (i != FILEWRITE(lpbyWorkBuffer, i))  // Overwrite are between
            CLOSEOUT(EXTSTATUS_FILE_ERROR);     // end of virus segment and
                                                // next segment w/ zeros
        dwx -= i;
    }
                                // Move bytes from beginning of reloc table
    dwx = dwFix - dwRelRead;    // to beginning of next sector
    j = RepairFileMoveBytes(lpCallBack, hFile, lpbyWorkBuffer,
                                                    dwRelRead, dwEP, dwx);
    x = EXTSTATUS_FILE_ERROR;
 
    if (0 != j)
        CLOSEOUT(x);

    dwRelRead = (DWORD) wSegFix;
    dwEP = dwRelRead - wFixVal;

    dwRelRead <<= wAlign;
    dwEP <<= wAlign;

    dwx = dwFileLen - dwRelRead;

    j = RepairFileMoveBytes(lpCallBack, hFile, lpbyWorkBuffer,
                                                    dwRelRead, dwEP, dwx);

    if (0 == j)
    {
        dwx = dwRelRead - dwEP;

        j = FILETRUNCATE(dwx);

        if (0 == j)
            x = EXTSTATUS_OK;
    }

    CLOSEOUT(x);
}   // EXTRepairFileRedTeam



// Function:
//  WORD IsEinstein32()
//
// Purpose: Checks if a file is an Einstein32 executable by comparing the
//          name in the Export Directory with "updat32.exe".
//
// pstPEHdr             : Pointer to PE header structure - must be filled
// dwWH                 : File offset to PE header
// lpCallBack           : Callback pointer for file ops
// hFile                : Handle to file - should already be open
// lpbyBufr             : temporary 2-4K work buffer.
// numsegs              : number of segments in PE header
//
// Returns:
//
//     WORD             : 0         if not an Einstein32
//                        1         if an Einstein32 file
//                        WERROR    if file or disk error

#define RDATLEN         7           // Length of name w/ 0 at end
#define NAME_RVA_OFF    0x0c        // Offset of Export name RVA in export
                                    //  name struct
#define EXPNAMELEN      12          // Length of "UPDAT32.EXE"

WORD IsEinstein32(LPNAVEX_IMAGE_NT_HEADERS pstPEHdr,
                    DWORD dwWH,
                    LPCALLBACKREV1 lpCallBack,
                    HFILE hFile,
                    LPBYTE lpbyBufr,
                    WORD numsegs)
{
    int                         x, y;       // Misc variables
    char                        segname[] = ".rdata";   // Seg we look for
    char                        expname[] = "updat32.exe";  // Export we seek
    BOOL                        res;        // Result holder
    DWORD                       dwx;        // Misc DWORD
    DWORD                       dwExpRVA;   // RVA of Export directory
    DWORD                       dwSegRVA;   // RVA of Segment w/ Exp directory
    DWORD                       dwSegOff;   // File offset of segment
    DWORD                       dwSegLen;   // Length of segment
    DWORD                       dwOffset;   // Offset between RVA and file loc
    DWORD                       dwLoc;      // Physical location of stuff
    LPNAVEX_IMAGE_SECTION_HEADER pstPESg;   // Ptr to section header

    // _asm    int 3            // &&E32

    // There must be export directory entries

    if (0 == pstPEHdr->OptionalHeader.
                    DataDirectory[NAVEX_IMAGE_DIRECTORY_ENTRY_EXPORT].Size)
        return(0);

    dwExpRVA = DWENDIAN(pstPEHdr->OptionalHeader.
            DataDirectory[NAVEX_IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);

    x = 0;
    pstPESg = (LPNAVEX_IMAGE_SECTION_HEADER) lpbyBufr;

                                    // Look for .rdata segment
    do
    {
        if (EXTSTATUS_OK != GetPESectionInfo(lpCallBack,
                                                hFile,
                                                lpbyBufr,
                                                dwWH,
                                                x,
                                                &dwSegOff,
                                                &dwSegLen,
                                                &dwx,
                                                &dwSegRVA))
            return(0);

        x++;
        res = TRUE;
        y = 0;

        do
        {
            res = (segname[y] == pstPESg->Name[y]);
            y++;
        } while ((TRUE == res) && (y < RDATLEN));
    } while ((x < numsegs) && (FALSE == res));

    if (FALSE == res)       // Bail if we didn't find .rdata
        return(0);

    if ((dwSegRVA > dwExpRVA) || ((dwSegRVA + dwSegLen) < dwExpRVA))
        return(0);          // Bail if Export directory is not in .rdata

    dwOffset = dwSegRVA - dwSegOff;
    dwLoc = (dwExpRVA - dwOffset) + NAME_RVA_OFF;

    if (dwLoc != FILESEEK(dwLoc))
        return(WERROR);

    x = sizeof(DWORD);

    if (x != FILEREAD(&dwx, x))
        return(WERROR);

    dwx = DWENDIAN(dwx) - dwOffset;

    if (dwx != FILESEEK(dwx))
        return(WERROR);

    if (EXPNAMELEN != FILEREAD(lpbyBufr, EXPNAMELEN))
        return(WERROR);

    res = TRUE;
    x = 0;

    do
    {
        if (0 != lpbyBufr[x])
            lpbyBufr[x] |= 0x20;    // Set the bit to make it lowercase

        if (lpbyBufr[x] != expname[x])
            res = FALSE;
    } while ((TRUE == res) && (++x < EXPNAMELEN));

    return((res) ? 1 : 0);
}   // IsEinstein32

// Function:
// WORD EXTRepairFileMarkJ() -- Darren K. 08/31/98
//
// Purpose:  Fixes up the PE header and section table.
//           Virus is truncated from the file.
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

WORD EXTRepairFileMarkJ(LPCALLBACKREV1  lpCallBack, // File op callbacks
                        LPN30           lpsN30,     // Ptr to N30 structure
                        LPTSTR          lpszFileName,   // Infected filename
                        LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE                       hFile;      //assigned in FILEOPEN
    UINT                        uAttr;      //assigned in FILEOPEN

    WORD                        x;          // Misc variable
    DWORD                       dwx;        // Misc Variable

    NAVEX_IMAGE_NT_HEADERS      stPEHeader;     // PE Header Info
    DWORD                       dwWinHdOffset;  // PE Hdr loc

    DWORD                       dwOriginalEP;
    DWORD                       dwOffsetOfOriginalEP;

    BYTE                        bZero = 0;  // zero byte for writing
    DWORD                       j;

    NAVEX_IMAGE_SECTION_HEADER  stSectionHeader;
    DWORD                       dwOffsetSectionHeader;

    WORD                        wNumSecs;
    DWORD                       dwOffsetNumSecs;

    DWORD                       dwOffsetVirus;


    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    // Get PE header
    if (WINDOWS_PE_FORMAT != DetermineWindowsType(lpCallBack,
                                                   hFile,
                                                   lpbyWorkBuffer,
                                                   &dwWinHdOffset))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    stPEHeader = *((LPNAVEX_IMAGE_NT_HEADERS) lpbyWorkBuffer);


    //get original EP (stored at EP + 0x28, as RVA)

    dwOffsetOfOriginalEP = stPEHeader.OptionalHeader.AddressOfEntryPoint + 0x28;

    if (dwOffsetOfOriginalEP != FILESEEK(dwOffsetOfOriginalEP))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = sizeof(DWORD);
    if ( x != FILEREAD( (LPBYTE) &dwOriginalEP, x) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //find the last section and truncate

    wNumSecs = WENDIAN(stPEHeader.FileHeader.NumberOfSections);
    if (wNumSecs < 1)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    dwOffsetSectionHeader = dwWinHdOffset +
                            sizeof(NAVEX_IMAGE_NT_HEADERS) +
                            (wNumSecs-1)*sizeof(NAVEX_IMAGE_SECTION_HEADER);

    if (dwOffsetSectionHeader != FILESEEK(dwOffsetSectionHeader))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = sizeof(NAVEX_IMAGE_SECTION_HEADER);
    if ( x != FILEREAD( (LPBYTE) &stSectionHeader, x) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    dwOffsetVirus = DWENDIAN(stSectionHeader.PointerToRawData);

    if (dwOffsetVirus != FILESEEK(dwOffsetVirus))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    if (0 != FILEWRITE( (LPBYTE)&bZero, 0) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    //zero out the last section header and code (0x28+0x2e bytes)

    if (dwOffsetSectionHeader != FILESEEK(dwOffsetSectionHeader))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    for (j=0;j<0x56;j++)
    {
        if (1 != FILEWRITE(&bZero,1))
            CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }

    //decrement the number of sections and write it to PE header

    wNumSecs = WENDIAN(wNumSecs - 1);

    dwOffsetNumSecs = dwWinHdOffset + 0x06;
    if (dwOffsetNumSecs != FILESEEK(dwOffsetNumSecs))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = sizeof(WORD);
    if (x != FILEWRITE( (LPBYTE)&wNumSecs, x ) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //write original EntryPoint

    dwx = dwWinHdOffset + 0x28; //0x28 is offset of AddressOfEntryPoint from PE
    if (dwx != FILESEEK(dwx))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = sizeof(DWORD);
    if (x != FILEWRITE( (LPBYTE)&dwOriginalEP, x ) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    CLOSEOUT(EXTSTATUS_OK);

} // ExtRepairFileMarkJ



// Function:
// WORD EXTRepairFileInca() -- Darren K. 09/29/98
//
// Purpose:  Fixes up the PE header and section table.
//           Virus is truncated from the file.
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

WORD EXTRepairFileInca(LPCALLBACKREV1  lpCallBack, // File op callbacks
                       LPN30           lpsN30,     // Ptr to N30 structure
                       LPTSTR          lpszFileName,   // Infected filename
                       LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE                       hFile;      //assigned in FILEOPEN
    UINT                        uAttr;      //assigned in FILEOPEN

    WORD                        x;          // Misc variable
    DWORD                       dwx;        // Misc Variable

    NAVEX_IMAGE_NT_HEADERS      stPEHeader;     // PE Header Info
    DWORD                       dwWinHdOffset;  // PE Hdr loc

    BYTE                        abyOriginalEP[4];
    DWORD                       dwOffsetOfOriginalEP;

    BYTE                        bZero = 0;  // zero byte for writing
    DWORD                       i,j;

    NAVEX_IMAGE_SECTION_HEADER  stSectionHeader;
    DWORD                       dwOffsetSectionHeader;

    WORD                        wNumSecs;
    DWORD                       dwOffsetNumSecs;

    DWORD                       dwOffsetVirus;
    DWORD                       dwOffsetTable;


    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    // Get PE header
    if (WINDOWS_PE_FORMAT != DetermineWindowsType(lpCallBack,
                                                   hFile,
                                                   lpbyWorkBuffer,
                                                   &dwWinHdOffset))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    stPEHeader = *((LPNAVEX_IMAGE_NT_HEADERS) lpbyWorkBuffer);


    //read in the last section header

    wNumSecs = WENDIAN(stPEHeader.FileHeader.NumberOfSections);
    if (wNumSecs < 1)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    dwOffsetSectionHeader = dwWinHdOffset +
                            sizeof(NAVEX_IMAGE_NT_HEADERS) +
                            (wNumSecs-1)*sizeof(NAVEX_IMAGE_SECTION_HEADER);

    if (dwOffsetSectionHeader != FILESEEK(dwOffsetSectionHeader))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = sizeof(NAVEX_IMAGE_SECTION_HEADER);
    if ( x != FILEREAD( (LPBYTE) &stSectionHeader, x) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //sanity check:  make sure EP is beginning of last section
    if ( DWENDIAN(stPEHeader.OptionalHeader.AddressOfEntryPoint) !=
            DWENDIAN(stSectionHeader.VirtualAddress) )
    {
        CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }


    //get original EP (DWORD stored at EP + 0x784, as RVA, encrypted)

    dwOffsetVirus = DWENDIAN(stSectionHeader.PointerToRawData);
    dwOffsetOfOriginalEP = dwOffsetVirus + 0x784;

    if (dwOffsetOfOriginalEP != FILESEEK(dwOffsetOfOriginalEP))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    if ( 4 != FILEREAD( abyOriginalEP, 4) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //find the beginning of the translation table, for decryption

    if (dwOffsetVirus != FILESEEK(dwOffsetVirus))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    if (512 != FILEREAD(lpbyWorkBuffer, 512) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    for (i=0; (i+4)<512; i++)
    {
        if (lpbyWorkBuffer[i] == 0xE8 &&
            lpbyWorkBuffer[i+1] == 0x00 &&
            lpbyWorkBuffer[i+2] == 0x01 &&
            lpbyWorkBuffer[i+3] == 0x00 &&
            lpbyWorkBuffer[i+4] == 0x00)
        {
            break;
        }
    }

    if ( (i+4) >= 512) // didn't find it yet
    {
        //make sure there's overlap!
        if ( (dwOffsetVirus+508) != FILESEEK(dwOffsetVirus+508) )
            CLOSEOUT(EXTSTATUS_NO_REPAIR);

        if (512 != FILEREAD(lpbyWorkBuffer, 512) )
            return(EXTSTATUS_FILE_ERROR);

        for (i=0; (i+4)<512; i++)
        {
            if (lpbyWorkBuffer[i] == 0xE8 &&
                lpbyWorkBuffer[i+1] == 0x00 &&
                lpbyWorkBuffer[i+2] == 0x01 &&
                lpbyWorkBuffer[i+3] == 0x00 &&
                lpbyWorkBuffer[i+4] == 0x00)
            {
                break;
            }
        }

        //still didn't find it
        if ( (i+4) >= 512 )
            CLOSEOUT(EXTSTATUS_NO_REPAIR);

        dwOffsetTable = dwOffsetVirus + 508 + i + 5;

    }
    else
    {
        dwOffsetTable = dwOffsetVirus + i + 5;
    }


    //read the translation table into lpbyWorkBuffer
    if (dwOffsetTable != FILESEEK(dwOffsetTable) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (256 != FILEREAD(lpbyWorkBuffer, 256) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //decrypt the original EP
    for (i=0; i<4; i++)
    {
        abyOriginalEP[i] = lpbyWorkBuffer[ abyOriginalEP[i] ];
    }


    //truncate from beginning of virus

    if (dwOffsetVirus != FILESEEK(dwOffsetVirus))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    if (0 != FILEWRITE( (LPBYTE)&bZero, 0) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //zero out the last section header (0x28 bytes)

    if (dwOffsetSectionHeader != FILESEEK(dwOffsetSectionHeader))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    for (j=0;j<0x28;j++)
    {
        if (1 != FILEWRITE(&bZero,1))
            CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }

    //decrement the number of sections and write it to PE header

    wNumSecs = WENDIAN(wNumSecs - 1);

    dwOffsetNumSecs = dwWinHdOffset + 0x06;
    if (dwOffsetNumSecs != FILESEEK(dwOffsetNumSecs))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = sizeof(WORD);
    if (x != FILEWRITE( (LPBYTE)&wNumSecs, x ) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //write original EntryPoint

    dwx = dwWinHdOffset + 0x28; //0x28 is offset of AddressOfEntryPoint from PE
    if (dwx != FILESEEK(dwx))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    if (4 != FILEWRITE( abyOriginalEP, 4 ) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    CLOSEOUT(EXTSTATUS_OK);

} // ExtRepairFileInca



// Function:
// WORD EXTRepairFileCabanas() -- Darren K. 10/14/98
//
// Purpose:  Fixes up the PE header and section table.
//           Virus is truncated from the file.
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

WORD EXTRepairFileCabanas(LPCALLBACKREV1  lpCallBack,     // File op callbacks
                          LPN30           lpsN30,         // Ptr to N30 structure
                          LPTSTR          lpszFileName,   // Infected filename
                          LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE                       hFile;      //assigned in FILEOPEN
    UINT                        uAttr;      //assigned in FILEOPEN

    NAVEX_IMAGE_NT_HEADERS      stPEHeader;         // PE Header
    DWORD                       dwPEHeaderOffset;   // file offset of PE Header

    WORD                        i;

    DWORD                       dwEPRVA, dwEPOffset;
    DWORD                       dwJumpDistance;
    DWORD                       dwVirusRVA, dwVirusOffset;
    WORD                        wBytesPatched;
    BYTE                        byTemp;


    // open the file and get PE header
    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    if (WINDOWS_PE_FORMAT != DetermineWindowsType(lpCallBack,
                                                   hFile,
                                                   lpbyWorkBuffer,
                                                   &dwPEHeaderOffset))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    stPEHeader = *((LPNAVEX_IMAGE_NT_HEADERS) lpbyWorkBuffer);


    //W95.Cabanas patches the host EP code with a jump to the viral code,
    //  which is appended to the host


    //seek to EP, read in e9 ?? ?? ?? ??

    dwEPRVA = DWENDIAN(stPEHeader.OptionalHeader.AddressOfEntryPoint);

    if (RVAToFileOffset(lpCallBack,
                        hFile,
                        &stPEHeader,
                        lpbyWorkBuffer,
                        dwPEHeaderOffset,
                        dwEPRVA,
                        &dwEPOffset)
                        != EXTSTATUS_OK)
        return EXTSTATUS_FILE_ERROR;

    if (dwEPOffset != FILESEEK(dwEPOffset) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (5 != FILEREAD(lpbyWorkBuffer, 5) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (lpbyWorkBuffer[0] != 0xe9)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    dwJumpDistance = AVDEREF_DWORD(lpbyWorkBuffer + 1);


    //calculate file offset of virus

    dwVirusRVA = dwEPRVA + 5 + dwJumpDistance;

    if (RVAToFileOffset(lpCallBack,
                        hFile,
                        &stPEHeader,
                        lpbyWorkBuffer,
                        dwPEHeaderOffset,
                        dwVirusRVA,
                        &dwVirusOffset)
                        != EXTSTATUS_OK)
        return EXTSTATUS_FILE_ERROR;


    //read in the first few bytes of the virus, make sure there's a CALL

    if (dwVirusOffset != FILESEEK(dwVirusOffset) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (64 != FILEREAD(lpbyWorkBuffer, 64) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (lpbyWorkBuffer[0] != 0xe8)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);


    //sanity check -- check original host EP at dwVirusOffset+5,
    //  encrypted with hard-coded ADD

    if (dwEPRVA != (DWORD)( AVDEREF_DWORD(lpbyWorkBuffer+5) + 0xb2fd26a3 ) )
        CLOSEOUT(EXTSTATUS_NO_REPAIR);


    //decrypt the original host bytes, at dwVirusOffset+11
    //encryption is hard coded in the virus -- luckily, we had access to
    //  the source code for this one, compliments of some VX site

    wBytesPatched = AVDEREF_WORD(lpbyWorkBuffer + 9);

    if (wBytesPatched > 5)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    for (i=wBytesPatched;i>0;i--)
    {
        byTemp = lpbyWorkBuffer[11 + wBytesPatched - i];

        //XOR byTemp, 0x6a
        byTemp ^= 0x6a;

        //ROL byTemp, i
        byTemp = (BYTE)( (byTemp << i) | (byTemp >> (8-i)) ) ;

        lpbyWorkBuffer[11 + wBytesPatched - i] = byTemp;
    }


    //write original host bytes to EP
    if (dwEPOffset != FILESEEK(dwEPOffset) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (wBytesPatched != FILEWRITE(lpbyWorkBuffer+11, wBytesPatched) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //truncate the virus
    if (dwVirusOffset != FILESEEK(dwVirusOffset) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (0 != FILEWRITE(lpbyWorkBuffer, 0) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //we're done!
    CLOSEOUT(EXTSTATUS_OK);

}//EXTRepairFileCabanas


// Function:
// WORD EXTRepairFileHPS() -- Darren K. 11/12/98
//
// Purpose:  Fixes up the PE header
//           Virus is truncated from the file.
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

WORD EXTRepairFileHPS(LPCALLBACKREV1  lpCallBack,     // File op callbacks
                      LPN30           lpsN30,         // Ptr to N30 structure
                      LPTSTR          lpszFileName,   // Infected filename
                      LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE                       hFile;      //assigned in FILEOPEN
    UINT                        uAttr;      //assigned in FILEOPEN

    NAVEX_IMAGE_NT_HEADERS      stPEHeader;         // PE Header
    DWORD                       dwPEHeaderOffset;   // file offset of PE Header

    WORD                        wResult;
    DWORD                       dwx;
    DWORD                       dwEPRVA, dwEPOffset;
    DWORD                       dwVirusOffset, dwDecryptKey;
    BYTE                        byDecryptFlags;



    //open the file and get PE header

    if (0 != FILEOPEN()) return(EXTSTATUS_FILE_ERROR);

    if (WINDOWS_PE_FORMAT != DetermineWindowsType(lpCallBack,
                                                   hFile,
                                                   lpbyWorkBuffer,
                                                   &dwPEHeaderOffset))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    stPEHeader = *((LPNAVEX_IMAGE_NT_HEADERS) lpbyWorkBuffer);


    //calculate offset of entrypoint

    dwEPRVA = DWENDIAN(stPEHeader.OptionalHeader.AddressOfEntryPoint);

    if (RVAToFileOffset(lpCallBack,
                        hFile,
                        &stPEHeader,
                        lpbyWorkBuffer,
                        dwPEHeaderOffset,
                        dwEPRVA,
                        &dwEPOffset)
                        != EXTSTATUS_OK)
        return EXTSTATUS_FILE_ERROR;


    //locate the start of the virus, and get the decryption key
    wResult = LocateHPSStart(lpCallBack,
                             hFile,
                             lpbyWorkBuffer,
                             &stPEHeader,
                             dwPEHeaderOffset,
                             dwEPOffset,
                             &dwVirusOffset,
                             &dwDecryptKey,
                             &byDecryptFlags);

    if (wResult != EXTSTATUS_VIRUS_FOUND)
        return EXTSTATUS_NO_REPAIR;


    //read in the first 24 bytes of the encrypted virus
    if (dwVirusOffset != FILESEEK(dwVirusOffset) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (24 != FILEREAD(lpbyWorkBuffer, 24) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //decrypt 'em
    if (EXTSTATUS_OK != DecryptBufferWithKey(lpbyWorkBuffer,
                                             24,
                                             byDecryptFlags,
                                             dwDecryptKey,
                                             0) )
        CLOSEOUT(EXTSTATUS_NO_REPAIR);



    //write original EntryPoint
    //the DWORD at dwVirusOffset+20 is the host EP

    dwx = dwPEHeaderOffset + 0x28; //0x28 is offset of AddressOfEntryPoint
    if (dwx != FILESEEK(dwx))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    if (4 != FILEWRITE( lpbyWorkBuffer+20, 4 ) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //truncate the virus
    if (dwVirusOffset != FILESEEK(dwVirusOffset) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (0 != FILEWRITE(lpbyWorkBuffer, 0) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //we're done
    CLOSEOUT(EXTSTATUS_OK);


}//EXTRepairFileHPS



// Function:
// WORD EXTRepairFilePadania() -- Darren K. 11/16/98
//
// Purpose:  Fixes up the PE header and section table.
//           Virus is truncated from the file.
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

#define PADANIA_INFECT_EP       0
#define PADANIA_INFECT_PATCH    7


WORD EXTRepairFilePadania(LPCALLBACKREV1  lpCallBack, // File op callbacks
                          LPN30           lpsN30,     // Ptr to N30 structure
                          LPTSTR          lpszFileName,   // Infected filename
                          LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE                           hFile;      //assigned in FILEOPEN
    UINT                            uAttr;      //assigned in FILEOPEN

    WORD                            x, i;          // Misc variable
    DWORD                           dwx;

    DWORD                           dwPEHeaderOffset;  // PE Header Offset
    NAVEX_IMAGE_NT_HEADERS          stPEHeader;        // PE Header

    DWORD                           dwSectionHeaderOffset;
    LPNAVEX_IMAGE_SECTION_HEADER    lpstSectionHeader;
    LPBYTE                          lpbyVirusCode;

    WORD                            wNumSecs;
    DWORD                           dwVirusEntryOffset, dwVirusMainOffset;
    DWORD                           dwOriginalEP;
    DWORD                           dwPatchRVA, dwPatchOffset;



    // Open the file and get the PE header

    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    if (WINDOWS_PE_FORMAT != DetermineWindowsType(lpCallBack,
                                                   hFile,
                                                   lpbyWorkBuffer,
                                                   &dwPEHeaderOffset))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    stPEHeader = *((LPNAVEX_IMAGE_NT_HEADERS) lpbyWorkBuffer);


    //read in virus section header and code (61 bytes) into lpbyWorkBuffer

    wNumSecs = WENDIAN(stPEHeader.FileHeader.NumberOfSections);
    if (wNumSecs < 1)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    dwSectionHeaderOffset = dwPEHeaderOffset +
                            sizeof(NAVEX_IMAGE_NT_HEADERS) +
                            (wNumSecs-1)*sizeof(NAVEX_IMAGE_SECTION_HEADER);

    if (dwSectionHeaderOffset != FILESEEK(dwSectionHeaderOffset))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    x = sizeof(NAVEX_IMAGE_SECTION_HEADER) + 61;
    if ( x != FILEREAD( lpbyWorkBuffer, x) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //set some variables

    lpstSectionHeader   = (LPNAVEX_IMAGE_SECTION_HEADER)lpbyWorkBuffer;
    lpbyVirusCode       = lpbyWorkBuffer + sizeof(NAVEX_IMAGE_SECTION_HEADER);
    dwVirusEntryOffset  = dwSectionHeaderOffset
        + sizeof(NAVEX_IMAGE_SECTION_HEADER);
    dwVirusMainOffset   = DWENDIAN(lpstSectionHeader->PointerToRawData);


    //sanity check

    if ( AVDEREF_DWORD(lpbyWorkBuffer) != (DWORD)0x61646150 )   //"Pada"
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    if ( AVDEREF_DWORD(lpbyVirusCode+16) != (DWORD)0x274e4450 ) //"PDN'"
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    if ( lpbyVirusCode[41] != (BYTE)0xeb )
        CLOSEOUT(EXTSTATUS_NO_REPAIR);



    //undo host code patch, or fix host EP, depending on how host was infected

    // Viral entry code immediately follows section table.
    // Code to return to host starts at offset 41 from viral entry code.
    //
    // offset:   bytes:              instruction:
    // ------    -----               -----------
    //
    // 41:       eb 00/07            jmp host1/host2
    //
    // host1: (EP)
    // 43:       b8 dwHostVA         mov eax, dwHostVA
    // 48:       ff e0               jmp eax
    //
    // host2: (PATCH)
    // 50:       byHostInstr[6]      patched host instruction (prefixed by 90's)
    // 56:       e9 Host_Code        jmp Host_Code, after patch

    switch( lpbyVirusCode[42] )
    {
        case PADANIA_INFECT_EP:

            //calculate original EP
            dwOriginalEP = AVDEREF_DWORD(lpbyVirusCode + 44) -
                DWENDIAN(stPEHeader.OptionalHeader.ImageBase);

            //write the original EP
            dwx = dwPEHeaderOffset + 0x28;     // PE+0x28: AddressOfEntryPoint
            if (dwx != FILESEEK(dwx))
                CLOSEOUT(EXTSTATUS_FILE_ERROR);

            x = sizeof(DWORD);
            if (x != FILEWRITE( (LPBYTE)&dwOriginalEP, x ) )
                CLOSEOUT(EXTSTATUS_FILE_ERROR);

            break;


        case PADANIA_INFECT_PATCH:

            //calculate RVA of patched code
            dwPatchRVA =
                dwVirusEntryOffset + 61 +             //base address
                AVDEREF_DWORD(lpbyVirusCode + 57) -   //relative JMP offset
                6;                                    //adjustment for patch

            //convert to file offset
            if (RVAToFileOffset(lpCallBack,
                                hFile,
                                &stPEHeader,
                                lpbyWorkBuffer,
                                dwPEHeaderOffset,
                                dwPatchRVA,
                                &dwPatchOffset)
                                != EXTSTATUS_OK)
                return EXTSTATUS_FILE_ERROR;

            //write the original host bytes (up to 6 bytes)

            for (i=0;i<6;i++)
            {
                if (lpbyVirusCode[50+i] != 0x90)
                    break;
                else
                    dwPatchOffset ++;
            }

            if (dwPatchOffset != FILESEEK(dwPatchOffset))
                CLOSEOUT(EXTSTATUS_FILE_ERROR);

            if ( (6-i) != FILEWRITE(lpbyVirusCode+50+i, 6-i) )
                CLOSEOUT(EXTSTATUS_FILE_ERROR);

            break;


        default:
            CLOSEOUT(EXTSTATUS_NO_REPAIR);
            break;

    }//switch


    //truncate the virus

    if (dwVirusMainOffset != FILESEEK(dwVirusMainOffset))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    if (0 != FILEWRITE( lpbyWorkBuffer, 0) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //zero out the last section header and code (0x28+61 bytes)

    if (dwSectionHeaderOffset != FILESEEK(dwSectionHeaderOffset))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    for (i=0;i<0x28+61;i++)
    {
        lpbyWorkBuffer[i] = 0;
    }

    if (0x28+61 != FILEWRITE(lpbyWorkBuffer,0x28+61))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);


    //decrement the number of sections and write it to PE header

    wNumSecs --;

    dwx = dwPEHeaderOffset + 0x06;              // PE+0x06: NumberOfSections
    if (dwx != FILESEEK(dwx))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    x = sizeof(WORD);
    if (x != FILEWRITE( (LPBYTE)&wNumSecs, x ) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //we're done!
    CLOSEOUT(EXTSTATUS_OK);

} // ExtRepairFilePadania



// Function:
// WORD EXTRepairFileParvo() -- Darren K. 11/23/98
//
// Purpose:  Fixes up the PE header
//           Virus is truncated from the file.
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

WORD EXTRepairFileParvo(LPCALLBACKREV1  lpCallBack,     // File op callbacks
                        LPN30           lpsN30,         // Ptr to N30 structure
                        LPTSTR          lpszFileName,   // Infected filename
                        LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE                       hFile;      //assigned in FILEOPEN
    UINT                        uAttr;      //assigned in FILEOPEN

    NAVEX_IMAGE_NT_HEADERS      stPEHeader;         // PE Header
    DWORD                       dwPEHeaderOffset;   // file offset of PE Header

    WORD                        i, wResult;
    DWORD                       dwEPRVA, dwEPOffset;
    DWORD                       dwVirusOffset, dwDecryptKey;
    BYTE                        byDecryptFlags;

    DWORD                       dwHostDataOffset;
    WORD                        wNumBytesToPatch;



    //open the file and get PE header

    if (0 != FILEOPEN()) return(EXTSTATUS_FILE_ERROR);

    if (WINDOWS_PE_FORMAT != DetermineWindowsType(lpCallBack,
                                                   hFile,
                                                   lpbyWorkBuffer,
                                                   &dwPEHeaderOffset))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    stPEHeader = *((LPNAVEX_IMAGE_NT_HEADERS) lpbyWorkBuffer);


    //calculate offset of entrypoint

    dwEPRVA = DWENDIAN(stPEHeader.OptionalHeader.AddressOfEntryPoint);

    if (RVAToFileOffset(lpCallBack,
                        hFile,
                        &stPEHeader,
                        lpbyWorkBuffer,
                        dwPEHeaderOffset,
                        dwEPRVA,
                        &dwEPOffset)
                        != EXTSTATUS_OK)
        return EXTSTATUS_FILE_ERROR;


    //locate the start of the virus, and get the decryption key
    wResult = LocateParvoStart(lpCallBack,
                               hFile,
                               lpbyWorkBuffer,
                               &stPEHeader,
                               dwPEHeaderOffset,
                               dwEPOffset,
                               &dwVirusOffset,
                               &dwDecryptKey,
                               &byDecryptFlags);

    if (wResult != EXTSTATUS_VIRUS_FOUND)
        return EXTSTATUS_NO_REPAIR;


    //read in the original host data:
    //  Virus+0x2042:  dwNumBytes
    //  Virus+0x2046:  (DWORD) 0
    //  Virus+0x204a:  byOriginalHostBytes[dwNumBytes]

    //make sure we're on a DWORD boundary!
    dwHostDataOffset = dwVirusOffset + 0x2040;

    if (dwHostDataOffset != FILESEEK(dwHostDataOffset) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (512 != FILEREAD(lpbyWorkBuffer, 512) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);



    //decrypt original host data

    //1st decryption:  use key
    if (EXTSTATUS_OK != DecryptBufferWithKey(lpbyWorkBuffer,
                                             512,
                                             byDecryptFlags,
                                             dwDecryptKey,
                                             0) )
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    //2nd decryption:  ROL byte, 4
    for (i=0;i<512;i++)
    {
        lpbyWorkBuffer[i] = (BYTE)
            ( (lpbyWorkBuffer[i]<<4) | (lpbyWorkBuffer[i]>>4) );
    }


    //calculate the number of bytes to patch

    wNumBytesToPatch = AVDEREF_WORD(lpbyWorkBuffer+2);

    if (wNumBytesToPatch > 256)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);


    //write the original host bytes

    if (dwEPOffset != FILESEEK(dwEPOffset))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    if (wNumBytesToPatch != FILEWRITE(lpbyWorkBuffer+10,wNumBytesToPatch) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //truncate the virus
    if (dwVirusOffset != FILESEEK(dwVirusOffset) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (0 != FILEWRITE(lpbyWorkBuffer, 0) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //we're done
    CLOSEOUT(EXTSTATUS_OK);


}//EXTRepairFileParvo



// Function:
// WORD EXTRepairFileK32() -- Darren K. 12/10/98
//
// Purpose:  Fixes up the PE header
//           Virus is truncated from the file.
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

WORD EXTRepairFileK32(LPCALLBACKREV1  lpCallBack,     // File op callbacks
                        LPN30           lpsN30,         // Ptr to N30 structure
                        LPTSTR          lpszFileName,   // Infected filename
                        LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE                       hFile;      //assigned in FILEOPEN
    UINT                        uAttr;      //assigned in FILEOPEN

    NAVEX_IMAGE_NT_HEADERS      stPEHeader;         // PE Header
    DWORD                       dwPEHeaderOffset;   // file offset of PE Header

    DWORD                       dwEPRVA, dwEPOffset;
    DWORD                       dwOriginalEP;


    //open the file and get PE header

    if (0 != FILEOPEN()) return(EXTSTATUS_FILE_ERROR);

    if (WINDOWS_PE_FORMAT != DetermineWindowsType(lpCallBack,
                                                   hFile,
                                                   lpbyWorkBuffer,
                                                   &dwPEHeaderOffset))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    stPEHeader = *((LPNAVEX_IMAGE_NT_HEADERS) lpbyWorkBuffer);


    //calculate offset of entrypoint ( == offset of virus )

    dwEPRVA = DWENDIAN(stPEHeader.OptionalHeader.AddressOfEntryPoint);

    if (RVAToFileOffset(lpCallBack,
                        hFile,
                        &stPEHeader,
                        lpbyWorkBuffer,
                        dwPEHeaderOffset,
                        dwEPRVA,
                        &dwEPOffset)
                        != EXTSTATUS_OK)
        return EXTSTATUS_FILE_ERROR;


    //read in bytes from Virus + 0x7b7
    if (dwEPOffset+0x7b7 != FILESEEK(dwEPOffset+0x7b7) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (20 != FILEREAD(lpbyWorkBuffer, 20) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    //bookmarks

    if (lpbyWorkBuffer[0] != 0xb8)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    if (AVDEREF_DWORD(lpbyWorkBuffer+5) != 0x858be0ff)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    //original EP at Virus+0x7b8, stored as absolute VA

    dwOriginalEP = AVDEREF_DWORD(lpbyWorkBuffer+1)
        - DWENDIAN(stPEHeader.OptionalHeader.ImageBase);


    //write the original EP (at PE+0x28)

    if (dwPEHeaderOffset+0x28 != FILESEEK(dwPEHeaderOffset+0x28))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (4 != FILEWRITE( (LPBYTE)&dwOriginalEP,4) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //truncate the virus
    if (dwEPOffset != FILESEEK(dwEPOffset) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (0 != FILEWRITE(lpbyWorkBuffer, 0) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //we're done!
    CLOSEOUT(EXTSTATUS_OK);


}//EXTRepairFileK32


// Function:
// WORD EXTRepairFileRemoteExplore()
//
// Purpose:  Extract the compressed ( & encrypted?) host
//            from the .rsrc section and decompress it.
//
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


#define WINDOW_SIZE      2048
#define RCDATA_HOST_NAME 0x000022AF //seems constant
WORD EXTRepairFileRemoteExplore(LPCALLBACKREV1  lpCallBack,     // File op callbacks
                                LPN30           lpsN30,         // Ptr to N30 structure
                                LPTSTR          lpszFileName,   // Infected filename
                                LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE                       hFile;      //assigned in FILEOPEN
    UINT                        uAttr;      //assigned in FILEOPEN
    DWORD                       dwFileSize;
    WORD                        i;
    LPBYTE                      lpbyInitWorkBuffer;
    BOOL                        bResult;

    LPNAVEX_IMAGE_NT_HEADERS    lpstPEHeader;         // PE Header
    DWORD                       dwPEHeaderOffset;   // file offset of PE Header

    DWORD                       dwCurrentSection;
    LPNAVEX_IMAGE_SECTION_HEADER  lpstSectionHeader;
    DWORD                       dwSectionFileOffset;
    DWORD                       dwSectionLength;
    DWORD                       dwSectionFlags;
    DWORD                       dwSectionRVABase;

    LPNAVEX_RESOURCE_DIR        lpstResourceDirectory,
                                lpstRCDATADirectory;
    LPNAVEX_RESOURCE_DIR_ENTRY  lpstResourceDirectoryEntry,
                                lpstRCDATADirectoryEntry,
                                lpstRCDATADataEntry;
    WORD                        wNumberOfEntry;
    DWORD                       dwOffsetToData,
                                dwType,
                                dwCompressedHostOffset,
                                dwCompressedHostLength;

    lpbyInitWorkBuffer = lpbyWorkBuffer;
    //open the file and get PE header
    if (0 != FILEOPEN())
        return(EXTSTATUS_FILE_ERROR);

    if (WINDOWS_PE_FORMAT != DetermineWindowsType(lpCallBack,
                                                   hFile,
                                                   lpbyWorkBuffer,
                                                   &dwPEHeaderOffset))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    lpstPEHeader = (LPNAVEX_IMAGE_NT_HEADERS) lpbyWorkBuffer;

    dwFileSize = FILESIZE();

    //search for the .rsrc section starting from last section
    dwCurrentSection = DWENDIAN(lpstPEHeader->FileHeader.NumberOfSections);
    dwCurrentSection--;
    do
    {
      lpbyWorkBuffer=lpbyInitWorkBuffer;
      if (EXTSTATUS_OK != GetPESectionInfo(lpCallBack,
                                         hFile,
                                         lpbyWorkBuffer,
                                         dwPEHeaderOffset,
                                         dwCurrentSection,
                                         &dwSectionFileOffset,
                                         &dwSectionLength,
                                         &dwSectionFlags,
                                         &dwSectionRVABase))
         CLOSEOUT(EXTSTATUS_NO_REPAIR);

      lpstSectionHeader = (LPNAVEX_IMAGE_SECTION_HEADER) lpbyWorkBuffer;

      if (  (lpstSectionHeader->Name[0]=='.')
         && (lpstSectionHeader->Name[1]=='r')
         && (lpstSectionHeader->Name[2]=='s')
         && (lpstSectionHeader->Name[3]=='r')
         && (lpstSectionHeader->Name[4]=='c')
         && (lpstSectionHeader->Name[5]== 0)
         && (lpstSectionHeader->Name[6]== 0)
         && (lpstSectionHeader->Name[7]== 0))
           break;
      dwCurrentSection--;
    }while (dwCurrentSection >= 0);


    if (dwCurrentSection < 0)
    {
       //can't find .rsrc section
       CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }
    //seek to the .rsrc Section
    if (dwSectionFileOffset != FILESEEK(dwSectionFileOffset))
       CLOSEOUT(EXTSTATUS_FILE_ERROR);

    //read WINDOW_SIZE bytes from .rsrc Section into buffer
    lpbyWorkBuffer=lpbyInitWorkBuffer;
    if ((UINT)-1 == FILEREAD(lpbyWorkBuffer, WINDOW_SIZE))
    {
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    }

    lpstResourceDirectory = (LPNAVEX_RESOURCE_DIR)lpbyWorkBuffer;
    wNumberOfEntry = lpstResourceDirectory->NumberOfNamedEntries
                     + lpstResourceDirectory->NumberOfIdEntries;

    //points to the first resource directory entry
    lpbyWorkBuffer+=NAVEX_SIZEOF_RESOURCE_DIR;
    for(i=0; i<wNumberOfEntry; i++)
    {
       lpstResourceDirectoryEntry = (LPNAVEX_RESOURCE_DIR_ENTRY)
                     (lpbyWorkBuffer + i * NAVEX_SIZEOF_RESOURCE_DIR_ENTRY);
       dwOffsetToData = lpstResourceDirectoryEntry->OffsetToData
                        & NAVEX_RES_DIR_OFFSET_MASK;
       dwType = lpstResourceDirectoryEntry->OffsetToData
                & NAVEX_RES_DIR_OFFSET_TO_SUBDIR;
       if ( (lpstResourceDirectoryEntry->Name==NAVEX_RES_DIR_TYPE_RCDATA)
            &&(dwType==NAVEX_RES_DIR_OFFSET_TO_SUBDIR) )
           break;
    }

    if (i==wNumberOfEntry)
    {
        //can't find RCDATA
        CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }

    //seek to the RCDATA directory
    dwOffsetToData += dwSectionFileOffset;
    if (dwOffsetToData != FILESEEK(dwOffsetToData))
    {
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    }
    //read the RCDATA directory
    lpbyWorkBuffer=lpbyInitWorkBuffer;
    if ((UINT)-1 == FILEREAD(lpbyWorkBuffer, WINDOW_SIZE))
    {
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    }
    lpstRCDATADirectory=(LPNAVEX_RESOURCE_DIR)lpbyWorkBuffer;
    wNumberOfEntry = lpstRCDATADirectory->NumberOfNamedEntries
                     + lpstRCDATADirectory->NumberOfIdEntries;

    if (wNumberOfEntry<3)
    {
        //must be the IE403R.SYS or TASKMGR.SYS file
        CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }

    lpbyWorkBuffer+=NAVEX_SIZEOF_RESOURCE_DIR;

    for(i=0; i<wNumberOfEntry; i++)
    {
       lpstRCDATADirectoryEntry = (LPNAVEX_RESOURCE_DIR_ENTRY)
                      (lpbyWorkBuffer + i * NAVEX_SIZEOF_RESOURCE_DIR_ENTRY);
       dwOffsetToData = lpstRCDATADirectoryEntry->OffsetToData
                         & NAVEX_RES_DIR_OFFSET_MASK;
       dwType = lpstRCDATADirectoryEntry->OffsetToData
                         & NAVEX_RES_DIR_OFFSET_TO_SUBDIR;
       if ( (lpstRCDATADirectoryEntry->Name==RCDATA_HOST_NAME)
            &&(dwType==NAVEX_RES_DIR_OFFSET_TO_SUBDIR) )
            break;
    }

    if ( (lpstRCDATADirectoryEntry->Name!=RCDATA_HOST_NAME)
           ||(dwType!=NAVEX_RES_DIR_OFFSET_TO_SUBDIR) )
    {
        //fail to find compressed host in RCDATA directory
        CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }

    //seek to the RCDATA subdirectory
    dwOffsetToData += dwSectionFileOffset;
    if (dwOffsetToData != FILESEEK(dwOffsetToData))
         CLOSEOUT(EXTSTATUS_FILE_ERROR);
    //read the RCDATA subdirectory
    lpbyWorkBuffer=lpbyInitWorkBuffer;
    if ((UINT)-1 == FILEREAD(lpbyWorkBuffer, WINDOW_SIZE))
    {
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    }
    lpstRCDATADirectory=(LPNAVEX_RESOURCE_DIR)lpbyWorkBuffer;
    wNumberOfEntry = lpstRCDATADirectory->NumberOfNamedEntries
                     + lpstRCDATADirectory->NumberOfIdEntries;

    lpbyWorkBuffer+=NAVEX_SIZEOF_RESOURCE_DIR;
    if (wNumberOfEntry > 1)
    {
       //the subdir should only have one entry!
       CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }

    lpstRCDATADirectoryEntry = (LPNAVEX_RESOURCE_DIR_ENTRY)lpbyWorkBuffer;
    dwOffsetToData = lpstRCDATADirectoryEntry->OffsetToData
                     & NAVEX_RES_DIR_OFFSET_MASK;
    dwType = lpstRCDATADirectoryEntry->OffsetToData
             & NAVEX_RES_DIR_OFFSET_TO_SUBDIR;
    if (dwType==NAVEX_RES_DIR_OFFSET_TO_SUBDIR)
    {
       //we should be getting DataPointer now, not another SUBDIR.
       CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }

    //seek to the RCDATA data entry
    dwOffsetToData += dwSectionFileOffset;
    if (dwOffsetToData != FILESEEK(dwOffsetToData))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    //read the RCDATA subdirectory
    lpbyWorkBuffer=lpbyInitWorkBuffer;
    if ((UINT)-1 == FILEREAD(lpbyWorkBuffer, WINDOW_SIZE))
    {
        CLOSEOUT(EXTSTATUS_FILE_ERROR);
    }

    lpstRCDATADataEntry = (LPNAVEX_RESOURCE_DIR_ENTRY)lpbyWorkBuffer;

    //find offset and length of the compressed host
    dwCompressedHostOffset = lpstRCDATADataEntry->Name - 0x0000C200;
    dwCompressedHostLength = lpstRCDATADataEntry->OffsetToData;

    //DEFLATE the host
    lpbyWorkBuffer=lpbyInitWorkBuffer;
    bResult=RemoteXRestoreHost(lpCallBack, hFile, dwCompressedHostOffset,
                       dwCompressedHostLength, lpbyWorkBuffer, WINDOW_SIZE);
    if (bResult)
    {
       CLOSEOUT(EXTSTATUS_OK);
    }
    else
    {
       CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }

}//EXTRepairFileRemoteExplore

// Function:
// WORD EXTRepairFileRemoteXCorrupt()
//
// Purpose:  Extract the compressed ( & encrypted?) host
//            from the .rsrc section and decompress it.
//
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

WORD EXTRepairFileRemoteXCorrupt(LPCALLBACKREV1  lpCallBack,     // File op callbacks
                                 LPN30           lpsN30,         // Ptr to N30 structure
                                 LPTSTR          lpszFileName,   // Infected filename
                                 LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE                        hFile;      //assigned in FILEOPEN
    UINT                         uAttr;      //assigned in FILEOPEN

    (void)lpsN30;

    if (0 != FILEOPEN())                // Open file
        return(EXTSTATUS_NO_REPAIR);

    if (RemoteXRestoreNonEXEHost(lpCallBack,
                                 hFile,
                                 lpbyWorkBuffer,
                                 WINDOW_SIZE) == FALSE)
    {
       CLOSEOUT(EXTSTATUS_NO_REPAIR);
    }
    else
    {
       CLOSEOUT(EXTSTATUS_OK);
    }
}//EXTRepairFileRemoteXCorrupt


#ifdef SARC_CUST_11108

// Function:
// WORD EXTRepairFileMemorial() -- Darren K. 01/14/1999
//
// Purpose:  Fixes up the PE header
//           Virus is truncated from the file.
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

WORD EXTRepairFileMemorial(LPCALLBACKREV1  lpCallBack,     // File op callbacks
                        LPN30           lpsN30,         // Ptr to N30 structure
                        LPTSTR          lpszFileName,   // Infected filename
                        LPBYTE          lpbyWorkBuffer) // Buffer >= 512 bytes
{
    HFILE                       hFile;      //assigned in FILEOPEN
    UINT                        uAttr;      //assigned in FILEOPEN

    NAVEX_IMAGE_NT_HEADERS      stPEHeader;         // PE Header
    DWORD                       dwPEHeaderOffset;   // file offset of PE Header

    extern  WORD                gawMemorialSig[];

    WORD                        i;
    DWORD                       dwx;

    DWORD                       dwEPRVA, dwEPOffset;

    BYTE                        byDecryptFlags;
    DWORD                       dwDecryptKey, dwDecryptDelta;

    DWORD                       dwOriginalEPOffset;
    BYTE                        abyOriginalEP[4];

    WORD                        wNumSecs;
    DWORD                       dwSectionHeaderOffset;



    //open the file and get PE header

    if (0 != FILEOPEN()) return(EXTSTATUS_FILE_ERROR);

    if (WINDOWS_PE_FORMAT != DetermineWindowsType(lpCallBack,
                                                   hFile,
                                                   lpbyWorkBuffer,
                                                   &dwPEHeaderOffset))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    stPEHeader = *((LPNAVEX_IMAGE_NT_HEADERS) lpbyWorkBuffer);


    //calculate offset of entrypoint

    dwEPRVA = DWENDIAN(stPEHeader.OptionalHeader.AddressOfEntryPoint);

    if (RVAToFileOffset(lpCallBack,
                        hFile,
                        &stPEHeader,
                        lpbyWorkBuffer,
                        dwPEHeaderOffset,
                        dwEPRVA,
                        &dwEPOffset)
                        != EXTSTATUS_OK)
        return EXTSTATUS_FILE_ERROR;


    //locate the start of the virus, and get the decryption key


    //read in 66 bytes from entry point (46 for decryptor + 20 for signature)

    if (dwEPOffset != lpCallBack->FileSeek(hFile, dwEPOffset, SEEK_SET))
        return EXTSTATUS_FILE_ERROR;

    if (UERROR == lpCallBack->FileRead(hFile, lpbyWorkBuffer, 66))
        return EXTSTATUS_FILE_ERROR;


    //verify gawMemorialSig at EP+46, encrypted using byte XOR w/ delta

    if ( GenericDecrypt(lpbyWorkBuffer + 46,
                        gawMemorialSig,
                        20,
                        DECRYPT_BYTE | DECRYPT_XOR | DECRYPT_DELTA,
                        &byDecryptFlags,
                        &dwDecryptKey,
                        &dwDecryptDelta) != EXTSTATUS_OK)
        return EXTSTATUS_NO_REPAIR;


    //read in the original EP, stored at offset Virus+0x2ec

    dwOriginalEPOffset = dwEPOffset + 0x2ec;

    if (dwOriginalEPOffset != FILESEEK(dwOriginalEPOffset) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (4 != FILEREAD(abyOriginalEP, 4) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //decrypt original EP

    //account for changing key
    //  0x2e == size of decryptor (at Virus+0x0000)
    // 0x2ec == offset of bytes to be decrypted
    dwDecryptKey += dwDecryptDelta * (0x2ec-0x2e);

    if (EXTSTATUS_OK != DecryptBufferWithKey(abyOriginalEP,
                                             4,
                                             byDecryptFlags,
                                             dwDecryptKey,
                                             dwDecryptDelta) )
        CLOSEOUT(EXTSTATUS_NO_REPAIR);


    //write the original EP (at PE+0x28)

    if (dwPEHeaderOffset+0x28 != FILESEEK(dwPEHeaderOffset+0x28))
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (4 != FILEWRITE(abyOriginalEP,4) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //find the virus section header

    wNumSecs = WENDIAN(stPEHeader.FileHeader.NumberOfSections);
    if (wNumSecs < 1)
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    dwSectionHeaderOffset = dwPEHeaderOffset +
                            sizeof(NAVEX_IMAGE_NT_HEADERS) +
                            (wNumSecs-1)*sizeof(NAVEX_IMAGE_SECTION_HEADER);


    //zero out the last section header (0x28 bytes)

    if (dwSectionHeaderOffset != FILESEEK(dwSectionHeaderOffset))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    for (i=0;i<0x28;i++)
    {
        lpbyWorkBuffer[i] = 0;
    }

    if (0x28 != FILEWRITE(lpbyWorkBuffer,0x28))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);


    //decrement the number of sections and write it to PE header

    wNumSecs --;

    dwx = dwPEHeaderOffset + 0x06;              // PE+0x06: NumberOfSections
    if (dwx != FILESEEK(dwx))
        CLOSEOUT(EXTSTATUS_NO_REPAIR);

    if (sizeof(WORD) != FILEWRITE((LPBYTE)&wNumSecs,sizeof(WORD)) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //truncate the virus
    if (dwEPOffset != FILESEEK(dwEPOffset) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);

    if (0 != FILEWRITE(lpbyWorkBuffer, 0) )
        CLOSEOUT(EXTSTATUS_FILE_ERROR);


    //we're done
    CLOSEOUT(EXTSTATUS_OK);


}//EXTRepairFileMemorial

#endif // SARC_CUST_11108



#endif  // #ifndef SYM_NLM
