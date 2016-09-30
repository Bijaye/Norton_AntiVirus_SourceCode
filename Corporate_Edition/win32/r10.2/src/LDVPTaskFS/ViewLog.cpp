// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ViewLog.cpp : implementation file
//

#include "stdafx.h"
#include "ldvptaskfs.h"
#include "ViewLog.h"
#include "Wizard.h"
#include "SymSaferRegistry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewLog property page

IMPLEMENT_DYNCREATE(CViewLog, CWizardPage)

CViewLog::CViewLog() : CWizardPage(CViewLog::IDD)
{
	//{{AFX_DATA_INIT(CViewLog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_psp.dwFlags |= PSP_HASHELP;
	//Set the Help Context ID
	SetHelpID( IDD );
}

CViewLog::~CViewLog()
{
}

void CViewLog::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CViewLog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CViewLog, CPropertyPage)
	//{{AFX_MSG_MAP(CViewLog)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	ON_EN_SETFOCUS(IDC_LOG, OnSetfocusLog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewLog message handlers

BOOL CViewLog::OnInitDialog() 
{
	CTime	cTime = CTime::GetCurrentTime();
	CString strFilename;
	CString strDir = GetLogDir();

	CPropertyPage::OnInitDialog();
	CWizard	*ptrParent = (CWizard*)GetParent();
	
	//Set the Context-sensitive ID
	SetHelpID( IDD_VIEW_LOG );

	//Load the options
	if( ptrParent->IsKindOf( RUNTIME_CLASS( CWizard ) ) )
	{
		ptrParent->SetFinishText( IDS_CLOSE );
		ptrParent->SetWizardButtons( PSWIZB_FINISH );
	}

	
	//Create the filename
	strFilename.Format( "%s\\%02d%02d%04d.Log", strDir, cTime.GetMonth(), cTime.GetDay(), cTime.GetYear() );
	
	OpenLogFile( strFilename );

	return TRUE;  
}

void CViewLog::OnOpen() 
{
	CString		strFilename;
	CString		strFilter;
	CString		strInitDir = GetLogDir();

	strFilter.LoadString( IDS_LOG_FILTER );

	CFileDialog	dlg( TRUE,	//Open file dialog
					 "LOG",	//Default extension
					 NULL,	//Filename
					 OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_PATHMUSTEXIST,
					 strFilter,
					 this );

	dlg.m_ofn.lpstrInitialDir = (LPCTSTR)strInitDir;

	//Initialize the FileOPen dialog with the
	//	proper directory
	if( IDOK == dlg.DoModal() )
	{
		strFilename = dlg.GetPathName();

		//Open the log file
		OpenLogFile( strFilename );
	}

	AfxGetMainWnd()->Invalidate();
	AfxGetMainWnd()->UpdateWindow();
	GetParent()->Invalidate();
	GetParent()->UpdateWindow();
}

void CViewLog::OpenLogFile( const CString &strFilename )
{
	CStdioFile	file;
//	CEdit		*ptrWnd = (CEdit*)GetDlgItem( IDC_LOG );
	CListBox	*ptrWnd = (CListBox*)GetDlgItem( IDC_LOG );
	CString		strOneLine,
				strFullText;
	CDC			*dc;
	CSize		size;
	int			iOldExtent = 0;

	if( !ptrWnd )
		return;

	dc = ptrWnd->GetDC();

//	ptrWnd->SetSel( 0, -1 );
//	ptrWnd->Clear();
	ptrWnd->ResetContent();

	//Now, open the file and place the contents in the
	//	edit window
	try
	{
		if( file.Open( strFilename, CFile::modeRead ) )
		{
			while( file.ReadString( strOneLine ) )
			{
//				strFullText += ParseLogLine( strOneLine ) + "\r\n";
				ptrWnd->AddString( ParseLogLine( strOneLine ) );
				size = dc->GetTextExtent( strOneLine );
				if( size.cx > iOldExtent )
				{
					ptrWnd->SetHorizontalExtent( size.cx *2 );
					iOldExtent = size.cx * 2;
				}
			}
			
//			ptrWnd->SetWindowText( strFullText );

			file.Close();
		}
	}
	catch( ... )
	{
		CString strError;
		AfxFormatString1( strError, IDS_FILE_READ_ERROR, strFilename );
		
		AfxMessageBox( strError );
	}

	ptrWnd->ReleaseDC( dc );
	//ptrWnd->SetSel( -1, 0 );
	ptrWnd->PostMessage( WM_ACTIVATE, MAKELPARAM( WA_CLICKACTIVE, 0), 0L );
}


