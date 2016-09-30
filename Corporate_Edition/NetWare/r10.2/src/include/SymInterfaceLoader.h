// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004, 2005 Symantec Corporation.. All rights reserved.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
// SymInterfaceLoader.h 
//

#pragma once

//#include <shlwapi.h>

#include "SymInterface.h"

#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif

//#pragma comment(lib, "shlwapi.lib")

#define SYMERR_PATH_NOT_FOUND			SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x300 )
#define SYMERR_MISSING_EXPORTS			SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x302 )
#define SYMERR_INVALID_FILE				SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x303 )

///////////////////////////////////////////////////////////////////////////////
// class CSymInterfaceLoader - A simple class that wraps a SymInterface DLL.
//
// NOTE: Since this class contains the handle to the DLL, and this handle is
// free'd when the object goes out of scope, be sure to release all objects
// implemented by the DLL!   If you don't, you get asserts and crashes aplenty...

class CSymInterfaceLoader
{
public:
	CSymInterfaceLoader() : 
		m_hMod( NULL ),
		m_pfnGetFactory( NULL ),
		m_pfnGetObjectCount( NULL ),
        m_bUnimport( FALSE )
	{
	}

	virtual ~CSymInterfaceLoader()
	{
		clear();
	}

private:
    CSymInterfaceLoader(const CSymInterfaceLoader& SymInterfaceLoader);
	CSymInterfaceLoader& operator =(const CSymInterfaceLoader& SymInterfaceLoader);

public:
	SYMRESULT Initialize( LPCTSTR pszDllPath ) throw()
	{
		// Handle case were we have already been initialized.
		if( m_pfnGetFactory && m_pfnGetObjectCount )
			return SYM_OK;

		// Validate input.
		if( pszDllPath == NULL ) 
			return SYMERR_INVALIDARG;

		// Clean up pointers.
		clear();
		
		// Make sure the dll is valid.
		SYMRESULT result = Validate( pszDllPath );
        if( SYM_FAILED( result ) )
			return result;

		// Load the DLL.
		m_hMod = LoadLibraryEx( pszDllPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );
		if( m_hMod == NULL )
			return SYMERR_PATH_NOT_FOUND;

		// Fetch the exports.
		result = Initialize( m_hMod );
		if( SYM_FAILED( result ) )
		{
			FreeLibrary( m_hMod );
			m_hMod = NULL;
		}

		return result;
	}
	
	// Alternate initialization path.  Does NOT free the HMODULE.
	SYMRESULT Initialize( HMODULE hMod ) throw()
	{
		// Handle case where we have already been initialized.	
		if( m_pfnGetFactory && m_pfnGetObjectCount )
			return SYM_OK;

        // request UnimportSymbol
        m_bUnimport = TRUE;

		// Locate exports.
		m_pfnGetFactory = (pfnGETFACTORY) GetProcAddress( hMod, "SymGetFactory" );
		m_pfnGetObjectCount = (pfnGETOBJECTCOUNT) GetProcAddress( hMod, "SymGetObjectCount" );
		if( m_pfnGetFactory == NULL ||	m_pfnGetObjectCount == NULL )
		{
			// No exports is a fatal error.
			return SYMERR_MISSING_EXPORTS;
		}

		// All is well.
		return SYM_OK;
	}

	// Call this routine to create objects.
	SYMRESULT CreateObject( REFSYMOBJECT_ID objectID, REFSYMINTERFACE_ID iid, void** ppvOut ) throw()
	{
		// Validate.
		if( ppvOut == NULL || m_pfnGetFactory == NULL )
			return SYMERR_INVALIDARG;
	
		// Clear output
		*ppvOut = NULL;

		// Create the proper class factory for this object.
		CSymPtr<ISymFactory> pFactory;
		SYMRESULT result = m_pfnGetFactory( objectID, (ISymFactory**) &pFactory );
		if( SYM_SUCCEEDED( result ) )
		{
			// Create the object.
			result = pFactory->CreateInstance( iid, ppvOut );
		}

		return result;
	}

	// Returns the Dlls global object count.
	long GetObjectCount() throw()
	{
		if( m_pfnGetObjectCount )
			return m_pfnGetObjectCount();
		
		return 0;
	}

	// Override this to perform digita signature check.
	virtual SYMRESULT Validate( LPCTSTR pszDllPath ) throw()
	{
        UNREFERENCED_PARAMETER(pszDllPath);
		return SYM_OK;
	}

protected:
	void clear()
	{
        // this clears the dependency we have on the factory NLM so it can be unloaded.

        if ( m_bUnimport ) {
            (void) UnimportSymbol( GetNLMHandle( ), "SymGetFactory" );
            (void) UnimportSymbol( GetNLMHandle( ), "SymGetObjectCount" );
        }

        if( m_hMod )
		{
            FreeLibrary( m_hMod );
			m_hMod = NULL;
		}

		m_pfnGetFactory = NULL;
		m_pfnGetObjectCount = NULL;
	}

protected:
	// Handle to the DLL.
	HMODULE				m_hMod;

    // for Netware we always need to unimport the symbol when we are done - 

