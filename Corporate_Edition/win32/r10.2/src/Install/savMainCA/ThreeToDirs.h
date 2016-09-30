// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#include "resource.h"		// main symbols

// Internal functions
CString GetParam( CString * );
BOOL ToSAV( MSIHANDLE, CString , CString );
BOOL ToApp( MSIHANDLE, CString , CString );
BOOL ToLU( MSIHANDLE, CString , CString );

// MSI Calls
UINT __stdcall ProcessToDirs( MSIHANDLE );
