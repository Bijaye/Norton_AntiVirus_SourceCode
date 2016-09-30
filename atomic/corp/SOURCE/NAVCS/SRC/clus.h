#ifndef _CLUS_H
#define _CLUS_H


#define STRICT 1

//stop L4 warnings from Windows include files
#pragma warning(disable:4201) //nonstandard extension used
#pragma warning(disable:4514) //unreferenced inline function
#pragma warning(disable:4214) //nonstandard extension used

#pragma warning(disable:4710) // function 'x' not expanded
#pragma warning(disable:4711) // function 'x' selected for inline expansion

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <tchar.h>
#include <clusapi.h>
#include <resapi.h>

#include "navecs.h"
#include "clusinst.h"

#include <WinError.h>


    #include "proplist.h"             // CClusPropList definition

// Define these three here because the version of WinError.h shipped with VC 5 doesn't.
#ifndef ERROR_NOT_FOUND
#define ERROR_NOT_FOUND                  1168L
#endif

#ifndef ERROR_INVALID_STATE
#define ERROR_INVALID_STATE              5023L
#endif

#ifndef ERROR_RESOURCE_PROPERTIES_STORED
#define ERROR_RESOURCE_PROPERTIES_STORED 5024L
#endif



#define DEFAULT_PATH                        L"\\NAVMSE"
#define DEFAULT_NAVMSE_RESOURCE_NAME        L"Norton AntiVirus for Microsoft Exchange"
#define DEFAULT_NAVMSE_RESOURCE_DESCRIPTION L"Norton AntiVirus for Microsoft Exchange"
#define NAVMSE_SERVICE_NAME                 L"NavExchange"
#define NAVMSE_SERVICE_REGISTRY_KEY         L"SYSTEM\\CurrentControlSet\\Services\\NavExchange"
#define NAVMSE_APP_REGISTRY_KEY             L"SOFTWARE\\Symantec\\NAVMSE"
#define NAVMSE_EVENTLOG_REGISTRY_KEY        L"SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\NavExchange"
#define MAX_STR_LENGTH  (_MAX_PATH+1)
#define DEFAULT_ERROR   ERROR_INVALID_PARAMETER;

#define NAVAP_SERVICE_NAME	L"NAV Auto-Protect"
#define NAVALERT_SERVICE_NAME L"NAV Alert"
#define NPS_SERVICE_NAME L"Norton Program Scheduler"

#define MESSAGE( x )        MessageBoxA( NULL, x, "Cluster Info", MB_OK );
#define MESSAGE_W( x )      MessageBoxW( NULL, x, L"Cluster Info", MB_OK );



#if _DEBUG
    #define VALIDATE_LAST_ERROR( dw )  if (dw==ERROR_SUCCESS){ \
        char szMsg[1024]; \
        wsprintf(szMsg, "LastError not set  %s, %d\n",__FILE__, __LINE__); \
        OutputDebugString( szMsg ); \
        dw = DEFAULT_ERROR; }
#else
    #define VALIDATE_LAST_ERROR( dw )  if (dw==ERROR_SUCCESS){ \
        dw = DEFAULT_ERROR; }
#endif



///  General Cluster prototypes





/////////////////////////////////////////////////////
// Local funcations

// Wide char interface functions


DWORD ClusterGetNameW( LPWSTR wcsClusterName, DWORD dwMaxBuffer );

DWORD ClusterGetNodeNameW( LPWSTR wcsNodeName, DWORD dwMaxBuffer );

DWORD ClusterGetDefaultInstallPathW( LPWSTR wcsInstallPath, DWORD dwMaxBuffer );

BOOL  ClusterIsValidInstallPathW( LPCWSTR wcsInstallPath );

DWORD ClusterGetNetworkNameW( LPWSTR wcsNetworkName, DWORD dwMaxBuffer );

DWORD ClusterGetIPAddressW( LPWSTR wcsIPAddress, DWORD dwMaxBuffer );

DWORD ClusterGetIPSubnetMaskW( LPWSTR wcsMask, DWORD dwMaxBuffer );


// ... for clustering


HCLUSTER OpenLocalCluster();

DWORD GetPhysicalDisk(LPWSTR wcsValue, DWORD dwBufferSize);

DWORD AddRegKeyCheckPointsToResource( 
        HRESOURCE hResource,
        LPWSTR wcsRegKey );

DWORD AddDependencyToResource( 
        HCLUSTER hCluster, 
        HRESOURCE hResource, 
        LPWSTR wcsResourceDependency );

DWORD SetPrivateProperties(
        HRESOURCE hResource, 
        CLUSPROP_LIST * pPropertyList,
        DWORD dwListBufferSize );

DWORD ValidatePrivateProperties( 
        HRESOURCE hResource, 
        CLUSPROP_LIST * pPropertyList,
        DWORD dwListBufferSize );

DWORD IsClusterServiceResource( 
        HCLUSTER hCluster,  
        LPCWSTR wcsResourceName,
        LPCWSTR wcsServiceName,
        BOOL * p_bIsMatch );

BOOL IsIPAddressResource( 
        HRESOURCE hResource );

