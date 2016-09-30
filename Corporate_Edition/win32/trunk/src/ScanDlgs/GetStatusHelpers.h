// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef __GETSTATUSHELPERS_H__
#define __GETSTATUSHELPERS_H__

//#ifdef _WINDOWS
//	#include <windows.h>
//#endif // WINDOWS
#include "stdafx.h"

// Prototypes for functions defined in GetStatusHelpers.cpp
#ifdef __cplusplus
extern "C" {
#endif

	BOOL IsLocalMachine(CString sRemoteComputer);
	BOOL WarnAndRestore();
	BOOL CreateMissingDirectory(LPCTSTR lpFullPath);

#ifdef __cplusplus
}
#endif
#endif // __GETSTATUSHELPERS_H__
