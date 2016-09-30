
/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/Scancfg.cpv   1.14   18 Jun 1998 21:29:40   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// ScanCfg.cpp: implementation of the CQuarantineOptions class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/Scancfg.cpv  $
// 
//    Rev 1.14   18 Jun 1998 21:29:40   SEDWARD
// Added support for reading a custom corporate email address from the
// registry (see Atomic bug #116913); added support for a custom corporate
// priority code to be read from the registry (see the same bug).
//
//    Rev 1.13   14 Jun 1998 18:25:48   SEDWARD
// Removed Nav version code (no longer needed), added support to distinguish
// between the NAV corporate features value (found under the NAV regkey) and
// the AdmTools corporate features value (found under the Quarantine regkey).
//
//    Rev 1.12   08 Jun 1998 16:20:30   SEDWARD
// Changed Read/Write of operating system to support the external PLATFORM.DAT
// file.
//
//    Rev 1.11   03 Jun 1998 14:25:26   jtaylor
// Fixed a FindResource call to allow it to work with our resource DLL.
//
//    Rev 1.10   28 May 1998 13:59:14   SEDWARD
// We now handle the 'custom SMTP server' checkbox independently.
//
//    Rev 1.9   25 May 1998 13:51:36   SEDWARD
// Fixed return value bug in ReadSarcSocketServer().
//
//    Rev 1.8   18 May 1998 18:35:40   SEDWARD
// ReadSarcSocketServer() now reads binary resources.
//
//    Rev 1.7   14 May 1998 16:21:36   jtaylor
// Fixed KEY_ALL_ACCESS bug.
//
//    Rev 1.6   27 Apr 1998 15:03:46   jtaylor
// Improved functionality and error handling on ReadSmtpServer function.
//
//    Rev 1.5   26 Apr 1998 17:28:06   SEDWARD
// Added 'IsCorporateMode()'.
//
//    Rev 1.4   25 Apr 1998 19:08:14   SEDWARD
// Added support for SARC email and socket server stuff.
//
//    Rev 1.3   24 Apr 1998 18:18:26   SEDWARD
// -- strip file content is now a BOOL
// -- added 'GetCorporateMode'
// -- added support for 'hiding' the default SMTP server from the user
// -- checkin of some work-in-progress (SARC email addresses)
//
//    Rev 1.2   20 Apr 1998 18:25:24   SEDWARD
// Removed more virus defs stuff.
//
//    Rev 1.1   20 Apr 1998 18:05:40   SEDWARD
// Added support for the new UI.
//
//    Rev 1.0   06 Apr 1998 18:24:18   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "ScanDlvr.h"
#include "ScanCfg.h"
#include "TChar.h"
#include "Global.h"
#include "SdSocket.h"

#define INITGUID
#include "IScanDeliver.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


// ==== CScanDeliverConfiguration::CScanDeliverConfiguration ==============
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

CScanDeliverConfiguration::CScanDeliverConfiguration(void)
{
    memset((void*) &m_ConfigData, '\0', sizeof(m_ConfigData));
}



// ==== CScanDeliverConfiguration::CScanDeliverConfiguration ==============
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

CScanDeliverConfiguration::CScanDeliverConfiguration(LPSTR  lpszFilePath
                                                        , BOOL  bCreate)
{
    memset((void*) &m_ConfigData, '\0', sizeof(m_ConfigData));

    // now use the constructor args to complete initialization
    if ((NULL != lpszFilePath)  &&  (NULL != *lpszFilePath))
        {
        Initialize(lpszFilePath, bCreate);
        }
}



// ==== CScanDeliverConfiguration::~CScanDeliverConfiguration =============
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

CScanDeliverConfiguration::~CScanDeliverConfiguration(void)
{
}










// ==== Initialize ========================================================
//
//  This function is used to initialize the Scan and Deliver configuration
//  data object.
//
//  Input:
//      lpszFileName    -- a fully-qualified path to the configuration file
//      bCreate         -- TRUE means that if the configuration file does not
//                         exist, a default copy will be generated
//
//  Output:
//      TRUE if all goes well, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::Initialize(LPSTR  lpszFileName, BOOL  bCreate)
{
    auto    BOOL        bResult = TRUE;


    // make sure the file exists
    if ((NULL != lpszFileName)  &&  (NULL != *lpszFileName))
        {
        if (ConfigFileExists(lpszFileName))
            {
            // save full path to file
            lstrcpy(m_ConfigFilePath, lpszFileName);
            }
        else if (TRUE == bCreate)
            {
            lstrcpy(m_ConfigFilePath, lpszFileName);
            if (FALSE == CreateDefaultScanCfgDatFile(lpszFileName))
                {
                bResult = FALSE;
                }
            }
        else    // file does not exist and the caller does not want to create one
            {
            bResult = FALSE;
            }
        }
    else    // bogus file argument
        {
        bResult = FALSE;
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::Initialize"



// ==== ConfigFileExists ==================================================
//
//  This function checks to see if the configuration file exists or not.
//
//
//  Input:
//      lpszFileName    -- a fully-qualified path to the configuration file
//
//  Output:
//      TRUE if the configuration file is found, FALSE if not
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ConfigFileExists(LPSTR  lpszFileName)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwTempAttributes = 0;

    dwTempAttributes = GetFileAttributes(lpszFileName);
    if (0xFFFFFFFF == dwTempAttributes)
        {
        bResult = FALSE;
        }
    else
        {
        bResult = TRUE;
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::OriginalFileExists"



// ==== ReadConfigFile ====================================================
//
//  This function reads the Scan and Deliver configuration file.
//
//  Input:
//      dwErrorBits     -- a pointer to the DWORD that will be used to
//                         store error specific flags
//
//  Output:
//      TRUE if all goes well, FALSE if not (the DWORD argument will
//      contain specific error flags)
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadConfigFile(DWORD*  dwErrorBits)
{
    auto    BOOL        bResult = TRUE;
    auto    BOOL        bRetVal = TRUE;

    // initialize the error bits
    *dwErrorBits = 0;

    // start reading data items
    bResult = ReadFirstName();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_NAME;
        }

    bResult = ReadLastName();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_NAME;
        }

    bResult = ReadAddress1();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_ADDRESS;
        }

    bResult = ReadAddress2();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_ADDRESS;
        }

    bResult = ReadCity();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_CITY;
        }

    bResult = ReadCountry();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_COUNTRY;
        }

    bResult = ReadFax();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_FAX;
        }

    bResult = ReadPhone();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_PHONE;
        }

    bResult = ReadState();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_STATE;
        }

    bResult = ReadEmail();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_EMAIL;
        }

    bResult = ReadZipCode();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_ZIP_CODE;
        }

    bResult = ReadAtlasNumber();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_ATLAS_NUMBER;
        }

    bResult = ReadOperatingSystem();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_OS;
        }

    bResult = ReadProductLanguage();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_PRODUCT_LANGUAGE;
        }

    bResult = ReadStripFileContent();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_STRIP_FILE_CONTENT;
        }

    bResult = ReadCompany();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_COMPANY;
        }

    bResult = ReadCorporateSupportNumber();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_CORPORATE_SUPPORT_NUMBER;
        }

    bResult = ReadSmtpServer();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_SMTP_SERVER;
        }

    bResult = ReadCountryOfInfection();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_COUNTRY_OF_INFECTION;
        }

    bResult = ReadStateOfInfection();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_STATE_OF_INFECTION;
        }

    bResult = ReadSymptoms();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_SYMPTOMS;
        }

    bResult = ReadSarcEmailAddress();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_SARC_EMAIL_ADDRESS;
        }

    // this has a reasonable default
    ReadSarcPriorityCode();

    bResult = ReadSarcSocketServer();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_SYMPTOMS;
        }

	bResult = ReadBypassHTTP();
	if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_SYMPTOMS;
        }

    // if any errors occurred, set the return value
    if (*dwErrorBits != 0)
        {
        bRetVal = FALSE;
        }

    return (bRetVal);

}  // end of "CScanDeliverConfiguration::ReadConfigFile"



