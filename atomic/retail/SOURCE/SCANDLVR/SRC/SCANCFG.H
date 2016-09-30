
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/Scancfg.h_v   1.14   18 Jun 1998 21:30:46   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// ScanCfg.h interface for the CQuarantineOptions class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/Scancfg.h_v  $
// 
//    Rev 1.14   18 Jun 1998 21:30:46   SEDWARD
// Added support for reading a custom corporate email address from the
// registry (see Atomic bug #116913); added support for a custom corporate
// priority code to be read from the registry (see the same bug).
//
//    Rev 1.13   14 Jun 1998 18:26:52   SEDWARD
// Removed Nav version code (no longer needed), added support to distinguish
// between the NAV corporate features value (found under the NAV regkey) and
// the AdmTools corporate features value (found under the Quarantine regkey).
//
//    Rev 1.12   08 Jun 1998 16:21:08   SEDWARD
// Added functions to track the number of platform IDs.
//
//    Rev 1.11   01 Jun 1998 17:05:00   SEDWARD
// Added country code structure members and access functions.
//
//    Rev 1.10   28 May 1998 13:59:36   SEDWARD
// We now handle the 'custom SMTP server' checkbox independently.
//
//    Rev 1.9   26 May 1998 13:02:34   SEDWARD
// Made sure _tcsncpy calls in Get/Set functions result in null-terminated
// strings.
//
//    Rev 1.8   27 Apr 1998 18:14:22   SEDWARD
// Changed arguments for the "Set" functions to LPCTSTR; this allows us to pass
// CString arguments directly.
//
//    Rev 1.7   26 Apr 1998 17:27:58   SEDWARD
// Added 'IsCorporateMode()'.
//
//    Rev 1.6   25 Apr 1998 19:02:14   SEDWARD
// Added support for SARC email and socket server stuff.
//
//    Rev 1.5   24 Apr 1998 18:19:32   SEDWARD
// -- strip file content is now a BOOL
// -- added 'GetCorporateMode'
// -- added support for 'hiding' the default SMTP server from the user
// -- checkin of some work-in-progress (SARC email addresses)
//
//    Rev 1.4   20 Apr 1998 18:20:02   SEDWARD
// Removed 'virus defs' functions.
//
//    Rev 1.3   20 Apr 1998 18:06:38   SEDWARD
// Added support for new UI changes.
//
//    Rev 1.0   06 Apr 1998 18:46:36   SEDWARD
// Initial revision.
//
//    Rev 1.0   06 Apr 1998 18:40:28   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef SCAN_CONFIGURATION_HEADER
#define SCAN_CONFIGURATION_HEADER


// general define statements
#define MAX_CONFIG_FILENAME_LEN             260
#define MAX_INI_BUF_SIZE                    2048
#define INI_DEFAULT_STR                     "Default"
#define MAX_NAME_LEN                        50
#define MAX_ADDRESS_LEN                     100
#define MAX_CITY_LEN                        50
#define MAX_COUNTRY_LEN                     50
#define MAX_FAX_LEN                         25
#define MAX_PHONE_LEN                       25
#define MAX_STATE_LEN                       25
#define MAX_EMAIL_LEN                       75
#define MAX_ZIPCODE_LEN                     15
#define MAX_OPERATING_SYSTEM_LEN            25
#define MAX_PRODUCT_LANGUAGE_LEN            25
#define MAX_DEFS_DATE_LEN                   25
#define MAX_COMPANY_LEN                     75
#define MAX_ATLAS_NUMBER_LEN                25
#define MAX_CORPORATE_SUPPORT_NUMBER_LEN    25
#define MAX_SMTP_SERVER_LEN                 75
#define MAX_SOCKET_SERVER_LEN               75
#define MAX_STATE_OF_INFECTION_LEN          25
#define MAX_SYMPTOMS_LEN                    2048
#define FALSE_STRING                        _T("FALSE")
#define TRUE_STRING                         _T("TRUE")
#define SARC_MUNGE_VALUE                    0x3B
#define SCAN_CONFIG_DAT_FILENAME            _T("ScanCfg.dat")
#define REGKEY_HKLM_SW_NAV                  "SOFTWARE\\Symantec\\Norton AntiVirus"
#define REGKEY_HKLM_SW_NAV_QUARANTINE       "SOFTWARE\\Symantec\\Norton AntiVirus\\Quarantine"
#define REGVAL_CORPORATE_FEATURES           "CorporateFeatures"
#define REGVAL_EMAIL_SERVER_ADDRESS         "EmailServerAddress"
#define REGVAL_EMAIL_SERVER_NAME            "EmailServerName"
#define REGVAL_EMAIL_SERVER_PRIORITY        "EmailServerPriority"


