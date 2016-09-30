// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// GetStatus.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "ResultsView.h"
#include "GetStatus.h"
#include "resultitem.h"
#include <afxdlgs.h>
#include "ClientReg.h"
#include "veapi.h"
#include "shfolder.h"
#include "wow64helpers.h"
#include "SavrtModuleInterface.h"
#include "alternatedatastreamsutils.h"
#include "assert.h"
#include "vpcommon.h"
#include "SortedVBinEnumerator.h"
#include "GetStatusHelpers.h" // for IsLocalMachine, WarnAndRestore, CreateMissingDirectory

#ifdef _USE_CCW
#include "symcharconvert.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Local Functions

void wThreadSwitch(void) {

	MSG msg;

	if (PeekMessage(&msg, NULL, 0, 0,PM_NOREMOVE)) {
		if (msg.message == WM_QUIT) {
			// taboo to get a quit message here
			}
		else if (msg.message == WM_PAINT) {
			GetMessage(&msg, NULL, 0, 0);
			}
		else if (msg.message == WM_ENDSESSION && msg.wParam) {
			GetMessage(&msg, NULL, 0, 0);
			}
		else if (msg.message == WM_COMMAND) {
			// do not allow  WM_COMMANDS for this task.
			GetMessage(&msg, NULL, 0, 0);
//			MessageBeep(MB_OK);
			PostMessage(msg.hwnd,msg.message,msg.wParam,msg.lParam);
			}
		else {
			GetMessage(&msg, NULL, 0, 0);
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		return;
		}
}


/////////////////////////////////////////////////////////////////////////////
// CGetStatus dialog

CGetStatus::CGetStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CGetStatus::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetStatus)
	//}}AFX_DATA_INIT
}


void CGetStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetStatus)
	DDX_Control(pDX, IDC_STATUS_TEXT, m_StatusText);
	DDX_Control(pDX, IDHELP, m_Help);
	DDX_Control(pDX, IDCANCEL, m_Cancel);
	DDX_Control(pDX, IDC_ANIMATE1, m_Avi);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_TOTAL_FILES, m_TotalCount);
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetStatus, CDialog)
	//{{AFX_MSG_MAP(CGetStatus)
	ON_WM_DESTROY()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST, OnDeleteitemList)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST, OnItemchangingList)
	ON_BN_CLICKED(IDHELP, OnHelp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CGetStatus message handlers

CGetStatus::CGetStatus(CResultsView *ResultsView)
	: CDialog(CGetStatus::IDD, ResultsView)
{
    TCHAR     szSystemDir[MAX_PATH] = {0};
    TCHAR     szSHFolderDLL[MAX_PATH] = {0};
    TCHAR     szTempPath[MAX_PATH] = {0};
    HINSTANCE hFolderDLL = NULL;
    PFNSHGETFOLDERPATH pfnSHGetFolderPath = NULL;

	m_ResultsView = ResultsView;
	m_bInitialized = FALSE;
    m_dwNumberOfItemsProcessed = 0;
	m_bUserCancel = FALSE;


    //
    // Set up the default restore directory
    // 

    if ( GetSystemDirectory(szSystemDir, sizeof(szSystemDir)) )
    {
        // Append the DLL name
        sssnprintf ( szSHFolderDLL, sizeof(szSHFolderDLL), _T("%s\\shfolder.dll"), szSystemDir );

        // Load it.
        hFolderDLL = LoadLibrary( szSHFolderDLL );

        if ( hFolderDLL )
        {            
            // Get the function
            pfnSHGetFolderPath = (PFNSHGETFOLDERPATH)GetProcAddress( hFolderDLL, "SHGetFolderPathA" );

            if ( pfnSHGetFolderPath )
            {
                if ( SUCCEEDED(pfnSHGetFolderPath( NULL, CSIDL_PERSONAL, NULL, 0, szTempPath)) )
                {
                    m_szDefaultRestoreDir = szTempPath;
                }

            }
            
            FreeLibrary( hFolderDLL );
        }


        if ( m_szDefaultRestoreDir.IsEmpty() )
        {
            //
            // Failed to set the default restore directory. 
            // Use the root of the boot drive.
            //

            LPTSTR lpTemp = _tcschr(szSystemDir, '\\');

            lpTemp = CharNext( lpTemp );

            *lpTemp = 0;

            m_szDefaultRestoreDir = szSystemDir;
        }
    }
}


BOOL CGetStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RECT rect;
	GetClientRect(&rect);
	MinX = rect.right;
	MinY = rect.bottom;

	m_bInitialized = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CGetStatus::OnOK() 
{
	CString str2;
	CString str;
	CString path;
	DWORD action = Action;
	BOOL bPartialSuccess = FALSE;

	SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );

	if (Action == 0xffffffff)
		return;

	m_OK.EnableWindow(FALSE);
	Action = 0xffffffff;

	switch (action) 
	{
		case AC_MOVE:          str.LoadString(IDS_DO_MOVE);    break;
		case AC_RENAME:        str.LoadString(IDS_DO_RENAME);  break;
		case AC_DEL:           str.LoadString(IDS_DO_DELETE);  break;
		case AC_NOTHING:
		case AC_CLEAN:         str.LoadString(IDS_DO_CLEAN);   break;
		case AC_SAVE_AS:       str.LoadString(IDS_DO_SAVE);    break;
		case AC_SEND_TO_INTEL: str.LoadString(IDS_DO_SEND);    break;
		case AC_UNDO:
		case AC_MOVE_BACK:     str.LoadString(IDS_DO_RESTORE); break;
		case AC_RENAME_BACK:   str.LoadString(IDS_DO_RESTORE); break;
	}

	m_StatusText.SetWindowText(str);

	int act = 0,prev;
	int count = m_List.GetItemCount();

	m_Avi.Play(1,-1,-1);
	m_List.SetFocus();

    
	if( pSAVRT.Get() != NULL ){
		pSAVRT->UnProtectProcess();
	}

	for (int i=0;i<count;i++) 
	{
		STATUSITEM *item = (STATUSITEM *)m_List.GetItemData(i);
		prev = act;	// sore the number of actions so far

		if ( m_bUserCancel )
			break;

		if (item) 
		{
			m_List.SetItemState(i,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
			m_List.EnsureVisible(i,FALSE);
			UpdateWindow(); 
			wThreadSwitch();
            
			if (action == AC_SAVE_AS) 
			{
				if (i == 0)  
				{
					char szCurrentDir[IMAX_PATH];

					item->Log->GetFilename(str);

					// 1-3M55AB - save / reset Current Directory of the application
					// to resolve problems with Help - and MS Exchange plugin (crashes)
					strcpy(szCurrentDir, "");
					GetCurrentDirectory(IMAX_PATH, szCurrentDir);

					CFileDialog dlg(FALSE,NULL,(LPCSTR)str);
					if (dlg.DoModal() == IDOK) 
					{
						path = dlg.GetPathName();
					}
					SetCurrentDirectory(szCurrentDir);

				}
			
				CString name;
				item->Log->GetFilename(name);
				int pos = path.ReverseFind('\\');
				if (pos > -1) 
					path = path.Left(pos+1);
				path += name;

				if (m_ResultsView->m_GetCopyOfFile)
#ifdef _USE_CCW
					act += m_ResultsView->m_GetCopyOfFile(m_ResultsView->m_Context,item->Data,(char *)(LPCSTR)item->Log->CreateLogLine(),(wchar_t*)(const wchar_t*)CharToUni((const char *)path),item->State) == RV_SUCCESS?1:0;				
#else	//_USE_CCW
					act += m_ResultsView->m_GetCopyOfFile(m_ResultsView->m_Context,item->Data,(char *)(LPCSTR)item->Log->CreateLogLine(),(char *)(const char *)path,item->State) == RV_SUCCESS?1:0;				
#endif	//_USE_CCW
			}
			else 
			{
				if ( (action == AC_CLEAN) && (item->State&RV_STATE_INBIN))
				{
					act += CleanVBinItem(item);
					if( ERROR_CLEAN_PARTIAL_FAILED == GetResult(item) )
						bPartialSuccess = TRUE;
				}
                else if ( (action == AC_MOVE_BACK) && (item->State&RV_STATE_INBIN))
				{
                    act += UndoVBinItem(item);
					//Srikanth 02/03/05 Defect 1-3DWT4Z
					//As anomaly might comprise of more than one infected files, a partial success per anomaly
					//means that some of the infected files were not restored successfully.
					//Get the result of the restore on the item.
					if( ERROR_MOVE_PARTIAL_FAILED == GetResult(item) )
						bPartialSuccess = TRUE;
				}
				else if (m_ResultsView->m_TakeAction)
                {
                    // Changing AC_UNDO with (item->State&RV_STATE_INBIN) functionality to go 
                    // through the TakeAction processing.  When we attempted to undo items from 
                    // the ScanDialog previously, the undo code attempts to use 
                    // CliScan to walk the VBin items, which would fail
                    // because we can't initialize PScan twice, since we are already running in
                    // RTVScan's process space.  This would actually fail silently for ccEraser
                    // remediations, and tell you that the entire restore succeeded.  Infected
                    // files would actually be restored successfully, but nothing else.

					DWORD dwReturn = m_ResultsView->m_TakeAction(m_ResultsView->m_Context,item->Data,(char *)(LPCSTR)item->Log->CreateLogLine(),action,item->State,true);

                    // If the action partially succeeded, set this to be partial.
                    if ( dwReturn == ERROR_MOVE_PARTIAL_FAILED )
                        bPartialSuccess = true;

                    // If we succeeded in restoring anything, increment action.
                    if (( dwReturn == ERROR_SUCCESS ) || ( dwReturn == ERROR_MOVE_PARTIAL_FAILED ))
                        act++;
                }
			}

			// set the status to success or failed
			CString sStatus;

			DWORD dwType = -1;
			if (action == AC_CLEAN)
			{
				item->Log->GetVirusType(dwType);
			}

			if (act > prev)	// the we added a new action
			{
				if(bPartialSuccess)
				{
					if ( action == AC_CLEAN )
						sStatus.LoadString(IDS_AC_CLEAN_PARTIAL);
					else if ( action == AC_MOVE_BACK )
						sStatus.LoadString(IDS_AC_RESTORE_PARTIAL);
                    else if ( action == AC_UNDO )
						sStatus.LoadString(IDS_AC_UNDO_PARTIAL);
				}
				else if (action == AC_CLEAN)
				{
					if (dwType == 0)
						sStatus.LoadString(IDS_NO_THREAT_CLEANED); // No threat detected and file was cleaned
					else
						sStatus.LoadString(IDS_CLEANED); // Threat detected and file was cleaned
				}
				else 
					sStatus.LoadString(IDS_SUCCEEDED);
			}
			else if (act == prev && dwType == 0)
			{
				if (action == AC_CLEAN)
					sStatus.LoadString(IDS_NO_THREAT_NOT_CLEANED); // No threat detected and file was not cleaned
			}
			else
				sStatus.LoadString(IDS_FAILED);			
			m_List.SetItemText(i,2,sStatus);
		}
	}

    CleanupQuarantine(action);
    
	if( pSAVRT.Get() != NULL ){
		pSAVRT->ProtectProcess();
	}

	switch (action) 
	{
		case AC_MOVE:           str2.LoadString(IDS_AC_MOVE);    break;
		case AC_RENAME:         str2.LoadString(IDS_AC_RENAME);  break;
		case AC_DEL:            str2.LoadString(IDS_AC_DELETE);  break;
		case AC_NOTHING:
		case AC_CLEAN:          str2.LoadString(IDS_AC_CLEAN);   break;
		case AC_SAVE_AS:        str2.LoadString(IDS_AC_SAVE);    break;
		case AC_SEND_TO_INTEL:  str2.LoadString(IDS_AC_SEND);    break;
		case AC_MOVE_BACK:		str2.LoadString(IDS_AC_RESTORE); break;
		case AC_UNDO:       
		case AC_RENAME_BACK:    str2.LoadString(IDS_AC_UNDO);    break;
	}
	str.Format(IDS_GETSTATUSDONE,act,str2);
	m_StatusText.SetWindowText(str);

	if ( action==AC_DEL||action==AC_UNDO )
	{
        if ( m_ResultsView->m_Type == RV_TYPE_VIRUS_BIN ||
             m_ResultsView->m_Type == RV_TYPE_BACKUP    || 
             m_ResultsView->m_Type == RV_TYPE_REPAIR    )
        {
		    m_ResultsView->m_oLDVPResultsCtrl.ResetContent();
		    m_ResultsView->GetParent()->PostMessage(5025,0,0);
        }
	};


	m_Avi.Play(0,0,-1);
	m_Avi.Stop();

    // Save the number of items processed.
    m_dwNumberOfItemsProcessed = act;
}

BOOL CGetStatus::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	switch (wParam) {
		case 100:
			FillAndGetStatus();
			break;
		}


	return CDialog::OnCommand(wParam, lParam);
}

