// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003-2004, 2005 Symantec Corporation. All rights reserved.

// Configure for Win2000 and + symbols
#define _WIN32_WINNT        0x0500
#define WINVER              0x0500
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string.h>
#include <windows.h>
#include <msi.h>
#include <msiquery.h>
#include <list>
#include <string>
#include "SymSaferStrings.h"
#include "SymSaferRegistry.h"
#include "VPExceptionHandling.h"
#include "ClientReg.h"
#include "vpcommon.h"
#include "resource.h"
#include "Sav9UninstallFix.h"

int FixSav10ByRemoveCA( void )
{
    StringList      productCodes;
    TCHAR           databaseFilename[2*MAX_PATH]    = {""};
    DWORD           stringSize                      = 0;
    TCHAR           installDirectory[MAX_PATH]      = {""};
    bool            savDetected                     = false;
    HKEY            keyHandle                       = NULL;
    DWORD           dataType                        = REG_SZ;
    DWORD           valueDataSize                   = 0;
    DWORD           clientType                      = CLIENT_TYPE_STANDALONE;
    MSIHANDLE       databaseHandle                  = NULL;
    DWORD           exitCode                        = 1;
    DWORD           returnValDW                     = ERROR_OUT_OF_PAPER;
    HRESULT         returnValHR                     = E_FAIL;
    HRESULT         returnValHRb                    = E_FAIL;
    TCHAR           productVersion[MAX_PATH]        = {0};                  

    // Locate SAV 10
    returnValHR = GetProductsWithUpgradeCode(SAV_10_UPGRADECODE, 10,0, 10,0, &productCodes);
    if (FAILED(returnValHR))
        printf(IDS_ERROR_DETECTINGSAV10, returnValHR);

    returnValHR = GetProductsWithUpgradeCode(SAV_10_x64_UPGRADECODE, 10,0, 10,0, &productCodes);
    if (FAILED(returnValHR))
        printf(IDS_ERROR_DETECTINGSAV10, returnValHR);

    returnValHR = GetProductsWithUpgradeCode(SCS_3_UPGRADECODE, 10,0, 10,0, &productCodes);
    if (FAILED(returnValHR))
        printf(IDS_ERROR_DETECTINGSCS3, returnValHR);
    if (productCodes.size() > 0)
    {
        stringSize = sizeof(productVersion)/sizeof(productVersion[0]);
        returnValDW = MsiGetProductInfo(productCodes.front().c_str(), INSTALLPROPERTY_VERSIONSTRING, productVersion, &stringSize);
        if (returnValDW == ERROR_SUCCESS)
            printf(IDS_PRODUCT_VERSION_FOUND, productCodes.front().c_str(), productVersion );
        
        // There should be only one product
        stringSize = sizeof(databaseFilename)/sizeof(databaseFilename[0]);
        returnValDW = MsiGetProductInfo(productCodes.front().c_str(), INSTALLPROPERTY_LOCALPACKAGE, databaseFilename, &stringSize);
        if (returnValDW == ERROR_SUCCESS)
        {
            savDetected = true;
            if (productCodes.size() > 1)
                printf(IDS_WARNING_TOOMANYPRODUCTCODES, productCodes.size(), productCodes.front().c_str());
        }
        else
        {
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
            printf(IDS_ERROR_LOCATINGPACKAGE, returnValHR, productCodes.front().c_str());
        }
    }

    // Fix SAV 10
    if (savDetected)
    {
        // Is this server?
        returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szReg_Key_Main, NULL, KEY_READ, &keyHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            valueDataSize = sizeof(clientType);
            SymSaferRegQueryValueExA(keyHandle, szReg_Val_Client_Type, NULL, &dataType, (BYTE*) &clientType, &valueDataSize);
            RegCloseKey(keyHandle);
            keyHandle = NULL;
        }

        // Modify the msi file
        returnValDW = MsiOpenDatabase(databaseFilename, MSIDBOPEN_DIRECT, &databaseHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            returnValHR = FixSav10ByRemoveCA(databaseHandle, databaseFilename);
            returnValHRb = FixSav10ServerUninstall(databaseHandle, databaseFilename);
            if (SUCCEEDED(returnValHR))
                returnValHR = returnValHRb;
            MsiCloseHandle(databaseHandle);
            databaseHandle = NULL;

            if (SUCCEEDED(returnValHR))
            {
                exitCode = EXIT_CODE_SUCCESS;
                printf(IDS_INFO_APPLYSUCCESS);
            }
            else
            {
                exitCode = EXIT_CODE_FIXAPPLYERROR;
            }
        }
        else
        {
            exitCode = EXIT_CODE_MSIOPENERROR;
            printf(IDS_ERROR_OPENINGFILE, returnValDW, databaseFilename);
        }
    }
    else
    {
        exitCode = EXIT_CODE_SAVSCSNOTDETECTED;
    }

    return exitCode;
}

