// Copyright 1997 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/ISNAVNT/VCS/serveNT.h_v   1.0   08 Jul 1997 17:03:12   SEDWARD  $
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
// $Log:   S:/ISNAVNT/VCS/serveNT.h_v  $
// 
//    Rev 1.0   08 Jul 1997 17:03:12   SEDWARD
// Initial revision.
//***************************************************************************

// #define  WORKSTATION_BUILD