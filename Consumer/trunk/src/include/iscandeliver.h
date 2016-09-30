////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/IScanDeliver.h_v   1.1   13 Apr 1998 15:59:54   SEDWARD  $
////////////////////////////////////////////////////////////////////////////
//
// IQuaran.h - Contains interface definitions for Quarantine DLL
//
// Interfaces:      IQuarantineDLL
//                  IQuarantineItem
//                  IEnumQuarantineItems
//
////////////////////////////////////////////////////////////////////////////
// $Log:   S:/INCLUDE/VCS/IScanDeliver.h_v  $
//
//    Rev 1.1   13 Apr 1998 15:59:54   SEDWARD
// Changed 'DeliverFileToSarc' to 'DeliverFilesToSarc'.
//
//    Rev 1.0   13 Apr 1998 15:16:14   JTaylor
// Initial revision.
//
////////////////////////////////////////////////////////////////////////////

#ifndef _ISCANDELIVER_H
#define _ISCANDELIVER_H

#ifdef INITGUID
#include <initguid.h>
#endif

// Include the definitions for IQuarantineDLL and IQuarantineItem
#include "iquaran.h"

///////////////////////////////////////////////////////////////////////////
// IScanDeliverDLL

// {4c34b690-d1b7-11d1-b041-00104b252eea}
DEFINE_GUID(CLSID_ScanDeliverDLL,
    0x4c34b690, 0xd1b7, 0x11d1, 0xb0, 0x41, 0x00, 0x10, 0x4b, 0x25, 0x2e, 0xea);

// {4c34b691-d1b7-11d1-b041-00104b252eea}
DEFINE_GUID(IID_ScanDeliverDLL,
    0x4c34b691, 0xd1b7, 0x11d1, 0xb0, 0x41, 0x00, 0x10, 0x4b, 0x25, 0x2e, 0xea);

// {821C3059-DB33-11d2-8F4D-3078302C2030}
DEFINE_GUID(IID_ScanDeliverDLL2,
    0x821c3059, 0xdb33, 0x11d2, 0x8f, 0x4d, 0x30, 0x78, 0x30, 0x2c, 0x20, 0x30);



class IScanDeliverDLL : public IUnknown
{
public:
    STDMETHOD( DeliverFilesToSarc( IQuarantineItem **pQuarantineItemArray, int nNumberItems ) )
        PURE;
};


class IScanDeliverDLL2 : public IScanDeliverDLL
{
public:
    STDMETHOD( SetVirusDefinitionsDate( SYSTEMTIME* pstDate ) )
        PURE;

    STDMETHOD( SetDefinitionsDir( LPCSTR pszDefsDir ) )
        PURE;

    STDMETHOD( SetStartupDir( LPCSTR pszStartupDir ) )
        PURE;

    STDMETHOD( SetConfigDir( LPCSTR  pszConfigDir ) )
        PURE;

    STDMETHOD( SetLanguage( LPCSTR  pszLanguage ) )
        PURE;

};



////////////////////////////////////////////////////////////////////////////
#endif
