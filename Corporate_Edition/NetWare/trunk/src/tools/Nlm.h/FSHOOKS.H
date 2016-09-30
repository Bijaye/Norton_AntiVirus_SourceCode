/****************************************************************************
*
* (C) Unpublished Copyright Novell, Inc. All Rights Reserved.
*
* No part of this file may be duplicated, revised, translated,
* localized or modified in any manner or compiled, linked or 
* uploaded or downloaded to or from any computer system without
* the prior written consent of Novell, Inc.
*
* NetWare C NLM Runtime Library source code
*
*****************************************************************************/

#ifndef _FSHOOKS_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#endif

#include <nwtypes.h>


/* ------------  File System Monitor Hook Call Back Numbers ------------

   The defined constants below that have _GEN_ in the name represent call back
   numbers that will hook Generic versions of the respective OS routines.
   Namely, routines that support Name Spaces other than DOS.

   ---------------------------------------------------------------------*/


#define FSHOOK_PRE_ERASEFILE            0      // 0x0
#define FSHOOK_PRE_OPENFILE             1      // 0x1
#define FSHOOK_PRE_CREATEFILE           2      // 0x2
#define FSHOOK_PRE_CREATE_OPENFILE      3      // 0x3
#define FSHOOK_PRE_RENAME_OR_MOVE       4      // 0x4
#define FSHOOK_PRE_CLOSEFILE            5      // 0x5
#define FSHOOK_PRE_CREATEDIR            6      // 0x6
#define FSHOOK_PRE_DELETEDIR            7      // 0x7
#define FSHOOK_PRE_MODIFY_DIRENTRY      8      // 0x8
#define FSHOOK_PRE_SALVAGE_DELETED      9      // 0x9
#define FSHOOK_PRE_PURGE_DELETED       10      // 0xA
#define FSHOOK_PRE_RENAME_NS_ENTRY     11      // 0xB
#define FSHOOK_PRE_GEN_SALVAGE_DELETED 12      // 0xC
#define FSHOOK_PRE_GEN_PURGE_DELETED   13      // 0xD
#define FSHOOK_PRE_GEN_OPEN_CREATE     14      // 0xE
#define FSHOOK_PRE_GEN_RENAME          15      // 0xF
#define FSHOOK_PRE_GEN_ERASEFILE       16      // 0x10
#define FSHOOK_PRE_GEN_MODIFY_DOS_INFO 17      // 0x11
#define FSHOOK_PRE_GEN_MODIFY_NS_INFO  18      // 0x12

#define FSHOOK_POST_ERASEFILE           0x80000000
#define FSHOOK_POST_OPENFILE            0x80000001
#define FSHOOK_POST_CREATEFILE          0x80000002
#define FSHOOK_POST_CREATE_OPENFILE     0x80000003
#define FSHOOK_POST_RENAME_OR_MOVE      0x80000004
#define FSHOOK_POST_CLOSEFILE           0x80000005
#define FSHOOK_POST_CREATEDIR           0x80000006
#define FSHOOK_POST_DELETEDIR           0x80000007
#define FSHOOK_POST_MODIFY_DIRENTRY     0x80000008
#define FSHOOK_POST_SALVAGE_DELETED     0x80000009
#define FSHOOK_POST_PURGE_DELETED       0x8000000A
#define FSHOOK_POST_RENAME_NS_ENTRY     0x8000000B
#define FSHOOK_POST_GEN_SALVAGE_DELETED 0x8000000C
#define FSHOOK_POST_GEN_PURGE_DELETED   0x8000000D
#define FSHOOK_POST_GEN_OPEN_CREATE     0x8000000E
#define FSHOOK_POST_GEN_RENAME          0x8000000F
#define FSHOOK_POST_GEN_ERASEFILE       0x80000010
#define FSHOOK_POST_GEN_MODIFY_DOS_INFO 0x80000011
#define FSHOOK_POST_GEN_MODIFY_NS_INFO  0x80000012



   /*--------------------------------------------------------------------*
    * Structure returned for
    * FSHOOK_PRE_ERASEFILE and FSHOOK_POST_ERASEFILE
    *--------------------------------------------------------------------*/
