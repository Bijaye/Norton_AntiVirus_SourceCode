#include "clus.h"
// Ducati specific cluster support


CDECLEXPORTAPI fnTestFunction()
{

    DWORD dwResult = NAVE_SETUP_SUCCESS;
    
    dwResult = ClusterIsServer();
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        MESSAGE("This is a cluster server.");
    }
    else
    {
        MESSAGE("This is NOT a cluster server.");
        return NAVE_SETUP_FAILURE;
    }


#define TEST_BUFFER_SIZE _MAX_PATH

#if 1

    char szMsg[1024];
    WCHAR szMsgW[1024];


    char szClusterDBTest[TEST_BUFFER_SIZE];

    dwResult = ClusterSetNAVMSEValue( "test", "test value");
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        MESSAGE( "Test set value successful" );
    }
    else
    {
        MESSAGE("Could not determine cluster name");
        return dwResult;
    }


    dwResult = ClusterGetNAVMSEValue( "test", szClusterDBTest, TEST_BUFFER_SIZE );
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        wsprintf(szMsg,"Get Value : %s", szClusterDBTest);
        MESSAGE( szMsg );
    }
    else
    {
        MESSAGE("Could not determine cluster name");
        return dwResult;
    }

    char szClusterName[TEST_BUFFER_SIZE];
    DWORD dwMaxBuffer = TEST_BUFFER_SIZE;

    dwResult = ClusterGetName( szClusterName, dwMaxBuffer );
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        wsprintf(szMsg,"Cluster Name: %s", szClusterName);
        MESSAGE( szMsg );
    }
    else
    {
        MESSAGE("Could not determine cluster name");
        return dwResult;
    }

    char szNodeName[TEST_BUFFER_SIZE];

    dwResult = ClusterGetNodeName( szNodeName, dwMaxBuffer );
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        wsprintf(szMsg,"Node Name: %s", szNodeName);
        MESSAGE( szMsg );
    }
    else
    {
        MESSAGE("Could not determine node name");
        return dwResult;
    }

    char szGroupName[TEST_BUFFER_SIZE];

    dwResult = ClusterGetExchangeGroup( szGroupName, dwMaxBuffer );
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        wsprintf(szMsg,"Group Name: %s", szGroupName);
        MESSAGE( szMsg );
    }
    else
    {
        MESSAGE("Could not determine group name");
        return dwResult;
    }

    char szInstallPath[TEST_BUFFER_SIZE];

    dwResult = ClusterGetDefaultInstallPath( szInstallPath, dwMaxBuffer );
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        wsprintf(szMsg,"Default install path: %s", szInstallPath);
        MESSAGE( szMsg );
    }
    else
    {
        MESSAGE("Could not determine default install path");
        return dwResult;
    }


    dwResult = ClusterIsValidInstallPath( szInstallPath );
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        wsprintf(szMsg, "The path \"%s\" is a valid install path.", szInstallPath);
        MESSAGE(szMsg);
    }
    else
    {
        wsprintf(szMsg, "The path \"%s\" is NOT a valid install path.", szInstallPath);
        MESSAGE(szMsg);
        return NAVE_SETUP_FAILURE;
    }

    dwResult = ClusterIsExchangeActiveOnNode();
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        MESSAGE("Exchange is active on this node.");
    }
    else
    {
        MESSAGE("Exchange is NOT active on this node.");
    }

    dwResult = ClusterIsNAVMSEResourceInstalled();
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        MESSAGE("NAVMSE resource is installed.");

        WCHAR szGroupNameW[TEST_BUFFER_SIZE];

        dwResult = ClusterGetNAVMSEGroupW( szGroupNameW, dwMaxBuffer );
        if (dwResult == NAVE_SETUP_SUCCESS)
        {
            wsprintfW( szMsgW,L"NAVMSE Group Name: %s", szGroupNameW);
            MESSAGE_W( szMsgW );
        }
        else
        {
            MESSAGE("Could not determine NAVMSE group name");
        }

        WCHAR szResourceName[TEST_BUFFER_SIZE];

        dwResult = GetNAVMSEResourceName( szResourceName, dwMaxBuffer );
        if (dwResult == NAVE_SETUP_SUCCESS)
        {
            wsprintfW(szMsgW,L"Resource Name: %s", szResourceName);
            MESSAGE_W( szMsgW );
        }
        else
        {
            MESSAGE("Could not determine NAVMSE resource name");
        }
    
    }
    else
    {
        MESSAGE("NAVMSE resource is NOT installed.");
    }

    char szNetworkName[TEST_BUFFER_SIZE];

    dwResult = ClusterGetNetworkName( szNetworkName, dwMaxBuffer );
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        wsprintf(szMsg,"network name: %s", szNetworkName);
        MESSAGE( szMsg );
    }
    else
    {
        MESSAGE("Could not determine network name");
        return dwResult;
    }

    char szIPAddress[TEST_BUFFER_SIZE];

    dwResult = ClusterGetIPAddress( szIPAddress, dwMaxBuffer );
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        wsprintf(szMsg,"IP Address: %s", szIPAddress);
        MESSAGE( szMsg );
    }
    else
    {
        MESSAGE("Could not determine IP Address");
        return dwResult;
    }

    char szSubnetMask[TEST_BUFFER_SIZE];

    dwResult = ClusterGetIPSubnetMask( szSubnetMask, dwMaxBuffer );
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        wsprintf(szMsg,"Subnet mask: %s", szSubnetMask);
        MESSAGE( szMsg );
    }
    else
    {
        MESSAGE("Could not determine subnet mask");
        return dwResult;
    }
#else
    BOOL bIsNAVMSEInstalled;
#endif

    dwResult = ClusterIsNAVMSEResourceInstalled();
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        dwResult = ClusterRemoveNAVMSEResource();
        if (dwResult == NAVE_SETUP_SUCCESS)
        {
            MESSAGE("Removed NAVMSE Resource");
        }
        else
        {
            MESSAGE("Failed to remove NAVMSE Resource");
            return dwResult;
        }
    }

    dwResult = ClusterAddNAVMSEResource();
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        MESSAGE("Added NAVMSE Resource");
    }
    else
    {
        MESSAGE("Failed to add NAVMSE Resource");
        return dwResult;
    }

    dwResult = ClusterPutNAVMSEResourceOnline();
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        MESSAGE("NAVMSE resource has been put ONLINE");
    }
    else
    {
        MESSAGE("NAVMSE could NOT be put online.");
        return dwResult;
    }


    // Do it again.
    dwResult = ClusterIsNAVMSEResourceInstalled();
    if (dwResult == NAVE_SETUP_SUCCESS)
    {
        MESSAGE("NAVMSE resource is installed.");
    }
    else
    {
        MESSAGE("NAVMSE resource is NOT installed.");
    }

    return NAVE_SETUP_SUCCESS;
}


//
// Interface functions
//

CDECLEXPORTAPI ClusterIsExchangeActiveOnNode()
{
    DWORD dwSuccess;

    if (IsExchangeActiveOnNode())
    {
        dwSuccess = NAVE_SETUP_SUCCESS;
    }
    else
    {
        dwSuccess = NAVE_SETUP_FAILURE;
    }

    return dwSuccess;
}


CDECLEXPORTAPI ClusterIsNAVMSEResourceInstalled()
{
    DWORD dwIsInstalled = NAVE_SETUP_FAILURE;
    if (IsNAVMSEResourceInstalled())
    {
        dwIsInstalled = NAVE_SETUP_SUCCESS;
    }

    return dwIsInstalled;
}

