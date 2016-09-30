#pragma once

#include "ItemBag.h"
#include "ccLib.h"

// Thread-safe Item Bag
//
template< class T >
class CItemBagSafe : public CItemBag <T>
{
public:
    CItemBagSafe(void);
    virtual ~CItemBagSafe(void);

    bool GetData( T _Index, std::string & sOut );
    bool GetData( T _Index, std::wstring & sOut );
	bool GetData( T _Index, long & lOut );
    bool GetData( T _Index, BYTE * pData, long lLength );
	void SetData( T _Index, long lIn );
    void SetData( T _Index, const BYTE * pData, long lLength );

    // C-Style
    bool GetData( T _Index, const char*& sOut );
    void SetData( T _Index, LPCSTR& sIn );

    bool GetData( T _Index, LPCWSTR& sOut );
    void SetData( T _Index, const WCHAR*& sIn );

protected:
    ccLib::CCriticalSection m_critAccess;
	void setData( T _Index, const std::string & sIn );
	void setData( T _Index, const std::wstring & sIn );
};

//////////////////////////////////////////////////////////////////////
// CItemBagSafe::CItemBagSafe()
template< class T > 
CItemBagSafe< T >::CItemBagSafe()
{
}

template< class T > 
CItemBagSafe< T >::~CItemBagSafe()
{
}

template< class T > 
bool CItemBagSafe< T >::GetData( T _Index, std::string & sOut )
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);
    return CItemBag<T>::GetData ( _Index, sOut );
}

template< class T > 
bool CItemBagSafe< T >::GetData( T _Index, std::wstring & sOut )
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);
    return CItemBag<T>::GetData ( _Index, sOut );
}

template< class T > 
bool CItemBagSafe< T >::GetData( T _Index, long & lOut )
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);
    return CItemBag<T>::GetData ( _Index, lOut );
}

template< class T > 
bool CItemBagSafe< T >::GetData( T _Index, BYTE * pData, long lLength )
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);
    return CItemBag<T>::GetData ( _Index, pData, lLength );
}

template< class T > 
void CItemBagSafe< T >::setData( T _Index, const std::string & sIn )
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);
    return CItemBag<T>::setData ( _Index, sIn );
}

template< class T > 
void CItemBagSafe< T >::setData( T _Index, const std::wstring & sIn )
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);
    return CItemBag<T>::setData ( _Index, sIn );
}

template< class T > 
void CItemBagSafe< T >::SetData( T _Index, long lIn )
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);
    return CItemBag<T>::SetData ( _Index, lIn );
}

template< class T > 
void CItemBagSafe< T >::SetData( T _Index, const BYTE * pData, long lLength )
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);
    return CItemBag<T>::SetData ( _Index, pData, lLength );
}

template< class T >
bool CItemBagSafe< T >::GetData( T _Index, LPCWSTR& sOut )
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);
    return CItemBag<T>::GetData ( _Index, sOut);
}

template< class T >
bool CItemBagSafe< T >::GetData( T _Index, LPCSTR& sOut )
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);
    return CItemBag<T>::GetData ( _Index, sOut);
}

template< class T >
void CItemBagSafe< T >::SetData( T _Index, LPCSTR& sIn )
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);
    return CItemBag<T>::SetData ( _Index, sIn);
}

template< class T >
void CItemBagSafe< T >::SetData( T _Index, const WCHAR*& sIn )
{
    ccLib::CSingleLock lock (&m_critAccess, INFINITE, FALSE);
    return CItemBag<T>::SetData ( _Index, sIn);
}