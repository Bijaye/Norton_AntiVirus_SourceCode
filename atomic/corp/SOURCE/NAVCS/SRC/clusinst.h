// ----------------------------------------------------------------------------
// Norton AntiVirus for Microsoft Exchange
// Copyright (c) 1999
// ----------------------------------------------------------------------------
//
// FILE:     clustinst.h
//  
// MODULE:   navecs.dll (Nighthawk Setup Helper DLL)
//
// DESCRIPTION:
//      Contains Nighthawk functions to assist the installshield script.
//
// ----------------------------------------------------------------------------

extern "C" 
{

CDECLEXPORTAPI ClusterIsServer();

CDECLEXPORTAPI ClusterIsExchangeActiveOnNode();

CDECLEXPORTAPI ClusterGetName( LPSTR szClusterName, DWORD dwMaxBuffer );

CDECLEXPORTAPI ClusterGetNodeName( LPSTR szNodeName, DWORD dwMaxBuffer );

CDECLEXPORTAPI ClusterGetExchangeGroup( LPSTR szGroupName, DWORD dwMaxBuffer );

CDECLEXPORTAPI ClusterGetDefaultInstallPath( LPSTR szInstallPath, DWORD dwMaxBuffer );

CDECLEXPORTAPI ClusterIsValidInstallPath( LPCSTR szInstallPath );

CDECLEXPORTAPI ClusterGetNetworkName( LPSTR szNetworkName, DWORD dwMaxBuffer );

CDECLEXPORTAPI ClusterGetIPAddress( LPSTR szIPAddress, DWORD dwMaxBuffer );

CDECLEXPORTAPI ClusterGetIPSubnetMask( LPSTR szMask, DWORD dwMaxBuffer );

CDECLEXPORTAPI ClusterIsNAVMSEResourceInstalled();

CDECLEXPORTAPI ClusterAddNAVMSEResource();

CDECLEXPORTAPI ClusterRemoveNAVMSEResource();

CDECLEXPORTAPI ClusterIsNAVMSEResourceOnline();

CDECLEXPORTAPI ClusterPutNAVMSEResourceOnline();

CDECLEXPORTAPI ClusterTakeNAVMSEResourceOffline();

CDECLEXPORTAPI ClusterSetNAVMSEValue( LPCSTR szName, LPCSTR szValue );

CDECLEXPORTAPI ClusterGetNAVMSEValue( LPCSTR szName, LPSTR szValue, DWORD dwMaxBuffer );

// WORK IN PROGRESS
// CDECLEXPORTAPI fnTestFunction();

}
