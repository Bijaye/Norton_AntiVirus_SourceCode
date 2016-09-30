// PrivMANDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PrivMAN.h"
#include "PMANDlg.h"
#include "statdlg.h"

#include "afxcmn.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define		WM_REFRESH		(WM_USER + 1)

BOOL	GrantPrivileges (LPTSTR lpszUser, LPTSTR lpszServer);
BOOL	RevokePrivileges (LPTSTR lpszUser, LPTSTR lpszServer);


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrivMANDlg dialog

CPrivMANDlg::CPrivMANDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPrivMANDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrivMANDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPrivMANDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrivMANDlg)
	DDX_Control(pDX, IDC_DOMAIN_LIST, m_Tree);
	DDX_Control(pDX, IDC_GRPUSR, m_SrcList);
	DDX_Control(pDX, IDC_NT_LIST, m_SelList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPrivMANDlg, CDialog)
	//{{AFX_MSG_MAP(CPrivMANDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE (WM_REFRESH, OnRefresh)
	ON_BN_CLICKED(IDC_USERS, OnUsers)
	ON_BN_CLICKED(IDC_GROUPS, OnGroups)
	ON_LBN_SELCHANGE(IDC_GRPUSR, OnSelchangeGrpusr)
	ON_NOTIFY(TVN_SELCHANGED, IDC_DOMAIN_LIST, OnSelchangedDomainList)
	ON_NOTIFY(NM_DBLCLK, IDC_DOMAIN_LIST, OnDblclkDomainList)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_CLEAR_ALL, OnClearAll)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_GRANT, OnGrant)
	ON_BN_CLICKED(IDC_REVOKE, OnRevoke)
	ON_BN_CLICKED(IDC_GLOBAL_GROUPS, OnGlobalGroups)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrivMANDlg message handlers

