// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: WProtect32Doc.cpp
//  Purpose: CWProtect32Doc Implementation file
//
//	Date: 2-3-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "WProtect32.h"
#include "MainFrm.h"
#include "ClientReg.h"
#include "TaskPadView.h"
#include "WProtect32Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWProtect32Doc, CDocument)

BEGIN_MESSAGE_MAP(CWProtect32Doc, CDocument)
	//{{AFX_MSG_MAP(CWProtect32Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CWProtect32Doc::CWProtect32Doc()
{
	DWORD dwError = 0;

	m_fRecording = 0;
	m_bRecordingStarted = FALSE;
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CWProtect32Doc::~CWProtect32Doc()
{
}

//----------------------------------------------------------------
// OnNewDocument
//----------------------------------------------------------------
BOOL CWProtect32Doc::OnNewDocument()
{
	CString strTitle;

	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

//----------------------------------------------------------------
// Serialize
//----------------------------------------------------------------
void CWProtect32Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// add storing code here
	}
	else
	{
		// add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWProtect32Doc diagnostics

#ifdef _DEBUG
void CWProtect32Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWProtect32Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

//---------------------------------------------------------------
// CancelRecord
//---------------------------------------------------------------
void CWProtect32Doc::CancelRecord()
{
	//First, cancel the recording
	m_fRecording = 0;
	m_bRecordingStarted = FALSE;
}

//---------------------------------------------------------------
// TaskExists
//---------------------------------------------------------------
BOOL CWProtect32Doc::TaskExists( const CString &strTaskname )
{
	CString strTemp,
			strKey;
	HKEY	pKey;
	int		iIndex; 
	TCHAR	*keys[] = {	szReg_Value_Startup,
						szReg_Value_Scheduled,
						szReg_Value_Scripted
						};

	//Test to see if the taskname exists under any
	//	of the groups we save under
		//Strip out all the forward slashes
	strTemp = strTaskname;
	while( -1 != ( iIndex = strTemp.Find( _T('\\') ) ) )
	{
		strTemp.SetAt( iIndex, _T('~') );
	}

	for( int x = 0; x < 3; x++ )
	{
		strKey.Format( "%s\\%s\\%s\\%s", szReg_Key_Main, szReg_Key_CustomTasks, keys[ x ], strTemp );

		//Try to open the key under all three groups to
		//	see if it exists
		if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_CURRENT_USER,
											strKey,
											REG_OPTION_NON_VOLATILE,
											KEY_READ | KEY_WRITE | KEY_CREATE_SUB_KEY,
											&pKey ) )
		{
			RegCloseKey( pKey );
			return TRUE;
		}
	}

	return FALSE;
}

//---------------------------------------------------------------
// ValidateSettings
//---------------------------------------------------------------
BOOL CWProtect32Doc::ValidateSettings()
{
	HKEY	hKey;
	CString strFoundDrives,
			strSubKey;
	DWORD	dwIndex,
			dwSize;
	TCHAR	szPath[ MAX_PATH ];

	for( int x = 0; x < 2; x++ )
	{

		if( x == 0 )
		{
///			strSubKey.Format( "%s\\%s", m_strCurrentTaskKey, szReg_Key_Directories );
		}
		else
		{
//			strSubKey.Format( "%s\\%s", m_strCurrentTaskKey, szReg_Key_Files );
		}

		if( ERROR_SUCCESS == RegOpenKeyEx(	HKEY_CURRENT_USER,
											strSubKey,
											0,
											KEY_READ | KEY_WRITE | KEY_CREATE_SUB_KEY,
											&hKey ) )
		{
			dwIndex = 0;
			dwSize = MAX_PATH;
			while( ERROR_SUCCESS == RegEnumValue(	hKey,
													dwIndex,
													szPath,
													&dwSize,
													NULL,
													NULL,
													NULL,
													NULL
													) )
			{
				dwIndex++;
				//Set the third character to NULL.
				//	This should chop off the path name to
				//	a reference to the root directory
				//	i.e., c:[slash]Autoexec.bat becomes c:[slash]
				szPath[3] = 0x00;
				UINT uDriveType = GetDriveType( szPath );

				//Make sure this drive is not in my found drives string,
				//	AND make sure it is either REMOVABLE or CDROM
				if( ( -1 == strFoundDrives.Find( szPath[0] ) )	&&
					( uDriveType == DRIVE_REMOVABLE ||
					  uDriveType == DRIVE_CDROM ) )
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

						strDrive.Format( "%c:\\*.*", szPath[0] );	    
						memset( &info, '\0', sizeof( info ) );
						
						//If the FindFirst fails, then we can't get to the
						//	drive.
						if( INVALID_HANDLE_VALUE != ( hFind = FindFirstFile( (LPCTSTR)strDrive,  &info ) ) )
						{
							//Add the drive to my "cache"
							strFoundDrives += (CString)szPath[0];
							bRetry = FALSE;
						}
						else
						{               
							//I now know the disk is empty. Before
							//	I infrom the user of a missing disk,
							//	I will try to write a file to it.
							CFile			file;
							CFileException	error;

							strDrive.Format( "%c:\\k.sk", szPath[0] );

							BOOL bFileOpened = file.Open( (LPCTSTR)strDrive, CFile::modeCreate, &error );

							if( !bFileOpened &&
								error.m_cause == CFileException::hardIO )
							{
								CString strError;
	                    		strDrive.Format( "%c:\\", szPath[0] );	    
						
								AfxFormatString1( strError, IDS_NOT_READY, (LPCTSTR)strDrive );
								if( IDRETRY == AfxMessageBox( strError, MB_RETRYCANCEL | MB_ICONEXCLAMATION ) )
								{   
									bRetry = TRUE;
								}
								else
								{
									FindClose( hFind );
									return FALSE;
								}
							}
							else
							{
								//Remove my temp file
								if( bFileOpened )
								{
									file.Close();
									CFile::Remove( (LPCTSTR)strDrive );
								}
							}
						}

						FindClose( hFind );
					}while( bRetry );
											
					SetErrorMode( uintPrevState );
				} //End if Removable

			} //End if RegEnumvalue
		} //end If opened Reg Key
	} //end for

	return TRUE;
}

BOOL CWProtect32Doc::CanCloseFrame(CFrameWnd* pFrame) 
{
	//If we are in the middle of a scan, then check with
	//	the user to make sure they want to cancel
	if( ((CMainFrame*)pFrame)->IsScanning()  )
	{
		CString strMessage;

		strMessage.LoadString( IDS_STOP_CURRENT_SCAN );
		
		if( IDYES == AfxMessageBox( strMessage, MB_YESNO ) )
		{
			pFrame->SendMessage( WM_COMMAND, MAKEWPARAM( UWM_STOP_CURRENT_SCAN, 0 ), 0L );
		}
		else
		{
			return FALSE;
		}
	}

	
	return CDocument::CanCloseFrame(pFrame);
}

