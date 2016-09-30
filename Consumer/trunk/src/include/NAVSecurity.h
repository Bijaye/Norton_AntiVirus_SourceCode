////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVSecurity.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_NAVSECURITY_H__B46E1EE0_930D_4cfd_8A77_08EC1D25C81F__INCLUDED_)
#define _NAVSECURITY_H__B46E1EE0_930D_4cfd_8A77_08EC1D25C81F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "NAVTrust.h"


#include "cltsecurityValidation.h"

namespace NAV_Security
{
    inline void SecurityValidation(LPCTSTR lpcDLL , DWORD dwFlags)
    {
#define _NO_DIGISIG
#if defined(_DEBUG)
#define _NO_DIGISIG
#endif

#if !defined(_NO_DIGISIG)
        StahlSoft::HRX hrx;
        HRESULT hr = S_OK;

        ///////////////////////////////////////////////////////////////////////
        // VALIDATE_DigitalSignature
        if( (dwFlags&VALIDATE_DigitalSignature) == VALIDATE_DigitalSignature)
        {
            //  Validate signature of the licensing library.
            //  If the file signature is not valid, treat same as ValidateBoundFile failed.
            // Check LUALL.exe for a valid Symantec signature before launching it
//            hr =  ((NAVToolbox::NAVTRUST_OK != NAVToolbox::IsSymantecSignedImage(lpcDLL)) ? E_FAIL : S_OK);

            //            hr = DRMToolbox::DRM_IsSymantecSignedImage(lpcLicensingDLL);
            hrx << hr;
        }
#endif// _NO_DIGISIG
    }
}//namespace NAV_Security

#endif // !defined(_NAVSECURITY_H__B46E1EE0_930D_4cfd_8A77_08EC1D25C81F__INCLUDED_)
