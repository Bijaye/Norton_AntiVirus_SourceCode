/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/itempropsheet.cpv   1.3   20 May 1998 17:13:38   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// ItemPropSheet.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/itempropsheet.cpv  $
// 
//    Rev 1.3   20 May 1998 17:13:38   DBuches
// 1st pass at help.
// 
//    Rev 1.2   13 Mar 1998 15:20:14   DBuches
// Don't show virus tab if we have no virus.
// 
//    Rev 1.1   06 Mar 1998 11:31:08   DBuches
// Added DisplayProperties.
// 
//    Rev 1.0   27 Feb 1998 15:08:58   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////




#include "stdafx.h"
#include "qconsole.h"
#include "mainfrm.h"
#include "ItemPropSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CItemPropSheet

IMPLEMENT_DYNAMIC(CItemPropSheet, CPropertySheet)

CItemPropSheet::CItemPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
    AddPages();
}

CItemPropSheet::CItemPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
    AddPages();
}

CItemPropSheet::~CItemPropSheet()
{
}


BEGIN_MESSAGE_MAP(CItemPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CItemPropSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CItemPropSheet message handlers



////////////////////////////////////////////////////////////////////////////////
//
// Function name	: CItemPropSheet::AddPages
// Description	    : Add pages to sheet.
// Return type		: void 
//
////////////////////////////////////////////////////////////////////////////////
// 2/16/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
void CItemPropSheet::AddPages()
{
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CItemPropSheet::DisplayProperties
//
// Description	    : Displays the properties for a given item
//
// Return type		: int 
//
// Argument         : IQuarantineItem * pItem - item to display
// Argument         : CImageList* pImageList - pointer to system image list
// Argument         : int iImage - index of image
//
////////////////////////////////////////////////////////////////////////////
// 3/3/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int CItemPropSheet::DisplayProperties( IQuarantineItem * pItem, 
                                       CImageList* pImageList, int iImage )
{
    //
    // Genereral page
    //
    m_GenPage.SetItemPointer( pItem );
    m_GenPage.m_ImageButton.SetImageList( pImageList );
    m_GenPage.m_ImageButton.SetImageIndex( iImage );
    AddPage( &m_GenPage );

    //
    // Client info page
    //
    m_ServerPage.SetItemPointer( pItem );
    m_ServerPage.m_ImageButton.SetImageList( pImageList );
    m_ServerPage.m_ImageButton.SetImageIndex( iImage );
    // bugbug - need to make sure we need to add this
    AddPage( &m_ServerPage );


    //
    // Virus info page
    //

    // Check to make sure we should add this page.  
    // If there is no virus entry for this Virus ID,
    // don't add the page

    // Get the virus index
    N30* pVirInfo;
    WORD wIndex = 0;
    pItem->GetN30StructPointer( &pVirInfo );
    ASSERT( pVirInfo );
    CQconsoleDoc* pDoc = ( CQconsoleDoc* )((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();

    if( VLDetermineVirusIndex( pDoc->GetAVAPIContext(), 
                               pVirInfo->wVirID, //0x801
                               &wIndex ) == VS_OK && lstrlen( pVirInfo->lpVName) )
        {
        m_VirusPage.SetItemPointer( pItem );
        m_VirusPage.SetVirusIndex( wIndex );
        AddPage( &m_VirusPage );
        }

    // Fire off the dialog.
    return DoModal();
}


////////////////////////////////////////////////////////////////////////////
// Function name	: CItemPropSheet::OnInitDialog
//
// Description	    : Handles WM_INITDIALOG message.
//
// Return type		: BOOL 
//
//
////////////////////////////////////////////////////////////////////////////
// 5/20/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CItemPropSheet::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();
	
	// Hide the OK button and change the text of the cancel
    // button to Close.

    GetDlgItem( IDOK )->ShowWindow( SW_HIDE );

    CString s( (LPCTSTR) IDS_CLOSE );
    GetDlgItem( IDCANCEL )->SetWindowText( s );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
