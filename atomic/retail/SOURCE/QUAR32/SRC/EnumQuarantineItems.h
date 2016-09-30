/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/EnumQuarantineItems.h_v   1.1   25 Feb 1998 15:08:38   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// EnumQuarantineItems.h: interface for the CEnumQuarantineItems class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/EnumQuarantineItems.h_v  $
// 
//    Rev 1.1   25 Feb 1998 15:08:38   DBuches
// Fixed problems in testing.
// 
//    Rev 1.0   25 Feb 1998 14:00:32   DBuches
// Initial revision.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENUMQUARANTINEITEMS_H__0A004714_AD8E_11D1_9105_00C04FAC114A__INCLUDED_)
#define AFX_ENUMQUARANTINEITEMS_H__0A004714_AD8E_11D1_9105_00C04FAC114A__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "iquaran.h"
#include "shlobj.h"

class CEnumQuarantineItems : public IEnumQuarantineItems  
{
public:
    // IUnknown
    STDMETHOD(QueryInterface(REFIID, void**));
    STDMETHOD_(ULONG, AddRef());
    STDMETHOD_(ULONG, Release());
    
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
    // Reference count.
    DWORD           m_dwRef;

    // IShellFolder object for quarantine folder
    LPSHELLFOLDER   m_pFolder;
    
    // Enumeration object
    LPENUMIDLIST    m_pQuarantineFolderEnum;

    // Pointer to Shell Allocator
    LPMALLOC        m_pMalloc;

};

#endif // !defined(AFX_ENUMQUARANTINEITEMS_H__0A004714_AD8E_11D1_9105_00C04FAC114A__INCLUDED_)