CString CViewLog::ParseLogLine( CString &strSource )
{
	CString strReturn,
			strTemp;
	int		iLoc,
			iIndex,
			iValue;
	CTime	*cTime;

	for( iIndex = 0; iIndex <= LOG_LINES; iIndex++ )
	{
		//Find the next comma
		iLoc = strSource.Find( _T(',') );

		if( iLoc == -1 )
		{
			strTemp = strSource;
			strSource.Empty();
		}
		else
		{
			//Take everything up to the comma and place it in strTemp
			strTemp = strSource.Left( iLoc );
			//Set strSource to the remainder for the next time through the
			//	loop.
			strSource = strSource.Right( strSource.GetLength() - iLoc - 1 );
		}

		//Switch on the iIndex which determines
		//	which field I am looking at
		switch( iIndex )
		{
		case LOG_FIELD_VTIME:

			int iYear,
				iMonth,
				iDay,
				iHour,
				iMinute,
				iSecond;

			sscanf( (LPCTSTR)strTemp,
					"%2X%2X%2X%2X%2X%2X",	
					&iYear,
					&iMonth,
					&iDay,
					&iHour,
					&iMinute,
					&iSecond );

			iYear += 1970;
			iMonth++;


			try
			{
				if( cTime = new CTime( iYear, iMonth, iDay, iHour, iMinute, iSecond ) )
				{
					strTemp = cTime->Format( "%m/%d/%y  %I:%M %p" );
					delete cTime;
				}
			}
			catch (std::bad_alloc &) {}
			break;
		case LOG_FIELD_EVENT:
			strTemp.Empty();
			//iValue = atoi( (LPCTSTR)strTemp );
			//GetEventString( iValue, strTemp );
			break;
		case LOG_FIELD_CATEGORY:
			strTemp.Empty();
			//iValue = atoi( (LPCTSTR)strTemp );
			//GetCategoryString( iValue, strTemp );
			break;
		case LOG_FIELD_LOGGER:
			iValue = atoi( (LPCTSTR)strTemp );
			GetLoggerString( iValue, strTemp );
			break;
		case LOG_FIELD_FIRST_ACTION:
		case LOG_FIELD_NEXT_ACTION:
		case LOG_FIELD_REAL_ACTION:
			iValue = atoi( (LPCTSTR)strTemp );
			GetActionString( iValue, strTemp );
			break;
		case LOG_FIELD_VIRUS_TYPE:
			strTemp.Empty();
			//iValue = atoi( (LPCTSTR)strTemp );
			//GetVirusTypeString( iValue, strTemp );
			break;
		//These are all strings. I simply remove the
		//	comma, and leave the string
		case LOG_FIELD_COMPNAME:
		case LOG_FIELD_USERNAME:
			strTemp.Empty();
			break;
		case LOG_FIELD_VIRUSNAME:
		case LGO_FIELD_FILENAME:
			break;

		default:
			break;
		}
		
		//Finally, copy the temp string into the return buffer
		if( !strTemp.IsEmpty() )
		{
			strReturn += " " + strTemp;
		}
	}

	//Now add the description field
	if( !strSource.IsEmpty() )
	{
		strReturn += " " + strSource;
	}

	return strReturn;
}

