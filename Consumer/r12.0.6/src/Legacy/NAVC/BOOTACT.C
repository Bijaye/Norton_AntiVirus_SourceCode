// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVC/VCS/bootact.c_v   1.4   04 Aug 1997 22:01:50   BGERHAR  $
//
// Description:
//		These are the functions called to respond to infections in
//		boot records.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVC/VCS/bootact.c_v  $
//
//    Rev 1.4   04 Aug 1997 22:01:50   BGERHAR
// Prompt for MBR or boot record virus if /PROMPT used
//
//    Rev 1.3   30 Jul 1997 17:04:52   MKEATIN
// If were in Startup Mode sound a beep.
//
//    Rev 1.2   30 Jul 1997 16:50:06   MKEATIN
// If we are in startup mode, prompt the user when a virus is found.
//
//    Rev 1.1   31 May 1997 18:06:12   MKEATIN
// Passed NULL (temporarily) as the lpvCookie paramter in VirusScanRepairBoot.
//
//    Rev 1.0   06 Feb 1997 20:56:20   RFULLER
// Initial revision
//
//    Rev 1.0   31 Dec 1996 15:19:58   MKEATIN
// Initial revision.
//
//    Rev 1.9   06 Dec 1996 19:00:06   JBELDEN
// added code to prompt when doing a boot repair
//
//    Rev 1.8   19 Nov 1996 14:02:28   JBELDEN
// No change.
//
//    Rev 1.7   19 Nov 1996 11:22:32   JBELDEN
// added missing externs for logging
//
//    Rev 1.6   19 Nov 1996 09:57:40   JBELDEN
// added fprintf's for logging
//
//    Rev 1.5   08 Nov 1996 10:31:28   JBELDEN
// removed stubbed out routines.
//
//    Rev 1.4   07 Nov 1996 17:31:06   JBELDEN
//
//    Rev 1.3   31 Oct 1996 16:05:20   JBELDEN
// removed code related to dialog boxes
//
//    Rev 1.2   31 Oct 1996 11:12:18   JBELDEN
// fixed test messages from printf
//
//    Rev 1.1   30 Oct 1996 18:27:00   JBELDEN
// added printf for repair and can't repair
//
//    Rev 1.0   02 Oct 1996 12:59:02   JBELDEN
// Initial revision.
//************************************************************************

#include "platform.h"
#include <stdio.h>
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

extern   FILE  *fLogFile;
extern   BOOL  bAutoExportLog;




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
   extern char IDS_BOOTREPAIRED[];
   extern char IDS_BOOTNOTREPAIRED[];
   extern char IDS_MBRREPAIRED[];
   extern char IDS_MBRNOTREPAIRED[];
   extern char szRepairYN[];
   extern char SZ_YES[];
   extern char SZ_NO[];
   extern BOOL bStartupMode;
   extern BOOL bNavcPrompt;
#ifndef NAVSCAN
	auto	WORD		wResult;
	auto	LOGSTRUCT	Log;
#ifdef USE_NETWORKALERTS   //&?
	auto	NETALERT	rAlert;
#endif
	auto	char		szLocalVirus [SIZE_VIRUSNAME + 1];
#endif	// !NAVSCAN
   auto  char     szYESNO[4];
   auto  UINT     uOldAction;

	if (lpNoNav->Infected.uVirusSigIndex != 0)
		{
#ifndef NAVSCAN
		if ((lpNoNav->Alert.bBeep && lpNoNav->Opt.bPrestoNotify) || bStartupMode)
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

      if (bStartupMode || bNavcPrompt)
          {
          uOldAction = lpNoNav->Opt.uAction;
          lpNoNav->Opt.uAction = SCAN_REPAUTO;
          }

		switch (lpNoNav->Opt.uAction)
			{
			case SCAN_REPAUTO:
            printf(szRepairYN);
            scanf("%s", &szYESNO);
            while ( (0 != stricmp (SZ_YES,szYESNO) ) &&
                 (0 != stricmp (SZ_NO,szYESNO) ) )
               {
               printf(szRepairYN);
               scanf("%s", &szYESNO);
               }
            if ( 0 == stricmp(SZ_NO, szYESNO) )
                  return;

				wResult = RepairBoot(lpNoNav, &lpNoNav->Infected);
            switch(lpNoNav->Infected.wStatus)
               {
               case  FILESTATUS_BOOTREPAIRED:
                  printf(IDS_BOOTREPAIRED,lpNoNav->Infected.szFullPath[0], lpNoNav->Infected.Notes.lpVirName);
                  if (bAutoExportLog == TRUE)
                     fprintf(fLogFile, IDS_BOOTREPAIRED,lpNoNav->Infected.szFullPath[0], lpNoNav->Infected.Notes.lpVirName);
                  break;
               case  FILESTATUS_MASTERBOOTREPAIRED:
                  printf(IDS_MBRREPAIRED, lpNoNav->Infected.szFullPath[0], lpNoNav->Infected.Notes.lpVirName);
                  if (bAutoExportLog == TRUE)
                     fprintf(fLogFile, IDS_MBRREPAIRED, lpNoNav->Infected.szFullPath[0], lpNoNav->Infected.Notes.lpVirName);
                  break;
               case  FILESTATUS_BOOTNOTREPAIRED:
                  printf(IDS_BOOTNOTREPAIRED, lpNoNav->Infected.szFullPath[0], lpNoNav->Infected.Notes.lpVirName);
                  if (bAutoExportLog == TRUE)
                     fprintf(fLogFile, IDS_BOOTNOTREPAIRED, lpNoNav->Infected.szFullPath[0], lpNoNav->Infected.Notes.lpVirName);
                  break;
               case  FILESTATUS_MASTERBOOTNOTREPAIRED:
                  printf(IDS_MBRNOTREPAIRED, lpNoNav->Infected.szFullPath[0], lpNoNav->Infected.Notes.lpVirName);
                  if (bAutoExportLog == TRUE)
                     fprintf(fLogFile, IDS_MBRNOTREPAIRED, lpNoNav->Infected.szFullPath[0], lpNoNav->Infected.Notes.lpVirName);
                  break;
               default:
                  break;
               }
            break;

			case SCAN_REPORT:
			case SCAN_DELAUTO:
			default:

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

      if (bStartupMode)
          lpNoNav->Opt.uAction = uOldAction;

		AddVirusInfo(lpNoNav);
		}


} // End RespondToBootVirus()

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


	auto	UINT		uResult = FALSE;
	auto	UINT		uRepairType;
	auto	UINT		uStatus = FALSE;
	auto	INTEG		rInteg;
	auto	N30NOTES	rNotes;
	auto	char		szIntegFile [SYM_MAX_PATH];

	uRepairType = (lpInfected->wStatus == FILESTATUS_MASTERBOOTINFECTED ?
						MBR_REPAIR : BOOT_REPAIR);

	TSR_OFF;


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


