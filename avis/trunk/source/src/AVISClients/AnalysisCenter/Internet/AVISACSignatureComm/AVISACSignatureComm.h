// AVISACSignatureComm.h : main header file for the AVISACSIGNATURECOMM application
//

#if !defined(AFX_AVISACSIGNATURECOMM_H__2BC75335_E169_11D2_8A97_00203529AC86__INCLUDED_)
#define AFX_AVISACSIGNATURECOMM_H__2BC75335_E169_11D2_8A97_00203529AC86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "MyTraceClient.h"

/////////////////////////////////////////////////////////////////////////////
// CAVISACSignatureCommApp:
// See AVISACSignatureComm.cpp for the implementation of this class
//

class CAVISACSignatureCommApp : public CWinApp
{
public:
	CAVISACSignatureCommApp();
	static void CriticalError(LPCSTR pszFormat, ...);
	static void AutoTerminate(LPCSTR pszMessage = NULL);

private:
	static BOOL s_boAllowUserToRespondToCriticalError;
	static BOOL s_boSoundAlarmOnCriticalError;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAVISACSignatureCommApp)
public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CAVISACSignatureCommApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	static int s_iClientDelayAfterNetworkErrorSecs;
	static MyTraceClient s_tc;
};

// error macros
// return code bases (the AVISTransaction return codes range from 1000-1999)
#define APP_QUIT								100
#define APP_UNKNOWN_ERROR						101

// PostSignatureSet error base
#define PSS_RC_BASE								4000

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AVISACSIGNATURECOMM_H__2BC75335_E169_11D2_8A97_00203529AC86__INCLUDED_)
