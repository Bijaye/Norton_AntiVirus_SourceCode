// LibraryStatus.cpp : implementation file
//

#include "stdafx.h"
#include "nvlb.h"
#include "LibraryStatus.h"
#include "navengine.h"
#include "frecurse.h"
#include <afxmt.h>
#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// Global declaration
///////////////////////////////////////////////////////////////////////////////
CWinThread*		m_UIThread;
CMutex			m_Mutex(FALSE, _T("UIMutex"));
CNAVEngine		AVEngine;
BOOL			gbStopThread = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CLibraryStatus dialog


CLibraryStatus::CLibraryStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CLibraryStatus::IDD, pParent)
{
//	m_Mutex = ::CreateMutex( NULL, FALSE, NULL );

	//{{AFX_DATA_INIT(CLibraryStatus)
	m_Scan_Status = _T("");
	//}}AFX_DATA_INIT
}


void CLibraryStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLibraryStatus)
	DDX_Control(pDX, IDC_FILES_PROCESSED, m_Files_Processed);
	DDX_Control(pDX, IDC_FILES_ADDED, m_Files_Added);
	DDX_Control(pDX, IDC_SCAN_STATUS, m_Scan_Status_Ctrl);
	DDX_Text(pDX, IDC_SCAN_STATUS, m_Scan_Status);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLibraryStatus, CDialog)
	//{{AFX_MSG_MAP(CLibraryStatus)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLibraryStatus message handlers

void CLibraryStatus::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	//m_UIThread = AfxBeginThread( ProcessFilesProc, this );
	
}


BOOL CLibraryStatus::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	AVEngine.SetDefLocation (m_Defs_Dir);
	AVEngine.SetINIFile (m_Inf_Path);
	AVEngine.SetHeuristicsLevel (m_HeurLevel);

	if (AVEngine.InitNAVEngine() != NAV_OK)
	{
		MessageBox ("Error initializing AntiVirus Engine!", "Error", MB_OK);
	}


	gbStopThread = FALSE;

	m_UIThread = AfxBeginThread(ProcessFilesProc,this); 
	m_UIThread->ResumeThread();


	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLibraryStatus::OnClose() 
{
	m_UIThread = NULL;
	AVEngine.UnloadNAVEngine();
	CDialog::OnClose();
}


UINT ProcessFilesProc( LPVOID lpvObject )
{
	CLibraryStatus* lpLibraryStatus;

	lpLibraryStatus = reinterpret_cast< CLibraryStatus* > ( lpvObject );
	lpLibraryStatus->ProcessFiles();
	return( 0 );
}


