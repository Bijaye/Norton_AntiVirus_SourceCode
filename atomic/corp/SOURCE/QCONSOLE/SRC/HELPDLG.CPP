/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/HELPDLG.CPv   1.0   20 May 1998 17:15:32   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// HELPDLG - contains class declarations for CHelperDialog and CHelperPage
//	     helper classes.
//
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/HELPDLG.CPv  $
// 
//    Rev 1.0   20 May 1998 17:15:32   DBuches
// Initial revision.
// 
//    Rev 1.1   29 Dec 1997 16:53:58   DBuches
// Added help support.
// 
//    Rev 1.0   29 Dec 1997 16:33:38   DBuches
// Initial revision.
// 
//    Rev 1.1   20 Aug 1997 20:10:26   DBUCHES
// Fixed broken ? help in CHelperDialog derived dialogs.
//
//    Rev 1.0   20 Aug 1997 18:32:26   DBUCHES
// Initial revision.
//
//    Rev 1.8   28 Jul 1997 08:56:16   DBUCHES
// Fixed another problem with what's this help on grayed controls.
//
//    Rev 1.7   22 Jul 1997 13:13:50   DBUCHES
// Added call to NG_SetFontsInDialog()
//
//    Rev 1.6   21 Jul 1997 12:17:20   DBUCHES
// Fixed problem with context help on disabled controls.
//
//    Rev 1.5   20 Jun 1997 12:01:06   DBUCHES
// Added Help support.
//
//    Rev 1.4   11 Jun 1997 14:05:38   DALLEE
// Removed redefinition of WM_PASSWORD_CHECK. Define is now in SHEET.H.
//
//    Rev 1.3   10 Jun 1997 20:19:46   DBUCHES
// Added changes for alert password.
//
//    Rev 1.2   04 Jun 1997 22:15:50   DBUCHES
// Commented out help handlers.  Needed this for RoboHelp.
//
//    Rev 1.1   27 May 1997 20:56:12   DBUCHES
// I touched 30 files this rev.  Needed for support of NT Alerting.
//
//    Rev 1.0   14 May 1997 14:58:16   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "HelpDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


static BOOL CALLBACK EnumProc( HWND hwnd, LPARAM lParam );


/////////////////////////////////////////////////////////////////////////////
// CHelperDialog property page

//CMainDialog::CMainDialog(CWnd* pParent /*=NULL*/)
//  : CDialog(CMainDialog::IDD, pParent)

CHelperDialog::CHelperDialog(UINT nIDTemplate, CWnd * pParent /*=NULL*/):
    CDialog( nIDTemplate, pParent )
,m_pHelpHowToMap(NULL)
,m_pHelpWhatsThisMap(NULL)
,m_pHelpWhatsThisGrayMap(NULL)
,m_pCurrentContextWnd(NULL)
,m_uiMenuIndex((UINT)-1)
{
	Initialize();
}

void CHelperDialog::Initialize()
{
	//{{AFX_DATA_INIT(CHelperDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    m_dwHelpButtonHelpID = 0;
    m_pHelpWhatsThisGrayMap = NULL;
}

CHelperDialog::~CHelperDialog()
{
}

void CHelperDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHelperDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelperDialog, CDialog)
	//{{AFX_MSG_MAP(CHelperDialog)
	ON_WM_CONTEXTMENU()
	ON_WM_HELPINFO()
    ON_MESSAGE(WM_HELP, OnHelpMessage)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP_CONTENTS, OnHelpInfoDesk)
	ON_COMMAND(ID_WHATS_THIS, OnWhatsThis)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelperDialog message handlers


// This is a virtual function which checks whether there is a "what's this"
// help file for a given control ID.  Any class derived from CHelpDlg which
// needs to check this should provide their own version of this function.
BOOL CHelperDialog::MainDlgWhatsThisHelp(int ID)
{
	LPDWORD lpdwWT = m_pHelpWhatsThisMap;
	int index = 0;
	while(*lpdwWT != (DWORD)0)
	{
		if(*lpdwWT == (DWORD)ID)
			return TRUE;
		lpdwWT += 2;
	}
    return(FALSE);
}


