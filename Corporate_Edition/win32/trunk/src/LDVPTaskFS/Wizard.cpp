// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: Wizard.cpp
//  Purpose: CWizard Implementation file. Base class for Task wizards
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "Wizard.h"
#include "vphtmlhelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CWizard, CPropertySheet)


//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CWizard::CWizard( long ID, CWnd* pParentWnd )
:CClientPropSheet(_T("NOT USED"), pParentWnd, 0)
{
	//Set the Wizard mode
	SetWizardMode();
	
	m_pSelectedScanTypePage = NULL;

	m_bInitialized = FALSE;

	m_psh.dwFlags |= PSH_HASHELP;

	//Set the user mode for the task
	m_bUserTask = m_ptrCtrl->IsUserTask();
	m_strTaskName = m_ptrCtrl->GetTaskName();

	switch( ID )
	{
	case TASK_ID_SCAN_FLOPPY:
		m_strTaskName.Format( "%s (%s)", szReg_Key_ManualTask, m_ptrCtrl->GetTaskName() );
		AddPage( &scanFloppyPage );
		break;

	case TASK_ID_SCAN_SELECTED:
		m_strTaskName.Format( "%s (%s)", szReg_Key_ManualTask, m_ptrCtrl->GetTaskName() );
		AddPage( &scanSelectedPage );
		break;

	case TASK_ID_QUICK_SCAN:
		m_strTaskName.Format( "%s (%s)", szReg_Key_ManualTask, m_ptrCtrl->GetTaskName() );
		AddPage( &quickScanPage );
		break;

	case TASK_ID_FULL_SCAN:
		m_strTaskName.Format( "%s (%s)", szReg_Key_ManualTask, m_ptrCtrl->GetTaskName() );
		AddPage( &fullScanPage );
		break;

	case TASK_ID_SCAN_PCHEALTH:
		AddPage( &scanPCHealthPage );
		break;

	case TASK_ID_SCAN_RECORD:
		{
		//Get the count of already loaded pages and remove them
			int nPgIndex = 0, nPgCnt = GetPageCount();
			for(nPgIndex; nPgIndex < nPgCnt; nPgIndex++)
			{
				RemovePage(nPgIndex);
			}
		}

		m_ptrCtrl->SetRecording( TRUE );
		AddPage( &selectScanTypePage );
		AddPage( &scanSavePage );
		//If we are recording a schedule, add the pages to gather the info.
		if( m_ptrCtrl->RecordingSchedule() )
		{
			//Add the pages for the Scheduled scan
			AddPage ( &schedOptionsPage );
		}
		break;

	case TASK_ID_SCAN_CUSTOM:
		AddPage( &scanCustomPage );
		break;

	default:
		return;
	}
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CWizard::~CWizard()
{
	int iPage = GetPageCount() -1;

	//The CPropertyPage object itself is not destroyed in RemovePage.
	//So, the object created on heap is being deleted.
	if(m_pSelectedScanTypePage)
	{
		delete m_pSelectedScanTypePage;
		m_pSelectedScanTypePage = NULL;
	}
	//Remove all of my pages
	for( ; iPage >= 0; iPage-- )
	{
		RemovePage( iPage );
	}
}


BEGIN_MESSAGE_MAP(CWizard, CPropertySheet)
	//{{AFX_MSG_MAP(CWizard)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_COMMAND( UWM_DESTROYSCAN, OnDeleteScan )
	ON_BN_CLICKED( IDC_MSC, OnMscButton )
END_MESSAGE_MAP()

//----------------------------------------------------------------
// OnInitDialog
//----------------------------------------------------------------
BOOL CWizard::OnInitDialog()
{
	InitEngine();

	CClientPropSheet::OnInitDialog();

	ModifyStyleEx(0,WS_EX_CONTROLPARENT);

	m_bInitialized = TRUE;

	return TRUE;
}

//----------------------------------------------------------------
// OnClose
//----------------------------------------------------------------
void CWizard::OnClose()
{
	CClientPropSheet::OnClose();
}

//----------------------------------------------------------------
// Scan
//----------------------------------------------------------------
void CWizard::Scan( IScanCallback* pCallback /* = NULL */ )
{
	CWnd *ptrWnd = GetDlgItem( 0x3025 );

	//Since I added the ability to programatically
	//	start a scan, I need to ensure that the
	//	FINISH button is enabled.


	if( ptrWnd->IsWindowEnabled() )
	{
		CConfigObj	config( GetScanConfig() );
		CString		strTemp = m_strTitle;

		//Start by saving out the title of the scan
		
		//Remove any &'s in the name
		RemoveChars( strTemp, _T('&')  );

		config.SetOption( szReg_Value_ScanTitle, strTemp );
//		config.SetOption( szReg_Value_DisplayStatusDlg, (DWORD)15 );

		DWORD dwDisplayStatusDialog = 0 ;
		dwDisplayStatusDialog = config.GetOption( szReg_Value_DisplayStatusDlg, (DWORD)1 );
		config.SetOption( szReg_Value_DisplayStatusDlg, dwDisplayStatusDialog );

		//If we are recording a scan, then I am done. I just need
		//	to send the events to the container and ensure that
        //  the autodelete mode for newly created reg keys is turned off.
		if( m_ptrCtrl->Recording() )
		{
            // NOTE: GetScanConfig() does NOT AddRef on the interface returned, so no Release.
    	    IConfig		*pConfig = GetScanConfig();

	        if( pConfig )
	        {
                pConfig->ChangeCloseMode( FALSE );
	        }

			//Now, fire an event giving the name and description to the
			//	app.
			m_ptrCtrl->NotifyNameDescription( m_strTitle, m_strDescription );
		}
		else
		{
			//Validate the settings first
			if( !ValidateSettings() )
			{
				//In the ValidateSettings method, I have asked
				//	if they want to continue the scan or not.
				//	If not, I need to simply return.
				return;
			}

			//Start the scan and end the task
			CConfigObj config( GetScanConfig() );

			config.SetOption( szReg_Value_ScanStatus, (DWORD)S_NEVER_RUN );
			config.SetOption( szReg_Value_Logger, (DWORD)LOGGER_Manual );

			IScan		*pScan = GetScan( pCallback );
			DWORD		dwErr = 0;

			if( pScan )
			{
				if( 0 != ( dwErr = pScan->StartScan(TRUE /*Launch Async*/) ) )
				{
					CString strErr,
							strMsg;

					strErr.Format( "0x%X", dwErr );
					AfxFormatString1( strMsg, IDS_START_SCAN_ERROR, strErr );

					AfxMessageBox( strMsg );
				}

				pScan->Release();
			}
			else
			{
				AfxMessageBox( IDS_NO_I_SCAN );
			}

		}
		//And finally, fire an event telling the App to close the task
		m_ptrCtrl->NotifyEndTask();
	}

}

#define NUM_TO_DRIVE( n ) (TCHAR)(_T('a') + (n-1))

//---------------------------------------------------------------
// ValidateSettings
//---------------------------------------------------------------
BOOL CWizard::ValidateSettings()
{
	CString		strFoundDrives,
				strSubKey;
	DWORD		dwIndex = 0;
	CString		strPath, strPath2;
	CConfigObj	config( GetScanConfig() );
    UINT uDrive = DRIVE_UNKNOWN;

	for( int x = 0; x < 2; x++ )
	{

		if( x == 0 )
		{
			//Set the current sub-key to the Directories key
			config.SetSubOption( szReg_Key_Directories );
		}
		else
		{
			//Set the current sub-key to the Directories key
			config.SetSubOption( szReg_Key_Files );
		}


		//Enumerate the options
		CConfigValue	value;

		while( S_OK == config.EnumOptions( dwIndex++, strPath2, value ) )
		{
            strPath = strPath2;
			int		iIndex = strPath.Find( _T(',') );
			CString	strTemp = strPath.Mid( 1, iIndex - 1 );
            CString strDriveOnly(_T("A:"));
            uDrive = DRIVE_UNKNOWN;

            // Release data from value since we have no use for the data
            value.SetData(0,0, NULL);

			//If the item is NOT a filesystem item, then go to the next one
			if( strTemp.CompareNoCase( _T("filesystem") )  )
            {
                // check for the just the drive using the format "A:" instead of
                // the old format for the filesystem specification.  The new format
                // is being stored in the registry to save the maximum possible LFN.
			    if( strPath.GetLength() != strDriveOnly.GetLength() )
                {
                    strTemp.Empty();
                    continue;
                }
                              
                TCHAR ch = strPath2[0];
                CString strA = _T("A");
                TCHAR chA = strA[0];
                int ID = ch - chA;

                uDrive = GetDriveType(strPath2);
                if(uDrive == DRIVE_REMOVABLE)
                {            
                    strPath.Format("[FileSystem,%d]", ID+1);
			        iIndex = strPath.Find( _T(',') );
                }
                else if(uDrive == DRIVE_CDROM)
                {            
                    strPath.Format("[FileSystem,%d]", ID+128);
			        iIndex = strPath.Find( _T(',') );
                }
                else
                {
                    strTemp.Empty();
                    continue; 
                }
            }


			//I know I am looking at a filesystem item, now parse off
			//	the drive number & see if it is a removeable drive
			iIndex += 1;

			strTemp = strPath.Mid( iIndex, strPath.Find(_T(']') ) - iIndex );
			iIndex = atoi( (LPCTSTR)strTemp );
			
            strTemp.Empty();

			//All removeable drives will be lower than 127
			if( iIndex >= 127 )
            {
                if(uDrive != DRIVE_CDROM)
				    continue;
            }

			//Make sure this drive is not in my found drives string
			if( -1 == strFoundDrives.Find( strPath.Left(1) ) )
			{
				BOOL 			bRetry = FALSE;
				UINT			uintPrevState = SetErrorMode( SEM_FAILCRITICALERRORS );
				WIN32_FIND_DATA	info;
				CString			strDrive;
				HANDLE			hFind;
				
				do
				{
					//Start by setting Retry to FALSE. This is so
					//	it will be reset each time I go through the
					//	loop.
					bRetry = FALSE;

                    if(uDrive == DRIVE_CDROM)
					    strDrive.Format( "%s\\*.*", strPath2 );	    
                    else
					    strDrive.Format( "%c:\\*.*", NUM_TO_DRIVE(iIndex) );
                        	    
					memset( &info, '\0', sizeof( info ) );
					
					//If the FindFirst fails, then we can't get to the
					//	drive.
					if( INVALID_HANDLE_VALUE != ( hFind = FindFirstFile( (LPCTSTR)strDrive,  &info ) ) )
					{
						//Add the drive to my "cache"
                        if(uDrive == DRIVE_CDROM)
                            strFoundDrives += NUM_TO_DRIVE(iIndex-128);
                        else
						    strFoundDrives += NUM_TO_DRIVE(iIndex);

						bRetry = FALSE;
					}
					else // Empty CDROM drive or removable drive.
					{    
                        if(GetLastError() == ERROR_NOT_READY)
                        {
   							CString strError;
    
                            if(uDrive == DRIVE_CDROM)       
                            {
							    strDrive.Format( "%s\\", strPath2 );
                            }
                            else
                            {
                                strDrive.Format( "%c:\\", NUM_TO_DRIVE(iIndex) );
                            }	    
					
							AfxFormatString1( strError, IDS_NOT_READY, (LPCTSTR)strDrive );
							if( IDRETRY == AfxMessageBox( strError, MB_RETRYCANCEL | MB_ICONEXCLAMATION ) )
							{   
								bRetry = TRUE;
							}
							else
							{
								FindClose( hFind );
								config.SetSubOption( "" );
                                SetErrorMode(uintPrevState);
								return FALSE;
							}
                        }
                    }

					FindClose( hFind );
                    
				}while( bRetry );
										
				SetErrorMode( uintPrevState );
			} //End if Removable

		} //end enum
	} //end for

	//Return to the root option
	config.SetSubOption( "" );

	return TRUE;
}

void CWizard::OnDeleteScan()
{
	CPropertyPage *ptrPage = GetActivePage();

	if( ptrPage == &scanCustomPage )
	{
		GetActivePage()->SendMessage( WM_COMMAND, MAKEWPARAM( UWM_DESTROYSCAN, 0 ), 0L );
	}
}

void CWizard::OnDestroy() 
{
	CPropertyPage *ptrPage = GetActivePage();

	if( ptrPage )
	{
		//Now, inform the active child page that
		//	it is about to be destroyed so it can
		//	do what it needs to do with the open scan.
		GetActivePage()->SendMessage( WM_COMMAND, MAKEWPARAM( UWM_DESTROYING, 0 ), 0L );
	}
	
	DeInitEngine();

	CPropertySheet::OnDestroy();

}

void CWizard::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( GetActivePage() )
		GetActivePage()->SetFocus();
}

