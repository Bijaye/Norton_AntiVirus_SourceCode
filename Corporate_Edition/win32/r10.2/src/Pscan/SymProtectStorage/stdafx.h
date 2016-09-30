// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// This file exists solely for the benefit of the common client CPP files that
// must be included in this project

#pragma once

#include "ccIgnoreWarnings.h"

#include <windows.h>

#include "ccLib.h"
#include "ccLibDllLink.h"
#include "ccSymDebugOutput.h"

// Do not allow CC to delete our objects - lifetime is bound to the SymProtectController class, so
// we'll handle it.
template <class T>
class CSavStaticRefCount : public T
{
    virtual size_t AddRef() const throw()
    { 
        // For code that verifies that the create refcount is 1 
        // by calling AddRef() and making sure the return is greater than 1
        // we use the value 2 instead of 1
        return 2;
    } 
    virtual size_t Release() const throw()
    { 
        return 2;
    }
};
