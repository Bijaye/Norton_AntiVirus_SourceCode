// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// SendToIntel.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "resultsview.h"
#include "getstatus.h"
#include "SendToIntel.h"
#include "time.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSendToIntel dialog


CSendToIntel::CSendToIntel(CWnd* pParent /*=NULL*/)
	: CDialog(CSendToIntel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendToIntel)
	m_Name = _T("");
	m_City = _T("");
	m_Description = _T("");
	m_Email = _T("");
	m_State = _T("");
	m_Zip = _T("");
	m_Address = _T("");
	//}}AFX_DATA_INIT

	Messages = NULL;
	Ftp = NULL;

}


void CSendToIntel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendToIntel)
	DDX_Text(pDX, IDC_NAME, m_Name);
	DDX_Text(pDX, IDC_CITY, m_City);
	DDX_Text(pDX, IDC_DESCRIPTION, m_Description);
	DDX_Text(pDX, IDC_EMAIL_ADDRESS, m_Email);
	DDX_Text(pDX, IDC_STATE, m_State);
	DDX_Text(pDX, IDC_ZIP, m_Zip);
	DDX_Text(pDX, IDC_ADDRESS, m_Address);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendToIntel, CDialog)
	//{{AFX_MSG_MAP(CSendToIntel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendToIntel message handlers

BOOL CSendToIntel::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (CoCreateInstance(CLSID_Transman,NULL,CLSCTX_INPROC_SERVER,IID_IFTP,(void**)&Ftp) != 0)
		return FALSE;


	Ftp->Open("VirusUploadFTPConfig",Messages);
	Ftp->SetSubOption("UserData");

	BYTE *ptr;

	if (Ftp->GetOption("Name",&ptr,128,(BYTE *)"") == ERROR_SUCCESS) {
		m_Name = (char *)ptr;
		CoTaskMemFree(ptr);
		}

	if (Ftp->GetOption("Email",&ptr,128,(BYTE *)"") == ERROR_SUCCESS) {
		m_Email = (char *)ptr;
		CoTaskMemFree(ptr);
		}

	if (Ftp->GetOption("Address",&ptr,128,(BYTE *)"") == ERROR_SUCCESS) {
		m_Address = (char *)ptr;
		CoTaskMemFree(ptr);
		}

	if (Ftp->GetOption("City",&ptr,64,(BYTE *)"") == ERROR_SUCCESS) {
		m_City = (char *)ptr;
		CoTaskMemFree(ptr);
		}

	if (Ftp->GetOption("State",&ptr,16,(BYTE *)"") == ERROR_SUCCESS) {
		m_State = (char *)ptr;
		CoTaskMemFree(ptr);
		}

	if (Ftp->GetOption("Description",&ptr,1024,(BYTE *)"") == ERROR_SUCCESS) {
		m_Description = (char *)ptr;
		CoTaskMemFree(ptr);
		}

	if (Ftp->GetOption("Zip",&ptr,16,(BYTE *)"") == ERROR_SUCCESS) {
		m_Zip = (char *)ptr;
		CoTaskMemFree(ptr);
		}

	DWORD val = STI_EMAIL|STI_INFOOK;

	if (Ftp->GetOption("Flags",&ptr,4,(BYTE *)&val) == ERROR_SUCCESS) {
		m_Flags = *(DWORD *)ptr;
		CoTaskMemFree(ptr);
		}

	UpdateData(FALSE);
	Ftp->SetSubOption(NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSendToIntel::OnOK() 
{
	// write all options to Ftp
	
	UpdateData(TRUE);
	Ftp->SetSubOption("UserData");
	Ftp->SetOption("Name",REG_SZ,(BYTE *)(LPCSTR)m_Name,m_Name.GetLength()+1);
	Ftp->SetOption("Zip",REG_SZ,(BYTE *)(LPCSTR)m_Zip,m_Zip.GetLength()+1);
	Ftp->SetOption("Email",REG_SZ,(BYTE *)(LPCSTR)m_Email,m_Email.GetLength()+1);
	Ftp->SetOption("Address",REG_SZ,(BYTE *)(LPCSTR)m_Address,m_Address.GetLength()+1);
	Ftp->SetOption("City",REG_SZ,(BYTE *)(LPCSTR)m_City,m_City.GetLength()+1);
	Ftp->SetOption("State",REG_SZ,(BYTE *)(LPCSTR)m_State,m_State.GetLength()+1);
	Ftp->SetOption("Description",REG_SZ,(BYTE *)(LPCSTR)m_Description,m_Description.GetLength()+1);
	Ftp->SetOption("Flags",REG_SZ,(BYTE *)(LPCSTR)m_Description,m_Description.GetLength()+1);
	
	// write all options to GetStatus->UserDataUploadBlock.
	strcpy(UserData.Name,(LPCSTR)m_Name);
	strcpy(UserData.Zip,(LPCSTR)m_Zip);
	strcpy(UserData.Email,(LPCSTR)m_Email);
	strcpy(UserData.Address,(LPCSTR)m_Address);
	strcpy(UserData.City,(LPCSTR)m_City);
	strcpy(UserData.State,(LPCSTR)m_State);
	strcpy(UserData.Description,(LPCSTR)m_Description);

	UserData.Flags = m_Flags;
	UserData.Time = time(NULL);

	Ftp->SetSubOption(NULL);
	EndDialog(ERROR_SUCCESS);
}

CSendToIntel::~CSendToIntel()
{

	if (Messages)
		Messages->Release();

	if (Ftp)
		Ftp->Release();
}

DWORD CSendToIntel::Debug(IMessages * mess)
{
	Messages = mess;

	if (mess)
		mess->AddRef();
	return 0;
}

DWORD CSendToIntel::AttachData(char * FileName)
{

	int han;

	han = open(FileName,O_WRONLY|O_BINARY|O_APPEND,0);
	if (han != -1) {
		write(han,(const void *)&UserData,sizeof(UserData));
		close(han);
		}

	return 0;

}
