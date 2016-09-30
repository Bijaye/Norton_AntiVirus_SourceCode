////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// WNDSIZE.CPP - Created - 09/27/96
/////////////////////////////////////////////////////////////////////
// $Header:   S:/WNDSIZE/VCS/WNDSIZE.CPV   1.6.1.0   22 Jun 1998 19:43:04   RFULLER  $
//
// Description:
//    This class was implemented in order to attempt to make resizing
//    of windows a little bit easier of a chore.  The idea of this
//    class was that you would be able to simply register all of the
//    controls that you needed to be resized or moved with the window
//    resizer class upon initialization of your dialog.  After this,
//    you would simply have to call only one function upon resize
//    with the new width and height, and all of your controls would
//    be resized for you.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/WNDSIZE/VCS/WNDSIZE.CPV  $
// 
//    Rev 1.6.1.0   22 Jun 1998 19:43:04   RFULLER
// Branch base for version QAKN
// 
//    Rev 1.6   23 Apr 1998 20:38:38   ksackin
// Fixed two bugs having to do with resizing.  One issue was with the width and
// height variables being off by one, and the other with not getting a size
// message in a standard Win32 dialog's WM_SIZE message.  In MFC, we get a size 
// on initial startup, and in Win32, we don't get a size message until the
// window is actually resized once.
//
//    Rev 1.5   21 Apr 1998 16:57:46   DALLEE
// Added WS_CLIPSIBLINGS to size grip control created.
//
//    Rev 1.4   29 Jan 1998 12:58:14   KSACKIN
// Fixed an off by one bug in the initial size of registered controls.
//
//    Rev 1.3   26 Jan 1998 17:38:26   KSACKIN
// Now set the size of the WindowPlacement structure before calling
// GetWindowPlacement().  before we would get bad results on Win95.
//
//    Rev 1.2   20 Jan 1998 17:35:20   KSACKIN
// Commented all function, and added a method of adding controls to the
// CWindowResizer class only using the control ID and the sizing variables
// alone.
//
//    Rev 1.1   10 Dec 1997 20:34:08   KSACKIN
// Typecasted a function's integer arguments to FLOAT to avoid warnings.
//
//    Rev 1.0   10 Dec 1997 20:21:08   KSACKIN
// Initial revision.
//
//    Rev 1.1   09 Dec 1997 20:34:00   KSACKIN
// Added a function to use a SizeGrip in your dialog.  It will create one
// automatically when you initialize a window if you tell it you would like
// one.
//
//    Rev 1.0   07 Oct 1997 16:43:52   ksackin
// Initial revision.
//
//    Rev 1.0   18 Apr 1997 14:02:52   DSACKIN
// Ported from Tesla
//
//    Rev 1.0   13 Mar 1997 15:16:30   RFULLER
// Initial revision
//
//    Rev 1.1   09 Dec 1996 23:46:06   ksackin
// Added the member function RemoveAllItems() to destroy all controls currently
// being resized or moved by the CWindowResizer class.  This is so that we
// can refresh the contents if dialogs come and go.  This will happen if you
// are using an MFC dialog that can be called several times before it gets
// destructed.
//
//    Rev 1.0   20 Nov 1996 01:49:08   ksackin
// Initial revision.
//
//    Rev 1.0   27 Sep 1996 00:31:24   KSACKIN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <windowsx.h>
#include "wndsize.h"


/////////////////////////////////////////////////////////////////////
// Utility Functions.

WNDSIZE_MULTIPLIER_STRUCT *FillMultiplierStruct( WNDSIZE_MULTIPLIER_STRUCT *lprStruct, float xMult, float yMult, float cxMult, float cyMult )
{
    lprStruct->xMultiplier = xMult;
    lprStruct->yMultiplier = yMult;
    lprStruct->cxMultiplier = cxMult;
    lprStruct->cyMultiplier = cyMult;

    return lprStruct;
}

/////////////////////////////////////////////////////////////////////
// Class Definitions.

/////////////////////////////////////////////////////////////////////
// CControlContainer::CControlContainer( void )
//
// Purpose:
//    Constructor of the CControlContainer class.
//
// Parameters:
//    None.
//
// Returns:
//    Constructed class.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CControlContainer::CControlContainer( void )
{
    m_rOSize.cx = 0;
    m_rOSize.cy = 0;
    m_rOPos.x = 0;
    m_rOPos.y = 0;
    m_hWnd = NULL;
    m_dwSWPFlags = SWP_NOZORDER;
}

/////////////////////////////////////////////////////////////////////
// CControlContainer::CControlContainer(
//
// Purpose:
//    Constructor of the CControlContainer class.  This also does
//    initialization.
//
// Parameters:
//    HWND hWnd        - [IN] Handle of the window to contain.
//    WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct - [IN] Multiplier.
//    DWORD dwSWPFlags - [IN] Window moving flags.  See SetWindowPos.
//
// Returns:
//    Constructed class.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CControlContainer::CControlContainer(
    HWND hWnd,
    WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct,
    DWORD dwSWPFlags /*=SWP_NOZORDER*/
    )
{
    m_rOPos.x = 0;
    m_rOPos.y = 0;
    m_rOSize.cy = 0;
    m_rOSize.cx = 0;

    m_dwSWPFlags = SWP_NOZORDER;

    MapToControl( hWnd, lprMultiplierStruct, dwSWPFlags );
}


