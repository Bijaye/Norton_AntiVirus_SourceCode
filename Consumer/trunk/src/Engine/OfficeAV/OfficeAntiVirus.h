////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Copyright 1999 Symantec, SABU, AV Core team
// -------------------------------------------------------------------------
//
// Author:  Danny Wong
// Date:    06/02/99
//
// OfficeAntiVirus.h : Declaration of the COfficeAntiVirus
//

#ifndef __OFFICEANTIVIRUS_H_
#define __OFFICEANTIVIRUS_H_

#include "resource.h"       // main symbols
#include "ResResource.h"       // main symbols
#include "Navwinterface.h"

/////////////////////////////////////////////////////////////////////////////
// COfficeAntiVirus
class ATL_NO_VTABLE COfficeAntiVirus : 
    public CComObjectRootEx<CComSingleThreadModel>,
    public CComCoClass<COfficeAntiVirus, &CLSID_OfficeAntiVirus>,
    public IOfficeAntiVirus
{
public:
    /////////////////////////////////////////////////////////////////////////////
    //  Constructor and Destructor
    COfficeAntiVirus();
    ~COfficeAntiVirus();

DECLARE_REGISTRY_RESOURCEID(IDR_OFFICEANTIVIRUS)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(COfficeAntiVirus)
    COM_INTERFACE_ENTRY(IOfficeAntiVirus)
END_COM_MAP()

public:
    /////////////////////////////////////////////////////////////////////////////
    //  IOfficeAntiVirus method(s)
    STDMETHOD(Scan)(MSOAVINFO* psoavinfo);

private:
	bool SafeDeleteFile(LPCWSTR szFileName);
	NAVWRESULT COfficeAntiVirus::ScanFile(LPCWSTR szFileName);
	bool IsFeatureEnabled();
};


	//-----------------------------------------------------------------------
// BEM - 9/17/96
// Symantec special wait functions that allow message processing.
// Use these instead of WaitForSingleObject() in an OLE/COM thread.
// See KB Article: OLE Threads Must Dispatch Messages
//-----------------------------------------------------------------------

// Wait for one of the given objects to be signaled, while allowing messages to be dispatched:
// Returns same as MsgWaitForMultipleObjects()
DWORD WINAPI SymMsgWaitMultiple(
    DWORD dwCount,                      // Number of handles in lpHandles
    LPHANDLE lpHandles,                 // Array of handles - wiat for a signal from
                                        // one of these.
    DWORD dwMilliseconds);              // Timeout value (use INFINITE for none)


// Helper inline cover for waiting on only a single handle:
    inline DWORD SymMsgWaitSingle(
        HANDLE hHandle,
        DWORD dwMilliseconds)
    {
        return SymMsgWaitMultiple(1, &hHandle, dwMilliseconds);
    }

#endif //__OFFICEANTIVIRUS_H_