void CViewLog::GetEventString( int iValue, CString &strTemp )
{
	switch (iValue)
	{
	case GL_EVENT_IS_ALERT:
		strTemp.LoadString(IDS_GL_EVENT_IS_ALERT);
		break;

	case GL_EVENT_SCAN_STOP:
		strTemp.LoadString(IDS_GL_EVENT_SCAN_STOP);
		break;

	case GL_EVENT_SCAN_SUSPENDED:
		strTemp.LoadString(IDS_GL_EVENT_SCAN_SUSPENDED);
		break;

	case GL_EVENT_SCAN_START:
		strTemp.LoadString(IDS_GL_EVENT_SCAN_START);
		break;

	case GL_EVENT_SCAN_RESUMED:
		strTemp.LoadString(IDS_GL_EVENT_SCAN_RESUMED);
		break;

	case GL_EVENT_SCAN_DURATION_INSUFFICIENT:
		strTemp.LoadString(IDS_GL_EVENT_SCAN_DURATION_INSUFFICIENT);
		break;

	case GL_EVENT_PATTERN_UPDATE:
		strTemp.LoadString(IDS_GL_EVENT_PATTERN_UPDATE);
		break;

	case GL_EVENT_INFECTION:
		strTemp.LoadString(IDS_GL_EVENT_INFECTION);
		break;

	case GL_EVENT_FILE_NOT_OPEN:
		strTemp.LoadString(IDS_GL_EVENT_FILE_NOT_OPEN);
		break;

	case GL_EVENT_LOAD_PATTERN:
		strTemp.LoadString(IDS_GL_EVENT_LOAD_PATTERN);
		break;

	case GL_EVENT_CHECKSUM:
		strTemp.LoadString(IDS_GL_EVENT_CHECKSUM);
		break;

	case GL_EVENT_TRAP:
		strTemp.LoadString(IDS_GL_EVENT_TRAP);
		break;

	case GL_EVENT_CONFIG_CHANGE:
		strTemp.LoadString(IDS_GL_EVENT_CONFIG_CHANGE);
		break;

	case GL_EVENT_SHUTDOWN:
		strTemp.LoadString(IDS_GL_EVENT_SHUTDOWN);
		break;

	case GL_EVENT_STARTUP:
		strTemp.LoadString(IDS_GL_EVENT_STARTUP);
		break;

	case GL_EVENT_PATTERN_DOWNLOAD:
		strTemp.LoadString(IDS_GL_EVENT_PATTERN_DOWNLOAD);
		break;

	case GL_EVENT_TOO_MANY_VIRUSES:
		strTemp.LoadString(IDS_GL_EVENT_TOO_MANY_VIRUSES);
		break;
	}
}

void CViewLog::GetCategoryString( int iValue, CString &strTemp )
{
	switch (iValue)
	{
	case GL_CAT_INFECTION: // "Infection"
		strTemp.LoadString(IDS_GL_CAT_INFECTION);
		break;

	case GL_CAT_SUMMARY:	// "Summary"
		strTemp.LoadString(IDS_GL_CAT_SUMMARY);
		break;

	case GL_CAT_PATTERN:	// "Virus Pattern File"
		strTemp.LoadString(IDS_GL_CAT_PATTERN);
		break;

	case GL_CAT_SECURITY:	// "Security"
		strTemp.LoadString(IDS_GL_CAT_SECURITY);
		break;
	}
}

