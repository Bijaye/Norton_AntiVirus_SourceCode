/* Copyright 1991 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   I:/INCLUDE/VCS/WINMGR.H_V   1.20   22 Jan 1993 10:43:48   CRAIG  $ *
 *                                                                      *
 * Description:                                                         *
 *  This include file defines the structures and constants for the      *
 *  movable window managent system.                                     *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   I:/INCLUDE/VCS/WINMGR.H_V  $ *
 * 
 *    Rev 1.20   22 Jan 1993 10:43:48   CRAIG
 * Added WinClearBackgroundPlane and WinRestoreBackgroundPlane.
 *
 *    Rev 1.19   30 Oct 1992 07:48:16   MARKK
 * Window and VM Manager changes
 *
 *    Rev 1.18   27 Apr 1992 13:45:04   MARKK
 * Added back tracking functions
 *
 *    Rev 1.17   27 Apr 1992 13:28:30   BRAD
 * Combined WinChangeSizeTrackMouse() and WinChangeSizeTrackKeyboard()
 * into WinChangeSizeTrack().  Almost all of code was same.
 *
 *
 *    Rev 1.16   21 Apr 1992 14:39:30   CRAIG
 * Added WinKillDOSGraphicChars().
 *
 *    Rev 1.15   14 Apr 1992 12:53:16   BASIL
 * Added WIN_WAIT_TYPE
 *
 *    Rev 1.14   13 Apr 1992 10:54:22   ED
 * Added WinWriteFileNameAttr
 *
 *    Rev 1.13   16 Mar 1992 14:48:26   ED
 * Added WinDrawHotkeyStringAttr, WinLocalToGlobalRect, and
 * WinGlobalToLocalRect
 *
 *    Rev 1.12   28 Feb 1992 12:50:54   MARKK
 * Added WinBlankWindow
 *
 *    Rev 1.11   19 Feb 1992 14:57:52   DOUG
 * i dont remember checking this out
 *
 *    Rev 1.10   23 Jan 1992 09:19:58   MARKK
 * Added new functions
 *
 *    Rev 1.9   22 Jan 1992 21:42:08   MARKK
 * Change attr function
 *
 *    Rev 1.8   22 Jan 1992 11:46:56   TORSTEN
 * Removed superfluous semicolons.
 *
 *    Rev 1.7   22 Jan 1992 10:17:24   MARKK
 * Added new attribute function
 *
 *    Rev 1.6   14 Jan 1992 14:30:48   MARKK
 * Changed parms, added macro and globals
 *
 *    Rev 1.5   06 Dec 1991 14:33:50   ED
 * Added WinWriteLine and WinWriteLineAttr
 *
 *    Rev 1.4   02 Dec 1991 12:59:46   ED
 * Added the GETROW, GETCOL, and MAKEROWCOL macros to make working
 * with the RowCol functions easier.
 *
 *
 *
 *    Rev 1.3   23 Nov 1991 00:42:34   DOUG
 * added text edit window type
 *
 *    Rev 1.2   11 Nov 1991 16:03:22   DOUG
 * added WinClearWin()
 *
 *    Rev 1.1   04 Sep 1991 13:46:04   MARKK
 *
 *    Rev 1.0   13 Aug 1991 23:35:08   BARRY
 * Initial revision.
 ************************************************************************/

#ifndef INCL_WINMGR
#define INCL_WINMGR

#ifndef INCL_WINGLUE
#include <winglue.h>
#endif

#ifndef INCL_TYPES
#include <Types.h>
#endif

#ifndef INCL_PULLDOWN
#include <Pulldown.h>
#endif

#define	BYTERECT	RECT

#define GETROW(wPos)                    ((Byte)(((Word)(wPos) >> 8) & 0xFF))
#define GETCOL(wPos)                    ((Byte)(wPos))
#define MAKEROWCOL(row, col)            ((Word)(((row) << 8) | (col)))

typedef void  (*MWRCALLBACK)(struct MWindowRec far *window, WORD command);
typedef WORD  (*MWRINQUIRY)(struct MWindowRec far *window, int row, int col, BYTE global);

