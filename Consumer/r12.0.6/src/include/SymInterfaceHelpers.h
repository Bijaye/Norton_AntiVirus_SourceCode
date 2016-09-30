// SymInterfaceHelper.h 
//

#pragma once

#include "syminterface.h"

#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif

#define SYMERR_PATH_NOT_FOUND			SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x300 )
#define SYMERR_MISSING_EXPORTS			SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x302 )
#define SYMERR_INVALID_FILE				SYM_MAKE_RESULT_INTERNAL( SYM_ERROR, 0x303 )

///////////////////////////////////////////////////////////////////////////////
// class CSymInterfaceDllHelper - A simple class that wraps a SymIntrface DLL.
//
// NOTE: Since this class contains the handle to the DLL, and this handle is
// free'd when the object goes out of scope, be sure to release all objects
// implemented by the DLL!   If you don't, you get asserts and crashes aplenty...

class CSymInterfaceDllHelper
{
public:
	CSymInterfaceDllHelper() : 
		m_hMod( NULL ),
		m_pfnGetFactory( NULL ),
		m_pfnGetObjectCount( NULL )
	{
	}

	virtual ~CSymInterfaceDllHelper()
	{
		clear();
	}

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

		// Locate exports.
		m_pfnGetFactory = (pfnGETFACTORY) GetProcAddress( hMod, "GetFactory" );
		m_pfnGetObjectCount = (pfnGETOBJECTCOUNT) GetProcAddress( hMod, "GetObjectCount" );
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

	// Override this to perform digital signature check.
	virtual SYMRESULT Validate( LPCTSTR pszDllPath ) throw()
	{
		return SYM_OK;
	}

protected:
	void clear()
	{
		if( m_hMod )
		{
            FreeLibrary( m_hMod );
			m_hMod = NULL;
		}

		m_pfnGetFactory = NULL;
		m_pfnGetObjectCount = NULL;
	}

private:
	// Handle to the DLL.
	HMODULE				m_hMod;

	// Pointer to GetFactory export.
	pfnGETFACTORY		m_pfnGetFactory;

	// Pointer to GetObjectCount export.
	pfnGETOBJECTCOUNT	m_pfnGetObjectCount;
};
