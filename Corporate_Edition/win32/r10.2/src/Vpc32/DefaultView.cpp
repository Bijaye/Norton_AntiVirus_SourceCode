// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: DefaultView.cpp
//  Purpose: CDefaultView Implementation file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "wprotect32.h"
#include "DefaultView.h"
#include "MainFrm.h"
#include "WProtect32doc.h"
#include "ClientReg.h"

#include "scaninf.h"
#include "scaninf_i.c"
#include "vdbversion.h"
#include <atlbase.h>
#include "admininfo.h"
#include "savidefutilsloader.h"
#include "defutilsinterface.h"
#include "oem_obj.h"
#include "SymSaferRegistry.h"
#include "ccLibDllLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bEnableServicesCheckBox;

/////////////////////////////////////////////////////////////////////////////
// CDefaultView

IMPLEMENT_DYNCREATE(CDefaultView, CFormView)

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CDefaultView::CDefaultView()
	: CFormView(CDefaultView::IDD)
{
	//{{AFX_DATA_INIT(CDefaultView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bParentServerToolTipInitialized = FALSE;
	m_pParentServerToolTip = NULL;
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CDefaultView::~CDefaultView()
{
	if (m_pParentServerToolTip != NULL) {
		delete m_pParentServerToolTip;
	}
}

//----------------------------------------------------------------
// DoDataExchange
//----------------------------------------------------------------
void CDefaultView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDefaultView)
	DDX_Control(pDX, IDC_IMAGE, m_image);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDefaultView, CFormView)
	//{{AFX_MSG_MAP(CDefaultView)
	ON_BN_CLICKED(IDC_UPDATE_PATTERNFILE, OnUpdatePatternfile)
	ON_BN_CLICKED(IDC_SCHEDULE_UPDATES, OnScheduleUpdates)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_EXIT, OnExit)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE( NAV_CHECK_DEF_VERSION, OnUpdateDisplayedDefVersion )
	ON_MESSAGE(WM_UPDATE_QUARANTINE_STATUS, OnUpdateQuarantineStatus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDefaultView diagnostics

#ifdef _DEBUG
void CDefaultView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDefaultView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDefaultView message handlers


//----------------------------------------------------------------
// OnInitialUpdate
//----------------------------------------------------------------
void CDefaultView::OnInitialUpdate() 
{
	LOGFONT	lf;
	CFont	*pFont;
	CWnd	*pWnd;

	CFormView::OnInitialUpdate();
	
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit( !((CMainFrame*)GetParentFrame())->GetFirstTime() );

	// check if m_pParentServerToolTip already exists and if it does then use it, otherwise create a new CToolTipCtrl
	if ( m_pParentServerToolTip == NULL )
	{
		try
		{
			m_pParentServerToolTip = new CToolTipCtrl;
		}
		catch (...)
		{
		}

		m_bParentServerToolTipInitialized = m_pParentServerToolTip->Create(this);

		if( !m_welcomeFont.m_hObject )
		{
			pFont = GetFont();
			pFont->GetLogFont( &lf );
			//lf.lfWeight = 700;
			lf.lfHeight += 35;
			m_welcomeFont.CreateFontIndirect( &lf );

			//Set the font into the text control
			if( pWnd = GetDlgItem( IDC_WELCOME ) )
				pWnd->SetFont( &m_welcomeFont );
		}

		GetRegInfo();
	}
}

//----------------------------------------------------------------
// GetRegInfo
//----------------------------------------------------------------
void CDefaultView::GetRegInfo()
{
	HKEY	pKey;
	CString	strTemp,
			strSubKey,
			strVendor;
	DWORD	dwSize,
			dwID,
			dwClientType = GetClientType(),
			dwTemp,
            dwEnableAllUsers;
	int		iLock;
	CWnd	*ptrWnd;
	CTime	cTime;
	BYTE	*pTemp;
    BOOL    bCanUpdateRegKey = FALSE;
    BOOL    bEnableServicesCheckBox = TRUE;
	BOOL    bNormalUser = FALSE;

    CAdminInfo AdminInfo;

    bCanUpdateRegKey = AdminInfo.CanProcessUpdateRegKey( NULL, HKEY_LOCAL_MACHINE, szReg_Key_Main );

	//First, get the info from the main key
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
										szReg_Key_Main,
										0,
										KEY_READ,
										&pKey) )
	{
		//I need to determing the Scan Engine Vendor
		// so I know what to send to VDBVersion
		SymSaferRegQueryValueEx(	pKey,
							szReg_Val_ScanEngineVendor,
							NULL,
							NULL,
							NULL,
							&dwSize );
		if( dwSize )
		{
			pTemp = (BYTE*)strVendor.GetBuffer( dwSize );
			SymSaferRegQueryValueEx(	pKey,
								szReg_Val_ScanEngineVendor,
								NULL,
								NULL,
								pTemp,
								&dwSize );

			strVendor.ReleaseBuffer();
			strVendor.MakeUpper();
		}

		//First I want to destroy my locks. I do this because
		//	I want to bew sure and have the Tip updated if the
		//	control changed state. Since this can only be specified
		//	on create, I will destroy the locks and re-create them
		// below.
		if( ::IsWindow( m_updateLock.m_hWnd ) )
			m_updateLock.DestroyWindow();

		//Get the Pattern File Version
		if( ptrWnd = GetDlgItem( IDC_PATTERN_FILE_VERSION ) )
		{
		
			dwSize = sizeof( DWORD );
			SymSaferRegQueryValueEx(	pKey,
								szReg_Val_PatVersion,
								NULL,
								NULL,
								(BYTE*)&dwTemp,
								&dwSize );
		
			//Format the time string
			VDBVersionString(dwTemp, (LPCTSTR)strVendor, strTemp.GetBuffer(32), 32, 0);
			strTemp.ReleaseBuffer();
			ptrWnd->SetWindowText( strTemp );
			strTemp.Empty();		
		}
							
		//Get the Parent server
		if( ptrWnd = GetDlgItem( IDC_PARENT_SERVER ) )
		{
			dwSize = 128;
			pTemp = (BYTE*)strTemp.GetBuffer( 128 );
			pTemp[0] = 0; // clear any previous value!
			SymSaferRegQueryValueEx(	pKey,
								szReg_Val_Parent,
								NULL,
								NULL,
								pTemp,
								&dwSize );
		
			strTemp.ReleaseBuffer();
			if( !strTemp.IsEmpty() ) {
				ptrWnd->SetWindowText( strTemp );

				//Set a tooltip (handy in case Fully Qualified Domain Name is too large)
				if (m_pParentServerToolTip != NULL && m_bParentServerToolTipInitialized == TRUE) {
					m_pParentServerToolTip->AddTool(ptrWnd, strTemp);
					m_pParentServerToolTip->Activate(TRUE);
				}
			}


		}

		//Get the client group
		if( ptrWnd = GetDlgItem( IDC_CLIENT_GROUP ) )
		{
			dwSize = 128;
			pTemp = (BYTE*)strTemp.GetBuffer( 128 );
			pTemp[0] = 0; // clear any previous value!
			SymSaferRegQueryValueEx(	pKey,
								szReg_Val_ClientGroup,
								NULL,
								NULL,
								pTemp,
								&dwSize );
		
			strTemp.ReleaseBuffer();
			if( !strTemp.IsEmpty() )
				ptrWnd->SetWindowText( strTemp );
		}

		if( ptrWnd = GetDlgItem( IDC_SCAN_ENGINE_VERSION ) )
		{
		
			dwSize = sizeof( DWORD );
			SymSaferRegQueryValueEx(	pKey,
								szReg_Val_EngVersion,
								NULL,
								NULL,
								(BYTE*)&dwTemp,
								&dwSize );
			
			VEVersionString(dwTemp, (LPCTSTR)strVendor, strTemp.GetBuffer(20), 20);
			strTemp.ReleaseBuffer();
			ptrWnd->SetWindowText( strTemp );
		}

		//Display the product version
		if( ptrWnd = GetDlgItem( IDC_PRODUCT_VERSION ) )
		{
			if ((OEMObj.IsOEMBuild()) && (!OEMObj.IsOEM_TrialApp()))
	            strTemp.Format( "%u.%u.%u.%u (OEM)", MAINPRODUCTVERSION, SUBPRODUCTVERSION, INLINEPRODUCTVERSION, BUILDNUMBER );
			else
				strTemp.Format( "%u.%u.%u.%u", MAINPRODUCTVERSION, SUBPRODUCTVERSION, INLINEPRODUCTVERSION, BUILDNUMBER );
			ptrWnd->SetWindowText( strTemp );
		}
		
		//Now, create the locks on this dialog
		if( dwClientType != CLIENT_TYPE_SERVER )
		{
			HKEY    hLockKey;
			CString strSubKey;
            LONG    lResult;

			// MH 09.06.00
			// We also want to block access to NT non-admin users.  Add the check for
			// normal user b/c non-admin users in NT can update registry entries.
			AdminInfo.IsProcessNormalUser( &bNormalUser );

			// We don't want to be dependent on the 'security' regkey for this so
			// do the admin check here.
			if( bNormalUser )
			{
				bEnableServicesCheckBox = FALSE;
			}

			strSubKey.Format( "%s\\%s\\%s", szReg_Key_Main, szReg_Key_AdminOnly, szReg_Key_Security );
			//Open the proper key for this value
            lResult = RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
												strSubKey,
												0,
												KEY_READ,
												&hLockKey);

			if( ERROR_SUCCESS == lResult || !bCanUpdateRegKey)
			{
				//Can the user turn off the services?
				dwSize = sizeof( DWORD );
				dwTemp = 0;
				SymSaferRegQueryValueEx(	hLockKey,
									szReg_Val_LockUnloadServices,
									NULL,
									NULL,
									(BYTE*)&dwTemp,
									&dwSize );

                // If the registry says the services-related functions are
                // not locked, then we check further. We don't allow not 
                // normal users to change the settings.
                if ( dwTemp == 0 )
                {
                    if ( !bCanUpdateRegKey )
                        dwTemp = 1;
                }

                // Set the bEnableServicesCheckBox flag based on what
                // we think the user is allowed to do.
                if ( dwTemp )
                {
                    bEnableServicesCheckBox = FALSE;
                }

				g_bEnableServicesCheckBox = bEnableServicesCheckBox;

				RegCloseKey( hLockKey );
			}

			strSubKey.Format( "%s\\%s", szReg_Key_Main, szReg_Key_PatternManager );
			//Open the proper key for this value
            lResult = RegOpenKeyEx(	HKEY_LOCAL_MACHINE,
												strSubKey,
												0,
												KEY_READ,
												&hLockKey);

			if( ERROR_SUCCESS == lResult || !bCanUpdateRegKey )
			{
				//Can the user update the pattern file?
				dwSize = sizeof( DWORD );
				dwTemp = 0;
                dwEnableAllUsers = 0;

				SymSaferRegQueryValueEx(	hLockKey,
									szReg_Val_LockUpdatePattern,
									NULL,
									NULL,
									(BYTE*)&dwTemp,
									&dwSize );

                // TC: STS Defect 366765
                // 
                // Get the value of PatternManager/EnableAllUsers

				SymSaferRegQueryValueEx(	hLockKey,
									szReg_Val_EnableAllUsers,
									NULL,
									NULL,
									(BYTE*)&dwEnableAllUsers,
									&dwSize );						

                // If the registry says the pattern-related functions are
                // not locked, then we check further. We allow normal users 
                // to update the defs if EnableAllUsers is set (TC: STS Defect 366765).

                if ( dwTemp == 0 )
                {
                    if ( !bCanUpdateRegKey && !dwEnableAllUsers )
                        dwTemp = 1;
                }

				//Now, create the lock
				dwID = ( dwTemp == 1 ) ? IDS_LOCKED_OPTION : ID_NO_LOCK_TIP;
				iLock = ( dwTemp == 1 ) ? 1 : 0;

				m_updateLock.Create( WS_VISIBLE | WS_CHILD, IDC_UPDATE_PATTERNFILE, this, IDC_UPDATE_PATTERNFILE_LOCK, dwID  );
				m_updateLock.SetLockStyle( 0, LOCK_STYLE_USER );
				m_updateLock.Lock( iLock );

				RegCloseKey( hLockKey );
			}
		}
		else //Now, if we ARE in Server mode, I need to hide/display the proper controls
		{
			//Hide these controls
			if( ptrWnd = GetDlgItem( IDC_PARENT_TEXT ) )
				ptrWnd->ShowWindow( SW_HIDE );

			if( ptrWnd = GetDlgItem( IDC_CLIENT_GROUP ) )
				ptrWnd->ShowWindow( SW_HIDE );

			if( ptrWnd = GetDlgItem( IDC_CLIGROUP_TEXT ) )
				ptrWnd->ShowWindow( SW_HIDE );

			if( ptrWnd = GetDlgItem( IDC_PARENT_SERVER ) )
				ptrWnd->ShowWindow( SW_HIDE );

			//Display these controls
			if( ptrWnd = GetDlgItem( IDC_DOMAIN_TEXT ) )
				ptrWnd->ShowWindow( SW_SHOW );

			if( ptrWnd = GetDlgItem( IDC_DOMAIN ) )
			{
				ptrWnd->ShowWindow( SW_SHOW );

				strTemp.Empty();

				//Get the domain name & set it in the text field
				dwSize = 128;
				pTemp = (BYTE*)strTemp.GetBuffer( 128 );
				SymSaferRegQueryValueEx(	pKey,
									szReg_Val_Domain,
									NULL,
									NULL,
									pTemp,
									&dwSize );
			
				//Format the time string
				strTemp.ReleaseBuffer();
				if( !strTemp.IsEmpty() )
					ptrWnd->SetWindowText( strTemp );
			}

            // We still need to lock LiveUpdate if we're a normal user
            // running in server mode.
            if ( !bCanUpdateRegKey )
            {
				m_updateLock.Create( WS_VISIBLE | WS_CHILD, 
                    IDC_UPDATE_PATTERNFILE, this, IDC_UPDATE_PATTERNFILE_LOCK, IDS_LOCKED_OPTION  );

				m_updateLock.SetLockStyle( 0, LOCK_STYLE_USER );
				m_updateLock.Lock( 1 );
            }
		}

        UpdateQuarantineStatus();

		RegCloseKey( pKey );
	}
}


