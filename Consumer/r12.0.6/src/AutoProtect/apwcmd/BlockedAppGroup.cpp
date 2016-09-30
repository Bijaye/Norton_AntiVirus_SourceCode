#include "StdAfx.h"
#include "blockedappgroup.h"

// For cancelling scans
//
#import "navapsvc.tlb" 					// For COM interfaces to service.
#include "navapcommands.h"				// Commands for service.

DWORD CBlockedAppGroup::m_dwBlockedDelay = 0;

CBlockedAppGroup::CBlockedAppGroup(CEventData& EventData, CBlockedAppGroupSink* pBAGSink /*NULL*/)
{
    m_pUI = NULL;
    m_pBAGSink = pBAGSink;
    m_hCookie = NULL;
    m_lProcessID = 0;
    m_bClosingUI = false;

    if ( EventData.GetData ( AV::Event_Container_ProcessID, m_lProcessID ))
    {
        CCTRACEI ( _T("CBlockedAppGroup::CBlockedAppGroup creating processid=%u"), m_lProcessID );
    }

    // Store the cookie.
    //
    long lSize = sizeof (SAVRT_ROUS64);
    if ( EventData.GetData ( AV::Event_Container_Cookie, (BYTE*) &m_hCookie, lSize ) &&
         m_hCookie != NULL )
    {
        CCTRACEI ( _T("CBlockedAppGroup::CBlockedAppGroup creating cookie=%d"), m_hCookie );
    }

    // Store the file name - optional
    //
    EventData.GetData ( AV::Event_Container_ObjectName, m_strPath );

    m_timeSinceCreation.Start ();
}

CBlockedAppGroup::~CBlockedAppGroup(void)
{
    if ( !m_bClosingUI && m_pUI )
    {
        m_pUI->Close (true);
        m_bClosingUI = true;
    }
    
    if ( m_pUI )
    {
        delete m_pUI;
        m_pUI = NULL;
    }
}

void CBlockedAppGroup::AddDup ()
{
    // A new thread just blocked. Reset the timer for
    // the UI, if it's running.
    //
    if ( !m_bClosingUI && m_pUI )
    {
        m_pUI->timer.Reset ();
        m_pUI->timer.Start ();
    }
}

// Begin - UI Sink
//
void CBlockedAppGroup::OnBlockedAppUICancel()
{
    // Create object and call it.
    //
    // We need COM
    //
    m_bClosingUI = true;

    try
    {
        ccLib::CCoInitialize COM (ccLib::CCoInitialize::eMTAModel);

        // Cancel scans for all associated Event Data
        //
        HRESULT hr = E_FAIL;

        CEventData EventDataCancel;
        long lSize = sizeof (SAVRT_ROUS64);
        EventDataCancel.SetData ( AV::Event_Container_Cookie, (BYTE*)&m_hCookie, lSize );

        // Gather the event data from this class and it's parents.
        // All we *really* need is the cookie, but man this makes
        // it easy.
        //
        VARIANT v;

        // Initialize the variant data.
	    v.vt = VT_ARRAY | VT_UI1;
	    v.parray = EventDataCancel.Serialize ();
	    _ASSERT( v.parray );
	    NAVAPSVCLib::INAVAPServicePtr pNAVAPSVC( __uuidof( NAVAPSVCLib::NAVAPService ) );
	    _bstr_t sCommand( NAVAPCMD_CANCELSCAN );
	    hr = pNAVAPSVC->SendCommand( sCommand, v );
        
        if ( SUCCEEDED(hr))
            CCTRACEI (_T("CBlockedAppUI::cancelScan - Scan cancelled OK"));
    }
    catch (...)
    {
        CCTRACEE (_T("CBlockedAppUI::cancelScan"));
    }

    return;
}

// UI is closing (decontructor)
//
void CBlockedAppGroup::OnBlockedAppUIClosed()
{
    if (m_pBAGSink)
    {
        m_bClosingUI = true;

        // If we are closing because the user clicked, not
        // because the BAG owner deleted us.
        //
        m_pBAGSink->OnBAGUIClosed(this);
    }
}
//
// End - UI Sink

bool CBlockedAppGroup::Show ()
{
    // Show UI - Check all parameters before creating the object.
    //
    if ( !m_pUI && 
         m_hCookie != NULL &&
         m_lProcessID != 0 &&
         !m_strPath.empty() )
    {
        DWORD dwElapsedTime = m_timeSinceCreation.Stop ();
        DWORD dwWait = 0;

        if ( dwElapsedTime >= 0 && dwElapsedTime < m_dwBlockedDelay )
        {
            dwWait = m_dwBlockedDelay - dwElapsedTime;
        }           

        CCTRACEI ( _T("CBlockedAppGroup::Show - Showing UI in %d"), dwWait);
        m_pUI = new CBlockedAppUI (m_hCookie, m_lProcessID, m_strPath, dwWait, this /*sink*/ );
        m_pUI->Create (NULL, 0, 0);
        return true;
    }

    return false;
}

void CBlockedAppGroup::SetWaitBeforeDisplay ( DWORD dwBlockedDelay )
{
    m_dwBlockedDelay = dwBlockedDelay;
}

// If the cookie matches remove the BAG. If the process doesn't match it's OK.
//
bool CBlockedAppGroup::Remove ( /*in*/ CEventData& EventData )
{
    SAVRT_ROUS64 hRemoveCookie;
    long lSize = sizeof (SAVRT_ROUS64);
    
    if ( !EventData.GetData ( AV::Event_Container_Cookie, (BYTE*) &hRemoveCookie, lSize ))
        return false;

    CCTRACEI (_T("CBlockedAppGroup::Remove - Searching for %d"), hRemoveCookie);

    CCTRACEI (_T("CBlockedAppGroup::Remove - comparing %d"), m_hCookie );

    if ( 0 == memcmp ( &m_hCookie, &hRemoveCookie , lSize ))
    {
        // Matched. Remove the entry
        //
        if ( m_pUI )
        {
            m_pUI->Close (false);     // Close the UI, if it's running.
            m_bClosingUI = true;
        }
        CCTRACEI (_T("CBlockedAppGroup::Remove - Removing - %d"), m_hCookie);
        return true;
    }

    // Can't find it
    return false;
}

// *********************************
//
// Comparision operators
//
//
// Equality
//
bool CBlockedAppGroup::operator==(const CBlockedAppGroup& BAG) const
{
    return ( m_lProcessID == BAG.m_lProcessID && m_hCookie == BAG.m_hCookie );
}
// End comparison operators
//
// ***************************
