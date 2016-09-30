#pragma once

#include "stdafx.h"

#include "ccLogViewerEventFactory.h"

#include "SNLogEventFactory.h"
#include "SNLogLogViewerSymNetiCategory.h"
#include "ccEventId.h"
#include "sndEventId.h"


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
        ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
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
        ccLib::CSingleLock csl(&m_Crit, INFINITE, FALSE);
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


class CLogViewerEventFactory : public cc::ILogViewerEventFactory,
                               public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    CLogViewerEventFactory(void);
    virtual ~CLogViewerEventFactory(void);

private:
    CLogViewerEventFactory& operator=(const CLogViewerEventFactory&);
    CLogViewerEventFactory(const CLogViewerEventFactory&);

public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(cc::IID_ILogViewerEventFactory, ILogViewerEventFactory)
    SYM_INTERFACE_MAP_END()


public:
    // ILogViewerEventFactory methods
    virtual HRESULT Create(ccEvtMgr::CEventEx* pEvent, cc::ILogViewerEvent*& pILogViewerEvent) throw();
    virtual HRESULT Destroy(cc::ILogViewerEvent* pILogViewerEvent) throw();

protected:
    CSymStaticRefCount<CLogViewerSymNetiCategoryHelper> m_LogViewerHelper;
};
