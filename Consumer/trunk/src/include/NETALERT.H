////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/netalert.h_v   1.2   25 Apr 1997 17:47:22   JBRENNA  $
//
// Description:
//      Header for NAV network alerts.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/netalert.h_v  $
// 
//    Rev 1.2   25 Apr 1997 17:47:22   JBRENNA
// Use EXTERNC rather than extern "C" for more portability.
// 
//    Rev 1.1   24 Apr 1997 11:29:52   JBRENNA
// Tesla port: Add prototype for C++ version of NetAlert(). Note that the
// C++ NetAlert() is only available in the NAVWORK project. Hence the
// NAV_IN_NAVWORK #ifdef. See below for more info.
// 
//    Rev 1.0   06 Feb 1997 20:51:18   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:47:28   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:11:52   BARRY
// Initial revision.
// 
//    Rev 1.0   31 Oct 1994 12:49:52   MFALLEN
// Initial revision.
// 
//    Rev 1.6   10 Mar 1994 22:58:52   DALLEE
// Added network alert message for registration changes.
//************************************************************************

#ifndef _NETALERT
#define _NETALERT
/*
enum    {
        EV_ALERT_INOC_CHANGE = 1,
        EV_ALERT_END_SCAN,
        EV_ALERT_VIRLIST_CHANGE,
        EV_ALERT_KNOWN_VIRUS,
        EV_ALERT_SYSTEMINOC_MBOOT_CHANGE,
        EV_ALERT_SYSTEMINOC_BOOT_CHANGE
        };
*/
#define EV_ALERT_INOC_CHANGE    1
#define EV_ALERT_END_SCAN       2
#define EV_ALERT_VIRLIST_CHANGE 3
#define EV_ALERT_KNOWN_VIRUS    4
#define EV_ALERT_SYSTEMINOC_MBOOT_CHANGE    5
#define EV_ALERT_SYSTEMINOC_BOOT_CHANGE     6
#ifdef BANKERS
  #define EV_ALERT_REG_CHANGE   7
#endif  // BANKERS

typedef struct tagNETALERTSTRUCT
{
    LPNETUSERITEM    lpNet;         // Array of users/groups to alert
    LPINFECTEDFILE   lpInfected;    // information about the infected file
    LPALERTOPT       lpAlertOpt;    // Alert Options
    UINT             uTypeOfEvent;  // Use one of the EV_ALRT_xxx
} NETALERT, FAR * LPNETALERT;

// JBRENNA: 4/24/97: The NAVWORK project defines its own C++ version of 
// NetAlert in N_ALERT.CPP. If we are in the NAVWORK project, the 
// NAV_IN_NAVWORK symbol is defined. This varies from the exact TESLA
// implementation in that the NetAlert prototype existed in NAVWORK:stub.h.
// Since it does not belong in STUB.H, I move it here.
//
// Note that normally NetAlert is defined in the NAVSERVE project. Also,
// remember that there are two implementations of NetAlert in our codebase.

#if defined(NAV_IN_NAVWORK)
    VOID WINAPI NetAlert(LPNETALERT  lpNetAlert);
#else
    EXTERNC VOID SYM_EXPORT WINAPI NetAlert (LPNETALERT lpNetAlert);
#endif

#endif
