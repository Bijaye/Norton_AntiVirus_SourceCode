// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// dwhClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <exception>
#include "idefevnt.h"
#include "dwClient.h"

//#include "..\dwLdPntScan\dwLdPntScan.h"

using namespace std;

const CLSID CLSID_DefWatchLdPntScanEventHandler = 
{ 0x83684a2, 0x47ab, 0x4839, { 0xa3, 0xb3, 0x81, 0x9, 0xf4, 0x26, 0x6b, 0x29 } };



const IID IID_DefWatchEventHandler = 
{ 0x66e44fa0, 0x7608, 0x11d2, { 0x8e, 0x55, 0x72, 0xc9, 0xee, 0x0, 0x0, 0x0 } };


int _tmain(int argc, _TCHAR* argv[])
{

	HRESULT hr;									// COM error code
    IDefWatchEventHandler *IHandler;				// pointer to interface

	cout << "Start\n";

    hr = CoInitialize(0);							// initialize COM

	try {

	if (SUCCEEDED(hr))								// macro to check for success
    {
		cout << "COM initialized\n";

		hr = CoCreateInstance(
			CLSID_DefWatchLdPntScanEventHandler,    // COM class id
			NULL,									// outer unknown
			CLSCTX_INPROC_SERVER,					// server INFO
			IID_DefWatchEventHandler,				// interface id
			(void**)&IHandler );						// pointer to interface

        if (SUCCEEDED(hr))
        {
			cout << "Interface created \n";
			
			if (IHandler == NULL) 
			{
				cout << "IHandler is null " << endl;
			}

			// call method
            hr = IHandler->OnNewDefsInstalled();

			cout << "OnNewDefsInstalled method called " << hr << endl;

            // release interface
            hr = IHandler->Release();

			cout << "handler released " << hr << "\n";

		} else {
			cout << "Interface creation failed " << hr << endl;
		}
	} else {

			cout << "COM initialization failed\n";
	}

	} catch (std::exception &e) {
			cout << "Exception " << e.what() << endl;
			throw;
//	} catch (...) {
//			cout << "Exception occurred";
//			throw;
	}

	// close COM
    CoUninitialize();

	cout << "COM uninitialized\n";

	return 0;
}

