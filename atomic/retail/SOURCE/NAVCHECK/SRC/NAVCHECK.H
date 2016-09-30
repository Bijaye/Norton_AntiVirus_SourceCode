// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVCHECK/VCS/navcheck.h_v   1.0   18 Aug 1997 21:15:16   TCASHIN  $
//
// Description:
//      This include file contains the prototypes, constants and typedefs
//      for NAVCHECK.
//************************************************************************
// $Log:   S:/NAVCHECK/VCS/navcheck.h_v  $
// 
//    Rev 1.0   18 Aug 1997 21:15:16   TCASHIN
// Initial revision.
// 
//************************************************************************

#ifndef __NAVCHECK_H
#define __NAVCHECK_H


//************************************************************************
// DEFINES
//************************************************************************
#define NAVCHK_ERR_CMD      1
#define NAVCHK_ERR_W16      2
#define NAVCHK_ERR_W32      3   

#define NAVCHK_PLAT_W16     16
#define NAVCHK_PLAT_W32     32

#define NAVCHK_16BIT        1
#define NAVCHK_32BIT        2
#define NAVCHK_UNKNOWN      3



//************************************************************************
// STRUCTS
//************************************************************************

typedef struct tagEXESTARTINFO
    {
    char                byHelp;
    char                byNoupdate;
    char                szW16CmdLine [SYM_MAX_PATH];
    char                szW16ExeFile [SYM_MAX_PATH];
    char                szW16NNSFile [SYM_MAX_PATH];
    char                szW32CmdLine [SYM_MAX_PATH];
    char                szW32ExeFile [SYM_MAX_PATH];
    char                szW32NNSFile [SYM_MAX_PATH];
    BYTE                byRFU2       [SYM_MAX_PATH];
    } EXESTARTINFO, FAR *LPEXESTARTINFO;


#endif  //  __NAVCHECK_H