CDECLEXPORTAPI ClusterIsNAVMSEResourceOnline()
{
    BOOL bOnline = FALSE;
    DWORD dwResult = IsNAVMSEOnline(&bOnline);
    if (dwResult == ERROR_SUCCESS)
    {
        if (!bOnline)
        {
            dwResult = DEFAULT_ERROR;
        }
    }

    return ConvertErrorCodeToNaveSetupError(dwResult);
}

CDECLEXPORTAPI ClusterGetExchangeGroup( LPSTR szGroupName, DWORD dwMaxBuffer )
{
    DWORD dwResult = DEFAULT_ERROR; // some error.

    WCHAR wcsPropertyValue[MAX_STR_LENGTH];

    dwResult = ClusterGetExchangeGroupW(wcsPropertyValue, MAX_STR_LENGTH);

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = CopyW2A( szGroupName, dwMaxBuffer, wcsPropertyValue );
    }

    return ConvertErrorCodeToNaveSetupError(dwResult);
}

CDECLEXPORTAPI ClusterAddNAVMSEResource()
{
    DWORD dwResult = AddNAVMSEResource();
    return ConvertErrorCodeToNaveSetupError(dwResult);
}

CDECLEXPORTAPI ClusterRemoveNAVMSEResource()
{
    DWORD dwResult = RemoveNAVMSEResource();

    return ConvertErrorCodeToNaveSetupError(dwResult);
}

CDECLEXPORTAPI ClusterPutNAVMSEResourceOnline()
{
    DWORD dwResult = PutNAVMSEResourceOnline();
    return ConvertErrorCodeToNaveSetupError(dwResult);
}

CDECLEXPORTAPI ClusterTakeNAVMSEResourceOffline()
{
    DWORD dwResult = TakeNAVMSEResourceOffline();
    return ConvertErrorCodeToNaveSetupError(dwResult);
}

CDECLEXPORTAPI ClusterSetNAVMSEValue( LPCSTR szName, LPCSTR szValue )
{
    DWORD dwResult = DEFAULT_ERROR;

    WCHAR wcsName[MAX_STR_LENGTH];
    WCHAR wcsValue[MAX_STR_LENGTH];

    dwResult = CopyA2W( wcsName, MAX_STR_LENGTH, szName );

    if ( dwResult == ERROR_SUCCESS )
    {
        dwResult = CopyA2W( wcsValue, MAX_STR_LENGTH, szValue );
    }

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = ClusterSetNAVMSEValueW(wcsName, wcsValue);
    }

    return ConvertErrorCodeToNaveSetupError(dwResult);
}

CDECLEXPORTAPI ClusterGetNAVMSEValue( LPCSTR szName, LPSTR szValue, DWORD dwMaxBuffer )
{
    DWORD dwResult = DEFAULT_ERROR;

    WCHAR wcsName[MAX_STR_LENGTH];
    WCHAR wcsValue[MAX_STR_LENGTH];

    dwResult = CopyA2W( wcsName, MAX_STR_LENGTH, szName );

    if ( dwResult == ERROR_SUCCESS )
    {
        dwResult = ClusterGetNAVMSEValueW(wcsName, wcsValue, MAX_STR_LENGTH);
    }

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = CopyW2A( szValue, dwMaxBuffer, wcsValue );
    }

    return ConvertErrorCodeToNaveSetupError(dwResult);
}


//
// Internal Interface functions
//

BOOL IsNAVMSEResourceInstalled()
{
    BOOL bIsInstalled = FALSE;
    
    DWORD dwResult = DEFAULT_ERROR;

    TFindNAVMSEServiceResource findgroup;
    
    dwResult = findgroup.Enum();
    if (dwResult == ERROR_SUCCESS)
    {
        if (findgroup.WasFound())
        {
            bIsInstalled = TRUE;
        }
        else
        {
            bIsInstalled = FALSE;
        }
    }
    else
    {
        DEBUG_MSG("Error finding service!");
    }

    return bIsInstalled;
}

DWORD GetExchangeGroupState(
        CLUSTER_GROUP_STATE * pState, 
        LPWSTR wcsActiveNode, 
        DWORD dwMaxBuffer )
{
    DWORD dwResult = DEFAULT_ERROR;
    WCHAR wcsExchangeGroup[MAX_STR_LENGTH];

    dwResult = ClusterGetExchangeGroupW(wcsExchangeGroup, MAX_STR_LENGTH);
    if (dwResult == ERROR_SUCCESS)
    {
        HCLUSTER hCluster = OpenLocalCluster();
        if (hCluster)
        {
            HGROUP hGroup = OpenClusterGroup(hCluster, wcsExchangeGroup);
            if (hGroup)
            {
                *pState = GetClusterGroupState(  
                    hGroup,        
                    wcsActiveNode,
                    &dwMaxBuffer  );

                if (*pState == ClusterGroupStateUnknown)
                {
                    dwResult = GetLastError();
                    VALIDATE_LAST_ERROR( dwResult );
                }

                if (!CloseClusterGroup( hGroup ))
                {
                    DEBUG_MSG("CloseClusterGroup failed");
                }
            }
            else
            {
                dwResult = GetLastError();
                VALIDATE_LAST_ERROR( dwResult );
            }


            if ( !CloseCluster(hCluster) )
            {
                DEBUG_MSG("CloseCuster failed");
            }
        }
        else
        {
            dwResult = GetLastError();
            VALIDATE_LAST_ERROR( dwResult );
        }
    }

    return dwResult;
}

BOOL IsExchangeActiveOnNode()
{
    BOOL bIsActive = FALSE;
    DWORD dwResult = DEFAULT_ERROR;

    WCHAR wcsActiveNode[MAX_STR_LENGTH];
    wcsActiveNode[0] = 0;

    CLUSTER_GROUP_STATE state = ClusterGroupStateUnknown;

    dwResult = GetExchangeGroupState(
        &state, 
        wcsActiveNode, 
        MAX_STR_LENGTH );
    
    TRACE_STRING_W(wcsActiveNode);

    if (dwResult == ERROR_SUCCESS)
    {
        if (wcslen(wcsActiveNode) > 0)
        {
            WCHAR wcsLocalNode[MAX_STR_LENGTH];

            dwResult = ClusterGetNodeNameW( wcsLocalNode, MAX_STR_LENGTH );
            if (dwResult == ERROR_SUCCESS)
            {
                if ( wcscmp( wcsLocalNode, wcsActiveNode ) == 0 )
                {
                    bIsActive = TRUE;
                }
            }
        }
    }

    return bIsActive;
}





DWORD ClusterGetExchangeGroupW( LPWSTR wcsGroupName, DWORD dwMaxBuffer )
{
    static BOOL  g_bCacheSet = FALSE;
    static WCHAR g_wcsCachedGroup[MAX_STR_LENGTH] = L"";

    DWORD dwResult = DEFAULT_ERROR;

    if (g_bCacheSet)
    {
        dwResult = CopyW2W( wcsGroupName, dwMaxBuffer, g_wcsCachedGroup );
        return dwResult;
    }

    ZeroMemory( wcsGroupName, dwMaxBuffer );

    TFindExchangeInformationStoreServiceResource findgroup;
    
    dwResult = findgroup.Enum();
    if (dwResult == ERROR_SUCCESS)
    {
        if (findgroup.WasFound())
        {
            LPCWSTR wcsGroupNameFound = findgroup.GetValue();
            dwResult = CopyW2W( wcsGroupName, dwMaxBuffer, wcsGroupNameFound );

            // cache for future use.
            DWORD dwResultCached = CopyW2W( g_wcsCachedGroup, MAX_STR_LENGTH, wcsGroupNameFound );
            g_bCacheSet = (dwResultCached == ERROR_SUCCESS);
        }
        else
        {
            DEBUG_MSG("Exchange Service Not Found!");
            dwResult = ERROR_NOT_FOUND;
        }
    }
    else
    {
        DEBUG_MSG("Error finding service!");
    }

    return dwResult;
}


