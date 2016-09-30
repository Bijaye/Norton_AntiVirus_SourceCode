#pragma once

#include "apw.h"
#include "apwntcmd.h"
#include "VirusAlertQueue.h"        // Need this for specific access to the functions

#include "serviceconfig.h"  // For sharing the service's options
#include "Scanner.h"        // Scanner class shared by the Agent and the Service

class CShutdownScan : public ccLib::CThread
{
public:
    CShutdownScan(CApwNtCmd* pCmd);
    ~CShutdownScan(void);

    int Run (); // override of the thread, starts the scan

protected:
    AP_STATE getStatusOfDriver();
    CApwNtCmd* m_pCmd;
    CServiceConfig m_config;        // We share the options that the service uses so we're in sync.
    CScanner*      m_pScanner;

    bool ScanFirstFloppy();
    bool scanBootRecord( char chDrive, bool &bRepaired );
    void reportVirus( char chDrive, IScanResults* pResults );
    TCHAR findFirstFloppy();
    DWORD getActiveSessionID();

private:
    CShutdownScan(){}; // not allowed
};
