// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1994 - 2003, 2005 Symantec Corporation. All rights reserved.
//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/callbk15.h_v   1.10   17 Jun 1997 16:42:10   MKEATIN  $
//
// Description:
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/INCLUDE/VCS/callbk15.h_v  $
// 
//    Rev 1.10   17 Jun 1997 16:42:10   MKEATIN
// Define CALLBACKREV1 only if NAVEX15 is defined.
// 
//    Rev 1.9   06 Jun 1997 14:23:18   CNACHEN
// Added RelinquishControl callback
// 
//    Rev 1.8   02 Jun 1997 10:06:06   RStanev
// Changed bUserInterventionRequired to bUserInterventionAllowed in
// MESSAGE_BOX_COOKIE.
//
//    Rev 1.7   02 Jun 1997 09:58:32   RStanev
// Added MESSAGE_BOX_COOKIE.
//
//    Rev 1.6   31 May 1997 15:55:04   MKEATIN
// Added BUTTON_INVALID_MASK
//
//    Rev 1.5   30 May 1997 19:39:30   MKEATIN
// Fixed a build issue - apps should start defined NAVEX15 in their makefiles.
// This fix is a kluge for those that don't.
//
//    Rev 1.4   30 May 1997 18:58:14   MKEATIN
// Added the lpvCookie parameter to several CALLBACKREV2 functions.
//
//
//    Rev 1.1   30 May 1997 12:09:14   CNACHEN
// Added new callback for getting config info, updated ui callbacks
// to take a cookie
//
//    Rev 1.0   02 May 1997 13:29:46   CNACHEN
// Initial revision.
//
//************************************************************************

#ifndef CALLBACK2_INCLUDE

#define CALLBACK2_INCLUDE

#define BUTTON_FLAG_OK                  0x00000001UL
#define BUTTON_FLAG_CANCEL              0x00000002UL
#define BUTTON_FLAG_CONTINUE            0x00000004UL
#define BUTTON_FLAG_YES                 0x00000008UL
#define BUTTON_FLAG_NO                  0x00000010UL
#define BUTTON_FLAG_PREVIOUS            0x00000020UL
#define BUTTON_INVALID_MASK             0xFFFFFF00UL

#define NAVEX_UI_OK                     0
#define NAVEX_UI_ERROR                  1
#define NAVEX_UI_TIMEOUT                2

typedef LPVOID FAR *LPLPVOID;

