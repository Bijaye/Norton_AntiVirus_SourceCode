//***************************************************************************
// $Header:   S:/MAPISEND/VCS/DirDialog.cpv   1.0   21 Apr 1998 15:20:16   CBROWN  $
//
// Description:
//
// Contains:
//
// See Also:
//
//***************************************************************************
// $Log:   S:/MAPISEND/VCS/DirDialog.cpv  $
// 
//    Rev 1.0   21 Apr 1998 15:20:16   CBROWN
// Initial revision.
//
//***************************************************************************

///////////////////////////////////////////////////////////////////////////
// DirDialog.cpp: implementation of the CDirDialog class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DirDialog.h"
#include "shlobj.h"
#include <dlgs.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//***************************************************************************
//
//***************************************************************************
COldFileDirDialog::COldFileDirDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, 
                                     LPCTSTR lpszFileName, DWORD dwFlags, 
                                     LPCTSTR lpszFilter, CWnd* pParentWnd) 
  : CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
  //{{AFX_DATA_INIT(COldFileDirDialog)
  //}}AFX_DATA_INIT
}


//***************************************************************************
//
//***************************************************************************
BEGIN_MESSAGE_MAP(COldFileDirDialog, CFileDialog)
    //{{AFX_MSG_MAP(COldFileDirDialog)
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


//***************************************************************************
//
//***************************************************************************
BOOL COldFileDirDialog::OnInitDialog()
{  
    //CenterWindow();

    //Let's hide these windows so the user cannot tab to them.  Note that in
    //the private template the coordinates for these guys are
    //*outside* the coordinates of the dlg window itself.  Without the following
    //ShowWindow()'s you would not see them, but could still tab to them.
    
    GetDlgItem(stc2)->ShowWindow(SW_HIDE);
    GetDlgItem(stc3)->ShowWindow(SW_HIDE);
    GetDlgItem(edt1)->ShowWindow(SW_HIDE);
    GetDlgItem(lst1)->ShowWindow(SW_HIDE);
    GetDlgItem(cmb1)->ShowWindow(SW_HIDE);
    
    //We must put something in this field, even though it is hidden.  This is
    //because if this field is empty, or has something like "*.txt" in it,
    //and the user hits OK, the dlg will NOT close.  We'll jam something in
    //there (like "Junk") so when the user hits OK, the dlg terminates.
    //Note that we'll deal with the "Junk" during return processing (see below)

    SetDlgItemText(edt1, _T("Junk"));

    //Now set the focus to the directories listbox.  Due to some painting
    //problems, we *must* also process the first WM_PAINT that comes through
    //and set the current selection at that point.  Setting the selection
    //here will NOT work.  See comment below in the on paint handler.
            
    GetDlgItem(lst2)->SetFocus();
            
    m_bDlgJustCameUp=TRUE;
               
    CFileDialog::OnInitDialog();
     
    return(FALSE);
}
  
  
//***************************************************************************
//
//***************************************************************************
void COldFileDirDialog::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    
    //This code makes the directory listbox "highlight" an entry when it first
    //comes up.  W/O this code, the focus is on the directory listbox, but no
    //focus rectangle is drawn and no entries are selected.  Ho hum.

    if (m_bDlgJustCameUp)
    {
        m_bDlgJustCameUp=FALSE;
        SendDlgItemMessage(lst2, LB_SETCURSEL, 0, 0L);
    }
    
    // Do not call CFileDialog::OnPaint() for painting messages
}


//***************************************************************************
//
//***************************************************************************
CDirDialog::CDirDialog() :
m_iImageIndex(0)
{

}

//***************************************************************************
//
//***************************************************************************
CDirDialog::~CDirDialog()
{

}

