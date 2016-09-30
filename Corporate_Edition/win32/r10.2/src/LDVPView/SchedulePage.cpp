// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SchedulePage.cpp : implementation file
//

#include "stdafx.h"
#include "ldvpview.h"
#include "Wizard.h"
#include "SchedulePage.h"
#include "SymSaferRegistry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSchedulePage property page

IMPLEMENT_DYNCREATE(CSchedulePage, CWizardPage)

CSchedulePage::CSchedulePage() : CWizardPage(CSchedulePage::IDD)
{
	//{{AFX_DATA_INIT(CSchedulePage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID( IDD );

}

CSchedulePage::~CSchedulePage()
{
}

void CSchedulePage::DoDataExchange(CDataExchange* pDX)
{
	CWizardPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSchedulePage)
	DDX_Control(pDX, IDC_LIST1, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSchedulePage, CPropertyPage)
	//{{AFX_MSG_MAP(CSchedulePage)
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSchedulePage message handlers

BOOL CSchedulePage::OnInitDialog() 
{
	CWizardPage::OnInitDialog();
	CString strText;

	m_List.InsertColumn(0,"",LVCFMT_LEFT,25,0);
	strText.LoadString(IDS_COL_NAME);
	m_List.InsertColumn(1,strText,LVCFMT_LEFT,100,1);
	strText.LoadString(IDS_COL_WHEN);
	m_List.InsertColumn(2,strText,LVCFMT_LEFT,150,2);
	strText.LoadString(IDS_COL_ORIGIN);
	m_List.InsertColumn(3,strText,LVCFMT_LEFT,70,2);

	//Create the image list
	if( m_ilImages.Create( IDB_HISTORY_IMAGES, 16, 10, RGB( 0x00, 0xff, 0x00 ) ) )
	{
		m_List.SetImageList( &m_ilImages, LVSIL_SMALL );
	}
	else
	{
		TRACE("Failed to create Schedule View Image List!\n" );
	}

	//Enumerate the Admin scans and fill in the list
	GetAdminScans();

	//Enumerate the client scans and fill in the list
	GetClientScans();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
BOOL IsSAVServiceRunning(BOOL& bRunning );

void CSchedulePage::GetAdminScans()
{
	//For admin scans, I need to look under the
	//	LocalScans section
	IScanConfig		*ptrScanConfigEnum = NULL,
					*ptrScanConfig = NULL;
	CConfigObj		configEnum,
					config;
	int				iIndex = 0;
	CString			strOptionName;

	//Start by getting a Virus Protect interface
	if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IScanConfig, (void**)&ptrScanConfigEnum ) ) )
	{
		//Open an unnamed scan
		ptrScanConfigEnum->Open( NULL, HKEY_VP_ADMIN_SCANS, "" );

		//Enum the SubOptions
		configEnum.SetConfig( ptrScanConfigEnum );

		while( !configEnum.EnumSubOptions( iIndex++, strOptionName ) )
		{
			if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IScanConfig, (void**)&ptrScanConfig ) ) )
			{
				//Open an unnamed scan
				ptrScanConfig->Open( NULL, HKEY_VP_ADMIN_SCANS, strOptionName.GetBuffer( strOptionName.GetLength() ) );
				strOptionName.ReleaseBuffer();

				config.SetConfig( ptrScanConfig );

				//Check to see if this is a Scheduled Scan
				config.SetSubOptionReadOnly( szReg_Key_ScheduleKey );

				if( 0 != config.GetOption( szReg_Val_HowOften, 0 ) )
				{
					//Get the scan name
					strOptionName = config.GetOption( szReg_Value_Name, "" );

					//Now, add the information
					AddScan( strOptionName, GetScheduleString( config ), IDS_BY_ADMIN );
				}

				//Release my scan object
				ptrScanConfig->Release();
			}
		}

		//Release the scan config
		ptrScanConfigEnum->Release();
	}

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

