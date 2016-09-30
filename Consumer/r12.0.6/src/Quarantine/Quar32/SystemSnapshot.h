//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// SystemSnapshot.h
//


#pragma once

#include "ccSymStreamArchive.h"


class CSystemSnapshot
{
public:
	CSystemSnapshot(void);
	~CSystemSnapshot(void);

	STDMETHODIMP TakeSystemSnapshot(LPCTSTR szSnapShotPath);

protected:
	HRESULT LoadDataFromResource(HINSTANCE hModule, UINT iResourceId, cc::IStream** ppStream);
	HRESULT GenerateSystemSnapshot(LPCTSTR szOutputFile);
};
