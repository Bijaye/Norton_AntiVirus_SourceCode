//************************************************************************
//
// $Header:   S:/NAVEX/VCS/NAVEXSB.CPv   1.51   20 Nov 1998 16:07:10   rpulint  $
//
// Description:
//      Contains NAVEX EXTScanBoot code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/NAVEXSB.CPv  $
// 
//    Rev 1.51   20 Nov 1998 16:07:10   rpulint
// WATCOM Changes: moved extern declarations to global section from with 
// -in functions
// 
//    Rev 1.50   05 Aug 1998 18:29:46   hjaland
// Carey added code to disable the IBM scanner if he find an excluded boot rec.
// 
//    Rev 1.49   10 Jun 1998 13:42:36   AOONWAL
// Removed printf statement
// 
//    Rev 1.48   09 Jun 1998 16:55:50   CNACHEN
// updated to call naveng...
// 
//    Rev 1.47   16 Mar 1998 19:46:30   hjaland
// Removed #ifdef SARC_CST_1502 
// 
//    Rev 1.46   16 Mar 1998 17:34:26   JWILBER
// Removed error check for SARCBOT build.
//
//    Rev 1.45   16 Mar 1998 17:26:18   JWILBER
// Added error check to see if we're doing a SARCBOT build.
//
//    Rev 1.44   11 Mar 1998 21:22:16   JWILBER
// Modified EXTScanBootChan for CI 1502.  I made the string search
// start closer to the beginning of the boot sector.  Chan is VID 1d68.
//
//    Rev 1.43   10 Mar 1998 17:09:42   CFORMUL
// Malloc'd 2K for lpbyWorkBuffer.  Commented out temp buffers and
// used lpbyWorkBuffer instead in individual scan functions.
//
//    Rev 1.42   05 Feb 1998 17:19:00   DKESSNE
// added detection for Wolleh 0x6105
//
//    Rev 1.41   29 Dec 1997 17:56:20   JWILBER
// Modified during Jan98 build.
//
//    Rev JAN98  19 Dec 1997 16:19:00   ECHIEN
// Added 0xcd 0x13 to valid byte set for lilith
//
//    Rev JAN98  19 Dec 1997 12:47:58   ECHIEN
// Added 0xb9 to the lilith valid set
//
//    Rev JAN98  17 Dec 1997 17:38:26   ECHIEN
// Added Lilith
//
//    Rev 1.40   08 Dec 1997 10:15:28   CNACHEN
// Disabled bloodhound-boot/mbr for all but picasso and NAVEX1.5 NAV products.
//
//    Rev 1.39   02 Dec 1997 11:49:54   CNACHEN
// Added creation of NAVBOOFP.DAT file in C:\ if the file C:\MAKEBOO.DAT is
// found, when a bloodhound detection occurs.
//
//    Rev 1.38   20 Nov 1997 19:40:36   JWILBER
// Modified during Dec97 build.
//
//    Rev DEC97  17 Nov 1997 18:17:06   DCHI
// Added code_seg() pragma for SYM_WIN16.
//
//    Rev 1.37   10 Nov 1997 16:47:56   CNACHEN
// Removed bloodhound ifdefs
//
//    Rev 1.36   27 Oct 1997 12:28:22   JWILBER
// Modified during Nov 97 build.
//
//    Rev NOV97  20 Oct 1997 13:13:00   CFORMUL
// Made change to scan string function for cHaN detection.
//
//    Rev NOV97  20 Oct 1997 12:24:28   CFORMUL
// Made change to cHaN string scan
//
//    Rev NOV97  16 Oct 1997 17:16:58   CFORMUL
// Added floppy boot detection for cHaN
//
//    Rev 1.35   16 Oct 1997 13:01:42   CFORMUL
// Added detection for cHaN floppy boot
//
//    Rev 1.34   15 Sep 1997 11:47:58   CNACHEN
// Added check for USE_BLOOD_BOOT ifdef
//
//    Rev 1.33   05 Sep 1997 20:53:34   CNACHEN
// Removed int 3
//
//    Rev 1.32   05 Sep 1997 20:26:38   CNACHEN
// Added bloodhound boot.
//
//    Rev 1.31   05 Sep 1997 20:24:40   CNACHEN
//
//    Rev 1.30   28 Jul 1997 12:54:08   AOONWAL
// Modified during AUG97 update
//
//    Rev AUG97   17 Jul 1997 18:24:26   JWILBER
// Added EXTScanBootOneHalf, to detect OneHalf and Shin variants in boot
// sectors.  This routine calls EXTScanOneHalf in DANISH.CPP.
//
//    Rev 1.28   17 Jul 1997 12:12:42   JWILBER
// No changes.
//
//    Rev 1.27   26 Dec 1996 15:22:20   AOONWAL
// No change.
//
//    Rev 1.26   02 Dec 1996 14:00:26   AOONWAL
// No change.
//
//    Rev 1.25   29 Oct 1996 12:59:22   AOONWAL
// No change.
//
//    Rev 1.24   16 Aug 1996 18:22:52   CFORMUL
// Fixed bug in Hare code under 32-bit.
//
//    Rev 1.23   29 Jul 1996 12:26:42   SCOURSE
// Removed int 3
//
//    Rev 1.22   28 Jul 1996 17:53:40   SCOURSE
// Mod to Moloch detection -
//
//    Rev 1.21   24 Jul 1996 18:29:18   CFORMUL
// Reworked hare Boot detection so it is sure to detect all
// variants in the floppy boot.
//
//    Rev 1.20   24 Jul 1996 18:21:18   SCOURSE
// Finalized (pre-QA) Moloch
//
//    Rev 1.19   11 Jul 1996 16:47:52   CFORMUL
// Removed reference to old Hare define
//
//    Rev 1.18   08 Jul 1996 19:01:08   CRENERT
// Added #if define for Hare
//
//    Rev 1.17   08 Jul 1996 17:38:20   CFORMUL
// Added Detection for Hare.7610.A
//
//    Rev 1.16   08 Jul 1996 16:02:56   CFORMUL
// Fixed up floppy detect for Hare
//
//    Rev 1.15   03 Jul 1996 02:47:44   CFORMUL
// Added Floppy detect for Hare.7610.B
//
//    Rev 1.14   25 Jun 1996 17:04:50   JWILBER
// Added EXTScanBootDanish, which looks for Danish Boot, VID 1f8f.
// This routine calls EXTScanDanish, which is also called by
// EXTScanPartDanish.
//
//    Rev 1.13   05 Mar 1996 12:19:12   JSULTON
// Removed comments Predator in jump table.
//
//    Rev 1.12   01 Mar 1996 19:42:50   JWILBER
// Added EXTScanBootPredt, so we can pick up Predator on floppy disks,
// too, and not just MBRs.  It is commented out of the jump table.
//
//    Rev 1.11   13 Feb 1996 17:56:02   MKEATIN
// Added DX Support
//
//    Rev 1.10   30 Jan 1996 15:43:34   DCHI
// Added #ifndef SYM_NLM and #endif pairs to ifdef out functions unused
// on NLM platform.
//
//    Rev 1.9   19 Jan 1996 14:50:36   JALLEE
// Rev 1.7
//
//    Rev 1.7   10 Jan 1996 13:20:24   JALLEE
// Changed return value of Neuroquila boot scanning function.
//
//    Rev 1.6   13 Nov 1995 19:26:42   JALLEE
// Bug Fix Cast -1 to WORD
//
//    Rev 1.5   09 Nov 1995 13:32:38   JALLEE
// Added boot detection for Neuroquila.
//
//    Rev 1.4   01 Nov 1995 10:49:06   DCHI
// Removed include of navexhdr.h due to change in format of navexshr.h.
//
//    Rev 1.3   19 Oct 1995 14:20:56   DCHI
// Added LOADDS to external functions for WIN16.  Modified DOS export
// table structure to support tiny model modules.
//
//    Rev 1.2   18 Oct 1995 11:49:06   CNACHEN
// Voided unused parameters to build without errors under NLM.
//
//    Rev 1.1   16 Oct 1995 13:02:10   DCHI
// Added include of navexhdr.h.
//
//    Rev 1.0   13 Oct 1995 13:05:44   DCHI
// Initial revision.
//
//************************************************************************

