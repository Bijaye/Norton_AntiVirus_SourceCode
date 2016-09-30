// main.cpp : Fixes the SAV9 Uninstall so SAV10 can upgrade it
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003-2004, 2005 Symantec Corporation. All rights reserved.

// The purpose of this project is to fix a problem with the SAV9 Uninstall while migrating where LuComServerPS.dll is locked by CA's in the 
// SAV9 uninstall, resulting in crashes when using LU COM objects backed by it and LuComServer.exe, due to the mismatch.  Reference defect 1-4JQ57P.
// MSI cannot be configured to run a program silently, so this project is necessary to prevent a flicker of the black console screen
// shown by the Console project.

#include "Sav9UninstallFix.h"
#include <msiquery.h>
#include "resource.h"

// ** FUNCTIONS **
int __stdcall WinMain( HINSTANCE instanceHandle, HINSTANCE previousInstanceHandle, LPSTR commandLine, int showCommand )
{
    int result = FixSav10ByRemoveCA();
    if (result == EXIT_CODE_SAVSCSNOTDETECTED)
        printf(IDS_WARNING_NODETECTIONS);

    return result;
}
