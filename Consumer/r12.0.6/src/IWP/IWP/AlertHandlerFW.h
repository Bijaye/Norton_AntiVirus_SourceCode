#pragma once

#include "ccSerializableEventEx.h"
#include "SymNetDrvAlertEventEx.h"
#include "symfwagtinterface.h"
#include "ProtocolHelper.h"
#include "AlertUI.h"
#include "SymNetDriverApi.h"


class CAlertHandlerFW
{
public:
    CAlertHandlerFW(void);
    virtual ~CAlertHandlerFW(void);

protected:
    int displayCommonAlert() throw ();
    void commonInit (void);

    static CProtocolHelper m_protocols;
    void formatPort ( std::string& strPortOut/*in/out*/, DWORD dwPortIn, int iProtocol );
    void getICMPCommandName ( std::string& strCommandOut/*in/out*/, DWORD dwCommandIn );
    bool isNetBIOS ( DWORD dwLocalPort, DWORD dwRemotePort );
    bool getAppRealName ( const std::string& strAppFullPath /*in*/, std::string& strAppRealName /*out*/ );
    void launchScan ( LPCSTR lpszFilePath );

    CAlertUI m_alert;
    std::string m_strDescription;
    std::string m_strAppName;
    SymNeti::CSNAlertEventEx* m_pSNEvent;
    IAlertData::ALERT_TYPE m_eAlertType;
    bool m_bAppIsKnown;    // Did we get a good app name from the ALEs or are we guessing?
    std::string m_strFilePath;
};
