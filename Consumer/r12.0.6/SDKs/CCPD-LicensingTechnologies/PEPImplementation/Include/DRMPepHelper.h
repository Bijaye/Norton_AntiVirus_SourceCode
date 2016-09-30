#ifndef DRMPEPHELPER_H
#define DRMPEPHELPER_H

#pragma once

#include "Subscription_Static.h"
#include "drmnamedproperties.h"
#include "simon.h"
#include "stahlsoft.h"
#include "simoncollectiontoolbox.h"

/////////////////////////////////////////////////////////////////////////////////////////////
// Macros
/////////////////////////////////////////////////////////////////////////////////////////////

// DWORD_HAS_FLAG
// Tests a DWORD bit, usually used for disposition field
#define DWORD_HAS_FLAG(item,test) ((item&test)==test)


/////////////////////////////////////////////////////////////////////////////////////////
// PEP Event handlers
/////////////////////////////////////////////////////////////////////////////////////////
#define DECLARE_PEP_HANDLERS \
    namespace DRMPEP \
    { \
    HRESULT HandleQueryAboutBoxText(SIMON_COLLECTION::IDataCollection* pCollection); \
    HRESULT HandleQueryProductState(SIMON_COLLECTION::IDataCollection* pCollection); \
    HRESULT HandleActivateSubscription(SIMON_COLLECTION::IDataCollection* pCollection); \
    HRESULT HandleInitSubscription(SIMON_COLLECTION::IDataCollection* pCollection); \
    HRESULT HandleQuerySubscriptionRecord(SIMON_COLLECTION::IDataCollection* pCollection); \
    HRESULT HandleForceSubscriptionRenewal(SIMON_COLLECTION::IDataCollection* pCollection); \
    HRESULT HandleSubscriptionManagement(SIMON_COLLECTION::IDataCollection* pCollection); \
    HRESULT HandleQueryLicenseRecord(SIMON_COLLECTION::IDataCollection* pCollection); \
    }

// DRM_PEPHANDLER_ENTRIES
// Defines the standard DRM PEP handlers
#define DRM_PEPHANDLER_ENTRIES \
    PEPHANDLER_ENTRY(&DRMGUID, DRMPEP::POLICY_ID_QUERY_ABOUT_BOX_TEXT                ,DRMPEP::HandleQueryAboutBoxText) \
    PEPHANDLER_ENTRY(&DRMGUID, DRMPEP::POLICY_ID_QUERY_PRODUCT_STATE                 ,DRMPEP::HandleQueryProductState) \
    PEPHANDLER_ENTRY(&DRMGUID, DRMPEP::POLICY_ID_QUERY_SUBSCRIPTION_RECORD           ,DRMPEP::HandleQuerySubscriptionRecord) \
    PEPHANDLER_ENTRY(&DRMGUID, DRMPEP::POLICY_ID_ACTIVATE_SUBSCRIPTION               ,DRMPEP::HandleActivateSubscription) \
    PEPHANDLER_ENTRY(&DRMGUID, DRMPEP::POLICY_ID_REGISTER_SUBSCRIPTION               ,DRMPEP::HandleInitSubscription) \
    PEPHANDLER_ENTRY(&DRMGUID, DRMPEP::POLICY_ID_QUERY_SUBSCRIPTION_FORCE_RENEWAL    ,DRMPEP::HandleForceSubscriptionRenewal) \
    PEPHANDLER_ENTRY(&DRMGUID, DRMPEP::POLICY_ID_SUBSCRIPTION_MANAGEMENT             ,DRMPEP::HandleSubscriptionManagement) \
    PEPHANDLER_ENTRY(&DRMGUID, DRMPEP::POLICY_ID_QUERY_LICENSE_RECORD				 ,DRMPEP::HandleQueryLicenseRecord)

///////////////////////////////////////////////////////////////////////////
// DRMGUID = {D55106E7-07AD-4677-B4B2-E53238AB8D69}
// Context GUID for DRM PEP events
DEFINE_GUID(DRMGUID, 
            0xd55106e7, 0x7ad, 0x4677, 0xb4, 0xb2, 0xe5, 0x32, 0x38, 0xab, 0x8d, 0x69);