/////////////////////////////////////////////////////////////////////
// void CControlContainer::InitSize( HWND hWnd )
//
// Purpose:
//    Function to initialize the size of a control.
//
// Parameters:
//    hWnd - [IN] handle of the window to initialize the internal size
//                members of this class with.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CControlContainer::InitSize(
    HWND hWnd
    )
{
WINDOWPLACEMENT rWindowPlacement;

    m_hWnd = hWnd;

                                        // Set the size of the WindowPlacement
                                        // structure.  If you don't you get
                                        // really jacked up!
    rWindowPlacement.length = sizeof( rWindowPlacement );

                                        // Set up the initial window placement for the
                                        // control that we are mapping to.
    GetWindowPlacement( m_hWnd, &rWindowPlacement );
    m_rOPos.x = rWindowPlacement.rcNormalPosition.left;
    m_rOPos.y = rWindowPlacement.rcNormalPosition.top;
    m_rOSize.cx = rWindowPlacement.rcNormalPosition.right - rWindowPlacement.rcNormalPosition.left;
    m_rOSize.cy = rWindowPlacement.rcNormalPosition.bottom - rWindowPlacement.rcNormalPosition.top;
}

/////////////////////////////////////////////////////////////////////
// void CControlContainer::MapToControl( HWND hWnd, WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct, DWORD dwSWPFlags /*=SWP_NOZORDER*/ )
//
// Purpose:
//    This function will map this CControlContainer to a control from
//    your dialog.  You simply pass in the hWnd of the control, and
//    the sizing multiplier struct, and it will initialize this
//    container.
//
// Parameters:
//    HWND hWnd        - [IN] Handle of the window to contain.
//    WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct - [IN] Multiplier.
//    DWORD dwSWPFlags - [IN] Window moving flags.  See SetWindowPos.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CControlContainer::MapToControl(
    HWND hWnd,
    WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct,
    DWORD dwSWPFlags /*=SWP_NOZORDER*/
    )
{
    InitSize( hWnd );
    memcpy( &m_rMultiplierStruct, lprMultiplierStruct, sizeof(WNDSIZE_MULTIPLIER_STRUCT) );
    m_dwSWPFlags = dwSWPFlags;
}

/////////////////////////////////////////////////////////////////////
// void CControlContainer::UpdateControl( int nTotalDeltaX, int nTotalDeltaY )
//
// Purpose:
//    This function will update a control to a new size for the
//    window.  It takes the original position of the control and
//    moves it according to the total delta of width and
//    height of the dialog according to how it is supposed to move.
//
// Parameters:
//    int nTotalDeltaX - [IN] Total width change of the parent window.
//    int nTotalDeltaY - [IN] Total height change of the parent window.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CControlContainer::UpdateControl(
    int nTotalDeltaX,
    int nTotalDeltaY
    )
{
int nNewX = m_rOPos.x;
int nNewY = m_rOPos.y;
int nNewCX = m_rOSize.cx;
int nNewCY = m_rOSize.cy;

     nNewX  += (int)(nTotalDeltaX * m_rMultiplierStruct.xMultiplier);
     nNewY  += (int)(nTotalDeltaY * m_rMultiplierStruct.yMultiplier);
     nNewCX += (int)(nTotalDeltaX * m_rMultiplierStruct.cxMultiplier);
     nNewCY += (int)(nTotalDeltaY * m_rMultiplierStruct.cyMultiplier);

     MoveWindow( m_hWnd,
                 nNewX,
                 nNewY,
                 nNewCX,
                 nNewCY,
                 !(m_dwSWPFlags & SWP_NOREDRAW) );
}


/////////////////////////////////////////////////////////////////////
// CControlContainerNode::CControlContainerNode( void )
//
// Purpose:
//    Constructor of a CControlContainerNode.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CControlContainerNode::CControlContainerNode( void )
{
   m_lpNextContainerNodeWidth=NULL;
   m_lpPrevContainerNodeWidth=NULL;
   m_lpNextContainerNodeHeight=NULL;
   m_lpPrevContainerNodeHeight=NULL;
}

/////////////////////////////////////////////////////////////////////
// CControlContainerNode::CControlContainerNode( HWND  hWnd, WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct, DWORD dwSWPFlags /*=SWP_NOZORDER*/ )
//
// Purpose:
//    Constructor of a CControlContainerNode that initialized the
//    node as well.
//
// Parameters:
//    HWND hWnd        - [IN] Handle of the window to contain.
//    WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct - [IN] Multiplier.
//    DWORD dwSWPFlags - [IN] Window moving flags.  See SetWindowPos.
//
// Returns:
//    Constructed class.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CControlContainerNode::CControlContainerNode(
    HWND  hWnd,
    WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct,
    DWORD dwSWPFlags /*=SWP_NOZORDER*/
    )
{
   m_lpNextContainerNodeWidth=NULL;
   m_lpPrevContainerNodeWidth=NULL;
   m_lpNextContainerNodeHeight=NULL;
   m_lpPrevContainerNodeHeight=NULL;
   m_cControlContainer.MapToControl( hWnd, lprMultiplierStruct, dwSWPFlags );
}

