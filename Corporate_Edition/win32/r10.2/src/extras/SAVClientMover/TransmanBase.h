// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
//#include "cba.h"
#include "vpcommon.h"

//#define IMAX_PATH 384
#define MAX_PACKET_DATA 986
//#define GC_INCLUDE_CHILDREN         0x00000004
//#define GC_EXCLUDE_SPECIAL_CHILDREN 0x00000008
//#define NumBytes strlen


/*
DWORD SendCOM_DEL_CLIENT(char *CName,char *name);
DWORD SendCOM_ALIVE(char *CName,PONGDATA *pong);
DWORD SendCOM_VIRUS_FOUND(char *CName,PEVENTBLOCK eb);
DWORD SendCOM_GET_DRIVE_LIST(char *CName,BYTE *list);
DWORD SendCOM_WRITE_FILE(char *CName,DWORD handle,DWORD loc,DWORD *len,BYTE *data);
DWORD SendCOM_GET_SERVER_STATUS(char *CName,DWORD len,DWORD loc,BYTE *buf);
DWORD SendCOM_START_SCAN2(char *CName,char *name,DWORD *han,DWORD Flags,DWORD *ScanID);
DWORD SendCOM_GET_SCAN_INFO(char *CName,DWORD han,DWORD loc,DWORD len,BYTE *buf);
DWORD SendCOM_GET_SCAN_PROGRESS(char *CName,DWORD han,DWORD loc,DWORD len,BYTE *buf);
DWORD SendCOM_GET_SCAN_INFECTIONS(char *CName,DWORD han,DWORD *index,BYTE *List,DWORD *count);
DWORD SendCOM_STOP_SCAN(char *CName,DWORD han);
DWORD SendCOM_GET_PONG_DATA(char *CName,PONGDATA *pong);
DWORD SendCOM_FREE_SCAN_HAN(char *CName,DWORD han);
DWORD SendCOM_GET_LOGIN_ARRAY(char *CName,DWORD *index,DWORD *count,PCLIENT_ARRAY array);
*/

/*
DWORD SendCOM_FIND_ALL_LOCAL(char *CName,BOOL async);
DWORD SendCOM_FORWARD_LOG(char *CName,PEVENTBLOCK cur);
DWORD SendCOM_GET_CHILD_ADDRESS_LIST(char *CName,BYTE *buf,DWORD *index,DWORD *count,DWORD len);
DWORD SendSET_FILE_DATE(char *CName,char *FilePath,VTIME *Time);
DWORD SendGET_FILE_DATE(char *CName,char *FilePath,VTIME *Time);
DWORD SendCOM_GET_HANDLE_OF_SCAN(char *CName,DWORD ScanID,DWORD *han);
DWORD SendCOM_GET_COPY_OF_FILE(char *CName,char *LogLine,char *RemotePath,DWORD State);
DWORD SendCOM_GET_STATE_OF_FILE_VIEW(char *CName,char *LogLine,DWORD *State, DWORD *ViewType);
DWORD SendCOM_TAKE_ACTION(char *CName,char *LogLine,DWORD Action,DWORD State);
DWORD SendCOM_MODIFY_SCAN_OPTIONS(char *CName,DWORD han,DWORD Flags);
DWORD SendCOM_GET_DRIVE_LIST2(char *CName,BYTE *list,DWORD *Index);
DWORD SendCOM_GET_DRIVE_LIST3(char *CName,BYTE *list,DWORD *Index,DWORD dwHiddenDrives);
DWORD SendCOM_REG_COPY_KEYS(char *CName,char *From,char *To);
DWORD SendCOM_BWRITE_FILE(char *CName,DWORD handle,DWORD loc,DWORD len,BYTE *data,DWORD ChunckNumber);
DWORD SendCOM_BCLOSE_FILE(char *CName,DWORD handle,DWORD *ChunckArray,VTIME *Time);
DWORD SendCOM_BOPEN_FILE(char *CName,char *path,char *options,DWORD *handle,DWORD size,DWORD ChunckSize);

DWORD GetAddress(char *name,CBA_Addr *address);
DWORD InvalidateAddressCacheEntry(char *name);
DWORD ClearAddressCacheEntry(char *name);
DWORD UpdateAddressCacheEntry(CBA_Addr *address,PONGDATA *pong);
DWORD FileCopyFromRemoteServer(char *CName,char *rpath,char *lpath);
DWORD FileCopyToRemoteServer(char *CName,char *lpath,char *rpath);
*/