HRESULT FixSav10ByRemoveCA( MSIHANDLE databaseHandle, LPCTSTR databaseFilename )
// Fixes the specified SAV10 cached MSI by deleting the offending custom actions
// Logs all errors
// Returns S_OK on success, else the error code of the failure
{
    HRESULT         returnValHR                     = S_OK;
    HRESULT         returnValHRb                    = E_FAIL;

    // Validate parameters
    if (databaseHandle == NULL)
        return E_INVALIDARG;
    if (databaseFilename == NULL)
        return E_POINTER;

    // Delete the records sequencing in the offending custom actions
    returnValHRb = DeleteRecordByKey(databaseHandle, TABLE_NAME_INSTALLEXECUTE, FIELDNAME_INSTALLEXECUTE_NAME, CUSTOM_ACTION_NAME_10_A);
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x deleting custom action %s from %s table of %s.\n", returnValHR, CUSTOM_ACTION_NAME_10_A, FIELDNAME_INSTALLEXECUTE_NAME, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }
    returnValHRb = DeleteRecordByKey(databaseHandle, TABLE_NAME_INSTALLEXECUTE, FIELDNAME_INSTALLEXECUTE_NAME, CUSTOM_ACTION_NAME_10_ROLLBACK_A);
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x deleting custom action %s from %s table of %s.\n", returnValHR, CUSTOM_ACTION_NAME_10_ROLLBACK_A, FIELDNAME_INSTALLEXECUTE_NAME, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }
    returnValHRb = DeleteRecordByKey(databaseHandle, TABLE_NAME_INSTALLEXECUTE, FIELDNAME_INSTALLEXECUTE_NAME, CUSTOM_ACTION_NAME_10_B);
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x deleting custom action %s from %s table of %s.\n", returnValHR, CUSTOM_ACTION_NAME_10_B, FIELDNAME_INSTALLEXECUTE_NAME, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }
    returnValHRb = HRESULT_FROM_WIN32(MsiDatabaseCommit(databaseHandle));
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x writing remove CA changes to disk for file %s.\n", returnValHR, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }

    return returnValHR;
}

HRESULT FixSav10ServerUninstall( MSIHANDLE databaseHandle, LPCTSTR databaseFilename )
// Part of fix to 1-5DXJA1:  When upgrading a 10.0 server, we should not require a reboot.
// The 10.0 uninstall has unnecessary reboot and reboot prompting.  Fix the conditions on the
// these custom actions.
// Returns S_OK on success, S_FALSE if change already made, else the error code of the failure
{
    HRESULT         returnValHR                     = S_OK;
    HRESULT         returnValHRb                    = E_FAIL;

    // Validate parameters
    if (databaseHandle == NULL)
        return E_INVALIDARG;
    if (databaseFilename == NULL)
        return E_POINTER;
   
    // Modify the ScheduleReboot action's condition
    returnValHRb = ModifyRecordByKey(databaseHandle, TABLE_NAME_INSTALLEXECUTE, FIELDNAME_INSTALLEXECUTE_NAME, ACTION_NAME_SCHEDULEREBOOT, 2, CONDITION_10_E);
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x modifying condition for custom action %s from %s table of %s.\n", returnValHR, ACTION_NAME_SCHEDULEREBOOT, TABLENAME_INSTALLEXECUTE, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }
    // Modify the SetRebootAtEnd action's condition
    returnValHRb = ModifyRecordByKey(databaseHandle, TABLE_NAME_INSTALLEXECUTE, FIELDNAME_INSTALLEXECUTE_NAME, CUSTOM_ACTION_NAME_10_F, 2, CONDITON_10_F);
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x modifying condition for custom action %s from %s table of %s.\n", returnValHR, CUSTOM_ACTION_NAME_10_F, TABLENAME_INSTALLEXECUTE, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }

    // Write changes to disk
    returnValHRb = HRESULT_FROM_WIN32(MsiDatabaseCommit(databaseHandle));
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x writing server CA changes to disk for file %s.\n", returnValHR, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }

    return returnValHR;
}

int FixSav9ByRemoveCA( void )
{
    StringList      productCodes;
    TCHAR           databaseFilename[2*MAX_PATH]    = {""};
    DWORD           stringSize                      = 0;
    HKEY            keyHandle                       = NULL;
    DWORD           dataType                        = REG_DWORD;
    TCHAR           installDirectory[MAX_PATH]      = {""};
    bool            sav9Detected                    = false;
    MSIHANDLE       databaseHandle                  = NULL;
    DWORD           exitCode                        = 1;
    DWORD           returnValDW                     = ERROR_OUT_OF_PAPER;
    HRESULT         returnValHR                     = E_FAIL;

    // Locate SAV9
    returnValHR = GetProductsWithUpgradeCode(SAV_9_UPGRADECODE, 9,0, 9,255, &productCodes);
    if (FAILED(returnValHR))
        printf(IDS_ERROR_DETECTINGSAV9, returnValHR);
    returnValHR = GetProductsWithUpgradeCode(SCS_2_UPGRADECODE, 9,0, 9,255, &productCodes);
    if (FAILED(returnValHR))
        printf(IDS_ERROR_DETECTINGSCS2, returnValHR);
    if (productCodes.size() > 0)
    {
        // There should be only one product
        stringSize = sizeof(databaseFilename)/sizeof(databaseFilename[0]);
        returnValDW = MsiGetProductInfo(productCodes.front().c_str(), INSTALLPROPERTY_LOCALPACKAGE, databaseFilename, &stringSize);
        if (returnValDW == ERROR_SUCCESS)
        {
            sav9Detected = true;
            if (productCodes.size() > 1)
                printf(IDS_WARNING_TOOMANYPRODUCTCODES, productCodes.size(), productCodes.front().c_str());
        }
        else
        {
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
            printf(IDS_ERROR_LOCATINGPACKAGE, returnValHR, productCodes.front().c_str());
        }
    }

    // Fix SAV9
    if (sav9Detected)
    {
        returnValDW = MsiOpenDatabase(databaseFilename, MSIDBOPEN_DIRECT, &databaseHandle);
        if (returnValDW == ERROR_SUCCESS)
        {
            returnValHR = FixSav9ByRemoveCA(databaseHandle, databaseFilename);
            MsiCloseHandle(databaseHandle);
            databaseHandle = NULL;

            if (SUCCEEDED(returnValHR))
            {
                exitCode = EXIT_CODE_SUCCESS;
                printf(IDS_INFO_APPLYSUCCESS);
            }
            else
            {
                exitCode = EXIT_CODE_FIXAPPLYERROR;
            }
        }
        else
        {
            exitCode = EXIT_CODE_MSIOPENERROR;
            printf(IDS_ERROR_OPENINGFILE, returnValDW, databaseFilename);
        }
    }
    else
    {
        exitCode = EXIT_CODE_SAVSCSNOTDETECTED;
    }

    return exitCode;
}

