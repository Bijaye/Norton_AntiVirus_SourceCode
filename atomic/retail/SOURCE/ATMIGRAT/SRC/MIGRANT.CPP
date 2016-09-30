/////////////////////////////////////////////////////////////////////
// File: Migrant.cpp
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ATMIGRAT/VCS/MIGRANT.CPv   1.1   28 Sep 1998 11:36:50   MBROWN  $
//
// Description:
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGRAT/VCS/MIGRANT.CPv  $
// 
//    Rev 1.1   28 Sep 1998 11:36:50   MBROWN
// Removed options modification, added write to Handled section of inf, modify shell links, move prog groups, etc.
// 
//    Rev 1.0   26 May 1998 14:31:28   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////


#include    <windows.h>
#include    <windowsx.h>
#include    "global.h"
#include    "Migrant.h"
#include    "Utils.h"


#include    <stdio.h>
BOOL    MyWritePrivateProfileString(LPSTR  szSectionName, LPSTR  szValueName
                                                        , LPSTR  szValue
                                                        , LPSTR  szMigrateInfFile);

// constructor
CMigrant::CMigrant(void)
{

    Initialize();
}


// destructor
CMigrant::~CMigrant(void)
{

}



// ==== CopyAppFiles ======================================================
//
//  This function is responsible for copying files for the three main target
//  directories to the target system.  The three main directories are:
//
//      1) the main application directory
//      2) the "Program Files\Symantec" directory
//      3) the "Common Files\Symantec Shared" directory
//
//  Note that the CMigrant class has three static members to store the array
//  of files for each of these target directories, and they must be initialized
//  before this function is called.
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CMigrant::CopyAppFiles(void)
{
    auto    BOOL        bResult = TRUE;
    auto    CHAR*       filename;
    auto    CWString    szDestFile;
    auto    CWString    szSourceFile;
    auto    int         index;


    LTrace("BEGIN CMigrant::CopyAppFiles");

    // if we have no working directory, something's very wrong!
    if (TRUE == m_szMigWorkingDir.IsEmpty())
        {
        LTrace("  TRUE == m_szMigWorkingDir.IsEmpty()");
        bResult = FALSE;
        goto  Exit_Function;
        }


    // copy over files that belong in the main application directory
    if (FALSE == m_szAppInstallDir.IsEmpty())
        {
        index = 0;
        filename = m_szInstallDirFiles[index];
        while (NULL != *filename)
            {
            szSourceFile = m_szMigWorkingDir;
            szSourceFile += "\\";
            szSourceFile += filename;

            szDestFile = m_szAppInstallDir;
            szDestFile += "\\";
            szDestFile += filename;

            LTrace("  About to call CopyFile(%s, %s, FALSE)", (LPSTR)szSourceFile, (LPSTR)szDestFile);
            if (0 == CopyFile(szSourceFile, szDestFile, FALSE))
                {
                LTrace("  Failed to copy %s to %s", (LPSTR)szSourceFile, (LPSTR)szDestFile);
                bResult = FALSE;
                }

            ++index;
            filename = m_szInstallDirFiles[index];
            }
        }


    // copy over files that belong in the "Common Files\Symantec Shared" directory
    if (FALSE == m_szCommonFilesSymantecDir.IsEmpty())
        {
        index = 0;
        filename = m_szCommonFilesSymantecFiles[index];
        while (NULL != *filename)
            {
            szSourceFile = m_szMigWorkingDir;
            szSourceFile += "\\";
            szSourceFile += filename;

            szDestFile = m_szCommonFilesSymantecDir;
            szDestFile += "\\";
            szDestFile += filename;

            LTrace("  About to call CopyFile(%s, %s, FALSE)", (LPSTR)szSourceFile, (LPSTR)szDestFile);
            if (0 == CopyFile(szSourceFile, szDestFile, FALSE))
                {
                LTrace("  Failed to copy %s to %s", (LPSTR)szSourceFile, (LPSTR)szDestFile);
                bResult = FALSE;
                }

            ++index;
            filename = m_szCommonFilesSymantecFiles[index];
            }
        }


    // copy over files that belong in the "Common Files\Symantec Shared" directory
    if (FALSE == m_szProgramFilesSymantecDir.IsEmpty())
        {
        index = 0;
        filename = m_szProgramFilesSymantecFiles[index];
        while (NULL != *filename)
            {
            szSourceFile = m_szMigWorkingDir;
            szSourceFile += "\\";
            szSourceFile += filename;

            szDestFile = m_szProgramFilesSymantecDir;
            szDestFile += "\\";
            szDestFile += filename;

            LTrace("  About to call CopyFile(%s, %s, FALSE)", (LPSTR)szSourceFile, (LPSTR)szDestFile);
            if (0 == CopyFile(szSourceFile, szDestFile, FALSE))
                {
                LTrace("  Failed to copy %s to %s", (LPSTR)szSourceFile, (LPSTR)szDestFile);
                bResult = FALSE;
                }

            ++index;
            filename = m_szProgramFilesSymantecFiles[index];
            }
        }



Exit_Function:
    LTrace("END CMigrant::CopyAppFiles");
    return  (bResult);

}  // end of "CMigrant::CopyAppFiles"



