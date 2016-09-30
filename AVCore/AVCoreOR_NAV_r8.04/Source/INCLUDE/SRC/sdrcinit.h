/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/sdrcinit.h_v   1.4   09 Jun 1998 16:32:10   mdunn  $
/////////////////////////////////////////////////////////////////////////////
//
// QResInit.h - contains exported functions from QConRes.DLL
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/INCLUDE/VCS/sdrcinit.h_v  $
// 
//    Rev 1.4   09 Jun 1998 16:32:10   mdunn
// Changed the function names to not be mangled for intel.
// 
// 
//    Rev 1.3   09 Jun 1998 16:12:06   mdunn
// Changed the function export to be extern c
// 
//    Rev 1.2   08 Jun 1998 20:40:46   jtaylor
// Added a string for the dll name.
//
//    Rev 1.1   08 Jun 1998 20:18:50   jtaylor
// Added string for the function name for dynamic loading.
//
//    Rev 1.0   02 Jun 1998 18:34:34   jtaylor
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _SDRCINIT_H
#define _SDRCINIT_H

extern "C" 
    {
    // Call this function to force loading of the resource DLL
    BOOL _declspec( dllimport ) InitScanDeliverResources();
    }

#define szInitScanDeliverResources   "InitScanDeliverResources"

#define szDLLName                    "scandres.dll"

#endif


