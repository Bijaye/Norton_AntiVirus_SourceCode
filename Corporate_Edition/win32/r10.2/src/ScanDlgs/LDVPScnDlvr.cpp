// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// LDVPScnDlvr.cpp: implementation of the CLDVPScnDlvr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "scandlgs.h"
#include "ResultsView.h"
#include "resultitem.h"
#include "LDVPScnDlvr.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLDVPScnDlvr::CLDVPScnDlvr()
{
    m_resultsview = NULL;
}


CLDVPScnDlvr::CLDVPScnDlvr(CResultsView *ResultsView)
{
    m_resultsview = ResultsView;
}


CLDVPScnDlvr::~CLDVPScnDlvr()
{

}


DWORD CLDVPScnDlvr::MakeScnDlvrItems()
{
    DWORD           dwData=0;
	long            iIndex=0;
    int             iNumSelectedItems = 0;
    int             iNumItemsToSend = 0;
    CResultItem*    pResultItem = NULL;

    iNumSelectedItems = m_resultsview->m_oLDVPResultsCtrl.GetSelectedItemCount();

	for (int i=0;i<iNumSelectedItems;i++) 
	{
		CString logline = m_resultsview->m_oLDVPResultsCtrl.EnumSelectedItems(&iIndex,(long*)&dwData);
		
		if (logline.IsEmpty())
			break;

        iNumItemsToSend++;

		try
		{
			// get info from the log line
			CResultItem* pResultItem = new CResultItem(logline,0);
		}
		catch (std::bad_alloc &){}

        if (m_resultsview->m_GetVBinData)
        {
            m_resultsview->m_GetVBinData(m_resultsview->m_Context,0,NULL,NULL);
        }

        logline.Empty();
        delete pResultItem;
    }    

    return (iNumItemsToSend);
}


DWORD CLDVPScnDlvr::SendVBinFilesToSARC()
{
    return (0);
}
