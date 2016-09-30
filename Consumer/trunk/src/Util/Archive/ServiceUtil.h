////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ServiceUtil.h: interface for the CServiceUtil class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "SCSmartHandle.h"

namespace NAVToolbox
{
class CServiceUtil  
{
public:
	static bool StartService( LPCTSTR szServiceName );
	static bool StopService( LPCTSTR szServiceName );
	static bool ChangeServicePath( LPCTSTR szServiceName, LPCTSTR szServicePath );
	static bool DeleteService ( LPCTSTR szServiceName );
	
private:
	static bool waitForState( SC_HANDLE hService, DWORD dwState );
};
} // end namespace NAVToolbox

