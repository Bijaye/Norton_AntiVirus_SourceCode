// Copyright 1995 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:/NAVEX/VCS/nechild.cpv   1.6   26 Dec 1996 15:21:38   AOONWAL  $ 
// 
// Description: NLM NAVEX Child Loading Source                                                        
// 
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:/NAVEX/VCS/nechild.cpv  $ 
// 
//    Rev 1.6   26 Dec 1996 15:21:38   AOONWAL
// No change.
// 
//    Rev 1.5   02 Dec 1996 14:01:00   AOONWAL
// No change.
// 
//    Rev 1.4   29 Oct 1996 12:59:40   AOONWAL
// No change.
// 
//    Rev 1.3   30 Jan 1996 15:11:46   DCHI
// Initialized unused external callback pointers to NULL.
// 
//    Rev 1.2   19 Oct 1995 16:53:26   MLELE
// 
//    Rev 1.1   19 Oct 1995 16:44:08   MLELE
// Added function NLMPreventUnload to prevent unloading on a
// console command.  Also added some general comments.
// 
//    Rev 1.0   18 Oct 1995 14:31:52   cnachen
// Initial revision.
//***************************************************************************


#include "platform.h"                   // cross-platform type definitions

#include <process.h>                    // AtUnload
#include <stdio.h>                      // sscanf
#include <conio.h>                      // ConsolePrintf, etc.


// exported functions are in these prototypes

#include "ctsn.h"
#include "callback.h"
#include "navexshr.h"
#include "navex.h"
#include "nlmload.h"


//
//  Copied from NLMDLL.H
//
#define DLL_LOAD_REQUESTED          0
#define DLL_LOADED                  1
#define DLL_FAILED_REGISTER         2
#define DLL_UNLOADED                3

#define OK                          0


//
// Protos from SYMSERVE.H
//
WORD wSleepThread(                      // suspend thread
    HTHREAD hThreadId );                // thread to suspend

WORD wWakenThread(                      // resume thread
    HTHREAD hThreadId );                // thread to resume


//
// Protos from MARIO's SCREEN.H. These are exported by NAV NLM
//
WORD SetConsoleFocus( int *pnPreviousInputFocus,    // previous handle
                      int *pnSystemConsole      );  // handle to console

WORD RestoreFocus( int *pnPreviousInputFocus,       // previous handle
                   int *pnSystemConsole      );     // handle to console copy

VOID EmptyTypeAhead( VOID );




//**********************************************************************
//
//  Local component prototypes
//
//**********************************************************************

VOID LibraryExitFunction (VOID);
WORD WakenOsLib ( VOID );


//**********************************************************************
//
//  global data
//
//**********************************************************************

EXTERN BYTE szCantUnload[];             //  Exported by NAV
EXTERN BYTE byDontUnload;               //  Exported by NAV

int     hThreadId;
int     hThreadGroupId;
BOOL    bNlmExiting = FALSE;


//**********************************************************************
//
//  static local data
//
//**********************************************************************

INIT_PACKET_TYPE    *lpDLLStartup;


                     
//***************************************************************************
// AtExitOsLib()
// 
// Description:
//      Final clean-up function.
//      Registered as atexit callback
// 
// Parameters:  None.
// 
// Return Value:    None.
// 
// See Also: 
// 
//***************************************************************************
// 10/19/1995 MLELE Function Created.                                   
//***************************************************************************

VOID AtExitOsLib()
{
    //
    //  I've almost shut down.
    //  The last thing to do: update status for parent.
    //
    lpDLLStartup->nStatus = DLL_UNLOADED;

}


//***************************************************************************
// main()
// 
// Description: generic library entry routine
// 
// Parameters:
// 
// Return Value: 
// 
// See Also: 
// 
//***************************************************************************
// 05/28/1993 JMILLARD Function Created.                                
//***************************************************************************

int main(
    int         argc,           // command line argument count
    char       *argv[] )        // command line arguments
    
