// Copyright 1997 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:/NAVEX/VCS/navexnlm.cpv   1.5   28 Oct 1997 17:39:08   DDREW  $ 
// 
// Description: NAVEX15 NLM  Startup Source                                                        
// 
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:/NAVEX/VCS/navexnlm.cpv  $ 
// 
//    Rev 1.5   28 Oct 1997 17:39:08   DDREW
// Turned the nlm unload protection flag back on
// 
//    Rev 1.4   25 Sep 1997 19:08:02   DDREW
// Made a new QueryInterFace all of our own
// 
//    Rev 1.3   17 Sep 1997 17:17:36   DDREW
// Made calls to start/stop MVP
// 
//    Rev 1.2   07 Aug 1997 09:44:26   DDREW
// A few minor NLM fixes
// 
//    Rev 1.1   05 Aug 1997 10:59:48   DDREW
// Fixed log line
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
#include "mvp.h"
#include "symserve.h"


//
//  Copied from NLMDLL.H
//
#define DLL_LOAD_REQUESTED          0
#define DLL_LOADED                  1
#define DLL_FAILED_REGISTER         2
#define DLL_UNLOADED                3

#define OK                          0

//**********************************************************************
//
//  Local component prototypes
//
//**********************************************************************

VOID AtExitOsLib();
VOID LibraryExitFunction (VOID);
DWORD Navex15_Query_Interface( LPNAVNLM_PROC *lplpNavProcID );

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

BYTE szStartPath[SYM_MAX_PATH];

NAVNLM_PROC NavNlmProc[] = {
    { LIBEXITFUNCTION, LibraryExitFunction },
    { 0,0                }
    };

//**********************************************************************
//
//  static local data
//
//**********************************************************************



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
// 07/06/1997 DDREW Function Created.                                
//***************************************************************************
int main(
    int         argc,           // command line argument count
    char       *argv[] )        // command line arguments
    {

    (void) argc;

    SaveStartPath( argv );

    // Initialize thread and group IDs
    hThreadId      = GetThreadID();
    hThreadGroupId = GetThreadGroupID();

    atexit( AtExitOsLib );

    MVPLoadData(&hThreadId);

    wSleepThread( hThreadId );          // sleep until roused to leave
 
    ExitThread( EXIT_NLM, OK );

    return (OK);
    }

                     
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
    // may need to do some custom cleanup??

    MVPUnloadData();

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
// 07/07/1997 DDREW Function Created.                                
//***************************************************************************
VOID LibraryExitFunction (VOID)
    {
    //
    //  The parent has told me to shut down.
    //  bNlmExiting MUST be set to TRUE, otherwise the NLM will not
    //  terminate.
    //

    bNlmExiting = TRUE;
    wWakenThread( hThreadId );
    }

//***************************************************************************
// Navex15_Query_Interface()
// 
// Description: 
// 
// Parameters:
// 
// Return Value: 
// 
// See Also: 
// 
//***************************************************************************
// 07/07/1997 DDREW Function Created.                                
//***************************************************************************

BOOL NAVEX15_QUERY_INTERFACE(
    LPNAVNLM_PROC *lplpNavProcID )
    {

    if(lplpNavProcID)
        {
        *lplpNavProcID = &NavNlmProc;

        return(FALSE);
        }

    return(TRUE);
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

//***************************************************************************
// SaveStartPath()
// 
// Description:
// 
// Parameters:
// 
// Return Value: 
// 
// See Also: 
// 
//***************************************************************************
// 09/17/1997 DDREW Function Created.                                
//***************************************************************************
VOID SaveStartPath( char *argv[] )
    {
    int i, nLen;

    STRCPY( szStartPath, argv[0] );

    nLen = STRLEN( szStartPath ) - 1;

    for (i=nLen; i>0; i--)
        {
        if (szStartPath[i] == '\\')
            {
            szStartPath[i+1] = EOS;
            break;
            }
        }
    return;
    }

//***************************************************************************
// GetStartPath()
// 
// Description:
// 
// Parameters:
// 
// Return Value: 
// 
// See Also: 
// 
//***************************************************************************
// 09/17/1997 DDREW Function Created.                                
//***************************************************************************
VOID GetStartPath( LPBYTE *lpszPath )
{
    *lpszPath = szStartPath;
}



// ===== Dead code =================================================


/*
// ------------------------------------------------------------------
//
// Protos from SYMSERVE.H
// Look in symserve.h in AVCORE global include for root values
//
#define PROCIDSIZE 16
//
typedef    BYTE  NAVPROCID[ PROCIDSIZE +4 ];
//
#define LIBEXITFUNCTION "LibExitFunction0"    // all Proc id's s/b 16 bytes long
// 
typedef struct
    {
    NAVPROCID   NavProcID;
    LPVOID      ProcAddress;
    } NAVNLM_PROC, *LPNAVNLM_PROC;
//
WORD wSleepThread(                      // suspend thread
    HTHREAD hThreadId );                // thread to suspend
//
WORD wWakenThread(                      // resume thread
    HTHREAD hThreadId );                // thread to resume
//
WORD SetConsoleFocus( int *pnPreviousInputFocus,    // previous handle
                      int *pnSystemConsole      );  // handle to console
//
WORD RestoreFocus( int *pnPreviousInputFocus,       // previous handle
                   int *pnSystemConsole      );     // handle to console copy
//
VOID EmptyTypeAhead( VOID );
// ------------------------------------------------------------------
*/