void CWizard::OnRButtonDown(UINT nFlags, CPoint point)
{
	if( GetActivePage() )
		GetActivePage()->SetFocus();
}

void CWizard::ShowWindow(int nCmdShow)
{
	CPropertySheet::ShowWindow(nCmdShow);
	//EA 03/13/2000
	//Force a lbutton down of the mouse so that it gets the mouseactivate message
	//this is done so that we can still tab after help exits
	CWnd* pWnd;
	pWnd = AfxGetMainWnd();
	if(pWnd)
	{
		if(pWnd->m_hWnd)
			PostMessage(WM_MOUSEACTIVATE, (WPARAM)pWnd->m_hWnd,(MAKELONG(HTCLIENT,WM_LBUTTONDOWN)) );
	}
	//EA 03/13/2000
	
}
CWnd* CWizard::SetFocus()
{
	//EA 03/13/2000
	//fix for STS 330451 where when dialog creation of custom scan, new scan and scheduled scan
	//occurs the focus is on help shifted the focus to next
	CWnd* pWnd = CPropertySheet::SetFocus();
	CWnd* pNextWnd = GetDlgItem(ID_WIZNEXT);
	if(pNextWnd)
		pNextWnd->SetFocus();
	return pWnd;
	//EA 03/13/2000
}

