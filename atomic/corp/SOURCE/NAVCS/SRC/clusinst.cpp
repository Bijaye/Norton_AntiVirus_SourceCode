
#include "clus.h"

//
// Interface functions
//

CDECLEXPORTAPI ClusterIsServer()
{
    DWORD dwSuccess = false;
    HCLUSTER hCluster = OpenLocalCluster();
    if (hCluster)
    {
        dwSuccess = true;
        CloseCluster(hCluster);
    }
    else
    {
        dwSuccess = false;
    }
    return dwSuccess;
}

//NAVE
CDECLEXPORTAPI ClusterGetName( LPSTR szClusterName, DWORD dwMaxBuffer )
{
    DWORD dwResult = DEFAULT_ERROR;

    WCHAR wcsClusterName[MAX_STR_LENGTH];

    dwResult = ClusterGetNameW(wcsClusterName, MAX_STR_LENGTH);

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = CopyW2A( szClusterName, dwMaxBuffer, wcsClusterName );
    }

    return ConvertErrorCodeToNaveSetupError(dwResult);
}

//NAVE
CDECLEXPORTAPI ClusterGetNodeName( LPSTR szNodeName, DWORD dwMaxBuffer )
{
    DWORD dwResult = DEFAULT_ERROR;

    WCHAR wcsPropertyValue[MAX_STR_LENGTH];

    dwResult = ClusterGetNodeNameW(wcsPropertyValue, MAX_STR_LENGTH);

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = CopyW2A( szNodeName, dwMaxBuffer, wcsPropertyValue );
    }

    return ConvertErrorCodeToNaveSetupError(dwResult);
}



//NAVE
CDECLEXPORTAPI ClusterGetDefaultInstallPath( LPSTR szInstallPath, DWORD dwMaxBuffer )
{
    DWORD dwResult = DEFAULT_ERROR; // some error.

    WCHAR wcsPropertyValue[MAX_STR_LENGTH];

    dwResult = ClusterGetDefaultInstallPathW(wcsPropertyValue, MAX_STR_LENGTH);

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = CopyW2A( szInstallPath, dwMaxBuffer, wcsPropertyValue );
    }

    return ConvertErrorCodeToNaveSetupError(dwResult);
}

//NAVE
CDECLEXPORTAPI ClusterIsValidInstallPath( LPCSTR szInstallPath )
{
    DWORD dwIsValid = NAVE_SETUP_FAILURE;
    WCHAR wcsInstallPath[MAX_STR_LENGTH];
    
    DWORD dwResult = CopyA2W( wcsInstallPath, MAX_STR_LENGTH, szInstallPath );
    if (dwResult == ERROR_SUCCESS)
    {
        if( ClusterIsValidInstallPathW( wcsInstallPath ) )
        {
            dwIsValid = NAVE_SETUP_SUCCESS;
        }
        else
        {
            dwIsValid = NAVE_SETUP_FAILURE;
        }
    }

    return dwIsValid;
}

//NAVE
CDECLEXPORTAPI ClusterGetNetworkName( LPSTR szNetworkName, DWORD dwMaxBuffer )
{
    DWORD dwResult = DEFAULT_ERROR; // some error.

    WCHAR wcsPropertyValue[MAX_STR_LENGTH];

    dwResult = ClusterGetNetworkNameW(wcsPropertyValue, MAX_STR_LENGTH);

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = CopyW2A( szNetworkName, dwMaxBuffer, wcsPropertyValue );
    }

    return ConvertErrorCodeToNaveSetupError(dwResult);
}


//NAVE
CDECLEXPORTAPI ClusterGetIPAddress( LPSTR szIPAddress, DWORD dwMaxBuffer )
{
    DWORD dwResult = DEFAULT_ERROR; // some error.

    WCHAR wcsPropertyValue[MAX_STR_LENGTH];

    dwResult = ClusterGetIPAddressW(wcsPropertyValue, MAX_STR_LENGTH);

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = CopyW2A( szIPAddress, dwMaxBuffer, wcsPropertyValue );
    }

    return ConvertErrorCodeToNaveSetupError(dwResult);
}

//NAVE
CDECLEXPORTAPI ClusterGetIPSubnetMask( LPSTR szMask, DWORD dwMaxBuffer )
{
    DWORD dwResult = DEFAULT_ERROR; // some error.

    WCHAR wcsPropertyValue[MAX_STR_LENGTH];

    dwResult = ClusterGetIPSubnetMaskW(wcsPropertyValue, MAX_STR_LENGTH);

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = CopyW2A( szMask, dwMaxBuffer, wcsPropertyValue );
    }

    return ConvertErrorCodeToNaveSetupError(dwResult);
}


//
// Internal Interface functions
//


DWORD ClusterGetNameW( LPWSTR wcsClusterName, DWORD szNameSize )
{
    DWORD dwResult = DEFAULT_ERROR;

    ZeroMemory( wcsClusterName, szNameSize );
    // DWORD szNameSize = 1024;

    HCLUSTER hCluster = OpenLocalCluster();
    if (hCluster)
    {
        CLUSTERVERSIONINFO stClusterInfo;
        ZeroMemory( &stClusterInfo, sizeof(stClusterInfo) );
        stClusterInfo.dwVersionInfoSize = sizeof(stClusterInfo);

        dwResult = GetClusterInformation(  
            hCluster,                  
            wcsClusterName,
            &szNameSize,           
            &stClusterInfo  );

        if (dwResult == ERROR_SUCCESS)
        {
            // DEBUG_MSG_W( wcsClusterName );
        }

        BOOL bSuccess = CloseCluster(hCluster);
        if (!bSuccess)
        {
            DEBUG_MSG( "EEEP! CloseCluster Failed" );
        }
    }
    else
    {
        // Could not open cluster.
        dwResult = GetLastError();
        VALIDATE_LAST_ERROR(dwResult); 
    }

    return dwResult;
}


