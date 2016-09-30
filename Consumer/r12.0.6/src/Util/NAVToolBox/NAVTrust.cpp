#include "stdafx.h"
#include <atlbase.h>
#include "NAVTrust.h"

namespace NAVToolbox
{
// This allows us to use only one instance of IVerifyTrust module regardless of
// how the CNAVTrust gets scoped. Can also be used for more direct acces to the
// CommonClient trust stuff.
//
class CNAVTrust
{
public:
    CNAVTrust();
    virtual ~CNAVTrust();

    bool GetVerifyTrust ( ccVerifyTrust::IVerifyTrustQIPtr& pTrust );

    static NAVTRUSTSTATUS TranslateCCResultToNAV (ccVerifyTrust::ErrorType ccErrorType);

private:
    bool init ();

    // Do not reorder these two variables! - See defect # 422838.
    //
    ccVerifyTrust::ccVerifyTrust_ICachedVerifyTrust m_VerifyTrustLoader; 
    ccVerifyTrust::IVerifyTrustQIPtr m_pVerifyTrust;
    ccVerifyTrust::ICachedVerifyTrustPtr m_pCachedTrust;

    // ---------
}; // end CNAVTrust

// global inits
bool g_bCurrentProcessSafe;

CNAVTrust::CNAVTrust()
{
    m_pVerifyTrust = NULL;
    g_bCurrentProcessSafe = false;
}

bool CNAVTrust::GetVerifyTrust ( ccVerifyTrust::IVerifyTrustQIPtr& pTrust )
{
    if ( init () && m_pVerifyTrust )
    {
        pTrust = m_pVerifyTrust; 
        return true;
    }
    else
        return false;
}


bool CNAVTrust::init ()
{
    if ( m_pVerifyTrust == NULL )
    {
        SYMRESULT symRes = m_VerifyTrustLoader.CreateObject ( &m_pCachedTrust );

        if (SYM_FAILED(symRes) ||
            m_pCachedTrust == NULL)
        {
            //TraceError(_T("NAVToolbox::CNAVTrust - Failed to create the Trust Module."));
            return false;
        }
        else
        {
            m_pVerifyTrust = m_pCachedTrust;
            if ( m_pVerifyTrust )
            {
                if (m_pVerifyTrust->Create(true) != ccVerifyTrust::eNoError)
			    {
                    m_pVerifyTrust->Destroy();    
                    m_pVerifyTrust.Release();
                    //TraceError(_T("NAVToolbox::CNAVTrust - Failed on call to create."));
                    return false;
                }

                m_pCachedTrust->SetCacheEnabled(true); // use per-process trust cache
            }
            else
                return false;
        }
    }

    return true;
};

CNAVTrust::~CNAVTrust()
{
    if (m_pVerifyTrust != NULL)
    {
        m_pVerifyTrust->Destroy();    
        m_pVerifyTrust.Release();
    }
};

NAVTRUSTSTATUS CNAVTrust::TranslateCCResultToNAV (ccVerifyTrust::ErrorType ccErrorType)
{
    if( ccErrorType == ccVerifyTrust::eVerifyError )
        return NAVTRUST_ERROR_NOT_SIGNED;
    else
        return NAVTRUST_OK;
    /*
    switch ( ccErrorType )
    {
    case cc::eNoError:
        return NAVTRUST_OK;
        break;
    case cc::eVerifyError:
        return NAVTRUST_ERROR_NOT_SIGNED;
        break;
    case cc::eTrustSystemError:
        return NAVTRUST_ERROR_NO_TRUST_VERIFIER;
        break;
    case cc::eBadParameterError:
        return NAVTRUST_ERROR_INVALID_PARAM;
        break;
    case cc::eOutOfMemoryError:
        return NAVTRUST_ERROR_OUTOFMEMORY;
        break;
    case cc::eUnknownError:
    case cc::eFatalError:
    default:
        return NAVTRUST_ERROR_UNKNOWN;
        break;
    }
    */
}

///////////////////////////////////////////////////////////////////////////////
//
// IsSymantecComServer - Verifies that a Com Server is signed by Symantec 
// using the ProgID of the server
//
///////////////////////////////////////////////////////////////////////////////
NAVTRUSTSTATUS IsSymantecComServer(LPCSTR lpszProgID)
{
    CComBSTR bstrProgID(lpszProgID);

    // Make sure we have something for the ProgID
    if( 0 == bstrProgID.Length() )
        return NAVTRUST_ERROR_INVALID_PARAM;

    CLSID clsid; 

    // Get the CLSID and call the CLSID version of this function    
    //
    if( FAILED(::CLSIDFromProgID(bstrProgID, &clsid)) )
        return NAVTRUST_ERROR_INVALID_PARAM;

    return IsSymantecComServer(clsid);
}

NAVTRUSTSTATUS IsSymantecComServer(REFCLSID rclsid)
{
    CNAVTrust trustTemp;
    ccVerifyTrust::IVerifyTrustQIPtr pTrust;
    if ( !trustTemp.GetVerifyTrust ( pTrust ))
        return NAVTRUST_ERROR_NO_TRUST_VERIFIER;

    return CNAVTrust::TranslateCCResultToNAV( pTrust->VerifyCLSID( rclsid, ccVerifyTrust::eSymantecSignature));
}

NAVTRUSTSTATUS IsSymantecSignedImage(LPCSTR lpcFileName)
{
    CNAVTrust trustTemp;
    ccVerifyTrust::IVerifyTrustQIPtr pTrust;
    if ( !trustTemp.GetVerifyTrust ( pTrust ))
        return NAVTRUST_ERROR_NO_TRUST_VERIFIER;

    return CNAVTrust::TranslateCCResultToNAV( pTrust->VerifyFile( lpcFileName, ccVerifyTrust::eSymantecSignature));
}

NAVTRUSTSTATUS IsSymantecSignedImage(LPWSTR lpcFileName)
{
    CNAVTrust trustTemp;
    ccVerifyTrust::IVerifyTrustQIPtr pTrust;
    if ( !trustTemp.GetVerifyTrust ( pTrust ))
        return NAVTRUST_ERROR_NO_TRUST_VERIFIER;

    return CNAVTrust::TranslateCCResultToNAV( pTrust->VerifyFile( lpcFileName, ccVerifyTrust::eSymantecSignature));
}

NAVTRUSTSTATUS IsSymantecApplication()
{
    // Cache the result of this call if it's SAFE. Since the calling process
    // can never change we can get some performance here.
    //
    if ( !g_bCurrentProcessSafe )
    {
        CNAVTrust trustTemp;
        ccVerifyTrust::IVerifyTrustQIPtr pTrust;
        if ( !trustTemp.GetVerifyTrust ( pTrust ))
            return NAVTRUST_ERROR_NO_TRUST_VERIFIER;

        NAVTRUSTSTATUS result = CNAVTrust::TranslateCCResultToNAV( pTrust->VerifyCurrentProcess( ccVerifyTrust::eSymantecSignature));

        if ( NAVTRUST_OK == result )
            g_bCurrentProcessSafe = true;

        return result;
    }
    else
        return NAVTRUST_OK;
}
}   // end namespace NAVToolbox