// Copyright 1995 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVSDK/VCS/device.h_v   1.1   04 Dec 1997 09:48:18   DALLEE  $
//
// Description:
//      This file declares types and functions for working with NAVAP's
//      device list.
//
// Contains:
//      -
//
// See Also:
//      device.cpp
//
// Note:
//***************************************************************************
// $Log:   S:/NAVSDK/VCS/device.h_v  $
// 
//    Rev 1.1   04 Dec 1997 09:48:18   DALLEE
// Added usage counts for device info list and critical sections around
// create and destroy, so this can be shared by mutiple threads.
// 
//    Rev 1.0   02 Dec 1997 14:22:54   DALLEE
// Initial revision.
//***************************************************************************
// From NEWVIR-NAVOEM device.h_v r1.4
//
//    Rev 1.4   14 Jun 1996 17:57:56   TCASHIN
// Added loadds for 16-bit.
//***************************************************************************

#ifndef _DEVICE_H_
#define _DEVICE_H_

//---------------------------------------------------------------------------
//
// The following structure is used to build a list of devices.
//
//---------------------------------------------------------------------------
typedef struct tagDEVICEINFOREC
    {
    tagDEVICEINFOREC FAR *prNextDeviceInfo; // Pointer to next record in list
    UINT              uPhysicalDevice;  // Device number (int 13 standard)
    BOOL              bVisible;         // Device / Partition visible to user
    BOOL              bPartitionable;   // Device supports partitioning
    UINT              uPartition;       // Partition number (0 through 3)
    UINT              uExtPartition;    // Extended partition number in chain
    BOOL              bPartitionBootable;  // Partition is bootable
    UINT              uBootSectorLocation; // Boot sector's physical location
    char              chPhysicalID;     // Identifier for the user interface
    char              chLogicalID;      // Identifier for the user interface
  } DEVICEINFOREC, *PDEVICEINFOREC, FAR * LPDEVICEINFOREC;

//---------------------------------------------------------------------------
//
// Define function prototypes for working with device list.
//
//---------------------------------------------------------------------------

// These two functions should be called from DllMain (before and after all
// access to the device info list) so we can synchronize access to the list.
BOOL WINAPI InitDeviceListSynchronization (VOID);
BOOL WINAPI CloseDeviceListSynchronization (VOID);

BOOL WINAPI CreateDeviceList (VOID);
BOOL WINAPI DestroyDeviceList (VOID);

BYTE WINAPI GetLogDrvFromPhysDev (BYTE byPhysicalDeviceNum, BYTE byPartitionNum);

LPDEVICEINFOREC WINAPI GetDeviceListHead (VOID);
LPDEVICEINFOREC WINAPI GetLogicalDrive (BYTE byLogicalDrive);
LPDEVICEINFOREC WINAPI GetPhysicalDevice (UINT uPhysicalDeviceNumber);
LPDEVICEINFOREC WINAPI GetNextDevice (LPDEVICEINFOREC);
LPDEVICEINFOREC WINAPI GetNextPartition (LPDEVICEINFOREC);
LPDEVICEINFOREC WINAPI GetNextRemovableDevice (LPDEVICEINFOREC);

#endif // _DEVICE_H_
