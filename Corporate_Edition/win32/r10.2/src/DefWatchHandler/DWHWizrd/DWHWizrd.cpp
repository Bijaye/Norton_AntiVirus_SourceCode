// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// DWHWizrd.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <atlbase.h>
#include "DWHWizrd.h"
#include "DWHWizrdDlg.h"
#include "dwhwiz.h"
#include "rwizard.h"
#include "vpcommon.h"
#include "veapi.h"
#include "quaritem.h"
#include "clientreg.h"
#include "wow64helpers.h"
#include "SavrtModuleInterface.h"
#include "assert.h"

#include "QuarantineSessionItem.h"
#include "QuarantineInfectedFileItem.h"
#include "QuarantineRemediationItem.h"
#include "SymSaferRegistry.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "OSMigrat.h" //EA for Apllication migration of Win2K
#ifdef __cplusplus
}
#endif
#include "navver.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL SetRestorePriv(void);

IMP_VPCOMMON_IIDS

typedef BOOL (WINAPI *NAVAPFunctionPointer) ();

// 
// Pointers to AP functions.
// 

///////////////////////////////////////////////////////////////////////////
// DefWatch Wizard Implementation of IScanCallback
interface IScanCallbackImp :  public IScanCallback
{
	BOOL            closeview;
	LONG            m_refcount;
	CDWHWizrdApp*   m_pDWHWizApp;

	IScanCallbackImp() : m_pDWHWizApp(NULL),m_refcount(1),closeview(FALSE) 
    {
        m_pDWHWizApp = (CDWHWizrdApp*)AfxGetApp();
    };

	~IScanCallbackImp() {};
	
	ULONG STDMETHODCALLTYPE AddRef(void)
	{
		return InterlockedIncrement(&m_refcount);
	}

	ULONG STDMETHODCALLTYPE Release(void)
	{
		ULONG r = InterlockedDecrement(&m_refcount);
		if (r==0) delete this;
		return r;
	}
	HRESULT STDMETHODCALLTYPE QueryInterface(const IID & riid,void **ppvObj) 
	{
		GUID iid = _IID_IScanCallback;
		*ppvObj = NULL;
		if (IsEqualIID(riid, iid) || IsEqualIID(riid, IID_IUnknown))
		{
    		*ppvObj =this;
			AddRef();
			return S_OK;
		}
   		return E_NOINTERFACE;
	}

	VI(ULONG) Progress(PPROGRESSBLOCK Progress)
	{
		return S_OK;
	}

	VI(ULONG) RTSProgress(PREALTIMEPROGRESSBLOCK Progress)
	{
		return S_OK;
	}

	VI(ULONG) Virus(const char *line)   
	{
        CQuarItem*      pQuarItem;
        PEVENTBLOCK     pEventBlock;
        DWORD           dwError = 0;
        
        pQuarItem = m_pDWHWizApp->GetCurrentItem();
        
        if ( pQuarItem )
        {
            dwError = m_pDWHWizApp->m_pVBin2->CreateEvent(&pEventBlock, line);

            if ( dwError == ERROR_SUCCESS )
			{
                pQuarItem->SetNewEvent(pEventBlock);
			}

            if ( pEventBlock->RealAction == AC_CLEAN )
            {
                pQuarItem->SetState(QUARITEM_STATE_REPAIRABLE);
            }
        }

		return S_OK;
	}

    VI(ULONG) ScanInfo(PEVENTBLOCK pEventBlk)
	{ 
        return S_OK; 
    }
};


IScanCallbackImp*   g_pScanCallback;

/////////////////////////////////////////////////////////////////////////////
// CDWHWizrdApp

BEGIN_MESSAGE_MAP(CDWHWizrdApp, CWinApp)
	//{{AFX_MSG_MAP(CDWHWizrdApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	//ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDWHWizrdApp construction

