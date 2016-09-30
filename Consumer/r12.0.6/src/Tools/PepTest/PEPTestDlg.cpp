// PEPTestDlg.cpp : implementation file
//

#include "stdafx.h"

#if !defined(_NO_SIMON_INITGUID)
#define SIMON_INITGUID
#endif
#define INITGUID
#include "initguid.h"

#define INITIIDS
#include "DRMTrustHelper.h"

#ifdef _DEBUG
//#define new DEBUG_NEW
#endif

#include "PEPTest.h"
#include "PEPTestDlg.h"

#include <stahlsoft.h>
#include <simon.h>
#define WM_UPDATESTATUS WM_USER+5

// PEP Headers
#define INIT_V2AUTHORIZATION_IMPL
#define _INIT_V2LICENSINGAUTHGUIDS
#define _V2AUTHORIZATION_SIMON
#define _DRM_V2AUTHMAP_IMPL
#define _SIMON_PEP_
#define _AV_COMPONENT_CLIENT
#include "ComponentPepHelper.h"


#define SUBSCRIPTION00_INITGUID

#include "Subscription_Helper.h"
#include "SimonCollectionImpl.h"
#include "SimonCollectionToolbox.h"
#include "DRMNamedProperties.h"
#define _INIT_COSVERSIONINFO
#include "DJSExtendedLicenseInfo.h"

#include "SymAlertStatic.h"
#include ".\peptestdlg.h"

// CPEPTestDlg dialog

SIMON::CSimonModule _SimonModule;

STDMETHODIMP CPEPTestRunnable::Run()
{
    m_bRunning = true;
    Fire_OnStart();
    HANDLE hWait[1] = {m_shStop};
    DWORD uiFrequency = m_uiFrequency;
    CString csOut;

    STAHLSOFT_HRX_TRY(hr)
    {
        bool bRet = true;
        long nCount = 1;
        do
        {

            DWORD dwRet = ::WaitForMultipleObjects(1,&hWait[0],FALSE,uiFrequency);
            switch(dwRet)
            {
            default:
            case WAIT_OBJECT_0:
                bRet = false;
                break;
            case WAIT_TIMEOUT:
            case WAIT_OBJECT_0 + 1:
                {
                    ++m_nCount;
                    HRESULT hr = DoTimeout(csOut);
                    Fire_OnStatus(hr,csOut,m_nCount);

                }
                break;
            }
        }while(bRet);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
    Fire_OnStop();
    ::ResetEvent(m_shStop);
    m_bRunning = false;
    return hr;
}		
HRESULT CPEPTestRunnable::DoTimeout(CString& csOut)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        DWORD dwPolicyID                = DRMPEP::POLICY_ID_QUERY_SUBSCRIPTION_RECORD;
        DWORD dwDisposition             = DRMPEP::DISPOSITION_NONE;
        DWORD dwSubscriptionDisposition = SUBSCRIPTION_STATIC::DISPOSITION_NONE;

        bool bGetText = false;

        switch(m_nCurSel)
        {
        default:
        case 0:
            {
                bGetText = true;
                dwPolicyID = DRMPEP::POLICY_ID_QUERY_ABOUT_BOX_TEXT;
            }
            break;
        case 1:
            {
                dwPolicyID = DRMPEP::POLICY_ID_QUERY_PRODUCT_STATE;
            }
            break;
        case 2:
            {
                dwPolicyID = DRMPEP::POLICY_ID_ACTIVATE_SUBSCRIPTION;
            }
            break;
        case 3:
            {
                dwPolicyID = DRMPEP::POLICY_ID_REGISTER_SUBSCRIPTION;
            }
            break;
        case 4:
            {
                dwPolicyID = DRMPEP::POLICY_ID_QUERY_SUBSCRIPTION_RECORD;
                dwDisposition = DRMPEP::DISPOSITION_CALL_DIRECT;
            }
            break;
        case 5:
            {
                dwPolicyID = DRMPEP::POLICY_ID_QUERY_SUBSCRIPTION_FORCE_RENEWAL;
            }
            break;
        case 6:
            {
                dwPolicyID = DRMPEP::POLICY_ID_SUBSCRIPTION_MANAGEMENT;
            }
            break;
        case 7:
            {
                dwPolicyID = DRMPEP::POLICY_ID_QUERY_LICENSE_RECORD;
                dwSubscriptionDisposition = DRMPEP::DISPOSITION_GET_SUBSCRIPTION_INFO;
            }
            break;
        case 8:
            {
                dwPolicyID = DRMPEP::POLICY_ID_QUERY_LICENSE_RECORD;
                dwDisposition = DRMPEP::DISPOSITION_CALL_DIRECT;
            }
            break;
        case 9:
            {
                dwPolicyID = AntiVirusComponent::POLICY_ID_PAID_CONTENT_VDEF_UPDATES;
                dwSubscriptionDisposition = (SUBSCRIPTION_STATIC::DISPOSITION_FORCE_VERIFICATION | SUBSCRIPTION_STATIC::DISPOSITION_VERBOSE);
                dwDisposition = DRMPEP::DISPOSITION_POSSIBLY_ONLINE;
            }
            break;
        }

        DECLARE_DRM_COLLECTION_CONTEXT(m_guidContext, dwPolicyID, dwDisposition, dwSubscriptionDisposition);

//        UpdateData(TRUE);
        /*
        if(m_bSeed)
        {
            MessageBox("Seeding collection", "Seeded");
            //SIMON_COLLECTION::SetCollectionString(spColl, SZ_PROPERTY_OWNER_SEED, V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID);
            SIMON_COLLECTION::SetCollectionString(spColl, SZ_PROPERTY_OWNER_SEED, V2LicensingAuthGuids::SZ_NPF_SUITEOWNERGUID);
        }
*/
        CALL_DRM_QUERY_POLICY;

        CString sText;
        if(bGetText)
        {
            // Get the subscription disposition
            dwDisposition = SUBSCRIPTION_STATIC::DISPOSITION_NONE;
            DRM_COLLECTION_READ_DWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION, dwDisposition, SUBSCRIPTION_STATIC::DISPOSITION_NONE)

                StahlSoft::CSmartDataPtr<BYTE> spbyText;
            DRM_COLLECTION_READ_DATA(SIMON_PEP::SZ_MANAGEMENT_PROPERTY_TEXT, spbyText)
                sText = (LPCTSTR)spbyText.m_p;

            try
            {
                DRM_COLLECTION_READ_DATA(SIMON_PEP::SZ_MANAGEMENT_PROPERTY_TEXT2, spbyText)
                    sText += _T("\n");
                sText += (LPCTSTR)spbyText.m_p;
            } catch(...){}
        }
        csOut = sText;
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
    return hr;
}