DWORD CGetStatus::FillAndGetStatus()
{
	CString str;

    // Column 0
	str.LoadString(IDS_FILENAME);
	m_List.InsertColumn(0,(char *)(LPCSTR)str,LVCFMT_LEFT,150,0 );

    // Column 1: set the title according to the view.
    if ( m_ResultsView->m_Type == RV_TYPE_VIRUS_BIN ||
         m_ResultsView->m_Type == RV_TYPE_BACKUP    || 
         m_ResultsView->m_Type == RV_TYPE_REPAIR    )
    {
    	str.LoadString(IDS_COL_ORIG);
    }
    else
    {
    	str.LoadString(IDS_LOCATION);
    }

	m_List.InsertColumn(1,(char *)(LPCSTR)str,LVCFMT_LEFT,150,1 );

    // Column 2
	str.LoadString(IDS_STATUS);
	m_List.InsertColumn(2,(char *)(LPCSTR)str,LVCFMT_LEFT,150,2 );

    // Get a string depending on the action.
	switch (Action) {
		case AC_MOVE:          str.LoadString(IDS_ST_MOVE);    break;
		case AC_RENAME:        str.LoadString(IDS_ST_RENAME);  break;
		case AC_DEL:           str.LoadString(IDS_ST_DELETE);  break;
		case AC_NOTHING:       break;
		case AC_CLEAN:         str.LoadString(IDS_ST_CLEAN);   break;
		case AC_SAVE_AS:       str.LoadString(IDS_ST_SAVE);    break;
		case AC_SEND_TO_INTEL: str.LoadString(IDS_ST_SEND);    break;
		case AC_UNDO:          str.LoadString(IDS_ST_UNDO);    break;
		case AC_MOVE_BACK:     str.LoadString(IDS_ST_RESTORE); break; 
		case AC_RENAME_BACK:   str.LoadString(IDS_ST_RENAME);  break;
		}

	m_OK.SetWindowText(str);

	long i=0;
	int act = 0,r;
	int count = 1;
	DWORD data=0;
//	LLSTUFF *stuff;

	m_Avi.Open(IDR_AVI3);
	m_Avi.Play(1,-1,-1);
	
    SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );
	if( pSAVRT.Get() != NULL ){
		pSAVRT->UnProtectProcess();
	}

	for (count=1;;count++) 
	{
		CString logline = m_ResultsView->m_oLDVPResultsCtrl.EnumSelectedItems(&i,(long*)&data);
		
		if (logline.IsEmpty()) // check to see if we got a logline
			break;

        // If we are trying to do a repair...
        if ( Action == AC_CLEAN )
        {
            CResultItem cResultItem( logline );

            // Make sure that this item is virally infected.
            // If it is not, we cannot clean the item.
            DWORD   dwVirusType = 0;
            cResultItem.GetVirusType( dwVirusType );

            // Only try and repair viral threats.  Bail out if this is not a viral threat.
			if ( ! IsViralCategory( dwVirusType ) )
                continue;
        }

		STATUSITEM stItem;
	
		stItem.Data = data; // set the data from ken

		// check and make sure we have a callback
		if (m_ResultsView->m_GetFileStateForView == NULL)
			continue;

		// get the state of the item
		m_ResultsView->m_GetFileStateForView(m_ResultsView->m_Context,data,(char*)(LPCSTR)logline,&stItem.State, &m_ResultsView->m_Type);
		
		// check and see of we can act on the state
		r = GetItemStatus(str,stItem.State,Action, logline);
		act += r;
		
		// if we can't act on or get the state continue
		if (!r)
			continue;

		CResultItem* item = NULL;
		try
		{
			// get info from the log line
			item = new CResultItem(logline,0);
		}
		catch (std::bad_alloc &) {}
		DWORD dwAction;
		stItem.Log = item;
		item->GetFilename(str);

		STATUSITEM *pItem =  NULL;
		try
		{
			// add the item info		
			pItem = new STATUSITEM;
			*pItem = stItem;
		}
		catch (std::bad_alloc &) {}

        // Set the result field to it's default value
        pItem->dwResult = STATUSITEM_NOT_PROCESSED;

		r = m_List.InsertItem(LVIF_TEXT|LVIF_PARAM,32768,(LPCSTR)str,0,0,0,(LPARAM)pItem);
	
		// if the action is moved to Virus-Bin change location to In Virus bin
		item->GetActionTaken(dwAction);
		if (dwAction==AC_MOVE)
			str.LoadString(IDS_INBIN);
		else
			item->GetFilepath(str);

		m_List.SetItemText(r,1,(LPCSTR)str);
	
		str.Format("%u",act);
		m_TotalCount.SetWindowText(str); // = count;

		// update the list view every 10 items
		if (act%10 == 0)
				UpdateWindow();

	}

	if( pSAVRT.Get() != NULL ){
		pSAVRT->ProtectProcess();
	}


	// through up a message box telling the user the number we can't act on
	if (act < count-1)
	{
		CString sTitle, sMessage,sResult;
		
		sTitle.LoadString(IDS_TITLE);
		sMessage.LoadString(IDS_NO_ACTION);
		sResult.Format(sMessage,(count-1) - act);
		MessageBox(sResult,sTitle,MB_OK|MB_ICONINFORMATION);

	}

	CString str2;

	switch (Action) {
		case AC_MOVE:           str2.LoadString(IDS_AC_MOVE);    break;
		case AC_RENAME:         str2.LoadString(IDS_AC_RENAME);  break;
		case AC_DEL:            str2.LoadString(IDS_AC_DELETE);  break;
		case AC_NOTHING:
		case AC_CLEAN:          str2.LoadString(IDS_AC_CLEAN);   break;
		case AC_SAVE_AS:        str2.LoadString(IDS_AC_SAVE);    break;
		case AC_SEND_TO_INTEL:  str2.LoadString(IDS_AC_SEND);    break;
		case AC_MOVE_BACK:      str2.LoadString(IDS_AC_RESTORE); break;
		case AC_UNDO:
		case AC_RENAME_BACK:    str2.LoadString(IDS_AC_UNDONE);  break;
		}
	str.Format(IDS_STATUSDONE,act,str2);
	m_StatusText.SetWindowText(str);
	if (act == 0)
		m_OK.EnableWindow(FALSE);
		

	m_Avi.Play(0,0,-1);
	m_Avi.Stop();
	return act;
}

void CGetStatus::OnDestroy() 
{
	
	CDialog::OnDestroy();
}


