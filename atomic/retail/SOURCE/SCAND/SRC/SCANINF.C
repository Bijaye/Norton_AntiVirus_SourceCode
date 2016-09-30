// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/scand/VCS/scaninf.c_v   1.0   06 Feb 1997 21:08:40   RFULLER  $
//
// Description:
//      These are the functions to display the Infected File Information
//      dialog during scanning, or from the Problems Found dialog.
//
// Contains:
//      ScanInfoDialog()
//
// See Also:
//************************************************************************
// $Log:   S:/scand/VCS/scaninf.c_v  $
// 
//    Rev 1.0   06 Feb 1997 21:08:40   RFULLER
// Initial revision
// 
//    Rev 1.1   08 Aug 1996 13:26:16   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.28   01 Nov 1995 11:27:00   JWORDEN
// Fix file name displayed for MBR for info dialog
// 
//    Rev 1.27   24 Oct 1995 20:40:26   JWORDEN
// Process filename for MBR as a single digit drive number
// 
//    Rev 1.26   07 Jun 1995 10:27:30   BARRY
// Remove ZIP dependencies
// 
//    Rev 1.25   21 Mar 1995 14:11:38   DALLEE
// Commented out some unused zip code for NAVBOOT.
// 
//    Rev 1.24   29 Dec 1994 16:46:30   DALLEE
// Include syminteg.h before nonav.h
// 
//    Rev 1.23   28 Dec 1994 14:16:08   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.2   12 Dec 1994 16:16:56   DALLEE
    // Changed SPRINTF's to FastStringPrints() so format strings wouldn't have
    // to be modified (also preserves %p functionality).
    //
    //    Rev 1.1   05 Dec 1994 18:10:20   DALLEE
    // CVT1 script.
    //
//    Rev 1.22   28 Dec 1994 13:53:44   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:30   DALLEE
    // Initial revision.
    //
    //    Rev 1.19   16 Sep 1993 04:18:20   BRAD
    // No change.
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"

#include "navutil.h"
#include "options.h"
#include "syminteg.h"
#include "nonav.h"

#include "navdprot.h"
#include "scand.h"

#ifdef SYM_DOS
 #include <stdio.h>
#endif

//************************************************************************
// DEFINES USED LOCALLY
//************************************************************************

#define SCAN_INFO_CLOSE_BUTTON          0
#define SCAN_INFO_PRINT_BUTTON          1
#define SCAN_INFO_HELP_BUTTON           2
#define PREVIOUS_BUTTON                 3
#define NEXT_BUTTON                     4

#define VIRUS_COMMENT_WIDTH	(60)    // width of comment in virus list
					// information dialog


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID LOCAL PASCAL UpdateScanInfoDialog (LPINFECTEDFILE lpInfected);
BOOL LOCAL PASCAL FindInfectedRecord(LPUINT lpuCurrent,		// Contains result is search
				     UINT   uMax,		// Maximum to search through
				     BOOL   bSearchUp);		// TRUE = search up, FALSE = seach down

BOOL LOCAL PASCAL  AreMoreInfected(UINT     uCurrent,		// Contains result is search
				     UINT   uMax,		// Maximum to search through
				     BOOL   bSearchUp);




//************************************************************************
// ScanInfoDialog()
//
// This routine displays the Infected File Information dialog.
//
// Parameters:
//      BOOL    bDuringScan             Is the dialog being called
//                                      during scanning? (TRUE)
//                                      or post scanning? (FALSE)
//
// Returns:
//      Nothing
//************************************************************************
// 3/5/93 DALLEE Function created.
//************************************************************************