void CWizard::WinHelpInternal(DWORD dwData, UINT nCmd) 
{
    CString strHelpFile = SAV_HTML_HELP;

	::HtmlHelp( AfxGetMainWnd()->GetSafeHwnd(),
              strHelpFile,
              HH_HELP_CONTEXT,
              dwData );
}

//----------------------------------------------------------------
// AddSelectedScanTypePage
//	PURPOSE		Adds the page for the selected scan type.
//	ARGUMENTS	
//				none
//	RETURNS
//				none
//	REVISIONS
//				10\15\2004 Srikanth - Added the method
//----------------------------------------------------------------
void CWizard::AddSelectedScanTypePage()
{
	switch( m_dwSelectedScanType )
	{
    default:

        SAVASSERT( FALSE ); // add handling for new scan types
        // intentionally fall through to handle corrupt settings

	case SCAN_TYPE_CUSTOM:

		m_pSelectedScanTypePage = (CPropertyPage*)new CScanSelectedPage;
		AddPage( m_pSelectedScanTypePage );
		break;

	case SCAN_TYPE_QUICK:

		m_pSelectedScanTypePage = new CQuickScanPage;
		AddPage( m_pSelectedScanTypePage );
		break;

	case SCAN_TYPE_FULL:

		m_pSelectedScanTypePage = new CFullScanPage;
		AddPage( m_pSelectedScanTypePage );
		break;
	}
}

//----------------------------------------------------------------
// RemoveSelectedScanTypePage
//	PURPOSE		Removes the already added page for the selected scan type.
//	ARGUMENTS	
//				none
//	RETURNS
//				none
//	REVISIONS
//				10\18\2004 Srikanth - Added the method
//----------------------------------------------------------------
void CWizard::RemoveSelectedScanTypePage()
{	
	//Gets the count of already loaded pages. The specific scan type page 
	//is the third one and if it is already loaded, remove it.
	//This will let us load the scan type page at run time, when the 
	//user changes the scan type by going back to the first page.
	int nPgCnt = GetPageCount();
	int scanTypePage = NULL;
	if( m_ptrCtrl->RecordingSchedule() )
		scanTypePage = 4;
	else 
		scanTypePage = 3;
	if(nPgCnt == scanTypePage)
	{
		//The CPropertyPage object itself is not destroyed in RemovePage.
		//So, the object created on heap is being deleted.
		if(m_pSelectedScanTypePage)
		{
			delete m_pSelectedScanTypePage;
			m_pSelectedScanTypePage = NULL;
		}
		RemovePage(nPgCnt-1);
	}
}

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