typedef struct {
   LONG   connection;
   LONG   task;
   LONG   volume;
   LONG   dirBase;
   BYTE   *pathString;
   LONG   pathComponentCount;
   LONG   nameSpace;
   LONG   attributeMatchBits;
   } EraseFileCallBackStruct;

   /*--------------------------------------------------------------------*
    * Structure returned for
	 * FSHOOK_PRE_OPENFILE and FSHOOK_POST_OPENFILE
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   task;
	LONG   volume;
	LONG   dirBase;
	BYTE   *pathString;
	LONG   pathComponentCount;
	LONG   nameSpace;
	LONG   attributeMatchBits;
	LONG   requestedAccessRights;
	LONG   dataStreamNumber;
	LONG   *fileHandle;
	} OpenFileCallBackStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_CREATEFILE and FSHOOK_POST_CREATEFILE
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   task;
	LONG   volume;
	LONG   dirBase;
	BYTE   *pathString;
	LONG   pathComponentCount;
	LONG   nameSpace;
	LONG   createAttributeBits;
	LONG   createFlagBits;
	LONG   dataStreamNumber;
	} CreateFileCallBackStruct;


	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_CREATE_OPENFILE and FSHOOK_POST_CREATE_OPENFILE
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   task;
	LONG   volume;
	LONG   dirBase;
	BYTE   *pathString;
	LONG   pathComponentCount;
	LONG   nameSpace;
	LONG   createAttributeBits;
	LONG   requestedAccessRights;
	LONG   createFlagBits;
	LONG   dataStreamNumber;
	LONG   *fileHandle;
	} CreateAndOpenCallBackStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_RENAME_OR_MOVE and FSHOOK_POST_RENAME_OR_MOVE
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   task;
	LONG   volume;
	LONG   dirBase;
	BYTE   *pathString;
	LONG   pathComponentCount;
	LONG   nameSpace;
	LONG   attributeMatchBits;
	LONG   subDirsOnlyFlag;
	LONG   newDirBase;
	BYTE   *newPathString;
	LONG   originalNewCount;
	LONG   compatibilityFlag;
	LONG   allowRenamesToMyselfFlag;
	} RenameMoveEntryCallBackStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_CLOSEFILE and FSHOOK_POST_CLOSEFILE
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   task;
	LONG   fileHandle;
	} CloseFileCallBackStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_CREATEDIR and FSHOOK_POST_CREATEDIR
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   volume;
	LONG   dirBase;
	BYTE   *pathString;
	LONG   pathComponentCount;
	LONG   nameSpace;
	LONG   directoryAccessMask;
	} CreateDirCallBackStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_DELETEDIR and FSHOOK_POST_DELETEDIR
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   volume;
	LONG   dirBase;
	BYTE   *pathString;
	LONG   pathComponentCount;
	LONG   nameSpace;
	} DeleteDirCallBackStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_MODIFYDIRENTRY and FSHOOK_POST_MODIFYDIRENTRY
	 *--------------------------------------------------------------------*/
