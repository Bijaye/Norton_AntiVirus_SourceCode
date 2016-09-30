// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// ListViruses.cpp : implementation file
//

#include "stdafx.h"
#include "scandlgs.h"
#include "ListViruses.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IID	IID_IVirusEnum = _IID_IVirusEnum;

/////////////////////////////////////////////////////////////////////////////
// CListViruses dialog


CListViruses::CListViruses(CWnd* pParent /*=NULL*/)
	: CDialog(CListViruses::IDD, pParent)
{
	//{{AFX_DATA_INIT(CListViruses)
	m_strInfo = _T("");
	//}}AFX_DATA_INIT
	m_selectedViruses = _T( "" );
}


void CListViruses::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListViruses)
	DDX_Control(pDX, IDC_INFO, m_ctlInfo);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDHELP, m_btnHelp);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_VIRUS_LIST, m_ctlList);
	DDX_Text(pDX, IDC_INFO, m_strInfo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CListViruses, CDialog)
	//{{AFX_MSG_MAP(CListViruses)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(LVN_DELETEITEM, IDC_VIRUS_LIST, OnDeleteitemVirusList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CListViruses::AddVirusItem( VIRUSNAME* pVirusName )
{
	int iIndex;
	LV_ITEM		lvItem;

	//Add the info to the control
	memset( &lvItem, 0x00, sizeof( lvItem ) );
	lvItem.mask = LVIF_TEXT;
	lvItem.pszText = pVirusName->name;
	lvItem.cchTextMax = pVirusName->namesize;	
	
	//Add the new virus to the list
	if( -1 != ( iIndex = m_ctlList.InsertItem( &lvItem ) ) )
	{
		m_ctlList.SetItemText( iIndex, 1, "Dunno - Colin hasn't told me yet!" );
		m_ctlList.SetItemText( iIndex, 2, "Dunno - Colin hasn't told me yet!" );			
		m_ctlList.SetItemData( iIndex, (LPARAM)pVirusName );
	}

	return iIndex;
}

/*---------------------------------------------------------------*/
// OnInitDialog
/*---------------------------------------------------------------*/
BOOL CListViruses::OnInitDialog() 
{
	CString		strCol;
	int			iIndex,
				iCount;
	IVirusEnum	*pVirusEnum;
	VIRUSNAME	virus,
				*pVirus = NULL;
	CWaitCursor	wait;

	CDialog::OnInitDialog();
	//Start by creating the columns
	//Initialize my control
	strCol.LoadString( IDS_EXCLUSION_COL_HEADING );
	//Start by adding the name column
	m_ctlList.InsertColumn(0, strCol );

	//And the Cleanable column
	strCol.LoadString( IDS_EXCLUSION_COL_CLEANABLE );
	m_ctlList.InsertColumn(1, strCol );

	//Finally, the Virus Type column
	strCol.LoadString( IDS_EXCLUSION_COL_TYPE );
	m_ctlList.InsertColumn(2, strCol );
	
	m_ctlList.SetColumnWidth( 0, 150 );
	m_ctlList.SetColumnWidth( 1, 75 );
	m_ctlList.SetColumnWidth( 2, 75 );

	if ( m_selectedViruses.IsEmpty() )
	{
		//Now, create the COM object and ask for the Detectable Viruses
		if( S_OK == CoCreateInstance( m_clsidProvider, NULL, CLSCTX_INPROC_SERVER, IID_IVirusEnum, (void**)&pVirusEnum ) )
		{
			//Open the enumeration
			pVirusEnum->Open( m_strComputerName.IsEmpty() ? NULL : (LPSTR)(LPCTSTR)m_strComputerName );	
	
			iCount = 1;
			//While I can get another Virus
			while( S_OK == pVirusEnum->GetNext( &virus ) )
			{
				TRACE2("Adding %s (#%d)\n", virus.name, iCount++ );
				//Copy the virus info so I can place it in the lParam
				try
				{
					if( pVirus = new VIRUSNAME )
						memcpy( pVirus, &virus, sizeof( virus) );
				}
				catch (std::bad_alloc &){}
				iIndex = AddVirusItem( pVirus );
				if ( iIndex == -1 )
					delete pVirus;
			}
	
			//Get the viruses out of the interface
			pVirusEnum->Release();
		}
	}
	else
	{
		// we expect the name of selected viruses to be in the m_selectedViruses list
		CString currentName;
		CString cleanable;
		CString type;
		CString selected = m_selectedViruses;

		if ( !selected.IsEmpty() )
		{
			int iSemi = selected.Find( ';' );
			int iComma;

			while( iSemi != -1 )
			{
				currentName = selected.Left( iSemi );
				selected = selected.Mid( iSemi + 1 );
				iComma = currentName.Find( ',' );
				cleanable = currentName.Mid( iComma + 1 );
				currentName = currentName.Left( iComma );
				iComma = cleanable.Find( ',' );
				type = cleanable.Mid( iComma + 1 );
				cleanable = cleanable.Left( iComma );
				try
				{
					pVirus = new VIRUSNAME;
				}
				catch (std::bad_alloc &) {}
				memset( (LPVOID)pVirus, 0, sizeof( VIRUSNAME ) );
				_tcscpy( pVirus->name, currentName );
				pVirus->namesize = currentName.GetLength();
				pVirus->type = _ttol( type );
				pVirus->cleanable = _ttol( cleanable );

				iIndex = AddVirusItem( pVirus );
				if ( iIndex == -1 )
					delete pVirus;
				iSemi = selected.Find( ';' );
			}
			if ( !selected.IsEmpty() )
			{
				try 
				{
					pVirus = new VIRUSNAME;
				}
				catch (std::bad_alloc &) {}
				memset( (LPVOID)pVirus, 0, sizeof( VIRUSNAME ) );
				currentName = selected;
				iComma = currentName.Find( ',' );
				cleanable = currentName.Mid( iComma + 1 );
				currentName = currentName.Left( iComma );
				iComma = cleanable.Find( ',' );
				type = cleanable.Mid( iComma + 1 );
				cleanable = cleanable.Left( iComma );
				_tcscpy( pVirus->name, currentName );
				pVirus->namesize = currentName.GetLength();
				pVirus->type = _ttol( type );
				pVirus->cleanable = _ttol( cleanable );

				iIndex = AddVirusItem( pVirus );
				if ( iIndex == -1 )
					delete pVirus;
			}				
		}
	}

	return TRUE;  
}

