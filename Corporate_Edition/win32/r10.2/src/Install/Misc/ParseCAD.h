// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2004, 2005, Symantec Corporation, All rights reserved.
// ParseCAD.h

#ifndef __SYM_INSTALL_PARSE_CAD__
#define __SYM_INSTALL_PARSE_CAD__

#include <msi.h>

// ParseCAD
//
//  Helper function for parsing comma-delimited custom action data.
//
// @param hInstall  Handle to MSI.
// @param multipath Comma-delimited text.
// @param count     The expected number of strings to parse out.
// @param ...       Variable args are TCHAR*, DWORD pairs, where the DWORD specifies
//                  the length of the buffer pointed to by the TCHAR*
//
// @return DWORD -- The number of strings actually parsed.
#define DOUBLE_MAX_PATH			(MAX_PATH * 2)
DWORD ParseCAD ( MSIHANDLE hInstall, LPTSTR multipath, UINT count, ...  );

#endif // __SYM_INSTALL_PARSE_CAD__

