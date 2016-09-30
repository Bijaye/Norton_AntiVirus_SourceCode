// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////

#ifndef _SYMINTERFACE_H_
#define _SYMINTERFACE_H_

#ifndef  __cplusplus
#error C++ required for SymInterface.h 
#endif

#if defined WIN32 || defined NLM || defined LINUX
#include "SavAssert.h"
#endif

#if defined LINUX
	#include "winThreads.h"
#endif

#include <new> // For bad_alloc exception.

#if !defined (UNREFERENCED_PARAMETER)
    #define UNREFERENCED_PARAMETER(P) (P)
#endif

///////////////////////////////////////////////////////////////////////////////
// SYMGUID defined.
//

#ifndef SYMGUID_DEFINED
#define SYMGUID_DEFINED
typedef struct _SYMGUID
{
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} SYMGUID;
#endif /* SYMGUID_DEFINED */

///////////////////////////////////////////////////////////////////////////////
// Interface and object IDs defined.
//

typedef _SYMGUID SYMINTERFACE_ID;
typedef _SYMGUID SYMOBJECT_ID;
typedef const _SYMGUID& REFSYMINTERFACE_ID;
typedef const _SYMGUID& REFSYMOBJECT_ID;
                          
///////////////////////////////////////////////////////////////////////////////
// Result type defined.
//

typedef unsigned long SYMRESULT;

///////////////////////////////////////////////////////////////////////////////
// Macros

// Macros used to define GUIDs.
#ifndef INITIIDS
#define SYM_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    extern "C" const SYMGUID name
#else

#define SYM_DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    extern "C" const SYMGUID name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#endif // INITIIDS

#define SYM_DEFINE_INTERFACE_ID SYM_DEFINE_GUID
#define SYM_DEFINE_OBJECT_ID    SYM_DEFINE_GUID

// Success/Failure macros.
#define SYM_SUCCEEDED(x)        ( ((x) & 0x80000000) == 0 )
#define SYM_FAILED(x)           ( !SYM_SUCCEEDED(x) )

// Used for defining SYMRESULT values.
#define SYM_SUCCESS             0
#define SYM_ERROR               1
#define SYM_MAKE_RESULT( suc, code ) (( SYMRESULT) ((suc << 31) | ( 0xFFFF & code)) )
#define SYM_MAKE_RESULT_INTERNAL( suc, code ) ( SYM_MAKE_RESULT( suc, code ) | 0x10000 )
 
// Standard success return values.
#define SYM_OK                          SYM_MAKE_RESULT_INTERNAL( SYM_SUCCESS, 0x0 )
#define SYM_FALSE                       SYM_MAKE_RESULT_INTERNAL( SYM_SUCCESS, 0x1 )

// Standard error return values
#define SYMERR_NOINTERFACE              SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x200 )
#define SYMERR_OUTOFMEMORY              SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x201 )
#define SYMERR_NOTIMPLEMENTED           SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x202 )
#define SYMERR_INVALIDARG               SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x203 )
#define SYMERR_ACCESSDENIED             SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x204 )
#define SYMERR_PATH_NOT_FOUND		    SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x300 )
#define SYMERR_MISSING_EXPORTS		    SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x302 )
#define SYMERR_INVALID_FILE		        SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x303 )
#define SYMERR_UNKNOWN                  SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0xFFFF )
 
// Test for equal interface ids.
#define SymIsEqualIID( x1,x2 ) (memcmp( &x1, &x2, sizeof( SYMGUID ) ) == 0 ) 

///////////////////////////////////////////////////////////////////////////////
// ISymBase - This is the class from which all interfaces derive.  
// See the COM IUnknown specification for details.
//

class ISymBase
{
public:
    virtual SYMRESULT QueryInterface( REFSYMINTERFACE_ID iid, void** ppvObject ) const throw() = 0;
    virtual size_t AddRef() const throw() = 0;
    virtual size_t Release() const throw() = 0;
};
SYM_DEFINE_INTERFACE_ID( IID_SymBase, 0x00000000, 0x0000, 0x0000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 );

///////////////////////////////////////////////////////////////////////////////
// ISymFactory
//

class ISymFactory : public ISymBase
{
public:
    virtual SYMRESULT CreateInstance( REFSYMINTERFACE_ID iid, void** ppvObject ) throw() = 0;
};
SYM_DEFINE_INTERFACE_ID( IID_SymFactory, 0x00000001, 0x0000, 0x0000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 );

