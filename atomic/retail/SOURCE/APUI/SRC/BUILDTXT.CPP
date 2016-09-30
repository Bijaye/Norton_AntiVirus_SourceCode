// Copyright 1994 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APUI/VCS/BUILDTXT.CPv   1.5   10 Jun 1998 15:55:38   RStanev  $
//
// Description:
//      This file defines functions for building text strings for
//      NAVAPMessage().
//
// Contains:
//      BuildText()
//      BuildButtons()
//      BuildBoxSize()
//      GetDefaultButton()
//      ParseLine()         (local function)
//
// See Also:
//      apui.h
//
// Note:
//      UI module
//
//***************************************************************************
// $Log:   S:/APUI/VCS/BUILDTXT.CPv  $
// 
//    Rev 1.5   10 Jun 1998 15:55:38   RStanev
// BuildBoxSize() should disregard SYMEvent's special RECTs for closing
// message boxes with no buttons.
//
//    Rev 1.4   20 Feb 1998 19:31:06   RStanev
// Changed calculation of lines for message boxes due to a bug in SYMEvent.
//
//    Rev 1.3   20 Feb 1998 17:16:46   RStanev
// Added dynamic adjustment of message boxes.
//
//    Rev 1.2   17 Feb 1998 20:53:52   RStanev
// Updated copyright.
//
//    Rev 1.1   15 May 1997 02:22:20   RStanev
// Updated to service the new initialization functions.
//
//    Rev 1.0   09 Apr 1997 12:15:58   RStanev
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "xapi.h"
#include "navap.h"
#include "apui.h"

//---------------------------------------------------------------------------
//
// Local defines
//
// Note:
//      BUTTON_BASE should not be changed during localization.
//
//---------------------------------------------------------------------------
#define BUTTON_BASE                     ( '0' )

//---------------------------------------------------------------------------
//
// Local function prototypes
//
//---------------------------------------------------------------------------
PTSTR ParseLine (
    PTSTR pszString,
    UINT  uMaxLineSize
    );