DWORD ClusterGetNodeNameW( LPWSTR wcsNodeName, DWORD dwMaxBuffer )
{
    DWORD dwResult = DEFAULT_ERROR;

    BOOL bSuccess = GetComputerNameW(wcsNodeName, &dwMaxBuffer);
    if (bSuccess)
    {
        dwResult = ValidateNodeName( wcsNodeName );
    }
    else
    {
        dwResult = GetLastError();
        VALIDATE_LAST_ERROR( dwResult );
    }

    return dwResult;
}




DWORD ClusterGetDefaultInstallPathW( LPWSTR wcsInstallPath, DWORD dwMaxBuffer )
{
    DWORD dwResult = DEFAULT_ERROR;
    
    WCHAR wcsPhysicalDisk[MAX_STR_LENGTH];

    // Should this be a list of physical disks?
    dwResult = GetPhysicalDisk(wcsPhysicalDisk, MAX_STR_LENGTH);
    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = CopyW2W( wcsInstallPath, dwMaxBuffer, wcsPhysicalDisk );
        if (dwResult == ERROR_SUCCESS)
        {
            LPCWSTR cwcsDefaultDir = DEFAULT_PATH;
            dwResult = CatW2W( wcsInstallPath, dwMaxBuffer, cwcsDefaultDir );
        }
    }
    else
    {
        DEBUG_MSG("Could not get physical disk");
    }

    return dwResult;
}

BOOL ClusterIsValidInstallPathW( LPCWSTR wcsInstallPath )
{
    BOOL bIsValid = FALSE;

    DWORD dwResult = DEFAULT_ERROR;
    
/*
    TValidatePathOnDisk Validator;

    dwResult = Validator.IsValidPath( wcsInstallPath );

    TRACE_VALUE( dwResult );

    bIsValid = (dwResult == ERROR_SUCCESS);
*/

    WCHAR wcsPhysicalDisk[MAX_STR_LENGTH];

    // Should this be a list of physical disks?
    dwResult = GetPhysicalDisk(wcsPhysicalDisk, MAX_STR_LENGTH);
    if (dwResult == ERROR_SUCCESS)
    {
        WCHAR wcsInstallDrive[_MAX_DRIVE];
        _wsplitpath(wcsInstallPath, wcsInstallDrive, NULL, NULL, NULL);

        if (wcscmp(wcsPhysicalDisk, wcsInstallDrive) == 0)
        {
            bIsValid = TRUE;
        }
        else
        {
            bIsValid = FALSE;
        }
    }
    else
    {
        DEBUG_MSG("Could not get physical disk");
    }

    return bIsValid;
}

DWORD ClusterGetNetworkNameW( LPWSTR wcsValue, DWORD dwBufferSize )
{
    TFindNetworkName find;
    DWORD dwResult = GetResourcePropertyInExchangeGroup( find, wcsValue, dwBufferSize);
    return dwResult;
}

DWORD ClusterGetIPAddressW( LPWSTR wcsValue, DWORD dwBufferSize )
{
    TFindIPAddress find;
    DWORD dwResult = GetResourcePropertyInExchangeGroup( find, wcsValue, dwBufferSize );
    return dwResult;
}


DWORD ClusterGetIPSubnetMaskW( LPWSTR wcsValue, DWORD dwBufferSize )
{
    TFindIPSubnetMask find;
    DWORD dwResult = GetResourcePropertyInExchangeGroup( find, wcsValue, dwBufferSize );
    return dwResult;
}

//
//  Implementation functions.
//

HCLUSTER OpenLocalCluster()
{
    HCLUSTER hCluster = NULL;
    // Note: if the open is valid function then every else should be too.
    /*
    try
    {
    */
        hCluster = OpenCluster( NULL );    // open local cluster

    /*
    }
    catch(...)
    {
        // Most likely a delay load failure because the dll is missing.
        // This is an expected failure.
        SetLastError( ERROR_NOT_SUPPORTED );
    }
    */

    return hCluster;
}


DWORD ValidateNodeName( LPWSTR wcsNodeName )
{
    DWORD dwResult = DEFAULT_ERROR;

    HCLUSTER hCluster = OpenLocalCluster();
    if (hCluster)
    {
        HNODE hNode = OpenClusterNode( hCluster, wcsNodeName );
        if (hNode)
        {
            dwResult = ERROR_SUCCESS;
            if (!CloseClusterNode( hNode ))
            {
                DEBUG_MSG("Can not close cluster node.");
            }
        }
        else
        {
            dwResult = GetLastError();
            VALIDATE_LAST_ERROR( dwResult );
        }

        BOOL bSuccess = CloseCluster(hCluster);
        if (!bSuccess)
        {
            DEBUG_MSG( "EEEP! CloseCluster Failed" );
        }
    }
    else
    {
        // Could not open cluster.
        dwResult = GetLastError();
        VALIDATE_LAST_ERROR(dwResult); 
    }

    return dwResult;
}




DWORD AddDependencyToResource( 
        HCLUSTER hCluster, 
        HRESOURCE hResource, 
        LPWSTR wcsResourceDependency )
{
    DWORD dwResult = DEFAULT_ERROR;

    HRESOURCE hDependency = OpenClusterResource( hCluster, wcsResourceDependency );
    if (hDependency)
    {
        dwResult = AddClusterResourceDependency( hResource, hDependency );
        if (dwResult != ERROR_SUCCESS)
        {
            WCHAR wcsMsg[MAX_STR_LENGTH];
            wsprintfW(wcsMsg, L"Could not add dependency %s", wcsResourceDependency );
            DEBUG_MSG_W(wcsMsg);
        }

        if (!CloseClusterResource( hDependency ) )
        {
            DEBUG_MSG("CloseClusterResource failed");
        }
    }
    else
    {
        dwResult = GetLastError();
        DEBUG_MSG("OpenClusterResource failed.");
        VALIDATE_LAST_ERROR( dwResult );
    }

    return dwResult;
}


