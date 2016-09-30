// Copyright 1992 - 1994 Symantec, Peter Norton Group
// ************************************************************************
//
// $Header:   S:/NAVWINX5/VCS/NAVVIEW.CPv   1.0   01 Jun 1998 16:07:46   DBuches  $
//
// Description:
//
// See Also:
//
// ************************************************************************
// $Log:   S:/NAVWINX5/VCS/NAVVIEW.CPv  $
// 
//    Rev 1.0   01 Jun 1998 16:07:46   DBuches
// Initial revision.
// 
//    Rev 1.1   01 Mar 1998 19:04:08   pagbabi
// Removed do nothing switch statement in Command
// 
//    Rev 1.0   01 Mar 1998 18:51:06   pagbabi
// Initial revision.
// ************************************************************************

#include "stdafx.h"

#include "resource.h"
#include "NavWinX.h"
#include "node.h"
#include "namespace.h"
#include "navnode.h"

#include "nodeview.h"
#include "navview.h"

#include "dataobj.h"

// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns:
//
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

CNavWinView::CNavWinView()
{
    AFX_MANAGE_STATE( AfxGetStaticModuleState() );

}

// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns:
//
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

STDMETHODIMP CNavWinView::AddMenuItems(
	LPDATAOBJECT pDataObject,
	LPCONTEXTMENUCALLBACK pCallback,
    long* pInsertionAllowed)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;

    CNode* pNode = GetNode();
    ASSERT( pNode );
    CLSID clsid = pNode->GetCoClassID();

	return hr;
}

// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns:
//
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

STDMETHODIMP CNavWinView::Command( long lCommandID, LPDATAOBJECT pDataObject )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

    //
    // these commands work on individual machine recorditems
    //

    HRESULT hr = E_FAIL;
    CNavWinNode* pNode = GetNode();
    ASSERT( pNode );

    // put command handlers here

    hr = S_OK;

	return hr;
}

// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns:
//
// Remarks:     Pure Override
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

STDMETHODIMP CNavWinView::DoDescription( LPDATAOBJECT pDataObject )
{
    HRESULT hr = S_OK;

    long lView = 0;
    hr = m_pResult->GetViewMode( &lView );

    // don't do bitmaps if in OCX view
    if( FAILED(hr) )
        return hr;

    return hr;
}

