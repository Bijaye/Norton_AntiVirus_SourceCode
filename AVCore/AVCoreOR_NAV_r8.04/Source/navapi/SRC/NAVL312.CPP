/////////////////////////////////////////////////////////////////////////////
//
// navl312.cpp
// NAV API Library V312 Helper NLM (NAVL312.NLM) main source file.
// Copyright 1998 by Symantec Corporation.  All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NAVAPI/VCS/navl312.CPv   1.0   06 Aug 1998 21:07:16   DHERTEL  $
// $Log:   S:/NAVAPI/VCS/navl312.CPv  $
// 
//    Rev 1.0   06 Aug 1998 21:07:16   DHERTEL
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Headers

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <process.h>
#include <conio.h>


/////////////////////////////////////////////////////////////////////////////
// Types

typedef void* (*LPGETPROCADDRESS)(char*);


/////////////////////////////////////////////////////////////////////////////
// Prototypes
//

// Private functions

int main(int argc, char* argv[]);
void SignalHandler(int nSig);
void* GetProcAddress(char* szName);

// Functions exported via GetProcAddress

void* NAVLIB_ImportSymbol(int iModuleHandle, char* szSymbolName);


/////////////////////////////////////////////////////////////////////////////
// Global data

int gnThreadCount = 0;


/////////////////////////////////////////////////////////////////////////////
// main()

int main(int argc, char* argv[])
{
    LPGETPROCADDRESS* ppfnGetProcAddress = NULL;
    int bInitialized = FALSE;

ConsolePrintf("NAVL 1 main entered\n");
    ++gnThreadCount;
    signal(SIGTERM, SignalHandler);
    if (argc == 2)
        {
ConsolePrintf("NAVL 2 two args\n");
        ppfnGetProcAddress = (LPGETPROCADDRESS*) atoi(argv[1]);
        *ppfnGetProcAddress = GetProcAddress;
        bInitialized = TRUE;
        }
    --gnThreadCount;
    if (bInitialized)
        ExitThread(TSR_THREAD, 0);
ConsolePrintf("NAVL 3 after ExitThread\n");
    return 0;
}


/////////////////////////////////////////////////////////////////////////////
// SignalHandler()

void SignalHandler(int nSig)
{
    switch (nSig)
    {
    case SIGTERM:
        while (gnThreadCount != 0)
            ThreadSwitch();
        break;
    }
}


/////////////////////////////////////////////////////////////////////////////
// GetProcAddress()

void* GetProcAddress(char* szName)
{
ConsolePrintf("NAVL 4 GetProcAddress entered\n");
    if (strcmp(szName, "NAVLIB_ImportSymbol") == 0)
        return NAVLIB_ImportSymbol;

ConsolePrintf("NAVL 5 GetProcAddress failed\n");
    return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// NAVLIB_ImportSymbol()

void* NAVLIB_ImportSymbol(int iModuleHandle, char* szSymbolName)
{
ConsolePrintf("NAVL 6 ImportSymbol called\n");
    return ImportSymbol(iModuleHandle, szSymbolName);
}