DWORD ValidatePrivateProperties( 
        HRESOURCE hResource, 
        CLUSPROP_LIST * pPropertyList,
        DWORD dwListBufferSize )
{
    DWORD dwResult = DEFAULT_ERROR;

    dwResult = ClusterResourceControl( 
        hResource, 
        NULL,
        CLUSCTL_RESOURCE_VALIDATE_PRIVATE_PROPERTIES,
        pPropertyList,
        dwListBufferSize,
        NULL,
        0,
        NULL );
    
    return dwResult;
}

DWORD SetPrivateProperties(
        HRESOURCE hResource, 
        CLUSPROP_LIST * pPropertyList,
        DWORD dwListBufferSize )
{
    DWORD dwResult = DEFAULT_ERROR;

    dwResult = ClusterResourceControl( 
        hResource, 
        NULL,
        CLUSCTL_RESOURCE_SET_PRIVATE_PROPERTIES,
        pPropertyList,
        dwListBufferSize,
        NULL,
        0,
        NULL );

    if (dwResult == ERROR_RESOURCE_PROPERTIES_STORED)
    {
        dwResult = ERROR_SUCCESS;
    }

    return dwResult;
}



DWORD AddRegKeyCheckPointsToResource( 
        HRESOURCE hResource,
        LPWSTR wcsRegKey )
{
    DWORD dwResult = DEFAULT_ERROR;

    DWORD dwRegKeyLength = (wcslen(wcsRegKey)+ 1) * sizeof( WCHAR );

    dwResult = ClusterResourceControl( 
        hResource, 
        NULL,
        CLUSCTL_RESOURCE_ADD_REGISTRY_CHECKPOINT,
        wcsRegKey,
        dwRegKeyLength,
        NULL,
        0,
        NULL );
    
    return dwResult;
}



DWORD GetPhysicalDisk(LPWSTR wcsValue, DWORD dwBufferSize)
{
    TFindPhysicalDisk find;
    DWORD dwResult = GetResourcePropertyInExchangeGroup( find, wcsValue, dwBufferSize );
    return dwResult;
}


//
//
DWORD IsClusterServiceResource( 
        HCLUSTER hCluster,  
        LPCWSTR wcsResourceName,
        LPCWSTR wcsServiceNameToMatch,
        BOOL * p_bIsMatch)
{
    DWORD dwResult = DEFAULT_ERROR;
    BOOL bIsMatching = FALSE;

    HRESOURCE hResource = NULL;
    hResource = OpenClusterResource( hCluster, wcsResourceName );
    if (hResource)
    {
        if (IsGenericServiceResource(hResource))
        {
            WCHAR wcsServiceNamePropertyValue[MAX_STR_LENGTH];
            DWORD dwServiceNameBufferSize = MAX_STR_LENGTH;

            dwResult = GetResourcePropertyString( 
                    hResource,  
                    L"ServiceName",
                    wcsServiceNamePropertyValue,
                    dwServiceNameBufferSize);
            if (dwResult == ERROR_SUCCESS)
            {
                if (wcscmp(wcsServiceNamePropertyValue, wcsServiceNameToMatch)==0)
                {
                    bIsMatching = TRUE;
                }
            }
            else
            {
                DEBUG_MSG("Failed to get ServiceName property");
            }

        }
        else
        {
            dwResult = ERROR_SUCCESS;
        }

        BOOL bSuccess = CloseClusterResource( hResource );
        if (!bSuccess)
        {
            DEBUG_MSG(" CloseClusterResource failed! "); 
        }
    }
    else
    {
        dwResult = GetLastError();
        DEBUG_MSG(" OpenClusterResource failed! "); 
        VALIDATE_LAST_ERROR( dwResult );
    }

    *p_bIsMatch = bIsMatching;
    return dwResult;
}


//
//
DWORD GetResourcePropertyString( 
        HRESOURCE hResource,  
        LPCWSTR wcsPropertyName,
        LPWSTR wcsBuffer,
        DWORD dwMaxBuffer)
{
    DWORD dwResult = ERROR_NOT_FOUND;
    DWORD dwBufferSize = 0;
    
    dwResult = ClusterResourceControl( 
        hResource, 
        NULL, 
        CLUSCTL_RESOURCE_GET_PRIVATE_PROPERTIES,
        NULL,
        0,
        NULL,
        0,
        &dwBufferSize );
    
    // I was expecting ERROR_MORE_DATA, but I got ERROR_SUCCESS, so let us handle both.
    if ( ( (dwResult == ERROR_SUCCESS) || (dwResult == ERROR_MORE_DATA) ) && (dwBufferSize > 0))
    {
        CLUSPROP_LIST * pPropertyList = (CLUSPROP_LIST *) malloc(dwBufferSize);
        
        dwResult = ClusterResourceControl( 
            hResource, 
            NULL, 
            CLUSCTL_RESOURCE_GET_PRIVATE_PROPERTIES,
            NULL,
            0,
            (void *)pPropertyList,
            dwBufferSize,
            &dwBufferSize );
        
        if (dwResult == ERROR_SUCCESS)
        {
            LPWSTR pValue = NULL;
            BOOL bFoundProperty = FindPropertyString(
                pPropertyList,
                dwBufferSize,
                wcsPropertyName,
                &pValue );
            
            if (bFoundProperty) // if (dwResult == ERROR_SUCCESS)
            {
                dwResult = CopyW2W( wcsBuffer, dwMaxBuffer, pValue );
            }
            else
            {
                dwResult = ERROR_NOT_FOUND;
                DEBUG_MSG("EEP! Could not find property");
            }
        }
        else
        {
            DEBUG_MSG("EEP! ClusterResourceControl failed");
        }

        free( pPropertyList );
        pPropertyList = NULL;
    }
    else
    {
        DEBUG_MSG("EEP! ClusterResourceControl is behaving unexpectedly");
    }
        

    return dwResult;
}

BOOL IsIPAddressResource( 
        HRESOURCE hResource )
{
    return IsResourceOfType( 
            hResource, 
            L"IP Address");
}