void CViewLog::GetLoggerString( int iValue, CString &strTemp )
{
	switch (iValue)
	{
	case LOGGER_Scheduled:	// "Scheduled Scan"
		strTemp.LoadString(IDS_LOGGER_Scheduled);
		break;

	case LOGGER_Manual:	// "Manual Scan"
		strTemp.LoadString(IDS_LOGGER_Manual);
		break;

	case LOGGER_Real_Time:	// "Realtime Protection"
		strTemp.LoadString(IDS_LOGGER_Real_Time);
		break;

	case LOGGER_Integrity_Shield:	// "Integrity Shield"
		strTemp.LoadString(IDS_LOGGER_Integrity_Shield);
		break;

	case LOGGER_Console: // "Console"
		strTemp.LoadString(IDS_LOGGER_Console);
		break;

	case LOGGER_VPDOWN: // "Pattern Downloader"
		strTemp.LoadString(IDS_LOGGER_VPDOWN);
		break;

	case LOGGER_System: // "System"
		strTemp.LoadString(IDS_LOGGER_SYSTEM);
		break;

	case LOGGER_Startup: // "Startup"
		strTemp.LoadString(IDS_LOGGER_SYSTEM);
		break;

	case LOGGER_Idle:
		strTemp.LoadString( IDS_LOGGER_IDLE );
		break;
	}

	strTemp += ":";
}

void CViewLog::GetActionString( int iValue, CString &strTemp )
{
	switch( iValue )
	{
	case AC_MOVE:
		strTemp.LoadString( IDS_ACTION_MOVE );
		break;
	case AC_RENAME:
		strTemp.LoadString( IDS_ACTION_RENAME );
		break;
	case AC_DEL:
		strTemp.LoadString( IDS_ACTION_DELETE );
		break;
	case AC_NOTHING:
		strTemp.LoadString( IDS_ACTION_LEAVEALONE );
		break;
	case AC_CLEAN:
		strTemp.LoadString( IDS_ACTION_CLEAN );
		break;
	}
}

void CViewLog::GetVirusTypeString( int iValue, CString &strTemp )
{
	//For now, I will ignore this field.
}

CString CViewLog::GetLogDir()
{
	HKEY	pKey;
	CString strDir,
			strLogDir;
	BYTE	*pbData;
	DWORD	dwSize;

	//First, get the proper directory out of the registry
	if( ERROR_SUCCESS == RegOpenKeyEx(  HKEY_LOCAL_MACHINE,
										szReg_Key_Main,
										0,
										KEY_READ,
										&pKey
									  )
	   )
	{
		
		
		//Get the main directory
		SymSaferRegQueryValueEx(	pKey,
							szReg_Val_LocalAppDir,
							NULL,
							NULL,
							NULL,
							&dwSize
							);
		pbData = (BYTE*)strDir.GetBuffer( dwSize );

		SymSaferRegQueryValueEx(	pKey,
							szReg_Val_LocalAppDir,
							NULL,
							NULL,
							pbData,
							&dwSize
							);
		strDir.ReleaseBuffer();


		//Now get the Log Dir
		SymSaferRegQueryValueEx(	pKey,
							szReg_Val_LogDir,
							NULL,
							NULL,
							NULL,
							&dwSize
							);
		pbData = (BYTE*)strLogDir.GetBuffer( dwSize );

		SymSaferRegQueryValueEx(	pKey,
							szReg_Val_LogDir,
							NULL,
							NULL,
							pbData,
							&dwSize
							);
		strLogDir.ReleaseBuffer();

		if( strLogDir.IsEmpty() )
		{
			//NOTE: I can hard-code this string because it
			//	is a directory - it WILL NOT be localized.
			strLogDir = "\\Logs";
		}

		RegCloseKey( pKey );
	}

	strDir += strLogDir;

	return strDir;
}



BOOL CViewLog::OnWizardFinish() 
{
	//Fire an event telling the App to close the task
	((CLDVPTaskFSApp*)AfxGetApp())->m_ptrCtl->NotifyEndTask();

	return CPropertyPage::OnWizardFinish();
}

void CViewLog::OnSetfocusLog() 
{
//	CEdit		*ptrWnd = (CEdit*)GetDlgItem( IDC_LOG );

	//Place the cursor at the beginning of the list
//	ptrWnd->SetSel( 1, 1 );
	//and remove the selection
//	ptrWnd->SetSel( -1, 0 );
}
