#pragma once
#include "stdafx.h"
#include "resource.h"
#define PROTECTIONPROVIDER_HELPERTYPES_WITHNAMESPACE
#import "ProtectionProvider.tlb" raw_interfaces_only exclude("wireHWND", "_RemotableHandle", "__MIDL_IWinTypes_0009")
#include "ProtectionProvider.h"
#include "util.h"
#include "afxwin.h"
#include "afxcmn.h"


// CTestHarnessDlg dialog
class CTestHarnessDlg : public CDialog
{
	DECLARE_DYNAMIC(CTestHarnessDlg)

public:
    // Constructor-destructor
	CTestHarnessDlg(CWnd* pParent = NULL);
	virtual ~CTestHarnessDlg();

    // Dialog Data
	enum { IDD = IDD_TESTHARNESS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual afx_msg void PostNcDestroy();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedSnapinConfigure();
    afx_msg void OnBnClickedSnapinViewlogs();
    afx_msg void OnBnClickedExit();
    afx_msg void OnBnClickedRefreshSnapinInfo();
    afx_msg void OnBnClickedTest1();
    afx_msg void OnBnClickedTest2();
    afx_msg void OnBnClickedTest3();
    afx_msg void OnBnClickedTest4();
    afx_msg void OnRClickAutoprotects(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnLvnItemActivateProtectionTechnologies(NMHDR *pNMHDR, LRESULT *pResult);
private:
	// ** CONSTANTS **
	static const unsigned int ProtectionTechnologiesColumn_Name = 0;
	static const unsigned int ProtectionTechnologiesColumn_Status = 1;
	static const unsigned int AutoprotectsColumn_Name = 0;
	static const unsigned int AutoprotectsColumn_Status = 1;

	// ** FUNCTION MEMBERS **
	HRESULT ShowProtectionProviderInfo( ProtectionProviderLib::IProtection_Provider* protectionProvider );
	// Returns the currently selected protection provider, or NULL if none selected
	ProtectionProviderLib::IProtection_Provider* GetSelectedProtectionProvider( void );
	// Returns the currently selected protection provider, or NULL if none selected
	ProtectionProviderLib::IProtection* GetSelectedAutoprotect( void );
	// Sets *errorDescription equal to the description of the system message ID systemMessageID.
	// If trimTrailingNewline is TRUE, trims any trailing newline
	HRESULT GetErrorDescription( HRESULT systemMessageID, _bstr_t* errorDescription, bool trimTrailingNewline );

    // ** DATA MEMBERS:  CONTROLS **
	CListCtrl	protectionTechnologiesCtrl;
	CListCtrl	autoprotectsCtrl;
	CButton		supportsBasicScan;
	CButton		supportsConfigureableScans;

	// ** DATA MEMBERS:  OTHERS **
	CLSIDList												protectionTechnologiesIDs;
	ProtectionProviderLib::ProtectionProviderList			protectionTechnologies;
	ProtectionProviderLib::ProtectionList					autoprotects;
};
