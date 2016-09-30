// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------
#pragma once

#include "nscISelfRegistrar.h" 
#include "hrx.h"

// This is the interface that must be implemented in order for the dll to register the
// products and classes that it provides
class CSelfRegistrar: public ISymBaseImpl<CSymThreadSafeRefCount>,public nsc::ISelfRegistrar
{
    // This implements the ISelfRegistrar interface, so QI will work
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(nsc::IID_nscISelfRegistrar,nsc::ISelfRegistrar)
    SYM_INTERFACE_MAP_END()

public:
    // constructor, destructor... in this sample they don't do anything
    CSelfRegistrar() : m_dwAggregated(0)
    {
        // Find out if NAV is a child product
	    READ_SETTING_START()
		    READ_SETTING_DWORD(LICENSE_Aggregated, m_dwAggregated, 0)
	    READ_SETTING_END
    }
    virtual ~CSelfRegistrar(void){}
private:
    // we disable the copy constructor and the assignment operator
    CSelfRegistrar(const CSelfRegistrar&);
    CSelfRegistrar& operator=(const CSelfRegistrar&) throw();

    DWORD m_dwAggregated;
public:

    // This function is called by the server in order to have the dll register the Products and 
    // Features that this DLL provides.  All of the registration should occur right away, then 
    // the function should return. 
    virtual nsc::NSCRESULT RegisterProvidedClasses(const nsc::IRegistration* registration_in) const
    {
        if (registration_in == NULL)
        {
            TRACEE(_T("Invalid param to RegisterProvidedClasses"));
            return nsc::NSCERR_ILLEGALPARAM;
        }

        nsc::NSCRESULT nscReturn = nsc::NSC_SUCCESS;
        
        if(!m_dwAggregated)
        {
            // register product
            if (NSC_FAILED(registration_in->Register(CLSID_NAV_Product,nsc::IPRODUCT)))
            {
                TRACEE(_T("Error registering product"));
                nscReturn = nsc::NSC_FAIL;
            }
        }

        return nsc::NSC_SUCCESS;
    }

    // This function is called by the server in order to have the dll unregister the products and
    // features that this dll provides.  
    virtual nsc::NSCRESULT UnregisterProvidedClasses(const nsc::IRegistration* registration_in) const
    {
        if (registration_in == NULL)
        {
            TRACEE(_T("Invalid param to UnregisterProvidedClasses"));
            return nsc::NSCERR_ILLEGALPARAM;
        }

        nsc::NSCRESULT nscReturn = nsc::NSC_SUCCESS;

        if(!m_dwAggregated)
        {        
            // unregister product
            if (NSC_FAILED(registration_in->Unregister(CLSID_NAV_Product)))
            {
                TRACEE(_T("Error unregistering product"));
                nscReturn = nsc::NSC_FAIL;
            }
        }

        return nscReturn;
    }
};