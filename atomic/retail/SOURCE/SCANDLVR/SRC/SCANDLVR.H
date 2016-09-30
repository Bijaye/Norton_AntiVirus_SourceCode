// ScanDlvr.h : main header file for the SCANDLVR DLL
//

#if !defined(AFX_SCANDLVR_H__8D28F8E7_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_)
#define AFX_SCANDLVR_H__8D28F8E7_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "scandres.h"		// main symbols
#include "ScanCfg.h"        // global configuration data object
#include "IScanDeliver.h"
#include "ScanDeliverDLL.h"



extern HINSTANCE g_hInstance;   // Global instance handle to this module.



/////////////////////////////////////////////////////////////////////////////
// CScanDlvrApp
// See ScanDlvr.cpp for the implementation of this class
//

class CScanDlvrApp : public CWinApp
{
public:
	CScanDeliverDLL *GetDeliverDLL( void );
	BOOL SetDeliverDLL( CScanDeliverDLL *pDll );
	CScanDlvrApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScanDlvrApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CScanDlvrApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


// global objects
extern CScanDeliverConfiguration   g_ConfigInfo;


// exported functions
extern "C" BOOL PASCAL EXPORT StartSarcDeliver(BOOL  bCreateDatFile);


#endif // !defined(AFX_SCANDLVR_H__8D28F8E7_C3FA_11D1_A7C2_0000E8D34392__INCLUDED_)