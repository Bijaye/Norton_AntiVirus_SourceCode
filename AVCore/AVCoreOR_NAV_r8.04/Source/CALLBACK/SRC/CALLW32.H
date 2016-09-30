// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CALLBACK/VCS/callw32.h_v   1.0   18 Jun 1997 13:16:24   MKEATIN  $
//
// Description:  This files contains the prototypes for the WIN32 
//               NAVEX UI Callbacks.
//
// See Also:  callbk15.h  msgbxw32.cpp infbxw32.cpp edbxw32.cpp callw32.cpp
//            virscan.rc
//************************************************************************
// $Log:   S:/CALLBACK/VCS/callw32.h_v  $
// 
//    Rev 1.0   18 Jun 1997 13:16:24   MKEATIN
// Initial revision.
// 
//    Rev 1.0   17 Jun 1997 15:07:50   MKEATIN
// Initial revision.
// 
//    Rev 1.0   31 May 1997 15:51:00   MKEATIN
// Initial revision.
//************************************************************************

UINT WINAPI NAVEXMessageBox32
(
    DWORD   dwBoxID,
    LPTSTR  lpszMessage,
    DWORD   dwButtonFlags,
    LPDWORD lpdwResultFlags,
    DWORD   dwMSTimeOut,
    LPVOID  lpvCookie
);

UINT WINAPI NAVEXTextEditBox32 
(  
    DWORD  dwBoxID,
    LPTSTR lpszPrompt,
    LPTSTR lpszSeedText,
    LPTSTR lpszPutHere,
    UINT   nMaxLength,
    DWORD  dwMSTimeOut,
    LPVOID lpvCookie
);


UINT WINAPI NAVEXInfoBoxInit32
(
    LPTSTR   lpszTitle,
    DWORD    dwButtonFlags,      // CANCEL only
    LPLPVOID lplpvInfoBoxInfo,
    LPVOID   lpvCookie
);

UINT WINAPI NAVEXInfoBoxUpdate32
(
    LPVOID  lpvInfoBoxInfo,
    LPTSTR  lpszText,
    LPDWORD lpdwResultFlags,
    LPVOID  lpvCookie
);

UINT WINAPI NAVEXInfoBoxClose32
(
    LPVOID lpvInfoBoxInfo,
    LPVOID lpvCookie
);
    