STDMETHODIMP CPEPTestRunnable::IsRunning()
{
    STAHLSOFT_HRX_TRY(hr)
    {
        hr = m_bRunning?S_OK:S_FALSE;
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
    return hr;
}		
STDMETHODIMP CPEPTestRunnable::RequestExit()
{
    STAHLSOFT_HRX_TRY(hr)
    {
        ::SetEvent(m_shStop);  
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);
    return hr;
}		

CPEPTestDlg::CPEPTestDlg(CWnd* pParent /*=NULL*/)
: CDialog(CPEPTestDlg::IDD, pParent)
, m_bSeed(false)
, m_csCount(_T("Count: 0"))
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_bStressThreadStarted = FALSE;
}

void CPEPTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_ActionList);
    DDX_Control(pDX, IDC_RESULT, m_Result);
    DDX_Control(pDX, IDC_RETURN, m_Return);
    DDX_Check(pDX, IDC_SEED, (int&)m_bSeed);
    DDX_Text(pDX, IDC_COUNT,m_csCount);
    DDX_Text(pDX, IDC_RESULT,m_csStatus);
    DDX_Text(pDX, IDC_RETURN,m_csReturn);

    DDX_Control(pDX, IDC_COMPONENT_SELECT, m_componentList);
}

BEGIN_MESSAGE_MAP(CPEPTestDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_UPDATESTATUS,OnUpdateStatus)
    ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
    ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON_START_STRESS, OnBnClickedButtonStartStress)
    ON_BN_CLICKED(IDC_BUTTON_STOP_STRESS, OnBnClickedButtonStopStress)
    ON_LBN_SELCHANGE(IDC_LIST1, OnLbnSelchangeList1)
END_MESSAGE_MAP()

afx_msg LRESULT CPEPTestDlg::OnUpdateStatus(WPARAM, LPARAM)
{
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    StahlSoft::CSmartLock smLock(&m_crit);
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    UpdateData(FALSE);
    return 0;
}

