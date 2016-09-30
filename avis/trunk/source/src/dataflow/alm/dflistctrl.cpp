// DFListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "afxtempl.h"
#include "ALM.h"
#include "listctrlex.h"
#include "dfevalstatus.h"
#include "dfevalstate.h"
#include "dfjob.h"
#include "dfsample.h"

#include "DFListCtrl.h"

#include "DFEvalStatus.h"
#include "DFEvalState.h"
#include "DFSample.h"
#include "dferror.h"  
#include "dfmatrix.h"
#include "listctrlex.h"
#include "dflistctrl.h"
#include "DFManager.h"
#include "ALMDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define IDS_ONHOLD  100
#define IDS_PROCESS 101

/////////////////////////////////////////////////////////////////////////////
// CDFListCtrl

CDFListCtrl::CDFListCtrl()
{
}

CDFListCtrl::~CDFListCtrl()
{
}


BEGIN_MESSAGE_MAP(CDFListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CDFListCtrl)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFListCtrl message handlers
/*
void CDFListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CListCtrl::OnLButtonDown(nFlags, point);
	     int index;
             point.x = 2;

             if( ( index = HitTest( point, NULL )) != -1 )
             {
                     SetItemState( index, LVIS_SELECTED | LVIS_FOCUSED , 
                                     LVIS_SELECTED | LVIS_FOCUSED);
			 }			   
}
*/

void CDFListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    CPoint clientPoint(point);
	ScreenToClient(&clientPoint);
    int colnum;
	int index;
	CWnd *dlg= GetParent();
	CALMDlg *almDlg = (CALMDlg *) dlg;
	int i = almDlg->GetCurSel();
	POSITION pos;
	DFSample *sample = almDlg->FindSampleObject(i, pos);

	 if( (index = HitTestEx( clientPoint, &colnum )) != -1 )
     { 
		//Create a menu  on the fly without Move command
        
		CMenu contextMenu;

		if (contextMenu.CreatePopupMenu() == TRUE)
		{
			CString menuString;
			if( sample->GetCurrentStatus() == "Hold") {

			     menuString = CString("Process");
			     contextMenu.InsertMenu (0, MF_BYPOSITION | MF_STRING, IDS_PROCESS, menuString);
            }
			else {
			 	//
                //
                //
    		    //menuString.LoadString(IDS_SCAN);
			    menuString = CString("Put on Hold");
                contextMenu.InsertMenu (0, MF_BYPOSITION | MF_STRING, IDS_ONHOLD, menuString);
			//
			//separator
			//
	        //contextMenu.InsertMenu (0, MF_BYPOSITION | MF_SEPARATOR);
			//
		    //
			}
			contextMenu.TrackPopupMenu( TPM_LEFTALIGN,
				                        point.x,
					                    point.y,
						                this );	
	
	
		}
    }	// TODO: Add your message handler code here
	
}

BOOL CDFListCtrl::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
    int nID = (int) LOWORD(wParam);
	CWnd *dlg= GetParent();
	CALMDlg *almDlg = (CALMDlg *) dlg;
	int i = almDlg->GetCurSel();
	POSITION pos;
	DFSample *sample = almDlg->FindSampleObject(i, pos);


	switch (nID)
	{
    
    case IDS_ONHOLD:
        sample->SetCurrentStatus(DFSample::HOLD);		
//		almDlg->UpdateSampleList();
		return 1;
    }
    return CListCtrl::OnCommand(wParam, lParam);
}
