#ifndef AVPEPCOMPONENT_H
#define AVPEPCOMPONENT_H
#pragma message("****************************CompPephelper")
#pragma once

#include "stdafx.h"

#ifdef _PEP_IMPL_
// For defining
//#define _CSIMONV2AUTHORIZATION
#define _SIMON_PEP_
#define _V2AUTHORIZATION_SIMON          
#define INIT_V2AUTHORIZATION_IMPL
#define INIT_RSA_PUBLIC_KEY
#define INIT_RSA_PRIVATE_KEY
#define _PEPNOTIFY_V2AUTHMAP_IMPL
#endif //PEP_IMPL

// generic PEP header
#include "PolicyEnforcement_Simon.h"

#include "V2Authorization.h"
#include "V2LicensingAuthGuids.h"

// CLT Pep constants
#include "cltPepConstants.h"

// Licensing constants
#include "cltLicenseConstants.h"
#include "DRMNamedProperties.h"

// Subscription constants
#include "Subscription_static.h"

// Digital signature checking implementation
#include "DRMTrustHelper.h"

//============================================================================================
// PEP V2 Authorization MAP
//============================================================================================
#ifndef PRODUCTPEPHELPER_H
	#ifdef _DRM_V2AUTHMAP_IMPL
#pragma message("AV Component V2 MAP")
    AUTHORIZATION_MAP_BEGIN(DRMPEPSinkMustHaveThisMap)
        AUTHORIZATION_MAP_ENTRY(V2LicensingAuthGuids::GUID_AntiVirus_Component)
	AUTHORIZATION_MAP_END2(DRMPEPSinkMustHaveThisMap)
	#else // !_DRM_V2AUTHMAP_IMPL
		AUTHORIZATION_MAP_EXTERN(DRMPEPSinkMustHaveThisMap)
	#endif // _DRM_V2AUTHMAP_IMPL
#endif // !PRODUCTPEPHELPER_H

#ifdef _AV_COMPONENT_CLIENT
    static const TCHAR *SZ_PRODUCT_SUITEOWNER_GUID = V2LicensingAuthGuids::SZ_ANTIVIRUS_COMPONENT;
#endif // _AV_COMPONENT_CLIENT
	
namespace AntiVirusComponent
{
    /////////////////////////////////////////////////////////////////////////////////////////
    // Policy IDs for AntiVirusComponent specific events
    /////////////////////////////////////////////////////////////////////////////////////////
    const DWORD POLICY_ID_PAID_CONTENT_VDEF_UPDATES                    = 0x00000001;
}//namespace AntiVirusComponent

#endif // AVPEPCOMPONENT_H