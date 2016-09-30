// ----------------------------------------------------------------------------
// PROPRIETARY/CONFIDENTIAL.  Use of this product is subject to license terms.
// Copyright (C) 2005 Symantec Corporation.  All rights reserved.
// ----------------------------------------------------------------------------

#include "stdafx.h"

#include "CNAVActionsCollection.h"

#include "util.h"

nsc::NSCRESULT CNAVActionsCollection::AddClientAction (
    const DWORD dwNameID,
    const DWORD dwDescID,
    const CClientCallback::e_CallbackCookie eCallbackCookie,
    const GUID& guidAction,
	const GUID& guidCallback)
{
    // check params
    if(eCallbackCookie == CClientCallback::eCOOKIE_INVALID)
    {
        CCTRCTXE0(_T("Bad param(s)"));
        return nsc::NSCERR_ILLEGALPARAM;
    }

    nsc::IAction* pActionTemp = NULL;
    nsc::NSCRESULT result = CreateClientAction(&pActionTemp, dwNameID, dwDescID, eCallbackCookie, guidAction, guidCallback);
    if(NSC_FAILED(result))
    {
        CCTRCTXE1(_T("CreateClientAction failed nscerror:%d"),result);
        return result;
    }

    // add to collection
    result = PushBack(pActionTemp);
	pActionTemp->Release();
    if (NSC_FAILED(result))
    {
        CCTRCTXE1(_T("IActionsCollection::PushBack failed nscerror:%d"),result);
        return result;
    }

    return result;
}