///////////////////////////////////////////////////////////////////////////////
// ISymFactory2
//
// Forward declaration for module manager.
namespace cc
{
    class IModuleLifetimeMgr;
};

class ISymFactory2 : public ISymFactory
{
public:
    virtual SYMRESULT CreateInstance( REFSYMINTERFACE_ID iid, void** ppvObject, ::cc::IModuleLifetimeMgr* pModuleMgr ) throw() = 0;
};
SYM_DEFINE_INTERFACE_ID( IID_SymFactory2, 0xae9a53f3, 0xaf14, 0x4c9d, 0xa2, 0x6e, 0x77, 0xb5, 0xd2, 0x87, 0xa9, 0xcf);

///////////////////////////////////////////////////////////////////////////////
// CSymPtr - Semi-Smart pointer for ISymBase derived objects.
// Similar to ATL's CComPtr.
//

template< class T >
class CSymPtr
{
public:
    CSymPtr() throw()
        : m_p(NULL)
    { 
    }
    ~CSymPtr() throw()
    { 
        Release();
    }
    CSymPtr(int nNull) throw()
        : m_p(NULL)
    {
        *this = nNull;
    }
    CSymPtr(const CSymPtr<T>& p) throw()
        : m_p(NULL)
    { 
        *this = p; 
    }
    CSymPtr(const T* p) throw()
        : m_p(NULL)
    { 
        *this = p; 
    }
    T* operator =(int nNull) throw() 
    {
        SAVASSERT(nNull == 0);
        UNREFERENCED_PARAMETER(nNull);
        Release();
        return m_p;
    }
    T* operator =(const CSymPtr<T>& p) throw() 
    { 
        if (&p == this)
        {
            return m_p;
        }
        return operator =(p.m_p);
    }
    T* operator =(const T* p) throw()
    { 
        if (p == m_p)
        {
            return m_p;
        }
        Release();
        m_p = const_cast<T*>(p); 
        AddRef();
        return m_p; 
    }
    operator T*() const throw()
    { 
        return m_p; 
    }
    operator T*&() throw()
    {
        return m_p;
    }
    T& operator *() const throw()
    { 
        return *m_p; 
    }
    T** operator &() throw()
    { 
        return &m_p; 
    }
    T* operator ->() const throw()
    { 
        return m_p; 
    }
    bool operator !() const throw()
    {
        return (m_p == NULL);
    }
    bool operator <(T* p) const throw()
    {
        return m_p < p;
    }
    bool operator ==(T* p) const throw()
    {
        return m_p == p;
    }
    size_t Release() const throw()
    {
        size_t nRefCount = 0;
        if (m_p != NULL)
        {
            nRefCount = m_p->Release();
            m_p = NULL;
        }
        return nRefCount;
    }
    size_t AddRef() const throw()
    {
        size_t nRefCount = 0;
        if (m_p != NULL)
        {
            nRefCount = m_p->AddRef();
        }
        return nRefCount;
    }
    void Attach(T* p) throw()
    {
        Release();
        m_p = p;
    }
    T* Detach() throw()
    {
        T* p = m_p;
        m_p = NULL;
        return p;
    }

public:
    mutable T* m_p;
};

template <class T, const SYMINTERFACE_ID* piid>
class CSymQIPtr : public CSymPtr<T>
{
public:
    CSymQIPtr() throw()
    {
    }
    CSymQIPtr(int nNull) throw()
        : CSymPtr<T>(nNull)
    {
    }
    CSymQIPtr(const T* p) throw()
        : CSymPtr<T>(p)
    {
    }
    CSymQIPtr(const CSymQIPtr<T, piid>& p) throw()
        : CSymPtr<T>(p)
    {
    }
    CSymQIPtr(const ISymBase* p) throw()
    {
        CSymPtr<T>::m_p = NULL;
        *this = p;
    }
    T* operator =(int nNull) throw()
    {
        return CSymPtr<T>::operator =(nNull);
    }
    T* operator =(const T* p) throw()
    {
        return CSymPtr<T>::operator =(p);
    }
    T* operator =(const CSymQIPtr<T, piid>& p) throw()
    {
        if (&p == this)
        {
            return CSymPtr<T>::m_p;
        }
        return CSymPtr<T>::operator =(p.m_p);
    }
    T* operator =(const ISymBase* p) throw()
    {
        CSymPtr<T>::Release();
        if (p != NULL)
        {
            p->QueryInterface(*piid, (void**)&CSymPtr<T>::m_p);
        }
        return CSymPtr<T>::m_p;
    }
};

