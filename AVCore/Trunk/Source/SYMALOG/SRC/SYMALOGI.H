/**********************************************************************
** Copyright 1993 Symantec, Peter Norton Group
**
** File:  symalogi.h    Internal declarations for SYMALOG project
**
** See Also: SYMALOG.H
**
** Future Enhancements:
***********************************************************************
** $Header:   S:\symalog\vcs\symalogi.h_v   1.19   20 Dec 1994 17:11:30   MARKL  $
** 04/20/93 WRL is William R. Lee II (BLEE)
**            - Moved to Quake from NIRVANA:ACTILOG
**            - Renamed from ACTILOG.H to SYMALOG.H
** 06/28/93 WRL Added ALOG_FTB_LOCALTMPL flag.
**********************************************************************/
#ifndef _SYMALOGI_H
#define _SYMALOGI_H
#include "platform.h"                   /* Symantec global stuff     */

#ifndef SYM_NLM
#include "npt.h"                        /* PRINTDLGSTRUCT */
#endif

#include "symalog.h"                    /* Proj specific extern dcls */

#ifdef SYM_WIN
#   include "symarc.h"                  /* Definitions for .RC       */
#else
#   include "stddos.h"                  /* DIALOGREC (for symintl.h) */
#endif

/**********************************************************************
** - Macro definitions
**********************************************************************/

                                        /* Data Fields Types:        */
#define ALOG_FT_ACTION     'A'       /* 'A' Action string         */
#define ALOG_FT_DATE       'a'       /* 'a' Date of log record    */
#define ALOG_FT_DIRECTION  'r'       /* 'r' File movement direction */
                                        /*   ALOG_DIR_IN|ALOG_DIR_OUT*/
#define ALOG_FT_FILESIZE   'B'       /* 'B' Size of file in bytes */
#define ALOG_FT_MESSAGE    'M'       /* 'M' Information text      */
#define ALOG_FT_NETWORK    'N'       /* 'N' Network address       */
#define ALOG_FT_OS         'S'       /* 'S' Operating system      */
#define ALOG_FT_PATH       'P'       /* 'P' Full file path        */
#define ALOG_FT_PROGTYPE   'p'       /* 'p' Program type          */
                                        /*    ALOG_PT_NLM|ALOG_PT_TSR*/
                                        /*    |ALOG_PT_WINDOWS       */
#define ALOG_FT_SERVER     's'       /* 's'                       */
#define ALOG_FT_TIME       't'       /* 't' Time of log entry     */
#define ALOG_FT_USER       'u'       /* 'u'                       */
#define ALOG_FT_VIRUSNAME  'v'       /* 'v'                       */

#define ALOG_FT_RECTYPE     'T'         /* 'T' Record type:  ALOG_FI_* */
#define ALOG_FT_DESC        'e'         /* description/extra         */
//#define ALOG_FT_DRIVE       'd'       /* Drive letter and colon    */
//#define ALOG_FT_FILE        'f'       /* filename                  */
//#define ALOG_FT_INFO        'I'

#define ALOG_FTB_CONTINUED  0x80000000l /* Reserved: more flag DWords*/
#define ALOG_FTB_LOCALTMPL  0x40000000l /* Use local output template */

#define ALOG_FTB_ACTION     0x00000001l
#define ALOG_FTB_DATE       0x00000002l
#define ALOG_FTB_DIRECTION  0x00000004l
#define ALOG_FTB_FILESIZE   0x00000008l
#define ALOG_FTB_MESSAGE    0x00000010l
#define ALOG_FTB_NETWORK    0x00000020l
#define ALOG_FTB_OS         0x00000040l
#define ALOG_FTB_PATH       0x00000080l
#define ALOG_FTB_PROGTYPE   0x00000100l
#define ALOG_FTB_SERVER     0x00000200l
#define ALOG_FTB_TIME       0x00000400l
#define ALOG_FTB_USER       0x00000800l
#define ALOG_FTB_VIRUSNAME  0x00001000l

/**********************************************************************
** - Typedefs, etc.
**********************************************************************/
typedef WORD FIELDLEN;

/**********************************************************************
** - Global variables
**********************************************************************/
EXTERN HINSTANCE hDLLInst;              /* This DLL's instance handle*/

/**********************************************************************
**
**********************************************************************/
#define WinGetInstance(hWnd) ((HINSTANCE)GetWindowWord((hWnd),GWW_HINSTANCE))

#endif // _SYMALOG_H