BOOL CPrivMANDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	if (!strAboutMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	PostMessage (WM_REFRESH, 0, 0L);

    m_hWaitCursor = ::LoadCursor (NULL, IDC_WAIT);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPrivMANDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPrivMANDlg::OnPaint()
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
HCURSOR CPrivMANDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


LONG CPrivMANDlg::OnRefresh (UINT wParam, LPARAM lParam)
{
    Refresh ();
    return 0;
}

VOID CPrivMANDlg::Refresh()
{
	InitImages ();

    m_Tree.SetImageList (&m_Image, TVSIL_NORMAL);

    EnumMachines (TVI_ROOT, NULL);

//    GetDlgItem (IDC_USERS) -> SendMessage (BM_SETCHECK, 1, 0L);
//    RefreshSrcList (SRC_USER);
    GetDlgItem (IDC_GROUPS) -> SendMessage (BM_SETCHECK, 1, 0L);
    RefreshSrcList (SRC_LOCAL_GROUP);
}

VOID CPrivMANDlg::InitImages()
{
    CBitmap     Bmp;

    m_Image.Create (16, 16, TRUE, 1, 0);

    Bmp.LoadBitmap (IDB_DOMAIN);
    m_nDomainBM = m_Image.Add (&Bmp, RGB (255, 255, 255));
    Bmp.DeleteObject ();
    Bmp.LoadBitmap (IDB_DOMAINSEL);
    m_nDomainSelBM = m_Image.Add (&Bmp, RGB (255, 255, 255));
    Bmp.DeleteObject ();
    Bmp.LoadBitmap (IDB_COMPUTER);
    m_nMachineBM = m_Image.Add (&Bmp, RGB (255, 255, 255));
    Bmp.DeleteObject ();
    Bmp.LoadBitmap (IDB_COMPSEL);
    m_nMachineSelBM = m_Image.Add (&Bmp, RGB (255, 255, 255));
    Bmp.DeleteObject ();

    Bmp.LoadBitmap (IDB_TOP);
    m_nTopBM = m_Image.Add (&Bmp, RGB (255, 255, 255));
    Bmp.DeleteObject ();

    Bmp.LoadBitmap (IDB_TOPMASK);
    m_nTopSelBM = m_Image.Add (&Bmp, RGB (255, 255, 255));
    Bmp.DeleteObject ();
}

VOID CPrivMANDlg::EnumMachines(HTREEITEM hCurItem, LPTSTR lpszName)
{
    DWORD               dwType = (hCurItem == TVI_ROOT) ? SV_TYPE_DOMAIN_ENUM : SV_TYPE_NT;
    DWORD               dwCount = 100;
//    LPTSTR              lpszName = NULL;
    HTREEITEM           hItem;
    LPSERVER_INFO_100   lpList;
    DWORD               dwIndex;

    if (hCurItem == TVI_ROOT)
        hItem = m_Tree.InsertItem (TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM,
                                    TEXT ("My Computer"),
                                    m_nTopBM,
                                    m_nTopSelBM,
                                    0,
                                    0,
                                    0L,
                                    TVI_ROOT,
                                    TVI_LAST);
    else
        hItem = hCurItem;

    lpList = (LPSERVER_INFO_100) GlobalAlloc (GPTR, dwCount * sizeof (SERVER_INFO_100));

    if (GetAllMachines (dwType, lpszName, &dwCount, &lpList))
    {
        for (dwIndex = 0; dwIndex < dwCount; dwIndex++)
        {
            m_Tree.InsertItem (TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM,
                            lpList[dwIndex].sv100_name,
                            (hCurItem == TVI_ROOT) ? m_nDomainBM : m_nMachineBM,
                            (hCurItem == TVI_ROOT) ? m_nDomainSelBM : m_nMachineSelBM,
                            0,
                            0,
                            (hCurItem == TVI_ROOT) ? 0L : 1L,
                            hItem,
                            TVI_LAST);
        }
    }
    GlobalFree (lpList);
}


BOOL CPrivMANDlg::GetAllMachines (DWORD dwType, LPTSTR lpszServer, LPDWORD lpdwCount, LPSERVER_INFO_100 *lplpList)
{
    DWORD               dwCount = *lpdwCount;
    LPSERVER_INFO_100   lpList = *lplpList;
    BOOL                bOver = FALSE;
    DWORD               dwRead = 0, dwTotal = 0, hResume = 0;
    NET_API_STATUS      naStat;
    HCURSOR             hCursor;

    hCursor = ::SetCursor (m_hWaitCursor);
    while (!bOver)
    {
        naStat = NetServerEnum (NULL,
                                100,
                                (LPBYTE *) &lpList,
                                dwCount * sizeof (SERVER_INFO_100),
                                &dwRead,
                                &dwTotal,
                                dwType,
                                lpszServer,
                                &hResume);
        if (naStat == ERROR_SUCCESS)
            bOver = TRUE;
        else
        {
            if (naStat == ERROR_MORE_DATA)
            {
                dwCount += dwCount;
                lpList = (LPSERVER_INFO_100)GlobalReAlloc (lpList,
                                    dwCount * sizeof (SERVER_INFO_100),
                                    GPTR);
            }
            else
                break;
        }
    }

    if (bOver)
    {
        *lpdwCount = dwRead;
        *lplpList = lpList;
    }
    ::SetCursor (hCursor);
    return bOver;
}

void CPrivMANDlg::OnUsers()
{
	RefreshSrcList (SRC_USER);
}

void CPrivMANDlg::OnGroups()
{
	RefreshSrcList (SRC_LOCAL_GROUP);
}

void CPrivMANDlg::OnSelchangeGrpusr()
{
    DWORD dwIndex = m_SrcList.GetCurSel () ;

	m_Tree.EnableWindow (dwIndex != LB_ERR);
}

void CPrivMANDlg::OnSelchangedDomainList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	m_hCurItem = pNMTreeView->itemNew.hItem;

    if (m_hCurItem != NULL)
        GetDlgItem (IDC_ADD) -> EnableWindow (TRUE);
    else
        GetDlgItem (IDC_ADD) -> EnableWindow (FALSE);

	*pResult = 0;
}

void CPrivMANDlg::OnDblclkDomainList(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_ITEM tvi;
    TCHAR   szDomain[MAX_COMPUTERNAME_LENGTH + 1];

    tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
    tvi.hItem = m_hCurItem;
    tvi.pszText = szDomain;
    tvi.cchTextMax = MAX_COMPUTERNAME_LENGTH + 1;

    if (m_Tree.GetItem (&tvi))
    {
        if (tvi.lParam == 1)
        {
            if (m_SelList.FindString (0, tvi.pszText) == LB_ERR)
            {
                m_SelList.AddString (tvi.pszText);
//                GetDlgItem (IDC_DELETE) -> EnableWindow (
//                                        m_SelList.GetCount () >= 1);
//                GetDlgItem (IDC_CLEAR_ALL) -> EnableWindow (
//                                        m_SelList.GetCount () >= 1);
            }
        }
        else if (!m_Tree.ItemHasChildren (m_hCurItem))
            EnumMachines (m_hCurItem, szDomain);
    }

    GetDlgItem (IDC_DELETE) -> EnableWindow (m_SelList.GetCount () > 0);
    GetDlgItem (IDC_CLEAR_ALL) -> EnableWindow (m_SelList.GetCount () > 0);
    GetDlgItem (IDC_GRANT) -> EnableWindow (m_SelList.GetCount () > 0);
    GetDlgItem (IDC_REVOKE) -> EnableWindow (m_SelList.GetCount () > 0);

	*pResult = 0;
}

