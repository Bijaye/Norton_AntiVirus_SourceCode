/////////////////////////////////////////////////////////////////////
// PROCESSSTATUS.H - Created - 05-26-97
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/processstatus.h_v   1.0   01 Jun 1997 19:54:18   DSACKIN  $
//
// Description:
//    This is where the process Status fields will go.  These will
//    be returned from the SymAgent when the call of GetRemoteProcessStatus
//    is issued.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/processstatus.h_v  $
// 
//    Rev 1.0   01 Jun 1997 19:54:18   DSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__PROCESSSTATUS_H)
#define __PROCESSSTATUS_H


#define    PROCESS_NOT_RUNNING   0
#define    PROCESS_RUNNING       1
#define    PROCESS_COMPLETED     2


#define    PROCESS_MIF_SUCCESSFUL    0x10
#define    PROCESS_MIF_UNSUCCESSFUL  0x11

#endif // if !defined(__PROCESSSTATUS_H)
