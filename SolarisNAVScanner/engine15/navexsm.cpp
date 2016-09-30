//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/NAVEXSM.CPv   1.0   15 May 1997 15:02:20   jsulton  $
//
// Description:
//      Contains NAVEX EXTScanMemory code.
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/NAVEXSM.CPv  $
// 
//    Rev 1.0   15 May 1997 15:02:20   jsulton
// Initial revision.
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

