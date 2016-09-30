// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/excelheu.H_v   1.0   04 Aug 1998 11:18:30   DCHI  $
//
// Description:
//  Contains Excel 5.0/95/97 heuristic macro scan function header.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/excelheu.H_v  $
// 
//    Rev 1.0   04 Aug 1998 11:18:30   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _EXCELHEU_H_

#define _EXCELHEU_H_

#define XL5H_VSTR_ID_activesheet            0
#define XL5H_VSTR_ID_activeworkbook         1
#define XL5H_VSTR_ID_add                    2
#define XL5H_VSTR_ID_after                  3
#define XL5H_VSTR_ID_altstartuppath         4
#define XL5H_VSTR_ID_application            5
#define XL5H_VSTR_ID_before                 6
#define XL5H_VSTR_ID_copy                   7
#define XL5H_VSTR_ID_filename               8
#define XL5H_VSTR_ID_insertfile             9
#define XL5H_VSTR_ID_modules                10
#define XL5H_VSTR_ID_name                   11
#define XL5H_VSTR_ID_pathseparator          12
#define XL5H_VSTR_ID_saveas                 13
#define XL5H_VSTR_ID_savecopyas             14
#define XL5H_VSTR_ID_sheets                 15
#define XL5H_VSTR_ID_startuppath            16
#define XL5H_VSTR_ID_thisworkbook           17
#define XL5H_VSTR_ID_workbooks              18

#define XL5H_VSTR_ID_COUNT                  19

#define XL5H_DEF_BUF_SIZE                   4096

#define XL5H_STATE_DONT_SCAN                0x01
#define XL5H_STATE_HAS_SAVE                 0x02
#define XL5H_STATE_HAS_COPY                 0x04
#define XL5H_STATE_HAS_ADD                  0x08
#define XL5H_STATE_HAS_NAME                 0x10
#define XL5H_STATE_HAS_INSERTFILE           0x20

#define XL5H_STATE_SIZE                     64

#define XL5H_MAX_ASSIGNS                    16
#define XL5H_MAX_WITHS                      4

typedef struct tagXL5H_FAST_SCAN
{
    BYTE                byModState;

    BYTE                abyOp[2];
    WORD                wOpLen;
    int                 nOpcode;
    DWORD               dwN;

    LPWORD              lpawAppStrID;
} XL5H_FAST_SCAN_T, FAR *LPXL5H_FAST_SCAN;

typedef struct tagXL5H_SCAN_STATE
{
    WORD                wNextScanLine;

    WORD                wFuncBeginLine;

    int                 nAssignHead;
    int                 nAssignCount;
    WORD                awAssignLines[XL5H_MAX_ASSIGNS];
    BYTE                abyAssignIDs[XL5H_MAX_ASSIGNS][2];

    int                 nWithHead;
    int                 nWithCount;
    WORD                awWithLines[XL5H_MAX_WITHS];
    WORD                awEndWithLines[XL5H_MAX_WITHS];
} XL5H_SCAN_STATE_T, FAR *LPXL5H_SCAN_STATE;

// Top level objects

#define XL5H_OBJ_UNKNOWN                0
#define XL5H_OBJ_STRING                 1
#define XL5H_OBJ_ID                     2
#define XL5H_OBJ_ID_REF                 3
#define XL5H_OBJ_STARTUP_PATH           4
#define XL5H_OBJ_PATH_SEPARATOR         5
#define XL5H_OBJ_VSTR_ID                6
#define XL5H_OBJ_WORD_VALUE             7
#define XL5H_OBJ_ROOTED                 8

#define XL5H_OBJ_application            1
#define XL5H_OBJ_activeworkbook         2
#define XL5H_OBJ_thisworkbook           3
#define XL5H_OBJ_modules                4
#define XL5H_OBJ_modules_obj            5
#define XL5H_OBJ_activesheet            6
#define XL5H_OBJ_sheets                 7
#define XL5H_OBJ_sheets_obj             8

#define XL5H_OBJ_MAX_PARTS      16
#define XL5H_OBJ_MAX_DATA_SIZE  128

typedef struct tagXL5H_OBJ
{
    BYTE                byNumParts;
    BYTE                byDataUsed;
    BYTE                abyType[XL5H_OBJ_MAX_PARTS];
    BYTE                abyParam[XL5H_OBJ_MAX_PARTS][2];
    BYTE                abyInfo[XL5H_OBJ_MAX_PARTS][2];
    BYTE                abyData[XL5H_OBJ_MAX_DATA_SIZE];
} XL5H_OBJ_T, FAR *LPXL5H_OBJ;

#define XL5H_RESOLVE_MAX_OPS        64

#define XL5H_MAX_SAVE_NAME_SIZE     64

#define XL5H_TYPE_95                0
#define XL5H_TYPE_97                1
#define XL5H_TYPE_98                2

