// Copyright 1995-1997 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVSDK/VCS/selftest.cpv   1.0   08 Dec 1997 16:23:30   DALLEE  $
//
// Description:
//
//      This file contains the NAVSDK internal implementation for
//      the self test CRC routines.
//
// Contains:
//    Exports:
//      VirusLibrarySelfTest()
//
//    Local:
//      GetCrc()
//***************************************************************************
// $Log:   S:/NAVSDK/VCS/selftest.cpv  $
// 
//    Rev 1.0   08 Dec 1997 16:23:30   DALLEE
// Initial revision.
//***************************************************************************

//***************************************************************************
// Include Files
//***************************************************************************
#include "platform.h"
#include "file.h"
#include "navsdk.h"

//***************************************************************************
// Local Definitions
//***************************************************************************

#define SELF_TEST_BUFFER_SIZE   512

//***************************************************************************
// Local Function Prototypes
//***************************************************************************

static WORD WINAPI GetCrc ( 
    WORD    wSeed, 
    LPBYTE  lpbyBuffer, 
    UINT    uCount );

/*@API:**********************************************************************
@Declaration:
    NAVSDKAPI WORD WINAPI VirusLibrarySelfTest (
        LPCSTR      lpszFile )

@Description:
    This function does a CRC self test on the specified file. The test
    will be successfull if the CRC that exists in the file matches the CRC
    calculated by this function.

@Parameters:
    $lpszFile$  Name of the file to scan

@Returns:
    VIRLIB_SELFTEST_OK          Passed Self Test.
    VIRLIB_SELFTEST_NO_CRC      File has not been stamped.
    VIRLIB_SELFTEST_FAIL        CRC does not match.
    VIRLIB_SELFTEST_ERROR       Error reading file.

@See:

@Include:
    navsdk.h

@Compatibility: W32
****************************************************************************/
NAVSDKAPI WORD WINAPI VirusLibrarySelfTest (// Checks integrity of specified File
    LPCSTR      lpszFile )              // [in] File to delete
{
    auto    HFILE   hFile;
    auto    UINT    uBytesRead;
    auto    WORD    wStampedCRC;
    auto    WORD    wNewCRC;

    auto    LPBYTE  lpbyBuffer;

    // Validate input parameters

    SYM_ASSERT ( lpszFile != NULL );

    // Allocate read buffer

    lpbyBuffer = (LPBYTE) MemAllocPtr( GMEM_MOVEABLE, SELF_TEST_BUFFER_SIZE );
    if ( NULL == lpbyBuffer )
    {
        return ( VIRLIB_SELFTEST_ERROR );
    }

    // Open file

    hFile = FileOpen( lpszFile, OF_READ | OF_SHARE_DENY_NONE );
    if ( HFILE_ERROR == hFile )
    {
        return ( VIRLIB_SELFTEST_ERROR );
    }

    // Read file into buffer

    uBytesRead = FileRead( hFile, lpbyBuffer, SELF_TEST_BUFFER_SIZE );

    // Close file

    FileClose( hFile );

    // Make sure that the file was read correctly

    if ( uBytesRead != SELF_TEST_BUFFER_SIZE )
    {
        return ( VIRLIB_SELFTEST_ERROR );
    }

    // Save CRC value stamped in file

    wStampedCRC = *(LPWORD)( lpbyBuffer + 0x12 );

    // Check if there was a CRC

    if ( 0 == wStampedCRC )
    {
        return ( VIRLIB_SELFTEST_NO_CRC );
    }

    // Calculate current CRC

    *(LPWORD)( lpbyBuffer + 0x12 ) = 0;

    wNewCRC = GetCrc( 0, lpbyBuffer, SELF_TEST_BUFFER_SIZE );

    // Compare the CRCs

    if ( wNewCRC != wStampedCRC )
    {
        return ( VIRLIB_SELFTEST_FAIL );
    }

    // All OK.

    return ( VIRLIB_SELFTEST_OK );
} // VirusLibrarySelfTest()


//***************************************************************************
// GetCrc()
//
// static WORD WINAPI GetCrc (
//      WORD    wSeed,              
//      LPBYTE  lpbyBuffer,
//      UINT    uCount )
//
// This routine returns the CRC of the buffer passed in.
// CRC routine is the same as in AVCORE-CERTLIBN function MEM4_CRC.
//***************************************************************************
// 12/08/1997 DALLEE, from DBUCHES prototype for Alpha. Thanks, Dave.
//***************************************************************************

static WORD WINAPI GetCrc ( 
    WORD    wSeed, 
    LPBYTE  lpbyBuffer, 
    UINT    uCount )
{
	auto 	WORD		wCRC;
	auto 	WORD 		wBufferByteValue;
	auto 	UINT		uIndex = 0;

    // Initialize CRC with seed value.

    wCRC = wSeed;

    // Calculate CRC.  This code is ported from MCRC.ASM in AVCORE-CERTLIBN

    while ( uCount-- )
    {
        // Get byte from buffer
            
        wBufferByteValue = (WORD) lpbyBuffer[uIndex];

        // Need to sign extend this guy

        if( wBufferByteValue & 0x80 )
            wBufferByteValue |= 0xFF00;

        // Compute CRC

        wCRC = (((wCRC << 8) + (wCRC >> 8)) ^ wBufferByteValue);
        wCRC ^= ( LOBYTE(wCRC) >> 4 );
        wCRC ^= (( LOBYTE(wCRC) << 5 ) ^
            ((((wCRC & 0x00FF) >> 8) + (LOBYTE(wCRC) << 8)) << 4));

        // Increment array index

        uIndex++;
    }

	// Return CRC.

	return wCRC;
} // GetCrc()