BOOL IsGenericServiceResource( 
        HRESOURCE hResource )
{
    return IsResourceOfType( 
            hResource, 
            L"Generic Service");

}

BOOL IsNetworkNameResource( 
        HRESOURCE hResource )
{
    return IsResourceOfType( 
            hResource, 
            L"Network Name");
}

BOOL IsPhysicalDiskResource( 
        HRESOURCE hResource )
{
    return IsResourceOfType( 
            hResource, 
            L"Physical Disk");
}

BOOL IsResourceOfType( 
        HRESOURCE hResource, 
        LPWSTR wcsResourceTypeToMatch )
{
    BOOL bIsMatch = FALSE;

    DWORD dwResult = ERROR_SUCCESS;
    WCHAR wcsResourceType[MAX_STR_LENGTH];
    
    dwResult = GetResourceType(hResource, wcsResourceType);
    if (dwResult == ERROR_SUCCESS)
    {
        // DEBUG_MSG_W(wcsResourceType);
        if (wcscmp(wcsResourceType,wcsResourceTypeToMatch) == 0)
        {
            bIsMatch = TRUE;
        }
        else
        {
            bIsMatch = FALSE;
        }
    }

    return bIsMatch;
}

// Assumes a MAX_STR_LENGTH character buffer.
DWORD GetResourceType( 
        HRESOURCE hResource, 
        LPWSTR wczTypeName )
{
    DWORD dwResult = ERROR_SUCCESS;

    dwResult = ClusterResourceControl( 
        hResource, 
        NULL, 
        CLUSCTL_RESOURCE_GET_RESOURCE_TYPE,
        NULL,
        0,
        wczTypeName,
        MAX_STR_LENGTH,
        NULL );


    if (dwResult != ERROR_SUCCESS)
    {
        DEBUG_MSG("ClusterResourceControl - GET_RESOURCE_TYPE failed.");
    }
        
    return dwResult;
}

BOOL FindPropertyString(
    CLUSPROP_LIST * pPropertyList,
    DWORD dwBufferSize,
    LPCWSTR wcsPropertyToFind,
    LPWSTR * pwcsValue )
{
    BOOL bSuccess = FALSE;
 
    BOOL   bContinue   = TRUE;          // loop exit condition
    BOOL   bFound      = FALSE;         // tests whether property has been found
    DWORD  cbOffset    = 0;    // offset to next entry in the value list
    DWORD  cbPosition  = 0;    // tracks the advance through the value list buffer
    DWORD  dwStringLength = 0;

//   debug
/*

    TRACE_VALUE( CLUSPROP_SYNTAX_ENDMARK         );
    TRACE_VALUE( CLUSPROP_SYNTAX_NAME            );
    TRACE_VALUE( CLUSPROP_SYNTAX_RESCLASS        );

    TRACE_VALUE( CLUSPROP_SYNTAX_LIST_VALUE_SZ           );
    TRACE_VALUE( CLUSPROP_SYNTAX_LIST_VALUE_EXPAND_SZ    );
    TRACE_VALUE( CLUSPROP_SYNTAX_LIST_VALUE_DWORD        );
    TRACE_VALUE( CLUSPROP_SYNTAX_LIST_VALUE_BINARY       );
    TRACE_VALUE( CLUSPROP_SYNTAX_LIST_VALUE_MULTI_SZ     );

    TRACE_VALUE( CLUSPROP_SYNTAX_DISK_SIGNATURE  );
    TRACE_VALUE( CLUSPROP_SYNTAX_SCSI_ADDRESS    );
    TRACE_VALUE( CLUSPROP_SYNTAX_DISK_NUMBER     );
    TRACE_VALUE( CLUSPROP_SYNTAX_PARTITION_INFO  );
    TRACE_VALUE( CLUSPROP_SYNTAX_FTSET_INFO      );
*/
// end debug


    CLUSPROP_BUFFER_HELPER ListEntry;  // to parse the list

    //    
    // Set the pb member to the start of the list
    //
    ListEntry.pList = pPropertyList;
    
    // "Walk the buffer" to the location of the next list entry
    ListEntry.pb += sizeof( ListEntry.pList->nPropertyCount );


    int nMaxProperties = pPropertyList->nPropertyCount;
    int nNextProperty = 1;
    int nVerifyPropertyCount = 0;

    //
    // Main loop:
    // 1. Check syntax of current list entry
    // 2. If it is a property name, check that we have the right property.
    // 3. If it is a string value, check that we found the right name.
    // 4. Advance the position counter and test vs. size of list.    
    //
    do
    {        
        switch( *ListEntry.pdw ) // check the syntax of the entry        
        {
        case CLUSPROP_SYNTAX_ENDMARK:
            cbOffset = sizeof( DWORD );
            nNextProperty++;
            break;
        
        case CLUSPROP_SYNTAX_NAME:
            nVerifyPropertyCount++;
            //
            // If this is the property,
            // The next pass through the loop should yield the Security value.
            TRACE_STRING_W(ListEntry.pName->sz);
            if ( wcscmp( ListEntry.pName->sz, wcsPropertyToFind ) == 0 )
            {
                // wprintf( L"Found name.\n" );
                bFound = TRUE;
            }
            else
            {
                bFound = FALSE;
            }
            //
            // Calculate offset to next entry. Note the use of ALIGN_CLUSPROP
            //
            cbOffset = sizeof( *ListEntry.pName ) + ALIGN_CLUSPROP( ListEntry.pName->cbLength );
            break;
        
        case CLUSPROP_SYNTAX_LIST_VALUE_SZ:
            // If this is the property we are looking for, bFound will be TRUE
            //
            TRACE_STRING_W(ListEntry.pStringValue->sz);
            if ( bFound )
            {
                dwStringLength = (ListEntry.pStringValue->cbLength) / sizeof(WCHAR);
                *pwcsValue = ListEntry.pStringValue->sz;

                if ((*pwcsValue)[dwStringLength] != 0)
                {
                    DEBUG_MSG("EEEP! Bad String?");
                }
                
                bSuccess = TRUE;
                bContinue = FALSE; // stop the loop
            }
            else
            {
                cbOffset = sizeof( *ListEntry.pStringValue ) + ALIGN_CLUSPROP( ListEntry.pStringValue->cbLength );
            }
            break;

        // we are only interested in strings here, skip over the next.
        case CLUSPROP_SYNTAX_LIST_VALUE_DWORD:
        case CLUSPROP_SYNTAX_LIST_VALUE_BINARY:

            cbOffset = sizeof( *ListEntry.pValue ) + ALIGN_CLUSPROP( ListEntry.pValue->cbLength );
            break;
        
        
        default:
#ifdef _DEBUG
            DebugBreak();  // unknown... possible alignment problem?
#endif
            cbOffset = sizeof( *ListEntry.pValue ) + ALIGN_CLUSPROP( ListEntry.pValue->cbLength );
            break;
        }

        // Have we traversed all the properties?
        if (nNextProperty > nMaxProperties)
        {
            if (nVerifyPropertyCount != (nNextProperty-1))
            {
                DEBUG_MSG("Too many CLUSPROP_SYNTAX_ENDMARK");
            }

            break;
        }

        //
        // Verify that the offset to the next entry is
        // within the value list buffer, then advance
        // the CLUSPROP_BUFFER_HELPER pointer.
        //
        if ( cbPosition == dwBufferSize )
        {
            DEBUG_MSG("Clean but premature end to list.");
            break;
        }

        cbPosition += cbOffset;
        if ( cbPosition > dwBufferSize )
        {
            DEBUG_MSG("Gone past the end of buffer");
            break;
        }

        ListEntry.pb += cbOffset;    
    } while ( bContinue );

    return bSuccess;
}


