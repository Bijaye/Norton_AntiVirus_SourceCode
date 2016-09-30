// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------------------
// Provides Registry services over IPC.
//----------------------------------------------------------------------------


#ifndef REGISTRY_IPC_H_INCLUDED
#define REGISTRY_IPC_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif


#define REGIPC_SERVER 1
#define REGIPC_CLIENT 2

#define REGIPC_OP_OPEN_KEY                  10
#define REGIPC_OP_CREATE_KEY                11
#define REGIPC_OP_CREATE_KEY_EX             12
#define REGIPC_OP_DELETE_KEY                13
#define REGIPC_OP_DELETE_VALUE              14
#define REGIPC_OP_CLOSE_KEY                 15
#define REGIPC_OP_SET_VALUE_EX              16
#define REGIPC_OP_QUERY_VALUE_EX            17
#define REGIPC_OP_ENUM_VALUE                18
#define REGIPC_OP_ENUM_KEY_EX               19
#define REGIPC_OP_ENUM_KEY                  20
#define REGIPC_OP_FLUSH_KEY                 21
#define REGIPC_OP_NOTIFY_CHANGE_KEY_VALUE   22
#define REGIPC_OP_DUPLICATE_HANDLE          23
#define REGIPC_OP_OPEN_KEY_EX               24

#define REGIPC_MAX_PAYLOAD 10240

typedef struct {
  long mtype;
  DWORD opcode;
  DWORD size;
  char payload[REGIPC_MAX_PAYLOAD];
} vpRegMsgbuf;



#ifdef __cplusplus
}
#endif

#endif // REGISTRY_IPC_H_INCLUDED