HRESULT FixSav9ByForceUnload( MSIHANDLE databaseHandle, LPCTSTR databaseFilename )
// Fixes the specified SAV9 cached MSI by sequencing in 2 custom actions to force unload LuComServerPS.dll
// Logs all errors
// Returns S_OK on success, else the error code of the failure
{
    DWORD           stringSize                      = 0;
    HKEY            keyHandle                       = NULL;
    DWORD           dataType                        = REG_DWORD;
    TCHAR           installDirectory[MAX_PATH]      = {""};
    TCHAR           targetFilename[2*MAX_PATH]      = {""};
    DWORD           fixupAsequenceNo                = 2500;
    DWORD           fixupBsequenceNo                = 2100;
    DWORD           returnValDW                     = ERROR_OUT_OF_PAPER;
    HRESULT         returnValHR                     = E_FAIL;

    // Validate parameters
    if (databaseHandle == NULL)
        return E_INVALIDARG;
    if (databaseFilename == NULL)
        return E_POINTER;

    // Determine program directory
    returnValDW = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Symantec\\InstalledApps", NULL, KEY_READ, &keyHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        stringSize = sizeof(installDirectory)/sizeof(installDirectory[0]);
        SymSaferRegQueryValueExA(keyHandle, "SAVCE", NULL, &dataType, (BYTE*) installDirectory, &stringSize);
        RegCloseKey(keyHandle);
        keyHandle = NULL;
    }
    else
    {
        printf(IDS_ERROR_LOCATINGPRODUCT, returnValHR);
    }
    sssnprintf(targetFilename, sizeof(targetFilename), "%s%s", installDirectory, BINARY_FILENAME);

    // Apply the fix - add 2 CA's sequenced after certain other CAs
    if (returnValDW == ERROR_SUCCESS)
    {
        // Determine sequence numbers to use.  Dupe are OK in the sequence table, just no ordering guarantee
        returnValHR = GetActionSequenceNumber(databaseHandle, TABLE_NAME_INSTALLEXECUTE, CUSTOM_ACTION_NAME_BASE_FOR_A, &fixupAsequenceNo);
        if (SUCCEEDED(returnValHR))
            fixupAsequenceNo += 1;
        else
            printf(IDS_WARNING_LOCATIONGBASEACTION, returnValHR, CUSTOM_ACTION_NAME_BASE_FOR_A, databaseFilename);
        returnValHR = GetActionSequenceNumber(databaseHandle, TABLE_NAME_INSTALLEXECUTE, CUSTOM_ACTION_NAME_BASE_FOR_B, &fixupBsequenceNo);
        if (SUCCEEDED(returnValHR))
            fixupBsequenceNo += 1;
        else
            printf(IDS_WARNING_LOCATIONGBASEACTION, returnValHR, CUSTOM_ACTION_NAME_BASE_FOR_B, databaseFilename);

        // Extract embedded DLL to a file in the SAV9 directory
        returnValHR = ExtractResourceToFile(RESOURCE_NAME, targetFilename);
        if (SUCCEEDED(returnValHR))
        {
            // Modify the cached MSI
            // Add a BinaryTable record with the file extracted above
            returnValHR = AddBinaryTableRecord(databaseHandle, BINARY_RECORD_NAME, targetFilename);
            if (returnValHR == S_FALSE)
                printf(IDS_INFO_ALREADYAPPLIED, databaseFilename);
            else if (FAILED(returnValHR))
                printf(IDS_ERROR_ADDINGBINARYTABLERECORD, returnValHR, databaseFilename);

            // Add custom action record - 1 for type 3122, 1 for type 1025, since there are 2 separate CA server processes
            if (SUCCEEDED(returnValHR))
            {
                returnValHR = AddCustomAction(databaseHandle, CUSTOM_ACTION_NAME_A, 3073, BINARY_RECORD_NAME, FIXUP_FUNCTION_NAME);
                if (FAILED(returnValHR))
                    printf(IDS_ERROR_ADDINGCUSTOMACTION, returnValHR, databaseFilename);
            }
            if (SUCCEEDED(returnValHR))
            {
                returnValHR = AddCustomAction(databaseHandle, CUSTOM_ACTION_NAME_B, 1025, BINARY_RECORD_NAME, FIXUP_FUNCTION_NAME);
                if (FAILED(returnValHR))
                    printf("Error 0x%08x adding custom action to %s.\n", returnValHR, databaseFilename);
            }

            // Sequence the CAs into the InstallExecuteSequence
            if (SUCCEEDED(returnValHR))
            {
                returnValHR = AddActionToSequenceTable(databaseHandle, TABLE_NAME_INSTALLEXECUTE, CUSTOM_ACTION_NAME_A, "", fixupAsequenceNo);
                if (FAILED(returnValHR))
                    printf(IDS_ERROR_ADDINGSEQUENCERECORD, returnValHR, TABLE_NAME_INSTALLEXECUTE, databaseFilename);
            }
            if (SUCCEEDED(returnValHR))
            {
                returnValHR = AddActionToSequenceTable(databaseHandle, TABLE_NAME_INSTALLEXECUTE, CUSTOM_ACTION_NAME_B, "", fixupBsequenceNo);
                if (FAILED(returnValHR))
                    printf(IDS_ERROR_ADDINGSEQUENCERECORD, returnValHR, databaseFilename);
            }

            // Cleanup and write out changes.
            if (SUCCEEDED(returnValHR))
                returnValHR = HRESULT_FROM_WIN32(MsiDatabaseCommit(databaseHandle));
            else
                DeleteFile(targetFilename);
        }
        else
        {
            printf(IDS_ERROR_EXTRACTINGFILE, returnValHR);
        }
    }

    return returnValHR;
}

