// DLLModuleState.h : header file
//
// This gets around the MFC Extension DLL limitation of not searching for
// resources in the current (or specified) module first.
// 
// To use this:
//    - Include this header in every source file within the DLL that needs resources from it.
//    - Include this header in source files external to modules but #define DLLINST_EXTERNAL_MODULE.
//      This allows these source files to use the SetResourceHandleTo(...) macro.
//    - Add SetResourceHandleToThisModule() macro call before any resource loading calls.
//    - When accessing resources via other modules, call SetResourceHandleTo(...) using an instance
//      handle passed to the module (via a call or variable, you decide).
//    - Change your DLLMain and the declaration above it as noted in the code 
//      that follows...
//
///*static*/ AFX_EXTENSION_MODULE EXTENSIONDLL = { NULL, NULL }; //**Make sure this variable is changed to the name shown here!****
//                                                               //**Also remove static storage spec.**
//
//extern "C" int APIENTRY
//DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/ )
//{
//	if (dwReason == DLL_PROCESS_ATTACH)
//	{
//		TRACE0("UIMAN.DLL Initializing!\n");
//		
//		// Extension DLL one-time initialization
//		AfxInitExtensionModule(EXTENSIONDLL, hInstance);            //*********** Also change name here ************
//
//		// Insert this DLL into the resource chain
//		new CDynLinkLibrary(UIManDLL);
//	}
//	else if (dwReason == DLL_PROCESS_DETACH)
//	{
//		TRACE0("UIMAN.DLL Terminating!\n");
//	}
//	return 1;   // ok
//}
//
//  That's it!

#ifndef DLLINST_EXTERNAL_MODULE

    extern AFX_EXTENSION_MODULE EXTENSIONDLL; 

    inline HINSTANCE GetResourceHandleToThisModule() { return EXTENSIONDLL.hResource; };
    #define SetResourceHandleToThisModule() CDLLResourceState Res_No_Namespace_Collision_Here

    class CDLLResourceState
    {
    public:
        CDLLResourceState( HINSTANCE hResInst ) : m_hinstOld( AfxGetResourceHandle() )
            { AfxSetResourceHandle( hResInst ); };
        CDLLResourceState() : m_hinstOld( AfxGetResourceHandle() )
            { AfxSetResourceHandle( GetResourceHandleToThisModule() ); };
	    ~CDLLResourceState()
		    { SetBack(); };
	    void SetBack()
		    { AfxSetResourceHandle( m_hinstOld ); };
    private:
	    HINSTANCE m_hinstOld;
    };

#endif // !DLLINST_EXTERNAL_MODULE

#define SetResourceHandleTo(hRes) CDLLResourceState2 Res_No_Namespace_Collision_Here( (hRes) )

class CDLLResourceState2
{
public:
    CDLLResourceState2( HINSTANCE hResInst ) : m_hinstOld( AfxGetResourceHandle() )
        { AfxSetResourceHandle( hResInst ); };
	~CDLLResourceState2()
		{ SetBack(); };
	void SetBack()
		{ AfxSetResourceHandle( m_hinstOld ); };
private:
	HINSTANCE m_hinstOld;
};