/////////////////////////////////////////////////////////////////////
// void CControlContainerNode::MapToControl( HWND hWnd, WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct, DWORD dwSWPFlags /*=SWP_NOZORDER*/ )
//
// Purpose:
//    Function to map a control container node to a control.
//
// Parameters:
//    HWND hWnd        - [IN] Handle of the window to contain.
//    WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct - [IN] Multiplier.
//    DWORD dwSWPFlags - [IN] Window moving flags.  See SetWindowPos.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CControlContainerNode::MapToControl(
    HWND hWnd,
    WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct,
    DWORD dwSWPFlags /*=SWP_NOZORDER*/
    )
{
   m_cControlContainer.MapToControl( hWnd, lprMultiplierStruct, dwSWPFlags );
}


/////////////////////////////////////////////////////////////////////
// CControlContainerList::CControlContainerList(void)
//
// Purpose:
//    Constructor for a ControlContainerList.
//
// Parameters:
//    None.
//
// Returns:
//    Constructed class.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CControlContainerList::CControlContainerList(void)
{
   m_lpcTopNode=NULL;
   m_lpcBottomNode=NULL;
   m_lpcLeftNode=NULL;
   m_lpcRightNode=NULL;
}

/////////////////////////////////////////////////////////////////////
// CControlContainerList::~CControlContainerList(void)
//
// Purpose:
//    Destructor for a ControlContainerList.  This will take care of
//    deleting all of the data we are storing in this list.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CControlContainerList::~CControlContainerList(void)
{
CControlContainerNode *lpcControlContainerNodeIterator=m_lpcTopNode;
CControlContainerNode *lpcNodeToDelete=NULL;
                                        // set the control iterator to the top, and then we are
                                        // going to traverse the list, telling each to update
                                        // itself.

   while ( lpcControlContainerNodeIterator )
      {
      lpcNodeToDelete = lpcControlContainerNodeIterator;
      lpcControlContainerNodeIterator = lpcControlContainerNodeIterator->GetNextControlNodeHeight();
      delete lpcNodeToDelete;
      }
}

/////////////////////////////////////////////////////////////////////
// void CControlContainerList::AddControl(
//
// Purpose:
//    This function will add a control to a CControlContainerList.
//    This function will be called from the RegisterWindow call to
//    a CWindowResizer class.  This will add the control to our
//    internal list of controls to resize and move with the window.
//    also, this will do all initialization of our internal tracking
//    of that window.
//
// Parameters:
//    HWND  hWnd          - [IN] handle of the control to add.
//    float xMultiplier   - [IN] X Multiplier for this window.
//    float yMultiplier   - [IN] Y Multiplier for this window.
//    float cxMultiplier  - [IN] Cy Multiplier for this window.
//    float xyMultiplier  - [IN] Cy Multiplier for this window.
//    DWORD dwSWPFlags    - [IN] Window movement flags. (=SWP_NOZORDER)
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CControlContainerList::AddControl(
    HWND  hWnd,
    float xMultiplier,
    float yMultiplier,
    float cxMultiplier,
    float cyMultiplier,
    DWORD dwSWPFlags /*=SWP_NOZORDER*/
    )
{
CControlContainerNode *lpcControlContainerNode;
WNDSIZE_MULTIPLIER_STRUCT rMultiplierStruct;

                                        // Fill in the fields of our multiplier
                                        // structure.
    FillMultiplierStruct( &rMultiplierStruct, xMultiplier, yMultiplier, cxMultiplier, cyMultiplier );

                                        // Create a control container node for
                                        // this item.
    lpcControlContainerNode = new CControlContainerNode( hWnd, &rMultiplierStruct, dwSWPFlags );

                                        // If there is nothing in our list, we are going
                                        // to set the top and the bottom pointer to the only
                                        // item, which will have the next item pointing to NULL.
	if ( m_lpcTopNode == NULL )
		{
		m_lpcTopNode = m_lpcBottomNode = m_lpcLeftNode = m_lpcRightNode = lpcControlContainerNode;
		}
	else
		{
                                        // Else, if there are items in the list already, we are
                                        // going to insert this item in the list horizontally,
                                        // as well as vertically (same node, just pointer sorting).
        InsertNodeHorizontal( lpcControlContainerNode );
        InsertNodeVertical( lpcControlContainerNode );
		}
}


/////////////////////////////////////////////////////////////////////
// void CControlContainerList::AddControl( HWND hWnd, WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct, DWORD dwSWPFlags /*=SWP_NOZORDER*/ )
//
// Purpose:
//    This function will add a control to a CControlContainerList.
//    This function will be called from the RegisterWindow call to
//    a CWindowResizer class.  This will add the control to our
//    internal list of controls to resize and move with the window.
//    also, this will do all initialization of our internal tracking
//    of that window.
//
// Parameters:
//    HWND  hWnd          - [IN] handle of the control to add.
//    WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct - [IN] Multiplier.
//    DWORD dwSWPFlags    - [IN] Window movement flags. (=SWP_NOZORDER)
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CControlContainerList::AddControl( HWND hWnd, WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct, DWORD dwSWPFlags /*=SWP_NOZORDER*/ )
{
CControlContainerNode *lpcControlContainerNode;

    lpcControlContainerNode = new CControlContainerNode( hWnd, lprMultiplierStruct, dwSWPFlags );

                                        // If there is nothing in our list, we are going
                                        // to set the top and the bottom pointer to the only
                                        // item, which will have the next item pointing to NULL.
	if ( m_lpcTopNode == NULL )
		{
		m_lpcTopNode = m_lpcBottomNode = m_lpcLeftNode = m_lpcRightNode = lpcControlContainerNode;
		}
	else
		{
                                        // Else, if there are items in the list already, we are
                                        // going to insert this item in the list horizontally,
                                        // as well as vertically (same node, just pointer sorting).
        InsertNodeHorizontal( lpcControlContainerNode );
        InsertNodeVertical( lpcControlContainerNode );
		}
}

