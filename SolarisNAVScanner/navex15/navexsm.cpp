//************************************************************************
//
// $Header:   S:/NAVEX/VCS/navexsm.cpv   1.8   26 Dec 1996 15:21:58   AOONWAL  $
//
// Description:
//      Contains NAVEX EXTScanMemory code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/navexsm.cpv  $
// 
//    Rev 1.8   26 Dec 1996 15:21:58   AOONWAL
// No change.
// 
//    Rev 1.7   02 Dec 1996 14:00:20   AOONWAL
// No change.
// 
//    Rev 1.6   29 Oct 1996 12:59:14   AOONWAL
// No change.
// 
//    Rev 1.5   13 Feb 1996 17:56:20   MKEATIN
// Added DX Support
// 
//    Rev 1.4   01 Nov 1995 10:49:08   DCHI
// Removed include of navexhdr.h due to change in format of navexshr.h.
// 
//    Rev 1.3   19 Oct 1995 14:20:58   DCHI
// Added LOADDS to external functions for WIN16.  Modified DOS export
// table structure to support tiny model modules.
// 
// 
//    Rev 1.2   18 Oct 1995 11:51:12   CNACHEN
// 
//    Rev 1.1   16 Oct 1995 13:03:08   DCHI
// Added include of navexhdr.h.
// 
//    Rev 1.0   13 Oct 1995 13:06:10   DCHI
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

// Declare shared routines

#include "navexshr.h"

#ifdef SYM_DOSX
#undef SYM_DOS
#endif

#if defined(SYM_DOS)

WORD wDS, wES;                      // save loader's DS,ES

#endif


// EXTScanMemory arguments:
//
// lpCallBack           : pointer to callback structure
// wVersionNumber       : NAV version which is using external code
// lpwVID               : virus ID # of the virus if found
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if no virus found
//                        EXTSTATUS_VIRUS_FOUND         if virus was found
//                        EXTSTATUS_MEM_ERROR           if memory error
//

EXTSTATUS FAR WINAPI NLOADDS EXTScanMemory(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           BOOL           bScanHighMemory,
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

    (void)lpCallBack;
    (void)wVersionNumber;
    (void)bScanHighMemory;
    (void)lpwVID;

    // insert code here


#if defined(SYM_DOS)

    _asm
        {
        mov ds, cs:[wDS]
        mov es, cs:[wES]
        }

#endif


    return ( EXTSTATUS_OK );
}




#if defined(SYM_DOS)

MODULE_EXPORT_TABLE_TYPE module_export_table =
{
  EXPORT_STRING,
  (void (*)())EXTScanMemory
};

void main(void)
{
}

#endif

