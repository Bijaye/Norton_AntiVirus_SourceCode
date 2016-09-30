////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <SymInterface.h>
#include <AVInterfaces.h>

namespace avScanUI
{
//
// Internal use IIDs
//
// {8E7ACF17-3B46-4193-B975-8E639EC2FE89}
SYM_DEFINE_INTERFACE_ID(IID_ScanUICallbacksInt,
                        0x8e7acf17, 0x3b46, 0x4193, 0xb9, 0x75, 0x8e, 0x63, 0x9e, 0xc2, 0xfe, 0x89);

//////////////////////////////////////////////////////////////////////////
// Internal use data
//////////////////////////////////////////////////////////////////////////
struct ScanStatistics
{
    int nTotalScanned;
    int nVirDetected;
    int nVirRemoved;
    int nNonVirDetected;
    int nNonVirRemoved;
    int nScanTime;
    int nScanType;
    int nScanResult;

    // Individual stats (from eraser)
    int nRegistryItems;
    int nFileItems;
    int nProcessItems;
    int nBatchItems;
    int nINIItems;
    int nServiceItems;
    int nStartupItems;
    int nCOMItems;
    int nHostsItems;
    int nDirectoryItems;
    int nLSPItems;
    int nBrowserCacheItems;
    int nCookieItems;
    int nApplicationHeuristicItems;
};

//////////////////////////////////////////////////////////////////////////
// Internal use interfaces
//////////////////////////////////////////////////////////////////////////

///////////////////////////////////////
// IQBackupItemBaseInt interface:
class IScanUICallbacksInt : public ISymBase
{
public:
    virtual void RefreshThreatLists() = 0;
    virtual void SetScanStats(const ScanStatistics& scanStats) = 0;
    virtual void GetScanStats(ScanStatistics& scanStats, bool bRaw = false) = 0;
	virtual void UpdateScanStats() = 0;
    virtual HRESULT ShowError(DWORD dwErrorId, DWORD dwResId) = 0;
    virtual HRESULT LaunchHelp(DWORD dwHelpID, HWND hParentWnd) = 0;
    virtual HRESULT SwitchToTab(LPCSTR pszTabID) = 0;
    virtual HRESULT LaunchMsgCenter() = 0;
	virtual HRESULT GetScanTitle(cc::IString*& pScanTitleString) = 0;
    virtual HRESULT GetScanInstanceId(REFGUID guidScanInstanceId) = 0;

};
typedef CSymPtr<IScanUICallbacksInt> IScanUICallbacksIntPtr;
typedef CSymQIPtr<IScanUICallbacksInt, &IID_ScanUICallbacksInt> IScanUICallbacksIntQIPtr;


} // end avScanUI namespace