// ==== CopyUpgradeDirs ===================================================
//
//  This function copies the upgrade directories that the Microsoft upgrade
//  program generates for our migration DLL into member variables.  (These
//  are the "Working" and "Source" directories.)
//
//  Input:
//      szWorkingDir    -- this is the temporary working directory the MS
//                         upgrade program uses to temporarily store all
//                         of our migration files
//
//      szSourceDir     -- this is the location of the upgrade program being
//                         executed (usually the CD-ROM)
//
//  Output: ERROR_SUCCESS
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

DWORD   CMigrant::CopyUpgradeDirs(LPCSTR  szWorkingDir, LPCSTR  szSourceDir)
{
    LTrace("BEGIN CMigrant::CopyUpgradeDirs");
    LTrace("  szWorkingDir = %s, szSourceDir = %s", szWorkingDir, szSourceDir);

    // make global copies of the working and source directories
    m_szMigWorkingDir = (LPSTR)szWorkingDir;
    m_szMigSourceDir = (LPSTR)szSourceDir;
    LTrace("  About to return, m_szMigWorkingDir = %s, m_szMigSourceDir = %s"
                                                        , (LPSTR)m_szMigWorkingDir
                                                        , (LPSTR)m_szMigSourceDir);
    LTrace("END CMigrant::CopyUpgradeDirs");
    return (ERROR_SUCCESS);

}  // end of "CMigrant::CopyUpgradeDirs"



// ==== GetMigInfPath =====================================================
//
//  This function generates a complete path to the "migrate.inf" file
//  (located in the working directory) and saves it in a member variable.
//
//  Input:  nothing
//  Output: a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CMigrant::GetMigInfPath(void)
{
    auto    BOOL    bResult = TRUE;

    LTrace("BEGIN CMigrant::GetMigInfPath");
    LTrace("  m_szMigWorkingDir = %s", (LPSTR)m_szMigWorkingDir);

    if (TRUE == m_szMigWorkingDir.IsEmpty())
        {
        bResult = FALSE;
        }
    else
        {
        m_szMigrateInf = m_szMigWorkingDir;
        m_szMigrateInf += "\\migrate.inf";
        }

    LTrace("  m_szMigrateInf = %s", (LPSTR)m_szMigrateInf);
    LTrace("END CMigrant::GetMigInfPath");
    return  (bResult);

}  // end if "CMigrant::GetMigInfPath"



// ==== Initialize ========================================================
//
//  This function is used to help initialize the CMigrant object.
//  (Currently we do nothing here...)
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CMigrant::Initialize(void)
{
    auto    BOOL        bResult = TRUE;
    return (bResult);

}  // end of "CMigrant::Initialize"




