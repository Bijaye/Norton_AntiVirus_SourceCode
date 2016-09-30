// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// GetUploadFiles.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "GetUploadFiles.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetUploadFiles dialog


CGetUploadFiles::CGetUploadFiles(CWnd* pParent /*=NULL*/)
	: CDialog(CGetUploadFiles::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetUploadFiles)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CGetUploadFiles::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetUploadFiles)
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_LIST, m_List);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetUploadFiles, CDialog)
	//{{AFX_MSG_MAP(CGetUploadFiles)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DEL, OnDel)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetUploadFiles message handlers

void CGetUploadFiles::OnOK() 
{
	CString str2;
	CString str;
	CString path;
	try
	{
		CDebugUpload *MyMessage = new CDebugUpload(this);
		
		CSendToIntel send;

		m_OK.EnableWindow(FALSE);

		send.Debug((IMessages *)&MyMessage);
		if (send.DoModal() != ERROR_SUCCESS) {
			EndDialog(ERROR_GENERAL);
			return;
			}
		send.Ftp->Connect();

		int act = 0;
		int count = m_List.GetItemCount();

		for (int i=0;i<count;i++) {
			CString *item = (CString *)m_List.GetItemData(i);
			if (!item->IsEmpty()) {
				m_List.SetItemState(i,LVIS_SELECTED|LVIS_FOCUSED,LVIS_SELECTED|LVIS_FOCUSED);
				m_List.EnsureVisible(i,FALSE);
				UpdateWindow(); 
				wThreadSwitch();
				send.AttachData((char *)(LPCSTR)path);
				send.Ftp->SendUnique((char *)(LPCSTR)item,FTP_TYPE_I);
				}
			}

		send.Ftp->Disconnect();
		}
	catch(std::bad_alloc &){}
}

void CGetUploadFiles::OnAdd() 
{
	CFileDialog dlg(TRUE);
	CString *name = NULL;
	char *Name = NULL;
	char *path = NULL;
	
	if(IDOK == dlg.DoModal()) 
	{
		*name = dlg.GetFileName();

        path = (char *)name->LockBuffer();
 
		int i = name->ReverseFind('\\');
		if (i != -1)
			*Name = 0;
		else
			Name = path + i;

		i = m_List.InsertItem(0,Name+1);

		if (i != -1) {
			m_List.SetItemText(i,1,path);
			*Name = '\\';
			name->UnlockBuffer();
			m_List.SetItemData(i,(unsigned long)name);
		}
	}
}

void CGetUploadFiles::OnDel() 
{
	int i = m_List.GetNextItem(0,LVIS_SELECTED);
	CString *item = (CString *)m_List.GetItemData(i);
	m_List.DeleteItem(i);
	delete item;	
}

BOOL CGetUploadFiles::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_List.InsertColumn(0,"File Name",LVCFMT_LEFT,150,0 );
	m_List.InsertColumn(1,"Loaction",LVCFMT_LEFT,150,1 );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CGetUploadFiles::OnCancel() 
{
	CDialog::OnCancel();
}

void CGetUploadFiles::OnDestroy() 
{
	CDialog::OnDestroy();

	while (m_List.GetItemCount()) {
		CString *item = (CString *)m_List.GetItemData(0);
		m_List.DeleteItem(0);
		delete item;
		}
}
