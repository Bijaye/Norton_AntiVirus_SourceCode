// Copyright 1995 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/INCLUDE/VCS/navex.h_v   1.0   27 Feb 1998 15:45:04   AOONWAL  $
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
// $Log:   S:/INCLUDE/VCS/navex.h_v  $
// 
//    Rev 1.0   27 Feb 1998 15:45:04   AOONWAL
// File moved from NAVEX project.
// 
//    Rev 1.24   16 Jul 1997 10:42:02   TCASHIN
// Changed the navex version number to 3.
// 
//    Rev 1.23   03 Jul 1997 11:14:14   JALLEE
// Added prototypes for EXTInitDX() and EXTCloseDX() used to initialize external
// scanning on the DX platform.
//
//    Rev 1.22   02 May 1997 14:31:46   CNACHEN
// Fixed QAK6 vs Trunk problems with hvcontext->stcallback vs hvcontext->lpstcallback
//
//    Rev 1.21   02 May 1997 13:33:14   CNACHEN
// Added support for NAVEX1.5
//
//    Rev 1.20   29 Apr 1997 10:28:08   DDREW
// Had to make the same changes for the Win platform as done below
//
//    Rev 1.19   28 Apr 1997 16:31:24   DDREW
// No changes - maybe this will be the last time
//
//    Rev 1.4   28 Apr 1997 14:19:58   DDREW
// made hcontext->lpstCallBack >> &hcontext->stCallBack
//
//    Rev 1.3   28 Apr 1997 11:11:58   DDREW
// Tried to fix thinkgs back the way they were before the way they are...
//
//    Rev 1.18   22 Nov 1996 18:26:28   GDZIECI
// Retracted my previous changes, at least for now.
//
//    Rev 1.17   21 Nov 1996 17:12:00   AOONWAL
// No change.
//
//    Rev 1.16   13 Nov 1996 20:08:56   GDZIECI
// Rrestored QUAKE6 defines. We must be able to build Elwood.
//
//    Rev 1.15   29 Oct 1996 13:00:26   AOONWAL
// No change.
//
//    Rev 1.14   25 Jul 1996 17:53:28   DALLEE
// WIN and W32 platforms now dynamically load NAVEX DLL in the same manner
// as VXD and NLM.
// Added prototypes for this: EXTInitWin() and EXTCloseWin()
// Also modified AVAPI_INCLUDE EXTScanXXX() inline wrapper functions to use
//   NAVEX pointers inside hVContext for WIN and W32.
//
//    Rev 1.13   17 Jun 1996 14:05:10   MKEATIN
// Added EXTSTATUS_INIT_ERROR to status constants
//
//    Rev 1.12   03 May 1996 17:08:38   DCHI
// Changed EXTERNAL_DETECT_VERSION from 1 to 2.
//
//    Rev 1.11   18 Apr 1996 17:15:28   MKEATIN
// Added prototypes for NavEx32Init() and NavEx32End()
//
//    Rev 1.10   15 Apr 1996 19:04:18   RSTANEV
// Added SYM_NTK support.
//
//    Rev 1.9   16 Feb 1996 10:41:58   DALLEE
// Removed a #ifndef SYM_DOSX around VCONTEXT prototyped EXTxxx functions.
//
//    Rev 1.8   15 Feb 1996 16:15:24   MKEATIN
// Took wrapper functions out of this header file and put them in scn_init.cpp
// in virscan project
//
//    Rev 1.7   13 Feb 1996 17:55:18   MKEATIN
// Added DX Support
//
//    Rev 1.6   02 Feb 1996 18:28:06   RSTANEV
// Added Unicode support.
//
//    Rev 1.5   11 Jan 1996 07:56:38   JWORDEN
// Remove ifdefs for QUAKE6. Use lpszCallBack for everyone
//
//    Rev 1.4   25 Oct 1995 17:54:06   DCHI
// Added important notes in the version header.
//
//    Rev 1.3   24 Oct 1995 18:13:24   CNACHEN
// Added proper #ifdef for QAK6 AVAPI usage...
//
//    Rev 1.2   19 Oct 1995 14:17:50   DCHI
// Added LOADDS to external functions for WIN16.  Modified DOS export
// table structure to support tiny model modules.
//
//    Rev 1.1   13 Oct 1995 12:57:28   DCHI
// Added NLM specific prototypes.
//
//    Rev 1.0   13 Oct 1995 10:09:06   DCHI
// Initial revision.
//
//***************************************************************************

#if !defined(__NAVEX_H)
#define __NAVEX_H


#ifdef NAVEX15
#include "navex15.h"
#else

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

#define EXTERNAL_DETECT_VERSION 3

