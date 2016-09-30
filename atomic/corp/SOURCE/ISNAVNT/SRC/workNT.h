// Copyright 1997 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/ISNAVNT/VCS/worknt.h_v   1.3   03 Oct 1997 14:11:28   jtaylor  $
//
// Description:
//  This header file is included in the "setup.rul" InstallShield script.
//  Since the differences between NT Server and NT Workstation installations
//  are minor, this header file is used to adjust the active portions of the
//  "setup.rul" file.  If the "WORKSTATION_BUILD" is defined, the workstation
//  version of setup.rul will be compiled; else, the server version will be
//  compiled.
//
//***************************************************************************
// $Log:   S:/ISNAVNT/VCS/worknt.h_v  $
// 
//    Rev 1.3   03 Oct 1997 14:11:28   jtaylor
// -- undid my previous change.
// 
//
//    Rev 1.2   03 Oct 1997 13:50:48   jtaylor
// -- Removed the #defineWorkstation.  We no longer want to check for that.
//
//    Rev 1.1   08 Jul 1997 19:06:32   JBRENNA
// Add a return on the end of the #define.
//
//    Rev 1.0   08 Jul 1997 17:02:32   SEDWARD
// Initial revision.
//***************************************************************************

#define  WORKSTATION_BUILD