// Scan and Deliver ("SD") error codes for configuration file manipulation
#define SD_CONFIG_ERROR_NAME                        0x00000001
#define SD_CONFIG_ERROR_ADDRESS                     0x00000002
#define SD_CONFIG_ERROR_CITY                        0x00000004
#define SD_CONFIG_ERROR_COUNTRY                     0x00000008
#define SD_CONFIG_ERROR_FAX                         0x00000010
#define SD_CONFIG_ERROR_PHONE                       0x00000020
#define SD_CONFIG_ERROR_STATE                       0x00000040
#define SD_CONFIG_ERROR_EMAIL                       0x00000080
#define SD_CONFIG_ERROR_ZIP_CODE                    0x00000100
#define SD_CONFIG_ERROR_NAV_VERSION                 0x00000200
#define SD_CONFIG_ERROR_OS                          0x00000400
#define SD_CONFIG_ERROR_PRODUCT_LANGUAGE            0x00000800
#define SD_CONFIG_ERROR_DEFS_DATE                   0x00001000
#define SD_CONFIG_ERROR_STRIP_FILE_CONTENT          0x00002000
#define SD_CONFIG_ERROR_COMPANY                     0x00004000
#define SD_CONFIG_ERROR_CORPORATE_SUPPORT_NUMBER    0x00008000
#define SD_CONFIG_ERROR_SMTP_SERVER                 0x00010000
#define SD_CONFIG_ERROR_COUNTRY_OF_INFECTION        0x00020000
#define SD_CONFIG_ERROR_STATE_OF_INFECTION          0x00040000
#define SD_CONFIG_ERROR_SYMPTOMS                    0x00080000
#define SD_CONFIG_ERROR_ATLAS_NUMBER                0x00010000
#define SD_CONFIG_ERROR_SARC_EMAIL_ADDRESS          0x00020000



// strings used to access the Scan and Deliver configuration data
#define SD_CONFIG_SECTION_USER_INFO                 "USER_INFO"
#define SD_CONFIG_SECTION_SYSTEM_INFO               "SYSTEM_INFO"
#define SD_CONFIG_SECTION_CORPORATE_INFO            "CORPORATE_INFO"
#define SD_CONFIG_SECTION_OS_INFO                   "OS_INFO"
#define SD_CONFIG_SECTION_VIRUS_INFO                "VIRUS_INFO"
#define SD_CONFIG_KEY_FIRST_NAME                    "First Name"
#define SD_CONFIG_KEY_LAST_NAME                     "Last Name"
#define SD_CONFIG_KEY_ADDRESS1                      "Address1"
#define SD_CONFIG_KEY_ADDRESS2                      "Address2"
#define SD_CONFIG_KEY_CITY                          "City"
#define SD_CONFIG_KEY_STATE                         "State"
#define SD_CONFIG_KEY_COUNTRY                       "Country"
#define SD_CONFIG_KEY_ZIPCODE                       "ZipCode"
#define SD_CONFIG_KEY_PHONE                         "Phone"
#define SD_CONFIG_KEY_FAX                           "Fax"
#define SD_CONFIG_KEY_EMAIL                         "Email"
#define SD_CONFIG_KEY_OPERATING_SYSTEM              "OperatingSystem"
#define SD_CONFIG_KEY_PRODUCT_LANGUAGE              "ProductLanguage"
#define SD_CONFIG_KEY_STRIP_FILE_CONTENT            "StripFileContent"
#define SD_CONFIG_KEY_COMPANY                       "Company"
#define SD_CONFIG_KEY_ATLAS_NUMBER                  "Atlas Number"
#define SD_CONFIG_KEY_SUPPORT_NUMBER                "SupportNumber"
#define SD_CONFIG_KEY_USE_CUSTOM_SMTP_SERVER        "Use Custom SmtpServer"
#define SD_CONFIG_KEY_SMTP_SERVER                   "SmtpServer"
#define SD_CONFIG_KEY_SYMPTOMS                      "Symptoms"