void CGetStatus::OnSize(UINT nType, int cx, int cy) 
{
	if (!m_bInitialized)
		return;

	CDialog::OnSize(nType, cx, cy);

	RECT	rectControl,rect;
	LONG bottom;
	LONG high;

	m_StatusText.GetClientRect(&rect); 
	high = rect.bottom;

	m_OK.GetClientRect(&rectControl);
	m_OK.GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_OK.SetWindowPos(&wndTop,cx-(rectControl.right)-10,rect.top,0,0,SWP_NOZORDER|SWP_NOSIZE);

	m_Cancel.GetClientRect(&rectControl);
	m_Cancel.GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_Cancel.SetWindowPos(&wndTop,cx-(rectControl.right)-10,rect.top,0,0,SWP_NOZORDER|SWP_NOSIZE);

	m_Help.GetClientRect(&rectControl);
	m_Help.GetWindowRect(&rect);
	ScreenToClient(&rect);
	m_Help.SetWindowPos(&wndTop,cx-(rectControl.right)-10,rect.top,0,0,SWP_NOZORDER|SWP_NOSIZE);

	m_List.GetWindowRect(&rectControl); 
	ScreenToClient(&rectControl); 
	m_List.SetWindowPos(&wndTop,0,0,(cx - 20 -(rect.right-rect.left))-rectControl.left, (bottom = cy - high - 15) - rectControl.top, SWP_NOZORDER|SWP_NOMOVE);
	
//	m_Icon.GetClientRect(&rectControl);
//	m_Icon.GetWindowRect(&rect);
//	ScreenToClient(&rect);
//	m_Icon.SetWindowPos(&wndTop,rect.left,bottom + 5,0,0,SWP_NOZORDER|SWP_NOSIZE);

//	m_Text.GetClientRect(&rectControl);
//	m_Text.GetWindowRect(&rect);
//	ScreenToClient(&rect);
//	m_Text.SetWindowPos(&wndTop,rect.left,bottom + 5,0,0,SWP_NOZORDER|SWP_NOSIZE);


}

void CGetStatus::OnGetMinMaxInfo( MINMAXINFO* info )
{
	if (m_bInitialized) 
	{
		info->ptMinTrackSize.y = MinY;
		info->ptMinTrackSize.x = MinX;
	}
}


void CGetStatus::OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->lParam) {
		STATUSITEM *cur = (STATUSITEM *)pNMListView->lParam;
		delete cur->Log;
		delete cur;
		}
	*pResult = 0;
}


void CGetStatus::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
		
	if( bShow ) PostMessage(WM_COMMAND,100,0L);
	
}


void CGetStatus::OnItemchangingList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 1;
}


void CGetStatus::OnHelp() 
{
	AfxGetApp()->WinHelpInternal(CGetStatus::IDD);
	
}


int CGetStatus::CleanLegacyVBinItem(STATUSITEM *pAnomalyStatus, char *szInfectionLogLine )
{
    TCHAR       szTempFile[MAX_PATH] = {0};
	CString		sOriginalFileName;
    DWORD       dwReturnValue = 0;
    DWORD       dwError = ERROR_SUCCESS;
    PEVENTBLOCK pEventBlock = NULL;
	LPSTR		lpTempLogLinePtr = NULL;
    char        szUpdateLogLine[MAX_LOG_LINE_SIZE] = {0};
    LPSTR       pNameString = NULL;
    DWORD       dwFlags = 0;
	BOOL		bInitScanner = FALSE;
	char szCurrentDir[IMAX_PATH];

    // Create a temp file name 
    if ( !CreateTempFileName( pAnomalyStatus, szTempFile ) )
        goto All_Done;

    if ( !m_ResultsView->m_GetCopyOfFile || !m_ResultsView->m_TakeAction )
        goto All_Done;

    // Get a copy of the file from Virus Bin and put it in the temp file.
    dwError = m_ResultsView->m_GetCopyOfFile( m_ResultsView->m_Context,
                                    pAnomalyStatus->Data,
                                    szInfectionLogLine,
#ifdef _USE_CCW
									(wchar_t*)(const wchar_t*)CharToUni((const char *)szTempFile),
#else	//_USE_CCW
                                    (char *)(const char *)szTempFile,
#endif	//_USE_CCW
                                    pAnomalyStatus->State);

    if ( dwError != ERROR_SUCCESS )
        goto All_Done;

    // Now create an event block from our new log line.
    dwError = m_ResultsView->m_pVBin->CreateEvent(&pEventBlock, szInfectionLogLine);

    // Make sure we have a valid event block
    if ( dwError != ERROR_SUCCESS )
        goto Cleanup;

    // Get the "real"file name from the description in the event block.
    pNameString = pEventBlock->Description;

	// Make sure we have a valid Scan Interface
	if(!m_ResultsView->m_pScan)
	{
		if(!m_ResultsView->InitScanner(szReg_Key_CScan_Repair))
			goto Cleanup;
		else 
			bInitScanner = TRUE;
	}

    // Turn off reverse scans for this particular scan.
	const DWORD dwScanFlags = 0;
	DWORD dwScanResultFlags = 0;
    // And scan/repair it ...
	dwError =  m_ResultsView->m_pScan->ScanOneFileEx(szTempFile, dwScanFlags, &dwScanResultFlags);

	// Did the repair work?
	if ( (dwError != ERROR_SUCCESS) || (dwScanResultFlags & IScan2::SOFEX_FLAG_RESULT_WASCLEAN) == 0 )
    {
        // Make sure we tell the caller we processed *NO* files.
        dwReturnValue = 0;
		goto Cleanup;
    }

    // Get the original location of the file
    pAnomalyStatus->Log->GetFilepath(sOriginalFileName);

    // Get the flags for the item
    pAnomalyStatus->Log->GetFlags( dwFlags );

    // Is the mail snapin flag set?
    if ( dwFlags & N_MAILNODE )
    {
        // It is, so we need to prompt for the file name
        // Put up the file open dialog.

		// 1-3M55AB - save / reset Current Directory of the application
		// to resolve problems with Help - and MS Exchange plugin (crashes)
		strcpy(szCurrentDir, "");
		GetCurrentDirectory(IMAX_PATH, szCurrentDir);

        CFileDialog dlg ( FALSE,                 // SaveAs 
                        NULL,                  // No default extension
                        sOriginalFileName,     // Initial file name                                  
                        OFN_HIDEREADONLY /* |  // Flags
                        OFN_PATHMUSTEXIST*/,
                        NULL /*sFilter*/,      // Filter string
                        this );                // Parent window


        // Fire off dialog.
        if( dlg.DoModal() != IDOK )
        {
            // User pressed Cancel
            pAnomalyStatus->dwResult = ERROR_CAN_NOT_CLEAN;
            goto Cleanup;
        }
        else
        {
            // User pressed Ok.
            sOriginalFileName = dlg.GetPathName().GetBuffer(0);

			SetCurrentDirectory(szCurrentDir);
        }

    }
    // Note we have a return value of 1 because we return the 
    // number of items successfully processed
	if ( RestoreFile(szTempFile, LPCTSTR(sOriginalFileName) ) )
    {
		dwReturnValue = 1;
        pAnomalyStatus->dwResult = ERROR_SUCCESS;
    }
    else
    {
        pAnomalyStatus->dwResult = ERROR_CAN_NOT_CLEAN;
    }


Cleanup:
    
    // Get rid of the event block
    if ( pEventBlock )
        m_ResultsView->m_pVBin->DestroyEvent(pEventBlock);

	//If initialized, de initialize the scanner
	if( (m_ResultsView->m_pScan) && (bInitScanner) )
		m_ResultsView->DeInitScanner();

All_Done:

    // Delete the temp file.
    if ( _tcslen(szTempFile) )
    {
        DeleteFile(szTempFile);
    }

    return dwReturnValue;
}

int CGetStatus::CleanRemediationVBinItem( STATUSITEM *pAnomalyStatus, IVBinSession *pVBinSession, VBININFO *pRemediationInfo )
{
    // Only restore these items if they are generic loadpoints of an
    // infected file.

    // TODO : KJS - This is not done properly here yet.
	DWORD	dwReturn = 0;
	BOOL	bInitScanEngine = FALSE;

	//Srikanth: 1-3DWT4Z
	//Cliscan does not have scan engine initialized, which would be needed for cleaning expanded threats.
	//HACK: For remediations, eraser engine has to be loaded. Tried to initialize the scan engine
	//at cleananomalyvbinitem level, but cleananomaly calls cleanlegacy for the legacy file based
	//infections in an anomaly. cleanlegacy would succeed only if initscanner is called.
	//So, eventhough I hate it, I have to initialize and de-initialize the scan engine for cleaning 
	//every remediation inside an anomaly in quarantine. Hopefully, this would not be very costly as cleaning
	//the quarantine items does not happen very frequently.

	//Initializing the scan engine.
	bInitScanEngine = m_ResultsView->InitCliProxyScanner();


    // Simply extract this item from the VBin Record.
    if ( ERROR_SUCCESS == pVBinSession->ExtractItem( pRemediationInfo->RecordID, NULL, 0 ) )
    {
        // Successfully restored.  Return 1.
        dwReturn = 1;
    }

	//De initializing the scan engine which has been loaded for Cliscan.
	m_ResultsView->DeInitCliProxyScanner(bInitScanEngine);

    return(dwReturn);
}

