#pragma once

#include "eventdata.h"

class CNotifyAPSpywareDetections
{
public:
    CNotifyAPSpywareDetections(void);
    ~CNotifyAPSpywareDetections(void);

    void Initialize(bool bEmail);
    void Notify(IScanInfection* pInfection);
    void ScanComplete();

protected:
    bool m_bShouldSend;
    long m_lCookie;

    bool ConvertToWideStr ( LPCSTR lpOrig /*in*/, UINT nCodePage /*in*/, ATL::CAtlStringW& strWide /*out*/ );
    std::set<ATL::CAtlStringW> setAPFilesAlreadyNotified;

    bool SendEvent(CEventData* pEvent);
    bool IsAPServiceUp();
};
