// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/SCAND/VCS/bootact.c_v   1.1   31 May 1997 18:09:44   MKEATIN  $
//
// Description:
//		These are the functions called to respond to infections in
//		boot records.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/SCAND/VCS/bootact.c_v  $
// 
//    Rev 1.1   31 May 1997 18:09:44   MKEATIN
// Passed NULL as the lpvCookie parameter in VirusScanRepairBoot.
// 
//    Rev 1.0   06 Feb 1997 21:08:52   RFULLER
// Initial revision
// 
//    Rev 1.2   08 Aug 1996 13:28:00   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.53   13 Jun 1996 14:03:56   JALLEE
// #if USE_NETWORKALERTS changed to #ifdef USE_NETWORKALERTS.
// 
//    Rev 1.52   04 Mar 1996 18:38:18   MKEATIN
// Ported DX fixes from KIRIN
// 
//    Rev 1.1   31 Jan 1996 16:15:30   MKEATIN
// Some code still sent an array of three strings instead of five to cutstring()
// 
//    Rev 1.0   30 Jan 1996 16:00:08   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 14:53:22   BARRY
// Initial revision.
// 
//    Rev 1.51.1.0   11 Dec 1995 16:33:26   JREARDON
// Branch base for version LUG1
// 
//    Rev 1.51   15 Nov 1995 09:47:04   JWORDEN
// Fix floppy disk boot repair. Pass drive letter as a drive letter
// for floppies. Not as a physical drive number (+ 0x80)
//
//	  Rev 1.50	 14 Nov 1995 12:00:22	JWORDEN
// Initialize uStatus and uResult to FALSE in RepairBoot()
//
//	  Rev 1.49	 02 Nov 1995 17:04:20	JWORDEN
// Convert MasterBootRec booleans to counters to allow for multiple MBRs
//
//	  Rev 1.48	 30 Oct 1995 16:48:24	JWORDEN
// Fix RepairBoot () to detect physical drive number when testing for
// MBR integ repair
//
//	  Rev 1.47	 24 Oct 1995 20:35:10	JWORDEN
// Pass new parameter to VirusScanMasterBoot
// Change parameter passed to VirusScanRepairBoot for MBR
//
//	  Rev 1.46	 27 Jun 1995 20:48:04	SZIADEH
// Inititalized uResult in RepairBoot function.
//
//	  Rev 1.45	 26 Jun 1995 22:27:28	SZIADEH
// apply sysinteg repair first (if exists) on boot/mbr infections.
//
//	  Rev 1.44	 21 Jun 1995 11:39:34	SZIADEH
// activated mus4_beep.
//
//	  Rev 1.43	 14 Mar 1995 17:46:04	DALLEE
// Hide display during startup util problems are found.
//
//	  Rev 1.42	 21 Feb 1995 11:15:44	DALLEE
// Re-enabled system integrity.  See SYSINOC.C r1.25 for notes.
//
//	  Rev 1.41	 14 Feb 1995 21:49:26	DALLEE
// Ack!  Had to comment out sysinteg for one build... still bombing on some
// machines...
//
//	  Rev 1.40	 13 Feb 1995 22:59:04	DALLEE
// Re-enable system integrity repair backups for boot repairs.
//
//	  Rev 1.39	 29 Dec 1994 16:46:08	DALLEE
// Include syminteg.h before nonav.h
//
//	  Rev 1.38	 28 Dec 1994 14:15:44	DALLEE
// Latest NAVBOOT revision.
//
	//	  Rev 1.5	27 Dec 1994 19:36:18   DALLEE
	// Convert to use CERTLIBN.
	//
	//	  Rev 1.4	27 Dec 1994 15:40:42   DALLEE
	// Commented out MUS4_BEEP.  Replace later.
	//
	//	  Rev 1.3	12 Dec 1994 18:10:54   DALLEE
	// Commented out network alerts and password protection.
	//
	//	  Rev 1.2	06 Dec 1994 15:44:52   DALLEE
	// Comment out sysinteg.
	//
	//	  Rev 1.1	05 Dec 1994 18:10:10   DALLEE
	// CVT1 script.
	//
