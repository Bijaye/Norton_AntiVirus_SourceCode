// Copyright 1997 Symantec
// -------------------------------------------------------------------------
// sysworks.CPP
//
// This is a skeletal about box System Works extension for NAV.  This the minimum 
// you need to implement to add a page to the Symantec Standard About box.
//
// $Header:   S:/ABOUTSW/VCS/sysworks.cpv   1.4   16 Nov 1998 15:03:52   CEATON  $
// -------------------------------------------------------------------------
// $Log:   S:/ABOUTSW/VCS/sysworks.cpv  $
// 
//    Rev 1.4   16 Nov 1998 15:03:52   CEATON
// Modified the NSW dialog box to use font mapping (for international versions).
// 
// 
//    Rev 1.3   29 Oct 1998 21:33:30   CEATON
// Loaded product version number from string resource instead of VERSION.DAT due
// to uninstall issues (VERSION.DAT deleted).
// 
// 
//    Rev 1.2   29 Oct 1998 18:14:48   CEATON
// Modified Get/GenerateSystemWorksKey() to return the parent key value
// (HKEY_LOCAL_MACHINE/HKEY_CURRENT_USER).
// 
// 
//    Rev 1.1   22 Oct 1998 13:28:56   rchinta
// Added the new phone number and product offer
// 
//    Rev 1.0   20 Oct 1998 20:22:16   CEATON
// Initial revision.
//  
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "platform.h"                   // Platform independence
#include "resource.h"					// resource ID's
#include "ngui.h"                       // Norton look / feel
#include "symlean.h"					// light-weight class framework
#include "n32behav.h"
#include "aboutint.h"                                   // standard about interfaces
#include "aboutsw.h"
#include "sysworks.h"


/////////////////////////////////////////////////////////////////////////////
// Standard About Box Extension (Property Page)

///////////////////////////////////////
// Constructor

CSystemWorksPage::CSystemWorksPage()
{
}

///////////////////////////////////////
// Destructor

CSystemWorksPage::~CSystemWorksPage()
{
}

BOOL CSystemWorksPage::OnInitDialog()
{
    BOOL bValue;

   	CslEditCtrl edtContact;

    // Set the correct font for this dialog.
    bValue = CslDlg::OnInitDialog();

    NG_SetFontsInDialog( GetHandle(), 0 );

    edtContact.FromID( GetHandle(), IDC_EDIT_CONTACT );
    CslString strOffer1( IDS_PRODUCT_OFFER1 );
    CslString strOffer2( IDS_PRODUCT_OFFER2 );
    CslString strOffer3( IDS_PRODUCT_OFFER3 );
    CslString strOffer4( IDS_PRODUCT_OFFER4 );

    CslString strContact = strOffer1 + strOffer2 + strOffer3 + strOffer4;

    for ( int iString = IDS_PHONE00; iString <= IDS_PHONE37; iString++ )
    {
        CslString strPhone( iString );
        strContact += strPhone;
    }

    edtContact.SetText(strContact);

    return( bValue );
}

///////////////////////////////////////
// UpdateData

BOOL CSystemWorksPage::UpdateData( BOOL bSaveAndValidate )
{
	return TRUE;
}

///////////////////////////////////////
// OnCommand

BOOL CSystemWorksPage::OnCommand( int nID, HWND hCtrl, UINT uNotify )
{
	return CslDlg::OnCommand( nID, hCtrl, uNotify );
}


BOOL WINAPI DoSystemWorksAbout( HWND hwndParent )
{
    CSystemWorksPage cSystemWorks;

    cSystemWorks.DoModal( IDD_SYSTEM_WORKS,hwndParent );

    return( TRUE );
}

BOOL IsCorporateMode()
{
    auto    BOOL            bResult = FALSE;
    auto    DWORD           dwRegValueType = 0;
    auto    DWORD           dwDataBuf;
    auto    DWORD           dwSize;
    auto    HKEY            hKey = NULL;
    auto    long            lResult;


    // open the registry key
    lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,                      // handle of open key
                           "SOFTWARE\\Symantec\\Norton AntiVirus",  // address of name of subkey to open
                           0,                                       // reserved
                           (KEY_READ | KEY_WRITE),                  // security access mask
                           &hKey);                                  // address of handle of open key
    if (ERROR_SUCCESS != lResult)
        {
        return FALSE;
        }

    // get the value of "corporate features" (if it exists)
    dwSize = sizeof(DWORD);
    lResult = RegQueryValueEx(hKey              // handle of key to query
                    , "corporatefeatures"       // address of name of value to query
                    , 0                         // reserved (must be zero)
                    , &dwRegValueType           // address of buffer for value type
                    , (LPBYTE)&dwDataBuf        // address of data buffer
                    , &dwSize);                 // address of data buffer size

    if ((ERROR_SUCCESS != lResult)  ||  (REG_DWORD != dwRegValueType))
        {
        bResult = FALSE;
        }
    else
        {
        bResult = TRUE;
        }

    if (NULL != hKey)
        {
        RegCloseKey(hKey);
        }

    return bResult;
}

VOID GenerateSystemWorksKey( PHKEY pHKey,CslString& strSystemWorksKey )
{
        TCHAR szPath[ SYM_MAX_PATH ];
        // TCHAR szString[ 80 + 1 ],szFilename[ SYM_MAX_PATH ],szPath[ SYM_MAX_PATH ];
        LPTSTR pString;
        
        if( TRUE == IsCorporateMode() )
            *pHKey = HKEY_LOCAL_MACHINE;
        else
            *pHKey = HKEY_CURRENT_USER;

        CslString strExtName( IDS_COMPONENT_NAME );
        CslString strCompany( IDS_COMPANY_NAME );     
        CslString strProduct( NAV_IS_OEM() ? IDS_PRODUCT_NAME_OEM : IDS_PRODUCT_NAME );
        CslString strLanguage( IDS_LANGUAGE );

        GetModuleFileName( NULL,szPath,sizeof( szPath ) );

        if ( ( pString = strrchr( szPath,'\\' ) ) != NULL )
            *pString = 0;

//      SPRINTF( szFilename,"%s\\VERSION.DAT",szPath );
//      GetPrivateProfileString( "Versions","PublicVersion","5.01",szString,sizeof( szString ),szFilename );
        CslString strVersion ( IDS_VERSION );
//        SPRINTF( szFilename,"%s\\PARTNUM.DAT",szPath );
//        GetPrivateProfileString( "PartNumber","SKU","",szString,sizeof( szString ),szFilename );
//        CslString strPartNum ( szString );

        // Verify that the LoadStrings done by the CslString constructor succeeded:
//      ASSERT( !( strCompany.IsEmpty() || strProduct.IsEmpty() || strVersion.IsEmpty() || 
//                 strLanguage.IsEmpty() || strPartNum.IsEmpty() ) );

        CslString strFormat( IDS_FORMAT_REGKEY );
        ASSERT( !strFormat.IsEmpty() );
		strSystemWorksKey.Format( strFormat, 
            (LPCTSTR) strCompany, 
            (LPCTSTR) strProduct, 
            (LPCTSTR) strVersion,
            (LPCTSTR) strLanguage,
            (LPCTSTR) REGSTR_KEY_ABOUT,
            (LPCTSTR) strExtName );
}

LPTSTR WINAPI GetSystemWorksKey( PHKEY phKey,LPTSTR szKey )
{
        CslString strSystemWorksKey;

        GenerateSystemWorksKey( phKey,strSystemWorksKey );

        strcpy( szKey,( LPCTSTR ) strSystemWorksKey );

        return( szKey );
}