/////////////////////////////////////////////////////////////////////
// void CControlContainerList::InsertNodeHorizontal( CControlContainerNode *lpcControlContainerNode )
//
// Purpose:
//    This function inserts a node horizontally into our list of
//    controls.  This is neccessary as we store a sorted list both
//    horizontally and vertically for these controls so that when
//    a window is resized, we can move the windows accordingly
//    without window redrawing problems.
//
// Parameters:
//    CControlContainerNode *lpcControlContainerNode - [IN] Node to insert.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CControlContainerList::InsertNodeHorizontal(
    CControlContainerNode *lpcControlContainerNode
    )
{
CControlContainerNode *lpcNodeIterator=m_lpcLeftNode;

    if ( NULL == m_lpcLeftNode )
        {
                                        // If for some reason our left is NULL, our list is
                                        // empty, so this node will be the furthest left and right.
        m_lpcLeftNode = m_lpcRightNode = lpcControlContainerNode;
        }
    else
        {
                                        // Find item after us, or hit end of list.
        while( (lpcNodeIterator) &&
              (lpcNodeIterator->GetControlContainer()->GetXPos() < lpcControlContainerNode->GetControlContainer()->GetXPos()) )
           {
           lpcNodeIterator = lpcNodeIterator->GetNextControlNodeWidth();
           }


                                        // If we found one...
        if ( lpcNodeIterator )
            {
                                        // Set our prev to this item's prev.
            lpcControlContainerNode->SetPrevControlNodeWidth( lpcNodeIterator->GetPrevControlNodeWidth() );

                                        // Set our next to this item.
            lpcControlContainerNode->SetNextControlNodeWidth( lpcNodeIterator );

                                        // If there was a previous control...
            if ( lpcNodeIterator->GetPrevControlNodeWidth() )
                {
                                        // Set this item's previous nodes' next to us.
                lpcNodeIterator->GetPrevControlNodeWidth()->SetNextControlNodeWidth( lpcControlContainerNode );
                }

                                        // Set this item's previous to us.
            lpcNodeIterator->SetPrevControlNodeWidth( lpcControlContainerNode );

            if ( lpcNodeIterator == m_lpcLeftNode )
                {
                                        // The item that we inserted before was the old left,
                                        // therefore, the item that we inserter is the new
                                        // left.
                m_lpcLeftNode = lpcControlContainerNode;
                }
            }
        else
            {
                                        // If we did not find one, we are going to insert
                                        // ourselves at the end of the list.
            lpcControlContainerNode->SetPrevControlNodeWidth( m_lpcRightNode );
            m_lpcRightNode->SetNextControlNodeWidth( lpcControlContainerNode );

                                        // Since we inserted after the farthest right, then we need to
                                        // set the new right to us, since we are now the farthest right.
            m_lpcRightNode =  lpcControlContainerNode;
            }
        }
}

/////////////////////////////////////////////////////////////////////
// void CControlContainerList::InsertNodeVertical( CControlContainerNode *lpcControlContainerNode )
//
// Purpose:
//    This function inserts a node vertically into our list of
//    controls.  This is neccessary as we store a sorted list both
//    horizontally and vertically for these controls so that when
//    a window is resized, we can move the windows accordingly
//    without window redrawing problems.
//
// Parameters:
//    CControlContainerNode *lpcControlContainerNode - [IN] Node to insert.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CControlContainerList::InsertNodeVertical(
    CControlContainerNode *lpcControlContainerNode
    )
{
CControlContainerNode *lpcNodeIterator=m_lpcTopNode;

    if ( NULL == m_lpcTopNode )
        {
                                        // If for some reason our top is NULL, our list is
                                        // empty, so this node will be the furthest top and bottom.
        m_lpcTopNode = m_lpcBottomNode = lpcControlContainerNode;
        }
    else
        {
                                        // Find item after us, or hit end of list.
        while( (lpcNodeIterator) &&
              (lpcNodeIterator->GetControlContainer()->GetYPos() < lpcControlContainerNode->GetControlContainer()->GetYPos()) )
           {
           lpcNodeIterator = lpcNodeIterator->GetNextControlNodeHeight();
           }


                                        // If we found one...
        if ( lpcNodeIterator )
            {
                                        // Set our prev to this item's prev.
            lpcControlContainerNode->SetPrevControlNodeHeight( lpcNodeIterator->GetPrevControlNodeHeight() );

                                        // Set our next to this item.
            lpcControlContainerNode->SetNextControlNodeHeight( lpcNodeIterator );

                                        // If there was a previous control...
            if ( lpcNodeIterator->GetPrevControlNodeHeight() )
                {
                                        // Set this item's previous nodes' next to us.
                lpcNodeIterator->GetPrevControlNodeHeight()->SetNextControlNodeHeight( lpcControlContainerNode );
                }

                                        // Set this item's previous to us.
            lpcNodeIterator->SetPrevControlNodeHeight( lpcControlContainerNode );

            if ( lpcNodeIterator == m_lpcTopNode )
                {
                                        // The item that we inserted before was the old top,
                                        // therefore, the item that we inserter is the new
                                        // top.
                m_lpcTopNode = lpcControlContainerNode;
                }
            }
        else
            {
                                        // If we did not find one, we are going to insert
                                        // ourselves at the end of the list.
            lpcControlContainerNode->SetPrevControlNodeHeight( m_lpcBottomNode );
            m_lpcBottomNode->SetNextControlNodeHeight( lpcControlContainerNode );

                                        // Since we inserted after the bottom, then we need to
                                        // set the new bottom to us, since we are now the bottom.
            m_lpcBottomNode =  lpcControlContainerNode;
            }
        }
}


