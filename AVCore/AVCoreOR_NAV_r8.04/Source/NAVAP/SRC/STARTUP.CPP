// Copyright 1997 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVAP/VCS/STARTUP.CPv   1.3   17 Feb 1998 22:07:32   RStanev  $
//
// Description:
//      This file defines NAVAP startup functions for self-checking and
//      memory scanning.
//
// Contains:
//      CheckIfNAVAPIntact()
//      CheckIfMemoryInfected()         (SYM_VXD only)
//
// See Also:
//      init.cpp, navapnt.cpp, navapvxd.cpp
//
// Note:
//
//***************************************************************************
// $Log:   S:/NAVAP/VCS/STARTUP.CPv  $
// 
//    Rev 1.3   17 Feb 1998 22:07:32   RStanev
// Updated copyright.
//
//    Rev 1.2   08 Jul 1997 17:03:52   ksackin
// Only Intel compilations should call FileSelfTest().
//
//    Rev 1.1   23 May 1997 23:59:32   RStanev
// Forced CheckIfNAVAPIntact() for SYM_NTK to not fail if the driver is
// not stamped.  Read the header of the function for more details.
//
//    Rev 1.0   21 May 1997 04:50:14   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "navap.h"
#include "navapi.h"

//---------------------------------------------------------------------------
//
// BOOL CheckIfNAVAPIntact (
//      VOID
//      );
//
// This function checks if NAVAP's executable has not changed since it
// was stamped.  It is called during controlled initialization.
// CheckIfNAVAPIntact() will report the result of the check and will take
// appropriate actions if necessary.
//
// Entry:
//      -
//
// Exit:
//      TRUE if file not changed, FALSE if file changed or error
//      checking.
//
// Note:
//      Currently our NT driver does not get CRC-ed.  This is because
//      navcrc does not update the PE header checksum after writing the
//      CRC to the image, and NT's PE loader refuses to load the driver.
//      The SYM_NTK version of this function will succeed even if the
//      driver was not stamped.
//
//---------------------------------------------------------------------------
BOOL CheckIfNAVAPIntact (
    VOID
    )
    {
    auto BOOL  bResult;
    auto PVOID pvBuffer;
    auto UINT  uCheckResult;

                                        // Allocate work buffers.

    if ( pvBuffer = MemAllocPtr ( GHND, SELF_TEST_BUFFER_SIZE ) )
        {
                                        // Test executable.

        SYM_ASSERT ( pszNAVAPStartupName );
        SYM_VERIFY_STRING ( pszNAVAPStartupName );

#if !defined(_ALPHA_)

        uCheckResult = FileSelfTest ( pszNAVAPStartupName, pvBuffer );

#else

        uCheckResult = SELF_TEST_SUCCESS;

#endif

#if defined(SYM_NTK)
                                        // If FileSelfTest() determined that
                                        // our driver is not CRC-ed, then
                                        // fake a successful test.

        if ( uCheckResult == SELF_TEST_NO_CRC )
            {
            uCheckResult = SELF_TEST_SUCCESS;
            }

#endif

                                        // Release allocated resources.

        MemFreePtr ( pvBuffer );

                                        // Set bResult based on the status
                                        // of the test.

        bResult = ( uCheckResult == SELF_TEST_SUCCESS );
        }
    else
        {
                                        // We are out of memory.  Note, that
                                        // !bResult and uCheckResult ==
                                        // SELF_TEST_SUCCESS is a special
                                        // case for UISelfCheckFailed().

        UIOutOfMemoryError();

        uCheckResult = SELF_TEST_SUCCESS;

        bResult = FALSE;
        }

    if ( !bResult )
        {
                                        // Report error and initiate
                                        // shutdown.

        UISelfCheckFailed ( NULL,
                            uCheckResult,
                            AVACTION_SHUTDOWN,
                            TRUE,
                            NULL );

#if defined(SYM_VXD)

        InitiateShutdown();

#endif // #if defined(SYM_VXD)

        }

    return ( bResult );
    }

//---------------------------------------------------------------------------
//
// BOOL CheckIfMemoryInfected (
//      VOID
//      );
//
// This function scans the memory for known viruses.  It is called during
// controlled initialization.  CheckIfMemoryInfected() will report the
// result of the check and will take appropriate actions if necessary.
//
// Entry:
//      -
//
// Exit:
//      TRUE if memory infected, FALSE if memory not infected or error.
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD)

BOOL CheckIfMemoryInfected (
    VOID
    )
    {
    auto BOOL      bResult;
    auto HVCONTEXT hVContext;
    auto HVLVIRUS  hVLVirus;
    auto VSTATUS   uVStatus;

                                        // Initialize local variables.

    bResult = FALSE;

                                        // Prepare for using the memory
                                        // scanning APIs.

    hVContext = LockNAVAPScanConfiguration();

    SYM_ASSERT ( hVContext );

    hVLVirus = NULL;

                                        // Scan memory for viruses.

    uVStatus = VLScanMemory ( hVContext,
                              bNAVAPCfgStartupScanUpperMemory ?
                                  VL_SCANHI : VL_SCAN640,
                              &hVLVirus );

    if ( uVStatus == VS_MEMALLOC )
        {
        UIOutOfMemoryError();
        }
    else if ( uVStatus == VS_OK && hVLVirus )
        {
                                        // If virus was found, then display
                                        // the UI.  The hEventPacket and
                                        // prAVActionData will be set to
                                        // NULL so that the UI knows that
                                        // this is startup and display
                                        // correct message.

        UIMemoryInfected ( NULL, hVLVirus, AVACTION_SHUTDOWN, TRUE, NULL );

                                        // Release virus handle.

        if ( VLReleaseVirusHandle ( hVLVirus ) != VS_OK )
            {
            SYM_ASSERT ( FALSE );
            }

                                        // Set result.

        bResult = TRUE;

                                        // Initiate shutdown.

        InitiateShutdown();
        }

    UnlockNAVAPScanConfiguration ( hVContext );

    return ( bResult );
    }

#endif // #if defined(SYM_VXD)
