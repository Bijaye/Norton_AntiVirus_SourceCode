// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// VirusBinPage.cpp : implementation file
//////////////////////////////////////////////////////////////////////////////////////////////////
//	1/12/2005 KTALINKI - Modified to use IScanAndDeliverDll3, IQuarantineSession interfaces
//	OnScanDeliver method modified to initialize CSarcSession objects and send to Scan And Deliver
//	OnInitDialog Method modified to create IScanDeliverDll3 interface instead of IScanDeliverDll2.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ldvpview.h"
#include "VirusBinPage.h"
#include "Wizard.h"
#include "resultitem.h"
#include "sarcsession.h"
#include "savidefutilsloader.h"
#include "defutilsinterface.h"
#include <atlbase.h>
#include "SymSaferRegistry.h"
#include "ccLibDllLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVirusBinPage property page

IMPLEMENT_DYNCREATE(CVirusBinPage, CWizardPage)

CVirusBinPage::CVirusBinPage() : CWizardPage(CVirusBinPage::IDD)
{
	//{{AFX_DATA_INIT(CVirusBinPage)
	//}}AFX_DATA_INIT
	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID( IDD );

	m_pResultsViewDlg    = NULL;
    m_bCheckDefwatchKeys = TRUE;
    m_pScanDeliver       = NULL;

    CoInitialize(NULL);
}


CVirusBinPage::~CVirusBinPage()
{
    if ( m_pScanDeliver )
    {
        m_pScanDeliver->Release();
        m_pScanDeliver = NULL;
    }

    CoUninitialize();
}


void CVirusBinPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVirusBinPage)
	DDX_Control(pDX, IDC_TITLE, m_ctlTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVirusBinPage, CPropertyPage)
	//{{AFX_MSG_MAP(CVirusBinPage)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UWM_CLOSING, OnResultsDlgClosing )
	ON_MESSAGE(UWM_ADDCHILD, OnCreateScanDlg )
	ON_MESSAGE(UWM_READ_LOGS, OnFillBin )
    ON_MESSAGE(UWM_SCANDELIVER, OnScanDeliver )
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVirusBinPage message handlers

BOOL CVirusBinPage::OnWizardFinish() 
{
	//Fire an event telling the App to close the task
	((CLDVPViewApp*)AfxGetApp())->m_ptrCtl->NotifyEndTask();

	return CWizardPage::OnWizardFinish();
}


BOOL CVirusBinPage::OnSetActive() 
{
	CWizardPage::OnSetActive();
	// terrym 8-28-00 have to parse the raw resouce string now since
	// both the menu and resource string are enbedded
	CString sMenu,sTaskpad,sRaw;

	CWizard	*ptrParent = (CWizard*)GetParent();
	
	// terrym 8-28-00 have to parse the raw resouce string now since
	// both the menu and resource string are enbedded
	sRaw.LoadString( IDS_TASK_VIRUS_BIN    );
	ConvertRawMenuString(sRaw,sMenu,sTaskpad);

	//Load the options
	ptrParent->HideBack();
	ptrParent->m_strTitle= sTaskpad;
	ptrParent->SetFinishText( IDS_CLOSE );
	ptrParent->SetWizardButtons( PSWIZB_FINISH );

    m_szSNDLogString.LoadString(IDS_SCANDLVR);

	return TRUE;
}


void CVirusBinPage::SizePage( int cx, int cy )
{
	//First, call the base class version of this
	//		virtual method
	CWizardPage::SizePage( cx, cy );

	CRect	rect,
			dlgRect;

	m_ctlTitle.GetWindowRect( &rect );
	GetClientRect( &dlgRect );

	ScreenToClient( &rect );
	
	rect.top = rect.bottom + 2;
	rect.bottom = dlgRect.bottom - 10;
	rect.left = 0;
	rect.right = dlgRect.right - 10;

	if( ::IsWindow( m_hChildWnd ) )
		CWnd::FromHandle( m_hChildWnd )->SetWindowPos( &wndTop, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_SHOWWINDOW | SWP_NOZORDER);
}