/////////////////////////////////////////////////////////////////////
// void CControlContainerList::UpdateControlsHorizontally( int nCurrentDeltaX, int nTotalDeltaX, int nTotalDeltaY )
//
// Purpose:
//    This function will update all controls horizontally.  This will
//    be called by the UpdateSize function to update all controls
//    horizontally.  This needs to be done seperately so that we
//    will not have the window redrawing problem as well.
//    Also, this will only move the controls if the current delta
//    X is not zero.  This will avoid flicker if you only
//    resize a dialog in one direction.
//
// Parameters:
//    int nCurrentDeltaX - [IN] Change in width of the dialog of only
//                              the last size update.
//    int nTotalDeltaX   - [IN] Total change in width since we started
//                              tracking this dialog.
//    int nTotalDeltaY   - [IN] Total change in height since we started
//                              tracking this dialog.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CControlContainerList::UpdateControlsHorizontally(
    int nCurrentDeltaX,
    int nTotalDeltaX,
    int nTotalDeltaY
    )
{
CControlContainerNode *lpcControlContainerNodeIterator;
                                        // set the control iterator to the top, and then we are
                                        // going to traverse the list, telling each to update
                                        // itself.

    if ( nCurrentDeltaX )
        {
                                        // If we grew...
        if ( nCurrentDeltaX >= 0 )
            {
                                        // Start from the right...
            lpcControlContainerNodeIterator=m_lpcRightNode;

                                        // Resize controls from right to left.
            while ( lpcControlContainerNodeIterator )
                {
                lpcControlContainerNodeIterator->GetControlContainer()->UpdateControl( nTotalDeltaX, nTotalDeltaY );
                lpcControlContainerNodeIterator = lpcControlContainerNodeIterator->GetPrevControlNodeWidth();
                }
            }
                                        // If we shrunk...
        else
            {
                                        // Start from the left...
            lpcControlContainerNodeIterator=m_lpcLeftNode;

                                        // resize controls from left to right.
            while ( lpcControlContainerNodeIterator )
                {
                lpcControlContainerNodeIterator->GetControlContainer()->UpdateControl( nTotalDeltaX, nTotalDeltaY );
                lpcControlContainerNodeIterator = lpcControlContainerNodeIterator->GetNextControlNodeWidth();
                }
            }
        }
}

/////////////////////////////////////////////////////////////////////
// void CControlContainerList::UpdateControlsVertically( int nCurrentDeltaY, int nTotalDeltaX, int nTotalDeltaY )
//
// Purpose:
//    This function will update all controls vertically.  This will
//    be called by the UpdateSize function to update all controls
//    vertically.  This needs to be done seperately so that we
//    will not have a window redrawing problem.
//    Also, this will only move the controls if the current delta
//    Y is not zero.  This will avoid flicker if you only
//    resize a dialog in one direction.
//
// Parameters:
//    int nCurrentDeltaY - [IN] Change in height of the dialog of only
//                              the last size update.
//    int nTotalDeltaX   - [IN] Total change in width since we started
//                              tracking this dialog.
//    int nTotalDeltaY   - [IN] Total change in height since we started
//                              tracking this dialog.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CControlContainerList::UpdateControlsVertically(
    int nCurrentDeltaY,
    int nTotalDeltaX,
    int nTotalDeltaY
    )
{
CControlContainerNode *lpcControlContainerNodeIterator;
                                        // set the control iterator to the top, and then we are
                                        // going to traverse the list, telling each to update
                                        // itself.

    if ( nCurrentDeltaY )
        {
                                        // If we grew...
        if ( nCurrentDeltaY >= 0 )
            {
                                        // Start from the bottom...
            lpcControlContainerNodeIterator=m_lpcBottomNode;

                                        // Resize controls from bottom to top.
            while ( lpcControlContainerNodeIterator )
                {
                lpcControlContainerNodeIterator->GetControlContainer()->UpdateControl( nTotalDeltaX, nTotalDeltaY );
                lpcControlContainerNodeIterator = lpcControlContainerNodeIterator->GetPrevControlNodeHeight();
                }
            }
                                        // If we shrunk...
        else
            {
                                        // Start from the top...
            lpcControlContainerNodeIterator=m_lpcTopNode;

                                        // resize controls from top to bottom.
            while ( lpcControlContainerNodeIterator )
                {
                lpcControlContainerNodeIterator->GetControlContainer()->UpdateControl( nTotalDeltaX, nTotalDeltaY );
                lpcControlContainerNodeIterator = lpcControlContainerNodeIterator->GetNextControlNodeHeight();
                }
            }
        }
}

