////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TestProbeGSE.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TestProbeGSE.h"							// current instance

typedef BOOL (WINAPI *pfnGSEFunctions)(unsigned long);
typedef BOOL (WINAPI *pfnGSEAddFunction)(unsigned long, const char*);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    HMODULE hDll = NULL;
    pfnGSEFunctions fnGSECheck = NULL;
    pfnGSEFunctions fnGSERemove = NULL;
    pfnGSEAddFunction fnGSEAdd = NULL;
    BOOL bTest = TRUE;
    
    // Load the dll
    hDll = LoadLibrary("probeGSE.dll");
    if( hDll )
    {
        fnGSECheck = reinterpret_cast<pfnGSEFunctions>(GetProcAddress(hDll, "GSECheckVID"));
        fnGSEAdd = reinterpret_cast<pfnGSEAddFunction>(GetProcAddress(hDll, "GSEAdd"));
        fnGSERemove = reinterpret_cast<pfnGSEFunctions>(GetProcAddress(hDll, "GSERemove"));

        if( !fnGSEAdd || !fnGSECheck || !fnGSERemove )
            return -1;

        for( int i=0; i<100; i++ )
        {
            bTest = fnGSEAdd(i, NULL);
        }

        // Remove
        fnGSERemove(50);

        fnGSECheck(666);
        fnGSECheck(1);
        fnGSECheck(44);

        /* Add an item
        fnGSEAdd(100, "test", NULL);
        fnGSEAdd(100, "test", NULL); // This one should be ignored

        fnGSERemove(100, "test");

        fnGSECheck(22, "test");
        fnGSECheck(666, "blah");
        fnGSECheck(323, "test");

        fnGSEAdd(777, "test", NULL);
        fnGSEAdd(111, "blah", NULL);

        fnGSECheck(100, "test");
        fnGSECheck(111, "blah");

        /*for( ; i>=0; i-- )
        {
            bTest = fnGSERemove(i, "test");
        }*/
        
        //bTest = fnGSECheck(1, "blah");
        
        //bTest = fnGSERemove(1, "blah");
    }

    if( hDll )
        FreeLibrary(hDll);

	return 0;
}
