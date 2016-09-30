// Copyright 1995 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:\navfamly\vcs\nlmload.cpv   1.1   29 Nov 1995 18:02:20   GDZIECI  $ 
// 
// Description:                                                         
//
//   Contains NAVEX NLM loader and unloader.
// 
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:\navfamly\vcs\nlmload.cpv  $ 
// 
//    Rev 1.1   29 Nov 1995 18:02:20   GDZIECI
// Checking in from VIRLIB 6.
// 
//    Rev 1.0.1.1   20 Oct 1995 14:30:52   MLELE
//***************************************************************************



#include "platform.h"
#include "file.h"
#include "ctsn.h"                   // fixes twisted nested includes
#include "xapi.h"
#include "virscan.h"
#include "disk.h"
#include "repair.h"
#include "navex.h"
#include "avapi.h"

#include <conio.h>
#include <process.h>
#include <errno.h>

#include "symserve.h"
#include "nlmload.h"


#define DLL_LOAD_REQUESTED      0
#define DLL_LOADED              1
#define DLL_FAILED_REGISTER     2
#define DLL_UNLOADED            3

#define OK                      0


static INIT_PACKET_TYPE  rInitPacket;


EXTSTATUS WINAPI EXTInitNLM (
    HVCONTEXT hContext
    )
{
    LPSTR   lpszStartUpPath;
    WORD    wErr = OK;
    BYTE    OsLibPath [SYM_MAX_PATH];
    BYTE    szDllStartUpAddr [20];


    hContext->prExportTable = NULL;

    //
    //  Set up name of the load module
    //
    GetStartPath (&lpszStartUpPath);
    STRCPY (OsLibPath, lpszStartUpPath);

    STRCAT (OsLibPath, "NAVEX.NLM");

    rInitPacket.nStatus = DLL_LOAD_REQUESTED;

    hContext->prExportTable = MemAllocPtr (GHND, sizeof(EXPORT_TABLE_TYPE));

    if (NULL == hContext->prExportTable)
        goto BailOut;

    rInitPacket.prExportTable = hContext->prExportTable;

    //
    //  Create string with address of rInitPacket.
    //  This is shared by the caller and the called program. We do not
    //  want to export this symbol, hence it has to be passed as a
    //  command-line argument.
    //
    sprintf (szDllStartUpAddr, "%d", &rInitPacket);

    RelinquishControl( );

    wErr = LoadLibrary (OsLibPath, szDllStartUpAddr, LOADLIBRARY_WAIT);

    if (wErr)
        {
        goto BailOut;
        }

    //
    //  wait for the library to load
    //
    while (rInitPacket.nStatus == DLL_LOAD_REQUESTED)
        {
        RelinquishControl( );
        }

    if (rInitPacket.nStatus != DLL_LOADED)
        {
        goto BailOut;
        }

    return(wErr);

BailOut:

    if (hContext->prExportTable)
        MemFreePtr(hContext->prExportTable);

    hContext->prExportTable = NULL;

    return (wErr);
}



EXTSTATUS WINAPI EXTCloseNLM (
    HVCONTEXT hContext
    )
{
    if (hContext->prExportTable == NULL)
        return(EXTSTATUS_OK);

    rInitPacket.lpFunc();
    
    while (rInitPacket.nStatus == DLL_LOADED)
        {
        RelinquishControl( );
        }

    MemFreePtr(hContext->prExportTable);
    hContext->prExportTable = NULL;

    return (EXTSTATUS_OK);
}