//----------------------------------------------------------------
// OnUpdatePatternfile
//----------------------------------------------------------------
void CDefaultView::OnUpdatePatternfile() 
{
	((CMainFrame*)AfxGetMainWnd())->OnUpdatePatternfile();	
}

//----------------------------------------------------------------
// OnScheduleUpdates
//----------------------------------------------------------------
void CDefaultView::OnScheduleUpdates() 
{
	((CMainFrame*)AfxGetMainWnd())->OnScheduleUpdates();	
}

//----------------------------------------------------------------
// OnCreate
//----------------------------------------------------------------
int CDefaultView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
//	DWORD dwStyle = GetClassLong( m_hWnd, GCL_STYLE );
//	::SetClassLong( m_hWnd, GCL_STYLE,
//					dwStyle & ~(CS_VREDRAW | CS_HREDRAW ) );
	return 0;
}

//----------------------------------------------------------------
// OnExit
//----------------------------------------------------------------
void CDefaultView::OnExit() 
{
	AfxGetMainWnd()->PostMessage( WM_COMMAND, MAKEWPARAM( ID_APP_EXIT, 0),  0L );
}

//----------------------------------------------------------------
// OnSize
//----------------------------------------------------------------
void CDefaultView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd	*ptrWnd;
	CRect	buttonRect,
			dlgRect;

	CFormView::OnSize(nType, cx, cy);

	//Figure out the current client rect
	GetWindowRect( &dlgRect );
	ScreenToClient( &dlgRect );


	//Move the Exit button
	if( ptrWnd = GetDlgItem( IDC_EXIT ) )
	{
		ptrWnd->GetWindowRect( &buttonRect );
		ScreenToClient( &buttonRect );
		ptrWnd->MoveWindow( dlgRect.right - ( buttonRect.right - buttonRect.left ) - 10, dlgRect.bottom - ( buttonRect.bottom - buttonRect.top ) - 10, buttonRect.right - buttonRect.left, buttonRect.bottom - buttonRect.top );
	}

	//Resize the view
	TRACE( "Size message in Default View!\n" );
}

