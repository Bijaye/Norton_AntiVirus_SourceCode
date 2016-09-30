#ifndef _SECURITY
#define _SECURITY

#include "options.h"

#define PASSWORD_ASK             0x0001
#define PASSWORD_SHOWCONTROL     0x0002


typedef struct tagCHECKSTRUCT 
{
   HWND           hWnd;
   LPNAVOPTIONS   lpNavOptions;
   UINT           uID;
   WORD           wVirtKey;
   DWORD          dwKeyFlags;

} CHECKSTRUCT, FAR * LPCHECKSTRUCT;

#ifdef __cplusplus
extern "C" {
#endif

//  ********************************************************************** //
//  *****************      Function Prototypes     *********************** //
//  ********************************************************************** //

UINT SYM_EXPORT WINAPI PasswordCheck (LPCHECKSTRUCT, BOOL, BOOL);
WORD SYM_EXPORT WINAPI AskPassword (LPCHECKSTRUCT lpPwCheck);
VOID SYM_EXPORT WINAPI ResetPassword(VOID);
VOID SYM_EXPORT WINAPI RemoveProtection(VOID);
VOID SYM_EXPORT WINAPI DestroyPasswordWindow(VOID);

//  ********************************************************************** //

#if defined( __cplusplus )
}
#endif  // #if defined( __cplusplus )

#endif
