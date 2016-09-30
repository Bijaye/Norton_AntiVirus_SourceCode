// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/APEVENT/VCS/APEVENT.H_v   1.3   17 Feb 1998 16:47:44   RStanev  $
//
// Description:
//      This file prototypes NAVAP's main Event Manager functions.  Some
//      of these functions are exported to other NAVAP modules.
//
// Contains:
//      InitEDExtra()
//      CleanUpEDExtra()
//      ProcessEDExtra()
//
//      EDXSetbFileIsExecutable()
//      EDXSetbAltFileIsExecutable()
//      EDXSetpszOwner()
//      EDXSetpszOEMFile()                  (SYM_VXD only)
//      EDXSetpszAltOEMFile()               (SYM_VXD only)
//      EDXSetpszOEMDevice()                (SYM_VXD only)
//      EDXSetpszOEMOwner()                 (SYM_VXD only)
//
//      EventPacketCreate()
//      EventPacketDestroy()
//      EventPacketProcess()
//      EventPacketGetResult()
//      EventPacketSetResult()
//      GetDefaultEventResult()
//      EventPacketInitTaskQueue()
//      EventPacketGetTask()
//
//      CreateDeviceList()
//      DestroyDeviceList()
//      InstallDeviceEventHooks()
//      UninstallDeviceEventHooks()
//      GetNextRemovableDevice()
//      GetDeviceInfoBasedOnED()
//
//      ECBReboot()
//      InstallEventHooks()
//      UnInstallEventHooks()
//
// See Also:
//      edextra.cpp, evpacket.cpp, evtasks.cpp, device.cpp
//
// Note:
//      Event manager module
//
//***************************************************************************
// $Log:   S:/APEVENT/VCS/APEVENT.H_v  $
// 
//    Rev 1.3   17 Feb 1998 16:47:44   RStanev
// Updated copyright.
//
//    Rev 1.2   17 Jun 1997 21:04:44   RStanev
// Prototyped ECBReboot().
//
//    Rev 1.1   15 May 1997 01:29:04   RStanev
// Cleaned up.
//
//    Rev 1.0   26 Feb 1997 04:34:04   RSTANEV
// Initial revision.
//
//***************************************************************************

#ifndef _APEVENT_H_
#define _APEVENT_H_

//---------------------------------------------------------------------------
//
// Event Manager function prototypes.
//
//---------------------------------------------------------------------------
BOOL InitEDExtra (
    PEDEXTRA prEDExtra,
    REFDATA  uRefData,
    LPED     pEventData
    );

VOID CleanUpEDExtra (
    PEDEXTRA prEDExtra
    );

BOOL ProcessEDExtra (
    PEDEXTRA prEDExtra
    );

BOOL EDXSetbFileIsExecutable (
    PEDEXTRA prEDExtra
    );

BOOL EDXSetbAltFileIsExecutable (
    PEDEXTRA prEDExtra
    );

PTSTR EDXSetpszOwner (
    PEDEXTRA prEDExtra
    );

#if defined(SYM_VXD)

PSTR EDXSetpszOEMFile (
    PEDEXTRA prEDExtra
    );

PSTR EDXSetpszAltOEMFile (
    PEDEXTRA prEDExtra
    );

PSTR EDXSetpszOEMDevice (
    PEDEXTRA prEDExtra
    );

PSTR EDXSetpszOEMOwner (
    PEDEXTRA prEDExtra
    );

#endif // #if defined(SYM_VXD)

BOOL EventPacketCreate (
    PHEVENTPACKET phEventPacket,
    PEDEXTRA      prEDExtra
    );

BOOL EventPacketDestroy (
    PHEVENTPACKET phEventPacket
    );

VOID EventPacketProcess (
    HEVENTPACKET hEventPacket
    );

BOOL EventPacketGetResult (
    HEVENTPACKET prEventPacket
    );

VOID EventPacketSetResult (
    HEVENTPACKET prEventPacket,
    BOOL         bResult
    );

BOOL GetDefaultEventResult (
    LPED     prED,
    PEDEXTRA prEDExtra
    );

BOOL EventPacketInitTaskQueue (
    HEVENTPACKET prEventPacket
    );

BOOL EventPacketGetTask (
    HEVENTPACKET prEventPacket,
    PTASK        peTask
    );

//---------------------------------------------------------------------------
//
// Prototype functions for building and managing the list of devices.
//
//---------------------------------------------------------------------------
BOOL CreateDeviceList (
    VOID
    );

BOOL DestroyDeviceList (
    VOID
    );

BOOL InstallDeviceEventHooks (
    EVENTTYPE    EventType,
    EVENTFLAGS   EventFlags,
    EVENTHANDLER EventHandler,
    REFDATA      RefData
    );

BOOL UninstallDeviceEventHooks (
    EVENTTYPE    EventType,
    EVENTFLAGS   EventFlags,
    EVENTHANDLER EventHandler,
    REFDATA      RefData
    );

PDEVICEINFOREC GetNextRemovableDevice (
    PDEVICEINFOREC prDeviceInfo
    );

BOOL GetDeviceInfoBasedOnED (
    LPED           prEventData,
    PDEVICEINFOREC prDeviceInfo
    );

//---------------------------------------------------------------------------
//
// Prototype functions for managing NAVAP's event hooks.
//
//---------------------------------------------------------------------------
BOOL SECALLBACK ECBReboot (
    REFDATA uRefData,
    LPED    lpEventData
    );

BOOL InstallEventHooks (
    VOID
    );

BOOL UnInstallEventHooks (
    VOID
    );

#endif // #ifndef _APEVENT_H_