// Specialization for ISymBase
template<>
class CSymQIPtr<ISymBase, &IID_SymBase> : public CSymPtr<ISymBase>
{
public:
    CSymQIPtr() throw()
    {
    }
    CSymQIPtr(int nNull) throw()
        : CSymPtr<ISymBase>(nNull)
    {
    }
    CSymQIPtr(const CSymQIPtr<ISymBase, &IID_SymBase>& p) throw()
        : CSymPtr<ISymBase>(p)
    {
    }
    CSymQIPtr(const ISymBase* p) throw()
    {
        m_p = NULL;
        *this = p;
    }
    ISymBase* operator =(int nNull) throw()
    {
        return CSymPtr<ISymBase>::operator =(nNull);
    }
    ISymBase* operator =(const CSymQIPtr<ISymBase, &IID_SymBase>& p) throw()
    {
        if (&p == this)
        {
            return m_p;
        }
        return CSymPtr<ISymBase>::operator =(p.m_p);
    }
    ISymBase* operator =(const ISymBase* p) throw()
    {
        Release();
        if (p != NULL)
        {
            p->QueryInterface(IID_SymBase, (void**)&m_p);
        }
        return m_p;
    }
};

///////////////////////////////////////////////////////////////////////////////
// template < class T > ISymBaseImpl
//
// Default implementation of ISymBase.  This class handles reference counting,
// and interface management. Derive from this class, and add a section to your
// class that looks like this:
//
//  class CFoo: public ISymBaseImpl< CSymRefCount >,
//              public IFoo
//  {
//          .
//          .
//
//      SYM_INTERFACE_MAP_BEGIN()
//          SYM_INTERFACE_ENTRY( IID_Foo, IFoo )
//      SYM_INTERFACE_MAP_END()
//          .
//          .
//      // IFoo Methods...
//  };
//
// Template arguments:
//      class TRefCount - Must be either CSymRefCount or CSymThreadSafeRefCount.
//

template < class TRefCount >
class ISymBaseImpl
{
public:
    // Reference count manipulator.
    typedef TRefCount _RefCountClass;

public:
    ISymBaseImpl() throw()
    { 
        m_nRefCount = 0;
        extern long g_DLLObjectCount;
        InterlockedIncrement( &g_DLLObjectCount );
    }
    virtual ~ISymBaseImpl() throw()
    {
        extern long g_DLLObjectCount;
        InterlockedDecrement( &g_DLLObjectCount );
    }

    virtual void FinalRelease() throw()
    {
    }

    // Reference count
    mutable size_t m_nRefCount;
};


///////////////////////////////////////////////////////////////////////////////
// Module Lifetime Manager Interface Declaration.
//
namespace cc
{
    // /////////////////////////////////////////////////////////////////////////////////////////////////
    // Interface Declaration : IModuleLifetimeMgr
    // //////////////////////
    // Description : Interface for creating objects and decrementing object count on managed modules.
    // /////////////////////////////////////////////////////////////////////////////////////////////////
    class IModuleLifetimeMgr : public ISymBase
    {
    public:

        //
        // This enumeration allows a client to specify special behavior for the creation of
        // an object.  Note that the CREATE_USE_TRUST_CACHE option will only be valid if
        // the CREATE_WITH_TRUST option is set.
        // 
        typedef enum
        {
            CREATE_WITH_TRUST = 0x1,
            CREATE_USE_TRUST_CACHE = 0x2,
        } MANAGER_CREATE_OPTIONS;

        //
        // This create object method will load the specified DLL and create the appropriate object.  
        // If the specified DLL is already loaded in memory, it will keep the existing reference, 
        // but create the required object.  These methods will call the CreateObject overloads below
        // without any MANAGER_CREATE_OPTIONS set.
        //
        virtual SYMRESULT CreateObject(LPCSTR szDllPath, 
                                       REFSYMOBJECT_ID objectID, 
                                       REFSYMINTERFACE_ID iid, 
                                       void** ppvOut) throw() = 0;

        virtual SYMRESULT CreateObject(LPCWSTR szDllPath, 
                                       REFSYMOBJECT_ID objectID, 
                                       REFSYMINTERFACE_ID iid, 
                                       void** ppvOut) throw() = 0;

