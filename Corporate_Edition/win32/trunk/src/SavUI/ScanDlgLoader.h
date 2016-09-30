// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation. All rights reserved.


#pragma once

#include "guard.h"
#define SCAN_DLGS_DYNAMIC
#include "scndlgex.h"


/** CScanDlgLoader 
  * Singleton class that is used to load the scan dlgs dll. If the dll is loaded
  * Then you can get the correct function call from this class
  *
  **/
class CScanDlgLoader
{
public:
    static CScanDlgLoader& GetInstance();

    DWORD CreateVirusFoundDlg(PVIRUSFOUNDDLG pVirusFoundDlg);
    DWORD AddLogLineToVirusFound(CVirusFound *pVirusFoundDlg, char *line, char *Description);

	DWORD CreateResultsView(PRESULTSVIEW ScanView);
	DWORD AddLogLineToResultsView(CResultsView *pResultsViewDlg, LPARAM Data, char *line);
    DWORD AddProgressToResultsView(CResultsView *pResultsViewDlg, PPROGRESSBLOCK pb);


protected:
	// The constructor and destructor are protected so no one else can
	// create/destroy an object.
    CScanDlgLoader(void);
    virtual ~CScanDlgLoader(void);
	// These are declared and not implemented so people can't copy this object.
    void operator= (const CScanDlgLoader&);
    CScanDlgLoader(const CScanDlgLoader&);

    //loads scandlg.dll into memory
    DWORD LoadDll();

    //unloads scandlg.dll from memory
    void UnloadDll();

    //Free set pointers to functions back to NULL
    void SetFuncsToNull();

    //manages the loading process
    DWORD LoadManager();

    //loads the scan dlg functions
    DWORD LoadFuncs();

    //function pointers for scandlgs that are being hidden
private:
    tCreateResultsView                  m_pfnCreateResultsView;
    tAddProgressToResultsView           m_pfnAddProgressToResultsView;
    tAddLogLineToResultsView            m_pfnAddLogLineToResultsView;
    tGetViewThreadHandleFromResultsView m_pfnGetViewThreadHandleFromResultsView;
    tScanDoneToResultsView              m_pfnScanDoneToResultsView;
    tCreateVirusFoundDlg                m_pfnCreateVirusFoundDlg;
    tAddLogLineToVirusFound             m_pfnAddLogLineToVirusFound;
    tGetWindowHandle                    m_pfnGetWindowHandle;
    tGetVirusFoundWindowHandle          m_pfnGetVirusFoundWindowHandle;

private:
    static CScanDlgLoader   s_objScanDlgLoaderInstance;
    bool                    m_bInitialized;
    HMODULE                 m_hScanDlgDll;
    CLock                   g_oScanDlgLock;
};
