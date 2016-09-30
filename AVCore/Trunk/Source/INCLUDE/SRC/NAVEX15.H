// Copyright 1995 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/navex15.h_v   1.5   04 Aug 1998 13:25:34   MKEATIN  $
//
// Description:
//   This file defines protypes of external NAVEX functions and related
//   structures.
//
// Notes:
//   The master copy of this file resides on the VIRLIB trunk under the
//   NAVEX project.  It should only be modified from that location.
//   Doing a MKUPDATE from there will copy this file to all the global
//   include areas of all the branches and the trunk.
//
//   A checked in version of this file also exists in QUAKE 2's
//   VIRSCAN project.  However, that file should be an exact copy
//   of the original on the VIRLIB trunk's NAVEX project, except
//   possibly for the version header.  This must be ensured before
//   each build.
//
//***************************************************************************
// $Log:   S:/INCLUDE/VCS/navex15.h_v  $
//
//    Rev 1.5   04 Aug 1998 13:25:34   MKEATIN
// Prototyped EXTInitWin and EXTCloseWin for SYM_WIN instead of SYM_WIN32.
//
//    Rev 1.4   21 Jul 1997 16:54:12   DCHI
// Changed EXTERNAL_DETECT_VERSION from 2 to 3.
//
//    Rev 1.3   11 Jul 1997 12:25:56   DDREW
// This rev is from newvir, rev numbers are a bit screwed up
//
//    Rev 1.6   08 Jul 1997 15:53:38   DDREW
// Fixed up a few instances of CALLBACK_STRUCT that were left undone
//
//    Rev 1.5   30 May 1997 12:08:56   CNACHEN
// Changed repair funcs for NAVEX15 to take additional cookie param.
//
//    Rev 1.4   08 May 1997 15:04:42   MKEATIN
// Added SYM_DOSX to the inline wrappers
//
//    Rev 1.3   08 May 1997 14:20:48   MKEATIN
// uTableSize is now a member of EXPORT15_TABLE_TYPE on all platforms.
//
//    Rev 1.2   08 May 1997 13:12:30   MKEATIN
// Removed prototypes for Navex32Init and Navex32End.
//
//    Rev 1.1   02 May 1997 14:38:00   CNACHEN
// Fixed QAK6 vs trunk build problem dealing with stCallBack vs lpstcallback in hcontext structure
//
//    Rev 1.0   02 May 1997 13:29:58   CNACHEN
// Initial revision.
//
//
//***************************************************************************

#if !defined(__NAVEX15_H)
#define __NAVEX15_H

//---------------------------------------------------------------------------
//
// Global NAVEX type definitions and constants.
//
//---------------------------------------------------------------------------
typedef unsigned int EXTSTATUS;

#define EXTSTATUS_OK                0
#define EXTSTATUS_FILE_ERROR        1
#define EXTSTATUS_VIRUS_FOUND       2
#define EXTSTATUS_MEM_ERROR         3
#define EXTSTATUS_NO_REPAIR         4
#define EXTSTATUS_ENTRY_NOT_FOUND   5
#define EXTSTATUS_DISK_READ_ERROR   6
#define EXTSTATUS_DISK_WRITE_ERROR  7
#define EXTSTATUS_INIT_ERROR        8

#define EXTERNAL_DETECT_VERSION     3

//---------------------------------------------------------------------------
//
// Interface IDs
//
//---------------------------------------------------------------------------

#define NAVEX15_INTERFACE           1000

//---------------------------------------------------------------------------
//
// Prototypes of external functions.
//
//---------------------------------------------------------------------------