        //
        // This create object method allows the client to specify creation options.  The dwOptions 
        // parameter should be a bitmask of MANAGER_CREATE_OPTIONS.
        //
        virtual SYMRESULT CreateObject(LPCSTR szDllPath, 
                                       REFSYMOBJECT_ID objectID, 
                                       REFSYMINTERFACE_ID iid, 
                                       DWORD dwOptions,
                                       void** ppvOut) throw() = 0;

        virtual SYMRESULT CreateObject(LPCWSTR szDllPath, 
                                       REFSYMOBJECT_ID objectID, 
                                       REFSYMINTERFACE_ID iid, 
                                       DWORD dwOptions,
                                       void** ppvOut) throw() = 0;

        //
        // This method will search through all loaded DLLs to determine if the requested object id is 
        // in one of the modules already loaded.
        //
        virtual SYMRESULT CreateObject(HMODULE hModule,
                                       REFSYMOBJECT_ID objectID, 
                                       REFSYMINTERFACE_ID iid, 
                                       void** ppvOut) throw() = 0;    

        //
        // This create object method allows the client to specify creation options.  The dwOptions 
        // parameter should be a bitmask of MANAGER_CREATE_OPTIONS.
        //
        virtual SYMRESULT CreateObject(HMODULE hModule,
                                       REFSYMOBJECT_ID objectID, 
                                       REFSYMINTERFACE_ID iid, 
                                       DWORD dwOptions,
                                       void** ppvOut) throw() = 0;    

        //
        // This method will decrement the module managed object count for the specified module.  
        // It will also call GarbageCollection to free all modules with a ref count of 0, except
        // for the HMODULE passed into this method.
        //
        virtual SYMRESULT DecrementModuleObjectCount(HMODULE hModule) throw() = 0;

        //
        // This method will increment the module managed object count for the specified module.  
        // If the specified module is not registered, it will return SYMERR_INVALIDARG.
        virtual SYMRESULT IncrementModuleObjectCount(HMODULE hModule) throw() = 0;
    };

    SYM_DEFINE_INTERFACE_ID(IID_ModuleLifetimeMgr, 0x8f7c6659, 0x5b5e, 0x4905, 0xaf, 0x2c, 0x30, 0x63, 0x3f, 0xd6, 0x2b, 0x1a);

    typedef CSymPtr<IModuleLifetimeMgr> IModuleLifetimeMgrPtr;
    typedef CSymQIPtr<IModuleLifetimeMgr, &IID_ModuleLifetimeMgr> IModuleLifetimeMgrQIPtr;

} // cc

#ifdef WIN32
// from ATL 7.0 sources
#ifndef _delayimp_h
    extern "C" IMAGE_DOS_HEADER __ImageBase;
#endif // _delayimp_h
#endif // WIN32

///////////////////////////////////////////////////////////////////////////////
// Interface map macros for ISymBaseImpl.
//

#define SYM_INTERFACE_MAP_BEGIN() \
    protected: \
        ::cc::IModuleLifetimeMgrPtr m_pModuleLifetimeMgr; \
    public:\
        virtual SYMRESULT QueryInterface( REFSYMINTERFACE_ID iid, void** pItfc ) const throw() \
    {\
            (void)iid; \
            SAVASSERT( pItfc ); \
            if( pItfc == NULL ) \
            { \
                return SYMERR_INVALIDARG; \
            }

#define SYM_INTERFACE_ENTRY(itfc,cls) \
        if( SymIsEqualIID( IID_SymBase, iid ) || SymIsEqualIID( itfc, iid ) )\
        {\
                *pItfc = const_cast< cls* >(static_cast<const cls*> (this)); \
        } else

#define SYM_INTERFACE_TEAR_OFF(itfc,cls,tear) \
        if(  SymIsEqualIID( itfc, iid ) )\
        {\
                *pItfc = const_cast< cls* >(static_cast<const cls*> (tear)); \
        } else

#define SYM_INTERFACE_MAP_CHAIN(cls) \
        { \
        } \
        if( *pItfc != NULL ) \
        {\
            AddRef();\
            return SYM_OK;\
        }\
        return cls::QueryInterface(iid, pItfc); \
    } \
        virtual size_t AddRef() const throw() \
    { \
        return cls::AddRef(); \
    } \
        virtual size_t Release() const throw() \
    { \
        return cls::Release(); \
        } \
        virtual ::cc::IModuleLifetimeMgr* GetModuleManager() const throw() \
        { \
            return cls::GetModuleManager(); \
        } \
        virtual void SetModuleManager(::cc::IModuleLifetimeMgr* pMgr) throw() \
        { \
            return cls::SetModuleManager(pMgr); \
        } \
        virtual void AttachModuleManager(::cc::IModuleLifetimeMgr* pMgr) throw() \
        { \
            return cls::AttachModuleManager(pMgr); \
    }


