// XListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "globals.h"
#include "XListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CXListCtrl

CXListCtrl::CXListCtrl()
{
}

CXListCtrl::~CXListCtrl()
{
}


BEGIN_MESSAGE_MAP(CXListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CXListCtrl)
	//ON_NOTIFY(HDN_BEGINTRACK, 0, OnBegintrack)
	//ON_NOTIFY(HDN_DIVIDERDBLCLICK, 0, OnDividerdblclick)
	//}}AFX_MSG_MAP
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomdraw)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXListCtrl message handlers

void CXListCtrl::OnCustomdraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
static const COLORREF crMsg = RGB(0x7f,0x7f,0x7f);
NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

    *pResult = 0;

    // If the control is about to start painting, check the item count.
    // If it's 0, draw our message.  
    // Also, if the list control has a header control, the header is hidden
    // if the count is 0, and shown if it's non-zero.  This is done because
    // if the header is still visible, resizing the columns results in
    // funky repainting problems.  It was easier to just hide the header
    // than try and get the painting working exactly right.

    if ( pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT )
        {
        DWORD dwStyle = GetStyle();
        BOOL bHasHeaderCtrl = ( (dwStyle & LVS_REPORT) &&
                                !(dwStyle & LVS_NOCOLUMNHEADER) );
        CWnd* pHeader;

        if ( bHasHeaderCtrl )
            {
            pHeader = GetDlgItem(0);
            ASSERT_VALID(pHeader);
            }

        if ( 0 != GetItemCount() )
            {
            if ( bHasHeaderCtrl )
                {
                // Redisplay the header control.

                ModifyStyle ( LVS_NOSCROLL, 0, 0 );
                pHeader->ShowWindow ( SW_SHOW );
                }

            *pResult = CDRF_DODEFAULT;  // the control should handle painting
            }
        else
            {
            CRect rcClient;
            GetClientRect ( rcClient );

            rcClient.top += 2;
            rcClient.left += 2;

            if ( bHasHeaderCtrl )
                {
                // Hide the header control.

                ModifyStyle ( 0, LVS_NOSCROLL, 0 );
                pHeader->ShowWindow ( SW_HIDE );
                }

            // Draw our message.

            ::SetTextColor ( pLVCD->nmcd.hdc, crMsg );
            ::DrawText ( pLVCD->nmcd.hdc, _S(IDS_NO_ITEMS_IN_LIST),
                         -1, rcClient, DT_LEFT );
            }
        }
}
