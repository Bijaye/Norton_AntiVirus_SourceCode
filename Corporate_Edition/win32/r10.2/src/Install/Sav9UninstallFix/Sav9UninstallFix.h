// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003-2004, 2005 Symantec Corporation. All rights reserved.

// Configure for Win2000 and + symbols
#ifndef _WIN32_WINNT
    #define _WIN32_WINNT        0x0500
#endif
#ifndef WINVER
    #define WINVER              0x0500
#endif
#include <windows.h>
#include <msi.h>
#include <list>
#include <string>

// ** CONSTANTS **
#define SAV_9_UPGRADECODE                   "{24BF7A02-B60A-494B-843A-793BBC77DED4}"
#define SCS_2_UPGRADECODE                   "{C037167E-B804-4CBD-8D52-7D2A84BE543F}"
#define FIXUP_FUNCTION_NAME                 "_ForceUnloadLucomPS@4"
#define BINARY_RECORD_NAME                  "Sav9Fixup"
#define RESOURCE_NAME                       "FIXUPDLL"
#define BINARY_FILENAME                     "ForceUnload.dll"
#define CUSTOM_ACTION_NAME_A                "Sav9to10UninstallFixA"
#define CUSTOM_ACTION_NAME_B                "Sav9to10UninstallFixB"
#define CUSTOM_ACTION_NAME_BASE_FOR_A       "UnRegisterWithLU.6500F9C2_37EA_4F25_A4DE_6211026D9C01"
#define CUSTOM_ACTION_NAME_BASE_FOR_B       "LUUnregCC.18376EA1_A7B1_41ED_A75C_CD6FAB0434AA"
#define CUSTOM_ACTION_NAME_ROLLBACK_A       "RollBackUnRegisterWithLU.6500F9C2_37EA_4F25_A4DE_6211026D9C01"
#define CUSTOM_ACTION_NAME_ROLLBACK_B       "LUUnregCC_Rol.18376EA1_A7B1_41ED_A75C_CD6FAB0434AA"
#define CUSTOM_ACTION_NAME_C                "LUUnregCcProxy.47A1E91F_AF90_4A1A_ADBD_C7D18F172BDC"
#define CUSTOM_ACTION_NAME_ROLLBACK_C       "LUUnregCcProxy_Rol.47A1E91F_AF90_4A1A_ADBD_C7D18F172BDC"
#define CUSTOM_ACTION_NAME_D                "UnregisterWithLU.89FDBB04_BBE6_4132_8FF3_4BCCFB649A89"
// D has no rollback
#define ACTION_NAME_SCHEDULEREBOOT          "ScheduleReboot"
#define TABLE_NAME_INSTALLEXECUTE           "InstallExecuteSequence"
#define NOFIELDS_BINARY                     2
#define FIELDNO_BINARY_NAME                 1
#define FIELDNO_BINARY_DATA                 2
#define NOFIELDS_CUSTOMACTION               4
#define FIELDNO_CUSTOMACTION_ACTION         1
#define FIELDNO_CUSTOMACTION_TYPE           2
#define FIELDNO_CUSTOMACTION_SOURCE         3
#define FIELDNO_CUSTOMACTION_TARGET         4
#define NOFIELDS_SEQUENCE                   3
#define FIELDNO_SEQUENCE_ACTION             1
#define FIELDNO_SEQUENCE_CONDITION          2
#define FIELDNO_SEQUENCE_SEQUENCE           3
#define TABLENAME_BINARY                    "Binary"
#define FIELDNAME_BINARY_NAME               "Name"
#define TABLENAME_CUSTOMACTION              "CustomAction"
#define FIELDNAME_CUSTOMACTION_ACTION       "Action"
#define TABLENAME_INSTALLEXECUTE            "InstallExecuteSequence"
#define FIELDNAME_INSTALLEXECUTE_NAME       "Action"
#define EXIT_CODE_SUCCESS                       0
#define EXIT_CODE_SAVSCSNOTDETECTED             1
#define EXIT_CODE_MSIOPENERROR                  2
#define EXIT_CODE_FIXAPPLYERROR                 3

// ** SAV 10 Fixup Contants
#define SAV_10_UPGRADECODE                  "{24BF7A02-B60A-494B-843A-793BBC77DED4}"
#define SAV_10_x64_UPGRADECODE              "{F7BE9C8A-C2E6-470D-B703-0A1845E6FF8C}"
#define SCS_3_UPGRADECODE                   "{C037167E-B804-4CBD-8D52-7D2A84BE543F}"

