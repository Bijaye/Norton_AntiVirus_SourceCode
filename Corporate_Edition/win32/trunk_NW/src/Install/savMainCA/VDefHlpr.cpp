// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// VDefHlpr.cpp - Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "VDefHlpr.h"
#include "savMainCA.h"
#include <TCHAR.H>
#include "al.h"
#include <direct.h>
#define NAVAPPIDS_BUILDING
#define NAVAPPIDS_WANT_ARRAY

#include "NavAppIDs.h"
#include "resource.h"
#include "uncab.h"
#include "SymSaferRegistry.h"
#include "ClientReg.h"

#define PROPERTY_EXTRACTTARGETDIR		"COPYTARGETFOLDER"
#define PROPERTY_VIRUSDEFFOLDER			"VIRUSDEFFOLDER"

static UINT MsiLogMessage( MSIHANDLE hInstall, LPCTSTR szString )
{
    UINT uiRetVal = ERROR_SUCCESS;

    MSIHANDLE hRec = MsiCreateRecord(1);
    if (hRec)
    {
        TCHAR szLogLine[MAX_PATH*2];

        vpstrncpy    (szLogLine, "VDefHlpr: ", sizeof (szLogLine));
        vpstrnappend (szLogLine, szString,     sizeof (szLogLine));

        // Use OutputDebugString...
        OutputDebugString(szLogLine);

        // ...as well as the log file.
        MsiRecordClearData(hRec);
        MsiRecordSetString(hRec, 0, szLogLine);
        MsiProcessMessage(hInstall, INSTALLMESSAGE_INFO, hRec);
        MsiCloseHandle(hRec);
    }

    return uiRetVal;
}

//////////////////////////////////////////////////////////////////////////
// Function: RegisterDefAppIds()
//
// Description: This function calls CDefUtils::UseNewestDefs() for each
// of our AppIds.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 03/09/2005 - Jim Brennan
//////////////////////////////////////////////////////////////////////////
static UINT RegisterDefAppIds(MSIHANDLE hInstall, LPTSTR szTempBuf, DWORD nNumTempBufBytes)
{
    UINT    nRtn       = ERROR_SUCCESS;
    LPCTSTR szAppIds[] = {g_szNavAppIdNavDef, g_szNavAppIdNavCE, g_szSAVCorpID2};

    for (unsigned int nIndex = 0; nIndex < sizeof(szAppIds)/sizeof(szAppIds[0]); ++nIndex)
    {
        CDefUtils objRegistrar;

        if (!objRegistrar.InitInstallApp(szAppIds[nIndex]))
        {
            vpsnprintf(szTempBuf, nNumTempBufBytes, "Error registering virus definition application ID %s.", szAppIds[nIndex]);
            MsiLogMessage(hInstall, szTempBuf);
            nRtn = ERROR_INSTALL_FAILURE;
        }
        if (!objRegistrar.UseNewestDefs())
        {
            vpsnprintf(szTempBuf, nNumTempBufBytes, "Error switching virus definition application ID %s to latest definitions.", szAppIds[nIndex]);
            MsiLogMessage(hInstall, szTempBuf);
            nRtn = ERROR_INSTALL_FAILURE;
        }
        else
        {
            vpsnprintf(szTempBuf, nNumTempBufBytes, "Successfully registered application ID %s.", szAppIds[nIndex]);
            MsiLogMessage(hInstall, szTempBuf);
        }
    }
    return nRtn;
}

