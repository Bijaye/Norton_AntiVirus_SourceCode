// VDBUnpackerTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VDBUnpackerTest.h"
#include "VDBUnpackerTestDlg.h"

#include "MyTraceClient.h"
#include "..\VDBUnpacker.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVDBUnpackerTestDlg dialog

CVDBUnpackerTestDlg::CVDBUnpackerTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVDBUnpackerTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVDBUnpackerTestDlg)
	m_logFile = _T("");
	m_package = _T("");
	m_result = _T("");
	m_targetDir = _T("");
	m_traceFile = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVDBUnpackerTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVDBUnpackerTestDlg)
	DDX_Text(pDX, IDC_LOG_FILE, m_logFile);
	DDX_Text(pDX, IDC_PACKAGE, m_package);
	DDX_Text(pDX, IDC_RESULT, m_result);
	DDX_Text(pDX, IDC_TARGET_DIR, m_targetDir);
	DDX_Text(pDX, IDC_TRACE_FILE, m_traceFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVDBUnpackerTestDlg, CDialog)
	//{{AFX_MSG_MAP(CVDBUnpackerTestDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnUnpack)
	ON_BN_CLICKED(ID_PRUNE, OnPrune)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVDBUnpackerTestDlg message handlers

BOOL CVDBUnpackerTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_package	= "D:\\test\\VDB\\00001621.vdb";
	m_targetDir = "D:\\test\\VDB\\target";
	m_result	= "";
	m_logFile	= "D:\\test\\VDB\\log\\1621.log";
	m_traceFile	= "D:\\test\\VDB\\trace\\1621.trc";

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVDBUnpackerTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVDBUnpackerTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CVDBUnpackerTestDlg::OnUnpack() 
{
	UpdateData( TRUE );

	std::string packagePath = m_package;
	std::string targetDir	= m_targetDir;
	std::string logFile		= m_logFile;
	std::string traceFile	= m_traceFile;

	m_result = "wait...";
	UpdateData( FALSE );

	std::string result;
	
	static MyTraceClient	traceClient; // make sure traceClient lives longer than signatureComm
	traceClient.EnableTracing();
	traceClient.RegisterThread( logFile.c_str(), traceFile.c_str(), "VDBUnpackerTest:" );
	

	VDBPackage package;

	package.Unpack( packagePath, targetDir, NULL, static_cast<void*>(&traceClient) );

	VDBPackage::ErrorCode errorCode = package.Error();

	switch ( errorCode )
	{
		case VDBPackage::NoError:				result = "NoError";					break;	
		case VDBPackage::PackageNotFoundError: 	result = "PackageNotFoundError";	break;	
		case VDBPackage::TargetDirError: 		result = "TargetDirError";			break;	
		case VDBPackage::NoFilesFoundError: 	result = "NoFilesFoundError"; 		break;	
		case VDBPackage::DiskFullError: 		result = "DiskFullError";			break;	
		case VDBPackage::VersionError: 			result = "VersionError"; 			break;	
		case VDBPackage::SystemException: 		result = "SystemException";			break;	
		case VDBPackage::ZipArchiveError: 		result = "ZipArchiveError";			break;	
		case VDBPackage::MemoryError: 			result = "MemoryError";				break;	
		case VDBPackage::ParameterError: 		result = "ParameterError";			break;	
		case VDBPackage::ZipNoFilesFoundError: 	result = "ZipNoFilesFoundError";	break;	
		case VDBPackage::UnexpectedEOFError: 	result = "UnexpectedEOFError";		break;	
		case VDBPackage::ZipUnknownError:		result = "ZipUnknownError";			break;	
		case VDBPackage::FileRemovingError:		result = "FileRemovingError";		break;	
		case VDBPackage::DirPruningError:		result = "DirPruningError";			break;	
		default:								result = "UnknownError";			break;	
	}
		
	m_result = result.c_str();

	UpdateData( FALSE );
}

void CVDBUnpackerTestDlg::OnPrune() 
{
	UpdateData( TRUE );

	std::string packagePath = m_package;
	std::string targetDir	= m_targetDir;
	std::string logFile		= m_logFile;
	std::string traceFile	= m_traceFile;

	m_result = "wait...";
	UpdateData( FALSE );

	std::string result;
	
	static MyTraceClient	traceClient; // make sure traceClient lives longer than signatureComm
	traceClient.EnableTracing();
	traceClient.RegisterThread( logFile.c_str(), traceFile.c_str(), "VDBUnpackerTest:" );
	

	VDBPackage package;

	package.Prune( packagePath, targetDir, NULL, static_cast<void*>(&traceClient) );

	VDBPackage::ErrorCode errorCode = package.Error();

	switch ( errorCode )
	{
		case VDBPackage::NoError:				result = "NoError";					break;	
		case VDBPackage::PackageNotFoundError: 	result = "PackageNotFoundError";	break;	
		case VDBPackage::TargetDirError: 		result = "TargetDirError";			break;	
		case VDBPackage::NoFilesFoundError: 	result = "NoFilesFoundError"; 		break;	
		case VDBPackage::DiskFullError: 		result = "DiskFullError";			break;	
		case VDBPackage::VersionError: 			result = "VersionError"; 			break;	
		case VDBPackage::SystemException: 		result = "SystemException";			break;	
		case VDBPackage::ZipArchiveError: 		result = "ZipArchiveError";			break;	
		case VDBPackage::MemoryError: 			result = "MemoryError";				break;	
		case VDBPackage::ParameterError: 		result = "ParameterError";			break;	
		case VDBPackage::ZipNoFilesFoundError: 	result = "ZipNoFilesFoundError";	break;	
		case VDBPackage::UnexpectedEOFError: 	result = "UnexpectedEOFError";		break;	
		case VDBPackage::ZipUnknownError:		result = "ZipUnknownError";			break;	
		case VDBPackage::FileRemovingError:		result = "FileRemovingError";		break;	
		case VDBPackage::DirPruningError:		result = "DirPruningError";			break;	
		default:								result = "UnknownError";			break;	
	}
		
	m_result = result.c_str();

	UpdateData( FALSE );
}
