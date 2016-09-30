// Copyright 1992 - 1994 Symantec, Peter Norton Group
// ************************************************************************
//
// $Header:   S:/NAVWINX5/VCS/NAVNODE.CPv   1.1   03 Aug 1998 16:47:14   jallee  $
//
// Description:
//
// See Also:
//
// ************************************************************************
// $Log:   S:/NAVWINX5/VCS/NAVNODE.CPv  $
// 
//    Rev 1.1   03 Aug 1998 16:47:14   jallee
// Now call FreeLibrary on the navwrapc.dll (options).
// 
//    Rev 1.0   01 Jun 1998 16:07:42   DBuches
// Initial revision.
//
//    Rev 1.4   07 May 1998 08:29:58   TSmith
// Fixed 114279 - Added code to CNavWinNode::Command to compose the full path
// to the help file so that the Symantec standard help system can be initialized
// somewhere way down deep in 'DisplayNAVOptionsDialog'.
//
//    Rev 1.3   29 Apr 1998 13:17:20   jallee
// Change reg key to ...NSC
//
//    Rev 1.2   01 Apr 1998 21:59:36   TSmith
// Added handling to CNavWinNode::Command for the various NAV option pages.
// Included handling for heinous DLL path dependency stuff.
//
//    Rev 1.1   04 Mar 1998 19:03:22   PAGBABI
// Moved menu string init to the Init func.  Do a release on the pUnk interface
// in OnFolder.
//
//    Rev 1.0   01 Mar 1998 18:50:48   pagbabi
// Initial revision.
// ************************************************************************

#include "stdafx.h"
#include "resource.h"
#include "NavWinX.h"

#include <initguid.h>
#include "NSDU_I.h"

#include "node.h"
#include "namespace.h"
#include "navnode.h"
#include "dataobj.h"
#include "obquery.h"
#include "nodeview.h"
#include "navview.h"

#include "menuitem.h"

// define statics
CString CNavWinNode::m_strNodeName;

// menu items
CString     CNavWinNode::m_strConfig95;
CString     CNavWinNode::m_strConfig95Desc;
CString     CNavWinNode::m_strConfigNT;
CString     CNavWinNode::m_strConfigNTDesc;
CString     CNavWinNode::m_strConfigNTS;
CString     CNavWinNode::m_strConfigNTSDesc;

// unique Product IDs

static const GUID GUID_NAV95 =
{ 0x129bea0, 0xf99e, 0x11d1, { 0x91, 0x1b, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a } };

static const GUID GUID_NAVNT =
{ 0x129bea1, 0xf99e, 0x11d1, { 0x91, 0x1b, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a } };

static const GUID GUID_NAVNTS =
{ 0x129bea2, 0xf99e, 0x11d1, { 0x91, 0x1b, 0x0, 0xc0, 0x4f, 0xac, 0x11, 0x4a } };