#if defined(__cplusplus)
extern "C"
{
#endif

#if defined(SYM_WIN16) || defined(WIN16) || defined(SYM_DOSX)
#define NLOADDS     LOADDS
#else
#define NLOADDS
#endif


//---------------------------------------------------------------------------
//
// Structure containing pointers to the external functions.
//
// Note for VxD platform: This structure is filled by the only
// DeviceIOControl call that the external VxD supports.
//
//---------------------------------------------------------------------------
typedef struct tag_EXPORT15_TABLE
{
    UINT uTableSize;

    EXTSTATUS (FAR WINAPI *EXTScanFile)(LPCALLBACKREV2 ,
                        LPTSTR         ,
                        HFILE          ,
                        LPBYTE         ,
                        LPBYTE         ,
                        WORD           ,
                        LPWORD         );

    EXTSTATUS (FAR WINAPI *EXTScanBoot)(LPCALLBACKREV2 ,
                        BYTE       ,
                        BYTE       ,
                        WORD, WORD, WORD,
                        LPBYTE     ,
                        LPBYTE     ,
                        WORD,
                        LPWORD     );

    EXTSTATUS (FAR WINAPI *EXTScanPart)(LPCALLBACKREV2 ,
                        BYTE       ,
                        LPBYTE     ,
                        LPBYTE     ,
                        WORD       ,
                        LPWORD     );

    EXTSTATUS (FAR WINAPI *EXTScanMemory)(LPCALLBACKREV2 ,
                          WORD ,
                          BOOL ,
                          LPWORD     );

    EXTSTATUS (FAR WINAPI *EXTRepairFile)(LPCALLBACKREV2 ,
                          WORD,
                          LPN30      ,
                          LPTSTR     ,
                          LPBYTE     ,
                          LPVOID);

    EXTSTATUS (FAR WINAPI *EXTRepairBoot)(LPCALLBACKREV2 ,
                          WORD,
                          LPN30      ,
                          BYTE       ,
                          BYTE       ,
                          WORD, WORD, WORD,
                          LPBYTE,
                          LPVOID     );

    EXTSTATUS (FAR WINAPI *EXTRepairPart)(LPCALLBACKREV2 ,
                          WORD,
                          LPN30      ,
                          BYTE       ,
                          LPBYTE,
                          LPVOID     );

    EXTSTATUS (FAR WINAPI *EXTStartUp)
    (
        LPCALLBACKREV2                          lpstCallBacks,
        WORD                                    wClientVersion,
        struct tag_EXPORT15_TABLE FAR *         lpstChain,
        LPTSTR                                  lpszNAVEXDataDir,
        LPTSTR                                  lpszNAVEXINIFile,
        LPWORD                                  lpwServerVersion
    );

    EXTSTATUS (FAR WINAPI *EXTShutDown)
    (
        LPCALLBACKREV2                          lpstCallBacks
    );

} EXPORT15_TABLE_TYPE, *PEXPORT15_TABLE_TYPE,
  EXPORT_TABLE_TYPE, *PEXPORT_TABLE_TYPE;


#if !defined(SYM_NLM) || !defined(AVAPI_INCLUDE)

EXTSTATUS FAR WINAPI NLOADDS EXTStartUp
(
    LPCALLBACKREV2                          lpstCallBacks,
    WORD                                    wClientVersion,
    EXPORT15_TABLE_TYPE FAR *               lpstChain,
    LPTSTR                                  lpszNAVEXDataDir,
    LPTSTR                                  lpszNAVEXINIFile,
    LPWORD                                  lpwServerVersion
);

EXTSTATUS FAR WINAPI NLOADDS EXTShutDown
(
    LPCALLBACKREV2                          lpstCallBacks
);

EXTSTATUS FAR WINAPI NLOADDS EXTScanFile(LPCALLBACKREV2 ,
                        LPTSTR         ,
                        HFILE          ,
                        LPBYTE         ,
                        LPBYTE         ,
                        WORD           ,
                        LPWORD         );

EXTSTATUS FAR WINAPI NLOADDS EXTScanBoot(LPCALLBACKREV2 ,
                        BYTE       ,
                        BYTE       ,
                        WORD, WORD, WORD,
                        LPBYTE     ,
                        LPBYTE     ,
                        WORD,
                        LPWORD     );

EXTSTATUS FAR WINAPI NLOADDS EXTScanPart(LPCALLBACKREV2 ,
                        BYTE       ,
                        LPBYTE     ,
                        LPBYTE     ,
                        WORD       ,
                        LPWORD     );

EXTSTATUS FAR WINAPI NLOADDS EXTScanMemory(LPCALLBACKREV2 ,
                          WORD ,
                          BOOL ,
                          LPWORD     );

EXTSTATUS FAR WINAPI NLOADDS EXTRepairFile(LPCALLBACKREV2 ,
                          WORD,
                          LPN30      ,
                          LPTSTR     ,
                          LPBYTE     ,
                          LPVOID     );

EXTSTATUS FAR WINAPI NLOADDS EXTRepairBoot(LPCALLBACKREV2 ,
                          WORD,
                          LPN30      ,
                          BYTE       ,
                          BYTE       ,
                          WORD, WORD, WORD,
                          LPBYTE     ,
                          LPVOID);

EXTSTATUS FAR WINAPI NLOADDS EXTRepairPart(LPCALLBACKREV2 ,
                          WORD,
                          LPN30      ,
                          BYTE       ,
                          LPBYTE     ,
                          LPVOID);

#endif // !defined(SYM_NLM) || !defined(AVAPI_INCLUDE)


EXTSTATUS FAR WINAPI NLOADDS EXTQueryInterface
(
    DWORD  dwInterfaceID,
    LPLPVOID lplpvExportTable
);

#if defined(__cplusplus)
}
#endif