#ifdef SYM_WIN16
#pragma code_seg("NAVEX_NAVEXSB","NAVEX_BOOT_CODE")
#endif

#ifndef SYM_NLM

#include "platform.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
#include "ctsn.h"
#include "callback.h"
#endif



#include "navex.h"

// Declare shared routines

#include "navexshr.h"

// prototype for bloodhound boot

#include "booscan.h"

#ifdef SYM_DOSX
#undef SYM_DOS
#endif

#if defined(SYM_DOS)

WORD wDS, wES;                      // save loader's DS,ES

#endif

#ifdef SARCBOT
#include "config.h"
#endif

#ifdef NAVEX15
    #ifdef __WATCOMC__
            extern PEXPORT15_TABLE_TYPE     glpstEngineExportTable;
    #endif
#endif


// Prototypes for local Boot scanning functions

// Def Writer: Your prototype here.

WORD EXTScanBootNeuro (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);

WORD EXTScanBootPredt (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);

WORD EXTScanBootDanish (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);

WORD EXTScanBootHare (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);

WORD EXTScanBootMoloch (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);

WORD EXTScanBootWolleh (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);

WORD EXTScanBootChan (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);

WORD EXTScanBootOneHalf (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);

WORD EXTScanBootLilith (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);

WORD EXTScanBootBloodhound (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID);