DWORD ClusterGetNAVMSEGroupW( LPWSTR wcsGroupName, DWORD dwMaxBuffer )
{
    static BOOL  g_bCacheSet = FALSE;
    static WCHAR g_wcsCachedGroup[MAX_STR_LENGTH] = L"";

    DWORD dwResult = DEFAULT_ERROR;

    if (g_bCacheSet)
    {
        dwResult = CopyW2W( wcsGroupName, dwMaxBuffer, g_wcsCachedGroup );
        return dwResult;
    }

    ZeroMemory( wcsGroupName, dwMaxBuffer );

    TFindNAVMSEServiceResource findgroup;
    
    dwResult = findgroup.Enum();
    if (dwResult == ERROR_SUCCESS)
    {
        if (findgroup.WasFound())
        {
            LPCWSTR wcsGroupNameFound = findgroup.GetValue();
            dwResult = CopyW2W( wcsGroupName, dwMaxBuffer, wcsGroupNameFound );

            // cache for future use.
            DWORD dwResultCached = CopyW2W( g_wcsCachedGroup, MAX_STR_LENGTH, wcsGroupNameFound );
            g_bCacheSet = (dwResultCached == ERROR_SUCCESS);
        }
        else
        {
            DEBUG_MSG("Service Not Found!");
            dwResult = ERROR_NOT_FOUND;
        }
    }
    else
    {
        DEBUG_MSG("Error finding service!");
    }

    return dwResult;
}


//
//  Implementation functions.
//

DWORD AddNAVMSEResource()
{
    // Perform some prelimary checks...
    DWORD dwResult = DEFAULT_ERROR;

    // Is this the primary node?
    BOOL bOwned = IsExchangeActiveOnNode();
    if (bOwned)
    {
        BOOL bInstalled = IsNAVMSEResourceInstalled();
        if (bInstalled)
        {
            // DEBUG_MSG("Removing NAVMSE Resource");
            dwResult = RemoveNAVMSEResource();
            if (dwResult != ERROR_SUCCESS)
            {
                DEBUG_MSG("RemoveNAVMSEResource failed.");
            }
        }
        else
        {
            dwResult = ERROR_SUCCESS;  // every thing checks out ok.
        }

        if (dwResult == ERROR_SUCCESS)
        {
            dwResult = CreateNAVMSEResource();
            if (dwResult != ERROR_SUCCESS)
            {
                DEBUG_MSG("CreateNAVMSEResource failed");
            }
        }

    }
    else
    {
        BOOL bInstalled = IsNAVMSEResourceInstalled();
        if (bInstalled)
        {
            // DEBUG_MSG("Adding this node to NAVMSE Resource");
            // Only add this node.
            dwResult = AddThisNodeToNAVMSEResource();
            if (dwResult != ERROR_SUCCESS)
            {
                DEBUG_MSG("AddThisNodeToNAVMSEResource failed");
            }
        }
        else
        {
            dwResult = ERROR_INVALID_STATE;  // can't add it.
        }
    }

    return dwResult;
}

DWORD CreateNAVMSEResource()
{
    DWORD dwResult = DEFAULT_ERROR;
        
    // Create cluster resource
    
    WCHAR wcsExchangeGroup[MAX_STR_LENGTH];
    
    dwResult = ClusterGetExchangeGroupW(wcsExchangeGroup, MAX_STR_LENGTH);
    if (dwResult == ERROR_SUCCESS)
    {
        HCLUSTER hCluster = OpenLocalCluster();
        if (hCluster)
        {
            HGROUP hGroup = OpenClusterGroup(hCluster, wcsExchangeGroup);
            if (hGroup)
            {
                HRESOURCE hResource = CreateClusterResource(  
                    hGroup,             
                    DEFAULT_NAVMSE_RESOURCE_NAME,
                    L"Generic Service",  
                    0 );
                
                if (hResource)
                {
                    dwResult = AddNAVMSEDependenciesToResource( hCluster, hResource );
                    if (dwResult != ERROR_SUCCESS)
                    {
                        DEBUG_MSG("AddNAVMSEDependenciesToResource failed");
                    }

                    if (dwResult == ERROR_SUCCESS)
                    {
                        dwResult = AddNAVMSEPrivatePropertiesToResource( hResource );
                        if (dwResult != ERROR_SUCCESS)
                        {
                            DEBUG_MSG("AddNAVMSEPrivatePropertiesToResource failed");
                        }
                    }

                    if (dwResult == ERROR_SUCCESS)
                    {
                        dwResult = AddNAVMSERegKeyCheckPointsToResource( hResource );
                        if (dwResult != ERROR_SUCCESS)
                        {
                            DEBUG_MSG("AddNAVMSERegKeyCheckPointsToResource failed");
                        }
                    }

                    if (dwResult == ERROR_SUCCESS)
                    {
                        dwResult = AddThisNodeToNAVMSEResource();
                        if (dwResult != ERROR_SUCCESS)
                        {
                            DEBUG_MSG("AddThisNodeToNAVMSEResource failed");
                        }
                    }

                    if (dwResult != ERROR_SUCCESS)
                    {
                        if (!DeleteClusterResource( hResource ) == ERROR_SUCCESS)
                        {
                            DEBUG_MSG("could not clean up resource on create failure");
                        }
                    }

                    if (!CloseClusterResource( hResource ))
                    {
                        DEBUG_MSG("CloseClusterResource failed");
                    }
                    
                }
                else
                {
                    dwResult = GetLastError();
                    DEBUG_MSG("CreateClusterResource failed.");
                    VALIDATE_LAST_ERROR( dwResult );
                }
                
                if (!CloseClusterGroup( hGroup ))
                {
                    DEBUG_MSG("CloseClusterGroup failed");
                }
            }
            else
            {
                dwResult = GetLastError();
                VALIDATE_LAST_ERROR( dwResult );
            }
            
            
            if ( !CloseCluster(hCluster) )
            {
                DEBUG_MSG("CloseCuster failed");
            }
        }
        else
        {
            dwResult = GetLastError();
            DEBUG_MSG("OpenLocalCluster failed");
            VALIDATE_LAST_ERROR( dwResult );
        }
    }
    else
    {
        DEBUG_MSG("ClusterGetExchangeGroupW failed");
    }


    return dwResult;
}

DWORD AddThisNodeToNAVMSEResource()
{
    DWORD dwResult = DEFAULT_ERROR;

    HCLUSTER hCluster = OpenLocalCluster();
    if (hCluster)
    {
        HRESOURCE hResource = OpenNAVMSEResource();
        if (hResource)
        {
            WCHAR wcsNode[MAX_STR_LENGTH];
            dwResult = ClusterGetNodeNameW( wcsNode, MAX_STR_LENGTH );
            if (dwResult == ERROR_SUCCESS)
            {
                HNODE hNode = OpenClusterNode( hCluster, wcsNode );
                if (hNode)
                {
                    dwResult = AddClusterResourceNode( hResource, hNode );
                    if (dwResult != ERROR_SUCCESS)
                    {
                        DEBUG_MSG("AddClusterResourceNode failed");
                    }

                    if (!CloseClusterNode(hNode))
                    {
                        DEBUG_MSG("CloseClusterNode failed");
                    }
                }
                else
                {
                    dwResult = GetLastError();
                    DEBUG_MSG("OpenClusterNode failed");
                    VALIDATE_LAST_ERROR( dwResult );
                }
            }
            else
            {
                DEBUG_MSG("ClusterGetNodeNameW failed");
            }

            if (!CloseClusterResource(hResource))
            {
                DEBUG_MSG("CloseClusterResource failed");
            }
        }
        else
        {
            dwResult = GetLastError();
            DEBUG_MSG("OpenNAVMSEResource() failed");
            VALIDATE_LAST_ERROR( dwResult );
        }
    }
    else
    {
        dwResult = GetLastError();
        DEBUG_MSG("OpenLocalCluster() failed");
        VALIDATE_LAST_ERROR( dwResult );
    }

    return dwResult;
}


