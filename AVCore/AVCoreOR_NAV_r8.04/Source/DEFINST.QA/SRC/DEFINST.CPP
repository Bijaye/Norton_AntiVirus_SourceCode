///////////////////////////////////////////////////////////////////////////////
// Copyright 1997 Symantec Corp. / Peter Norton Product Group
///////////////////////////////////////////////////////////////////////////////
//
// File:
//
//      definst.cpp
//
// Description:
//
//      Sample application to install virus definition files using the
//      Virus Definition Library API.
//
//      Installing virus definition files is normally done at application
//      install time using InstallShield or a similar application installation
//      facility. This program is intended be used to only to install virus 
//      definition files for an application under developement.
//
//      DEFINST.EXE is a Win32 console mode application. The executeable
//      was built using MSVC 4.2.
//
// Functions:
//
//      main
//      DefInfo
//      DisplayHelp
//      InstallDefs
//      UninstallDefs
//
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// Include Files
/////////////////////////////////////////////////////////////////////////////
#include "windows.h"
#include "stdio.h"
#include "string.h"
#include "tchar.h"

#include "definst.h"
#include "navdefs.h"


/////////////////////////////////////////////////////////////////////////////
// Local Function Prototypes
/////////////////////////////////////////////////////////////////////////////
WORD DefInfo        ( LPSTR lpCmdLineParam );
void DisplayHelp    ( void );
WORD InstallDefs    ( LPSTR lpCmdLineParam );
WORD UninstallDefs  ( LPSTR lpCmdLineParam );


/////////////////////////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////////////////////////
WORD main (int argc, char *argv[])
{
    auto WORD       wRetValue = DEFINST_OK;


    if (argc == 1)
    {
        DisplayHelp();
        wRetValue = DEFINST_HELP;
    }
    else
    {
        printf("Sample Virus Definition Installer\n\n");

        if ( strstr(strupr(argv[1]), DEFCMD_INFO) != NULL )
            wRetValue = DefInfo ( argv[1] );
        else
        {
            if ( strstr(strupr(argv[1]), DEFCMD_UNINSTALL) != NULL )
                wRetValue = UninstallDefs ( argv[1] );
            else
                wRetValue = InstallDefs ( argv[1] );
        }
    }

    return wRetValue;
}



/////////////////////////////////////////////////////////////////////////////
// Local Functions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
// Functions:   DefInfo
//
// Description: Get information about the current virus definition files.
//
// Input:       lpCmdLineParam 
//
// Output:      DEFINST_OK        - the defs were successfully installed
//              DEFINST_NOINIT    - the Virus Definition Library could not
//                                  be initialized 
//              DEFINST_NOINFO    - could not get virus def info.
//
/////////////////////////////////////////////////////////////////////////////
WORD DefInfo ( LPSTR lpCmdLineParam )
{
    auto WORD       wRetValue = DEFINST_OK;
    auto HVIRUSDEF  hVirusDef;
    auto LPSTR      lpAppID;
    auto WORD       wYear, wMonth, wDay;
    auto DWORD      dwRev;
    auto char       szVirusDefDir[MAX_PATH_SIZE];

    lpAppID = _strinc(strchr(lpCmdLineParam, ':'));

    //
    // Initialize the Virus Definition Library
    //
    if ( VirusDefInit(&hVirusDef) == FALSE )
    {
        printf("\nVirus definition library failed to initialize.\n");
        return DEFINST_NOINIT;
    }
    else
    {
        if ( VirusDefInitWindowsApp(hVirusDef, lpAppID) == FALSE )
        {
            printf("Init for Virus Definition info failed.\n");
            wRetValue = DEFINST_NOINIT;
        }        
        else
        {
            if ( VirusDefGetCurrent(hVirusDef, (LPSTR)&szVirusDefDir[0], MAX_PATH_SIZE ) == FALSE )
            {
                printf("Error getting Virus Definition directory information.\n");
                wRetValue = DEFINST_NOINFO;
            }
            else
            {
                if ( VirusDefGetCurrentDate(hVirusDef, &wYear, &wMonth, &wDay, &dwRev ) == FALSE )
                {
                    printf("Error getting Virus Definition date information.\n");
                    wRetValue = DEFINST_NOINFO;
                }
                else
                {
                    printf("Virus Definition Info\n");
                    printf("----------------------------------------\n");
                    printf("AppID:     %s\n", lpAppID);
                    printf("Date:      %d/%d/%d\n", wMonth, wDay, wYear);
                    printf("Revision:  %d\n", dwRev);
                    printf("Directory: %s\n", &szVirusDefDir[0]);
                }
            }
        }
    }


    //
    // Close the Virus Definition Library
    //
    VirusDefClose(hVirusDef);
 
    return wRetValue;
}



