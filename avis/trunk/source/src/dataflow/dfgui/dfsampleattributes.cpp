// DFSampleAttributes.cpp : implementation file
//

#include "stdafx.h"
#include "DFGui.h"
#include "dferror.h"
#include "DFSampleAttributes.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// DFSampleAttributes dialog


DFSampleAttributes::DFSampleAttributes(CWnd* pParent /*=NULL*/)
	: CDialog(DFSampleAttributes::IDD, pParent)
{
	//{{AFX_DATA_INIT(DFSampleAttributes)
	m_strAttrText = _T("");
	m_strCookieId = _T("");
	//}}AFX_DATA_INIT
}


void DFSampleAttributes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(DFSampleAttributes)
	DDX_Text(pDX, IDC_EDIT1, m_strAttrText);
	DDV_MaxChars(pDX, m_strAttrText, 100000);
	DDX_Text(pDX, IDC_SAMPLEID, m_strCookieId);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(DFSampleAttributes, CDialog)
	//{{AFX_MSG_MAP(DFSampleAttributes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// DFSampleAttributes message handlers

BOOL DFSampleAttributes::SetFileText(char *fileName, CString id)
{
   char path[MAX_LENGTH];
   strcpy(path, fileName);
   char text[501];

   unsigned int nCount;

 
  
   int hFile;

   hFile = _lopen(path,OF_READ);

   if(hFile == HFILE_ERROR) return FALSE;
   
   m_strAttrText = "";
   m_strCookieId = CString("IssueTrackingNumber = ") + CString(id);
   while ((nCount = _lread(hFile,text,500)) > 0)
   {
	   text[nCount] = 0;
	   m_strAttrText += text;
   }
   int n = m_strAttrText.GetLength();
   _lclose(hFile);
   return TRUE;
}



BOOL DFSampleAttributes::OnInitDialog() 
{
	CDialog::OnInitDialog();
	// if (!SetFileText())
	//	return FALSE;
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
