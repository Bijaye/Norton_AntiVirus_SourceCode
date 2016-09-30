// Password.cpp: implementation of the CPassword class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NAVTrust.h"
#include "NAVPwd_h.h"
#include "NAVPwd_i.c"
#include "nuabout.h"
#include "Password.h"
#include "AVRESBranding.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPassword::CPassword()
{
    //
    // Create the password object
    //
    m_spNAVPass = NULL;

    // Need to fetch password protection enabled flag
    if( NAVToolbox::NAVTRUST_OK == NAVToolbox::IsSymantecComServer(CLSID_Password) )
    {
        if( SUCCEEDED(m_spNAVPass.CoCreateInstance(CLSID_Password, NULL, CLSCTX_INPROC)) )
        {
            // Set the information
			CBrandingRes BrandRes;
            CComBSTR bstrProductName(BrandRes.ProductName());
            CComBSTR bstrFeatureName;
            bstrFeatureName.LoadString(IDS_FEATURE_NAME);

            m_spNAVPass->put_ProductID(NAV_CONSUMER_PASSWORD_ID);
            m_spNAVPass->put_ProductName(bstrProductName);
            m_spNAVPass->put_FeatureName(bstrFeatureName);
        }
    }
}

CPassword::~CPassword()
{

}

bool CPassword::ResetPassword()
{
    if (m_spNAVPass)
    {
        BOOL bSucceeded = FALSE;

        if( SUCCEEDED(m_spNAVPass->Reset(&bSucceeded)) )
        {
            return( bSucceeded ? true : false );
        }
    }

    // If the password object couldn't be created then this failed to reset it!
    return false;
}

bool CPassword::IsPasswordCheckEnabled()
{
    if (m_spNAVPass)
    {
        BOOL bEnabled = FALSE;

        // Check for password if necessary
        if( SUCCEEDED(m_spNAVPass->get_Enabled(&bEnabled)) )
        {
            return( bEnabled ? true : false );
        }
    }

    // If the password object couldn't be created then 
    // assume not set
    return false;
}

bool CPassword::CheckPassword ()
{
    if( IsPasswordCheckEnabled() )
    {
        if (m_spNAVPass)
        {
            BOOL bSucceeded = FALSE;

            if( SUCCEEDED(m_spNAVPass->Check(&bSucceeded)) )
            {
                return( bSucceeded ? true : false );
            }
        }
    }

    // If the password object couldn't be created then 
    // assume not set
    return true;
}