VOID PASCAL ScanInfoDialog (BOOL bDuringScan)
{
    extern  char            SZ_PHYSICAL_DRIVE [];

    extern  LPNONAV         glpNoNav;
    extern  ListRec         lrScanFoundItems;

    extern  DIALOG          dlScanInfo;

    extern  OutputFieldRec  ofVirusName;
    extern  OutputFieldRec  ofVirusAlias;
    extern  OutputFieldRec  ofLikelihood;
    extern  OutputFieldRec  ofSize;
    extern  OutputFieldRec  ofInfects;

    extern  OutputFieldRec  ofScanInfoFile;
    extern  OutputFieldRec  ofScanInfoStatus;

    extern  OutputFieldRec  ofComment1;
    extern  OutputFieldRec  ofComment2;
    extern  OutputFieldRec  ofComment3;
    extern  OutputFieldRec  ofComment4;
    extern  OutputFieldRec  ofComment5;

    extern  CheckBoxRec     cbMemoryResident;
    extern  CheckBoxRec     cbTriggerEvent;
    extern  CheckBoxRec     cbSizeStealth;
    extern  CheckBoxRec     cbEncrypting;
    extern  CheckBoxRec     cbFullStealth;
    extern  CheckBoxRec     cbPolymorphic;

    extern  ButtonsRec      buVirusInfo;
    extern  ButtonsRec      buPrevNext;

    auto    DIALOG_RECORD   *lpdrDialog;
    auto    WORD            wResult;
    auto    WORD            wOldHelp;
    auto    BOOL            bDone = FALSE;
    auto    UINT            uCurrentEntry,uPastEntry;
    auto    INFECTEDFILE    rInfected;
    auto    WORD            wButton;
    auto    char 	    szAlias[SIZE_ALIAS + 1];
    auto    char            szComment[SIZE_COMMENT + 1];
    auto    char            szPath[80+1];


                                        // If we're called during a scan,
                                        // use the current infected file and
                                        // do not display the Previous/Next
                                        // buttons.
                                        // Otherwise, use the current
                                        // Problems Found list entry.
    if (bDuringScan)
        {
        uCurrentEntry = 0;
        rInfected = glpNoNav->Infected;
        dlScanInfo.buttons = NULL;
        }
    else
        {
        uCurrentEntry = (UINT) lrScanFoundItems.active_entry;
        GetInfectedFile(&rInfected, glpNoNav, uCurrentEntry);
        dlScanInfo.buttons = &buPrevNext;
        }


    // make uPastEntry != uCurrentEntry to initialize the screen.

    uPastEntry = uCurrentEntry + 1;



    if (NULL == (lpdrDialog = DialogOpen2(&dlScanInfo,
                                          &ofScanInfoFile,
                                          &ofScanInfoStatus,
                                          &ofVirusName,
                                          &ofVirusAlias,
                                          &ofInfects,
                                          &buVirusInfo,
                                          &ofLikelihood,
                                          &ofSize,
                                          &cbMemoryResident,
                                          &cbTriggerEvent,
                                          &cbSizeStealth,
                                          &cbEncrypting,
                                          &cbFullStealth,
                                          &cbPolymorphic,
                                          &ofComment1,
                                          &ofComment2,
                                          &ofComment3,
                                          &ofComment4,
                                          &ofComment5) ))
        {
        return;
        }

    wOldHelp = HyperHelpTopicGet();
    HyperHelpTopicSet(HELP_DLG_INFO);

    do
        {
	if (uCurrentEntry != uPastEntry)
	    {
	    uPastEntry = uCurrentEntry;

            if (!bDuringScan)
		{
                GetInfectedFile(&rInfected,glpNoNav,uCurrentEntry);
	    	DisablePrevNext(uCurrentEntry, (UINT) lrScanFoundItems.num);
		}

            UpdateScanInfoDialog(&rInfected);

            if (!bDuringScan)
		{
	    	/*----SKURTZ-----06-18-93 12:36:28pm---------------
 	     	 * If none infected prior to this spot, disable.
 	     	 *-------------------------------------------------*/

            	if (!AreMoreInfected(uCurrentEntry,
			             (UINT) lrScanFoundItems.num,
			             FALSE))
        	    ButtonEnable(&buPrevNext, 0,  (BYTE) FALSE );


	    	/*----SKURTZ-----06-18-93 12:37:21pm---------------
 	     	 * If none infected after this spot, disable
 	     	 *-------------------------------------------------*/

    	    	if (!AreMoreInfected(uCurrentEntry,
			 	(UINT) lrScanFoundItems.num,
			 	TRUE))
        	    ButtonEnable(&buPrevNext, 1,  (BYTE) FALSE );
		}

	    }

        wResult = DialogLoop(lpdrDialog, NULL);

        if (wResult == ABORT_DIALOG)
            break;

        if (wResult != ACCEPT_DIALOG)
            {
            /* doing nothing with the dialog */
            continue;
            }


        wButton = DialogGetUniqueButtonNumber(lpdrDialog);

        switch (wButton)
            {
            case SCAN_INFO_CLOSE_BUTTON:
                bDone = TRUE;
                break;

            case SCAN_INFO_PRINT_BUTTON:
    		if (GetVirusInfo(rInfected.Notes.wVirID,
				 szAlias,
				 szComment) != 0)
		    {
		    szAlias[0] = szComment[0] = szComment[1] = EOS;
		    }
    		else
        	    WordWrapVirusComment(VIRUS_COMMENT_WIDTH,szComment);

                if (rInfected.wStatus == FILESTATUS_MASTERBOOTINFECTED
                ||  rInfected.wStatus == FILESTATUS_MASTERBOOTREPAIRED
                ||  rInfected.wStatus == FILESTATUS_MASTERBOOTNOTREPAIRED)
                {
                    SPRINTF (szPath, SZ_PHYSICAL_DRIVE, rInfected.szFullPath[0]);
                }
                else
                {
                    STRCPY (szPath, rInfected.szFullPath);
                }

                PrintVirusInformation(dlScanInfo.title,
                                szPath,
                                ScanFoundReturnStatusString(rInfected.wStatus),
                                &rInfected.Notes,
                                szAlias,
                                szComment);
                break;

            case SCAN_INFO_HELP_BUTTON:
		HyperHelpDisplay();
                break;

            case PREVIOUS_BUTTON:
		if (!bDuringScan)
		    FindInfectedRecord(&uCurrentEntry,
				       (UINT) lrScanFoundItems.num,
				       FALSE);
                break;

            case NEXT_BUTTON:
		if (!bDuringScan)
		    FindInfectedRecord(&uCurrentEntry,
				       (UINT) lrScanFoundItems.num,
				       TRUE);
                break;
            }
        } while (!bDone);

    DialogClose(lpdrDialog, wResult);

    HyperHelpTopicSet(wOldHelp);

} // End ScanInfoDialog()


