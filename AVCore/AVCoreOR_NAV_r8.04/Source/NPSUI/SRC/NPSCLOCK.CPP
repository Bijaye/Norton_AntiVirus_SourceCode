/* Copyright 1991-1993 Symantec, Peter Norton Computing Group           */
/************************************************************************/
/*                                                                      */
/* $Header:   S:/NPSUI/VCS/NPSCLOCK.CPv   1.2   17 Apr 1998 12:17:04   mdunn  $ */
/*                                                                      */
/* Description:                                                         */
/*      This file contains the routine for painting the clock in the    */
/*      icon.                                                           */
/*                                                                      */
/* Contains:                                                            */
/*                                                                      */
/************************************************************************/
/* $Log:   S:/NPSUI/VCS/NPSCLOCK.CPv  $ */
// 
//    Rev 1.2   17 Apr 1998 12:17:04   mdunn
// #undef'ed TRACE since it now collides with an MFC function.
// 
//    Rev 1.1   09 Apr 1998 14:10:22   JALLEE
// Check NULL ptr return from localtime(). Occurs when date/time > 1/18/2038.
// 
//    Rev 1.0   19 May 1997 00:25:04   KSACKIN
// Initial revision.
// 
//    Rev 1.12   22 Feb 1996 13:26:00   YVANG
// Renamed 'nstdhelp.h' to 'stdhelp.h' to use Quake's StdHelp merged with the
// NAV version.
// 
//    Rev 1.11   11 Jan 1996 09:49:02   JWORDEN
// Port changes from Quake 8
// 
//    Rev 1.10.1.1   30 Nov 1995 21:03:24   BARRY
// Renamed stdhelp.h to nstdhelp.h to fix trunk and local infodesk problem
// 
//    Rev 1.10   03 May 1995 13:19:40   MFALLEN
// Moved stdhelp.h higher up in the include order.
// 
//    Rev 1.9   26 Mar 1994 21:00:18   BRAD
// Cleaned up Windows messages
// 
//    Rev 1.8   15 Mar 1994 11:58:24   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
// 
//    Rev 1.7   03 Mar 1994 12:10:14   BRAD
// AWK script applied
// 
//    Rev 1.6   29 Sep 1993 14:53:56   MARKL
// No change.
// 
//    Rev 1.5   23 Jul 1993 10:27:52   MARKL
// Help menu now looks like nirvana's.  Removed help/exit buttons.  All
// child dialogs are centered.
// 
//    Rev 1.4   22 Jul 1993 12:01:58   MARKL
// Hooked up quake help system.
// 
//    Rev 1.3   11 May 1993 20:08:08   MARKL
// Added a new icon such that scheduler no longer displays a working
// clock when iconized.  The application simply shows the icon and
// the current time.
// 
//    Rev 1.2   21 Apr 1993 22:04:54   MARKL
// Nirvana gives the schedule a chance to take an a radically new look/feel
// in the Add/Edit dialog!!!
// 
//    Rev 1.1   18 Mar 1993 20:32:16   MARKL
// No change.
// 
//    Rev 1.0   15 Mar 1993 17:51:04   MARKL
// Initial revision.
// 
//    Rev 1.2   26 Jun 1992 17:53:24   MEIJEN
// Get rid of strids.h because NWRES will eventually disappear
// 
//    Rev 1.1   02 Oct 1991 12:06:36   MARKL
// Made WinStric.H pass.
// 
//    Rev 1.0   28 Jul 1991 11:37:32   MARKL
// Initial revision.
// 
//    Rev 1.1   06 Jun 1991  1:04:40   MARKL
// Clock is rounded out instead of square.  Border color now set to fill
// color.
// 
//    Rev 1.0   04 Jun 1991 20:34:42   MARKL
// Initial revision.
/************************************************************************/



#ifdef CLOCK_WHEN_ICONIZED



#include    "platform.h"
#include    "stdhelp.h"
#define __STDC__
#include    <sys\timeb.h>
#include    <time.h>
#include    <math.h>
#include    "undoc.h"                  /* for extra typedefs           */
#include    "msgdisp.h"                    /* error message stuff          */
#include    "ngui.h"
#include    "nps.h"                     /* scheduler includes           */
#include    "npsres.h"
#include    "npsstb.h"

#undef TRACE
#define TRACE   ERR_TRACE_ID (_NPSCLOCK_C)



/************************************************************************/
/* NPSCLOCK routines                                                    */
/************************************************************************/




