
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QUAR32/VCS/QuarOpts.h_v   1.8   21 Apr 1998 14:25:50   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// QuarOpts.h interface for the CQuarantineOptions class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QUAR32/VCS/QuarOpts.h_v  $
// 
//    Rev 1.8   21 Apr 1998 14:25:50   SEDWARD
// Put the 'scan at startup' stuff back in; note that this really refers to
// the option to be prompted for a scan at startup if newer defs are found.
//
//    Rev 1.7   20 Apr 1998 22:28:00   SEDWARD
// Removed the 'scan at startup' stuff.
//
//    Rev 1.6   07 Apr 1998 22:36:50   SEDWARD
// Added DEFAULT_ values, CreateDefaultQuarOptsDatFile().
//
//    Rev 1.5   06 Apr 1998 19:25:22   JTaylor
// Added error codes to options.  Made initialize load the options, fixed an options check to see if options file exists to not return true if it is a directory.  Added Quarantine path validation.  Fixed header style for Quaropts.
//
//    Rev 1.4   01 Apr 1998 12:04:08   SEDWARD
// Added 'password enabled' functions.
//
//    Rev 1.3   31 Mar 1998 14:17:56   SEDWARD
// Increased password buffer length to 32 characters.
//
//    Rev 1.2   30 Mar 1998 19:50:56   SEDWARD
// Added password stuff.
//
//    Rev 1.1   16 Mar 1998 18:58:24   SEDWARD
// Changed include from 'Quar32Flags.h' to 'Quar32.h'.
//
//    Rev 1.0   16 Mar 1998 18:50:14   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef QUARANTINE_OPTS_HEADER
#define QUARANTINE_OPTS_HEADER

#include "Quar32.h"
#include "QuarAdd.h"
#include "IQuaran.h"


// general define statements
#define INI_DEFAULT_STR                 "DefaultString"
#define MAX_INI_BUF_SIZE                512
#define MAX_VALID_QFILEOPS_MEMBERS      5
#define MAX_QUAR_PASSWORD_LEN           32


// default values for when we create a new DAT file from scratch
#define DEFAULT_PASSWORD                "GUEST"
#define DEFAULT_SCAN_AT_STARTUP         "FALSE"
#define DEFAULT_FORWARDING_ENABLED      "FALSE"
#define DEFAULT_SCAN_REPAIRED_FILES     "FALSE"
#define DEFAULT_SARC_SUBMISSION_ENABLED "TRUE"
#define DEFAULT_SARC_QUARANTINE_SERVER  "DEFAULT QUARANTINE SERVER"
#define DEFAULT_FORWARD_SUBMITTED       "FALSE"
#define DEFAULT_FORWARD_REPAIRED        "FALSE"
#define DEFAULT_FORWARD_QUARANTINED     "TRUE"
#define DEFAULT_QUARANTINED_DELETE      "FALSE"
#define DEFAULT_QUARANTINED_SCAN        "TRUE"
#define DEFAULT_QUARANTINED_REPAIR      "TRUE"
#define DEFAULT_QUARANTINED_PUT_BACK    "TRUE"
#define DEFAULT_QUARANTINED_SUBMIT      "TRUE"
#define DEFAULT_SUBMITTED_DELETE        "TRUE"
#define DEFAULT_SUBMITTED_SCAN          "FALSE"
#define DEFAULT_SUBMITTED_REPAIR        "TRUE"
#define DEFAULT_SUBMITTED_PUT_BACK      "TRUE"
#define DEFAULT_SUBMITTED_SUBMIT        "FALSE"
#define DEFAULT_REPAIRED_DELETE         "TRUE"
#define DEFAULT_REPAIRED_SCAN           "FALSE"
#define DEFAULT_REPAIRED_REPAIR         "FALSE"
#define DEFAULT_REPAIRED_PUT_BACK       "TRUE"
#define DEFAULT_REPAIRED_SUBMIT         "FALSE"




