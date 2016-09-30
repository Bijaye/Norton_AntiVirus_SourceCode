// navprod.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

#define INITIIDS
#include "ccEventManagerHelperEx.h"

#include "ccAppPlugin.h"
#include "navproductplugin.h"

HINSTANCE g_hInstance;

//Resource loading code
#include "ccResourceLoader.h"
#define SYMC_RESOURCE_DLL _T("navprod.loc")
CAtlBaseModule _Module;
::cc::CResourceLoader g_ResLoader(&_Module, SYMC_RESOURCE_DLL);

void _InitResources();
#define INIT_RESOURCES() \
    _InitResources();

//////////////////////////////////////////////////////////////////////
// Declare plugin.

SYM_DEFINE_GUID(NAVPRODUCT_OBJECT_UUID, 
	0x6663edd, 0x150d, 0x4262, 0x88, 0xbe, 0xb6, 0x5e, 0xa, 0xc3, 0x62, 0xc3);
DECLARE_CCAPP_PLUGIN( NAVPRODUCT_OBJECT_UUID );

//////////////////////////////////////////////////////////////////////
// Object map.

SYM_OBJECT_MAP_BEGIN()
	INIT_RESOURCES()
    SYM_OBJECT_ENTRY( NAVPRODUCT_OBJECT_UUID, CNavProductPlugin )
SYM_OBJECT_MAP_END()

//////////////////////////////////////////////////////////////////////
// DllMain()

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    if ( ul_reason_for_call == DLL_PROCESS_ATTACH )
    {
        // Performance Optimization: see MSDN docs on DisableThreadLibraryCalls() func.
        DisableThreadLibraryCalls((HMODULE)hModule);

        g_hInstance = (HINSTANCE)hModule;

#ifdef _DEBUG
        ::MessageBox ( ::GetDesktopWindow (), "Starting...", "navprod", MB_OK );
#endif

    }
	
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// _InitResources - Sets up resource loading
/////////////////////////////////////////////////////////////////////////////
void _InitResources()
{
    if(NULL == g_ResLoader.GetResourceInstance())
    {
        if(g_ResLoader.Initialize())
        {
            _Module.SetResourceInstance(g_ResLoader.GetResourceInstance());
        }
    }

}
