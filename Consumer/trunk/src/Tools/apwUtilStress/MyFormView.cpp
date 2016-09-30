////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// MyFormView.cpp : implementation file
//

#include "stdafx.h"
#define INITIIDS
#include <initguid.h>
#define SIMON_INITGUID
#include <SIMON.h>
#include "apwUtilStress.h"
#include "MyFormView.h"
#include "NAVOptHelperEx.h"

#include "DJSMAR00_Static.h"
#include "OptNames.h"
#include "InstOptsNames.h"
#include "NAVInfo.h"
#include "Hrx.h"
// PEP Headers
#define _INIT_V2LICENSINGAUTHGUIDS
#define _V2AUTHORIZATION_SIMON
#define INIT_V2AUTHORIZATION_IMPL
#define _DRM_V2AUTHMAP_IMPL
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"

#include "ccLib.h"

#include "ccLibDllLink.h"
#include "ccSymDelayLoader.h"
// ccLib delay loader
// This will allow the ccLib dll to delay load at run-time
ccSym::CDelayLoader g_DelayLoader;

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("apwutil"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

SIMON::CSimonModule _SimonModule;
// SymInterface map for ccVerifyTrust static lib
SYM_OBJECT_MAP_BEGIN()                          
SYM_OBJECT_MAP_END()                            

IMPLEMENT_DYNCREATE(CMyFormView, CFormView)

CMyFormView::CMyFormView()
	: CFormView(CMyFormView::IDD)
{
}

CMyFormView::~CMyFormView()
{
}

void CMyFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMyFormView, CFormView)
    ON_BN_CLICKED(IDC_BUTTON_QUERY_POLICY, OnBnClickedButtonQueryPolicy)
END_MESSAGE_MAP()


// CMyFormView diagnostics

#ifdef _DEBUG
void CMyFormView::AssertValid() const
{
	CFormView::AssertValid();
}

void CMyFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CMyFormView message handlers

void CMyFormView::OnBnClickedButtonQueryPolicy()
{

    STAHLSOFT_HRX_TRY(hr)
    {
        // TODO: Add your control notification handler code here
        DJSMAR_LicenseType licType = DJSMAR_LicenseType_Violated;

        // Getting licensing and subscription properties needed to enable product features
        QUERY_DRM_POLICY(DRMPEP::POLICY_ID_QUERY_LICENSE_RECORD
            , DRMPEP::DISPOSITION_GET_SUBSCRIPTION_INFO
            , SUBSCRIPTION_STATIC::DISPOSITION_NONE);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);

}
