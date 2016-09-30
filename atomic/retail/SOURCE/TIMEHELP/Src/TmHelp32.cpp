// TmHelp32.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <math.h>
#include "TmHelp32.h"

extern "C" BOOL WINAPI thk_ThunkConnect32(LPSTR pszDll16, LPSTR pszDll32, 
	DWORD hInst, DWORD dwReason);

extern "C" __declspec(dllexport) DWORD WINAPI GetCMOSTime();

extern "C" DWORD WINAPI GetTime();

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( !(thk_ThunkConnect32(
		"TimeHelp.DLL",	// name of 16-bit DLL 
		"TmHelp32.DLL",	// name of 32-bit DLL 
		(unsigned long) hModule, ul_reason_for_call)) )     
	{        return FALSE;     } 

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
    }
    return TRUE;
}

extern "C" __declspec(dllexport) DWORD WINAPI GetCMOSTime()
{
	return GetTime();
}

static WORD BCD2Bin(BYTE bVal)
{	WORD wRet, wTemp;

	wTemp = bVal;
	wRet = (wTemp & 0x000F) + (((wTemp & 0x00F0) >> 4) * 10);
	return wRet;
}

extern "C" __declspec(dllexport) BOOL WINAPI EnsureCorrectSystemTime()
{	
	DWORD dwCMOSTime;
	WORD  wHour, wMin, wSec;
	SYSTEMTIME stSystemTime;

	dwCMOSTime = GetTime();
	GetSystemTime(&stSystemTime);

	wHour = BCD2Bin(HIBYTE(HIWORD(dwCMOSTime)));
	wMin  = BCD2Bin(LOBYTE(HIWORD(dwCMOSTime)));
	wSec  = BCD2Bin(HIBYTE(LOWORD(dwCMOSTime)));


	//
	// This call gets the timezone information.  This is needed because 
	// of funky 1/2 hour timezones.  If a funky timezone is present, we
	// must adjust our raw CMOS time to match the system time.  CMOS 
	// and system time can off by 1/2 hour in funky time zones. 
	//
	// CBROWN 10/13/1999
	//

	WORD  wMinDiff;
 	wMinDiff = abs( (int)wMin - (int)stSystemTime.wMinute );

	if ( wMinDiff >= 29 )
	{
		wMin += 30;

		if( wMin > 59 )
			wMin = wMin - 60;
	}

	//
	// If we have lost more than 3 seconds, adjust the time...
	//
	if( wMin != stSystemTime.wMinute ||
		abs( (int)wSec - (int)stSystemTime.wSecond ) > 3 )
	{
		stSystemTime.wMinute = wMin;
		stSystemTime.wSecond = wSec;
		
		if( !SetSystemTime( &stSystemTime ) )
		{
			return FALSE;
		}
	}

	return TRUE;
}