typedef DWORD (tSendVirusAlert)(PEVENTBLOCK EventBlock);
typedef DWORD (tReturnPong)(const char *Server,const char *Doamin,const char *Mom,PONGDATA *Pong);

typedef DWORD (*tSendCOM_ACT_ON_FILE) (char *CName,char *orgName,char *newName,BYTE action);
typedef DWORD (*tPutVal) (HKEY hkey,char *sval,DWORD val);
typedef DWORD (*tSendCOM_EXEC_PROGRAM) (char *CName,char *path);
typedef DWORD (*tFindAnNTServer) (char *CName);
typedef DWORD (*tMasterFileCopy)(char *FromComputer,char *FromPath,char *ToComputer,char *ToPath,DWORD Flags);
typedef DWORD (*tVerifyAnAddress)(char *name);
typedef DWORD (*tSendCOM_CLOSE_FILE)(char *CName,DWORD handle);
typedef char *(*tMakeCBAAddrString)(CBA_Addr *address);
typedef DWORD (*tSendCOM_GET_ADDRESS_INFO)(char *CName,DWORD *count,DWORD *prots,CBA_Addr *addresses);
typedef DWORD (*tSendCOM_DEL_VALUE)(char *CName,char *root,char *value);
typedef DWORD (*tSendCOM_START_SCAN)(char *CName,char *name,DWORD *han);
typedef DWORD (*tSendCOM_READ_FILE)(char *CName,DWORD handle,DWORD loc,DWORD *len,BYTE *data);
typedef DWORD (*tSendCOM_OPEN_FILE)(char *CName,char *path,char *options,DWORD *handle,DWORD *size);
typedef DWORD (*tSendCOM_DEL_KEY)(char *CName,char *root);
typedef DWORD (*tSendCOM_GET_VALUES)(char *CName,char *root,char *buf,DWORD len,DWORD *count);
typedef DWORD (*tSendCOM_LIST_VALUE_BLOCK)(char *CName,char *root,BYTE *buf,DWORD len,DWORD *index,DWORD *count);
typedef DWORD (*tSendCOM_DIR_BLOCK)(char *CName,char *Path,HANDLE *han,BYTE *buf,DWORD bufLen,WORD *num,DWORD flags);
typedef DWORD (*tSendCOM_DEL_CONSOLE)(char *CName,char *name);
typedef DWORD (*tSendCOM_REMOVE_PROGRAM)(char *CName,DWORD);
typedef DWORD (*tSendCOM_ADD_CONSOLE)(char *CName,char *name);
typedef DWORD (*tSendCOM_READ_GROUP_SETTINGS)(char *CName,DWORD *pGRCSrvID);
typedef DWORD (*tSendCOM_WRITE_GROUP_SETTINGS)(char *CName,DWORD GRCSrvID);

typedef DWORD (*tDeinitTransman)(DWORD);
typedef DWORD (*tInitTransman)(tSendVirusAlert *cb,tReturnPong *cb2);
typedef void (*tAbortFind)(void);
typedef void (*tFindAllNTServers)(void *,DISCOVERYCB cb);
typedef void (*tFindAllNetwareServers)(void *,DISCOVERYCB cb);
typedef DWORD (*tSendDomainPing)(char *name);
typedef DWORD (*tSendSinglePing)(char *name);
typedef DWORD (*tLocateServer)(PSERVERLOCATION loc);
typedef DWORD	  (*tCvtCBA2GRC)(DWORD t);
typedef void   *(*tGetData)(HKEY hkey,char *sval,void *in,int len,void *def,BOOL *);
typedef BYTE   *(*tGetValueFromEnumBlock)(BYTE *block,char *name,DWORD *type,BYTE *data,DWORD *size);
typedef BYTE   *(*tGetValueFromBlock)(BYTE *Block,int element,WORD *size);
typedef DWORD   (*tAddValueToBlock)(BYTE *block,char *value,DWORD type,WORD size,BYTE *data,DWORD *used);
typedef DWORD   (*tGetVal)(HKEY hkey,char *sval,DWORD dev);
typedef char   *(*tGetStr)(HKEY hkey,char *sval,char *in,int len,char *def);
typedef DWORD (*tSendCOM_LIST_KEY_BLOCK)(char *CName,char *root,BYTE *buf,DWORD len,DWORD *index,DWORD *count);
typedef DWORD (*tSendCOM_GET_SIG_LIST)(char *CName,char *List,DWORD *count,DWORD *index);
typedef DWORD (*tSendCOM_SET_VALUES)(char *CName,char *root,DWORD count,BYTE *data,DWORD len);
typedef DWORD (*tFindAllLocalComputers)(void);
typedef DWORD (*tSendCOM_LIST_PATTERNS)(char *CName,DWORD *count,DWORD *array);
typedef DWORD (*tSendCOM_MAKE_ACTIVE_PATTERN)(char *CName,DWORD pattern);
typedef char (*tResolveAddress)(char *name, CBA_NETADDR *rawAddress, unsigned short *prot);
typedef int  (*tCBASendPing)(unsigned long dstAppID, unsigned long srcAppID, char *pName, CBA_NETADDR *pNetAddr, unsigned char protocol, unsigned long instanceData, CBA_XLIST *pXlist, unsigned char xcludeTime);


