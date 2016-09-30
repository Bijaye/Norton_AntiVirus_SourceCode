////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CBBSettingsErrorStack.h"

class IBBSettingsErrorStackContainer : public ISymBase
{
public:
    virtual ~IBBSettingsErrorStackContainer() {}

public:
    // Gets a reference to the error stack to allow manipulation
    virtual SYMRESULT GetErrorStack( CBBSettingsErrorStack*& errStack )
                                                                   = 0;
};
// {64BC8431-7A18-4075-A755-E92C8BFA33E4}
SYM_DEFINE_INTERFACE_ID(IID_BBSettingsErrorStackContainer, 
0x64bc8431, 0x7a18, 0x4075, 0xa7, 0x55, 0xe9, 0x2c, 0x8b, 0xfa, 0x33, 0xe4);

typedef CSymPtr<IBBSettingsErrorStackContainer>   IBBSettingsErrorStackContainerPtr;
typedef CSymQIPtr<IBBSettingsErrorStackContainer, &IID_BBSettingsErrorStackContainer> IBBSettingsErrorStackContainerQIPtr;

