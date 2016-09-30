////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NAVBusinessRules.h
// 
//////////////////////////////////////////////////////////////////////
#if !defined(_NAVBUSINESSRULES_H__2BF51FC3_9F36_41ad_A20A_1DDC9E23E2A9__INCLUDED_)
#define _NAVBUSINESSRULES_H__2BF51FC3_9F36_41ad_A20A_1DDC9E23E2A9__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#include "SIMON_Interfaces.h"

// Forward define
class CNAVOptSettingsCache;

namespace NAV
{
//====================================================================================================
//
// IID's of Interfaces
//
//====================================================================================================
/////////////////////////////////////////////////////////
// ICalc
class ICalc: public SIMON::IInterfaceManagement
{
public:
    SIMON_STDMETHOD(Calc)(CNAVOptSettingsCache* pOptFile) = 0;
};

// {7A13AC5B-DFBB-44b3-98DA-A8A2AA4E6AA5}
DEFINE_SIMON_GUID(IID_ICalc, 0x7a13ac5b, 0xdfbb, 0x44b3, 0x98, 0xda, 0xa8, 0xa2, 0xaa, 0x4e, 0x6a, 0xa5);

//====================================================================================================
//
// CLSID's of Objects
//
//====================================================================================================

// {3F4BA980-FD10-4436-92B7-FBA581B324DC}
DEFINE_SIMON_GUID(CLSID_CNAVBusinessRules, 0x3f4ba980, 0xfd10, 0x4436, 0x92, 0xb7, 0xfb, 0xa5, 0x81, 0xb3, 0x24, 0xdc);

}//namespace NAV


#endif //_NAVBUSINESSRULES_H__2BF51FC3_9F36_41ad_A20A_1DDC9E23E2A9__INCLUDED_