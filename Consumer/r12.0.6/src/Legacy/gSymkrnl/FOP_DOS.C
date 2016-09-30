/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/fop_dos.c_v   1.7   09 Oct 1996 13:04:20   MARKK  $ *
 *                                                                      *
 * Description:                                                         *
 *      DOS & NTK equivalents for Windows file operations.              *
 *                                                                      *
 * Contains:                                                            *
 *      DoEnvironmentSubst                                              *
 *      FileClose                                                       *
 *      FileSeek                                                        *
 *      FileRead                                                        *
 *      FileWrite                                                       *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/fop_dos.c_v  $ *
// 
//    Rev 1.7   09 Oct 1996 13:04:20   MARKK
// Synchronized with branch E
// 
//    Rev 1.6.1.1   16 Sep 1996 17:32:44   RSTANEV
// Removed an unnecessary SYM_ASSERT ( FALSE ).
//
//    Rev 1.6   22 Jul 1996 18:32:58   RSTANEV
// SYM_NTK: Using the NT_SUCCESS() macro.
//
//    Rev 1.5   24 Jun 1996 19:03:28   RSTANEV
// Now using the Zv...() wraps instead of the Zw...() APIs.
//
//    Rev 1.4   06 Apr 1996 13:17:52   RSTANEV
// Extended FileWrite() for SYM_NTK to truncate the file when 0 bytes are
// written.
//
//    Rev 1.3   28 Feb 1996 11:08:38   RSTANEV
// FileRead() for SYM_NTK will now return 0 upon STATUS_END_OF_FILE.
//
//    Rev 1.2   09 Feb 1996 18:43:58   RSTANEV
// Restored back to use Zw... functions.
//
//    Rev 1.1   09 Feb 1996 15:40:52   RSTANEV
// Fixed a problem in FileSeek() with passing a negative value for lOffset.
//
//    Rev 1.0   26 Jan 1996 20:22:24   JREARDON
// Initial revision.
//
//    Rev 1.20   21 Dec 1995 10:58:32   RSTANEV
// Changed SYM_NTK platform to use Nt... APIs instead of Zw... APIs.
//
//    Rev 1.19   08 Dec 1995 14:16:08   RSTANEV
// Added SYM_NTK support.
//
//    Rev 1.18   12 Dec 1994 17:18:32   BRAD
// Added VxD support
//
//    Rev 1.17   18 Nov 1994 14:08:16   BRAD
// Made DBCS stuff a little more efficient
//
//    Rev 1.16   09 Nov 1994 16:29:12   BRAD
// DBCS-enabled
//
//    Rev 1.15   23 Mar 1994 19:41:50   BRAD
// Added LFN support for DOS and WIN16
//
//    Rev 1.14   21 Mar 1994 00:19:58   BRAD
// Cleaned up for WIN32
//
//    Rev 1.13   15 Mar 1994 12:32:10   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.12   25 Feb 1994 15:04:56   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.11   25 Feb 1994 12:22:40   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
//
//    Rev 1.10   13 Oct 1993 15:35:20   MFALLEN
// From Quake branch 1, added Barry's _ClearErrorFar()
//
//    Rev 1.9   17 Sep 1993 20:42:54   DALLEE
// Fixed FileClose() return.  Returns 0 (NOERR) if successful.
//
//    Rev 1.8   17 Jun 1993 19:56:28   MARKL
// SHELL.DLL is now dynamically loaded by SymKrnl.  The rouitines RegOpenKey(),
// RegCloseKey(), RegQueryValue(), and DoEnvironmentSubst() are now called
// through function pointers.  In the case of DOS, a function pointer has
// been established for DoEnvironmentSubst() to allow consistant code in both
// the Windows and DOS platforms.
//
//    Rev 1.7   15 Feb 1993 21:04:32   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.6   12 Feb 1993 04:28:46   ENRIQUE
// No change.
//
//    Rev 1.5   08 Sep 1992 14:46:00   MARKK
// Added a POP of DS to FileRead
//
//    Rev 1.4   04 Sep 1992 11:49:46   ED
// Set DX:CX up properly in FileSeek
//
//    Rev 1.3   04 Sep 1992 10:48:46   ED
// Changed _CheckError to _CheckErrorFar
//
//    Rev 1.2   03 Sep 1992 08:19:44   ED
// Fixed
//
//    Rev 1.1   03 Sep 1992 08:14:14   ED
// Added functions to process error codes consistently
//
//    Rev 1.0   03 Sep 1992 08:02:42   ED
// Initial revision.
//
//    Rev 1.0   27 Aug 1992 09:35:10   ED
// Initial revision.
//
//    Rev 1.2   07 Aug 1992 09:28:22   ED
// Adjustments to DoEnvironmentSubst.  Now it works almost exactly like
// the Windows version.
//
//    Rev 1.1   07 Aug 1992 07:49:48   ED
// Rewrote DoEnvironmentSubst
//
//    Rev 1.0   20 Jul 1992 16:59:18   ED
// Initial revision.
 ************************************************************************/

