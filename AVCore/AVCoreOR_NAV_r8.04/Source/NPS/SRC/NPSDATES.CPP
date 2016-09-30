/* Copyright 1991-1993 Symantec                                         */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/NPS/VCS/npsdates.cpv   1.13   09 Apr 1998 16:51:22   JALLEE  $ */
/*                                                                      */
/* Description:                                                         */
/*	This file contains date/time settings and valiations.		*/
/*                                                                      */
/* Contains:                                                            */
/*                                                                      */
/************************************************************************/
/* $Log:   S:/NPS/VCS/npsdates.cpv  $ */
// 
//    Rev 1.13   09 Apr 1998 16:51:22   JALLEE
// Check localtime() return for NULL ptr.
// 
//    Rev 1.12   22 Feb 1996 13:26:12   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
// 
//    Rev 1.11   11 Jan 1996 09:49:04   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.10.1.1   30 Nov 1995 21:03:30   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
// 
//    Rev 1.10   03 May 1995 13:19:52   MFALLEN
// Moved stdhelp.h higher up in the include order.
// 
//    Rev 1.9   26 Mar 1994 21:00:28   BRAD
// Cleaned up Windows messages
// 
//    Rev 1.8   15 Mar 1994 11:58:20   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
// 
//    Rev 1.7   03 Mar 1994 12:09:54   BRAD
// AWK script applied
// 
//    Rev 1.6   29 Sep 1993 14:53:54   MARKL
// No change.
// 
//    Rev 1.5   23 Jul 1993 10:28:02   MARKL
// Help menu now looks like nirvana's.  Removed help/exit buttons.  All
// child dialogs are centered.
// 
//    Rev 1.4   22 Jul 1993 12:02:22   MARKL
// Hooked up quake help system.
// 
//    Rev 1.3   10 Jun 1993 09:31:54   KYECK
// Added support for 'yearly' events.
// 
//    Rev 1.2   21 Apr 1993 22:05:08   MARKL
// Nirvana gives the schedule a chance to take an a radically new look/feel
// in the Add/Edit dialog!!!
// 
//    Rev 1.1   18 Mar 1993 20:32:18   MARKL
// Cleaned up unnecessary typecasting of LPSTR.
// 
//    Rev 1.0   15 Mar 1993 17:50:46   MARKL
// Initial revision.
// 
//    Rev 1.5   26 Jun 1992 17:55:14   MEIJEN
// Get rid of strids.h because NWRES will eventually disappear
// 
//    Rev 1.4   19 Jun 1992 10:34:00   MEIJEN
// Decentralize resources from NWRES to NPS(scheduler) for REVOLVER
// 
//    Rev 1.3   15 Oct 1991 10:21:18   MARKL
// The minutes field in the add/edit dialog box was not having its
// contents reset correctly.
// 
//    Rev 1.2   11 Oct 1991  8:10:44   MARKL
// Correct SpnButton_ResetContent() parameters.
// 
//    Rev 1.1   02 Oct 1991 12:06:28   MARKL
// Made WinStric.H pass.
// 
//    Rev 1.0   28 Jul 1991 11:37:34   MARKL
// Initial revision.
// 
//    Rev 1.0   01 Jun 1991 20:34:48   MARKL
// Initial revision.
/************************************************************************/
#include    "platform.h"
#include    "stdhelp.h"
#include    <windowsx.h>
#include    <sys\timeb.h>
#include    <time.h>
#include    "undoc.h"                  /* for extra typedefs           */
#include    "msgdisp.h"                    /* error message stuff          */
#include    "ngui.h"
#include    "symintl.h"
#include    "nps.h"                     /* scheduler includes           */
#include    "npsres.h"
#include    "npsstb.h"