/////////////////////////////////////////////////////////////////////////////
//
// Functions:   DisplayHelp
//
// Description: Display a help message
//
// Input:       None
//
// Output:      None
//
/////////////////////////////////////////////////////////////////////////////
void DisplayHelp ( void ) 
{
    printf("\n");
    printf("Sample Virus Definition Installer\n");
    printf("Usage:   definst [virus_def_dir | /u:AppID | /i:AppID] \n");
    printf("\n");
    printf("Options: virus_def_dir: directory where the virus definition files will be\n");
    printf("                        installed from.\n");
    printf("         /U:AppID       Uninstall the virus definition files used by the\n");
    printf("                        specified Application Identifier.\n");
    printf("         /I:AppID       Get information about the the virus definition files\n");
    printf("                        in use by the specified Application Identifier.\n\n");

    return;
}



/////////////////////////////////////////////////////////////////////////////
//
// Function:    InstallDefs
//
// Description: Install the virus definition files to the directory
//              specified on the command line. Write the AppID to the
//              definst.ini file and display appropriate messages to
//              stdout. This file is here **ONLY** to illustrate one 
//              possible method for keeping the AppID available. It is 
//              up to the application developer to decide the best method
//              for their particular case.
//              
// Input:       lpCmdLineParam 
//
// Output:      DEFINST_OK        - the defs were successfully installed
//              DEFINST_NOINIT    - the Virus Definition Library could not
//                                  be initialized 
//              DEFINST_NOINSTALL - the install function failed.
//
/////////////////////////////////////////////////////////////////////////////
WORD InstallDefs ( LPSTR lpCmdLineParam )
{
    auto WORD       wRetValue = DEFINST_OK;
    auto HVIRUSDEF  hVirusDef;
    auto char       szAppID[MAX_APPID_SIZE];
    auto char       szIniFile[MAX_APPID_SIZE];

    //
    // Initialize the Virus Definition Library
    //
    if ( VirusDefInit(&hVirusDef) == FALSE )
    {
        printf("\nVirus definition library failed to initialize.\n");
        printf("Definitions were not installed.\n");
        wRetValue = DEFINST_NOINIT;
    }

    //
    // Install the Virus Definition Files to the directory 
    // specified on he command line.
    //
    if ( VirusDefInstall(hVirusDef, szAppID, MAX_APPID_SIZE, NULL, lpCmdLineParam) == VIRDEF_OK )
    {     
        //
        // Build the full path name for definst.ini
        //
        GetCurrentDirectory(MAX_PATH_SIZE, szIniFile);
        strcat(szIniFile, "\\");
        strcat(szIniFile, DEFINST_INI_FILE);

        //
        // Write the AppID to the definst.ini file
        //
        if ( !WritePrivateProfileString( DEFINST_APPID_SECT,
                                         DEFINST_APPID_KEY,
                                         szAppID,
                                         szIniFile) )
        {
            printf("\nVirus definitions successfully installed, but the initialization \n");
            printf("file %s could not be created. AppID is %s\n\n", DEFINST_INI_FILE, &szAppID[0]);
        }
        else
            printf("\nVirus definitions successfully installed. AppID is %s\n", &szAppID[0]);
    }
    else
    {
        printf("\nVirus definitions failed to install.\n");
        wRetValue = DEFINST_NOINSTALL;
    }

    //
    // Close the Virus Definition Library
    //
    VirusDefClose(hVirusDef);
 
    return wRetValue;
}


/////////////////////////////////////////////////////////////////////////////
//
// Function:    UninstallDefs
//
// Description: Uninstall the virus definition files associated with a
//              specific AppID on the command line. 
//
// Input:       lpCmdLineParam 
//
// Output:      DEFINST_OK        - the defs were successfully installed
//              DEFINST_NOINIT    - the Virus Definition Library could not
//                                  be initialized 
//              DEFINST_NOINSTALL - the install function failed.
//
/////////////////////////////////////////////////////////////////////////////
WORD UninstallDefs ( LPSTR lpCmdLineParam )
{
    auto WORD       wRetValue = DEFINST_OK;
    auto HVIRUSDEF  hVirusDef;
    auto LPSTR      lpAppID;

    lpAppID = _strinc(strchr(lpCmdLineParam, ':'));

    //
    // Initialize the Virus Definition Library
    //
    if ( VirusDefInit(&hVirusDef) == FALSE )
    {
        printf("\nVirus definition library failed to initialize.\n");
        printf("Definitions were not installed.\n");
        return DEFINST_NOINIT;
    }
    else
    {
        if ( VirusDefUninstall(hVirusDef, lpAppID) == FALSE )
        {
            printf("\nAppID %s failed to uninstalled.\n", lpAppID);
            wRetValue = DEFINST_NOUNINSTALL;
        }
        else
            printf("\nAppID %s was successfully uninstalled as a Virus Definition user.\n", 
                   lpAppID);

        //
        // Close the Virus Definition Library
        //
        VirusDefClose(hVirusDef);
    }

    return wRetValue;
}