#define CUSTOM_ACTION_NAME_10_A             "LogUninstallEvent.93C43188_D2F5_461E_B42B_C3A2A318345C"
#define CUSTOM_ACTION_NAME_10_ROLLBACK_A    "LogUninstallEvent_RB.93C43188_D2F5_461E_B42B_C3A2A318345C"
#define CUSTOM_ACTION_NAME_10_B             "OEMRemoveSettings.97BD4B20_8282_4390_BFA7_87220712F521"
// SAV10 Server Fixup Constants
#define CONDITION_10_E                      "ISSCHEDULEREBOOT OR (MIGRATE AND (NOT OLDERFOUND))"
#define CUSTOM_ACTION_NAME_10_F             "SetRebootAtEnd.6500F9C2_37EA_4F25_A4DE_6211026D9C01"
#define CONDITON_10_F                       "(Version9X AND NOT Installed AND ($SYMTDI_VXD.6500F9C2_37EA_4F25_A4DE_6211026D9C01 = 3)) OR (VersionNT AND NOT Installed AND ($SYMTDI_SYS.6500F9C2_37EA_4F25_A4DE_6211026D9C01 = 3) AND (SYMTDISYS_INSTALLED = 1))"


// ** DATA TYPES **
typedef std::list<std::string> StringList;

// ** HELPERS **
HRESULT ExtractResourceToFile( LPCTSTR resourceName, LPTSTR filenameBuffer, DWORD filenameBufferSize );
HRESULT ExtractResourceToFile( LPCTSTR resourceName, LPTSTR filenameBuffer );
HRESULT AddBinaryTableRecord( MSIHANDLE databaseHandle, LPCTSTR recordName, LPCTSTR sourceFilename );
HRESULT AddCustomAction( MSIHANDLE databaseHandle, LPCTSTR actionName, int actionType, LPCTSTR sourceText, LPCTSTR targetText );
HRESULT AddActionToSequenceTable( MSIHANDLE databaseHandle, LPCTSTR tableName, LPCTSTR actionName, LPCTSTR condition, int sequenceNumber );
// Sets *productCodes equal to a list of all product codes installed with the specified upgrade code
HRESULT GetProductsWithUpgradeCode( LPCTSTR upgradeCode, DWORD dwMinMajorVer, DWORD dwMinMinorVer, DWORD dwMaxMajorVer, DWORD dwMaxMinorVer, StringList* productCodes );
// Determines if a record exists in the specified table with <fieldName>=<keyValue>
// If recordHandle is != NULL, sets it equal to a handle to the record, if found
// Returns S_OK if record exists, S_FALSE if it does not, else the error code of the failure
HRESULT FindRecordByKey( MSIHANDLE databaseHandle, LPCTSTR tableName, LPCTSTR fieldName, LPCTSTR keyValue, MSIHANDLE* recordHandle = NULL );
// As printf, but the message template is loaded from the specified string resource
int printf( DWORD templateResourceID, ... );
// Sets *sequenceNumber equal to the sequence number of the specified action in the specified sequence table.
HRESULT GetActionSequenceNumber( MSIHANDLE databaseHandle, LPCTSTR tableName, LPCTSTR actionName, DWORD* sequenceNumber );
// Deletes all records from tableName where <tableName>.<fieldName> = <fieldValue>
// Returns S_OK if at least one record deleted, S_FALSE if no record found that meet query, else the error code of the failure
HRESULT DeleteRecordByKey( MSIHANDLE databaseHandle, LPCTSTR tableName, LPCTSTR fieldName, LPCTSTR fieldValue );
// Fixes the specified SAV9 cached MSI by sequencing in 2 custom actions to force unload LuComServerPS.dll
// Logs all errors
// Returns S_OK on success, else the error code of the failure
HRESULT ModifyRecordByKey( MSIHANDLE databaseHandle, LPCTSTR tableName, LPCTSTR keyFieldName, LPCTSTR keyFieldValue, unsigned int changeFieldNo, LPCTSTR changeFieldNewValue );
// Modifies the record from tableName where <tableName>.<keyFieldName> = <keyFieldValue>
// Returns S_OK if record modified, S_FALSE if no record found that met query, else the error code of the failure
HRESULT FixSav9ByForceUnload( MSIHANDLE databaseHandle, LPCTSTR databaseFilename );
// Fixes the specified SAV9 cached MSI by deleting the offending custom actions
// Logs all errors
// Returns S_OK on success, else the error code of the failure
HRESULT FixSav9ByRemoveCA( MSIHANDLE databaseHandle, LPCTSTR databaseFilename );
int FixSav9ByRemoveCA( void );
// Fixes the specified SAV10 cached MSI by deleting the offending custom actions
// Logs all errors
// Returns S_OK on success, else the error code of the failure
HRESULT FixSav10ByRemoveCA( MSIHANDLE databaseHandle, LPCTSTR databaseFilename );
// Part of fix to 1-5DXJA1:  When upgrading a 10.0 server, we should not require a reboot.
// The 10.0 uninstall has unnecessary reboot and reboot prompting.  Fix the conditions on the
// these custom actions.
// Returns S_OK on success, S_FALSE if change already made, else the error code of the failure
HRESULT FixSav10ServerUninstall( MSIHANDLE databaseHandle, LPCTSTR databaseFilename );
int FixSav10ByRemoveCA( void );