//	  Rev 1.37	 28 Dec 1994 13:53:18	DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
//
	//	  Rev 1.0	23 Nov 1994 15:37:58   DALLEE
	// Initial revision.
	//
	//	  Rev 1.33	 24 Oct 1994 11:44:22	DALLEE
	// Added check for DISPLAY_DIALOG before showing long repair dialog.
	// Check NO_GENERIC and abort generic boot repair using sysinteg data.
//************************************************************************

#include "platform.h"
#include "stddos.h"
#include "ctsn.h"
#include "repair.h"
#include "virscan.h"

#include "options.h"
#include "navutil.h"
#include "syminteg.h"
#include "nonav.h"

#include "navdprot.h"

#include "scand.h"
#ifdef USE_NETWORKALERTS   //&?
 #include "netalert.h"
#endif
#include "tsr.h"
#include "tsrcomm.h"

//************************************************************************
// DEFINES
//************************************************************************

#define BOOT_REPAIR_BUTTON		0
#define BOOT_INFO_BUTTON		1
#define BOOT_STOP_BUTTON		2
#define BOOT_CONTINUE_BUTTON	3


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************

VOID LOCAL PASCAL DisablePromptBootResponseButtons (LPNONAV lpNoNav);
int STATIC PromptBootResponseButtonsProc(DIALOG_RECORD *lpdrDialog);


//************************************************************************
// RespondToBootVirus()
//
// This routine decides what action to take when a virus is found by
// the scanner depending on the Scanner Options settings.
//
// Parameters:
//		LPNONAV 	lpNoNav 			The present scan's information
//
// Returns:
//		Nothing
//************************************************************************
// 5/12/93 DALLEE Function created.
//************************************************************************

VOID PASCAL RespondToBootVirus (LPNONAV lpNoNav)
{
#ifndef NAVSCAN
	auto	WORD		wResult;
	auto	LOGSTRUCT	Log;
#ifdef USE_NETWORKALERTS   //&?
	auto	NETALERT	rAlert;
#endif
	auto	char		szLocalVirus [SIZE_VIRUSNAME + 1];
#endif	// !NAVSCAN

	if (lpNoNav->Infected.uVirusSigIndex != 0)
		{
#ifndef NAVSCAN
		if (lpNoNav->Alert.bBeep && lpNoNav->Opt.bPrestoNotify)
			{
			MUS4_BEEP(BEEP_KNOWNBOX);
			}

#ifdef USE_NETWORKALERTS   //&?
		MEMSET(&rAlert, '\0', sizeof(rAlert));
		rAlert.lpNet = lpNoNav->lpNetUserItem;
		rAlert.lpInfected = &lpNoNav->Infected;
		rAlert.lpAlertOpt = &lpNoNav->Alert;
		rAlert.uTypeOfEvent = EV_ALERT_KNOWN_VIRUS;

		NetAlert(&rAlert);
#endif
		ErrorLevelSet ( ERRORLEVEL_VIRUSFOUND ) ;

		switch (lpNoNav->Opt.uAction)
			{
			case SCAN_PROMPT:
										// If Immediate Notification is set,
										// act now.
				if (lpNoNav->Opt.bPrestoNotify)
					{
					ScanStopWatch(lpNoNav, FALSE);
					PromptBootResponse(lpNoNav);
					ScanStopWatch(lpNoNav, TRUE);
					}
				break;

			case SCAN_REPAUTO:
				wResult = RepairBoot(lpNoNav, &lpNoNav->Infected);

										// Should we notify the user now?
				if (lpNoNav->Opt.bPrestoNotify == TRUE)
					{
					ScanStopWatch(lpNoNav, FALSE);
					ScanActionMessage(&lpNoNav->Infected);
					ScanStopWatch(lpNoNav, TRUE);
					}
				break;

			case SCAN_REPORT:
			case SCAN_DELAUTO:
			default:
										// If Immediate Notification is set,
										// act now.
				if (lpNoNav->Opt.bPrestoNotify == TRUE)
					{
					ScanStopWatch(lpNoNav, FALSE);
					ScanNotify(lpNoNav);
					ScanStopWatch(lpNoNav, TRUE);
					}
				break;

			case SCAN_HALTCPU:
				if (NOERR == NAVDWriteLog(lpNoNav))
					{
					STRNCPY(szLocalVirus,
							lpNoNav->Infected.Notes.lpVirName,
							SIZE_VIRUSNAME);
					szLocalVirus [sizeof(szLocalVirus) - 1] = EOS;

					MEMSET(&Log, '\0', sizeof(Log));
					Log.uEvent		= LOGEV_HALTCPU;;
					Log.lpFileName	= lpNoNav->Infected.szFullPath;
					Log.lpVirusName = szLocalVirus;
					Log.lpAct		= &lpNoNav->Act;

					LogEvent(&Log);
					}
				ScanHalt(lpNoNav);
				break;

			} // End switch(lpNoNav->...
#endif	// !NAVSCAN

		AddVirusInfo(lpNoNav);
		}
} // End RespondToBootVirus()