// structure to store Quarantine options
typedef struct  tagQUAR_DAT_STRUCT
{
    BOOL        bScanAtStartup;                                         // default = TRUE
    BOOL        bForwardingEnabled;                                     // default = FALSE
    BOOL        bScanRepairedFiles;                                     // default = FALSE
    BOOL        bSarcSubmissionEnabled;                                 // default = TRUE
    char        szQuarantineServer[MAX_QUARANTINE_MACHINE_NAME_LEN];    // default = blank
    DWORD       dwValidForwardFileTypes;                                // default = quarantined and submitted
    DWORD       dwAllowableFileOperations[MAX_VALID_QFILEOPS_MEMBERS];
    char        szPassword[MAX_QUAR_PASSWORD_LEN];
    BOOL        bPasswordEnabled;                                       // default = FALSE

} QUAR_OPTS_STRUCT, FAR*  LPQUAR_OPTS_STRUCT;


// class to manipulate Quarantine options
class   CQuarantineOptions : public IQuarantineOpts
{
public:
    // IUnknown
    STDMETHOD( QueryInterface(REFIID, void**) );
    STDMETHOD_( ULONG, AddRef() );
    STDMETHOD_( ULONG, Release() );

    STDMETHOD( Initialize(LPSTR lpszFileName, BOOL  bCreate) );
    STDMETHOD( ReadQuarOptsFile() );
    STDMETHOD( WriteQuarOptsFile() );

    // simple access functions
    STDMETHOD( GetScanAtStartup(BOOL*  bValue) );
    STDMETHOD( SetScanAtStartup(BOOL  bValue) );
    STDMETHOD( GetForwardingEnabled(BOOL*  bValue) );
    STDMETHOD( SetForwardingEnabled(BOOL  bValue) );
    STDMETHOD( GetScanRepairedFiles(BOOL*  bValue) );
    STDMETHOD( SetScanRepairedFiles(BOOL  bValue) );
    STDMETHOD( GetSarcSubmissionEnabled(BOOL*  bValue) );
    STDMETHOD( SetSarcSubmissionEnabled(BOOL  bValue) );
    STDMETHOD( GetQuarantineServer(LPTSTR  szCharBuf, int  nBufSize) );
    STDMETHOD( SetQuarantineServer(LPTSTR  szCharBuf) );
    STDMETHOD( GetValidForwardTypes(DWORD*  dwValidTypes) );
    STDMETHOD( SetValidForwardTypes(DWORD  dwValidTypes) );
    STDMETHOD( GetValidFileOperations(DWORD*  dwFileOps, int  nIndex) );
    STDMETHOD( SetValidFileOperations(DWORD  dwFileOps, int  nIndex) );
    STDMETHOD( GetPassword(LPTSTR  szCharBuf, int  nBufSize) );
    STDMETHOD( SetPassword(LPTSTR  szCharBuf) );
    STDMETHOD( GetPasswordEnabled(BOOL*  bValue) );
    STDMETHOD( SetPasswordEnabled(BOOL  bValue) );


    // Construction
    CQuarantineOptions();
    ~CQuarantineOptions();


protected:
    BOOL    CreateDefaultQuarOptsDatFile(LPTSTR  lpszFileName);


private:
    // methods
    BOOL    QuarOptsFileExists(LPTSTR  lpszQuarOptsPath);
    BOOL    ReadScanAtStartup(void);
    BOOL    WriteScanAtStartup(void);
    BOOL    ReadForwardingEnabled(void);
    BOOL    WriteForwardingEnabled(void);
    BOOL    ReadScanRepairedFiles(void);
    BOOL    WriteScanRepairedFiles(void);
    BOOL    ReadSarcSubmissionEnabled(void);
    BOOL    WriteSarcSubmissionEnabled(void);
    BOOL    ReadQuarantineServer(void);
    BOOL    WriteQuarantineServer(void);
    BOOL    ReadValidForwardTypes(void);
    BOOL    WriteValidForwardTypes(void);
    BOOL    ReadValidFileOperations(DWORD  dwTargetFileStatus);
    BOOL    WriteValidFileOperations(DWORD  dwTargetFileStatus);
    BOOL    ReadPassword(void);
    BOOL    WritePassword(void);
    BOOL    ReadPasswordEnabled(void);
    BOOL    WritePasswordEnabled(void);

    // data members
    BOOL                m_bInitialized;
    DWORD               m_dwRef;
    QUAR_OPTS_STRUCT    m_QuarOpts;
    TCHAR               m_QuarOptsPath[MAX_QUARANTINE_FILENAME_LEN];
    TCHAR               m_szIniDataBuf[MAX_INI_BUF_SIZE];
};


#endif  // QUARANTINE_OPTS_HEADER