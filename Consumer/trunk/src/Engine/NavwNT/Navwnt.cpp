////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <atlstr.h>
#include "Switches.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, wchar_t* szCmdLine, int iShowCmd)
{
	// Navwnt.exe is a stub application that launches Navw32.exe.
	// In previous versions of NAV, the main scanner was named Navwnt.exe for
	// NT platforms and Navw32.exe for 9X platforms. We now have a single scanner
	// executable that runs on both 9X and NT platforms: Navw32.exe.
	// To prevent third-party applications such as WinZip from breaking, we've
	// added this stub executable which simply calls Navw32.exe.

	// Etrack Incident 631957:
	// Now using Navwnt.exe as stub application that launches Navw32.exe with 
	// commandline switch /IMSCAN. This is because Yahoo! IM clients do not 
	// support launching virus scanner with commandline switch.

	CStringW sCmdLine;
	sCmdLine.Format(L"%s %s", SWITCH_IMSCAN, szCmdLine);
	ShellExecute(NULL, L"open", L"navw32.exe", sCmdLine.GetString(), NULL, iShowCmd);

	return 0;
}

