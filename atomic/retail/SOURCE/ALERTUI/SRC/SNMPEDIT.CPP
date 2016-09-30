/////////////////////////////////////////////////////////////////////
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/SNMPEdit.cpv   1.0   04 Jun 1997 21:18:58   DBUCHES  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/SNMPEdit.cpv  $
// 
//    Rev 1.0   04 Jun 1997 21:18:58   DBUCHES
// Initial revision.
// 
// SNMPEdit.cpp : implementation file
//

#include "stdafx.h"
#include "platform.h"
#include "xapi.h"

#include "alertui.h"
#include "SNMPEdit.h"
#include "wsipx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSNMPEditDlg dialog


CSNMPEditDlg::CSNMPEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSNMPEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSNMPEditDlg)
	m_iProtocol = 0;
	m_sTargetName = _T("");
	m_sNetNumber = _T("");
	m_sNodeNumber = _T("");
	m_sIP1 = _T("0");
	m_sIP2 = _T("0");
	m_sIP3 = _T("0");
	m_sIP4 = _T("0");
	//}}AFX_DATA_INIT
    
    // Need to clear this guy
    ZeroMemory(&m_SockAddr, sizeof(SOCKADDR));
}


void CSNMPEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSNMPEditDlg)
	DDX_Control(pDX, IDC_NODENUMBER_EDIT, m_NodeEdit);
	DDX_Control(pDX, IDC_NETNUMBER_EDIT, m_NetEdit);
	DDX_CBIndex(pDX, IDC_SNMP_PROTOCOLS, m_iProtocol);
	DDX_Text(pDX, IDC_TARGET_NAME, m_sTargetName);
	DDV_MaxChars(pDX, m_sTargetName, 255);          
	DDX_Text(pDX, IDC_NETNUMBER_EDIT, m_sNetNumber);
	DDV_MaxChars(pDX, m_sNetNumber, 8);
	DDX_Text(pDX, IDC_NODENUMBER_EDIT, m_sNodeNumber);
	DDV_MaxChars(pDX, m_sNodeNumber, 12);
	DDX_Text(pDX, IDC_IP_EDIT1, m_sIP1);
	DDV_MaxChars(pDX, m_sIP1, 3);
	DDX_Text(pDX, IDC_IP_EDIT2, m_sIP2);
	DDV_MaxChars(pDX, m_sIP2, 3);
	DDX_Text(pDX, IDC_IP_EDIT3, m_sIP3);
	DDV_MaxChars(pDX, m_sIP3, 3);
	DDX_Text(pDX, IDC_IP_EDIT4, m_sIP4);
	DDV_MaxChars(pDX, m_sIP4, 3);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSNMPEditDlg, CDialog)
	//{{AFX_MSG_MAP(CSNMPEditDlg)
	ON_CBN_SELCHANGE(IDC_SNMP_PROTOCOLS, OnSelchangeSnmpProtocols)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSNMPEditDlg message handlers