BOOL CVirusBinPage::OnInitDialog() 
{
	RESULTSVIEW		view;
	WPARAM			wParam = 0;
    HRESULT         hr = 0;
    HINSTANCE       hResource = NULL;

	CPropertyPage::OnInitDialog();

    hResource = AfxGetResourceHandle();

    /*
	12/28/2004	KTALINKI
	Modified to use IID_ScanDeliverDLL3 instead of IID_ScanDeliverDLL2
	hr = CoCreateInstance( CLSID_ScanDeliverDLL,
                            NULL,
                            CLSCTX_INPROC_SERVER,
                            IID_ScanDeliverDLL2,
                            (LPVOID*)&m_pScanDeliver);
	*/

	hr = CoCreateInstance( CLSID_ScanDeliverDLL,
                            NULL,
                            CLSCTX_INPROC_SERVER,
                            IID_ScanDeliverDLL3,
                            (LPVOID*)&m_pScanDeliver);

    AfxSetResourceHandle( hResource );

	memset( &view, 0x00, sizeof( view ) );

	view.Size = sizeof( view );
	view.hWndParent = m_hWnd;
	view.Flags = RV_FLAGS_CHILD;
	view.Type = RV_TYPE_VIRUS_BIN;
	view.GetFileStateForView = GetFileStateForView;
	view.TakeAction = TakeAction;
	view.GetCopyOfFile = GetCopyOfFile;
	view.ViewClosed = ViewClosed;
    view.GetVBinData = GetVBinData;
	view.Context = this;
	view.Title = m_strTaskName.GetBuffer( m_strTaskName.GetLength() );
	view.szFilterSettingsKey = "VirusBinSettings";

    if ( hr == S_OK && m_pScanDeliver )
    {
        view.Flags |= RV_FLAGS_SCANDELIVER;
    }

	if( CreateResultsView && ( RV_SUCCESS == CreateResultsView( &view ) ) )
	{
		m_pResultsViewDlg = view.pResultsViewDlg;
		m_hInst = AfxGetInstanceHandle();
	}
	else
	{
		//Notify of an error
		AfxMessageBox(IDS_SCANDIALOGS_ERROR);
		wParam = -1;
	}

	m_strTaskName.ReleaseBuffer();

	PostMessage( UWM_READ_LOGS, wParam, 0 );

	return TRUE;
}


//----------------------------------------------------------------
// OnResultsDlgClosing
//----------------------------------------------------------------
long CVirusBinPage::OnResultsDlgClosing( WPARAM wParam, LPARAM )
{
	if( wParam == 1 )
	{
		//Now, dismiss the page
		CWizard		*ptrParent = (CWizard*)GetParent();
		ptrParent->PressButton( PSBTN_FINISH );

	}

	m_hChildWnd = NULL;

	return 1;
}


//----------------------------------------------------------------
// OnCreateScanDlg
//----------------------------------------------------------------
long CVirusBinPage::OnCreateScanDlg( WPARAM wParam, LPARAM )
{
	CWizard	*pWiz = (CWizard*)GetParent();

	m_hChildWnd = (HWND)wParam;

	if( pWiz )
	{
		//Now, size the dialog to my size
		CRect rect;

		pWiz->GetWindowRect( &rect );
		SizePage( rect.right - rect.left, rect.bottom - rect.top );
	}

	return 1;
}


void CVirusBinPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	PaintTitle( &dc );	
}