// ==== UpdateMigInfIncompatibleSection ===================================
//
//  This function is used to create an incompatibility message so that the
//  MS setup program will display it to the user before the migration
//  actually begins.  This gives us a chance to hold the customer's hand
//  a little before the system is changed.
//
//  Input:
//      resID   -- the resource ID of the message string to be displayed
//
//  Output:
//      a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CMigrant::UpdateMigInfIncompatibleSection(DWORD  resID)
{
    auto    BOOL        bResult = TRUE;
    auto    CWString    szSetupReportMessageBuf(MAX_MIGRATE_STRBUF_SIZE);
    auto    DWORD       dwResult = 0;

    LTrace("BEGIN CMigrant::UpdateMigInfIncompatibleSection");

    // fetch the setup report text from the DLL resource
    LoadString(g_hInst, resID, (LPSTR)szSetupReportMessageBuf, MAX_MIGRATE_STRBUF_SIZE);
    if (0 == MyWritePrivateProfileString("Incompatible Messages", m_szProductID
                                                        , (LPSTR)szSetupReportMessageBuf
                                                        , m_szMigrateInf))
        {
        LTrace("  WritePrivateProfileString failed, m_szProductID = %s, "
                            "szSetupReportMessageBuf = %s, g_MigrateDotInf = %s"
                                                    , (LPSTR)m_szProductID
                                                    , (LPSTR)szSetupReportMessageBuf
                                                    , (LPSTR)m_szMigrateInf);
        bResult = FALSE;
        dwResult = GetLastError();
        LTrace("  GetLastError() = %ld", dwResult);
        }
    else
        {
        UpdateMigInfMessageObjectSection(m_szProductID);
        }

    LTrace("END CMigrant::UpdateMigInfIncompatibleSection");
    return  (bResult);

}  // end of "CMigrant::UpdateMigInfIncompatibleSection"



// ==== UpdateMigInfMessageObjectSection ==================================
//
//  This function writes a string to the message object section in the
//  "migrate.inf" file.  In order to display an incompatibility message
//  before the actual upgrade, we must write an entry to this section.
//  Currently it really has no purpose other than providing a placeholder.
//  See the "migext.doc" document for more information about how to
//  customize this section.
//
//  Input:
//      szSectionName   -- the user-defined name of the section to write
//                         to (usually the AppID member).
//
//
//  Output:
//      a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    CMigrant::UpdateMigInfMessageObjectSection(LPSTR  szSectionName)
{
    auto    BOOL        bResult = TRUE;
    auto    DWORD       dwResult = 0;

    LTrace("BEGIN CMigrant::UpdateMigInfMessageObjectSection");
    if (0 == WritePrivateProfileString(szSectionName
                                , "C:\\Program Files\\Fake Directory", "Directory"
                                , (LPSTR)m_szMigrateInf))
        {
        LTrace("  WritePrivateProfileString failed, szSectionName = %s", szSectionName);
        bResult = FALSE;
        dwResult = GetLastError();
        LTrace("  GetLastError() = %ld", dwResult);
        }

    LTrace("END CMigrant::UpdateMigInfMessageObjectSection");
    return  (bResult);

}  // end of "CMigrant::UpdateMigInfMessageObjectSection"



// ==== UpdateMigInfHandledSection ===================================
//
//  This function is used to write file and path names to the [Handled]
//  section of migrate.inf. This tells not to complain about our app if
//  it finds these files on the drive.
//
//  Input:
//      void
//
//  Output:
//      a value of TRUE if all goes well, FALSE otherwise
//
// ========================================================================
//  Function created: 9/98, MBROWN
// ========================================================================