// ==== ReadFirstName =====================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadFirstName(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_USER_INFO      // section name
                                        , SD_CONFIG_KEY_FIRST_NAME      // key name
                                        , INI_DEFAULT_STR               // default string
                                        , m_szIniDataBuf                // destination buffer
                                        , MAX_INI_BUF_SIZE              // size of destination buffer
                                        , m_ConfigFilePath);            // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgFirstName, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadFirstName"



// ==== ReadLastName ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadLastName(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_USER_INFO      // section name
                                        , SD_CONFIG_KEY_LAST_NAME       // key name
                                        , INI_DEFAULT_STR               // default string
                                        , m_szIniDataBuf                // destination buffer
                                        , MAX_INI_BUF_SIZE              // size of destination buffer
                                        , m_ConfigFilePath);            // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgLastName, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadLastName"



// ==== ReadAddress1 ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadAddress1(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_USER_INFO      // section name
                                        , SD_CONFIG_KEY_ADDRESS1        // key name
                                        , INI_DEFAULT_STR               // default string
                                        , m_szIniDataBuf                // destination buffer
                                        , MAX_INI_BUF_SIZE              // size of destination buffer
                                        , m_ConfigFilePath);            // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgAddress1, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadAddress1"



// ==== ReadAddress2 ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadAddress2(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_USER_INFO      // section name
                                        , SD_CONFIG_KEY_ADDRESS2        // key name
                                        , INI_DEFAULT_STR               // default string
                                        , m_szIniDataBuf                // destination buffer
                                        , MAX_INI_BUF_SIZE              // size of destination buffer
                                        , m_ConfigFilePath);            // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgAddress2, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadAddress2"



// ==== ReadCity ==========================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadCity(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_USER_INFO      // section name
                                        , SD_CONFIG_KEY_CITY            // key name
                                        , INI_DEFAULT_STR               // default string
                                        , m_szIniDataBuf                // destination buffer
                                        , MAX_INI_BUF_SIZE              // size of destination buffer
                                        , m_ConfigFilePath);            // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgCity, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadCity"



// ==== ReadCountry =======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadCountry(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_USER_INFO      // section name
                                        , SD_CONFIG_KEY_COUNTRY         // key name
                                        , INI_DEFAULT_STR               // default string
                                        , m_szIniDataBuf                // destination buffer
                                        , MAX_INI_BUF_SIZE              // size of destination buffer
                                        , m_ConfigFilePath);            // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgCountry, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadCountry"



// ==== ReadFax ===========================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadFax(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_USER_INFO      // section name
                                        , SD_CONFIG_KEY_FAX             // key name
                                        , INI_DEFAULT_STR               // default string
                                        , m_szIniDataBuf                // destination buffer
                                        , MAX_INI_BUF_SIZE              // size of destination buffer
                                        , m_ConfigFilePath);            // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgFax, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadFax"



// ==== ReadPhone =========================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadPhone(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_USER_INFO      // section name
                                        , SD_CONFIG_KEY_PHONE           // key name
                                        , INI_DEFAULT_STR               // default string
                                        , m_szIniDataBuf                // destination buffer
                                        , MAX_INI_BUF_SIZE              // size of destination buffer
                                        , m_ConfigFilePath);            // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgPhone, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadPhone"



// ==== ReadState =========================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadState(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_USER_INFO      // section name
                                        , SD_CONFIG_KEY_STATE           // key name
                                        , INI_DEFAULT_STR               // default string
                                        , m_szIniDataBuf                // destination buffer
                                        , MAX_INI_BUF_SIZE              // size of destination buffer
                                        , m_ConfigFilePath);            // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgState, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadState"



// ==== ReadEmail =========================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadEmail(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_USER_INFO      // section name
                                        , SD_CONFIG_KEY_EMAIL           // key name
                                        , INI_DEFAULT_STR               // default string
                                        , m_szIniDataBuf                // destination buffer
                                        , MAX_INI_BUF_SIZE              // size of destination buffer
                                        , m_ConfigFilePath);            // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgEmail, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadEmail"



// ==== ReadZipCode =======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadZipCode(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_USER_INFO      // section name
                                        , SD_CONFIG_KEY_ZIPCODE         // key name
                                        , INI_DEFAULT_STR               // default string
                                        , m_szIniDataBuf                // destination buffer
                                        , MAX_INI_BUF_SIZE              // size of destination buffer
                                        , m_ConfigFilePath);            // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgZipCode, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadZipCode"



// ==== ReadAtlasNumber ===================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadAtlasNumber(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_USER_INFO      // section name
                                        , SD_CONFIG_KEY_ATLAS_NUMBER    // key name
                                        , INI_DEFAULT_STR               // default string
                                        , m_szIniDataBuf                // destination buffer
                                        , MAX_INI_BUF_SIZE              // size of destination buffer
                                        , m_ConfigFilePath);            // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgAtlasNumber, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadAtlasNumber"



