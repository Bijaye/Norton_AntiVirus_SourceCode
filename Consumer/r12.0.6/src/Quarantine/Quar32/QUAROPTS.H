//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// QuarOpts.h interface for the CQuarantineOptions class.
//

#ifndef QUARANTINE_OPTS_HEADER
#define QUARANTINE_OPTS_HEADER

#include "Quar32.h"

// Taken from QuarAdd_cc.h
#include "..\..\SDKs\AutoProtect\include\apquar.h"
#include "avtypes.h"

#include "IQuaran.h"
#include "ccCriticalSection.h"


// general define statements
#define INI_DEFAULT_STR                 "DefaultString"
#define MAX_INI_BUF_SIZE                512
#define MAX_VALID_QFILEOPS_MEMBERS      5


// default values for when we create a new DAT file from scratch
#define DEFAULT_SCAN_AT_STARTUP         "TRUE"
#define DEFAULT_QUARANTINED_DELETE      "TRUE"
#define DEFAULT_QUARANTINED_SCAN        "TRUE"
#define DEFAULT_QUARANTINED_REPAIR      "TRUE"
#define DEFAULT_QUARANTINED_PUT_BACK    "TRUE"
#define DEFAULT_QUARANTINED_SUBMIT      "TRUE"
#define DEFAULT_SUBMITTED_DELETE        "TRUE"
#define DEFAULT_SUBMITTED_SCAN          "TRUE"
#define DEFAULT_SUBMITTED_REPAIR        "TRUE"
#define DEFAULT_SUBMITTED_PUT_BACK      "TRUE"
#define DEFAULT_SUBMITTED_SUBMIT        "FALSE"
#define DEFAULT_REPAIRED_DELETE         "TRUE"
#define DEFAULT_REPAIRED_SCAN           "TRUE"
#define DEFAULT_REPAIRED_REPAIR         "FALSE"
#define DEFAULT_REPAIRED_PUT_BACK       "TRUE"
#define DEFAULT_REPAIRED_SUBMIT         "FALSE"
#define DEFAULT_THREAT_BACKUP_DELETE      "TRUE"
#define DEFAULT_THREAT_BACKUP_SCAN        "FALSE"
#define DEFAULT_THREAT_BACKUP_REPAIR      "FALSE"
#define DEFAULT_THREAT_BACKUP_PUT_BACK    "TRUE"
#define DEFAULT_THREAT_BACKUP_SUBMIT      "TRUE"


// structure to store Quarantine options
typedef struct tagQUAR_DAT_STRUCT
{
    BOOL  bScanAtStartup;                                        // default = TRUE
    DWORD dwAllowableFileOperations[MAX_VALID_QFILEOPS_MEMBERS];
} QUAR_OPTS_STRUCT, FAR* LPQUAR_OPTS_STRUCT;


// class to manipulate Quarantine options
class CQuarantineOptions : public IQuarantineOpts,
                           public ISymBaseImpl<CSymThreadSafeRefCount>
{
public:

    SYM_INTERFACE_MAP_BEGIN()
        SYM_INTERFACE_ENTRY(IID_QuarantineOpts, IQuarantineOpts)
	SYM_INTERFACE_MAP_END()

    STDMETHOD(Initialize(LPSTR lpszFileName, BOOL bCreate = FALSE));
    STDMETHOD(ReadQuarOptsFile());
    STDMETHOD(WriteQuarOptsFile());

    // simple access functions
    STDMETHOD(GetScanAtStartup(BOOL* bValue));
    STDMETHOD(SetScanAtStartup(BOOL bValue));
    STDMETHOD(GetValidFileOperations(DWORD* dwFileOps, int nIndex));
    STDMETHOD(SetValidFileOperations(DWORD  dwFileOps, int nIndex));

    // Construction
    CQuarantineOptions();
    ~CQuarantineOptions();

protected:
    BOOL    CreateDefaultQuarOptsDatFile(LPTSTR lpszFileName);

private:
    // methods
    BOOL QuarOptsFileExists(LPTSTR lpszQuarOptsPath);
    BOOL ReadScanAtStartup(void);
    BOOL WriteScanAtStartup(void);
    BOOL ReadValidFileOperations(DWORD dwTargetFileStatus);
    BOOL WriteValidFileOperations(DWORD dwTargetFileStatus);

    // data members
    bool             m_bInitialized;
    QUAR_OPTS_STRUCT m_QuarOpts;
    TCHAR            m_QuarOptsPath[MAX_QUARANTINE_FILENAME_LEN];
    TCHAR            m_szIniDataBuf[MAX_INI_BUF_SIZE];

    // Synchronization object to make calls to this object thread safe
    ccLib::CCriticalSection m_critSec;
};


#endif  // QUARANTINE_OPTS_HEADER