LRESULT CVirusBinPage::AddItemToView( IVBin2 *ptrVBin, VBININFO *pVBinInfoHeadRecord )
{
    IVBinSession *  pVBinSession    = NULL;
    VBININFO        cVBinInfoChild  = {0};
    HANDLE          hVBinFind       = 0;
    PEVENTBLOCK     pEB             = NULL;

    // See if the "infected" bit is set. Ignore the "manual" bit.
    if ( pVBinInfoHeadRecord->Flags & VBIN_INFECTED )
    {
        // First, add this item to the results view.
        AddLogLineToResultsView( m_pResultsViewDlg, NULL, pVBinInfoHeadRecord->LogLine );

        // If it is a session, also, add all of the contained records.
        if ( pVBinInfoHeadRecord->dwRecordType == VBIN_RECORD_SESSION_OBJECT )
        {
            if ( ERROR_SUCCESS == ptrVBin->OpenSession( pVBinInfoHeadRecord->dwSessionID, &pVBinSession ) )
            {
                // See if we can find an item within this session.
                if ( hVBinFind = pVBinSession->FindFirstItem( &cVBinInfoChild ) )
                {
                    do
                    {
                        // Add this item to the results view.
                        AddLogLineToResultsView( m_pResultsViewDlg, NULL, cVBinInfoChild.LogLine );

                        // Initialize our record for the next query.
			            memset( &cVBinInfoChild, 0x00, sizeof( cVBinInfoChild ) );
			            cVBinInfoChild.Size = sizeof( VBININFO );
                    }
                    while( pVBinSession->FindNextItem( hVBinFind, &cVBinInfoChild ) );

                    pVBinSession->FindClose( hVBinFind );
                }
            }                    
        }
    }
    else if( lstrlen(pVBinInfoHeadRecord->LogLine) > 0 && (ERROR_SUCCESS == ptrVBin->CreateEvent(&pEB, pVBinInfoHeadRecord->LogLine)) )
    {
        // Check if the VbinItem is a container with infected items
        if( pEB && pEB->pdfi->bIsCompressed && 
            pEB->pdfi->dwDepthLimit == 0 &&
            (pVBinInfoHeadRecord->Flags & VBIN_INFECTED)   )
        {
            AddLogLineToResultsView( m_pResultsViewDlg, NULL, pVBinInfoHeadRecord->LogLine ); 
        }

        if( pEB )
        {
            ptrVBin->DestroyEvent(pEB);
            pEB = NULL;
        }
    }

	return 0;
}

BOOL IsSAVServiceRunning(BOOL& bRunning );
LRESULT CVirusBinPage::OnFillBin( WPARAM wParam, LPARAM )
{
	CWaitCursor		wait;
	IVBin2			*ptrVBin = NULL;
	DWORD			dwIndex = 0;
	VBININFO		vBinInfo;
    HANDLE          hVBinFind = NULL;

	if( wParam == -1 )
	{
		//End the task
		((CPropertySheet*)GetParent())->PressButton( PSBTN_FINISH );
	}
	else
	{
		//Start by redrawing the window.
		RedrawWindow();

		//If I have a valid VBin object,
		//	I need to enum the items and add them to the 
		//	results view.
		//NOTE: If I couldn't create the results view, then I won't have
		//	a valid VBin pointer, and I will not get into this code.
		if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IVBin2, (void**)&ptrVBin ) ) )
		{
			memset( &vBinInfo, 0x00, sizeof( vBinInfo ) );
			vBinInfo.Size = sizeof( VBININFO );

            // Enumerate all the items in the virus bin.
            hVBinFind = ptrVBin->FindFirstItem(&vBinInfo);

            // Did we get a valid handle?
            if ( hVBinFind )

            {
                // Yep. Now check 'em out
			    do 
			    {
                    // Call our method to add this item, and all contained
                    // items to the view, if applicable.
                    AddItemToView( ptrVBin, &vBinInfo );

                    // Init before getting the next item.
				    memset( &vBinInfo, 0x00, sizeof( vBinInfo ) );
				    vBinInfo.Size = sizeof( VBININFO );

                } while( ptrVBin->FindNextItem(hVBinFind, &vBinInfo) );
        
                // We're done.
                ptrVBin->FindClose(hVBinFind);

            }
			ptrVBin->Release();
		}

		AddLogLineToResultsView( m_pResultsViewDlg, NULL, NULL );

        Invalidate();
		OnPaint();

        if ( m_bCheckDefwatchKeys )
            CheckDefwatchKeys();

		//
		// The objective here is to wait until operations are done in Rtvscan
		// before checking whether Rtvscan is up. Rtvscan might take some time 
		// to come up and if operations are completed, we are assured that it is
		// up. 
		//
		//
			
		HKEY hKey = NULL;
		BOOL bServiceRunning = TRUE; // Default to okay since we don't want to bring up dialog if some of errors occur
		//Find out if the service is running by looking at the registry key
		if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
											szReg_Key_Main,
											0,
											KEY_READ,
											&hKey) )
		{
			DWORD dwRunning = 0;
			DWORD dwSize = sizeof(DWORD);
			if( SymSaferRegQueryValueEx(	hKey,
								szReg_Val_ServiceRunning,
								NULL,
								NULL,
								(BYTE*)&dwRunning,
								&dwSize	) == ERROR_SUCCESS )
			{

				bServiceRunning = (BOOL)(dwRunning != KEYVAL_NOTRUNNING);	
			}
			RegCloseKey( hKey );
		}
		if (!bServiceRunning)
		{
			AfxMessageBox(IDS_SERVICE_CANT_START, MB_OK);
		}
	}
	return 0;
}