HRESULT FixSav9ByRemoveCA( MSIHANDLE databaseHandle, LPCTSTR databaseFilename )
// Fixes the specified SAV9 cached MSI by deleting the offending custom actions
// Logs all errors
// Returns S_OK on success, else the error code of the failure
{
    HRESULT         returnValHR                     = S_OK;
    HRESULT         returnValHRb                    = E_FAIL;

    // Validate parameters
    if (databaseHandle == NULL)
        return E_INVALIDARG;
    if (databaseFilename == NULL)
        return E_POINTER;

    // Delete the records sequencing in the offending custom actions
    returnValHRb = DeleteRecordByKey(databaseHandle, TABLE_NAME_INSTALLEXECUTE, FIELDNAME_INSTALLEXECUTE_NAME, CUSTOM_ACTION_NAME_BASE_FOR_A);
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x deleting custom action %s from %s table of %s.\n", returnValHR, CUSTOM_ACTION_NAME_BASE_FOR_A, FIELDNAME_INSTALLEXECUTE_NAME, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }
    returnValHRb = DeleteRecordByKey(databaseHandle, TABLE_NAME_INSTALLEXECUTE, FIELDNAME_INSTALLEXECUTE_NAME, CUSTOM_ACTION_NAME_ROLLBACK_A);
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x deleting custom action %s from %s table of %s.\n", returnValHR, CUSTOM_ACTION_NAME_ROLLBACK_A, FIELDNAME_INSTALLEXECUTE_NAME, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }
    returnValHRb = DeleteRecordByKey(databaseHandle, TABLE_NAME_INSTALLEXECUTE, FIELDNAME_INSTALLEXECUTE_NAME, CUSTOM_ACTION_NAME_BASE_FOR_B);
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x deleting custom action %s from %s table of %s.\n", returnValHR, CUSTOM_ACTION_NAME_BASE_FOR_B, FIELDNAME_INSTALLEXECUTE_NAME, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }
    returnValHRb = DeleteRecordByKey(databaseHandle, TABLE_NAME_INSTALLEXECUTE, FIELDNAME_INSTALLEXECUTE_NAME, CUSTOM_ACTION_NAME_ROLLBACK_B);
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x deleting custom action %s from %s table of %s.\n", returnValHR, CUSTOM_ACTION_NAME_ROLLBACK_B, FIELDNAME_INSTALLEXECUTE_NAME, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }
    // Following are SCS-specific
    returnValHRb = DeleteRecordByKey(databaseHandle, TABLE_NAME_INSTALLEXECUTE, FIELDNAME_INSTALLEXECUTE_NAME, CUSTOM_ACTION_NAME_C);
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x deleting custom action %s from %s table of %s.\n", returnValHR, CUSTOM_ACTION_NAME_C, FIELDNAME_INSTALLEXECUTE_NAME, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }
    returnValHRb = DeleteRecordByKey(databaseHandle, TABLE_NAME_INSTALLEXECUTE, FIELDNAME_INSTALLEXECUTE_NAME, CUSTOM_ACTION_NAME_ROLLBACK_C);
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x deleting custom action %s from %s table of %s.\n", returnValHR, CUSTOM_ACTION_NAME_ROLLBACK_C, FIELDNAME_INSTALLEXECUTE_NAME, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }
    returnValHRb = DeleteRecordByKey(databaseHandle, TABLE_NAME_INSTALLEXECUTE, FIELDNAME_INSTALLEXECUTE_NAME, CUSTOM_ACTION_NAME_D);
    if (FAILED(returnValHRb))
    {
        printf("Error 0x%08x deleting custom action %s from %s table of %s.\n", returnValHR, CUSTOM_ACTION_NAME_D, FIELDNAME_INSTALLEXECUTE_NAME, databaseFilename);
        if (SUCCEEDED(returnValHR))
            returnValHR = returnValHRb;
    }

    return returnValHR;
}

