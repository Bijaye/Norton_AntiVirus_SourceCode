// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//
// dwLdPntScan.h - contains class definition
//  
/////////////////////////////////////////////////////////////////////////////

#ifndef _LDPNTSCAN_H_
#define _LDPNTSCAN_H_

#include "idefevnt.h"
#include "vpcommon.h"

#define szReg_Value_GUID                        "{083684A2-47AB-4839-A3B3-8109F4266B29}"

#define szReg_Val_ShowScanDialog                "ShowScanDialog"
#define szReg_Val_ScanAllDrives                 "ScanAllDrives"
#define szReg_Val_ScanForGreyware               "ScanForGreyware"
#define szReg_Val_ScanProcesses                 "ScanProcesses"

#define szReg_Key_ScheduleKey                   "Schedule"
#define szReg_Val_Enabled                       "Enabled"

// {083684A2-47AB-4839-A3B3-8109F4266B29}
DEFINE_GUID(CLSID_DefWatchLdPntScanEventHandler, 
0x83684a2, 0x47ab, 0x4839, 0xa3, 0xb3, 0x81, 0x9, 0xf4, 0x26, 0x6b, 0x29);


class CLoadPointScanHandler : public IDefWatchEventHandler
{
public:
    CLoadPointScanHandler();
    ~CLoadPointScanHandler();

    // 
    // IUnknown
    // 
    STDMETHOD( QueryInterface(REFIID, void**) );
    STDMETHOD_( ULONG, AddRef() );
    STDMETHOD_( ULONG, Release() );
    
    // 
    // IDefWatchEventHandler
    // 
    STDMETHOD( OnNewDefsInstalled( ) ) ;

private:
    BOOL (InitObjects());
    STDMETHOD (Cleanup());

    // Check to see if this quickscan is enabled.
    bool GetDefwatchQuickscanEnabled( void );


private:
    DWORD m_dwRef;

    IUtil4*         m_pUtil4;
    IScan2*         m_pScan;
    IScanConfig*    m_pScanConfig;
    IVirusProtect*  m_pRootCliProxy;
};







#endif