// CPEPTestDlg message handlers

BOOL CPEPTestDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    // TODO: Add extra initialization here
    m_ActionList.AddString(_T("QueryAboutBoxText"));
    m_ActionList.AddString(_T("QueryProductState"));
    m_ActionList.AddString(_T("ActivateSubscription"));
    m_ActionList.AddString(_T("InitSubscription"));
    m_ActionList.AddString(_T("QuerySubscriptionRecord"));
    m_ActionList.AddString(_T("ForceSubscriptionRenewal"));
    m_ActionList.AddString(_T("SubscriptionManagement"));
    m_ActionList.AddString(_T("QueryLicenseRecord - Cached (GetsubscriptionInfo)"));
    m_ActionList.AddString(_T("QueryLicenseRecord - Direct"));
    m_ActionList.AddString(_T("AllowAccessPaidContent"));

    GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_START_STRESS)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_STOP_STRESS)->EnableWindow(FALSE);

    m_thread.SetRunnable(&m_runnable);
    m_runnable.RegisterEventSink(this);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPEPTestDlg::OnPaint() 
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialog::OnPaint();
    }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPEPTestDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CPEPTestDlg::OnBnClickedButton1()
{
    STAHLSOFT_HRX_TRY(hr)
    {
        DWORD dwPolicyID = DRMPEP::POLICY_ID_QUERY_SUBSCRIPTION_RECORD;
        DWORD dwDisposition = DRMPEP::DISPOSITION_NONE;
        DWORD dwSubscriptionDisposition = SUBSCRIPTION_STATIC::DISPOSITION_NONE;

        bool bGetText = false;

        GUID guidContext = DRMGUID;

        CString csActiontext;
        m_ActionList.GetText( m_ActionList.GetCurSel(),  csActiontext);

        MessageBox( csActiontext, "Action");

        switch(m_ActionList.GetCurSel())
        {
        default:
        case 0:
            {
                bGetText = true;
                dwPolicyID = DRMPEP::POLICY_ID_QUERY_ABOUT_BOX_TEXT;
            }
            break;
        case 1:
            {
                dwPolicyID = DRMPEP::POLICY_ID_QUERY_PRODUCT_STATE;
            }
            break;
        case 2:
            {
                dwPolicyID = DRMPEP::POLICY_ID_ACTIVATE_SUBSCRIPTION;
            }
            break;
        case 3:
            {
                dwPolicyID = DRMPEP::POLICY_ID_REGISTER_SUBSCRIPTION;
            }
            break;
        case 4:
            {
                dwPolicyID = DRMPEP::POLICY_ID_QUERY_SUBSCRIPTION_RECORD;
                dwDisposition = DRMPEP::DISPOSITION_CALL_DIRECT;
            }
            break;
        case 5:
            {
                dwPolicyID = DRMPEP::POLICY_ID_QUERY_SUBSCRIPTION_FORCE_RENEWAL;
            }
            break;
        case 6:
            {
                dwPolicyID = DRMPEP::POLICY_ID_SUBSCRIPTION_MANAGEMENT;
            }
            break;
        case 7:
            {
                dwPolicyID = DRMPEP::POLICY_ID_QUERY_LICENSE_RECORD;
                dwSubscriptionDisposition = DRMPEP::DISPOSITION_GET_SUBSCRIPTION_INFO;
            }
            break;
        case 8:
            {
                dwPolicyID = DRMPEP::POLICY_ID_QUERY_LICENSE_RECORD;
                dwDisposition = DRMPEP::DISPOSITION_CALL_DIRECT;
            }
            break;
        case 9:
            {
                dwPolicyID = AntiVirusComponent::POLICY_ID_PAID_CONTENT_VDEF_UPDATES;
                dwSubscriptionDisposition = (SUBSCRIPTION_STATIC::DISPOSITION_FORCE_VERIFICATION | SUBSCRIPTION_STATIC::DISPOSITION_VERBOSE);
                dwDisposition = DRMPEP::DISPOSITION_POSSIBLY_ONLINE;
            }
            break;
        }

        DECLARE_DRM_COLLECTION_CONTEXT(guidContext, dwPolicyID, dwDisposition, dwSubscriptionDisposition);

        UpdateData(TRUE);
        if(m_bSeed)
        {
            MessageBox("Seeding collection", "Seeded");
            //SIMON_COLLECTION::SetCollectionString(spColl, SZ_PROPERTY_OWNER_SEED, V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID);
            SIMON_COLLECTION::SetCollectionString(spColl, SZ_PROPERTY_OWNER_SEED, V2LicensingAuthGuids::SZ_NPF_SUITEOWNERGUID);
        }

        CALL_DRM_QUERY_POLICY;

        CString sText;
        if(bGetText)
        {
            // Get the subscription disposition
            dwDisposition = SUBSCRIPTION_STATIC::DISPOSITION_NONE;
            DRM_COLLECTION_READ_DWORD(SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION, dwDisposition, SUBSCRIPTION_STATIC::DISPOSITION_NONE)

                StahlSoft::CSmartDataPtr<BYTE> spbyText;
            DRM_COLLECTION_READ_DATA(SIMON_PEP::SZ_MANAGEMENT_PROPERTY_TEXT, spbyText)
                sText = (LPCTSTR)spbyText.m_p;

            try
            {
                DRM_COLLECTION_READ_DATA(SIMON_PEP::SZ_MANAGEMENT_PROPERTY_TEXT2, spbyText)
                    sText += _T("\n");
                sText += (LPCTSTR)spbyText.m_p;
            } catch(...){}
        }
        m_Result.SetWindowText(sText);
    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr);

    m_csReturn.Format("Return Value: 0x%x", hr );
    m_Return.SetWindowText(m_csReturn);
}