int CGetStatus::CleanAnomalyVBinItem(STATUSITEM *pAnomalyStatus)
{
    IVBinSession *  pVBinSession    = NULL;
    VBININFO        rVBinItemInfo   = {0};
    HANDLE          hVBinFind       = NULL;
    DWORD           dwItemReturn    = 0;
    DWORD           dwReturn        = 1;
    DWORD           dwVBinSessionID = 0;
	BOOL			bOneItemCleanFailed		= FALSE;
	BOOL			bOneItemCleanSucceeded	= FALSE;

	if ( 0 == m_ResultsView->m_pVBin )
    {
        // This should never happen.  If it does, assert, and return failure.
        SAVASSERT( 0 );
        return 0;
    }

    // Get the session ID associated with this Anomaly.
     pAnomalyStatus->Log->GetVBinSessionID( &dwVBinSessionID );

    // Attempt to open the VBin Session for this anomaly.
    if ( ERROR_SUCCESS != m_ResultsView->m_pVBin->OpenSession( dwVBinSessionID, &pVBinSession ) )
        return 0;

    hVBinFind = pVBinSession->FindFirstItem( &rVBinItemInfo );

    // Make sure we have a handle to enumerate.
    if ( hVBinFind )
    {
        do
        {
            // Initialize our item return parameter.
            dwItemReturn = 0;

            switch( rVBinItemInfo.dwRecordType )
            {
                case VBIN_RECORD_LEGACY_INFECTION:
                    dwItemReturn = CleanLegacyVBinItem( pAnomalyStatus, rVBinItemInfo.LogLine );
                    break;

                case VBIN_RECORD_REMEDIATION:
                    dwItemReturn = CleanRemediationVBinItem( pAnomalyStatus, pVBinSession, &rVBinItemInfo );
                    break;

                case VBIN_RECORD_SYSTEM_SNAPSHOT:
                case VBIN_RECORD_SESSION_OBJECT:
                    // Ignore these types.  Return success, as we don't want to fail the repair.
                    dwItemReturn = 1;
                    break;

                default:
                    // We found something unrecognized.
//                    dprintfTag1( DEBUGSCAN, "Unrecognized VBin record type when cleaning - Type = %d, RecordID = ", rVBinItemInfo.dwRecordType );

                    //  TODO : KJS - Do we want to flag this as a success?  This should never happen.
                    //               But if it does, we will not be able to repair this item.
                    dwItemReturn = 0;
                    break;
            };

            //If any actions failed, set the one item extraction failure flag to true.
			//If any actions succeeded, set the one item extraction succeeded flag to true.
            if( dwItemReturn )
				bOneItemCleanSucceeded = TRUE;
			else
                bOneItemCleanFailed = TRUE;
        }
        while ( pVBinSession->FindNextItem( hVBinFind, &rVBinItemInfo ) );

        // Close our find session.
        pVBinSession->FindClose( hVBinFind );
    }
/*
    loop through session objects
    if ( legacy )
        DoOldCode
    if ( Remediation )
        DoNewCode
    else
        DebugLog( Unrecognized type ); (System Snapshot, etc.)
        DoNothing
*/

	//Determining the return value.
	if(bOneItemCleanSucceeded)
	{
		if(bOneItemCleanFailed)
		{
			//Partial success
			dwReturn = ERROR_CLEAN_PARTIAL_FAILED;
		}
		else
		{
			//Complete success
			dwReturn = ERROR_SUCCESS;
		}
	}
	else
	{
		//Complete failure
		dwReturn = ERROR_CLEAN_FAILED;
	}

	return dwReturn;
}

int CGetStatus::CleanVBinItem(STATUSITEM *pItemStatus)
{
	CString     sLogMachineName;
    DWORD       dwReturnValue = 0;
	BOOL		bPartialSuccess = FALSE;

    SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );
	if( pSAVRT.Get() != NULL ){
		pSAVRT->UnProtectProcess();
	}

	// terrym 9-8-00  We need to check to see if the interfaces requred are present first.  NavCorp snap in
	// can call into here without any NAV client being installed (Nav server or Client)  If that happens there
	// is no interfaces.

	// MH 09.11.00
	// Check only for the IVBin interface since its possible to have a NULL
	// IScan interface here even if the client is installed.  InitScanner() will
	// initialize the IScan interface below.
	if ( m_ResultsView->m_pVBin == NULL )
	{
		CString sMsg;

		sMsg.LoadString( IDS_ERROR_NO_SCANNING_INTERFACE );

		AfxMessageBox( sMsg, MB_OK|MB_ICONEXCLAMATION );
		goto All_Done;
	}

	// MH 08.03.00
	// Determine if we're trying to restore to a remote machine by
	// comparing the local machine name with the logged one.
	// We're doing this check b/c quarantined files get restored to
	// the local machine even when they're on a remote client.
	pItemStatus->Log->GetComputerName( sLogMachineName );

	if( !IsLocalMachine( sLogMachineName ) )
	{
		// We're not local so warn the user that we are going to
		// restore locally.
		if( !WarnAndRestore() )
			goto All_Done;
	}

    // Get the event type.
    DWORD dwEvent=0;
    pItemStatus->Log->GetEvent( dwEvent );

    if ( dwEvent == GL_EVENT_INFECTION )
    {
        // Clean everything from this legacy object.
        dwReturnValue = CleanLegacyVBinItem( pItemStatus, (char *)(LPCSTR)pItemStatus->Log->CreateLogLine() );
    }
    else if ( dwEvent == GL_EVENT_ANOMALY_FINISH || dwEvent == GL_EVENT_ANOMALY_START )
    {
        // Clean everything from this anomaly.
        DWORD dwError = CleanAnomalyVBinItem( pItemStatus );

		//Was the clean successful?
		if( dwError == ERROR_SUCCESS )
		{
			dwReturnValue = 1;
		}
		//New error code to denote partial success.
		else if( dwError == ERROR_CLEAN_PARTIAL_FAILED )
		{
			dwReturnValue = 1;
			bPartialSuccess = TRUE;
		}
		//Complete failure
		else
			dwReturnValue = 0;
    }

All_Done:

	if( pSAVRT.Get() != NULL ){
		pSAVRT->ProtectProcess();
	}

    if(dwReturnValue)
    {
		if(bPartialSuccess)
			//This would be reported as a success case, as it did succeed in restoring some items.
			pItemStatus->dwResult = ERROR_CLEAN_PARTIAL_FAILED;
		else
			pItemStatus->dwResult = ERROR_SUCCESS;
	}
    else
    {
        pItemStatus->dwResult = ERROR_CLEAN_FAILED;
    }

    return dwReturnValue;
}


BOOL CGetStatus::CreateTempFileName(STATUSITEM* pItemStatus, LPTSTR lpTempFileName)
{
    int         iExtIndex = 0;
    int         iExtPos = 0;
    char        szTempPath[MAX_PATH] = {0};
    char        szTempFileName[MAX_PATH] = {0};
    DWORD       dwTicks = 0;
    UINT        uError = 0;
    BOOL        bReturn = FALSE;
	CString     sNewTempFileName;
	CString     sOrigFileName;
	CString     sFileExt;
	CString     szFileName;

    if ( pItemStatus && lpTempFileName )
    {
        // Get the full path original file name
        pItemStatus->Log->GetFilepath(sOrigFileName);
		
		
		//Strip the path of the file name
        int iFileIndex = sOrigFileName.ReverseFind(TCHAR('\\'));
		if ( iFileIndex != -1 )
		{
			int iFilePos = sOrigFileName.GetLength() - iFileIndex;
			szFileName = sOrigFileName.Right(iFilePos); //Now we have the filename with a \ in front
		
		}
		else //if we did not find a \ in the path the szFileName is the file name
		{
			szFileName = sOrigFileName;
		}
		
		// Get it's extension from the file name we have captured
        iExtIndex = szFileName.ReverseFind( TCHAR('.') );
        if ( iExtIndex != -1 )
        {
            iExtPos = szFileName.GetLength() - iExtIndex;
            sFileExt = szFileName.Right(iExtPos);
        }

        // Get the temp path
        GetTempPath(MAX_PATH, szTempPath);

        // Get the  system time
        dwTicks = GetTickCount();

        // Create a temp file for the quarantine server package
        uError = GetTempFileName(szTempPath, _T("VBR"), dwTicks, szTempFileName);

        if ( uError != 0 )
        {
            // Put the temp name in a CString (easier to manipulate)
            sNewTempFileName = szTempFileName;

            sNewTempFileName.MakeUpper();

            // Make the temp file name have the same extension as
            // the original file.
            if ( !sFileExt.IsEmpty() )
            {
                sNewTempFileName.Replace( _T(".TMP"), sFileExt );
            }

            // update the caller's buffer.
            if ( _tcscpy(lpTempFileName, LPCTSTR(sNewTempFileName)) )
            {
	            bReturn = TRUE;
            }
        }
    }

    return bReturn;

}

DWORD CGetStatus::GetResult(STATUSITEM* pItem)
{
	return( pItem->dwResult );
}

BOOL CGetStatus::RestoreFile(LPTSTR lpTempFile, LPCTSTR lpOriginalFile)
{	
	BOOL	bReturn = FALSE;

	if ( !CopyFile(lpTempFile, lpOriginalFile, TRUE) )
	{
		if ( GetLastError() == ERROR_PATH_NOT_FOUND )
		{
			// Yep. Try to create it.
            CString szTempDir = lpOriginalFile;

            szTempDir = szTempDir.Left( szTempDir.ReverseFind(TCHAR('\\')) );

			if ( !CreateMissingDirectory(szTempDir) )
				return bReturn;
		}

		if ( ReplaceFile( (LPTSTR)lpTempFile, lpOriginalFile) )
		{
			bReturn = TRUE;
		}
	}
	else
		bReturn = TRUE;

	return bReturn;
}


BOOL CGetStatus::ReplaceFile(LPTSTR lpTempFile, LPCTSTR lpNewFile)
{
    DWORD       dwAttr = 0;
    CString     sOriginalFileName = lpNewFile;
    CString     sFileName;
    CString     sFilePath;

    dwAttr = GetFileAttributes( sOriginalFileName );
    while( dwAttr != 0xFFFFFFFF )
    {
        // Prompt the user for overwrite
        CString sTitle, sText;
        sTitle.LoadString( IDS_RESTORE_OVERWRITE_TITLE );
        sText.Format( IDS_RESTORE_OVERWRITE_FORMAT, sOriginalFileName );

        if( MessageBox( sText, sTitle, MB_ICONWARNING | MB_YESNO ) != IDYES )     {
            // CString sFilter((LPCTSTR) IDS_ADD_FILTER_STRING);

			char szCurrentDir[IMAX_PATH];

			// 1-3M55AB - save / reset Current Directory of the application
			// to resolve problems with Help - and MS Exchange plugin (crashes)
			strcpy(szCurrentDir, "");
			GetCurrentDirectory(IMAX_PATH, szCurrentDir);

            CString sFilter(_T("*.*"));
            CFileDialog dlg( FALSE,                 // Save
                             NULL,                  // No default extension
                             sOriginalFileName,     // Initial file name
                             OFN_FILEMUSTEXIST |    // Flags
                             OFN_HIDEREADONLY |
                             OFN_PATHMUSTEXIST,
                             sFilter,               // Filter string
                             this );                // Parent window

            // Fire off dialog
            if( dlg.DoModal() == IDCANCEL )
                {
                return FALSE;
                }

            // Save off file name and path
            sFileName = dlg.GetFileName();
            sOriginalFileName = dlg.GetPathName();
            sFilePath.ReleaseBuffer();

			SetCurrentDirectory(szCurrentDir);
        }
        else
        {
            // User wants to overwrite this file, so delete it first.
            ::DeleteFile( sOriginalFileName );
            break;
        }

        // Get file attributes of this file
        dwAttr = GetFileAttributes( sOriginalFileName );
    }
    
    
    return CopyFile(lpTempFile, sOriginalFileName, TRUE);
}