/////////////////////////////////////////////////////////////////////
// void CControlContainerList::RemoveAllItems( void )
//
// Purpose:
//    This function will remove all items from a CControlContainerList.
//    Most likely, this will get called from the destructor of a
//    CWindowResizer class, but could get called during use of this
//    class to clear out all currently tracking controls.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CControlContainerList::RemoveAllItems( void )
{
CControlContainerNode *lpcControlContainerNodeToDelete;

                                        // Since these items are just linked
                                        // horizontally and vertically, if we just
                                        // remove them all vertically, we will have
                                        // removed them all.
    while ( m_lpcTopNode )
        {
                                        // Save this node to delete.
        lpcControlContainerNodeToDelete = m_lpcTopNode;

                                        // Set the top pointer to the next node.
        m_lpcTopNode = m_lpcTopNode->GetNextControlNodeHeight();

                                        // Delete this items.
        delete (lpcControlContainerNodeToDelete);
        }

   m_lpcTopNode = m_lpcBottomNode = m_lpcLeftNode = m_lpcRightNode = NULL;
}


/////////////////////////////////////////////////////////////////////
// CWindowResizer::CWindowResizer( void )
//
// Purpose:
//    Constructor of the CWindowResizer class.
//
// Parameters:
//    None.
//
// Returns:
//    Constructed class.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
CWindowResizer::CWindowResizer( void )
{
   m_bInitialized = FALSE;
   m_bUseSizeGrip = FALSE;
   m_nCurrentWindowWidth=NOT_INITIALIZED;
   m_nCurrentWindowHeight=NOT_INITIALIZED;
   m_nTotalDeltaX=0;
   m_nTotalDeltaY=0;
   m_hParentWindow=NULL;
   m_hSizeGrip=NULL;
}

LRESULT WndSize_DefWindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    return CallWindowProc(
            ((CWindowResizer *)GetWindowLong( hWnd, GWL_USERDATA ))->m_lpfnParentWindowProc,
            hWnd,
            msg,
            wParam,
            lParam );
}

/////////////////////////////////////////////////////////////////////
// CWindowResizer::AttachToParent( HWND hParent )
//
// Purpose:
//    This function will be used to attach a CWindowResizer class
//    to a particular window.  Usually, this window will be the
//    parent of the controls to be resized.  This will allow for
//    this class to receive window messages from it's parent window
//    so as to process messages like WM_RESIZE without having the
//    parent pass on the message.
//
// Parameters:
//    HWND hParent - [IN] Parent window to attach this class to.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CWindowResizer::AttachToParent(
    HWND hParent
    )
{
                                        // Set our internal variable for our
                                        // parent window.
    m_hParentWindow = hParent;

    SetWindowLong( hParent, GWL_USERDATA, (LPARAM)this );
    m_lpfnParentWindowProc = (WNDPROC)GetWindowLong( hParent, GWL_WNDPROC );
    SubclassWindow( hParent, WndSize_DefWindowProc );
}

/////////////////////////////////////////////////////////////////////
// void CWindowResizer::SetParent( HWND hParent )
//
// Purpose:
//    This function will allow a client of this class to specify the
//    parent window.  This should be done any time after the create,
//    most likely during the InitDialog.
//
// Parameters:
//    HWND hParent - [IN] Parent of the windows we are controling.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CWindowResizer::SetParent(
    HWND hParent
    )
{
RECT rWindowRect;

    // Set our internal parent variable.
    m_hParentWindow = hParent;

    // Get the size of the parent window.
    GetClientRect( hParent, &rWindowRect );

    // Set initial window size so that we will have a starting reference.
    m_nCurrentWindowWidth = rWindowRect.right;
    m_nCurrentWindowHeight = rWindowRect.bottom;

}

/////////////////////////////////////////////////////////////////////
// void CWindowResizer::UpdateSize( int cx, int cy )
//
// Purpose:
//    This is the update size function of the CWindowResizer.  This
//    function should get called from a parent window size message.
//    This will take care of updating all controls that this class
//    is currently tracking.
//
// Parameters:
//    UINT nType - [IN] Type of resizing (From WM_SIZE message)
//    int  cx    - [IN] New width of the parent window.
//    int  cy    - [IN] New height of the parent window.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CWindowResizer::UpdateSize(
    UINT nType,
    int  cx,
    int  cy
    )
{
int nCurrentDeltaX;
int nCurrentDeltaY;

                                        // For the initial sizing.  If the
                                        // size has not been initialized, we need to
                                        // set this as the initial size.
                                        // NOT_INITIALIZED == -1
    if ((m_nCurrentWindowWidth == NOT_INITIALIZED) && (m_nCurrentWindowHeight == NOT_INITIALIZED))
        {
        m_nCurrentWindowWidth = cx;
        m_nCurrentWindowHeight = cy;
        }

    nCurrentDeltaX = cx - m_nCurrentWindowWidth;
    nCurrentDeltaY = cy - m_nCurrentWindowHeight;
    m_nCurrentWindowWidth = cx;
    m_nCurrentWindowHeight = cy;

    if ( TRUE == m_bInitialized )
        {

                                        // Show or hide size grip according
                                        // to size type.
        if ( nType == SIZE_MAXIMIZED )
            ShowSizeGrip( FALSE );
        else if ( nType == SIZE_RESTORED )
            ShowSizeGrip( TRUE );

                                        // If we grew horizontally...
        if ( 0 != nCurrentDeltaX )
            {
                                        // Add the width member first, without the height.
            m_nTotalDeltaX += nCurrentDeltaX;

                                        // resize and move controls horizontally
            m_cControlContainerList.UpdateControlsHorizontally( nCurrentDeltaX, m_nTotalDeltaX, m_nTotalDeltaY );
            }
                                        // If we grew vertically...
        if ( 0 != nCurrentDeltaY )
            {
                                        // Then, add the height member.
            m_nTotalDeltaY += nCurrentDeltaY;

                                        // resize and move controls vertically
            m_cControlContainerList.UpdateControlsVertically( nCurrentDeltaY, m_nTotalDeltaX, m_nTotalDeltaY );
            }
        }
}

