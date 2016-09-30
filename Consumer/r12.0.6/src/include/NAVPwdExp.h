// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
#ifndef _CCPASSWDEXP_H_
#define _CCPASSWDEXP_H_

#include <ole2.h>

#ifdef CCPASSWD_EXPORTS
#define CCPASSWD_API __declspec(dllexport)
#else
#define CCPASSWD_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define CCPW_FAIL				MAKE_HRESULT(1, FACILITY_ITF, 0x10)
#define CCPW_REG_ERROR			MAKE_HRESULT(1, FACILITY_ITF, 0x11)
#define CCPW_INVALID_PASSWORD	MAKE_HRESULT(1, FACILITY_ITF, 0x12)
#define CCPW_USER_CANCEL		MAKE_HRESULT(1, FACILITY_ITF, 0x13)

CCPASSWD_API HRESULT ccpw_SetPassword(long lProductID, LPCTSTR szProductName, LPCTSTR szFeatureName, LPCTSTR szUserName, BOOL bPromptForOldPwd);
CCPASSWD_API HRESULT ccpw_CheckPassword(long lProductID, LPCTSTR szProductName, LPCTSTR szFeatureName, LPCTSTR szUserName);
CCPASSWD_API HRESULT ccpw_CheckPasswordSilent(long lProductID, LPCTSTR szUserName, LPCTSTR szPassword);
CCPASSWD_API HRESULT ccpw_ResetPassword(long lProductID, LPCTSTR szProductName, LPCTSTR szFeatureName, LPCTSTR szUserName);
CCPASSWD_API HRESULT ccpw_ClearPassword(long lProductID, LPCTSTR szProductName, LPCTSTR szFeatureName, LPCTSTR szUserName);
CCPASSWD_API HRESULT ccpw_AddUserAndPassword(long lProductID, LPCTSTR szUserName, LPCTSTR szPassword);
CCPASSWD_API BOOL	 ccpw_IsPasswordCheckEnabled(long lProductID); 
CCPASSWD_API HRESULT ccpw_SetPasswordCheckEnabled(long lProductID, BOOL bEnabled);
CCPASSWD_API BOOL	 ccpw_IsUserPasswordSet(long lProductID, LPCTSTR szUserName);

/////////////////////////////////////////////////////////////////////////
//	Password Module IDs

enum CC_PASSWORD_IDs
{
    NAV_CONSUMER_PASSWORD_ID = 1,
	NIS_CONSUMER_PASSWORD_ID = 2
};

/////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif

#endif