//***************************************************************************
//
//***************************************************************************
BOOL CDirDialog::DoBrowse
( 
	LPCTSTR szTitle, 
	CWnd* pParent, 
	LPCTSTR szInitFolder 
)
{	
	LPMALLOC pMalloc;

	m_strInitDir = szInitFolder;
	m_strTitle	 = szTitle;
	
	m_strPath.Empty();

    BYTE WinMajorVersion = LOBYTE(LOWORD(GetVersion()));
    if ((WinMajorVersion >= 4) ) //Running on Windows 95 shell and new style requested
    {

	    LPSTR string = m_strPath.GetBuffer(MAX_PATH);

        if( NOERROR != SHGetMalloc(&pMalloc) )
        {
            return FALSE;
        }

        BROWSEINFO	 bInfo;
        LPITEMIDLIST pidl;
    
	    ZeroMemory( (PVOID) &bInfo, sizeof(BROWSEINFO) );

         if( FALSE == m_strInitDir.IsEmpty() )
         {
              OLECHAR       olePath[MAX_PATH];
              ULONG         chEaten;
              ULONG         dwAttributes;
              HRESULT       hr;
              LPSHELLFOLDER pDesktopFolder;
              // // Get a pointer to the Desktop's IShellFolder interface. //
              if( SUCCEEDED( SHGetDesktopFolder(&pDesktopFolder) ) )
              {

    #ifndef _UNICODE						// If NOT defined UNICODE

                   // IShellFolder::ParseDisplayName requires the file name be in Unicode.
                   MultiByteToWideChar( CP_ACP, 
								        MB_PRECOMPOSED, 
									    LPCSTR(m_strInitDir), 
									    -1,
								        olePath, 
									    MAX_PATH );
    #else
			    
			       lstrcpy( olePath, LPCTSTR(m_strInitDir) );
    #endif

                   // Convert the path to an ITEMIDLIST.
                   hr = pDesktopFolder->ParseDisplayName( NULL,
												          NULL,
								                          olePath,
													      &chEaten,
													      &pidl,
													      &dwAttributes );

                   if( FAILED(hr) )
                   {
                        pMalloc->Free(pidl);
                        pMalloc->Release();

					    m_strPath.ReleaseBuffer();
                        return(FALSE);
                   }

                   bInfo.pidlRoot = pidl;
              }
         }
         
	    bInfo.hwndOwner	     = pParent->GetSafeHwnd();
        bInfo.pszDisplayName = (LPTSTR)LPCTSTR(m_strPath);
        bInfo.lpszTitle	     = LPCSTR(m_strTitle);
        bInfo.ulFlags		 = BIF_RETURNFSANCESTORS|BIF_RETURNONLYFSDIRS;

        if( NULL == (pidl = ::SHBrowseForFolder(&bInfo) ) )
        { 
		    m_strPath.ReleaseBuffer();
		    return(FALSE);
	    }
          
        m_iImageIndex = bInfo.iImage;

	    if( FALSE == (::SHGetPathFromIDList( pidl, string ) ))
        {
		    m_strPath.ReleaseBuffer();
		    pMalloc->Free(pidl);
            pMalloc->Release();
		    
		    m_strPath.ReleaseBuffer();
		    return(FALSE);
	    }

	    m_strPath.ReleaseBuffer();

        pMalloc->Free(pidl);
        pMalloc->Release();

    }
    else
    {
        COldFileDirDialog dlg( FALSE, 
                                NULL, 
                                NULL, 
                                OFN_HIDEREADONLY 
                              | OFN_OVERWRITEPROMPT 
                              | OFN_ENABLETEMPLATE, 
                               NULL, 
                               pParent );

        dlg.m_ofn.Flags          &= ~OFN_EXPLORER; //Turn of the explorer style customisation
        dlg.m_ofn.hInstance       = AfxGetInstanceHandle();
        dlg.m_ofn.lpTemplateName  = MAKEINTRESOURCE(FILEOPENORD);
        dlg.m_ofn.lpstrInitialDir = szInitFolder;
        dlg.m_ofn.lpstrTitle      = LPCSTR(m_strTitle);;

        if (dlg.DoModal() == IDOK)
        {
          dlg.m_ofn.lpstrFile[dlg.m_ofn.nFileOffset-1]=0; //Nuke the "Junk" text filename
        
          m_strPath = dlg.m_ofn.lpstrFile;
        }
        else
        {
            return(FALSE);
        }

    }
     
	return(TRUE);
}




