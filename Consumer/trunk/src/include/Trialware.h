////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Trialware.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_TRIALWARE_H__114EAD62_0CDE_4394_88F4_CE1468A9D1C1__INCLUDED_)
#define _TRIALWARE_H__114EAD62_0CDE_4394_88F4_CE1468A9D1C1__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


typedef enum _TrialWareType
{
    TWT_NagAlways = 1
        ,TWT_NagOnlyIfExpired
        ,TWT_NagNever
}TRIALWARETYPE;


BOOL IsTrialValid(TRIALWARETYPE twt = TWT_NagAlways);


#endif // !defined(_TRIALWARE_H__114EAD62_0CDE_4394_88F4_CE1468A9D1C1__INCLUDED_)
