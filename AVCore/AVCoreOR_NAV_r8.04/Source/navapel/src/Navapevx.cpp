#if !defined(SYM_VXD)
#error This file supports only SYM_VXD platform!
#endif

#include "platform.h"

BOOL NAVAPELGlobalInit (
    VOID
    );

extern "C" BOOL WINAPI NAVAPEL_Dynamic_Init ( VOID );

#define DRIVER_INIT_CODE_SEGMENT
#include "drvseg.h"

//---------------------------------------------------------------------------
//
// BOOL WINAPI NAVAPEL_Dynamic_Init (
//      VOID
//      );
//
// This function is called by NAVAPEL.VXD's control procedure upon
// SYS_DYNAMIC_DEVICE_INIT message.  If such message is sent to NAVAPEL, this
// means that the driver is loaded dynamically.  This function initializes
// global NAVAPEL modules.
//
// Entry:
//      -
//
// Exit:
//      TRUE if initialization successful, FALSE if error.
//
//---------------------------------------------------------------------------
BOOL WINAPI NAVAPEL_Dynamic_Init (
    VOID
    )
{
    // Initialize Core.

    if ( CoreGlobalInit() )
    {
        // Initialize global NAVAP modules.

        if ( NAVAPELGlobalInit() )
        {
            return ( TRUE );
        }

        // Undo CoreGlobalInit().

        if ( !CoreGlobalDeInit() )
        {
            SYM_ASSERT ( FALSE );
        }
    }

    // Report failure during initialization.

    return ( FALSE );
}