DWORD RemoveThisNodeFromNAVMSEResource()
{
    DWORD dwResult = DEFAULT_ERROR;

    HCLUSTER hCluster = OpenLocalCluster();
    if (hCluster)
    {
        HRESOURCE hResource = OpenNAVMSEResource();
        if (hResource)
        {
            WCHAR wcsNode[MAX_STR_LENGTH];
            dwResult = ClusterGetNodeNameW( wcsNode, MAX_STR_LENGTH );
            if (dwResult == ERROR_SUCCESS)
            {
                HNODE hNode = OpenClusterNode( hCluster, wcsNode );
                if (hNode)
                {
                    dwResult = RemoveClusterResourceNode( hResource, hNode );
                    if (dwResult != ERROR_SUCCESS)
                    {
                        DEBUG_MSG("AddClusterResourceNode failed");
                    }

                    if (!CloseClusterNode(hNode))
                    {
                        DEBUG_MSG("CloseClusterNode failed");
                    }
                }
                else
                {
                    dwResult = GetLastError();
                    DEBUG_MSG("OpenClusterNode failed");
                    VALIDATE_LAST_ERROR( dwResult );
                }
            }
            else
            {
                DEBUG_MSG("ClusterGetNodeNameW failed");
            }

            if (!CloseClusterResource(hResource))
            {
                DEBUG_MSG("CloseClusterResource failed");
            }
        }
        else
        {
            dwResult = GetLastError();
            DEBUG_MSG("OpenNAVMSEResource() failed");
            VALIDATE_LAST_ERROR( dwResult );
        }
    }
    else
    {
        dwResult = GetLastError();
        DEBUG_MSG("OpenLocalCluster() failed");
        VALIDATE_LAST_ERROR( dwResult );
    }

    return dwResult;
}



DWORD AddNAVMSEDependenciesToResource( HCLUSTER hCluster, HRESOURCE hResource )
{
    DWORD dwResult = DEFAULT_ERROR;

    WCHAR wcsDependencyIS[MAX_STR_LENGTH];
    WCHAR wcsDependencyDS[MAX_STR_LENGTH];
    WCHAR wcsDependencySA[MAX_STR_LENGTH];
    WCHAR wcsDependencyNetworkName[MAX_STR_LENGTH];
    WCHAR wcsDependencyNetworkIP[MAX_STR_LENGTH];
    WCHAR wcsDependencyDisk[MAX_STR_LENGTH];

    {
        TFindExchangeInformationStoreServiceResource FindIS;
        dwResult = GetResourceNameForResourceInExchangeGroup( 
            FindIS, 
            wcsDependencyIS, 
            MAX_STR_LENGTH);
        TRACE_STRING_W(wcsDependencyIS);
    }

    if (dwResult == ERROR_SUCCESS)
    {
        TFindExchangeDirectoryServiceResource        FindDS;
        dwResult = GetResourceNameForResourceInExchangeGroup( 
            FindDS, 
            wcsDependencyDS, 
            MAX_STR_LENGTH);
        TRACE_STRING_W(wcsDependencyDS);
    }

    if (dwResult == ERROR_SUCCESS)
    {
        TFindExchangeSystemAttendantServiceResource  FindSA;
        dwResult = GetResourceNameForResourceInExchangeGroup( 
            FindSA, 
            wcsDependencySA, 
            MAX_STR_LENGTH);
        TRACE_STRING_W(wcsDependencySA);
    }

    if (dwResult == ERROR_SUCCESS)
    {
        TFindNetworkName FindNetworkName;
        dwResult = GetResourceNameForResourceInExchangeGroup( 
            FindNetworkName, 
            wcsDependencyNetworkName, 
            MAX_STR_LENGTH);
        TRACE_STRING_W(wcsDependencyNetworkName);
    }

    if (dwResult == ERROR_SUCCESS)
    {
        TFindIPAddress FindIP;
        dwResult = GetResourceNameForResourceInExchangeGroup( 
            FindIP, 
            wcsDependencyNetworkIP, 
            MAX_STR_LENGTH);
        TRACE_STRING_W(wcsDependencyNetworkIP);
    }


    if (dwResult == ERROR_SUCCESS)
    {
        TFindPhysicalDisk FindDisk;
        dwResult = GetResourceNameForResourceInExchangeGroup( 
            FindDisk, 
            wcsDependencyDisk, 
            MAX_STR_LENGTH);
        TRACE_STRING_W(wcsDependencyDisk);
    }

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = AddDependencyToResource(hCluster, hResource, wcsDependencyIS);
        if (dwResult != ERROR_SUCCESS)
        {
            DEBUG_MSG("AddDependencyToResource IS failed");
        }
    }

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = AddDependencyToResource(hCluster, hResource, wcsDependencyDS);
        if (dwResult != ERROR_SUCCESS)
        {
            DEBUG_MSG("AddDependencyToResource DS failed");
        }
    }

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = AddDependencyToResource(hCluster, hResource, wcsDependencySA);
        if (dwResult != ERROR_SUCCESS)
        {
            DEBUG_MSG("AddDependencyToResource SA failed");
        }
    }

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = AddDependencyToResource(hCluster, hResource, wcsDependencyNetworkName );
        if (dwResult != ERROR_SUCCESS)
        {
            DEBUG_MSG("AddDependencyToResource Network Name failed");
        }
    }

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = AddDependencyToResource(hCluster, hResource, wcsDependencyNetworkIP );
        if (dwResult != ERROR_SUCCESS)
        {
            DEBUG_MSG("AddDependencyToResource Network IP failed");
        }
    }

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = AddDependencyToResource(hCluster, hResource, wcsDependencyDisk );
        if (dwResult != ERROR_SUCCESS)
        {
            DEBUG_MSG("AddDependencyToResource Physical Disk failed");
        }
    }

    return dwResult;
}