void CHelperDialog::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here

	m_pCurrentContextWnd = pWnd;
	BOOL b;
	CMenu	HowToMenu;
    CMenu	menu;
  	CMenu*	pPopup = NULL;
    if (menu.LoadMenu( IDR_VIEW_CONTEXT ))
    {
		CString csMenuTitle;
        pPopup		= menu.GetSubMenu(2);
		//csMenuTitle.LoadString(IDS_HOW_TO);
		CMenu menuHowToPopUp;
		menuHowToPopUp.CreatePopupMenu();

		pPopup->InsertMenu( 2
								, MF_BYPOSITION|MF_POPUP|MF_STRING
								, (UINT)menuHowToPopUp.GetSafeHmenu()
								,csMenuTitle );

        // enum through the how to command ID's to see if we have to act on any of them.
		LPDWORD lpCmdID = m_pHelpHowToMap;
		BOOL bFoundSomething = FALSE;
		while( *lpCmdID != 0 /* the end of the map */)
		{
			if(HowToMenu.GetMenuString(*lpCmdID,csMenuTitle, MF_BYCOMMAND) > 0 )
			{
				menuHowToPopUp.AppendMenu(MF_STRING,*lpCmdID,csMenuTitle);
				bFoundSomething = TRUE;
			}
			lpCmdID += 2;  // incriment to the next  CMD ID
		}
		if(!bFoundSomething)
		{
			b = pPopup->DeleteMenu(2,MF_BYPOSITION);  // get rid of the How To popup if we dont have any how to's
		}

		if ( pWnd == this )
		{
			CPoint pt = point;
			WORD wCtrlId;
            ScreenToClient(&pt);
			m_pCurrentContextWnd = ChildWindowFromPoint(pt,CWP_SKIPINVISIBLE);

            if( m_pCurrentContextWnd )
                wCtrlId = (WORD) m_pCurrentContextWnd->GetDlgCtrlID();
            else
                wCtrlId = (WORD) IDC_STATIC;

			if( (m_pCurrentContextWnd == this) ||
                (wCtrlId == (WORD)IDC_STATIC)  ||
                !MainDlgWhatsThisHelp(wCtrlId)
                )
			{
				b = pPopup->DeleteMenu(0,MF_BYPOSITION); // get rid of the what's this if we dont have one
				b = pPopup->DeleteMenu(0,MF_BYPOSITION); // get rid of the separator
			}
			else
			{
				//////////////////////////////////////////////
				//  Check to see if the Control we are on is even in our map
				//  if not, then get rid of the what's this.
				bFoundSomething = FALSE;
				lpCmdID = m_pHelpWhatsThisMap;
				//++lpCmdID;  // get to the control ID's
				while( *lpCmdID != 0 /* the end of the map */)
				{
					if(*lpCmdID == wCtrlId)
					{
						bFoundSomething = TRUE;
						break;
					}
					lpCmdID += 2;  // incriment to the next  CMD ID
				}
				if(!bFoundSomething)
				{
					b = pPopup->DeleteMenu(0,MF_BYPOSITION); // get rid of the what's this if we dont have one
					b = pPopup->DeleteMenu(0,MF_BYPOSITION); // get rid of the separator
				}
			}
		}
        else if(pWnd)
            {
            int ID = pWnd->GetDlgCtrlID();
            if( ID == IDC_STATIC || !MainDlgWhatsThisHelp(ID))
			    {
				b = pPopup->DeleteMenu(0,MF_BYPOSITION); // get rid of the what's this if we dont have one
				b = pPopup->DeleteMenu(0,MF_BYPOSITION); // get rid of the separator
			    }
            }
		else if (( point.x == -1 ) && ( point.y == -1 ))
		{
			CRect r;
			if ( pWnd )
			{
				pWnd->GetWindowRect( &r );
				point.x = r.left +  r.Width() / 2;
				point.y = r.top  + r.Height() / 2;
			}
			else
				return;
		}

	}
	ASSERT( pPopup != NULL );
	if(pPopup != NULL)
		pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		point.x, point.y, this );

}

