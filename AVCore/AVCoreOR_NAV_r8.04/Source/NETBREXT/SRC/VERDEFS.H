/////////////////////////////////////////////////////////////////////
// VerDefs.h : header file
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/NETBREXT/VCS/verdefs.h_v   1.1   20 Aug 1998 10:48:10   TSmith  $
//
// Description:
//  Define statements to supplement version information
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/NETBREXT/VCS/verdefs.h_v  $
// 
//    Rev 1.1   20 Aug 1998 10:48:10   TSmith
// Name strings now Unicode aware.
// 
//    Rev 1.0   21 Nov 1997 11:52:18   KSACKIN
// Initial revision.
// 
//    Rev 1.0   18 Apr 1997 14:03:52   DSACKIN
// Ported from Tesla
// 
//    Rev 1.0   13 Mar 1997 15:16:40   RFULLER
// Initial revision
// 
//    Rev 1.0   04 Feb 1997 14:52:34   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////
#ifndef VERTEST_HEADER
#define VERTEST_HEADER

#ifdef UNICODE
    #define VR_ORIGINALFILENAME             "NetBrExU.DLL"
    #define VR_INTERNALNAME                 "NetBrExU.DLL"
#else
    #define VR_ORIGINALFILENAME             "NetBrExt.DLL"
    #define VR_INTERNALNAME                 "NetBrExt.DLL"
#endif    

#endif  // VERTEST_HEADER
