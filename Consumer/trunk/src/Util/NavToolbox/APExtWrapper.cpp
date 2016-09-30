////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "APExtWrapper.h"

CAPExtWrapper::CAPExtWrapper():m_bIsDirty(false) {};

CAPExtWrapper::CAPExtWrapper(LPWSTR* arr):m_bIsDirty(false)
{
	Initialize(arr);
};

CAPExtWrapper::CAPExtWrapper(LPCWSTR arr):m_bIsDirty(false)
{
	Initialize(arr);
};

CAPExtWrapper::~CAPExtWrapper()
{
	Reset();
};

HRESULT CAPExtWrapper::Initialize(LPWSTR* arr)
{
	Reset();

	for(int i(0) ; arr[i] != L"\0"; i++){
		Add(arr[i]);
	}

	for(APExtList::const_iterator it = m_dataCurr.begin(); it != m_dataCurr.end(); it++)
	{
		LPWSTR tstr = new(std::nothrow) wchar_t[wcslen(*it) + 1];

		if(tstr == NULL)
		{
			CCTRCTXE0(_T("Error allocating new string."));
			return E_OUTOFMEMORY;
		}

		wcscpy(tstr, *it);
		m_dataOriginal.insert(m_dataOriginal.end(),tstr);
	}

	return S_OK;
};

HRESULT CAPExtWrapper::Initialize(LPCWSTR arr)
{
	LPWSTR tstr;
	Reset();

	while(wcslen(arr) > 0){
		tstr = new(std::nothrow) wchar_t[wcslen(arr) + 1];
		if(tstr == NULL)
		{
			CCTRCTXE0(_T("Error allocating new string."));
			return E_OUTOFMEMORY;
		}
		wcscpy(tstr, arr);
		Add(tstr);			
		arr += wcslen(arr) + 1;		
	}

	for(APExtList::const_iterator it = m_dataCurr.begin(); it != m_dataCurr.end(); it++)
	{
		tstr = new(std::nothrow) wchar_t[wcslen(*it) + 1];
		if(tstr == NULL)
		{
			CCTRCTXE0(_T("Error allocating new string."));
			return E_OUTOFMEMORY;
		}
		wcscpy(tstr, *it);
		m_dataOriginal.insert(m_dataOriginal.end(),tstr);
	}
	
	return S_OK;
};

LPWSTR CAPExtWrapper::operator[] (int index) //index starts at 0
{
	if(index >= 0 && ((size_t)index < m_dataCurr.size()))
		return m_dataCurr[index];        
	else if(index < 0 && ((size_t)abs(index) < m_dataCurr.size()))
		return m_dataCurr[m_dataCurr.size() + index];
	else
		return NULL;
}

HRESULT CAPExtWrapper::Reset()
{
	LPWSTR tstr;
	APExtList::iterator it1 = m_dataCurr.begin();
	while(it1 != m_dataCurr.end())
	{
		tstr = *it1;
		it1 = m_dataCurr.erase(it1);
		delete[] tstr;
	}

	APExtList::iterator it2 = m_dataOriginal.begin();
	while(it2 != m_dataOriginal.end())
	{
		tstr = *it2;
		it2 = m_dataOriginal.erase(it2);
		delete[] tstr;
	}

	m_bIsDirty = false;
	return S_OK;
};

HRESULT CAPExtWrapper::Add(LPCWSTR str)
{
	LPWSTR tstr = new(std::nothrow) wchar_t[wcslen(str) + 1];

	if(tstr == NULL)
	{
		CCTRCTXE0(_T("Unable to allocate scratch buffer."));
		return E_OUTOFMEMORY;
	}

	if(!IsPresent(str))
	{
		wcscpy(tstr, str);
		
		m_dataCurr.insert(m_dataCurr.end(), tstr);
		m_bIsDirty = true;
		return S_OK;
	}

	delete[] tstr;
	return S_FALSE;
};

HRESULT CAPExtWrapper::Remove(LPCWSTR str)
{
	for(APExtList::iterator it = m_dataCurr.begin(); it != m_dataCurr.end(); it++)
	{
		if(!wcscmp(*it, str))
		{
			LPWSTR tstr = *it;
			m_dataCurr.erase(it);
			delete[] tstr;
			m_bIsDirty = true;
			return S_OK;
		}
	}
	return S_FALSE;
};

BOOL CAPExtWrapper::IsPresent(LPCWSTR str)
{
	for(APExtList::const_iterator it = m_dataCurr.begin(); it != m_dataCurr.end(); it++)
	{
		if(!wcscmp(*it, str))
			return VARIANT_TRUE;
	}
	
	return VARIANT_FALSE;
};

BOOL CAPExtWrapper::IsDirty()
{

	return m_bIsDirty ? VARIANT_TRUE:VARIANT_FALSE;
/*	
	//Real IsDirty. But slow.
	if(m_dataCurr.size() != m_dataOriginal.size())
		return VARIANT_TRUE;

	for(APExtList::const_iterator it = m_dataOriginal.begin(); it != m_dataOriginal.begin(); it++)
	{
		if(!IsPresent(*it))
			return VARIANT_TRUE;
	}

	return VARIANT_FALSE;
*/
};
	
size_t CAPExtWrapper::GetSize()
{
	return m_dataCurr.size();
};

size_t CAPExtWrapper::GetRenderLength()
{
	size_t rlen(0);
	for(APExtList::const_iterator it(m_dataCurr.begin()); it != m_dataCurr.end(); it++)
	{
		rlen += wcslen(*it);
	}
	
	rlen += m_dataCurr.size();
	rlen += 1;
	
	return (rlen);
};

HRESULT CAPExtWrapper::Render(LPWSTR arr, size_t len)
{
	size_t renderLength = GetRenderLength();

	if(len < renderLength)
	{
		return S_FALSE;
	}
	
	for(APExtList::const_iterator it(m_dataCurr.begin()); it != m_dataCurr.end(); it++)
	{
		for(size_t i(0) ; i < wcslen(*it) ; i++){
			arr[i] = (*it)[i];
		}
		arr += wcslen(*it);
		*arr = L'\0';
		arr++;
	}
	*arr = L'\0';
	return S_OK;
};