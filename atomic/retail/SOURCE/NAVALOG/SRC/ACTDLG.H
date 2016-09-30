/**********************************************************************
** Copyright 1993-1995 Symantec, Peter Norton Group
**
** File: actdlg.h
**
** This contains symbol definitions for the ACTILOG.DLG file.
**
**Kedit:Tabs Incr 3
**Kedit:EditV SetF FExt CPP
**
** See Also:
** Future Enhancements:
***********************************************************************
** $Header:   S:/navalog/VCS/actdlg.h_v   1.0   06 Feb 1997 20:55:06   RFULLER  $
** 05/12/93 WRL is William R. Lee II (BLEE)
**            - Deleted IDI_AL_PLACEHLDR
**            - Renamed IDI_AL_EDIT to IDI_AL_VIEW
***********************************************************************
** $Log:   S:/navalog/VCS/actdlg.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:55:06   RFULLER
// Initial revision
// 
//    Rev 1.0   30 Jan 1996 15:52:24   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 10:14:38   BARRY
// Initial revision.
// 
//    Rev 1.5   14 Jan 1996 16:15:10   DALLEE
// Changed #ifdef NAVWNT to NAVSCANW.
// 
//    Rev 1.4   22 Nov 1995 11:25:50   YVANG
// For NAVWNT: Added #define ID_PROGRESS_BOX.
// 
//    Rev 1.3   06 Jun 1995 21:43:48   BARRY
// Removed animation
// 
//    Rev 1.2   30 Apr 1995 18:31:06   DALLEE
// Temporary reading activity log bitmaps for ET1.
**********************************************************************/
#ifndef _ACTDLG_H
#define _ACTDLG_H

#define ID_ICON_NAV          10


/**********************************************************************
** Activity Log Dialog Box Defines
**********************************************************************/
#define IDD_ACT_LOG         100
#define IDI_AL_TEXT         101
#define IDI_AL_VIEW         102
#define IDI_AL_CLOSE        103
#define IDI_AL_PRINT        104
#define IDI_AL_CLEAR        105
#define IDI_AL_FILTER       106
#define IDI_AL_ENTRIES      108
#define IDI_AL_ETITLE       109  // Static text, sometimes changes text

/**********************************************************************
** Clear Activity Log Dialog Box
**********************************************************************/
#define IDD_CLEARLOG        200

/**********************************************************************
** Activity Log Filter Dialog Box Defines
**********************************************************************/
#define IDD_ACT_FILTER      300         /* Dialog ID                 */
#define IDI_AF_CHECKBASE    100         /* Base for check boxes      */
#define IDI_AF_DATED        150
#define IDI_AF_COMBO        151
#define IDI_AF_DATE1        152
#define IDI_AF_DATE2        153
#define IDI_AF_GROUP        154

#define IDD_PROGRESS        400
#define ID_PROGRESS_BAR     100
#ifdef NAVSCANW
#define ID_PROGRESS_BOX     101
#endif

#endif // _ACTDLG_H
