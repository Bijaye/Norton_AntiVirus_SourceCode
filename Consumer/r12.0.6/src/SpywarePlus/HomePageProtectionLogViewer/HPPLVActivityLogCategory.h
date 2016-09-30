#pragma once

#include "ccLogViewerCategoryHelper.h"
#include "HPPLVInterface.h"

#include "HPPEventsInterface.h"
#include "HPPEventHelperLoader.h"

class CHPPLVActivityLogCategory : public ccEvtMgr::CLogViewerCategoryHelper
{
public:
    CHPPLVActivityLogCategory(void);
    virtual ~CHPPLVActivityLogCategory(void);

public:
    BOOL CreateEx();

    virtual HRESULT GetImageList(HIMAGELIST& hImageList);
    virtual HRESULT GetColumnCount(int& iColumnCount);
    virtual HRESULT GetCategoryID(int& iCategoryID);
    virtual HRESULT LaunchHelp();

protected:
    virtual HRESULT GetCategoryName(HMODULE& hModule, 
        UINT& nId);
    virtual HRESULT GetCategoryDescription(HMODULE& hModule, 
        UINT& nId);
    virtual HRESULT GetColumnName(int iColumn, 
        HMODULE& hModule, 
        UINT& nId);
    virtual HRESULT CopyEvent(const ccEvtMgr::CEventEx& Event, 
        const ccEvtMgr::CLogFactoryEx::EventInfo& Info,
        cc::ILogViewerEvent*& pLogViewerEvent);

    enum{ CategoryId = CC_HPPLV_CATID_ACTIVITY };
    
    HPP::HPPEventFactory_CEventFactoryEx m_EventFactoryLoader;

    static const UINT m_ColNameIds[];
};