BOOL IsNetworkNameResource( 
        HRESOURCE hResource );

BOOL IsPhysicalDiskResource( 
        HRESOURCE hResource );

BOOL IsGenericServiceResource( 
        HRESOURCE hResource );

BOOL IsResourceOfType( 
        HRESOURCE hResource, 
        LPWSTR wcsResourceTypeToMatch );

DWORD GetResourceType( 
        HRESOURCE hResource, 
        LPWSTR wczTypeName );

BOOL FindPropertyString(
        CLUSPROP_LIST * pPropertyList,
        DWORD dwBufferSize,
        LPCWSTR wcsPropertyToFind,
        LPWSTR * pwcsValue );

DWORD GetResourcePropertyString( 
        HRESOURCE hResource,  
        LPCWSTR wcsPropertyName,
        LPWSTR wcsBuffer,
        DWORD dwMaxBuffer);


DWORD ValidateNodeName( 
        LPWSTR wcsNodeName );

BOOL FindPropertyPartitionInfo(
        CLUSPROP_VALUE * pValueList,
        DWORD dwBufferSize,
        LPWSTR wcsInfo,
        DWORD * p_dwInfoBufferSize);

DWORD GetDiskPartitionList( 
        HRESOURCE hResource,  
        LPWSTR wcsBuffer,
        DWORD * p_dwMaxBuffer);



//
// Helper class for enumerating through cluster resources.
//
class TEnumerateCluster
{
public:
    TEnumerateCluster();
    virtual ~TEnumerateCluster();

    virtual DWORD Open();
    virtual DWORD Close();

    virtual DWORD OnPreEnum() { return ERROR_SUCCESS; }
    DWORD Enum(DWORD dwEnumType = CLUSTER_ENUM_GROUP );
    virtual DWORD OnPostEnum(DWORD dwResult) { return dwResult; }

    virtual DWORD OnGroup( LPWSTR wcsName, BOOL * p_bContinue );

    virtual DWORD OnResource( LPWSTR wcsName, BOOL * p_bContinue ) = 0;
	
	LPCWSTR GetCurrentGroupName() { return m_wcsCurrentGroup; }

protected:
    HCLUSTER m_hCluster;
    LPWSTR m_wcsCurrentGroup;
    LPWSTR m_wcsCurrentResource;
};

//
// Helper class for finding a group associated with a resource.
//
class TFindResource : public TEnumerateCluster
{
public:
    TFindResource();

    void Init();

    virtual DWORD OnPreEnum();

    virtual DWORD OnResource( LPWSTR wcsName, BOOL * p_bContinue );

    virtual DWORD IsMatchingResource(LPWSTR wcsName, BOOL * p_bMatching) = 0;

    virtual DWORD SetValueForResource( LPWSTR wcsValue );
    virtual DWORD SetNameForResource( LPWSTR wcsValue );

    BOOL    WasFound(){ return m_bFound; }
    LPCWSTR GetValue(){ return m_wcsResourceValue; }
    LPCWSTR GetName() { return m_wcsResourceName; }

protected:
    WCHAR m_wcsResourceValue[MAX_STR_LENGTH];
    WCHAR m_wcsResourceName[MAX_STR_LENGTH];
    BOOL  m_bFound;
};

//
// Class specifically for finding the exchange service.
//

class TFindServiceResource : public TFindResource
{
protected:
    virtual DWORD IsMatchingResource(LPWSTR wcsName, BOOL * p_bMatching);
    virtual LPCWSTR GetServiceName() = 0;
};

//
// Helper class for finding the value for a resource in a group.
//
class TFindResourcesInGroup : public TFindResource
{
protected:
    virtual DWORD OnResource( LPWSTR wcsName, BOOL * p_bContinue );
    virtual DWORD IsMatchingResource(LPWSTR /*wcsName*/, BOOL * /*p_bMatching*/)
    { DEBUG_MSG("don't go here"); return ERROR_NOT_SUPPORTED; }
};


//
// Class specifically for finding the physical disk
//
class TFindPhysicalDisk : public TFindResourcesInGroup
{
protected:
    virtual DWORD IsMatchingResource(LPWSTR wcsName, BOOL * p_bMatching);
    virtual BOOL IsMatchingResourceType(HRESOURCE hResource)
        { return IsPhysicalDiskResource(hResource); }
    virtual DWORD OnAction( HRESOURCE hResource );
};

//
// Class specifically for finding the physical disk
//
class TValidatePathOnDisk : public TFindPhysicalDisk
{
public:
    TValidatePathOnDisk();
    virtual DWORD IsValidPath( LPCWSTR wcsPath );

protected:
    virtual DWORD OnAction( HRESOURCE hResource );

    LPCWSTR m_wcsPath;
};

//
// Helper class for finding the value for a resource in a group.
//
class TFindResourceProperty : public TFindResourcesInGroup
{
protected:
    virtual DWORD IsMatchingResource(LPWSTR wcsName, BOOL * p_bMatching);
    virtual BOOL IsMatchingResourceType(HRESOURCE hResource) = 0;
    virtual LPCWSTR GetPropertyName() = 0;
};