HRESULT ExtractResourceToFile( LPCTSTR resourceName, LPTSTR filenameBuffer, DWORD filenameBufferSize )
//Extracts the resource named resourceName to targetFilename
//Returns:
//  HRESULT_FROM_WIN32(ERROR_RESOURCE_NAME_NOT_FOUND)   - named resource not found
//  STG_E_READFAULT                                     - error reading resource data
//  STG_E_LOCKVIOLATION                                 - error locking resource
//  CO_E_FAILEDTOCREATEFILE                             - error creating targetFilename
//  STG_E_WRITEFAULT                                    - error writing data to targetFilename
{
    TCHAR           tempDirectory[MAX_PATH]     = {""};
    HRESULT         returnValHR                 = E_FAIL;
    DWORD           returnValDW                 = ERROR_OUT_OF_PAPER;

    // Validate parmameters
    if (filenameBuffer == NULL)
        return E_POINTER;
    if (filenameBufferSize < 2*MAX_PATH)
        return HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER);

    // Generate a temporary filename
    returnValDW = GetTempPath(sizeof(tempDirectory), tempDirectory);
    if (returnValDW == ERROR_SUCCESS)
        returnValDW = GetTempFileName(tempDirectory, "symuf", 0, filenameBuffer);
    if (returnValDW != ERROR_SUCCESS)
        returnValHR = HRESULT_FROM_WIN32(returnValDW);

    // Extract the resource
    if (SUCCEEDED(returnValHR))
        returnValHR = ExtractResourceToFile(resourceName, filenameBuffer);
    return returnValHR;
}