LRESULT CHelperDialog::OnHelpMessage(WPARAM wParam, LPARAM lParam)
{
    ASSERT(m_pHelpWhatsThisMap != NULL);
    ASSERT(m_pHelpHowToMap != NULL);
	wParam = wParam;
    // Handles F1 help
    LPHELPINFO  pHelpInfo = (LPHELPINFO)lParam;
    LPVOID lpMap = (LPVOID)m_pHelpWhatsThisMap;


    if(m_pHelpWhatsThisGrayMap != NULL &&
        !::IsWindowEnabled((HWND)pHelpInfo->hItemHandle))
        lpMap = (LPVOID)m_pHelpWhatsThisGrayMap;




    if (MainDlgWhatsThisHelp(pHelpInfo->iCtrlId))  // check if a help file exists for this ID
        {
        if (pHelpInfo->iContextType == HELPINFO_WINDOW)
            {
            ::WinHelp ((HWND)pHelpInfo->hItemHandle, AfxGetApp()->m_pszHelpFilePath,
                        HELP_WM_HELP, (DWORD)(LPVOID)lpMap );
            }
        }

	return 0;
}


void CHelperDialog::OnHelpInfoDesk()
{
	::WinHelp(
                GetSafeHwnd()
				,AfxGetApp()->m_pszHelpFilePath
				,HELP_FINDER
				, 0
				);
}

void CHelperDialog::OnWhatsThis()
{
	// make sure that we have a  valid window handle for the help
	HWND hWnd = NULL;
	if(		(m_pCurrentContextWnd != NULL)
		&&	((hWnd = m_pCurrentContextWnd->GetSafeHwnd()) != NULL)
		)
	{
    LPVOID lpMap =  (LPVOID)m_pHelpWhatsThisMap;
    if( m_pHelpWhatsThisGrayMap && !m_pCurrentContextWnd->IsWindowEnabled() )
        {
        lpMap = (LPVOID)m_pHelpWhatsThisGrayMap;
        }

    ::WinHelp (
        (HWND)hWnd
        ,AfxGetApp()->m_pszHelpFilePath
        ,HELP_WM_HELP
        ,(DWORD)(LPVOID)lpMap );

	}

}


// This function goes through the "How To" map to see if a command is in the map - if so, we display
// the "How To" popup
BOOL CHelperDialog::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // Check for help button click.
    if( LOWORD(wParam) == IDC_HELP_BUTTON && m_dwHelpButtonHelpID)
        {
        ::WinHelp (GetSafeHwnd(), AfxGetApp()->m_pszHelpFilePath,
            HELP_CONTEXT, m_dwHelpButtonHelpID );
        return TRUE;
        }
	return CDialog::OnCommand(wParam, lParam);
}

void CHelperDialog::ShutdownHelp(void)
{
    ::WinHelp(GetSafeHwnd(),
			  AfxGetApp()->m_pszHelpFilePath,
			  HELP_QUIT,
			  0);
}

BOOL CHelperDialog::OnHelpInfo(HELPINFO* pHelpInfo)
{
	// TODO: Add your message handler code here and/or call default
    DWORD dwCtrlID = ::GetDlgCtrlID((HWND)pHelpInfo->hItemHandle);

	// Make sure we have help for this control
    LPDWORD lpdwWT = m_pHelpWhatsThisMap;
	int index = 0;
    BOOL bFound = FALSE;
	while(*lpdwWT != (DWORD)0)
	    {
		if(*lpdwWT == (DWORD)dwCtrlID)
            {
            bFound = TRUE;
            break;
            }
		lpdwWT += 2;
	    }

    // If no control, bail out.
    if(!bFound)
        return FALSE;

    if (pHelpInfo->iContextType == HELPINFO_WINDOW)
    {
        ::WinHelp ((HWND)pHelpInfo->hItemHandle, AfxGetApp()->m_pszHelpFilePath,
            HELP_WM_HELP, (DWORD)(LPVOID)m_pHelpWhatsThisMap );
    }
    return CDialog::OnHelpInfo(pHelpInfo);
}

BOOL CHelperDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

//	NG_SetFontsInDialog(GetSafeHwnd(), 0);
    // TODO: Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


/////////////////////////////////////////////////////////////////////////////
// CHelperPage property page

IMPLEMENT_DYNCREATE(CHelperPage, CPropertyPage)

CHelperPage::CHelperPage() : CPropertyPage()
,m_pHelpHowToMap(NULL)
,m_pHelpWhatsThisMap(NULL)
,m_pCurrentContextWnd(NULL)
{
    Initialize();
}

