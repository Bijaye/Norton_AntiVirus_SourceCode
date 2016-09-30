////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// CLTDoubleAgentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include "CLTDoubleAgent.h"
#include "CLTDoubleAgentDlg.h"
#include ".\cltdoubleagentdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CCLTDoubleAgentDlg dialog

CCLTDoubleAgentDlg::CCLTDoubleAgentDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCLTDoubleAgentDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCLTDoubleAgentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCLTDoubleAgentDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_QUERY, OnBnClickedQuery)
END_MESSAGE_MAP()


// CCLTDoubleAgentDlg message handlers

BOOL CCLTDoubleAgentDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCLTDoubleAgentDlg::OnPaint() 
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
HCURSOR CCLTDoubleAgentDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

#define ERROR_TEST(hresult, msg) if(FAILED(hresult)) { PostError(_T(msg), hresult); }

void CCLTDoubleAgentDlg::OnBnClickedQuery()
{
	HRESULT hr;
	bool bResult;
	DWORD dwResult;
	unsigned long ulResult;

	try
	{
		hr = cltHelper.Initialize();
	}
	catch(_com_error e)
	{
		this->MessageBoxW(e.Description(), _T("COM Error :: Initialize"));
		return;
	}

	ERROR_TEST(hr, "Initializing CLT Helper");

	try
	{
		hr = cltHelper.CanIRun(bResult);
	}
	catch(_com_error e)
	{
		this->MessageBoxW(e.Description(), _T("COM Error :: CanIRun"));
		return;
	}

	ERROR_TEST(hr, "CanIRun");

	PostBoolean(IDC_CANIRUN, bResult);

	try
	{
		hr = cltHelper.CanIAccessPaidContent(bResult);
	}
	catch(_com_error e)
	{
		this->MessageBoxW(e.Description(), _T("COM Error :: CanIAccessPaidContent"));
		return;
	}

	ERROR_TEST(hr, "CanIAccessPaidContent");

	PostBoolean(IDC_PAIDCONTENT, bResult);

	try
	{
		hr = cltHelper.GetLicenseType((clt::cltLicenseType)dwResult);
	}
	catch(_com_error e)
	{
		this->MessageBoxW(e.Description(), _T("COM Error :: GetLicenseType"));
		return;
	}

	ERROR_TEST(hr, "GetLicenseType");

	PostLicenseType(dwResult);

	try
	{
		hr = cltHelper.GetLicenseAttributes((clt::cltLicenseAttribs)dwResult);
	}
	catch(_com_error e)
	{
		this->MessageBoxW(e.Description(), _T("COM Error :: GetLicenseAttributes"));
		return;
	}

	ERROR_TEST(hr, "GetLicenseAttributes");

	PostLicenseAttributes(dwResult);

	try
	{
		hr = cltHelper.GetDaysRemaining(ulResult);
	}
	catch(_com_error e)
	{
		this->MessageBoxW(e.Description(), _T("COM Error :: GetDaysRemaining"));
		return;
	}

	ERROR_TEST(hr, "GetDaysRemaining");

	PostUnsigned(IDC_DAYSREMAINING, ulResult);

	try
	{
		hr = cltHelper.IsAggregated(bResult);
	}
	catch(_com_error e)
	{
		this->MessageBoxW(e.Description(), _T("COM Error :: IsAggregated"));
		return;
	}

	ERROR_TEST(hr, "IsAggregated");
	PostBoolean(IDC_AGGREGATED, bResult);

	try
	{
		hr = cltHelper.IsSubscriptionWarningPeriod(bResult);
	}
	catch(_com_error e)
	{
		this->MessageBoxW(e.Description(), _T("COM Error :: IsSubscriptionWarningPeriod"));
		return;
	}

	ERROR_TEST(hr, "IsSubscriptionWarningPeriod");
	PostBoolean(IDC_SUBWARN, bResult);

	try
	{
		hr = cltHelper.IsSubscriptionExpired(bResult);
	}
	catch(_com_error e)
	{
		this->MessageBoxW(e.Description(), _T("COM Error :: IsSubscriptionExpired"));
		return;
	}

	ERROR_TEST(hr, "IsSubscriptionExpired");
	PostBoolean(IDC_SUBEXPIRED, bResult);

	try
	{
		hr = cltHelper.IsSubscriptionPaid(bResult);
	}
	catch(_com_error e)
	{
		this->MessageBoxW(e.Description(), _T("COM Error :: IsSubscriptionPaid"));
		return;
	}

	ERROR_TEST(hr, "IsSubscriptionPaid");
	PostBoolean(IDC_SUBPAID, bResult);

	try
	{
		hr = cltHelper.CanAutoRenewSubscription(bResult);
	}
	catch(_com_error e)
	{
		this->MessageBoxW(e.Description(), _T("COM Error :: CanAutoRenewSubscription"));
		return;
	}

	ERROR_TEST(hr, "CanAutoRenewSubscription");
	PostBoolean(IDC_AUTORENEW, bResult);

	try
	{
		hr = cltHelper.CanRenewSubscription(bResult);
	}
	catch(_com_error e)
	{
		this->MessageBoxW(e.Description(), _T("COM Error :: CanRenewSubscription"));
		return;
	}

	ERROR_TEST(hr, "CanRenewSubscription");
	PostBoolean(IDC_RENEW, bResult);

	try
	{
		hr = cltHelper.ShouldShowAlerts(bResult);
	}
	catch(_com_error e)
	{
		this->MessageBoxW(e.Description(), _T("COM Error :: ShouldShowAlerts"));
		return;
	}

	ERROR_TEST(hr, "ShouldShowAlerts");
	PostBoolean(IDC_SHOWALERTS, bResult);
}

