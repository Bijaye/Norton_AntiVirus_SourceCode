// PEPTestDlg.h : header file
//

#pragma once
#include "afxwin.h"

class CPEPStressRunnableSink
{
public:
    virtual void OnStart_PEPStressRunnable() = 0;
    virtual void OnStop_PEPStressRunnable() = 0;
    virtual void OnStatus_PEPStressRunnable(HRESULT hr,LPCTSTR lpcStatus, int count) = 0;
//    virtual void OnStatus_DJSLicenseRunnable(DJSMAR00_LicenseState state,long nCount) = 0;
};

class CPEPTestRunnableSource: public StahlSoft::CEventSourceT<CPEPStressRunnableSink*>
{
public:
    void Fire_OnStart()
    {
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        StahlSoft::CSmartLock smLock(&m_lock);
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        iterator it = begin();
        while(it != end())
        {
            (*it)->OnStart_PEPStressRunnable();
            ++it;
        }
    }
    void Fire_OnStop()
    {
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        StahlSoft::CSmartLock smLock(&m_lock);
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        iterator it = begin();
        while(it != end())
        {
            (*it)->OnStop_PEPStressRunnable();
            ++it;
        }
    }
    void Fire_OnStatus(HRESULT hr,LPCTSTR lpcStatus, int nCount)
    {
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        StahlSoft::CSmartLock smLock(&m_lock);
        //--~~--//--~~--//--~~--//--~~--//--~~--//--~~--//--~~--
        iterator it = begin();
        while(it != end())
        {
            (*it)->OnStatus_PEPStressRunnable(hr,lpcStatus,nCount);
            ++it;
        }
    }
};

/////////////////////////////////////////////////////////////////////////////
// CInstalledAppsStressTest dialog
class CPEPTestRunnable: 
    public StahlSoft::CRunnable
    ,public CPEPTestRunnableSource
{
private:
    bool m_bRunning;
public:
    CPEPTestRunnable():m_bRunning(false)
    {
        m_shStop = ::CreateEvent(FALSE,TRUE,FALSE,NULL);
        m_uiFrequency = 100;
        m_nCurSel = -1;
        m_nCount = 0;
    }

    STDMETHOD(Run)();		
    STDMETHOD(IsRunning)();	
    STDMETHOD(RequestExit)();
    UINT	m_uiFrequency;
    GUID    m_guidContext;
    CString m_csActiontext;
    int     m_nCurSel;
    int m_nCount;
private:
    HRESULT DoTimeout(CString& csOut);
    StahlSoft::CSmartHandle m_shStop;
};

// CPEPTestDlg dialog
class CPEPTestDlg : public CDialog
    , public CPEPStressRunnableSink
{
    BOOL m_bStressThreadStarted;
// Construction
public:
	CPEPTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PEPTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

    virtual void OnStart_PEPStressRunnable();
    virtual void OnStop_PEPStressRunnable();
    virtual void OnStatus_PEPStressRunnable(HRESULT hr,LPCTSTR lpcStatus, int count);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()


    StahlSoft::CRunnableThread  m_thread;
    CPEPTestRunnable m_runnable;

public:
    afx_msg LRESULT OnUpdateStatus(WPARAM, LPARAM);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnLbnDblclkList1();
	CListBox m_ActionList;
	CStatic m_Result;
	CStatic m_Return;
    bool m_bSeed;
	afx_msg void OnBnClickedCancel();
	// Listbox that allows the user to select the component ID to execute the business rules on
	CComboBox m_componentList;
    afx_msg void OnBnClickedButtonStartStress();
    afx_msg void OnBnClickedButtonStopStress();
    afx_msg void OnLbnSelchangeList1();
    CString m_csCount;
    CString m_csStatus;
    CString m_csReturn;
    StahlSoft::CCritSec m_crit;
};
