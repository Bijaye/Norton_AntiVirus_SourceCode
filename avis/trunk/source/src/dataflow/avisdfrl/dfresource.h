// DFResource.h: interface for the CDFResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DFRESOURCE_H__9870F913_4BE1_11D2_B96F_0004ACEC31AA__INCLUDED_)
#define AFX_DFRESOURCE_H__9870F913_4BE1_11D2_B96F_0004ACEC31AA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>
#include "DFEvalStatus.h"
#include "DFEvalState.h"
#include "DFJob.h"

#define DF_RESOURCE_BUSY        0
#define DF_RESOURCE_COMPLETED   1
#define DF_RESOURCE_DOWN        2
#define DF_RESOURCE_FREE        3
#define DF_RESOURCE_IGNORE      4
#define DF_RESOURCE_TIMEDOUT    5
#define DF_RESOURCE_ERROR       6

class CDFResource : public CObject  
{
public:
    COleDateTime m_FinalErrorTime;
	FILETIME m_ResStartFileTime;
	FILETIME m_StartFileTime;
	ULONG m_TimeoutCount;
	BOOL m_MarkedForDisable;
	COleDateTime m_ResStartTime;
	BOOL m_ErrorFlag;
	COleDateTime m_LastErrorTime;
	ULONG m_JobCount;
    ULONG m_ErrorCount;
	COleDateTimeSpan m_BusyTime;
	BOOL m_Saved;
	int m_Index;
	int m_TryCount;
	CWinThread * m_pWinThread;
	HANDLE m_hProcess;
	DFJob * m_pDFJob;
    DFJob * m_pDFIgnoreJob;
	CString m_JobIDString;
	COleDateTimeSpan m_TimeoutPeriod;
	COleDateTime m_StartTime;
	UINT m_Status;
	CString m_ProgramName;
	CString m_UserName;
	CString m_IPAddress;
    CString m_NTMachineName;
	CDFResource()
    {
        m_TimeoutCount = 0;
        m_MarkedForDisable = FALSE;
        m_ResStartTime = COleDateTime::GetCurrentTime();
        m_JobCount = 0;
        m_ErrorCount = 0;
        m_ErrorFlag = FALSE;
        m_IPAddress.Empty();
        m_JobIDString.Empty();
        m_ProgramName.Empty();
        m_Status = DF_RESOURCE_FREE;
        m_UserName.Empty();
        m_NTMachineName.Empty();

        m_pDFJob = NULL;
        m_pDFIgnoreJob = NULL;
        m_hProcess = 0;
        m_pWinThread = NULL;
        m_Saved = TRUE;
    };
	virtual ~CDFResource()
    {
    };

};

typedef CTypedPtrArray<CObArray, CDFResource*> CDFResourceList;

#endif // !defined(AFX_DFRESOURCE_H__9870F913_4BE1_11D2_B96F_0004ACEC31AA__INCLUDED_)