#include "platform.h"

#if !defined(SYM_WIN)           // This module only applies to all but WIN

#include "file.h"
#include "xapi.h"
#include "dbcs.h"

#if !defined(SYM_VXD)
/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      HFILE PASCAL FileClose (HFILE hFile)                              *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      Returns 0 for success, -1 for error.                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/02/1992 ED Created.                                               *
 ************************************************************************/

#pragma optimize("", off)
HFILE WINAPI FileClose(HFILE hFile)
{
#if defined(SYM_NTK)

    auto NTSTATUS status;

    SYM_ASSERT ( hFile != HFILE_ERROR );

    status = ZvClose ( (HANDLE)hFile );

    if ( NT_SUCCESS ( status ) )
        {
        return ( 0 );
        }

    // Set last error.

    return ( HFILE_ERROR );

#else // #if defined(SYM_NTK)

    _ClearErrorFar ( ) ;                // Reset so we don't catch someone
                                        // else's error
    _asm
        {
        MOV     BX,hFile
        MOV     AH,3Eh                  // Close a handle file
        INT     21h
        jc      J_DoCheckError          // set AX to 0 as specified,
        mov     AX, 0                   // if no error.
J_DoCheckError:
        call    FileCheckError          // set AX to ERR if bad call
        mov     hFile,ax
        }

    return (hFile);

#endif // #if defined(SYM_NTK)
}
#pragma optimize("", on)

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      LONG PASCAL FileSeek(HFILE hFile, LONG lOffset, int iOrigin)     *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      Returns the new file offset, or -1 for error.                   *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/02/1992 ED Created.                                               *
 ************************************************************************/

#pragma optimize("", off)
LONG WINAPI FileSeek(HFILE hFile, LONG lOffset, int iOrigin)
{
#if defined(SYM_NTK)

    auto NTSTATUS                  status;
    auto IO_STATUS_BLOCK           IoStatusBlock;
    auto FILE_POSITION_INFORMATION FilePosInfo;
    auto FILE_STANDARD_INFORMATION FileStdInfo;

    SYM_ASSERT ( hFile != HFILE_ERROR );
    SYM_ASSERT ( iOrigin == SEEK_SET ||
                 iOrigin == SEEK_CUR ||
                 iOrigin == SEEK_END );

    switch ( iOrigin )
        {
        case SEEK_CUR:

            status = ZvQueryInformationFile ( (HANDLE)hFile,
                                              &IoStatusBlock,
                                              &FilePosInfo,
                                              sizeof(FilePosInfo),
                                              FilePositionInformation );

            if ( !NT_SUCCESS ( status ) )
                {
                // Set last error
                return ( -1L );
                }

            break;

        case SEEK_END:

            status = ZvQueryInformationFile ( (HANDLE)hFile,
                                              &IoStatusBlock,
                                              &FileStdInfo,
                                              sizeof(FileStdInfo),
                                              FileStandardInformation );
            if ( !NT_SUCCESS ( status ) )
                {
                // Set last error
                return ( -1L );
                }

            FilePosInfo.CurrentByteOffset = FileStdInfo.EndOfFile;

            break;

//        case SEEK_SET:
        default:

            FilePosInfo.CurrentByteOffset.LowPart  = 0L;
            FilePosInfo.CurrentByteOffset.HighPart = 0L;

        }

    if ( FilePosInfo.CurrentByteOffset.HighPart ||
         ( lOffset > 0 && FilePosInfo.CurrentByteOffset.LowPart + lOffset < FilePosInfo.CurrentByteOffset.LowPart ) ||
         ( lOffset < 0 && FilePosInfo.CurrentByteOffset.LowPart + lOffset > FilePosInfo.CurrentByteOffset.LowPart ) )
        {
        // Set last error to FILE_TOO_LARGE_FOR_QUAKE_API :-)
        return ( -1L );
        }

    FilePosInfo.CurrentByteOffset.LowPart += lOffset;

    status = ZvSetInformationFile ( (HANDLE)hFile,
                                    &IoStatusBlock,
                                    &FilePosInfo,
                                    sizeof(FilePosInfo),
                                    FilePositionInformation );

    if ( !NT_SUCCESS ( status ) )
        {
        // Set last error
        return ( -1L );
        }

    return ( FilePosInfo.CurrentByteOffset.LowPart );

#else

    _ClearErrorFar ( ) ;                // Reset so we don't catch someone
                                        // else's error
    _asm
        {
        MOV     BX,hFile                // Get the file handle
        MOV     CX,word ptr lOffset + 2 // Offset into CX:DX
        MOV     DX,word ptr lOffset
        mov     al, byte ptr iOrigin    // Get the method
        MOV     ah, 42h                 // Move to offset from start of file
        INT     21h                     // Move the file pointer
        CALL    _CheckErrorFar          // Check for disk errors
        JNC     DONE_LSEEK              // No error, we're all done
        XOR     AX,AX                   // Report that there was an error
        NOT     AX                      // Return -1L
        MOV     DX,AX
DONE_LSEEK:
        mov     word ptr lOffset,ax
        mov     word ptr lOffset + 2,dx
        call    FileSetError            // set AX to ERR if bad call
        }

    return (lOffset);

#endif // #if defined(SYM_NTK)
}
#pragma optimize("", on)

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      UINT PASCAL FileRead(HFILE hFile, void _huge *lpBuffer,           *
 *                         UINT wBytes)                                 *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      Returns the number of bytes actually read, or -1 for error.     *
 *      If read past EOF is requested, the function will return 0.      *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/02/1992 ED Created.                                               *
 ************************************************************************/