//---------------------------------------------------------------------------
//
// UINT BuildText (
//      void  **ppvTextTable,
//      void  **ppvParameterTable,
//      PTSTR pszText
//      );
//
// This function builds a text line from ppvTextTable using the parameters
// in ppvParameterTable and places it in pszText and/or calculates the
// length of the buffer for pszText.
//
// Entry:
//      ppvTextTable      - pointer to a text build table
//      ppvParameterTable - pointer to an array of parameters
//      pszText           - an ASCIIZ string to append the new text to or
//                          NULL if only string length should be calculated.
//
// Exit:
//      Returns the number of bytes for pszText, including terminating '\0'.
//      If pszText != NULL, the message in ppvTextTable appended to pszText.
//
// Notes:
//      Below is described the format of the two tables used:
//
//      ppvTextTable:
//          This is an array of void*.  Each member of the array can point
//          to a string or be NULL.  If it points to a string, this string
//          will automatically be appended to pszText.  If this member is
//          NULL, then the next member contains enumUIMessageNumbers.
//          If the enumerator is 0 (_UI_END_OF_TABLE), then the function
//          will stop processing.  Data associated with other enumerators
//          will be looked up in ppvParameterTable and will be appended
//          to the string.  There are two types of enumerators, first,
//          enumerators which use their field in ppvParameterTable as a
//          pointer to a string which will automatically be appended to
//          pszText, and second, enumerators which use their field in
//          ppvParameterTable to enumerate string in a string array,
//          which has to be appended to pszText.
//
//      ppvParameterTable:
//
//          This is an array of void*.  The size of this array is
//          determined by the size of enumUIMessageNumbers, which is
//          _UI_PARAMETER_TABLE_SIZE.  Each array member contains data
//          associated with the enumerator.  Some enumerators use their
//          member as a pointer to a string, others use their member
//          to enumerate string in a string array.
//
//          The following enumerators have use their part of ppvParameter
//          table as pointers to strings:
//
//          _UI_VIRUS_NAME
//          _UI_FILE_IN_TROUBLE
//          _UI_DRIVE_IN_TROUBLE
//          _UI_FILE_CAUSING_TROUBLE
//          _UI_DRIVE_TO_SCAN
//
//          The following enumerators use their part of ppvParameterTable
//          to point to string enumerators in a string array:
//
//          _UI_INIT_ERRORS             - points to string enumerator for
//                                        apszInitErrors
//          _UI_CONTROLLEDINIT_MESSAGES - points to string enumerator for
//                                        apszControlledInitMessages
//
//  Note:
//      The _UI_FILE_IN_TROUBLE and _UI_FILE_CAUSING_TROUBLE parameters will
//      be shortened before appending them to pszText.
//
//---------------------------------------------------------------------------
UINT BuildText (
    void  **ppvTextTable,
    void  **ppvParameterTable,
    PTSTR pszText
    )
    {
    auto UINT  uLength;
    auto UINT  uCurrentTableString;
    auto BOOL  bEndOfTable;
    auto UINT  uMessage;

                                        // Validate input parameters.

    SYM_ASSERT ( ppvTextTable );

                                        // Walk through each one of the
                                        // members of ppvTextTable, add
                                        // it to pszText and increment
                                        // total string length.

    for ( uLength = uCurrentTableString = 0, bEndOfTable = FALSE;
          !bEndOfTable;
          uCurrentTableString++ )
        {

        if ( ppvTextTable[uCurrentTableString] )
            {

                                        // a regular text string

            uLength += STRLEN ( (PTSTR)ppvTextTable[uCurrentTableString] );

            if ( pszText )
                {
                STRCAT ( pszText, (PTSTR)ppvTextTable[uCurrentTableString] );
                }
            }
        else
            {

                                        // get the message string enumeration

            uMessage = (UINT)ppvTextTable[++uCurrentTableString];

                                        // a string parameter or end-of-table

            switch ( uMessage )
                {

                                        // these are regular strings

                case _UI_VIRUS_NAME:
                case _UI_FILE_IN_TROUBLE:
                case _UI_DRIVE_IN_TROUBLE:
                case _UI_FILE_CAUSING_TROUBLE:
                case _UI_DRIVE_TO_SCAN:

                    SYM_ASSERT ( ppvParameterTable );

                    uLength += STRLEN ( (PTSTR)ppvParameterTable[uMessage] );

                    if ( pszText )
                        {
                        if ( uMessage == _UI_FILE_IN_TROUBLE ||
                             uMessage == _UI_FILE_CAUSING_TROUBLE )
                            {
                            NameShortenFileName (
                                (PTSTR)ppvParameterTable[uMessage],
                                &pszText[STRLEN ( pszText )],
                                64
                                );
                            }
                        else
                            {
                            STRCAT (
                                pszText,
                                (PTSTR)ppvParameterTable[uMessage]
                                );
                            }
                        }

                    break;

#if defined(SYM_VXD)

                                        // these are enumerated strings

                case _UI_INIT_ERRORS:
                case _UI_CONTROLLEDINIT_MESSAGES:

                    auto TCHAR **apszStringTable;

                    SYM_ASSERT ( ppvParameterTable );

                    if ( uMessage == _UI_INIT_ERRORS )
                        {
                        apszStringTable = apszInitErrors;
                        }
                    else
                        {
                        apszStringTable = apszControlledInitMessages;
                        }

                    uLength += STRLEN ( apszStringTable[ *(UINT*)ppvParameterTable[uMessage] ] );

                    if ( pszText )
                        {
                        STRCAT (
                            pszText,
                            apszStringTable[ *(UINT*)ppvParameterTable[uMessage] ]
                            );
                        }

                    break;

#endif // #if defined(SYM_VXD)

                                        // anything else (should only be
                                        // NULL, though) marks end-of-table.

                default:

                    SYM_ASSERT ( !uMessage );

                    bEndOfTable = TRUE;
                }
            }
        }

    return ( ( uLength + 1 ) * sizeof(TCHAR) );
    }