// extend the ObQuery namespace with these utilities
namespace ObQuery
{

//************************************************************************
// Function:
//
// Description:
//
// Parameters:
//
// Returns:
//
// Remarks:
//
//************************************************************************
// PMA Function created
//************************************************************************

BOOL IsScopePaneStaticNode( LPDATAOBJECT lpDataObject )
{
        BOOL bResult = FALSE;

    // m_cfInternal creates an object of type CSnapinDataObject
        FORMATETC formatetc = { CDataObject::m_cfNodeType, NULL,
                                                        DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

        GUID* ob;
        if(
        DoObjectQuery( lpDataObject, formatetc, ob ))
        {
                // static folder && CCT_SCOPE only
                if (*ob == CLSID_NSDU_STATICNODE)
                        bResult = TRUE;
        }

        FreeResources();
        return bResult;
}

//************************************************************************
// Function:
//
// Description:
//
// Parameters:
//
// Returns:
//
// Remarks:
//
//************************************************************************
// PMA Function created
//************************************************************************

BOOL IsScopePaneProductsNode( LPDATAOBJECT lpDataObject )
{
        BOOL bResult = FALSE;

    // m_cfInternal creates an object of type CSnapinDataObject
        FORMATETC formatetc = { CDataObject::m_cfNodeType, NULL,
                                                        DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

        GUID* ob;
        if(
        DoObjectQuery( lpDataObject, formatetc, ob ))
        {
                // static folder && CCT_SCOPE only
                if (*ob == CLSID_NSDU_PRODUCTSLIST)
                        bResult = TRUE;
        }

        FreeResources();
        return bResult;
}

};

/////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns:
//
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

CNavWinNode::CNavWinNode()
{
    AFX_MANAGE_STATE( AfxGetStaticModuleState() );  // needed for instance handle

    m_pSpace = NULL;
    m_pRegister = NULL;

    // load node decsription for scope pane display
    if (m_strNodeName.IsEmpty())
        m_strNodeName.LoadString( IDS_NODENAME );

}

// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns:
//
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

CNavWinNode::~CNavWinNode()
{
    delete m_pSpace;
    m_pSpace = NULL;

}

// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns:
//
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

HRESULT CNavWinNode::Init()
{
    // load menu item information
    if (m_strConfig95.IsEmpty())
    {
        m_strConfig95.LoadString( IDS_MENU_CONFIG_95 );

        m_strConfig95Desc.LoadString( IDS_MENU_CONFIG_95_DESC );

        m_strConfigNT.LoadString( IDS_MENU_CONFIG_NT );

        m_strConfigNTDesc.LoadString( IDS_MENU_CONFIG_NT_DESC );

        m_strConfigNTS.LoadString( IDS_MENU_CONFIG_NTS );

        m_strConfigNTSDesc.LoadString( IDS_MENU_CONFIG_NTS_DESC );
    }

    return S_OK;
}

//************************************************************************
// Function:
//
// Description:
//
// Parameters:
//
// Returns:
//
// Remarks:
//
//************************************************************************
// PMA Function created
//************************************************************************

STDMETHODIMP CNavWinNode::OnDestroy()
{
    HRESULT hr = S_OK;

    if (m_pRegister)
    {
        hr = m_pRegister->Unregister( GUID_NAV95 );
        hr = m_pRegister->Unregister( GUID_NAVNT );
        hr = m_pRegister->Unregister( GUID_NAVNTS );

        m_pRegister->Release();
        m_pRegister = NULL;
    }

    return hr;
}

/////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns:
//
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

STDMETHODIMP CNavWinNode::AddMenuItems(
        LPDATAOBJECT pDataObject,
        LPCONTEXTMENUCALLBACK pCallback,
    long* pInsertionAllowed)
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HRESULT hr = S_OK;

    if (*pInsertionAllowed & CCM_INSERTIONALLOWED_TASK)
    {
        // here is our opportunity to publish our menu items
        if (ObQuery::IsScopePaneProductsNode( pDataObject ))
        {
            // individual component item (result pane component)
            if (ObQuery::IsNodeIDEqual( pDataObject, GUID_NAV95 ))
            {
                CMMCMenuItem
                menu95( IDM_CONFIGURE_NAV95, m_strConfig95, m_strConfig95Desc );
                hr = pCallback->AddItem( &menu95 );
            }
            // individual component item (result pane component)
            else if (ObQuery::IsNodeIDEqual( pDataObject, GUID_NAVNT ))
            {
                CMMCMenuItem
                menuNT( IDM_CONFIGURE_NAVNT, m_strConfigNT, m_strConfigNTDesc );
                hr = pCallback->AddItem( &menuNT );
            }
            // individual component item (result pane component)
            else if (ObQuery::IsNodeIDEqual( pDataObject, GUID_NAVNTS ))
            {
                CMMCMenuItem
                menuNTS( IDM_CONFIGURE_NAVNTS, m_strConfigNTS, m_strConfigNTSDesc );
                hr = pCallback->AddItem( &menuNTS );
            }
            // publish all our menus with everyone else's
            else if (ObQuery::IsNodeIDEqual( pDataObject, GUID_NULL ))
            {
                CMMCMenuItem
                menu95( IDM_CONFIGURE_NAV95, m_strConfig95, m_strConfig95Desc );
                hr = pCallback->AddItem( &menu95 );

                CMMCMenuItem
                menuNT( IDM_CONFIGURE_NAVNT, m_strConfigNT, m_strConfigNTDesc );
                hr = pCallback->AddItem( &menuNT );

                CMMCMenuItem
                menuNTS( IDM_CONFIGURE_NAVNTS, m_strConfigNTS, m_strConfigNTSDesc );
                hr = pCallback->AddItem( &menuNTS );
            }
        }
    }

    return hr;
}

// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns:
//
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

INT ( STDAPICALLTYPE* lpfnDisplayNAVOptionsDialog )(
    HWND  hWnd,
    LPSTR lpHelpFile,
    LPSTR lpSourceDir,
    LPSTR lpTargetDir,
    BOOL  bIsNT
    );

STDMETHODIMP CNavWinNode::Command( long lCommandID, LPDATAOBJECT pDataObject )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    HINSTANCE hInst = NULL;
    HRESULT   hr = S_OK;
    char      szDatPath[ MAX_PATH + 1 ];
    char      szHelpFile[ MAX_PATH + 1 ];
    char      szBehavePath[ 32 ];
    TCHAR     szBinPath[ MAX_PATH + 1 ];
    TCHAR     szPath[ MAX_PATH + 1 ];
    DWORD     dwType;
    DWORD     dwSize;
    BOOL      bIsNt;
    HKEY      hKey;
    CString   TempPath;
    CString   BehavePath;
    CString   CurDir;
    CString   RegKey( _T( "SOFTWARE\\Symantec\\Norton System Center\\SnapIns\\NavWinX5\\" ) );

