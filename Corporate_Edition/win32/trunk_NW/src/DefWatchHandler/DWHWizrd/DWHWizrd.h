// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// DWHWizrd.h : main header file for the DWHWIZRD application
//

#if !defined(AFX_DWHWIZRD_H__512185C9_E696_11D2_A455_00A02438707D__INCLUDED_)
#define AFX_DWHWIZRD_H__512185C9_E696_11D2_A455_00A02438707D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "vpcommon.h"
#include "quaritem.h"


/////////////////////////////////////////////////////////////////////////////
// CDWHWizrdApp:
// See DWHWizrd.cpp for the implementation of this class
//

class CDWHWizrdApp : public CWinApp
{
public:
	CDWHWizrdApp();

	DWORD           SaveRepairedFile(CQuarItem* pQuarItem);
	DWORD           SaveRepairedFilesInQuarantine();
	BOOL            SetNewDefsKey(DWORD dwNewDefs);
	void            SetWizState(DWORD dwWizState);
	DWORD           GetWizState();
//	DWORD           RepairOneItem(CQuarItem* lpItem);
	DWORD           GetDefWatchMode();
	BOOL            SetDefwatchMode();
	DWORD           RestoreFiles();
	BOOL            CheckFiles(DWORD dwState);
	BOOL            SetCurrentItem(CQuarItem* pCurrentItem);
	CQuarItem*      GetCurrentItem();
	BOOL            IsVirallyInfected( VBININFO *pVBinInfo );
    CQuarItem*      CreateSpecificQuarantineItem( VBININFO *pVBinInfo );

	CPtrArray*      GetQuarItemArray();
	DWORD           CleanupQuarantineItemArray();

	DWORD           InitQuarantineItemArray();
	int             ExitInstance();
	BOOL            InitScanner(void);

    void            EnableAP();
    void            DisableAP();

	IUtil4*         m_pUtil4;
	IScan*          m_pScan;
	IScanConfig*    m_pScanConfig;
	IVBin2*         m_pVBin2;
	IVirusAction2*  m_pVirusAction2;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDWHWizrdApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDWHWizrdApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	IVirusProtect*  m_pRootCliScan;
	IVirusProtect*  m_pRootCliProxy;

	DWORD           m_dwWizState;
	HANDLE          m_hMutex;

	CQuarItem*      m_pCurrentQuarItem;
	CPtrArray       m_arQuarItems;
	DWORD           m_dwDefwatchMode;

protected:
	DWORD           RepairAllItems(void);

private: // don't implement to prevent use
	CDWHWizrdApp( const CDWHWizrdApp& );
	CDWHWizrdApp& operator = ( const CDWHWizrdApp& );
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DWHWIZRD_H__512185C9_E696_11D2_A455_00A02438707D__INCLUDED_)