void CPEPTestDlg::OnBnClickedCancel()
{
    // Close the dialog
    OnCancel();
}
/*
void CPEPTestDlg::PopulateLBComponents(CComboBox &cbox)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        CRegKey rk;
        LONG lRet = ERROR_SUCCESS;

        lRet = rk.Open(HKEY_LOCAL_MACHINE, DRMREG::SZ_SUITE_OWNER_KEY, KEY_READ);
        hrx << HRESULT_FROM_WIN32(lRet);

        while(ERROR_SUCCESS != lRet)
        {
            rk.EnumKey(
        }

    }
    STAHLSOFT_HRX_CATCH_ALL_NO_RETURN(hr)
}
*/

void CPEPTestDlg::OnStart_PEPStressRunnable()
{

}
void CPEPTestDlg::OnStop_PEPStressRunnable()
{

}
void CPEPTestDlg::OnStatus_PEPStressRunnable(HRESULT hr,LPCTSTR lpcStatus, int count)
{
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    StahlSoft::CSmartLock smLock(&m_crit);
    //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
    m_csStatus = lpcStatus;
    m_csCount.Format(_T("Count: %d"),count);
    m_csReturn.Format("Return Value: 0x%x", hr );
    PostMessage(WM_UPDATESTATUS);

}


void CPEPTestDlg::OnBnClickedButtonStartStress()
{
    // TODO: Add your control notification handler code here
    GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_START_STRESS)->EnableWindow(FALSE);
    GetDlgItem(IDC_BUTTON_STOP_STRESS)->EnableWindow(TRUE);
    m_runnable.m_guidContext = DRMGUID;
    m_runnable.m_nCurSel = m_ActionList.GetCurSel();
    m_runnable.m_nCount = 0;
    m_ActionList.GetText(m_runnable.m_nCurSel,  m_runnable.m_csActiontext);
    StahlSoft::CreateThread< StahlSoft::CRunnableThread >(m_thread);
    m_bStressThreadStarted = TRUE;
}

void CPEPTestDlg::OnBnClickedButtonStopStress()
{
    // TODO: Add your control notification handler code here
    GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_START_STRESS)->EnableWindow(TRUE);
    GetDlgItem(IDC_BUTTON_STOP_STRESS)->EnableWindow(FALSE);
    StahlSoft::StopThread< StahlSoft::CRunnableThread >(m_thread,INFINITE);
    m_bStressThreadStarted = FALSE;
}

void CPEPTestDlg::OnLbnSelchangeList1()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    int nCurSel = m_ActionList.GetCurSel();
    GetDlgItem(IDC_BUTTON1)->EnableWindow(nCurSel != -1);

    if(!m_bStressThreadStarted)
        GetDlgItem(IDC_BUTTON_START_STRESS)->EnableWindow(nCurSel != -1);
}
