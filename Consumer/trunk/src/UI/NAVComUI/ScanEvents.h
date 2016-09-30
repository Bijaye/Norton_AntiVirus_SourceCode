////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Author:  Chirstopher Brown 11/08/2004

#pragma once
#include "StahlSoft.h"
#include "ccEraserInterface.h"
#include "EZEraserObjectsW.h"

const DWORD UM_SETACTIVE = WM_USER+500; 
const DWORD UM_KILLACTIVE = WM_USER+501; 
const DWORD UM_GET_HELP_ID = WM_USER+502; 

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
class CScanEventsSink
{
public:

    virtual HRESULT OnAbortScan() = 0;
    virtual HRESULT OnResumeScan() = 0;
    virtual HRESULT OnPauseScan() = 0;
    virtual HRESULT OnHelp( const long lHelpID ) = 0;

    // Methos used to event when the UI is ready for action...
    virtual HRESULT OnReady() = 0;

    // Results view...
    virtual HRESULT OnFinished() = 0;
    virtual HRESULT OnDetails() = 0;

    // Manual remove view
    virtual HRESULT OnManualRemoval() = 0;

    // Repair / Remove view...
    virtual HRESULT OnRepairRemoveCancel() = 0;
    virtual HRESULT OnRepairRemoveApply() = 0;

    // Methods used to poll for the latest scan counts...
    virtual HRESULT OnGetCurrentFolder(LPTSTR szFolder, long nLength) = 0;
    virtual HRESULT OnGetScanFileCount(long& nScanFileCount) = 0;
    virtual HRESULT OnGetVirusCount(long& nVirusCount) = 0;
    virtual HRESULT OnGetRemovedVirusCount(long& nRemovedVirusCount) = 0;
    virtual HRESULT OnGetNonViralCount(long& nNonViralCount) = 0;
    virtual HRESULT OnGetRemovedNonViralCount(long& nRemovedNonViralCount) = 0;
    virtual HRESULT OnGetQuarantineCount(long& nQurantineCount) = 0;
    virtual HRESULT OnGetAnomalyList( ccEraser::IAnomalyListPtr& pAnomalyList ) = 0;
    virtual HRESULT OnGetAdditionalScan(bool& bAdditionalScan) = 0;

    virtual HRESULT OnRepairProgressComplete() = 0;
    virtual HRESULT OnGetVirusDefSubscriptionExpired(bool& bVal) = 0;
    virtual HRESULT OnGetAppLauncher( IAppLauncher** obj ) = 0;
    virtual HRESULT OnRiskDetailsDialog( CEZAnomaly & ezAnomaly,
                                         bool bRiskAssessment ) = 0;
 
    virtual HRESULT OnIsEmailScan( bool& bVal ) = 0;
    virtual HRESULT OnGetEmailValues( LPCTSTR* szEmailInfoSubject, 
                                      LPCTSTR* szEmailInfoSender, 
                                      LPCTSTR* szEmailInfoRecipient ) = 0;
    virtual HRESULT OnIsQuickScan( bool& bVal ) = 0;

};


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CScanEventsSource : public StahlSoft::CEventSourceT< CScanEventsSink* >
{
public:

    virtual HRESULT Event_OnAbortScan()
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnAbortScan();

        return E_FAIL;
     }

    virtual HRESULT Event_OnResumeScan()
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnResumeScan();

        return E_FAIL;
    }

    virtual HRESULT Event_OnPauseScan()
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnPauseScan();

        return E_FAIL;
    }

    virtual HRESULT Event_OnHelp( const long lHelpID )
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnHelp( lHelpID );

        return E_FAIL;
    }

    virtual HRESULT Event_OnReady()
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnReady();

        return E_FAIL;
    }

    virtual HRESULT Event_OnFinished()
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnFinished();

        return E_FAIL;
    }

    virtual HRESULT Event_OnDetails()
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnDetails();

        return E_FAIL;
    }

    virtual HRESULT Event_OnManualRemoval()
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnManualRemoval();

        return E_FAIL;
    }

    virtual HRESULT Event_OnRepairRemoveCancel()
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnRepairRemoveCancel();

        return E_FAIL;
    }

    virtual HRESULT Event_OnRepairRemoveApply()
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnRepairRemoveApply();

        return E_FAIL;
    }

    virtual HRESULT Event_OnGetCurrentFolder(LPTSTR szFolder, long nLength)
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnGetCurrentFolder(szFolder,nLength);

        return E_FAIL;
    }

    virtual HRESULT Event_OnGetScanFileCount(long& nScanFileCount)
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnGetScanFileCount(nScanFileCount);

        return E_FAIL;
    }

    virtual HRESULT Event_OnGetVirusCount(long& nVirusCount)
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnGetVirusCount(nVirusCount);

        return E_FAIL;
    }

    virtual HRESULT Event_OnGetRemovedVirusCount(long& nRemovedVirusCount)
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnGetRemovedVirusCount(nRemovedVirusCount);

        return E_FAIL;
    }

    virtual HRESULT Event_OnGetNonViralCount(long& nNonViralCount)
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnGetNonViralCount(nNonViralCount);

        return E_FAIL;
    }

    virtual HRESULT Event_OnGetAdditionalScan(bool& bAdditionalScan)
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnGetAdditionalScan(bAdditionalScan);

        return E_FAIL;
    }

    virtual HRESULT Event_OnGetRemovedNonViralCount(long& nRemovedNonViralCount)
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnGetRemovedNonViralCount(nRemovedNonViralCount);

        return E_FAIL;
    }

    virtual HRESULT Event_OnGetQuarantineCount(long& nQurantineCount)
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnGetQuarantineCount(nQurantineCount);

        return E_FAIL;
    }

    virtual HRESULT Event_OnGetAnomalyList( ccEraser::IAnomalyListPtr& pAnomalyList )
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnGetAnomalyList(pAnomalyList);

        return E_FAIL;
    }

    virtual HRESULT Event_OnRepairProgressComplete()
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnRepairProgressComplete();

        return E_FAIL;
    }

    virtual HRESULT Event_OnGetVirusDefSubscriptionExpired(bool& bVal)
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnGetVirusDefSubscriptionExpired( bVal );

        return E_FAIL;
    }

    virtual HRESULT Event_OnGetAppLauncher( IAppLauncher** obj )
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnGetAppLauncher(obj);

        return E_FAIL;
    }
    
    virtual HRESULT Event_OnRiskDetailsDialog( CEZAnomaly & ezAnomaly,
                                               bool bRiskAssessment )
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnRiskDetailsDialog( ezAnomaly, bRiskAssessment );

        return E_FAIL;
    }
    

    virtual HRESULT Event_OnIsEmailScan( bool& bVal )
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnIsEmailScan( bVal );

        return E_FAIL;

    }

    virtual HRESULT Event_OnGetEmailValues( LPCTSTR* szEmailInfoSubject, LPCTSTR* szEmailInfoSender, LPCTSTR* szEmailInfoRecipient )
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnGetEmailValues( szEmailInfoSubject, szEmailInfoSender, szEmailInfoRecipient );

        return E_FAIL;
    }

    virtual HRESULT Event_OnIsQuickScan( bool& bVal )
    {
        iterator it = begin(); 
        if( it != end() )
            return (*it)->OnIsQuickScan( bVal );

        return E_FAIL;
    }

};
