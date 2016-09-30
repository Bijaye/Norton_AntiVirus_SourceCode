/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/bufferio.c_v   1.1   25 Jul 1996 11:10:06   BMCCORK  $ *
 *                                                                      *
 * Description:                                                         *
 *      Buffered File I/O.                                              *
 *                                                                      *
 *      These routines are just like the old BFxxx routines, but they   *
 *      have been rewritten for C and also support FAR buffers and      *
 *      HUGE buffers (old one only allowed NEAR buffers, whose size     *
 *      could not exceed 64K).                                          *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/bufferio.c_v  $ *
// 
//    Rev 1.1   25 Jul 1996 11:10:06   BMCCORK
// Changed FileBufferedGetLine to use buffer length instead of string length and added asserts
// 
//    Rev 1.0   26 Jan 1996 20:21:28   JREARDON
// Initial revision.
// 
//    Rev 1.5   04 May 1994 00:11:04   BRAD
// Don't use NFileOpen()
// 
//    Rev 1.4   15 Mar 1994 12:32:26   BRUCE
// Changed EXPORT to SYM_EXPORT
// 
//    Rev 1.3   25 Feb 1994 12:23:18   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
// 
//    Rev 1.2   09 Dec 1993 17:09:42   MARKK
// Modified for windows
// 
//    Rev 1.1   13 Oct 1993 15:30:26   MFALLEN
// Added all the changes from Quake branch 1
// 
//    Rev 1.0.2.1   18 Sep 1993 19:11:52   REESE
// Added bFilter argument to FileBufferedGetLine().
// 
//    Rev 1.0.2.0   03 Sep 1993 22:19:18   SCOTTP
// Branch base for version QAK1
// 
//    Rev 1.0   10 Apr 1993 17:27:12   BRAD
// Initial revision.
 ************************************************************************/

#include "platform.h"
#include "stddos.h"
#include "file.h"


//************************************************************************
// LOCAL PROTOTYPES
//************************************************************************
DWORD   LOCAL PASCAL _FileBufferedReadBlock(LPFILE_BUFFERED pfb);
VOID    LOCAL PASCAL _FileBufferedReadNextBlock(LPFILE_BUFFERED pfb);
DWORD   LOCAL PASCAL _FileBufferedSeek(LPFILE_BUFFERED pfb, DWORD dwNewPtr);

#ifndef CR
#define         CR              0x00D
#define         LF              0x00A
#define         CTRL_Z          0x01A
#endif

//************************************************************************
// FileBufferedOpen()
//
// Opens a file for buffered I/O.
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 03/07/93 BRAD Function created.
//************************************************************************

UINT WINAPI FileBufferedOpen (LPFILE_BUFFERED pfb, LPSTR szFilename, 
                              HPBYTE hpBuffer, DWORD dwBufferSize, 
                              UINT uMode)
{
    auto        HFILE           hFile;

                                        // Open the file
    hFile = FileOpen(szFilename, uMode);   
                                        // If error, return error
    if ( hFile == HFILE_ERROR )
        return((UINT) HFILE_ERROR);
                                        // Fill in our FILE_BUFFERED struct
    pfb->hFile = hFile;
    pfb->hpBuffer = hpBuffer;
    pfb->dwBufferSize = dwBufferSize;
    pfb->dwBufferBase = 0;
                                        // Read the first block into memory
    return ( (UINT) _FileBufferedReadBlock(pfb) );
}



//************************************************************************
// FileBufferedClose()
//
// Closes a buffered file.
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 03/07/93 BRAD Function created.
//************************************************************************

HFILE WINAPI FileBufferedClose(LPFILE_BUFFERED pfb)
{
    return ( FileClose(pfb->hFile) );              // Close the file
}



//************************************************************************
// FileBufferedGetChar
//
// Gets the next buffered character.
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 03/07/93 BRAD Function created.
//************************************************************************

int WINAPI FileBufferedGetChar(LPFILE_BUFFERED pfb)
{
    auto        int     iChar;


    while (TRUE)
        {
                                        // If already in buffer, just return it
        if ( pfb->dwBufferOff < pfb->dwCharsInBuffer )
            {                           // Get character
            iChar = pfb->hpBuffer[pfb->dwBufferOff++];            
            break;
            }
                                        // If EOF, return it
        if ( pfb->dwBufferOff < pfb->dwBufferSize )
            {
            iChar = -1;                 // Return EOF flag
            break;
            }
        
        _FileBufferedReadNextBlock(pfb);
        }

    return( iChar );
}