//---------------------------------------------------------------------------
//
// UINT BuildButtons (
//      PTSTR pszButtonLine,
//      UINT  uButtonsAllowed
//      );
//
// This function creates a button list for NAVAPMessage().  The button
// IDs in the list are created so that they match the enumerations in
// AVACTION.
//
// Entry:
//      pszButtonLine   - an ASCIIZ string to append the button list to, or
//                        NULL if only the length of appended string required
//      uButtonsAllowed - PROMPTACTION_ flags with buttons which should be
//                        added
//
// Exit:
//      Returns the length (in bytes, including terminating '\0') of the
//      string that has been appended or will be appended to pszButtonLine.
//      pszButtonLine contains new buttons, or no changes if NULL.
//
// Note:
//      The strings in this function do not need translation.
//      If uButtonsAllowed bits which do not have corresponding buttons
//      are set, there might be a protection fault when this function is
//      trying to access strings associated with a button.
//
//---------------------------------------------------------------------------
UINT BuildButtons (
    PTSTR pszButtonLine,
    UINT  uButtonsAllowed
    )
    {
    auto UINT   uLength;
    auto TCHAR  szButtonID[2];
    auto UINT   uCurrentButton;

                                        // Initialize local variables.

    szButtonID[1] = '\0';

                                        // Scan each bit of uButtonsAllowed.

    for ( uCurrentButton = uLength = 0;
          uCurrentButton < sizeof(uButtonsAllowed) * 8;
          uCurrentButton++ )
        {

                                        // If bit set, then add button.

        if ( uButtonsAllowed & 1 )
            {
            szButtonID[0] = (TCHAR) uCurrentButton + BUTTON_BASE;

                                        // Append to string.

            if ( pszButtonLine )
                {
                STRCAT ( pszButtonLine, szButtonID );
                STRCAT ( pszButtonLine, szButtonLeftBracket );
                STRCAT ( pszButtonLine, apszButtons[uCurrentButton] );
                STRCAT ( pszButtonLine, szButtonRightBracket );
                }

                                        // Calculate size.

            uLength += STRLEN ( szButtonID ) +
                       STRLEN ( szButtonLeftBracket ) +
                       STRLEN ( apszButtons[uCurrentButton] ) +
                       STRLEN ( szButtonRightBracket );
            }

        uButtonsAllowed >>= 1;
        }

    return ( ( uLength + 1 ) * sizeof(TCHAR) );
    }