BOOL CSNMPEditDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // Set correct protocol
    switch(m_SockAddr.sa_family)
        {
        case AF_INET:
            m_iProtocol = 0;
            break;
        case AF_IPX:
            m_iProtocol = 1;
            break;
        default:
            m_iProtocol = 0;
            break;
        }

    // Set up text in edit boxes.
    if( m_iProtocol == 0 )  // Internet  
        {
        PSOCKADDR_IN p = (PSOCKADDR_IN)&m_SockAddr;
        CString s;

        // Set values
        m_sIP1.Format(_T("%d"), p->sin_addr.S_un.S_un_b.s_b1 );
        m_sIP2.Format(_T("%d"), p->sin_addr.S_un.S_un_b.s_b2 );
        m_sIP3.Format(_T("%d"), p->sin_addr.S_un.S_un_b.s_b3 );
        m_sIP4.Format(_T("%d"), p->sin_addr.S_un.S_un_b.s_b4 );
        }
    else
        { // IPX
        PSOCKADDR_IPX p = (PSOCKADDR_IPX)&m_SockAddr;
        CString sFormat;
        
        for(int i = 0; i < 4; i++)
            {
            sFormat.Format(_T("%02X"), (BYTE)p->sa_netnum[i]);
            m_sNetNumber += sFormat;
            }
  
        for( i = 0; i < 6; i++)                    
            {
            sFormat.Format(_T("%02X"), (BYTE)p->sa_nodenum[i]);
            m_sNodeNumber += sFormat;
            }
        }

    // Update states for controls.
    UpdateData(FALSE);
    OnSelchangeSnmpProtocols();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSNMPEditDlg::OnOK() 
{
    // Gather data from controls.
    UpdateData();

    // Make sure the user entered a name.
    if(m_sTargetName.GetLength() == 0)
        {
        CString sTitle, sError;
        sTitle.LoadString(IDS_ERROR_TITLE);
        sError.LoadString(IDS_ERROR_NO_NAME);
        MessageBox(sError, sTitle, MB_OK | MB_ICONEXCLAMATION);
        
        // Set focus 
        GetDlgItem(IDC_TARGET_NAME)->SetFocus();
        return;
        }

    // Set correct protocol
    switch(m_iProtocol)
        {
        case 0:
            m_SockAddr.sa_family = AF_INET;
            break;
        case 1:
            m_SockAddr.sa_family = AF_IPX;
            break;
        default:
            m_SockAddr.sa_family = AF_UNSPEC;            
        }


    // Need to convert edit control to address
    CString s;    
    if( m_iProtocol == 0 )  // Internet  
        {
        PSOCKADDR_IN p = (PSOCKADDR_IN)&m_SockAddr;
        int iValue; 
        

        // Make sure all values are in range.
        for(int i= IDC_IP_EDIT1; i <= IDC_IP_EDIT4; i++)
            {
            GetDlgItem(i)->GetWindowText(s);
            iValue = atoi(s);
            if( iValue > 255 || iValue < 0 )
                {
                // Tell the user that something bad happened
                CString sTitle, sError;
                sTitle.LoadString(IDS_ERROR_TITLE);
                sError.LoadString(IDS_IP_OUT_OF_RANGE);
                MessageBox(sError, sTitle, MB_OK | MB_ICONEXCLAMATION);
                
                // Set focus 
                GetDlgItem(i)->SetFocus();
                ((CEdit*)GetDlgItem(i))->SetSel(0, -1);
                return;
                }

            }

        // Save off values
        GetDlgItem(IDC_IP_EDIT1)->GetWindowText(s);
        p->sin_addr.S_un.S_un_b.s_b1 = atoi(s);
        GetDlgItem(IDC_IP_EDIT2)->GetWindowText(s);
        p->sin_addr.S_un.S_un_b.s_b2 = atoi(s); 
        GetDlgItem(IDC_IP_EDIT3)->GetWindowText(s);
        p->sin_addr.S_un.S_un_b.s_b3 = atoi(s);
        GetDlgItem(IDC_IP_EDIT4)->GetWindowText(s);
        p->sin_addr.S_un.S_un_b.s_b4 = atoi(s);
        }
    else
        { // IPX
        PSOCKADDR_IPX p = (PSOCKADDR_IPX)&m_SockAddr;
        int iLen;
        char szBuffer[16];
        char szByteBuffer[3];
        DWORD dwValue;
        FillMemory(szBuffer, 16 * sizeof(char), '0');
        ZeroMemory(szByteBuffer, 3);

        //IPX strings need to be zero-padded on the right
        GetDlgItem(IDC_NETNUMBER_EDIT)->GetWindowText(s);
        iLen = s.GetLength();
        CopyMemory(&szBuffer[8 - iLen], (LPBYTE)s.GetBuffer(0), iLen);
        
        for(int i = 0; i < 4; i++)
            {
            szByteBuffer[0] = szBuffer[ i * 2];
            szByteBuffer[1] = szBuffer[ i * 2 + 1];
            _ConvertStringToLong(&dwValue, szByteBuffer, 16, TRUE);
            p->sa_netnum[i] = (char) dwValue;
            }
        
        // Do the same thing for the node number
        FillMemory(szBuffer, 16 * sizeof(char), '0');        
        GetDlgItem(IDC_NODENUMBER_EDIT)->GetWindowText(s);
        iLen = s.GetLength();
        CopyMemory(&szBuffer[12 - iLen], (LPBYTE)s.GetBuffer(0), iLen);
        
        for(i = 0; i < 6; i++)
            {
            szByteBuffer[0] = szBuffer[ i * 2];
            szByteBuffer[1] = szBuffer[ i * 2 + 1];
            _ConvertStringToLong(&dwValue, szByteBuffer, 16, TRUE);
            p->sa_nodenum[i] = (char) dwValue;
            }
        }
	
	CDialog::OnOK();
}

