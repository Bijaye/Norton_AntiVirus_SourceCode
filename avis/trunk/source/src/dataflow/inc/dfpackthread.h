#if !defined(AFX_TESTTHREAD_H__95D079D3_92D6_11D3_A569_0004ACECC1E1__INCLUDED_)
#define AFX_TESTTHREAD_H__95D079D3_92D6_11D3_A569_0004ACECC1E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TestThread.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CTestThread thread

class DFManager;
class DFSample;

class CDFPackThread : public CWinThread
{
	DECLARE_DYNCREATE(CDFPackThread)
protected:

// Attributes
public:
	CDFPackThread() {;}         
	CDFPackThread(DFManager *mgr, DFSample *dfSample);           
    void Kill();
	BOOL Start();
	BOOL bKill;
	virtual ~CDFPackThread();
	BOOL Running(); 
    DFManager *manager;
    DFSample *pSample;
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int Run();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTestThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTTHREAD_H__95D079D3_92D6_11D3_A569_0004ACECC1E1__INCLUDED_)
