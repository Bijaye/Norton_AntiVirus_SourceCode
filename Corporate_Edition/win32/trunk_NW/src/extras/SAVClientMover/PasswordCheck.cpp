// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// PasswordCheck.cpp: implementation of the CPasswordCheck class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <afxtempl.h>
#include "SAVClientMover.h"
#include "PasswordCheck.h"
//#include "LDVPDefs.h"			// defines the PASS_KEY#
#include "vpcommon.h"
#include "SavHelper.h"
#include "DomainPasswordDlg.h"
#include "Password.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPasswordCheck::CPasswordCheck()
{

}

CPasswordCheck::~CPasswordCheck()
{
    m_CachedPasswords.RemoveAll( );
}

HRESULT CPasswordCheck::GetPassword( const CString &csServerName )
{
    HRESULT hResult = ERROR_NOT_FOUND;

    char szRemotePassword[128];

    // first get the password from the remote machine - I ignore the
    // locally cached password - that is too easily spoofed.

    DWORD dwRet = GetRemoteRegValue( csServerName, "", "ConsolePassword", (LPBYTE)&szRemotePassword, sizeof(szRemotePassword), 3 );
    if ( dwRet != ERROR_SUCCESS )
    {
        CString csError;

        csError.Format( IDS_GET_PASSWORD_ERROR, csServerName );
        dwRet = AfxMessageBox( csError, MB_ICONEXCLAMATION );
    }
    else
    {
        // first check the existing list

        int nCount = m_CachedPasswords.GetSize( );
        hResult = ERROR_NOT_FOUND;

        for ( int i=0; i<nCount; i++ )
        {
            if ( szRemotePassword == m_CachedPasswords[i] )
            {
                hResult = ERROR_SUCCESS;

                break;
            }
        }

        if ( hResult != ERROR_SUCCESS )
        {
            do
            {
                // password not found, ask the user

                CDomainPasswordDlg cDlg( NULL, csServerName );
                dwRet = cDlg.DoModal( );

                if ( dwRet == IDOK )
                {
                    char szMangledPassword[PASS_MAX_CIPHER_TEXT_BYTES];

                    MakeEP( szMangledPassword, sizeof(szMangledPassword), PASS_KEY1, PASS_KEY2, cDlg.m_csPassword, cDlg.m_csPassword.GetLength()+1 );

                    if ( !strcmp( szMangledPassword, szRemotePassword ) )
                    {
                        // got a good one - cache it and leave

                        m_CachedPasswords.Add( CString(szMangledPassword) );
                        hResult = ERROR_SUCCESS;
                    }
                    else
                    {
                        dwRet = AfxMessageBox( IDS_BAD_PASSWORD, MB_ICONEXCLAMATION | MB_OKCANCEL );
                    }
                }
            } while ( dwRet == IDOK && hResult != ERROR_SUCCESS );
        }
    }

	return hResult;
}