#pragma optimize("", off)
UINT WINAPI FileRead(HFILE hFile, void _huge *lpBuffer, UINT wBytes)
{
#if defined(SYM_NTK)

    auto NTSTATUS        status;
    auto IO_STATUS_BLOCK IoStatusBlock;

    SYM_ASSERT ( hFile != HFILE_ERROR );
    SYM_ASSERT ( lpBuffer );
    SYM_ASSERT ( wBytes && wBytes != -1 );

    status = ZvReadFile ( (HANDLE)hFile,
                          NULL,
                          NULL,
                          NULL,
                          &IoStatusBlock,
                          lpBuffer,
                          wBytes,
                          NULL,
                          NULL );

    if ( NT_SUCCESS ( status ) )
        {
        return ( IoStatusBlock.Information );
        }

                                        // This is not considered an error.

    if ( status == STATUS_END_OF_FILE )
        {
        return ( 0 );
        }

    // Set last error.

    return ( (UINT)-1 );

#else

    _ClearErrorFar ( ) ;                // Reset so we don't catch someone
                                        // else's error
    _asm
        {
        PUSH    DS
        MOV     BX,hFile
        MOV     CX,wBytes
        LDS     DX,lpBuffer
        MOV     AH,3Fh                  // Read a file
        INT     21h
        POP     DS
        call    FileCheckError          // set AX to ERR if bad call
        mov     wBytes,ax               // return bytes written (or error)
        }

    return (wBytes);

#endif // #if defined(SYM_NTK)
}
#pragma optimize("", on)

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      UINT PASCAL FileWrite(HFILE hFile, const void _huge *lpBuffer,    *
 *                         UINT wBytes)                                 *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *      Returns the number of bytes actually written, or -1 for error.  *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/02/1992 ED Created.                                               *
 ************************************************************************/