//Gets the number of infected items in a session, by going through all the items in a session.
DWORD CGetStatus::GetNumInfectedItemsInSession(STATUSITEM* pItem)
{
    IVBinSession*	pVBinSession	= NULL;
    VBININFO        rVBinItemInfo   = {0};
    HANDLE          hVBinFind       = NULL;
    DWORD           dwVBinSessionID = 0;
	DWORD			dwVBinId		= 0;
	DWORD			dwNumInfections = 0;

   //Get the session ID associated with this Anomaly.
    pItem->Log->GetVBinSessionID( &dwVBinSessionID );

    //Attempt to open the VBin Session for this anomaly.
    if( ERROR_SUCCESS != m_ResultsView->m_pVBin->OpenSession( dwVBinSessionID, &pVBinSession ) )
        return 0;
	
    hVBinFind = pVBinSession->FindFirstItem( &rVBinItemInfo );

    //Make sure we have a handle to enumerate.
    if(hVBinFind)
    {
		do
        {
			if( rVBinItemInfo.dwRecordType == VBIN_RECORD_LEGACY_INFECTION )
				dwNumInfections++;
        }
        while ( pVBinSession->FindNextItem( hVBinFind, &rVBinItemInfo ) );

        // Close our find session.
        pVBinSession->FindClose( hVBinFind );
    }

	return dwNumInfections;
}

int CGetStatus::UndoAnomalyVBinItems(STATUSITEM *pAnomalyStatus, LPCTSTR sOriginalFileName)
{
    CSortedVBinEnumerator   cSortedVBinEnumerator;
    IVBinSession*	        pVBinSession	            = NULL;
    VBININFO                rVBinItemInfo               = {0};
    HANDLE                  hVBinFind                   = NULL;
    DWORD                   dwItemReturn                = 0;
    DWORD                   dwReturn                    = 1;
    DWORD                   dwVBinSessionID             = 0;
	DWORD			        dwVBinRecordId	            = 0;
    DWORD                   dwItemCount                 = 0;
	BOOL			        bOneItemExtractFailed		= FALSE;
	BOOL			        bOneItemExtractSucceeded	= FALSE;

	// 1-4Z5B1Z
	// if the original path differs from the parameterized path then 
	// use the parameterized path
	CString	strFileNameOnly = sOriginalFileName;
	for ( int i = strFileNameOnly.GetLength(); i >= 0; i-- )
	{
		if ( strFileNameOnly.GetAt(i) == '\\' )
		{
			strFileNameOnly = strFileNameOnly.Mid(i+1);
			break;
		}
	}


	if(0 == m_ResultsView->m_pVBin)
    {
        //This should never happen. If it does, assert, and return failure.
        SAVASSERT( 0 );
        return 0;
    }

    //Get the session ID associated with this Anomaly.
     pAnomalyStatus->Log->GetVBinSessionID( &dwVBinSessionID );

    //Attempt to open the VBin Session for this anomaly.
    if( ERROR_SUCCESS != m_ResultsView->m_pVBin->OpenSession( dwVBinSessionID, &pVBinSession ) )
        return 0;
	
    hVBinFind = pVBinSession->FindFirstItem( &rVBinItemInfo );

    //Make sure we have a handle to enumerate.
    if(hVBinFind)
    {
		do
        {
            // Don't try and undo System Snapshot VBIN records!
            if ( rVBinItemInfo.dwRecordType != VBIN_RECORD_SYSTEM_SNAPSHOT )
            {
                // Add this VBININFO item to our sorted enumerator class.
                cSortedVBinEnumerator.AddItem( rVBinItemInfo );
            }
        }
        while ( pVBinSession->FindNextItem( hVBinFind, &rVBinItemInfo ) );

        // Close our find session.
        pVBinSession->FindClose( hVBinFind );

        // Sort these VBinItems by index;
        cSortedVBinEnumerator.SortByIndex();

        // Get the count of items in our list.
        cSortedVBinEnumerator.GetCount( dwItemCount );

        // Loop through all items from last to first, restoring each item.
        for ( int nLoop = dwItemCount - 1; nLoop >= 0; nLoop--)
        {
            // Get the element at our loop's position.
            cSortedVBinEnumerator.GetAt( nLoop, rVBinItemInfo );

            //Initialize our item return value.
            dwItemReturn = ERROR_SUCCESS;

            //Get the Virus bin Record Id.
            dwVBinRecordId = rVBinItemInfo.RecordID;

            // Use a CResultItem to parse the log line so that we can
            // find the original file name of this item.
            CResultItem cResultItem( rVBinItemInfo.LogLine );

            // Get the original file name for this item.
            CString strOriginalFileName;
            cResultItem.GetFilepath( strOriginalFileName );

            // 1-4Z5B1Z
            // if the original path differs from the parameterized path then 
            // use the parameterized path
            if ( strOriginalFileName.Find(strFileNameOnly) >= 0 )
            {
                if ( strOriginalFileName.CompareNoCase(sOriginalFileName) != 0 )
                    strOriginalFileName = sOriginalFileName;
            }

            // Extract this item from Quarantine, passing in the file name
            // listed in the "Description" field of the VBin Info structure.
            dwItemReturn = pVBinSession->ExtractItem(dwVBinRecordId, strOriginalFileName, FALSE);
            //If any actions failed, set the one item extraction failure flag to true.
            //If any actions succeeded, set the one item extraction succeeded flag to true.
            if( dwItemReturn != ERROR_SUCCESS )
                bOneItemExtractFailed = TRUE;
            else
                bOneItemExtractSucceeded = TRUE;
        }
    }

    // Release our VBin Session interface.
    if ( NULL != pVBinSession )
        pVBinSession->Release();

	if(bOneItemExtractSucceeded)
	{
		if(bOneItemExtractFailed)
		{
			//Partial success
			dwReturn = ERROR_MOVE_PARTIAL_FAILED;
		}
		else
		{
			//Complete success
			dwReturn = ERROR_SUCCESS;
		}
	}
	else
	{
		//Complete failure
		dwReturn = ERROR_MOVE_FAILED;
	}

	return dwReturn;
}

