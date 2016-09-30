////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once


// {CAEC0E52-4FFC-4825-807C-C3A19DDC76DC}
SYM_DEFINE_INTERFACE_ID(CLSID_SingleInstance, 
                        0xcaec0e52, 0x4ffc, 0x4825, 0x80, 0x7c, 0xc3, 0xa1, 0x9d, 0xdc, 0x76, 0xdc);

class ISingleInstance : public ISymBase
{
public:
    virtual HRESULT RegisterSingleInstance(LPCSTR lpName, bool &bAlreadyExists, DWORD &dwOwnerProcess, DWORD &dwOwnerThread) = 0;
    virtual HRESULT AlreadyExists(LPCSTR lpName, bool &bAlreadyExists, DWORD &dwOwnerProcess, DWORD &dwOwnerThread) = 0;
};

// {B6AE66E0-E34C-479d-99E1-63CA43B0F4EA}
SYM_DEFINE_INTERFACE_ID(IID_SingleInstance, 
                        0xb6ae66e0, 0xe34c, 0x479d, 0x99, 0xe1, 0x63, 0xca, 0x43, 0xb0, 0xf4, 0xea);

typedef CSymPtr<ISingleInstance> ISingleInstancePtr;
typedef CSymQIPtr<ISingleInstance, &IID_SingleInstance> ISingleInstanceQIPtr;