typedef struct
{
    BYTE byExportSig[16];
    void (*wEntrypoint)();
} MODULE_EXPORT_TABLE_TYPE, *PMODULE_EXPORT_TABLE_TYPE;

//---------------------------------------------------------------------------
//
// External structures.
//
//---------------------------------------------------------------------------
#if defined(__cplusplus)
extern "C"
{
#endif

extern EXPORT15_TABLE_TYPE export_table;

#if defined(__cplusplus)
}
#endif

//---------------------------------------------------------------------------
//
// Wrappers of the external functions.
//
//---------------------------------------------------------------------------

#ifdef QAK6
#define CONTEXT_CALL_BACK   &hContext->stCallBack
#else
#define CONTEXT_CALL_BACK   hContext->lpstCallBack
#endif

#if !defined(SYM_NLM)

#if defined(AVAPI_INCLUDE)

inline EXTSTATUS FAR WINAPI EXTStartUp
(
    HVCONTEXT                               hContext,
    WORD                                    a,
    EXPORT15_TABLE_TYPE FAR *               b,
    LPTSTR                                  c,
    LPTSTR                                  d,
    LPWORD                                  e
)
{
#if ( defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) || defined(SYM_DOSX) || defined(SYM_UNIX)) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTStartUp (
                    CONTEXT_CALL_BACK, a, b, c, d, e )
               );
        }

#endif

    return ( EXTSTATUS_OK );
}

inline EXTSTATUS FAR WINAPI EXTShutDown
(
    HVCONTEXT                               hContext
)
{
#if ( defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) || defined(SYM_DOSX) || defined(SYM_UNIX)) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTShutDown (
                    CONTEXT_CALL_BACK)
               );
        }

#endif

    return ( EXTSTATUS_OK );
}

inline EXTSTATUS FAR WINAPI EXTScanFile(HVCONTEXT hContext,
                        LPTSTR         a,
                        HFILE          b,
                        LPBYTE         c,
                        LPBYTE         d,
                        WORD           e,
                        LPWORD         f)
    {

#if ( defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) || defined(SYM_DOSX) || defined(SYM_UNIX)) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTScanFile (
                    CONTEXT_CALL_BACK, a, b, c, d, e, f )
               );
        }

#endif

    return ( EXTSTATUS_OK );
    }


inline EXTSTATUS FAR WINAPI EXTScanBoot(HVCONTEXT hContext,
                        BYTE       a,
                        BYTE       b,
                        WORD c, WORD d, WORD e,
                        LPBYTE     f,
                        LPBYTE     g,
                        WORD       h,
                        LPWORD     i)
    {

#if ( defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) || defined(SYM_DOSX) || defined(SYM_UNIX)) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTScanBoot (
                    CONTEXT_CALL_BACK, a, b, c, d, e, f, g, h, i )
               );
        }

#endif

    return ( EXTSTATUS_OK );
    }