/*---------------------------------------------------------------*/
// OnOK
/*---------------------------------------------------------------*/
void CListViruses::OnOK() 
{
	int			iIndex;
	PVIRUSNAME	pVirus;


	//If we don't have a clsID for the provider, place it in the m_selectedViruses string
	if ( !m_selectedViruses.IsEmpty() )
	{
		//Walk the list of selected viruses, and place it in the string
		while( -1 != ( iIndex = m_ctlList.GetNextItem( iIndex, LVNI_ALL | LVNI_SELECTED ) ) )
		{
			CString item;

			pVirus = (PVIRUSNAME)m_ctlList.GetItemData( iIndex );
			item.Format( _T( "%s,%lu, %lu;" ), pVirus->name, pVirus->cleanable, pVirus->type );
			m_selectedViruses = item;
		}
		return;
	}

	//If we don't have a callback routine, then simply return
	if( !m_callback )
		return;

	//Walk the list of selected viruses, and
	//	call the callback for each one
	while( -1 != ( iIndex = m_ctlList.GetNextItem( iIndex, LVNI_ALL | LVNI_SELECTED ) ) )
	{
		pVirus = (PVIRUSNAME)m_ctlList.GetItemData( iIndex );
		(*m_callback)( m_pContext, pVirus );	
	}

	CDialog::OnOK();
}

/*---------------------------------------------------------------*/
// OnSize
/*---------------------------------------------------------------*/
void CListViruses::OnSize(UINT nType, int cx, int cy) 
{
	CRect	controlRect,
			btnRect;

	CDialog::OnSize(nType, cx, cy);

#define POSITION_BUTTON( btn )	btn.GetWindowRect( controlRect ); \
								ScreenToClient( controlRect );		\
								controlRect.left = cx - ( controlRect.right - controlRect.left ) - 10;	\
								controlRect.right = cx - 10;	\
								btn.MoveWindow( controlRect, TRUE )



	if( !::IsWindow( m_btnOK.m_hWnd ) )
		return;

	//Move, don't size, the buttons
	// Set the OK button to 5 pixels away from the right edge
	POSITION_BUTTON( m_btnOK );
	//Move the cancel button to 5 pixels below the OK button
	POSITION_BUTTON( m_btnCancel );
	//Move the help button to 5 pixels below the cancel button
	POSITION_BUTTON( m_btnHelp );

	//Size, don't move, the controls

	//Use the OK button as a reference point for the other
	//	two controls
	m_btnOK.GetWindowRect( btnRect );
	ScreenToClient( btnRect );

	m_ctlInfo.GetWindowRect( controlRect );
	ScreenToClient( controlRect );
	controlRect.right = btnRect.left - 10;
	m_ctlInfo.MoveWindow( controlRect, TRUE  );

	m_ctlList.GetWindowRect( controlRect );
	ScreenToClient( controlRect );
	controlRect.right = btnRect.left - 10;
	controlRect.bottom = cy - 5;
	m_ctlList.MoveWindow( controlRect, TRUE );


}

/*---------------------------------------------------------------*/
// OnGetMinMaxInfo
/*---------------------------------------------------------------*/
void CListViruses::OnGetMinMaxInfo( MINMAXINFO* lpMMI )
{
	lpMMI->ptMinTrackSize.y = (130 * HIWORD( ::GetDialogBaseUnits() ))/8;
	lpMMI->ptMinTrackSize.x = (220 * LOWORD( ::GetDialogBaseUnits() ))/4;
}

/*---------------------------------------------------------------*/
// OnDeleteitemVirusList
/*---------------------------------------------------------------*/
void CListViruses::OnDeleteitemVirusList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	VIRUSNAME *pVirus;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	if( pVirus = (PVIRUSNAME)m_ctlList.GetItemData( pNMListView->iItem ) )
	{
		delete pVirus;
		//Just in case something wierd happens, set the ItemData to NULL
		m_ctlList.SetItemData( pNMListView->iItem, NULL );
	}
	
	*pResult = 0;
}