//////////////////////////////////////////////////////////////////////////
// Function: DefsDirExists()
//
// Description: This function returns true if the definitions directory
// exists on the system.
//
// Return Values: true if successful.
//
//////////////////////////////////////////////////////////////////////////
// 03/09/2005 - Jim Brennan
//////////////////////////////////////////////////////////////////////////
static bool DefsDirExists()
{
    HKEY hKey;
    if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, szReg_Key_Sym_InstalledApps, 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS)
    {
        SymSaferRegKeyJanitor objKeyJan (hKey);
        
        TCHAR szDefsDir[MAX_PATH + 1];
        DWORD dwNumBytes = sizeof (szDefsDir);
        if (SymSaferRegQueryStringValue (hKey, "AvengeDefs", szDefsDir, &dwNumBytes) == ERROR_SUCCESS)
        {
            if (GetFileAttributes(szDefsDir) == FILE_ATTRIBUTE_DIRECTORY)
                return true;
        }
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MsiInstallDefs2()
//
// Description: This function handles installing (DefUtils) a given
// set of Virus defs.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 06/16/2004 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiInstallDefs2( MSIHANDLE hInstall )
{
    TCHAR           szSrcPath[2*MAX_PATH+1]				= {0};
    TCHAR           szInstallSrcPath[2*MAX_PATH+1]		= {0};
    TCHAR           szDestPath[2*MAX_PATH+1]			= {0};
    TCHAR           szFileName[MAX_PATH]                = {0};
    TCHAR           errorMessage[2*MAX_PATH+1]          = {0};
    TCHAR           strCADTemp[2*MAX_PATH+42]           = {0}; // Two paths, two commas, and a guid...
	bool			bMigrate7x							= false;
    bool            defsUpdated                         = false;
    bool            bIntegrate                          = false;
    CString         strHubName;
    CString         strZipName;
    CDefUtils       duSAV;
    INT             dRet                                = 0;
    INT             commalocation                       = 0;
    DWORD           returnVal                           = ERROR_SUCCESS;
    DWORD           strCADTempLen                       = sizeof(strCADTemp);

    returnVal = MsiGetProperty(hInstall, "CustomActionData", strCADTemp, &strCADTempLen);
    if (returnVal != ERROR_SUCCESS)
    {
        vpsnprintf(errorMessage, sizeof (errorMessage), "Error %d retrieving CustomActionData property from MSI.", returnVal);
		MsiLogMessage(hInstall, errorMessage);
		return ERROR_SUCCESS;
	}

	vpsnprintf( errorMessage, sizeof(errorMessage), "MsiInstallDefs2: CustomActionData = %s", strCADTemp );
	MsiLogMessage( hInstall, errorMessage );

	TCHAR *pstrTemp = strtok( strCADTemp, _T(",") );  // First parameter...
	if( pstrTemp )
	{
		vpstrncpy( szSrcPath, pstrTemp, sizeof(szSrcPath) );
	}
	// We will now install the virusdefs from the new temp folder, forget installing from the original source
	// copy the temp path szSrcPath to the szInstallSrcPath
	vpstrncpy( szInstallSrcPath, szSrcPath, sizeof(szInstallSrcPath) );
	pstrTemp = strtok( NULL, _T(",") ); // Second parameter...
	if( pstrTemp )
	{
		bMigrate7x = true;
		MsiLogMessage( hInstall, _T("MsiInstallDefs2: Migrating 7.x") );
	}

    vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szInstallSrcPath %s", szInstallSrcPath);
    MsiLogMessage(hInstall, errorMessage);

    vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szSrcPath %s", szSrcPath);
    MsiLogMessage(hInstall, errorMessage);

    AddSlash( szSrcPath, sizeof (szSrcPath));
    vpstrncpy ( szDestPath, szSrcPath, sizeof (szDestPath));
    vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szDestPath %s", szDestPath);
    MsiLogMessage(hInstall, errorMessage);
    vpstrnappend( szDestPath, _T(VIRUS_DEF_TEMP) , sizeof (szDestPath));
    dRet = _tmkdir( szDestPath );
    vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szDestPath %s", szDestPath);
    MsiLogMessage(hInstall, errorMessage);

	if( 0 == dRet ) // Folder created or some error
	{
		vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder - _tmkdir : szDestPath szFile %s", szDestPath);
		MsiLogMessage(hInstall, errorMessage);
	}
	else if ( EEXIST == errno ) // if the folder already exists then continue
	{
		vpsnprintf(errorMessage, sizeof (errorMessage), "Error TempFolder (%s) - _tmkdir : szDestPath %s", strerror( errno ),szDestPath);
		MsiLogMessage(hInstall, errorMessage);
	}
	else	// some unhandled error, log it and end install
	{
		vpsnprintf(errorMessage, sizeof (errorMessage), "Error TempFolder (%s) - _tmkdir : szDestPath %s", strerror( errno ),szDestPath);
		MsiLogMessage(hInstall, errorMessage);
		return ERROR_SUCCESS;
	}
    AddSlash( szDestPath, sizeof (szDestPath));
    vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szDestPath szFile %s", szDestPath);
    MsiLogMessage(hInstall, errorMessage);

    // loadstring the BinHub temp name
    strHubName.LoadString( IDS_BIN_HUB );
    strZipName.LoadString( IDS_ZIP );
    vpstrnappendfile (szInstallSrcPath, strHubName, sizeof (szInstallSrcPath)); // + vdefhub
    vpstrnappend     (szInstallSrcPath, strZipName, sizeof (szInstallSrcPath)); // + .zip
    vpsnprintf(errorMessage, sizeof (errorMessage), "szInstallSrcPath szFile% s", szInstallSrcPath);
    MsiLogMessage(hInstall, errorMessage);

    vpsnprintf(errorMessage, sizeof (errorMessage), "szDestPath path %s", szDestPath);
    MsiLogMessage(hInstall, errorMessage);

    vpsnprintf(errorMessage, sizeof (errorMessage), "Findit File %s", szInstallSrcPath);
    MsiLogMessage(hInstall, errorMessage);
	// Verify the def zip file exists
	if (Findit (hInstall, szInstallSrcPath, szFileName, sizeof (szFileName)))
    {
        vpsnprintf(errorMessage, sizeof (errorMessage), "Findit %s, virus defs found %s", szInstallSrcPath, szFileName);
        MsiLogMessage(hInstall, errorMessage);
    }
    else 
    {
        vpsnprintf(errorMessage, sizeof (errorMessage), "Findit %s, no Vdefs found, skip def install.", szInstallSrcPath, szFileName);
        MsiLogMessage(hInstall, errorMessage);
        // If the Defs dir exists at this stage, we are upgrading from a previously
        // installed SAV. Make sure our AppIds are registered with the existing
        // definitions. This ensures the defs directory remains after we finish the
        // install.
        if (DefsDirExists ())
            RegisterDefAppIds (hInstall, errorMessage, sizeof (errorMessage));

        return ERROR_SUCCESS;
    }

	if(UnPakZip(hInstall, szInstallSrcPath, szDestPath))
	{
		vpsnprintf(errorMessage, sizeof (errorMessage), "UnPakZip %s complete.", szInstallSrcPath);
		MsiLogMessage(hInstall, errorMessage);
	}
	else
	{
		vpsnprintf(errorMessage, sizeof (errorMessage), "UnPakZip %s not complete, fail install.\n", szInstallSrcPath);
		MsiLogMessage(hInstall, errorMessage);
		return ERROR_SUCCESS;
	}

    // Register the primary app ID
	if (!duSAV.InitInstallApp(g_szNavAppIdNavCE))
	{
		vpsnprintf(errorMessage, sizeof (errorMessage), "Error registering virus definition application ID %s.", g_szNavAppIdNavCE);
		MsiLogMessage(hInstall, errorMessage);
		return ERROR_SUCCESS;
	}
	else
	{
		MsiLogMessage(hInstall, "InitInstallApp success");
	}
	// Integrate our definitions
	if (!duSAV.WillIntegrateDir(szDestPath, &bIntegrate))
	{
		vpsnprintf(errorMessage, sizeof (errorMessage), "Error determining if virus definitions will be integrated.");
		MsiLogMessage(hInstall, errorMessage);
		return ERROR_SUCCESS;
	}
	else
	{
		MsiLogMessage(hInstall, "WillIntegrateDir success");
	}

    //********************************************************************
    // DefUtils: PreDefUpdate(), CopyAllDefs(), PostDefUpdate() **********
    //********************************************************************

    if( bIntegrate || bMigrate7x )
    {
        TCHAR VDefszDestPath[MAX_PATH] = {0};

		MsiLogMessage(hInstall, "bIntegrate TRUE");
		if (!duSAV.PreDefUpdate(VDefszDestPath, sizeof(VDefszDestPath), DU_INSTALL))
		{
			vpsnprintf(errorMessage, sizeof (errorMessage), "Error preparing to update virus definitions.");
			MsiLogMessage(hInstall, errorMessage);
			return ERROR_SUCCESS;
		}
		vpsnprintf(errorMessage, sizeof (errorMessage), "CopyAllDefs from %s to %s", (LPCTSTR) szDestPath, (LPCTSTR) VDefszDestPath);
		MsiLogMessage(hInstall, errorMessage);

		if (!duSAV.CopyAllDefs(szDestPath, VDefszDestPath))
		{
			vpsnprintf(errorMessage, sizeof (errorMessage), "Error copying definition files from %s to %s", (LPCTSTR) szDestPath, (LPCTSTR) VDefszDestPath);
			MsiLogMessage(hInstall, errorMessage);
			return ERROR_SUCCESS;
		}

		if (!duSAV.PostDefUpdate( bMigrate7x ))
		{
			vpsnprintf(errorMessage, sizeof (errorMessage), "Error moving new virus definitions to shared directory.");
			MsiLogMessage(hInstall, errorMessage);
			return ERROR_SUCCESS;
		}
	}
	else
	{
		MsiLogMessage(hInstall, "Not integrating virus definitions.");
	}

    //********************************************************************
    // DefUtils: UseNewestDefs() *****************************************
    //********************************************************************

	if (!duSAV.UseNewestDefs(&defsUpdated))
	{
		vpsnprintf(errorMessage, sizeof (errorMessage), "Error updating to the included virus definitions.");
		MsiLogMessage(hInstall, errorMessage);
		return ERROR_SUCCESS;
	}
	if (defsUpdated)
	{
		MsiLogMessage(hInstall, "I am using the latest shared defs");
	}
	else
	{
		MsiLogMessage(hInstall, "I should call UseNewestDefs(), not running lastest shared defs");
	}
    // remove the temp unziped virus def directory
    if ( (DeleteDir( hInstall, szDestPath, "*.*")) != 0 )
    {   
        vpsnprintf(errorMessage, sizeof (errorMessage), "Virus definitions temp directory removed %s",szDestPath);
        MsiLogMessage(hInstall, errorMessage);
    }
    else
    {
        vpsnprintf(errorMessage, sizeof (errorMessage), "Failed: Virus definitions temp directory removal %s",szDestPath);
        MsiLogMessage(hInstall, errorMessage);
    }
    // Register all application IDs with DefUtils ... ignore failures.
    RegisterDefAppIds (hInstall, errorMessage, sizeof (errorMessage));

    return ERROR_SUCCESS;
}

UINT __stdcall MsiInstallDefsRB( MSIHANDLE hInstall )
{
    return MsiUnInstallDefs(hInstall);
}

UINT __stdcall MsiUnInstallDefs( MSIHANDLE hInstall )
{
    bool            osVersionNT                         = false;
    char            stringBuffer[MAX_PATH]              = {0};
    DWORD           stringBufferLen                     = MAX_PATH;
    LPCSTR*         appIDs                              = NULL;
    DWORD           currAppID                           = 0;
    TCHAR           errorMessage[2*MAX_PATH+1]          = {0};

    // The custom action data will be the value of VersionNT
    if (ERROR_SUCCESS==MsiGetProperty(hInstall, "CustomActionData", stringBuffer, &stringBufferLen))
    {
        if( stringBuffer[0] )
            osVersionNT = false;
        else
            osVersionNT = true;
    }
    else
    {
        MsiLogMessage(hInstall, "MsiGetProperty Failed!");
        return ERROR_INSTALL_FAILURE;
    }

    if (osVersionNT)
    {
        stringBufferLen = g_ucNavAppIdNtArrayNum;
        appIDs = g_lpszNavAppIdNtArray;
    }
    else
    {
        stringBufferLen = g_ucNavAppId95ArrayNum;
        appIDs = g_lpszNavAppId95Array;
    }

    CDefUtils appIDregistrar;
    for (currAppID = 0; currAppID < stringBufferLen; currAppID++)
    {
        if (appIDregistrar.InitInstallApp(appIDs[currAppID]))
        {
            if (!appIDregistrar.StopUsingDefs())
            {
                vpsnprintf(errorMessage, sizeof (errorMessage), "WARNING:  error occured removing virus definitions application ID %s", appIDs[currAppID]);
                MsiLogMessage(hInstall, errorMessage);
            }
        }
        else
        {
            vpsnprintf(errorMessage, sizeof (errorMessage), "WARNING:  error occured initializing to remove virus definitions application ID %s", appIDs[currAppID]);
            MsiLogMessage(hInstall, errorMessage);
        }
    }

    return ERROR_SUCCESS;
}

UINT __stdcall MsiUnInstallDefsRB( MSIHANDLE hInstall )
{
    // Stream out the Virusdef bin hub to the temp directory
    MsiUnCabRuntimeDlls( hInstall );
    return MsiInstallDefs2( hInstall );
}

////////////////////////////////////////////////////////////////////////////////
//  MsiMigrateHold
//
//  The purpose of this function is to register a temporary NavAppID so that
//  existing defs will be preserved during an upgrade.
//
UINT __stdcall MsiMigrateHold( MSIHANDLE hInstall )
{
    CDefUtils duSAV;
    bool defsUpdated = false;

    MsiLogMessage( hInstall, "Migrating existing definitions." );

    // Register the temporary app ID
    if( duSAV.InitInstallApp( "MigrateTemp" ) )
    {
        if( !duSAV.UseNewestDefs( &defsUpdated ) )
            MsiLogMessage( hInstall, "Error holding previous virus definitions." );
    }
    else
        MsiLogMessage( hInstall, "Error registering virus definition application ID MigrateTemp" );

    return ERROR_SUCCESS;
}

UINT __stdcall MsiMigrateHoldRB( MSIHANDLE hInstall )
{
    return MsiMigrateRelease( hInstall );
}

UINT __stdcall MsiMigrateRelease( MSIHANDLE hInstall )
{
    CDefUtils duSAV;

    MsiLogMessage( hInstall, "Releasing migrated definitions." );

    // Unregister the temporary app ID
    if( duSAV.InitInstallApp( "MigrateTemp" ) )
    {
        if( !duSAV.StopUsingDefs() )
            MsiLogMessage( hInstall, "WARNING: Could not remove virus definitions application ID MigrateTemp" );
    }
    else
        MsiLogMessage( hInstall, "WARNING: Could not initialize to remove virus definitions application ID MigrateTemp" );

    return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: Findit()
//
// Description: This function is supposed to be used to hunt (findfile)for 
// a given file pattern in a given folder.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 06/16/2004 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
BOOL Findit (MSIHANDLE hInstall, LPCTSTR szFile, TCHAR* szPath, size_t nNumPathBytes)
{
    TCHAR                       errorMessage[MAX_PATH+1]= {0};  
    WIN32_FIND_DATA             FindFileData;
    HANDLE                      hFind;

    vpsnprintf(errorMessage, sizeof (errorMessage), "Target file is %s", szFile);
    MsiLogMessage(hInstall, errorMessage);
    hFind = FindFirstFile(szFile, &FindFileData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        vpsnprintf(errorMessage, sizeof (errorMessage), "Invalid File Handle. Get Last Error reports %d", GetLastError ());
        MsiLogMessage(hInstall, errorMessage);
        return false;
    }
    else 
    {
        vpsnprintf(errorMessage, sizeof (errorMessage), "The first file found is %s", FindFileData.cFileName);
        MsiLogMessage(hInstall, errorMessage);
        FindClose(hFind);
        vpstrncpy (szPath, FindFileData.cFileName, nNumPathBytes);
    }
    return true;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: UnPakZip()
//
// Description: This mess of humanity code was partially lifted from the  
// Greenleaf lib files. As per the title it UnZip's a target file into a 
// destination directory.
//
// Return Values: 0 if successful.
//                Error value returned by GetLastError() otherwise.
//
//////////////////////////////////////////////////////////////////////////
// 06/16/2004 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
BOOL UnPakZip(MSIHANDLE hInstall, LPCTSTR szFile, LPCTSTR szDestDirectory )
{
    TCHAR           fileName[MAX_PATH+1]     = {0};
    TCHAR           errorMessage[MAX_PATH+1] = {0};

    ALPkArchive archive( szFile );
    ALEntryList list( 0, PkDecompressTools() );
    vpsnprintf(errorMessage, sizeof (errorMessage), "UnPakZip: szFile is %s.", szFile);
    MsiLogMessage(hInstall, errorMessage);
    archive.ReadDirectory( list );
    ALEntry *entry = list.GetFirstEntry();
    while ( entry ) {
        ALName name = entry->mpStorageObject->mName;
        vpstrncpy(fileName, name.GetSafeName(), sizeof (fileName));
        vpsnprintf(errorMessage, sizeof (errorMessage), "UnPakZip: fileName is %s.", fileName);
        MsiLogMessage(hInstall, errorMessage);
        name = ALName( szDestDirectory ) + (const char *) name;
        entry->mpStorageObject->mName = name;
        entry = entry->GetNextEntry();
    }
    return archive.Extract( list );
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MSIDeleteVDEFHUB( MSIHANDLE )
//
// Replaced - Function: MSIDeleteMSFT_RT_71_DLL( MSIHANDLE )
//
// Description: This function loads the defhub.zip names from
// the resource file, creates target paths and then deletes them. 
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 08/04/2004 - DKOWALYSHYN  
// 09/01/2005 - DKOWALYSHYN
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MSIDeleteVDEFHUB( MSIHANDLE hInstall )
{
    CString         szVDEFHUB_NAME;
    CString         strZIP;
    CString         strDLL;
    TCHAR           szTemp[MAX_PATH]                = {0};
    TCHAR           szTempFolder[MAX_PATH]          = {0};
    TCHAR           szVDEFHUB_ZIP_TARGET[MAX_PATH]  = {0};
    TCHAR           szZIP[MAX_PATH]                 = {0};
    TCHAR           szDLL[MAX_PATH]                 = {0};
    DWORD           dwLen                           = 0;

    dwLen = sizeof( szTempFolder );
    if ( 0 < GetTempPath ((sizeof(szTempFolder)), szTempFolder))
    {
        vpsnprintf( szTemp, sizeof (szTemp), "MSIInstallMSFT_RT_71_DLL - szTempFolder %s ", szTempFolder);
        MsiLogMessage( hInstall, szTemp );
		if ( strZIP.LoadString ( IDS_ZIP ) )
		{   
			strDLL.LoadString ( IDS_DLL );
			// Create target path
			vpstrncpy ( szVDEFHUB_ZIP_TARGET, szTempFolder, sizeof (szVDEFHUB_ZIP_TARGET));
			AddSlash  ( szVDEFHUB_ZIP_TARGET,               sizeof (szVDEFHUB_ZIP_TARGET));
			szVDEFHUB_NAME.LoadString( IDS_BIN_HUB );
			vpstrnappend( szVDEFHUB_ZIP_TARGET, szVDEFHUB_NAME, sizeof (szVDEFHUB_ZIP_TARGET));
			szVDEFHUB_NAME.LoadString( IDS_ZIP );
			vpstrnappend( szVDEFHUB_ZIP_TARGET, szVDEFHUB_NAME, sizeof (szVDEFHUB_ZIP_TARGET));

			// delete the VDEFBIN.ZIP file
			if ( DeleteFile( szVDEFHUB_ZIP_TARGET ) )
			{
				vpsnprintf( szTemp, sizeof (szTemp), "szVDEFHUB_ZIP_TARGET - delete from %s success", szVDEFHUB_ZIP_TARGET );
				MsiLogMessage( hInstall, szTemp );
			}
			else
			{
				vpsnprintf( szTemp, sizeof (szTemp), "szVDEFHUB_ZIP_TARGET - delete from %s failure", szVDEFHUB_ZIP_TARGET );
				MsiLogMessage( hInstall, szTemp );
			}
		}
    }
    else
    {
        MsiLogMessage( hInstall, "MSIInstallMSFT_RT_71_DLL get TempFolder failed." );
    }
    return( ERROR_SUCCESS );
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MsiCopyVirusDefsAdmin()
//
// Description: This function copies a Virusdef.zip file (if found) from
// the SOURCEDIR to the TARGETDIR only in the creation of an 
// Administrative install situation
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 10/11/2004 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiCopyVirusDefsAdmin( MSIHANDLE hInstall )
{
    TCHAR           szMIDTARGETDIR[MAX_PATH]            = {0};
    TCHAR           szMIDSOURCEDIR[MAX_PATH]            = {0};
    TCHAR           szTARGETDIR[MAX_PATH]               = {0};
    TCHAR           szSOURCEDIR[MAX_PATH]               = {0};
    TCHAR           strCADTemp[MAX_PATH]                = {0};
    TCHAR           *pdeststr                           = NULL;
    TCHAR           errorMessage[2*MAX_PATH+1]          = {0};
    DWORD           returnVal                           = ERROR_SUCCESS;
    DWORD           strCADTempLen                       = sizeof(strCADTemp);
    INT             commalocation                       = 0;

    returnVal = MsiGetProperty(hInstall, "CustomActionData", strCADTemp, &strCADTempLen);
    if (returnVal != ERROR_SUCCESS)
    {
        vpsnprintf(errorMessage, sizeof (errorMessage), "Error %d retrieving CustomActionData property from MSI.", returnVal);
        MsiLogMessage(hInstall, errorMessage);
        return ERROR_INSTALL_FAILURE;
    }
    pdeststr = strstr( strCADTemp, "," );               // location the comma delimiter
    commalocation = pdeststr - strCADTemp ;             // calc the comma location in the string
    vpstrncpy(szMIDSOURCEDIR, strCADTemp, commalocation+ 1 );
    vpstrncpy(szMIDTARGETDIR, strCADTemp + commalocation + 1, sizeof (szMIDTARGETDIR));

    for ( int nCount = 0; nCount < NUM_ADMIN_INSTALL_MANUAL_COPY_FILES; nCount++ )
    {
        vpstrncpy(szSOURCEDIR, szMIDSOURCEDIR, sizeof (szSOURCEDIR)); // Set the midsource strings
        vpstrncpy(szTARGETDIR, szMIDTARGETDIR, sizeof (szTARGETDIR));

        vpstrnappend(szSOURCEDIR, pszAdminSrcFiles[nCount], sizeof (szSOURCEDIR)); // Append filenames
        vpstrnappend(szTARGETDIR, pszAdminSrcFiles[nCount], sizeof (szTARGETDIR));

        vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szSOURCEDIR %s", szSOURCEDIR);
        MsiLogMessage(hInstall, errorMessage);

        vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : szTARGETDIR szFile %s", szTARGETDIR);
        MsiLogMessage(hInstall, errorMessage);

        if ( CopyFile( szSOURCEDIR, szTARGETDIR, FALSE ) )
        {
            vpsnprintf( errorMessage, sizeof (errorMessage), "CopyFile - copied from %s to %s success", szSOURCEDIR, szTARGETDIR);
            MsiLogMessage( hInstall, errorMessage );
        }
        else
        {
            vpsnprintf( errorMessage, sizeof (errorMessage), "CopyFile - copied from %s to %s failure - Get Last Error reports %d", szSOURCEDIR, szTARGETDIR,  GetLastError ());
            MsiLogMessage( hInstall, errorMessage );
        }
    }

    return ERROR_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////
//
// Function: MsiCopyVirusDefsUNC()
//
// Description: This function copies a Virusdef.zip file (if found) from
// the SOURCEDIR to the TEMPDIR in immedate mode when installed via a UNC  
// path situation because in deferred mode you won't have net access.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 11/08/2004 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiCopyVirusDefsUNC( MSIHANDLE hInstall )
{
    TCHAR           szTARGETDIR[MAX_PATH]               = {0};
    TCHAR           szSOURCEDIR[MAX_PATH]               = {0};
    TCHAR           sztmpTARGETDIR[MAX_PATH]            = {0};
    TCHAR           sztmpSOURCEDIR[MAX_PATH]            = {0};
    TCHAR           strCADTemp[MAX_PATH]                = {0};
    TCHAR           *pdeststr                           = NULL;
    TCHAR           errorMessage[2*MAX_PATH+1]          = {0};
    DWORD           returnVal                           = ERROR_SUCCESS;
    DWORD           szSOURCEDIRLen                      = sizeof(szSOURCEDIR);
	DWORD			dwTARGETDIRLen						= sizeof(szTARGETDIR);
    INT             commalocation                       = 0;

	// get the PROPERTY_VIRUSDEFFOLDER from the msi engine
	if( ERROR_SUCCESS == MsiGetProperty( hInstall, _T(PROPERTY_VIRUSDEFFOLDER), szTARGETDIR, &dwTARGETDIRLen ))
    {
		vpsnprintf(errorMessage, sizeof(errorMessage), "MsiCopyVirusDefsUNC - %s := %s", PROPERTY_VIRUSDEFFOLDER, szTARGETDIR);
		MsiLogMessage(hInstall, errorMessage);

        if( ERROR_SUCCESS == MsiGetProperty( hInstall, _T("SourceDir"), szSOURCEDIR, &szSOURCEDIRLen ))
        {
			for ( int nFileCount=0; nFileCount < NUM_VIRUSDEF_FILES; nFileCount++)
			{
				vpstrncpy( sztmpSOURCEDIR, szSOURCEDIR, sizeof(sztmpSOURCEDIR));
				vpstrncpy( sztmpTARGETDIR, szTARGETDIR, sizeof(sztmpTARGETDIR));

				vpstrnappend(sztmpSOURCEDIR,pszVirusDefFiles[nFileCount], sizeof (szSOURCEDIR)); // Append Virus Def name
				vpstrnappend(sztmpTARGETDIR,pszVirusDefFiles[nFileCount], sizeof (szTARGETDIR)); // Append Virus Def name

				vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : sztmpSOURCEDIR %s.", sztmpSOURCEDIR);
				MsiLogMessage(hInstall, errorMessage);

				vpsnprintf(errorMessage, sizeof (errorMessage), "TempFolder : sztmpTARGETDIR %s.", sztmpTARGETDIR);
				MsiLogMessage(hInstall, errorMessage);

				if ( CopyFile( sztmpSOURCEDIR, sztmpTARGETDIR, FALSE ) )
				{
					vpsnprintf( errorMessage, sizeof (errorMessage), "CopyFile - copied from %s to %s success", sztmpSOURCEDIR, sztmpTARGETDIR);
					MsiLogMessage( hInstall, errorMessage );
				}
				else
				{
					vpsnprintf( errorMessage, sizeof (errorMessage), "CopyFile - copied from %s to %s failure", sztmpSOURCEDIR, sztmpTARGETDIR);
					MsiLogMessage( hInstall, errorMessage );
				}
			}
        }
        else
            MsiLogMessage(hInstall,"MsiCopyVirusDefsUNC: MsiGetProperty SourceDir failed");
    }
    else
        MsiLogMessage(hInstall,"MsiCopyVirusDefsUNC: GetTempPath failed");

    return ERROR_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////
//
// Function: MsiDeleteRanVirDefsFolder()
//
// Description: This function deletes the random virus def folder
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 11/29/2005 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiDeleteRanVirDefsFolder( MSIHANDLE hInstall )
{
    TCHAR           szTARGETDIR[MAX_PATH]               = {0};
    TCHAR           errorMessage[2*MAX_PATH+1]          = {0};
 	DWORD			dwTARGETDIRLen						= sizeof(szTARGETDIR);
	DWORD           dwRetVal							= ERROR_SUCCESS;

	dwRetVal = MsiGetProperty(hInstall, "CustomActionData", szTARGETDIR, &dwTARGETDIRLen);
    if (dwRetVal != ERROR_SUCCESS)
    {
        vpsnprintf(errorMessage, sizeof (errorMessage), "MsiDeleteRanVirDefsFolder - Error %d retrieving CustomActionData property from MSI.", dwRetVal);
		MsiLogMessage(hInstall, errorMessage);
		return ERROR_SUCCESS;
	}
	vpsnprintf(errorMessage, sizeof(errorMessage), "MsiDeleteRanVirDefsFolder - %s.", szTARGETDIR );
	MsiLogMessage(hInstall, errorMessage);
	// Ensure that we get a path from the MSI engine, otherwise skip the delete.
	// Security risk as a null passed in will delete the whole harddrive.
	if ( 9 < _tcslen( szTARGETDIR ))
	{
		vpsnprintf(errorMessage, sizeof(errorMessage), "MsiDeleteRanVirDefsFolder - szTARGETDIR path =%s.", szTARGETDIR );
		MsiLogMessage(hInstall, errorMessage);
		// remove the random temp virus def binhub
		if ( (DeleteDir( hInstall, szTARGETDIR, "*.*")) != 0 )
		{
			vpsnprintf(errorMessage, sizeof (errorMessage), "MsiDeleteRanVirDefsFolder - DeleteFile succeded - szTARGETDIR %s", szTARGETDIR);
			MsiLogMessage(hInstall, errorMessage);
		}
		else
		{
			vpsnprintf(errorMessage, sizeof (errorMessage), "MsiDeleteRanVirDefsFolder - DeleteFile failed - szTARGETDIR %s", szTARGETDIR);
			MsiLogMessage(hInstall, errorMessage);
		}
	}
	else
	{
		vpsnprintf(errorMessage, sizeof(errorMessage), "MsiDeleteRanVirDefsFolder - szTARGETDIR path is too small =%s.", szTARGETDIR );
		MsiLogMessage(hInstall, errorMessage);
	}

	return ERROR_SUCCESS;
}
//////////////////////////////////////////////////////////////////////////
//
// Function: MsiSetRanVirDefsPath()
//
// Description: This function creates a Pseudo random file path for use
//	when unpacking virus defs. We have a problem were we can get an old
//	vdefhub stuck in the temp folder which will interfere during migration.
//	This function should guarantee the desired def package is always used.
//	This function then sets the msi property PROPERTY_VIRUSDEFFOLDER with
//	the folder path so it can be used elsewhere by the installer.
//
// Return Values: 0 if successful.
//
//////////////////////////////////////////////////////////////////////////
// 11/28/2005 - DKOWALYSHYN  
//////////////////////////////////////////////////////////////////////////
UINT __stdcall MsiSetRanVirDefsPath( MSIHANDLE hInstall )
{
	CString			strRndPath;
	TCHAR			strRndTempPath[MAX_PATH]			= {0};
	TCHAR			szTempFolder[MAX_PATH]				= {0};
    TCHAR           errorMessage[2*MAX_PATH+1]          = {0};
	DWORD			dwLen	= sizeof( szTempFolder );
	int				num		= 0;
	// Seed random table with time
	srand( (unsigned)time( NULL ) );
	MsiLogMessage( hInstall, "MsiSetRanVirDefsPath - seeding random table." );
	if ( 0 < GetTempPath ((sizeof(szTempFolder)), szTempFolder))
	{
		vpsnprintf( errorMessage, sizeof (errorMessage), "MsiSetRanVirDefsPath - GetTempPath %s", szTempFolder );
		MsiLogMessage( hInstall, errorMessage );

		for ( int count = 0; count < MAX_RND_PATH_CHRS; count ++)
		{
			// create random substring ( 65 char is 'A' rand %26 means 'A' to 'Z' possible)
			strRndTempPath[count] = (rand() % 26) + 65;
		}
		vpsnprintf( errorMessage, sizeof (errorMessage), "MsiSetRanVirDefsPath - strRndTempPath %s", strRndTempPath );
		MsiLogMessage( hInstall, errorMessage );
	}
	// append a random section on to the temp string
	strRndPath.Format("%s%s\\", szTempFolder, strRndTempPath);
	vpsnprintf( errorMessage, sizeof (errorMessage), "MsiSetRanVirDefsPath - strRndPath %s", strRndPath );
	MsiLogMessage( hInstall, errorMessage );

	MsiSetProperty(hInstall, _T(PROPERTY_VIRUSDEFFOLDER), strRndPath);
	vpsnprintf(errorMessage, sizeof(errorMessage), "MsiSetRanVirDefsPath - %s := %s", PROPERTY_VIRUSDEFFOLDER, strRndPath);
	MsiLogMessage(hInstall, errorMessage);

	// This code was moved from MsiCopyVirusDefsUNC and it's used to find the source directory for the LUSetup code
	MsiSetProperty(hInstall, _T(PROPERTY_EXTRACTTARGETDIR), strRndPath /*szTempFolder*/);
	vpsnprintf(errorMessage, sizeof(errorMessage), "%s := %s", PROPERTY_EXTRACTTARGETDIR, strRndPath);
	MsiLogMessage(hInstall, errorMessage);

	if ( CreateDirectory( strRndPath, NULL ) )
	{
		MsiLogMessage(hInstall, "MsiSetRanVirDefsPath - CreateDirectory success.");
	}
	else
	{
		MsiLogMessage(hInstall, "MsiSetRanVirDefsPath - CreateDirectory failed.");
	}
    return ERROR_SUCCESS;
}