////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "InterfaceHelper.h"
#include "AVSubmissionInterface.h"

namespace AVSubmit {

	static const TCHAR* const sz_AVSubmit_dll = _T("AVSubmit.dll");
	struct AVSubmit_dll { static const TCHAR* GetName() {return sz_AVSubmit_dll;} };

	//managed loader
	typedef CMgdInterfaceHelperStatic<AVSubmit_dll,
									ccSym::CNAVPathProvider,
									cc::CSymInterfaceTrustedCacheMgdLoader,
									IAVSubmissionManager,
									&IID_AVSubmissionManager,
									&IID_AVSubmissionManager> AVSubmit_IAVSubmissionManagerMgd;

	//unmanaged
	struct AVSubmit_IAVSubmissionManager : 
		CInterfaceHelper<ccSym::CNAVPathProvider, IAVSubmissionManager, &IID_AVSubmissionManager, &IID_AVSubmissionManager>
	{
		virtual LPCTSTR GetDLLName() throw() {return sz_AVSubmit_dll;}
	};

}