// ==== ReadOperatingSystem ===============================================
//
//  This function reads the platform IDs from the configuration file and
//  sets bits accordingly in the configuration object.  Each platform ID
//  is a power of two, and they start at "1".  So each entry in the file
//  looks like this:
//
//      1=TRUE
//      2=TRUE
//      4=FALSE
//      8=TRUE
//      etc...
//
//  The original source of the values are from the "platform.dat" file
//  provided by SARC.  The significance of each ID only has meaning to
//  SARC.  The contents of "platform.dat" are used to populate the
//  platform listbox in the corporate panel, so when a user selects an
//  item we only know it by its platform ID (the strings displayed are
//  whatever is in the "platform.dat" file).
//
//  Input:  nothing
//
//  Output: a value of TRUE if all goes well, FALSE if an error occurs.
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadOperatingSystem(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwPlatformID = 0;
    auto    DWORD       dwResult = 0;
    auto    int         nCounter = 0;
    auto    TCHAR       szBuf[10];

    // initialize the structure members
    m_ConfigData.dwOperatingSystem = 0;
    m_ConfigData.numPlatformIDs = 0;

    // read the platform IDs
    for (dwPlatformID = 1; ; dwPlatformID <<= 1, ++nCounter)
        {
        // convert the current platform ID to a string
        _itot(dwPlatformID, szBuf, 10);

        // set the corresponding bit to TRUE if the INI entry says to
        dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_OS_INFO    // section name
                                            , szBuf                     // key name
                                            , INI_DEFAULT_STR           // default string
                                            , m_szIniDataBuf            // destination buffer
                                            , MAX_INI_BUF_SIZE          // size of destination buffer
                                            , m_ConfigFilePath);        // location of INI file
        if (_tcslen(m_szIniDataBuf) != dwResult)
            {
            bResult = FALSE;
            }
        else if (0 == _tcscmp(TRUE_STRING, m_szIniDataBuf))
            {
            m_ConfigData.dwOperatingSystem |= dwPlatformID;
            }
        else if (0 == _tcscmp(INI_DEFAULT_STR, m_szIniDataBuf))
            {
            // we've run out of platform IDs, break out of the loop
            break;
            }

        // increment the platform ID counter
        ++m_ConfigData.numPlatformIDs;
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadOperatingSystem"



// ==== ReadProductLanguage ===============================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadProductLanguage(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_SYSTEM_INFO        // section name
                                        , SD_CONFIG_KEY_PRODUCT_LANGUAGE    // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_ConfigFilePath);                // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgProductLanguage, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadProductLanguage"



// ==== ReadStripFileContent ==============================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadStripFileContent(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO     // section name
                                        , SD_CONFIG_KEY_STRIP_FILE_CONTENT  // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_ConfigFilePath);                // location of INI file
    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        if (0 == _tcscmp(TRUE_STRING, m_szIniDataBuf))
            {
            m_ConfigData.bScanCfgStripFileContent = TRUE;
            }
        else
            {
            m_ConfigData.bScanCfgStripFileContent = FALSE;
            }
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadStripFileContent"



// ==== ReadCompany =======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadCompany(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO     // section name
                                        , SD_CONFIG_KEY_COMPANY             // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_ConfigFilePath);                // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgCompany, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadCompany"



// ==== ReadCorporateSupportNumber ========================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadCorporateSupportNumber(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO     // section name
                                        , SD_CONFIG_KEY_SUPPORT_NUMBER      // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_ConfigFilePath);                // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgCorporateSupportNumber, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadCorporateSupportNumber"



// ==== ReadUseCustomSmtpServer ===========================================
//
//
// ========================================================================
//  Function created: 5/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadUseCustomSmtpServer(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;
    auto    TCHAR       szCharBuf[25];  // only needs to hold "TRUE" or "FALSE"

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO         // section name
                                        , SD_CONFIG_KEY_USE_CUSTOM_SMTP_SERVER  // key name
                                        , INI_DEFAULT_STR                       // default string
                                        , szCharBuf                             // destination buffer
                                        , MAX_INI_BUF_SIZE                      // size of destination buffer
                                        , m_ConfigFilePath);                    // location of INI file
    if (_tcslen(szCharBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        if (0 == _tcscmp(TRUE_STRING, szCharBuf))
            {
            m_ConfigData.bScanCfgUseCustomSmtpServer = TRUE;
            }
        else
            {
            m_ConfigData.bScanCfgUseCustomSmtpServer = FALSE;
            }
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadUseCustomSmtpServer"



// ==== ReadSmtpServer ====================================================
//
//  If the function is unable to load the SMTP server from the string table,
//  then it returns FALSE and sets the string to "".
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// 4/27/98 - JTAYLOR - Function updated to handle LoadString failure.
//                     Updated CString usage.
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadSmtpServer(void)
{
    auto    BOOL        bResult = TRUE;
    auto    BOOL        bDefaultString = TRUE;
    auto    BOOL        bLoadString = TRUE;
    auto    CString     szServerName;
    auto    CString     szDefaultString;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO     // section name
                                        , SD_CONFIG_KEY_SMTP_SERVER         // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_ConfigFilePath);                // location of INI file

    // check for read error
    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        goto  Exit_Function;
        }

    // see if we're to use a custom SMTP server or not
    if  ((TRUE == ReadUseCustomSmtpServer())
    &&  (TRUE == m_ConfigData.bScanCfgUseCustomSmtpServer))
        {
        lstrcpy(m_ConfigData.szScanCfgSmtpServer, m_szIniDataBuf);
        }
    else
        {
        // use the default
        if( 0 != szServerName.LoadString(IDS_DEFAULT_SMTP_SERVER) )
            {
            lstrcpy(m_ConfigData.szScanCfgSmtpServer, szServerName);
            }
        else
            {
            bResult = FALSE;
            goto  Exit_Function;
            }
        }


Exit_Function:
    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadSmtpServer"



// ==== ReadCountryOfInfection ============================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadCountryOfInfection(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_VIRUS_INFO         // section name
                                        , SD_CONFIG_KEY_COUNTRY             // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_ConfigFilePath);                // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgCountryOfInfection, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadCountryOfInfection"



// ==== ReadStateOfInfection ==============================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadStateOfInfection(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_VIRUS_INFO         // section name
                                        , SD_CONFIG_KEY_STATE               // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_ConfigFilePath);                // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgStateOfInfection, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadStateOfInfection"



// ==== ReadSymptoms ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadSymptoms(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_VIRUS_INFO         // section name
                                        , SD_CONFIG_KEY_SYMPTOMS            // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , m_szIniDataBuf                    // destination buffer
                                        , MAX_INI_BUF_SIZE                  // size of destination buffer
                                        , m_ConfigFilePath);                // location of INI file

    if (_tcslen(m_szIniDataBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        lstrcpy(m_ConfigData.szScanCfgSymptoms, m_szIniDataBuf);
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadSymptoms"



// ==== ReadSarcEmailAddress ==============================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadSarcEmailAddress(void)
{
    auto    BOOL                bResult = TRUE;
    auto    CString             szEmailAddress;
    auto    DWORD               dwResult;
    auto    UINT                nResID = 0;


    // see if we're running in corporate or retail mode
    dwResult = GetCorporateMode();
    if (0xFFFFFFFF != dwResult)
        {
        // see if there's a custom corporate SARC email address we can use
        ReadCustomSarcEmailAddress(szEmailAddress);

        // if we got a custom email address, use it, otherwise use the string table entry
        if (TRUE == szEmailAddress.IsEmpty())
            {
            nResID = IDS_SARC_EMAIL_CORPORATE;
            }
        }
    else
        {
        nResID = IDS_SARC_EMAIL_RETAIL;
        }

    // if we want a string table entry and we fail to load the string, return
    if ((0 != nResID)  &&  (0 == szEmailAddress.LoadString(nResID)))
        {
        bResult = FALSE;
        goto  Exit_Function;
        }

    // decode the string
    auto    char*       strPtr;
    strPtr = szEmailAddress.GetBuffer(MAX_EMAIL_LEN);
    DecodeString(strPtr);

    // save the string to the configuration structure
    lstrcpy(m_ConfigData.szScanCfgSarcEmail, strPtr);
    szEmailAddress.ReleaseBuffer();



Exit_Function:

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadSarcEmailAddress"



// ==== ReadSarcSocketServer ==============================================
//
//
// ========================================================================
//  Function created: 4/98, SEDWARD
//  6/02/98 - JTAYLOR - Fixed FindResource to work with our new resource DLL.
// ========================================================================

BOOL    CScanDeliverConfiguration::ReadSarcSocketServer(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwSize;
    auto    DWORD       dwResID;
    auto    HRSRC       hResource;
    auto    LPBYTE      lpData;
    auto    TCHAR       szCharData[MAX_SOCKET_SERVER_LEN];


    // get the appropriate resource ID
    if (FALSE == g_ConfigInfo.IsCorporateMode())
        {
        dwResID = IDR_SOCK_SRVR_NAME_RETAIL;
        }
    else
        {
        dwResID = IDR_SOCK_SRVR_NAME_CORPORATE;
        }

    // locate the binary resource
    HINSTANCE   hRes = AfxFindResourceHandle( MAKEINTRESOURCE( dwResID ), "SOCKSRVR" );
    hResource = FindResource(hRes, (LPCTSTR)dwResID, "SOCKSRVR");
    if (NULL == hResource)
        {
        bResult = FALSE;
        goto  Exit_Function;
        }

    // get the number of bytes this resource occupies
    dwSize = SizeofResource(hRes, hResource);

    // copy the resource into our character buffer and terminate with a null
    lpData = (LPBYTE)LoadResource(hRes, hResource);
    if (NULL != lpData)
        {
        memcpy((void*)szCharData, (void*)lpData, dwSize);
        szCharData[dwSize] = NULL;
        }

    // decode the string
    DecodeString((char*)szCharData);

    // save the string to the configuration structure
    lstrcpy(m_ConfigData.szScanCfgSarcSocketServer, szCharData);



Exit_Function:

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadSarcSocketServer"

BOOL    CScanDeliverConfiguration::ReadBypassHTTP(void)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;
    auto    TCHAR       szCharBuf[25];  // only needs to hold "TRUE" or "FALSE"

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_BYPASS_HTTP  // section name
                                        , SD_CONFIG_KEY_ENABLED       // key name
                                        , INI_DEFAULT_STR             // default string
                                        , szCharBuf                   // destination buffer
                                        , MAX_INI_BUF_SIZE            // size of destination buffer
                                        , m_ConfigFilePath);          // location of INI file
    if (_tcslen(szCharBuf) != dwResult)
        {
        bResult = FALSE;
        }
    else
        {
        if (0 == _tcscmp(TRUE_STRING, szCharBuf))
            {
            m_ConfigData.bBypassHTTP = TRUE;
            }
        else
            {
            m_ConfigData.bBypassHTTP = FALSE;
            }
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::ReadBypassHTTP"

// ==== WriteConfigFile ===================================================
//
//  This function writes the Scan and Deliver configuration file.
//
//  Input:
//      dwErrorBits     -- a pointer to the DWORD that will be used to
//                         store error specific flags
//
//  Output:
//      TRUE if all goes well, FALSE if not (the DWORD argument will
//      contain specific error flags)
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteConfigFile(DWORD*  dwErrorBits)
{
    auto    BOOL        bResult = TRUE;
    auto    BOOL        bRetVal = TRUE;

    // initialize the error bits
    *dwErrorBits = 0;

    // start writing data items
    bResult = WriteFirstName();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_NAME;
        }

    bResult = WriteLastName();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_NAME;
        }

    bResult = WriteAddress1();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_ADDRESS;
        }

    bResult = WriteAddress2();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_ADDRESS;
        }

    bResult = WriteCity();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_CITY;
        }

    bResult = WriteCountry();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_COUNTRY;
        }

    bResult = WriteFax();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_FAX;
        }

    bResult = WritePhone();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_PHONE;
        }

    bResult = WriteState();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_STATE;
        }

    bResult = WriteEmail();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_EMAIL;
        }

    bResult = WriteZipCode();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_ZIP_CODE;
        }

    bResult = WriteOperatingSystem();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_OS;
        }

    bResult = WriteProductLanguage();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_PRODUCT_LANGUAGE;
        }

    bResult = WriteStripFileContent();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_STRIP_FILE_CONTENT;
        }

    bResult = WriteCompany();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_COMPANY;
        }

    bResult = WriteCorporateSupportNumber();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_CORPORATE_SUPPORT_NUMBER;
        }

    bResult = WriteAtlasNumber();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_ATLAS_NUMBER;
        }

    bResult = WriteSmtpServer();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_SMTP_SERVER;
        }

    bResult = WriteCountryOfInfection();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_COUNTRY_OF_INFECTION;
        }

    bResult = WriteStateOfInfection();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_STATE_OF_INFECTION;
        }

    bResult = WriteSymptoms();
    if (FALSE == bResult)
        {
        *dwErrorBits |= SD_CONFIG_ERROR_SYMPTOMS;
        }


    // if any errors occurred, set the return value
    if (*dwErrorBits != 0)
        {
        bRetVal = FALSE;
        }

    return (bRetVal);

}  // end of "CScanDeliverConfiguration::WriteConfigFile"



