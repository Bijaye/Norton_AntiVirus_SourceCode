// Copyright 1995 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:\include\vcs\avex.h_v   1.1   29 Aug 1995 12:16:08   CNACHEN  $
//
// This file defines protypes of external AVEX functions and related
// structures.
//
//***************************************************************************
// $Log:   S:\include\vcs\avex.h_v  $
// 
//    Rev 1.1   29 Aug 1995 12:16:08   CNACHEN
// 
//    Rev 1.0.1.0   11 Aug 1995 14:46:46   RSTANEV
// Branch base for version QAK8
// 
//    Rev 1.0   11 Aug 1995 14:37:04   RSTANEV
// Initial revision.
//
//***************************************************************************

#if !defined(__AVEX_H)
#define __AVEX_H

//---------------------------------------------------------------------------
//
// Global AVEX type definitions and constants.
//
//---------------------------------------------------------------------------
typedef unsigned int EXTSTATUS;

#define EXTSTATUS_OK            0
#define EXTSTATUS_FILE_ERROR    1
#define EXTSTATUS_VIRUS_FOUND   2
#define EXTSTATUS_MEM_ERROR     3

#define EXTERNAL_DETECT_VERSION 1

#define EXPORT_STRING           "NAVEXPORTTABLE!"

//---------------------------------------------------------------------------
//
// Prototypes of external functions.
//
//---------------------------------------------------------------------------
EXTSTATUS FAR WINAPI EXTScanFile(LPCALLBACKREV1 ,
                        LPSTR          ,
                        HFILE          ,
                        LPBYTE         ,
						LPBYTE         ,
                        WORD           ,
                        LPWORD         );

EXTSTATUS FAR WINAPI EXTScanBoot(LPCALLBACKREV1 ,
                        BYTE       ,
                        WORD, WORD, WORD,
                        LPBYTE     ,
                        LPBYTE     ,
                        WORD,
                        LPWORD     );

EXTSTATUS FAR WINAPI EXTScanPart(LPCALLBACKREV1 ,
                        BYTE       ,
                        LPBYTE     ,
                        LPBYTE     ,
                        WORD       ,
                        LPWORD     );

EXTSTATUS FAR WINAPI EXTScanMemory(LPCALLBACKREV1 ,
                          WORD ,
                          LPWORD     );

EXTSTATUS FAR WINAPI EXTRepairFile(LPCALLBACKREV1 ,
                          WORD,
                          LPN30      ,
                          LPSTR      ,
                          LPBYTE     );

EXTSTATUS FAR WINAPI EXTRepairBoot(LPCALLBACKREV1 ,
                          WORD,
                          LPN30      ,
                          BYTE       ,
                          WORD, WORD, WORD,
                          LPBYTE     );

EXTSTATUS FAR WINAPI EXTRepairPart(LPCALLBACKREV1 ,
                          WORD,
                          LPN30      ,
                          BYTE       ,
                          LPBYTE     );

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
    UINT uTableSize;

#if defined(SYM_DOS)
    BYTE byExportSig[16];
#endif

    EXTSTATUS (FAR WINAPI *EXTScanFile)(LPCALLBACKREV1 ,
                        LPSTR          ,
						HFILE          ,
						LPBYTE         ,
						LPBYTE         ,
						WORD           ,
						LPWORD         );

    EXTSTATUS (FAR WINAPI *EXTScanBoot)(LPCALLBACKREV1 ,
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
						  LPWORD     );

    EXTSTATUS (FAR WINAPI *EXTRepairFile)(LPCALLBACKREV1 ,
						  WORD,
						  LPN30      ,
						  LPSTR      ,
						  LPBYTE     );

    EXTSTATUS (FAR WINAPI *EXTRepairBoot)(LPCALLBACKREV1 ,
						  WORD,
						  LPN30      ,
						  BYTE       ,
						  WORD, WORD, WORD,
						  LPBYTE     );

    EXTSTATUS (FAR WINAPI *EXTRepairPart)(LPCALLBACKREV1 ,
						  WORD,
						  LPN30      ,
						  BYTE       ,
						  LPBYTE     );

} EXPORT_TABLE_TYPE, *PEXPORT_TABLE_TYPE;

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
#if defined(SYM_VXD) && defined(AVAPI_INCLUDE) && defined(__cplusplus)

inline EXTSTATUS FAR WINAPI EXTScanFile(HVCONTEXT hContext,
                        LPSTR          a,
                        HFILE          b,
                        LPBYTE         c,
                        LPBYTE         d,
                        WORD           e,
                        LPWORD         f)
    {
    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanFile (
                    hContext->lpstCallBack, a, b, c, d, e, f )
               );
        }

    return ( EXTSTATUS_OK );
    }

inline EXTSTATUS FAR WINAPI EXTScanBoot(HVCONTEXT hContext,
                        BYTE       a,
                        WORD b, WORD c, WORD d,
                        LPBYTE     e,
                        LPBYTE     f,
                        WORD       g,
                        LPWORD     h)
    {
    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanBoot (
                    hContext->lpstCallBack, a, b, c, d, e, f, g, h )
               );
        }

    return ( EXTSTATUS_OK );
    }

inline EXTSTATUS FAR WINAPI EXTScanPart(HVCONTEXT hContext,
                        BYTE       a,
                        LPBYTE     b,
                        LPBYTE     c,
                        WORD       d,
                        LPWORD     e)
    {
    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanPart (
                    hContext->lpstCallBack, a, b, c, d, e )
               );
        }

    return ( EXTSTATUS_OK );
    }

inline EXTSTATUS FAR WINAPI EXTScanMemory(HVCONTEXT hContext,
                          WORD   a,
                          LPWORD b)
    {
    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTScanMemory (
                    hContext->lpstCallBack, a, b )
               );
        }

    return ( EXTSTATUS_OK );
    }

inline EXTSTATUS FAR WINAPI EXTRepairFile(HVCONTEXT hContext,
                          WORD       a,
                          LPN30      b,
                          LPSTR      c,
                          LPBYTE     d)
    {
    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTRepairFile (
                    hContext->lpstCallBack, a, b, c, d )
               );
        }

    return ( EXTSTATUS_OK );
    }

inline EXTSTATUS FAR WINAPI EXTRepairBoot(HVCONTEXT hContext,
                          WORD       a,
                          LPN30      b,
                          BYTE       c,
                          WORD d, WORD e, WORD f,
                          LPBYTE     g)
    {
    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTRepairBoot (
                    hContext->lpstCallBack, a, b, c, d, e, f, g )
               );
        }

    return ( EXTSTATUS_OK );
    }

inline EXTSTATUS FAR WINAPI EXTRepairPart(HVCONTEXT hContext,
                          WORD       a,
                          LPN30      b,
                          BYTE       c,
                          LPBYTE     d)
    {
    if ( hContext->prExportTable != NULL )
        {
        return ( ((PEXPORT_TABLE_TYPE)hContext->prExportTable)->EXTRepairPart (
                    hContext->lpstCallBack, a, b, c, d )
               );
        }

    return ( EXTSTATUS_OK );
    }

#endif

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

#endif
