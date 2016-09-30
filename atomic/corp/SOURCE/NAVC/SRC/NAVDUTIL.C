// Copyright 1992-1996 Symantec, Peter Norton Product Group
//***********************************************************************
//
// $Header:   S:/NAVC/VCS/navdutil.c_v   1.4   12 Jun 1997 12:03:32   BGERHAR  $
//
// Description:
//  This module has misc routines needed by NAV for DOS.
//
// See Also:
//***********************************************************************
// $Log:   S:/NAVC/VCS/navdutil.c_v  $
// 
//    Rev 1.4   12 Jun 1997 12:03:32   BGERHAR
// Fix extern declaration problem
// 
//    Rev 1.3   08 Jun 1997 23:24:22   BGERHAR
// Don't wrap for deep scan or def directories
// 
//    Rev 1.2   01 Jun 1997 20:08:52   BGERHAR
// Includ stdio.h to fix printf warning
// 
//    Rev 1.1   26 May 1997 16:15:16   BGERHAR
// Fix warnings during compile
// 
//    Rev 1.0   06 Feb 1997 20:56:16   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:19:44   MKEATIN
// Initial revision.
// 
//    Rev 1.4   14 Nov 1996 12:54:16   JBELDEN
// removed unused code and cleaned it up a bit.
// 
//    Rev 1.3   08 Nov 1996 12:07:50   JBELDEN
// stubbed a lot of routines that won't need to do anything.
// 
//    Rev 1.2   16 Oct 1996 12:52:46   JBELDEN
// stubbed out the routines related to printing and activity log.
// 
//    Rev 1.1   03 Oct 1996 14:51:00   JBELDEN
// changed selfcheck to use printf()
// for TTYSCAN
// 
//    Rev 1.0   02 Oct 1996 12:30:48   JBELDEN
// Initial revision.
//***********************************************************************

#include "platform.h"
#include <stdio.h>                      // printf()
#include "xapi.h"
#include "symcfg.h"
#include "stddos.h"
#include "stddlg.h"
#include "file.h"

#include "ctsn.h"

#include "navutil.h"
#include "syminteg.h"
#include "nonav.h"
#include "defs.h"
#include "navdprot.h"

MODULE_NAME;


//************************************************************************
// GLOBAL VARIABLES
//************************************************************************

STATIC BOOL    bFirstTime;

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 02/20/93 BRAD Function Created.                                      *
 ************************************************************************/

BYTE PASCAL TunnelSelectFile (TERec *te, char *szSpec, char *szTitle)
{
    return ( 0 );
}


/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 02/20/93 BRAD Function Created.                                      *
 ************************************************************************/

BYTE PASCAL SelectFile (char *szFilename, char *szSpec, char *szTitle)
{
   return(0);
}


//************************************************************************
// UpdateCheckBox()
//
// This routine changes the value of a checkbox and redraws it.
//
// Parameters:
//      CheckBoxRec *pcbCheckBox        CheckBoxRec to modify
//      BOOL        bEnabled            TRUE - check the box
//                                       FALSE - clear the box
//
// Returns:
//      nothing
//************************************************************************
// 2/10/93 BRAD Function created.
//************************************************************************

VOID PASCAL UpdateCheckBox (CheckBoxRec *pcbCheckBox, BOOL bEnabled)
{
   return;
} // End UpdateCheckBox()


//************************************************************************
// NAVDLoadTextEdit()
//
// This routine copies a up to lpText->max_len characters into
// lpText->string.  lpText->string must be able to hold at least
// lpText->max_len + 1 bytes.  An EOS is appended to the string.
//
// Parameters:
//      TERec   *lpText
//      LPCSTR   szSource
//
// Returns:
//      lpText->string                  Pointer to destination string
//      NULL                            Failed - attempted NULL ptr assign
//************************************************************************
// 3/12/93 DALLEE Function created.
//************************************************************************

LPSTR PASCAL NAVDLoadTextEdit (TERec *lpText, LPCSTR szSource)
{
    return (NULL);
} // End NAVDLoadTextEdit()


//************************************************************************
// NavSelfCheck()
//
// This routine checks the integrity of NAV.EXE by checking the
// self-check CRC stored in the .EXE header.
//
// Parameters:
//      None
//
// Returns:
//      SELF_TEST_SUCCESS (0)           Program passed self-check
//      ERR (-1)                        Memory allocation error
//      SELF_TEST_FAIL (2)              FAIL: bad CRC
//      SELF_TEST_FILE_ERROR (3)        File error
//************************************************************************
// 6/10/93 DALLEE, Function created.
// 6/15/93 DALLEE, Use NavSelfCheck().
//************************************************************************

WORD PASCAL NavSelfCheck (BOOL bHalt)
{
    extern  char    lpszNavAltered [];
    extern  DIALOG  dlgNavAlteredHalt;
    auto    WORD    wResult;

    wResult = NavSelfTest(NULL);

    if (wResult != SELF_TEST_SUCCESS)
        {
        if ( !bHalt )
            printf(lpszNavAltered);
        else
            {
                                        //&? 07/15/95 Barry:
                                        // The shutdown code was copied
                                        // from SCAND/SCANUTIL.C.
                                        // I didn't make it a common
                                        // function because I didn't
                                        // want to risk breaking the
                                        // overlays or anything else
                                        // this close to ship. TBD.

            // Shutdown stuff and close files

            MouseHC ( ) ;               // Hide mouse
                                        // Don't let critical errors stop us
            DiskErrorOn((ERRORTRAPPROC)-1);
            VMMTerminate();             // Remove .SWP file (do last!)

            DiskDisableCache();         // Flush and turn off cache

            _asm
                {
                mov     ah, 0Dh         ; Flush buffers before we halt
                int     21h

                cli
            J_0:
                jmp     J_0
                }
            }
        }

    return (wResult);

} // End NavSelfCheck()


//************************************************************************
// NAVDWriteLog()
//
// Open the write log fillbar dialog then call WriteAllFileLogRcords().
//
// Parameters:
//      LPVOID lpNoNav                  This scan's information.
//
// Returns:
//      NOERR                           Successfully wrote log.
//      ALOGERR_MEMORY                  Out of memory.
//      ALOGERR_OPEN_FILE               Could not open log file.
//************************************************************************
// 8/11/93 DALLEE, Function created.
//************************************************************************

UINT PASCAL NAVDWriteLog (LPVOID lpNoNav)
{
    return(0);
} // End NAVDWriteLog();
