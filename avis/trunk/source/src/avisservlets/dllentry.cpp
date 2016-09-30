// dllentry.cpp - implementation of the Dll entrypoint method DllMain

#include "dllentry.h"
//  #include "traceclient.h"
//  #include "stacktrace.h"

// The functionality that was in here has been migrated to the extern
// "C" calls "servletInit" and "servletTerm" which are called by the
// LotusGo webserver on initialization and shutdown.  It's a much
// healthier way to perform the initialization. 

// However, I'll keep this in here because of the things that just
// never ever happen.
BOOL WINAPI DllMain (HINSTANCE hinstDll,DWORD fdwReason,LPVOID fImpLoad)
{
//    MTrace _traceServer("c:\\log\\dllmain.log");
//    TraceClient _trace(_traceServer,"dllentry");
//    StackTrace _stack(_trace,"DllMain");
  switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
      {
//          _stack.msg("DLL_PROCESS_ATTACH");
      }
      break;
    case DLL_THREAD_ATTACH:
      {
//          _stack.msg("DLL_THREAD_ATTACH");
      } 
      break;
    case DLL_THREAD_DETACH:
      {
//          _stack.msg("DLL_THREAD_DETACH");
      }
      break;
    case DLL_PROCESS_DETACH:
      {
//          _stack.msg("DLL_PROCESS_DETACH");
      }
      break;
    };
  return TRUE;
}

