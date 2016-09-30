// Copyright 1995 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/CERTLIBN/VCS/selftest.cpv   1.1   02 Feb 1996 21:42:52   RSTANEV  $
//
// Description:
//      This file defines functions for verifying the integrity of
//      executable files.
//
// Contains:
//      FileSelfTest()
//
//***************************************************************************
// $Log:   S:/CERTLIBN/VCS/selftest.cpv  $
// 
//    Rev 1.1   02 Feb 1996 21:42:52   RSTANEV
// Added Unicode support and paranoid parameter validation.
//
//    Rev 1.0   11 Apr 1995 12:31:56   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "file.h"
#include "ctsn.h"

/////////////////////////////////////////////////////////////////////////////
//
// UINT WINAPI FileSelfTest (
//     LPTSTR lpszFileName,
//     LPVOID lpvBuffer
//     );
//
// Performs a CRC test on a file previously stamped by FIL4_CRCFILE.
// Use EXECRC.EXE in the VIRLIB's TOOLS area to stamp an executable file
// with the correct CRC value.
//
// FileSelfTest() checks the WORD value in bytes 12h and 13h in the file's
// EXE header versus the CRC value computed for the first
// SELF_TEST_BUFFER_SIZE bytes of the file (disregarding bytes 12h and 13h).
//
// Entry:
//      lpszFileName - Pointer to the currently executing file (use the
//                     default character set for current platform/context)
//      lpvBuffer    - Pointer to a work buffer ( SELF_TEST_BUFFER_SIZE
//                     bytes long )
//
// Exit:
//      SELF_TEST_SUCCESS    - Success, CRC STAMP matched
//      SELF_TEST_NO_CRC     - File has not been CRC STAMPED
//      SELF_TEST_FAIL       - Failed CRC has CHANGED, stamp is different
//      SELF_TEST_FILE_ERROR - File IO error
//
//      Buffer at lpvBuffer destroyed.
//
// Note:
//      The file at lpszFileName should be at least SELF_TEST_BUFFER_SIZE
//      long.
//
/////////////////////////////////////////////////////////////////////////////
UINT WINAPI FileSelfTest (
    LPTSTR lpszFileName,
    LPVOID lpvBuffer
    )
    {
    auto HFILE hFile;
    auto UINT  uBytesRead;
    auto WORD  wPrevCRC, wNewCRC;

                                        // Validate input parameters

    SYM_ASSERT ( lpszFileName != NULL );
    SYM_VERIFY_STRING ( lpszFileName );
    SYM_ASSERT ( lpvBuffer != NULL );
    SYM_VERIFY_BUFFER ( lpvBuffer, SELF_TEST_BUFFER_SIZE );

                                        // Open file

    hFile = FileOpen ( lpszFileName, OF_READ );

    if ( hFile == HFILE_ERROR )
        {
        return ( SELF_TEST_FILE_ERROR );
        }

                                        // Read file into lpvBuffer

    uBytesRead = FileRead ( hFile, lpvBuffer, SELF_TEST_BUFFER_SIZE );

                                        // Close file

    FileClose ( hFile );

                                        // Make sure that the file was read
                                        // correctly

    if ( uBytesRead != SELF_TEST_BUFFER_SIZE )
        {
        return ( SELF_TEST_FILE_ERROR );
        }

                                        // Save previously calculated CRC

    wPrevCRC = *(LPWORD) &((LPBYTE)lpvBuffer)[0x12];

                                        // Check if there was a CRC

    if ( wPrevCRC == 0 )
        {
        return ( SELF_TEST_NO_CRC );
        }

                                        // Calculate current CRC

    *(LPWORD) &((LPBYTE)lpvBuffer)[0x12] = 0;

    wNewCRC = MEM4_CRC ( 0, (LPSTR)lpvBuffer, SELF_TEST_BUFFER_SIZE );

                                        // Compare the CRCs

    if ( wNewCRC != wPrevCRC )
        {
        return ( SELF_TEST_FAIL );
        }

                                        // All OK.

    return ( SELF_TEST_SUCCESS );
    }