/////////////////////////////////////////////////////////////////////
// void CWindowResizer::RegisterWindow( HWND  hControlWnd, WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct, DWORD dwSWPFlags /*=SWP_NOZORDER*/ )
//
// Purpose:
//    This will be the main way that an application will register
//    it's windows for resizing and moving.  You would call this
//    function from the initdialog message of your application.
//    This function actually registers the window in an internal
//    list so that we can track and set the movement of this control
//    as the parent window resizes.
//
// Parameters:
//    HWND hWnd        - [IN] Handle of the window to register.
//    WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct - [IN] Multiplier.
//    DWORD dwSWPFlags - [IN] Window moving flags.  See SetWindowPos.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CWindowResizer::RegisterWindow(
    HWND  hControlWnd,
    WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct,
    DWORD dwSWPFlags /*=SWP_NOZORDER*/
    )
{
   m_cControlContainerList.AddControl( hControlWnd, lprMultiplierStruct, dwSWPFlags );
}

/////////////////////////////////////////////////////////////////////
// void CWindowResizer::RegisterWindow( int nControlID, WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct, DWORD dwSWPFlags /*=SWP_NOZORDER*/ )
//
// Purpose:
//    This function is a passthrough to the real RegisterWindow
//    function.  You should use this if you have already set a parent
//    window, and this will do the GetDlgItem calls for you.
//
// Parameters:
//    int   nControlID - [IN] ID of the control to add.
//    WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct - [IN] Multiplier.
//    DWORD dwSWPFlags - [IN] Window moving flags.  See SetWindowPos.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CWindowResizer::RegisterWindow(
    int   nControlID,
    WNDSIZE_MULTIPLIER_STRUCT * lprMultiplierStruct,
    DWORD dwSWPFlags /*=SWP_NOZORDER*/
    )
{
HWND hControlWnd;

                                        // If we have a parent window...
    if ( NULL != m_hParentWindow )
        {
                                        // Get the hwnd of this dialog item.
        hControlWnd = GetDlgItem( m_hParentWindow, nControlID );

                                        // If we got one correctly.
        if ( NULL != hControlWnd )
            {
                                        // Pass this command on.
            RegisterWindow( hControlWnd, lprMultiplierStruct, dwSWPFlags );
            }
        }
}

/////////////////////////////////////////////////////////////////////
// void CWindowResizer::RegisterWindow(
//
// Purpose:
//    This will be the main way that an application will register
//    it's windows for resizing and moving.  You would call this
//    function from the initdialog message of your application.
//    This function actually registers the window in an internal
//    list so that we can track and set the movement of this control
//    as the parent window resizes.
//
// Parameters:
//    HWND hWnd          - [IN] Handle of the window to register.
//    float xMultiplier  - [IN] X Multiplier for this window.
//    float yMultiplier  - [IN] Y Multiplier for this window.
//    float cxMultiplier - [IN] Cy Multiplier for this window.
//    float xyMultiplier - [IN] Cy Multiplier for this window.
//    DWORD dwSWPFlags   - [IN] Window moving flags.  See SetWindowPos.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CWindowResizer::RegisterWindow(
    HWND    hControlWnd,
    float   xMultiplier,
    float   yMultiplier,
    float   cxMultiplier,
    float   cyMultiplier,
    DWORD   dwSWPFlags /*=SWP_NOZORDER*/
    )
{
   m_cControlContainerList.AddControl( hControlWnd, xMultiplier, yMultiplier, cxMultiplier, cyMultiplier, dwSWPFlags );
}

/////////////////////////////////////////////////////////////////////
// void CWindowResizer::RegisterWindow(
//
// Purpose:
//    This function is a passthrough to the real RegisterWindow
//    function.  You should use this if you have already set a parent
//    window, and this will do the GetDlgItem calls for you.
//
// Parameters:
//    int   nControlID - [IN] ID of the control to add.
//    float xMultiplier  - [IN] X Multiplier for this window.
//    float yMultiplier  - [IN] Y Multiplier for this window.
//    float cxMultiplier - [IN] Cy Multiplier for this window.
//    float xyMultiplier - [IN] Cy Multiplier for this window.
//    DWORD dwSWPFlags   - [IN] Window moving flags.  See SetWindowPos.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CWindowResizer::RegisterWindow(
    int     nControlID,
    float   xMultiplier,
    float   yMultiplier,
    float   cxMultiplier,
    float   cyMultiplier,
    DWORD   dwSWPFlags /*=SWP_NOZORDER*/
    )
{
HWND hControlWnd;

                                        // If we have a parent window...
    if ( NULL != m_hParentWindow )
        {
                                        // Get the hwnd of this dialog item.
        hControlWnd = GetDlgItem( m_hParentWindow, nControlID );

                                        // If we got one correctly.
        if ( NULL != hControlWnd )
            {
                                        // Pass this command on.
            RegisterWindow( hControlWnd, xMultiplier, yMultiplier, cxMultiplier, cyMultiplier, dwSWPFlags );
            }
        }
}