TEnumerateCluster::TEnumerateCluster()
    : m_hCluster( NULL ),
      m_wcsCurrentGroup( NULL ),
      m_wcsCurrentResource( NULL )
{
    // nothing else to do
}

TEnumerateCluster::~TEnumerateCluster()
{
    if (m_hCluster)
    {
        Close();
    }
}

DWORD TEnumerateCluster::Open()
{
    DWORD dwResult = DEFAULT_ERROR;

    if (!m_hCluster)
    {
        m_hCluster = OpenLocalCluster();
        if (m_hCluster)
        {
            dwResult = ERROR_SUCCESS;
        }
        else
        {
            dwResult = GetLastError();
            DEBUG_MSG("OpenLocalCluster failed.");
            VALIDATE_LAST_ERROR(dwResult);
        }
    }
    
    return dwResult;
}


DWORD TEnumerateCluster::Close()
{
    DWORD dwResult = DEFAULT_ERROR;

    if (m_hCluster)
    {
        if (CloseCluster(m_hCluster))
        {
            dwResult = ERROR_SUCCESS;
        }
        else
        {
            dwResult = GetLastError();
            DEBUG_MSG("CloseCluster failed.");
            VALIDATE_LAST_ERROR(dwResult);
        }

        m_hCluster = NULL;
    }

    
    return dwResult;
}

DWORD TEnumerateCluster::Enum(DWORD dwEnumType)
{

    if ( (dwEnumType != CLUSTER_ENUM_GROUP) 
         && (dwEnumType != CLUSTER_ENUM_RESOURCE))
    {
        return ERROR_INVALID_PARAMETER;
    }


    DWORD dwResult = ERROR_SUCCESS;
    // BOOL bFound = FALSE;

    if ( m_hCluster == NULL )
    {
        dwResult = Open();
        if (dwResult != ERROR_SUCCESS)
        {
            return dwResult;
        }
    }

    dwResult = OnPreEnum();
    if (dwResult != ERROR_SUCCESS)
    {
        return dwResult;
    }

    HCLUSENUM hEnum = ClusterOpenEnum( m_hCluster, dwEnumType );
    if (hEnum)
    {
        DWORD dwEnumIndex = 0;
        DWORD dwType = 0;
        WCHAR wcsItemName[2048];
        const DWORD dwNameMaxSize = 2048;

        do
        {
            DWORD dwNameSize = dwNameMaxSize;
            dwResult = ClusterEnum( 
                hEnum, 
                dwEnumIndex,
                &dwType,
                wcsItemName,
                &dwNameSize );
            if (dwResult == ERROR_SUCCESS)
            {
                BOOL bContinue = TRUE;

                if (dwType == CLUSTER_ENUM_GROUP)
                {
                    m_wcsCurrentGroup = wcsItemName;
                    dwResult = OnGroup( 
                        wcsItemName,
                        &bContinue);
                    if (dwResult == ERROR_SUCCESS)
                    {
                        if (!bContinue)
                        {
                            // Stop requested.
                            break;
                        }
                    }
                    else
                    {
                        DEBUG_MSG("OnGroupFailed.");
                        break;
                    }
                    m_wcsCurrentGroup = NULL;
                }
                else if (dwType == CLUSTER_ENUM_RESOURCE)
                {
                    m_wcsCurrentResource = wcsItemName;
                    dwResult = OnResource( 
                        wcsItemName,
                        &bContinue);
                    if (dwResult == ERROR_SUCCESS)
                    {
                        if (!bContinue)
                        {
                            // Stop requested.
                            break;
                        }
                    }
                    else
                    {
                        DEBUG_MSG("OnGroupFailed.");
                        break;
                    }
                    m_wcsCurrentResource = NULL;
                }
                else
                {
                    DEBUG_MSG("EEP! Unknown Enum Type");
                }

                dwEnumIndex++;  // go to the next one.
            }
            else
            {
                if ( dwResult == ERROR_NO_MORE_ITEMS )
                {
                    // Done and not found, that is still a success.
                    dwResult = ERROR_SUCCESS;
                    break;
                }
            }


        } while (dwResult == ERROR_SUCCESS);

        if (ClusterCloseEnum( hEnum ) != ERROR_SUCCESS)
        {
            DEBUG_MSG("ClusterCloseEnum Failed!");
        }
    }
    else
    {
        dwResult = GetLastError();
        VALIDATE_LAST_ERROR( dwResult );
    }

    dwResult = OnPostEnum(dwResult);

    return dwResult;
}

