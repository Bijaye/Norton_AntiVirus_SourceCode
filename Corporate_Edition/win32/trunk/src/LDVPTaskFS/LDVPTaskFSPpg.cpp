// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: LDVPTaskFSPpg.cpp
//  Purpose: Implementation of the CLDVPTaskFSPropPage property page class.
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
#include "LDVPTaskFS.h"
#include "LDVPTaskFSPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CLDVPTaskFSPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CLDVPTaskFSPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CLDVPTaskFSPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CLDVPTaskFSPropPage, "LDVPTASKFS.LDVPTaskFSPropPage.1",
	0xba0c6365, 0x7218, 0x11d0, 0x88, 0x65, 0x44, 0x45, 0x53, 0x54, 0, 0)


//----------------------------------------------------------------
// UpdateRegistry
//----------------------------------------------------------------
BOOL CLDVPTaskFSPropPage::CLDVPTaskFSPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_LDVPTASKFS_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CLDVPTaskFSPropPage::CLDVPTaskFSPropPage() :
	COlePropertyPage(IDD, IDS_LDVPTASKFS_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CLDVPTaskFSPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}

//----------------------------------------------------------------
// DoDataExchange
//----------------------------------------------------------------
void CLDVPTaskFSPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CLDVPTaskFSPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CLDVPTaskFSPropPage message handlers

/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/
