// QuarantineServer.h : Declaration of the CQuarantineServer

#ifndef __QUARANTINESERVER_H_
#define __QUARANTINESERVER_H_

#include "resource.h"       // main symbols
#include "Enumerator.h"



/////////////////////////////////////////////////////////////////////////////
// CQuarantineServer
class ATL_NO_VTABLE CQuarantineServer : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CQuarantineServer, &CLSID_QuarantineServer>,
	public IQuarantineServer,
    public IEnumQuarantineServerItems
{
public:
    CQuarantineServer();

DECLARE_REGISTRY_RESOURCEID(IDR_QUARANTINESERVER)
DECLARE_NOT_AGGREGATABLE(CQuarantineServer)

DECLARE_PROTECT_FINAL_CONSTRUCT()

BEGIN_COM_MAP(CQuarantineServer)
	COM_INTERFACE_ENTRY(IQuarantineServer)
    COM_INTERFACE_ENTRY(IEnumQuarantineServerItems)
END_COM_MAP()

// IQuarantineServer
public:
    STDMETHOD(EnumItems)(/*[out,retval]*/ IEnumQuarantineServerItems** pEnum);
    STDMETHOD(GetCopyInterface)(/*[ out ]*/ ICopyItemData** pCopy);
	STDMETHOD(RemoveAllItems)();
	STDMETHOD(RemoveItem)(/*[in]*/ ULONG ulFileID );
    STDMETHOD(EnumItemsVariant) (/* [in] */ VARIANT *va, 
                                 /* [ out ] */ IEnumQserverItemsVariant** pEnum );
	STDMETHOD(GetQuarantineItem)(/* [ in ] */ ULONG ulItem,
								 /* [ out ] */ IQuarantineServerItem** pItem );
    STDMETHOD(Config)( /* [out] */ IQserverConfig ** pConfig );
    STDMETHOD(GetItemCount)( /* [out */ ULONG* pulCount );


// IEnumQuarantineServerItems
public:
    STDMETHOD( Next )( /*[ in ]*/ ULONG celt, 
			           /*[out]*/  IQuarantineServerItem** aQserverItems, 
			           /*[out]*/  ULONG* pceltFetched );

    STDMETHOD( Skip ) ( /*[in]*/ ULONG celt );
    STDMETHOD( Reset ) ();
    STDMETHOD( Clone ) ( /*[out]*/ IEnumQuarantineServerItems** ppenum );


public:
    // 
    // Overrides
    // 
    void FinalRelease();

private:
    STDMETHOD( CreateQserverItem ) ( LPCTSTR szFileName, 
        IQuarantineServerItem** pItem );

private:
    // 
    // Enumeration helper object.
    // 
    CEnumerator*    m_pEnumerator;

};

#endif //__QUARANTINESERVER_H_
