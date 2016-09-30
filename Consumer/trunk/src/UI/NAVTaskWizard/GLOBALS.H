////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef AFX_GLOBALS_H__5E100C81_FD65_11d2_B9D2_00C04FAC114C__INCLUDED_
#define AFX_GLOBALS_H__5E100C81_FD65_11d2_B9D2_00C04FAC114C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "resource.h"
#include "..\\NavTaskWizardRes\\resource.h"

//CScanTask*     AllocateTask ( const CScanTaskInfo* pTaskInfo );
//void           GetDescriptionStringFromType ( const ETaskType eType,
//                                             CString&        sDescription );
int            GetImageIndexFromType ( long lType );
//BOOL           LaunchScanner ( CWnd* pParentWnd, LPCTSTR szParams );
//BOOL           LoadAutoProtect();
//const CString& MakeFullPathToTask ( CString& sPath, LPCTSTR szTaskName );
//LPCTSTR        MakeFullPathToTask ( LPTSTR szPath, LPCTSTR szTaskName );
//CString        PackSystemtime ( const SYSTEMTIME* pSystime );
BOOL           RecycleFile ( CWnd* pWnd, LPCTSTR szFilename );
//int            RunNavApp ( CWnd* pParentWnd, LPCTSTR szFilename );
//int            RunNavApp ( HWND hwndParent, LPCTSTR szFilename );
BOOL           ShouldConfirmDelete();
//void           TalkToVxD ( VXDACTION wVxDAction );
int CALLBACK   TaskItemSortCallback ( LPARAM p1, LPARAM p2, LPARAM p3 );
//BOOL           UnpackSystemTime ( LPCTSTR szPacked, SYSTEMTIME* pSystime );

#endif  // def AFX_GLOBALS_H__5E100C81_FD65_11d2_B9D2_00C04FAC114C__INCLUDED_