inline EXTSTATUS FAR WINAPI EXTScanPart(HVCONTEXT hContext,
                        BYTE       a,
                        LPBYTE     b,
                        LPBYTE     c,
                        WORD       d,
                        LPWORD     e)
    {

#if ( defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) || defined(SYM_DOSX) || defined(SYM_UNIX)) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTScanPart (
                    CONTEXT_CALL_BACK, a, b, c, d, e )
               );
        }

#endif

    return ( EXTSTATUS_OK );
    }

inline EXTSTATUS FAR WINAPI EXTScanMemory(HVCONTEXT hContext,
                          WORD   a,
                          BOOL   b,
                          LPWORD c)
    {

#if ( defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) || defined(SYM_DOSX) || defined(SYM_UNIX)) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTScanMemory (
                    CONTEXT_CALL_BACK, a, b, c)
               );
        }

#endif

    return ( EXTSTATUS_OK );
    }

inline EXTSTATUS FAR WINAPI EXTRepairFile(HVCONTEXT hContext,
                          WORD       a,
                          LPN30      b,
                          LPTSTR     c,
                          LPBYTE     d,
                          LPVOID     e)
    {

#if ( defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) || defined(SYM_DOSX) || defined(SYM_UNIX)) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTRepairFile (
                    CONTEXT_CALL_BACK, a, b, c, d, e)
               );
        }

#endif

    return ( EXTSTATUS_NO_REPAIR );
    }

inline EXTSTATUS FAR WINAPI EXTRepairBoot(HVCONTEXT hContext,
                          WORD       a,
                          LPN30      b,
                          BYTE       c,
                          BYTE       d,
                          WORD e, WORD f, WORD g,
                          LPBYTE     h,
                          LPVOID     i)
    {

#if ( defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) || defined(SYM_DOSX) || defined(SYM_UNIX)) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTRepairBoot (
                    CONTEXT_CALL_BACK, a, b, c, d, e, f, g, h, i)
               );
        }

#endif

    return ( EXTSTATUS_NO_REPAIR );
    }

inline EXTSTATUS FAR WINAPI EXTRepairPart(HVCONTEXT hContext,
                          WORD       a,
                          LPN30      b,
                          BYTE       c,
                          LPBYTE     d,
                          LPVOID     e)
    {

#if ( defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) || defined(SYM_DOSX) || defined(SYM_UNIX)) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTRepairPart (
                    CONTEXT_CALL_BACK, a, b, c, d, e)
               );
        }

#endif

    return ( EXTSTATUS_NO_REPAIR );
    }

#endif      // AVAPI_INCLUDE


#else       // ELSE if defined(SYM_NLM)

#if defined(AVAPI_INCLUDE)

EXTSTATUS FAR WINAPI EXTStartUp
(
    HVCONTEXT                               hContext,
    WORD                                    a,
    EXPORT_TABLE_TYPE FAR *                 b,
    LPTSTR                                  c,
    LPTSTR                                  d,
    LPWORD                                  e
)
{
    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTStartUp (
                    CONTEXT_CALL_BACK, a, b, c, d, e )
               );
        }

    return ( EXTSTATUS_OK );
}

EXTSTATUS FAR WINAPI EXTShutDown
(
    HVCONTEXT                               hContext
)
{

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTShutDown (
                    CONTEXT_CALL_BACK)
               );
        }

    return ( EXTSTATUS_OK );
}


EXTSTATUS FAR WINAPI EXTScanFile(HVCONTEXT hContext,
                        LPTSTR         a,
                        HFILE          b,
                        LPBYTE         c,
                        LPBYTE         d,
                        WORD           e,
                        LPWORD         f)
    {

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTScanFile (
                    CONTEXT_CALL_BACK, a, b, c, d, e, f )
               );
        }

    return ( EXTSTATUS_OK );
    }


EXTSTATUS FAR WINAPI EXTScanBoot(HVCONTEXT hContext,
                        BYTE       a,
                        BYTE       b,
                        WORD c, WORD d, WORD e,
                        LPBYTE     f,
                        LPBYTE     g,
                        WORD       h,
                        LPWORD     i)
    {

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTScanBoot (
                    CONTEXT_CALL_BACK, a, b, c, d, e, f, g, h, i )
               );
        }

    return ( EXTSTATUS_OK );
    }