{
    WORD            wErr;
    DWORD           dwStartUpArg;


    (void) argc;

#ifdef DEBUG
    Int03( );                           // enter stone knife debugging mode
#endif


    if (argc != 2)                      // ensure an argument present
        {
        goto BailOut;
        }

    wErr = sscanf( argv[1], "%d", &dwStartUpArg );
    if (wErr != 1)
        {
        goto BailOut;
        }

    //
    //  Initialize thread and group IDs
    //
    hThreadId      = GetThreadID();
    hThreadGroupId = GetThreadGroupID();

    lpDLLStartup = (INIT_PACKET_TYPE *) dwStartUpArg;

    atexit( AtExitOsLib );

    lpDLLStartup->lpFunc = LibraryExitFunction;

    lpDLLStartup->prExportTable->EXTScanFile = (LPVOID)EXTScanFile;
    lpDLLStartup->prExportTable->EXTScanBoot = (LPVOID)NULL;
    lpDLLStartup->prExportTable->EXTScanPart = (LPVOID)NULL;
    lpDLLStartup->prExportTable->EXTScanMemory = (LPVOID)EXTScanMemory;
    lpDLLStartup->prExportTable->EXTRepairFile = (LPVOID)NULL;
    lpDLLStartup->prExportTable->EXTRepairBoot = (LPVOID)NULL;
    lpDLLStartup->prExportTable->EXTRepairPart = (LPVOID)NULL;

    lpDLLStartup->nStatus      = DLL_LOADED;

    wSleepThread( hThreadId );          // sleep until roused to leave
 
BailOut:

    ExitThread( EXIT_NLM, OK );

    return (OK);
}




//***************************************************************************
// LibraryExitFunction()
// 
// Description: cleans up when library exits
// 
// Parameters:
// 
// Return Value: 
// 
// See Also: 
// 
//***************************************************************************
// 05/28/1993 JMILLARD Function Created.                                
//***************************************************************************

VOID LibraryExitFunction (VOID)
{
    //
    //  The parent has told me to shut down.
    //  bNlmExiting MUST be set to TRUE, otherwise the NLM will not
    //  terminate.
    //
    bNlmExiting = TRUE;
    (void)WakenOsLib( );
}




//***************************************************************************
// WakenOsLib()
// 
// Description: Waken OsLib to exit
// 
// Parameters:
// 
// Return Value: 
// 
// See Also: 
// 
//***************************************************************************
// 05/31/1993 JMILLARD Function Created.                                
//***************************************************************************

WORD WakenOsLib ( VOID )

{
    WORD    wErr;

    wErr = wWakenThread( hThreadId );

    return (wErr);
}





//***************************************************************************
// NLMPreventUnload()
// 
// Description:
//
//      Prevents the NLM from unloading on a console command. This function
//      should be specified as a CHECK linker directive. Here, if the unload
//      command was issued by the parent NLM bNlmExiting should be set to
//      TRUE, else we assume that it came from the console.
// 
// Parameters:  None.
//
// Return Value:
//
//      nReturn     (int)   If 0, NLM will unload.
//                          Else, OS expects Y/N confirmation at the
//                          console. We stuff the keyboard with 'N' before
//                          returning 1.
//
// See Also: 
// 
//***************************************************************************
// 10/19/1995 MLELE Function Created.                                   
//***************************************************************************

int NLMPreventUnload (VOID)

{
    int     nReturn = 0;                // return OK to unload

    int     nPreviousInputFocus;        // handle for current screen
    int     nSystemConsole;             // handle for console
    int     nOldThreadGroupId;
    WORD    wErr;


    //
    //  Is NAV NLM unloading me?
    //
    if (bNlmExiting)                     // if I've left, let me go
        {
        return nReturn;
        }



    //
    //  Set up a real thread group so things work
    //
    nOldThreadGroupId = SetThreadGroupID( hThreadGroupId );


    //  tell him he can't unload me in such a unstructured fashion
    //  I will make him type a password on unload

    ConsolePrintf ("\r\n%s\r\n", szCantUnload);     //  imported string

    wErr = SetConsoleFocus( &nPreviousInputFocus,   // previous handle
                            &nSystemConsole      ); // console handle

    if (wErr)
        {
        //  couldn't change focus, what to do?
        }

    EmptyTypeAhead( );

    //  push an n on the keyboard input buffer
    //  this will be the answer to the "Unload module anyway?" question the
    //  server will ask when I return

    ungetch( byDontUnload );

    wErr = RestoreFocus( &nPreviousInputFocus,   // previous handle
                         &nSystemConsole      ); // console handle

    if (wErr)
        {
        // couldn't restore focus, what to do?
        }

    // restore original thread group

    (void) SetThreadGroupID( nOldThreadGroupId );

    nReturn = 1;        // requests server not to unload me -
                        // I've typed ahead an n to the Unload question

    return (nReturn);
}