void CCLTDoubleAgentDlg::PostError(LPCTSTR description, HRESULT error)
{
	CStringW msg;
	msg.Format(_T("Error: [%s]. Hresult = 0x%08x"), description, error);
	this->MessageBoxW(msg);
}

void CCLTDoubleAgentDlg::PostLicenseType(DWORD dwType)
{
	CStringW cswTypeString;
	CEdit* outputBox;

	outputBox = reinterpret_cast<CEdit*>(GetDlgItem(IDC_LICENSETYPE));

	switch(dwType)
	{
	case clt::cltLicenseType_Activation:
		cswTypeString = _T("Activation/Retail"); break;
	case clt::cltLicenseType_Beta:
		cswTypeString = _T("Beta"); break;
	case clt::cltLicenseType_ESD:
		cswTypeString = _T("ESD"); break;
	case clt::cltLicenseType_OEM:
		cswTypeString = _T("OEM"); break;
	case clt::cltLicenseType_SOS:
		cswTypeString = _T("SOS"); break;
	case clt::cltLicenseType_TryBuy:
		cswTypeString = _T("TryBuy"); break;
	case clt::cltLicenseType_TryDie:
		cswTypeString = _T("TryDie"); break;
	case clt::cltLicenseType_Unlicensed:
		cswTypeString = _T("Unlicensed"); break;
	case clt::cltLicenseType_Violated:
		cswTypeString = _T("Violated"); break;
	default:
		cswTypeString = _T("[internal error - undefined type]"); break;
	}

	outputBox->SetWindowText(cswTypeString);
}

