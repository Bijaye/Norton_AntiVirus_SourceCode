// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/factory.h_v   1.1   16 Mar 1998 18:46:02   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// factory.h - contains IClassFactory declaration for ScanObj project
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/factory.h_v  $
// 
//    Rev 1.1   16 Mar 1998 18:46:02   SEDWARD
// Include 'Quar32.h', added CQuarantineFactory.
//
//    Rev 1.0   24 Feb 1998 17:33:38   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Class factory for Quarantine DLL object

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



