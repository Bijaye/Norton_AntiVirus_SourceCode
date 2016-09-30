// luIWPCallback.cpp : Implementation of CluIWPCallback

#include "stdafx.h"
#include "luIWPCallback.h"
#include "idsupdater.h"

// We *must* use the same IDS object for both the pre- and post- session calls.
//
CIDSUpdater g_IDSUpdater;

// CluIWPCallback

STDMETHODIMP CluIWPCallback::OnLUNotify(tagLU_CALLBACK_TYPE nType, BSTR strMoniker)
{
    HRESULT hr = S_OK;

	try
	{
		switch ( nType )
		{
		case PreSession:
			CCTRACEI ("CIWPLUCallbackImpl::OnLUNotify - PreSession");
			{
				g_IDSUpdater.PreSession();
			}
			break;

		case PostSession:
			CCTRACEI ("CIWPLUCallbackImpl::OnLUNotify - PostSession");
			{
				g_IDSUpdater.PostSession();
			}
			break;
		}
	}
	catch (...)
	{
		hr = E_FAIL;
	}

	if (hr != S_OK)
		CCTRACEE("CIWPLUCallbackImpl::OnLUNotify - Failed");

    return hr;
}