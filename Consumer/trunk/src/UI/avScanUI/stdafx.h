// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

// Enable all Windows Server 2003+ features
#define _WIN32_WINNT 0x0502
#define WINVER 0x0500

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define VC_EXTRALEAN
#define STRICT

// ATL defines
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
#define _ATL_ALL_WARNINGS

// Disable warnings that can safely be ignored
#pragma warning(disable : 4018)
#include <ccIgnoreWarnings.h>

// Windows Header Files:
#include <windows.h>
#include <tchar.h>
#include <shellapi.h>

// StdLib includes
#include <vector>

// Include ccLib
#include <ccLib.h>
#include <ccLibDllLink.h>
#include <ccTrace.h>
#include <ccCoInitialize.h>
#include <ccSynchronize.h>
#include <ccCriticalSection.h>
#include <ccSingleLock.h>
#include <ccThread.h>
#include <ccMessageLock.h>
#include <ccMutex.h>
#include <ccSymPathProvider.h>

// #pragma TODO stuff...
#undef STRIZE
#define STRIZE(x) #x
#undef PPSTRIZE
#define PPSTRIZE(x) STRIZE(x)
#undef FILE_LINE
#define FILE_LINE __FILE__ "(" PPSTRIZE(__LINE__) ") : "
#define TODO(msg) message(FILE_LINE "TODO:  " msg)

// Refcount logging helpers
//#define  _TRACE_RELEASES_
#ifdef  _TRACE_RELEASES_
#define  TRACE_REF_COUNT(xOBJ)  if(xOBJ) \
                                { \
                                    xOBJ->AddRef(); \
                                    size_t iRefCount = xOBJ->Release(); \
                                    CCTRCTXI1(L"--Releasing " L#xOBJ L" refcount is=%lu", iRefCount); \
                                } \
                                else \
                                { \
                                    CCTRCTXI0(L#xOBJ L" is NULL"); \
                                }
#else
#define  TRACE_REF_COUNT(xOBJ)
#endif

// Error logging helpers
#define  LOG_FAILURE(xMSG, xHR)  if(FAILED(xHR)) { CCTRACEE(CCTRCTX xMSG L" (hr=%08X)", xHR); }
#define  LOG_FAILURE_AND_RETURN(xMSG, xHR)  if(FAILED(xHR)) { CCTRACEE(CCTRCTX xMSG L" (hr=%08X)", xHR); return xHR; }
#define  LOG_FAILURE_AND_CONTINUE(xMSG, xHR)  if(FAILED(xHR)) { CCTRACEE(CCTRCTX xMSG L" (hr=%08X)", xHR); continue; }
#define  LOG_FAILURE_AND_BREAK(xMSG, xHR)  if(FAILED(xHR)) { CCTRACEE(CCTRCTX xMSG L" (hr=%08X)", xHR); break; }

#define  SYM_LOG_FAILURE(xMSG, xHR)  if(SYM_FAILED(xHR)) { CCTRACEE(CCTRCTX xMSG L" (symRes=%08X)", xHR); }
#define  SYM_LOG_FAILURE_AND_RETURN(xMSG, xHR)  if(SYM_FAILED(xHR)) { CCTRACEE(CCTRCTX xMSG L" (symRes=%08X)", xHR); return xHR; }
#define  SYM_LOG_FAILURE_AND_CONTINUE(xMSG, xHR)  if(SYM_FAILED(xHR)) { CCTRACEE(CCTRCTX xMSG L" (symRes=%08X)", xHR); continue; }
#define  SYM_LOG_FAILURE_AND_BREAK(xMSG, xHR)  if(SYM_FAILED(xHR)) { CCTRACEE(CCTRCTX xMSG L" (symRes=%08X)", xHR); break; }

#define  ENTER_SCOPED_SECTION() do 
#define  LEAVE_SCOPED_SECTION() while(false);

// Load string helpers
#define CASE_LOAD_STRING(xCase, xStr, xResource) case xCase: xStr.LoadString(xResource); break;
#define CASE_EMPTY(xCase, xStr) case xCase: xStr = L""; break;
#define _S(xResID) (CString((LPCTSTR)xResID))

// ATL
#include <atlbase.h>
#include <atlapp.h>
#include <atlsecurity.h>

// ResourceModule
#include "SymResourceModule.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class CResourceModule : public CSymResourceModuleT< CAppModule >
{
public:
};

extern CResourceModule _Module;

// More ATL
#include <atlcom.h>
#include <stdexcept>
#include <atlwin.h>
#include <atlcoll.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlctrlw.h>
#include <atlctrlx.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlimage.h>
#include <atlsecurity.h>

// StahlSoft
#include <SmartPtr.h>
#include <hrx.h>


#include <ISVersion.h>
#include <ccSymDebugOutput.h>
extern ccSym::CDebugOutput g_DebugOutput;