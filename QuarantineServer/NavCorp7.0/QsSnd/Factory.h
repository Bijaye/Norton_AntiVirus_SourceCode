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
#if 0
class CQsSndDLLFactory : IClassFactory
{
public:
    // Construction
    CQsSndDLLFactory();
    ~CQsSndDLLFactory();

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
#endif
/////////////////////////////////////////////////////////////////////////////
// Class factory for Quarantine item object


class CQuarantineItemFactory : IClassFactory
{
public:
    // Construction
    CQuarantineItemFactory();
    ~CQuarantineItemFactory();

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



/////////////////////////////////////////////////////////////////////////////
// Class factory for Quarantine options object

#if 0
class CQuarantineOptsFactory : IClassFactory
{
public:
    // Construction
    CQuarantineOptsFactory();
    ~CQuarantineOptsFactory();

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
#endif

