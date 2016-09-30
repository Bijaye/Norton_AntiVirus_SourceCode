// lureg.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "lureg.h"
#include "navcommandlines.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	HRESULT hResult = S_OK;
	hResult = CoInitialize(NULL);
	
	CNavCommandLines cmdLines;
	HRESULT hr = cmdLines.RegisterCommandLines();
		
	if (SUCCEEDED(hResult))
	{
		CoUninitialize();
	}

	return 0;
}