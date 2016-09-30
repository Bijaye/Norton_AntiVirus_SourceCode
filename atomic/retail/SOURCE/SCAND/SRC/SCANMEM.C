// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/scand/VCS/scanmem.c_v   1.0   06 Feb 1997 21:08:44   RFULLER  $
//
// Description:
//      These are the DOS NAV memory scanning functions.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/scand/VCS/scanmem.c_v  $
// 
//    Rev 1.0   06 Feb 1997 21:08:44   RFULLER
// Initial revision
// 
//    Rev 1.2   08 Aug 1996 15:40:06   JBRENNA
// LuigiPlus Port:
//   JWORDEN: Pass file name in Infected.szFullOemPath to match shared
//     functions in NAVXUTIL
// 
// 
//    Rev 1.1   08 Aug 1996 13:26:22   JBRENNA
// Ported from DOSNAV environment
// 
// 
//    Rev 1.23   13 Jun 1996 14:04:38   JALLEE
// #if USE_NETWORKALERTS changed to #ifdef USE_NETWORKALERTS.
// 
//    Rev 1.22   21 Jun 1995 11:41:42   SZIADEH
// activated mus4_beep()
// 
//    Rev 1.21   14 Mar 1995 17:46:00   DALLEE
// Hide display during startup util problems are found.
// 
//    Rev 1.20   29 Dec 1994 16:46:48   DALLEE
// Include syminteg.h before nonav.h
// 
//    Rev 1.19   28 Dec 1994 14:15:52   DALLEE
// Latest NAVBOOT revision.
// 
    //    Rev 1.4   27 Dec 1994 19:36:28   DALLEE
    // Convert to use CERTLIBN.
    //
    //    Rev 1.3   27 Dec 1994 15:41:52   DALLEE
    // Commented out MUS4_BEEP.
    //
    //    Rev 1.2   12 Dec 1994 18:10:56   DALLEE
    // Commented out network alerts and password protection.
    //
    //    Rev 1.1   05 Dec 1994 18:10:22   DALLEE
    // CVT1 script.
    //
//    Rev 1.18   28 Dec 1994 13:53:24   DALLEE
// Basis for NAVBOOT code from NAV v3.05.
// First (.DX) port to trunk is previous version labeled "PRENAVBOOT".
// 
    //    Rev 1.0   23 Nov 1994 15:38:38   DALLEE
    // Initial revision.
    //
    //    Rev 1.12   08 Oct 1993 16:37:34   DALLEE
    // #ifdef'd net alerts, inoculation, and zip code from NAVSCAN builds.
//************************************************************************

#include "platform.h"
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
    extern  OutputFieldRec  ofScanMemoryBar;

    auto    UINT            uVirusIndex;
    auto    LOGSTRUCT       Log;
    auto    char            szLocalVirus [SIZE_VIRUSNAME + 1];
#ifndef NAVSCAN
#ifdef USE_NETWORKALERTS   //&?
    auto    NETALERT        rAlert;
#endif
#endif  // !NAVSCAN

    StdDlgStartWait();

    if (0 != (uVirusIndex = VirusScanMemory(&lpNoNav->Infected.Notes,
                                            lpNoNav->Gen.bScanHighMemory)))
        {
        lpNoNav->Infected.uVirusSigIndex = uVirusIndex;
        lpNoNav->Infected.wStatus = FILESTATUS_MEMORYINFECTED;
        lpNoNav->Infected.szFullPath[0] = EOS;
        lpNoNav->Infected.szFullOemPath[0] = EOS;

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

        ErrorLevelSet ( ERRORLEVEL_VIRUSFOUND ) ;

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

    StdDlgEndWait();

    ScanMemoryBootFillBar(&ofScanMemoryBar, FALSE);

} // End ScanMemory()