void CSchedulePage::GetClientScans()
{
	HKEY			pKey;
	CString			strKeyName,
					strSubKeyName,
					strFilename;
	DWORD			dwIndex = 0,
					dwSize;
	CStringList		listKeys;
	CString			strTaskname;
	POSITION		pos;
	FILETIME		tm;
	TCHAR			szTaskname[124];
	IScanConfig		*ptrScanConfig = NULL;
	CConfigObj		config;

	//The easiest way for me to get the Client scans is to
	//	simply enumerate the scans in the TaskPad Scheduled Scan section
	dwSize = 124;

	//Generate the proper key to hold the values
	strKeyName.Format( "%s\\%s\\%s", szReg_Key_Main, szReg_Key_CustomTasks, szReg_Value_Scheduled );

	//Enumerate all keys under this group
	if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_CURRENT_USER,
											strKeyName,
											0,
											KEY_READ,
											&pKey) )
	{
		//Enumerate through the keys adding all under it
		while( ERROR_SUCCESS == RegEnumKeyEx( 
												pKey,
												dwIndex,
												szTaskname,
												&dwSize,
												NULL,
												NULL,
												NULL,
												&tm ) )

		{
			dwIndex++;

			listKeys.AddTail( CString( szTaskname ) );

			dwSize = 124;
					
		} // End while enumerating

		RegCloseKey( pKey );

		//Now that I have all of the keys, I need to open each one and
		// get the info from them
	
		pos = listKeys.GetHeadPosition();

		while( pos != NULL )
		{
			//Get the item at pos
			strTaskname = listKeys.GetNext( pos );
			TRACE( strTaskname + "\n" );

			//Now, I need to open the scan associated with this name, and
			//	get it's schedule information
			if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IScanConfig, (void**)&ptrScanConfig ) ) )
			{
				//Get the scan info so I can display it
				strSubKeyName.Format( "%s\\%s", strKeyName, strTaskname );
				if( ERROR_SUCCESS == RegOpenKeyEx( HKEY_CURRENT_USER,
													strSubKeyName,
													0,
													KEY_READ,
													&pKey ) )
				{
					dwSize = 124;
					//Read the default value
					SymSaferRegQueryValueEx(pKey,
									NULL,
									NULL,
									NULL,
									(BYTE*)szTaskname,
									&dwSize );
					//And close the key
					RegCloseKey( pKey );
				}

				//Open the scan
				ptrScanConfig->Open( NULL, HKEY_VP_USER_SCANS, szTaskname );

				//Give the pointer to my Config Object
				config.SetConfig( ptrScanConfig );

				//Now, add the information
				AddScan( strTaskname, GetScheduleString( config ), IDS_BY_USER );

				//And release it - I will not use it again
				ptrScanConfig->Release();
			}

		} //End while pos	
	
	} // End IF opened key

#if 0	// keep code here, but only do Admin Scans, otherwise multiple dialogs come up whenever you select View/Scheduled Scans
	//
	// The objective here is to wait until operations are done in Rtvscan
	// before checking whether Rtvscan is up. Rtvscan might take some time 
	// to come up and if operations are completed, we are assured that it is
	// up. 
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

#endif
}

CString CSchedulePage::GetMinutes( DWORD dwMinutes )
{
	CString		strTime;
	SYSTEMTIME	tm;

	memset( &tm, 0x00, sizeof( tm ) );

	tm.wHour = (WORD)(dwMinutes / 60);
	tm.wMinute = (WORD)(dwMinutes % 60);

	GetTimeFormat( LOCALE_SYSTEM_DEFAULT, LOCALE_NOUSEROVERRIDE, &tm, NULL, strTime.GetBuffer( 256 ), 256 );
	strTime.ReleaseBuffer();

	return strTime;
}

