//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/NAVEXSP.CPv   1.3   20 Nov 1998 17:01:32   rpulint  $
//
// Description:
//      Contains NAVEX EXTScanPart code.
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/NAVEXSP.CPv  $
// 
//    Rev 1.3   20 Nov 1998 17:01:32   rpulint
// WATCOM Changes: moved extern declaration to theglobal section from
// within functions
// 
//    Rev 1.2   05 Aug 1998 19:19:12   hjaland
// Carey added an if statement to prevent scan of IBMStringScanBoot when navex
// sends a disable byte of 'I'^'B'^'M'.
// 
//    Rev 1.1   05 Jun 1998 15:16:58   CNACHEN
// Modified to perform IBM string scan.
// 
//    Rev 1.0   15 May 1997 15:01:00   jsulton
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

#ifdef SYM_DOSX
#undef SYM_DOS
#endif

#ifdef  __WATCOMC__
    extern SEARCH_INFO_T       gstIBMFile;
    extern SEARCH_INFO_T       gstIBMBoot;
#endif

#if defined(SYM_DOS)

WORD wDS, wES;                      // save loader's DS,ES

#endif

// Prototypes for local MBR scanning functions

// Def Writer: Your prototype here.

// Jump Table used by EXTScanPart

WORD    (*SPJumpTable[]) (LPCALLBACKREV1 lpCallBack,
              BYTE           byDrive,
              LPBYTE         lpbyPartBuffer,
              LPBYTE         lpbyWorkBuffer,
              WORD           wVersionNumber,
              LPWORD         lpwVID) = {

    // Def Writer: Insert your function name here.
    NULL
};                              // wID = 0 => end of table.

// EXTScanPart arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// byDrive              : drive # containing the boot sector (00=A, 0x80=first
//                        physical HD
// lpbyPartBuffer       : contains image of appropriate part sector
//                        to scan (ignore byDrive)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
// wVersionNumber       : version number of NAV calling external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_READ_ERROR          if disk error occured
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//                        EXTSTATUS_MEM_ERROR           if memory alloc. error
//


EXTSTATUS FAR WINAPI NLOADDS EXTScanPart(LPCALLBACKREV1 lpCallBack,
                                         BYTE           byDrive,
                                         LPBYTE         lpbyPartBuffer,
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

    (void)byDrive;
    (void)lpbyWorkBuffer;
    (void)wVersionNumber;
#ifndef  __WATCOMC__
    extern SEARCH_INFO_T       gstIBMFile;
    extern SEARCH_INFO_T       gstIBMBoot;
#endif

    if (lpbyWorkBuffer[0] != (BYTE)('I' ^ 'B' ^ 'M'))
    {
        // only scan if we didn't get a disablement from NAVEX

        if (IBMStringScanBoot(lpCallBack,
                              &gstIBMBoot,
                              &gstIBMFile,
                              lpbyPartBuffer) == STRING_SCAN_FOUND_STRING)
        {
            *lpwVID = IBM_STRING_BOOT_VID;

            return(EXTSTATUS_VIRUS_FOUND);
        }
    }

    return(EXTSTATUS_OK);

    return(EXTSTATUS_OK);
}

#endif  // #ifndef SYM_NLM