CDWHWizrdApp::CDWHWizrdApp()
 :	m_pUtil4( NULL ),
	m_pScan( NULL ),
	m_pScanConfig( NULL ),
	m_pVBin2( NULL ),
	m_pVirusAction2( NULL ),
	m_pRootCliProxy( NULL ),
	m_hMutex( NULL )
{
    m_dwDefwatchMode = DWM_NOTHING; // Default to text in VBin UI
    m_dwWizState = DWH_WIZ_STATE_CONTINUE;
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CDWHWizrdApp object

CDWHWizrdApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDWHWizrdApp initialization

// *************************************************************************
//
// Name:        CDWHWizrdApp::InitInstance()
//
// Description: Do all the startup processing. Note that we allow only one
//              instance of the wizard to run. Do the real work based on the
//              mode setting in the registry.
//
// Parameters:  None
//
// Returns:     Always return FALSE so we tell te MFC's WinMain function
//              to exit the application.
//
// *************************************************************************
// 4/14/99 - TCashin : Function added.
// *************************************************************************
BOOL CDWHWizrdApp::InitInstance()
{
	HRESULT		hInit;
	CString szMigrationMessage;
	CString szAppName;
	//EA Application Migration for Win2K
	if(DetermineOSMigrationMessage() == 95)
	{
		szMigrationMessage.LoadString(IDS_OSMIGRATEDMESS95TO2K);
		szAppName.LoadString(IDS_PROG_NAME);
		::MessageBox(NULL, szMigrationMessage, szAppName, MB_OK|MB_ICONERROR);
		return FALSE;
	}
	else if(DetermineOSMigrationMessage() == 98)
	{
		szMigrationMessage.LoadString(IDS_OSMIGRATEDMESS98TO2K);
		szAppName.LoadString(IDS_PROG_NAME);
		::MessageBox(NULL, szMigrationMessage, szAppName, MB_OK|MB_ICONERROR);
		return FALSE;
	}
	//EA - END Application Migration for Win2K
    // Make sure only one instance of ourselves is loaded.
    m_hMutex = CreateMutex( NULL, FALSE, _T("DefWatchWizardMutex") );
    if( GetLastError() == ERROR_ALREADY_EXISTS )
        {
        CloseHandle( m_hMutex );
        m_hMutex = NULL;
        return FALSE;
        }

	hInit = CoInitialize(NULL);
	if ( hInit != S_OK )
		return FALSE;

    if ( !InitScanner() )
        return FALSE;

    if ( DWH_OK != InitQuarantineItemArray() )
        return FALSE;

    SetDefwatchMode();

    switch ( m_dwDefwatchMode )
    {
        case DWM_SILENT:

            RepairAllItems();

            RestoreFiles();

            break;

        case DWM_REPAIRONLY:

            SetRestorePriv();

            RepairAllItems();

            // Add the repaired files back into Quarantine
            // with the repaired flag set. 
            if ( DWH_OK == SaveRepairedFilesInQuarantine() )
            {
                SetNewDefsKey( DW_NEWDEFS_REPAIRED );
            }

            break;

        case DWM_PROMPT:

            {

            CRepairWizard dlg(NULL);

	        m_pMainWnd = &dlg;

	        int nResponse = dlg.DoModal();

            }

            break;

        case DWM_NOTHING:

            SetNewDefsKey( DW_NEWDEFS_NEW );

            break;
    }

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


// *************************************************************************
//
// Name:        CDWHWizrdApp::InitScanner()
//
// Description: Initialize the LDVP COM Objects needed for scanning.
//
// Parameters:  None
//
// Returns:     TRUE is all the COM objects we need at created.
//
// *************************************************************************
// 4/14/99 - TCashin : Function added.
// *************************************************************************
BOOL CDWHWizrdApp::InitScanner()
{
    BOOL            bReturn = FALSE;
    DWORD           dwError = 0;
    DWORD           latest;
	HRESULT         hr;
	GUID iid_root  = _IID_IVirusProtect,
         iid_vbin2 = _IID_IVBin2,
		 iid_iscan = _IID_IScan2,
		 iid_iscanconfig = _IID_IScanConfig,
         iid_ivirusaction2 = _IID_IVirusAction2,
		 iid_iutil4 = _IID_IUtil4,
		 cliproxy = _CLSID_CliProxy;

	// To get our job done, we need both CliScan and CliProxy.

	// CliProxy will give us interfaces into the service to use latest defs.

	hr = CoCreateInstance(cliproxy,NULL,CLSCTX_INPROC_SERVER,iid_root,(LPVOID*)&m_pRootCliProxy);
	if ( FAILED(hr) ) goto All_Done;

 	// get the IUtil4 Interface

	hr = m_pRootCliProxy->CreateByIID(iid_iutil4,(void**)&m_pUtil4);
	if ( FAILED(hr) ) goto All_Done;

	// Prompt Rtvscan to use the latest defs.
	// This also catches if the service isn't running.
    // Ensure we're on the latest defs before starting scan engine.

	latest = m_pUtil4->UseLatestDefs();
	if( latest != 0 && latest != 1 )
        goto All_Done;
    
	// get the IScanConfig Interface
	hr = m_pRootCliProxy->CreateByIID(iid_iscanconfig,(void**)&m_pScanConfig);
	if ( FAILED(hr) )
        goto All_Done;

	// get the IScan2 Interface
	hr = m_pRootCliProxy->CreateByIID(iid_iscan,(void**)&m_pScan);
	if ( FAILED(hr) )
        goto All_Done;

	// get the IVirusAction2 Interface
	hr = m_pRootCliProxy->CreateByIID(iid_ivirusaction2,(void**)&m_pVirusAction2);
	if ( FAILED(hr) )
        goto All_Done;

	// get the IVBin2 Interface
	hr = m_pRootCliProxy->CreateByIID(iid_vbin2,(void**)&m_pVBin2);
	if ( FAILED(hr) )
        goto All_Done;

	try
	{
		// IScanCallback implemnation: see above
		g_pScanCallback = new IScanCallbackImp;

		// Get the Defwatch scan options.
		//Defect 384717.Open it from a differnt scan key Defwatch so that the logger (value Logger set
		//under this key by install should be Logger_DefWatch) is identifiable
		//when cliscan is scanning a defwatch initiated scan and can log its logger as
		//Defwatch
		dwError = m_pScanConfig->Open( NULL, HKEY_VP_ADMIN_SCANS, szReg_Key_Defwatch_CScan_Repair );

        // Turn off reverse scans for DefWatch repairs.  This is only used to repair
        // infected files aready in quarantine, so we don't need reverse scans.
        DWORD dwReverseScan = 0;
        m_pScanConfig->SetOption( szReg_Val_ReverseLookup, REG_DWORD, (BYTE *)&dwReverseScan, sizeof(dwReverseScan) );

		// Open the scan object
		if ( dwError == ERROR_SUCCESS )
		{
			dwError = m_pScan->Open( g_pScanCallback, m_pScanConfig );

			if ( dwError == ERROR_SUCCESS )
			{
				bReturn = TRUE;
			}
		}
	}
	catch (std::bad_alloc &){}

All_Done:

    return (bReturn);    
}


// *************************************************************************
//
// Name:		CDWHWizrdApp::ExitInstance()	
//
// Description: Cleanup, release our COM objects and mutex.
//
// Parameters:  None
//
// Returns:		Always return DWH_OK
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
int CDWHWizrdApp::ExitInstance()
{
    CleanupQuarantineItemArray();

	if( m_pUtil4 != NULL )
		m_pUtil4->Release();

    if( m_pVirusAction2 != NULL )
        m_pVirusAction2->Release();

    if( m_pVBin2 != NULL )
        m_pVBin2->Release();

    if( m_pScan != NULL )
        m_pScan->Release();

    if( m_pScanConfig != NULL )
        m_pScanConfig->Release();

	if( g_pScanCallback != NULL ) {
        g_pScanCallback->Release();
	}

    if( m_pRootCliProxy != NULL ) 
    {
        m_pRootCliProxy->Release();
    }

    CoUninitialize();

    if( m_hMutex )
        CloseHandle( m_hMutex );

    return( DWH_OK );
}


// *************************************************************************
//
// Name:		CDWHWizrdApp::InitQuarantineItemArray()		
//
// Description: Initialize the array of CQuarItem pointers. Go through
//              the virus bin and create a CQuarItem for each file in the
//              the virus bin.
//
// Parameters:	None
//
// Returns:		DWH_NO_QUARANTINE_ITEMS if the virus bin is empty.
//              DWH_OK otherwise.
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// 1/11/05 - Ksackin : Now filtering to only include virally infected 
//                     quarantine items, since expanded threats can also be
//                     placed in quarantine.
// *************************************************************************
DWORD CDWHWizrdApp::InitQuarantineItemArray()
{
    DWORD       dwError = DWH_OK;
    DWORD       dwVBinIndex = 0;
    DWORD       dwNumInfectedItems = 0;
	VBININFO    stInfo = {0};
    CQuarItem*  pItem = NULL;
    HANDLE      hVBinFind = NULL;
    

    m_pCurrentQuarItem = NULL;

    stInfo.Size = sizeof(VBININFO);

    // Enumerate all the items in the virus bin.
    hVBinFind = m_pVBin2->FindFirstItem(&stInfo);

    // Did we get a valid handle?
    if ( hVBinFind )
    {
		try
		{
			do 
			{
				// Check that it's a quarantine guy (not backup or submitted)
				if ( stInfo.Flags &= VBIN_INFECTED )
				{
                    // We can only repair virally infected items in quarantine.
                    // Expanded threats cannot be "cleaned".
                    if ( IsVirallyInfected( &stInfo ) )
                    {
					    // If we're interested in this guy, make a new CQuarItem and
					    // add its address to the pointer array.
					    pItem = CreateSpecificQuarantineItem( &stInfo );

                        m_arQuarItems.Add((void*)pItem);

					    pItem->SetState(QUARITEM_STATE_QUARANTINED);

					    dwNumInfectedItems++;
                    }
				}

				// Reset the buffer
				memset(&stInfo, 0, sizeof(VBININFO));
				stInfo.Size = sizeof(VBININFO);

			} while( m_pVBin2->FindNextItem(hVBinFind, &stInfo) );

			// We're done.
			m_pVBin2->FindClose(hVBinFind);
		}
		catch (exception e) {}
    }

    // Did we find any infected items?
    if ( dwNumInfectedItems == 0 )
        dwError = DWH_NO_QUARANTINE_ITEMS;

    return (dwError);
}


// *************************************************************************
//
// Name:		CDWHWizrdApp::CleanupQuarantineItemArray()		
//
// Description: Goes through the array of CQuarItem pointers and deletes
//				each object, then makes sure the array itself is empty.
//
// Parameters:  None
//
// Returns:		DWH_OK
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
DWORD CDWHWizrdApp::CleanupQuarantineItemArray()
{
    int         i = 0;
    int         iArraySize = 0;
    CQuarItem*  pItem = NULL;

    iArraySize = m_arQuarItems.GetSize();

    for (i=0;i<iArraySize;i++)
    {
        pItem = (CQuarItem*)m_arQuarItems.GetAt(i);

        delete pItem;
    }

    m_arQuarItems.RemoveAll();

    return (DWH_OK);
}


// *************************************************************************
//
// Name:		CDWHWizrdApp::GetQuarItemArray()		
//
// Description: Get the address of the CQuarItem pointer array
//
// Parameters:	None
//
// Returns:		CPtrArray* - pointer to m_arQuarItems	
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
CPtrArray* CDWHWizrdApp::GetQuarItemArray()
{
    return &m_arQuarItems;
}


// *************************************************************************
//
// Name:		CDWHWizrdApp::GetCurrentItem()		
//
// Description: Get a pointer to the current CQuarItem. This is used by 
//              IScanCallbackImp above to update the current item with the
//              results of a scan.
//
// Parameters:	None
//
// Returns:		CQuarItem* - pointer to the current CQuarItem
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
CQuarItem* CDWHWizrdApp::GetCurrentItem()
{
    return m_pCurrentQuarItem;
}


// *************************************************************************
//
// Name:		CDWHWizrdApp::SetCurrentItem(CQuarItem *pCurrentItem)				
//
// Description: Sets the m_pCurrentQuarItem member
//
// Parameters:  pCurrentItem [in] - pointer the current CQuarItem object.
//
// Returns:		Alway returns TRUE.
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
BOOL CDWHWizrdApp::SetCurrentItem(CQuarItem *pCurrentItem)
{
    m_pCurrentQuarItem = pCurrentItem;

    return TRUE;
}


// *************************************************************************
//
// Name:		CDWHWizrdApp::CheckFiles()		
//
// Description: Iterate through m_arQuarItems and check each item to see
//              if the item state matches the requested state.
//
// Parameters:	None
//
// Returns:		TRUE if any items have been repaired, otherwise FALSE
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
BOOL CDWHWizrdApp::CheckFiles(DWORD dwState)
{
    int         i;
    int         iNumItems;
    int         iCleanedItems = 0;
    CQuarItem*  pItem = NULL;
    BOOL        bReturn = FALSE;


    iNumItems = m_arQuarItems.GetSize();

    for (i=0;i<iNumItems;i++)
    {
        pItem = (CQuarItem*)m_arQuarItems.GetAt(i);

        if (pItem)
        {
            if ( pItem->GetState() == dwState )
            {
                iCleanedItems++;
                bReturn = TRUE;
            }
        }
    }

    return (bReturn);

}


// *************************************************************************
//
// Name:		CDWHWizrdApp::RestoreFiles()		
//
// Description: For every CQuarItem, try to copy its temp file to its
//              original location. If the copy fails because the directory
//              doesn't exist, then try to create it.
//
// Parameters:	None
//
// Returns:		DWH_OK
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
DWORD CDWHWizrdApp::RestoreFiles()
{
    int         i;
    int         iNumItems;
    int         iCleanedItems = 0;
    CQuarItem*  pItem = NULL;
    PEVENTBLOCK pEventBlk = NULL;
    DWORD       dwReturn = DWH_OK;
    LPCTSTR     lpTempFile = NULL;
    BOOL        bFileUpdated = FALSE;
    BOOL        bSaveInQuarantine = FALSE;


    iNumItems = m_arQuarItems.GetSize();

    for (i=0;i<iNumItems;i++)
    {
		// Get a pointer from the array
        pItem = (CQuarItem*)m_arQuarItems.GetAt(i);

        // Restore this item, moving it to backup if successful.
        pItem->Restore( TRUE );
    }

    return (dwReturn);

}

// *************************************************************************
//
// Name:		CDWHWizrdApp::SetDefwatchMode()		
//
// Description: Sets the m_dwDefwatchMode member based on the registry key
//
// Parameters:	None
//
// Returns:		TRUE if we can successfull read the reg key.
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
BOOL CDWHWizrdApp::SetDefwatchMode()
{
    BOOL        bReturn = FALSE;
    HKEY        hKey;
    DWORD       dwSize = sizeof(DWORD);
    DWORD       dwType = REG_DWORD;
    CString     sQuarantineKey;
        
    // Make the string for the Quarantine reg key
    sQuarantineKey.Format("%s\\%s", 
                          szReg_Key_Main, 
                          szReg_Key_Quarantine);

    // Can we open the AdminOnly/Quarantine reg key?
    if ( SUCCEEDED( RegOpenKeyEx ( HKEY_LOCAL_MACHINE,
                                   LPCTSTR(sQuarantineKey),
                                   0,
                                   KEY_READ,
                                   &hKey )))
    {
        // Get the value for the Defwatch mode.
        long lRetVal = SymSaferRegQueryValueEx(hKey,
                                        szReg_Val_DefWatchMode,
                                        NULL,
                                        &dwType,
                                        (LPBYTE) &m_dwDefwatchMode,
                                        &dwSize);

        // Done with the registry.
        RegCloseKey(hKey);

        // Did we read the value successfully?
        if ( lRetVal == ERROR_SUCCESS )
        {            
            bReturn = TRUE;
        }
    }

    return bReturn;
}


// *************************************************************************
//
// Name:		CDWHWizrdApp::GetDefWatchMode()				
//
// Description: Get the value of the m_dwDefwatchMode data member
//
// Parameters:  None
//
// Returns:		m_dwDefwatchMode
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
DWORD CDWHWizrdApp::GetDefWatchMode()
{
    return m_dwDefwatchMode;
}

// *************************************************************************
//
// Name:		CDWHWizrdApp::RepairAllItems()				
//
// Description: For every CQuarItem, repair its infected temp file(s).
//
// Parameters:	None
//
// Returns:		DWH_OK
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
DWORD CDWHWizrdApp::RepairAllItems()
{
    auto    int         iNumItems = 0;
    auto    int         i = 0;
    auto    CQuarItem*  lpItem = NULL;


    iNumItems = m_arQuarItems.GetSize();

    for ( i=0;i<iNumItems;i++)
    {
        lpItem = (CQuarItem*)m_arQuarItems.GetAt(i);

        if ( lpItem )
        {
            lpItem->Repair();
        }
    }

    return DWH_OK;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name: EnableAP
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/31/99 - DBUCHES: Function created / header added 
// 4/11/99 - TCASHIN: Borrowed from QConsole
///////////////////////////////////////////////////////////////////////////////
void CDWHWizrdApp::EnableAP()
{
	SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );
	if( pSAVRT.Get() == NULL ){
		return;
	}

	pSAVRT->ProtectProcess();
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: DisableAP
//
// Description  : 
//
// Return type  : void 
//
//
///////////////////////////////////////////////////////////////////////////////
// 3/31/99 - DBUCHES: Function created / header added 
// 4/11/99 - TCASHIN: Borrowed from QConsole
///////////////////////////////////////////////////////////////////////////////
void CDWHWizrdApp::DisableAP()
{
    SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );
	if( pSAVRT.Get() == NULL ){
		return;
	}

	pSAVRT->UnProtectProcess();
}


// *************************************************************************
//
// Name:		CDWHWizrdApp::SetWizState()				
//
// Description: Set the current state of the wizard.
//
// Parameters:	DWORD dwWizState - See DWH_WIZ_STATE_* in dwhwiz.h
//
// Returns:		None
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
void CDWHWizrdApp::SetWizState(DWORD dwWizState)
{
    m_dwWizState = dwWizState;
}


// *************************************************************************
//
// Name:		CDWHWizrdApp::GetWizState()				
//
// Description: Get the current state of the wizard.
//
// Parameters:	None
//
// Returns:		DWORD dwWizState - See DWH_WIZ_STATE_* in dwhwiz.h
//
// *************************************************************************
// 4/14/99 - TCashin : Function added
// *************************************************************************
DWORD CDWHWizrdApp::GetWizState()
{
    return m_dwWizState;
}


// *************************************************************************
//
// Name:		CDWHWizrdApp::SetNewDefsKey()				
//
// Description: Sets the Quarantine\DefWatchNewDefs reg key
//
// Parameters:	DWORD dwNewDefs - See DW_NEWDEFS_* in vpcommon.h
//
// Returns:		TRUE if the reg key was successfully set with the
//              specified value.
//
// *************************************************************************
// 7/8/99 - TCashin : Function added
// *************************************************************************
BOOL CDWHWizrdApp::SetNewDefsKey(DWORD dwNewDefs)
{
    BOOL        bRetValue = FALSE;
    CString     sQuarantineKey;
    CRegKey     reg;
    LONG        lResult;

    // Make the Quarantine key
    sQuarantineKey.Format( "%s\\%s", szReg_Key_Main, szReg_Key_Quarantine );

    // Open it.
    if ( ERROR_SUCCESS == reg.Open(HKEY_LOCAL_MACHINE, sQuarantineKey) )
    {
        // Set the Value
        lResult = reg.SetDWORDValue( szReg_Val_DefWatchNewDefs, dwNewDefs );

        if( ERROR_SUCCESS == lResult )                                
        {
            bRetValue = TRUE;
        }

        // And close the key.
        reg.Close();
    }  
    
    return bRetValue;
}


// *************************************************************************
//
// Name:		CDWHWizrdApp::SaveRepairedFilesInQuarantine()				
//
// Description: Save all repaired files in the local Quarantine. 
//
// Parameters:	None
//
// Returns:		DWH_OK on success.
//
// *************************************************************************
// 7/8/99 - TCashin : Function added
// *************************************************************************
DWORD CDWHWizrdApp::SaveRepairedFilesInQuarantine()
{
    int         i;
    int         iNumItems;
    CQuarItem*  pItem = NULL;
    DWORD       dwError;

    iNumItems = m_arQuarItems.GetSize();

    for (i=0;i<iNumItems;i++)
    {
		// Get a pointer from the array
        pItem = (CQuarItem*)m_arQuarItems.GetAt(i);

        if ( pItem && (pItem->GetState() == QUARITEM_STATE_REPAIRABLE) )
        {
            // Save this repaired item to quarantine.
            dwError = pItem->SaveToQuarantine();

            // Did it get repaired/added?
            if( dwError == DWH_OK )
            {
                // Save state info.
                pItem->SetState(QUARITEM_STATE_REPAIRED_FILE_IN_QUAR);

                // And make it a backup item
                pItem->SetItemToBackup();
            }
        }
    }

    return DWH_OK;
}


// *************************************************************************
//
// Name:		SetRestorePriv()				
//
// Description: Try to set the SE_RESTORE_NAME privilege.
//
// Parameters:	None
//
// Returns:		TRUE when the SE_RESTORE_NAME privilege is enabled.
//
// *************************************************************************
// 5/8/2002 - TCashin : Function added
// *************************************************************************
BOOL SetRestorePriv( void ) 
{
    TOKEN_PRIVILEGES    tp = {0};
    LUID                luid = {0};
    HANDLE              hToken = NULL;
    BOOL                bRetValue = FALSE;
  
    //
    // Open the process token
    //

    if ( !OpenProcessToken( GetCurrentProcess() , TOKEN_WRITE, &hToken) )
    {
        return bRetValue;
    }


    //
    // Get the privilege value 
    //

    if ( LookupPrivilegeValue( NULL, SE_RESTORE_NAME, &luid ) ) 
    {

        //
        // Set the token privilege structure
        //

        tp.PrivilegeCount = 1;
        tp.Privileges[0].Luid = luid;
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;


        //
        // Enable the privilege
        //

        AdjustTokenPrivileges( hToken, 
                               FALSE, 
                               &tp, 
                               sizeof(TOKEN_PRIVILEGES), 
                               (PTOKEN_PRIVILEGES) NULL, 
                               (PDWORD) NULL); 
 
        //
        // Was the privilege set?
        //

        if ( GetLastError() == ERROR_SUCCESS ) 
        { 
              bRetValue = TRUE; 
        } 
    }


    //
    // Close the token
    //

    if ( hToken )
    {
        CloseHandle( hToken );
        hToken = NULL;
    }

    return bRetValue;
}



// *************************************************************************
//
// Name:		CDWHWizrdApp::SetItemToBackup()				
//
// Description: This method will check a VBININFO structure to see if the
//              item contained within the quarantine record is virally
//              infected.
//
// Parameters:	CQuarItem *pQuarItem [in] - pointer to the current item.
//
// Returns:		TRUE  - Item is virally infected.
//              FALSE - Item is not virally infected (expanded threat?).
//
// *************************************************************************
// 1/11/2005 - KSackin : Function created
// *************************************************************************
BOOL CDWHWizrdApp::IsVirallyInfected( VBININFO *pVBinInfo )
{
    EVENTBLOCK *pEventBlock         = NULL;
    BOOL        bVirallyInfected    = TRUE;  // Default to virally infected in case anything goes wrong.

    // Make sure we have a IVBIN2 interface pointer.
    if ( m_pVBin2 != NULL )
    {
        // Create an event block from our log line.
        if ( ERROR_SUCCESS == m_pVBin2->CreateEvent( &pEventBlock, pVBinInfo->LogLine ) )
        {
            // Until we have a better check, Viruses don't have threat categories.
            // We can't check the VEFILEVIRUS type flags, since event expanded threats
            // are tagged as viruses with an additional threat category to narrow them down.
            if ( ! IsViralCategory( pEventBlock->VirusType ) )
                bVirallyInfected = FALSE;

            // Destroy the event block that we just created.
            m_pVBin2->DestroyEvent( pEventBlock );
        }
    }

    return bVirallyInfected;
}

CQuarItem * CDWHWizrdApp::CreateSpecificQuarantineItem( VBININFO *pVBinInfo )
{
    CQuarItem *pQuarItem = NULL;

    // Do we have a session object?
    switch ( pVBinInfo->dwRecordType )
    {
        // Session containing multple items.
        case VBIN_RECORD_SESSION_OBJECT:
            pQuarItem = new CQuarantineSessionItem(m_pVBin2, pVBinInfo);
            break;

        // Single, legacy-style infected file.
        case VBIN_RECORD_LEGACY_INFECTION:
            pQuarItem = new CQuarantineInfectedFileItem(m_pVBin2, pVBinInfo);
            break;

        case VBIN_RECORD_REMEDIATION:
            pQuarItem = new CQuarantineRemediationItem(m_pVBin2, pVBinInfo);
            break;

        default:
            // This should never happen.  If we ever create new types of base 
            // quarantine items, these need to be specifically handled here.
            SAVASSERT( 0 );
            break;
    };

    return pQuarItem;
}