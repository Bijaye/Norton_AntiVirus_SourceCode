#include "platform.h"
#include "drvcomm.h"
#include "apcomm.h"
#include "navapel.h"
#include "pel.h"

#define DRIVER_PAGEABLE_CODE_SEGMENT
#include "drvseg.h"

BOOL __cdecl NAVAPELQueryInterface (
    PNAVAPEL_INTERFACE pInterface
    )
{
    if ( pInterface->dwRev == 1 )
    {
        pInterface->dwRev = 0;

        pInterface->pAddCurrentThreadToPEL1       = AddCurrentThreadToPEL1;
        pInterface->pForceCurrentThreadIntoPEL1   = ForceCurrentThreadIntoPEL1;
        pInterface->pIsCurrentThreadInPEL1        = IsCurrentThreadInPEL1;
        pInterface->pDeleteCurrentThreadFromPEL1  = DeleteCurrentThreadFromPEL1;
        pInterface->pAddCurrentProcessToPEL2      = AddCurrentProcessToPEL2;
        pInterface->pGetCurrentProcessPEL2Count   = GetCurrentProcessPEL2Count;
        pInterface->pDeleteCurrentProcessFromPEL2 = DeleteCurrentProcessFromPEL2;

        return TRUE;
    }

    return FALSE;
}

VOID NAVAPELServiceCallback (
    DWORD  dwServiceCode,
    PVOID  pvInBuffer,
    DWORD  dwInBufferSize,
    PVOID  pvOutBuffer,
    DWORD  dwOutBufferSize,
    PDWORD pdwBytesReturned
    )
{
    auto PVOID pvCoreInit;

    // Assume we return nothing in pvOutBuffer.

    if ( pdwBytesReturned )
        *pdwBytesReturned = 0;

    // Initialize Core.

    if ( pvCoreInit = CoreLocalInit() )
    {
        // Dispatch the request that the user mode client is issuing.

        switch ( dwServiceCode )
        {
        case NAVAPEL_COMM_UNPROTECTPROCESS:

            AddCurrentProcessToPEL2();

            break;

        case NAVAPEL_COMM_PROTECTPROCESS:

            DeleteCurrentProcessFromPEL2();

            break;

        default:

            SYM_ASSERT ( FALSE );
        }

        // Deinitialize Core.

        CoreLocalDeInit ( pvCoreInit );
    }
    else
        SYM_ASSERT ( FALSE );
}

#define DRIVER_INIT_CODE_SEGMENT
#include "drvseg.h"

BOOL NAVAPELGlobalInit (
    VOID
    )
{
    auto PVOID pvCoreInit;

    // Initialize local instance of Core.

    if ( pvCoreInit = CoreLocalInit() )
    {
        // Initialize PEL.

        if ( PELInit() )
        {
            auto DWORD dwStatus;

            // Initialize DriverComm.

            while ( ( dwStatus = DriverCommInit() ) == ERROR_DRVCOMM_UNSAFE )
                Sleep ( 100 );

            SYM_ASSERT ( dwStatus == ERROR_DRVCOMM_SUCCESS );

            if ( dwStatus == ERROR_DRVCOMM_SUCCESS )
            {
                // Install DriverComm callbacks.

                if ( DriverCommRegisterServiceCallback ( NAVAPELServiceCallback ) == ERROR_DRVCOMM_SUCCESS )
                {
                    CoreLocalDeInit ( pvCoreInit );

                    return TRUE;
                }
            }

            // Undo PELInit().

            if ( !PELDeInit() )
            {
                SYM_ASSERT ( FALSE );
            }
        }

        CoreLocalDeInit ( pvCoreInit );
    }

    return FALSE;
}