#define EXPORT_STRING           "NAVEXPORTTABLE!"

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

#if !defined(SYM_NLM) || !defined(AVAPI_INCLUDE)

EXTSTATUS FAR WINAPI NLOADDS EXTScanFile(LPCALLBACKREV1 ,
                        LPTSTR         ,
                        HFILE          ,
                        LPBYTE         ,
                        LPBYTE         ,
                        WORD           ,
                        LPWORD         );

EXTSTATUS FAR WINAPI NLOADDS EXTScanBoot(LPCALLBACKREV1 ,
                        BYTE       ,
                        BYTE       ,
                        WORD, WORD, WORD,
                        LPBYTE     ,
                        LPBYTE     ,
                        WORD,
                        LPWORD     );

EXTSTATUS FAR WINAPI NLOADDS EXTScanPart(LPCALLBACKREV1 ,
                        BYTE       ,
                        LPBYTE     ,
                        LPBYTE     ,
                        WORD       ,
                        LPWORD     );

EXTSTATUS FAR WINAPI NLOADDS EXTScanMemory(LPCALLBACKREV1 ,
                          WORD ,
                          BOOL ,
                          LPWORD     );

EXTSTATUS FAR WINAPI NLOADDS EXTRepairFile(LPCALLBACKREV1 ,
                          WORD,
                          LPN30      ,
                          LPTSTR     ,
                          LPBYTE     );

EXTSTATUS FAR WINAPI NLOADDS EXTRepairBoot(LPCALLBACKREV1 ,
                          WORD,
                          LPN30      ,
                          BYTE       ,
                          BYTE       ,
                          WORD, WORD, WORD,
                          LPBYTE     );

EXTSTATUS FAR WINAPI NLOADDS EXTRepairPart(LPCALLBACKREV1 ,
                          WORD,
                          LPN30      ,
                          BYTE       ,
                          LPBYTE     );

#endif // !defined(SYM_NLM) || !defined(AVAPI_INCLUDE)


#ifdef SYM_WIN32

STATUS SYM_EXPORT WINAPI NavEx32Init (VOID);
STATUS SYM_EXPORT WINAPI NavEx32End (VOID);

#endif // SYM_WIN32


#if defined(__cplusplus)
}
#endif

//---------------------------------------------------------------------------
//
// Structure containing pointers to the external functions.
//
// Note for VxD platform: This structure is filled by the only
// DeviceIOControl call that the external VxD supports.
//
//---------------------------------------------------------------------------
typedef struct
{
#if !defined(SYM_DOS)
    UINT uTableSize;
#endif

#if defined(SYM_DOS)
    BYTE byExportSig[16];
#endif

    EXTSTATUS (FAR WINAPI *EXTScanFile)(LPCALLBACKREV1 ,
                        LPTSTR         ,
                        HFILE          ,
                        LPBYTE         ,
                        LPBYTE         ,
                        WORD           ,
                        LPWORD         );

    EXTSTATUS (FAR WINAPI *EXTScanBoot)(LPCALLBACKREV1 ,
                        BYTE       ,
                        BYTE       ,
                        WORD, WORD, WORD,
                        LPBYTE     ,
                        LPBYTE     ,
                        WORD,
                        LPWORD     );

    EXTSTATUS (FAR WINAPI *EXTScanPart)(LPCALLBACKREV1 ,
                        BYTE       ,
                        LPBYTE     ,
                        LPBYTE     ,
                        WORD       ,
                        LPWORD     );

    EXTSTATUS (FAR WINAPI *EXTScanMemory)(LPCALLBACKREV1 ,
                          WORD ,
                          BOOL ,
                          LPWORD     );

    EXTSTATUS (FAR WINAPI *EXTRepairFile)(LPCALLBACKREV1 ,
                          WORD,
                          LPN30      ,
                          LPTSTR     ,
                          LPBYTE     );

    EXTSTATUS (FAR WINAPI *EXTRepairBoot)(LPCALLBACKREV1 ,
                          WORD,
                          LPN30      ,
                          BYTE       ,
                          BYTE       ,
                          WORD, WORD, WORD,
                          LPBYTE     );

    EXTSTATUS (FAR WINAPI *EXTRepairPart)(LPCALLBACKREV1 ,
                          WORD,
                          LPN30      ,
                          BYTE       ,
                          LPBYTE     );

} EXPORT_TABLE_TYPE, *PEXPORT_TABLE_TYPE;

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

extern EXPORT_TABLE_TYPE export_table;

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

