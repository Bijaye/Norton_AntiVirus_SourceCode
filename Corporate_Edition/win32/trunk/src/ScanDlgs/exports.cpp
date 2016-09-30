// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#include "stdafx.h"
#include "scandlgs.h"
#include "ResultsView.h"
#include "virusfound.h"

BOOL g_bEngineStarted = FALSE;
long g_cThreads =0;


DWORD CreateResultsView(PRESULTSVIEW ScanView) 
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CResultsView *dlg = NULL;
	try
	{
		dlg = new CResultsView();
	}
	catch (std::bad_alloc &){}

	if (dlg == NULL)
		return RV_ERROR_NO_MEMORY;

	DWORD cc = dlg->Open(ScanView);

	if (cc != RV_SUCCESS)
		delete dlg;

	return cc;
}


DWORD AddProgressToResultsView(CResultsView *pResultsViewDlg,PPROGRESSBLOCK Block) // local or remote
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return pResultsViewDlg->AddProgress(Block);
}



DWORD AddLogLineToResultsView(CResultsView *pResultsViewDlg,LPARAM Data,char *line)       // local or remote
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return pResultsViewDlg->AddLogLine(Data,line);
}


DWORD AddLogLineToResultsViewEx(CResultsView *pResultsViewDlg,LPARAM Data,char *line,int cSize) // remote
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return pResultsViewDlg->AddLogLine(Data,line,cSize);
}

HANDLE GetViewThreadHandleFromResultsView (CResultsView *pResultsViewDlg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return pResultsViewDlg->GetViewThreadHandle();
}

void ScanDoneToResultsView (CResultsView *pResultsViewDlg)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return pResultsViewDlg->ScanDone();
}

DWORD CreateVirusFoundDlg(PVIRUSFOUNDDLG VirusFound) 
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CVirusFound *dlg = NULL;
	try
	{
		dlg = new CVirusFound();
	}
	catch(std::bad_alloc &){}

	if (dlg == NULL)
		return RV_ERROR_NO_MEMORY;

	DWORD cc = dlg->Open(VirusFound);

	if (cc != RV_SUCCESS)
		delete dlg;

	return cc;
}


DWORD AddLogLineToVirusFound(CVirusFound *pVirusFoundDlg,char *line,char *Description)       // local or remote
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return pVirusFoundDlg->AddVirus(line,Description);
}


HWND GetWindowHandle(CResultsView *pResultsViewDlg) // local
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	return pResultsViewDlg->GetSafeHwnd();
}


HWND GetVirusFoundWindowHandle(CVirusFound *pVirusFoundDlg) // local
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	return pVirusFoundDlg->GetSafeHwnd();
}	

