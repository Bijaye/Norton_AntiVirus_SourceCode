// AVISACSampleComm.h : main header file for the AVISACSAMPLECOMM application
//

#if !defined(AFX_AVISACSAMPLECOMM_H__87CB27C9_C1C5_11D2_8A91_00203529AC86__INCLUDED_)
#define AFX_AVISACSAMPLECOMM_H__87CB27C9_C1C5_11D2_8A91_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "MyTraceClient.h"

/////////////////////////////////////////////////////////////////////////////
// CAVISACSampleCommApp:
// See AVISACSampleComm.cpp for the implementation of this class
//

class CAVISACSampleCommApp : public CWinApp
{
public:
	static int s_iClientDelayAfterNetworkErrorSecs;
	static MyTraceClient s_tc;

private:
	static BOOL s_boAllowUserToRespondToCriticalError;
	static BOOL s_boSoundAlarmOnCriticalError;

public:
	CAVISACSampleCommApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAVISACSampleCommApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CAVISACSampleCommApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	static void CriticalError(LPCSTR pszFormat, ...);
	static void AutoTerminate(LPCSTR pszMessage = NULL);

};

// return code bases (the AVISTransaction return codes range from 1000-1999)
#define APP_QUIT								100
#define APP_UNKNOWN_ERROR						101

// GetSuspectSample
#define GSS_RC_BASE								2000

// PostSampleStatus
#define PSS_RC_BASE								3000

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVISACSAMPLECOMM_H__87CB27C9_C1C5_11D2_8A91_00203529AC86__INCLUDED_)
