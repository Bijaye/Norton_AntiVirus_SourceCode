// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#if !defined(AFX_DEFUPDATEEVENT_H__FF13B857_4309_4475_873D_D4557D920B70__INCLUDED_)
#define AFX_DEFUPDATEEVENT_H__FF13B857_4309_4475_873D_D4557D920B70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DefUpdateEvent.h : header file
//

#ifdef WIN32
#include "ccEveryoneDacl.h"
#endif

/////////////////////////////////////////////////////////////////////////////
// defines
#define DEF_UPDATE_EVENT_NAME _T("HawkingCheckDefs")
#define DEF_UPDATE_EVERYONE_GROUP _T("Everyone")


/////////////////////////////////////////////////////////////////////////////
// CDefUpdateEvent class

class CDefUpdateEvent
{
// Construction
public:
	CDefUpdateEvent();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDefUpdateEvent)
	//}}AFX_VIRTUAL

// Implementation
public:
	DWORD   CloseHandle();
	HANDLE  GetHandle();
	virtual ~CDefUpdateEvent();

	// Generated message map functions
protected:
	DWORD SetupSecurityDescriptor();
	//{{AFX_MSG(CDefUpdateEvent)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
private:
	PACL m_lpACL;
	PSID m_lpGroupSID;
	SECURITY_DESCRIPTOR m_SecDesc;
	SECURITY_ATTRIBUTES m_SecAttr;
	SAV::CEveryoneDACL m_nullDacl;
	PACL m_pDacl;
	HANDLE m_hDefUpdateEvent;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEFUPDATEEVENT_H__FF13B857_4309_4475_873D_D4557D920B70__INCLUDED_)