/**************************************************************************
 * AreMoreInfected
 *
 * DESCRIPTION  : Encapsulates FindInfectedRecord() to just return a TRUE
 *		  or FALSE condition if there are any more infected files
 * 		  before or after this current record.
 * IN      :
 *
 * RETURN VALUES: TRUE or FALSE
 *************************************************************************
 * SKURTZ : 06-18-93 at 12:29:04pm Function created
 ************************************************************************* */
BOOL LOCAL PASCAL  AreMoreInfected(UINT     uCurrent,		// Contains result is search
				     UINT   uMax,		// Maximum to search through
				     BOOL   bSearchUp)
{

    return(FindInfectedRecord(&uCurrent,uMax,bSearchUp));

} // End AreMoreInfected()


/**************************************************************************
 * FindInfectedRecord
 *
 * DESCRIPTION  : Search either up or down (depending on bSearchUp) to find
 *		  another infected record.
 *
 * IN      :	  The current record #,
 * 		  the max number of records, and the search direction
 *
 * RETURN VALUES: TRUE or FALSE indicating success.  Also, lprInfected and
 *  		  lpuCurrent will be properly filled out.
 *    		  If FALSE condition, then original values are retained.
 *************************************************************************
 * SKURTZ : 06-18-93 at 11:49:00am Function created
 * DALLEE : 07-31-93 Check whether virus info is available for the problem
 *          file.  Easier than maintaining all the xxx FILESTATUS's that
 *          need to be checked.
 ************************************************************************* */
BOOL LOCAL PASCAL FindInfectedRecord(LPUINT lpuCurrent,		// Contains result is search
				     UINT   uMax,		// Maximum to search through
				     BOOL   bSearchUp)		// TRUE = search up, FALSE = seach down
{
    extern  LPNONAV             glpNoNav;
    extern  FILESTATUSSTRUCT    FileStatus[];

    auto    UINT                uLocalCurrent;
    auto    INFECTEDFILE        rInfected;


    uLocalCurrent = *lpuCurrent;



    for (;;)
	{

    	if (bSearchUp)
	    {
	    if (uLocalCurrent++ == (uMax - 1))
	       break;
	    }
    	else
	    {
	    if (uLocalCurrent-- == 0)
	       break;
	    }

        GetInfectedFile(&rInfected,glpNoNav,uLocalCurrent);

	// if found one that is infected, set return values and leave

        if (FileStatus[rInfected.wStatus].byHasVirusInfo)
	    {
	    *lpuCurrent = uLocalCurrent;
	    return(TRUE);
	    }
	}


     // None others found that were infected. Reset and leave.

     return(FALSE);


} // End FindInfectedRecord()


//************************************************************************
// UpdateScanInfoDialog()
//
// This routine updates all the output fields in the Problems Found Info
// dialog.
//
// Parameters:
//      LPINFECTEDFILE  lpInfected      Pointer to current infected file.
//
// Returns:
//      nothing
//************************************************************************
// 4/29/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL UpdateScanInfoDialog (LPINFECTEDFILE lpInfected)
{
    extern  char                SZ_INFO_FILE_FORMAT [];
    extern  char                SZ_PHYSICAL_DRIVE [];
    extern  OutputFieldRec      ofScanInfoFile;
    extern  OutputFieldRec      ofScanInfoStatus;

    auto    char                szBuffer [80+1];
    auto    char                szPath   [80+1];

    if (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED
    ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTREPAIRED
    ||  lpInfected->wStatus == FILESTATUS_MASTERBOOTNOTREPAIRED)
    {
        SPRINTF (szPath, SZ_PHYSICAL_DRIVE, *lpInfected->szFullPath);
    }
    else
    {
        STRCPY (szPath, lpInfected->szFullPath);
    }

    FastStringPrint(szBuffer,
                    SZ_INFO_FILE_FORMAT,
                    szPath);

    DialogOutput(&ofScanInfoFile, szBuffer);

    DialogOutput(&ofScanInfoStatus,
                 ScanFoundReturnStatusString(lpInfected->wStatus));

    VirusInfoUpdate(&lpInfected->Notes);

} // UpdateScanInfoDialog()