/************************************************************************/
/* NPSDATES routines                                                    */
/************************************************************************/




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      Initialize the add/edit event dialog box controls.              */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           -- Function completed.          */
/*                                                                      */
/************************************************************************/
/* 01/05/91 CJR Function created.                                       */
/* 02/22/91 MSL Months are now a spin button.  Spin button              */
/*              initialization was moved here.                          */
/* 02/28/91 MSL Uses internationalization values for some spin button   */
/*              formats.  Can set current selection to previous value   */
/*              were applicable.                                        */
/* 03/01/91 MSL Fixed some major big time errors in logic.              */
/* 05/07/91 MSL Since spin buttons are now editable, screw formatting	*/
/*		the numeric output EXCEPT FOR ID_NPS_MIN!!!     	*/
/* 04/19/93 MSL Radically modified and renamed to NpsAddEditCtlsInit()  */
/*              to initialize all of the controls in the Add/Edit       */
/*              dialog box.                                             */
/************************************************************************/
WORD PASCAL NpsAddEditCtlsInit (  	/* Init Controls in Add/Edit    */
    HWND        hDlg,                   /* Handle to dialog box         */
    BOOL        bInitDialog)            /* TRUE if initing dialog box   */ 
{
    WORD        	wIndex;         /* Index into items             */
    char                szBuf[DSIZE + 1];
    struct      tm      *curtm;
    struct      _timeb  curtimeb;

                                        /* Get the current date and     */
                                        /* time.                        */
    _ftime(&curtimeb);
    curtm = localtime (&curtimeb.time); // Careful, localtime() can
                                        // return NULL.

                                        /* Do those that only have to 	*/
					/* be initialized once.         */
    if (bInitDialog)
        {
                                        /* Initialize frequencies combo */
                                        /* box.                         */
        for (wIndex = 0; wIndex < NFREQ; wIndex++)
            {
            ComboBox_AddString (GetDlgItem (hDlg, IDC_NPS_FREQ), 
                                freqs[wIndex]);
            }

                                        /* Initialize the minutes       */
                                        /* spin button.                 */
        SpinButton_ResetContent (GetDlgItem (hDlg, IDC_NPS_MINUTES), 
                                 SPN_TYPE_NUMBERS, NPS_FORMAT_MIN);
        SpinButton_SetRange (GetDlgItem (hDlg, IDC_NPS_MINUTES), 0, 59);

                                        /* Initialize the days of month */
                                        /* spin button.                 */
        SpinButton_ResetContent (GetDlgItem (hDlg, IDC_NPS_DAYOFMONTH), 
                                 SPN_TYPE_NUMBERS, NPS_FORMAT_DAY);
        SpinButton_SetRange (GetDlgItem (hDlg, IDC_NPS_DAYOFMONTH), 1, 31);


                                        /* Initialize the months of the */
                                        /* year spin button.            */
        SpinButton_ResetContent (GetDlgItem (hDlg, IDC_NPS_MONTH), 
                                 SPN_TYPE_STRINGS, NULL);
        for (wIndex = 0; wIndex < NMONTH; wIndex++)
            {
            SpinButton_AddString (GetDlgItem (hDlg, IDC_NPS_MONTH), 
                                  months[wIndex]);
            }

                                        /* Initialize the days of the   */
                                        /* week spin button.            */
        SpinButton_ResetContent (GetDlgItem (hDlg, IDC_NPS_DAY), 
                                 SPN_TYPE_STRINGS, NULL);
        for (wIndex = 0; wIndex < NDAY; wIndex++)
            {
            SpinButton_AddString (GetDlgItem (hDlg, IDC_NPS_DAY), 
                                  days[wIndex]);
            }

                                        /* Initialize the runstyle drop */
                                        /* down combo.                  */
        for (wIndex = 0; wIndex < NRUNSTYLE; wIndex++)
            {
            ComboBox_AddString (GetDlgItem (hDlg, IDC_NPS_RUNSTYLE), 
                                runstyle[wIndex]);
            }
        }
                                        /* Now initialize those things  */
                                        /* that could be changed.       */

                                        // Set current minute
    
    if ( NULL != curtm )
        {
                                            /* Update the Date field.       */
        NSetIntlDate (szBuf, curtm->tm_mon+1, curtm->tm_mday, curtm->tm_year);
        SetDlgItemText (hDlg, IDC_NPS_DATE, szBuf);
    
                                            /* Update the Time field.       */
        NSetIntlTime (szBuf, curtm->tm_hour, curtm->tm_min, curtm->tm_sec,
                      FALSE, !(BOOL)GetProfileInt ("Intl", "iTime", 0));
        SetDlgItemText (hDlg, IDC_NPS_TIME, szBuf);
        }
    
                                        /* Get out of here!             */
    return (NOERR);
} // NpsAddEditCtlsInit()