void CPrivMANDlg::OnDelete()
{
    DWORD   dwIndex;
	if ((dwIndex = m_SelList.GetCurSel ()) != LB_ERR)
        m_SelList.DeleteString (dwIndex);

    GetDlgItem (IDC_DELETE) -> EnableWindow (m_SelList.GetCount () > 0);
    GetDlgItem (IDC_CLEAR_ALL) -> EnableWindow (m_SelList.GetCount () > 0);
    GetDlgItem (IDC_GRANT) -> EnableWindow (m_SelList.GetCount () > 0);
    GetDlgItem (IDC_REVOKE) -> EnableWindow (m_SelList.GetCount () > 0);
}

void CPrivMANDlg::OnClearAll()
{
	m_SelList.ResetContent ();
    GetDlgItem (IDC_DELETE) -> EnableWindow (FALSE);
    GetDlgItem (IDC_CLEAR_ALL) -> EnableWindow (FALSE);
    GetDlgItem (IDC_GRANT) -> EnableWindow (FALSE);
    GetDlgItem (IDC_REVOKE) -> EnableWindow (FALSE);
}

void CPrivMANDlg::OnAdd()
{
    LRESULT     lResult;
    CString     strName;
    LPTSTR      lpszName;
    HTREEITEM   hItem;
    TV_ITEM     tvi;
    TCHAR       szDomain[MAX_COMPUTERNAME_LENGTH + 1];

    hItem = m_Tree.GetSelectedItem ();
    if (hItem != NULL)
    {
        OnDblclkDomainList (NULL, &lResult);


        tvi.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
        tvi.hItem = m_hCurItem;
        tvi.pszText = szDomain;
        tvi.cchTextMax = MAX_COMPUTERNAME_LENGTH + 1;

        if (m_Tree.GetItem (&tvi))
        {
            if (lstrcmp (szDomain, TEXT ("My Computer")) == 0)
            {
                CString strMessageText;
                strMessageText.LoadString(IDS_SELECT_DOMAIN);

                AfxMessageBox (strMessageText);
                return;
            }
        }

        hItem = m_Tree.GetChildItem (m_hCurItem);
        while (hItem != NULL)
        {
            strName = m_Tree.GetItemText (hItem);
            lpszName = strName.GetBuffer (strName.GetLength ());
            if (m_SelList.FindString (0, lpszName) == LB_ERR)
                m_SelList.AddString (lpszName);
            strName.ReleaseBuffer ();

            hItem = m_Tree.GetNextSiblingItem (hItem);
        }
        GetDlgItem (IDC_DELETE) -> EnableWindow (m_SelList.GetCount () > 0);
        GetDlgItem (IDC_CLEAR_ALL) -> EnableWindow (m_SelList.GetCount () > 0);
        GetDlgItem (IDC_GRANT) -> EnableWindow (m_SelList.GetCount () > 0);
        GetDlgItem (IDC_REVOKE) -> EnableWindow (m_SelList.GetCount () > 0);
    }
    else
    {
        CString strMessageText;
        strMessageText.LoadString(IDS_SELECT_DOMAIN);

        AfxMessageBox (strMessageText);
    }
}

void CPrivMANDlg::OnGrant()
{
	DWORD	    dwIndex = m_SrcList.GetCurSel ();
	DWORD	    dwCount;
	TCHAR	    szUser[MAX_PATH + 1], szMachine[MAX_COMPUTERNAME_LENGTH + 1];
    CStatusDlg  dlg;
    TCHAR       szGrant[256];

    // Get the selected User name.
	m_SrcList.GetText (dwIndex, szUser);

    // Get the count of machines selected.
	dwCount = m_SelList.GetCount ();

    // Get the string denoting the operation.
    LoadString (AfxGetResourceHandle (), IDS_GRANT, szGrant, sizeof (szGrant));

    dlg.Create (IDD_STATUS);            // Create the display for status
    dlg.ShowWindow (SW_SHOW);           // Make it visible
    EnableWindow (FALSE);               // Set current window disbaled.
                                        // Status Dlg is Modelless and so
                                        // we need to disable the main window
                                        //

    // Grant privilieges on all selected machines
	for (dwIndex = 0; dwIndex < dwCount; dwIndex++)
	{
        // Get the current machine
		m_SelList.GetText (dwIndex, szMachine);

        // Update the msg on the status display
        dlg.SetMsg (szGrant, szUser, szMachine);

        // Grant the priviliege for the selected User/Group
		GrantPrivileges (szUser, szMachine);
	}

    dlg.DestroyWindow ();               // We are Done.  Kill the status
    EnableWindow (TRUE);                // Enable the main window back.
    SetActiveWindow ();                 // Set the focus to this
}