EXTSTATUS FAR WINAPI EXTScanPart(HVCONTEXT hContext,
                        BYTE       a,
                        LPBYTE     b,
                        LPBYTE     c,
                        WORD       d,
                        LPWORD     e)
    {

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTScanPart (
                    CONTEXT_CALL_BACK, a, b, c, d, e )
               );
        }

    return ( EXTSTATUS_OK );
    }

EXTSTATUS FAR WINAPI EXTScanMemory(HVCONTEXT hContext,
                          WORD   a,
                          BOOL   b,
                          LPWORD c)
    {

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTScanMemory (
                    CONTEXT_CALL_BACK, a, b, c)
               );
        }

    return ( EXTSTATUS_OK );
    }

EXTSTATUS FAR WINAPI EXTRepairFile(HVCONTEXT hContext,
                          WORD       a,
                          LPN30      b,
                          LPTSTR     c,
                          LPBYTE     d,
                          LPVOID     e)
    {

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTRepairFile (
                    CONTEXT_CALL_BACK, a, b, c, d, e )
               );
        }

    return ( EXTSTATUS_NO_REPAIR );
    }

EXTSTATUS FAR WINAPI EXTRepairBoot(HVCONTEXT hContext,
                          WORD       a,
                          LPN30      b,
                          BYTE       c,
                          BYTE       d,
                          WORD e, WORD f, WORD g,
                          LPBYTE     h,
                          LPVOID     i)
    {

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTRepairBoot (
                    CONTEXT_CALL_BACK, a, b, c, d, e, f, g, h, i )
               );
        }

    return ( EXTSTATUS_NO_REPAIR );
    }

EXTSTATUS FAR WINAPI EXTRepairPart(HVCONTEXT hContext,
                          WORD       a,
                          LPN30      b,
                          BYTE       c,
                          LPBYTE     d,
                          LPVOID     e)
    {

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT15_TABLE_TYPE)hContext->prExportTable)->EXTRepairPart (
                    CONTEXT_CALL_BACK, a, b, c, d, e)
               );
        }

    return ( EXTSTATUS_NO_REPAIR );
    }

#endif      // AVAPI_INCLUDE
#endif      // SYM_NLM



//---------------------------------------------------------------------------
//
// Prototypes of functions that applications use to access NAVEX15.
//
//---------------------------------------------------------------------------
#if defined(AVAPI_INCLUDE)

#if defined(SYM_VXD)

    EXTSTATUS WINAPI EXTInitVxD (
        HVCONTEXT hContext
        );

    EXTSTATUS WINAPI EXTCloseVxD (
        HVCONTEXT hContext
        );

#elif defined(SYM_NTK)

    EXTSTATUS WINAPI EXTInitNTK (
        HVCONTEXT hContext
        );

    EXTSTATUS WINAPI EXTCloseNTK (
        HVCONTEXT hContext
        );

#elif defined(SYM_WIN)

    EXTSTATUS WINAPI EXTInitWin (
        HVCONTEXT hContext
        );

    EXTSTATUS WINAPI EXTCloseWin (
        HVCONTEXT hContext
        );

#elif defined(SYM_DOSX)

    EXTSTATUS WINAPI EXTInitDX (
        HVCONTEXT hContext
        );

    EXTSTATUS WINAPI EXTCloseDX (
        HVCONTEXT hContext
        );

#elif defined(SYM_NLM)

    EXTSTATUS WINAPI EXTInitNLM (
        HVCONTEXT hContext
        );

    EXTSTATUS WINAPI EXTCloseNLM (
        HVCONTEXT hContext
        );

#elif defined(SYM_UNIX)

    EXTSTATUS WINAPI EXTInitUnix (
        HVCONTEXT hContext
        );

    EXTSTATUS WINAPI EXTCloseUnix (
        HVCONTEXT hContext
        );

#endif

#endif // AVAPI_INCLUDE

#endif // !defined __NAVEX15_H
