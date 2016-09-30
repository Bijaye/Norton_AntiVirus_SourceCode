#include "StdAfx.h"
#include "EraserScanSink.h"

// Constructor.
CEraserScanSink::CEraserScanSink()
{
}

// Destructor
CEraserScanSink::~CEraserScanSink()
{
}

// IScanSink overrides.
SCANSTATUS CEraserScanSink::OnBusy()
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnError(IScanError* pError)
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnNewItem(const char* pszItem)
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnNewFile(const char* pszLongName, const char* pszShortName)
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnNewDirectory(const char* pszLongName, const char* pszShortName)
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnInfectionFound(IScanInfection* pInfection)
{
    return SCAN_FALSE;
}

SCANSTATUS CEraserScanSink::OnRepairFailed(IScanInfection* pInfection)
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnRemoveMimeComponent(IScanInfection* pInfection, char* pszMessage,
    int iMessageBufferSize)
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::LockVolume(const char* szVolume, bool bLock)
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnCleanFile(const char* pszFileName, const char* pszTempFileName)
{
    return SCAN_OK;
}

// IScanSink2 overrides
SCANSTATUS CEraserScanSink::OnBeginThread()
{
    return SCAN_OK;
}

SCANSTATUS CEraserScanSink::OnEndThread()
{
    return SCAN_OK;
}