//	1/12/05	KTALINKI - Modified to use IScanAndDeliverDll3, IQuarantineSession interfaces
//						Enhancement for Anomaly/Extended Threat
LRESULT CVirusBinPage::OnScanDeliver( WPARAM wParam, LPARAM lParam)
{
    DWORD               dwVBinId,dwVBinSessionID;
    int                 i;
    int                 iNumSelectedItems = 0;
    int                 iItemsInitialized = 0;
	int					iItemsNotInitialized = 0;
    CPtrArray           *pSelectedItemArray = NULL;
    CResultItem         *pSelectedItem = NULL;
    HRESULT             hr;
    //IQuarantineItem     **pItems   = NULL;
    //CSarcItem           *pSarcItem = NULL;
    CString             szFileName;
	CString				szErrorMsg;
    SYSTEMTIME          stDefinitionDate = {0};
    TCHAR               szText[MAX_PATH*2] = {0};  // MAX_PATH is ok for these msgs.
    TCHAR               szCaption[MAX_PATH] = {0}; // MAX_PATH is ok for the caption.
    HINSTANCE           hResource = NULL;
	
	IQuarantineSession  **pItems   = NULL;
    CSarcSession        *pSarcSes = NULL;

    hResource = AfxGetResourceHandle();

    pSelectedItemArray = (CPtrArray*)lParam;

    if (pSelectedItemArray)
        iNumSelectedItems = pSelectedItemArray->GetSize();

    if (iNumSelectedItems == 0)
        goto All_Done;

	// Set the default error message
    ::LoadString(m_hInst, IDS_APP_NAME, szCaption, sizeof(szCaption));
    ::LoadString(m_hInst, IDS_ERROR_SCANDLVR_START, szText, sizeof(szText));

    try
    {
        //pItems = new IQuarantineItem*[iNumSelectedItems];
		pItems = new IQuarantineSession*[iNumSelectedItems];
    }
    catch (...)
    {
		return E_OUTOFMEMORY;
    }

    // Go through the selected items.
    for (i=0;i<iNumSelectedItems;i++)
    {
        pSelectedItem = (CResultItem*)pSelectedItemArray->GetAt(i);  
    
        pSelectedItem->GetVBinID(&dwVBinId);
		pSelectedItem->GetVBinSessionID(&dwVBinSessionID);

        // Create a new CSarcItem object.
        pSarcSes = NULL;
        try
        {
            //pSarcItem = new CSarcItem;
			pSarcSes = new CSarcSession;
        }
        catch (...)
        {
			//Clean up the memory allocated so far
			for (int j=0; j<i; j++)
			{
				pSarcSes = (CSarcSession*)pItems[i];
				delete pSarcSes;
			}
			delete[] pItems;

			return E_OUTOFMEMORY;
        }

        // Convert dwVBinId from a DWORD to a string  to
        // use it as the file name.
        //szFileName.Empty();
        //szFileName.Format("%8X", dwVBinId); 

		if(0 != dwVBinSessionID)
		{
			// Convert dwVBinSessionID from a DWORD to a string  to
			// use it as the file name.

			szFileName.Empty();
			szFileName.Format("%08lX", dwVBinSessionID); 
			// Initialize the new item 
			hr = pSarcSes->Initialize( (LPSTR) szFileName.GetBuffer(8), NULL);			
		}
		else
		{
			// Convert dwVBinId from a DWORD to a string  to
			// use it as the file name.
			szFileName.Empty();
			szFileName.Format("%08lX", dwVBinId); 
			// Initialize the new item 
			hr = pSarcSes->Initialize( NULL, (LPSTR) szFileName.GetBuffer(8) );
		}
        
        if ( FAILED(hr))
        {
            delete pSarcSes;
			iItemsNotInitialized++;
        }
        else
        {
            CString szComputerName;
			CString szTmp = _T("");
			DWORD dwTmp = 0;
            int     iLen;

            // Add the computer name to the CSarcItem
            pSelectedItem->GetComputerName(szComputerName);
            iLen = szComputerName.GetLength();
            pSarcSes->SetOriginalMachineName( szComputerName.GetBuffer(iLen) );
			szComputerName.ReleaseBuffer();

            // Set the log string 
            pSarcSes->SetLogString( LPCTSTR(m_szSNDLogString) );

			//Set AnomalyID and AnamolyCategory
			pSelectedItem->GetAnomalyGuid(szTmp);
            pSarcSes->SetAnomalyID((LPCSTR)szTmp);			

            // Add it to an array of CSarcSession objects
            pItems[iItemsInitialized] = (IQuarantineSession*)pSarcSes;
            iItemsInitialized++;
        }
    }


    if ( iItemsInitialized )
    {
        if ( !m_pScanDeliver )
        {            
			hr = CoCreateInstance( CLSID_ScanDeliverDLL,
                                    NULL,
                                    CLSCTX_INPROC_SERVER,
                                    IID_ScanDeliverDLL3,
                                    (LPVOID*)&m_pScanDeliver);

            if (FAILED(hr))
            {
                ::MessageBox( NULL, szText, szCaption, MB_ICONSTOP|MB_OK);
				goto Cleanup_SarcItems;
            }
        }
        //else
        //{           
            CString     szNavDir;
            CString     szDefsDir;
			TCHAR		szLanguage[MAX_PATH]={0};

			// Get the Language from the String Resource
            ::LoadString ( m_hInst, IDS_LANGUAGE_LOCAL, szLanguage, sizeof ( szLanguage ) );

            // Get the date of the current defs
            if ( !GetDefinitionDate( &stDefinitionDate ) )
            {
                ::MessageBox( NULL, szText, szCaption, MB_ICONSTOP|MB_OK);
                goto Cleanup_SND;
            }

            // Get the NAV directory
            if ( !GetInstallDir(&szNavDir) )
            {
                ::MessageBox( NULL, szText, szCaption, MB_ICONSTOP|MB_OK);
                goto Cleanup_SND;
            }

            // Get the def directory
            if ( !GetDefinitionsDir(&szDefsDir) )
            {
                ::MessageBox( NULL, szText, szCaption, MB_ICONSTOP|MB_OK);
                goto Cleanup_SND;
            }

            // Set the def date
            hr = m_pScanDeliver->SetVirusDefinitionsDate( &stDefinitionDate );
            if ( FAILED(hr) )
            {
                ::MessageBox( NULL, szText, szCaption, MB_ICONSTOP|MB_OK);
                goto Cleanup_SND;
            }

            // Set the def directory
            hr = m_pScanDeliver->SetDefinitionsDir ( LPCTSTR(szDefsDir) );
            if ( FAILED(hr) )
            {
                ::MessageBox( NULL, szText, szCaption, MB_ICONSTOP|MB_OK);
                goto Cleanup_SND;
            }

            // Set the NAV directory
            hr = m_pScanDeliver->SetStartupDir( LPCTSTR(szNavDir) );
            if ( FAILED(hr) )
            {
                ::MessageBox( NULL, szText, szCaption, MB_ICONSTOP|MB_OK);
                goto Cleanup_SND;
            }

            // Set the Scan & Deliver config directory (Same as the NAV dir).
            hr = m_pScanDeliver->SetConfigDir( LPCTSTR(szNavDir) );
            if ( FAILED(hr) )
            {
                ::MessageBox( NULL, szText, szCaption, MB_ICONSTOP|MB_OK);
                goto Cleanup_SND;
            }

            // Set the language
            hr = m_pScanDeliver->SetLanguage( LPCTSTR(szLanguage) );
            if ( FAILED(hr) )
            {
                ::MessageBox( NULL, szText, szCaption, MB_ICONSTOP|MB_OK);
                goto Cleanup_SND;
            }

            // Deliver the files
            hr = m_pScanDeliver->DeliverFilesToSarc(pItems, iItemsInitialized);
            if ( FAILED(hr) )
            {
                ::MessageBox( NULL, szText, szCaption, MB_ICONSTOP|MB_OK);
            }

Cleanup_SND:

            m_pScanDeliver->Release();
            m_pScanDeliver = NULL;
        //}

        AfxSetResourceHandle( hResource );

    }
	else
	{
		// There were no items initialized. Were there uninitialized items?
		if ( iItemsNotInitialized )
		{			
            ::LoadString(m_hInst, IDS_SCANDLVR_ITEMS_ALREADY_SUBMITTED, szText, sizeof(szText));
            ::MessageBox( NULL, szText, szCaption, MB_ICONINFORMATION|MB_OK);
		}
	}

Cleanup_SarcItems:
    // Cleanup all the CSarcItems
    for (i=0;i<iItemsInitialized;i++)
    {
        pSarcSes = (CSarcSession*)pItems[i];
        delete pSarcSes;
    }

    // Delete the array
    delete[] pItems;

All_Done:

    return 0;
}