/////////////////////////////////////////////////////////////////////////////////////////////
// namespace DRMPEP
/////////////////////////////////////////////////////////////////////////////////////////////
namespace DRMPEP
{
    /////////////////////////////////////////////////////////////////////////////////////////////
    // Property names reserved for subscription
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Property name for subscription disposition   : Type: DWORD
    static const char* SZ_PROPERTY_DISPOSITION       = "DRM_PROPERTY_DISPOSITION";

    /////////////////////////////////////////////////////////////////////////////////////////
    // Policy IDs for DRM specific events
    /////////////////////////////////////////////////////////////////////////////////////////
    const DWORD POLICY_ID_QUERY_PRODUCT_STATE                   = 0xA0000001;
    const DWORD POLICY_ID_QUERY_SUBSCRIPTION_RECORD	            = 0xA0000002;
    const DWORD POLICY_ID_QUERY_SUBSCRIPTION_FORCE_RENEWAL	    = 0xA0000003;
    const DWORD POLICY_ID_ACTIVATE_SUBSCRIPTION	                = 0xA0000004;
    const DWORD POLICY_ID_REGISTER_SUBSCRIPTION	                = 0xA0000005;
    const DWORD POLICY_ID_QUERY_ABOUT_BOX_TEXT	                = 0xA0000006;
    const DWORD POLICY_ID_SUBSCRIPTION_MANAGEMENT               = 0xA0000007;
    const DWORD POLICY_ID_QUERY_LICENSE_RECORD					= 0xA0000008;

    /////////////////////////////////////////////////////////////////////////////////////////
    //DRM Dispositions
    /////////////////////////////////////////////////////////////////////////////////////////
    const DWORD DISPOSITION_NONE		                    = 0x00000000;
    const DWORD DISPOSITION_GET_SUBSCRIPTION_INFO		    = 0x00000001;
    const DWORD DISPOSITION_DO_NOT_UPDATE_CACHE     		= 0x00000002;
    const DWORD DISPOSITION_POSSIBLY_ONLINE            		= 0x00000004;
    const DWORD DISPOSITION_VERBOSE                    		= 0x00000008;
    const DWORD DISPOSITION_CALL_DIRECT                     = 0x00000008;
}// DRMPEP namespace

// Cache related functions
namespace DRMCACHE
{
// Cache Data Type
static const DWORD TYPE_DWORD				= 1;
static const DWORD TYPE_STRING				= 2;

// Cache category defines
static const DWORD CACHEDATATYPE_SUBSCRIPTION		= 1;
static const DWORD CACHEDATATYPE_LICENSE			= CACHEDATATYPE_SUBSCRIPTION << 1;
static const DWORD CACHEDATATYPE_ALL				= CACHEDATATYPE_SUBSCRIPTION | CACHEDATATYPE_LICENSE;

// DRM cache entry information map
typedef struct tagMAPValueData
{
	CString sValue;
	DWORD dwType;
} VALUEDATAPAIR, *LPVALUEDATAPAIR;


// Subscription values
static VALUEDATAPAIR g_SubValueList[] = 
{
	{SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DAYS_REMAINING, TYPE_DWORD},
	{SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_MAX_DAYS, TYPE_DWORD},
	{SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_WARNING_PERIOD, TYPE_DWORD},
	{SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION, TYPE_DWORD},
	{SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_END_DATE, TYPE_DWORD},
	{SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_BEGIN_DATE, TYPE_DWORD},
	{SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_SKU, TYPE_STRING},
	{SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_PAIDSKU, TYPE_STRING},
	{SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_PART_NUMBER, TYPE_STRING},
};

// Licensing values
static VALUEDATAPAIR g_LicValueList[] = 
{
	{DRM::szDRMLicenseType, TYPE_DWORD},
	{DRM::szDRMLicenseState, TYPE_DWORD},
	{DRM::szDRMLicenseZone, TYPE_DWORD},
	{DRM::szDRMLicenseVendorId, TYPE_DWORD},
	{DRM::szDRMLicenseVersion, TYPE_DWORD},
	{DRM::szDRMMaxUses, TYPE_DWORD},
	{DRM::szDRMRemainingUses, TYPE_DWORD},
	{DRM::szDRMMaxDays, TYPE_DWORD},
	{DRM::szDRMRemainingDays, TYPE_DWORD},
	{DRM::szDRMMaxMinutes, TYPE_DWORD},
	{DRM::szDRMRemainingMinutes, TYPE_DWORD},
	{DRM::szDRMDieDate, TYPE_DWORD},
	{DRM::szDRMActivationKey, TYPE_STRING},
	{DRM::szDRMActivationGUID, TYPE_STRING},
	{DRM::szDRMSCSSTrialMaxDays, TYPE_DWORD},
	{DRM::szDRMSCSSTrialRemainingDays, TYPE_DWORD},
	{DRM::szDRMSCSSGraceMaxDays, TYPE_DWORD},
	{DRM::szDRMSCSSGraceRemainingDays, TYPE_DWORD},
	{DRM::szDRMSCSSEarlyRenewMaxDays, TYPE_DWORD},
	{DRM::szDRMSCSSEarlyRemainingDays, TYPE_DWORD},
	{DRM::szDRMSCSSRenewMaxDays, TYPE_DWORD},
	{DRM::szDRMSCSSRenewRemainingDays, TYPE_DWORD},
	{DRM::szDRMSCSSCancellationCode, TYPE_DWORD},
	{DRM::szDRMSCSSStartupMode, TYPE_DWORD},
	{DRM::szDRMSCSSUpsellUrl, TYPE_STRING},
	{DRM::szDRMSCSSResolveUrl, TYPE_STRING},
};
} // DRMCACHE namespace

