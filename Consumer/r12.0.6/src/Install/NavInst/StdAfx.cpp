// stdafx.cpp : source file that includes just the standard includes
//	DefDll.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#include "SymScriptSafe_i.c"

CDebugLog g_Log ( "NAVInst.log");
CComModule _Module;
HINSTANCE ghInstance;