// ==== WriteFirstName ====================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteFirstName(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO         // section name
                                        , SD_CONFIG_KEY_FIRST_NAME          // key name
                                        , m_ConfigData.szScanCfgFirstName   // data buffer
                                        , m_ConfigFilePath);                // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteFirstName"



// ==== WriteLastName =====================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteLastName(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO         // section name
                                        , SD_CONFIG_KEY_LAST_NAME           // key name
                                        , m_ConfigData.szScanCfgLastName    // data buffer
                                        , m_ConfigFilePath);                // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteLastName"



// ==== WriteAddress1 =====================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteAddress1(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_ADDRESS1        // key name
                                        , m_ConfigData.szScanCfgAddress1// data buffer
                                        , m_ConfigFilePath);            // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteAddress1"



// ==== WriteAddress2 =====================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteAddress2(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_ADDRESS2        // key name
                                        , m_ConfigData.szScanCfgAddress2// data buffer
                                        , m_ConfigFilePath);            // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteAddress2"



// ==== WriteCity =========================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteCity(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_CITY            // key name
                                        , m_ConfigData.szScanCfgCity    // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteCity"



// ==== WriteCountry ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteCountry(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_COUNTRY         // key name
                                        , m_ConfigData.szScanCfgCountry // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteCountry"



// ==== WriteFax ==========================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteFax(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_FAX             // key name
                                        , m_ConfigData.szScanCfgFax     // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteFax"



