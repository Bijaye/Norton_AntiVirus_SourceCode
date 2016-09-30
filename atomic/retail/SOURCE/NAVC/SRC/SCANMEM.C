// Copyright 1993-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVC/VCS/scanmem.c_v   1.2   19 Oct 1998 15:05:32   JBELDEN  $
//
// Description:
//      These are the DOS NAV memory scanning functions.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/NAVC/VCS/scanmem.c_v  $
// 
//    Rev 1.2   19 Oct 1998 15:05:32   JBELDEN
// Changed code for the video mode switch when a 
// virus is detected in memory.
// 
// 
//    Rev 1.1   11 May 1998 16:45:14   jbelden
// Added changes to switch to text mode if a virus is found in
// memory so that the end user can see the message if the
// Windows logo screen was otherwise being displayed.
// 
//    Rev 1.0   06 Feb 1997 20:56:24   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:20:10   MKEATIN
// Initial revision.
// 
//    Rev 1.3   27 Nov 1996 09:36:48   JBELDEN
// added logging 
// 
//    Rev 1.2   27 Nov 1996 09:02:06   JBELDEN
// added message when virus found in mem. and changed errorlevel
// to _MEMVIRUSFOUND
// 
//    Rev 1.1   04 Oct 1996 17:06:28   JBELDEN
// removed some dialog code since not used.
// 
//    Rev 1.0   02 Oct 1996 12:58:52   JBELDEN
// Initial revision.
//************************************************************************

#include "platform.h"
#include <stdio.h>
#include "xapi.h"
#include "stddos.h"
#include "stddlg.h"

#include "ctsn.h"
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


//************************************************************************
// #DEFINES
//************************************************************************


//************************************************************************
// ScanMemory()
//
// This routine scans for viruses in memory.
//
// Parameters:
//      LPNONAV     lpNoNav             Pointer to info for this scan.
//
// Returns:
//      Nothing
//************************************************************************
// 5/27/93 DALLEE, Function created.
//************************************************************************

VOID PASCAL ScanMemory (LPNONAV lpNoNav)
{
    extern  char  szMemoryInfected[];
    extern  FILE  *fLogFile;
    extern  BOOL  bAutoExportLog;
    auto    UINT            uVirusIndex;
    auto    LOGSTRUCT       Log;
    auto    char            szLocalVirus [SIZE_VIRUSNAME + 1];
#ifndef NAVSCAN
#ifdef USE_NETWORKALERTS   //&?
    auto    NETALERT        rAlert;
#endif
#endif  // !NAVSCAN


    if (0 != (uVirusIndex = VirusScanMemory(&lpNoNav->Infected.Notes,
                                            lpNoNav->Gen.bScanHighMemory)))
        {
        // Change to text mode so that user can see the message
        // if the Windows logo, in graphics mode, was being displayed.
         _asm
            {
            mov     ax, 0003h
            int     10h
            }

        lpNoNav->Infected.uVirusSigIndex = uVirusIndex;
        lpNoNav->Infected.wStatus = FILESTATUS_MEMORYINFECTED;
        lpNoNav->Infected.szFullPath[0] = EOS;
        printf(szMemoryInfected, lpNoNav->Infected.Notes.lpVirName);
        if ( bAutoExportLog )
            {
            fprintf(fLogFile, szMemoryInfected, lpNoNav->Infected.Notes.lpVirName);
            fflush(fLogFile);
            }

                                        // Do Alerts and log system halted.
        if (lpNoNav->Alert.bBeep)
            {
            MUS4_BEEP(BEEP_KNOWNBOX);
            }

#ifndef NAVSCAN
#ifdef USE_NETWORKALERTS   //&?
        MEMSET(&rAlert, '\0', sizeof(rAlert));
        rAlert.lpNet = lpNoNav->lpNetUserItem;
        rAlert.lpInfected = &lpNoNav->Infected;
        rAlert.lpAlertOpt = &lpNoNav->Alert;
        rAlert.uTypeOfEvent = EV_ALERT_KNOWN_VIRUS;

        NetAlert(&rAlert);
#endif
#endif  // !NAVSCAN

        ErrorLevelSet ( ERRORLEVEL_MEMVIRUSFOUND ) ;

        StringMaxCopy( szLocalVirus,
                       lpNoNav->Infected.Notes.lpVirName,
                       SIZE_VIRUSNAME );

        MEMSET(&Log, '\0', sizeof(Log));
        Log.uEvent      = LOGEV_HALTCPU;;
        Log.lpFileName  = lpNoNav->Infected.szFullPath;
        Log.lpVirusName = szLocalVirus;
        Log.lpAct       = &lpNoNav->Act;

        LogEvent(&Log);


        ScanHalt(lpNoNav);
        }


} // End ScanMemory()