struct ModifyStructure;
struct ModifyStructure;
typedef struct {
	LONG   connection;
	LONG   task;
	LONG   volume;
	LONG   dirBase;
	BYTE   *pathString;
	LONG   pathComponentCount;
	LONG   nameSpace;
	LONG   attributeMatchBits;
	LONG   targetNameSpace;
	struct ModifyStructure *modifyVector;
	LONG   modifyBits;
	LONG   allowWildCardsFlag;
	} ModifyDirEntryCallBackStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_SALVAGE_DELETED and FSHOOK_POST_SALVAGE_DELETED
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   volume;
	LONG   dirBase;
	LONG   toBeSalvagedDirBase;
	LONG   nameSpace;
	BYTE   *newName;
	} SalvageDeletedCallBackStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_PURGE_DELETED and FSHOOK_POST_PURGE_DELETED
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   volume;
	LONG   dirBase;
	LONG   toBePurgedDirBase;
	LONG   nameSpace;
	} PurgeDeletedCallBackStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_RENAME_NS_ENTRY and FSHOOK_POST_RENAME_NS_ENTRY
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   task;
	LONG   volume;
	LONG   dirBase;
	BYTE   *pathString;
	LONG   pathComponentCount;
	LONG   nameSpace;
	LONG   matchBits;
	BYTE   *newName;
	} RenameNSEntryCallBackStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_GEN_SALVAGE_DELETED and FSHOOK_POST_GEN_SALVAGE_DELETED
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   nameSpace;
	LONG   sequence;
	LONG   volume;
	LONG   dirBase;
	BYTE   *newName;
	} GenericSalvageDeletedCBStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_GEN_PURGE_DELETED and FSHOOK_POST_GEN_PURGE_DELETED
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   nameSpace;
	LONG   sequence;
	LONG   volume;
	LONG   dirBase;
	} GenericPurgeDeletedCBStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_GEN_OPEN_CREATE and FSHOOK_POST_GEN_OPEN_CREATE
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   task;
	LONG   volume;
	LONG   pathComponentCount;
	LONG   dirBase;
	BYTE   *pathString;
	LONG   nameSpace;
	LONG   dataStreamNumber;
	LONG   openCreateFlags;
	LONG   searchAttributes;
	LONG   createAttributes;
	LONG   requestedAccessRights;
	LONG   returnInfoMask;
	LONG   *fileHandle;
	BYTE   *openCreateAction;
	} GenericOpenCreateCBStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_GEN_RENAME and FSHOOK_POST_GEN_RENAME
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   task;
	LONG   nameSpace;
	LONG   renameFlag;
	LONG   searchAttributes;
	LONG   srcVolume;
	LONG   srcPathComponentCount;
	LONG   srcDirBase;
	BYTE   *srcPathString;
	LONG   dstVolume;
	LONG   dstPathComponentCount;
	LONG   dstDirBase;
	BYTE   *dstPathString;
	} GenericRenameCBStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_GEN_ERASEFILE and FSHOOK_POST_GEN_ERASEFILE
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   task;
	LONG   volume;
	LONG   pathComponentCount;
	LONG   dirBase;
	BYTE   *pathString;
	LONG   nameSpace;
	LONG   searchAttributes;
	} GenericEraseFileCBStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_GEN_MODIFY_DOS_INFO and FSHOOK_POST_GEN_MODIFY_DOS_INFO
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   task;
	LONG   volume;
	LONG   pathComponentCount;
	LONG   dirBase;
	BYTE   *pathString;
	LONG   nameSpace;
	LONG   searchAttributes;
	LONG   modifyMask;
	void   *modifyInfo;
	} GenericModifyDOSInfoCBStruct;

	/*--------------------------------------------------------------------*
	 * Structure returned for
	 * FSHOOK_PRE_GEN_MODIFY_NS_INFO and FSHOOK_POST_GEN_MODIFY_NS_INFO
	 *--------------------------------------------------------------------*/
typedef struct {
	LONG   connection;
	LONG   task;
	LONG   dataLength;
	LONG   srcNameSpace;
	LONG   dstNameSpace;
	LONG   volume;
	LONG   dirBase;
	LONG   modifyMask;
	void   *modifyInfo;
	} GenericModifyNSInfoCBStruct;


extern LONG NWAddFSMonitorHook(LONG callBackNumber, void *callBackFunc,
										LONG *callBackHandle);

extern LONG NWRemoveFSMonitorHook(LONG callBackNumber, LONG callBackHandle);

#define _FSHOOKS_H_INCLUDED
#ifdef __cplusplus
};
#endif
#endif  /* _FSHOOKS_H_INCLUDED  */