#define SYM_INTERFACE_MAP_END() \
        {\
            *pItfc = NULL; \
        }\
        if( *pItfc != NULL ) \
        {\
            AddRef();\
            return SYM_OK;\
        }\
        return SYMERR_NOINTERFACE;\
    } \
        virtual size_t AddRef() const throw() \
    { \
        return _RefCountClass::AddRef(this); \
    } \
        virtual size_t Release() const throw() \
        { \
            SAVASSERT(m_nRefCount > 0); \
            size_t nTempRefCount = _RefCountClass::Release(this); \
            if( nTempRefCount == 0 ) \
            { \
                const_cast<ISymBaseImpl<_RefCountClass>* >( static_cast< const ISymBaseImpl<_RefCountClass> * >(this) )->FinalRelease(); \
                /* Store a temporary pointer to use after deletion. */ \
                ::cc::IModuleLifetimeMgrPtr pModuleLifetimeMgr = m_pModuleLifetimeMgr; \
                delete this; \
                if( pModuleLifetimeMgr ) \
                { \
                    /*No __ImageBase on Netware*/ \
                    /*pModuleLifetimeMgr->DecrementModuleObjectCount(reinterpret_cast<HMODULE>(&__ImageBase));*/ \
                    SAVASSERT (false); \
                } \
            } \
            return nTempRefCount; \
        } \
        virtual ::cc::IModuleLifetimeMgr* GetModuleManager() const throw() \
        { \
            return m_pModuleLifetimeMgr; \
        } \
        virtual void SetModuleManager(::cc::IModuleLifetimeMgr* pMgr) throw() \
        { \
            m_pModuleLifetimeMgr = pMgr; \
        } \
        virtual void AttachModuleManager(::cc::IModuleLifetimeMgr* pMgr) throw() \
        { \
            SetModuleManager(pMgr); \
            if(m_pModuleLifetimeMgr != NULL) \
            { \
                /*No __ImageBase on Netware*/ \
                /*m_pModuleLifetimeMgr->IncrementModuleObjectCount(reinterpret_cast<HMODULE>(&__ImageBase));*/ \
                SAVASSERT (false); \
            } \
    }

#define SYM_INTERFACE_TEAR_OFF_CHAIN(pParent) \
        virtual SYMRESULT QueryInterface(REFSYMINTERFACE_ID iid, void** ppvObject) const throw() \
    { \
        return pParent->QueryInterface(iid, ppvObject); \
    } \
        virtual size_t AddRef() const throw() \
    { \
        return pParent->AddRef(); \
    } \
        virtual size_t Release() const throw() \
    { \
        return pParent->Release(); \
        } \
        virtual ::cc::IModuleLifetimeMgr* GetModuleManager() const \
        { \
            return pParent->GetModuleManager(); \
        } \
        virtual void SetModuleManager(::cc::IModuleLifetimeMgr* pMgr) \
        { \
            return pParent->SetModuleManager(pMgr); \
        } \
        virtual void AttachModuleManager(::cc::IModuleLifetimeMgr* pMgr) \
        { \
            return pParent->AttachModuleManager(pMgr); \
    }

///////////////////////////////////////////////////////////////////////////////
// Reference count classes.  These simple classes help manage reference
// counting in ISymBaseImpl derived classes.
//

#if defined (_SYM_DEBUG_INTERFACES)
#  define DEBUG_SYM_INTERFACE(p, nRefCount)  \
    __if_exists(::ccLib::TraceInformation)  \
    {   \
        CCTRCTXI2(_T("address:0x%08x, refcnt:%d"), p, nRefCount); \
    }
#else
#  define DEBUG_SYM_INTERFACE(p, nRefCount)
#endif