#pragma optimize("", off)
UINT WINAPI FileWrite(HFILE hFile, const void _huge *lpBuffer, UINT wBytes)
{
#if defined(SYM_NTK)

    auto NTSTATUS                     status;
    auto IO_STATUS_BLOCK              IoStatusBlock;
    auto FILE_POSITION_INFORMATION    Pos;
    auto FILE_END_OF_FILE_INFORMATION Eof;

    SYM_ASSERT ( hFile != HFILE_ERROR );
    SYM_ASSERT ( lpBuffer );
    SYM_ASSERT ( wBytes != -1 );

    if ( wBytes )
        {
        status = ZvWriteFile ( (HANDLE)hFile,
                               NULL,
                               NULL,
                               NULL,
                               &IoStatusBlock,
                               (PVOID)lpBuffer,
                               wBytes,
                               NULL,
                               NULL );

        if ( !NT_SUCCESS ( status ) )
            {
            // Set last error
            return ( (UINT)-1 );
            }
        }
    else
        {
                                        // Writing 0 bytes to file in DOS
                                        // truncates the file at the
                                        // current position.  This is not
                                        // how NT works, so we have to
                                        // implement that functionality.

        status = ZvQueryInformationFile ( (HANDLE)hFile,
                                          &IoStatusBlock,
                                          &Pos,
                                          sizeof(Pos),
                                          FilePositionInformation );

        if ( !NT_SUCCESS ( status ) )
            {
            // Set last error
            return ( (UINT)-1 );
            }

        Eof.EndOfFile = Pos.CurrentByteOffset;

        status = ZvSetInformationFile ( (HANDLE)hFile,
                                        &IoStatusBlock,
                                        &Eof,
                                        sizeof(Eof),
                                        FileEndOfFileInformation );

        if ( !NT_SUCCESS ( status ) )
            {
            // Set last error
            return ( (UINT)-1 );
            }

        return ( 0 );
        }

    return ( IoStatusBlock.Information );

#else

    _ClearErrorFar ( ) ;                // Reset so we don't catch someone
                                        // else's error
    _asm
        {
        PUSH    DS
        MOV     BX,hFile
        MOV     CX,wBytes
        LDS     DX,lpBuffer
        MOV     AH,40h                  // Write to the file
        INT     21h
        POP     DS
        call    FileCheckError          // set AX to ERR if bad call
        mov     wBytes,ax               // return bytes written (or error)
        }

    return (wBytes);

#endif // #if defined(SYM_NTK)
}
#pragma optimize("", on)
#endif

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      This is a DOS version of the undocumented Windows function by   *
 *      the same name.  It takes a pathname that includes environment   *
 *      variables and substitutes the value of the variable in the      *
 *      environment in place of the name.                               *
 *                                                                      *
 *      An environment name is designated by surrounding it with        *
 *      percent signs.  For example, the PATH variable would appear     *
 *      as "%PATH%".                                                    *
 *                                                                      *
 *      NOTE: This function does no checks for backslashes, path        *
 *      validity, or even legal path length.                            *
 *                                                                      *
 * Parameters:                                                          *
 *                                                                      *
 * Return Value:                                                        *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/20/1992 ED Created.                                               *
 ************************************************************************/

#if !defined(SYM_NTK)
#pragma optimize("", off)
DWORD WINAPI DoEnvironmentSubst (LPSTR lpName, UINT uSize)
{
    LPSTR       lpTemp;
    LPSTR       lpVarName;
    BYTE        szBuild [256];
    BYTE        szBuffer [256];
    BOOL        bFirst = TRUE;
    WORD        wIndex = 0;
    WORD        wLen;
    WORD        wLeft;
    WORD        wBack;

    lpTemp = lpName;
                                        // check for the end of the string
                                        // or end of buffer
    while (*lpTemp && wIndex < sizeof (szBuffer))
        {
        if (*lpTemp == '%')             // check for a '%'
            {
            if (!bFirst)                // is this the second one in a name?
                {
                                        // is this a double '%'
                if (lpTemp - 1 > lpVarName)
                    {
                    *lpTemp = '\0';     // terminate the end of the variable

#if defined(SYM_VXD)
// We haven't converted this so far, so dont' return ANYTHING
                    if (0)
#else
                                        // get the value of the variable
                    if (DOSGetEnvironment (lpVarName + 1, szBuild,
                                sizeof(szBuild) - 1) == (UINT)NOERR)
#endif
                        {
                        wLeft = sizeof(szBuild) - (lpTemp - lpName);

                        wBack = lpTemp - lpVarName;

                                        // is there enough room to copy?
                        wLen = STRLEN (szBuild);

                        if (wLen < wLeft)
                            {
                            wIndex -= wBack;
                            STRCPY (&szBuffer [wIndex], szBuild);
                            wIndex += wLen;
                            }
                        else            // copy the final '%'
                            szBuffer [wIndex++] = '%';
                        }
                    else                // store the '%' for a '%%' occurrence
                        szBuffer [wIndex++] = '%';
                    }

                lpTemp++;
                bFirst = TRUE;          // the next one will be the first
                continue;
                }
            else
                {
                lpVarName = lpTemp;     // store the start of the name
                bFirst = FALSE;         // the next one won't be the first
                }
            }

        if (DBCSIsLeadByte(*lpTemp))
            szBuffer [wIndex++] = *lpTemp++;
        szBuffer [wIndex++] = *lpTemp++;
        }

    if (wIndex < uSize)
        {
        szBuffer [wIndex] = 0;
        STRCPY (lpName, szBuffer);

        _asm        mov     dx,1        // Windows rigs the return code
        return (wIndex);
        }
    else
        {
        _asm        mov     dx,0        // Windows rigs the return code
        return (STRLEN (lpName));
        }
}
#pragma optimize("", on)
#endif // #if !defined(SYM_NTK)

#endif