inline EXTSTATUS FAR WINAPI EXTScanFile(HVCONTEXT hContext,
                        LPTSTR         a,
                        HFILE          b,
                        LPBYTE         c,
                        LPBYTE         d,
                        WORD           e,
                        LPWORD         f)
    {

#if ( defined (SYM_DOSX) || defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) ) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanFile (
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

#if ( defined (SYM_DOSX) || defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) ) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanBoot (
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

#if (  defined (SYM_DOSX) || defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) ) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanPart (
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

#if (  defined (SYM_DOSX) || defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) ) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanMemory (
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
                          LPBYTE     d)
    {

#if (  defined (SYM_DOSX) || defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) ) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTRepairFile (
                    CONTEXT_CALL_BACK, a, b, c, d )
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
                          LPBYTE     h)
    {

#if (  defined (SYM_DOSX) || defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) ) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTRepairBoot (
                    CONTEXT_CALL_BACK, a, b, c, d, e, f, g, h )
               );
        }

#endif

    return ( EXTSTATUS_NO_REPAIR );
    }

inline EXTSTATUS FAR WINAPI EXTRepairPart(HVCONTEXT hContext,
                          WORD       a,
                          LPN30      b,
                          BYTE       c,
                          LPBYTE     d)
    {

#if (  defined (SYM_DOSX) || defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) ) && defined(__cplusplus)

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTRepairPart (
                    CONTEXT_CALL_BACK, a, b, c, d )
               );
        }

#endif

    return ( EXTSTATUS_NO_REPAIR );
    }

#endif      // AVAPI_INCLUDE


#else       // ELSE if defined(SYM_NLM)

#if defined(AVAPI_INCLUDE)

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
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanFile (
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
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanBoot (
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
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanPart (
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
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanMemory (
                    CONTEXT_CALL_BACK, a, b, c)
               );
        }

    return ( EXTSTATUS_OK );
    }

EXTSTATUS FAR WINAPI EXTRepairFile(HVCONTEXT hContext,
                          WORD       a,
                          LPN30      b,
                          LPTSTR     c,
                          LPBYTE     d)
    {

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTRepairFile (
                    CONTEXT_CALL_BACK, a, b, c, d )
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
                          LPBYTE     h)
    {

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTRepairBoot (
                    CONTEXT_CALL_BACK, a, b, c, d, e, f, g, h )
               );
        }

    return ( EXTSTATUS_NO_REPAIR );
    }

EXTSTATUS FAR WINAPI EXTRepairPart(HVCONTEXT hContext,
                          WORD       a,
                          LPN30      b,
                          BYTE       c,
                          LPBYTE     d)
    {

    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTRepairPart (
                    CONTEXT_CALL_BACK, a, b, c, d )
               );
        }

    return ( EXTSTATUS_NO_REPAIR );
    }

#endif      // AVAPI_INCLUDE
#endif      // SYM_NLM



//---------------------------------------------------------------------------
//
// Prototypes of functions for communicating with the VxD containing
// external AV functions.  These functions will be available only when
// AVAPI.H is included (this means, the wrappers of the external functions,
// the ones that transfer control to the external functions.)
//
//---------------------------------------------------------------------------
#if defined(SYM_VXD) && defined(AVAPI_INCLUDE)

EXTSTATUS WINAPI EXTInitVxD (
    HVCONTEXT hContext
    );

EXTSTATUS WINAPI EXTCloseVxD (
    HVCONTEXT hContext
    );

#endif

#if defined(SYM_NTK) && defined(AVAPI_INCLUDE)

EXTSTATUS WINAPI EXTInitNTK (
    HVCONTEXT hContext
    );

EXTSTATUS WINAPI EXTCloseNTK (
    HVCONTEXT hContext
    );

#endif

#if defined(SYM_WIN) && defined(AVAPI_INCLUDE)

EXTSTATUS WINAPI EXTInitWin (
    HVCONTEXT hContext
    );

EXTSTATUS WINAPI EXTCloseWin (
    HVCONTEXT hContext
    );

#endif


#if defined(SYM_NLM) && defined(AVAPI_INCLUDE)

EXTSTATUS WINAPI EXTInitNLM (
    HVCONTEXT hContext
    );

EXTSTATUS WINAPI EXTCloseNLM (
    HVCONTEXT hContext
    );

#endif

#if defined( SYM_DOSX ) && defined( AVAPI_INCLUDE )

EXTSTATUS WINAPI EXTInitDX (
    HVCONTEXT hContext
    );

EXTSTATUS WINAPI EXTCloseDX (
    HVCONTEXT hContext
    );

#endif


#if defined( SYM_UNIX ) && defined( AVAPI_INCLUDE )

EXTSTATUS WINAPI EXTInitUnix (
    HVCONTEXT hContext
    );

EXTSTATUS WINAPI EXTCloseUnix (
    HVCONTEXT hContext
    );

#endif

#endif

#endif

