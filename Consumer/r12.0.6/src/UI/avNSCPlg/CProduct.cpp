// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------
#include "stdafx.h"


#include "CProduct.h"
#include "nscGetProductFeatures.h"

#include "StahlSoft.h"
#include "V2Authorization.h"

#include "Help_URI.h"

#include "AVRESBranding.h" // Required in order to get product name

// feature guid array
GUID aGUIDFeatures [] = {
	CLSID_NAV_Feature_NAVLicense,
    CLSID_NAV_Feature_AutoProtect,
    CLSID_NAV_Feature_EmailScanning,
    CLSID_NAV_Feature_SpywareProtection,
    CLSID_NAV_Feature_IFP,
    CLSID_NAV_Feature_IMScanning,
    CLSID_NAV_Feature_WormBlocking,
    CLSID_NAV_Feature_FSS,
    CLSID_NAV_Feature_VirusDefs,
    CLSID_NAV_Feature_Scan_Now,
    CLSID_NAV_Feature_Quarantine,
};

UINT FEATURE_COUNT = sizeof(aGUIDFeatures)/sizeof(aGUIDFeatures[0]);




// IWrapper 
nsc::NSCRESULT CProduct::GetName(const nsc::IContext* /*context_in*/, 
                                            const cc::IString*& result_out) 
                                            const throw()
{
	CBrandingRes BrandRes;
	ccLib::CString sProdName = BrandRes.ProductName();

    cc::IStringPtr spStrTemp;
	spStrTemp.Attach(ccSym::CStringImpl::CreateStringImpl(sProdName));
    if(!spStrTemp)
    {
        TRACEE(_T("String implementation failed for product GetName."));
        return nsc::NSC_FAIL;
    }
	result_out = spStrTemp;
    return nsc::NSC_SUCCESS;
}
    
nsc::NSCRESULT CProduct::GetGUID(const nsc::IContext* /*context_in*/, 
                                            GUID& guid_out) 
                                            const throw()
{
    guid_out = CLSID_NAV_Product;
    return nsc::NSC_SUCCESS;
}


nsc::NSCRESULT CProduct::GetDescription(const nsc::IContext* /*context_in*/, 
                                            const cc::IString*& description_out)
                                            const throw()
{
	ccLib::CString sProdDesc;	
	if (!sProdDesc.LoadString(IDS_PRODUCT_DESC_NAV))
    {
        TRACEE(_T("LoadString failed on %d with error %d"), IDS_PRODUCT_DESC_NAV, ::GetLastError());
        sProdDesc = "";
    }
    cc::IStringPtr spStrTemp = ccSym::CStringImpl::CreateStringImpl(sProdDesc);
    if(!spStrTemp)
    {
        TRACEE(_T("String implementation failed for product GetDesc."));
        return nsc::NSC_FAIL;
    }
	description_out = spStrTemp;
    return nsc::NSC_SUCCESS;
}


nsc::NSCRESULT CProduct::GetHelpURI(const nsc::IContext* /*context_in*/, 
                                            const cc::IString*& uri_out) 
                                            const throw()
{
    cc::IStringPtr spStrTemp = ccSym::CStringImpl::CreateStringImpl(URI_PRODUCT);
    if(!spStrTemp)
    {
        TRACEE(_T("String implementation failed for product GetHelpURI."));
        return nsc::NSC_FAIL;
    }
	uri_out = spStrTemp;
    return nsc::NSC_SUCCESS;
}


nsc::NSCRESULT CProduct::GetMonitorsCollection( const nsc::IContext* /*context_in*/,
                                            const nsc::IMonitorsCollection*& /*monitorCollection_out*/)
                                            const throw()
{
    return nsc::NSC_DOESNOTEXIST;
}
                                                    

nsc::NSCRESULT CProduct::GetPropertyBag(            const nsc::IContext* /*context_in*/, 
                                            const nsc::IPropertyBag*&  /*propertyBag_out*/)
                                            const throw()
{
    return nsc::NSC_DOESNOTEXIST;
}

nsc::NSCRESULT CProduct::OnMarkedDirty() const throw()
{
    return nsc::NSC_SUCCESS;
}


// IProduct
nsc::NSCRESULT CProduct::GetFeatureGUIDs(       const nsc::IContext* /*context_in*/, 
                                        const nsc::IFeatureGUIDsCollection*& features_out)
                                        const throw()
{
    nscLib::CFeatureGUIDsCollection* features = new nscLib::CFeatureGUIDsCollection;
    features_out = features;
    if(SUCCEEDED(CFeatureHelpers::GetProductFeatures(V2LicensingAuthGuids::SZ_NAV_SUITEOWNERGUID, features)))
        return nsc::NSC_SUCCESS;
    else
        return nsc::NSC_FAIL;

}

nsc::NSCRESULT CProduct::GetProductGraphic( const nsc::IContext* /*context_in*/, 
                                        const nsc::IGraphic*& /*graphic_out*/ ) 
                                        const throw()
{
	return nsc::NSC_DOESNOTEXIST;
}

nsc::NSCRESULT CProduct::GetContextMenu(const nsc::IContext* /*context_in*/, 
                                        const nsc::IMenu*& /*menu_out */) 
                                        const throw()
{
	return nsc::NSC_DOESNOTEXIST;
}