BOOL CVirusBinPage::GetDefinitionDate(SYSTEMTIME* lpDefDate)
{
    VTIME   vTime;
	DWORD   dwSize = sizeof( VTIME );;
    HKEY    hKey;
    BOOL    bRet = FALSE;


	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										szReg_Key_Main,
										0,
										KEY_QUERY_VALUE,
										&hKey ) )
    {

        if ( ERROR_SUCCESS == SymSaferRegQueryValueEx(	hKey,
			    		                    szReg_Val_PatDate,
				    	                    NULL,
					                        NULL,
					                        (BYTE*)&vTime,
					                        &dwSize ) )
        {
            //Format the time string
            lpDefDate->wYear = vTime.year + 1970;
            lpDefDate->wMonth = vTime.month + 1;
            lpDefDate->wDay = vTime.day;

            bRet = TRUE;
        }

        RegCloseKey(hKey);
    }

    return(bRet);
}

BOOL CVirusBinPage::GetInstallDir(CString *lpInstallDir)
{
    BOOL    bRet = FALSE;
    TCHAR   szDir[MAX_PATH] = {0};
    DWORD   dwSize = MAX_PATH;
    HKEY    hKey;

	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										szReg_Key_Main,
										0,
										KEY_QUERY_VALUE,
										&hKey ) )
    {

        if ( ERROR_SUCCESS == SymSaferRegQueryValueEx(	hKey,
			    		                        szReg_Val_HomeDir,
				    	                        NULL,
					                            NULL,
					                            (BYTE*)szDir,
					                            &dwSize ) )
        {
            //Format the time string
            *lpInstallDir = szDir;
            bRet = TRUE;
        }

        RegCloseKey(hKey);
    }

    return (bRet);
}

