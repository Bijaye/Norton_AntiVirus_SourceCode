////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <ScanUIInterfaces.h>
#include <AVInterfaces.h>
#include <AVDefines.h>
#include <SymTheme.h>
#include "FrameDocument.h"
#include <resource.h>
#include "mainfrm.h"
#include "ThreatTracker.h"
#include "avScanUIInternalInterfaces.h"

namespace avScanUI
{
// CEmailScanUI
class CEmailScanUI :
    public ISymBaseImpl<CSymThreadSafeRefCount>,
    public avScanUI::IEmailScanUI
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(avScanUI::IID_EmailScanUI, avScanUI::IEmailScanUI)
    SYM_INTERFACE_MAP_END()

public:
    CEmailScanUI(void);
    virtual ~CEmailScanUI(void);

    // IEmailScanUI methods:
    virtual HRESULT Initialize() throw();
    virtual HRESULT DisplayThreatUI(AVModule::IAVMapStrData* pEmailInfo, AVModule::IAVMapStrData* pReturnData) throw();

protected:
    HRESULT InitSkinEngine(symtheme::ISymSkinWindow*& pSkinWindow);

protected:
    symtheme::ISymSkinWindowPtr m_spSkin;
};

} // namespace