// PEP Related helper function

/////////////////////////////////////////////////////////////////////////////////////////////
// Function ProcessSubscriptionDates
//
// Description:
// Calculates subscription dates based on the days remaining in the subscription.
// 
/////////////////////////////////////////////////////////////////////////////////////////////
inline HRESULT ProcessSubscriptionDates(SIMON_COLLECTION::IDataCollection *pCollection)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        DWORD dwDaysRemaining = 0;

        hrx << SIMON_COLLECTION::GetValueFromCollection(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DAYS_REMAINING
            , dwDaysRemaining);

        DWORD dwDaysMax = 0;

        hrx << SIMON_COLLECTION::GetValueFromCollection(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_MAX_DAYS
            , dwDaysMax);

        DATE dateToday = 0;
        DWORD subBeginDate = 0,
            subEndDate   = 0;

        SYSTEMTIME sysToday= {0};
        ::GetSystemTime(&sysToday);
        ::SystemTimeToVariantTime(&sysToday,&dateToday);
        subEndDate = (DWORD) (dateToday + static_cast<DOUBLE>(dwDaysRemaining));
        subBeginDate = (DWORD) (subEndDate) - dwDaysMax;

        hrx << SIMON_COLLECTION::SetCollectionDWORD(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_END_DATE
            , subEndDate);

        hrx << SIMON_COLLECTION::SetCollectionDWORD(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_BEGIN_DATE
            , subBeginDate);
    }STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Function ProductStateToDisposition
