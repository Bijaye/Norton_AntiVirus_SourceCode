// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// SavProduct.cpp
//
// Implementation of CSavProductPlugin -- ccApp product plug-in for SAV Corporate Edition.
// Responsible for loading our functionality plug-in's (at this time, ccEmailProxy).
//***************************************************************************

#include "stdafx.h"
#include "ccAppPlugin.h"
#include "SavProduct.h"

extern HINSTANCE g_hInstance;


//////////////////////////////////////////////////////////////////////
// Plugin DLL names.  

/*
const char * CSavProductPlugin::m_sSAVPluginDllNames[] = 
    {
    NULL
    };
*/

const char * CSavProductPlugin::m_sCommonPluginDllNames[] = 
    {
    "ccEmlPxy.dll",
    NULL
    };


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSavProductPlugin::CSavProductPlugin()
{
    // Intentionally empty.
}

CSavProductPlugin::~CSavProductPlugin()
{
    // Intentionally empty.
}

//////////////////////////////////////////////////////////////////////
//  ProductPlugIn::VerifyProduct()

SYMRESULT CSavProductPlugin::VerifyProduct()            
{
    // TODO: Add code to verify the correct installation/configuration
    // of the product.  Note that the verification code needs to be as
    // fast as possible, since this code is called synchronously by 
    // ccApp.

    auto    TCHAR   szFullPath[ MAX_PATH ];

    auto    TCHAR   szDrive[ MAX_PATH ];
    auto    TCHAR   szDir[ MAX_PATH ];
    auto    TCHAR   szFilename[ MAX_PATH ];

    auto    SYMRESULT   result;

    szDrive[0]    = _T('\0');
    szDir[0]      = _T('\0');
    szFilename[0] = _T('\0');

    result = SYMERR_CCAPP_BAD_INSTALL;

    // Get program directory

    if ( 0 == GetModuleFileName( g_hInstance, szFullPath, sizeof( szFullPath ) ) )
    {
        goto BailOut;
    }

    _tsplitpath( szFullPath, szDrive, szDir, szFilename, NULL );

    m_strPath = szDrive;
    m_strPath += szDir;

    // Get Symantec Common directory
    //&? Best way to go about this? Consumer NAV plug-in gets location of
    // common program files directory and appends hard-coded "Symantec Shared".
    // I'm making the assumption here that the loading application is ccApp which
    // resides in the shared common client directory.

    if ( 0 == GetModuleFileName( NULL, szFullPath, sizeof( szFullPath ) ) )
    {
        goto BailOut;
    }

    _tsplitpath( szFullPath, szDrive, szDir, szFilename, NULL );

    m_strCCPath = szDrive;
    m_strCCPath += szDir;

    // build our list of plugins.

    buildPluginList();

    if ( m_vPlugins.size() > 0 )
    {
        result = SYM_OK;
    }

BailOut:
    return ( result );
}


//////////////////////////////////////////////////////////////////////
//  CSavProductPlugin::GetPluginDLLCount()

unsigned int CSavProductPlugin::GetPluginDLLCount()
{
    try
    {
        return ( m_vPlugins.size() );
    }
    catch(...)
    {
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////
//  CSavProductPlugin::GetPluginDLLPath()

const char * CSavProductPlugin::GetPluginDLLPath( unsigned int uIndex )
{
    try
    {
        return ( m_vPlugins.at( uIndex ).c_str() );
    }
    catch(...)
    {
    }

    return ( NULL );
}

//////////////////////////////////////////////////////////////////////
//  CSavProductPlugin::CNavProductPlugin()

const char * CSavProductPlugin::GetProductName()
{
    return ( "Symantec AntiVirus" );        // Logical tag -- not for display.
}


//////////////////////////////////////////////////////////////////////
//  CNavProductPlugin::buildPluginList()

bool CSavProductPlugin::buildPluginList()
{
    auto    int     i;

    // Add SAV specific plugins.
    // (none currently)

/*
    for( i = 0; m_sSAVPluginDllNames[i] != NULL; i++ )
    {
        string sPluginPath = m_strPath;
        sPluginPath += '\\';
        sPluginPath += m_sSAVPluginDllNames[i];
        m_vPlugins.push_back( sPluginPath );
    }
*/

    // Add common client plugins

    for( i = 0; m_sCommonPluginDllNames[i] != NULL; i++ )
    {
        string sPluginPath = m_strCCPath;
        sPluginPath += '\\';
        sPluginPath += m_sCommonPluginDllNames[i];
        m_vPlugins.push_back( sPluginPath );
    }

    return true;
}