// ==== WritePhone ========================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WritePhone(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_PHONE           // key name
                                        , m_ConfigData.szScanCfgPhone   // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WritePhone"



// ==== WriteState ========================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteState(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_STATE           // key name
                                        , m_ConfigData.szScanCfgState   // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteState"



// ==== WriteEmail ========================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteEmail(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_EMAIL           // key name
                                        , m_ConfigData.szScanCfgEmail   // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteEmail"



// ==== WriteZipCode ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteZipCode(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_ZIPCODE         // key name
                                        , m_ConfigData.szScanCfgZipCode // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteZipCode"



// ==== WriteOperatingSystem ==============================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteOperatingSystem(void)
{

    auto    BOOL        bResult = TRUE;
    auto    BOOL        bRetVal = TRUE;
    auto    char*       szValue;
    auto    DWORD       dwPlatformID = 1;
    auto    int         nCounter;
    auto    TCHAR       szBuf[10];


    // since this portion of the configuration file is dependent on another external
    // file, clear out the entries in this section before writing; this way, if the
    // external file changes we'll inherit those changes
    WritePrivateProfileSection(SD_CONFIG_SECTION_OS_INFO        // section name
                                        , NULL                  // data
                                        , m_ConfigFilePath);    // location of INI file

    // write out as many platform IDs as we detected during the "read" operation
    for (nCounter = 0; nCounter < m_ConfigData.numPlatformIDs; dwPlatformID <<= 1, ++nCounter)
        {
        // convert the current platform ID value into a string
        _itot(dwPlatformID, szBuf, 10);

        // see if we need a TRUE or FALSE string for the current platform ID
        if (dwPlatformID & m_ConfigData.dwOperatingSystem)
            {
            szValue = TRUE_STRING;
            }
        else
            {
            szValue = FALSE_STRING;
            }

        // write the entry to the configuration file
        bResult = WritePrivateProfileString(SD_CONFIG_SECTION_OS_INFO   // section name
                                            , szBuf                     // key name
                                            , szValue                   // data
                                            , m_ConfigFilePath);        // location of INI file
        if (FALSE == bResult)
            {
            bRetVal = FALSE;
            }
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteOperatingSystem"



// ==== WriteProductLanguage ==============================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteProductLanguage(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_SYSTEM_INFO           // section name
                                        , SD_CONFIG_KEY_PRODUCT_LANGUAGE        // key name
                                        , m_ConfigData.szScanCfgProductLanguage // data buffer
                                        , m_ConfigFilePath);                    // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteProductLanguage"



// ==== WriteStripFileContent =============================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteStripFileContent(void)
{
    auto    BOOL        bResult = TRUE;

    if (TRUE == m_ConfigData.bScanCfgStripFileContent)
        {
        bResult = WritePrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO    // section name
                                            , SD_CONFIG_KEY_STRIP_FILE_CONTENT  // key name
                                            , TRUE_STRING                       // data
                                            , m_ConfigFilePath);                // location of INI file
        }
    else
        {
        bResult = WritePrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO    // section name
                                            , SD_CONFIG_KEY_STRIP_FILE_CONTENT  // key name
                                            , FALSE_STRING                      // data
                                            , m_ConfigFilePath);                // location of INI file
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteStripFileContent"



// ==== WriteCompany ======================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteCompany(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO    // section name
                                        , SD_CONFIG_KEY_COMPANY             // key name
                                        , m_ConfigData.szScanCfgCompany     // data buffer
                                        , m_ConfigFilePath);                // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteCompany"



// ==== WriteCorporateSupportNumber =======================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteCorporateSupportNumber(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO                // section name
                                        , SD_CONFIG_KEY_SUPPORT_NUMBER                  // key name
                                        , m_ConfigData.szScanCfgCorporateSupportNumber  // data buffer
                                        , m_ConfigFilePath);                            // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteCorporateSupportNumber"



// ==== WriteAtlasNumber ==================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteAtlasNumber(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO             // section name
                                        , SD_CONFIG_KEY_ATLAS_NUMBER            // key name
                                        , m_ConfigData.szScanCfgAtlasNumber     // data buffer
                                        , m_ConfigFilePath);                    // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteAtlasNumber"