CHelperPage::CHelperPage(UINT nIDTemplate):CPropertyPage(nIDTemplate)
,m_pHelpHowToMap(NULL)
,m_pHelpWhatsThisMap(NULL)
,m_pCurrentContextWnd(NULL)
,m_uiMenuIndex((UINT)-1)
{
    Initialize();
}

void CHelperPage::Initialize()
{
    //{{AFX_DATA_INIT(CHelperPage)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_dwHelpButtonHelpID = 0;
    m_psp.dwFlags |= PSP_HASHELP;
}

CHelperPage::~CHelperPage()
{
}

void CHelperPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CHelperPage)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHelperPage, CPropertyPage)
    //{{AFX_MSG_MAP(CHelperPage)
    ON_WM_CONTEXTMENU()
    ON_MESSAGE(WM_HELP, OnHelpMessage)
    //}}AFX_MSG_MAP
    ON_COMMAND(ID_HELP_CONTENTS, OnHelpInfoDesk)
    ON_COMMAND(ID_WHATS_THIS, OnWhatsThis)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHelperPage message handlers

void CHelperPage::OnContextMenu(CWnd* pWnd, CPoint point)
{
    // TODO: Add your message handler code here
    m_pCurrentContextWnd = pWnd;
    BOOL b;
    CMenu   HowToMenu;
    CMenu   menu;
    CMenu*  pPopup = NULL;
    if (menu.LoadMenu( IDR_VIEW_CONTEXT ))
    {
        CString csMenuTitle;
        pPopup      = menu.GetSubMenu(2);
        //csMenuTitle.LoadString(IDS_HOW_TO);
        CMenu menuHowToPopUp;
        menuHowToPopUp.CreatePopupMenu();

        pPopup->InsertMenu( 2
                                , MF_BYPOSITION|MF_POPUP|MF_STRING
                                , (UINT)menuHowToPopUp.GetSafeHmenu()
                                ,csMenuTitle );



        // enum through the how to command ID's to see if we have to act on any of them.
        LPDWORD lpCmdID = m_pHelpHowToMap;
        BOOL bFoundSomething = FALSE;
        while( *lpCmdID != 0 /* the end of the map */)
        {
            if(HowToMenu.GetMenuString(*lpCmdID,csMenuTitle, MF_BYCOMMAND) > 0 )
            {
                menuHowToPopUp.AppendMenu(MF_STRING,*lpCmdID,csMenuTitle);
                bFoundSomething = TRUE;
            }
            lpCmdID += 2;  // incriment to the next  CMD ID
        }
        if(!bFoundSomething)
        {
            b = pPopup->DeleteMenu(2,MF_BYPOSITION);  // get rid of the How To popup if we dont have any how to's
        }

        if ( pWnd == this )
        {
            CPoint pt = point;
            ScreenToClient(&pt);
            m_pCurrentContextWnd = ChildWindowFromPoint(pt,CWP_ALL | CWP_SKIPINVISIBLE);
            WORD wCtrlId;

            if( m_pCurrentContextWnd )
                wCtrlId = (WORD) m_pCurrentContextWnd->GetDlgCtrlID();
            else
                wCtrlId = (WORD) IDC_STATIC;

            if( (m_pCurrentContextWnd == this) ||
                (wCtrlId == (WORD)IDC_STATIC)  ||
                !HelpTopicExists(wCtrlId)
                )
            {
                b = pPopup->DeleteMenu(0,MF_BYPOSITION); // get rid of the what's this if we dont have one
                b = pPopup->DeleteMenu(0,MF_BYPOSITION); // get rid of the separator
            }
            else
            {
                //////////////////////////////////////////////
                //  Check to see if the Control we are on is even in our map
                //  if not, then get rid of the what's this.
                bFoundSomething = FALSE;
                lpCmdID = m_pHelpWhatsThisMap;
                //++lpCmdID;  // get to the control ID's
                while( *lpCmdID != 0 /* the end of the map */)
                {
                    if(*lpCmdID == wCtrlId)
                    {
                        bFoundSomething = TRUE;
                        break;
                    }
                    lpCmdID += 2;  // incriment to the next  CMD ID
                }
                if(!bFoundSomething)
                {
                    b = pPopup->DeleteMenu(0,MF_BYPOSITION); // get rid of the what's this if we dont have one
                    b = pPopup->DeleteMenu(0,MF_BYPOSITION); // get rid of the separator
                }
            }
        }
        else if(pWnd)
            {
            int ID = pWnd->GetDlgCtrlID();
            if( ID == IDC_STATIC || !HelpTopicExists(ID))
                {
                b = pPopup->DeleteMenu(0,MF_BYPOSITION); // get rid of the what's this if we dont have one
                b = pPopup->DeleteMenu(0,MF_BYPOSITION); // get rid of the separator
                }
            }

        else if (( point.x == -1 ) && ( point.y == -1 ))
        {
            CRect r;
            if ( pWnd )
            {
                pWnd->GetWindowRect( &r );
                point.x = r.left +  r.Width() / 2;
                point.y = r.top  + r.Height() / 2;
            }
            else
                return;
        }
//#endif

    }
    ASSERT( pPopup != NULL );
    if(pPopup != NULL)
        pPopup->TrackPopupMenu( TPM_LEFTALIGN | TPM_RIGHTBUTTON,
        point.x, point.y, this );
}