int CGetStatus::UndoVBinItem(STATUSITEM *pItemStatus)
{
    BOOL        bMissingDirCreated = FALSE;
    BOOL        bPromptForFileName = FALSE;
    BOOL        bTruncateFileName = FALSE;
	CString		sOriginalFileName;
    DWORD       dwReturnValue = 0;
    DWORD       dwError = ERROR_OPEN_FAIL;
    PEVENTBLOCK pEventBlock = NULL;
    HANDLE      hFile = NULL;
    DWORD       dwCreateError = ERROR_SUCCESS;
    DWORD       dwFlags = 0;
	CString     sLogMachineName;
	BOOL		bRedirectedPath;
    DWORD		dwEvent = 0;
	BOOL		bPartialSuccess = FALSE;
	BOOL		bInitScanEngine = FALSE;
#ifdef _USE_CCW
	VBININFO stVBinInfo;
	CString strUserSelDestFileName = _T("");
#endif	//_USE_CCW
    SAVRT_PTR pSAVRT( CSavrtModuleInterface::Init() );

    //Get the event type.
    pItemStatus->Log->GetEvent(dwEvent);

	//Srikanth: 1-3DWT4Z
	//Cliscan does not have scan engine initialized, which would be needed for restoring expanded threats.
	//Initializing the scan engine.
	bInitScanEngine = m_ResultsView->InitCliProxyScanner();

	// MH 09.12.00
	// Check to see if we have a valid IVBin interface pointer.  If not,
	// warn the user that we can't undo, then fail the operation.
	if ( m_ResultsView->m_pVBin == NULL )
	{
		CString sMsg;

		sMsg.LoadString( IDS_ERROR_NO_SCANNING_INTERFACE );

		AfxMessageBox( sMsg, MB_OK|MB_ICONEXCLAMATION );
		goto All_Done;
	}

	// TCashin/MH 08.23.00
	// Determine if we're trying to undo to a remote machine by
	// comparing the local machine name with the logged one.
	// We're doing this check b/c quarantined files get restored to
	// the local machine even when they're on a remote client.

	pItemStatus->Log->GetComputerName( sLogMachineName );

	if( !IsLocalMachine( sLogMachineName ) )
	{
		// We're not local so warn the user that we are going to
		// restore locally.
		if( !WarnAndRestore() )
			goto All_Done;
	}

    if ( !m_ResultsView->m_GetCopyOfFile )
        goto All_Done;

	if( pSAVRT.Get() != NULL ){
		pSAVRT->UnProtectProcess();
	}


    // Get the original file name
    pItemStatus->Log->GetFilepath(sOriginalFileName);

	// Determine if we are going to have to overcome 64-bit redirection.
	bRedirectedPath = IsWow64RedirectedPath( sOriginalFileName );

    // Get the flags for the item
    pItemStatus->Log->GetFlags( dwFlags );

    // Is the mail snapin flag set?
    if ( dwFlags & N_MAILNODE )
    {
        // It is, so don't check to see if the
        // file exists until after the SaveAs dialog.
        bPromptForFileName = TRUE;
    }
    else
    {
        // Flag is set for some other storage type. So we 
        // can expect a fully qualified path name here.
        // MMENDON 09-08-2000 
        // Note that GetLastError() is called below if FileExists() fails.  
        // No other Win32 function should be called before the GetLastError() 
        // call or the error code may be corrupted.

		if( bRedirectedPath )
			Wow64EnableFileRedirection( FALSE );

        CString strUnavailableText;
        strUnavailableText.LoadString( IDS_PATH_UNAVAILABLE );

#ifdef _USE_CCW
		DWORD dwVBinSessionID = 0;
		DWORD dwVBinID = 0;
		IVBinSession *pVBinSession = NULL;
				
		memset(&stVBinInfo, 0x00, sizeof(VBININFO));
		//Get the session ID associated with this Anomaly.
		pItemStatus->Log->GetVBinSessionID( &dwVBinSessionID );
		pItemStatus->Log->GetVBinID( &dwVBinID );

		
		if(dwVBinSessionID == 0 && dwVBinID != 0)	//Legacy VBIN Item
		{
			m_ResultsView->m_pVBin->GetInfo(dwVBinID, &stVBinInfo);
		}
		else
		{
			//Attempt to open the VBin Session for this anomaly.
			if( ERROR_SUCCESS == m_ResultsView->m_pVBin->OpenSession( dwVBinSessionID, &pVBinSession ) )
				pVBinSession->GetSessionInfo(&stVBinInfo);

			// Release our VBin Session interface.
			if ( NULL != pVBinSession )
				pVBinSession->Release();
		}
#endif	//_USE_CCW

        // Don't look at the file path if "Unavailable".  In the case where
        // we did not have a file name, we shouldn't block the threat from
        // being restored by prompting for a file path.  There was no
        // file path in this scenario.
        if ( strUnavailableText.CompareNoCase( sOriginalFileName ) )
        {
#ifdef _USE_CCW
			bool bFileExits = false;
			//Verify with Widechar Description
			if(stVBinInfo.WDescription[0])
				bFileExits = FileExists( stVBinInfo.WDescription, FALSE );
			else
				bFileExits = FileExists( sOriginalFileName, FALSE );
			if(bFileExits)
#else	//_USE_CCW		
            if( FileExists( sOriginalFileName, FALSE ) )
#endif	//_USE_CCW
            {
                // The file exists .Prompt the user to overwrite.
                CString sTitle, sText;
                sTitle.LoadString( IDS_RESTORE_OVERWRITE_TITLE );
                sText.Format( IDS_RESTORE_OVERWRITE_FORMAT, sOriginalFileName );

                if( MessageBox(sText, sTitle, MB_ICONWARNING | MB_YESNO ) == IDYES )
                {
#ifdef _USE_CCW
					// The user wants to overwrite the existing file
					if(stVBinInfo.WDescription[0])
						DeleteFileW(stVBinInfo.WDescription);
					else
						DeleteFile(sOriginalFileName);					
#else	//_USE_CCW
                    // The user wants to overwrite the existing file
                    DeleteFile(sOriginalFileName);
#endif	//_USE_CCW
                }
                else
                {
                    bPromptForFileName = TRUE;
                }
            }
            else
            {
                // Check the error from CreateFile()
                dwCreateError = GetLastError();

                switch ( dwCreateError )
                { 
                    case ERROR_SUCCESS:
                    case ERROR_FILE_NOT_FOUND: 
                        break;

                    case ERROR_PATH_NOT_FOUND: 
				    case ERROR_INVALID_NAME: //EA 09/22/2000 Start of Partial fix for STS 344531 & 344917 this only fixes undoing quarantine items in virus history for Netware m/c
										    //for Netware paths also we need to truncate it to file names since netware paths give back error codes
										    //for the FileExists function ERROR_INVALID_NAME  or 	ERROR_BAD_PATHNAME	and the filedialog called below with the paths sys:\ or vol1:\ fail
										    //	on DoModal hence after truncation i.e. removed the path and only passed filename to filedialog called its DoModal
										    //this fix will restore all types of files in quarantine except for specific compressed items of the format sys:\virus1.zip(dir/virusfile)		
				    case ERROR_BAD_PATHNAME: //EA 09/22/2000 End of Partial fix for STS 344531 & 344917 this only fixes undoing quarantine items in virus history for Netware m/c

                        // Don't prompt for a missing path if this is an Anomaly event, since
                        // this Anomaly could contain path remediation actions to restore
                        // the path itself.
                        // TODO : KJS - Write a check to verify within the Anomaly if the path
                        //              will be restored by a remediation action.
                        if (( dwEvent != GL_EVENT_ANOMALY_START ) && ( dwEvent != GL_EVENT_ANOMALY_FINISH ))
                        {
                            // No path. Prompt the user to save to another location.
                            CString sTitle, sText;
                            sTitle.LoadString( IDS_UNDO_NO_PATH_TITLE );
                            sText.Format( IDS_UNDO_NO_PATH_FORMAT, sOriginalFileName );

                            if( MessageBox(sText, sTitle, MB_ICONWARNING | MB_YESNO ) == IDYES )
                            {
                                bPromptForFileName = TRUE;
                                bTruncateFileName = TRUE;
                            }
                        }
                        break;

                    default:

                        // We failed for some other reason ... prompt
                        bPromptForFileName = TRUE;
                }
            }

		    if( bRedirectedPath )
			    Wow64EnableFileRedirection( TRUE );
        }
    }

    // Should we put up the dialog?
    if ( bPromptForFileName )
    {
        CString sPromptFileName;
		char szCurrentDir[IMAX_PATH];

        // The bTruncateFileName flag is set we can't find the original
        // path and the user wants to save the file in some other folder.
        // So we put just the file name in the SaveAs dialog box.

        if ( bTruncateFileName )
        {
            int iFileNameLength = 0;
            
            iFileNameLength = sOriginalFileName.GetLength() - sOriginalFileName.ReverseFind('\\') - 1;

            if ( iFileNameLength > 0 )
            {
                sPromptFileName = sOriginalFileName.Right( iFileNameLength ); 
            }
            else
            {
                sPromptFileName = sOriginalFileName;
            }
        }
        else
        {
            sPromptFileName = sOriginalFileName;
        }

        // Put up the SaveAs dialog.

		// 1-3M55AB - save / reset Current Directory of the application
		// to resolve problems with Help - and MS Exchange plugin (crashes)
		strcpy(szCurrentDir, "");
		GetCurrentDirectory(IMAX_PATH, szCurrentDir);
 
        CFileDialog dlg ( FALSE,                 // SaveAs 
                          NULL,                  // No default extension
                          sPromptFileName,       // Initial file name                                  
                          OFN_HIDEREADONLY /*    // Flags
                          OFN_PATHMUSTEXIST*/,
                          NULL /*sFilter*/,      // Filter string
                          this );                // Parent window

        dlg.m_ofn.lpstrInitialDir = m_szDefaultRestoreDir;

        // Fire off dialog.
        if( dlg.DoModal() != IDOK )
        {
            // Cancel
            goto Turn_On_AP;
        }
        else
        {
            // User pressed Ok.
#ifdef _USE_CCW
			strUserSelDestFileName = dlg.GetPathName().GetBuffer(0);
#else	//_USE_CCW
            sOriginalFileName = dlg.GetPathName().GetBuffer(0);
#endif
			SetCurrentDirectory(szCurrentDir);
        }

        // Is the mail snapin flag set? If so, see if the file
        // name specified exists.

        if ( dwFlags & N_MAILNODE )
        {
#ifdef _USE_CCW
			if ( FileExists(strUserSelDestFileName, TRUE) )
#else	//_USE_CCW
            if ( FileExists(sOriginalFileName, TRUE) )
#endif
            {
                goto Turn_On_AP;
            }
        }
    }

Copy_File:

	//Srikanth 02/03/05 Defect 1-3DWT4Z
	//For anomaly type threats, there might be more than one infected files
	//and these files have to be enumerated and restored one at a time.
	if( (dwEvent == GL_EVENT_ANOMALY_START) || (dwEvent == GL_EVENT_ANOMALY_FINISH) )
	{
#ifdef _USE_CCW
		dwError = UndoAnomalyVBinItems(pItemStatus, (LPCTSTR)strUserSelDestFileName);
#else	//_USE_CCW
		dwError = UndoAnomalyVBinItems(pItemStatus, (LPCTSTR)sOriginalFileName);
#endif
	}
	else
		// Get a copy of the file from Virus Bin and put it in the temp file.
		dwError = m_ResultsView->m_GetCopyOfFile( m_ResultsView->m_Context,
										pItemStatus->Data,
										(char *)(LPCSTR)pItemStatus->Log->CreateLogLine(),
#ifdef _USE_CCW
										strUserSelDestFileName.IsEmpty()?(stVBinInfo.WDescription[0]?stVBinInfo.WDescription:((wchar_t*)(const wchar_t*)CharToUni(LPCTSTR(sOriginalFileName)))):((wchar_t*)(const wchar_t*)CharToUni(LPCTSTR(strUserSelDestFileName))),
#else	//_USE_CCW
										(char *)LPCTSTR(sOriginalFileName),
#endif	//_USE_CCW
										pItemStatus->State);				

	//Was the restore successful?
    if( dwError == ERROR_SUCCESS )
    {
        dwReturnValue = 1;
    }
	//New error code to denote partial success.
	else if( dwError == ERROR_MOVE_PARTIAL_FAILED )
	{
		dwReturnValue = 1;
		bPartialSuccess = TRUE;
	}
    else
    {
        if ( !bMissingDirCreated && dwError == ERROR_NO_PATH1 )
        {
			// Yep. Try to create it.
#ifdef _USE_CCCW
			CString szTempDir = strUserSelDestFileName.IsEmpty()?sOriginalFileName:strUserSelDestFileName;
#else	//_USE_CCW
            CString szTempDir = sOriginalFileName;
#endif	//_USE_CCW
            szTempDir = szTempDir.Left( szTempDir.ReverseFind(TCHAR('\\')) );

			if( !bRedirectedPath || Wow64EnableFileRedirection( FALSE ) )
			{
				BOOL bCreated = CreateMissingDirectory(LPCTSTR(szTempDir));

				if( bRedirectedPath )
					Wow64EnableFileRedirection( TRUE );

				if( bCreated )
				{
					bMissingDirCreated = TRUE;
					goto Copy_File;
				}
			}
        }
    }

Turn_On_AP:

	if( pSAVRT.Get() != NULL ){
		pSAVRT->ProtectProcess();
	}

All_Done:

    if(dwReturnValue)
    {
		if(bPartialSuccess)
			//This would be reported as a success case, as it did succeed in restoring some items.
			pItemStatus->dwResult = ERROR_MOVE_PARTIAL_FAILED;
		else
			pItemStatus->dwResult = ERROR_SUCCESS;
	}
    else
    {
        pItemStatus->dwResult = ERROR_MOVE_FAILED;
    }

	//De initializing the scan engine which has been loaded for Cliscan.
	m_ResultsView->DeInitCliProxyScanner(bInitScanEngine);

	return dwReturnValue;
}

