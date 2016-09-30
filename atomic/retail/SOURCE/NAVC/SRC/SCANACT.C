// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navc/VCS/scanact.c_v   1.0   06 Feb 1997 20:56:20   RFULLER  $
//
// Description:
//      These are the functions for actions taken after scanning for
//      viruses.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/navc/VCS/scanact.c_v  $
// 
//    Rev 1.0   06 Feb 1997 20:56:20   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:20:02   MKEATIN
// Initial revision.
// 
//    Rev 1.5   19 Nov 1996 11:22:12   JBELDEN
// added fprintf's for logging
// 
//    Rev 1.4   07 Nov 1996 17:33:42   JBELDEN
// 
//    Rev 1.3   07 Nov 1996 15:10:46   JBELDEN
// cleaned up and removed unused code
// 
//    Rev 1.2   31 Oct 1996 13:07:00   JBELDEN
// added responses to delete and repair files
// 
//    Rev 1.1   28 Oct 1996 10:16:28   JBELDEN
// put ifdef's around code for USE_NETWORKALERTS that should
// have been there anyway.
// 
//    Rev 1.0   02 Oct 1996 12:58:58   JBELDEN
// Initial revision.
//************************************************************************

#include "platform.h"
#include <stdio.h>
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"

#include "ctsn.h"
#include "virscan.h"

#include "navutil.h"
#include "options.h"
#include "syminteg.h"
#include "nonav.h"

#include "navdprot.h"
#include "scand.h"
#ifdef USE_NETWORKALERTS   //&?
 #include "netalert.h"
#endif

extern   FILE   *fLogFile;
extern   BOOL   bAutoExportLog;

//************************************************************************
// RespondToVirus()
//
// This routine decides what action to take when a virus is found by
// the scanner depending on the Scanner Options settings.
//
// Parameters:
//      LPNONAV     lpNav               The present scan's information
//
// Returns:
//      Nothing
//************************************************************************
// 3/28/93 DALLEE Function created.
//************************************************************************

VOID PASCAL RespondToVirus (LPNONAV lpNoNav)
{

extern char IDS_FILENOTREPAIRED[];
extern char IDS_FILEREPAIRED[];
extern char IDS_FILEDELETED[];
extern char IDS_FILENOTDELETED[];


#ifndef NAVSCAN
    auto    LOGSTRUCT   Log;
#ifdef USE_NETWORKALERTS   //&?
    auto    NETALERT    rAlert;
#endif
#endif  // !NAVSCAN

                                        // VirusIndex = -1 is error
    if ( (lpNoNav->Infected.uVirusSigIndex != 0) &&
         (lpNoNav->Infected.uVirusSigIndex != (WORD) -1) )
        {
        if (lpNoNav->Alert.bBeep && lpNoNav->Opt.bPrestoNotify)
            {
            MUS4_BEEP(BEEP_KNOWNBOX);
            }
        lpNoNav->Stats.Files.uInfected++;

        ErrorLevelSet ( ERRORLEVEL_VIRUSFOUND ) ;

#ifndef NAVSCAN
#ifdef USE_NETWORKALERTS   //&?
        MEMSET(&rAlert, '\0', sizeof(rAlert));
        rAlert.lpNet = lpNoNav->lpNetUserItem;
        rAlert.lpInfected = &lpNoNav->Infected;
        rAlert.lpAlertOpt = &lpNoNav->Alert;
        rAlert.uTypeOfEvent = EV_ALERT_KNOWN_VIRUS;

        NetAlert(&rAlert);
#endif

                                        // Switch on what type of action to
                                        // take for known virus.
        switch (lpNoNav->Opt.uAction)
            {

            case SCAN_REPAUTO:
                ScanRepairFile(lpNoNav, &lpNoNav->Infected);
                switch(lpNoNav->Infected.wStatus)
                  {
                  case  FILESTATUS_REPAIRED:
                        printf(IDS_FILEREPAIRED, lpNoNav->Infected.szFullPath);
                        if (bAutoExportLog == TRUE)
                           fprintf(fLogFile, IDS_FILEREPAIRED, lpNoNav->Infected.szFullPath);
                        break;
                  case  FILESTATUS_NOTREPAIRED:
                        printf(IDS_FILENOTREPAIRED, lpNoNav->Infected.szFullPath);
                        if (bAutoExportLog == TRUE)
                           fprintf(fLogFile, IDS_FILENOTREPAIRED, lpNoNav->Infected.szFullPath);
                        break;
                  }
                break;

            case SCAN_DELAUTO:
                DeleteFile(lpNoNav, &lpNoNav->Infected);
                switch(lpNoNav->Infected.wStatus)
                  {
                  case  FILESTATUS_DELETED:
                        printf(IDS_FILEDELETED, lpNoNav->Infected.szFullPath);
                        if (bAutoExportLog == TRUE)
                           fprintf(fLogFile, IDS_FILEDELETED, lpNoNav->Infected.szFullPath);
                        break;
                  case  FILESTATUS_NOTDELETED:
                        printf(IDS_FILENOTDELETED, lpNoNav->Infected.szFullPath);
                        if (bAutoExportLog == TRUE)
                           fprintf(fLogFile, IDS_FILENOTDELETED, lpNoNav->Infected.szFullPath);
                        break;
                  }
                break;


            case SCAN_HALTCPU:
                if (NOERR == NAVDWriteLog(lpNoNav))
                    {
                    MEMSET(&Log, '\0', sizeof(Log));
                    Log.uEvent      = LOGEV_HALTCPU;
                    Log.lpFileName  = lpNoNav->Infected.szFullPath;
                    Log.lpVirusName = lpNoNav->Infected.Notes.lpVirName;
                    Log.lpAct       = &lpNoNav->Act;
                    LogEvent(&Log);
                    }
                ScanHalt(lpNoNav);
                break;
            } // End switch(lpNoNav->...
#endif  // !NAVSCAN

        AddVirusInfo(lpNoNav);
        }

} // End RespondToVirus()

