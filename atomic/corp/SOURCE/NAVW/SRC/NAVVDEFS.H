// Copyright 1997 Symantec Corporation
//************************************************************************
// $Header:   S:/NAVW/VCS/navvdefs.h_v   1.0   04 Jun 1997 15:17:02   JBRENNA  $
//
// Description:
//   This header file contains the Virus Definition Init and DeInit
//   functionality. These routines encapsulate the CDefUtils management
//   for the NAVW AppId.
//
//************************************************************************
// $Log:   S:/NAVW/VCS/navvdefs.h_v  $
// 
//    Rev 1.0   04 Jun 1997 15:17:02   JBRENNA
// Initial revision.
// 
//************************************************************************

#ifndef __NAVVDEFS_H
#define __NAVVDEFS_H

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////
// Description:
//   Initialize VirScan and our local CDefUtils class.
//
// Returns:
//   NOERR      Init successful.
//   ERR        Init failed.
//
//////////////////////////////////////////////////////////////////////////
UINT NavVDefsVirusScanInit();

//////////////////////////////////////////////////////////////////////////
// Description:
//   DeInitialize VirScan and our local CDefUtils class.
//
// Returns:
//   TRUE   success.
//   FALSE  failure.
//////////////////////////////////////////////////////////////////////////
BOOL NavVDefsVirusScanDeInit();

#ifdef __cplusplus
}
#endif

#endif // __NAVVDEFS_H