//************************************************************************
// PromptBootResponse()
//
// This routine prompts the user for what action to take when a
// virus has been found in a sector.
//
// Parameters:
//		LPNONAV lpNoNav 				Info on this scan.
//
// Returns:
//		Nothing
//************************************************************************
// 5/12/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL PromptBootResponse (LPNONAV lpNoNav)
{
	extern	DIALOG			dlBootVirusFound;
	extern	ButtonsRec		buBootVirusFound;
	extern	char			*lpszBootVirusFoundPromptStrings [];

	extern	DIALOG_RECORD	*glpScanFilesDialog;
	extern	DIALOG_RECORD	*glpMemoryBootDialog;
	extern	BOOL			bScreenStarted;

	auto	BOOL			bDone = FALSE;
	auto	WORD			wResult;
	auto	DIALOG_RECORD	*lpdrDialog;

	auto	LPSTR			lpszAlertMessageNext;
	auto	char			lpAlertMessageBuffer [2][50 + 1];
	auto	char			lpMessageBuffer [SCAN_PROMPT_MESSAGE_LINES][50 + 1];
	auto	LPSTR			lpszMessage [SCAN_PROMPT_MESSAGE_LINES] =
								{
								lpMessageBuffer[0],
								lpMessageBuffer[1],
								lpMessageBuffer[2],
								lpMessageBuffer[3],
								lpMessageBuffer[4]
								};

										// Make sure screen IO is started.
	if (!bScreenStarted)
		{
		InitDisplay(TRUE);
		glpScanFilesDialog = OpenScanFilesDialog(lpNoNav);
		glpMemoryBootDialog = OpenMemoryBootDialog(lpNoNav);
		}

										// Set up the dialog.
	dlBootVirusFound.buttons = &buBootVirusFound;
	dlBootVirusFound.strings = lpszBootVirusFoundPromptStrings;

	if ( lpNoNav->Alert.bDispAlertMsg )
		{
		lpszAlertMessageNext = CutString(lpAlertMessageBuffer[0],
									 lpNoNav->Alert.szAlertMsg,
									 50);
		CutString(lpAlertMessageBuffer[1],
			  lpszAlertMessageNext,
			  50);
		}
	else
		{
		lpAlertMessageBuffer[0][0] = lpAlertMessageBuffer[1][0] = EOS;
		}

	ScanFoundCreateMessage(lpszMessage,
						   &lpNoNav->Infected);

	lpdrDialog = DialogOpen2( &dlBootVirusFound,
							  lpszMessage[0],
							  lpszMessage[1],
							  lpszMessage[2],
							  lpszMessage[3],
							  lpszMessage[4],
							  lpAlertMessageBuffer[0],
							  lpAlertMessageBuffer[1]);

	DisablePromptBootResponseButtons(lpNoNav);

	do
		{
		wResult = DialogProcessEvent(lpdrDialog, DialogGetEvent());

										// If ESC was hit, continue with
										// the scan (if allowed).
		if ((wResult == ABORT_DIALOG) && (lpNoNav->Opt.bDispCont))
			{
			bDone = TRUE;
			}
										// Otherwise check for a button
										// press.
		else if (wResult == ACCEPT_DIALOG)
			{
			wResult = PromptBootResponseButtonsProc(lpdrDialog);
			if ((wResult == ACCEPT_DIALOG) || (wResult == ABORT_DIALOG))
				{
				bDone = TRUE;
				}
			DisablePromptBootResponseButtons(lpNoNav);
			}
		}
		while (!bDone);

	DialogClose(lpdrDialog, wResult);
} // End PromptBootResponse()


