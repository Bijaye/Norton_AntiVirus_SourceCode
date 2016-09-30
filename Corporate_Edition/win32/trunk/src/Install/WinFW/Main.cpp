///////////////////////////////////////////////////////////////////////
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005, Symantec Corporation, All rights reserved.
///////////////////////////////////////////////////////////////////////
// Main.cpp
///////////////////////////////////////////////////////////////////////
//	Created: JZ, 1/21/2005
///////////////////////////////////////////////////////////////////////
//	
//	define guids
#define INITIIDS


#include "stdafx.h"
#include "SymInterface.h"
#include "IWinFW.h"
#include "WinFW.h"
//
//	implement class factory
//
SYM_OBJECT_MAP_BEGIN()
    SYM_OBJECT_ENTRY( SYMOBJECT_WinFW,     CWinFW)
SYM_OBJECT_MAP_END()

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
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

