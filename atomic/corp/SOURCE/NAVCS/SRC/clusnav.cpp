#include "clus.h"


//locals
DWORD CreateServiceResource(LPWSTR wcsServiceName,LPWSTR wcsDependency = NULL,LPWSTR wcsDependency2 = NULL);
DWORD TakeResourceOffline(HRESOURCE hResource);
DWORD TakeResourceOnline(HRESOURCE hResource);



// Atomic specific cluster support

//  Steps to make cluster aware

// X Find group with Physical Disk resource
// X Add resource NAV Autoprotect to this group
// X Add resource NAV Alert to this group
// X Add resource Norton Program Scheduler to this group
// X Make NAVAP dependent on NAV Alert
// X Make all three dependent on Physical Disk

// Per resource:
//	X resource name -- base resource
//	X service name
//	- node in resource (may not be needed)
//  - registry entries (shouldn't be needed)


CDECLEXPORTAPI GetDiskResource (LPWSTR wcsDriveLetter)
{
	DWORD dwResult = ERROR_SUCCESS;
	TFindPhysicalDisk fpd;

	// start group enum, which calls OnGroup() by default which in turn calls OnResource(),
	//  which calls IsMatchingResource which, in this case, is the TFindPhysicalDisk one..
	fpd.Enum();

    if ( fpd.WasFound() )
    {
        dwResult = CopyW2W( wcsDriveLetter, MAX_STR_LENGTH, fpd.GetName() );
    }
    else
    {
        dwResult = ERROR_NOT_FOUND;
        DEBUG_MSG("Could not find property in group");
    }

	return dwResult;
}



// This will give the name of the group that the physical disk is found in
//  This is useful since we must install in this group
CDECLEXPORTAPI GetPhysicalDiskGroupName (LPWSTR wcsGroupName)
{
	DWORD dwResult = ERROR_SUCCESS;
	TFindPhysicalDisk fpd;

	// start group enum, which calls OnGroup() by default which in turn calls OnResource(),
	//  which calls IsMatchingResource which, in this case, is the TFindPhysicalDisk one..
	fpd.Enum();

    if ( fpd.WasFound() )
    {
        dwResult = CopyW2W( wcsGroupName, MAX_STR_LENGTH, fpd.GetCurrentGroupName() );
    }
    else
    {
        dwResult = ERROR_NOT_FOUND;
        DEBUG_MSG("Could not find property in group");
    }

	return dwResult;
}

// creates a default resource
CDECLEXPORTAPI CreateResourceInGroup(LPWSTR wcsGroupName, LPWSTR wcsResourceName, LPWSTR wcsResourceType)
{
    DWORD dwResult = ERROR_SUCCESS;
        
	// get handle to the Cluster
    HCLUSTER hCluster = OpenLocalCluster();
    if (hCluster)
    {
		// get handle to the group that we need to create this resource in
        HGROUP hGroup = OpenClusterGroup(hCluster, wcsGroupName);
        if (hGroup)
        {
			// now create the resource
            HRESOURCE hResource = CreateClusterResource(  
                hGroup,             
                wcsResourceName,
                wcsResourceType,  
                0 );
            
            if (hResource)
            {
				// close the resource handle since we don't need to do anything with it
                if (!CloseClusterResource( hResource ))
                {
                    DEBUG_MSG("CloseClusterResource failed");
                }
            }
            else
            {
				// The resource creation failed
                dwResult = GetLastError();
                DEBUG_MSG("CreateClusterResource failed.");
                VALIDATE_LAST_ERROR( dwResult );
            }
            
			// done with the group handle, close it
            if (!CloseClusterGroup( hGroup ))
            {
				// close failed!  
                DEBUG_MSG("CloseClusterGroup failed");
            }
        }
        else
        {
			// opening the group failed; maybe it doesn't exist?
            dwResult = GetLastError();
            VALIDATE_LAST_ERROR( dwResult );
        }
        
        // done with the cluster handle, close it
        if ( !CloseCluster(hCluster) )
        {
            DEBUG_MSG("CloseCuster failed");
        }
    }
    else
    {
		// couldn't open the cluster; are we on a clustered machine?
        dwResult = GetLastError();
        DEBUG_MSG("OpenLocalCluster failed");
        VALIDATE_LAST_ERROR( dwResult );
    }

    return dwResult;
}


