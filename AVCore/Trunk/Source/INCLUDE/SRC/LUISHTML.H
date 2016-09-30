// ----------------------------------------------------------------------------
// LUISHTTP.H - Little UI Server HTTP Helper module
// ----------------------------------------------------------------------------
//
// $Header:   S:/INCLUDE/VCS/luishtml.H_v   1.0   21 Aug 1997 14:25:42   Ray  $
//
// Description:
//      Contains all HTML Specific Routines
//
// ----------------------------------------------------------------------------
// $Log:   S:/INCLUDE/VCS/luishtml.H_v  $
// 
//    Rev 1.0   21 Aug 1997 14:25:42   Ray
// Initial revision.
// 
//    Rev 1.5   15 May 1997 13:20:18   Ray
// Added Time handlers
// 
//    Rev 1.4   03 Jan 1997 13:58:48   RAY
// Added Select handler
// 
//    Rev 1.3   20 Nov 1996 15:22:12   AOONWAL
// No change.
// 
//    Rev 1.2   22 Oct 1996 12:12:22   AOONWAL
// No change.
// 
//    Rev 1.1   27 Sep 1996 18:44:00   RAY
// 
//    Rev 1.0   27 Sep 1996 13:38:06   RAY
// Initial revision.
// 
// ----------------------------------------------------------------------------

#if !defined ( LUISHTTP_H )
#define LUISHTTP_H

#if defined ( __cplusplus )
extern "C" {
#endif

#define LUISHTTP_MAXLISTSIZE        8192
#define LUISHTTP_LISTITEMPREFIX     "<OPTION>"
#define LUISHTTP_LISTITEMPOSTFIX    "\r\n"
#define sLUISHTTP_LISTITEMPREFIX    (strlen( LUISHTTP_LISTITEMPREFIX ))
#define sLUISHTTP_LISTITEMPOSTFIX   (strlen( LUISHTTP_LISTITEMPOSTFIX ))

LUISSTATUS addCheckboxHandler
(
    LPLUISTABLE lprRootTable
);

LUISSTATUS deleteCheckboxHandler
(
    LPLUISTABLE lprRootTable
);

LUISSTATUS WINAPI checkboxQuery
(
    DWORD       dwCookie,
    LPSTR       lpstrName,
    LPBYTE      lpbyResultBuffer,
    LPDWORD     lpdwResultBufferSize
);

LUISSTATUS WINAPI checkboxSet
(
    DWORD dwCookie,
    LPSTR lpstrName,
    LPSTR lpstrValue
);


LUISSTATUS addListboxHandler
(
    LPLUISTABLE lprRootTable
);

LUISSTATUS deleteListboxHandler
(
    LPLUISTABLE lprRootTable
);

LUISSTATUS WINAPI listboxQuery
(
    DWORD       dwCookie,
    LPSTR       lpstrName,
    LPBYTE      lpbyResultBuffer,
    LPDWORD     lpdwResultBufferSize
);

LUISSTATUS WINAPI listboxSet
(
    DWORD dwCookie,
    LPSTR lpstrName,
    LPSTR lpstrValue
);


LUISSTATUS addRadiobuttonHandler
(
    LPLUISTABLE lprRootTable
);

LUISSTATUS deleteRadiobuttonHandler
(
    LPLUISTABLE lprRootTable
);

LUISSTATUS WINAPI radiobuttonQuery
(
    DWORD       dwCookie,
    LPSTR       lpstrName,
    LPBYTE      lpbyResultBuffer,
    LPDWORD     lpdwResultBufferSize
);

LUISSTATUS WINAPI radiobuttonSet
(
    DWORD dwCookie,
    LPSTR lpstrName,
    LPSTR lpstrValue
);

LUISSTATUS addSelectHandler
(
    LPLUISTABLE lprRootTable
);

LUISSTATUS deleteSelectHandler
(
    LPLUISTABLE lprRootTable
);

LUISSTATUS WINAPI selectQuery
(
    DWORD       dwCookie,
    LPSTR       lpstrName,
    LPBYTE      lpbyResultBuffer,
    LPDWORD     lpdwResultBufferSize
);

LUISSTATUS addTimeHandlers
(
    LPLUISTABLE lprRootTable
);

LUISSTATUS deleteTimeHandlers
(
    LPLUISTABLE lprRootTable
);

LUISSTATUS WINAPI queryTwelveHour
(
    DWORD       dwCookie,
    LPSTR       lpstrName,
    LPBYTE      lpbyResultBuffer,
    LPDWORD     lpdwResultBufferSize
);

LUISSTATUS WINAPI queryAMPM
(
    DWORD       dwCookie,
    LPSTR       lpstrName,
    LPBYTE      lpbyResultBuffer,
    LPDWORD     lpdwResultBufferSize
);

#if defined ( __cplusplus )
}
#endif

#endif                                  // LUISHTTP_H