// default values used when generating a DAT file from scratch
#define SD_DEFAULT_USER_FIRST_NAME                  ""
#define SD_DEFAULT_USER_LAST_NAME                   ""
#define SD_DEFAULT_ADDRESS1                         ""
#define SD_DEFAULT_ADDRESS2                         ""
#define SD_DEFAULT_CITY                             ""
#define SD_DEFAULT_COUNTRY                          ""
#define SD_DEFAULT_FAX                              ""
#define SD_DEFAULT_PHONE                            ""
#define SD_DEFAULT_STATE                            ""
#define SD_DEFAULT_ZIP_CODE                         ""
#define SD_DEFAULT_ATLAS_NUMBER                     ""
#define SD_DEFAULT_EMAIL                            ""
#define SD_DEFAULT_NAV_VER20                        "FALSE"
#define SD_DEFAULT_NAV_VER40                        "FALSE"
#define SD_DEFAULT_NAV_VER50                        "FALSE"
#define SD_DEFAULT_OS_WIN31                         "FALSE"
#define SD_DEFAULT_OS_UNIX                          "FALSE"
#define SD_DEFAULT_OS_ALPHA                         "FALSE"
#define SD_DEFAULT_OS_NLM                           "FALSE"
#define SD_DEFAULT_PRODUCT_LANGUAGE                 ""
#define SD_DEFAULT_STRIP_FILE_CONTENT               "FALSE"
#define SD_DEFAULT_COMPANY                          ""
#define SD_DEFAULT_SUPPORT_NUMBER                   ""
#define SD_DEFAULT_SMTP_SERVER                      "Default SMTP Server"
#define SD_DEFAULT_SYMPTOMS                         ""


// buffer for Scan and Deliver configuration
typedef struct  tagSCAN_CONFIG_STRUCT
{
    char        szScanCfgFirstName[MAX_NAME_LEN];
    char        szScanCfgLastName[MAX_NAME_LEN];
    char        szScanCfgAddress1[MAX_ADDRESS_LEN];
    char        szScanCfgAddress2[MAX_ADDRESS_LEN];
    char        szScanCfgCity[MAX_CITY_LEN];
    char        szScanCfgCountry[MAX_COUNTRY_LEN];
    char        szScanCfgFax[MAX_FAX_LEN];
    char        szScanCfgPhone[MAX_PHONE_LEN];
    char        szScanCfgState[MAX_STATE_LEN];
    char        szScanCfgEmail[MAX_EMAIL_LEN];
    char        szScanCfgSarcEmail[MAX_EMAIL_LEN];
    int         nPriorityCode;
    char        szScanCfgSarcSocketServer[MAX_SOCKET_SERVER_LEN];
    char        szScanCfgZipCode[MAX_ZIPCODE_LEN];
    DWORD       dwNavVersion;
    DWORD       dwOperatingSystem;
    int         numPlatformIDs;
    char        szScanCfgProductLanguage[MAX_PRODUCT_LANGUAGE_LEN];
    BOOL        bScanCfgStripFileContent;
    char        szScanCfgCompany[MAX_COMPANY_LEN];
    char        szScanCfgAtlasNumber[MAX_ATLAS_NUMBER_LEN];
    char        szScanCfgCorporateSupportNumber[MAX_CORPORATE_SUPPORT_NUMBER_LEN];
    BOOL        bScanCfgUseCustomSmtpServer;
    char        szScanCfgSmtpServer[MAX_SMTP_SERVER_LEN];
    char        szScanCfgCountryOfInfection[MAX_COUNTRY_LEN];
    char        szScanCfgStateOfInfection[MAX_STATE_OF_INFECTION_LEN];
    char        szScanCfgSymptoms[MAX_SYMPTOMS_LEN];
    DWORD       dwInfectionCountryCode;
    DWORD       dwUserCountryCode;

} SCAN_CONFIG_STRUCT, FAR*  LPSCAN_CONFIG_STRUCT;