void CSNMPEditDlg::OnSelchangeSnmpProtocols() 
{
	// Get Data
    UpdateData();
    
    int i;
    CString s;

    // Need to deal with IPX related controls.
    for(i = IDC_NODENUMBER; i <= IDC_NODENUMBER_EDIT; i++)
        GetDlgItem(i)->ShowWindow( m_iProtocol == 0 ? SW_HIDE : SW_SHOW );
    
    // Need to Show IP related controls.
    for(i = IDC_IP_EDIT1; i <= IDC_IP_EDIT4; i++)
        GetDlgItem(i)->ShowWindow( m_iProtocol == 1 ? SW_HIDE : SW_SHOW);

    // Set label text.
    s.LoadString(m_iProtocol == 0 ? IDS_IP_ADDRESS : IDS_NETWORK_NUMBER);
    GetDlgItem(IDC_ADDRESS)->SetWindowText(s);

    // If IPX protcol, need to move controls
    if( m_iProtocol == 1 )
        {
        CRect rect;
        CRect rect2;
                              
        // Get position of first IP edit window.
        GetDlgItem(IDC_IP_EDIT1)->GetWindowRect(&rect);
        ScreenToClient(&rect);

        GetDlgItem(IDC_NETNUMBER_EDIT)->GetWindowRect(&rect2);
        GetDlgItem(IDC_NETNUMBER_EDIT)->MoveWindow( rect.left, 
                                                    rect.top, 
                                                    rect2.Width(),
                                                    rect2.Height());

        // Get position of third IP edit window.
        GetDlgItem(IDC_IP_EDIT4)->GetWindowRect(&rect);
        ScreenToClient(&rect);

        GetDlgItem(IDC_NODENUMBER_EDIT)->GetWindowRect(&rect2);
        GetDlgItem(IDC_NODENUMBER_EDIT)->MoveWindow( rect.left, 
                                                    rect.top, 
                                                    rect2.Width(),
                                                    rect2.Height());

        }
}
/////////////////////////////////////////////////////////////////////////////
// CAddressEdit

CAddressEdit::CAddressEdit()
{
    // Do not allow non-numeric characters
    m_bAllowHex = FALSE;
}

CAddressEdit::~CAddressEdit()
{
}


BEGIN_MESSAGE_MAP(CAddressEdit, CEdit)
	//{{AFX_MSG_MAP(CAddressEdit)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddressEdit message handlers


void CAddressEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
    // Let other characters go by
    if( (isxdigit(nChar) &&
        !ispunct(nChar) &&
        !isspace(nChar)) || iscntrl(nChar))
        {
    	// Call default handler
        CEdit::OnChar(nChar, nRepCnt, nFlags);
        }
    else
        {
        // Character not in range.  Bail.
        MessageBeep(0);
        }
	
}
