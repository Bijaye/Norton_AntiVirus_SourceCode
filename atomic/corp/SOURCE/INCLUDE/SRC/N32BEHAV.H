////////////////////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/n32behav.h_v   1.3   03 Sep 1998 12:34:38   rchinta  $
////////////////////////////////////////////////////////////////////////////////////
//
//  N32BEHAV - Contains behavior flags and exported functions for
//             N32BEHAV.DLL
//
//
//
//
//
////////////////////////////////////////////////////////////////////////////////////
// $Log:   S:/INCLUDE/VCS/n32behav.h_v  $
// 
//    Rev 1.3   03 Sep 1998 12:34:38   rchinta
// Changed the OEM behaviour definition (from SCAN_ONLY to OEM)
// and added an additional macro.
// 
//    Rev 1.2   24 Jun 1998 12:13:34   jtaylor
// Added Quarantine Behavior.
//
//    Rev 1.1   04 Nov 1997 00:21:38   jtaylor
// Added ScannerOnly attribute to the flags.
//
//    Rev 1.0   23 May 1997 12:44:52   DBUCHES
// Initial revision.
////////////////////////////////////////////////////////////////////////////////////

#ifndef N32BEHAVE_H_
#define N32BEHAVE_H_

////////////////////////////////////////////////////////////////////////////////////
// Behavior Flag IDs.  Note that 0 is reserved for the array terminator.
////////////////////////////////////////////////////////////////////////////////////

#define BEHAVOIR_RESERVED       0
#define BEHAVIOR_SERVER         1
#define BEHAVIOR_WORKSTATION    2
#define BEHAVIOR_TRIAL_VERSION  3
#define BEHAVIOR_SCAN_ONLY      4
#define BEHAVIOR_QUARANTINE     5
#define BEHAVIOR_OEM            6


////////////////////////////////////////////////////////////////////////////////////
// Exports
////////////////////////////////////////////////////////////////////////////////////
#ifdef BUILDING_N32BEHAV
#define DECLSPEC_N32BEHAV __declspec( dllexport )
#else
#define DECLSPEC_N32BEHAV __declspec( dllimport )
#endif

BOOL DECLSPEC_N32BEHAV GetBehavior(DWORD dwBehavior, DWORD dwExtra);

////////////////////////////////////////////////////////////////////////////////////
// Macros.
////////////////////////////////////////////////////////////////////////////////////
#define NAV_IS_SERVER()(GetBehavior(BEHAVIOR_SERVER, 0))
#define NAV_IS_WORKSTATION()(GetBehavior(BEHAVIOR_WORKSTATION, 0))
#define NAV_IS_TRIALVERSION()(GetBehavior(BEHAVIOR_TRIAL_VERSION, 0))
#define NAV_IS_SCANONLY()(GetBehavior(BEHAVIOR_SCAN_ONLY, 0))
#define NAV_IS_QUARANTINE()(GetBehavior(BEHAVIOR_QUARANTINE, 0))
#define NAV_IS_OEM()(GetBehavior(BEHAVIOR_OEM, 0))


////////////////////////////////////////////////////////////////////////////////////
// Flag ID array.  Note that this array is platform specific.
////////////////////////////////////////////////////////////////////////////////////

// Only defined when building DLL
#ifdef BUILDING_N32BEHAV
////////////////////////////////////////////////////////////////////////////////////

DWORD gFlags[] = {

// set server or workstation behavior
#if defined(NAV_SERVER)
                    BEHAVIOR_SERVER,
#else
                    BEHAVIOR_WORKSTATION,
#endif // NAV_SERVER

// Set OEM/non-OEM behaviors
#if defined(NAV_OEM)
                    BEHAVIOR_OEM,
#endif // NAV_OEM
                    BEHAVIOR_QUARANTINE,


                    0 // Must be zero terminated
    };



////////////////////////////////////////////////////////////////////////////////////
#endif // BUILDING_N32BEHAV

////////////////////////////////////////////////////////////////////////////////////
#endif // N32BEHAVE_H_
