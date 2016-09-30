/*******************************************************************
*                                                                  *
* File name:     RcFrame.cpp                                       *
*                                                                  *
* Description:   Main window for the rc application                *
*                                                                  *
*                                                                  *
* Statement:     Licensed Materials - Property of IBM              *
*                (c) Copyright IBM Corp. 1999                      *
*                                                                  *
* Author:        Alla Segal                                        *
*                                                                  *
*                U.S. Government Users Restricted Rights - use,    *
*                duplication or disclosure restricted by GSA ADP   *
*                Schedule Contract with IBM Corp.                  *
*                                                                  *
*                                                                  *
********************************************************************
*                                                                  *
* Function:      Implements main frame class                       *
*                                                                  *
*******************************************************************/

#include "stdafx.h"
#include "RcFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TMyFrame

//IMPLEMENT_DYNCREATE(RcFrame, CFrameWnd)

RcFrame::RcFrame()
{
	  //  gMainBmp = NULL;
        RECT tRect;
        tRect.top = tRect.left = 0;
        tRect.right = 1;
        tRect.bottom = 1;
        CreateEx(0, AfxRegisterWndClass(0), "", WS_POPUP |WS_VISIBLE,
                   tRect.left,tRect.top,tRect.right, tRect.bottom, NULL, NULL);
}


BEGIN_MESSAGE_MAP(RcFrame, CWnd)
        //{{AFX_MSG_MAP(AvFrame)
         //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TMyFrame message handlers


