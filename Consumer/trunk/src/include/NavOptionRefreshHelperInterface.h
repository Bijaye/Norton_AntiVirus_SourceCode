////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// NavOptionRefreshHelperInterface.h
//
//////////////////////////////////////////////////////////////////////

#if !defined(_NAVOPTIONREFRESHHELPERINTERFACE_H__78B1E01B_3239_4c7f_8865_4B953B112656__INCLUDED_)
#define _NAVOPTIONREFRESHHELPERINTERFACE_H__78B1E01B_3239_4c7f_8865_4B953B112656__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//====================================================================================================
//
// IID's of Interfaces
//
//====================================================================================================

#define REFRESH_FORCE_COMMIT        (0x00000001)
#define REFRESH_COMMIT_ON_CHANGE    (0x00000002)
#define REFRESH_EVENT_CHANGE        (0x00000004)
#define REFRESH_UPDATE_AP           (0x00000008)
#define REFRESH_SILENT           	(0x00000010)
#define REFRESH_FORCE_FULLY                 (REFRESH_UPDATE_AP|REFRESH_EVENT_CHANGE|REFRESH_FORCE_COMMIT)
#define REFRESH_COMMIT_ON_CHANGE_FULLY      (REFRESH_UPDATE_AP|REFRESH_EVENT_CHANGE|REFRESH_COMMIT_ON_CHANGE)


/////////////////////////////////////////////////////////
// INAVOptionRefresh
class INAVOptionRefresh: public SIMON::IInterfaceManagement
{
public:
    SIMON_STDMETHOD(Refresh)(DWORD) = 0;
};

// {16DE16AE-8A72-4fd9-AEE9-D4F9DE059F89}
DEFINE_SIMON_GUID(IID_INAVOptionRefresh, 0x16de16ae, 0x8a72, 0x4fd9, 0xae, 0xe9, 0xd4, 0xf9, 0xde, 0x5, 0x9f, 0x89);

//====================================================================================================
//
// CLSID's of Objects
//
//====================================================================================================

// {1BFCB47D-AEE9-45f8-8387-F4C0CF098802}
DEFINE_SIMON_GUID(CLSID_CNAVOptionRefresh, 0x1bfcb47d, 0xaee9, 0x45f8, 0x83, 0x87, 0xf4, 0xc0, 0xcf, 0x9, 0x88, 0x2);


#endif // !defined(_NAVOPTIONREFRESHHELPERINTERFACE_H__78B1E01B_3239_4c7f_8865_4B953B112656__INCLUDED_)
