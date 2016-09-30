////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <atlbase.h>
#include <atlconv.h>

#pragma warning( disable : 4290 )

// Library headers
#include "ccLibStaticLink.h"

// Import headers
#define INITIIDS
#include "SymInterface.h"
#include "ccIndexValueCollectionInterface.h"
#include "ccKeyValueCollectionInterface.h"
#include "ccMemoryInterface.h"
#include "ccSymKeyValueCollectionImpl.h"
#include "ccStringInterface.h"
#include "ccSymStringImpl.h"
#include "ccInstanceFactory.h"	// who uses this??

#include "ccLibStatic.h"

#include "n32exclu.h"
#include "ExclusionInterface.h"
#include "CFileSystemExclusion.h"
#include "CAnomalyExclusion.h"
#include "CExclusionFactory.h"
#include "CExclusionManager.h"
#include "ExclusionManagerLoader.h"

#include "ccTrace.h"
#include "ccSymDebugOutput.h"
ccSym::CDebugOutput g_DebugOutput(_T("N32EXCLU"));
IMPLEMENT_CCTRACE(::g_DebugOutput);

HINSTANCE g_hInstance = NULL;

BOOL APIENTRY DllMain( HINSTANCE hInstance, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    if (DLL_PROCESS_ATTACH == ul_reason_for_call)
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls(hInstance);

        g_hInstance = hInstance;
    }
    else if (DLL_PROCESS_DETACH == ul_reason_for_call)
    {
        g_hInstance = NULL;
    }

    return TRUE;
}

// Object Map for SymInterface
SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_ENTRY(NavExclusions::IID_IFileSystemExclusion, NavExclusions::CFileSystemExclusion)
SYM_OBJECT_ENTRY(NavExclusions::IID_IAnomalyExclusion,    NavExclusions::CAnomalyExclusion)
SYM_OBJECT_ENTRY(NavExclusions::IID_IExclusionManager,    NavExclusions::CExclusionManager)
SYM_OBJECT_MAP_END()

/*
long g_DLLObjectCount = 0;

extern "C" long WINAPI GetObjectCount( void ) 
{ return g_DLLObjectCount; }

extern "C" SYMRESULT WINAPI GetFactory( REFSYMOBJECT_ID objectID, ISymFactory** ppvFactory ) 
{ 
objectID;
ppvFactory; 

if( SymIsEqualIID( objectID, NavExclusions::IID_IFileSystemExclusion ) )
{
CSymFactoryImpl<NavExclusions::CFileSystemExclusion>* pFactory = NULL;

pFactory = new CSymFactoryImpl<NavExclusions::CFileSystemExclusion>; 


if( pFactory == NULL ) 
return SYMERR_OUTOFMEMORY; 

*ppvFactory = pFactory; 
pFactory->AddRef();
return SYM_OK;
}
if( SymIsEqualIID( objectID, NavExclusions::IID_IAnomalyExclusion ) )
{
CSymFactoryImpl<NavExclusions::CAnomalyExclusion>* pFactory = NULL;

pFactory = new CSymFactoryImpl<NavExclusions::CAnomalyExclusion>; 


if( pFactory == NULL ) 
return SYMERR_OUTOFMEMORY; 

*ppvFactory = pFactory; 
pFactory->AddRef();
return SYM_OK;
}
if( SymIsEqualIID( objectID, NavExclusions::IID_IExclusionManager ) )
{
CSymFactoryImpl<NavExclusions::CExclusionManager>* pFactory = NULL;

pFactory = new CSymFactoryImpl<NavExclusions::CExclusionManager>; 


if( pFactory == NULL ) 
return SYMERR_OUTOFMEMORY; 

*ppvFactory = pFactory; 
pFactory->AddRef();
return SYM_OK;
}

return SYMERR_INVALIDARG;
}
*/