    BOOL                m_bUnimport;

	// Pointer to GetFactory export.
	pfnGETFACTORY		m_pfnGetFactory;

	// Pointer to GetObjectCount export.
	pfnGETOBJECTCOUNT	m_pfnGetObjectCount;
};


///////////////////////////////////////////////////////////////////////////////
// class CSymInterfaceHelper - Create a specific interface type from a DLL
// E.g. 
// typedef CSymInterfaceHelper<CSymInterfaceLoader, IFoo, OID_IFoo, IID_IFOO> CFooLoader;
// CFooLoader fooLoader;
// fooLoader.Initialize("C:\\Temp\\FOO.DLL");
// IFoo* pFoo;
// fooLoader.CreateObject(pFoo);
// 
template <class TLoader, 
          class TInterface, 
          const SYMOBJECT_ID* pOID, 
          const SYMINTERFACE_ID* pIID>
class CSymInterfaceHelper : public TLoader
{
public:
    CSymInterfaceHelper() throw() {}
    virtual ~CSymInterfaceHelper() throw() {}

private:
    CSymInterfaceHelper(const CSymInterfaceHelper&) throw();
    CSymInterfaceHelper& operator =(const CSymInterfaceHelper&) throw();

public:
    SYMRESULT CreateObject(TInterface** ppInterface) throw()
    {
        return TLoader::CreateObject(*pOID, *pIID, (void**)ppInterface);
    }
    SYMRESULT CreateObject(TInterface*& pInterface) throw()
    {
        return CreateObject(&pInterface);
    }
};

///////////////////////////////////////////////////////////////////////////////
// class CSymInterfacePathHelper - Create a specific interface type from a DLL
// by only passing the DLL name to Initialize(). The path is returned from the 
// CSymInterfacePathProvider type.
// E.g. 
//class CPathProvider
//{
//public:
//    static bool GetPath(LPTSTR szPath, size_t& nSize) throw()
//    {
//        lstrcpy(szPath, _T("C:\\Temp"));
//        return true;
//    }
//};
// typedef CSymInterfacePathHelper<CPathProvider, CSymInterfaceLoader, IFoo, OID_IFoo, IID_IFOO> CFooLoader;
// CFooLoader fooLoader;
// fooLoader.Initialize("FOO.DLL");
// IFoo* pFoo;
// fooLoader.CreateObject(pFoo);
// 

template <class TPathProvider, 
          class TLoader, 
          class TInterface, 
          const SYMOBJECT_ID* pOID, 
          const SYMINTERFACE_ID* pIID>
class CSymInterfacePathHelper : public CSymInterfaceHelper<TLoader, TInterface, pOID, pIID>
{
public:
    typedef CSymInterfaceHelper<TLoader, TInterface, pOID, pIID> TBase;

public:
    CSymInterfacePathHelper() throw() {}
    virtual ~CSymInterfacePathHelper() throw() {}

private:
    CSymInterfacePathHelper(const CSymInterfacePathHelper&) throw();
    CSymInterfacePathHelper& operator =(const CSymInterfacePathHelper&) throw();

public:
    SYMRESULT Initialize(LPCTSTR szDLLName) throw()
    {
        // Get the base directory from the path provider
        TCHAR szPath[_MAX_PATH + 1] = {0};
        size_t nSize = sizeof(szPath);
        if (TPathProvider::GetPath(szPath, nSize) == false)
        {
            return SYMERR_PATH_NOT_FOUND;
        }

        // Append the DLL name to the path
        PathAddBackslash(szPath);
        lstrcat(szPath, szDLLName);

        return TBase::Initialize(szPath);
    }
};

template <const LPCTSTR* szDLLName,
          class TPathProvider, 
          class TLoader, 
          class TInterface, 
          const SYMOBJECT_ID* pOID, 
          const SYMINTERFACE_ID* pIID>
class CSymInterfaceDLLHelper : public CSymInterfacePathHelper<TPathProvider, 
                                                              TLoader, 
                                                              TInterface, 
                                                              pOID, 
                                                              pIID>
{
public:
    typedef CSymInterfacePathHelper<TPathProvider,
                                    TLoader, 
                                    TInterface, 
                                    pOID, 
                                    pIID> TBase;

public:
    CSymInterfaceDLLHelper() throw() {}
    virtual ~CSymInterfaceDLLHelper() throw() {}

private:
    CSymInterfaceDLLHelper(const CSymInterfaceDLLHelper&) throw();
    CSymInterfaceDLLHelper& operator =(const CSymInterfaceDLLHelper&) throw();

public:
    SYMRESULT CreateObject(TInterface** ppInterface) throw()
    {
        if (TBase::m_hMod == NULL)
        {
            SYMRESULT symRes = Initialize();
            if (SYM_FAILED(symRes))
            {
                return symRes;
            }
        }
        return TBase::CreateObject(ppInterface);
    }
    SYMRESULT CreateObject(TInterface*& pInterface) throw()
    {
        return CreateObject(&pInterface);
    }
    SYMRESULT Initialize() throw()
    {
        return TBase::Initialize(*szDLLName);
    }
};

