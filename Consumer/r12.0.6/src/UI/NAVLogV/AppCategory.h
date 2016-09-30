// AppCategory.h: interface for the CAppCategory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPCATEGORY_H__40AB20C3_1C7E_4224_9E5C_E1863F5816BD__INCLUDED_)
#define AFX_APPCATEGORY_H__40AB20C3_1C7E_4224_9E5C_E1863F5816BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"

#include "resourcehelper.h"
#include "AVCategory.h"
#include "AVNamesMap.h"

class CAppCategory : public CAVCategory 
{
public:
	CAppCategory();
	virtual ~CAppCategory();

protected:
    enum // Columns
    {
        columnDate = 0,
        columnFeature,
        columnAction,
        columnDetails,
        columnUserName,
        columnComputerName,
        columnLast          // always the last
    };

    BOOL SetEventProperties(ccEvtMgr::CEventEx* pCEvent,
                            const ccEvtMgr::CLogFactoryEx::EventInfo& Info,
                            cc::ILogViewerEvent* pLogViewerEvent);

};

#endif // !defined(AFX_APPCATEGORY_H__40AB20C3_1C7E_4224_9E5C_E1863F5816BD__INCLUDED_)