//
// Class specifically for finding the network name
//
class TFindNetworkName : public TFindResourceProperty
{
protected:
    virtual BOOL IsMatchingResourceType(HRESOURCE hResource)
        { return IsNetworkNameResource(hResource); }
    virtual LPCWSTR GetPropertyName()
        { return L"Name"; }
};

//
// Class specifically for finding the IpAddress
//
class TFindIPAddress : public TFindResourceProperty
{
protected:
    virtual BOOL IsMatchingResourceType(HRESOURCE hResource)
        { return IsIPAddressResource(hResource); }
    virtual LPCWSTR GetPropertyName()
        { return L"Address"; }
};

//
// Class specifically for finding the subnet mask
//
class TFindIPSubnetMask : public TFindIPAddress
{
protected:
    virtual LPCWSTR GetPropertyName()
        { return L"SubnetMask"; }
};

// NAVMSE Specific prototypes

/////////////////////////////////////////////////////
// Local funcations

// Wide char interface functions
DWORD ClusterGetExchangeGroupW( LPWSTR wcsGroupName, DWORD dwMaxBuffer );

DWORD ClusterGetNAVMSEGroupW( LPWSTR wcsGroupName, DWORD dwMaxBuffer );

DWORD AddNAVMSEResource();

DWORD RemoveNAVMSEResource();

DWORD ClusterSetNAVMSEValueW( LPWSTR wcsName, LPWSTR wcsValue );

DWORD ClusterGetNAVMSEValueW( LPWSTR wcsName, LPWSTR wcsValue, DWORD dwMaxBuffer );

BOOL  IsExchangeActiveOnNode();

BOOL  IsNAVMSEResourceInstalled();






// ... for clustering


DWORD CreateNAVMSEResource();  // called by AddNAVMSEResource

DWORD AddThisNodeToNAVMSEResource();

DWORD RemoveThisNodeFromNAVMSEResource();

DWORD AddNAVMSEDependenciesToResource( HCLUSTER hCluster, HRESOURCE hResource );

DWORD AddNAVMSEPrivatePropertiesToResource( HRESOURCE hResource );

DWORD AddNAVMSERegKeyCheckPointsToResource( HRESOURCE hResource );

DWORD IsNAVMSEOwnedByLocalNode( BOOL * p_bOwned );

DWORD IsNAVMSEOnline( BOOL * p_bOnline );

DWORD PutNAVMSEResourceOnline();

DWORD TakeNAVMSEResourceOffline();

HKEY  GetClusterNAVMSEKey();

HRESOURCE OpenNAVMSEResource();

DWORD GetNAVMSEResourceName( LPWSTR wcsName, DWORD dwMaxNameLength );


class TFindResourcesInGroup; // forward

DWORD GetResourcePropertyInExchangeGroup(
        TFindResourcesInGroup & Find,
        LPWSTR wcsValue, 
        DWORD dwBufferSize );

class TFindResource; // forward

DWORD GetResourceNameForResourceInExchangeGroup(
        TFindResource & Find, 
        LPWSTR wcsResourceName, 
        DWORD wcsMaxNameLength);

DWORD GetExchangeGroupState(
        CLUSTER_GROUP_STATE * pState, 
        LPWSTR wcsActiveNode, 
        DWORD dwMaxBuffer );

DWORD GetNAVMSEResourceState(
        CLUSTER_RESOURCE_STATE * pState, 
        LPWSTR wcsActiveNode,
        DWORD dwMaxBuffer );




//
// Class specifically for finding the exchange service.
//

class TFindExchangeInformationStoreServiceResource : public TFindServiceResource
{
protected:
    virtual LPCWSTR GetServiceName() { return L"MSExchangeIS"; }
};

class TFindExchangeDirectoryServiceResource : public TFindServiceResource
{
protected:
    virtual LPCWSTR GetServiceName() { return L"MSExchangeDS"; }
};

class TFindExchangeSystemAttendantServiceResource : public TFindServiceResource
{
protected:
    virtual LPCWSTR GetServiceName() { return L"MSExchangeSA"; }
};

class TFindNAVMSEServiceResource : public TFindServiceResource
{
protected:
    virtual LPCWSTR GetServiceName() { return L"NavExchange"; }
};

// ... general utility

DWORD ConvertErrorCodeToNaveSetupError(DWORD dwResult);

/////////////////////////////////////////////////////



// nav specific prt
#ifdef  __cplusplus
extern "C"  {
#endif
CDECLEXPORTAPI GetDiskResource (LPWSTR wcsDriveLetter);
CDECLEXPORTAPI GetPhysicalDiskGroupName (LPWSTR wcsGroupName);
CDECLEXPORTAPI CreateResourceInGroup(LPWSTR wcsGroupName, LPWSTR wcsResourceName, LPWSTR wcsResourceType);
CDECLEXPORTAPI CreateNAVResources(void);
CDECLEXPORTAPI DeleteNAVResources(void);
DWORD AddStringPropertyToResource(HRESOURCE hResource, LPWSTR wcsPropName, LPWSTR wcsPropNameValue );
CDECLEXPORTAPI SetNewProp(void);
#ifdef  __cplusplus
}
#endif

#endif // _CLUS_H