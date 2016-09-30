#include "StdAfx.h"
#include ".\idsmanager.h"
#include "build.h"				// for our version information

#include "IDSLoaderLink.h"          // Links to IDS libs

#define IDS_HAWKING_SUBDIR "IDS-Diskless"

using namespace SymIDS;

CIDSManager::CIDSManager(void)
{
    // Use NAV's internal values as the defaults
    //
	m_ui64LocalVersion = (VER_NUM_PRODVERMAJOR << 48) + 
						 (VER_NUM_PRODVERMINOR << 32) + 
						 (VER_NUM_PRODVERSUBMINOR << 16) + 
						 (VER_NUM_BUILDNUMBER);

    m_strLocalProductName = "NAV_IWP";
}

CIDSManager::~CIDSManager(void)
{
    CCTRACEI ( "CIDSManager::~CIDSManager");
}

SYMRESULT CIDSManager::ResetIDSSettings(LPCSTR lpszProductName, UINT64 ui64ProductVersion)
{
	CCTRACEI("CIDSManager::ResetIDSSettings()");
    
	CSymPtr<SymIDS::IIDSFactorySettings> pIDSFactorySettings;
	SYMRESULT sr = m_IDSLoader.GetObject ( IID_IDSFactorySettings, (ISymBase**) &pIDSFactorySettings );

	if(SYM_FAILED(sr) || pIDSFactorySettings == NULL)
	{
		CCTRACEE("CIDSManager::ResetIDSSettings(): CreateObject() == 0x%x, Unable to load IIDSFactorySettings object.", sr);
		return SYMERR_UNKNOWN;
	}
			
    // Product name
    std::string strTempProductName;
    UINT64 ui64TempVersion = 0;

    if (lpszProductName)
        strTempProductName = lpszProductName;
    else
        strTempProductName = m_strLocalProductName;

    // Product version
    if ( ui64ProductVersion )
        ui64TempVersion = ui64ProductVersion;
    else
        ui64TempVersion = m_ui64LocalVersion;

	CCTRACEI("CIDSManager::ResetIDSSettings(): Initializing IDSFactorySettings with: ProductName = %s", strTempProductName.c_str());
    
    sr = pIDSFactorySettings->Initialize(strTempProductName.c_str(), ui64TempVersion);				
	if(SYM_FAILED(sr))
	{
		CCTRACEE("CIDSManager::ResetIDSSettings(): pIDSFactorySettings->Initialize == 0x%x.", sr);
		return SYMERR_UNKNOWN;		
	}
		
	sr = pIDSFactorySettings->Reset();
	if(SYM_SUCCEEDED(sr))
	{
		CCTRACEI("CIDSManager::ResetIDSSettings(): pIDSFactorySettings->Reset() Succeeded.");
		return SYM_OK;
	}
	else
	{
		CCTRACEE("CIDSManager::ResetIDSSettings(): pIDSFactorySettings->Initialize == 0x%x.", sr);
		return SYMERR_UNKNOWN;
	}

	// shouldn't get here
	return SYMERR_UNKNOWN;
}

// Init our internal IDS Subscription object.
//
SYMRESULT CIDSManager::initSubscriptions ()
{
    CCTRACEI ( "CIDSManager::initSubscriptions - starting");

    if ( !m_pIDSSub.m_p )
    {
        SYMRESULT result = m_IDSLoader.GetObject ( IID_IDSSubscription, (ISymBase**) &m_pIDSSub );

        if ( SYM_FAILED (result))
        {
            CCTRACEE ( "CIDSManager::initSubscriptions - Failed to load IDS - 0x%x", result );
            return result;
        }
        
        result = m_pIDSSub->Initialize ();
        if ( SYM_FAILED (result))
        {
            CCTRACEE ( "CIDSManager::initSubscriptions - Failed to init IDS sub - 0x%x", result );
            return result;
        }
    }
    CCTRACEI ( "CIDSManager::initSubscriptions - done");

    return SYM_OK;
}

SYMRESULT CIDSManager::Subscribe (time_t timeExpiredDateTime)
{
    CCTRACEI ( "CIDSManager::Subscribe - starting");

    // Tell IDS the current state
    //
    SYMRESULT result = SYMERR_UNKNOWN;
    result = initSubscriptions();

    if ( SYM_FAILED (result))
        return result;

    CCTRACEI ( "CIDSManager::Subscribe - SetExpirationDate");
    result = m_pIDSSub->SetExpirationDate (m_strLocalProductName.c_str(), timeExpiredDateTime);
    if ( SYM_FAILED (result))
    {
        // A failure of 0x8001022c before CfgWiz is OK, we aren't a registered product yet. 
        CCTRACEE ( "CIDSManager::Subscribe - Failed to SetExpirationDate - 0x%x", result );
        return result;
    }

    
    CCTRACEI ( "CIDSManager::Subscribe - SetTamperedStatus");
    result = m_pIDSSub->SetTamperedStatus (m_strLocalProductName.c_str(), FALSE);

    if ( SYM_FAILED (result))
    {
        CCTRACEE ( "CIDSManager::Subscribe - Failed to SetTamperedStatus - 0x%x", result );
        return result;
    }

    CCTRACEI ( "CIDSManager::Subscribe - done");

    return SYM_OK;
}

SYMRESULT CIDSManager::Unsubscribe ()
{
    // Tell IDS the current state
    //
    SYMRESULT result = SYMERR_UNKNOWN;
    result = initSubscriptions();

    if ( SYM_FAILED (result))
        return result;

    result = m_pIDSSub->UnSubscribe ( m_strLocalProductName.c_str ());

    if ( SYM_FAILED (result))
        CCTRACEE ( "CIDSManager::Unsubscribe - Failed to unsubscribe to IDS - 0x%x", result );

    return result;
}

SYMRESULT CIDSManager::Enable (bool bOn)
{
    CCTRACEI ( "CIDSManager::Enable - starting");

    // Tell IDS the current state
    //
    CSymPtr<SymIDS::IIDSGlobalSettings> pIDSGlobal;

    SYMRESULT result = m_IDSLoader.GetObject ( IID_IDSGlobalSettings, (ISymBase**) &pIDSGlobal );
    if ( SYM_FAILED (result))
    {
        CCTRACEE ( "CIDSManager::Enable - Failed to load IDS - 0x%x", result );
        return result;
    }

    // Fragmented packet blocking is not supported in IWP. 1-2V165Z
    //
    bool bReturn = pIDSGlobal->SetSetting (SymIDS::IIDSGlobalSettings::SETTING_ID_ENABLED, (DWORD) bOn );
    if ( !pIDSGlobal->SetSetting (SymIDS::IIDSGlobalSettings::SETTING_ID_FRAGMENTED_PKT, SymIDS::FRAG_ACTION_PERMIT ))
        CCTRACEE ( "CIDSManager::Enable - failed setting fragmented packet action");

    CCTRACEI ( "CIDSManager::Enable - done");
    return bReturn;
}
