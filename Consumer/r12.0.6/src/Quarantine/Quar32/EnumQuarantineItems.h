/////////////////////////////////////////////////////////////////////////////
//
// EnumQuarantineItems.h: interface for the CEnumQuarantineItems class.
//

#if !defined(AFX_ENUMQUARANTINEITEMS_H__0A004714_AD8E_11D1_9105_00C04FAC114A__INCLUDED_)
#define AFX_ENUMQUARANTINEITEMS_H__0A004714_AD8E_11D1_9105_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "iquaran.h"
#include "shlobj.h"
#include "ccsynclock.h"

class CEnumQuarantineItems : public IEnumQuarantineItems,
                             public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(IID_EnumQuarantineItems, IEnumQuarantineItems)
	SYM_INTERFACE_MAP_END()

    // IEnumQuarantineItems
    STDMETHOD( Next( ULONG celt,
        IQuarantineItem ** pItems,
        ULONG * pceltFetched ) );

    STDMETHOD( Skip( ULONG celt ) );

    STDMETHOD( Reset( void ) );

    STDMETHOD( Clone( IEnumQuarantineItems ** ppvOut ) );
	
    // Construction
    CEnumQuarantineItems();
	~CEnumQuarantineItems();

    HRESULT Initialize( LPSTR lpszFolder );

private:
    void StringFromStrRet(STRRET* p, LPITEMIDLIST pidl, LPSTR szFileName );

private:
    // IShellFolder object for quarantine folder
    LPSHELLFOLDER   m_pFolder;
    
    // Enumeration object
    LPENUMIDLIST    m_pQuarantineFolderEnum;

    // Pointer to Shell Allocator
    LPMALLOC        m_pMalloc;

    // Synchronization object to make calls to this object thread safe
    ccLib::CCriticalSection m_critSec;

};

#endif // !defined(AFX_ENUMQUARANTINEITEMS_H__0A004714_AD8E_11D1_9105_00C04FAC114A__INCLUDED_)
