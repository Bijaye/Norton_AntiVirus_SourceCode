// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ScanUIInterfaces.h>
#include <AVInterfaceLoader.h>
#include <AVInterfaces.h>
#include <AVDefines.h>
#include <SymTheme.h>
#include "mainfrm.h"
#include "ThreatTracker.h"
#include "avScanUIInternalInterfaces.h"

namespace avScanUI
{
// CScanUIMisc
class CScanUIMisc :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public avScanUI::IScanUIMisc
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(avScanUI::IID_ScanUIMisc, avScanUI::IScanUIMisc)
    SYM_INTERFACE_MAP_END()

public:
    CScanUIMisc(void);
    virtual ~CScanUIMisc(void);

    // IScanUIMisc methods:
    virtual HRESULT Initialize() throw();
    virtual HRESULT DisplayVIDDetails(HWND hParentWnd, DWORD dwVID) throw();
    virtual HRESULT DisplayThreatDetails(HWND hParentWnd, const GUID& idThreat) throw();
    virtual HRESULT ProcessSingleThreat(HWND hParentWnd, const GUID& idThreat) throw();
    virtual HRESULT QuarantineAdd(HWND hParentWnd, GUID& idThreat) throw();
    virtual HRESULT QuarantineRestore(HWND hParentWnd, const GUID& idThreat) throw();
    virtual eRebootPromptResult QueryUserForReboot(HWND hParentWnd) throw();
    virtual eProcTermResult QueryUserForProcTerm(HWND hParentWnd) throw();

protected:
    // No protected methods

protected:
    AVModule::AVLoader_IAVThreatInfo m_ldrThreatInfo;
};

} // namespace