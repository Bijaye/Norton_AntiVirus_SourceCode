// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// DCOMObject.h: interface for the CDCOMObject class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DCOMOBJECT_H__0B26CF37_A760_11D2_8E6D_281ED2000000__INCLUDED_)
#define AFX_DCOMOBJECT_H__0B26CF37_A760_11D2_8E6D_281ED2000000__INCLUDED_

#ifndef _WIN32_DCOM
#pragma message("The _WIN32_DCOM preprocessor directive must be defined for all files")
#endif

#include <new>

#pragma warning( disable:4786 )
#include <comdef.h>
#include <tchar.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

template < class T, class TInterfaceFacePtr > class CDCOMObject : public TInterfaceFacePtr
{
public:
    ///////////////////////////////////////////////////////////////////////////////
    //
    // Function name: CDCOMObject::CDCOMObject
    //
    // Description  : Constructor
    //
    ///////////////////////////////////////////////////////////////////////////////
    // 1/8/99 - DBUCHES: Function created / header added
    ///////////////////////////////////////////////////////////////////////////////
    CDCOMObject()
    {
        //
        // Set default values
        //
        DCO_Initialize();
    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    // Function name: CDCOMObject::CDCOMObject
    //
    // Description  : This routine will attempt to create a COM object on a
    //                remote machine.
    //
    // Argument     : PCTSTR pszServer
    // Argument     : LPCTSTR pszUserName = NULL
    // Argument     : LPCTSTR pszPassword = NULL
    // Argument     : IUnknown* pOuter = NULL
    // Argument     : DWORD dwClsContext = CLSCTX_ALL
    //
    ///////////////////////////////////////////////////////////////////////////////
    // 1/8/99 - DBUCHES: Function created / header added
    ///////////////////////////////////////////////////////////////////////////////
    CDCOMObject(
            LPCTSTR pszServer,
            LPCTSTR pszUserName = NULL,
            LPCTSTR pszPassword = NULL,
            LPCTSTR pszDomainName = NULL
            ) throw()
    {
        //
        // Set default values
        //
        DCO_Initialize();

        // 
        // Save off server name
        // 
        DCO_SetServer( pszServer );

        // 
        // If we are running remotely, save off user identity.
        // 
        if( !m_bLocal )
            {
            //
            // Set user identity
            //
            DCO_SetIdentity( pszUserName, pszPassword, pszDomainName );
            }

        // 
        // Create the object.
        // 
        HRESULT hr = DCO_CreateObject();
        if (FAILED(hr) && (hr != E_NOINTERFACE))
            _com_issue_error(hr);
    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    // Function name : DCO_SetServer
    //
    // Description   : This routine will set the server name and determine if the 
    //                 server is running on a machine other than this one.
    //
    // Return type   : void 
    //
    // Argument      : LPCTSTR pszServer
    //
    ///////////////////////////////////////////////////////////////////////////////
    // 2/1/99 - DBUCHES: Function created / header added 
    ///////////////////////////////////////////////////////////////////////////////
    void DCO_SetServer( LPCTSTR pszServer )
    {
        //
        // Q: Are running locally?
        //
        if( pszServer == NULL )
            {
            m_bLocal = TRUE;
            return;
            }
        else
            {
            m_bLocal = FALSE;
            TCHAR szMachineName[MAX_COMPUTERNAME_LENGTH + 1];
            DWORD dwBufSize = MAX_COMPUTERNAME_LENGTH + 1;
            if( GetComputerName(szMachineName, &dwBufSize) != 0 )
                {
                //
                // Skip leading backslashes
                //
                TCHAR sNew[MAX_COMPUTERNAME_LENGTH + 1];
                LPTSTR pStr = (LPTSTR)sNew;
                _tcsncpy( sNew, pszServer, MAX_COMPUTERNAME_LENGTH );
                while (*pStr == _T('\\') )
                    ++pStr;

                if ( _tcsicmp(pStr, szMachineName) == 0 )
                    {
                    m_bLocal = TRUE;
                    return;
                    }
                }
            }

        // 
        // Delete old server info
        // 
        if( m_sServerName )
            {
            delete [] m_sServerName;
            m_sServerName = NULL;
            }

		try
		{
			//
			// Convert servername.
			//
			int iLength = _tcslen( pszServer ) + 1;
			m_sServerName = new WCHAR [ iLength ];
		}
		catch (std::bad_alloc &){}
#ifdef UNICODE
        _tcscpy( m_sServerName, pszServer );
#else
        MultiByteToWideChar( CP_ACP, 0, pszServer, -1, m_sServerName, iLength );
#endif
    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    // Function name: CDCOMObject::SetIdentity
    //
    // Description  : This function allows users to specify the access identity
    //                on the remote machine for object creation.
    //
    // Return type  : void
    //
    // Argument     : LPCTSTR pszUserName
    // Argument     : LPCTSTR pszPassword
    // Argument     : LPCTSTR pszDomain = NULL
    //
    ///////////////////////////////////////////////////////////////////////////////
    // 1/9/99 - DBUCHES: Function created / header added
    ///////////////////////////////////////////////////////////////////////////////
    void DCO_SetIdentity(
        LPCTSTR pszUserName,
        LPCTSTR pszPassword,
        LPCTSTR pszDomain = NULL
        ) throw()
        {
        //
        // Delete old user info
        //
        DCO_Cleanup();

		try
		{
        //
        // Setup user identity
        //
        if( pszUserName == NULL || pszPassword == NULL )
            return;
#ifdef UNICODE
#define AUTHCAST unsigned short *
#else
#define AUTHCAST unsigned char *
#endif
        m_pIdentity = new SEC_WINNT_AUTH_IDENTITY;
        ZeroMemory( m_pIdentity, sizeof( SEC_WINNT_AUTH_IDENTITY ) );

        m_pIdentity->UserLength = _tcslen( pszUserName );
        m_pszUserName = new TCHAR[ m_pIdentity->UserLength + 1 ];
        _tcscpy( m_pszUserName, pszUserName );
        m_pIdentity->User = (AUTHCAST) m_pszUserName;

        m_pIdentity->PasswordLength = _tcslen( pszPassword );
        m_pszPassword = new TCHAR[ m_pIdentity->PasswordLength + 1 ];
        _tcscpy( m_pszPassword, pszPassword );
        m_pIdentity->Password = (AUTHCAST) m_pszPassword;

        //
        // If no domain specified, set to single period.
        //
        if( pszDomain == NULL )
            m_pIdentity->DomainLength = 1;
        else
            m_pIdentity->DomainLength = _tcslen( pszDomain );
        m_pszDomain = new TCHAR[ m_pIdentity->DomainLength + 1 ];
        _tcscpy( m_pszDomain, pszDomain == NULL ? _T(".") : pszDomain );
        m_pIdentity->Domain = (AUTHCAST) m_pszDomain;

        if (sizeof(TCHAR) > 1)
            m_pIdentity->Flags = SEC_WINNT_AUTH_IDENTITY_UNICODE;
        else
            m_pIdentity->Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
		}
		catch (std::bad_alloc &){}


    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    // Function name: CDCOMObject::~CDCOMObject
    //
    // Description  : Destructor
    //
    ///////////////////////////////////////////////////////////////////////////////
    // 1/8/99 - DBUCHES: Function created / header added
    ///////////////////////////////////////////////////////////////////////////////
    ~CDCOMObject()
    {
        //
        // Cleanup
        //
        DCO_Cleanup();

        if( m_sServerName )
            {
            delete [] m_sServerName;
            m_sServerName = NULL;
            }
    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    // Function name: Cleanup
    //
    // Description  : Free memory allocated by identity structures.
    //
    // Return type  : void
    //
    //
    ///////////////////////////////////////////////////////////////////////////////
    // 1/9/99 - DBUCHES: Function created / header added
    ///////////////////////////////////////////////////////////////////////////////
    void DCO_Cleanup()
    {

		try
		{
        if( m_pszUserName )
            {
            delete [] m_pszUserName;
            m_pszUserName = NULL;
            }

        if( m_pszPassword )
            {
            delete [] m_pszPassword;
            m_pszPassword = NULL;
            }

        if( m_pszDomain )
            {
            delete [] m_pszDomain;
            m_pszDomain = NULL;
            }

        if( m_pIdentity )
            {
            delete m_pIdentity;
            m_pIdentity = NULL;
            }
		}
		catch (std::bad_alloc &)
		{
		}

    }

    ///////////////////////////////////////////////////////////////////////////////
    //
    // Function name: CDCOMObject::DCO_Initialize
    //
    // Description  : This routine sets default values for the object.
    //
    // Return type  : void
    //
    //
    ///////////////////////////////////////////////////////////////////////////////
    // 1/8/99 - DBUCHES: Function created / header added
    ///////////////////////////////////////////////////////////////////////////////
    void DCO_Initialize()
    {
        //
        // Set some reasonable defaults
        //
        m_sServerName = NULL;
        m_pszUserName = NULL;
        m_pszPassword = NULL;
        m_pszDomain = NULL;
        m_pIdentity = NULL;
        m_bLocal = FALSE;

        DCO_SetSecurityAttributes();
    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    // Function name: CDCOMObject::DCO_CreateObject
    //
    // Description  : This routine actually attempts creation of the object in
    //                question.
    //
    // Return type  : HRESULT
    //
    // Argument     : onst CLSID& rclsid
    // Argument     : IUnknown* pOuter = NULL
    // Argument     : DWORD dwClsContext = CLSCTX_ALL
    //
    ///////////////////////////////////////////////////////////////////////////////
    // 1/8/99 - DBUCHES: Function created / header added
    ///////////////////////////////////////////////////////////////////////////////
    virtual HRESULT DCO_CreateObject(
            const CLSID& rclsid = __uuidof(T),
            IUnknown* pOuter = NULL,
            DWORD dwClsContext = CLSCTX_ALL
            )
    {
        HRESULT hr;

        //
        // If we are running locally, just create the object and return.
        //
        if( m_bLocal )
            {
            return CoCreateInstance( rclsid, pOuter, dwClsContext, GetIID(), reinterpret_cast<void**>( &(*this) ) );
            }

        //
        // Set up NT security stuff.
        //
        COSERVERINFO serverInfo;
        COAUTHINFO   authInfo;
        ZeroMemory( &serverInfo, sizeof( COSERVERINFO ) );
        ZeroMemory( &authInfo, sizeof( COAUTHINFO ) );
        serverInfo.pwszName = m_sServerName;
        serverInfo.pAuthInfo= &authInfo;

        authInfo.dwAuthnSvc          = m_dwAuthnService;
        authInfo.dwAuthzSvc          = m_dwAuthzService;
        authInfo.dwAuthnLevel        = m_dwAuthnLevel;
        authInfo.dwImpersonationLevel= m_dwImpLevel;
        authInfo.pAuthIdentityData   = ( _COAUTHIDENTITY* ) m_pIdentity;
        authInfo.dwCapabilities      = EOAC_NONE;

        //
        // Create object on remote system
        //
        MULTI_QI mQI = { &__uuidof( TInterfaceFacePtr ), NULL, 0 };
        hr = CoCreateInstanceEx( rclsid, pOuter, dwClsContext, &serverInfo, 1, &mQI );
        if (FAILED(hr) || FAILED(mQI.hr))
            {
            return hr;
            }

        //
        // Set security for all calls to this interface.
        //
        hr = DCO_SetProxyBlanket( mQI.pItf );
        if( FAILED( hr ) )
            return hr;

        //
        // Query for specified interface.
        //
        hr = mQI.pItf->QueryInterface( GetIID(), reinterpret_cast<void**>( &(*this) ) );
        if( SUCCEEDED( hr ) )
            mQI.pItf->Release();

        return hr;
    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    // Function name: CDCOMObject::_Release
    //
    // Description  : Release a reference to this object
    //
    // Return type  : void
    //
    //
    ///////////////////////////////////////////////////////////////////////////////
    // 1/8/99 - DBUCHES: Function created / header added
    ///////////////////////////////////////////////////////////////////////////////
    void _Release() throw()
    {
        if (((Interface*)(*this)) != NULL)
            ((Interface*)(*this))->Release();
    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    // Function name: CDCOMObject::DCO_SetSecurityAttributes
    //
    // Description  : This function sets up the various security attributes
    //                needed in order to start a DCOM conversation.  Override this
    //                routine to change these values.
    //
    // Return type  : virtual void
    //
    ///////////////////////////////////////////////////////////////////////////////
    // 1/8/99 - DBUCHES: Function created / header added
    ///////////////////////////////////////////////////////////////////////////////
    void DCO_SetSecurityAttributes(
        DWORD dwAuthnLevel      = RPC_C_AUTHN_LEVEL_CONNECT,
        DWORD dwImpLevel        = RPC_C_IMP_LEVEL_IMPERSONATE,
        DWORD dwAuthnService    = RPC_C_AUTHN_WINNT,
        DWORD dwAuthzService    = RPC_C_AUTHZ_NONE
        )
    {
        //
        // Set up security values.
        //
        m_dwAuthnService    = dwAuthnService;
        m_dwAuthzService    = dwAuthzService;
        m_dwAuthnLevel      = dwAuthnLevel;
        m_dwImpLevel        = dwImpLevel;
    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    // Function name: CDCOMObject::DCO_SetProxyBlanket
    //
    // Description  : This routine sets the sets the security blanket for the
    //                specified interface.  Note that this routine is called for
    //                TInterfaceFacePtr, if the object returns other interfaces,
    //                (ie. an Enum() method that returns an IEnumXXX derivative )
    //                this routine will have to be called for this interface.
    //
    // Return type  : void
    //
    // Argument     : IUnknown* pUnk
    //
    ///////////////////////////////////////////////////////////////////////////////
    // 1/8/99 - DBUCHES: Function created / header added
    ///////////////////////////////////////////////////////////////////////////////
    HRESULT DCO_SetProxyBlanket( IUnknown* pUnk )
    {
        //
        // Validate input.
        //
        if (NULL == pUnk)
            return E_INVALIDARG;

        //
        // Set security blanket info for this object
        //
        return CoSetProxyBlanket( pUnk,
                                m_dwAuthnService,
                                m_dwAuthzService,
                                NULL,
                                m_dwAuthnLevel,
                                m_dwImpLevel,
                                (_COAUTHIDENTITY*) m_pIdentity,
                                EOAC_NONE);
    }


    ///////////////////////////////////////////////////////////////////////////////
    //
    // Function name : DCO_GetIdentity
    //
    // Description   : This routine will return the identity structure used by
    //                 this object for calls to CoSetProxyBlanket().
    //
    // Return type   : const SEC_WINNT_AUTH_IDENTITY* 
    //
    //
    ///////////////////////////////////////////////////////////////////////////////
    // 3/9/99 - DBUCHES: Function created / header added 
    ///////////////////////////////////////////////////////////////////////////////
    SEC_WINNT_AUTH_IDENTITY* DCO_GetIdentity()
        {
        return m_pIdentity; 
        }



//
// Data.
//
protected:
    //
    // Server information
    //
    LPWSTR      m_sServerName;
    BOOL        m_bLocal;

    //
    // User information.
    //
    LPTSTR      m_pszUserName;
    LPTSTR      m_pszPassword;
    LPTSTR      m_pszDomain;

    //
    // NT Security specific stuff.
    //
    DWORD       m_dwAuthnService;
    DWORD       m_dwAuthzService;
    DWORD       m_dwAuthnLevel;
    DWORD       m_dwImpLevel;
    SEC_WINNT_AUTH_IDENTITY *m_pIdentity;

};


#define DCOM_Object(Itfc) CDCOMObject<Itfc, I##Itfc##Ptr>

#endif // !defined(AFX_DCOMOBJECT_H__0B26CF37_A760_11D2_8E6D_281ED2000000__INCLUDED_)