//----------------------------------------------------------------
// OnUpdateQuarantineStatus
//----------------------------------------------------------------
LRESULT CDefaultView::OnUpdateQuarantineStatus( WPARAM wparam, LPARAM lparam )
{
    CString     strTemp;
    DWORD  dwNumberOfItems = lparam;
	if ( dwNumberOfItems == 1 )
        strTemp.LoadString( IDS_ONE_QUARANTINE_ITEM );
    else
        strTemp.Format( IDS_NUM_QUARANTINE_ITEMS, dwNumberOfItems );
    

    CWnd        *ptrWnd;
    // Finally, if we can get the dialog item and there 
    // is something in the buffer, put it in the window.
    if ( (ptrWnd = GetDlgItem(IDC_QUARANTINE_ITEMS)) && !strTemp.IsEmpty() )
    {
        ptrWnd->SetWindowText( strTemp );
        ptrWnd->ShowWindow( SW_SHOW );
    }

	return( 0 );
}

//----------------------------------------------------------------
// UpdateQuarantineStatus
//----------------------------------------------------------------
void CDefaultView::UpdateQuarantineStatus()
{
    DWORD       dwNewDefsValue = DW_NEWDEFS_RESET;
    DWORD       dwDefwatchMode = DWM_NOTHING;
    CString     sQuarantineKey;
    CString     strTemp;
    CRegKey     reg;
    LONG        lResult;
    BOOL        bCountFilesAndDisplay = TRUE;


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

        // And close the key.
        reg.Close();
    }  
    
    // Did we successfully open and read the reg key?
    if ( lResult == ERROR_SUCCESS )
    {
        // Yep. We were able to read the registry ok.
        // But do we care? Is there something in newdefsvalue?

        if ( dwNewDefsValue != DW_NEWDEFS_RESET )
        {
            switch ( dwDefwatchMode )
            {
                case DWM_SILENT:
                case DWM_REPAIRONLY:

                    if ( dwNewDefsValue == DW_NEWDEFS_REPAIRED )
                    {
                        // Files are in the Repaired View.
                        // Recommend restore.
                        strTemp.Empty();
                        strTemp.LoadString( IDS_QUARANTINE_RESTORE );
                        bCountFilesAndDisplay = FALSE;
                    }

                    break;

                case DWM_PROMPT:

                    // Just count the files. The Defwatch wizard should
                    // have already come up.

                    break;

                case DWM_NOTHING:

                    if ( dwNewDefsValue == DW_NEWDEFS_NEW )
                    {
                        // New defs have arrived. Recommend repair.
                        strTemp.Empty();
                        strTemp.LoadString( IDS_QUARANTINE_REPAIR );
                        bCountFilesAndDisplay = FALSE;
                    }
                        
                    break;
            }
        }
    }

    // Do we simply count and display the 
    // number of Quarantine files?
    if ( bCountFilesAndDisplay )
    {
        strTemp.Empty();
        UpdateQuarantineCount( VBIN_INFECTED );
    }
    
}


