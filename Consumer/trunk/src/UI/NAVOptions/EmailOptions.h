////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/************************************************************************/
/* NAV Email Options Object                                             */
/************************************************************************/

#ifndef __NAVOPTIONS_EMAILOPTIONS_H__
#define __NAVOPTIONS_EMAILOPTIONS_H__

#include "stdafx.h"
#include "NAVSettingsCache.h"
#include "EmailProxyInterface.h"
#include "AVInterfaces.h"
#include "AVInterfaceLoader.h"

class CEmailOptions
{
public:
    CEmailOptions();
    ~CEmailOptions();
    
    // CORE OPTIONS INTERFACE BEGIN
    HRESULT Initialize();
    HRESULT Save();
    HRESULT Default();
    HRESULT Notify();
    bool IsDirty();
    // CORE OPTIONS INTERFACE END

    // Specific Options
    HRESULT GetTimeoutProtection(DWORD& bProtect);  // Provider: IEmailOptions3
    HRESULT SetTimeoutProtection(DWORD bProtect);
    HRESULT GetShowTrayIcon(DWORD& bShow);          // Provider: IEmailOptions3
    HRESULT SetShowTrayIcon(DWORD bShow);
    HRESULT GetShowProgressOut(DWORD& bShow);       // Provider: IEmailOptions3
    HRESULT SetShowProgressOut(DWORD bShow);

	// IAVEmail Options
	HRESULT GetScanIncoming(DWORD &dwScan);
	HRESULT SetScanIncoming(const DWORD dwScan);
	HRESULT GetScanOutgoing(DWORD &dwScan);
	HRESULT SetScanOutgoing(const DWORD dwScan);
	HRESULT GetScanOEH(DWORD &dwScan);
	HRESULT SetScanOEH(const DWORD dwScan);

private:
    bool m_bInitialized;
    bool m_bDirty;

    // Used for email proxy options
    HINSTANCE m_hccEmlPxy;
    IEmailOptions3Ptr m_pEmailOptions;
    bool m_bEmlTimeout, m_bEmlProgress, m_bEmlTray;
	DWORD m_dwScanIncoming, m_dwScanOutgoing, m_dwOEH;
	AVModule::AVLoader_IAVEmail m_AVEmailLoader;
	AVModule::IAVEmailPtr m_spAVEmail;
};

#endif // __NAVOPTIONS_EMAILOPTIONS_H__