typedef struct MWindowRec               // This is the movable window record
    {                                   // available for public consumption
    int     row;                        // The top row of the window (may be
                                        // negative)
    int     col;                        // The left col of the window (may be
                                        // negative)
    BYTE    width;                      // The width of the window
    BYTE    height;                     // The height of the window
    BYTE    maxWidth;                   // The maximum width this window can
                                        // be
    BYTE    maxHeight;                  // The maximum height this window can
                                        // be
    BYTE    minWidth;                   // The minimum width this window can
                                        // be
    BYTE    minHeight;                  // The minimum height this window can
                                        // be
    DWORD   flags;                      // Various flags controlling the
                                        // window
    BYTE   *title;                      // The title of the window
    MWRCALLBACK callBack;               // Call back function for actions
    BYTE    border;                     // The border control word
    BYTE    activeAttribute;            // The border attribute when active
    BYTE    inactiveAttribute;          // The border attribute when not
                                        // active
    BYTE    textAttribute;              // The attribute for text
    DWORD   userWord;                   // User definable word
    BYTE    type;                       // The type of this window
    BYTE    priority;                   // The priority of this window
    MWRINQUIRY inquiry;                 // Routine to identify where in a
                                        // window we clicked
    } MWindowRec, far *MWindowRecPtr;

/****************************************************************************/
/*                                                                          */
/* Define border bit values                                                 */
/*                                                                          */
/****************************************************************************/

#define BORDER_NONE     0x0000          // No border window
#define BORDER_SINGLE   0x0001          // Single line border
#define BORDER_DOUBLE   0x0002          // Double line border
#define BORDER_GRAPHICAL 0x0004         // Graphical border
#define BORDER_TITLE    0x0008          // Title bar border

/****************************************************************************/
/*                                                                          */
/* Define flag values                                                       */
/*                                                                          */
/****************************************************************************/

#define WF_MOVABLE      0x0001          // Window is movable
#define WF_SIZABLE      0x0002          // Window is sizable
#define WF_INVISIBLE    0x0004          // Window is invisible
#define WF_NOSHADOW     0x0008          // Window has no shadow
#define WF_NOINIT       0x0010          // Window should not be initialized

/****************************************************************************/
/*                                                                          */
/* Define call back command values                                          */
/*                                                                          */
/****************************************************************************/

enum __WindowCommands
    {
    WIN_MOVED = 0,                      // The window has moved
    WIN_SELECTED,                       // The window was selected
    WIN_DESELECTED,                     // The window was delselected
    WIN_RESIZED,                        // The window was resized
    };


/****************************************************************************/
/*                                                                          */
/* Define the window types                                                  */
/*                                                                          */
/****************************************************************************/

enum __WindowTypes
    {
    WIN_USER_TYPE = 0,                  // A user defined window
    WIN_DIALOG_TYPE,                    // A dialog
    WIN_PULLDOWN_TYPE,                  // A pulldown window
    WIN_MENUBAR_TYPE,                   // The menu bar
    WIN_HELPLINE_TYPE,                  // The help line
    WIN_PANEL_TYPE,                     // NFM Panel type
    WIN_EDIT_TYPE,                      // Text Edit WIndow
    WIN_NMENU_TYPE,                     // NMenu window
    WIN_WAIT_TYPE,                      // Wait... window
    };

/************************************************************************
 *                                                                      *
 * Define inquiry function responses                                    *
 *                                                                      *
 ************************************************************************/

enum __WindowInquiries
    {
    WIN_INQ_IGNORE = 0,                 // This mouse click should be ignored
    WIN_INQ_MOVE,                       // The window should now be moved
    WIN_INQ_RESIZE,                     // The window should now be resized
    WIN_INQ_CLOSE,                      // The close box was clicked
    WIN_INQ_TITLEBAR,                   // The window title bar was clicked
    WIN_INQ_BORDER,                     // The window border was clicked
    WIN_INQ_MAXMIN,                     // Window max/minimize icon was clicked
    WIN_INQ_BUTTONBAR,                  // Norton Panel Manager Button Bar
    };

/************************************************************************
 *                                                                      *
 * Define return codes for MWindowInit                                  *
 *                                                                      *
 ************************************************************************/

enum __MWindowInitReturnCodes
    {
    MWI_NO_ERROR = 0,                   // No error, init. successful
    MWI_NO_MEMORY,                      // Insufficent memory for init
    MWI_BAD_BUFFER,                     // Buffer passed not paragraph
                                        // aligned.
    };

