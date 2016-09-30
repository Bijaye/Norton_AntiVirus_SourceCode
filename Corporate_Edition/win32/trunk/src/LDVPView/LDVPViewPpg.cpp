// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: LDVPViewPpg.cpp
//  Purpose: Implementation of the CLDVPViewPropPage property page class.
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "LDVPView.h"
#include "LDVPViewPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CLDVPViewPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CLDVPViewPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CLDVPViewPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CLDVPViewPropPage, "LDVPView.LDVPViewPropPage.1",
	0xba0c6365, 0x7218, 0x11d0, 0x88, 0x65, 0x44, 0x45, 0x53, 0x54, 0, 0)


//----------------------------------------------------------------
// UpdateRegistry
//----------------------------------------------------------------
BOOL CLDVPViewPropPage::CLDVPViewPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_LDVPVIEW_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CLDVPViewPropPage::CLDVPViewPropPage() :
	COlePropertyPage(IDD, IDS_LDVPVIEW_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CLDVPViewPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}

//----------------------------------------------------------------
// DoDataExchange
//----------------------------------------------------------------
void CLDVPViewPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CLDVPViewPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CLDVPViewPropPage message handlers

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
