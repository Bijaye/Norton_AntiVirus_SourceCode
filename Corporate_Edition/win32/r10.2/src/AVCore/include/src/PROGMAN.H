// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 1996 - 2003, 2005 Symantec Corporation. All rights reserved.
//***********************************************************************
//                                                                
// $Header:   S:/INCLUDE/VCS/progman.h_v   1.1   24 Aug 1998 14:04:06   KSACKIN  $ 
//                                                                
// Description:                                                   
//      This header file contains protoypes for those routines necessary for    
//      Adding, Deleting - Items and Groups in Program Manager.
//                                                                
// Contains :           
//      ProgmanGroupManipulation(), GroupDDECallback()
//                                                                
//***********************************************************************
// $Log:   S:/INCLUDE/VCS/progman.h_v  $
// 
//    Rev 1.1   24 Aug 1998 14:04:06   KSACKIN
// Ported new function prototypes from the Atomic version of this project.  New
// function added include IsProgmanIconic, MinimizeProgman, and RestoreProgman.
// 
//    Rev 1.1   24 Apr 1997 19:00:52   JBRENNA
// Tesla port: TSmith r1.1:
// Added function declarations 'IsProgmanIconic', 'RestoreProgman' and
// 'MinimizeProgman'.
//    Rev 1.0   11 Dec 1996 15:46:50   rpulint
// Initial revision.
// 
//    Rev 1.7   22 Jul 1996 09:45:40   RPULINT
// Fixed defect 61054: Added ProgmanGroupAddItemNT4()
// 
//    Rev 1.6   16 Jul 1996 11:09:00   RPULINT
// renamed GetProgramGroupName() to GetProgmanGroupName()
// 
//    Rev 1.5   16 Jul 1996 11:03:46   RPULINT
// Fixed defect 57791: Added GetProg
// 
//    Rev 1.4   15 Jul 1996 10:27:34   RPULINT
// Fixed defect 58146: Added parameter to support common/private groups
// 
//    Rev 1.3   11 Jul 1996 09:21:54   RPULINT
// Fixes to defect 60116, 60114, 59916
// 
//    Rev 1.2   09 Jul 1996 17:53:36   RPULINT
// Added ProgmanGroupManipulationNT4 - fixed defects 59916, 60108,
// 60114, 60116
// 
//    Rev 1.1   18 Jun 1996 14:12:34   RPULINT
// Defect 58336 - Added ProgmanQueryItemInGroup()
// 
//    Rev 1.0   11 Jun 1996 10:48:04   RPULINT
// Initial revision.

#ifndef PROGMAN_H_INCLUDED
#define PROGMAN_H_INCLUDED

#define     ACTION_ADD_ITEM     1
#define     ACTION_DELETE_ITEM  2

#define     ACTION_ADD_GROUP    3
#define     ACTION_DELETE_GROUP 4

#define     GROUPTYPE_COMMON      1                    // Common Group
#define     GROUPTYPE_PRIVATE     0                    // Private/Personal Group

#define     SI_MAXCOMMAND       512

#define     ITEM_ERROR         -2
#define     DDE_ERROR          -1
#define     ITEM_NOTFOUND       0
#define     ITEM_FOUND          1

// Prototype for ProgmanGroupManipulation() function
BOOL ProgmanGroupManipulation ( LPSTR lpGroupName, LPSTR lpCommand, int nActionFlag, int nGroupType ) ;

// Prototype for DDECallback() function. used in ProgmanGroupManipulation()
HDDEDATA CALLBACK GroupDDECallback (UINT, UINT, HANDLE, HSZ, HSZ, HDDEDATA,LONG, LONG);

int ProgmanQueryItemInGroup (  LPSTR lpGroupName, LPSTR lpItemName, int nGroupType ) ;

BOOL ProgmanGroupManipulationNT4 ( LPSTR lpGroupName, LPSTR lpItemName, int nActionFlag, int nGroupType ) ;

int ProgmanQueryItemInGroupNT4 (  LPSTR lpGroupName, LPSTR lpItemName, int nGroupType ) ;

LPSTR GetProgmanGroupName ( LPSTR lpGroupName, int nGroupType );

// When using this function you need to link 2 libraries - OLE32.LIB and UUID.LIB
BOOL ProgmanGroupAddItemNT4 ( LPCSTR lpGroupName, LPCSTR lpItemName, LPCSTR lpCmdLine, 
                              LPCSTR lpArguments, LPCSTR lpIconPath, int nIconIndex,
                              LPCSTR lpDefDir, int nGroupType ) ;


BOOL RestoreProgman  (VOID);
BOOL MinimizeProgman (VOID);
BOOL IsProgmanIconic (VOID);

#endif // PROGMAN_H_INCLUDED