    ::ZeroMemory( szBinPath, sizeof( szBinPath ) );
    ::ZeroMemory( szDatPath, sizeof( szDatPath ) );
    ::ZeroMemory( szHelpFile, sizeof( szHelpFile ) );
    ::ZeroMemory( szBehavePath, sizeof( szBehavePath ) );
    ::ZeroMemory( szPath, sizeof( szPath ) );

    // Initialize product specific data.
    switch( lCommandID )
        {
        case IDM_CONFIGURE_NAV95:
            RegKey += _T( "NAV95" );
            BehavePath = _T( "\\WKSPDLL" );
            strcpy( szBehavePath, "\\WKSPDLL" );
            bIsNt = FALSE;
            break;
        case IDM_CONFIGURE_NAVNT:
            RegKey += _T( "NAVNTWKS" );
            BehavePath = _T( "\\WKSPDLL" );
            strcpy( szBehavePath, "\\WKSPDLL" );
            bIsNt = TRUE;
            break;
        case IDM_CONFIGURE_NAVNTS:
            RegKey += _T( "NAVNTSRV" );
            BehavePath = _T( "\\SVRPDLL" );
            strcpy( szBehavePath, "\\SVRPDLL" );
            bIsNt = TRUE;
            break;
        default:
            break;
        }

    if ( ERROR_SUCCESS != ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, ( LPCTSTR )RegKey, 0, KEY_READ, &hKey ) )
        {
        hr = E_FAIL;
        goto Command_Exit;
        }

    dwSize = MAX_PATH;
    if ( ERROR_SUCCESS != ::RegQueryValueEx( hKey, _T( "BinPath" ), NULL, &dwType, ( LPBYTE )szBinPath, &dwSize ) )
        {
        hr = E_FAIL;
        goto Command_Exit;
        }

    // Use the ANSI version for the next value explicitly since szDatPath will
    // be passed on to the non Unicode capable N32WRAPC.DLL.
    dwSize = MAX_PATH;
    if ( ERROR_SUCCESS != ::RegQueryValueExA( hKey, "DatPath", NULL, &dwType, ( LPBYTE )szDatPath, &dwSize ) )
        {
        hr = E_FAIL;
        goto Command_Exit;
        }

    // Save the current directory in preparation for changing it.
    ::GetCurrentDirectory( MAX_PATH, CurDir.GetBufferSetLength( MAX_PATH ) );
    CurDir.ReleaseBuffer( );

    // Set the current directory to the location of the options DLLs.
    ::SetCurrentDirectory( szBinPath );

    // Compose the directory of the product specific help file.
    wcstombs( szHelpFile, szBinPath, sizeof( szHelpFile ) );
    strcat( szHelpFile, szBehavePath );
    strcat( szHelpFile, "\\NAVWNT.HLP" );

    // Set up the path temporarily so that all the various and sundry DLLs
    // can find one another.
    ::GetEnvironmentVariable( _T( "PATH" ), szPath, MAX_PATH );
    TempPath = szBinPath;
    TempPath += _T( ";" );
    TempPath += szBinPath;
    TempPath += BehavePath;
    ::SetEnvironmentVariable( _T( "PATH" ), ( LPCTSTR )TempPath );

    // Load the NAV options wrapper DLL.
    _tcscat( szBinPath, _T( "\\N32WRAPC.DLL" ) );
    hInst = ::LoadLibraryEx( szBinPath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH );

    if ( NULL != hInst )
        {
        ( FARPROC& )lpfnDisplayNAVOptionsDialog =
            ::GetProcAddress( hInst, "_DisplayNAVOptionsDialog@20" );

        if ( lpfnDisplayNAVOptionsDialog )
            {
            lpfnDisplayNAVOptionsDialog( AfxGetMainWnd()->m_hWnd, szHelpFile, szDatPath, szDatPath, bIsNt );
            }
        FreeLibrary ( hInst );
        }

    // Put the path back the way it was.
    ::SetEnvironmentVariable( _T( "PATH" ), szPath );

    // Put the current directory back.
    ::SetCurrentDirectory( ( LPCTSTR )CurDir );