/////////////////////////////////////////////////////////////////////
// void CWindowResizer::UseSizeGrip(
//
// Purpose:
//    This function will tell the WindowResizer whether to use a size
//    grip control or not.  If you set this flag to TRUE, the
//    CWindowResizer class will create and move a size grip control
//    in the lower left side of the parent window.  This removes the
//    burden from the parent window, and takes care of all sizing
//    and maximizing issues with the size grip.
//
// Parameters:
//    BOOL bUseSizeGrip  - [IN] Whether to use the size grip or not.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CWindowResizer::UseSizeGrip(
    BOOL bUseSizeGrip
    )
{
                                        // This will set up our flag to use a
                                        // size grip with this CWindowResizer.
                                        // When we get the SetInitialized(TRUE)
                                        // function call, we are going to
                                        // create a size grip control for the
                                        // dialog.
    m_bUseSizeGrip = bUseSizeGrip;
}

/////////////////////////////////////////////////////////////////////
// void CWindowResizer::RemoveAllItems( void )
//
// Purpose:
//    Function to remove all items from the CWindowResizer.  This
//    function will get called from the destructor normally, but
//    you can call this function at any time to clear the Window
//    resizer of all it's wathced controls to reuse the class.
//
// Parameters:
//    None.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CWindowResizer::RemoveAllItems( void )
{
                                        // Remove all items from our
                                        // control container list.
    m_cControlContainerList.RemoveAllItems();

    m_bInitialized =
        m_nCurrentWindowHeight =
        m_nCurrentWindowWidth =
        m_nTotalDeltaX =
        m_nTotalDeltaY =
        0;
}

/////////////////////////////////////////////////////////////////////
// void CWindowResizer::SetInitialized(
//
// Purpose:
//   This function will set the initialized flag in our class, and
//   if you have asked to have a sizegrip, it will create one upon
//   the first call to SetTnitialize( TRUE ).
//
// Parameters:
//   BOOL bNewState - New state of Windows Resizer.
//                    TRUE  - Ready to start processing.
//                    FALSE - Still Adding Control.  This should
//                            probably never be passed to this function.
// Returns:
//   None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 12/09/97 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CWindowResizer::SetInitialized(
    BOOL bNewState
    )
{
RECT rParentRect;
RECT rSizeGripRect;
WNDSIZE_MULTIPLIER_STRUCT rMultiplierStruct;

                                        // If we are initializing, and we want
                                        // to use a SizeGrip control...
                                        // Also, verify that we have a window
                                        // to place it on...
    if (( TRUE == bNewState ) &&
        ( TRUE == m_bUseSizeGrip ) &&
        ( NULL != m_hParentWindow ))
        {
                                        // Get the dimentions of the window we
                                        // are to add a size grip control to.
        GetClientRect( m_hParentWindow, &rParentRect );

                                        // Set up the size of our size grip
                                        // control based on dialog units.
        rSizeGripRect.left=0;
        rSizeGripRect.top=0;
        rSizeGripRect.right=20;
        rSizeGripRect.bottom=20;
                                        // Convert our dialog units to pixels.
        MapDialogRect( m_hParentWindow, &rSizeGripRect );


                                        // Create the size grip control.
        m_hSizeGrip = CreateWindow(
            _T("SCROLLBAR"),            // Window class to create.
            _T(""),                     // Window name.
            SBS_SIZEBOXBOTTOMRIGHTALIGN | SBS_SIZEBOX | SBS_SIZEGRIP | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
            rParentRect.right-rSizeGripRect.right,   // Left side position.
            rParentRect.bottom-rSizeGripRect.bottom, // Top of control.
            rSizeGripRect.right,        // Width of size grip.
            rSizeGripRect.bottom,       // Height of size grip.
            m_hParentWindow,            // Parent for this window.
            NULL,
            GetWindowInstance( m_hParentWindow ),
            NULL
            );

                                        // If we successfully created the
                                        // control, we need to add it to the
                                        // controls we are going to move and
                                        // size.  This is because we need to
                                        // move the SizeGrip with the dialog
                                        // size, or it will stay in the same
                                        // place.
        if ( NULL != m_hSizeGrip )
            {
                                        // Register this window with our
                                        // controls to size.
            RegisterWindow(
                m_hSizeGrip,
                FillMultiplierStruct( &rMultiplierStruct, (FLOAT)1, (FLOAT)1, (FLOAT)0, (FLOAT)0 ),
                SWP_NOSIZE | SWP_NOZORDER );
            }
        }

                                        // Set initialized flag to new state.
    m_bInitialized = bNewState;
}

/////////////////////////////////////////////////////////////////////
// void CWindowResizer::ShowSizeGrip( BOOL bShow )
//
// Purpose:
//    This function will be used to show and to hide the size grip
//    control.  Typically this function will get called with a FALSE
//    parameter when a window is Maximized, and the a TRUE again when
//    it is restored.
//
// Parameters:
//    BOOL bShow = [IN] Show sizegrip?
//                      TRUE  - Show the size grip.
//                      FALSE - Hide the size grip.
//
// Returns:
//    None.
//
// See Also:
//
/////////////////////////////////////////////////////////////////////
// 01/20/98 - KSACKIN - Function Created.
/////////////////////////////////////////////////////////////////////
void CWindowResizer::ShowSizeGrip(
    BOOL bShow
    )
{
    if ( TRUE == bShow )
        {
        ShowWindow( m_hSizeGrip, SW_SHOW );
        }
    else
        {
        ShowWindow( m_hSizeGrip, SW_HIDE );
        }
}