DWORD TEnumerateCluster::OnGroup( LPWSTR wcsName, BOOL * p_bContinue )
{
    DWORD dwResult = DEFAULT_ERROR;

    if ( m_hCluster == NULL )
    {
        dwResult = Open();
        if (dwResult != ERROR_SUCCESS)
        {
            return dwResult;
        }
    }

    HGROUP hGroup = OpenClusterGroup(m_hCluster, wcsName);
    if (hGroup)
    {
        HGROUPENUM hEnum = ClusterGroupOpenEnum( hGroup, CLUSTER_GROUP_ENUM_CONTAINS );
        if (hEnum)
        {
            DWORD dwEnumIndex = 0;
            DWORD dwType = 0;
            WCHAR wcsItemName[MAX_STR_LENGTH];
            const DWORD dwNameMaxSize = MAX_STR_LENGTH;
            
            do
            {
                DWORD dwNameSize = dwNameMaxSize;
                dwResult = ClusterGroupEnum( 
                    hEnum, 
                    dwEnumIndex,
                    &dwType,
                    wcsItemName,
                    &dwNameSize );
                if (dwResult == ERROR_SUCCESS)
                {
                    if (dwType == CLUSTER_GROUP_ENUM_CONTAINS)
                    {
                        m_wcsCurrentResource = wcsItemName;
                        dwResult = OnResource( 
                            wcsItemName,
                            p_bContinue);
                        if (dwResult == ERROR_SUCCESS)
                        {
                            if (!*p_bContinue)
                            {
                                // Stop requested.
                                m_wcsCurrentResource = NULL; 
                                break;
                            }
                        }
                        else
                        {
                            DEBUG_MSG("OnResource Failed."); 
                            m_wcsCurrentResource = NULL;
                            break;
                        }
                        m_wcsCurrentResource = NULL;
                    }
                    else
                    {
                        DEBUG_MSG("EEP! unexpected enum type");
                    }
                    
                    dwEnumIndex++;  // go to the next one.
                }
                else
                {
                    if ( dwResult == ERROR_NO_MORE_ITEMS )
                    {
                        // Done and not found, that is still a success.
                        dwResult = ERROR_SUCCESS;
                        break;
                    }
                }
                
                
            } while (dwResult == ERROR_SUCCESS);
            
            if (ClusterGroupCloseEnum( hEnum ) != ERROR_SUCCESS)
            {
                DEBUG_MSG("ClusterGroupCloseEnum Failed!");
            }
        }
        else
        {
            dwResult = GetLastError();
            DEBUG_MSG("ClusterGroupOpenEnum failed.");
            VALIDATE_LAST_ERROR( dwResult );
        }

        if (!CloseClusterGroup( hGroup ))
        {
            DEBUG_MSG("ClusterCloseGroup Failed!");
        }
    }
    else
    {
        dwResult = GetLastError();
        DEBUG_MSG("OpenClusterGroup failed.");
        VALIDATE_LAST_ERROR( dwResult );
    }

    return dwResult;
}

DWORD TEnumerateCluster::OnResource( LPWSTR /*wcsName*/, BOOL * /*p_bContinue*/ )
{
    return ERROR_NOT_SUPPORTED;
}

//
// TFindResource
//

TFindResource::TFindResource()
{
    Init();
}

void TFindResource::Init()
{
    ZeroMemory(m_wcsResourceValue, (MAX_STR_LENGTH * sizeof(WCHAR)) );
    ZeroMemory(m_wcsResourceName, (MAX_STR_LENGTH * sizeof(WCHAR)) );
    
    m_bFound = FALSE;
}

DWORD TFindResource::OnPreEnum()
{
    Init();
    return TEnumerateCluster::OnPreEnum();
}

DWORD TFindResource::OnResource( LPWSTR wcsName, BOOL * p_bContinue )
{
    m_bFound = FALSE;
    *p_bContinue = TRUE;
    DWORD dwResult = IsMatchingResource( wcsName, &m_bFound );
    if (dwResult == ERROR_SUCCESS)
    {
        if (m_bFound)
        {
            dwResult = SetNameForResource( wcsName );
            if (dwResult == ERROR_SUCCESS)
            {
                if (m_wcsCurrentGroup != NULL)
                {
                    dwResult = SetValueForResource( m_wcsCurrentGroup );
                }
            }

            *p_bContinue = FALSE;
        }
    }
    else
    {
        DEBUG_MSG("IsMatchingResource failed");
    }

    return dwResult;
}

DWORD TFindResource::SetValueForResource( LPWSTR wcsValue )
{
    DWORD dwResult = CopyW2W( m_wcsResourceValue, MAX_STR_LENGTH, wcsValue );

    return dwResult;
}

DWORD TFindResource::SetNameForResource( LPWSTR wcsValue )
{
    DWORD dwResult = CopyW2W( m_wcsResourceName, MAX_STR_LENGTH, wcsValue );

    return dwResult;
}


//
//
//

DWORD TFindServiceResource::IsMatchingResource(
        LPWSTR wcsName, 
        BOOL * p_bMatching)
{
    DWORD dwResult = DEFAULT_ERROR;

    LPCWSTR cwcsServiceName = GetServiceName();

    dwResult = IsClusterServiceResource(
        m_hCluster,
        wcsName,
        cwcsServiceName,
        p_bMatching);

    return dwResult;
}

//
// Helper class for finding the value for a resource in a group.
//
DWORD TFindResourcesInGroup::OnResource( LPWSTR wcsName, BOOL * p_bContinue )
{
    m_bFound = FALSE;
    DWORD dwResult = IsMatchingResource( wcsName, &m_bFound );
    if (dwResult == ERROR_SUCCESS)
    {
        *p_bContinue = !m_bFound;
        if (m_bFound)
        {
            SetNameForResource(wcsName);
        }
    }

    return dwResult;
}

