#include "platform.h"
#include "arclib.h"
#include "pkarc.h"
#include "symzip.h"
#include "mntr.h"

AL_PROTO MyMonitor::MyMonitor(LPUNZIP_STRUCT lpUnZipStruct)
    : ALMonitor( AL_MONITOR_OBJECTS )
{
    lpCallBack = lpUnZipStruct->lpCallback;
    dwUserData = lpUnZipStruct->dwUserData;     // Really an LPNONAV &!
    bAbort = FALSE;
}


void AL_PROTO MyMonitor::ArchiveOperation( ALArchiveOperation operation,
                                           ALArchive *,
                                           ALEntry *entry )
{
    UINT    uRet;
    switch ( operation ) {
        case AL_EXTRACTION_OPEN :
                                        // Unused by ZipCallback in scanutil.c
            if (lpCallBack)
            {
                ZipRecord.lpszBuffer = entry->mpStorageObject->mName;

                (lpCallBack)(ZIP_PROGRESS_INIT, 0, &ZipRecord, dwUserData);
            }
            break;
        case AL_EXTRACTION_CLOSE :
            if (lpCallBack)
                uRet = (lpCallBack)(ZIP_EXTRACTED, 0, &ZipRecord, dwUserData);
                                        // Check for user abort scan.
            if (uRet == ZIP_ABORT)
                bAbort = TRUE;
            break;
        default :
            break;
    }
}