BOOL CVirusBinPage::GetDefinitionsDir(CString *pstrDefsDir)
{
    CDefUtilsLoaderHelper objDefUtilsLdr;
    IDefUtils4Ptr   ptrDefUtils;
	
    if (SYM_FAILED (objDefUtilsLdr.CreateObject (ptrDefUtils)))
        return FALSE;

    if (ptrDefUtils->InitWindowsApp("NAVCORP_70") == false)
	{
        const char *szErr = ptrDefUtils->DefUtilsGetLastResultString ();

		// We error out of this method as the actual error does not need to be logged or handled here. This same
		// error will show up in VELoadPatternFile in less than 3 mins, per the call to CheckPattern in the 
		// MainTimer thread. 
		return FALSE;
	}

    TCHAR szDir[MAX_PATH+1];
    if (ptrDefUtils->GetCurrentDefs(szDir, sizeof (szDir)) == FALSE)
	{
        const char *szErr = ptrDefUtils->DefUtilsGetLastResultString ();
		// Same comment as above error.
		return FALSE;
	}

    // It's possible to have corrupt defs that were once loaded by this APP and thus have an entry in usage.dat.
    // GetCurrentDefs doesn't care if the actually defs are there and valid, just that an entry exists.
    // Since the caller is most likely wanting to do something with these defs, we need to make sure to set the path
    // to NULL since these defs are bad.
    if ( CDefUtilsLoaderHelper::GetDefAuthCheck() == true  )
    {
        bool fIsAuthenticate = false;
        if (false == ptrDefUtils->IsAuthenticDefs(szDir, &fIsAuthenticate) || !fIsAuthenticate)
        {
            const char *szErr = ptrDefUtils->DefUtilsGetLastResultString ();
	        return FALSE;
        }

        *pstrDefsDir = szDir;
    }

    return TRUE;
}