void CLibraryStatus::ProcessFiles()
{
	char				szFilePath[_MAX_PATH]={0};
	char				szBackupFile[_MAX_PATH]={0};
	char				szVirLibDir[_MAX_PATH]={0};
	char				szArchivePath[_MAX_PATH]={0};
	char				szVirPathName[_MAX_PATH]={0};
	char				szVirusName[30];
	char				szResultDlgBuffer[4096]={0};
	DWORD				dwFilesProcessed = 0;
	DWORD				dwFilesAdded = 0;
	BOOL				bResult = FALSE;
	CRecurseFolder		RecurseFolder;
	DWORD				dwResult;
	char				szFileProcessed[20]={0};
	char				szFileAdded[20]={0};

	m_Mutex.Lock(INFINITE);

	sprintf(szFilePath, "%s\\*.*", m_Incoming_Dir);

	if( RecurseFolder.FindFile ( szFilePath ,0, TRUE) )
	{


		while(RecurseFolder.FindNextFile())
		{
			if( gbStopThread)
			{
				m_Mutex.Unlock();
				RecurseFolder.Close();
				AVEngine.UnloadNAVEngine();
				AfxEndThread( 0 );
				m_UIThread = NULL;
				m_Scan_Status_Ctrl.SetWindowText("Cancelled!");
				MessageBox("User Cancelled library creation!", "Alert!", MB_ICONSTOP|MB_OK);
				return;
			}

			if(!RecurseFolder.IsFolder())
			{
				strcpy(szFilePath, RecurseFolder.GetFilePath());
				m_Scan_Status_Ctrl.SetWindowText(szFilePath);


				// Make a Back up of original file here
				CreateBackupFile(szFilePath, szBackupFile, "vlt");
				
				// Scan file here
				if ((dwResult = AVEngine.ScanFile(szBackupFile)) == NAV_OK)
				{
					
					// Increament Files Processed number
					dwFilesProcessed++;

					// Clear Virus Name
					szVirusName[0] = NULL;

					// Store Virus Name
					strcpy( szVirusName, (LPSTR)AVEngine.GetVirusName());
				

					if (szVirusName[0] != NULL )
					{


						// Attempt file repair here
						if ( (dwResult = AVEngine.RepairFile(szBackupFile)) == NAV_OK )
						{

							// Create repaired directory
							sprintf(szVirLibDir, "%s\\repaired", m_Virlib_Dir);
							GenerateDirNameFromVirusName(szVirusName,
								                         szVirLibDir,
														 szVirPathName);


							// Archive repaired file in repaired folder
							if (CreateDir(szVirPathName))
							{
								ArchiveFile(AVEngine.GetVirusID(), szBackupFile, szVirPathName);
							}



							// Create scanrep directory
							sprintf(szVirLibDir, "%s\\scanrep", m_Virlib_Dir);
							GenerateDirNameFromVirusName(szVirusName,
								                         szVirLibDir,
														 szVirPathName);

	
							// Archive file in scanrep folder
							if (CreateDir(szVirPathName))
							{
								ArchiveFile(AVEngine.GetVirusID(), szFilePath, szVirPathName);
							}



						}
						else if(dwResult == NAV_CANT_REPAIR )
						{

							// Create Directory
							sprintf(szVirLibDir, "%s\\scanonly", m_Virlib_Dir);
							GenerateDirNameFromVirusName(szVirusName,
								                         szVirLibDir,
														 szVirPathName);

	
							if (CreateDir(szVirPathName))
							{
								ArchiveFile(AVEngine.GetVirusID(), szFilePath, szVirPathName);
							}

						}

						// Increament Files Processed number
						dwFilesAdded++;

					}

					
					ltoa( dwFilesProcessed, szFileProcessed, 10);
					m_Files_Processed.SetWindowText(szFileProcessed);

					ltoa( dwFilesAdded, szFileAdded, 10);
					m_Files_Added.SetWindowText(szFileAdded);

				}

				// Remove Files
				DeleteFile(szFilePath);
				DeleteFile(szBackupFile);
			}

		}

	}

	RecurseFolder.Close();

	m_Mutex.Unlock();
	m_Scan_Status_Ctrl.SetWindowText("Done!");
	sprintf(szResultDlgBuffer, "Files processed:    %d\nFiles Add:    %d", dwFilesProcessed, dwFilesAdded);
	MessageBox(szResultDlgBuffer, "Library creation complete!", MB_OK);
	this->SendMessage(WM_CLOSE, 0, 0 ); 
}

void CLibraryStatus::OnCancel() 
{
	// TODO: Add extra cleanup here
	gbStopThread = TRUE;
	CDialog::OnCancel();
}


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ArchiveFile()
//
//	Description:
//		Extracts the name part of a file name.
//			example:	passing in [c:\some\dir\test.file] returns [c:\some\dir]
//
//
//	Parameters:
//		wVirusID			Virus ID
//		lpszFilePath		Pointer to full file path name
//		lpszVirLibDir		Pointer to virus library directory
//
//	Returns:
//		TRUE
//		FALSE
//
///////////////////////////////////////////////////////////////////////////////
BOOL ArchiveFile
( 
	WORD  wVirusID,
	LPCTSTR lpszFileName, 
	LPCTSTR lpszVirLibDir
)
{
	UINT	uFileCnt = 0;
	char	szFileName[_MAX_PATH]={0};
	char	szNewFileName[_MAX_PATH]={0};
	BOOL	bCopyResult = FALSE;
	char	szFileExt[_MAX_PATH];
	char	szVirDir[_MAX_PATH];
	int		iStrLen;

	HANDLE			hDirectory = NULL;
	WIN32_FIND_DATA stFileData = {0};

	
	ExtractFileName(lpszFileName, szFileName);
	ExtractFileExt(szFileName, szFileExt);

	// Strip off last slash
	strcpy(szVirDir, lpszVirLibDir);
	iStrLen = strlen(szVirDir);

	if(szVirDir[iStrLen-1] == '\\')
	{
		szVirDir[iStrLen-1] = NULL;
	}


	while( !bCopyResult )
	{

		// Check for file count greater than 0x9999
		if(uFileCnt == 10000)
		{
			return( FALSE );
		}

		// Increament file count number
		uFileCnt++;

		// Generate File
		sprintf(szFileName, "%s\\%.4x%.4d.*", szVirDir, wVirusID, uFileCnt);

		// Reset directory handle
		hDirectory = NULL;

		// Check to see if file exists already
		if ((hDirectory = FindFirstFile( szFileName, &stFileData)) != INVALID_HANDLE_VALUE)
		{
			// Close directory
			FindClose(hDirectory);
			bCopyResult = FALSE;
		}
		else
		{
			sprintf(szNewFileName, "%s\\%.4x%.4d.%s", szVirDir, wVirusID, uFileCnt, szFileExt);
			MoveFile( lpszFileName, szNewFileName);
			return( TRUE );
		}

	}

	return (FALSE);
}