DWORD GetResourceNameForResourceInExchangeGroup(
            TFindResource & Find, 
            LPWSTR wcsResourceName, 
            DWORD wcsMaxNameLength)
{
    DWORD dwResult = DEFAULT_ERROR;

    WCHAR wcsExchangeGroup[MAX_STR_LENGTH];
    dwResult = ClusterGetExchangeGroupW( wcsExchangeGroup, MAX_STR_LENGTH );
    if (dwResult == ERROR_SUCCESS)
    {
        BOOL bDummy = FALSE;
        Find.Init();
        dwResult = Find.OnGroup(wcsExchangeGroup, &bDummy);
        if (dwResult == ERROR_SUCCESS)
        {
            if ( Find.WasFound() )
            {
                dwResult = CopyW2W( wcsResourceName, wcsMaxNameLength, Find.GetName() );
            }
            else
            {
                dwResult = ERROR_NOT_FOUND;
                DEBUG_MSG("Could not find property in group");
            }
        }
        else
        {
            DEBUG_MSG("Find.OnGroup failed");
        }
    }
    else
    {
        DEBUG_MSG("ClusterGetExchangeGroupW failed.");
    }

    return dwResult;
}
DWORD AddNAVMSEPrivatePropertiesToResource_old( HRESOURCE hResource )
{
    DWORD dwResult = DEFAULT_ERROR;

    // create property values

        DWORD dwPropCount = 0;

        // service name

        dwPropCount++;

        WCHAR wcsPropServiceName[] = L"ServiceName";

        CLUSPROP_PROPERTY_NAME_DECLARE( 
            PropServiceName, 
            sizeof( wcsPropServiceName ) / sizeof( WCHAR ) );
        PropServiceName.Syntax.dw = CLUSPROP_SYNTAX_LIST_VALUE_SZ;
        PropServiceName.cbLength = sizeof( wcsPropServiceName );
        lstrcpyW( PropServiceName.sz, wcsPropServiceName );

        WCHAR wcsPropServiceNameData[] = NAVMSE_SERVICE_NAME;
        CLUSPROP_SZ_DECLARE( 
            PropServiceNameValue, 
            sizeof( wcsPropServiceNameData ) / sizeof( WCHAR ) );
        PropServiceNameValue.Syntax.dw = CLUSPROP_SYNTAX_LIST_VALUE_SZ;
        PropServiceNameValue.cbLength = sizeof( wcsPropServiceNameData );
        lstrcpyW( PropServiceNameValue.sz, wcsPropServiceNameData );

        // ENDMARK

        // use network name

        dwPropCount++;

        WCHAR wcsPropUseNetworkName[] = L"UseNetworkName";
        CLUSPROP_PROPERTY_NAME_DECLARE( 
            PropUseNetworkName, 
            sizeof( wcsPropUseNetworkName ) / sizeof( WCHAR ) );
        PropUseNetworkName.Syntax.dw = CLUSPROP_SYNTAX_LIST_VALUE_SZ;
        PropUseNetworkName.cbLength = sizeof( wcsPropUseNetworkName );
        lstrcpyW( PropUseNetworkName.sz, wcsPropUseNetworkName );

        CLUSPROP_DWORD PropUseNetworkNameValue;
        PropUseNetworkNameValue.Syntax.dw = CLUSPROP_SYNTAX_LIST_VALUE_DWORD;
        PropUseNetworkNameValue.cbLength = sizeof(DWORD);
        PropUseNetworkNameValue.dw = TRUE;

        // ENDMARK

        // declare an endmark to copy.
        CLUSPROP_SYNTAX EndMark;
        EndMark.dw = CLUSPROP_SYNTAX_ENDMARK;


    // create property list

        // calculate size

        DWORD dwSizeOfProptyList = 
           sizeof(dwPropCount)     // Property Count

           + sizeof(PropServiceName)        // ServiceName property 
           + ALIGN_CLUSPROP( sizeof(PropServiceName) )
           
           + sizeof(PropServiceNameValue)   // ServiceName value
           + ALIGN_CLUSPROP( sizeof(PropServiceNameValue) )
           
           + sizeof(EndMark)                // EndMark
           // + ALIGN_CLUSPROP( sizeof(EndMark) )

           + sizeof(PropUseNetworkName)     // UseNetworkName property 
           + ALIGN_CLUSPROP( sizeof(PropUseNetworkName) )
           
           + sizeof(PropUseNetworkNameValue)// UseNetworkName value
           + ALIGN_CLUSPROP( sizeof(PropUseNetworkNameValue) )
           
           + sizeof(EndMark)                // EndMark
           // + ALIGN_CLUSPROP( sizeof(EndMark) )
            ;

        //
        CLUSPROP_LIST * pPropertyList = (CLUSPROP_LIST *)malloc(dwSizeOfProptyList);
        if (pPropertyList!=NULL)
        {
            dwResult = ERROR_SUCCESS;
            ZeroMemory( pPropertyList, dwSizeOfProptyList );

            BYTE * pb = (BYTE *) pPropertyList;
            DWORD nBlockSize = 0;
            
#define     APPEND_ITEM( Src )    \
                {\
                    int iSizeOf = sizeof(Src); \
                    int iPadding = ALIGN_CLUSPROP( sizeof(Src) ); \
                    memcpy( (pb), &(Src),  iSizeOf); \
                    nBlockSize += iSizeOf + iPadding; \
                    pb += iSizeOf + iPadding; \
                }

            // APPEND_ITEM( dwPropCount ); // no padding for count
            int iSizeOf = sizeof(dwPropCount);
            memcpy( (pb), &(dwPropCount),  iSizeOf);
            nBlockSize += iSizeOf;
            pb += iSizeOf;

            APPEND_ITEM( PropServiceName);
            APPEND_ITEM( PropServiceNameValue);
            APPEND_ITEM( EndMark);

            APPEND_ITEM( PropUseNetworkName);
            APPEND_ITEM( PropUseNetworkNameValue);
            APPEND_ITEM( EndMark);

            if (nBlockSize != dwSizeOfProptyList)
            {
                dwResult = DEFAULT_ERROR;
                DEBUG_MSG( "Aggg! alignment error" );
            }
        }
        else
        {
            dwResult = ERROR_NOT_ENOUGH_MEMORY;
            DEBUG_MSG("malloc failed");
        }

    // validate and save

        if (dwResult == ERROR_SUCCESS)
        {
            // validate property list
            dwResult = ValidatePrivateProperties( 
                hResource, 
                pPropertyList, 
                dwSizeOfProptyList );
            if (dwResult != ERROR_SUCCESS)
            {
                DEBUG_MSG("ValidatePrivateProperties failed");
            }
        }

        if (dwResult == ERROR_SUCCESS)
        {
            // set private properties
            dwResult = SetPrivateProperties( 
                hResource, 
                pPropertyList, 
                dwSizeOfProptyList );
            if (dwResult != ERROR_SUCCESS)
            {
                DEBUG_MSG("SetPrivateProperties failed");
            }
        }

    return dwResult;
}