void CPrivMANDlg::OnRevoke()
{
	DWORD	    dwIndex = m_SrcList.GetCurSel ();
	DWORD	    dwCount;
	TCHAR	    szUser[MAX_PATH + 1], szMachine[MAX_COMPUTERNAME_LENGTH + 1];
    TCHAR       szRevoke[256];
    CStatusDlg  dlg;

    LoadString (AfxGetResourceHandle (), IDS_REVOKE, szRevoke, sizeof (szRevoke));

	m_SrcList.GetText (dwIndex, szUser);
	dwCount = m_SelList.GetCount ();

    dlg.Create (IDD_STATUS);
    dlg.ShowWindow (SW_SHOW);
    EnableWindow (FALSE);

	for (dwIndex = 0; dwIndex < dwCount; dwIndex++)
	{
		m_SelList.GetText (dwIndex, szMachine);
        dlg.SetMsg (szRevoke, szUser, szMachine);
		RevokePrivileges (szUser, szMachine);
	}
    dlg.DestroyWindow ();
    EnableWindow (TRUE);
    SetActiveWindow ();
}


VOID CPrivMANDlg::RefreshSrcList (short nType)
{
    BOOL                bOver = FALSE;
    DWORD               dwRead, dwTotal, hResume = 0, dwIndex, dwCur;
    NET_API_STATUS      naStat;
    LPUSER_INFO_0       pUser;
    LPLOCALGROUP_INFO_0 pGroup;
	LPGROUP_INFO_0		pGroups;
    DWORD               dwSrcIndex = 0;

    m_SrcList.ResetContent ();
    m_Tree.EnableWindow (FALSE);

	switch (nType)
	{
		case	SRC_USER:
			dwCur = 0;
		    while (!bOver)
			{
				naStat = NetUserEnum (NULL,
                                    0,
                                    FILTER_NORMAL_ACCOUNT,
                                    (LPBYTE *) &pUser,
                                    MAX_PREFERRED_LENGTH,
                                    &dwRead,
                                    &dwTotal,
                                    &hResume);
	            if (naStat == ERROR_SUCCESS)
		        {
			        for (dwIndex = 0; dwIndex < dwRead; dwIndex++)
				        m_SrcList.AddString (pUser[dwIndex].usri0_name);
	            }
		        else
			        break;

				NetApiBufferFree (pUser);

	            dwCur += dwRead;
		        if (dwCur >= dwTotal)
			        bOver = TRUE;
			}
			break;

		case	SRC_LOCAL_GROUP:
	        dwCur = 0;
		    while (!bOver)
			{
	            naStat = NetLocalGroupEnum (NULL,
                                    0,
                                    (LPBYTE *) &pGroup,
                                    MAX_PREFERRED_LENGTH,
                                    &dwRead,
                                    &dwTotal,
                                    &hResume);
		        if (naStat == ERROR_SUCCESS)
			    {
				    for (dwIndex = 0; dwIndex < dwRead; dwIndex++)
					    m_SrcList.AddString (pGroup[dwIndex].lgrpi0_name);
	            }
		        else
			        break;

				NetApiBufferFree (pGroup);

	            dwCur += dwRead;
		        if (dwCur >= dwTotal)
			        bOver = TRUE;
			}
			break;

		case	SRC_GLOBAL_GROUP:
			dwCur = 0;
			while (!bOver)
			{
				naStat = NetGroupEnum (NULL,
										0,
										(LPBYTE *)&pGroups,
										MAX_PREFERRED_LENGTH,
										&dwRead,
										&dwTotal,
										&hResume);
				if (naStat == ERROR_SUCCESS)
				{
					for (dwIndex = 0; dwIndex < dwRead; dwIndex++)
                    {
                        // Skip the dummy record "None" if no records.
                        if (lstrcmp (pGroups[dwIndex].grpi0_name, TEXT ("None")) != 0)
					        m_SrcList.AddString (pGroups[dwIndex].grpi0_name);
                    }
				}
				NetApiBufferFree (pGroups);

	            dwCur += dwRead;
		        if (dwCur >= dwTotal)
			        bOver = TRUE;
			}
			break;
	}

	ShowWindow (SW_SHOW);
}

void CPrivMANDlg::OnGlobalGroups()
{
	RefreshSrcList (SRC_GLOBAL_GROUP);
}