///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL CreateBackupFile()
//
//	Description:
//		Create a temp backup file.
//			example:	passing in [c:\dir\test.exe] returns [c:\temp\tmp424.exe]
//
//
//	Parameters:
//		lpszFileName		Pointer full file name including the extension
//		lpszNewFile			Pointer to new file name
//		lpszPrefix			Pointer to prefix
//
//	Returns:
//		TRUE
//		FALSE
//
///////////////////////////////////////////////////////////////////////////////
BOOL CreateBackupFile
(
	LPCTSTR lpszFileName,
	LPTSTR lpszNewFile,
	LPCTSTR lpszPrefix
)
{
	char	szFileExt[_MAX_PATH]={0};
	char	szFileName[_MAX_PATH]={0};
	char	szPrefix[_MAX_PATH]={0};
	char	szTempPath[_MAX_PATH]={0};
	UINT	uTmpNumber;

	// Get file prefix	
	if(lpszPrefix == NULL)
	{
		strcpy(szPrefix, "");
	}
	else
	{
		strcpy(szPrefix, lpszPrefix);
	}

	// Get windows temp path
	GetTempPath( sizeof(szTempPath), szTempPath);

	// Extract file name
	ExtractFileName(lpszFileName, szFileName);

	// Get file extension
	ExtractFileExt( szFileName, szFileExt);

	// Generate random number for file
	srand( (unsigned)time( NULL ) );
	uTmpNumber = rand();

	// Generate new file path name
	sprintf(lpszNewFile, "%s%s%x.%s", szTempPath, szPrefix, uTmpNumber, szFileExt);
	
	// Copy file to new location and return new file path name
	return( CopyFile(lpszFileName, lpszNewFile, TRUE ) );
}

///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void GenerateDirNameFromVirusName()
//
//	Description:
//		Extracts the name part of a file name.
//			example:	passing in [c:\some\dir\test.file] returns [c:\some\dir]
//
//
//	Parameters:
//		lpszFilePath		Pointer to full file path name
//		lpszPathName		Pointer to path name
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void GenerateDirNameFromVirusName
(
	LPCTSTR	lpszVirusName,
	LPCTSTR	lpszVirusDir,
	LPTSTR	lpszPathName
)
{
	char	szStr[_MAX_PATH];
	char	szVirDir[_MAX_PATH];
	LPTSTR	lpszStr;
	int		iStrLen;

	lpszStr = (LPTSTR)&szStr;

	// Strip off last slash
	strcpy(szVirDir, lpszVirusDir);
	iStrLen = strlen(szVirDir);

	if(szVirDir[iStrLen-1] == '\\')
	{
		szVirDir[iStrLen-1] = NULL;
	}
	

	// Replace all invalid characters with under score "_"
	strcpy(szStr, lpszVirusName);
	while(*lpszStr)
	{
		switch(*lpszStr)
		{

			case '\\':
			case '/':
			case ':':
			case '*':
			case '?':
			case '\"':
			case '<':
			case '>':
			case '|':
				*lpszStr = '_';
				break;

		}

		// Increment pointer
		*lpszStr++;
	}

	// Return virus path string
	sprintf(lpszPathName, "%s\\%s",szVirDir, szStr );

}

