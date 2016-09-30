// Copyright 1997 Symantec Corporation
//***************************************************************************/
//
// $Header: /Build Sets/Roswell 95/is5/Script Files/grpconvert.h 1     2/11/99 5:26p Mmavrom $
//
// Description:
//  This header file is included in the "setup.rul" InstallShield script.
//  It is used to define if grpconvert should run after reboot or not.
//  This is a fix the conflict between the Novell App Launcher and grpconvert.exe.
//
//***************************************************************************/

#define NO_GRP_CONVERT		// Remove the GrpConv.exe registry setting from the RunOnce key
