////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//
// SnapshotServerLoader.h
//

#pragma once

#include <InterfaceHelper.h>
#include <SnapServerSX_i.h>
#include <ccSymPathProvider.h>

// ISnapServer loader
class ss_ISnapServer :
	public CInterfaceHelper
	<
		ccSym::CNAVPathProvider,
        SnapServerSX::ISnapServer,
		&SnapServerSX::IID_ISnapServer,
		&SnapServerSX::IID_ISnapServer
	>
{
	public:
	LPCTSTR GetDLLName(void) throw() { return _T("SDSnapSX.dll"); }
};