typedef struct tagXL5H
{
    int                 nType;

    LPSS_ROOT           lpstRoot;
    LPSS_STREAM         lpstModStream;

    DWORD               dw_VBA_PROJECTChildEntry;

    LPSS_STREAM         lpstDirStream;
    LPSS_STREAM         lpstSaveStream;
    LPSS_STREAM         lpstCopyStream;

    XL5_DIR_INFO_T      stDirInfo;

    LPSS_ENUM_SIBS      lpstSaveSibs;
    LPSS_ENUM_SIBS      lpstCopySibs;

    BYTE                abypsSaveSheetName[256];
    BYTE                abypsCopySheetName[256];

    BYTE                abypsSaveStreamName[SS_MAX_NAME_LEN + 2];
    BYTE                abypsCopyStreamName[SS_MAX_NAME_LEN + 2];

    WORD                awAppStrID[XL5H_VSTR_ID_COUNT];

    int                 nBufSize;
    LPBYTE              lpabyBuf;

    union
    {
        XL5_MOD_ACC_T       st95;
        O97_MOD_ACC_T       st97;
    } stMod;

    BOOL                bLittleEndian;

    // CRC and scan requirement state

    BYTE                abyState[XL5H_STATE_SIZE];
    WORD                wStateNextModToCheck;
    WORD                wStateStartGroup;
    WORD                wStateFold;

    XL5H_FAST_SCAN_T    stFastScan;

    int                 nNumOps;
    BOOL                bCallIsRooted;
    int                 nLastCallOp;
    int                 nParamStart;
    int                 nParamEnd;
    LPBYTE              lpabyLine;
    int                 anOpOffsets[XL5H_RESOLVE_MAX_OPS];
    BYTE                abyOps[XL5H_RESOLVE_MAX_OPS];

    // SaveAs/SaveCopyAs state

    WORD                wModuleStreamIdx;
    WORD                wModuleSheetIdx;
    DWORD               dwGlobalNameCRC;
    DWORD               dwModuleNameCRC;

    XL5H_SCAN_STATE_T   stSaveState;

    BYTE                abypsszSaveName[XL5H_MAX_SAVE_NAME_SIZE];

    // Copy state

    WORD                wCopyModuleStreamIdx;
    XL5H_SCAN_STATE_T   stCopyState;

    // Objects

    XL5H_OBJ_T          stCallObj;
    XL5H_OBJ_T          stParamObj;

    XL5H_OBJ_T          stTempObj;
} XL5H_T, FAR *LPXL5H;

LPXL5H XL5HAlloc
(
    LPSS_ROOT       lpstRoot,
    LPSS_STREAM     lpstDirStream
);

void XL5HInit
(
    LPXL5H          lpstEnv
);

BOOL XL5HFree
(
    LPXL5H          lpstEnv
);

BOOL XL5HGetAppStrID
(
    LPXL5H          lpstEnv
);

BOOL XL5HModuleNeedsSaveScan
(
    LPXL5H              lpstEnv,
    WORD                wModuleNum,
    LPBOOL              lpbNeedsScan
);

BOOL XL5HModuleNeedsCopyScan
(
    LPXL5H              lpstEnv,
    WORD                wModuleNum,
    LPBOOL              lpbNeedsScan
);

BOOL XL5HModuleNeedsAddNameScan
(
    LPXL5H              lpstEnv,
    WORD                wModuleNum,
    LPBOOL              lpbNeedsScan
);

BOOL XL5HModuleNeedsAddInsertScan
(
    LPXL5H              lpstEnv,
    WORD                wModuleNum,
    LPBOOL              lpbNeedsScan
);

BOOL XL5HModuleFindSave
(
    LPXL5H              lpstEnv,
    LPBOOL              lpbFound
);

BOOL XL5HResolveSaveCallLine
(
    LPXL5H              lpstEnv,
    WORD                wLine
);

BOOL XL5HModuleFindCopy
(
    LPXL5H              lpstEnv,
    LPBOOL              lpbFound
);

BOOL XL5HCopyCallIsViral
(
    LPXL5H              lpstEnv,
    WORD                wLine
);

BOOL XL5HHasAWBModulesAdd
(
    LPXL5H              lpstEnv
);

BOOL XL5HInsertFileCallIsViral
(
    LPXL5H              lpstEnv,
    WORD                wLine
);

BOOL XL5HModuleFindInsertFile
(
    LPXL5H              lpstEnv,
    LPBOOL              lpbFound
);

BOOL XL5HModuleFindNameAssign
(
    LPXL5H              lpstEnv,
    LPBOOL              lpbFound
);

BOOL XL5HNameAssignmentIsViral
(
    LPXL5H              lpstEnv,
    WORD                wLine
);

BOOL XL5H97GetAppStrID
(
    LPXL5H          lpstEnv
);

#endif // #ifndef _EXCELHEU_H_