//----------------------------------------------------------------
// CountQuarantineItems
//----------------------------------------------------------------
DWORD CDefaultView::UpdateQuarantineCount(DWORD dwQuarantineFlag)
{
    DWORD       dwNumItems = 0;
    // The startup process is complete. Count all quarantine items.
    AfxBeginThread((AFX_THREADPROC)CMainFrame::SetNumQuarantineItems, ((CMainFrame*)GetParentFrame()), THREAD_PRIORITY_NORMAL, 0, 0, NULL);

    return dwNumItems;
}


//----------------------------------------------------------------
// GetDefsDate
//----------------------------------------------------------------
BOOL CDefaultView::GetDefsDate(LPWORD pwYear, LPWORD pwMonth, LPWORD pwDay)
{
    CDefUtilsLoaderHelper objDefUtilsLdr;
    IDefUtils4Ptr   ptrDefUtils;

    if (SYM_FAILED (objDefUtilsLdr.CreateObject (ptrDefUtils)))
        return FALSE;

    // Set up defutils object.
    if (false == ptrDefUtils->InitWindowsApp ("NAVCORP_70"))
	{
        const char *szErr = ptrDefUtils->DefUtilsGetLastResultString();

        // No need to display an error message; any type of error is treated as having a Defs Date of zero.
        return FALSE;
	}

    // Get def information.
    DWORD dwRev;
    if (false == ptrDefUtils->GetCurrentDefsDate (pwYear, pwMonth, pwDay, &dwRev))
	{
        const char *szErr = ptrDefUtils->DefUtilsGetLastResultString();

        // No need to display an error message; any type of error is treated as having a Defs Date of zero.
		return FALSE;
	}
    return TRUE;
}

BOOL CDefaultView::PreTranslateMessage(MSG* pMsg) 
{
	//Tooltip use: See MSDN's Q141758 for details
	if (NULL != m_pParentServerToolTip) {
		m_pParentServerToolTip->RelayEvent(pMsg);
	}
	
	return CFormView::PreTranslateMessage(pMsg);
}

LRESULT CDefaultView::OnUpdateDisplayedDefVersion( WPARAM wparam, LPARAM lparam )
{
	// Refresh the Pattern File Version displayed.

	GetRegInfo();

	return( 0 );
}