//---------------------------------------------------------------------------
//
// VOID BuildBoxSize (
//      LPRECT lprBoxTemplate,
//      PTSTR  pszText,
//      PTSTR  pszUser,
//      PTSTR  pszButtons
//      );
//
// This function makes sure that an LPRECT structure passed to SYMEvent's
// AsyncMsgBox() function will be able to contain the full text of the
// message displayed.
//
// Entry:
//      lprBoxTemplate - Template of a RECT structure.  The top and bottom
//                       values of this template, which have been OR-ed with
//                       0x8000L will be adjusted so that the text defined
//                       by pszText and pszButtons can fit on the screen.
//      pszText        - Text line passed to AsyncMsgBox() (can be NULL).
//      pszUser        - User line passed to AsyncMsgBox() (can be NULL).
//      pszButtons     - Buttons passed to AsyncMsgBox() (can be NULL).
//
// Exit:
//      -
//      lprBoxTemplate->bottom is adjusted so that pszText and pszButtons
//      can fit in AsyncMsgBox().
//
// Note:
//      lprBoxTemplate->top should not be OR-ed with 0x8000 unless
//      lprBoxTemplate->bottom is OR-ed with that value.
//
//---------------------------------------------------------------------------
VOID BuildBoxSize (
    LPRECT lprBoxTemplate,
    PTSTR  pszText,
    PTSTR  pszUser,
    PTSTR  pszButtons
    )
    {
    auto UINT uCharsPerLine;
    auto BOOL bVerticalCenter;
    auto UINT uLineCount;
    auto UINT uTemplateLines;

                                        // Validate input parameters.

    SYM_ASSERT ( lprBoxTemplate );

                                        // Before we complete validation,
                                        // check if this is a SYMEvent rect
                                        // which closes a message box
                                        // without any buttons.  This rect
                                        // should have its members set to 0.
                                        // No adjustment should be done for
                                        // such rects.

    if ( !lprBoxTemplate->left &&
         !lprBoxTemplate->right &&
         !lprBoxTemplate->top &&
         !lprBoxTemplate->bottom )
        {
        return;
        }

                                        // Continue parameter validation.

    SYM_ASSERT ( lprBoxTemplate->right > lprBoxTemplate->left + 4 );
    SYM_ASSERT ( (lprBoxTemplate->bottom & (~0x8000L)) >
                 (lprBoxTemplate->top & (~0x8000L)) + 1 );

                                        // If we are working on a fixed RECT
                                        // structure, don't adjust anything.

    if ( lprBoxTemplate->bottom & 0x8000L )
        {
        lprBoxTemplate->bottom &= (~0x8000L);

                                        // Check if we need to adjust only
                                        // RECT.bottom, or if we need to
                                        // evenly adjust RECT.bottom and
                                        // RECT.top.

        if ( lprBoxTemplate->top & 0x8000L )
            {
            lprBoxTemplate->top &= (~0x8000L);

            bVerticalCenter = TRUE;
            }
        else
            {
            bVerticalCenter = FALSE;
            }

                                        // Calculate the number of chars per
                                        // line.  Leave two chars for border,
                                        // and two chars for space around the
                                        // border.

        uCharsPerLine = lprBoxTemplate->right - lprBoxTemplate->left - 3;

                                        // Due to a bug in SYMEvent, where it
                                        // leaves two empty spaces from the
                                        // right border, we need to adjust
                                        // uCharsPerLine.

        uCharsPerLine--;

                                        // Calculate the number of lines
                                        // excluding border lines.

        uLineCount = 0;

                                        // Determine how many lines we need
                                        // for pszText.

        while ( pszText )
            {
            uLineCount++;

            pszText = ParseLine ( pszText, uCharsPerLine );
            }

                                        // Add an empty line between pszText
                                        // and pszUser or pszButtons.

        uLineCount++;

                                        // Determine how many lines we need
                                        // for pszUser.  Currently SYMEvent
                                        // always allocates two if a line is
                                        // present, and none if line is not
                                        // present.

        if ( pszUser )
            {
            uLineCount += 2;
            }

                                        // Determine how many lines we need
                                        // for buttons.  Note, that this is
                                        // currently hard coded for 2, which
                                        // is one more than what the current
                                        // AsyncMsgBox() in SYMEvent
                                        // supports.

        uLineCount += 2;

                                        // If the produced line count is
                                        // greater than what was given to us
                                        // in the template, then adjust the
                                        // template.

        uTemplateLines = lprBoxTemplate->bottom - lprBoxTemplate->top - 1;

        if ( uTemplateLines < uLineCount )
            {
            if ( bVerticalCenter )
                {
                                        // Adjust the top and bottom borders
                                        // evenly.

                lprBoxTemplate->top -= ( uLineCount - uTemplateLines ) / 2;

                                        // We can't go negative here.

                if ( int(lprBoxTemplate->top) < 0 )
                    {
                    lprBoxTemplate->top = 0;
                    }

                lprBoxTemplate->bottom = lprBoxTemplate->top + uLineCount + 1;
                }
            else
                {
                                        // Adjust only the bottom border.

                lprBoxTemplate->bottom += uLineCount - uTemplateLines;
                }

                                        // We can't go past uMessageBoxBottom.

            if ( lprBoxTemplate->bottom > uMessageBoxBottom )
                {
                lprBoxTemplate->bottom = uMessageBoxBottom;
                }
            }
        }
    }