Command_Exit:

    return hr;
}

// ----------------------------------------------------------------
// Function:
//
// Description:
//
// Parameters:
//
// Returns:
//
// Remarks:
//
// ----------------------------------------------------------------
// PMA Function created
// ----------------------------------------------------------------

STDMETHODIMP CNavWinNode::CreateComponent( LPCOMPONENT* ppComponent )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    ASSERT( ppComponent );
    HRESULT hr = S_OK;

    // create the Snapin results object
    CComObject<CNavWinView>* pObject;
    CComObject<CNavWinView>::CreateInstance( &pObject );
    ASSERT( pObject );

    pObject->SetIComponentData( this );

    // get the interface to the component object and return to MMC
    hr = pObject->QueryInterface( IID_IComponent,
                                  reinterpret_cast<void**>(ppComponent) );

        return hr;
}

STDMETHODIMP CNavWinNode::OnFolder(
    LPDATAOBJECT pDataObject,
    BOOL bExpand,
    HSCOPEITEM hParent )
{
    HRESULT hr = S_OK;
    BOOL bRet;

    if (bExpand == FALSE)
        return hr;

    if (ObQuery::IsScopePaneStaticNode( pDataObject ) )
    {
        HICON hIcon1 = LoadIcon( AfxGetInstanceHandle(),
                                 MAKEINTRESOURCE(IDI_NAV_BAG) );
        HICON hIcon2 = LoadIcon( AfxGetInstanceHandle(),
                                 MAKEINTRESOURCE(IDI_NAVSERVER_BAG) );
        if (hIcon1 == NULL || hIcon2 == NULL)
        {
            DWORD dw = GetLastError();
            DisplaySystemMessage( dw );
        }

        LPUNKNOWN pUnk = NULL;

        bRet = ObQuery::GetIUnknownFromNode( pDataObject, &pUnk );
        if (bRet && pUnk && !m_pRegister)
        {
            hr = pUnk->QueryInterface( IID_IRegister, (void**)&m_pRegister );
            if (SUCCEEDED(hr))
            {
                CString strFamily, strProduct, strVersion, strDesc;
                strFamily.LoadString( IDS_PRODUCT_FAMILY );
                strProduct.LoadString( IDS_PRODUCT_NAV95 );

                // for now, all versions are 1.0 and have same description
                strVersion.LoadString( IDS_PRODUCT_COMPONENT_VERSION );
                strDesc.LoadString( IDS_PRODUCT_COMPONENT_DESC );

                m_pRegister->Register( strFamily,
                                 strProduct,
                                 strVersion,
                                 strDesc,
                                 hIcon1,
                                 CLSID_CNavExtension,
                                 GUID_NAV95 );

                strProduct.LoadString( IDS_PRODUCT_NAVNT );

                m_pRegister->Register( strFamily,
                                 strProduct,
                                 strVersion,
                                 strDesc,
                                 hIcon1,
                                 CLSID_CNavExtension,
                                 GUID_NAVNT );

                strProduct.LoadString( IDS_PRODUCT_NAVNTS );

                m_pRegister->Register( strFamily,
                                 strProduct,
                                 strVersion,
                                 strDesc,
                                 hIcon2,
                                 CLSID_CNavExtension,
                                 GUID_NAVNTS );

            }
        }

                if (pUnk)
                        pUnk->Release();
    }

    return hr;
}

//************************************************************************
// Function:
//
// Description:
//              Create an object that implements IDataObject and
//              return the interface.
//
// cookie [in]  - the lParam attached to the scope item struct
//
//
// type [in]    - MMC defined SCOPE_PANE, RESULT_PANE, SNAPIN_MANAGER
//
// nodetype[in] - unique identifier set by application identifying
//              the type of node, i.e. scope or result item type
//              as defined by us (see nodetypes.h)
//
// ppDataObject [out]
//                        CDataObject derived COM object
//
// Returns:   S_OK if successful
//
// Remarks:   Pure override to create console objects
//
//************************************************************************
// PMA Function created
//************************************************************************

STDMETHODIMP CNavWinNode::OnCreateDataObject(
    long cookie,
    DATA_OBJECT_TYPES type,
    NodeType nodetype,
    CDataObject** ppDataObject )
{
    CComObject<CDataObject> *pObject = NULL;
        CComObject<CDataObject>::CreateInstance( &pObject );
        ASSERT( pObject );

    *ppDataObject = pObject;

    return S_OK;
}


