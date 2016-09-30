#pragma once

#include "ccScanInterface.h"

class CEraserScanSink :
    public IScanSink2,
    public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:
    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(IID_ScanSink, IScanSink)
        SYM_INTERFACE_ENTRY(IID_ScanSink2, IScanSink2)
    SYM_INTERFACE_MAP_END()

    // Constructor.
    CEraserScanSink();

    // Destructor
    virtual ~CEraserScanSink();

    // IScanSink overrides.
    virtual SCANSTATUS OnBusy();

    virtual SCANSTATUS OnError(IScanError* pError);

    virtual SCANSTATUS OnNewItem(const char* pszItem);

    virtual SCANSTATUS OnNewFile(const char* pszLongName, const char* pszShortName);
    virtual SCANSTATUS OnNewDirectory(const char* pszLongName, const char* pszShortName);

    virtual SCANSTATUS OnInfectionFound(IScanInfection* pInfection);

    virtual SCANSTATUS OnRepairFailed(IScanInfection* pInfection);

    virtual SCANSTATUS OnRemoveMimeComponent(IScanInfection* pInfection, char* pszMessage,
        int iMessageBufferSize);

    virtual SCANSTATUS LockVolume(const char* szVolume, bool bLock);

    virtual SCANSTATUS OnCleanFile(const char* pszFileName, const char* pszTempFileName);

    // IScanSink2 overrides
    virtual SCANSTATUS OnBeginThread();

    virtual SCANSTATUS OnEndThread();
};
