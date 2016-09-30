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


class IScanDeliverDLL : public IUnknown
{
public:
    STDMETHOD( DeliverFilesToSarc( IQuarantineItem **pQuarantineItemArray, int nNumberItems ) )
        PURE;
};




////////////////////////////////////////////////////////////////////////////
#endif