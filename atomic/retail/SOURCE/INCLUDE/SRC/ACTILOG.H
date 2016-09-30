#ifdef __cplusplus                      /* For C++ force a C call,   */
#  define EXTERNC extern "C"            /* i.e., an unmangled name   */
#else                                   /* If not C++,               */
#  define EXTERNC                       /* then unmangled is the dft */
#endif
#include "symalog.h"                    /* Quake filter routines     */

typedef struct {                        /* Show activity log struct  */
   LPTSTR       szTitle;                /* Dialog Box Title          */
   LPTSTR       szLogFile;              /* Path to log file          */
   BOOL         bAllowClear;            /* Enable clear button?      */
   LPALOGFILTER lpLogFilter;            /* Filter structure          */
   int          nTypes;                 /* Number of types supported */
   LPALOGFORMAT pFormats;               /* Template string IDs       */
   UINT  FAR    *aFilterDesc;           /* Control description array */
   LPNAVOPTIONS lpNav;
   UINT         uHelpID;
   UINT         uHelpIDFilter;
} ACTILOG, FAR *LPACTILOG;

/**********************************************************************
** - External function prototypes
**********************************************************************/

EXTERNC int SYM_EXPORT WINAPI ShowActivityLog(
   HWND hDlg,                           /* Mommy window handle       */
   LPACTILOG lpAct);                    /* provided by user          */