HRESULT ExtractResourceToFile( LPCTSTR resourceName, LPTSTR targetFilename )
//Extracts the resource named resourceName to targetFilename
//Returns:
//  HRESULT_FROM_WIN32(ERROR_RESOURCE_NAME_NOT_FOUND)   - named resource not found
//  STG_E_READFAULT                                     - error reading resource data
//  STG_E_LOCKVIOLATION                                 - error locking resource
//  CO_E_FAILEDTOCREATEFILE                             - error creating targetFilename
//  STG_E_WRITEFAULT                                    - error writing data to targetFilename
{
    HMODULE         moduleHandle                = NULL;
    TCHAR           tempDirectory[MAX_PATH]     = {""};
    HRSRC           infoHandle                  = NULL;                     //Handle to info about the resource
    HGLOBAL         dataHandle                  = NULL;                     //Handle to the resource data
    LPBYTE          dataPointer                 = NULL;                     //Pointer to the binay resource data in memory
    DWORD           dataSize                    = 0;                        //Size of this resource
    HANDLE          targetFileHandle            = INVALID_HANDLE_VALUE;     //Handle to target file to save this data to
    DWORD           bytesWritten                = 0;                        //Dummy for WriteFile
    HRESULT         returnValHR                 = HRESULT_FROM_WIN32(ERROR_RESOURCE_NAME_NOT_FOUND);
    DWORD           returnValDW                 = ERROR_OUT_OF_PAPER;

    // Validate parameters
    if (resourceName == NULL)
        return E_POINTER;
    if (targetFilename == NULL)
        return E_POINTER;

    moduleHandle = GetModuleHandle(NULL);
    infoHandle = FindResource(moduleHandle, resourceName, RT_RCDATA);
    if (infoHandle != NULL)
    {
        dataHandle = LoadResource(moduleHandle, infoHandle);
        if (dataHandle)
        {
            dataPointer = (LPBYTE) LockResource(dataHandle);
            if (dataPointer)
            {
                dataSize = SizeofResource(moduleHandle, infoHandle);
                targetFileHandle = CreateFile(targetFilename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
                if (targetFileHandle != INVALID_HANDLE_VALUE)
                {
                    if (WriteFile(targetFileHandle, dataPointer, dataSize, &bytesWritten, NULL))
                        returnValHR = S_OK;
                    else
                        returnValHR = STG_E_WRITEFAULT;
                    CloseHandle(targetFileHandle);
                }
                else
                {
                    returnValHR = HRESULT_FROM_WIN32(ERROR_CANNOT_MAKE);
                }
            }
            else
            {
                returnValHR = STG_E_LOCKVIOLATION;
            }
        }
        else
        {
            returnValHR = HRESULT_FROM_WIN32(GetLastError());
        }
    }

    return returnValHR;
}

HRESULT AddBinaryTableRecord( MSIHANDLE databaseHandle, LPCTSTR recordName, LPCTSTR sourceFilename )
{
    MSIHANDLE       recordHandle        = NULL;
    MSIHANDLE       viewHandle          = NULL;
    DWORD           returnValDW         = ERROR_OUT_OF_PAPER;

    // Validate parameters
    if (databaseHandle == NULL)
        return E_INVALIDARG;
    if (recordName == NULL)
        return E_POINTER;
    if (sourceFilename == NULL)
        return E_POINTER;

    // Check to see if the record already exists (if so, nothing to do)
    if (FindRecordByKey(databaseHandle, TABLENAME_BINARY, FIELDNAME_BINARY_NAME, recordName) == S_OK)
        return S_FALSE;

    // Create the record and add it to the table
    returnValDW = MsiDatabaseOpenView(databaseHandle, "SELECT * from Binary", &viewHandle);
    if (returnValDW == ERROR_SUCCESS)
        returnValDW = MsiViewExecute(viewHandle, NULL);
    if (returnValDW == ERROR_SUCCESS)
    {
        recordHandle = MsiCreateRecord(NOFIELDS_BINARY);
        if (recordHandle != NULL)
        {
            returnValDW = MsiRecordSetString(recordHandle, FIELDNO_BINARY_NAME, recordName);
            if (returnValDW == ERROR_SUCCESS)
                returnValDW = MsiRecordSetStream(recordHandle, FIELDNO_BINARY_DATA, sourceFilename);
            if (returnValDW == ERROR_SUCCESS)
                returnValDW = MsiViewModify(viewHandle, MSIMODIFY_INSERT, recordHandle);
            if (returnValDW == ERROR_SUCCESS)
                returnValDW = MsiCloseHandle(recordHandle);
        }
        MsiCloseHandle(viewHandle);
    }

    return HRESULT_FROM_WIN32(returnValDW);
}

HRESULT AddCustomAction( MSIHANDLE databaseHandle, LPCTSTR actionName, int actionType, LPCTSTR sourceText, LPCTSTR targetText )
{
    MSIHANDLE       recordHandle        = NULL;
    MSIHANDLE       viewHandle          = NULL;
    DWORD           returnValDW         = ERROR_OUT_OF_PAPER;

    // Validate parameters
    if (databaseHandle == NULL)
        return E_INVALIDARG;
    if (actionName == NULL)
        return E_POINTER;

    // Check to see if the record already exists (if so, nothing to do)
    if (FindRecordByKey(databaseHandle, TABLENAME_CUSTOMACTION, FIELDNAME_CUSTOMACTION_ACTION, actionName) == S_OK)
        return S_FALSE;

    // Create the record and add it to the table
    returnValDW = MsiDatabaseOpenView(databaseHandle, "SELECT * from CustomAction", &viewHandle);
    if (returnValDW == ERROR_SUCCESS)
        returnValDW = MsiViewExecute(viewHandle, NULL);
    if (returnValDW == ERROR_SUCCESS)
    {
        recordHandle = MsiCreateRecord(NOFIELDS_CUSTOMACTION);
        if (recordHandle != NULL)
        {
            returnValDW = MsiRecordSetString(recordHandle, FIELDNO_CUSTOMACTION_ACTION, actionName);
            if (returnValDW == ERROR_SUCCESS)
                returnValDW = MsiRecordSetInteger(recordHandle, FIELDNO_CUSTOMACTION_TYPE, actionType);
            if (returnValDW == ERROR_SUCCESS)
                returnValDW = MsiRecordSetString(recordHandle, FIELDNO_CUSTOMACTION_SOURCE, sourceText);
            if (returnValDW == ERROR_SUCCESS)
                returnValDW = MsiRecordSetString(recordHandle, FIELDNO_CUSTOMACTION_TARGET, targetText);
            if (returnValDW == ERROR_SUCCESS)
                returnValDW = MsiViewModify(viewHandle, MSIMODIFY_INSERT, recordHandle);
            if (returnValDW == ERROR_SUCCESS)
                returnValDW = MsiCloseHandle(recordHandle);
        }
        MsiCloseHandle(viewHandle);
    }

    return HRESULT_FROM_WIN32(returnValDW);
}

HRESULT AddActionToSequenceTable( MSIHANDLE databaseHandle, LPCTSTR tableName, LPCTSTR actionName, LPCTSTR condition, int sequenceNumber )
{
    TCHAR           queryString[MAX_PATH]       = {""};
    MSIHANDLE       recordHandle                = NULL;
    MSIHANDLE       viewHandle                  = NULL;
    DWORD           returnValDW                 = ERROR_OUT_OF_PAPER;

    // Validate parameters
    if (databaseHandle == NULL)
        return E_INVALIDARG;
    if (tableName == NULL)
        return E_POINTER;
    if (actionName == NULL)
        return E_POINTER;

    // Check to see if the record already exists (if so, nothing to do)
    if (FindRecordByKey(databaseHandle, TABLENAME_INSTALLEXECUTE, FIELDNAME_INSTALLEXECUTE_NAME, actionName) == S_OK)
        return S_FALSE;

    // Create the record and add it to the table
    sssnprintf(queryString, sizeof(queryString), "SELECT * from %s", tableName);
    returnValDW = MsiDatabaseOpenView(databaseHandle, queryString, &viewHandle);
    if (returnValDW == ERROR_SUCCESS)
        returnValDW = MsiViewExecute(viewHandle, NULL);
    if (returnValDW == ERROR_SUCCESS)
    {
        recordHandle = MsiCreateRecord(NOFIELDS_SEQUENCE);
        if (recordHandle != NULL)
        {
            returnValDW = MsiRecordSetString(recordHandle, FIELDNO_SEQUENCE_ACTION, actionName);
            if (returnValDW == ERROR_SUCCESS)
                returnValDW = MsiRecordSetString(recordHandle, FIELDNO_SEQUENCE_CONDITION, condition);
            if (returnValDW == ERROR_SUCCESS)
                returnValDW = MsiRecordSetInteger(recordHandle, FIELDNO_SEQUENCE_SEQUENCE, sequenceNumber);
            if (returnValDW == ERROR_SUCCESS)
                returnValDW = MsiViewModify(viewHandle, MSIMODIFY_INSERT, recordHandle);
            if (returnValDW == ERROR_SUCCESS)
                returnValDW = MsiCloseHandle(recordHandle);
        }
        MsiCloseHandle(viewHandle);
    }

    return HRESULT_FROM_WIN32(returnValDW);
}


HRESULT GetProductsWithUpgradeCode( LPCTSTR upgradeCode, DWORD dwMinMajorVer, DWORD dwMinMinorVer, DWORD dwMaxMajorVer, DWORD dwMaxMinorVer, StringList* productCodes )
// Sets *productCodes equal to a list of all product codes installed with the specified upgrade code
{
    TCHAR       productCode[50]     = {""};
    DWORD       productNo           = 0;
    DWORD       returnValDW         = ERROR_OUT_OF_PAPER;
    TCHAR       majorVer[12]        = {0};
    DWORD       productMajorVer     = 0;
    TCHAR       minorVer[12]        = {0};
    DWORD       productMinorVer     = 0;
    DWORD       stringSize          = 0;

    // Validate parameters
    if (upgradeCode == NULL)
        return E_POINTER;
    if (productCodes == NULL)
        return E_POINTER;

    returnValDW = MsiEnumRelatedProducts(upgradeCode, NULL, productNo, productCode);
    while (returnValDW == ERROR_SUCCESS)
    {
        try
        {
            productMajorVer = 0;
            productMinorVer = 0;

            stringSize = sizeof(majorVer)/sizeof(majorVer[0]);
            returnValDW = MsiGetProductInfo(productCode, INSTALLPROPERTY_VERSIONMAJOR, majorVer, &stringSize);
            if (returnValDW == ERROR_SUCCESS)
            {
                productMajorVer = _ttoi(majorVer);

                stringSize = sizeof(minorVer)/sizeof(minorVer[0]);
                returnValDW = MsiGetProductInfo(productCode, INSTALLPROPERTY_VERSIONMINOR, minorVer, &stringSize);
                if (returnValDW == ERROR_SUCCESS)
                {
                    productMinorVer = _ttoi(minorVer);

                    if ( ( (dwMinMajorVer < productMajorVer) || ( (dwMinMajorVer == productMajorVer) && (dwMinMinorVer <= productMinorVer) ) )
                          && ( (dwMaxMajorVer > productMajorVer) || ( (dwMaxMajorVer == productMajorVer) && (dwMaxMinorVer >= productMinorVer) ) ) )
                    {
                        productCodes->push_back(productCode);
                    }
                }
            }

            productNo += 1;
            returnValDW = MsiEnumRelatedProducts(upgradeCode, NULL, productNo, productCode);
        }
        VP_CATCH_MEMORYEXCEPTIONS(returnValDW = ERROR_NOT_ENOUGH_MEMORY;);
    }

    // Return result
    if (returnValDW == ERROR_NO_MORE_ITEMS)
        return S_OK;
    else
        return HRESULT_FROM_WIN32(returnValDW);
}

int printf( DWORD templateResourceID, ... )
// As printf, but the message template is loaded from the specified string resource
{
    TCHAR       messageTemplate[2*MAX_PATH]     = {""};
    va_list     args;
    int         returnValDW                     = -1;

    if (LoadString(GetModuleHandle(NULL), templateResourceID, messageTemplate, sizeof(messageTemplate)/sizeof(messageTemplate[0])) > 0)
    {
        va_start(args, templateResourceID);
        returnValDW = vprintf(messageTemplate, args);
        va_end(args);
    }
    return returnValDW;
}

HRESULT FindRecordByKey( MSIHANDLE databaseHandle, LPCTSTR tableName, LPCTSTR fieldName, LPCTSTR keyValue, MSIHANDLE* recordHandle )
// Determines if a record exists in the specified table with <fieldName>=<keyValue>
// If recordHandle is != NULL, sets it equal to a handle to the record, if found
// Returns S_OK if record exists, S_FALSE if it does not, else the error code of the failure
{
    TCHAR           queryString[2*MAX_PATH]     = {""};
    MSIHANDLE       viewHandle                  = NULL;
    MSIHANDLE       recordHandleTemp            = NULL;
    DWORD           returnValDW                 = ERROR_OUT_OF_PAPER;
    HRESULT         returnValHR                 = E_FAIL;

    // Validate parameters
    if (databaseHandle == NULL)
        return E_INVALIDARG;
    if (tableName == NULL)
        return E_POINTER;
    if (fieldName == NULL)
        return E_POINTER;
    if (keyValue == NULL)
        return E_POINTER;

    // Create the record and add it to the table
    sssnprintf(queryString, sizeof(queryString), "SELECT * from %s where %s='%s'", tableName, fieldName, keyValue);
    returnValDW = MsiDatabaseOpenView(databaseHandle, queryString, &viewHandle);
    if (returnValDW == ERROR_SUCCESS)
        returnValDW = MsiViewExecute(viewHandle, NULL);
    if (returnValDW == ERROR_SUCCESS)
        returnValDW = MsiViewFetch(viewHandle, &recordHandleTemp);
    if (returnValDW == ERROR_SUCCESS)
    {
        if (recordHandle != NULL)
        {
            *recordHandle = recordHandleTemp;
        }
        else
        {
            MsiCloseHandle(recordHandleTemp);
            recordHandleTemp = NULL;
        }
        MsiCloseHandle(viewHandle);
        returnValHR = S_OK;
    }
    else if (returnValDW == ERROR_NO_MORE_ITEMS)
    {
        returnValHR = S_FALSE;
    }
    else
    {
        returnValHR = HRESULT_FROM_WIN32(returnValDW);
    }

    return returnValHR;
}

HRESULT GetActionSequenceNumber( MSIHANDLE databaseHandle, LPCTSTR tableName, LPCTSTR actionName, DWORD* sequenceNumber )
// Sets *sequenceNumber equal to the sequence number of the specified action in the specified sequence table.
{
    MSIHANDLE       recordHandle        = NULL;
    HRESULT         returnValHR         = E_FAIL;

    // Validate parameters
    if (databaseHandle == NULL)
        return E_POINTER;
    if (tableName == NULL)
        return E_POINTER;
    if (actionName == NULL)
        return E_POINTER;
    if (sequenceNumber == NULL)
        return E_POINTER;

    returnValHR = FindRecordByKey(databaseHandle, tableName, "Action", actionName, &recordHandle);
    if (SUCCEEDED(returnValHR))
    {
        *sequenceNumber = MsiRecordGetInteger(recordHandle, FIELDNO_SEQUENCE_SEQUENCE);
        MsiCloseHandle(recordHandle);
        recordHandle = NULL;
        returnValHR = S_OK;
    }
    return returnValHR;
}

HRESULT DeleteRecordByKey( MSIHANDLE databaseHandle, LPCTSTR tableName, LPCTSTR fieldName, LPCTSTR fieldValue )
// Deletes all records from tableName where <tableName>.<fieldName> = <fieldValue>
// Returns S_OK if at least one record deleted, S_FALSE if no record found that meet query, else the error code of the failure
{
    TCHAR           queryString[2*MAX_PATH]     = {""};
    MSIHANDLE       viewHandle                  = NULL;
    MSIHANDLE       recordHandleTemp            = NULL;
    bool            targetRecordsExist          = false;
    DWORD           returnValDW                 = ERROR_OUT_OF_PAPER;
    HRESULT         returnValHR                 = E_FAIL;

    // Validate parameters
    if (databaseHandle == NULL)
        return E_INVALIDARG;
    if (tableName == NULL)
        return E_POINTER;
    if (fieldName == NULL)
        return E_POINTER;

    // First see if any records match this query
    sssnprintf(queryString, sizeof(queryString), "SELECT * FROM %s where %s='%s'", tableName, fieldName, fieldValue);
    returnValDW = MsiDatabaseOpenView(databaseHandle, queryString, &viewHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValDW = MsiViewExecute(viewHandle, NULL);
        if (returnValDW == ERROR_SUCCESS)
            returnValDW = MsiViewFetch(viewHandle, &recordHandleTemp);
        if (returnValDW == ERROR_SUCCESS)
        {
            targetRecordsExist = true;
            MsiCloseHandle(recordHandleTemp);
            recordHandleTemp = NULL;
        }
        MsiCloseHandle(viewHandle);
        viewHandle = NULL;
    }

    // Now try to delete them
    sssnprintf(queryString, sizeof(queryString), "DELETE FROM %s where %s='%s'", tableName, fieldName, fieldValue);
    returnValDW = MsiDatabaseOpenView(databaseHandle, queryString, &viewHandle);
    if (returnValDW == ERROR_SUCCESS)
        returnValDW = MsiViewExecute(viewHandle, NULL);
    if (returnValDW == ERROR_SUCCESS)
    {
        // NOTE:  Delete is not committed until the handle is closed.
        MsiCloseHandle(viewHandle);
        if (targetRecordsExist)
            returnValHR = S_OK;
        else
            returnValHR = S_FALSE;
    }
    else
    {
        returnValHR = HRESULT_FROM_WIN32(returnValDW);
    }

    return returnValHR;
}

HRESULT ModifyRecordByKey( MSIHANDLE databaseHandle, LPCTSTR tableName, LPCTSTR keyFieldName, LPCTSTR keyFieldValue, unsigned int changeFieldNo, LPCTSTR changeFieldNewValue )
// Modifies the record from tableName where <tableName>.<keyFieldName> = <keyFieldValue>
// Returns S_OK if record modified, S_FALSE if no record found that met query, else the error code of the failure
{
    TCHAR           queryString[2*MAX_PATH]     = {""};
    MSIHANDLE       viewHandle                  = NULL;
    MSIHANDLE       recordHandle                = NULL;
    bool            targetRecordsExist          = false;
    DWORD           returnValDW                 = ERROR_OUT_OF_PAPER;
    HRESULT         returnValHR                 = E_FAIL;

    // Validate parameters
    if (databaseHandle == NULL)
        return E_INVALIDARG;
    if (tableName == NULL)
        return E_POINTER;
    if (keyFieldName == NULL)
        return E_POINTER;
    if (keyFieldValue == NULL)
        return E_POINTER;        

    // Find a record matching this query
    sssnprintf(queryString, sizeof(queryString), "SELECT * FROM %s where %s='%s'", tableName, keyFieldName, keyFieldValue);
    returnValDW = MsiDatabaseOpenView(databaseHandle, queryString, &viewHandle);
    if (returnValDW == ERROR_SUCCESS)
    {
        returnValDW = MsiViewExecute(viewHandle, NULL);
        if (returnValDW == ERROR_SUCCESS)
        {
            returnValDW = MsiViewFetch(viewHandle, &recordHandle);
            if (returnValDW == ERROR_SUCCESS)
            {
                // Modify the record
                returnValDW = MsiRecordSetString(recordHandle, changeFieldNo, changeFieldNewValue);
                if (returnValDW == ERROR_SUCCESS)
					returnValDW = MsiViewModify(viewHandle, MSIMODIFY_UPDATE, recordHandle);
                MsiCloseHandle(recordHandle);
                recordHandle = NULL;
                if (returnValDW == ERROR_SUCCESS)
                    returnValHR = S_OK;
                else
                    returnValHR = HRESULT_FROM_WIN32(returnValDW);
            }
            else if (returnValDW == ERROR_NO_MORE_ITEMS)
            {
                returnValHR = S_FALSE;
            }
            else
            {
                returnValHR = HRESULT_FROM_WIN32(returnValDW);
            }
        }
        else
        {
            returnValHR = HRESULT_FROM_WIN32(returnValDW);
        }
        MsiCloseHandle(viewHandle);
        viewHandle = NULL;
    }

    return returnValHR;
}