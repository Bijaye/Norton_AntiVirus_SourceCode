// DRMPEPImpl.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_DRMPEPIMPL_H__7DF6ECBE_DF1A_4479_B0AA_B01868A6E52D__INCLUDED_)
#define _DRMPEPIMPL_H__7DF6ECBE_DF1A_4479_B0AA_B01868A6E52D__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "stahlsoft.h"

#include "SIMON.h"
#include "SimonCollectionImpl.h"

#include "V2Authorization.h"

#include "DJSMAR00_Static.h"
#include "djsmar.h"

#include "drmnamedproperties.h"

#define SUBSCRIPTION00_INITGUID
#include "Subscription_Helper.h"

#include "resource.h"
#include "SymAlertStatic.h"
#include "DJSExtendedLicenseInfo.h"

// Display alert dialog
#include "ccWebWnd_h.h"

#ifdef _DRMPEP_IMPL
// CLSID for CCWebWnd
#include "ccWebWnd_i.c"
#endif

#include "DRMPepHelper.h"
#include "ProductPepHelper.h"

namespace DRMPEP
{
// Product based helper functions
/////////////////////////////////////////////////////////////////////////////////////////////
// Function CachedInfoToRecord
//
// Description:
// 
/////////////////////////////////////////////////////////////////////////////////////////////
inline HRESULT CachedInfoToRecord(DWORD dwType, SIMON_COLLECTION::IDataCollection* pCollection)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        MessageBox(NULL, "attach", "attach", MB_OK);
        CProdPepHelper pphProduct;
        pphProduct.GetCachedData(pCollection, dwType);
    }STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Function ShowAlert
//
// Description:
// 
/////////////////////////////////////////////////////////////////////////////////////////////
inline HRESULT ShowAlert(HWND hWnd, SYMALERT_TYPE alertType, SYMALERT_RESULT &Result)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        TCHAR szResDLL[MAX_PATH] = {0};

        CString sModule;
        GetModuleFileName(_Module.GetModuleInstance(), sModule.GetBuffer(MAX_PATH), MAX_PATH);
        sModule.ReleaseBuffer();

        sModule = sModule.Left(sModule.ReverseFind(_T('\\')));

        // Get DRM alerts from DJSAlert.dll.
        wsprintf(szResDLL, _T("%s\\DJSAlert.dll"), sModule);

        StahlSoft::CSmartModuleHandle shResDLL = LoadLibrary(szResDLL);
        hrx << ((shResDLL == NULL) ? E_FAIL : S_OK);

        TCHAR szHTML[MAX_PATH] = {0};
        RECT rectHTML = {0};

        typedef bool (WINAPI* PFGETHTMLINFO)(SYMALERT_TYPE, LPTSTR, RECT&);
        PFGETHTMLINFO pfGetHTMLInfo = NULL;
        pfGetHTMLInfo = (PFGETHTMLINFO)(GetProcAddress(shResDLL, "_GetAlertHTMLInfo@12"));
        hrx << ((pfGetHTMLInfo == NULL) ? E_FAIL : S_OK);
        hrx << ((pfGetHTMLInfo(alertType, szHTML, rectHTML) == false)? E_FAIL : S_OK);

        CProdPepHelper pphProduct;
        IDispatch* pDispatch = NULL;
        pphProduct.GetLicenseDispatch(&pDispatch);

        CComPtr<IccWebWindow> spWebCtrl;
        spWebCtrl.CoCreateInstance(CLSID_ccWebWindow, NULL, CLSCTX_ALL);

        // Pass in the dispatch to the WebWindo object argument so the collection
        // can be used by the JS

        _variant_t varObjectArg(pDispatch);
        _variant_t vRetVal;
        spWebCtrl->put_ObjectArg(varObjectArg);

        HINSTANCE hInstance;
        UINT uIcon;
        pphProduct.GetIcon(hInstance, uIcon);
        spWebCtrl->SetIcon((long)hInstance, uIcon);

        // Set the window height, width and icon
        spWebCtrl->put_FrameWindowStyle(WS_SYSMENU | WS_CAPTION);

        DWORD dwHeight = rectHTML.bottom-rectHTML.top;
        DWORD dwWidth = rectHTML.right-rectHTML.left;

        _bstr_t bstrURL(szHTML);
        spWebCtrl->showModalDialogEx( (long) hWnd, bstrURL, dwWidth, dwHeight, NULL, &vRetVal );

        SYMALERT_RESULT Result = SYMALERT_RESULT_CANCEL;
        Result = (SYMALERT_RESULT) (vRetVal.lVal);
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

