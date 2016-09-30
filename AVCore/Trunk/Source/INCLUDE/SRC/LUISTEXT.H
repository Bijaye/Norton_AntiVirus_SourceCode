// ----------------------------------------------------------------------------
// LUISTEXT.H - Little UI Server text replacement and DB module
// ----------------------------------------------------------------------------
//
// $Header:   S:/INCLUDE/VCS/LUISTEXT.H_v   1.0   21 Aug 1997 14:25:40   Ray  $
//
// Description:
//      Contains everything needed to do text replacement.
//
// ----------------------------------------------------------------------------
// $Log:   S:/INCLUDE/VCS/LUISTEXT.H_v  $
// 
//    Rev 1.0   21 Aug 1997 14:25:40   Ray
// Initial revision.
// 
//    Rev 1.4   31 Jan 1997 03:14:44   RAY
// Added removeItem()
// 
//    Rev 1.3   20 Nov 1996 20:11:40   RAY
// Added BADREQUEST and NOTIMPLEMENTED
// 
//    Rev 1.1   22 Oct 1996 12:12:14   AOONWAL
// No change.
// 
//    Rev 1.0   26 Sep 1996 19:29:18   RAY
// Initial revision.
// 
// ----------------------------------------------------------------------------
#if !defined(LUISTEXT_H)
#define LUISTEXT_H

#if defined( __cplusplus )
extern "C" {
#endif

#define MAX_ITEMS               ((WORD) -1)
#define DEFAULT_TABLE_SIZE      1021

#define TABLEENTRY_EMPTY        ((LPLUISITEM) 0)
#define TABLEENTRY_DELETED      ((LPLUISITEM) 1)

#define INI_BUFFER_SIZE         32767

typedef WORD LUISSTATUS;

#define LUIS_OK             (0x0000)
#define LUIS_NOTFOUND       (0x0001)
#define LUIS_NOMEMORY       (0x0002)
#define LUIS_NOSOCKET       (0x0003)
#define LUIS_NOTHREAD       (0x0004)
#define LUIS_NOBIND         (0x0005)
#define LUIS_NOLISTEN       (0x0006)
#define LUIS_NOHEADER       (0x0007)
#define LUIS_NOBODY         (0x0008)
#define LUIS_NORECV         (0x0009)
#define LUIS_NOADD          (0x000A)
#define LUIS_NOWRITE        (0x000B)
#define LUIS_BUFFERTOOSMALL (0x000C)
#define LUIS_NOMUTEX        (0x000D)
#define LUIS_NOREAD         (0x000E)
#define LUIS_PARENTITEM     (0x000F)

#define LUIS_ABORTED        (0x0011)
#define LUIS_UNEXPECTEDBODY (0x0012)
#define LUIS_HUGEHEADER     (0x0013)
#define LUIS_HUGEBODY       (0x0014)

#define LUIS_URLNOTFOUND    (0x0021)
#define LUIS_URLTOOLONG     (0x0022)
#define LUIS_BADTEMPLATE    (0x0023)
#define LUIS_BADREQUEST     (0x0024)
#define LUIS_NOTIMPLEMENTED (0x0025)

#define LUIS_TABLEFULL      (0x0031)
#define LUIS_HUGEINI        (0x0032)

#define LUISSERV_POSTERROR  (0x0041)

#define LUISHTTP_LISTTOOLONG    (0x0081)

typedef LUISSTATUS ( WINAPI FAR *LUISQUERYCALLBACK )
(
    DWORD           dwCookie,
    LPSTR           lpstrQuery,
    LPBYTE          lpbyResultBuffer,
    LPDWORD         lpdwResultBufferSize
);


typedef LUISSTATUS ( WINAPI FAR *LUISSETCALLBACK )
(
    DWORD           dwCookie,
    LPSTR           lpstrQuery,
    LPSTR           lpstrValue
);


typedef struct tagLUISITEM
{
    LPSTR                       lpstrName;
    LPSTR                       lpstrValue;
    LUISQUERYCALLBACK           lpfnQueryCallback;
    LUISSETCALLBACK             lpfnSetCallback;
    DWORD                       dwCookie;
    struct tagLUISTABLE FAR *   lprTable;
    struct tagLUISITEM  FAR *   lprNext;
} LUISITEM, FAR * LPLUISITEM;
#define sLUISITEM ( sizeof( LUISITEM ))


typedef struct tagLUISTABLE
{
    HANDLE          hMutex;
    WORD            wCapacity;
    LPLUISITEM      alprItems[1];
} LUISTABLE, FAR * LPLUISTABLE;
#define sLUISTABLE ( sizeof( LUISTABLE ))


LUISSTATUS WINAPI createTable
(
    WORD                wTableSize,
    LPLUISTABLE FAR *   lplprTable
);


LUISSTATUS WINAPI clearTable
(
    LPLUISTABLE     lprTable
);

  
LUISSTATUS WINAPI destroyTable
(
    LPLUISTABLE     lprTable
);


LUISSTATUS WINAPI readTable
(
    LPLUISTABLE     lprTable,
    LPSTR           lpstrININame,
    LPSTR           lpstrINISection
);


LUISSTATUS WINAPI readItem
(
    LPLUISTABLE     lprTable,
    LPSTR           lpstrName,
    LPSTR           lpstrININame,
    LPSTR           lpstrINISection
);


LUISSTATUS WINAPI writeItem
(
    LPLUISITEM      lprItem,
    LPSTR           lpstrININame,
    LPSTR           lpstrINISection
);


LUISSTATUS WINAPI writeTable
(
    LPLUISTABLE     lprTable,
    LPSTR           lpstrININame,
    LPSTR           lpstrINISection
);


LUISSTATUS WINAPI addItem
(
    LPLUISTABLE         lprTable,
    LPSTR               lpstrName,
    LPSTR               lpstrValue,
    LPLUISTABLE         lprNewTable,
    LUISQUERYCALLBACK   lpfnQueryCallback,
    LUISSETCALLBACK     lpfnSetCallback,
    DWORD               dwCookie
);


LUISSTATUS WINAPI addItems
(
    LPLUISTABLE     lprTable,
    LPLUISITEM      lprItems,
    LPWORD          lpwItemsAdded
);


LUISSTATUS WINAPI deleteItem
(
    LPLUISTABLE     lprTable,
    LPSTR           lpstrName
);


LUISSTATUS WINAPI removeItem
(
    LPLUISTABLE     lprTable,
    LPSTR           lpstrName
);


LUISSTATUS WINAPI findItem
(
    LPLUISTABLE     lprTable,
    LPSTR           lpstrName,
    LPWORD          lpwIndex
);


LUISSTATUS WINAPI getItem
(
    LPLUISTABLE         lprTable,
    LPSTR               lpstrName,
    LPLUISITEM FAR *    lplprItem
);


LUISSTATUS WINAPI setItem
(
    LPLUISTABLE     lprTable,
    LPSTR           lpstrName,
    LPSTR           lpstrValue
);


LUISSTATUS WINAPI resolveTemplate
(
    LPLUISTABLE     lprTable,
    LPLUISTABLE     lprCache,
    LPSTR           lpstrTemplate,
    LPBYTE          lpbyResultBuffer,
    LPDWORD         lpdwResultBufferSize
);


LUISSTATUS WINAPI resolveToken
(
    LPLUISTABLE     lprTable,
    LPSTR           lpstrToken,
    LPBYTE          lpbyResultBuffer,
    LPDWORD         lpdwResultBufferSize
);


#if defined ( __cplusplus )
}                                       // extern "C" {
#endif

#endif                                  // LUISTEXT_H