//************************************************************************
// FileBufferedReverseGetChar()
//
// Reverse-gets the previous character.
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 03/07/93 BRAD Function created.
//************************************************************************

int WINAPI FileBufferedReverseGetChar(LPFILE_BUFFERED pfb)
{
    auto        int     iChar = 0;


    if ( pfb->dwBufferOff == 0 )
        {
        if ( pfb->dwBufferBase == 0 )
            {
            iChar = -1;               // At beginning of file. Error
            }
        else
            {
            if ( FileBufferedSetPtr(pfb, pfb->dwBufferBase - 1) == HFILE_ERROR )
                iChar = -1;           // Error                         
            }
        }

    if ( iChar == 0 )
        iChar = pfb->hpBuffer[--pfb->dwBufferOff];            

    return (iChar);
}



//************************************************************************
// FileBufferedUngetChar()
//
// Ungets the current character.
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 03/07/93 BRAD Function created.
//************************************************************************

VOID WINAPI FileBufferedUngetChar(LPFILE_BUFFERED pfb)
{
    if ( pfb->dwBufferOff != 0 )
        pfb->dwBufferOff--;
}


//************************************************************************
// FileBufferedSetPtr()
//
// Sets the buffered file pointer.
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 03/07/93 BRAD Function created.
//************************************************************************

DWORD WINAPI FileBufferedSetPtr(LPFILE_BUFFERED pfb, DWORD dwNewPtr)
{
    auto        DWORD           dwHalfway;
    auto        DWORD           dwPtr;


    if ( pfb->dwBufferBase > dwNewPtr )
        {
                                        // Need new block previous in file
                                        // Only move 1/2 into block, to speed
                                        // up moving backwards through a file
        dwHalfway = pfb->dwBufferSize / 2;
        dwPtr = ( dwNewPtr > dwHalfway ) ? dwNewPtr - dwHalfway : 0;
        pfb->dwBufferBase = _FileBufferedSeek(pfb, dwPtr);
        if ( _FileBufferedReadBlock(pfb) == HFILE_ERROR )
            dwNewPtr = (DWORD) HFILE_ERROR;
        else
            FileBufferedSetPtr(pfb, dwNewPtr);
        }
    else if ( pfb->dwBufferBase + pfb->dwCharsInBuffer > dwNewPtr )
        {
                                        // Point within this block
        pfb->dwBufferOff = dwNewPtr - pfb->dwBufferBase;
        }
    else
        {
                                        // Need new block further in file
        pfb->dwBufferBase = _FileBufferedSeek(pfb, dwNewPtr);
        if ( _FileBufferedReadBlock(pfb) == HFILE_ERROR )
            dwNewPtr = (DWORD) HFILE_ERROR;
        }
                                        // Return new ptr
    return ( dwNewPtr );
}


//************************************************************************
// FileBufferedGetPtr
//
// Returns the buffered file pointer.
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 03/07/93 BRAD Function created.
//************************************************************************

DWORD WINAPI FileBufferedGetPtr(LPFILE_BUFFERED pfb)
{
    return (pfb->dwBufferBase + pfb->dwBufferOff);
}


//************************************************************************
// FileBufferedLength()
//
// Returns the length of the file.
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 03/07/93 BRAD Function created.
//************************************************************************

DWORD WINAPI FileBufferedLength(LPFILE_BUFFERED pfb)
{
    return ( FileLength(pfb->hFile) );
}


/************************************************************************
 * This procedure reads one line from viewed data stream. It returns    
 * the ASCIIZ string with the end-of-line characters stripped off.      
 *                                  
 * If bFilter is set, lots of characters are mapped into periods, to    
 * clean up the display of binary files.  The NULL character MUST be    
 * mapped to something else, or it will terminate the string            
 * prematurely.                                                 
 *                            
 * A line ends with CR, LF, CR/LF, LF/CR, or CTRL_Z.            
 *                                  
 * Returns: -1  in case of an error, or end of file.        
 *       Length of string otherwise.                
 ************************************************************************/