#if 1
DWORD AddNAVMSEPrivatePropertiesToResource( HRESOURCE hResource )
{
    DWORD dwResult = ERROR_SUCCESS;

    CLUSPROP_LIST *  pPropList  = NULL; // Pointer to the final property list
    DWORD  cbListSize = 0;    // List size counter; increments as size grows
    DWORD  dwMaxSize  = 2048;  // Arbitrary initial size of property list buffer.
    
    CLUSPROP_BUFFER_HELPER ListEntry; // CLUSPROP_BUFFER_HELPER structure used to 
                                      // navigate the buffer and create the property list
    //    
    // Property values (hard coded for simplicity)   
    //

    DWORD dwPropCount                = 2;

    WCHAR wcsPropServiceName[] = L"ServiceName";
    WCHAR wcsPropServiceNameValue[] = NAVMSE_SERVICE_NAME;

    WCHAR wcsPropUseNetworkName[] = L"UseNetworkName";
    DWORD dwPropUseNetworkNameValue = TRUE;

    // Allocate the property list buffer and set pPropList to the 
    // address of the buffer    //
    ListEntry.pb = (PBYTE) LocalAlloc( LPTR, dwMaxSize );
    
    if ( ListEntry.pb == NULL )
    {
        dwResult = GetLastError();
        VALIDATE_LAST_ERROR( dwResult );
        goto SizeError;
    }

    pPropList = ListEntry.pList;
    
    //
    // CREATE THE PROPERTY LIST
    // Walk the buffer using ListEnry's structure pointers to
    // "format" an area of memory for each entry in the list.    //        //

        // ENTRY START - number of properties        //

            // Property Count - uses a CLUSPROP_LIST structure.
            ListEntry.pList->nPropertyCount = dwPropCount;    
            
            // Increment the size counter and test vs. max size.
            cbListSize += sizeof( ListEntry.pList->nPropertyCount );
            if ( cbListSize > dwMaxSize ) goto SizeError;

            // "Walk the buffer" to the location of the next list entry
            ListEntry.pb += sizeof( ListEntry.pList->nPropertyCount );

        // END ENTRY - number of properties        //



        // ENTRY START - ServiceName Property        //

            // Property Name
            ListEntry.pName->Syntax.dw = CLUSPROP_SYNTAX_NAME;
            ListEntry.pName->cbLength  = sizeof( wcsPropServiceName );
            lstrcpyW( ListEntry.pName->sz, wcsPropServiceName );

            cbListSize   += sizeof( *ListEntry.pName ) + ALIGN_CLUSPROP( sizeof( wcsPropServiceName ) );
            if ( cbListSize > dwMaxSize ) goto SizeError;
            ListEntry.pb += sizeof( *ListEntry.pName ) + ALIGN_CLUSPROP( sizeof( wcsPropServiceName ) );
            //           
                // Property Value List.            //
                // Value List Entry - Description value
                ListEntry.pStringValue->Syntax.dw = CLUSPROP_SYNTAX_LIST_VALUE_SZ;
                ListEntry.pStringValue->cbLength  = sizeof( wcsPropServiceNameValue );
                lstrcpyW( ListEntry.pStringValue->sz, wcsPropServiceNameValue );

                cbListSize   += sizeof( *ListEntry.pStringValue ) + ALIGN_CLUSPROP( sizeof( wcsPropServiceNameValue ) );
                if ( cbListSize > dwMaxSize ) goto SizeError;
                ListEntry.pb += sizeof( *ListEntry.pStringValue ) + ALIGN_CLUSPROP( sizeof( wcsPropServiceNameValue ) );

                // Value List Endmark
                ListEntry.pSyntax->dw = CLUSPROP_SYNTAX_ENDMARK;
                
                cbListSize   += sizeof( *ListEntry.pSyntax );
                if ( cbListSize > dwMaxSize ) goto SizeError;
                ListEntry.pb += sizeof( *ListEntry.pSyntax );
            // end Property Value List

        // END ENTRY - ServiceName Property        
        //

               
        // ENTRY START - UseNetworkName Property
        //
            
            // Property Name
            ListEntry.pName->Syntax.dw = CLUSPROP_SYNTAX_NAME;
            ListEntry.pName->cbLength  = sizeof( wcsPropUseNetworkName );
            lstrcpyW( ListEntry.pName->sz, wcsPropUseNetworkName );
            cbListSize   += sizeof( *ListEntry.pName ) + ALIGN_CLUSPROP( sizeof( wcsPropUseNetworkName ) );
            if ( cbListSize > dwMaxSize ) goto SizeError;
            ListEntry.pb += sizeof( *ListEntry.pName ) + ALIGN_CLUSPROP( sizeof( wcsPropUseNetworkName ) );
            //            
            // Property Value List.
            //
                // Value List Entry - PersistentState value
                ListEntry.pDwordValue->Syntax.dw = CLUSPROP_SYNTAX_LIST_VALUE_DWORD;
                ListEntry.pDwordValue->cbLength  = sizeof( DWORD );
                ListEntry.pDwordValue->dw        = dwPropUseNetworkNameValue;

                cbListSize   += sizeof( *ListEntry.pDwordValue );
                if ( cbListSize > dwMaxSize ) goto SizeError;
                ListEntry.pb += sizeof( *ListEntry.pDwordValue );

                // Value List Endmark
                ListEntry.pSyntax->dw = CLUSPROP_SYNTAX_ENDMARK;

                cbListSize   += sizeof( *ListEntry.pSyntax );
                if ( cbListSize > dwMaxSize ) goto SizeError;
                ListEntry.pb += sizeof( *ListEntry.pSyntax );
            // end Property Value List
        // END ENTRY - UseNetworkName Property

    
    // END CREATE PROPERTY LIST

    //    
    // USE THE PROPERTY LIST
    //
    // pPropList points to the start of the property list constructed by ListEntry.
    // You can now validate pPropList and use it to set the properties,
    // return pPropList, or copy it to an output buffer.


    // validate and save

        if (dwResult == ERROR_SUCCESS)
        {
            // validate property list
            dwResult = ValidatePrivateProperties( 
                hResource, 
                pPropList, 
                cbListSize );
            if (dwResult != ERROR_SUCCESS)
            {
                DEBUG_MSG("ValidatePrivateProperties failed");
            }
        }

        if (dwResult == ERROR_SUCCESS)
        {
            // set private properties
            dwResult = SetPrivateProperties( 
                hResource, 
                pPropList, 
                cbListSize );
            if (dwResult != ERROR_SUCCESS)
            {
                DEBUG_MSG("SetPrivateProperties failed");
            }
        }

    //
SizeError:  
    if (pPropList)
    {
        if (LocalFree( pPropList ) != NULL)
        {
            DEBUG_MSG("LocalFree failed.");
        }
    }

    return dwResult;
} // end GrpCreatePropList_Buff()
#endif


DWORD AddNAVMSERegKeyCheckPointsToResource( HRESOURCE hResource )
{
    DWORD dwResult = DEFAULT_ERROR;

    dwResult = AddRegKeyCheckPointsToResource( 
        hResource, NAVMSE_SERVICE_REGISTRY_KEY );
    if (dwResult != ERROR_SUCCESS)
    {
        DEBUG_MSG("adding NAVMSE_SERVICE_REGISTRY_KEY failed.");
    }

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = AddRegKeyCheckPointsToResource( 
            hResource, NAVMSE_APP_REGISTRY_KEY );
        if (dwResult != ERROR_SUCCESS)
        {
            DEBUG_MSG("adding NAVMSE_APP_REGISTRY_KEY failed.");
        }
    }

    if (dwResult == ERROR_SUCCESS)
    {
        dwResult = AddRegKeyCheckPointsToResource( 
            hResource, NAVMSE_EVENTLOG_REGISTRY_KEY );
        if (dwResult != ERROR_SUCCESS)
        {
            DEBUG_MSG("adding NAVMSE_EVENTLOG_REGISTRY_KEY failed.");
        }
    }

    return dwResult;
}


DWORD IsNAVMSEOnline( BOOL * p_bOnline )
{
    DWORD dwResult = DEFAULT_ERROR;
    WCHAR wcsOwningNode[MAX_STR_LENGTH];
    
    CLUSTER_RESOURCE_STATE state = ClusterResourceStateUnknown;
    dwResult = GetNAVMSEResourceState( 
        &state,
        wcsOwningNode,
        MAX_STR_LENGTH );

    if (dwResult == ERROR_SUCCESS)
    {
        switch (state)
        {
        
        case ClusterResourceOnline:
        case ClusterResourceOnlinePending:
            *p_bOnline = TRUE;
            break;

        case ClusterResourceOffline:
        case ClusterResourceOfflinePending:
        case ClusterResourceFailed:
            *p_bOnline = FALSE;
            break;
    
        case ClusterResourceInitializing:
        case ClusterResourceInherited:
        case ClusterResourceStateUnknown:
        case ClusterResourcePending:
        default:
            DEBUG_MSG("Unknown resource state");
            dwResult = DEFAULT_ERROR;
            break;
        }
    }
    else
    {
        DEBUG_MSG("GetNAVMSEResourceState failed.");
    }

    return dwResult;
}

