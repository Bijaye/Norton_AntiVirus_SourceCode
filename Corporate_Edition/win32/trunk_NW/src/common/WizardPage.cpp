// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------
//
//  File: WizardPage.cpp
//  Purpose: CWizardPage Implementation file. 
//				Base class for Task wizard pages
//
//	Date: 1-20-97
//
//	Owner: Ken Knapton
//
//	***** Intel Corp. Confidential ******
//
//----------------------------------------------------------------

#include "stdafx.h"
//#include "LDVPTaskFS.h"
#include "WizardPage.h"
#include "Wizard.h"
#include "ModalConfig.h"
#include "LDVPActions.h"
#include "LDVPExtensions.h"
//#include "shellSelDlg.h"
#include "ConfigObj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CWizardPage, CPropertyPage)

BEGIN_MESSAGE_MAP(CWizardPage, CPropertyPage)
	//{{AFX_MSG_MAP(CWizardPage)
	//}}AFX_MSG_MAP
	ON_COMMAND( UWM_DESTROYING, OnDestroying )
END_MESSAGE_MAP()

//----------------------------------------------------------------
// Constructor
//----------------------------------------------------------------
CWizardPage::CWizardPage() 
{
}

//----------------------------------------------------------------
// Constructor - overloaded
//----------------------------------------------------------------
CWizardPage::CWizardPage( int iID) : CPropertyPage( iID )
{
	//{{AFX_DATA_INIT(CWizardPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

//----------------------------------------------------------------
// Destructor
//----------------------------------------------------------------
CWizardPage::~CWizardPage()
{
}

//----------------------------------------------------------------
// DoDataExchange
//----------------------------------------------------------------
void CWizardPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWizardPage)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

//----------------------------------------------------------------
// OnSetActive
//----------------------------------------------------------------
BOOL CWizardPage::OnSetActive()
{
	CPropertyPage::OnSetActive();

	//Set the font for the title
	LOGFONT	lf;
	CFont	*pFont;
	CWnd	*pWnd;

	if( !m_titleFont.m_hObject )
	{
		pFont = GetFont();
		pFont->GetLogFont( &lf );
		//lf.lfWeight = 700;
		lf.lfHeight += 35;
		m_titleFont.CreateFontIndirect( &lf );

		//Set the font into the text control
		if( pWnd = GetDlgItem( IDC_TITLE ) )
			pWnd->SetFont( &m_titleFont );
	}

	return TRUE;
}

IVPStorage *CWizardPage::GetStorage()
{
	IVPStorage2 *iStorage = NULL;

	//Get the storage
	if( SUCCEEDED( CoCreateLDVPObject( CLSID_CliProxy, IID_IVPStorage2, (void**)&iStorage ) ) )
	{
		iStorage->InitRootForScan( NULL, NULL );
	}

	return iStorage;
}

void CWizardPage::SizePage( int cx, int cy )
{
	//First, size the dialog to give me some working room
	MoveWindow( 14, 14, cx-28, cy-86 );
}

//----------------------------------------------------------------
// OnDestroying
//----------------------------------------------------------------
void CWizardPage::OnDestroying() 
{
}


//----------------------------------------------------------------
// PaintTitle
//----------------------------------------------------------------
void CWizardPage::PaintTitle( CDC* pDC )
{
	CString strItem = ((CClientPropSheet*)GetParent())->m_strTitle,
			strDescription = ((CClientPropSheet*)GetParent())->m_strDescription,
			strLeft,
			strRight;

	//I don't want any underscores in the Taskpad
	//	so remove them all now
	RemoveChars( strItem, _T('&') );

	//Draw the title
	CWnd *ptrWnd = GetDlgItem( IDC_TITLE );
	
	if( ptrWnd )
	{
		ptrWnd->SetWindowText( strItem );
	}
	
	//Draw the description
	ptrWnd = GetDlgItem( IDC_DESCRIPTION );
	
	if( ptrWnd )
	{
		ptrWnd->SetWindowText( strDescription );
	}
}


void CWizardPage::OnMscButton()
{
	//This should be handled by each individual page.
	// The default here is to do nothing.
}


/*------------------------------------------------------------------
*
* $History: $
*
*
  ------------------------------------------------------------------*/