LRESULT CHelperPage::OnHelpMessage(WPARAM wParam, LPARAM lParam)
{
    ASSERT(m_pHelpWhatsThisMap != NULL);
    ASSERT(m_pHelpHowToMap != NULL);
    wParam = wParam;
    // Handles F1 help
    LPHELPINFO  pHelpInfo = (LPHELPINFO)lParam;
    DWORD dwCtrlID = ::GetDlgCtrlID((HWND)pHelpInfo->hItemHandle);

    // Make sure we have help for this control
    LPDWORD lpdwWT = m_pHelpWhatsThisMap;
    int index = 0;
    BOOL bFound = FALSE;
    while(*lpdwWT != (DWORD)0)
        {
        if(*lpdwWT == (DWORD)dwCtrlID)
            {
            bFound = TRUE;
            break;
            }
        lpdwWT += 2;
        }

    // If no control, bail out.
    if(!bFound)
        return FALSE;

    if (pHelpInfo->iContextType == HELPINFO_WINDOW)
    {
        ::WinHelp ((HWND)pHelpInfo->hItemHandle, AfxGetApp()->m_pszHelpFilePath,
            HELP_WM_HELP, (DWORD)(LPVOID)m_pHelpWhatsThisMap );
    }

    return 0;
}


void CHelperPage::OnHelpInfoDesk()
{
    ::WinHelp(
                GetSafeHwnd()
                ,AfxGetApp()->m_pszHelpFilePath
                ,HELP_FINDER
                , 0
                );
}

void CHelperPage::OnWhatsThis()
{
    // make sure that we have a  valid window handle for the help
    HWND hWnd = NULL;
    if(     (m_pCurrentContextWnd != NULL)
        &&  ((hWnd = m_pCurrentContextWnd->GetSafeHwnd()) != NULL)
        )
    {
        ::WinHelp (
            (HWND)hWnd
            ,AfxGetApp()->m_pszHelpFilePath
            ,HELP_WM_HELP
            ,(DWORD)(LPVOID)m_pHelpWhatsThisMap );

    }

}

// this is where we go through our how to map and see if the command comming through
// here is one which is in the map.  If it is, then lets fire of the how to help pop up.


BOOL CHelperPage::OnCommand(WPARAM wParam, LPARAM lParam)
{
    // Check for help button click.
    if( LOWORD(wParam) == ID_HELP && m_dwHelpButtonHelpID)
        {
        CString sHelpFile =  AfxGetApp()->m_pszHelpFilePath;
        sHelpFile += _T(" >TASK");

        ::WinHelp (GetSafeHwnd(), sHelpFile,
            HELP_CONTEXT, m_dwHelpButtonHelpID );
        return TRUE;
        }

// TODO: Add your specialized code here and/or call the base class
    return CPropertyPage::OnCommand(wParam, lParam);
}

BOOL CHelperPage::HelpTopicExists(int ID)
{
    LPDWORD lpdwWT = m_pHelpWhatsThisMap;
    int index = 0;
    while(*lpdwWT != (DWORD)0)
    {
        if(*lpdwWT == (DWORD)ID)
            return TRUE;
        lpdwWT += 2;
    }
    return(FALSE);

}


BOOL CHelperPage::OnInitDialog()
{
    return CPropertyPage::OnInitDialog();
}






