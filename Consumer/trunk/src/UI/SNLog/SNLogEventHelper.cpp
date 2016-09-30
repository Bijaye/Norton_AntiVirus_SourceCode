////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "SNLogEventFactory.h"
#include "SNLogLogViewerSymNetiCategory.h"
#include "sndEventId.h"

#include "ccEventId.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

using namespace cc;
using namespace ccEvtMgr;
using namespace ccLib;

extern "C"
{
// We use the CLogViewerSymNetiCategory class to handle the wrapping of events for us
class CLogViewerSymNetiCategoryHelper : public CLogViewerSymNetiCategory
{
public:
    CLogViewerSymNetiCategoryHelper() : CLogViewerSymNetiCategory(CC_SYMNETDRV_EVENT_LOG_IDS, 0, 0)
    {
        m_nUsageCount = 0;
    }
    virtual ~CLogViewerSymNetiCategoryHelper() {}
private:
    CLogViewerSymNetiCategoryHelper(const CLogViewerSymNetiCategoryHelper&);
    CLogViewerSymNetiCategoryHelper& operator =(const CLogViewerSymNetiCategoryHelper&);

public:
    virtual BOOL Create()
    {
        CSingleLock csl(&m_Crit, INFINITE, FALSE);
        if (m_nUsageCount <= 0)
        {
            if (CLogViewerSymNetiCategory::Create() == FALSE)
            {
                return FALSE;
            }
        }
        m_nUsageCount ++;
        return TRUE;
    }
    virtual void Destroy()
    {
        CSingleLock csl(&m_Crit, INFINITE, FALSE);
        if (m_nUsageCount <= 0)
        {
            return;
        }
        if (m_nUsageCount == 1)
        {
            CLogViewerSymNetiCategory::Destroy();
        }
        m_nUsageCount --;
    }

protected:
    long m_nUsageCount;
    ccLib::CCriticalSection m_Crit;
};

} // extern "C"