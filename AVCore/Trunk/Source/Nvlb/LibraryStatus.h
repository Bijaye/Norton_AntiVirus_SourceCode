#if !defined(AFX_LIBRARYSTATUS_H__41743FDC_324D_4F50_99C8_D743B63D7D5E__INCLUDED_)
#define AFX_LIBRARYSTATUS_H__41743FDC_324D_4F50_99C8_D743B63D7D5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LibraryStatus.h : header file
//

#include "shared.h"

UINT ProcessFilesProc( LPVOID lpvObject );


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
	WORD	wVirusID,
	LPCTSTR lpszFileName, 
	LPCTSTR lpszVirLibDir
);


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
);

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
);


/////////////////////////////////////////////////////////////////////////////
// CLibraryStatus dialog

class CLibraryStatus : public CDialog
{
// Construction
public:
	void ProcessFiles();
	CString m_Defs_Dir;
	CString	m_Inf_Path;
	CString	m_Incoming_Dir;
	CString m_Virlib_Dir;
	int		m_HeurLevel;
	CLibraryStatus(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLibraryStatus)
	enum { IDD = IDD_STATUS };
	CStatic	m_Files_Processed;
	CStatic	m_Files_Added;
	CStatic	m_Scan_Status_Ctrl;
	CString	m_Scan_Status;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLibraryStatus)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLibraryStatus)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIBRARYSTATUS_H__41743FDC_324D_4F50_99C8_D743B63D7D5E__INCLUDED_)
