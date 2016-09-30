//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/NAVEXSB.CPv   1.3   20 Nov 1998 17:02:04   rpulint  $
//
// Description:
//      Contains NAVEX EXTScanBoot code.
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/NAVEXSB.CPv  $
// 
//    Rev 1.3   20 Nov 1998 17:02:04   rpulint
// WATCOM Changes: moved extern declaration to theglobal section from
// within functions
// 
//    Rev 1.2   05 Aug 1998 19:03:16   hjaland
// Carey added an if statement to prevent scan of IBMStringScanBoot when navex 
// sends a disable byte of 'I'^'B'^'M'.
// 
//    Rev 1.1   05 Jun 1998 15:16:44   CNACHEN
// Modified to perform IBM string scan.
// 
//    Rev 1.0   15 May 1997 15:00:58   jsulton
// Initial revision.
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

// Declare shared routines

#include "navexshr.h"

#include "strscan.h"

#ifdef __WATCOMC__
    extern SEARCH_INFO_T       gstIBMFile;
    extern SEARCH_INFO_T       gstIBMBoot;
#endif

#ifdef SYM_DOSX
#undef SYM_DOS
#endif

#if defined(SYM_DOS)

WORD wDS, wES;                      // save loader's DS,ES

#endif

// Prototypes for local Boot scanning functions

// Def Writer: Your prototype here.


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
#ifndef __WATCOMC__
    extern SEARCH_INFO_T       gstIBMFile;
    extern SEARCH_INFO_T       gstIBMBoot;
#endif

    (void)byDrive;
    (void)byPartitionNum;
    (void)wSector;
    (void)wCylinder;
    (void)wHead;
    (void)wVersionNumber;

    if (lpbyWorkBuffer[0] != (BYTE)('I' ^ 'B' ^ 'M'))
    {
        // only scan if we didn't get a disablement from NAVEX

        if (IBMStringScanBoot(lpCallBack,
                              &gstIBMBoot,
                              &gstIBMFile,
                              lpbyBootBuffer) == STRING_SCAN_FOUND_STRING)
        {
            *lpwVID = IBM_STRING_BOOT_VID;

            return(EXTSTATUS_VIRUS_FOUND);
        }
    }

    return(EXTSTATUS_OK);
    
}
#endif  // #ifndef SYM_NLM