//
// Class specifically for finding the network name
//

DWORD TFindResourceProperty::IsMatchingResource(LPWSTR wcsResourceName, BOOL * p_bIsMatch)
{
    DWORD dwResult = DEFAULT_ERROR;
    BOOL bIsMatching = FALSE;

    HRESOURCE hResource = NULL;
    hResource = OpenClusterResource( m_hCluster, wcsResourceName );
    if (hResource)
    {
        if (IsMatchingResourceType(hResource))
        {
            WCHAR wcsPropertyValue[MAX_STR_LENGTH];
            DWORD dwBufferSize = MAX_STR_LENGTH;

            LPCWSTR cwcsProperyName = GetPropertyName();

            dwResult = GetResourcePropertyString( 
                    hResource,  
                    cwcsProperyName,
                    wcsPropertyValue,
                    dwBufferSize);
            if (dwResult == ERROR_SUCCESS)
            {
                bIsMatching = TRUE;
                dwResult = SetValueForResource( wcsPropertyValue );

                if (dwResult == ERROR_SUCCESS)
                {
                    dwResult = SetNameForResource( wcsResourceName );
                }
            }
            else
            {
                DEBUG_MSG("Failed to get named property");
            }

        }
        else
        {
            dwResult = ERROR_SUCCESS; // It doesn't match but it is still a "success"
        }

        BOOL bSuccess = CloseClusterResource( hResource );
        if (!bSuccess)
        {
            DEBUG_MSG(" CloseClusterResource failed! "); 
        }
    }
    else
    {
        dwResult = GetLastError();
        DEBUG_MSG(" OpenClusterResource failed! "); 
        VALIDATE_LAST_ERROR( dwResult );
    }

    *p_bIsMatch = bIsMatching;
    return dwResult;
}

DWORD TFindPhysicalDisk::OnAction(HRESOURCE hResource)
{
    WCHAR wcsPropertyValue[MAX_STR_LENGTH];
    DWORD dwBufferSize = MAX_STR_LENGTH;

    DWORD dwResult = GetDiskPartitionList( 
            hResource,  
            wcsPropertyValue,
            &dwBufferSize);
    if (dwResult == ERROR_SUCCESS)
    {
        // Only save the first drive letter.
        dwResult = SetValueForResource( wcsPropertyValue );
    }
    else
    {
        DEBUG_MSG("Failed to get named property");
    }

    return dwResult;
}

DWORD TFindPhysicalDisk::IsMatchingResource(LPWSTR wcsResourceName, BOOL * p_bIsMatch)
{
    DWORD dwResult = DEFAULT_ERROR;
    *p_bIsMatch = FALSE;

    HRESOURCE hResource = NULL;
    hResource = OpenClusterResource( m_hCluster, wcsResourceName );
    if (hResource)
    {
        if (IsMatchingResourceType(hResource))
        {
            dwResult = OnAction(hResource);
            if (dwResult == ERROR_SUCCESS)
            {
                *p_bIsMatch = TRUE;  // found property of interest.
            }
        }
        else
        {
            dwResult = ERROR_SUCCESS; // It doesn't match but it is still a "success"
        }

        BOOL bSuccess = CloseClusterResource( hResource );
        if (!bSuccess)
        {
            DEBUG_MSG(" CloseClusterResource failed! "); 
        }
    }
    else
    {
        dwResult = GetLastError();
        DEBUG_MSG(" OpenClusterResource failed! "); 
        VALIDATE_LAST_ERROR( dwResult );
    }

    return dwResult;
}

TValidatePathOnDisk::TValidatePathOnDisk()
    : m_wcsPath( NULL )
{
    // nothing else to do
}

//
// Class specifically for finding the physical disk and validating a path on it.
//
// NAVE
DWORD TValidatePathOnDisk::IsValidPath( LPCWSTR wcsPath )
{
    DWORD dwResult = DEFAULT_ERROR;
    if ( (wcsPath == NULL) || (wcslen(wcsPath)== 0))
    {
        return ERROR_INVALID_PARAMETER;
    }

    m_wcsPath = wcsPath;  // path to validate.
    
    WCHAR wcsExchangeGroup[MAX_STR_LENGTH];
    dwResult = ClusterGetExchangeGroupW( wcsExchangeGroup, MAX_STR_LENGTH );
    if (dwResult == ERROR_SUCCESS)
    {
        BOOL bDummy = FALSE;
        Init();
        dwResult = OnGroup(wcsExchangeGroup, &bDummy);
        if (dwResult == ERROR_SUCCESS)
        {
            if ( !WasFound() )
            {
                dwResult = ERROR_NOT_FOUND;
                DEBUG_MSG("Could not find property in group");
            }
        }
        else
        {
            DEBUG_MSG("OnGroup failed");
        }
    }
    else
    {
        DEBUG_MSG("ClusterGetExchangeGroupW failed.");
    }

    return dwResult;

}

DWORD TValidatePathOnDisk::OnAction( HRESOURCE hResource )
{
    DWORD dwResult = ERROR_NOT_FOUND;

    // m_wcsPath = L"H:\\JUNK";

    DWORD dwBufferSize = (wcslen(m_wcsPath) + 1) * sizeof(WCHAR);
    
    dwResult = ClusterResourceControl( 
        hResource, 
        NULL, 
        CLUSCTL_RESOURCE_STORAGE_IS_PATH_VALID,
        (VOID *)m_wcsPath,
        dwBufferSize,
        NULL,
        0,
        NULL );

    return dwResult;
}


