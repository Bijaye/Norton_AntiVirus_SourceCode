// NAVOptsMigrate.h
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "ccSettingsInterface.h"
#include "ccSettingsManagerHelper.h"
// #include "NAVSettingsHelper.h"
#include "navopt32.h"
#include "cctrace.h"
#include <string>
#include <map>
#include <Msiquery.h>


// Install should use this class with the navopts.dat file to create a navopts.dat and navopts.def
// ccSettings hive.

typedef std::map< std::basic_string<char>, ccSettings::ISettingsPtr > _NAVSETTINGSMAP;

// May want this class to take an additional parameter telling it to make a duplicate .def hive since we need it...
class CNAVOptMigrate
{
public:
    // Takes an options primitive file and creates a ccSettings store with all of it's
    // values or if an MSI\MSM\WSM database file is passed in as the second parameter it will
    // write all of the options primitive values to a ccSettings table in that database file.
    // If an MSI path is passed in there must also be a component ID provided which will be the
    // component we will associate these ccSettings keys with for removal and adding.
    // If the bCreateDefaults value is set to true there will be an additional ccSettings hive
    // created with the file name passed in using .def instead of .dat to be used as the default
    // settings
    bool MigrateNAVOptFile(LPCTSTR pcszOptFile, bool bCreateDefaults = FALSE, LPCTSTR pcszMSIFile = NULL, LPCTSTR pcszComponentID = NULL)
    {
        // Validate the input paramaters
        if( !pcszOptFile )
            return false;
        
        if( -1 == GetFileAttributes(pcszOptFile) )
            return false;

        if( pcszMSIFile )
        {
            // If an MSI file is passed in there must also be a component ID to use
            if( !pcszComponentID )
                return false;

            DWORD dwAttributes = -1;
            if( -1 == (dwAttributes = GetFileAttributes(pcszMSIFile)) )
                return false;

            // Remove read-only attributes on the MSI file
            SetFileAttributes(pcszMSIFile, dwAttributes & ~FILE_ATTRIBUTE_READONLY);
        }

        // Allocates a HNAVOPTS32 object
        HNAVOPTS32 hOptions;
        NAVOPTS32_STATUS Status = NavOpts32_Allocate(&hOptions);
        if(Status != NAVOPTS32_OK)
            return false;

        // Load the options primitive file
        Status = NavOpts32_Load( pcszOptFile, hOptions, TRUE);
        if(Status != NAVOPTS32_OK)
        {
            NavOpts32_Free(hOptions);
            hOptions = NULL;
            return false;
        }

        // Create the ccSettings key that these values will be stored under
        // Remove the path and just use the file name
        std::string strMainKeyName = pcszOptFile;
        std::string::size_type nBackSlash = std::string::npos;
        if( std::string::npos != (nBackSlash = strMainKeyName.find_last_of('\\')) )
        {
            // There is a backslash...if it's the end of the string
            // remove it and go back one more backslash
            if( nBackSlash == strMainKeyName.length()-1 )
            {
                strMainKeyName.erase(nBackSlash);
                nBackSlash = strMainKeyName.find_last_of('\\');
            }
            strMainKeyName = strMainKeyName.substr(nBackSlash+1);
        }

        strMainKeyName = _T("Norton AntiVirus\\") + strMainKeyName;

        // Get the default hive name if requested to create one
        std::string strDefKeyName;
        if( bCreateDefaults )
        {
            std::string::size_type nDot = strMainKeyName.find(".");
            if( std::string::npos != nDot )
            {
                strDefKeyName = strMainKeyName;
                strDefKeyName.replace(nDot+1, 3, "def");
            }
        }

        // Objects needed to manipulate ccSettings
        ccSettings::CSettingsManagerHelper SettingsHelper;
        ccSettings::ISettingsManagerPtr pSettingsManager;

        // This is the map of settings objects that is built and then stored
        // after all values have been read from the primitive file
        _NAVSETTINGSMAP mapSettingsToStore;
        _NAVSETTINGSMAP mapDefaultSettingsToStore;
        
        // Handle to the MSI DB being modified
        PMSIHANDLE hNAVdb = NULL;

        // If we are writing directly to ccSettings then create the main key now
        if( NULL == pcszMSIFile )
        {
            SYMRESULT symRes = SettingsHelper.Create(pSettingsManager.m_p);
            if (SYM_FAILED(symRes) ||
                pSettingsManager == NULL)
            {
                // Failed to load settings manager
                NavOpts32_Free(hOptions);
                hOptions = NULL;
                return false;
            }

            // Scope this settings object so it is released immediately
            {
                // Create the main settings key
                ccSettings::ISettingsPtr pMainSettings;
                if( SYM_FAILED(pSettingsManager->CreateSettings(strMainKeyName.c_str(), &pMainSettings)) )
                {
                    // Failed to load settings manager
                    NavOpts32_Free(hOptions);
                    hOptions = NULL;
                    return false;
                }
                pSettingsManager->PutSettings(pMainSettings);

                // Create the default hive
                if( bCreateDefaults )
                {
                    pMainSettings.Release();
                    if( SYM_SUCCEEDED(pSettingsManager->CreateSettings(strDefKeyName.c_str(), &pMainSettings)) )
                    {
                        pSettingsManager->PutSettings(pMainSettings);
                    }
                }
            }
        }
        // If we are writing to the msi DB then open it up
        else
        {
            // Open the database for the MSI
	        if( ERROR_SUCCESS != MsiOpenDatabase(pcszMSIFile, MSIDBOPEN_DIRECT, &hNAVdb) )
	        {
		        // Failed to open the database
                NavOpts32_Free(hOptions);
                hOptions = NULL;
                return false;
	        }

            // See if the ccSettings table already exists
            PMSIHANDLE hTempRec = NULL;
            if( ERROR_INVALID_TABLE == MsiDatabaseGetPrimaryKeys(hNAVdb, _T("ccSettings"), &hTempRec) )
            {
                // Create the ccSettings table
                TCHAR szQuery[1024] = {0};
                _stprintf(szQuery, _T("CREATE TABLE `ccSettings` (`ccSettings` CHAR(72) NOT NULL, `Key` CHAR(255) NOT NULL, `Name` CHAR(255) NOT NULL, `Value` CHAR(0), `Component_` CHAR(72) NOT NULL PRIMARY KEY `ccSettings`)"));

                PMSIHANDLE hView = NULL;
                UINT ret = MsiDatabaseOpenView(hNAVdb,szQuery,&hView);

                if( ERROR_BAD_QUERY_SYNTAX == ret )
                {
                    NavOpts32_Free(hOptions);
                    hOptions = NULL;
                    return false;
                }
                else if( ERROR_SUCCESS != ret )
                {
                    NavOpts32_Free(hOptions);
                    hOptions = NULL;
                    return false;
                }

                if( ERROR_SUCCESS != MsiViewExecute(hView, 0) )
                {
                    NavOpts32_Free(hOptions);
                    hOptions = NULL;
                    return false;
                }

                MsiViewClose(hView);
                MsiDatabaseCommit(hNAVdb);
            }
        }

        // Enumerate all of the Navopt primitive values and create ccSettings values for them
	    unsigned int uCount = 0;
	    PNAVOPT32_VALUE_INFO pInfo = NULL, pCurrent = NULL;

        // Get the options info
	    if( NAVOPTS32_OK != NavOpts32_GetValuesInfo(hOptions, &pInfo, &uCount) )
        {
            NavOpts32_Free(hOptions);
            hOptions = NULL;
            return false;
        }

        // Walk the standard options info structure
	    for( unsigned int i = 0; i < uCount; i++ )
	    {
            pCurrent = &pInfo[i];

            // pCurrent->iType is the type of this option
            // pCurrent->szValue is the name of this option

            // Get the ccSettings names
            std::string strKeyName, strValueName, strDefKey;
            GetSettingsNames(pCurrent->szValue, strMainKeyName, strKeyName, strValueName);

            if( bCreateDefaults )
                GetSettingsNames(pCurrent->szValue, strDefKeyName, strDefKey, strValueName);

            // Place the value into ccSettings
            
            if(NAVOPTS32_DATA_DWORD == pCurrent->iType)
            {
                DWORD dwValue = 0;
                if( NAVOPTS32_OK == NavOpts32_GetDwordValue(hOptions, pCurrent->szValue, &dwValue, 0) )
                {
                    // Write directly to ccSettings
                    if( NULL == pcszMSIFile )
                    {
                        // First see if we already created this settings object
                        if( !mapSettingsToStore[strKeyName] )
                        {
                            // This settings object doesn't exist so create it
                            if( SYM_FAILED( pSettingsManager->CreateSettings(strKeyName.c_str(), &mapSettingsToStore[strKeyName]) ) )
                            {
                                CCTRACEE("Could not create a settings object for %s. Moving on to next setting", strKeyName.c_str());
                                continue;
                            }
                        }

                        // Now add the value to the settings object
                        if( SYM_FAILED(mapSettingsToStore[strKeyName]->PutDword(strValueName.c_str(), dwValue)) )
                        {
                            CCTRACEE("Failed to set the setting for Key: %s\\%s to %d", strKeyName.c_str(), strValueName.c_str(), dwValue);
                        }

                        // Do the same stuff for default settings if necessary
                        if( bCreateDefaults )
                        {
                            if( !mapDefaultSettingsToStore[strDefKey] )
                            {
                                // This settings object doesn't exist so create it
                                if( SYM_FAILED( pSettingsManager->CreateSettings(strDefKey.c_str(), &mapDefaultSettingsToStore[strDefKey]) ) )
                                {
                                    CCTRACEE("Could not create a default settings object for %s. Moving on to next setting", strDefKey.c_str());
                                    continue;
                                }
                            }

                            // Now add the value to the settings object
                            if( SYM_FAILED(mapDefaultSettingsToStore[strDefKey]->PutDword(strValueName.c_str(), dwValue)) )
                            {
                                CCTRACEE("Failed to set the default setting for Key: %s\\%s to %d", strDefKey.c_str(), strValueName.c_str(), dwValue);
                            }
                        }
                    }
                    // Write to the msi DB
                    else
                    {
                        // value formats:
                        // # = DWORD value
                        TCHAR szValue[64] = {0};
                        _ultot(dwValue, szValue, 10);

                        std::string strQuery = "INSERT INTO `ccSettings` (`ccSettings`.`ccSettings`, `ccSettings`.`Key`, `ccSettings`.`Name`, `ccSettings`.`Value`, `ccSettings`.`Component_`) VALUES (";
                        strQuery += "'";
                        strQuery += pCurrent->szValue;
                        strQuery += "', ";
                        strQuery += "'";
                        strQuery += strKeyName.c_str();
                        strQuery += "', ";
                        strQuery += "'";
                        strQuery += strValueName.c_str();
                        strQuery += "', ";
                        strQuery += "'#";
                        strQuery += szValue;
                        strQuery += "', ";
                        strQuery += "'";
                        strQuery += pcszComponentID;
                        strQuery += "')";

                        PMSIHANDLE hView = NULL;
                        if( ERROR_SUCCESS == MsiDatabaseOpenView(hNAVdb,strQuery.c_str(),&hView) )
                        {
                            MsiViewExecute(hView, 0);
                            MsiViewClose(hView);
                            MsiDatabaseCommit(hNAVdb);
                        }

                        // Put the default value into the db
                        if( bCreateDefaults )
                        {
                            // Need to make the key value unique
                            std::string strccSettingsDefKey = "DEFAULT_";
                            strccSettingsDefKey += pCurrent->szValue;

                            strQuery = "INSERT INTO `ccSettings` (`ccSettings`.`ccSettings`, `ccSettings`.`Key`, `ccSettings`.`Name`, `ccSettings`.`Value`, `ccSettings`.`Component_`) VALUES (";
                            strQuery += "'";
                            strQuery += strccSettingsDefKey.c_str();
                            strQuery += "', ";
                            strQuery += "'";
                            strQuery += strDefKey.c_str();
                            strQuery += "', ";
                            strQuery += "'";
                            strQuery += strValueName.c_str();
                            strQuery += "', ";
                            strQuery += "'#";
                            strQuery += szValue;
                            strQuery += "', ";
                            strQuery += "'";
                            strQuery += pcszComponentID;
                            strQuery += "')";

                            PMSIHANDLE hView = NULL;
                            if( ERROR_SUCCESS == MsiDatabaseOpenView(hNAVdb,strQuery.c_str(),&hView) )
                            {
                                MsiViewExecute(hView, 0);
                                MsiViewClose(hView);
                                MsiDatabaseCommit(hNAVdb);
                            }
                        }
                    }
                }
            }

            if( NAVOPTS32_DATA_STRING == pCurrent->iType)
            {
                TCHAR szValue[NAVOPTS32_MAX_STRING_LENGTH] = {0};
                if( NAVOPTS32_OK == NavOpts32_GetStringValue(hOptions, pCurrent->szValue, szValue, NAVOPTS32_MAX_STRING_LENGTH, _T("")) )
                {
                    // Place the value in ccSettings
                    if( NULL == pcszMSIFile )
                    {
                        // First see if we already created this settings object
                        if( !mapSettingsToStore[strKeyName] )
                        {
                            // This settings object doesn't exist so create it
                            if( SYM_FAILED( pSettingsManager->CreateSettings(strKeyName.c_str(), &mapSettingsToStore[strKeyName]) ) )
                            {
                                CCTRACEE("Could not create a settings object for %s. Moving on to next setting", strKeyName.c_str());
                                continue;
                            }
                        }

                        // Now add the value to the settings object
                        if( SYM_FAILED(mapSettingsToStore[strKeyName]->PutString(strValueName.c_str(), szValue)) )
                        {
                            CCTRACEE("Failed to set the setting for Key: %s\\%s to %s", strKeyName.c_str(), strValueName.c_str(), szValue);
                        }

                        // Do the same stuff for default settings if necessary
                        if( bCreateDefaults )
                        {
                            if( !mapDefaultSettingsToStore[strDefKey] )
                            {
                                // This settings object doesn't exist so create it
                                if( SYM_FAILED( pSettingsManager->CreateSettings(strDefKey.c_str(), &mapDefaultSettingsToStore[strDefKey]) ) )
                                {
                                    CCTRACEE("Could not create a default settings object for %s. Moving on to next setting", strDefKey.c_str());
                                    continue;
                                }
                            }

                            // Now add the value to the settings object
                            if( SYM_FAILED(mapDefaultSettingsToStore[strDefKey]->PutString(strValueName.c_str(), szValue)) )
                            {
                                CCTRACEE("Failed to set the default setting for Key: %s\\%s to %s", strDefKey.c_str(), strValueName.c_str(), szValue);
                            }
                        }
                    }
                    // Place the value in the MSI database
                    else
                    {
                        std::string strQuery = "INSERT INTO `ccSettings` (`ccSettings`.`ccSettings`, `ccSettings`.`Key`, `ccSettings`.`Name`, `ccSettings`.`Value`, `ccSettings`.`Component_`) VALUES (";
                        strQuery += "'";
                        strQuery += pCurrent->szValue;
                        strQuery += "', ";
                        strQuery += "'";
                        strQuery += strKeyName.c_str();
                        strQuery += "', ";
                        strQuery += "'";
                        strQuery += strValueName.c_str();
                        strQuery += "', ";
                        strQuery += "'";
                        strQuery += szValue;
                        strQuery += "', ";
                        strQuery += "'";
                        strQuery += pcszComponentID;
                        strQuery += "')";

                        PMSIHANDLE hView = NULL;
                        if( ERROR_SUCCESS == MsiDatabaseOpenView(hNAVdb,strQuery.c_str(),&hView) )
                        {
                            MsiViewExecute(hView, 0);
                            MsiViewClose(hView);
                            MsiDatabaseCommit(hNAVdb);
                        }

                        // Put the default value into the db
                        if( bCreateDefaults )
                        {
                            // Need to make the key value unique
                            std::string strccSettingsDefKey = "DEFAULT_";
                            strccSettingsDefKey += pCurrent->szValue;

                            strQuery = "INSERT INTO `ccSettings` (`ccSettings`.`ccSettings`, `ccSettings`.`Key`, `ccSettings`.`Name`, `ccSettings`.`Value`, `ccSettings`.`Component_`) VALUES (";
                            strQuery += "'";
                            strQuery += strccSettingsDefKey.c_str();
                            strQuery += "', ";
                            strQuery += "'";
                            strQuery += strDefKey.c_str();
                            strQuery += "', ";
                            strQuery += "'";
                            strQuery += strValueName.c_str();
                            strQuery += "', ";
                            strQuery += "'";
                            strQuery += szValue;
                            strQuery += "', ";
                            strQuery += "'";
                            strQuery += pcszComponentID;
                            strQuery += "')";

                            PMSIHANDLE hView = NULL;
                            if( ERROR_SUCCESS == MsiDatabaseOpenView(hNAVdb,strQuery.c_str(),&hView) )
                            {
                                MsiViewExecute(hView, 0);
                                MsiViewClose(hView);
                                MsiDatabaseCommit(hNAVdb);
                            }
                        }
                    }
                }
            }

            if( NAVOPTS32_DATA_BINARY == pCurrent->iType)
            {
                BYTE bufValue[NAVOPTS32_MAX_BINARY_LENGTH] = {0};
                if( NAVOPTS32_OK == NavOpts32_GetBinaryValue(hOptions, pCurrent->szValue, bufValue, NAVOPTS32_MAX_BINARY_LENGTH) )
                {
                    if( NULL == pcszMSIFile )
                    {
                        // First see if we already created this settings object
                        if( !mapSettingsToStore[strKeyName] )
                        {
                            // This settings object doesn't exist so create it
                            if( SYM_FAILED( pSettingsManager->CreateSettings(strKeyName.c_str(), &mapSettingsToStore[strKeyName]) ) )
                            {
                                CCTRACEE("Could not create a settings object for %s. Moving on to next setting", strKeyName.c_str());
                                continue;
                            }
                        }

                        // Now add the value to the settings object
                        if( SYM_FAILED(mapSettingsToStore[strKeyName]->PutBinary(strValueName.c_str(), bufValue, NAVOPTS32_MAX_BINARY_LENGTH)) )
                        {
                            CCTRACEE("Failed to set the setting for Key: %s\\%s to some binary value", strKeyName.c_str(), strValueName.c_str());
                        }

                        // Do the same stuff for default settings if necessary
                        if( bCreateDefaults )
                        {
                            if( !mapDefaultSettingsToStore[strDefKey] )
                            {
                                // This settings object doesn't exist so create it
                                if( SYM_FAILED( pSettingsManager->CreateSettings(strDefKey.c_str(), &mapDefaultSettingsToStore[strDefKey]) ) )
                                {
                                    CCTRACEE("Could not create a default settings object for %s. Moving on to next setting", strDefKey.c_str());
                                    continue;
                                }
                            }

                            // Now add the value to the settings object
                            if( SYM_FAILED(mapDefaultSettingsToStore[strDefKey]->PutBinary(strValueName.c_str(), bufValue, NAVOPTS32_MAX_BINARY_LENGTH)) )
                            {
                                CCTRACEE("Failed to set the default setting for Key: %s\\%s to some binary value", strDefKey.c_str(), strValueName.c_str());
                            }
                        }
                    }
                    else
                    {
                        // value formats:
                        // #x = Binary value

                        // Convert the binary buffer to a string containing the binary data
                        std::string strVal = "#x";
                        TCHAR szSingleValue[20] = {0};
                        for(int ind=0; ind<NAVOPTS32_MAX_BINARY_LENGTH; ind++)
                        {
                            _stprintf(szSingleValue, "%02x", bufValue[ind]);
                            strVal += szSingleValue;
                        }

                        std::string strQuery = "INSERT INTO `ccSettings` (`ccSettings`.`ccSettings`, `ccSettings`.`Key`, `ccSettings`.`Name`, `ccSettings`.`Value`, `ccSettings`.`Component_`) VALUES (";
                        strQuery += "'";
                        strQuery += pCurrent->szValue;
                        strQuery += "', ";
                        strQuery += "'";
                        strQuery += strKeyName.c_str();
                        strQuery += "', ";
                        strQuery += "'";
                        strQuery += strValueName.c_str();
                        strQuery += "', ";
                        strQuery += "'";
                        strQuery += strVal.c_str();
                        strQuery += "', ";
                        strQuery += "'";
                        strQuery += pcszComponentID;
                        strQuery += "')";
                        
                        PMSIHANDLE hView = NULL;
                        if( ERROR_SUCCESS == MsiDatabaseOpenView(hNAVdb,strQuery.c_str(),&hView) )
                        {
                            MsiViewExecute(hView, 0);
                            MsiViewClose(hView);
                            MsiDatabaseCommit(hNAVdb);
                        }

                        // Put the default value into the db
                        if( bCreateDefaults )
                        {
                            // Need to make the key value unique
                            std::string strccSettingsDefKey = "DEFAULT_";
                            strccSettingsDefKey += pCurrent->szValue;

                            strQuery = "INSERT INTO `ccSettings` (`ccSettings`.`ccSettings`, `ccSettings`.`Key`, `ccSettings`.`Name`, `ccSettings`.`Value`, `ccSettings`.`Component_`) VALUES (";
                            strQuery += "'";
                            strQuery += strccSettingsDefKey.c_str();
                            strQuery += "', ";
                            strQuery += "'";
                            strQuery += strDefKey.c_str();
                            strQuery += "', ";
                            strQuery += "'";
                            strQuery += strValueName.c_str();
                            strQuery += "', ";
                            strQuery += "'";
                            strQuery += strVal.c_str();
                            strQuery += "', ";
                            strQuery += "'";
                            strQuery += pcszComponentID;
                            strQuery += "')";
                           
                            PMSIHANDLE hView = NULL;
                            if( ERROR_SUCCESS == MsiDatabaseOpenView(hNAVdb,strQuery.c_str(),&hView) )
                            {
                                MsiViewExecute(hView, 0);
                                MsiViewClose(hView);
                                MsiDatabaseCommit(hNAVdb);
                            }
                        }
                    }
                }
            }
        }

        // Free the options info
        NavOpts32_FreeValuesInfo( pInfo );
        NavOpts32_Free(hOptions);
        hOptions = NULL;

        // Commit all of the settings in the map(s) if we are directly porting to ccSettings
        if( NULL == pcszMSIFile )
        {
            _NAVSETTINGSMAP::iterator it;
            SYMRESULT symRes = SYM_OK;
            for( it = mapSettingsToStore.begin(); it != mapSettingsToStore.end(); ++it )
	        {
		        if ( (*it).second )
                {
                    if( SYM_FAILED(symRes = pSettingsManager->PutSettings( (*it).second )) )
                    {
                        CCTRACEE(_T("CNAVOptMigrate::MigrateNAVOptFile() - Failed to save the settings for: %s. SYMRESLT Error Code = 0x%X."), (*it).first.c_str(), symRes);
                    }
                }
	        }

            if( bCreateDefaults )
            {
                for( it = mapDefaultSettingsToStore.begin(); it != mapDefaultSettingsToStore.end(); ++it )
	            {
		            if ( (*it).second )
                    {
                        if( SYM_FAILED(symRes = pSettingsManager->PutSettings( (*it).second )) )
                        {
                            CCTRACEE(_T("CNAVOptMigrate::MigrateNAVOptFile() - Failed to save the default settings for: %s. SYMRESLT Error Code = 0x%X."), (*it).first.c_str(), symRes);
                        }
                    }
	            }
            }
        }

        return true;
    }

private:
    /////////////////////////////////////////////////////////////////////////////
    //
    // Function:	GetSettingsNames
    //
    // Description:	Takes an options primitive formated name lpszColonName and
    //              returns the settings key name and value name
    //              to be used with ccSettings functions
    //
    ////////////////////////////////////////////////////////////////////////////
    void GetSettingsNames(/*in*/LPCTSTR lpszColonName, /*in*/const std::string strSettingsFullKeyName, /*out*/std::string &strKeyName, /*out*/std::string &strValueName)
    {
        strValueName = strSettingsFullKeyName;
        strValueName += "\\";
        strValueName += lpszColonName;
        std::string::size_type nColonPos = strValueName.find_first_of(":");
        strKeyName = strValueName.substr(0, nColonPos);
        strValueName = strValueName.substr(nColonPos+1);
    }
};

// Include MSI.lib and navopt32.lib
#pragma comment( lib, "MSI.LIB" )
#pragma comment( lib, "NAVOpt32.lib" )