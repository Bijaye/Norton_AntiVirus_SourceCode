// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------

#include "nscLibCActionsCollection.h"

#include "CClientCallback.h"

class CNAVActionsCollection : public nscLib::CActionsCollection
{
public:

    nsc::NSCRESULT AddClientAction (
        const DWORD dwNameID,
        const DWORD dwDescID,
        const CClientCallback::e_CallbackCookie eCallbackCookie,
        const GUID& guidAction,
		const GUID& guidCallback = CLSID_NAV_Client_Callback);

    // ctor
    CNAVActionsCollection() {};

    // dtor
    virtual ~CNAVActionsCollection() {};


private:
    // hide default methods
    CNAVActionsCollection(const CNAVActionsCollection&);
    CNAVActionsCollection& operator=(const CNAVActionsCollection&) throw();
};

typedef CSymPtr<CNAVActionsCollection> CNAVActionsCollectionPtr;