DWORD IsNAVMSEOwnedByLocalNode( BOOL * p_bOwned )
{
    DWORD dwResult = DEFAULT_ERROR;
    WCHAR wcsOwningNode[MAX_STR_LENGTH];
    
    CLUSTER_RESOURCE_STATE state = ClusterResourceStateUnknown;
    dwResult = GetNAVMSEResourceState( 
        &state,
        wcsOwningNode,
        MAX_STR_LENGTH );

    if (dwResult == ERROR_SUCCESS)
    {
        WCHAR wcsLocalNode[MAX_STR_LENGTH];
        dwResult = ClusterGetNodeNameW(wcsLocalNode, MAX_STR_LENGTH);
        if (dwResult == ERROR_SUCCESS)
        {
            if (wcscmp(wcsLocalNode, wcsOwningNode) == 0)
            {
                *p_bOwned = TRUE;
            }
            else
            {
                *p_bOwned = FALSE;
            }
        }
        else
        {
            DEBUG_MSG("ClusterGetNodeNameW failed.");
        }
    }
    else
    {
        DEBUG_MSG("GetNAVMSEResourceState failed.");
    }

    return dwResult;
    
}

DWORD GetNAVMSEResourceState(
        CLUSTER_RESOURCE_STATE * pState, 
        LPWSTR wcsActiveNode,
        DWORD dwMaxBuffer )
{
    DWORD dwResult = DEFAULT_ERROR;

    HRESOURCE hResource = OpenNAVMSEResource();
    if (hResource)
    {
        *pState = GetClusterResourceState(  
            hResource,   
            wcsActiveNode,
            &dwMaxBuffer,
            NULL,  
            NULL);

        if (*pState != ClusterResourceStateUnknown)
        {
            dwResult = ERROR_SUCCESS;
        }
        else
        {
            dwResult = GetLastError();
            VALIDATE_LAST_ERROR( dwResult );
        }

        if (!CloseClusterResource( hResource ))
        {
            DEBUG_MSG("CloseClusterResource failed.");
        }
    }

    return dwResult;
}

DWORD RemoveNAVMSEResource()
{
    DWORD dwResult = DEFAULT_ERROR;

    // Perform some prelimary checks...

    // Is this the primary node?
    BOOL bOwned = FALSE;
    dwResult = IsNAVMSEOwnedByLocalNode(&bOwned);
    if (dwResult == ERROR_SUCCESS)
    {
        if (bOwned)
        {
            BOOL bOnline = FALSE;
            dwResult = IsNAVMSEOnline(&bOnline);
            if (dwResult == ERROR_SUCCESS)
            {
                if (bOnline)
                {
                    dwResult = TakeNAVMSEResourceOffline();
                    if (dwResult != ERROR_SUCCESS)
                    {
                        DEBUG_MSG("TakeNAVMSEOffline failed.");
                    }
                }
            }
            else
            {
                DEBUG_MSG("IsNAVMSEOnline failed.");
            }


            // Were all the check successful?

            if (dwResult == ERROR_SUCCESS)
            {
                // Ok, remove the resource...

                HRESOURCE hResource = OpenNAVMSEResource();
                if (hResource)
                {
                    dwResult = DeleteClusterResource( hResource );
                    if (dwResult != ERROR_SUCCESS)
                    {
                        DEBUG_MSG("DeleteClusterResource failed.");
                    }
            
                    if (!CloseClusterResource( hResource ))
                    {
                        DEBUG_MSG("CloseClusterResource failed.");
                    }
                }
            }


        }
        else
        {
            dwResult = RemoveThisNodeFromNAVMSEResource();

        }
    }
    else
    {
        DEBUG_MSG("IsNAVMSEOwnedByLocalNode failed.");
    }

    return dwResult;
}


DWORD TakeNAVMSEResourceOffline()
{
    DWORD dwResult = DEFAULT_ERROR;

    HRESOURCE hResource = OpenNAVMSEResource();
    if (hResource)
    {
        dwResult = OfflineClusterResource( hResource );

        if (dwResult == ERROR_IO_PENDING)
        {
            BOOL bOffline = FALSE;
            BOOL bWait = TRUE;
            do
            {
                WCHAR wcsOwningNode[MAX_STR_LENGTH];
    
                CLUSTER_RESOURCE_STATE state = ClusterResourceStateUnknown;
                dwResult = GetNAVMSEResourceState( 
                    &state,
                    wcsOwningNode,
                    MAX_STR_LENGTH );

                if (dwResult == ERROR_SUCCESS)
                {
                    if (state != ClusterResourceOfflinePending)
                    {
                        bWait = FALSE;
                        if (state == ClusterResourceOffline)
                        {
                            bOffline = TRUE;
                        }
                        else
                        {
                            // DEBUG_MSG("Unable to take NAVMSE offline");
                            dwResult = DEFAULT_ERROR;
                        }
                    }
                }
                else
                {
                    DEBUG_MSG("GetNAVMSEResourceState failed.");
                }

            } while ( bWait );
        }

        if (dwResult != ERROR_SUCCESS)
        {
            DEBUG_MSG("OfflineClusterResource failed.");
        }

        if (!CloseClusterResource( hResource ))
        {
            DEBUG_MSG("CloseClusterResource failed.");
        }
    }

    return dwResult;
}


DWORD PutNAVMSEResourceOnline()
{
    DWORD dwResult = DEFAULT_ERROR;

    HRESOURCE hResource = OpenNAVMSEResource();
    if (hResource)
    {
        dwResult = OnlineClusterResource( hResource );
        if (dwResult == ERROR_IO_PENDING)
        {
            BOOL bOnline = FALSE;
            BOOL bWait = TRUE;
            do
            {
                WCHAR wcsOwningNode[MAX_STR_LENGTH];
    
                CLUSTER_RESOURCE_STATE state = ClusterResourceStateUnknown;
                dwResult = GetNAVMSEResourceState( 
                    &state,
                    wcsOwningNode,
                    MAX_STR_LENGTH );

                if (dwResult == ERROR_SUCCESS)
                {
                    if (state != ClusterResourceOnlinePending)
                    {
                        bWait = FALSE;
                        if (state == ClusterResourceOnline)
                        {
                            bOnline = TRUE;
                        }
                        else
                        {
                            // DEBUG_MSG("Unable to take NAVMSE online");
                            dwResult = DEFAULT_ERROR;
                        }
                    }
                }
                else
                {
                    DEBUG_MSG("GetNAVMSEResourceState failed.");
                }

            } while ( bWait );
        }

        if (dwResult != ERROR_SUCCESS)
        {
            DEBUG_MSG("OnlineClusterResource failed.");
        }

        if (!CloseClusterResource( hResource ))
        {
            DEBUG_MSG("CloseClusterResource failed.");
        }
    }

    return dwResult;
}



HRESOURCE OpenNAVMSEResource()
{
    DWORD dwResult = DEFAULT_ERROR;
    HRESOURCE hResource = NULL;

    WCHAR wcsNAVMSE[MAX_STR_LENGTH];

    dwResult = GetNAVMSEResourceName(wcsNAVMSE, MAX_STR_LENGTH);
    if (dwResult == ERROR_SUCCESS)
    {
        HCLUSTER hCluster = OpenLocalCluster();
        if (hCluster)
        {
            hResource = OpenClusterResource( hCluster, wcsNAVMSE );

            if (!CloseCluster( hCluster))
            {
                DEBUG_MSG("CloseCluster failed");
            }
        }
    }

    if ((hResource == NULL) && (dwResult == ERROR_SUCCESS))
    {
        dwResult = GetLastError();
        VALIDATE_LAST_ERROR( dwResult );
        SetLastError( dwResult );
    }

    return hResource;
}

