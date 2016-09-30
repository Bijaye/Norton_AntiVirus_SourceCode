#ifndef __NAVEXISI_H
#define __NAVEXISI_H

// Interface ID
#define NAVEX15_ISI_INTERFACE       1001

// Prototypes of external functions.
#ifdef __cplusplus
extern "C"
{
#endif

#if defined(SYM_WIN16) || defined(WIN16) || defined(SYM_DOSX)
#define NLOADDS     LOADDS
#else
#define NLOADDS
#endif

typedef LPVOID FAR *LPLPVOID;

// Structure containing pointers to the external functions.
typedef struct tag_EXPORT15_ISI_TABLE
{
    UINT uTableSize;

    EXTSTATUS (FAR WINAPI *EXTIsInfectableFile)
    (
        LPCALLBACKREV2          lpstCallBacks,
        HFILE                   hFile,
        LPBYTE                  lpbyWorkBuffer,
        LPTSTR                  lpszFileExt,
        LPBOOL                  lpbInfectable
    );

    EXTSTATUS (FAR WINAPI *EXTStartUpIsInfectable)
    (
        LPCALLBACKREV2                          lpstCallBacks,
        LPTSTR                                  lpszNAVEXDataDir,
        LPTSTR                                  lpszNAVEXINIFile
    );

    EXTSTATUS (FAR WINAPI *EXTShutDownIsInfectable)
    (
        LPCALLBACKREV2                          lpstCallBacks
    );

} EXPORT15_ISI_TABLE_TYPE, FAR *PEXPORT15_ISI_TABLE_TYPE;

#if !defined(SYM_NLM) || !defined(AVAPI_INCLUDE)

EXTSTATUS FAR WINAPI NLOADDS EXTIsInfectableFile
(
    LPCALLBACKREV2          lpstCallBacks,
    HFILE                   hFile,
    LPBYTE                  lpbyWorkBuffer,
    LPTSTR                  lpszFileExt,
    LPBOOL                  lpbInfectable
);

EXTSTATUS FAR WINAPI NLOADDS EXTStartUpIsInfectable
(
    LPCALLBACKREV2                          lpstCallBacks,
    LPTSTR                                  lpszNAVEXDataDir,
    LPTSTR                                  lpszNAVEXINIFile
);

EXTSTATUS FAR WINAPI NLOADDS EXTShutDownIsInfectable
(
    LPCALLBACKREV2                          lpstCallBacks
);

#endif // !defined(SYM_NLM) || !defined(AVAPI_INCLUDE)

EXTSTATUS FAR WINAPI NLOADDS EXTQueryInterface
(
    DWORD       dwInterfaceID,
    LPLPVOID    lplpvExportTable
);

extern EXPORT15_ISI_TABLE_TYPE export_ISI_table;

#ifdef __cplusplus
}
#endif

// Wrappers of the external functions.
#if defined(AVAPI_INCLUDE)

inline EXTSTATUS FAR WINAPI EXTStartUpIsInfectable
(
    HVCONTEXT                               hContext,
    LPTSTR                                  a,
    LPTSTR                                  b
)
{
#if ( defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) || defined(SYM_DOSX)) && defined(__cplusplus)

    if (hContext->prExportIsInfectableTable != NULL)
    {
        return (((PEXPORT15_ISI_TABLE_TYPE)hContext->prExportIsInfectableTable)->EXTStartUpIsInfectable(
                    CONTEXT_CALL_BACK, a, b));
    }

#endif

    return EXTSTATUS_OK;
}

inline EXTSTATUS FAR WINAPI EXTShutDownIsInfectable
(
    HVCONTEXT                               hContext
)
{
#if ( defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) || defined(SYM_DOSX)) && defined(__cplusplus)

    if (hContext->prExportIsInfectableTable != NULL)
    {
        return (((PEXPORT15_ISI_TABLE_TYPE)hContext->prExportIsInfectableTable)->EXTShutDownIsInfectable(
                    CONTEXT_CALL_BACK));
    }

#endif

    return EXTSTATUS_OK;
}

inline EXTSTATUS FAR WINAPI EXTIsInfectableFile
(
    HVCONTEXT       hContext,
    HFILE           a,
    LPBYTE          b,
    LPTSTR          c,
    LPBOOL          d
)
{

#if ( defined(SYM_VXD) || defined(SYM_NTK) || defined(SYM_WIN) || defined(SYM_DOSX)) && defined(__cplusplus)

    if (hContext->prExportIsInfectableTable != NULL)
    {
        return (((PEXPORT15_ISI_TABLE_TYPE)hContext->prExportIsInfectableTable)->EXTIsInfectableFile(
                    CONTEXT_CALL_BACK, a, b, c, d));
    }

#endif

    return EXTSTATUS_OK;
}

// Prototypes of functions for communicating with the VxD containing
// external AV functions.  These functions will be available only when
// AVAPI.H is included (this means, the wrappers of the external functions,
// the ones that transfer control to the external functions.)

EXTSTATUS WINAPI EXTInitIsInfectable(HVCONTEXT hContext);

EXTSTATUS WINAPI EXTCloseIsInfectable(HVCONTEXT hContext);

#endif  // #if defined(AVAPI_INCLUDE)

#endif  // #ifdef __NAVEXISI_H