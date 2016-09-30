// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1998, 2005 Symantec, Peter Norton Product Group. All rights reserved.
//************************************************************************
//
// $Header:   S:/SDPACK/VCS/PSM.H_v   1.0   13 Mar 1998 12:51:32   DCHI  $
//
// Description:
//  Header file for presubmission data function.
//
//************************************************************************
// $Log:   S:/SDPACK/VCS/PSM.H_v  $
// 
//    Rev 1.0   13 Mar 1998 12:51:32   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _PSM_H_

#define _PSM_H_

BOOL PreSubmitDataFill
(
    LPTSTR              lpszFileName,
    LPBYTE              lpbySubmitBuffer,
    DWORD               dwSubmitBufferSize
);

#endif // #ifndef _PSM_H_