/*----------------------------------------------------------------------*
 * This structure describes how to put patterns together to build       *
 * boxes.                                                               *
 *----------------------------------------------------------------------*/

typedef struct PATTERNREC
    {
    BYTE c;
    BYTE count;
    } PATTERNREC;

#ifndef INCL_WININT
extern MWindowRecPtr CDECL currentWindow;
extern MWindowRecPtr CDECL backGroundPlane;
#endif

#define ActiveWindow() ((MWindowRecPtr) currentWindow)

/* Function definitions in wininit.c */

UINT PASCAL WinInitManager(WORD wNWindows,
			   BYTE saveBackground,
			   BYTE topLine);
       void PASCAL    MWindowTerminate(void);

/* Function definitions in wincreat.c */

       MWindowRecPtr PASCAL WinCreateWindow (MWindowRecPtr window,
                                          MWindowRecPtr behind);
       void PASCAL          WinDestroyWindow(MWindowRecPtr window);
       void PASCAL          WinDrawBorder(MWindowRecPtr window, BYTE attr);
       void PASCAL          WinDrawBox   (struct BOXES *box, BYTE attr);

/* Function definitions in winfast.c */

       void PASCAL   WinFastGetLine        (BYTE *dest);
       void PASCAL   _WinFastWriteRawString(BYTE *string);
       WORD PASCAL   _WinFastReadChar      (void);
       WORD PASCAL   _WinFastReadAttr      (void);
       BYTE * PASCAL _WinFastWriteLine     (BYTE *string,
                                            WORD width,
                                            WORD offset);
       void PASCAL   _WinFastWriteChar     (BYTE ch);
       void PASCAL   _WinFastWriteRawChar  (BYTE ch);
       void PASCAL   _WinFastWriteNRawChar (BYTE ch, WORD nChars);
       void PASCAL NEAR  _WinFastSetRowCol (WORD pos);
       WORD PASCAL   _WinFastGetRowCol     (void);
       WORD PASCAL   _WinFastGetRow        (void);
       WORD PASCAL   _WinFastGetCol        (void);
       void PASCAL   _WinFastSetAttr       (BYTE attr);
       void PASCAL   _WinFastSetCursor     (void);
       void PASCAL   _WinFastFlush         (void);
       void PASCAL   WinFastScrollLeft     (WORD top,
                                            WORD left,
                                            WORD height,
                                            WORD width,
                                            WORD distance);
       void PASCAL   WinFastScrollRight    (WORD top,
                                            WORD left,
                                            WORD height,
                                            WORD width,
                                            WORD distance);
       void PASCAL   _WinSetAttr           (BYTE row,
                                            BYTE col,
                                            WORD wide,
                                            WORD height,
                                            BYTE attr);
       void PASCAL   _WinClearWindow       (WORD upperLeft,
                                            WORD lowerRight);
       void PASCAL   _WinVideoScrollUp     (WORD upperLeft,
                                            WORD lowerRight,
                                            WORD nLines);
       void PASCAL   _WinVideoScrollDown   (WORD upperLeft,
                                            WORD lowerRight,
                                            WORD nLines);
       void PASCAL   _WinFastWriteCRLF     (void);

/* Function definitions in winflush.c */

void PASCAL WinFlush         (void);
void PASCAL WinSuspendUpdates(void);
void PASCAL WinResumeUpdates (void);
WORD PASCAL WinClearSuspends (void);
void PASCAL WinResetSuspends (WORD wSuspends);
void PASCAL WinResetClips    (void);

/* Function definitions in winio.c */

void PASCAL       WinCoordWriteNCharsAttr (BYTE byRow,
                                           BYTE byCol,
                                           BYTE c,
                                           WORD nChars,
                                           BYTE attr);
void PASCAL       WinCoordDrawString      (BYTE byRow,
                                           BYTE byCol,
                                           BYTE *string);
void PASCAL       WinCoordDrawStringAttr  (BYTE byRow,
                                           BYTE byCol,
                                           BYTE *string,
                                           BYTE attr);
void PASCAL       WinCoordDrawNString     (BYTE byRow,
                                           BYTE byCol,
                                           BYTE *string,
                                           WORD len);