// class to manipulate Scan and Deliver options
class   CScanDeliverConfiguration
{
public:
    DWORD   GetCorporateMode(LPCTSTR  lpszRegKey = REGKEY_HKLM_SW_NAV);
    BOOL    IsCorporateMode(LPCTSTR  lpszRegKey = REGKEY_HKLM_SW_NAV);
    BOOL    Initialize(LPSTR  lpszFilePath, BOOL  bCreate = FALSE);
    BOOL    ReadConfigFile(DWORD*  dwErrorBits);
    BOOL    WriteConfigFile(DWORD*  dwErrorBits);
    BOOL    GetSmtpServerDisplayString(LPTSTR  szCharBuf, int  nBufSize);

    // public access functions
    void    GetFirstName(LPTSTR  szCharBuf, int  nBufSize);
    void    SetFirstName(LPCTSTR  szCharBuf);
    void    GetLastName(LPTSTR  szCharBuf, int  nBufSize);
    void    SetLastName(LPCTSTR  szCharBuf);
    void    GetAddress1(LPTSTR  szCharBuf, int  nBufSize);
    void    SetAddress1(LPCTSTR  szCharBuf);
    void    GetAddress2(LPTSTR  szCharBuf, int  nBufSize);
    void    SetAddress2(LPCTSTR  szCharBuf);
    void    GetCity(LPTSTR  szCharBuf, int  nBufSize);
    void    SetCity(LPCTSTR  szCharBuf);
    void    GetCountry(LPTSTR  szCharBuf, int  nBufSize);
    void    SetCountry(LPCTSTR  szCharBuf);
    void    GetFax(LPTSTR  szCharBuf, int  nBufSize);
    void    SetFax(LPCTSTR  szCharBuf);
    void    GetPhone(LPTSTR  szCharBuf, int  nBufSize);
    void    SetPhone(LPCTSTR  szCharBuf);
    void    GetState(LPTSTR  szCharBuf, int  nBufSize);
    void    SetState(LPCTSTR  szCharBuf);
    void    GetEmail(LPTSTR  szCharBuf, int  nBufSize);
    void    SetEmail(LPCTSTR  szCharBuf);
    void    GetZipCode(LPTSTR  szCharBuf, int  nBufSize);
    void    SetZipCode(LPCTSTR  szCharBuf);
    void    GetNavVersion(DWORD*  dwNavVer);
    void    SetNavVersion(DWORD  dwNavVer);
    void    GetOperatingSystem(DWORD*  dwOS);
    void    SetOperatingSystem(DWORD  dwOS);
    void    GetNumPlatforms(int*  numPlatforms);
    void    SetNumPlatforms(int  numPlatforms);
    void    GetProductLanguage(LPTSTR  szCharBuf, int  nBufSize);
    void    SetProductLanguage(LPCTSTR  szCharBuf);
    BOOL    GetStripFileContent(void);
    void    SetStripFileContent(BOOL  bValue);
    void    GetCompany(LPTSTR  szCharBuf, int  nBufSize);
    void    SetCompany(LPCTSTR  szCharBuf);
    void    GetCorporateSupportNumber(LPTSTR  szCharBuf, int  nBufSize);
    void    SetCorporateSupportNumber(LPCTSTR  szCharBuf);
    void    GetAtlasNumber(LPTSTR  szCharBuf, int  nBufSize);
    void    SetAtlasNumber(LPCTSTR  szCharBuf);
    BOOL    GetUseCustomSmtpServer(void);
    void    SetUseCustomSmtpServer(BOOL  bValue);
    void    GetSmtpServer(LPTSTR  szCharBuf, int  nBufSize);
    void    SetSmtpServer(LPCTSTR  szCharBuf);
    void    GetCountryOfInfection(LPTSTR  szCharBuf, int  nBufSize);
    void    SetCountryOfInfection(LPCTSTR  szCharBuf);
    void    GetStateOfInfection(LPTSTR  szCharBuf, int  nBufSize);
    void    SetStateOfInfection(LPCTSTR  szCharBuf);
    void    GetSymptoms(LPTSTR  szCharBuf, int  nBufSize);
    void    SetSymptoms(LPCTSTR  szCharBuf);
    void    GetSarcEmailAddress(LPTSTR  szCharBuf, int  nBufSize);  // read-only
    void    GetSarcSocketServer(LPTSTR  szCharBuf, int  nBufSize);  // read-only
    void    GetPriorityCode(int*  nCodePtr);
    void    SetPriorityCode(int  nCode);
    void    GetUserCountryCode(DWORD*  dwCountryCode);
    void    SetUserCountryCode(DWORD  dwCountryCode);
    void    GetInfectionCountryCode(DWORD*  dwCountryCode);
    void    SetInfectionCountryCode(DWORD  dwCountryCode);

