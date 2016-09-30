// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------

#include "nscIWrapper.h"
#include "nscIProduct.h"

class CProduct : public ISymBaseImpl<CSymThreadSafeRefCount>, 
    public nsc::IProduct, public nsc::IWrapper
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(nsc::IID_nscIProduct,nsc::IProduct)
        SYM_INTERFACE_ENTRY(nsc::IID_nscIWrapper,nsc::IWrapper)
    SYM_INTERFACE_MAP_END()

    // IWrapper 
    virtual nsc::NSCRESULT GetName(const nsc::IContext* context_in, 
                                                const cc::IString*& result_out) 
                                                const throw();
        
    virtual nsc::NSCRESULT GetGUID(const nsc::IContext* context_in, 
                                                GUID& guid_out) 
                                                const throw();


    virtual nsc::NSCRESULT GetDescription(const nsc::IContext* context_in, 
                                                const cc::IString*& description_out)
                                                const throw();

    
    virtual nsc::NSCRESULT GetHelpURI(              const nsc::IContext* context_in, 
                                                const cc::IString*& uri_out) 
                                                const throw();


    virtual nsc::NSCRESULT GetMonitorsCollection(   const nsc::IContext* context_in,
                                                const nsc::IMonitorsCollection*& monitorCollection_out)
                                                const throw();
                                                        

    virtual nsc::NSCRESULT GetPropertyBag(          const nsc::IContext* context_in, 
                                                const nsc::IPropertyBag*&  propertyBag_out)
                                                const throw();

    virtual nsc::NSCRESULT OnMarkedDirty() const throw();

    // IFeature 
    virtual nsc::NSCRESULT GetFeatureGUIDs(     const nsc::IContext* context_in, 
                                            const nsc::IFeatureGUIDsCollection*& features_out)
                                            const throw();
    
    virtual nsc::NSCRESULT GetProductGraphic(   const nsc::IContext* context_in, 
                                            const nsc::IGraphic*& graphic_out ) 
                                            const throw();
    
    virtual nsc::NSCRESULT GetContextMenu(const nsc::IContext* context_in, 
                                            const nsc::IMenu*& menu_out ) 
                                            const throw();
    // ctor
    CProduct() {};

private:
    // hide default methods
    CProduct(const CProduct&);
    CProduct& operator=(const CProduct&) throw();
};