//************************************************************************
// PromptBootResponseButtonsProc()
//
// This routine handles button presses for the Virus Found prompt for
// response dialog.
//
// Parameters:
//		DIALOG_RECORD *drDialog
//
// Returns:
//		EVENT_AVAIL 					further processing of event
//		ABORT_DIALOG					Stop hit
//		ACCEPT_DIALOG					Continue hit
//************************************************************************
// 5/12/93 DALLEE, Function created.
//************************************************************************

int STATIC PromptBootResponseButtonsProc(DIALOG_RECORD *lpdrDialog)
{
	extern	LPNONAV 	glpNoNav;
	extern	BOOL		gbContinueScan;

	auto	int 		nRetVal = EVENT_AVAIL;


	switch (lpdrDialog->d.buttons->value)	// Current button selected
		{
		case BOOT_REPAIR_BUTTON:
			if (RepairBoot(glpNoNav, &glpNoNav->Infected) == TRUE)
				{
				nRetVal = ACCEPT_DIALOG;
				}
			ScanActionMessage(&glpNoNav->Infected);
			break;

		case BOOT_INFO_BUTTON:
			ScanInfoDialog(TRUE);
			break;

		case BOOT_STOP_BUTTON:
			gbContinueScan = FALSE;
			nRetVal = ABORT_DIALOG;
			break;

		case BOOT_CONTINUE_BUTTON:
			nRetVal = ACCEPT_DIALOG;
			break;
		} // End switch (lpButtons->value)

	return (nRetVal);					// default return value
} // End PromptResponseButtonsProc()


//************************************************************************
// DisablePromptBootResponseButtons()
//
// This routine disables buttons in the PromptResponse dialog depending
// on the SCANNER options.
//
// Parameters:
//		LPNONAV 	lpNoNav 			Options for the present scan.
//
// Returns:
//		nothing
//************************************************************************
// 5/12/93 DALLEE, Function created.
//************************************************************************

VOID LOCAL PASCAL DisablePromptBootResponseButtons (LPNONAV lpNoNav)
{
	extern	FILESTATUSSTRUCT	FileStatus[];
	extern	ButtonsRec			buBootVirusFound;

	auto	BYTE				byRepair;

	byRepair = FileStatus[lpNoNav->Infected.wStatus].byCanRepair &&
			   lpNoNav->Opt.bDispRepair;

	ButtonEnable(&buBootVirusFound,
				 BOOT_REPAIR_BUTTON,
				 byRepair);

	ButtonEnable(&buBootVirusFound,
				 BOOT_STOP_BUTTON,
				 lpNoNav->Opt.bAllowScanStop);

										// Force the Continue button
										// on if the other buttons are all
										// disabled.
	if (!byRepair &&
		!lpNoNav->Opt.bAllowScanStop)
	{
		ButtonEnable(&buBootVirusFound,
				 BOOT_CONTINUE_BUTTON,
				 TRUE);
	}
	else
	{
		ButtonEnable(&buBootVirusFound,
				 BOOT_CONTINUE_BUTTON,
				 lpNoNav->Opt.bDispCont);
	}

} // End DisablePrompResponseButtons()


//************************************************************************
// RepairBoot()
//
// This routine attempts to repair a specified boot record.
//
// Parameters:
//
// Parameters:
//		LPNONAV 		lpNoNav 		Info on the current scan.
//		LPINFECTEDFILE	lpInfected		Info on boot record to repair.
//
// Returns:
//		TRUE							Boot record successfully repaired.
//		FALSE							Couldn't repair it.
//************************************************************************
// 5/12/93 DALLEE, Function created.
//************************************************************************