typedef struct
{
    HFILE   (WINAPI FAR *FileOpen)(LPTSTR lpName, UINT uOpenMode);
    HFILE   (WINAPI FAR *FileClose)(HFILE hHandle);
    UINT    (WINAPI FAR *FileRead)(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
    UINT    (WINAPI FAR *FileWrite)(HFILE hHandle, LPVOID lpBuffer, UINT uBytes);
    DWORD   (WINAPI FAR *FileSeek)(HFILE hHandle, LONG dwOffset, int nFlag);
    DWORD   (WINAPI FAR *FileSize)(HFILE hHandle);
    BOOL    (WINAPI FAR *FileGetDateTime)(HFILE hHandle, UINT uType, WORD FAR *lpuDate,
                                          WORD FAR *lpuTime);
    BOOL    (WINAPI FAR *FileSetDateTime)(HFILE hHandle, UINT uType, WORD uDate,
                                          WORD uTime);
    BOOL    (WINAPI FAR *FileDelete)(LPTSTR lpFilename);
    UINT    (WINAPI FAR *FileGetAttr)(LPTSTR lpFilename, UINT FAR *lpuAttr);
    UINT    (WINAPI FAR *FileSetAttr)(LPTSTR lpFilename, UINT uAttr);
    HFILE   (WINAPI FAR *FileCreate)(LPTSTR lpFilename, UINT uAttr);

    // new additions!

    BOOL    (WINAPI FAR *FileLock)(HFILE hHandle, DWORD dwStart, DWORD dwLength);
    BOOL    (WINAPI FAR *FileUnlock)(HFILE hHandle, DWORD dwStart, DWORD dwLength);
    BOOL    (WINAPI FAR *Progress)(WORD wProgress, DWORD dwCookie);

    // low-level disk i/o

    UINT    (WINAPI FAR *SDiskReadPhysical)(LPVOID lpBuffer, BYTE bCount,
                                            WORD wCylinder, BYTE  bSector,
                                            BYTE bHead,  BYTE bDrive);

    UINT    (WINAPI FAR *SDiskWritePhysical)(LPVOID lpBuffer, BYTE bCount,
                                             WORD wCylinder, BYTE  bSector,
                                             BYTE bHead,  BYTE bDrive);


    ////////////////////////////////////////////////
    // Memory allocation callback functions (common)
    ////////////////////////////////////////////////

    LPVOID  (WINAPI FAR *TempMemoryAlloc)(DWORD uSize);         // not used
    VOID    (WINAPI FAR *TempMemoryFree)(LPVOID lpBuffer);      // not used

    LPVOID  (WINAPI FAR *PermMemoryAlloc)(DWORD uSize);
    VOID    (WINAPI FAR *PermMemoryFree)(LPVOID lpBuffer);

    /////////////////////////////////////////////////
    // New callbacks (NAVEX 1.5)
    ////////////////////////////////////////////////

    /////////////////////////////////////////////////
    // VOID Callbacks
    ////////////////////////////////////////////////

    UINT (WINAPI FAR *CreateMutex)
    (
        LPLPVOID                            lplpvMutex
    );

    UINT (WINAPI FAR *DestroyMutex)
    (
        LPVOID                             lpvMutex
    );

    UINT (WINAPI FAR *WaitMutex)
    (
        LPVOID                              lpvMutex,
        DWORD                               dwMSTicks
    );

    UINT (WINAPI FAR *ReleaseMutex)
    (
        LPVOID                             lpvMutex
    );

    /////////////////////////////////////////////////
    // Temporary file callback
    ////////////////////////////////////////////////

    HFILE (WINAPI FAR *GetTempFile)
    (
        LPTSTR                              lpszFileName,
        LPVOID                              lpvCookie
    );

    /////////////////////////////////////////////////
    // UI functionality
    ////////////////////////////////////////////////

    UINT (WINAPI FAR *MessageBox)
    (
        DWORD                               dwBoxID,
        LPTSTR                              lpszMessage,
        DWORD                               dwButtonFlags,
        LPDWORD                             lpdwResult,
        DWORD                               dwMSTimeOut,
        LPVOID                              lpvCookie
    );

    UINT (WINAPI FAR *InfoBoxInit)
    (
        LPTSTR                              lpszTitle,
        DWORD                               dwButtonFlags,      // CANCEL only
        LPLPVOID                            lplpvInfoBoxInfo,
        LPVOID                              lpvCookie
    );

    UINT (WINAPI FAR *InfoBoxUpdate)
    (
        LPVOID                              lpvInfoBoxInfo,
        LPTSTR                              lpszText,
        LPDWORD                             lpdwResult,
        LPVOID                              lpvCookie
    );

    UINT (WINAPI FAR *InfoBoxClose)
    (
        LPVOID                              lpvInfoBoxInfo,
        LPVOID                              lpvCookie
    );


    UINT (WINAPI FAR *TextEditBox)
    (
        DWORD                               dwBoxID,
        LPTSTR                              lpszPrompt,
        LPTSTR                              lpszSeedText,
        LPTSTR                              lpszPutHere,
        UINT                                uMaxLength,
        DWORD                               dwMSTimeOut,
        LPVOID                              lpvCookie
    );

    VOID (WINAPI FAR *GetConfigInfo)
    (
        LPTSTR                              lpszID,
        LPVOID                              lpvResult,
        UINT                                uMaxResultLen,
        LPBOOL                              lpbImplemented,
        LPVOID                              lpvCookie
    );

    VOID (WINAPI FAR *RelinquishControl)
    (
        VOID
    );

#if !defined(CALLBACK_INCLUDE) && defined(NAVEX15)

} CALLBACKREV2, CALLBACKREV1;

typedef CALLBACKREV1 FAR *LPCALLBACKREV1;

#else

} CALLBACKREV2;

#endif

typedef CALLBACKREV2 FAR *LPCALLBACKREV2;

typedef struct
    {
    BOOL bUserInterventionAllowed;
    UINT uTimeOut;
    } MESSAGE_BOX_COOKIE, *PMESSAGE_BOX_COOKIE;

#endif
