// DFGuiDoc.cpp : implementation of the CDFGuiDoc class
//

#include "stdafx.h"
#include "DFGui.h"
#include "dfpackthread.h"
#include "DFGuiDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDFGuiDoc

IMPLEMENT_DYNCREATE(CDFGuiDoc, CDocument)

BEGIN_MESSAGE_MAP(CDFGuiDoc, CDocument)
	//{{AFX_MSG_MAP(CDFGuiDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFGuiDoc construction/destruction

CDFGuiDoc::CDFGuiDoc()
{
	// TODO: add one-time construction code here

}

CDFGuiDoc::~CDFGuiDoc()
{
	;
}

BOOL CDFGuiDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDFGuiDoc serialization

void CDFGuiDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDFGuiDoc diagnostics

#ifdef _DEBUG
void CDFGuiDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDFGuiDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDFGuiDoc commands