DWORD CGetStatus::GetItemStatus(CString &str, DWORD State, DWORD Action, CString &szLogLine)
{
	DWORD ret = 0;

	str.LoadString(IDS_NA);

	switch (Action) {
		case AC_MOVE:       

            if (State&RV_STATE_NORMAL)
            {
                CResultItem cResultItem( szLogLine );

                if( CheckInfectorFlags(&cResultItem, VECOMPRESSED) && !(State & RV_STATE_WRITE_PROTECTED)  ) 
                {
                    ret = 1;  
                }
                else if( CheckInfectorFlags(&cResultItem, VEDELETABLE)  && !(State & RV_STATE_WRITE_PROTECTED) )
                {
                    ret = 1;
                }
                else if ( IsExpandedThreat( &cResultItem ) )
                {
                    // TODO : KJS - This is a hack because expanded threats return back not deletable.
                    //              In this case, we will return back that we can quarantine the
                    //              threat until we can get a fix from response.
                    ret = 1;
                }
                else if ( !(State&RV_STATE_INFECTED) )
                {
                    ret = 0;
                }
            }
            break;

		case AC_RENAME:     

            ret = 0; 
            break;

		case AC_SAVE_AS:
		case AC_SEND_TO_INTEL:
		case AC_DEL:        

            if ( State&(RV_STATE_NORMAL|RV_STATE_RENAMED) && 
                 CheckInfectorFlags(szLogLine, VEDELETABLE) && 
                 !(State & RV_STATE_WRITE_PROTECTED) ) 
            {
                ret = 1; 
            }
            else if ( State & RV_STATE_INBIN )
            {

                // Special case: if we're deleting a file from quarantine,
                // we can safely assume that the virus is deletable. It could
                // not be here otherwise.

                ret = 1;
            }
            else if( (State&RV_STATE_NORMAL) && 
                     CheckInfectorFlags(szLogLine, VECOMPRESSED) && 
                     !(State & RV_STATE_WRITE_PROTECTED) ) 
            {
                // Special case: if it is a container with infected items, we can delete
                // the container.
                ret = 1;  
            }

            break;

		case AC_NOTHING:    
            ret = 0; 
            break;

		case AC_CLEAN:      
 
            if ( State&RV_STATE_INFECTED )
            { 
				// Don't repair infected containers in quarantine.
                if( CheckInfectorFlags(szLogLine, VECOMPRESSED) && (State & RV_STATE_INBIN) ) 
                {
                    ret = 0;  
                }
                // Don't check the for the disk being write protected when trying to clean boot infections.
                // Let NAVAPI handle the case.
                else if( (State&RV_STATE_BOOT_INFECTED) || 
                    ( CheckInfectorFlags(szLogLine, VECLEANABLE) && !(State & RV_STATE_WRITE_PROTECTED) ) )
                {
                    ret = 1; 
                }
            }
            break;

		case AC_UNDO:       

            if ( (State&(RV_STATE_INBIN|RV_STATE_RENAMED)) && !(State & RV_STATE_WRITE_PROTECTED) ) 
            {
                ret = 1; 
            }
            break;

		case AC_MOVE_BACK:  

            if ( (State&RV_STATE_INBIN) && !(State & RV_STATE_WRITE_PROTECTED) )
            {
                ret = 1; 
            }
            break;   

		case AC_RENAME_BACK: 

            if ( (State&RV_STATE_RENAMED) && !(State & RV_STATE_WRITE_PROTECTED) )
            {
                ret = 1; 
            }
            break;   

		}

	if (ret) {
		if (State&RV_STATE_INFECTED) 
			str.LoadString(IDS_INFECTED);
		else 
			str.LoadString(IDS_CLEAN);
		}
	return ret;
}


BOOL CGetStatus::CheckInfectorFlags(CString &szLogLine, DWORD dwInfectorFlags)
{
    CResultItem cResultItem( szLogLine );

    return CheckInfectorFlags( &cResultItem, dwInfectorFlags );
}

BOOL CGetStatus::CheckInfectorFlags(CResultItem *pResultItem, DWORD dwInfectorFlags)
{
	BOOL        bRet = FALSE;

	if( NULL != pResultItem )
	{
		switch( dwInfectorFlags )
			{
			case VEDELETABLE:
				{
				DWORD dwDeleteType = 0;

				pResultItem->GetDeleteType( dwDeleteType );
				if( dwDeleteType == VEDELETABLE )
					{
					bRet = TRUE;
					}
				}
				break;
			case VECLEANABLE:
				{
				DWORD dwCleanType = 0;

				pResultItem->GetCleanType( dwCleanType );
				if( dwCleanType == VECLEANABLE )
					{
					bRet = TRUE;
					}
				}
				break;
			case VECOMPRESSED:
				{
				DWORD dwDepth = 0;
				BOOL bIsCompressed = FALSE;

				pResultItem->GetFileType( bIsCompressed );
				pResultItem->GetDepth( dwDepth );
				if( bIsCompressed && dwDepth == 0 )
					{
					bRet = TRUE;
					}
				}
				break;
			}
		}

    return bRet;
}

DWORD CGetStatus::CleanupQuarantine(DWORD dwAction)
{
    // Get the number of items in the list.
	int count = m_List.GetItemCount();

    // Go through each one
	for (int i=0;i<count;i++) 
	{
        // Get a pointer to a specific item
		STATUSITEM *pItem = (STATUSITEM *)m_List.GetItemData(i);

        // A valid pointer and valid result?
		if ( pItem && (pItem->dwResult == ERROR_SUCCESS) ) 
		{
			wThreadSwitch();
		
            // Check for the state/action combinations we need.
            if ( pItem->State&RV_STATE_INBIN )
            {
                if ( (dwAction == AC_CLEAN)     ||
                     (dwAction == AC_UNDO)      ||
                     (dwAction == AC_MOVE_BACK) )
                {
                    // We good to go. Change the vbin header info to backup.
                    SetQuarItemToBackup( pItem );
                }
            }
		}
	}

    return ERROR_SUCCESS;
}

DWORD CGetStatus::SetQuarItemToBackup(STATUSITEM *pItem)
{
    CString     sTempLogLine;
    DWORD       dwError = ERROR_SUCCESS;
    PEVENTBLOCK pEventBlock = NULL;
	LPSTR		lpTempLogLinePtr = NULL;
    VBININFO    vbi = {0};

    // Make a log line for our own use
    sTempLogLine = pItem->Log->CreateLogLine();

	lpTempLogLinePtr = sTempLogLine.GetBuffer(MAX_LOG_LINE_SIZE);

    // Now create an event block from our new log line.
    dwError = m_ResultsView->m_pVBin->CreateEvent(&pEventBlock, lpTempLogLinePtr);

    // Is there a backup ID in the event block?
    if ( dwError == ERROR_SUCCESS && pEventBlock->dwBackupID )
    {
        // If so, see if there is a backup file.
        dwError = m_ResultsView->m_pVBin->GetInfo( pEventBlock->dwBackupID, &vbi );

        if ( dwError == ERROR_SUCCESS )
        {
            // There is a backup. We must delete the current quarantine item.
            dwError = m_ResultsView->m_pVBin->Delete( pEventBlock->VBinID );

            // That's it.
            goto Cleanup;
        }
    }

    // Make sure we have a valid event block
    if ( dwError != ERROR_SUCCESS )
        goto Cleanup;

    // Get the quarantine info for this guy.
    dwError = m_ResultsView->m_pVBin->GetInfo( pEventBlock->VBinID, &vbi );

    // If we don't get the vbin info, we're sunk.
    if ( dwError != ERROR_SUCCESS )
        goto Cleanup;

    // Set the backup flag
    vbi.Flags = VBIN_BACKUP;

    // Set the real action to backup
    pEventBlock->RealAction = AC_BACKUP;

    // Create our own log line because the event block is different
    dwError = m_ResultsView->m_pVBin->CreateLogLine( vbi.LogLine, pEventBlock );

    // If we don't get the vbin info, we're toast.
    if ( dwError != ERROR_SUCCESS )
        goto Cleanup;

    // And update the Quarantine file.
    dwError = m_ResultsView->m_pVBin->SetInfo( pEventBlock->VBinID, &vbi );

Cleanup:
    
    // Get rid of the event block
    if ( pEventBlock )
        m_ResultsView->m_pVBin->DestroyEvent(pEventBlock);

    return dwError;
}

DWORD CGetStatus::GetNumberOfItemsProcessed()
{
    return m_dwNumberOfItemsProcessed;
}


//************************************************************************
//
// BOOL CGetStatus::FileExists(LPCTSTR lpFile, BOOL bPrompt)
//
//  Determines if lpFile exists
//
//  lpFile:  File path
//  bPrompt: if file exists, should a prompt be displayed for overwriting the file
//
//  Return:  TRUE - if the file exists
//           FALSE - if the file does not exist
//
//  Remarks:  callers of this function are dependent on the last error
//            set by FindFirstFile().  It is very important that no
//            other Win32 call be made if FindFirstFile() fails.
//
//***********************************************************************


