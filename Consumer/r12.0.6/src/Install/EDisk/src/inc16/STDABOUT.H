/* Copyright 1991 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   I:/INCLUDE/VCS/STDABOUT.H_V   1.3   16 Oct 1992 13:19:56   CRAIG  $ *
 *                                                                      *
 * Description:                                                         *
 *  This files contains defines/prototypes for the Standard Dialog      *
 *  About Box.                                                          *
 *                                                                      *
 * See Also:                                                            *
 *  STDABOUT.C                                                          *
 *                                                                      *
 ************************************************************************
 * $Log:   I:/INCLUDE/VCS/STDABOUT.H_V  $ *
 * 
 *    Rev 1.3   16 Oct 1992 13:19:56   CRAIG
 * Added StdLittleAboutBox() prototype.
 *
 *    Rev 1.2   16 Oct 1991 16:01:32   PETERD
 * Updated.
 *
 *    Rev 1.1   26 Sep 1991 21:59:48   BARRY
 * Added the version record character defines.
 *
 *    Rev 1.0   26 Sep 1991 19:59:00   BARRY
 * Initial revision.
 ************************************************************************/

#ifndef  INCL_STDABOUT
#define  INCL_STDABOUT

#ifndef  INCL_WINGLUE
#include <WinGlue.h>
#endif

/************************************************************************
 * Version Characters                                                   *
 ************************************************************************/

#define STDABOUT_ALPHA_CHAR     'à'
#define STDABOUT_BETA_CHAR      'á'
#define STDABOUT_PRE_CHAR       'p'
#define STDABOUT_NULL_STR       ""
#define STDABOUT_EMPTY_CHAR     ' '

/************************************************************************
 * Buffer Sizes                                                         *
 ************************************************************************/

#define STDABOUT_MAX_TITLE      80
#define STDABOUT_MAX_VERSION    40
#define STDABOUT_MAX_VERNUM     10
#define STDABOUT_MAX_VERCLASS   20

/************************************************************************
 * Dialog Control Defines                                               *
 ************************************************************************/

#define STDABOUT_OK_BUTTON      0

/************************************************************************
 * Module Prototypes                                                    *
 ************************************************************************/

WORD FAR PASCAL StdDlgAboutBox(VOID);
WORD FAR PASCAL StdDlgLittleAboutBox(VOID);

#endif