class CSymThreadSafeRefCount
{
public:
    static size_t AddRef( const ISymBaseImpl<CSymThreadSafeRefCount>* p ) throw()
    {
#if defined( WIN32 )
        SAVASSERT(p);
#ifdef _WIN64
        size_t nRefCount = InterlockedIncrement64( reinterpret_cast<LONGLONG*>( &p->m_nRefCount ) );
#else
        size_t nRefCount = InterlockedIncrement( reinterpret_cast<LONG*>( &p->m_nRefCount ) );
#endif
        DEBUG_SYM_INTERFACE(p, nRefCount);
        return nRefCount;
#else
        //TODO: Need a Unix version someday.
        return ++p->m_nRefCount;
#endif
    }
    
    static size_t Release( const ISymBaseImpl<CSymThreadSafeRefCount>* p ) throw()
    {
#if defined( WIN32 )
        SAVASSERT(p);
#ifdef _WIN64
        size_t nRefCount = InterlockedDecrement64( reinterpret_cast<LONGLONG*>(&p->m_nRefCount) );
#else
        size_t nRefCount = InterlockedDecrement( reinterpret_cast<LONG*>(&p->m_nRefCount) );
#endif
        DEBUG_SYM_INTERFACE(p, nRefCount);
        return nRefCount;
#else
        //TODO: Need a Unix version someday.
        return --p->m_nRefCount;
#endif
    }
} ;

// There is no good reason to not have a thread safe reference counting class
// We no longer support non-thread safe reference counting
typedef CSymThreadSafeRefCount CSymRefCount;

// Use of CSymStaticRefCount is discouraged due to how easily it is abused,
// and abuse often results in crashes due to the object being in use while out of scope.
template <class T>
class CSymStaticRefCount : public T
{
    virtual size_t AddRef() const throw()
    { 
        // For code that verifies that the create refcount is 1 
        // by calling AddRef() and making sure the return is greater than 1
        // we use the value 2 instead of 1
        return 2;
    } 
    virtual size_t Release() const throw()
    { 
        return 2;
    }
};

#ifdef __AFX_H__
#define SYMINTCATCHMEMORYEXECPTION() \
    catch( CMemoryException* e ) { e->Delete();} \
    catch( std::bad_alloc& ) {}
#else
#define SYMINTCATCHMEMORYEXECPTION() \
    catch( std::bad_alloc& ) {}
#endif

///////////////////////////////////////////////////////////////////////////////
// template< class T > class CSymFactoryImpl
//
// Default implementation of ISymFactory.  To use, simply derive from this class
// specifying the class that implements the object to be created by the factory
// as the template argument.   Example:
//
//  class CMyFactory : public CSymFactoryImpl< CMyObj >
//  {
//      .   
//      .   
//      .   
//  };
//
// In your component DLL's implementation of GetFactory(), return an instance
// of your derived class (AddRef'd of course) to clients.
//

template< class T > 
class CSymFactoryImpl : 
    public ISymBaseImpl< CSymThreadSafeRefCount >,
    public ISymFactory2
{
public:
    // Class typedef
    typedef T _ClassType;

    SYM_INTERFACE_MAP_BEGIN()               
        SYM_INTERFACE_ENTRY( IID_SymFactory, ISymFactory )
        SYM_INTERFACE_ENTRY( IID_SymFactory2, ISymFactory2 )
    SYM_INTERFACE_MAP_END()                 

    // ISymFactory
    virtual SYMRESULT CreateInstance( REFSYMINTERFACE_ID iid, void** ppvObject ) throw()
    {
        return CreateInstance(iid, ppvObject, NULL);
    }

    // ISymFactory2
    virtual SYMRESULT CreateInstance( REFSYMINTERFACE_ID iid, void** ppvObject, ::cc::IModuleLifetimeMgr* pModuleMgr ) throw()
    {
        SAVASSERT( ppvObject );

        if( ppvObject == NULL )
            return SYMERR_INVALIDARG;
        
        *ppvObject = NULL;
        
        _ClassType* p = NULL;
        try
        {
            p = new _ClassType;
        }
        SYMINTCATCHMEMORYEXECPTION()

        if( p == NULL )
            return SYMERR_OUTOFMEMORY;
        
        SYMRESULT result = p->QueryInterface( iid, ppvObject );
        if( SYM_FAILED ( result ) )
        {
            delete p;
            return result;
        }

        // Assign the passed in module manager to this.
        if(NULL != pModuleMgr)
            p->SetModuleManager(pModuleMgr);

        return result;
    }
};