//************************************************************************
// RespondToVirusInZip()
//
// This routine decides what action to take when a virus is found by
// the scanner in a ZIP file depending on the Scanner Options settings.
//
// Parameters:
//      LPNONAV     lpNav               The present scan's information
//
// Returns:
//      Nothing
//************************************************************************
// 7/13/93 DALLEE Function created.
// 8/12/93 DALLEE, Simplify filename creation.  Now no longer storing
//              paths internal to the zip file.
// 9/18/93 BarryG Handle HALT.  NOTE:  This function should be merged with
//                RespondToVirus() to avoid inconsistencies.
//************************************************************************

VOID PASCAL RespondToVirusInZip (LPNONAV lpNoNav)
{
    extern  BOOL        gbContinueScan;
#ifndef NAVSCAN
    auto    LOGSTRUCT   Log;
#ifdef USE_NETWORKALERTS
    auto    NETALERT rAlert;
#endif
#endif  // !NAVSCAN
                                        // ------------------------------
                                        // Get the filename in the form
                                        // "C:\DIR\FILE.ZIP#INFECTED.FIL"
                                        // ------------------------------
    STRCPY(lpNoNav->Infected.szFullPath, lpNoNav->Zip.lpZipFileName);
    StringAppendChar(lpNoNav->Infected.szFullPath, ZIP_SEPARATOR);
    STRCAT(lpNoNav->Infected.szFullPath, lpNoNav->Zip.szZipChildFile);

                                        // VirusIndex = -1 is error
    if ( (lpNoNav->Infected.uVirusSigIndex != 0) &&
         (lpNoNav->Infected.uVirusSigIndex != (WORD) -1) )
        {
        lpNoNav->Infected.wStatus = FILESTATUS_ZIPINFECTED;

        if (lpNoNav->Alert.bBeep && lpNoNav->Opt.bPrestoNotify)
            {
            MUS4_BEEP(BEEP_KNOWNBOX);
            }
        lpNoNav->Stats.Files.uInfected++;

        ErrorLevelSet ( ERRORLEVEL_VIRUSFOUND ) ;

#ifndef NAVSCAN
#ifdef USE_NETWORKALERTS
        MEMSET(&rAlert, '\0', sizeof(rAlert));
        rAlert.lpNet = lpNoNav->lpNetUserItem;
        rAlert.lpInfected = &lpNoNav->Infected;
        rAlert.lpAlertOpt = &lpNoNav->Alert;
        rAlert.uTypeOfEvent = EV_ALERT_KNOWN_VIRUS;

        NetAlert(&rAlert);
#endif

        switch ( lpNoNav->Opt.uAction )
            {
            case SCAN_PROMPT:
            case SCAN_REPAUTO:
            case SCAN_DELAUTO:
            case SCAN_REPORT:
                break ;

            case SCAN_HALTCPU:
                // Can't act now because ZIP engine must cleanup.
                // So, force an exit for ZIP, and let main loop actually
                // do the halt (everything else preparing for it is done
                // here).

                gbContinueScan = FALSE; // Force an exit from ZIP

                                        // Save log information
                                        // (including this new entry)
                if (NOERR == NAVDWriteLog(lpNoNav))
                    {
                    MEMSET(&Log, '\0', sizeof(Log));
                    Log.uEvent      = LOGEV_HALTCPU;
                    Log.lpFileName  = lpNoNav->Infected.szFullPath;
                    Log.lpVirusName = lpNoNav->Infected.Notes.lpVirName;
                    Log.lpAct       = &lpNoNav->Act;
                    LogEvent(&Log);
                    }

                // Last step (ScanHalt) is done when exiting ZipProcessFile()
                break ;
            }
#endif  // !NAVSCAN

        AddVirusInfo(lpNoNav);
        }

} //n End RespondToVirusInZip()

