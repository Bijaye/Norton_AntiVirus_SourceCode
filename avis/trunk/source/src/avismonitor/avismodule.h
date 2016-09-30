// AVISModule.h: interface for the CAVISModule class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVISMODULE_H__66A8FF57_D7CB_11D2_B98E_0004ACEC31AA__INCLUDED_)
#define AFX_AVISMODULE_H__66A8FF57_D7CB_11D2_B98E_0004ACEC31AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MODULE_STOPPED      0
#define MODULE_STARTED      1
#define MODULE_UNKNOWN      2

class CAVISModule : public CObject  
{
public:
	BOOL m_SendMail;
	UINT m_RestartTime;
	DWORD m_ProcessID;
	BOOL m_WMCloseMessageSent;
    PROCESS_MEMORY_COUNTERS m_MemCounter;
    DWORD   m_OrigWorkingSetSize;
    DWORD   m_OrigVMSize;
	BOOL    m_ModuleExternalStarted;
	BOOL    m_ModuleEnabled;
	LONG    m_ModuleCheckCount;
	CString m_ModuleArguments;
	CString m_ModuleCheckFileName;
	HANDLE  m_hProcess;
	BOOL    m_Saved;
	UINT    m_ModuleErrorCount;
	UINT    m_ModuleStatus;
	CString m_ModuleFilePath;
	CString m_ModuleName;
	BOOL	m_AutoRestart;
	UINT	m_AutoStopTime;
	COleDateTime    m_ModuleLastCheckTime;
	COleDateTime    m_ModuleStartTime;
	COleDateTime    m_ModuleStopTime;
	CWinThread *    m_pWinThread;
	CAVISModule();
	virtual ~CAVISModule();
};

typedef CTypedPtrArray<CObArray, CAVISModule*> CAVISModuleList;

#endif // !defined(AFX_AVISMODULE_H__66A8FF57_D7CB_11D2_B98E_0004ACEC31AA__INCLUDED_)