CDECLEXPORTAPI CreateNAVResources(void)
{
	WCHAR wcsDiskResource[MAX_STR_LENGTH];
	GetDiskResource(wcsDiskResource);
	CreateServiceResource(NAVALERT_SERVICE_NAME,wcsDiskResource);
	CreateServiceResource(NAVAP_SERVICE_NAME,wcsDiskResource, NAVALERT_SERVICE_NAME);
	return(CreateServiceResource(NPS_SERVICE_NAME,wcsDiskResource));
}

CDECLEXPORTAPI DeleteNAVResources(void)
{
    HCLUSTER hCluster = OpenLocalCluster();
    if (hCluster)
    {
		HRESOURCE hResource;

		hResource = OpenClusterResource(hCluster, NAVAP_SERVICE_NAME);
		if (hResource)
		{
			if (TakeResourceOffline(hResource) == ERROR_SUCCESS)
			{
				if (!DeleteClusterResource( hResource ) == ERROR_SUCCESS)
				{
					DEBUG_MSG("Could not clean up resource.");
					MESSAGE_W(L"Delete failed!");
				}
			}
			else
			{
				MESSAGE_W(L"Take offline failed.");
			}
		}
		Sleep(1000);
		hResource = OpenClusterResource(hCluster, NAVALERT_SERVICE_NAME);
		if (hResource)
		{
			if (TakeResourceOffline(hResource) == ERROR_SUCCESS)
			{
				if (!DeleteClusterResource( hResource ) == ERROR_SUCCESS)
				{
					DEBUG_MSG("Could not clean up resource.");
					MESSAGE_W(L"Delete failed!");
				}
			}
			else
			{
				MESSAGE_W(L"Take offline failed.");
			}
		}
		Sleep(1000);
		hResource = OpenClusterResource(hCluster, NPS_SERVICE_NAME);
		if (hResource)
		{
			if (TakeResourceOffline(hResource) == ERROR_SUCCESS)
			{
				if (!DeleteClusterResource( hResource ) == ERROR_SUCCESS)
				{
					MESSAGE_W(L"Delete failed!");
					DEBUG_MSG("Could not clean up resource.");
				}
			}
			else
			{
				MESSAGE_W(L"Take offline failed.");
			}
		}
		CloseCluster(hCluster);
	}

	return ERROR_SUCCESS;
}

DWORD AddStringPropertyToResource(HRESOURCE hResource, LPWSTR wcsPropName, LPWSTR wcsPropNameValue )
{
	DWORD dwResult = ERROR_SUCCESS;


	CClusPropList cPropList; // the property list

	// Add the property to the list.
	// Compare how easy this is relative to the other
	// ways to create a property list.
	cPropList.AddProp( wcsPropName, 
					   wcsPropNameValue,
					   L"");


	// Set the property.  Use the following as input parameters:
	//   cPropList.PbProplist() returns a pointer to the property list
	//  .cPropList.CbProplist() returns the size of the property list.
	dwResult = ClusterResourceControl( hResource,
								   NULL, 
								   CLUSCTL_RESOURCE_SET_PRIVATE_PROPERTIES,
								   cPropList.PbProplist(),
								   cPropList.CbProplist(),
								   NULL,
								   0,
								   NULL );
	

    return dwResult;
} // end GrpCreatePropList_Buff()


CDECLEXPORTAPI SetNewProp(void)
{

    DWORD dwError = ERROR_SUCCESS;

    CClusPropList cPropList; // the property list
    HCLUSTER hCluster = OpenLocalCluster();
    if (hCluster)
    {
		// get handle to the group that we need to create this resource in
        HRESOURCE hResource = OpenClusterResource(hCluster, L"NAV Auto-Protect");
        if (hResource)
        {

			// Add the property to the list.
			// Compare how easy this is relative to the other
			// ways to create a property list.
			cPropList.AddProp( L"Description", 
							   L"Test",
							   L"");


			// Set the property.  Use the following as input parameters:
			//   cPropList.PbProplist() returns a pointer to the property list
			//  .cPropList.CbProplist() returns the size of the property list.
			dwError = ClusterResourceControl( hResource,
										   NULL,  // optional, can be NULL
										   CLUSCTL_RESOURCE_SET_COMMON_PROPERTIES,
										   cPropList.PbProplist(),
										   cPropList.CbProplist(),
										   NULL,
										   0,
										   NULL );
	
		}
	}

	return dwError;

}

