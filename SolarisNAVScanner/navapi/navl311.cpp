/////////////////////////////////////////////////////////////////////////////
//
// navl311.cpp
// NAV API Library V311 Helper NLM (NAVL311.NLM) main source file.
// Copyright 1998 by Symantec Corporation.  All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/NAVAPI/VCS/NAVL311.CPv   1.0   06 Aug 1998 21:07:10   DHERTEL  $
// $Log:   S:/NAVAPI/VCS/NAVL311.CPv  $
// 
//    Rev 1.0   06 Aug 1998 21:07:10   DHERTEL
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

typedef struct tagSYMBOLENTRY
    {
    struct tagSYMBOLENTRY*      lpNextSymbol;
    void*                       lpNlmEntry;
    unsigned long int           dwSymValue;
    char*                       lpcpSymName;
    } 
    SYMBOLENTRY, *LPSYMBOLENTRY;

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

    ++gnThreadCount;
    signal(SIGTERM, SignalHandler);
    if (argc == 2)
        {
        ppfnGetProcAddress = (LPGETPROCADDRESS*) atoi(argv[1]);
        *ppfnGetProcAddress = GetProcAddress;
        bInitialized = TRUE;
        }
    --gnThreadCount;
    if (bInitialized)
        ExitThread(TSR_THREAD, 0);
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
    if (strcmp(szName, "NAVLIB_ImportSymbol") == 0)
        return NAVLIB_ImportSymbol;

    return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// NAVLIB_ImportSymbol()

void* NAVLIB_ImportSymbol(int iModuleHandle, char* szSymbolName)
{
    extern LPSYMBOLENTRY ExternalPublicList;
    void* pSymbolValue = NULL;
    LPSYMBOLENTRY lpSymbol;
    unsigned char cpSymbol[256];

    // iModuleHandle is not used.
    // This argument exists to match the arguments of ImportSymbol()
    (void) iModuleHandle;

    // Convert szSymbolName to a count-preceded string cpSymbol
    cpSymbol[0] = strlen(szSymbolName);
    strncpy(&cpSymbol[1], szSymbolName, cpSymbol[0]);

    // Look for the symbol name in the ExternalPublicList
    for (lpSymbol = ExternalPublicList;
         lpSymbol != NULL;
         lpSymbol = lpSymbol->lpNextSymbol)
    {
        if (cpSymbol[0] == lpSymbol->lpcpSymName[0] &&
            strncmp(&cpSymbol[1], &lpSymbol->lpcpSymName[1], cpSymbol[0]) == 0)
        {
            pSymbolValue = (void*)lpSymbol->dwSymValue;
            break;
        }
    }

    return pSymbolValue;
}


