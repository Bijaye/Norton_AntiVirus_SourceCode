////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace AvProdWidgets
{
    inline HRESULT HRESULT_FROM_SYMRESULT(SYMRESULT sr)
    {
        switch(sr)
        {
        case SYM_OK:
            return S_OK;
            break;

        case SYMERR_NOINTERFACE:
            return E_NOINTERFACE;
            break;

        case SYMERR_INVALIDARG:
            return E_INVALIDARG;
            break;
        }

        return E_UNEXPECTED;
    };

    template<const SYMINTERFACE_ID* _piidInterface, class _Interface, class _BaseClass>
    class CSymObjectCreator
    {
    public:
        CSymObjectCreator(void){};
        virtual ~CSymObjectCreator(void){};
        static HRESULT CreateInstance(_BaseClass*& pInstance)
        {
            pInstance = new _BaseClass;
            if(!pInstance)
                return E_OUTOFMEMORY;

            return S_OK;
        };

        static HRESULT CreateObjectFromInstance(_BaseClass* pInstance, _Interface*& pObject)
        {
            ISymBaseQIPtr spMySymBase = (_Interface*)pInstance;
            SYMRESULT sr = spMySymBase->QueryInterface(*_piidInterface, (void**)&pObject);
            if(SYM_SUCCEEDED(sr))
            {
                pInstance->AttachModuleManager(GETMODULEMGR());
            }

            return HRESULT_FROM_SYMRESULT(sr);
        };

        static HRESULT CreateObject(_Interface*& pObject)
        {
            HRESULT hr;
            _BaseClass* pInstance = NULL;
            hr = _BaseClass::CreateInstance(pInstance);
            if(FAILED(hr))
                return hr;

            if(!pInstance)
                return E_OUTOFMEMORY;

            return _BaseClass::CreateObjectFromInstance(pInstance, pObject);
        };
    };

}; // AvProdSvc