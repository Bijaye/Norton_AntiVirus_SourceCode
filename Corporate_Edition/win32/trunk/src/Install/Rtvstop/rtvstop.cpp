// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////
//
// rtvstop.cpp : Loads up the ServiceControl interface and unloads RTVScan
//
//
// Symantec Corporation, 1999
//
// Melissa Mendonca, 12-16-99
// 
/////////////////////////////////////////////////////////////////////////////


// Note:  this project compiles scaninf.idl

#include "scaninf.h"
#include "scaninf_i.c"



HRESULT UnloadServices();



int APIENTRY WinMain( HINSTANCE   hInstance,
                        HINSTANCE   hPrevInstance,
                        LPTSTR      lpCmdLine,
                        int         nCmdShow )
//{
//int main(int argc, char* argv[])
{

    UnloadServices();
    return 0;
}





//----------------------------------------------------------------
// UnloadServices
//----------------------------------------------------------------
HRESULT UnloadServices()
{
	IScanDialogs *pScanDlgs = NULL;
	HRESULT hr = E_FAIL;
	
    
    if( SUCCEEDED( CoInitialize(NULL) ))
    {
	    if( SUCCEEDED( CoCreateInstance( CLSID_ScanDialogs, NULL, CLSCTX_INPROC_SERVER, IID_IScanDialogs, (void**)&pScanDlgs) ) )
	    {
		    if(pScanDlgs)
			{
				hr = pScanDlgs->UnLoadServices();
				pScanDlgs->Release();
			}
	    }
    }

    CoUninitialize();

	return hr;
}