//************************************************************************
// 03/07/93 BRAD Function created.
// 07/25/96 BEM  Modified to use buffer size instead of line length to
//               avoid confusion.
//************************************************************************

int SYM_EXPORT WINAPI FileBufferedGetLine(
    LPFILE_BUFFERED pfb,                // [in] buffered file struct to read from
    LPSTR lpszLineBuffer,               // [in/out] Buffer to receive line
    UINT uLineBufferSize,               // [in] Size in BYTES of the buffer
    BOOL bFilter)                       // [in] map unprintable chars to '.'
{
    auto    int     c;
    auto    int     nextC;
    auto    UINT        i, j;
                                        // uMaxLineBufferIndex is the index of the last possible character 
                                        // in the lpszLineBuffer (buffer size -1)
    auto    UINT    uMaxLineBufferIndex=uLineBufferSize-1;

                                        // Verify that the pointers are not NULL
    SYM_ASSERT(lpszLineBuffer);
    SYM_ASSERT(pfb);
                                        // Verify that each byte of the buffer can be read and written to
                                        // (only if SYM_PARANOID is defined)
    SYM_VERIFY_BUFFER(lpszLineBuffer, uLineBufferSize);

    for (i = 0; i < uMaxLineBufferIndex; i++)
        {
        c = FileBufferedGetChar(pfb);

        if (c == CR || c == LF || c == CTRL_Z || c == -1)
            {
            if (c == -1 && i == 0)
                {
                lpszLineBuffer[i] = '\0';
                return (-1);
                }
            lpszLineBuffer[i++] = EOS;
            if (i < uMaxLineBufferIndex && (c == CR || c == LF))
                {
                nextC = FileBufferedGetChar(pfb);
                if (nextC != -1 && (nextC == CR || nextC == LF) && nextC != c)
                    i++;
                }
            break;
            }
        else if (bFilter && c == '\t')
            {
            for (j = 8 - (i % 8); j && i != uMaxLineBufferIndex; j--)
                {
                lpszLineBuffer[i++] = ' ';
                }
            if (i == uMaxLineBufferIndex)
                break;
            i--;
            }
        else if (bFilter && c < 0x20)
            lpszLineBuffer[i] = '.';
        else
            lpszLineBuffer[i] = (char) c;
        }

    SYM_ASSERT(i <= uMaxLineBufferIndex);    
    if (i == uMaxLineBufferIndex)
        lpszLineBuffer[i] = '\0';

                                        // Verify that the string is NULL terminated (only if SYM_PARANOID is defined)
    SYM_VERIFY_STRING(lpszLineBuffer);    
    #ifndef SYM_WIN
    FastFilterString(lpszLineBuffer);
    #endif

    return( STRLEN(lpszLineBuffer) );
}


//************************************************************************
// _FileBufferedSeek()
//
// Seeks to a specific offset.
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 03/07/93 BRAD Function created.
//************************************************************************

DWORD LOCAL PASCAL _FileBufferedSeek(LPFILE_BUFFERED pfb, DWORD dwNewPtr)
{
    return( FileSeek(pfb->hFile, dwNewPtr, SEEK_FROM_BEGINNING) );
}


//************************************************************************
// _FileBufferedReadBlock()
//
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 03/07/93 BRAD Function created.
//************************************************************************

DWORD LOCAL PASCAL _FileBufferedReadBlock(LPFILE_BUFFERED pfb)
{
    auto        DWORD   dwResult;


    pfb->dwBufferOff = 0;               // At beginning of buffer
    dwResult = FileHugeRead(pfb->hFile, pfb->hpBuffer, pfb->dwBufferSize);
    if (dwResult != HFILE_ERROR )
        pfb->dwCharsInBuffer = dwResult;

    return (dwResult);
}


//************************************************************************
// _FileBufferedReadNextBlock()
//
// Parameters:
//  none
//
// Returns:
//  nothing
//************************************************************************
// 03/07/93 BRAD Function created.
//************************************************************************

VOID LOCAL PASCAL _FileBufferedReadNextBlock(LPFILE_BUFFERED pfb)
{
    pfb->dwBufferBase = FileBufferedSetPtr(pfb, 
                                        pfb->dwBufferBase + pfb->dwBufferSize);
}


