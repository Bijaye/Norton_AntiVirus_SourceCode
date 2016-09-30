#pragma once

#include "ccScanInterface.h"

class CScanPause
{
public:

    CScanPause(IScanner4* pScanner)
    {
        m_spScanner = pScanner;

        if( m_spScanner )
        {
            CCTRACEI(_T("CScanPause::CScanPause() - Pausing the scanning threads."));
            SCANSTATUS status = m_spScanner->PauseAllScans();
            CCTRACEI(_T("CScanPause::CScanPause() - Paused the scanning threads. Return value = 0x%X"), status);
        }
        else
            CCTRACEE(_T("CScanPause::CScanPause() - Invalid scanner object."));
    }

    ~CScanPause(void)
    {
        if( m_spScanner )
        {
            CCTRACEI(_T("CScanPause::~CScanPause() - Resuming the scanning threads."));
            SCANSTATUS status = m_spScanner->ResumeAllScans();
            CCTRACEI(_T("CScanPause::~CScanPause() - Resumed the scanning threads. Return value = 0x%X"), status);
        }
        else
            CCTRACEE(_T("CScanPause::~CScanPause() - Invalid scanner object."));
    }

private:
    IScanner4Ptr m_spScanner;

    // Disallowed
    CScanPause::CScanPause();
};