//
//
DWORD GetDiskPartitionList( 
        HRESOURCE hResource,  
        LPWSTR wcsBuffer,
        DWORD * p_dwMaxBuffer)
{
    DWORD dwResult = ERROR_NOT_FOUND;
    DWORD dwBufferSize = 0;
    
    dwResult = ClusterResourceControl( 
        hResource, 
        NULL, 
        CLUSCTL_RESOURCE_STORAGE_GET_DISK_INFO,
        NULL,
        0,
        NULL,
        0,
        &dwBufferSize );
    
    // I was expecting ERROR_MORE_DATA, but I got ERROR_SUCCESS, so let us handle both.
    if ( ( (dwResult == ERROR_SUCCESS) || (dwResult == ERROR_MORE_DATA) ) && (dwBufferSize > 0))
    {
        CLUSPROP_VALUE * pValueList = (CLUSPROP_VALUE *) malloc(dwBufferSize);
        
        ZeroMemory(wcsBuffer, *p_dwMaxBuffer * sizeof(WCHAR));

        dwResult = ClusterResourceControl( 
            hResource, 
            NULL, 
            CLUSCTL_RESOURCE_STORAGE_GET_DISK_INFO,
            NULL,
            0,
            (void *)pValueList,
            dwBufferSize,
            &dwBufferSize );
        
        if (dwResult == ERROR_SUCCESS)
        {
            // LPWSTR pValue = NULL;
            BOOL bFoundProperty = FindPropertyPartitionInfo(
                pValueList,
                dwBufferSize,
                wcsBuffer,
                p_dwMaxBuffer );
            
            if (!bFoundProperty) 
            {
                dwResult = ERROR_NOT_FOUND;
                DEBUG_MSG("EEP! Could not find property");
            }
        }
        else
        {
            DEBUG_MSG("EEP! ClusterResourceControl failed");
        }

        free( pValueList );
        pValueList = NULL;
    }
    else
    {
        DEBUG_MSG("EEP! ClusterResourceControl is behaving unexpectedly");
    }
        

    return dwResult;
}

BOOL FindPropertyPartitionInfo(
    CLUSPROP_VALUE * pValueList,
    DWORD dwBufferSize,
    LPWSTR wcsInfo,
    DWORD * p_dwInfoBufferSize)
{
    BOOL bSuccess = FALSE;
    BOOL bContinue = TRUE;
    DWORD  cbOffset    = 0;    // offset to next entry in the value list
    DWORD  cbPosition  = 0;    // tracks the advance through the value list buffer

    CLUSPROP_BUFFER_HELPER ListEntry;  // to parse the list

    // DWORD dwMaxBufferSize = *p_dwInfoBufferSize;

    wcsInfo[0] = 0;
    *p_dwInfoBufferSize = 1;

    LPWSTR wcsNextEntry = wcsInfo;

    //    
    // Set the pb member to the start of the list
    //
    ListEntry.pValue = pValueList;

    // int nMaxProperties = pPropertyList->nPropertyCount;
    int nNextProperty = 1;
    // int nVerifyPropertyCount = 0;

    //
    // Main loop:
    // 1. Check syntax of current list entry
    // 3. If it is a partition info, copy the disk info to our result string.
    // 4. Advance the position counter and test vs. size of list.    
    //
    do
    {        
        switch( *ListEntry.pdw ) // check the syntax of the entry        
        {
        case CLUSPROP_SYNTAX_ENDMARK:
            cbOffset = sizeof( DWORD );
            nNextProperty++;
            break;
        
        case CLUSPROP_SYNTAX_PARTITION_INFO:
            {
            TRACE_STRING_W( (ListEntry.pPartitionInfoValue)->szDeviceName );

            wcscat(wcsNextEntry, (ListEntry.pPartitionInfoValue)->szDeviceName );
            // move to next spot and add double null
            DWORD dwEntryLength = (wcslen(wcsNextEntry) + 1);
            *p_dwInfoBufferSize += dwEntryLength;
            wcsNextEntry += dwEntryLength;
            wcsNextEntry[0] = 0;

            bSuccess = TRUE;
            
            cbOffset = sizeof( *ListEntry.pValue ) + ALIGN_CLUSPROP( ListEntry.pValue->cbLength );
            }
            break;

        // These are known and expected.
        case CLUSPROP_SYNTAX_DISK_SIGNATURE :
        case CLUSPROP_SYNTAX_SCSI_ADDRESS :
        case CLUSPROP_SYNTAX_DISK_NUMBER :
            
            cbOffset = sizeof( *ListEntry.pValue ) + ALIGN_CLUSPROP( ListEntry.pValue->cbLength );
            break;

            
        // These are known, but not expected...
        case CLUSPROP_SYNTAX_NAME :
        case CLUSPROP_SYNTAX_RESCLASS :
            
        case CLUSPROP_SYNTAX_LIST_VALUE_SZ :
        case CLUSPROP_SYNTAX_LIST_VALUE_EXPAND_SZ :
        case CLUSPROP_SYNTAX_LIST_VALUE_MULTI_SZ :
            
        case CLUSPROP_SYNTAX_LIST_VALUE_DWORD:
        case CLUSPROP_SYNTAX_LIST_VALUE_BINARY:

        case CLUSPROP_SYNTAX_FTSET_INFO :
#ifdef _DEBUG
            DebugBreak();  
#endif
            cbOffset = sizeof( *ListEntry.pValue ) + ALIGN_CLUSPROP( ListEntry.pValue->cbLength );
            break;
        
        // unknown... possible alignment problem?
        default:
#ifdef _DEBUG
            DebugBreak();  
#endif
            cbOffset = sizeof( *ListEntry.pValue ) + ALIGN_CLUSPROP( ListEntry.pValue->cbLength );
            break;
        }


        //
        // Verify that the offset to the next entry is
        // within the value list buffer, then advance
        // the CLUSPROP_BUFFER_HELPER pointer.
        //
        cbPosition += cbOffset;
        if ( cbPosition == dwBufferSize )
        {
            // done, the size of the buffer is the only indicator.
            bContinue = FALSE;
            break;
        }

        if ( cbPosition > dwBufferSize )
        {
            DEBUG_MSG("Gone past the end of buffer");
            bContinue = FALSE;
            break;
        }

        ListEntry.pb += cbOffset;    
    } while (bContinue);

    return bSuccess;
}


