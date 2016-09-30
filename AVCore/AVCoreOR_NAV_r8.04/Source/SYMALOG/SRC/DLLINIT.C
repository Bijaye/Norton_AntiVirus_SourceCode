/**********************************************************************
** Copyright 1993 Symantec, Peter Norton Group
**
** File: dllinit.c
**
** This file contains startup and termination routines for a Windows
** DLL.
**
** See Also:
** Future Enhancements:
***********************************************************************
** $Header:   S:/SYMALOG/VCS/dllinit.c_v   1.7   01 Jul 1998 10:41:08   TSmith  $
** 05/04/93 WRL is William R. Lee II (BLEE)
**********************************************************************/
#define MAIN                            /* Allocate globals here!    */
#include "platform.h"
#include "symalogi.h"                   /* Project specific dcls     */
#include "logio.h"                      /* LogioInit()               */
#include "symtla.h"                     /* Tivoli Logfile Adapter    */


#if defined( SYM_WIN32 )
                                        /* Function pointer and      */
                                        /* instance handle for doing */
                                        /* explicit linking to the   */
                                        /* SYMTLA DLL.               */
SYMTLA_TLALOGADD lpfnTlaLogAdd = NULL;
HINSTANCE        hSymTlaInst   = NULL;

                                        /* Macro to compose the Core */
                                        /* version DLL name for use  */
                                        /* with explicit linking.    */
#define SYMTLA_DLL( v ) "S32TLA" ##v ".DLL"

#endif /* SYM_WIN32 */


#if defined(SYM_WIN16)

/*** LibMain() ********************************************************
**
** Future Enhancements:
***********************************************************************
** 05/03/93 WRL Added LogioInit() call in LibMain()
**********************************************************************/
int WINAPI LibMain(                     /* DLL initialization        */
    HINSTANCE hInst,                    /* DLL instance              */
    WORD wDataSeg,                      /* Local Data Segment        */
    WORD wHeapSize,                     /* Local heap size           */
    LPSTR lpszCmdLine)                  /* Parameter line            */
{
    if (wHeapSize > 0)                  /* No need to lock for       */
       UnlockData (0);                  /*      non-real mode DLL's  */

    LogioInit();                        /* "Low-level" log routines  */
    hDLLInst = hInst;                   /* Save DLL's instance handle*/
    return (1);                         /* Return success            */
} /* LibMain() */

/*** WEP() ************************************************************
**
** Future Enhancements:
***********************************************************************
** 04/20/93 WRL
**********************************************************************/
int SYM_EXPORT WINAPI WEP(                  /* Windows exit procedure    */
    int nExitType)                      /* Type is WEP_FREE_DLL or   */
{                                       /*    WEP_SYSTEM_EXIT        */
    return(1);                          /* Return success            */
} /* WEP() */

#elif defined(SYM_WIN32)

/****************************************************************************
   FUNCTION: DllMain

   PURPOSE:  Called when a new process is either using or not using
             this DLL anymore.

*******************************************************************************/
BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwReason, LPVOID lpvReserved)
{
    LogioInit();                        /* "Low-level" log routines  */
    hDLLInst = hInstDLL;                /* Save DLL's instance handle*/

    switch ( dwReason )
        {
        case DLL_PROCESS_ATTACH:
                                        /* Load the correct Core     */
                                        /* version of the SYMTLA DLL */
           hSymTlaInst = LoadLibrary( SYMTLA_DLL( Q_VER ) );
    
                                        /* Don't error check the     */
                                        /* LoadLibrary call since    */
                                        /* the presence or absence   */
                                        /* of the DLL will determine */
                                        /* if the Tivoli logfile     */
                                        /* will be created or not.   */
           if ( NULL != hSymTlaInst )
               {
               lpfnTlaLogAdd = ( SYMTLA_TLALOGADD )
                   GetProcAddress( hSymTlaInst, "_TlaLogAdd@24" );
               }
            break;
    
        case DLL_THREAD_ATTACH:
            break;
    
        case DLL_THREAD_DETACH:
            break;
    
        case DLL_PROCESS_DETACH:
                                        /* Release the SYMTLA DLL if */
                                        /* if it was loaded.         */
            if ( NULL != hSymTlaInst )
                {
                FreeLibrary( hSymTlaInst );
                lpfnTlaLogAdd = NULL;
                }
            break;
    }
    
    return(TRUE);
}

#endif