    // Construction
    CScanDeliverConfiguration();
    CScanDeliverConfiguration(LPSTR  lpszFilePath, BOOL  bCreate);
    ~CScanDeliverConfiguration();

protected:
    // helper functions
    BOOL    CreateDefaultScanCfgDatFile(LPTSTR  lpszFileName);
    BOOL    ReadFirstName(void);
    BOOL    ReadLastName(void);
    BOOL    ReadAddress1(void);
    BOOL    ReadAddress2(void);
    BOOL    ReadCity(void);
    BOOL    ReadCountry(void);
    BOOL    ReadFax(void);
    BOOL    ReadPhone(void);
    BOOL    ReadState(void);
    BOOL    ReadEmail(void);
    BOOL    ReadZipCode(void);
    BOOL    ReadNavVersion(void);
    BOOL    ReadOperatingSystem(void);
    BOOL    ReadProductLanguage(void);
    BOOL    ReadStripFileContent(void);
    BOOL    ReadCompany(void);
    BOOL    ReadCorporateSupportNumber(void);
    BOOL    ReadAtlasNumber(void);
    BOOL    ReadUseCustomSmtpServer(void);
    BOOL    ReadSmtpServer(void);
    BOOL    ReadCountryOfInfection(void);
    BOOL    ReadStateOfInfection(void);
    BOOL    ReadSymptoms(void);
    void    ReadCustomSarcEmailAddress(CString&  szEmail, BOOL  bDecodeString = FALSE);
    void    ReadSarcPriorityCode(void);
    BOOL    ReadSarcEmailAddress(void);
    BOOL    ReadSarcSocketServer(void);
    BOOL    WriteFirstName(void);
    BOOL    WriteLastName(void);
    BOOL    WriteAddress1(void);
    BOOL    WriteAddress2(void);
    BOOL    WriteCity(void);
    BOOL    WriteCountry(void);
    BOOL    WriteFax(void);
    BOOL    WritePhone(void);
    BOOL    WriteState(void);
    BOOL    WriteEmail(void);
    BOOL    WriteZipCode(void);
    BOOL    WriteNavVersion(void);
    BOOL    WriteOperatingSystem(void);
    BOOL    WriteProductLanguage(void);
    BOOL    WriteStripFileContent(void);
    BOOL    WriteCompany(void);
    BOOL    WriteCorporateSupportNumber(void);
    BOOL    WriteAtlasNumber(void);
    BOOL    WriteUseCustomSmtpServer(void);
    BOOL    WriteSmtpServer(void);
    BOOL    WriteCountryOfInfection(void);
    BOOL    WriteStateOfInfection(void);
    BOOL    WriteSymptoms(void);

private:
    // methods
    BOOL    ConfigFileExists(LPSTR  lpszFileName);

    // data members
    SCAN_CONFIG_STRUCT  m_ConfigData;
    TCHAR               m_ConfigFilePath[MAX_CONFIG_FILENAME_LEN];
    TCHAR               m_szIniDataBuf[MAX_INI_BUF_SIZE];
};





