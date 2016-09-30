////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "stdafx.h"
#include <vector>
#include <wchar.h>

typedef std::vector<LPWSTR> APExtList;

class CAPExtWrapper
{
public:
	CAPExtWrapper();
	CAPExtWrapper(LPWSTR* arr);	//Arg is null terminated array of pointers to extension strs
	CAPExtWrapper(LPCWSTR arr);	//Arg is string in rendered format
	~CAPExtWrapper();

	LPWSTR operator[] (int index); //index starts at 0

	HRESULT Initialize(LPWSTR* arr); //Arg is null terminated array of pointers to extension strs
	HRESULT Initialize(LPCWSTR arr); //Arg is string in rendered format
	HRESULT Reset();
	HRESULT Add(LPCWSTR str);
	HRESULT Remove(LPCWSTR str);
	BOOL IsPresent(LPCWSTR str);
	BOOL IsDirty();
	size_t GetSize();

	size_t GetRenderLength();
	HRESULT Render(LPWSTR arr, size_t len);

private:
	APExtList m_dataCurr;	
	APExtList m_dataOriginal;
	bool m_bInitialized;
	bool m_bIsDirty;
};
