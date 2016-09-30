// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// FindDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SavCustom.h"
#include "FindDlg.h"
#include "CRegent.h"

#include "TransmanClass.h"

#include <afxpriv.h>

extern LPTM g_pTrans;

CStringArray g_saList;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog


CFindDlg::CFindDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindDlg)
	m_strComputer = _T("");
	m_iRadioName = -1;
	m_bIsValidServer = FALSE;
	//}}AFX_DATA_INIT
}


void CFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindDlg)
	DDX_Control(pDX, IDC_COMPUTERTYPE, m_ctlComputerType);
	DDX_Control(pDX, IDC_COMPUTER, m_ctlComputer);
	DDX_CBString(pDX, IDC_COMPUTER, m_strComputer);
	DDX_Radio(pDX, IDC_RADIONAME, m_iRadioName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindDlg, CDialog)
	//{{AFX_MSG_MAP(CFindDlg)
	ON_BN_CLICKED(IDC_RADIOADDRESS, OnRadioaddress)
	ON_BN_CLICKED(IDC_RADIONAME, OnRadioname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindDlg message handlers

BOOL FindRemoteServer(CString &strComputer, int iType)
{
	DWORD rv;
	BOOL bFound = FALSE;
	SERVERLOCATION sl;

	AfxGetApp()->DoWaitCursor(1);

	strComputer.MakeUpper();

	if (iType == I_SEARCH_NTSERVER || iType == I_SEARCH_NETWARESERVER || iType == I_SEARCH_NTORNETWARESERVER ||
			iType == I_SEARCH_IPADDRESS || iType == I_SEARCH_IPXADDRESS)
	{
		strncpy(sl.FindServer, strComputer, NAME_SIZE);

		switch(iType)
		{
		case I_SEARCH_NETWARESERVER:
			sl.Flags = SL_NAME_IS_NETWARE;
			break;

		case I_SEARCH_NTSERVER:
			sl.Flags = SL_NAME_IS_NT;
			break;

		case I_SEARCH_NTORNETWARESERVER:
			// This is the only case where I will do all the search work here.
			// Look for NetWare
			sl.Flags = SL_NAME_IS_NETWARE;
			rv = g_pTrans->LocateServer(&sl);

			if (rv != ERROR_SUCCESS)
			{
				// If it didn't find it, look for NT
				sl.Flags = SL_NAME_IS_NT;
				rv = g_pTrans->LocateServer(&sl);
			}
			break;

		case I_SEARCH_IPADDRESS:
			sl.Flags = SL_NAME_IS_IP;
			break;

		case I_SEARCH_IPXADDRESS:
			sl.Flags = SL_NAME_IS_IPX;
			break;
			
		}

		if (iType != I_SEARCH_NTORNETWARESERVER)
			rv = g_pTrans->LocateServer(&sl);

	}
	if (!rv)
	{
		bFound = TRUE;
		strComputer = sl.ReportedName;
	}

	AfxGetApp()->DoWaitCursor(-1);

	return bFound;
}


BOOL CFindDlg::OnInitDialog() 
{
	m_iRadioName = 0;

	CDialog::OnInitDialog();
	
	OnRadioname();

	m_ctlComputer.SetFocus();
	m_ctlComputer.SetCurSel(0);
	m_ctlComputerType.SetCurSel(0);

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFindDlg::OnOK() 
{
	CString	sText;
	m_bFound = FALSE;
	UpdateData(TRUE);

	int iMachineType = m_ctlComputerType.GetCurSel();

	if (m_strComputer == "")
	{
		sText.LoadString(IDS_INVALIDFINDSERVERNAME);
		MessageBox( sText, (LPCTSTR)"Setup", MB_OK | MB_ICONSTOP | MB_TASKMODAL );
		m_ctlComputer.SetFocus();
		return;
	}

	m_sFoundComputer = m_strComputer;
	if (m_iRadioName == 0)
		m_bFound = FindRemoteServer(m_sFoundComputer, iMachineType);
	else
		m_bFound = FindRemoteServer(m_sFoundComputer, iMachineType + I_ADDRESSOFFSET);

	m_bIsValidServer = m_bFound;
	SaveServerInfo();

	if (!m_bFound)
	{
		sText.Format(IDS_CANTFINDSERVER, m_strComputer);
		MessageBox( sText, (LPCTSTR)"Setup", MB_OK | MB_ICONSTOP | MB_TASKMODAL );
		return;
	}
	else if (!m_iRadioName)
		AddStringOnce("1" + m_strComputer);
	else
		AddStringOnce("2" + m_strComputer);
	
	CDialog::OnOK();
}

void CFindDlg::OnRadioaddress() 
{
	CString	str;

	// Fill computer list with computer addresses
	FillComboList(1);

	m_ctlComputerType.ResetContent();
	str.LoadString(IDS_SEARCH_IPADDRESS);
	m_ctlComputerType.AddString(str);
	m_ctlComputerType.SetCurSel(0);	
}

void CFindDlg::OnRadioname() 
{
	CString	str;

	// Fill computer list with computer names
	FillComboList(1);

	m_ctlComputerType.ResetContent();
	str.LoadString(IDS_SEARCH_SERVEREITHER);
	m_ctlComputerType.AddString(str);
	str.LoadString(IDS_SEARCH_SERVERNT);
	m_ctlComputerType.AddString(str);
	str.LoadString(IDS_SEARCH_SERVERNETWARE);
	m_ctlComputerType.AddString(str);
	m_ctlComputerType.SetCurSel(0);	
}

void CFindDlg::AddStringOnce(CString sNewString)
{
	for (int index = 0; index < g_saList.GetSize(); index++)
	{
		if (!sNewString.CompareNoCase(g_saList.GetAt(index)))
			return;
	}
	g_saList.Add(sNewString);
}

void CFindDlg::FillComboList(int nType)
{
	CString sName, sText;
	
	m_ctlComputer.GetWindowText(sText);

	m_ctlComputer.ResetContent();
	for (int index = 0; index < g_saList.GetSize(); index++)
	{
		sName = g_saList.GetAt(index);
		if (atoi(sName.Left(1)) == nType)
			m_ctlComputer.AddString(sName.Mid(1));
	}
	m_ctlComputer.SetWindowText(sText);
}

CString CFindDlg::GetReportedName()
{
	if (m_bFound)
		return m_sFoundComputer;
	else
		return "";
}

void CFindDlg::SaveServerInfo()
{
	CRegistryEntry* cReg;
	DWORD dwType;
	DWORD dwSize;
	DWORD dwDataBuf;
	int	  nSize;   
	
	try
	{
		cReg = new CRegistryEntry( HKEY_LOCAL_MACHINE, 
								   NAVCORPREG_ROOT, 
								   SERVEREXIST_VALUE );
	}
	catch (std::bad_alloc &) {}

	if ( NULL == cReg )
	{
		return;
	}

	// Write some information to 
	// SOFTWARE\\INTEL\\LANDesk\\VirusProtect6\\CurrentVersion
	if ( ERROR_SUCCESS == cReg->OpenKey ( KEY_WRITE ) )
	{
		dwType = REG_DWORD;
		dwDataBuf = (DWORD) m_bIsValidServer;
		dwSize = sizeof ( dwDataBuf );

		// Set to value-pair "ServerExists" and write the value
		if ( ERROR_SUCCESS == cReg->SetValue ( dwType, 
											   (LPBYTE)&dwDataBuf,
											   dwSize ) )
		{
			// Change to "ParentServer" 
			cReg->SetValueName ( SERVERNAME_VALUE );

			// Convert CString to null terminated string
			nSize = m_sFoundComputer.GetLength()+1;
			LPTSTR lpszStrBuf = NULL;
			try
			{
				lpszStrBuf = new TCHAR[nSize];
			}
			catch(std::bad_alloc &)
			{
				return;
			}
			_tcscpy ( lpszStrBuf, m_sFoundComputer );

			// Write the value
			dwType = REG_SZ;
			dwSize = (DWORD) nSize;
			cReg->SetValue ( dwType, (LPBYTE)lpszStrBuf, dwSize );
		}

		cReg->CloseKey();
	}

    delete cReg;
}