BOOL    CMigrant::UpdateMigInfHandledSection(void)
{
    auto    BOOL        bResult = TRUE;
    auto    CWString    szSystemDir;
    auto    CWString    szFileName;
    auto    DWORD       dwResult = 0;

    LTrace("BEGIN CMigrant::UpdateMigInfHandledSection");

    // Write Install path to handled section
    if (0 == WritePrivateProfileString("Handled", m_szAppInstallDir
                                                , "Directory"
                                                , m_szMigrateInf))
        {
        LTrace("  WritePrivateProfileString failed, m_szAppInstallDir = %s, "
                                                       "g_MigrateDotInf = %s"
                                                    , (LPSTR)m_szAppInstallDir
                                                    , (LPSTR)m_szMigrateInf);
        bResult = FALSE;
        dwResult = GetLastError();
        LTrace("  GetLastError() = %ld", dwResult);
        }

    
    // Write Symantec path to handled section
    if (0 == WritePrivateProfileString("Handled", m_szProgramFilesSymantecDir
                                                , "Directory"
                                                , m_szMigrateInf))
        {
        LTrace("  WritePrivateProfileString failed, m_szProgramFilesSymantecDir = %s, "
                                                       "g_MigrateDotInf = %s"
                                                    , (LPSTR)m_szProgramFilesSymantecDir
                                                    , (LPSTR)m_szMigrateInf);
        bResult = FALSE;
        dwResult = GetLastError();
        LTrace("  GetLastError() = %ld", dwResult);
        }
    
    // Write Common Files path to handled section
    if (0 == WritePrivateProfileString("Handled", m_szCommonFilesSymantecDir
                                                , "Directory"
                                                , m_szMigrateInf))
        {
        LTrace("  WritePrivateProfileString failed, m_szCommonFilesSymantecDir = %s, "
                                                       "g_MigrateDotInf = %s"
                                                    , (LPSTR)m_szCommonFilesSymantecDir
                                                    , (LPSTR)m_szMigrateInf);
        bResult = FALSE;
        dwResult = GetLastError();
        LTrace("  GetLastError() = %ld", dwResult);
        }

    // Write symevent file names to handled section
    GetSystemDirectory( szSystemDir, MAX_PATH - 1 );
    szSystemDir += "\\";

    // Write symevent.vxd file name
    szFileName = szSystemDir;
    szFileName += SYMEVENT_VXD_NAME;
    if (0 == WritePrivateProfileString("Handled", szFileName
                                                , "File"
                                                , m_szMigrateInf))
        {
        LTrace("  WritePrivateProfileString failed, szFileName = %s, "
                                                "g_MigrateDotInf = %s"
                                                    , (LPSTR)szFileName
                                                    , (LPSTR)m_szMigrateInf);
        bResult = FALSE;
        dwResult = GetLastError();
        LTrace("  GetLastError() = %ld", dwResult);
        }

    // Write symevnt1.dll file name
    szFileName = szSystemDir;
    szFileName += SYMEVENT_DLL_NAME;
    if (0 == WritePrivateProfileString("Handled", szFileName
                                                , "File"
                                                , m_szMigrateInf))
        {
        LTrace("  WritePrivateProfileString failed, szFileName = %s, "
                                                "g_MigrateDotInf = %s"
                                                    , (LPSTR)szFileName
                                                    , (LPSTR)m_szMigrateInf);
        bResult = FALSE;
        dwResult = GetLastError();
        LTrace("  GetLastError() = %ld", dwResult);
        }
    
    // Write s32evnt1.dll file name
    szFileName = szSystemDir;
    szFileName += S32EVENT_DLL_NAME;
    if (0 == WritePrivateProfileString("Handled", szFileName
                                                , "File"
                                                , m_szMigrateInf))
        {
        LTrace("  WritePrivateProfileString failed, szFileName = %s, "
                                                "g_MigrateDotInf = %s"
                                                    , (LPSTR)szFileName
                                                    , (LPSTR)m_szMigrateInf);
        bResult = FALSE;
        dwResult = GetLastError();
        LTrace("  GetLastError() = %ld", dwResult);
        }

    LTrace("END CMigrant::UpdateMigInfHandledSection");
    return  (bResult);

}  // end of "CMigrant::UpdateMigInfHandledSection"



// ==== MyWritePrivateProfileString =======================================
//
//  This function writes a string out to the INF file.  Apparently, the
//  traditional "WritePrivateProfileString" function was truncating the
//  setup report string (seemed to only write out MAX_PATH chars).  This
//  function is a kludge to get around that limitation.  Note that it
//  does NOT behave as the traditional function, this version simply
//  appends a section/string to the end of the INF file.
//
//  Input:
//      szSectionName       -- the user-defined name of the section to write
//                             to
//      szValueName         -- the value name
//      szValue             -- the string value
//      szMigrateInfFile    -- the name of the migrate.inf file
//
//  Output:
//      a value of TRUE if all goes well, FALSE otherwise
// ========================================================================
//  Function created: 12/97, SEDWARD
// ========================================================================

BOOL    MyWritePrivateProfileString(LPSTR  szSectionName, LPSTR  szValueName
                                                        , LPSTR  szValue
                                                        , LPSTR  szMigrateInfFile)
{
    auto    BOOL        bResult = FALSE;
    auto    CWString    szTempBuf;
    auto    FILE*       filePtr;

    filePtr = fopen(szMigrateInfFile, "at");
    if (filePtr)
        {
        // write out the section name
        szTempBuf = "\n[";
        szTempBuf = szTempBuf + szSectionName;
        szTempBuf = szTempBuf + "]\n";
        fprintf(filePtr, (LPSTR)szTempBuf);

        // write out the value name and value
        szTempBuf = szValueName;
        szTempBuf = szTempBuf + "=";
        fprintf(filePtr, (LPSTR)szTempBuf);
        fprintf(filePtr, "%s\n\n", szValue);
        fclose(filePtr);
        bResult = TRUE;
        }

    return (bResult);

}  // end of "MyWritePrivateProfileString"