// Implementation for DRM Provided PEP Handlers

/////////////////////////////////////////////////////////////////////////////////////////////
// Function HandleQueryAboutBoxText
//
// Description:
// 
/////////////////////////////////////////////////////////////////////////////////////////////
inline HRESULT HandleQueryAboutBoxText(SIMON_COLLECTION::IDataCollection* pCollection)
{
    TCHAR szCombined [512] = {0};
    TCHAR szCombinedFormat [512] = {0};

    TCHAR szDefault[64] = {0};      // Empty string

    CString sSubSKU, sSubSKUPaid, sPartNumOpts, sActKey;

    STAHLSOFT_HRX_TRY(hr)
    {
        SIMON::CSimonPtr< SIMON_COLLECTION::IDataCollection >               spColl;
        SIMON::CSimonObject< SIMON_COLLECTION::CDataCollectionObject >* pObj = NULL;
        hrx << SIMON::CSimonObject< SIMON_COLLECTION::CDataCollectionObject >::CreateInstance(&pObj);
        hrx << pObj->QueryInterface(SIMON_COLLECTION::IID_IDataCollection,(void**)&spColl);

        hrx << CachedInfoToRecord(DRMCACHE::CACHEDATATYPE_ALL, spColl);

        DJSMAR_LicenseType licType = DJSMAR_LicenseType_Violated;

        DWORD dwValue;
        hrx << SIMON_COLLECTION::GetValueFromCollection(spColl
            , DRM::szDRMLicenseType
            , dwValue);
        licType = static_cast<DJSMAR_LicenseType>(dwValue);

        StahlSoft::CSmartDataPtr<BYTE> spbyText;

        hr = SIMON_COLLECTION::GetCollectionData( spColl
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_SKU
            , spbyText);

        sSubSKU = (LPCSTR)spbyText.m_p;

        hr = SIMON_COLLECTION::GetCollectionData( spColl
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_PAIDSKU
            , spbyText);

        sSubSKUPaid = (LPCSTR)spbyText.m_p;

        hr = SIMON_COLLECTION::GetCollectionData( spColl
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_PART_NUMBER
            , spbyText);


        sPartNumOpts = (LPCSTR)spbyText.m_p;

        hr = SIMON_COLLECTION::GetCollectionData( spColl
            , DRM::szDRMActivationKey
            , spbyText);

        sActKey = (LPCSTR)spbyText.m_p;

        // Get the subscription disposition
        DWORD dwDisposition = SUBSCRIPTION_STATIC::DISPOSITION_NONE;

        hrx << SIMON_COLLECTION::GetValueFromCollection(spColl
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION
            , dwDisposition);

        CProdPepHelper pphProduct;

        if (DJSMAR_LicenseType_Rental == licType)
        {
            pphProduct.GetString(IDS_RENTAL_PROD_INFO, szCombinedFormat, 250);
            _stprintf (szCombined, szCombinedFormat, sPartNumOpts);
        }
        else
        {
            BOOL bExpired = TRUE;
            BOOL bWarning = FALSE;

            BOOL bPaid = DWORD_HAS_FLAG(dwDisposition, SUBSCRIPTION_STATIC::DISPOSITION_SUBSCRIPTION_PAID);

            if (bPaid)
            {
                // In PAID mode
                //
                pphProduct.GetString(IDS_SUBINFO_PAID, szCombinedFormat, 250);
                _stprintf (szCombined, szCombinedFormat, sSubSKUPaid, sPartNumOpts);
            }
            else
            {
                // In FREE mode
                //
                pphProduct.GetString(IDS_SUBINFO_FREE, szCombinedFormat, 250);
                _stprintf (szCombined, szCombinedFormat, sSubSKU, sSubSKUPaid, sPartNumOpts);
            }

            hrx << SIMON_COLLECTION::SetCollectionString(pCollection
                , SIMON_PEP::SZ_MANAGEMENT_PROPERTY_TEXT
                , szCombined);

            if(!sActKey.IsEmpty())
                hrx << SIMON_COLLECTION::SetCollectionString(pCollection
                , SIMON_PEP::SZ_MANAGEMENT_PROPERTY_TEXT2
                , sActKey);
        }
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Function HandleQueryProductState
//
// Description:
// 
/////////////////////////////////////////////////////////////////////////////////////////////
inline HRESULT HandleQueryProductState(SIMON_COLLECTION::IDataCollection* pCollection)
{
    BOOL    bDiscrete = FALSE;

    DWORD   dwDisposition = SUBSCRIPTION_STATIC::DISPOSITION_NONE;

    CachedInfoToRecord(DRMCACHE::CACHEDATATYPE_SUBSCRIPTION, pCollection);

    if(DWORD_HAS_FLAG(dwDisposition, SUBSCRIPTION_STATIC::DISPOSITION_PRODUCT_STATE_TRIAL)
        || DWORD_HAS_FLAG(dwDisposition, SUBSCRIPTION_STATIC::DISPOSITION_PRODUCT_STATE_ACTIVATED))
        bDiscrete = TRUE;

    HRESULT hr = SIMON_COLLECTION::SetCollectionDWORD(pCollection
        , SIMON_PEP::SZ_MANAGEMENT_PROPERTY_DISCRETE
        , (DWORD) bDiscrete);

    return hr;
} 

/////////////////////////////////////////////////////////////////////////////////////////////
// Function HandleActivateSubscription
//
// Description:
// 
/////////////////////////////////////////////////////////////////////////////////////////////
inline HRESULT HandleActivateSubscription(SIMON_COLLECTION::IDataCollection* pCollection)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        CString sModule;
        GetModuleFileName(_Module.GetModuleInstance(), sModule.GetBuffer(MAX_PATH), MAX_PATH);
        sModule.ReleaseBuffer();

        // Load Suitechild
        StahlSoft::CSmartModuleHandle shModule = ::LoadLibrary(sModule);

        // Initializing collection
        hrx << SIMON_COLLECTION::SetCollectionDWORD(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DAYS_REMAINING
            , 0);

        hrx << SIMON_COLLECTION::SetCollectionDWORD(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_MAX_DAYS
            , 0);

        hrx << SIMON_COLLECTION::SetCollectionDWORD(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_WARNING_PERIOD
            , 0);

        SIMON::CSimonPtr< SIMON_COLLECTION::IDataCollection >               spColl;
        SIMON::CSimonObject< SIMON_COLLECTION::CDataCollectionObject >* pObj = NULL;

        hrx << SIMON::CSimonObject< SIMON_COLLECTION::CDataCollectionObject >::CreateInstance(&pObj);
        hrx << pObj->QueryInterface(SIMON_COLLECTION::IID_IDataCollection,(void**)&spColl);

        hrx << LTSUBSCRIPTION::CopySubscriptionProperties(pCollection, spColl);
        hrx << LTSUBSCRIPTION::StartSubscription(shModule, spColl);
        hrx << LTSUBSCRIPTION::CopySubscriptionProperties(spColl, pCollection);

        hrx << ProcessSubscriptionDates(pCollection);

        // Update the cache
        CProdPepHelper pphProduct;
        pphProduct.SetCachedData(pCollection, DRMCACHE::CACHEDATATYPE_SUBSCRIPTION);
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Function HandleInitSubscription
//
// Description:
// 
/////////////////////////////////////////////////////////////////////////////////////////////
inline HRESULT HandleInitSubscription(SIMON_COLLECTION::IDataCollection* pCollection)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        CString sModule;
        GetModuleFileName(_Module.GetModuleInstance(), sModule.GetBuffer(MAX_PATH), MAX_PATH);
        sModule.ReleaseBuffer();

        // Load Suitechild
        StahlSoft::CSmartModuleHandle shModule = ::LoadLibrary(sModule);

        SIMON::CSimonPtr< SIMON_COLLECTION::IDataCollection >               spColl;
        SIMON::CSimonObject< SIMON_COLLECTION::CDataCollectionObject >* pObj = NULL;

        hrx << SIMON::CSimonObject< SIMON_COLLECTION::CDataCollectionObject >::CreateInstance(&pObj);
        hrx << pObj->QueryInterface(SIMON_COLLECTION::IID_IDataCollection,(void**)&spColl);

        hrx << LTSUBSCRIPTION::CopySubscriptionProperties(pCollection, spColl);
        hrx << LTSUBSCRIPTION::InitializeSubscription(shModule, spColl);
        hrx << LTSUBSCRIPTION::CopySubscriptionProperties(spColl, pCollection);

        // Update the cache
        CProdPepHelper pphProduct;
        pphProduct.SetCachedData(pCollection, DRMCACHE::CACHEDATATYPE_SUBSCRIPTION);
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Function HandleQuerySubscriptionRecord
//
// Description:
// 
/////////////////////////////////////////////////////////////////////////////////////////////
inline HRESULT HandleQuerySubscriptionRecord(SIMON_COLLECTION::IDataCollection* pCollection)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        CString sModule;
        GetModuleFileName(_Module.GetModuleInstance(), sModule.GetBuffer(MAX_PATH), MAX_PATH);
        sModule.ReleaseBuffer();

        // Load Suitechild
        StahlSoft::CSmartModuleHandle shModule = ::LoadLibrary(sModule);

        // Initializing collection
        hrx << SIMON_COLLECTION::SetCollectionDWORD(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DAYS_REMAINING
            , 0);

        hrx << SIMON_COLLECTION::SetCollectionDWORD(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_MAX_DAYS
            , 0);

        hrx << SIMON_COLLECTION::SetCollectionDWORD(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_WARNING_PERIOD
            , 0);

        DWORD  dwDisposition = SUBSCRIPTION_STATIC::DISPOSITION_NONE;

        SIMON_COLLECTION::GetValueFromCollection(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION
            , dwDisposition);

        DWORD  dwDRMDisposition = DRMPEP::DISPOSITION_NONE;

        SIMON_COLLECTION::GetValueFromCollection(pCollection
            , DRMPEP::SZ_PROPERTY_DISPOSITION
            , dwDRMDisposition);

        if(DWORD_HAS_FLAG(dwDRMDisposition,DRMPEP::DISPOSITION_POSSIBLY_ONLINE))
        {
            // do something here to kick off a silent confirmation of activation via the DRM Activation Facilities.
        }

        if(DWORD_HAS_FLAG(dwDRMDisposition,DRMPEP::DISPOSITION_CALL_DIRECT))
        {
            // Go direct to the subscription to get the info
            SIMON::CSimonPtr< SIMON_COLLECTION::IDataCollection >               spColl;
            SIMON::CSimonObject< SIMON_COLLECTION::CDataCollectionObject >* pObj = NULL;

            hrx << SIMON::CSimonObject< SIMON_COLLECTION::CDataCollectionObject >::CreateInstance(&pObj);
            hrx << pObj->QueryInterface(SIMON_COLLECTION::IID_IDataCollection,(void**)&spColl);

            hrx << LTSUBSCRIPTION::CopySubscriptionProperties(pCollection, spColl);
            hrx << LTSUBSCRIPTION::GetSubscriptionInfo(shModule, spColl);
            hrx << LTSUBSCRIPTION::CopySubscriptionProperties(spColl, pCollection);

            hrx << ProcessSubscriptionDates(pCollection);

            // Update the cache only if is not set
            if(!DWORD_HAS_FLAG(dwDRMDisposition,DRMPEP::DISPOSITION_DO_NOT_UPDATE_CACHE))
            {
                CProdPepHelper pphProduct;
                pphProduct.SetCachedData(pCollection, DRMCACHE::CACHEDATATYPE_SUBSCRIPTION);
            }
        }
        else
            CachedInfoToRecord(DRMCACHE::CACHEDATATYPE_SUBSCRIPTION, pCollection); // Do all operations on the cache
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Function HandleSubscriptionManagement
//
// Description:
// 
/////////////////////////////////////////////////////////////////////////////////////////////
inline HRESULT HandleSubscriptionManagement(SIMON_COLLECTION::IDataCollection* pCollection)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        // Initializing collection
        hrx << SIMON_COLLECTION::SetCollectionDWORD(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DAYS_REMAINING
            , 0);

        hrx << SIMON_COLLECTION::SetCollectionDWORD(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_MAX_DAYS
            , 0);

        hrx << SIMON_COLLECTION::SetCollectionDWORD(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_WARNING_PERIOD
            , 0);

        DWORD  dwDisposition = SUBSCRIPTION_STATIC::DISPOSITION_NONE;

        SIMON_COLLECTION::GetValueFromCollection(pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_DISPOSITION
            , dwDisposition);

        DWORD  dwDRMDisposition = SUBSCRIPTION_STATIC::DISPOSITION_NONE;

        SIMON_COLLECTION::GetValueFromCollection(pCollection
            , DRMPEP::SZ_PROPERTY_DISPOSITION
            , dwDRMDisposition);

        if(DWORD_HAS_FLAG(dwDRMDisposition,DRMPEP::DISPOSITION_POSSIBLY_ONLINE))
        {
            // do something here to kick off a silent confirmation of activation via the DRM Activation Facilities.
        }

        DWORD  dwSubMgmtTask = SYMALERT_RESULT_NONE;

        // Read the task that we need to do here
        hrx << SIMON_COLLECTION::GetValueFromCollection(pCollection
            , SUBSCRIPTION_STATIC::SZ_PROPERTY_SUB_MGMT_TASK
            , dwSubMgmtTask);

        CString sModule;
        GetModuleFileName(_Module.GetModuleInstance(), sModule.GetBuffer(MAX_PATH), MAX_PATH);
        sModule.ReleaseBuffer();

        // Load Suitechild
        StahlSoft::CSmartModuleHandle shModule = ::LoadLibrary(sModule);

        SIMON::CSimonPtr< SIMON_COLLECTION::IDataCollection >               spColl;
        SIMON::CSimonObject< SIMON_COLLECTION::CDataCollectionObject >* pObj = NULL;

        hrx << SIMON::CSimonObject< SIMON_COLLECTION::CDataCollectionObject >::CreateInstance(&pObj);
        hrx << pObj->QueryInterface(SIMON_COLLECTION::IID_IDataCollection,(void**)&spColl);

        hrx << LTSUBSCRIPTION::CopySubscriptionProperties(pCollection, spColl);

        // execute that task
        switch(dwSubMgmtTask)
        {
        case SYMALERT_RESULT_RENEW_SUB_WITH_KEY:
            // TODO: Put in code to trigger smellycat
            hrx << LTSUBSCRIPTION::RenewSubscription(shModule, spColl);
            break;
        case SYMALERT_RESULT_SYNC_SUB:
            hrx << LTSUBSCRIPTION::SynchronizeSubscription(shModule, spColl);
            break;
        case SYMALERT_RESULT_RENEW_SUB:
        default:
            hrx << LTSUBSCRIPTION::RenewSubscription(shModule, spColl);
            break;
        }

        hrx << LTSUBSCRIPTION::CopySubscriptionProperties(spColl, pCollection);
        hrx << ProcessSubscriptionDates(pCollection);

        CProdPepHelper pphProduct;
        hrx << pphProduct.SetCachedData(pCollection, DRMCACHE::CACHEDATATYPE_SUBSCRIPTION);
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Function HandleForceSubscriptionRenewal
//
// Description:
// 
/////////////////////////////////////////////////////////////////////////////////////////////
inline HRESULT HandleForceSubscriptionRenewal(SIMON_COLLECTION::IDataCollection* pCollection)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        DWORD dwhWnd = 0;
        // Get SZ_SUBSCRIPTION_PROPERTY_PARENT_HWND from collection
        HRESULT hrInternal = SIMON_COLLECTION::GetValueFromCollection( pCollection
            , SUBSCRIPTION_STATIC::SZ_SUBSCRIPTION_PROPERTY_PARENT_HWND
            , dwhWnd );

        SYMALERT_RESULT result = SYMALERT_RESULT_NONE;
        hrx << ShowAlert((HWND) dwhWnd, SYMALERT_TYPE_SubscriptionPrompt, result);

        DWORD  dwSubMgmtTask = result;
        // Read the task that we need to do here
        hrx << SIMON_COLLECTION::SetCollectionDWORD(pCollection
            , SUBSCRIPTION_STATIC::SZ_PROPERTY_SUB_MGMT_TASK
            , dwSubMgmtTask);

        hrx << HandleSubscriptionManagement(pCollection);
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Function HandleQueryLicenseRecord
//
// Description:
// 
/////////////////////////////////////////////////////////////////////////////////////////////
inline HRESULT HandleQueryLicenseRecord(SIMON_COLLECTION::IDataCollection* pCollection)
{
    STAHLSOFT_HRX_TRY(hr)
    {
        DJSMAR::LICENSE_INFO_EX_SCSS liex;

        CString sUpsellUrl, sResolveUrl;

        CString sModule;
        GetModuleFileName(_Module.GetModuleInstance(), sModule.GetBuffer(MAX_PATH), MAX_PATH);
        sModule.ReleaseBuffer();

        DWORD  dwDRMDisposition = SUBSCRIPTION_STATIC::DISPOSITION_NONE;

        SIMON_COLLECTION::GetValueFromCollection(pCollection
            , DRMPEP::SZ_PROPERTY_DISPOSITION
            , dwDRMDisposition);

        DWORD dwType = DRMCACHE::CACHEDATATYPE_LICENSE;

        if(DWORD_HAS_FLAG(dwDRMDisposition,DRMPEP::DISPOSITION_GET_SUBSCRIPTION_INFO))
            dwType = DRMCACHE::CACHEDATATYPE_ALL;

        if(DWORD_HAS_FLAG(dwDRMDisposition,DRMPEP::DISPOSITION_CALL_DIRECT))
        {
            DWORD dwStartupMode = 0;

            StahlSoft::CSmartModuleHandle   shModule = ::LoadLibrary(sModule);

            hrx << GetExtendedLicenseInfo(shModule, liex, sUpsellUrl, sResolveUrl, dwStartupMode);

            hrx << SetDRMProperties(liex, sUpsellUrl, sResolveUrl, dwStartupMode, pCollection);

            hrx << HandleQuerySubscriptionRecord(pCollection);

            CProdPepHelper pphProduct;
            hrx << pphProduct.SetCachedData(pCollection, dwType);
        }
        else
            CachedInfoToRecord(dwType, pCollection);             // Do all operations on the cache
    }
    STAHLSOFT_HRX_CATCH_ALL_RETURN(hr);
}
}//namespace DRMPEP
#endif // !defined(_DRMPEPIMPL_H__7DF6ECBE_DF1A_4479_B0AA_B01868A6E52D__INCLUDED_)