// This  class exists to allow us to create a factory that 
// only provides for iteration of the ids in an object.
// Calling create instance on this class will fail since it does not actually
// support any exposed interfaces.
class CSymObjectIterator : 
    public ISymBaseImpl< CSymThreadSafeRefCount >
{
    // This class simply exists to allow us to get a generic factory object.
    SYM_INTERFACE_MAP_BEGIN()
    SYM_INTERFACE_MAP_END()
};

///////////////////////////////////////////////////////////////////////////////
// Object map macros.  Use these to quickly implement object creation in 
// component DLLs.  Here is what you need to do:
//
// 1) Create your component object class implementation (eg. CMyObj, implements
//    IMyObj).
//
// 2) In your project, add an object map:
//      
//      SYM_OBJECT_MAP_BEGIN()                           
//          SYM_OBJECT_ENTRY( MYOBJ_OBJECT_ID, CMyObj )
//      SYM_OBJECT_MAP_END()                             
//
// 3) Make sure you include a .DEF file in your project that exports GetFactory()
//    and GetObjectCount().  Example:
//    
//      LIBRARY   MYOBJ                 
//      DESCRIPTION "Implements MYOBJ" 
//      EXPORTS                              
//         GetFactory           @1           
//         GetObjectCount       @2           
//
// 4) It's just that easy!
//

//Note: Name change: because netware dynamic import doesn't link export names with it's NLM we must
//use a unique name, so "Sym" was added to our export.
#ifdef SYM_LIBC_NLM
#define SYM_OBJECT_MAP_BEGIN() \
    long g_DLLObjectCount = 0; \
    extern "C" long WINAPI SymGetObjectCount( void ) \
    { return g_DLLObjectCount; } \
    extern "C" SYMRESULT WINAPI SymGetFactory( REFSYMOBJECT_ID objectID, ISymFactory** ppvFactory ) \
    { \
        objectID; \
        if(ppvFactory == NULL) \
            return SYMERR_INVALIDARG;
#else
#define SYM_OBJECT_MAP_BEGIN() \
    long g_DLLObjectCount = 0; \
    extern "C" long WINAPI GetObjectCount( void ) \
    { \
        return g_DLLObjectCount; \
    } \
    extern "C" SYMRESULT WINAPI GetFactory( REFSYMOBJECT_ID objectID, ISymFactory** ppvFactory ) \
    { \
        objectID; \
        if(ppvFactory == NULL) \
            return SYMERR_INVALIDARG;
#endif // SYM_LIBC_NLM

#define SYM_OBJECT_ENTRY( id, cls ) \
    if( SymIsEqualIID( objectID, id ) ) \
    { \
        CSymFactoryImpl<cls>* pFactory = NULL;\
        try \
        { \
            pFactory = new CSymFactoryImpl<cls>; \
        } \
        SYMINTCATCHMEMORYEXECPTION()\
        if( pFactory == NULL ) \
            return SYMERR_OUTOFMEMORY; \
        *ppvFactory = pFactory; \
        pFactory->AddRef(); \
        return SYM_OK; \
    }

#define SYM_OBJECT_MAP_END() \
        return SYMERR_INVALIDARG; \
    }

///////////////////////////////////////////////////////////////////////////////
// Functions prototypes for exported exports from SymComponent DLLs.  

extern "C"
{
    // Function prototype and pointer for GetFactory() method.

    //Note: Name change: because netware dynamic import doesn't link export names with it's NLM we must
    //use a unique name, so "Sym" was added to our export
    SYMRESULT WINAPI SymGetFactory( REFSYMOBJECT_ID objectID, ISymFactory** ppvFactory );
    typedef SYMRESULT ( WINAPI *pfnGETFACTORY )( REFSYMOBJECT_ID objectID, ISymFactory** ppvFactory );

    // Function prototype pointer for GetObjectCount() method.
    long WINAPI SymGetObjectCount( void );
    typedef SYMRESULT ( WINAPI *pfnGETOBJECTCOUNT)( void );
};

///////////////////////////////////////////////////////////////////////////////

typedef CSymPtr<ISymBase> ISymBasePtr;
typedef CSymQIPtr<ISymBase, &IID_SymBase> ISymBaseQIPtr;

typedef CSymPtr<ISymFactory> ISymFactoryPtr;
typedef CSymQIPtr<ISymFactory, &IID_SymFactory> ISymFactoryQIPtr;

typedef CSymPtr<ISymFactory2> ISymFactory2Ptr;
typedef CSymQIPtr<ISymFactory2, &IID_SymFactory2> ISymFactory2QIPtr;

#endif // #define _SYMINTERFACE_H_