// access functions
inline  void    CScanDeliverConfiguration::GetFirstName(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgFirstName, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetFirstName(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgFirstName, szCharBuf, MAX_NAME_LEN);
    m_ConfigData.szScanCfgFirstName[MAX_NAME_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetLastName(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgLastName, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetLastName(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgLastName, szCharBuf, MAX_NAME_LEN);
    m_ConfigData.szScanCfgLastName[MAX_NAME_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetAddress1(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgAddress1, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetAddress1(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgAddress1, szCharBuf, MAX_ADDRESS_LEN);
    m_ConfigData.szScanCfgAddress1[MAX_ADDRESS_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetAddress2(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgAddress2, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetAddress2(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgAddress2, szCharBuf, MAX_ADDRESS_LEN);
    m_ConfigData.szScanCfgAddress2[MAX_ADDRESS_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetCity(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgCity, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetCity(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgCity, szCharBuf, MAX_CITY_LEN);
    m_ConfigData.szScanCfgCity[MAX_CITY_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetCountry(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgCountry, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetCountry(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgCountry, szCharBuf, MAX_COUNTRY_LEN);
    m_ConfigData.szScanCfgCountry[MAX_COUNTRY_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetUserCountryCode(DWORD*  dwCountryCode)
{
    *dwCountryCode = m_ConfigData.dwUserCountryCode;
}

inline  void    CScanDeliverConfiguration::SetUserCountryCode(DWORD  dwCountryCode)
{
    m_ConfigData.dwUserCountryCode = dwCountryCode;
}

inline  void    CScanDeliverConfiguration::GetInfectionCountryCode(DWORD*  dwCountryCode)
{
    *dwCountryCode = m_ConfigData.dwInfectionCountryCode;
}

inline  void    CScanDeliverConfiguration::SetInfectionCountryCode(DWORD  dwCountryCode)
{
    m_ConfigData.dwInfectionCountryCode = dwCountryCode;
}

inline  void    CScanDeliverConfiguration::GetFax(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgFax, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetFax(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgFax, szCharBuf, MAX_FAX_LEN);
    m_ConfigData.szScanCfgFax[MAX_FAX_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetPhone(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgPhone, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetPhone(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgPhone, szCharBuf, MAX_PHONE_LEN);
    m_ConfigData.szScanCfgPhone[MAX_PHONE_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetState(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgState, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetState(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgState, szCharBuf, MAX_STATE_LEN);
    m_ConfigData.szScanCfgState[MAX_STATE_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetEmail(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgEmail, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetEmail(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgEmail, szCharBuf, MAX_EMAIL_LEN);
    m_ConfigData.szScanCfgEmail[MAX_EMAIL_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetZipCode(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgZipCode, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetZipCode(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgZipCode, szCharBuf, MAX_ZIPCODE_LEN);
    m_ConfigData.szScanCfgZipCode[MAX_ZIPCODE_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetNavVersion(DWORD*  dwNavVer)
{
    *dwNavVer = m_ConfigData.dwNavVersion;
}

inline  void    CScanDeliverConfiguration::SetNavVersion(DWORD  dwNavVer)
{
    m_ConfigData.dwNavVersion = dwNavVer;
}

inline  void    CScanDeliverConfiguration::GetOperatingSystem(DWORD*  dwOS)
{
    *dwOS = m_ConfigData.dwOperatingSystem;
}

inline  void    CScanDeliverConfiguration::SetOperatingSystem(DWORD  dwOS)
{
    m_ConfigData.dwOperatingSystem = dwOS;
}

inline  void    CScanDeliverConfiguration::GetNumPlatforms(int*  numPlatforms)
{
    *numPlatforms = m_ConfigData.numPlatformIDs;
}

inline  void    CScanDeliverConfiguration::SetNumPlatforms(int  numPlatforms)
{
    m_ConfigData.numPlatformIDs = numPlatforms;
}

inline  void    CScanDeliverConfiguration::GetProductLanguage(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgProductLanguage, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetProductLanguage(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgProductLanguage, szCharBuf, MAX_PRODUCT_LANGUAGE_LEN);
    m_ConfigData.szScanCfgProductLanguage[MAX_PRODUCT_LANGUAGE_LEN - 1] = NULL;
}

inline  BOOL    CScanDeliverConfiguration::GetStripFileContent(void)
{
    return (m_ConfigData.bScanCfgStripFileContent);
}

inline  void    CScanDeliverConfiguration::SetStripFileContent(BOOL  bValue)
{
    m_ConfigData.bScanCfgStripFileContent = bValue;
}

inline  void    CScanDeliverConfiguration::GetCompany(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgCompany, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetCompany(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgCompany, szCharBuf, MAX_COMPANY_LEN);
    m_ConfigData.szScanCfgCompany[MAX_COMPANY_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetCorporateSupportNumber(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgCorporateSupportNumber, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetCorporateSupportNumber(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgCorporateSupportNumber, szCharBuf, MAX_CORPORATE_SUPPORT_NUMBER_LEN);
    m_ConfigData.szScanCfgCorporateSupportNumber[MAX_CORPORATE_SUPPORT_NUMBER_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetAtlasNumber(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgAtlasNumber, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetAtlasNumber(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgAtlasNumber, szCharBuf, MAX_ATLAS_NUMBER_LEN);
    m_ConfigData.szScanCfgAtlasNumber[MAX_ATLAS_NUMBER_LEN - 1] = NULL;
}

inline  BOOL    CScanDeliverConfiguration::GetUseCustomSmtpServer(void)
{
    return (m_ConfigData.bScanCfgUseCustomSmtpServer);
}

inline  void    CScanDeliverConfiguration::SetUseCustomSmtpServer(BOOL  bValue)
{
    m_ConfigData.bScanCfgUseCustomSmtpServer = bValue;
}

inline  void    CScanDeliverConfiguration::GetSmtpServer(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgSmtpServer, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetSmtpServer(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgSmtpServer, szCharBuf, MAX_SMTP_SERVER_LEN);
    m_ConfigData.szScanCfgSmtpServer[MAX_SMTP_SERVER_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetCountryOfInfection(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgCountryOfInfection, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetCountryOfInfection(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgCountryOfInfection, szCharBuf, MAX_COUNTRY_LEN);
    m_ConfigData.szScanCfgCountryOfInfection[MAX_COUNTRY_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetStateOfInfection(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgStateOfInfection, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetStateOfInfection(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgStateOfInfection, szCharBuf, MAX_STATE_OF_INFECTION_LEN);
    m_ConfigData.szScanCfgStateOfInfection[MAX_STATE_OF_INFECTION_LEN - 1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetSymptoms(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgSymptoms, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::SetSymptoms(LPCTSTR  szCharBuf)
{
    _tcsncpy(m_ConfigData.szScanCfgSymptoms, szCharBuf, MAX_SYMPTOMS_LEN);
    m_ConfigData.szScanCfgSymptoms[MAX_SYMPTOMS_LEN - 1] = NULL;
}

// NOTE: there is no "set" function for this item, it's read and decrypted from
// the string table
inline  void    CScanDeliverConfiguration::GetSarcEmailAddress(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgSarcEmail, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

// NOTE: there is no "set" function for this item, it's read and decrypted from
// the string table
inline  void    CScanDeliverConfiguration::GetSarcSocketServer(LPTSTR  szCharBuf, int  nBufSize)
{
    _tcsncpy(szCharBuf, m_ConfigData.szScanCfgSarcSocketServer, nBufSize);
    szCharBuf[nBufSize-1] = NULL;
}

inline  void    CScanDeliverConfiguration::GetPriorityCode(int*  nCodePtr)
{
    *nCodePtr = m_ConfigData.nPriorityCode;
}

inline  void    CScanDeliverConfiguration::SetPriorityCode(int  nCode)
{

    m_ConfigData.nPriorityCode = nCode;
}


#endif  // SCAN_CONFIGURATION_HEADER