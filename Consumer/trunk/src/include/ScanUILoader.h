////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <InterfaceHelper.h>
#include <ScanUIInterfaces.h>
#include <ccSymPathProvider.h>

namespace avScanUI
{
// IScanUI loader
class sui_IScanUI :
	public CInterfaceHelper
	<
		ccSym::CNAVPathProvider,
        IScanUI,
		&SYMOBJECT_AVSCANUI,
		&IID_ScanUI
	>
{
	public:
	LPCTSTR GetDLLName(void) throw() { return _T("avScanUI.dll"); }
};

// IEmailScanUI loader
class sui_IEmailScanUI :
	public CInterfaceHelper
	<
		ccSym::CNAVPathProvider,
        IEmailScanUI,
		&SYMOBJECT_AVEMAILSCANUI,
		&IID_EmailScanUI
	>
{
	public:
	LPCTSTR GetDLLName(void) throw() { return _T("avScanUI.dll"); }
};

// IScanUIMisc loader
class sui_IScanUIMisc :
	public CInterfaceHelper
	<
		ccSym::CNAVPathProvider,
        IScanUIMisc,
		&SYMOBJECT_SCANUIMISC,
		&IID_ScanUIMisc
	>
{
	public:
	LPCTSTR GetDLLName(void) throw() { return _T("avScanUI.dll"); }
};

// ISingleInstance loader
class sui_ISingleInstance :
	public CInterfaceHelper
	<
		ccSym::CNAVPathProvider,
        ISingleInstance,
		&SYMOBJECT_SINGLEINSTANCE,
		&IID_SingleInstance
	>
{
	public:
	LPCTSTR GetDLLName(void) throw() { return _T("avScanUI.dll"); }
};

};	// namespace