//
// Description:
// Sets the subscription disposition based on the license state
// 
/////////////////////////////////////////////////////////////////////////////////////////////
inline void ProductStateToDisposition(DJSMAR00_LicenseState state, DWORD& dwDisposition)
{
    switch(state)
    {
    case DJSMAR00_LicenseState_TRIAL:
        dwDisposition |= SUBSCRIPTION_STATIC::DISPOSITION_PRODUCT_STATE_TRIAL;
        break;
    case DJSMAR00_LicenseState_PURCHASED:
        dwDisposition |= SUBSCRIPTION_STATIC::DISPOSITION_PRODUCT_STATE_ACTIVATED;
        break;
    case DJSMAR00_LicenseState_EXPIRED:
        dwDisposition |= SUBSCRIPTION_STATIC::DISPOSITION_PRODUCT_STATE_EXPIRED;
        break;
    default:
    case DJSMAR00_LicenseState_Violated:
        dwDisposition |= SUBSCRIPTION_STATIC::DISPOSITION_PRODUCT_STATE_VIOLATED;
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Function GetNAVPreferredOwnerHandlerPath
//
// Description:
// Function to get the DLL path to NAV's handler.  Currently it gets the first registered
// owner.  When there are multiple owners this will need to be modified.
//
// 
/////////////////////////////////////////////////////////////////////////////////////////////
inline HRESULT GetOwnerHandlerPath(CString &csOwnerDLLPath)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        /*
        // Initialize csOwnerDLLPath to empty string.
        csOwnerDLLPath.Empty();

        // Get the NAV directory
        CNAVInfo navInfo;
        CString csDefaultHandler(navInfo.GetNAVDir());

        // If we can't get it lets fail out since obviously its been tampered with.
        // This is the behaviour the rest of the product would expect.
        if(csDefaultHandler.IsEmpty())
            hrx << HRESULT_FROM_WIN32(ERROR_NOT_FOUND);

        // Got the right directory so lets put it in csOwnerDLLPath
        csOwnerDLLPath = csDefaultHandler;

        CRegKey rk;
        CString regPath(DRMREG::SZ_SUITE_OWNER_KEY);
        long rc = ERROR_SUCCESS;

        regPath += _T("\\");
        regPath += V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID;

        // Open the NAV suiteowner key in the registry
        rc = rk.Open(HKEY_LOCAL_MACHINE
            , regPath
            , KEY_READ);

        if(ERROR_SUCCESS == rc)
        {
            TCHAR szDLLName[MAX_PATH] = {0};
            ULONG ulSize = MAX_PATH;

            // Read the name of the business rule handler
            rc = rk.QueryStringValue(DRMREG::SZ_BR_DLL_NAME
                , szDLLName
                , &ulSize);

            // Default to NAVOptRF.DLL on failure
            if (ERROR_SUCCESS != rc)
                _tcscpy(szDLLName, SZ_LICENSING_DLL);

            // We have the fallback handler
            csDefaultHandler += _T("\\");
            csDefaultHandler += szDLLName;
        }
        // Go after an owner handler if we have one.  Right now we just take the first one if
        // multiple handlers are registered
        rk.Close();

        BOOL bFoundOwnerHandler = FALSE;
        // Open the component owners key
        regPath += _T("\\");
        regPath += DRMREG::SZ_OWNER_GUID_KEY_NAME;
        rc = rk.Open(HKEY_LOCAL_MACHINE
            , regPath
            , KEY_READ);

        if(ERROR_SUCCESS == rc)
        {
            TCHAR szOwnerValueName[MAX_PATH] = {0};
            DWORD dwLength = MAX_PATH;

            TCHAR szOwnerGUID[MAX_PATH] = {0};
            DWORD dwSize = MAX_PATH;

            // Get the first owner
            rc = RegEnumValue(HKEY(rk)
                , 0
                , szOwnerValueName
                , &dwLength
                , NULL
                , NULL
                , (BYTE *) szOwnerGUID
                , &dwSize );

            rk.Close();

            if(ERROR_SUCCESS == rc)
            {
                // Open the owner's key
                regPath = DRMREG::SZ_SUITE_OWNER_KEY;
                regPath += _T("\\");
                regPath += szOwnerGUID;

                rc = rk.Open(HKEY_LOCAL_MACHINE
                    , regPath
                    , KEY_READ);

                if(ERROR_SUCCESS == rc)
                {
                    // Get the business rule handler of the owner
                    TCHAR szBRDLLName[MAX_PATH] = {0};
                    dwSize = MAX_PATH;
                    rc = rk.QueryStringValue(DRMREG::SZ_BR_DLL_NAME
                        , szBRDLLName
                        , &dwSize);

                    if(ERROR_SUCCESS == rc)
                    {
                        // Got it!
                        bFoundOwnerHandler = TRUE;
                        csOwnerDLLPath += _T("\\");
                        csOwnerDLLPath += szBRDLLName;
                    }
                }
            }
        }

        if(!bFoundOwnerHandler)
            csOwnerDLLPath = csDefaultHandler;
    */
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

#endif // DRMPEPHELPER_H