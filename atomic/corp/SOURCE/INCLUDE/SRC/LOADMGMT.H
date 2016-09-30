// Copyright 1996 Symantec Corporation
//**********************************************************************
//
// $Header:   S:/include/VCS/loadmgmt.h_v   1.0   06 Feb 1997 20:52:00   RFULLER  $
//
// Description: Prototypes for loadfile.c
//
// See Also:
//
//**********************************************************************
// $Log:   S:/include/VCS/loadmgmt.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:52:00   RFULLER
// Initial revision
// 
//    Rev 1.0   16 Apr 1996 19:26:00   TSmith
// Initial revision.
//
//    Rev 1.0   15 Aug 1994 16:14:56   GDZIECI
// Baseline Mario from Vishnu with Win32 awk changes applied
//
//    Rev 1.6   04 Dec 1993 14:16:24   JMILLARD
// return error from InitServerLoadManagement
//
//    Rev 1.5   12 Apr 1993 21:52:00   JMILLARD
// Add finer load control parameters
// Add DeInitServerLoadManagement
//
//    Rev 1.4   09 Apr 1993 17:06:54   JMILLARD
// raise value of INFINITY
// add LOAD_INCREMENT
//
//    Rev 1.3   09 Apr 1993 13:36:32   JMILLARD
// change HLOAD to LPLOAD
//
//    Rev 1.2   09 Apr 1993 12:16:28   JMILLARD
// in progress
//
//    Rev 1.1   07 Apr 1993 15:01:32   JMILLARD
// remove residue for now
//
//    Rev 1.0   07 Apr 1993 09:29:00   JMILLARD
// Initial revision.
//
/************************************************************************/
#if !defined(_LOADMGMT_H)
#define _LOADMGMT_H

//  .h files required to include this .h file

#if !defined(INCL_PLATFORM)
#error platform.h MUST be included before loadmgmt.h
#endif

//**********************************************************************
//
//  defines
//
//**********************************************************************

// dwCurrentDelay defines - units are number of thread switches

#define INFINITY				2000
#define NO_DELAY				1
#define LOAD_INCREMENT			5
#define MAX_SPIN				5
#define DELAY_CYCLE				18
#define LOAD_SCALE				100
#define MAX_SPIN_BREAKPOINT		((MAX_SPIN-1)*DELAY_CYCLE+1)


//**********************************************************************
//
//  Typedefs
//
//**********************************************************************

//  This structure defines the load management structures

// struct tagLOADMANAGEMENT;				// incomplete LOADMANAGEMENT so I can typedef
                                        // PLOADMANAGEMENT

// typedef struct tagLOADMANAGEMENT FAR *LPLOADMANAGEMENT;
typedef struct tagLOADMANAGEMENT            // complete declaration
    {
    DWORD       dwStructSize;               // size of basic structure

#if defined( SYM_NLM )
    tagLOADMANAGEMENT *lpPreviousBackList;    // previous event in list
    tagLOADMANAGEMENT *lpNextBackList;        // next event in list
#endif

    DWORD       dwTargetLoad;               // maximum desired load
    DWORD       dwCurrentDelay;             // current delay to achieve
                                            // desired load

    // hierarchical delay chain to allow fine tuning of delay
    // the delays are processed in the order given, processing those
    // with non-zero counts each time

    WORD        wTick1DelayCount;           // # times to do this
    WORD        wTick1Delay;                // Delay each time

    WORD        wTick2DelayCount;           // # times to do this
    WORD        wTick2Delay;                // Delay each time

    WORD        wThreadSwitch1Count;        // # times to do this
    WORD        wThreadSwitch1;             // Delay each time

    WORD        wThreadSwitch2;             // Default delay if nothing else

    BOOL        bLoadManagementInitialized; //

    } LOADMANAGEMENT, *LPLOADMANAGEMENT;


//**********************************************************************
//
//  Prototypes
//
//**********************************************************************

BOOL InitServerLoadManagement( LPLOADMANAGEMENT lpLoadManagement,
                               DWORD dwTargetServerLoad );

WORD UpdateServerLoad(
#if defined( SYM_WIN32 )
	LPLOADMANAGEMENT lpLoad
#else
	VOID
#endif
);

VOID ServerLoadManagementDelay( LPLOADMANAGEMENT lpLoadManagement );

VOID DeInitServerLoadManagement( LPLOADMANAGEMENT lpLoadManagement );

#endif  // _LOADMGMT_H
