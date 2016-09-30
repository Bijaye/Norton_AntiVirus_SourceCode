////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// TestScan.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TestScan.h"
#include "TScanner.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    //MessageBox(NULL, "test", "test", MB_OK);
    // First load up the scanner
    CTScanner scanner;
    if( SUCCEEDED(scanner.Initialize("Test_Scan")) )
    {
        // Now process the command line
        scanner.ProccessCmdLine();
        if( FAILED(scanner.Scan()) )
            MessageBox(NULL, "Failed to perform the scan", "TestScan", MB_OK | MB_ICONERROR);
    }
    else
        //MessageBox(NULL, "Failed to initialize the scanner", "TestScan", MB_OK | MB_ICONERROR);

    //MessageBox(NULL, "test", "test", MB_OK);

 	return 0;
}

void ProccessCmdLine(LPTSTR lpCmdLine)
{
}