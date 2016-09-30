// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
//
// factory.h - contains IClassFactory declaration for ScanObj project
//
/////////////////////////////////////////////////////////////////////////////

class CDefWatchEventHandlerFactory : IClassFactory
{
public:
    // Construction
    CDefWatchEventHandlerFactory();
    ~CDefWatchEventHandlerFactory();

    // IUnknown
    STDMETHOD(QueryInterface(REFIID, void**));
    STDMETHOD_(ULONG, AddRef());
    STDMETHOD_(ULONG, Release());

    // ICLassFactory
    STDMETHOD(CreateInstance(LPUNKNOWN pUnk, REFIID riid, void** ppvObj));
    STDMETHOD(LockServer(BOOL fLock));

private:
    DWORD m_dwRef;
};



