#ifndef PRODHELPERBASE_H
#define PRODHELPERBASE_H

#pragma once

#include "Subscription_Static.h"
#include "simon.h"
#include "stahlsoft.h"
#include "simoncollectiontoolbox.h"
#include "DRMPepHelper.h"

// ProductHelper interface
class IProdPepHelper
{
public:
    // ShowAlert - dispatch to license alert
    virtual HRESULT GetLicenseDispatch(IDispatch** pLicDisp) = 0;

    // Get resources
    virtual HRESULT GetIcon(HINSTANCE& hInstance, UINT& uIconID) = 0;
    virtual HRESULT GetString(UINT uID, LPTSTR szString, DWORD dwBuffer) = 0;

    // Get Cached data
    virtual HRESULT GetCachedData(SIMON_COLLECTION::IDataCollection* pCollection, DWORD dwType = DRMCACHE::CACHEDATATYPE_ALL) = 0;

    // Set Cached data
    virtual HRESULT SetCachedData(SIMON_COLLECTION::IDataCollection* pCollection, DWORD dwType = DRMCACHE::CACHEDATATYPE_ALL) = 0;
    virtual HRESULT SaveCachedData() = 0;

protected:
    virtual HRESULT GetCachedDword(const char* cszQuery, DWORD& dwData) = 0;
    virtual HRESULT GetCachedString(const char* cszQuery, CString& sData) = 0;
    virtual HRESULT SetCachedDword(const char* cszQuery, DWORD dwData) = 0;
    virtual HRESULT SetCachedString(const char* cszQuery, CString sData) = 0;
};

// ProductHelper base implementation
class CProdPepHelperImpl : public IProdPepHelper
{
public:
    CProdPepHelperImpl() {}
    virtual ~CProdPepHelperImpl() {}

    HRESULT GetCachedDataInternal(SIMON_COLLECTION::IDataCollection* pCollection, DRMCACHE::LPVALUEDATAPAIR lpVdp, DWORD dwSize)
    {
        STAHLSOFT_HRX_TRY(hr)
        {
            for(DWORD i = 0; i < dwSize; ++i)
            {
                switch(lpVdp[i].dwType)
                {
                case DRMCACHE::TYPE_DWORD:
                    {
                        DWORD dwData;
                        hr = GetCachedDword(lpVdp[i].sValue, dwData);

                        // if it succeeded add it to the collection
                        if(SUCCEEDED(hr))
                        {
                            hrx << SIMON_COLLECTION::SetCollectionDWORD(pCollection, lpVdp[i].sValue, dwData);
                        }
                        // if it wasn't found get the next one
                        else if(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr)
                        {
                            continue;
                        }
                        // critical failure
                        else
                        {
                            hrx << hr;
                        }
                    }
                    break;
                case DRMCACHE::TYPE_STRING:
                    {
                        CString sData;
                        hr =  GetCachedString(lpVdp[i].sValue, sData);

                        // if it succeeded add it to the collection
                        if(SUCCEEDED(hr))
                        {
                            hrx << SIMON_COLLECTION::SetCollectionString(pCollection, lpVdp[i].sValue, sData);
                        }
                        // if it wasn't found get the next one
                        else if(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr)
                        {
                            continue;
                        }
                        // critical failure
                        else
                        {
                            hrx << hr;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
        STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
    }

    // Get Cached data
    HRESULT GetCachedData(SIMON_COLLECTION::IDataCollection* pCollection, DWORD dwType = DRMCACHE::CACHEDATATYPE_ALL)
    {
        STAHLSOFT_HRX_TRY(hr)
        {
            // get Subscription properties
            if((dwType & DRMCACHE::CACHEDATATYPE_SUBSCRIPTION) == DRMCACHE::CACHEDATATYPE_SUBSCRIPTION)
            {
                DWORD dwSize = sizeof(DRMCACHE::g_SubValueList) / sizeof(*DRMCACHE::g_SubValueList);
                hrx << GetCachedDataInternal(pCollection, DRMCACHE::g_SubValueList, dwSize);
            }

            // get License properties
            if((dwType & DRMCACHE::CACHEDATATYPE_LICENSE) == DRMCACHE::CACHEDATATYPE_LICENSE)
            {
                DWORD dwSize = sizeof(DRMCACHE::g_LicValueList) / sizeof(*DRMCACHE::g_LicValueList);
                hrx << GetCachedDataInternal(pCollection, DRMCACHE::g_LicValueList, dwSize);
            }
        }
        STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
    }

    HRESULT SetCachedDataInternal(SIMON_COLLECTION::IDataCollection* pCollection, DRMCACHE::LPVALUEDATAPAIR lpVdp, DWORD dwSize)
    {
        STAHLSOFT_HRX_TRY(hr)
        {
            for(DWORD i = 0; i < dwSize; ++i)
            {
                switch(lpVdp[i].dwType)
                {
                case DRMCACHE::TYPE_DWORD:
                    {
                        DWORD dwData;

                        hr = SIMON_COLLECTION::GetValueFromCollection(pCollection, lpVdp[i].sValue, dwData);

                        // if it succeeded add it to the collection
                        if(SUCCEEDED(hr))
                        {
                            hrx << SetCachedDword(lpVdp[i].sValue, dwData);
                        }
                        // if it wasn't found get the next one
                        else if(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr)
                        {
                            continue;
                        }
                        // critical failure
                        else
                        {
                            hrx << hr;
                        }
                    }
                    break;
                case DRMCACHE::TYPE_STRING:
                    {
                        StahlSoft::CSmartDataPtr<BYTE> spbyText;

                        hr = SIMON_COLLECTION::GetCollectionData(pCollection, lpVdp[i].sValue, spbyText);
                        CString sData = (LPCSTR)spbyText.m_p;

                        // if it succeeded add it to the collection
                        if(SUCCEEDED(hr))
                        {
                            hrx << SetCachedString(lpVdp[i].sValue, sData);
                        }
                        // if it wasn't found get the next one
                        else if(HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr)
                        {
                            continue;
                        }
                        // critical failure
                        else
                        {
                            hrx << hr;
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }
        STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
    }

    // Set Cached data
    HRESULT SetCachedData(SIMON_COLLECTION::IDataCollection* pCollection, DWORD dwType = DRMCACHE::CACHEDATATYPE_ALL)
    {
        STAHLSOFT_HRX_TRY(hr)
        {
            // get Subscription properties
            if((dwType & DRMCACHE::CACHEDATATYPE_SUBSCRIPTION) == DRMCACHE::CACHEDATATYPE_SUBSCRIPTION)
            {
                DWORD dwSize = sizeof(DRMCACHE::g_SubValueList) / sizeof(*DRMCACHE::g_SubValueList);
                hrx << SetCachedDataInternal(pCollection, DRMCACHE::g_SubValueList, dwSize);
            }

            // get License properties
            if((dwType & DRMCACHE::CACHEDATATYPE_LICENSE) == DRMCACHE::CACHEDATATYPE_LICENSE)
            {
                DWORD dwSize = sizeof(DRMCACHE::g_LicValueList) / sizeof(*DRMCACHE::g_LicValueList);
                hrx << SetCachedDataInternal(pCollection, DRMCACHE::g_LicValueList, dwSize);
            }
            hrx << SaveCachedData();
        }
        STAHLSOFT_HRX_CATCH_ALL_RETURN(hr)
    }
};
#endif // PRODHELPERBASE_H
