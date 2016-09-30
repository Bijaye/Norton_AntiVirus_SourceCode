// Copyright (C) 1998 Symantec Corporation
//*****************************************************************************
// $Header:   S:/INCLUDE/VCS/symtla.h_v   1.0   30 Jun 1998 18:25:12   TSmith  $
//
// Description: Public structures and definitions for SymTLA (Symantec Tivoli
//              Logfile Adapter).
//
//*****************************************************************************
// $Log:   S:/INCLUDE/VCS/symtla.h_v  $
// 
//    Rev 1.0   30 Jun 1998 18:25:12   TSmith
// Initial revision.
//*****************************************************************************
#if !defined( __SymTla_h__ )
#define       __SymTla_h__



typedef int ( WINAPI* SYMTLA_TLALOGADD )(
    LPVOID  lpRecord,
    BYTE    byRecType,
    BYTE    byProgType,
    DWORD   fdwFieldFlags,
    WORD    cbMaxRecSize,
    LPCTSTR lpFileLocation
    );



EXTERNC int SYM_EXPORT WINAPI TlaLogAdd(
    LPVOID  lpRecord,                   // Pointer to raw NAV activity log record.    
    BYTE    byRecType,                  // Flag indicating record type (see SYMALOG.H)
    BYTE    byProgType,                 // Flag indicating program type (see SYMALOG.H)
    DWORD   fdwFieldFlags,              // Field information flags (see SYMALOGI.H)
    WORD    cbMaxRecSize,               // Size of record in bytes
    LPCTSTR lpFileLocation              // Path to log file directory.
    );



#endif     // __SymTla_h__