DWORD InitializeTransman();
DWORD DeinitializeTransman();


extern tSendCOM_ACT_ON_FILE pfSendCOM_ACT_ON_FILE;
extern tPutVal pfPutVal;
extern tSendCOM_EXEC_PROGRAM pfSendCOM_EXEC_PROGRAM;
extern tMasterFileCopy pfMasterFileCopy;
extern tVerifyAnAddress pfVerifyAnAddress;
extern tSendCOM_CLOSE_FILE pfSendCOM_CLOSE_FILE;
extern tMakeCBAAddrString pfMakeCBAAddrString;
extern tSendCOM_GET_ADDRESS_INFO pfSendCOM_GET_ADDRESS_INFO;
extern tSendCOM_DEL_VALUE pfSendCOM_DEL_VALUE;
extern tSendCOM_START_SCAN pfSendCOM_START_SCAN;
extern tSendCOM_READ_FILE pfSendCOM_READ_FILE;
extern tSendCOM_OPEN_FILE pfSendCOM_OPEN_FILE;
extern tSendCOM_DEL_KEY pfSendCOM_DEL_KEY;
extern tSendCOM_GET_VALUES pfSendCOM_GET_VALUES;
extern tSendCOM_LIST_VALUE_BLOCK pfSendCOM_LIST_VALUE_BLOCK;
extern tSendCOM_DIR_BLOCK pfSendCOM_DIR_BLOCK;
extern tSendCOM_DEL_CONSOLE pfSendCOM_DEL_CONSOLE;
extern tSendCOM_REMOVE_PROGRAM pfSendCOM_REMOVE_PROGRAM;
extern tSendCOM_ADD_CONSOLE pfSendCOM_ADD_CONSOLE;
extern tDeinitTransman pfDeinitTransman;
extern tInitTransman pfInitTransman;
extern tAbortFind pfAbortFind;
extern tFindAllNTServers pfFindAllNTServers;
extern tFindAllNetwareServers pfFindAllNetwareServers;
extern tSendDomainPing pfSendDomainPing;
extern tSendSinglePing pfSendSinglePing;
extern tLocateServer pfLocateServer;
extern tCvtCBA2GRC pfCvtCBA2GRC;
extern tGetData pfGetData;
extern tGetValueFromEnumBlock pfGetValueFromEnumBlock;
extern tGetValueFromBlock pfGetValueFromBlock;
extern tAddValueToBlock pfAddValueToBlock;
extern tGetVal pfGetVal;
extern tGetStr pfGetStr;
extern tSendCOM_LIST_KEY_BLOCK  pfSendCOM_LIST_KEY_BLOCK;
extern tSendCOM_GET_SIG_LIST pfSendCOM_GET_SIG_LIST;
extern tFindAllLocalComputers pfFindAllLocalComputers;
extern tSendCOM_LIST_PATTERNS pfSendCOM_LIST_PATTERNS;
extern tSendCOM_MAKE_ACTIVE_PATTERN pfSendCOM_MAKE_ACTIVE_PATTERN;
extern tSendCOM_SET_VALUES pfSendCOM_SET_VALUES;

extern tResolveAddress pfResolveAddress;
extern tCBASendPing pfSendCBAPing;
extern tSendCOM_READ_GROUP_SETTINGS pfSendCOM_READ_GROUP_SETTINGS;
extern tSendCOM_WRITE_GROUP_SETTINGS pfSendCOM_WRITE_GROUP_SETTINGS;
