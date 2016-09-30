//***************************************************************************
// $Header:   S:/MAPISEND/VCS/DirDialog.h_v   1.0   21 Apr 1998 15:24:10   CBROWN  $
//
// Description:
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/MAPISEND/VCS/DirDialog.h_v  $
// 
//    Rev 1.0   21 Apr 1998 15:24:10   CBROWN
// Initial revision.
//
//***************************************************************************

//***************************************************************************
// DirDialog.h: interface for the CDirDialog class.
//
//***************************************************************************
#if ! defined(AFX_DIRDIALOG_H__62FFAC92_1DEE_11D1_B87A_0060979CDF6D__INCLUDED_)
#define AFX_DIRDIALOG_H__62FFAC92_1DEE_11D1_B87A_0060979CDF6D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "StdAfx.h"

//***************************************************************************
//***************************************************************************
class CDirDialog : public CObject
{
private:

     CString m_strPath;
     CString m_strInitDir;
     CString m_strTitle;
     int  m_iImageIndex;

	 CDirDialog( CDirDialog& obj ){ (void)obj; };  // Copy removed...

public:

     CDirDialog();
     virtual ~CDirDialog();

     BOOL DoBrowse( LPCTSTR szTitle, CWnd* pParent = NULL, LPCTSTR szInitFolder = NULL );

	 inline LPCSTR GetPath( void ) const { return m_strPath; }
	 inline int GetImageIndex( void ) const { return m_iImageIndex; }
};

//***************************************************************************
//***************************************************************************
class COldFileDirDialog : public CFileDialog
{
public:
// Public data members
  BOOL m_bDlgJustCameUp;
    
// Constructors
  COldFileDirDialog(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
                 LPCTSTR lpszDefExt = NULL,
                 LPCTSTR lpszFileName = NULL,
                 DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                 LPCTSTR lpszFilter = NULL,
                 CWnd* pParentWnd = NULL);
                                          
// Implementation
protected:
  //{{AFX_MSG(COldFileDirDialog)
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()
};


#endif //!defined(AFX_DIRDIALOG_H__62FFAC92_1DEE_11D1_B87A_0060979CDF6D__INCLUDED_)