// ==== WriteUseCustomSmtpServer ==========================================
//
//
// ========================================================================
//  Function created: 5/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteUseCustomSmtpServer(void)
{
    auto    BOOL        bResult = TRUE;

    if (TRUE == m_ConfigData.bScanCfgUseCustomSmtpServer)
        {
        bResult = WritePrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO        // section name
                                            , SD_CONFIG_KEY_USE_CUSTOM_SMTP_SERVER  // key name
                                            , TRUE_STRING                           // data
                                            , m_ConfigFilePath);                    // location of INI file
        }
    else
        {
        bResult = WritePrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO        // section name
                                            , SD_CONFIG_KEY_USE_CUSTOM_SMTP_SERVER  // key name
                                            , FALSE_STRING                          // data
                                            , m_ConfigFilePath);                    // location of INI file
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteUseCustomSmtpServer"



// ==== WriteSmtpServer ===================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteSmtpServer(void)
{
    auto    BOOL        bResult = TRUE;
    auto    TCHAR*      szBufPtr;

    // write out the custom checkbox setting
    WriteUseCustomSmtpServer();

    // only write the smtp server if it's custom (we never want to write the
    // "secret" SARC address)
    if (TRUE == m_ConfigData.bScanCfgUseCustomSmtpServer)
        {
        // write the string out to the configuration file
        szBufPtr = m_ConfigData.szScanCfgSmtpServer;
        bResult = WritePrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO    // section name
                                            , SD_CONFIG_KEY_SMTP_SERVER         // key name
                                            , szBufPtr                          // data buffer
                                            , m_ConfigFilePath);                // location of INI file
        }


    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteSmtpServer"



// ==== WriteCountryOfInfection ===========================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteCountryOfInfection(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_VIRUS_INFO                // section name
                                        , SD_CONFIG_KEY_COUNTRY                     // key name
                                        , m_ConfigData.szScanCfgCountryOfInfection  // data buffer
                                        , m_ConfigFilePath);                        // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteCountryOfInfection"



// ==== WriteStateOfInfection =============================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteStateOfInfection(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_VIRUS_INFO                // section name
                                        , SD_CONFIG_KEY_STATE                       // key name
                                        , m_ConfigData.szScanCfgStateOfInfection    // data buffer
                                        , m_ConfigFilePath);                        // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteStateOfInfection"



// ==== WriteSymptoms =====================================================
//
//
// ========================================================================
//  Function created: 3/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::WriteSymptoms(void)
{
    auto    BOOL        bResult = TRUE;

    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_VIRUS_INFO        // section name
                                        , SD_CONFIG_KEY_SYMPTOMS            // key name
                                        , m_ConfigData.szScanCfgSymptoms    // data buffer
                                        , m_ConfigFilePath);                // location of INI file
    return (bResult);

}  // end of "CScanDeliverConfiguration::WriteSymptoms"



// ==== CreateDefaultScanCfgDatFile =======================================
//
//  This function is used to generate a default DAT file.
//
//  Input:
//      lpszFileName    - a fully-qualified path to the ScanCfg.dat file
//
//  Output:
//      TRUE if all goes well, FALSE if not
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::CreateDefaultScanCfgDatFile(LPTSTR  lpszFileName)
{
    auto    BOOL        bResult = TRUE;
    auto    BOOL        bRetVal = TRUE;
    auto    char*       szSectionName = NULL;
    auto    HANDLE      fileHandle = INVALID_HANDLE_VALUE;
	auto    CString     szDefaultSMTPServer;//EA
	auto    CString     szLoadString;  //EA

    // create the file
    fileHandle = CreateFile(lpszFileName                        // name of the file
                                , GENERIC_WRITE                 // access mode
                                , 0                             // no sharing
                                , NULL                          // no secuity
                                , CREATE_ALWAYS                 // always create the file
                                , FILE_FLAG_SEQUENTIAL_SCAN     // flags and attributes
                                , NULL);                        // no template attributes

    if (INVALID_HANDLE_VALUE == fileHandle)
        {
        bRetVal = FALSE;
        goto  Exit_Function;
        }
    else
        {
        CloseHandle(fileHandle);
        }



    // write out the default values...

    // first name
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_FIRST_NAME      // key name
                                        , SD_DEFAULT_USER_FIRST_NAME    // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // last name
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_LAST_NAME       // key name
                                        , SD_DEFAULT_USER_LAST_NAME     // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // address1
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_ADDRESS1        // key name
                                        , SD_DEFAULT_ADDRESS1           // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // address2
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_ADDRESS2        // key name
                                        , SD_DEFAULT_ADDRESS2           // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // city
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_CITY            // key name
                                        , SD_DEFAULT_CITY               // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // country
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_COUNTRY         // key name
                                        , SD_DEFAULT_COUNTRY            // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // fax
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_FAX             // key name
                                        , SD_DEFAULT_FAX                // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // phone
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_PHONE           // key name
                                        , SD_DEFAULT_PHONE              // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // state
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_STATE           // key name
                                        , SD_DEFAULT_STATE              // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // email
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_EMAIL           // key name
                                        , SD_DEFAULT_EMAIL              // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // zip code
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_ZIPCODE         // key name
                                        , SD_DEFAULT_ZIP_CODE           // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // atlas number
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_USER_INFO     // section name
                                        , SD_CONFIG_KEY_ATLAS_NUMBER    // key name
                                        , SD_DEFAULT_ATLAS_NUMBER       // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }



    // operating systems...

    #if 0   // IFDEF_DISABLE
    // ...Windows 3.1
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_OS_INFO   // section name
                                        , SD_CONFIG_KEY_OS_WIN31    // key name
                                        , SD_DEFAULT_OS_WIN31       // data
                                        , m_ConfigFilePath);        // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }


    // ...Unix
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_OS_INFO   // section name
                                        , SD_CONFIG_KEY_OS_UNIX     // key name
                                        , SD_DEFAULT_OS_UNIX        // data
                                        , m_ConfigFilePath);        // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }


    // ...DEC Alpha
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_OS_INFO   // section name
                                        , SD_CONFIG_KEY_OS_ALPHA    // key name
                                        , SD_DEFAULT_OS_ALPHA       // data
                                        , m_ConfigFilePath);        // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }


    // ...NLM
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_OS_INFO   // section name
                                        , SD_CONFIG_KEY_OS_NLM      // key name
                                        , SD_DEFAULT_OS_NLM         // data
                                        , m_ConfigFilePath);        // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }
    #endif  // IFDEF_DISABLE



    // product language
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_SYSTEM_INFO           // section name
                                        , SD_CONFIG_KEY_PRODUCT_LANGUAGE        // key name
                                        , SD_DEFAULT_PRODUCT_LANGUAGE           // data buffer
                                        , m_ConfigFilePath);                    // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // strip file content
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO            // section name
                                        , SD_CONFIG_KEY_STRIP_FILE_CONTENT          // key name
                                        , SD_DEFAULT_STRIP_FILE_CONTENT             // data buffer
                                        , m_ConfigFilePath);                        // location of INI file

    // company
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO    // section name
                                        , SD_CONFIG_KEY_COMPANY             // key name
                                        , SD_DEFAULT_COMPANY                // data buffer
                                        , m_ConfigFilePath);                // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // corporate support number
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO    // section name
                                        , SD_CONFIG_KEY_SUPPORT_NUMBER      // key name
                                        , SD_DEFAULT_SUPPORT_NUMBER         // data buffer
                                        , m_ConfigFilePath);                // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // smtp server
	///EA - Adding for localization of "Default SMTP Server" by picking "Default" and "SMTP Server" from scandres.rc string table///////////////////////
	if( szLoadString.LoadString(IDS_DEFAULT) != 0)
	{
		//empty the string before adding anything
		szDefaultSMTPServer.Empty();
		szDefaultSMTPServer += szLoadString;//EA
		szDefaultSMTPServer += " ";
		if( szLoadString.LoadString(IDS_SMTP_SERVER) != 0)
		{
			szDefaultSMTPServer += szLoadString;//EA
			bResult = WritePrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO    // section name
                                        , SD_CONFIG_KEY_SMTP_SERVER         // key name
                                        , szDefaultSMTPServer            // data buffer
                                        , m_ConfigFilePath);                // location of INI file
		}
		else //if we cant get from the resource then we just write the english name
			bResult = WritePrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO    // section name
                                        , SD_CONFIG_KEY_SMTP_SERVER         // key name
                                        , SD_DEFAULT_SMTP_SERVER            // data buffer
                                        , m_ConfigFilePath);                // location of INI file

	}
	else //if we cant get from the resource then we just write the english name
	//////////////////////////////EA////////////////////////////////
		bResult = WritePrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO    // section name
                                        , SD_CONFIG_KEY_SMTP_SERVER         // key name
                                        , SD_DEFAULT_SMTP_SERVER            // data buffer
                                        , m_ConfigFilePath);                // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // country of infection
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_VIRUS_INFO    // section name
                                        , SD_CONFIG_KEY_COUNTRY         // key name
                                        , SD_DEFAULT_COUNTRY            // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // state of infection
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_VIRUS_INFO    // section name
                                        , SD_CONFIG_KEY_STATE           // key name
                                        , SD_DEFAULT_STATE              // data buffer
                                        , m_ConfigFilePath);            // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

    // symptoms
    bResult = WritePrivateProfileString(SD_CONFIG_SECTION_VIRUS_INFO        // section name
                                        , SD_CONFIG_KEY_SYMPTOMS            // key name
                                        , SD_DEFAULT_SYMPTOMS               // data buffer
                                        , m_ConfigFilePath);                // location of INI file
    if (FALSE == bResult)
        {
        bRetVal = FALSE;
        }

