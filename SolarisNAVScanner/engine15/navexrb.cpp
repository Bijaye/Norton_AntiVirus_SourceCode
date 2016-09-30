//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/navexrb.cpv   1.1   04 Jun 1997 12:01:50   CNACHEN  $
//
// Description:
//      Contains NAVEX EXTRepairBoot code.
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/navexrb.cpv  $
// 
//    Rev 1.1   04 Jun 1997 12:01:50   CNACHEN
// Fixed prototypes for EXTRepair* functions.
// 
//    Rev 1.0   15 May 1997 14:59:50   jsulton
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

// Prototypes for local boot repair functions
// *************************
//
// Native Navex function
//
// *************************


// EXTRepairBoot arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// wVersionNumber       : NAV version number.
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// byDrive              : physical drive # containing the boot sec 0=A, 0x80=C
// byPartitionNum       : if != FFh, this is the partition # of the boot sector
// wSector, wCylinder,
// wHead                : location of boot sector (not used currently)
// lpbyWorkBuffer       : 2-4K buffer for temporary operations.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK                  if OK
//                        EXTSTATUS_DISK_READ_ERROR     if disk read error
//                        EXTSTATUS_DISK_WRITE_ERROR    if disk write error
//                        EXTSTATUS_MEM_ERROR           if memory error
//                        EXTSTATUS_NO_REPAIR           if unable to repair
//

EXTSTATUS FAR WINAPI NLOADDS EXTRepairBoot(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           BYTE           byDrive,
                                           BYTE           byPartitionNum,
                                           WORD           wSector,
                                           WORD           wCylinder,
                                           WORD           wHead,
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
    (void)byPartitionNum;
    (void)wSector;
    (void)wCylinder;
    (void)wHead;
    (void)lpbyWorkBuffer;

    return(EXTSTATUS_OK);
    

                                        // REP_EXEC_CODE defined in repair.inc
                                        // Check if external is called.
    if (lpsN30->wHardType == REP_EXEC_CODE)
    {
                                        // wHostJmp field contains ID of
                                        // repair to perform.  This kludgy
                                        // shared purpose goes way back.

//        switch (lpsN30->wHardLoc1)
//        {
            // defwriter: your repairs here.
//        }

    }



#if defined(SYM_DOS)

    _asm
        {
        mov ds, cs:[wDS]
        mov es, cs:[wES]
        }



#endif

    return (wReturn);
}


#if defined(SYM_DOS)

MODULE_EXPORT_TABLE_TYPE module_export_table =
{
  EXPORT_STRING,
  (void (*)())EXTRepairBoot
};

void main(void)
{
}

#endif

#endif  // #ifndef SYM_NLM
