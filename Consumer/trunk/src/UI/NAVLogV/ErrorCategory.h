////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// CErrorCategory.h: interface for the CErrorCategory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CERRORCATEGORY_H__79C39F19_5B3B_45D9_B498_11A31AB0BE1F__INCLUDED_)
#define AFX_CERRORCATEGORY_H__79C39F19_5B3B_45D9_B498_11A31AB0BE1F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

#include "ccLogViewerInterface.h"
#include "NAVLogV.h"
#include "AllNAVEvents.h"

#include "ccLogManagerHelper.h"

#include "AVCategory.h"
#include "AVNamesMap.h"

#include "TechSupportURL.h"
#include <string>

class CErrorCategory :	
    public CAVCategory
{
                           
public:
	CErrorCategory();
	virtual ~CErrorCategory();

protected:
    enum // Columns
    {
        columnDate = 0,
        columnFeature,
        columnMessage,
        columnErrorCode,
        columnProductVersion,
        columnUserName,
        columnComputerName,
        columnLast          // always the last
    };

    BOOL SetEventProperties(ccEvtMgr::CEventEx* pCEvent,
                            const ccEvtMgr::CLogFactoryEx::EventInfo& Info,
                            cc::ILogViewerEvent* pLogViewerEvent);
    
    CTechSupportURL m_TechSupport;
    tstring m_strErrorLink;
};

#endif // !defined(AFX_ERRORCATEGORY_H__79C39F19_5B3B_45D9_B498_11A31AB0BE1F__INCLUDED_)