/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This routine paints a clock in the client area of the window.   */
/*                                                                      */
/* Return values:                                                       */
/*      NOERR                           - Everything went fine.         */
/*                                                                      */
/************************************************************************/
/* 06/04/91 MSL Function created.                                       */
/* 06/05/91 MSL Instead of a rectangle, we are bordered by an ellipse.  */
/* 04/17/93 MSL Background is now button face instead of white.         */
/************************************************************************/
WORD PASCAL NpsPaintClock (             /* Paints clock in client area  */
    HWND        hDlg)                   /* Handle to main dialog window */
{
#define NUMHANDPOSITIONS     60
#define COSINESCALE        8000
#define TWOPI              6.283185307
#define HOUR_LENGTH        0.60
#define MINUTE_LENGTH      0.90

    int         Xext = 0, Yext = 0;
    int         xSize = 0, ySize = 0;   
    POINT       dotPositions[NUMHANDPOSITIONS]; 
    int         xCenter, yCenter;
    HPEN        hOldPen, hPen, hBorderPen, hDotPen;
    HBRUSH      hBrush;
    HBRUSH      hBkBrush;
    HBRUSH      hDotBrush;
    HBRUSH      hOldBrush;
    RECT        dotRect;
    RECT        rClient;
    int         i;
    int         hours, mins;
    int         hourAngle;
    POINT       hoursPT, hoursPTm, hoursPTp;
    POINT       minsPT, minsPTm, minsPTp;
    POINT       handPoly[5];
    COLORREF    PenColor;
    COLORREF    BkColor;
    COLORREF    BorderColor;
    COLORREF    DotColor;
    HDC         hDC;
    PAINTSTRUCT ps;
    struct      tm      *curtm;
    struct      _timeb  curtimeb;

					/* Get a hDC and our client     */
					/* size.                        */
    hDC = BeginPaint (hDlg, &ps);
    GetClientRect (hDlg, &rClient);

					/* What time is it?             */
    _ftime (&curtimeb);
    curtm = localtime (&curtimeb.time);
    if ( NULL != curtm )
    {
        hours = curtm->tm_hour;
        mins  = curtm->tm_min;

					    /* Determine drawing colors     */
        PenColor    = GetSysColor (COLOR_WINDOWTEXT);
        BkColor     = GetSysColor (COLOR_BTNFACE);
        DotColor    = GetSysColor (COLOR_BTNSHADOW);
        BorderColor = GetSysColor (COLOR_WINDOWTEXT);

					    /* Set up dot positions.  Don't */
					    /* EVEN try to figure our this  */
					    /* formula.... Played till it   */
					    /* worked.                      */
        xSize = (rClient.right - 3)  / 2;
        ySize = (rClient.bottom - 3) / 2;
        for (i = 0; i < NUMHANDPOSITIONS; i++)
          {
          dotPositions[i].x =  (int)(xSize * sin(i * TWOPI / NUMHANDPOSITIONS));
          dotPositions[i].y = -(int)(ySize * cos(i * TWOPI / NUMHANDPOSITIONS));
          }
  
					    /* Add 2 for the big dots       */
  
        xCenter = rClient.left + xSize + 2;
        yCenter = rClient.top  + ySize + 2;
  
					    /* Calculate the end points of  */
					    /* the hands.                   */
        if (hours >= 12) {
          hours = hours - 12;
        }
  
        hourAngle = hours * 5 + mins / 12;
        hoursPT  = dotPositions[hourAngle];
        hoursPTm = dotPositions[(hourAngle == 0) ? 59 : hourAngle-1];
        hoursPTp = dotPositions[(hourAngle == 59) ? 0 : hourAngle+1];
  
        minsPT   = dotPositions[mins];
        minsPTm  = dotPositions[(mins == 0) ? 59 : mins-1];
        minsPTp  = dotPositions[(mins == 59) ? 0 : mins+1];
  
  
        /* Scale the hours and minutes hands */
        hoursPT.x  = (int)(hoursPT.x * HOUR_LENGTH);
        hoursPT.y  = (int)(hoursPT.y * HOUR_LENGTH);
        hoursPTm.x = (int)(hoursPTm.x * HOUR_LENGTH/3);
        hoursPTm.y = (int)(hoursPTm.y * HOUR_LENGTH/3);
        hoursPTp.x = (int)(hoursPTp.x * HOUR_LENGTH/3);
        hoursPTp.y = (int)(hoursPTp.y * HOUR_LENGTH/3);
  
        minsPT.x   = (int)(minsPT.x  * MINUTE_LENGTH);
        minsPT.y   = (int)(minsPT.y  * MINUTE_LENGTH);
        minsPTm.x  = (int)(minsPTm.x  * MINUTE_LENGTH/3);
        minsPTm.y  = (int)(minsPTm.y  * MINUTE_LENGTH/3);
        minsPTp.x  = (int)(minsPTp.x  * MINUTE_LENGTH/3);
        minsPTp.y  = (int)(minsPTp.y  * MINUTE_LENGTH/3);
  
					    /* Create our pens & brushes.   */
        hPen       = CreatePen (PS_SOLID, 1, PenColor);
        hBorderPen = CreatePen (PS_SOLID, 1, BorderColor);
        hDotPen    = CreatePen (PS_SOLID, 1, DotColor);
        hBrush     = CreateSolidBrush (PenColor);
        hBkBrush   = CreateSolidBrush (BkColor);
        hDotBrush  = CreateSolidBrush (DotColor);

					    /* Draw our background.         */
        hOldBrush = SelectObject(hDC, hBkBrush);
        hOldPen   = SelectObject(hDC, hBorderPen);
        Ellipse (hDC, 
	         rClient.left + 1, 
	         rClient.top + 1, 
	         rClient.right, 
	         rClient.bottom);

					    /* Change pens and brushes for  */
					    /* the Dots around the clock.   */
        SelectObject (hDC, hBrush);
        SelectObject (hDC, hDotPen);
  
        /* Draw the dots */
        for (i = 0; i < NUMHANDPOSITIONS; i++)
          {
          if (i % 5 == 0)
	    { /* Big Dot */
	    dotRect.left   = xCenter + dotPositions[i].x - 1;
	    dotRect.right  = xCenter + dotPositions[i].x + 2;
	    dotRect.top    = yCenter + dotPositions[i].y - 1;
	    dotRect.bottom = yCenter + dotPositions[i].y + 2;
  
	    FillRect(hDC, (LPRECT)&dotRect, hDotBrush);
	    }
          else
	    { /* Little Dot */
	    

    MoveToEx(hDC,xCenter + dotPositions[i].x,yCenter + dotPositions[i].y,NULL);
	    LineTo(hDC,
	           xCenter + dotPositions[i].x,
	           yCenter + dotPositions[i].y+1);
	    }
          }
  
					    /* Change pens and brushes for  */
					    /* the hands of the clock.      */
        SelectObject (hDC, hBrush);
        SelectObject (hDC, hPen);
  
       /*
        *   Draw the hands. Changed from lines to polygons to allow
        *   distinction between hour and second hands.
        */
  
        dotRect.left   = xCenter - 1;
        dotRect.right  = xCenter + 2;
        dotRect.top    = yCenter - 1;
        dotRect.bottom = yCenter + 2;
        FillRect(hDC, (LPRECT)&dotRect, hBrush);
  
        handPoly[0].x = xCenter;
        handPoly[0].y = yCenter;
        handPoly[1].x = xCenter + hoursPTm.x;
        handPoly[1].y = yCenter + hoursPTm.y;
        handPoly[2].x = xCenter + hoursPT.x;
        handPoly[2].y = yCenter + hoursPT.y;
        handPoly[3].x = xCenter + hoursPTp.x;
        handPoly[3].y = yCenter + hoursPTp.y;
        handPoly[4].x = xCenter;
        handPoly[4].y = yCenter;
        Polygon (hDC, handPoly, 5);
  
        handPoly[0].x = xCenter;
        handPoly[0].y = yCenter;
        handPoly[1].x = xCenter + minsPTm.x;
        handPoly[1].y = yCenter + minsPTm.y;
        handPoly[2].x = xCenter + minsPT.x;
        handPoly[2].y = yCenter + minsPT.y;
        handPoly[3].x = xCenter + minsPTp.x;
        handPoly[3].y = yCenter + minsPTp.y;
        handPoly[4].x = xCenter;
        handPoly[4].y = yCenter;
        Polygon (hDC, handPoly, 5);
  
  
					    /* Return to caller             */
        SelectObject(hDC, hOldPen);
        SelectObject(hDC, hOldBrush);
        DeleteObject(hPen);
        DeleteObject(hBorderPen);
        DeleteObject(hBrush);
        DeleteObject(hBkBrush);
        DeleteObject(hDotBrush);
        EndPaint (hDlg, &ps);
    }

    return (NOERR);
} /* NpsPaintClock() */



#endif // CLOCK_WHEN_ICONIZED
  