CString CSchedulePage::GetScheduleString( CConfigObj &config )
{
	CString strRet,
			strDay,
			strTime;

	//Read the schedule out of the ConfigObj passed in
	config.SetSubOptionReadOnly( szReg_Key_ScheduleKey );

	//Fiogure out the time first
	strTime = GetMinutes( config.GetOption( szReg_Val_MinOfDay, 720 )  );

	switch( config.GetOption( szReg_Val_HowOften, DAILY ) )
	{
		case DAILY:
			AfxFormatString1( strRet, IDS_SCAN_PROMPT_DAILY, strTime );
			break;
		case MONTHLY:
			//Get the day
			strDay.Format( "%u", config.GetOption( szReg_Val_DayOfMonth, 1 ) );
			AfxFormatString2( strRet, IDS_SCAN_PROMPT_MONTHLY, strDay, strTime );
			break;
		case WEEKLY:
			strDay = GetDayName( config.GetOption( szReg_Val_DayOfWeek, 1 ) );
			AfxFormatString2( strRet, IDS_SCAN_PROMPT_WEEKLY, strDay, strTime );
			break;
	}

	//Reset the SubOption
	config.SetSubOptionReadOnly( "" );

	return strRet;
}

void CSchedulePage::AddScan( CString strTaskname, CString strScheduleInfo, DWORD dwID )
{
	LV_ITEM item;
	CString	strOrigin;

	strOrigin.LoadString( dwID );

	memset( &item, 0x00, sizeof( item ) );

	item.mask = LVIF_IMAGE | LVIF_PARAM;
	item.iImage = ( dwID == IDS_BY_USER ) ? 0 : 1;
	m_List.InsertItem( &item );

	m_List.SetItemText( item.iItem, 1, (LPCTSTR)strTaskname );
	m_List.SetItemText( item.iItem, 2, (LPCTSTR)strScheduleInfo );
	m_List.SetItemText( item.iItem, 3, (LPCTSTR)strOrigin );
}

void CSchedulePage::OnDestroy() 
{
	m_ilImages.DeleteImageList();
	CWizardPage::OnDestroy();
}

void CSchedulePage::SizePage( int cx, int cy )
{
	//First, call tha base class version of this
	//		virtual method
	CWizardPage::SizePage( cx, cy );

	//Resize the list control
	m_List.SetWindowPos( &wndTop, 0, 0, cx - 50, cy - 130, SWP_NOMOVE | SWP_NOZORDER );
}

BOOL CSchedulePage::OnWizardFinish() 
{
	//Fire an event telling the App to close the task
	((CLDVPViewApp*)AfxGetApp())->m_ptrCtl->NotifyEndTask();

	return CWizardPage::OnWizardFinish();
}

BOOL CSchedulePage::OnSetActive() 
{
	CWizardPage::OnSetActive();
	// terrym 8-28-00 have to parse the raw resouce string now since
	// both the menu and resource string are enbedded
	CString sMenu,sTaskpad,sRaw;

	CWizard	*ptrParent = (CWizard*)GetParent();
	

	//Set the Context-sensitive ID
	SetHelpID( IDD_TASK_SCHEDULE );
	
	// terrym 8-28-00 have to parse the raw resouce string now since
	// both the menu and resource string are enbedded
	sRaw.LoadString( IDS_TASK_SCHEDULE   );
	ConvertRawMenuString(sRaw,sMenu,sTaskpad);

	//Load the options
	ptrParent->HideBack();
	ptrParent->m_strTitle=sTaskpad;
	ptrParent->SetFinishText( IDS_CLOSE );
	ptrParent->SetWizardButtons( PSWIZB_FINISH );

	return TRUE;
}


void CSchedulePage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	PaintTitle( &dc );	
}

void CSchedulePage::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	CPropertyPage::OnLButtonDown(nFlags, point);

	SetFocus();
}

void CSchedulePage::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	CPropertyPage::OnRButtonDown(nFlags, point);

	SetFocus();
}
