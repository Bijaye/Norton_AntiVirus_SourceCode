////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ItemIDListPtr.h

#ifndef __ItemIDListPtr_h__
#define __ItemIDListPtr_h__

class CStrRet : public STRRET  // Cleanup utility class
{
	CComPtr<IMalloc>& m_spMalloc;

public:
	CStrRet(CComPtr<IMalloc>& spMalloc) : m_spMalloc(spMalloc)
	{
		::memset(static_cast<STRRET*>(this), 0, sizeof(STRRET));
	}
	~CStrRet(void)
	{
		// Free memory
		if (STRRET_WSTR == uType && pOleStr)
			m_spMalloc->Free(pOleStr);
	}
	HRESULT CopyTo(BSTR *psz, LPCITEMIDLIST pidl)
	{
		USES_CONVERSION;

		switch(uType)
		{
		case STRRET_WSTR:
			*psz = ::SysAllocString(pOleStr);
			break;
		case STRRET_CSTR:
			*psz = ::SysAllocString(A2W(cStr));
			break;
		case STRRET_OFFSET:
			*psz = ::SysAllocString(T2W(PTCHAR(pidl) + uOffset));
			break;
		default:
			*psz = NULL;
			return E_INVALIDARG;
		}

		return *psz ? S_OK : E_OUTOFMEMORY;
	}
};

class CItemIDListPtr // Cleanup for 
{
	LPITEMIDLIST m_p;
	CComPtr<IMalloc>& m_spMalloc;

public:
	CItemIDListPtr(CComPtr<IMalloc>& spMalloc) : m_spMalloc(spMalloc), m_p(NULL) {}
	~CItemIDListPtr(void) { Clear(); }

	void Clear(void)
	{
		if (m_p)
		{
			if (m_spMalloc)
			{
				// Release system memory
				m_spMalloc->Free(m_p);
			}

			// Mark it as free
			m_p = NULL;
		}
	}
	operator LPCITEMIDLIST(void) { return m_p; }
	LPITEMIDLIST* operator&(void) { return &m_p; }
	LPITEMIDLIST operator=(LPITEMIDLIST p) { return m_p = p; }
};

#endif __ItemIDListPtr_h__
