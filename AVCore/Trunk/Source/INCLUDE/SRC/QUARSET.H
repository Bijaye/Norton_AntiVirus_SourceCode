// Copyright 1992-1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/Quarset.h_v   1.4   10 Jun 1997 11:08:44   DBUCHES  $
//
// Description:
//      This include file defines the Quarantine options structure.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/Quarset.h_v  $
// 
//    Rev 1.4   10 Jun 1997 11:08:44   DBUCHES
// Added 1 byte packing pragma.
// 
//    Rev 1.3   14 May 1997 20:42:24   TIVANOV
// remov the activity log byte field from the structure
// 
//    Rev 1.2   14 May 1997 10:45:08   TIVANOV
// change the dir buffer to 260
// 
//    Rev 1.1   25 Apr 1997 20:54:06   JBRENNA
// Change MAX_PATH to SYM_MAX_PATH for DOS support.
// 
//    Rev 1.0   25 Apr 1997 20:48:32   JBRENNA
// Initial revision.
//
//************************************************************************

#ifndef QUARSET_H_INCLUDED
#define QUARSET_H_INCLUDED



#ifdef SYM_WIN
#pragma pack(push, 1)
#endif

typedef struct tagQuarantineset
{
    BYTE    bQuarantineAP;                  // Auto-protected
    BYTE    bQuarantineScan;                // Scan
    BYTE    bQuarantineInoc;                // Inoculate

    char   szQuarantineDir[260];  // Quarantine path
}  QUARANTINESET, *PQUARANTINESET, FAR * LPQUARANTINESET;

#ifdef SYM_WIN
#pragma pack(pop)
#endif


#endif // QUARSET_H_INCLUDED
