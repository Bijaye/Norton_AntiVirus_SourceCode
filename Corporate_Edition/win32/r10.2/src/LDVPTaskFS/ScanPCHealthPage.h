// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_SCANPCHEALTHPAGE_H__53DAE602_DAF6_11D1_A2DC_00A0C9749EDB__INCLUDED_)
#define AFX_SCANPCHEALTHPAGE_H__53DAE602_DAF6_11D1_A2DC_00A0C9749EDB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// ScanPCHealthPage.h : header file
//

#include "vpcommon.h"

/////////////////////////////////////////////////////////////////////////////
// IScanPCHealthCallbackImp
interface IScanPCHealthCallbackImp :  public IScanCallback
{
	LONG m_refcount;
	IScanConfig* m_pConfig;
	int	m_infectedCount;

	IScanPCHealthCallbackImp(IScanConfig* pConfig=NULL)
	{ m_infectedCount = 0, m_refcount = 1, m_pConfig = pConfig; }
	~IScanPCHealthCallbackImp() {};
	
	ULONG STDMETHODCALLTYPE AddRef(void)
	{ return InterlockedIncrement(&m_refcount);	}

	ULONG STDMETHODCALLTYPE Release(void)
	{
		ULONG r = InterlockedDecrement(&m_refcount);
		if (r==0) delete this;
		return r;
	}

	HRESULT STDMETHODCALLTYPE QueryInterface(const IID & riid,void **ppvObj) 
	{
		GUID iid = _IID_IScanCallback;
		*ppvObj = NULL;
		if (riid == IID_IScanCallback || riid==IID_IUnknown)
		{
    		*ppvObj =this;
			AddRef();
			return S_OK;
		}
   		return E_NOINTERFACE;
	}

	VI(ULONG) RTSProgress(PREALTIMEPROGRESSBLOCK Progress)
	{ 
        return S_OK; 
    }
	
	VI(ULONG) Progress(PPROGRESSBLOCK Progress);
    VI(ULONG) Virus(const char *line);

    VI(ULONG) ScanInfo(PEVENTBLOCK pEventBlk)
	{ 
        return S_OK; 
    }
};

/////////////////////////////////////////////////////////////////////////////
// CScanPCHealthPage dialog
#include "WizardPage.h"
class CScanPCHealthPage : public CWizardPage
{
	DECLARE_DYNCREATE(CScanPCHealthPage)

// Construction
public:
	CScanPCHealthPage();
	~CScanPCHealthPage();

	void OnDestroying();
	BOOL OnKillActive();

// Dialog Data
	//{{AFX_DATA(CScanPCHealthPage)
	enum { IDD = IDD_SCAN_PCHEALTH };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

	IScanPCHealthCallbackImp	m_callback;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CScanPCHealthPage)
	protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CScanPCHealthPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL OnSetActive();
	BOOL OnWizardFinish();

	BOOL CopyRealtimeScan();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCANPCHEALTHPAGE_H__53DAE602_DAF6_11D1_A2DC_00A0C9749EDB__INCLUDED_)