//---------------------------------------------------------------------------
//
// UINT GetDefaultButton (
//      PTSTR pszButtonLine
//      );
//
// This function returns the default button for NAVAP message with
// pszButtonLine buttons.
//
// Entry:
//      pszButtonLine - an ASCIIZ string with list of buttons, formatted
//                      for NAVAPMessage().
//
// Exit:
//      Returns the default button.
//
// Notes:
//      There is no check for validating *pszButtonLine.
//      The strings in this file should not be translated.
//
//---------------------------------------------------------------------------
UINT GetDefaultButton (
    PTSTR pszButtonLine
    )
    {
                                        // Validate input parameters.

    SYM_ASSERT ( pszButtonLine );
    SYM_ASSERT ( STRLEN ( pszButtonLine ) >= STRLEN ( _T("?[?]") ) );

                                        // Return first button.

    return ( (UINT)(*pszButtonLine - BUTTON_BASE) );
    }

//---------------------------------------------------------------------------
//
//  PTSTR ParseLine (
//      PTSTR pszString,
//      UINT  uMaxLineSize
//      );
//
// This function parses a long string and finds the end of a text line with
// a specific character length.  The code in this function must be in sync
// with SYMEvent's AsyncMsgBox().
//
// Entry:
//      pszString    - string to parse.
//      uMaxLineSize - maximum number of characters per line.
//
// Exit:
//      Returns a pointer to the start of the next line, or NULL if end of
//      string reached.
//
//---------------------------------------------------------------------------
PTSTR ParseLine (
    PTSTR pszString,
    UINT  uMaxLineSize
    )
    {
    auto PTSTR pszDelimiter;

                                        // Validate input parameters.

    SYM_ASSERT ( pszString );
    SYM_ASSERT ( uMaxLineSize > 2 );

                                        // We will update this value as soon
                                        // as we find white space.

    pszDelimiter = NULL;

                                        // Find the end of the line.

    while ( uMaxLineSize )
        {
        switch ( *pszString )
            {
            case _T('\0'):
                                        // Null terminating character stops
                                        // parsing and tells the client that
                                        // there are no more lines.

                pszDelimiter = pszString = NULL;

                uMaxLineSize = 0;

                break;

            case _T(' '):
                                        // The last space is where we will
                                        // end the line if the word after it
                                        // does not fit on the line.

                pszDelimiter = ++pszString;

                uMaxLineSize--;

                break;

            case _T('\n'):
                                        // New line forces us to end parsing
                                        // right away.  The beginning of the
                                        // next line will be the character
                                        // after the new line.

                pszDelimiter = pszString + 1;

                uMaxLineSize = 0;

                break;

            case _T('\r'):
                                        // CR forces us to end parsing right
                                        // away.  The beginning of the next
                                        // line will be the character after
                                        // the CR, unless that character is
                                        // new line.  In that case, the next
                                        // line will start at the character
                                        // after the new line.

                if ( *( pszDelimiter = pszString + 1 ) == _T('\n') )
                    {
                    pszDelimiter++;
                    }

                uMaxLineSize = 0;

                break;

            default:
                                        // Any other character will just be
                                        // part of the line.

                if ( CharNext(pszString) != pszString + 1 )
                    {
                                        // Make sure that a DBCS lead byte
                                        // doesn't end our line.

                    if ( uMaxLineSize == 1 )
                        {
                        pszString--;
                        }
                    else
                        {
                        pszString++;
                        uMaxLineSize--;
                        }
                    }

                pszString++;
                uMaxLineSize--;
            }
        }

    return ( pszDelimiter ? pszDelimiter : pszString );
    }