BOOL PASCAL RepairBoot (LPNONAV lpNoNav, LPINFECTEDFILE lpInfected)
{
	extern	DIALOG_RECORD	*glpScanFilesDialog;
	extern	DIALOG_RECORD	*glpMemoryBootDialog;
	extern	BOOL			bScreenStarted;

	extern	DIALOG			dlWaitForRepair;
	extern	OutputFieldRec	ofRepairSteps;

	auto	UINT		uResult = FALSE;
	auto	UINT		uRepairType;
	auto	UINT		uStatus = FALSE;
	auto	INTEG		rInteg;
	auto	N30NOTES	rNotes;
	auto	char		szIntegFile [SYM_MAX_PATH];

	auto	char			szVirusName [SIZE_VIRUSNAME + 1];
	auto	DIALOG_RECORD	*lpdrDialog = NULL;

	uRepairType = (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED ?
						MBR_REPAIR : BOOT_REPAIR);

	TSR_OFF;

	if (DISPLAY_DIALOG & lpInfected->Notes.wControl2)
		{
		if (!bScreenStarted)
			{
			InitDisplay(TRUE);
			glpScanFilesDialog = OpenScanFilesDialog(lpNoNav);
			glpMemoryBootDialog = OpenMemoryBootDialog(lpNoNav);
			}

		STRNCPY(szVirusName, lpInfected->Notes.lpVirName, sizeof(szVirusName));
		szVirusName[sizeof(szVirusName) - 1] = EOS;

		lpdrDialog = DialogOpen2(&dlWaitForRepair,
								 szVirusName,
								 &ofRepairSteps);
		if (NULL != lpdrDialog)
			{
			StdDlgStartWait();
			CursorSetRowCol(ofRepairSteps.row, ofRepairSteps.col);
			}
		}


	uResult = FALSE;
											   // try sysinteg first
	if ((uRepairType == MBR_REPAIR && lpInfected->szFullPath[0] == 0x00)
	||	(uRepairType != MBR_REPAIR && lpInfected->szFullPath[0] == 'C'))
		{
		if (!(NO_GENERIC & lpInfected->Notes.wControl2)
		&&	lpNoNav->Inoc.bUseSystemInoc )
			{
			rInteg.wStatus = INTEG_FAIL_VERIFY;
			STRCPY(rInteg.byFilename, (uRepairType == MBR_REPAIR ? "MBR80"
																 : "BOOT800"));

			GetSysIntegFile(szIntegFile, SYM_MAX_PATH, &lpNoNav->Inoc);

			if (IntegVerifyDatabaseMatch(szIntegFile))
				{
				IntegRepair(szIntegFile, &rInteg);
				}

											// Verify the repair.
			uResult = (uRepairType == MBR_REPAIR
					   ? !VirusScanMasterBoot ((UINT)(lpInfected->szFullPath[0] + 0x80),
											   &rNotes,
											   &uStatus)
					   : !VirusScanBoot(lpInfected->szFullPath[0],
										&rNotes,
										&uStatus) );
			if (NOERR != uStatus)			// Make sure the scan was valid.
				{
				uResult = FALSE;
				}
			}
		}

										// if sysinteg fails, do it
	if ( uResult == FALSE ) 			// the old fashion way!
		{
		if (uRepairType == MBR_REPAIR)
			{
			uResult = VirusScanRepairBoot((BYTE)(lpInfected->szFullPath[0] + 0x80),
										  lpInfected->uVirusSigIndex,
										  uRepairType,
                                NULL);
			}
		else
			{
			uResult = VirusScanRepairBoot((BYTE)(lpInfected->szFullPath[0]),
										  lpInfected->uVirusSigIndex,
										  uRepairType,
                                NULL);
			}
		if (NULL != lpdrDialog)
			{
			StdDlgEndWait();
			DialogClose(lpdrDialog, 0);
			}
		}


	TSR_ON;

	lpInfected->wStatus = UpdateFileStatus(lpInfected->wStatus,
										   PROCESS_REPAIR,
										   uResult);

										// Update the NONAV info if we
										// repaired the boot rec.
	if (uResult == TRUE)
		{
		if (lpInfected->wStatus == FILESTATUS_MASTERBOOTREPAIRED)
			{
			lpNoNav->Stats.MasterBootRec.uCleaned++;
			}
		else
			{
			lpNoNav->Stats.BootRecs.uCleaned++;
			}
		}

	return ((BOOL) uResult);
} // End RepairFile()