void PASCAL       WinCoordDrawNStringAttr (BYTE byRow,
                                           BYTE byCol,
                                           BYTE *string,
                                           WORD len,
                                           BYTE attr);
void PASCAL       WinGetRowCol            (BYTE *row,
                                           BYTE *col);
void PASCAL       WinSetRowCol            (BYTE row,
                                           BYTE col);
void PASCAL       WinDrawString           (BYTE *string);
void PASCAL       WinDrawStringAttr       (BYTE *string,
                                           BYTE attr);
void PASCAL       WinDrawNString          (BYTE *string,
                                           WORD len);
void PASCAL       WinDrawNStringAttr      (BYTE *string,
                                           WORD len,
                                           BYTE attr);
void PASCAL       WinWriteNChars          (BYTE c,
                                           WORD nChars);
void PASCAL       WinWriteNCharsAttr      (BYTE c,
                                           WORD nChars,
                                           BYTE attr);
void PASCAL       WinWriteCRLF            (void);
BYTE FAR * PASCAL WinReturnFrameBufferAddress(MWindowRecPtr window);
void PASCAL WinUnlockFrameBufferAddress(MWindowRecPtr window);
void PASCAL       WinPutBuffer            (BYTE *buffer,
                                           WORD nChars);
void PASCAL       WinCoordDrawString      (BYTE byRow,
                                           BYTE byCol,
                                           BYTE *string);
void PASCAL       WinCoordDrawNString     (BYTE byRow,
                                           BYTE byCol,
                                           BYTE *string,
                                           WORD wLength);
void PASCAL       WinCoordDrawNStringAttr (BYTE byRow,
                                           BYTE byCol,
                                           BYTE *string,
                                           WORD len,
                                           BYTE attr);
void PASCAL       WinCoordWriteNCharsAttr (BYTE byRow,
                                           BYTE byCol,
                                           BYTE c,
                                           WORD nChars,
                                           BYTE attr);
void PASCAL       WinChangeNAttr          (BYTE byRow,
                                           BYTE byCol,
                                           BYTE byAttrAnd,
                                           BYTE byAttrOr,
                                           BYTE byLen);
void PASCAL       WinDrawHotkeyStringAttr (BYTE *string,
                                           BYTE attr,
                                           BYTE hotkeyAttr);

/* Function definitions in winmove.c */

       void PASCAL WinMove          (MWindowRecPtr window,
                                     int newRow,
                                     int newCol);
       void PASCAL WinMoveRel       (MWindowRecPtr window,
                                     int deltaX,
                                     int deltaY);
       void PASCAL WinMoveToFront   (MWindowRecPtr window);
       void PASCAL WinSendToBack    (MWindowRecPtr window);
       void PASCAL WinMoveTrackMouse(MWindowRecPtr window,
                                     void (*callBack)(MWindowRecPtr window,
                                                      WORD row,
                                                      WORD col,
                                                      DWORD userData),
                                     DWORD userData);
       void PASCAL WinSetLink       (MWindowRecPtr window,
                                     MWindowRecPtr link);


/* Function definitions in winscrll.c */

       void PASCAL WinScrollUp  (WORD row1,
                                 WORD col1,
                                 WORD row2,
                                 WORD col2,
                                 WORD n);

       void PASCAL WinScrollDown(WORD row1,
                                 WORD col1,
                                 WORD row2,
                                 WORD col2,
                                 WORD n);


/* Function definitions in winsize.c */

       void PASCAL          WinSetWindowSize          (MWindowRecPtr window,
                                                       BYTE width,
                                                       BYTE height);
       MWindowRecPtr PASCAL WinChangeWindowSize       (MWindowRecPtr window,
                                                       BYTE width,
                                                       BYTE height,
                                                       BYTE maxWidth,
                                                       BYTE maxHeight);
       void PASCAL          WinChangeSizeTrack        (MWindowRecPtr window,
                                                       BYTE maxWidth,
                                                       BYTE maxHeight,
                                                       BYTE bMouse,
                                                       void (*callBack)(MWindowRecPtr window,
                                                                        WORD row,
                                                                        WORD col,
                                                                        DWORD userData),
                                                       DWORD userData);
       void PASCAL          WinChangeSizeTrackMouse   (MWindowRecPtr window,
                                                       BYTE maxWidth,
                                                       BYTE maxHeight,
                                                       void (*callBack)(MWindowRecPtr window,
                                                                        WORD row,
                                                                        WORD col,
                                                                        DWORD userData),
                                                       DWORD userData);
       void PASCAL          WinChangeSizeTrackKeyboard(MWindowRecPtr window,
                                                       BYTE maxWidth,
                                                       BYTE maxHeight,
                                                       void (*callBack)(MWindowRecPtr window,
                                                                        WORD row,
                                                                        WORD col,
                                                                        DWORD userData),
                                                       DWORD userData);