// Jump Table used by EXTScanBoot

WORD    (*SBJumpTable[]) (LPCALLBACKREV1 lpCallBack,
                          BYTE           byDrive,
                          BYTE           byPartitionNum,
                          WORD           wSector,
                          WORD           wCylinder,
                          WORD           wHead,
                          LPBYTE         lpbyBootBuffer,
                          LPBYTE         lpbyWorkBuffer,
                          WORD           wVersionNumber,
                          LPWORD         lpwVID) = {

    // Def Writer: Insert your function name here.

    EXTScanBootNeuro,           // boot detect for neuroquila
    EXTScanBootPredt,           // boot detect for predator
    EXTScanBootDanish,          // boot detect for danish boot
    EXTScanBootMoloch,          // boot detect for Moloch
    EXTScanBootWolleh,          // boot detect for Wolleh
    EXTScanBootChan,            // boot detect for Chan
    EXTScanBootHare,            // boot detect for Hare
    EXTScanBootOneHalf,         // boot detect for OneHalf and Shin variants
    EXTScanBootLilith,          // boot detect for Lilith
    EXTScanBootBloodhound,      // THIS MUST ALWAYS BE LAST!!!!!!!!!!!
    NULL
};                              // wID = 0 => end of table.

#define WORK_BUFFER_SIZE 0x800

// EXTScanBoot arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_READ_ERROR          if disk error occured
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//                        EXTSTATUS_MEM_ERROR           if memory alloc. error
//