Exit_Function:

    return (bRetVal);

}  // end of "CScanDeliverConfiguration::CreateDefaultQuarOptsDatFile"




// ==== GetCorporateMode ==================================================
//
//  This function fetches the corporate value from the registry.  If the
//  value does not exist, a value is returned to indicate this.
//
//  Input:
//      lpszRegKey  -- a pointer to the registry key path that contains the
//                     corporate features value we're interested in
//
//  Output:
//      The corporate features value, or 0xFFFFFFFF if it does not exist
//
// ========================================================================
//  Function created: 4/98, SEDWARD
//  5/14/98 - JTAYLOR - Fixed KEY_ALL_ACCESS bug.
// ========================================================================

DWORD   CScanDeliverConfiguration::GetCorporateMode(LPCTSTR  lpszRegKey)

{
    auto    BOOL            bResult = FALSE;
    auto    DWORD           dwRegValueType;
    auto    DWORD           dwDataBuf;
    auto    DWORD           dwSize;
    auto    HKEY            hKey = NULL;
    auto    long            lResult;


    if (NULL == lpszRegKey)
        {
        dwDataBuf = 0xFFFFFFFF;
        goto  Exit_Function;
        }

    // open the registry key
    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE                       // handle of open key
                                        , lpszRegKey                // address of name of subkey to open
                                        , 0                         // reserved
                                        , (KEY_READ | KEY_WRITE)    // security access mask
                                        , &hKey);                   // address of handle of open key
    if (ERROR_SUCCESS != lResult)
        {
        dwDataBuf = 0xFFFFFFFF;
        goto  Exit_Function;
        }

    // get the value of "corporate features" (if it exists)
    dwSize = sizeof(DWORD);
    lResult = RegQueryValueEx(hKey              // handle of key to query
                    , REGVAL_CORPORATE_FEATURES // address of name of value to query
                    , 0                         // reserved (must be zero)
                    , &dwRegValueType           // address of buffer for value type
                    , (LPBYTE)&dwDataBuf        // address of data buffer
                    , &dwSize);                 // address of data buffer size

    if ((ERROR_SUCCESS != lResult)  ||  (REG_DWORD != dwRegValueType))
        {
        dwDataBuf = 0xFFFFFFFF;
        goto  Exit_Function;
        }


Exit_Function:

    if (NULL != hKey)
        {
        RegCloseKey(hKey);
        }

    return (dwDataBuf);

}  // end of "CScanDeliverConfiguration::GetCorporateMode"



// ==== IsCorporateMode ===================================================
//
//  This function simply lets the caller know if we're running in corporate
//  mode or not.
//
//  Input:
//      lpszRegKey  -- a pointer to the registry key path that contains the
//                     corporate features value we're interested in
//
//  Output:
//      TRUE if we're running in corporate mode, FALSE if not
//
// ========================================================================
//  Function created: 4/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::IsCorporateMode(LPCTSTR  lpszRegKey)
{
    if (0xFFFFFFFF == GetCorporateMode(lpszRegKey))
        {
        return (FALSE);
        }
    else
        {
        return (TRUE);
        }

}  // end of "CScanDeliverConfiguration::IsCorporateMode"



// ==== GetSmtpServerDisplayString ========================================
//
//  If the function is unable to load the SMTP server from the string table,
//  then it returns FALSE and sets the string to "".
//
// ========================================================================
//  Function created: 5/98, SEDWARD
// ========================================================================

