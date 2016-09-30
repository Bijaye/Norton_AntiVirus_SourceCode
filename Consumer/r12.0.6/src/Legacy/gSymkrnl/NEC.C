/*--------------------------------------------------------------------------*/
/* $Header:   S:/SYMKRNL/VCS/NEC.C_v   1.0   26 Jan 1996 20:21:18   JREARDON  
/*                                                                          */
/* NEC.C							                                        */
/*                                                                          */
/*	Contains routines specific to the NEC 9800 computer, including          */
/*      detection.                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* $Log:   S:/SYMKRNL/VCS/nec.c_v  $*/
// 
//    Rev 1.5   18 Feb 1998 10:17:54   MDUNN
// Ack... I was missing a semicolon.
// 
//    Rev 1.4   18 Feb 1998 10:02:20   mdunn
// Ported ROSWELL and ROSNEC changes from the K branch.
// 
//    Rev 1.3   19 Mar 1997 21:40:40   BILL
// Modified files to build properly under Alpha platform
// 
//    Rev 1.2   14 Feb 1997 14:50:58   BMCCORK
// Fixed syntax errors introduced by mkeatin
// 
//    Rev 1.1   13 Feb 1997 13:52:22   MKEATIN
// Ported changes from branch F
// 
//    Rev 1.0   26 Jan 1996 20:21:18   JREARDON
// Initial revision.
// 
//    Rev 1.5   10 Oct 1995 21:01:48   SRYAN
// 
// Added _HWIsNEC() for DOS detectiond of NEC hardware.
// 
//    Rev 1.4   27 Apr 1995 11:37:12   MARKK
// Just return false if not testing
// 
//    Rev 1.3   19 Oct 1994 17:50:50   JMILLARD
// clean up warning for non-SYM_WIN platform
// 
//    Rev 1.2   12 Oct 1994 17:39:20   BRAD
// Added actual code for NEC detection
// 
//    Rev 1.1   10 Oct 1994 08:29:20   JMILLARD
// fix nested comment in header
// 
//    Rev 1.0   04 Oct 1994 14:39:02   BRAD
// Initial revision.
/*--------------------------------------------------------------------------*/

#include "platform.h"
#include "xapi.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#if defined(SYM_WIN)
BOOL SYM_EXPORT WINAPI HWIsNEC (VOID)
{
#ifdef _M_ALPHA // WES No (known) NEC Alpha machines
	return FALSE;
#else	
    static      BOOL bIsNEC = FALSE;
    static      BOOL bTested = FALSE;

    if (!bTested)
        {
        UINT    uType;


        bTested = TRUE;
                                        // Get main KB type
        uType = GetKeyboardType(0);
        if (uType == 7)                 // '7' is japanese keyboard type
            {
                                        // Get the sub-type.
            uType = GetKeyboardType(1);
            if (uType >= 0x0D01)
                bIsNEC = TRUE;
            }

        }
    return(bIsNEC);
#endif    
}

#elif defined ( SYM_DOS )

#pragma check_stack (off)               // Stack checking leaves some
                                        // registers dirty - bad for
BOOL SYM_EXPORT WINAPI HWIsNEC (VOID)   // all those assembly calls
{
    return (_HWIsNEC());
}

#pragma check_stack (on)

#elif defined(SYM_VXD)

BOOL SYM_EXPORT WINAPI HWIsNEC()
{
    return bHWIsNEC;
}

#else

BOOL SYM_EXPORT WINAPI HWIsNEC()
{
    return FALSE;
}

#endif