DWORD GetNAVMSEResourceName( LPWSTR wcsName, DWORD dwMaxNameLength )
{
    static WCHAR g_wcsCachedResourceName[ MAX_STR_LENGTH ] = L"";
    static BOOL g_bCacheSet = FALSE;

    DWORD dwResult = DEFAULT_ERROR;

    if (g_bCacheSet)
    {
        dwResult = CopyW2W( wcsName, dwMaxNameLength, g_wcsCachedResourceName );
        return dwResult;
    }

    // retrieve resource name into cache.
    TFindNAVMSEServiceResource findgroup;
    
    dwResult = findgroup.Enum();
    if (dwResult == ERROR_SUCCESS)
    {
        if (findgroup.WasFound())
        {
            LPCWSTR wcsResourceNameFound = findgroup.GetName();
            dwResult = CopyW2W( wcsName, dwMaxNameLength, wcsResourceNameFound );

            // cache for future use.
            DWORD dwResultCached = CopyW2W( g_wcsCachedResourceName, MAX_STR_LENGTH, wcsResourceNameFound );
            g_bCacheSet = (dwResultCached == ERROR_SUCCESS);
        }
        else
        {
            DEBUG_MSG("Service Not Found!");
            dwResult = ERROR_NOT_FOUND;
        }
    }
    else
    {
        DEBUG_MSG("Error finding service!");
    }

    return dwResult;
}


DWORD GetResourcePropertyInExchangeGroup(
        TFindResourcesInGroup & Find,
        LPWSTR wcsValue, 
        DWORD dwBufferSize )
{
    DWORD dwResult = DEFAULT_ERROR;

    WCHAR wcsExchangeGroup[MAX_STR_LENGTH];
    dwResult = ClusterGetExchangeGroupW( wcsExchangeGroup, MAX_STR_LENGTH );
    if (dwResult == ERROR_SUCCESS)
    {
        BOOL bDummy = FALSE;
        Find.Init();
        dwResult = Find.OnGroup(wcsExchangeGroup, &bDummy);
        if (dwResult == ERROR_SUCCESS)
        {
            if ( Find.WasFound() )
            {
                dwResult = CopyW2W( wcsValue, dwBufferSize, Find.GetValue() );
            }
            else
            {
                dwResult = ERROR_NOT_FOUND;
                DEBUG_MSG("Could not find property in group");
            }
        }
        else
        {
            DEBUG_MSG("pFind->OnGroup failed");
        }
    }
    else
    {
        DEBUG_MSG("ClusterGetExchangeGroupW failed.");
    }

    return dwResult;
}


//
// Cluster Database helper functions.
// 

DWORD ClusterSetNAVMSEValueW( LPWSTR wcsName, LPWSTR wcsValue )
{
    DWORD dwResult = DEFAULT_ERROR;
    HKEY hKey = GetClusterNAVMSEKey();
    if (hKey)
    {
        DWORD dwBufferSize = (wcslen(wcsValue) +1) * sizeof(WCHAR);

        dwResult = ClusterRegSetValue(
            hKey,
            wcsName,
            REG_SZ,
            (LPBYTE)(wcsValue),
            dwBufferSize);

        if (dwResult != ERROR_SUCCESS)
        {
            DEBUG_MSG("ClusterRegSetValue failed");
        }

        if (ClusterRegCloseKey(hKey) != ERROR_SUCCESS)
        {
            DEBUG_MSG("ClusterCloseRegKey failed");
        }
    }
    else
    {
        dwResult = GetLastError();
        DEBUG_MSG("OpenNAVMSECluserDBKey failed");
        VALIDATE_LAST_ERROR( dwResult );
    }

    return dwResult;
}

DWORD ClusterGetNAVMSEValueW( LPWSTR wcsName, LPWSTR wcsValue, DWORD dwMaxBuffer )
{
    DWORD dwResult = DEFAULT_ERROR;
    HKEY hKey = GetClusterNAVMSEKey();
    if (hKey)
    {
        DWORD dwType = 0;
        DWORD dwBufferSize = dwMaxBuffer * sizeof(WCHAR);

        dwResult = ClusterRegQueryValue(
            hKey,
            wcsName,
            &dwType,
            (LPBYTE)(wcsValue),
            &dwBufferSize );


        if (dwResult == ERROR_SUCCESS)
        {
            if (dwType != REG_SZ)
            {
                dwResult = DEFAULT_ERROR;
                DEBUG_MSG("ClusterRegQueryValue returned a bad type");
            }
        }
        else
        {
            DEBUG_MSG("ClusterRegQueryValue failed");
        }

        if (ClusterRegCloseKey(hKey) != ERROR_SUCCESS)
        {
            DEBUG_MSG("ClusterCloseRegKey failed");
        }
    }
    else
    {
        dwResult = GetLastError();
        DEBUG_MSG("OpenNAVMSECluserDBKey failed");
        VALIDATE_LAST_ERROR( dwResult );
    }

    return dwResult;
}

HKEY  GetClusterNAVMSEKey()
{
    HKEY hKeyReturn = NULL;

    DWORD dwResult = DEFAULT_ERROR;

    HRESOURCE hResource = OpenNAVMSEResource();
    if (hResource)
    {
        HKEY hKey = GetClusterResourceKey( hResource, KEY_ALL_ACCESS );
        if (hKey)
        {
            HKEY hKeyNAVMSE = NULL;
            DWORD dwActionTaken = 0;
            dwResult = ClusterRegCreateKey(  
                hKey,                                   
                L"NAVMSE",                          
                REG_OPTION_NON_VOLATILE,                             
                KEY_ALL_ACCESS,                           
                NULL,  
                &hKeyNAVMSE,                             
                &dwActionTaken);
            if (dwResult == ERROR_SUCCESS)
            {
                hKeyReturn = hKeyNAVMSE;
            }
            else
            {
                DEBUG_MSG("ClusterRegCreateKey failed");
            }

            if (ClusterRegCloseKey(hKey) != ERROR_SUCCESS)
            {
                DEBUG_MSG("ClusterCloseRegKey failed");
            }
        }
        else
        {
            dwResult = GetLastError();
            DEBUG_MSG("GetClusterResourceKey failed");
            VALIDATE_LAST_ERROR( dwResult );
        }

        if (!CloseClusterResource(hResource))
        {
            DEBUG_MSG("ClusterCloseResource failed");
        }

    }
    else
    {
        dwResult = GetLastError();
        DEBUG_MSG("OpenNAVMSEResource failed");
        VALIDATE_LAST_ERROR( dwResult );
    }

    if (dwResult != ERROR_SUCCESS)
    {
        SetLastError( dwResult );
    }

    return hKeyReturn;
}



//
// Generic helper functions
//



DWORD ConvertErrorCodeToNaveSetupError(DWORD dwResult)
{
    switch(dwResult)
    {
    case ERROR_SUCCESS:
        return NAVE_SETUP_SUCCESS;

    case ERROR_INSUFFICIENT_BUFFER:
    case ERROR_MORE_DATA:
        return NAVE_SETUP_BUFFER_TOO_SMALL;

    default:
        return NAVE_SETUP_FAILURE;
    }
}


