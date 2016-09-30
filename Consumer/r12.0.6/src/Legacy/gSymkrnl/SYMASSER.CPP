#include "platform.h"

#ifdef SYM_WIN

#ifndef SYM_DEBUG

// --------------------------------------------------------------------------
// This is required since if the codeview options is not turned on
// this function will not have a prototype.
// --------------------------------------------------------------------------

EXTERNC VOID WINAPI AssertFailedLine(// Display assertion failed information
    const LPSTR lpFileName,     // [in] name of the file that err
    int         iLine );        // [in] line number where it occured

#endif

// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: AssertFailedLine()
//
// Author:      Peter Graves                        Date: July 24, 1994
//              Martin Fallenstedt
//
// Description: When compiling with debug information turned on the SYM_ASSERT
//              macro will trigger this function that displays the file and
//              line number where the error condition occured.
//
// Input:       lpFileName,     Name of the file where assertion occured.
//              iLine,          Line number where the assertion occured.
//
// Output:      Nothing
//
// --------------------------------------------------------------------------

VOID WINAPI AssertFailedLine(           // Display assertion failed information
    const LPSTR lpFileName,             // [in] name of the file that err
    int         iLine )                 // [in] line number where it occured
{
    auto    char    szMsg[256];

    SPRINTF ( szMsg, "Assertion failed: %s, line %d", lpFileName, iLine );

    int nCode = MessageBox( NULL, szMsg, "Assert Error",
                            MB_SYSTEMMODAL | MB_ICONHAND | MB_ABORTRETRYIGNORE );

    if ( nCode == IDIGNORE )
        return;
    else if ( nCode == IDRETRY )
        DebugBreak( );
    else
        FatalExit( 0 );
}
#endif  // SYM_WIN




#ifdef SYM_NLM

#include <conio.h>          // ConsolePrintf

// this generates an in-line int 3

VOID DebugBreak( VOID );
#pragma aux DebugBreak    = 0xcc parm caller [];

#ifndef SYM_DEBUG

// --------------------------------------------------------------------------
// This is required since if the codeview options is not turned on
// this function will not have a prototype.
// --------------------------------------------------------------------------

EXTERNC VOID WINAPI AssertFailedLine(// Display assertion failed information
    const LPSTR lpFileName,     // [in] name of the file that err
    int         iLine );        // [in] line number where it occured


#endif

// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: AssertFailedLine()
//
// Author:      Peter Graves                        Date: July 24, 1994
//              Martin Fallenstedt
//
// Description: When compiling with debug information turned on the SYM_ASSERT
//              macro will trigger this function that displays the file and
//              line number where the error condition occured.
//
// Input:       lpFileName,     Name of the file where assertion occured.
//              iLine,          Line number where the assertion occured.
//
// Output:      Nothing
//
// --------------------------------------------------------------------------

VOID WINAPI AssertFailedLine(           // Display assertion failed information
    const LPSTR lpFileName,             // [in] name of the file that err
    int         iLine )                 // [in] line number where it occured
{
    ConsolePrintf ( "Assertion failed: %s, line %d", lpFileName, iLine );

    DebugBreak( );

}
#endif  // SYM_NLM






// this section is for command-line mode DOS - it won't work if you are
// in graphics mode

#if defined(SYM_DOS) || defined(SYM_X86)

#include <stdio.h>          // fprintf

#ifndef SYM_DEBUG

// --------------------------------------------------------------------------
// This is required since if the codeview options is not turned on
// this function will not have a prototype.
// --------------------------------------------------------------------------

EXTERNC VOID WINAPI AssertFailedLine(// Display assertion failed information
    const LPSTR lpFileName,     // [in] name of the file that err
    int         iLine );        // [in] line number where it occured


#endif

// --------------------------------------------------------------------------
// (C) Copyright Symantec Corporation,  Peter Norton Group.         1994
// --------------------------------------------------------------------------
//
// Function Name: AssertFailedLine()
//
// Author:      Peter Graves                        Date: July 24, 1994
//              Martin Fallenstedt
//
// Description: When compiling with debug information turned on the SYM_ASSERT
//              macro will trigger this function that displays the file and
//              line number where the error condition occured.
//
// Input:       lpFileName,     Name of the file where assertion occured.
//              iLine,          Line number where the assertion occured.
//
// Output:      Nothing
//
// --------------------------------------------------------------------------

VOID WINAPI AssertFailedLine(           // Display assertion failed information
    const LPSTR lpFileName,             // [in] name of the file that err
    int         iLine )                 // [in] line number where it occured
{
    fprintf (stderr, "\nAssertion failed: %s, line %d", lpFileName, iLine );

    __asm
        {
        int 3
        }
}
#endif  // SYM_DOS