void CCLTDoubleAgentDlg::PostLicenseAttributes(DWORD dwAttribs)
{
	CStringW cswAttrString = _T("");
	CEdit* outputBox;

	outputBox = reinterpret_cast<CEdit*>(GetDlgItem(IDC_LICENSEATTRIBUTES));

	/** STATE **/

	if((dwAttribs & clt::cltLicenseAttrib_Active) == clt::cltLicenseAttrib_Active)
	{
		cswAttrString += _T("Active ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_NotStarted) == clt::cltLicenseAttrib_NotStarted)
	{
		cswAttrString += _T("NotStarted ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_Trial) == clt::cltLicenseAttrib_Trial)
	{
		cswAttrString += _T("Trial ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_Expired) == clt::cltLicenseAttrib_Expired)
	{
		cswAttrString += _T("Expired ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_Violated) == clt::cltLicenseAttrib_Violated)
	{
		cswAttrString += _T("Violated ");
	}

	/** XLOK REGISTRATION **/
	if((dwAttribs & clt::cltLicenseAttrib_PreActivation) == clt::cltLicenseAttrib_PreActivation)
	{
		cswAttrString += _T("PreActivation ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_PostActivation) == clt::cltLicenseAttrib_PostActivation)
	{
		cswAttrString += _T("PostActivation ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_XLOK_Hacked) == clt::cltLicenseAttrib_XLOK_Hacked)
	{
		cswAttrString += _T("XLokHacked ");
	}

	/** SOS **/
	if((dwAttribs & clt::cltLicenseAttrib_Grace) == clt::cltLicenseAttrib_Grace)
	{
		cswAttrString += _T("Active ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_Killed) == clt::cltLicenseAttrib_Killed)
	{
		cswAttrString += _T("Killed ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_NewISP) == clt::cltLicenseAttrib_NewISP)
	{
		cswAttrString += _T("NewISP ");
	}

	/** OEM **/
	if((dwAttribs & clt::cltLicenseAttrib_Prepaid) == clt::cltLicenseAttrib_Prepaid)
	{
		cswAttrString += _T("Prepaid ");
	}

	/** SUBSCRIPTION **/
	if((dwAttribs & clt::cltLicenseAttrib_LimitedSub) == clt::cltLicenseAttrib_LimitedSub)
	{
		cswAttrString += _T("LimitedSub ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_Subscription) == clt::cltLicenseAttrib_Subscription)
	{
		cswAttrString += _T("Subscription ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_NoSubscription) == clt::cltLicenseAttrib_NoSubscription)
	{
		cswAttrString += _T("NoSubscription ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_Subscription_Active) == clt::cltLicenseAttrib_Subscription_Active)
	{
		cswAttrString += _T("Sub_Active ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_Subscription_Warning) == clt::cltLicenseAttrib_Subscription_Warning)
	{
		cswAttrString += _T("Sub_Warning ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_Subscription_Expired) == clt::cltLicenseAttrib_Subscription_Expired)
	{
		cswAttrString += _T("Sub_Expired ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_Subscription_Renewable) == clt::cltLicenseAttrib_Subscription_Renewable)
	{
		cswAttrString += _T("Sub_Renewable ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_Subscription_AutoRenew) == clt::cltLicenseAttrib_Subscription_AutoRenew)
	{
		cswAttrString += _T("Sub_AutoRenew ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_Subscription_AutoRenew_Ready) == clt::cltLicenseAttrib_Subscription_AutoRenew_Ready)
	{
		cswAttrString += _T("Sub_AutoRenewReady ");
	}

	if((dwAttribs & clt::cltLicenseAttrib_Subscription_AutoUpgrade) == clt::cltLicenseAttrib_Subscription_AutoUpgrade)
	{
		cswAttrString += _T("Sub_AutoUpgrade ");
	}

	/** SAAS **/
	if((dwAttribs & clt::cltLicenseAttrib_SaaS) == clt::cltLicenseAttrib_SaaS)
	{
		cswAttrString += _T("SaaS ");
	}

	outputBox->SetWindowText(cswAttrString);
}

void CCLTDoubleAgentDlg::PostUnsigned(int uiControl, unsigned long ulValue)
{
	CStringW cswOutput;
	CEdit* outputBox;
	cswOutput.Format(_T("%lu"), ulValue);
	outputBox = reinterpret_cast<CEdit*>(GetDlgItem(uiControl));
	if(outputBox)
		outputBox->SetWindowText(cswOutput);
}

void CCLTDoubleAgentDlg::PostBoolean(int uiControl, bool bValue)
{
	CStringW cswOutput;
	CEdit* outputBox;
	cswOutput = bValue ? _T("TRUE") : _T("FALSE");
	outputBox = reinterpret_cast<CEdit*>(GetDlgItem(uiControl));
	if(outputBox)
		outputBox->SetWindowText(cswOutput);
}

void CCLTDoubleAgentDlg::PostString(int uiControl, LPCTSTR pszValue)
{
	CEdit* outputBox;
	outputBox = reinterpret_cast<CEdit*>(GetDlgItem(uiControl));
	if(outputBox)
		outputBox->SetWindowText(pszValue);
}