// Creates a "Generic Service" resource with up to two dependencies
DWORD CreateServiceResource(LPWSTR wcsServiceName,LPWSTR wcsDependency,LPWSTR wcsDependency2 )
{
	DWORD		dwResult	= ERROR_SUCCESS;
	HRESOURCE	hResource	= 0;
	HCLUSTER	hCluster	= 0;

	WCHAR		wcsGroupName[MAX_STR_LENGTH];

	// find the group that contains a shared physical disk resource
	dwResult = GetPhysicalDiskGroupName(wcsGroupName);
	if (dwResult != ERROR_SUCCESS)
	{
		return dwResult;
	}

	// use this group to create our resource in
	dwResult = CreateResourceInGroup(wcsGroupName,wcsServiceName,L"Generic Service");
	if (dwResult != ERROR_SUCCESS)
	{
		return dwResult;
	}

	// now assign properties to our new resource
	hCluster = OpenLocalCluster();
	if (hCluster)
	{
		hResource = OpenClusterResource( hCluster, wcsServiceName );
		if (hResource)
		{
			dwResult = AddStringPropertyToResource(hResource, L"ServiceName", wcsServiceName);
			if (dwResult != ERROR_SUCCESS)
			{
				// we delete the resource here since we failed in assigning the properties
				//	for it to work correctly
                if (!DeleteClusterResource( hResource ) == ERROR_SUCCESS)
                {
                    DEBUG_MSG("Could not clean up resource on property assign failure.");
                }
			}
			else
			{
				if (wcsDependency != NULL)
				{
					AddDependencyToResource(hCluster, hResource, wcsDependency);
					if (wcsDependency2 != NULL)
					{
						AddDependencyToResource(hCluster, hResource, wcsDependency2);
					}
				}
				TakeResourceOnline(hResource);
			}
		}
		else
		{
			// can't open the resource, which is unlikely if it was really created successfully
			dwResult = DEFAULT_ERROR;
		}
	}
	else
	{
		// this is very unlikely, as we've succeeded in opening the cluster to create
		//	the resource already
		dwResult = DEFAULT_ERROR;
	}

	// close resource and cluster handles
	if (hResource)
	{
        if (!CloseClusterResource( hResource ))
        {
            DEBUG_MSG("CloseClusterResource failed");
        }
	}

	if (hCluster)
	{
        if ( !CloseCluster(hCluster) )
        {
            DEBUG_MSG("CloseCuster failed");
        }
	}

	return dwResult;
}

DWORD TakeResourceOffline(HRESOURCE hResource)
{
    DWORD dwResult = DEFAULT_ERROR;

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

				DWORD dwMaxBuffer = MAX_STR_LENGTH;
				state = GetClusterResourceState(  
							hResource,   
							wcsOwningNode,
							&dwMaxBuffer,
							NULL,  
							NULL);

				if (state != ClusterResourceStateUnknown)
				{
					dwResult = ERROR_SUCCESS;
				}
				else
				{
					dwResult = GetLastError();
					VALIDATE_LAST_ERROR( dwResult );
				}
                
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
                    DEBUG_MSG("GetResourceState failed.");
                }

            } while ( bWait );
        }

        if (dwResult != ERROR_SUCCESS)
        {
            DEBUG_MSG("OfflineClusterResource failed.");
        }

    }

    return dwResult;
}



DWORD TakeResourceOnline(HRESOURCE hResource)
{
    DWORD dwResult = DEFAULT_ERROR;

    if (hResource)
    {
        dwResult = OnlineClusterResource( hResource );

        if (dwResult == ERROR_IO_PENDING)
        {
            BOOL bOffline = FALSE;
            BOOL bWait = TRUE;
            do
            {
				WCHAR wcsOwningNode[MAX_STR_LENGTH];

				CLUSTER_RESOURCE_STATE state = ClusterResourceStateUnknown;

				DWORD dwMaxBuffer = MAX_STR_LENGTH;
				state = GetClusterResourceState(  
							hResource,   
							wcsOwningNode,
							&dwMaxBuffer,
							NULL,  
							NULL);

				if (state != ClusterResourceStateUnknown)
				{
					dwResult = ERROR_SUCCESS;
				}
				else
				{
					dwResult = GetLastError();
					VALIDATE_LAST_ERROR( dwResult );
				}
                
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
                    DEBUG_MSG("GetResourceState failed.");
                }

            } while ( bWait );
        }

        if (dwResult != ERROR_SUCCESS)
        {
            DEBUG_MSG("OfflineClusterResource failed.");
        }

    }

    return dwResult;
}