EXTSTATUS FAR WINAPI NLOADDS EXTScanBoot(LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{

#if defined(SYM_DOS)

    _asm
        {
        mov cs:[wDS], ds
        mov cs:[wES], es
        mov ax, cs
        mov ds, ax
        mov es, ax
        }

#endif

    WORD        cwI=0;
    WORD        wReturn = EXTSTATUS_OK;

    (void)lpCallBack;
    (void)byDrive;
    (void)byPartitionNum;
    (void)wSector;
    (void)wCylinder;
    (void)wHead;
    (void)lpbyBootBuffer;
    (void)lpbyWorkBuffer;
    (void)wVersionNumber;
    (void)lpwVID;

#ifndef SYM_WIN32
    lpbyWorkBuffer = (LPBYTE) lpCallBack->PermMemoryAlloc (WORK_BUFFER_SIZE);
#else
    lpbyWorkBuffer = (LPBYTE) GlobalAlloc (GMEM_FIXED, WORK_BUFFER_SIZE);
#endif

    while (SBJumpTable[cwI] != NULL && wReturn != EXTSTATUS_VIRUS_FOUND)
    {
        wReturn = SBJumpTable[ cwI++ ] ( lpCallBack,
                                         byDrive,
                                         byPartitionNum,
                                         wSector,
                                         wCylinder,
                                         wHead,
                                         lpbyBootBuffer,
                                         lpbyWorkBuffer,
                                         wVersionNumber,
                                         lpwVID);
    }

#if defined(SYM_DOS)
    _asm
        {
        mov ds, cs:[wDS]
        mov es, cs:[wES]
        }

#endif



    //////////////////////////////////////////////////////////////////////////
    // Now see if we have an engine virus if we havent' found one so far
    //////////////////////////////////////////////////////////////////////////

#ifdef NAVEX15

//    printf("About to scan boot!\n");

    if (wReturn != EXTSTATUS_VIRUS_FOUND)
    {
#ifndef __WATCOMC__
        extern PEXPORT15_TABLE_TYPE     glpstEngineExportTable;
#endif

        if (glpstEngineExportTable != NULL)
        {
            wReturn = glpstEngineExportTable->
                            EXTScanBoot(lpCallBack,
                                        byDrive,
                                        byPartitionNum,
                                        wSector,
                                        wCylinder,
                                        wHead,
                                        lpbyBootBuffer,
                                        lpbyWorkBuffer,
                                        wVersionNumber,
                                        lpwVID);
        }
    }

#endif // #ifdef NAVEX15

    //////////////////////////////////////////////////////////////////////////////
    // Done with engine work
    //////////////////////////////////////////////////////////////////////////////

#ifndef SYM_WIN32
    lpCallBack->PermMemoryFree ((LPVOID) lpbyWorkBuffer);
#else
    GlobalFree ((LPVOID) lpbyWorkBuffer);
#endif

    return ( wReturn );
}

// *************************************
// Local Boot scanning functions
// Def Writer: Insert your function here
// *************************************

// EXTScanBootNeuro     boot detection for Neuroquila
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot sectors
//                        on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanBootNeuro (LPCALLBACKREV1 lpCallBack,
                       BYTE           byDrive,
                       BYTE           byPartitionNum,
                       WORD           wSector,
                       WORD           wCylinder,
                       WORD           wHead,
                       LPBYTE         lpbyBootBuffer,
                       LPBYTE         lpbyWorkBuffer,
                       WORD           wVersionNumber,
                       LPWORD         lpwVID)

{

    BYTE    bSearch[] = {0xcd, 0x13, 0x72, 0xfe, 0xcb};
    WORD    cwI,wReturn = EXTSTATUS_OK;

    (void)lpCallBack;
    (void)byDrive;
    (void)byPartitionNum;
    (void)wSector;
    (void)wCylinder;
    (void)wHead;
    (void)lpbyWorkBuffer;
    (void)wVersionNumber;


    if ((WORD)-1 != (cwI = ScanString(lpbyBootBuffer + 25,
                       485,             // sizeof lpbyBootBuffer - 25 - 2
                       bSearch,         // that for which we search
                       sizeof(bSearch))))
    {
        // Look for these bytes in front of the string we found for
        // a double check.

        if (lpbyBootBuffer[25 + cwI - 3] == 0xba &&
            lpbyBootBuffer[25 + cwI - 7] == 0x02 &&
            lpbyBootBuffer[25 + cwI - 10] == 0x93)
        {
            wReturn = EXTSTATUS_VIRUS_FOUND;
            *lpwVID = VID_NEURO_BOOT;
        }
    }
    return (wReturn);
}

