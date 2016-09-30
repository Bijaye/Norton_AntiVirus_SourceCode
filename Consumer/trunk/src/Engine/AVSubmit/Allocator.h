////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "SymInterface.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include <new>

namespace mem {

template <typename T>
struct CAllocator
{
	static bool Allocate(T*& t) throw()
	{
		if(t != NULL)
		{
			t->Release();
		}
		t = new (std::nothrow) T;
		if (t != NULL)
		{
			t->AddRef();
			t->AttachModuleManager(GETMODULEMGR());
		}
		return t != NULL;
	}
};

}