BOOL    CScanDeliverConfiguration::GetSmtpServerDisplayString(LPTSTR  szCharBuf
                                                                , int  nBufSize)
{
    auto    BOOL        bResult = TRUE;
    auto    BOOL        bDefaultString = TRUE;
    auto    BOOL        bLoadString = TRUE;
    auto    CString     szServerName;
    auto    CString     szDefaultString;
    auto    DWORD       dwResult = 0;

    // fetch the string from the INI file
    dwResult = GetPrivateProfileString(SD_CONFIG_SECTION_CORPORATE_INFO     // section name
                                        , SD_CONFIG_KEY_SMTP_SERVER         // key name
                                        , INI_DEFAULT_STR                   // default string
                                        , szCharBuf                         // destination buffer
                                        , nBufSize                          // size of destination buffer
                                        , m_ConfigFilePath);                // location of INI file

    // check for read error
    if (_tcslen(szCharBuf) != dwResult)
        {
        bResult = FALSE;
        *szCharBuf = NULL;
        }

    return (bResult);

}  // end of "CScanDeliverConfiguration::GetSmtpServerDisplayString"



// ==== ReadCustomSarcEmailAddress ========================================
//
//  This function attempts to read a custom SARC email address from the
//  registry.  This custom email address will only exist in the registry
//  if technical support has provided a corporate user (ie, platinum) with
//  a reg file containing the address.  If we can't find a custom email
//  address, the CString argument will be set to empty; otherwise, it will
//  contain the custom email address.
//
//  Input:
//      szEmail         -- a CString reference that is the destination for
//                         the fetched custom email string
//
//      bDecodeString   -- a flag to indicate whether or not the string (if
//                         it is found) is to be decoded (TRUE means decode
//                         the string, FALSE means not to)
//
//  Output:
//      If the custom email address is found in the registry, the CString
//      argument will be initialized with that value.  If we can't find the
//      custom email address, the CString argument will be set to 'empty'.
//
// ========================================================================
//  Function created: 6/98, SEDWARD
// ========================================================================

void    CScanDeliverConfiguration::ReadCustomSarcEmailAddress(CString&  szEmail
                                                        , BOOL  bDecodeString)
{
    auto    BOOL            bResult = TRUE;
    auto    char            dataBuf[MAX_EMAIL_LEN+1];
    auto    DWORD           dwRegValueType;
    auto    DWORD           dwSize;
    auto    HKEY            hKey = NULL;
    auto    long            lResult;


    // open the registry key
    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE                   // handle of open key
                            , REGKEY_HKLM_SW_NAV_QUARANTINE     // address of name of subkey to open
                            , 0                                 // reserved
                            , (KEY_READ | KEY_WRITE)            // security access mask
                            , &hKey);                           // address of handle of open key
    if (ERROR_SUCCESS != lResult)
        {
        // if we can't open the key, make sure the email string argument is empty and exit
        szEmail.Empty();
        goto  Exit_Function;
        }

    // get the value of the custom email address (it's encrypted)
    dwSize = MAX_EMAIL_LEN;
    lResult = RegQueryValueEx(hKey                  // handle of key to query
                    , REGVAL_EMAIL_SERVER_ADDRESS   // address of name of value to query
                    , 0                             // reserved (must be zero)
                    , &dwRegValueType               // address of buffer for value type
                    , (LPBYTE)&dataBuf              // address of data buffer
                    , &dwSize);                     // address of data buffer size
    if ((ERROR_SUCCESS != lResult)  ||  (REG_BINARY != dwRegValueType))
        {
        // if we can't access the value, make sure the email string argument is empty and exit
        szEmail.Empty();
        goto  Exit_Function;
        }

    // if the caller wants the string decoded, do so
    if (TRUE == bDecodeString)
        {
        DecodeString((char*)dataBuf);
        }

    // save the string to the configuration structure
    szEmail = (char*)dataBuf;



Exit_Function:

    if (NULL != hKey)
        {
        RegCloseKey(hKey);
        }

}  // end of "CScanDeliverConfiguration::ReadCustomSarcEmailAddress"



// ==== ReadSarcPriorityCode ==============================================
//
//  This function attempts to read the SARC priority code from the registry.
//  The priority code will only exist in the registry if technical support
//  has provided a corporate user (ie, platinum) with a reg file containing
//  the priority code.  If we can't find the priority code value in the
//  registry, we default to "normal" priority (ie, CUSTOMER_PRIORITY).
//
//  Input:  nothing
//
//  Output: nothing (however, the "nPriorityCode" member of the "m_ConfigData"
//          structure will be initialized to either the custom priority code
//          found in the registry, or CUSTOMER_PRIORITY be default.
//
// ========================================================================
//  Function created: 6/98, SEDWARD
// ========================================================================

void    CScanDeliverConfiguration::ReadSarcPriorityCode(void)
{
    auto    DWORD           dwDataBuf;
    auto    DWORD           dwRegValueType;
    auto    DWORD           dwSize;
    auto    HKEY            hKey = NULL;
    auto    long            lResult;


    // open the registry key
    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE                   // handle of open key
                            , REGKEY_HKLM_SW_NAV_QUARANTINE     // address of name of subkey to open
                            , 0                                 // reserved
                            , (KEY_READ | KEY_WRITE)            // security access mask
                            , &hKey);                           // address of handle of open key
    if (ERROR_SUCCESS != lResult)
        {
        // if we can't open the key, use the retail default and exit
        m_ConfigData.nPriorityCode = CUSTOMER_PRIORITY;
        goto  Exit_Function;
        }

    // get the value of the SARC priority code
    dwSize = sizeof(DWORD);
    lResult = RegQueryValueEx(hKey                  // handle of key to query
                    , REGVAL_EMAIL_SERVER_PRIORITY  // address of name of value to query
                    , 0                             // reserved (must be zero)
                    , &dwRegValueType               // address of buffer for value type
                    , (LPBYTE)&dwDataBuf            // address of data buffer
                    , &dwSize);                     // address of data buffer size
    if ((ERROR_SUCCESS != lResult)  ||  (REG_DWORD  != dwRegValueType))
        {
        // if we can't open the key, use the retail default and exit
        m_ConfigData.nPriorityCode = CUSTOMER_PRIORITY;
        goto  Exit_Function;
        }

    // save the value we just retrieved to the corresponding member variable
    m_ConfigData.nPriorityCode = (int)dwDataBuf;



Exit_Function:

    if (NULL != hKey)
        {
        RegCloseKey(hKey);
        }

}  // end of "CScanDeliverConfiguration::ReadSarcPriorityCode"