void CVirusBinPage::CheckDefwatchKeys()
{
    DWORD       dwNewDefsValue = DW_NEWDEFS_RESET;
    DWORD       dwDefwatchMode = DWM_NOTHING;
    CString     sQuarantineKey;
    CRegKey     reg;
    LONG        lResult;


    // Make the Quarantine key
    sQuarantineKey.Format( "%s\\%s", szReg_Key_Main, szReg_Key_Quarantine );

    // Open it.
    lResult = reg.Open(HKEY_LOCAL_MACHINE, sQuarantineKey);

    if ( lResult == ERROR_SUCCESS )
    {
        // Get the NewDefs value
        lResult = reg.QueryDWORDValue( szReg_Val_DefWatchNewDefs, dwNewDefsValue );

        if ( lResult == ERROR_SUCCESS )
        {
            // Get the Defwatch mode
            lResult = reg.QueryDWORDValue( szReg_Val_DefWatchMode, dwDefwatchMode );
        }
    }  
    
    // If defwatch is in manual mode and new defs arrived,
    // then we need to put up a message box.
    if ( dwDefwatchMode == DWM_NOTHING && dwNewDefsValue == DW_NEWDEFS_NEW )
    {
        // Yep ... Put up a message box.
        AfxMessageBox( IDS_NEW_DEFS, MB_OK );

        // Reset the newdefs value.
        reg.SetDWORDValue( szReg_Val_DefWatchNewDefs, DW_NEWDEFS_RESET );
    }

    // And close the key.
    reg.Close();

    // Set our member variable to FALSE come here again
    m_bCheckDefwatchKeys = FALSE;
}

void CVirusBinPage::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnLButtonDown(nFlags, point);

	SetFocus();
}

void CVirusBinPage::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPropertyPage::OnRButtonDown(nFlags, point);

	SetFocus();
}