/* Function definitions in winutil.c */

       MWindowRecPtr PASCAL WinSetWindow         (MWindowRecPtr window);
       void PASCAL          WinSetRect           (MWindowRecPtr window,
                                                  int top,
                                                  int left,
                                                  int bottom,
                                                  int right);
       MWindowRecPtr PASCAL WinFrontWindow       (void);
       MWindowRecPtr PASCAL WinBottomWindow      (void);
       MWindowRecPtr PASCAL WinNextWindow        (MWindowRecPtr window);
       MWindowRecPtr PASCAL WinPrevWindow        (MWindowRecPtr window);
       MWindowRecPtr PASCAL WinFindWindow        (WORD row, WORD col);
       void PASCAL          WinSelect            (MWindowRecPtr window);
       void PASCAL          WinLocalToGlobal     (MWindowRecPtr window,
                                                  int *gRow,
                                                  int *gCol);
       void PASCAL          WinGlobalToLocal     (MWindowRecPtr window,
                                                  int gRow,
                                                  int gCol,
                                                  BYTE *lRow,
                                                  BYTE *lCol);
       void PASCAL          WinLocalToGlobalRect (MWindowRecPtr window,
                                                  BYTERECT *rcRect);
       void PASCAL          WinGlobalToLocalRect (MWindowRecPtr window,
                                                  BYTERECT *rcRect);
       BYTE PASCAL          WinGlobalRowToLocal(MWindowRecPtr window, BYTE byRow);
       BYTE PASCAL          WinGlobalColToLocal(MWindowRecPtr window, BYTE byCol);

       void PASCAL          WinInvalidateWindow  (MWindowRecPtr window);
       WORD                 WinStdInquiry        (struct MWindowRec far *window,
                                                  int iRow,
                                                  int iCol,
                                                  BYTE global);
       void PASCAL          WinSetClipRect       (MWindowRecPtr window,
                                                  BYTE t,
                                                  BYTE l,
                                                  BYTE b,
                                                  BYTE r);
       void PASCAL          WinReSetClipRect     (MWindowRecPtr window);
       void PASCAL          WinInvalidateScreen  (void);
       WORD PASCAL          WinGetNumberOfWindows(BYTE byWndType);
       void PASCAL          WinRectToLocal       (MWindowRecPtr lpWin, BYTERECT *lpRect);
       void PASCAL          WinRectToGlobal      (MWindowRecPtr lpWin, BYTERECT *lpRect);
void PASCAL          WinClearWin          (BYTE top,
                                           BYTE left,
                                           BYTE bottom,
                                           BYTE right);
void PASCAL          WinClearWindowAttr   (BYTE top,
                                           BYTE left,
                                           BYTE bottom,
                                           BYTE right,
                                           BYTE attr);
void PASCAL          WinGetDOSScreen      (void);
void PASCAL          WinBlankScreen       (void);
void PASCAL WinBlankWindow(MWindowRecPtr window);

/* Function definitions in winline.c */

BYTE *PASCAL WinWriteLine     (BYTE *string, WORD offset, WORD width);
BYTE *PASCAL WinWriteLineAttr (BYTE *string, WORD offset, WORD width, BYTE attr);

void PASCAL WinWriteFileNameAttr (BYTE *name1, WORD width, BYTE attr);

/* Function definitions in winkill.c */

void PASCAL WinKillDOSGraphicChars( MWindowRecPtr pwrWindow );
void PASCAL KillDOSGraphicChars(BYTE far *pbyFrameBuffer, UINT wBufSize);
BOOL PASCAL WinRestoreBackgroundPlane(void);
void PASCAL WinClearBackgroundPlane(void);

#endif
