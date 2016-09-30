////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// apwUtilStressDoc.cpp : implementation of the CapwUtilStressDoc class
//

#include "stdafx.h"
#include "apwUtilStress.h"

#include "apwUtilStressDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CapwUtilStressDoc

IMPLEMENT_DYNCREATE(CapwUtilStressDoc, CDocument)

BEGIN_MESSAGE_MAP(CapwUtilStressDoc, CDocument)
END_MESSAGE_MAP()


// CapwUtilStressDoc construction/destruction

CapwUtilStressDoc::CapwUtilStressDoc()
{
	// TODO: add one-time construction code here

}

CapwUtilStressDoc::~CapwUtilStressDoc()
{
}

BOOL CapwUtilStressDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CapwUtilStressDoc serialization

void CapwUtilStressDoc::Serialize(CArchive& ar)
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


// CapwUtilStressDoc diagnostics

#ifdef _DEBUG
void CapwUtilStressDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CapwUtilStressDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CapwUtilStressDoc commands