// EXTScanBootPredt     boot detection for Predator
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanBootPredt (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{

    BYTE    bSearch[] = { 0x2e, 0xd3, 0x0d, 0xd1, 0xc1,
                          0x47, 0x47, 0x48, 0x75, 0xf6 };
    WORD    wReturn = EXTSTATUS_OK;

    (void)lpCallBack;
    (void)byDrive;
    (void)byPartitionNum;
    (void)wSector;
    (void)wCylinder;
    (void)wHead;
    (void)lpbyWorkBuffer;
    (void)wVersionNumber;
    (void)lpwVID;

    // _asm    int 3                    // remove when not debugging &&&

    // 0x0a is offset from beginning of MBR to start searching
 
    if ( 0xffff != ScanString(lpbyBootBuffer + 0x0a,
                       500,             // sizeof lpbyPartBuffer - 0x0a - 2
                       bSearch,         // that for which we search
                       sizeof(bSearch)))
    {
        wReturn = EXTSTATUS_VIRUS_FOUND;
        *lpwVID = VID_PREDT_BOOT;
    }
 
    return (wReturn);
}   // End of EXTScanBootPredt

// EXTScanBootDanish        boot detection for Danish Boot
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanBootDanish (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{
    return(EXTScanDanish(lpCallBack, lpbyBootBuffer, lpwVID));
}   // End of EXTScanBootDanish


// EXTScanBootMoloch
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//

WORD EXTScanBootMoloch (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)


{
    WORD    wReturn = EXTSTATUS_OK;
    BYTE    i;
    BYTE    byEP;                   // offset to boot-program EP
    WORD    wCyl = 0x4F;            // should be a static value
    BYTE    byHead = 0x01,          // value always 1
            bySec = 0x00;           // value variable based on diskette type
//    BYTE    buf[0x200];
    BYTE    bSearch1[] = {0x4F};
    BYTE    bSearch2[] = {0xFB, 0xFC, 0x51, 0xB9, 0x6D, 0x04, 0x2E, 0x80, 0x34};

//    lpbyWorkBuffer = buf;

    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    if (lpbyBootBuffer[0x00] != 0xEB)
        return(wReturn);

    byEP = lpbyBootBuffer[0x01];

    //_asm        int 3

    for (i = 0; i < 30; i++)
        if (lpbyBootBuffer[0x00 + byEP + i] == 0x4F)
        {
            bySec = lpbyBootBuffer[0x18] - 3;   // assign bySec value based on BPB info
                                                // necessary to detect virus on 720K
                                                // or 1.4 Mb diskettes

            if (lpCallBack->SDiskReadPhysical( (LPSTR)lpbyWorkBuffer,   // buffer
                                                (BYTE)1,                // count
                                                wCyl,                   // cyl
                                                bySec,                  // sector
                                                byHead,                 // head
                                                byDrive))               // drive
            {
                return (wReturn);
            }

            if ( 0xffff != ScanString(lpbyWorkBuffer + 0x09,
                               501,             // sizeof lpbyPartBuffer - 0x09 - 2
                               bSearch2,         // that for which we search
                               sizeof(bSearch2)))
            {
               wReturn = EXTSTATUS_VIRUS_FOUND;
               *lpwVID = VID_MOLOCH;
            }
        }

return(wReturn);
}


// EXTScanBootWolleh
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//

WORD EXTScanBootWolleh (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)


{
    WORD    wReturn = EXTSTATUS_OK;
    BYTE    byEP;
    BYTE    bSearch1[] = {0xb8, 0x00, 0x9f, 0x8e, 0xc0, 0xb4, 0x02, 0xb0, 0x04, 0xbb, 0x00, 0x01, 0xb5};
    BYTE    bSearch2[] = {0xcd, 0x13, 0xea, 0x00, 0x01, 0x00, 0x9f};


    if (lpbyBootBuffer[0x00] != 0xEB)
        return(wReturn);

    byEP = lpbyBootBuffer[0x01];


    if ( 0xffff == ScanString(lpbyBootBuffer + byEP + 2,
                              512 - (byEP + 2), // sizeof lpbyPartBuffer - byEP - 2
                              bSearch1,         // that for which we search
                              sizeof(bSearch1)))
    {
        return(wReturn);
    }

    if ( 0xffff != ScanString(lpbyBootBuffer + byEP + 22,
                              512 - (byEP + 22), // sizeof lpbyPartBuffer - byEP - 2
                              bSearch2,         // that for which we search
                              sizeof(bSearch2)))
    {
         wReturn = EXTSTATUS_VIRUS_FOUND;
         *lpwVID = VID_WOLLEH;
    }


    return(wReturn);
}


// EXTScanBootChan
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//

#define CHANSRCHOFF     0x3e        // Offset into floppy boot to begin search
#define CHANSRCHLEN     (0x200 - CHANSRCHOFF)

#define CHANSRCHDELTA   0x0f        // Difference between search1 and search2

#define CHANSRCHOFF2    (CHANSRCHOFF + CHANSRCHDELTA)
#define CHANSRCHLEN2    (0x200 - CHANSRCHOFF2)

WORD EXTScanBootChan (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)


{
    WORD    wReturn = EXTSTATUS_OK;
    //The infection looks Like this:
    // 0x16, 0x07, 0xBB, 0x78, 0x06, 0x8D, 0x87, 0x8A, 0xFB, 0xBA
    // HD SD 0xB9 SS CL where HD is Drive Number (0x80 usually)
    //                        SD is Side
    //                        SS is Sector
    //                        CL is Cylinder
    //                  of location of the 2 viral sectors in that HD.
    // 0x16, 0xCD, 0x13, 0x72, 0xE4, 0x53, 0xCB, 0xAF, 0x29
    BYTE    bSearch1[] = {0x16, 0x07, 0xBB, 0x78, 0x06, 0x8D, 0x87, 0x8A, 0xFB, 0xBA};
    BYTE    bSearch2[] = {0x16, 0xCD, 0x13, 0x72, 0xE4, 0x53, 0xCB};

    //Search for the first string at offset CHANSRCHOFF of the boot sector
    // CHANSRCHOFF is defined at the top of the function    jjw 11 Mar 98
    if ( 0xffff != ScanString(lpbyBootBuffer + CHANSRCHOFF,
                       CHANSRCHLEN,
                       bSearch1,
                       sizeof(bSearch1)))
    {
        //Found the first string, skip the Location Info (Skip 6 basically),
        //search for the 2nd one at offset CHANSRCHOFF2 of the boot sector
        if ( 0xffff != ScanString(lpbyBootBuffer + CHANSRCHOFF2,
                           CHANSRCHLEN2,
                           bSearch2,        //
                           sizeof(bSearch2)))
        {
           wReturn = EXTSTATUS_VIRUS_FOUND;
           *lpwVID = VID_CHAN;
        }
    }

return(wReturn);
}

// EXTScanBootLilith
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//

WORD EXTScanBootLilith (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)


{
    WORD    wReturn = EXTSTATUS_OK;
    WORD    wCyl = 0x4F;            // should be a static value
    BYTE    byHead = 0x01,          // value always 1
            bySec = 0x0F;           // sector
//    BYTE    buf[0x200];
    BYTE    bSearch[] = {0xcd, 0x13, 0x5a, 0x80, 0x3e, 0xef, 0x04, 0x4c,
                         0x75, 0x03, 0xe9, 0x82, 0x00, 0xc6, 0x06, 0xef,
                         0x04, 0x4c};
    BYTE    byValidSet[36] = {0x00, 0x01, 0x02, 0x03, 0x07, 0x0d, 0x0f, 0x13,
                              0x16, 0x2b, 0x2e, 0x33, 0x4f, 0x7c, 0x7e, 0x80, 0x8e,
                              0x9c, 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
                              0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xc0, 0xcd,
                              0xd0, 0xfa, 0xff };
    BYTE    by;
    BYTE    byValidSetIndex;
    BYTE    byValidSetStatus;


    //////////////////////////////////////////////////////////////////
    // This virus is polymorphic in the bs.  It sets up the registers in
    // the bs and calls INT13 read of cyl/sec 4f/0x0F.  The code below
    // checks a length of bytes and verifies the bytes only equal the
    // bytes in the validset of bytes
 
    // first look for a eb/e9 4c 00

    if ((!(lpbyBootBuffer[0] == 0xEB ||
           lpbyBootBuffer[0] == 0xE9)) ||
        lpbyBootBuffer[1] != 0x4C ||
        lpbyBootBuffer[2] != 0x00)
    {
        return(wReturn);
    }


    for(by=0;by<24;by++)
    {
        byValidSetStatus = 0;
        for (byValidSetIndex=0;byValidSetIndex<36;byValidSetIndex++)
        {
            if ((lpbyBootBuffer[79 + by]) == byValidSet[byValidSetIndex])
            {
                byValidSetStatus = 1;
                break;
            }
        }
        if (byValidSetStatus == 0)
        {
            return(wReturn);
        }
    }

    //////////////////////////////////////////////////////////////////
    // The virus appears to be in the bs so, we now load the viral
    // sector and check for our scan string

//    lpbyWorkBuffer = buf;

    if (lpCallBack->SDiskReadPhysical((LPSTR)lpbyWorkBuffer,   // buffer
                                      (BYTE)1,                // count
                                       wCyl,                   // cyl
                                       bySec,                  // sector
                                       byHead,                 // head
                                       byDrive))               // drive
    {
        return (wReturn);
    }

    if ( 0xffff != ScanString(lpbyWorkBuffer,
                              0x200,
                              bSearch,
                              sizeof(bSearch)))
    {
        wReturn = EXTSTATUS_VIRUS_FOUND;
        *lpwVID = VID_LILITH;
    }

    return(wReturn);
} // EXTScanBootLilith


// EXTScanBootHare    MBR detection for Hare.7610 Hare.7750 Hare.7786
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//

WORD EXTScanBootHare (LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)


{
    WORD 	wInfect = 0xCCFF;
    WORD 	wReturn = EXTSTATUS_OK;
    WORD    i,j,k;
	BYTE	byVOffset;
    BOOL    bVirusIndex = FALSE,
			bVirusKey   = FALSE,
			bDecrypt	= FALSE;

	// Table of Register encodings for encryption

	BYTE	byRegEncode[] = { 0x07, 0x27, 0x0f, 0x2f, 0x17,
                              0x37, 0x04, 0x24, 0x0c, 0x2c,
                              0x14, 0x34, 0x05, 0x25, 0x0d,
                              0x2d, 0x15, 0x3d, 0x1d, 0x3c,
                              0x1c, 0x35 };


    //_asm    int 3


    // This is the viruses own self check.  If the word at offset 0x100
    // into the MBR/BS minus the word at offset 0x102 is equal to 0xccff
    // then it will not infect.  It needs to do this because the virus
    // is polymorphic in the MBR/BS and it cannot easily decrypt and
    // check that way.

    if(wInfect != (WORD) (*((LPWORD) (lpbyBootBuffer + 0x102)) -
                          *((LPWORD) (lpbyBootBuffer + 0x100))))
	{
		return(wReturn);
	}


	// Get the offset of the starting jump to the decryptor

	byVOffset = lpbyBootBuffer[1] + 2;


    for (i = byVOffset + 8; i < byVOffset + 15 ; i++)
    {

        // Check for index initialization.

        if (lpbyBootBuffer[i] == 0xbb ||
            lpbyBootBuffer[i] == 0xbe ||
            lpbyBootBuffer[i] == 0xbf)

		{
			if (*(LPWORD) (lpbyBootBuffer + i + 1) > (0x7c14 + byVOffset) &&
				*(LPWORD) (lpbyBootBuffer + i + 1) < (0x7c1c + byVOffset) )
			{
	            bVirusIndex = TRUE;
				break;
			}
        }
    }


	// If we didn't find a mov into an index register then there's
    // no virus here.

	if (!bVirusIndex)
		return (wReturn);


	for (i = byVOffset + 8 ; i < byVOffset + 15 ; i++)
	{
		// Check for key initialization

		if ((lpbyBootBuffer[i] & 0xf8) == 0xb0 )
		{
			bVirusKey = TRUE;
			break;
		}
	}

	
	if (!bVirusKey)
		return (wReturn);



	for (j = i ; i < j + 10; i ++)
    {
		if (lpbyBootBuffer[i] == 0x00 ||
			lpbyBootBuffer[i] == 0x28 ||
			lpbyBootBuffer[i] == 0x30  )
		{
			for (k = 0; k < 22 ; k++)
			{
				if (lpbyBootBuffer[i+1] == byRegEncode[k])
				{
					bDecrypt = TRUE;
					break;
				}
			}
		}

		if (bDecrypt)
			break;
	}


	if (!bDecrypt)
		return (wReturn);

	
	wReturn = EXTSTATUS_VIRUS_FOUND;
	*lpwVID = VID_HARE_BOOT;


    return(wReturn);
}

// EXTScanBootOneHalf       boot detection for OneHalf boot
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanBootOneHalf(LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{
    return(EXTScanOneHalf(lpCallBack, lpbyBootBuffer, lpwVID));
}   // End of EXTScanBootOneHalf


// EXTScanBootBloodhound       boot detection for Bloodhound boot
// arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : physical drive # containing the boot sector
//                        (0=A, 1=B, 0x80=C). code will scan *all* boot
//                        sectors on the drive.
// byPartitionNum       : if != FFh, its the partition # of the logical drive
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyBootBuffer       : contains image of appropriate boot sector
//                        to scan (ignore byDrive, byBootSecNum)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV which is calling EXTERNAL CODE
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found

WORD EXTScanBootBloodhound(LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         BYTE           byPartitionNum,
                                         WORD           wSector,
                                         WORD           wCylinder,
                                         WORD           wHead,
                                         LPBYTE         lpbyBootBuffer,
                                         LPBYTE         lpbyWorkBuffer,
                                         WORD           wVersionNumber,
                                         LPWORD         lpwVID)
{
    WORD                                wReturn;
    BOOL                                bExcluded = FALSE;

    // _asm int 3h

#if !defined(NAVEX15)
    if (wVersionNumber < 3)
    {
        *lpwVID = 0;
        return(EXTSTATUS_OK);
    }
#endif

    if (BloodhoundFindBootVirus(lpCallBack,lpbyBootBuffer,&bExcluded) == TRUE)
    {
        *lpwVID = VID_BLOODHOUND_BOOT;
        wReturn = EXTSTATUS_VIRUS_FOUND;
    }
    else
    {
        *lpwVID = 0;
        wReturn = EXTSTATUS_OK;
    }

    // since the bloodhound scanner is always called last in NAVEX just
    // before NAVENG is called, we can fill the work buffer with a request
    // to disable the IBM scanner if we find an excluded boot record...

    if (bExcluded == TRUE)
    {
        lpbyWorkBuffer[0] = 'I' ^ 'B' ^ 'M';
    }
    else
    {
        lpbyWorkBuffer[0] = 0;
    }

    return(wReturn);

}   // End of EXTScanBootBloodhound


// DANISH.CPP contains EXTScanDanish() and EXTScanOneHalf, which are also
// called for MBR detection.  The prototypes for these are in NAVEXSHR.H.

#if defined(SYM_DOS)    // DANISH.CPP is also included in NAVEXSP.CPP and
                        // only needs to be included again in DOS because
#include "danish.cpp"   // partition and boot scanning are in separate
                        // files in DOS NAVEX.

MODULE_EXPORT_TABLE_TYPE module_export_table =
{
  EXPORT_STRING,
  (void (*)())EXTScanBoot
};

void main(void)
{
}

#endif

#endif  // #ifndef SYM_NLM

