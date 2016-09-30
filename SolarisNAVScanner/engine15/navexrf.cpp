//************************************************************************
//
// $Header:   S:/ENGINE15/VCS/navexrf.cpv   1.2   02 Sep 1997 14:44:24   CNACHEN  $
//
// Description:
//      Contains NAVEX EXTRepairFile code.
//
//************************************************************************
// $Log:   S:/ENGINE15/VCS/navexrf.cpv  $
// 
//    Rev 1.2   02 Sep 1997 14:44:24   CNACHEN
// Fixed for NAVEX.NLM
// 
//    Rev 1.1   04 Jun 1997 12:01:48   CNACHEN
// Fixed prototypes for EXTRepair* functions.
// 
//    Rev 1.0   15 May 1997 14:59:14   jsulton
// Initial revision.
//************************************************************************

#include "platform.h"

#include "endutils.h"

#ifdef WIN16        // VIRSCAN QUAKE 2
#include "cts4.h"
#else
//#include "ctsn.h"	// NEW_UNIX
#include "n30type.h"	// NEX_UNIX
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

//#include "macrorep.h"

//#include "neperep.h"

//#include "excelrep.h"

//#include "mvp.h"

// ******************************
//
// Prototypes for local functions
//
// ******************************


// ***************************************************************************
//
// RepairJumpTable[]
//
//      Jump Table to Local File repair functions.
//      Used by EXTRepairFile to call individual repairs.
//      Note: 0x00 is an illegal value for either hi or lo byte of function id.
//            This is for backwards compatibility in the virscan.dat w/ 3.0.7.
//
//      Defwriter, add your function id and function pointer here.
//
// ***************************************************************************

/*
struct tagJumpTableEntry
{
    WORD    wID;
    WORD    (*fnRepairFunction) (LPCALLBACKREV1 lpCallBack,
                                 LPN30           lpsN30,
                                 LPTSTR          lpszFileName,
                                 LPBYTE         WorkBuf);
} RepairJumpTable[] =
    {


//        {ID_WEED5850,       EXTRepairFileWeed5850},

    };
*/

// ***************************************************************************
//
// Native NAVEX function
//
// ***************************************************************************


// EXTRepairFile arguments:
//
// lpCallBack           : pointer to callback structure for FileOpen, etc.
// wVersionNumber       : version of NAV calling external code
// lpsN30               : pointer to N30 structure containing VIRSCAN.DAT
//                        information and virus stats
// lpszFileName         : name of the file to repair
// lpbyWorkBuffer       : temporary 2-4K work buffer.
//
// Returns:
//
//     EXTSTATUS        : EXTSTATUS_OK          if OK
//                        EXTSTATUS_FILE_ERROR  if file error
//                        EXTSTATUS_MEM_ERROR   if memory error
//                        EXTSTATUS_NO_REPAIR   if unable to repair

EXTSTATUS FAR WINAPI NLOADDS EXTRepairFile(LPCALLBACKREV1 lpCallBack,
                                           WORD           wVersionNumber,
                                           LPN30          lpsN30,
                                           LPTSTR         lpszFileName,
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

    WORD        wReturn = EXTSTATUS_NO_REPAIR;
    WORD        i = 0;


    (void)lpCallBack;
    (void)wVersionNumber;
    (void)lpsN30;
    (void)lpszFileName;
    (void)lpbyWorkBuffer;

    return(EXTSTATUS_OK);
}