BOOL CGetStatus::FileExists(LPCTSTR lpFile, BOOL bPrompt)
{
    BOOL            bRet = FALSE;
    DWORD           dwError = ERROR_SUCCESS;
    HANDLE          hFind = NULL;
    WIN32_FIND_DATA finddata = {0};
    TCHAR           szTestDir[MAX_PATH] = {0};
    LPTSTR          lpTemp = NULL;

    // Copy the file name to the test dir buffer
    ssStrnCpy( szTestDir, lpFile, sizeof(szTestDir) );

    // Find the trailing delimiter
    lpTemp = _tcsrchr( szTestDir, '\\' );

    // Truncate the string there
    if(lpTemp != NULL)
        *lpTemp = 0;

    // Fix for STS #345749
    // If the path is the root of the drive, then append "\*" to the path based on 
    // MSDN documentation (copied the statement from MSDN) for FindFirstFile().
    // To examine files in a root directory, use something like "C:\*" and step through the 
    // directory with FindNextFile(). 
    if(lstrlen(szTestDir) == 2  && ( _tcsnicmp(CharNext(szTestDir), _T(":"), 1) == 0 )) 
    {
        ssStrnAppend(szTestDir, _T("\\*"), sizeof(szTestDir));
    }

    // Now see if we can find the drive/directory
    hFind = FindFirstFile( szTestDir, &finddata );
    if ( hFind == INVALID_HANDLE_VALUE )
    {
        // MMENDON 09-08-2000 STS defect #341598
        //         On Win9x, FindFirstFile sets last error ERROR_FILE_NOT_FOUND
        //         When the directory doesn't exist and the path is an lfn. In all
        //         other cases, platforms it appears that the error is ERROR_PATH_NOT_FOUND.
        //         The calling function looks for ERROR_PATH_NOT_FOUND when the directory
        //         does not exist.  If this FindFirstFile() fails, I'm checking last error
        //         for ERROR_FILE_NOT_FOUND and then setting last error to ERROR_PATH_NOT_FOUND.
        //         Don't think it's a good idea for the calling function to also check for 
        //         ERROR_FILE_NOT_FOUND because that implies that the directory exists, but the
        //         file doesn't.  In that case we try to copy the restored file to the path.

        DWORD dwLastError = GetLastError();
        if(dwLastError == ERROR_FILE_NOT_FOUND)
            SetLastError(ERROR_PATH_NOT_FOUND);
        else
            SetLastError(dwLastError);

        // MMENDON 09-08-2000 End STS defect #341598

        goto All_Done;
    }
    
    FindClose( hFind );
    hFind = NULL;


    bool bFoundFile(false);
#ifdef _USE_CCW
	if( true == IsADSPath(CharToUni(lpFile)) )
#else	//_USE_CCW
    if( true == IsADSPath(lpFile) )
#endif	//_USE_CCW
    {
        bFoundFile = DoesADSStreamExist(lpFile);
    }
    else
    {
        // Use findfirst since it will generate  
        // ERROR_ACCESS_DENIED when appropriate.
        hFind = FindFirstFile( lpFile, &finddata );
        if( hFind != INVALID_HANDLE_VALUE )
        {
            bFoundFile = true;
            // Yep. Close the find handle
            FindClose( hFind );
        }
    }

    // Do we have a valid handle?
    if( true == bFoundFile )
    {
        

        // Prompt for overwrite?
        if ( bPrompt )
        {
            // Yep .Prompt the user
            CString sTitle, sText;
            sTitle.LoadString( IDS_RESTORE_OVERWRITE_TITLE );
            sText.Format( IDS_RESTORE_OVERWRITE_FORMAT, lpFile );

            if( MessageBox(sText, sTitle, MB_ICONWARNING | MB_YESNO ) == IDYES )
            {
                // The user wants to overwrite the existing file
                DeleteFile(lpFile);
                bRet = FALSE;
            }
            else
            {               
                // They do NOT want to overwrite so we'll return
                // that the file DOES exist.
                bRet = TRUE;
            }
        }
        else
        {
            // Don't prompt. Just return that the file exists.
            bRet = TRUE;
        }
    }
    else
    {
        dwError = GetLastError();
    }

All_Done:

    return bRet;
}

void CGetStatus::OnCancel() 
{
	CDialog::OnCancel();

	m_Cancel.EnableWindow(FALSE);

	m_bUserCancel = TRUE;
}

BOOL CGetStatus::IsExpandedThreat( CResultItem *pResultItem )
{
    DWORD dwVirusType = 0;

    if ( pResultItem == NULL )
        return FALSE;

    // Get the virus type from this result item.
    pResultItem->GetVirusType( dwVirusType );

    // Is this any one of the known expanded threat virus types?
    if ( dwVirusType & (  VE_NON_VIRAL_MALICIOUS | 
                          VE_RESERVED_MALICIOUS | 
                          VE_SECURITY_RISK_ON | 
                          VE_HACKER_TOOLS | 
                          VE_SPYWARE | 
                          VE_TRACKWARE | 
                          VE_DIALERS | 
                          VE_REMOTE_ACCESS |
                          VE_ADWARE |
                          VE_JOKE_PROGRAMS ) )
    {
        return TRUE;
    }

    return FALSE;
}

#ifdef _USE_CCW
//************************************************************************
//
// BOOL CGetStatus::FileExists(LPCWSTR lpFile, BOOL bPrompt)
//
//  Determines if lpFile exists
//
//  lpFile:  File path
//  bPrompt: if file exists, should a prompt be displayed for overwriting the file
//
//  Return:  TRUE - if the file exists
//           FALSE - if the file does not exist
//
//  Remarks:  callers of this function are dependent on the last error
//            set by FindFirstFile().  It is very important that no
//            other Win32 call be made if FindFirstFile() fails.
//
//***********************************************************************


BOOL CGetStatus::FileExists(LPCWSTR lpFile, BOOL bPrompt)
{
	BOOL            bRet = FALSE;
	DWORD           dwError = ERROR_SUCCESS;
	HANDLE          hFind = NULL;
	WIN32_FIND_DATAW finddata = {0};
	WCHAR           szTestDir[IMAX_PATH] = {0};
	LPWSTR          lpTemp = NULL;

	// Copy the file name to the test dir buffer
	ssStrnCpy( szTestDir, lpFile, sizeof(szTestDir) );

	// Find the trailing delimiter
	lpTemp = vpstrutils::vpStrRChar( szTestDir, L'\\' );

	// Truncate the string there
	if(lpTemp != NULL)
		*lpTemp = 0;

	// Fix for STS #345749
	// If the path is the root of the drive, then append "\*" to the path based on 
	// MSDN documentation (copied the statement from MSDN) for FindFirstFile().
	// To examine files in a root directory, use something like "C:\*" and step through the 
	// directory with FindNextFile(). 
	if(vpstrutils::vpNumChars(szTestDir) == 2  && ( wcsnicmp(vpstrutils::vpNextChar(szTestDir), L":", 1) == 0 )) 
	{
		ssStrnAppend(szTestDir, L"\\*", sizeof(szTestDir));
	}

	// Now see if we can find the drive/directory
	hFind = FindFirstFileW( szTestDir, &finddata );
	if ( hFind == INVALID_HANDLE_VALUE )
	{
		// MMENDON 09-08-2000 STS defect #341598
		//         On Win9x, FindFirstFile sets last error ERROR_FILE_NOT_FOUND
		//         When the directory doesn't exist and the path is an lfn. In all
		//         other cases, platforms it appears that the error is ERROR_PATH_NOT_FOUND.
		//         The calling function looks for ERROR_PATH_NOT_FOUND when the directory
		//         does not exist.  If this FindFirstFile() fails, I'm checking last error
		//         for ERROR_FILE_NOT_FOUND and then setting last error to ERROR_PATH_NOT_FOUND.
		//         Don't think it's a good idea for the calling function to also check for 
		//         ERROR_FILE_NOT_FOUND because that implies that the directory exists, but the
		//         file doesn't.  In that case we try to copy the restored file to the path.

		DWORD dwLastError = GetLastError();
		if(dwLastError == ERROR_FILE_NOT_FOUND)
			SetLastError(ERROR_PATH_NOT_FOUND);
		else
			SetLastError(dwLastError);

		// MMENDON 09-08-2000 End STS defect #341598

		goto All_Done;
	}

	FindClose( hFind );
	hFind = NULL;


	bool bFoundFile(false);
	if( true == IsADSPath(lpFile) )
	{
		bFoundFile = DoesADSStreamExist(lpFile);
	}
	else
	{
		// Use findfirst since it will generate  
		// ERROR_ACCESS_DENIED when appropriate.
		hFind = FindFirstFileW( lpFile, &finddata );
		if( hFind != INVALID_HANDLE_VALUE )
		{
			bFoundFile = true;
			// Yep. Close the find handle
			FindClose( hFind );
		}
	}

	// Do we have a valid handle?
	if( true == bFoundFile )
	{


		// Prompt for overwrite?
		if ( bPrompt )
		{
			// Yep .Prompt the user
			CString sTitle, sText;
			sTitle.LoadString( IDS_RESTORE_OVERWRITE_TITLE );
			sText.Format( IDS_RESTORE_OVERWRITE_FORMAT, lpFile );

			if( MessageBox(sText, sTitle, MB_ICONWARNING | MB_YESNO ) == IDYES )
			{
				// The user wants to overwrite the existing file
				DeleteFileW(lpFile);
				bRet = FALSE;
			}
			else
			{               
				// They do NOT want to overwrite so we'll return
				// that the file DOES exist.
				bRet = TRUE;
			}
		}
		else
		{
			// Don't prompt. Just return that the file exists.
			bRet = TRUE;
		}
	}
	else
	{
		dwError = GetLastError();
	}

All_Done:

	return bRet;
}
#endif	//_USE_CCW