//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/navexrp.cpv   1.3   02 Sep 1997 14:34:46   CNACHEN  $
//
// Description:
//      Contains NAVEX EXTRepairPart code.
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/navexrp.cpv  $
// 
//    Rev 1.3   02 Sep 1997 14:34:46   CNACHEN
// 
//    Rev 1.2   02 Sep 1997 14:30:08   CNACHEN
// Fixed mismatched #ifdef / #endif
// 
//    Rev 1.1   04 Jun 1997 11:40:06   CNACHEN
// Fixed prototypes for EXTRepair* functions.
// 
//    Rev 1.0   15 May 1997 14:59:54   jsulton
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

#ifdef SYM_DOSX
#undef SYM_DOS
#endif

#if defined(SYM_DOS)

WORD wDS, wES;                      // save loader's DS,ES

#endif

// *********************
//
// Navex native function
//
// *********************


// EXTRepairPart arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// wVersionNumber       : NAV version number.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the partition sector
//                        (0x80=first physical HD
// lpbyWorkBuffer       : NULL pointer in version 1
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

EXTSTATUS FAR WINAPI NLOADDS EXTRepairPart(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           BYTE           byDrive,
                                           LPBYTE         lpbyWorkBuffer,
                                           LPVOID         lpvCookie)
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
    WORD    wReturn = EXTSTATUS_NO_REPAIR;

    (void)lpCallBack;
    (void)wVersionNumber;
    (void)lpsN30;
    (void)byDrive;
    (void)lpbyWorkBuffer;

    return(EXTSTATUS_OK);
    
}

#endif  // #ifndef SYM_NLM
