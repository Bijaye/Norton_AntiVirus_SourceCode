////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////


/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Tue Jul 12 14:08:05 2005
 */
/* Compiler settings for \work\bb\qatools\clientevtgen\cc\include\ccevtmgr\ccEventManager.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */
#pragma warning( disable: 4211 )  /* redefine extent to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 475
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "ccEventManager_h.h"

#define TYPE_FORMAT_STRING_SIZE   1141                              
#define PROC_FORMAT_STRING_SIZE   1975                              
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   2            

typedef struct _MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } MIDL_TYPE_FORMAT_STRING;

typedef struct _MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } MIDL_PROC_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString;
extern const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEventEx_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEventEx_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISerializableEventEx_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISerializableEventEx_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISubscriberEx_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISubscriberEx_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IProviderEx_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IProviderEx_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IEventManager_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IEventManager_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ILogManager_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ILogManager_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ILogManager2_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ILogManager2_ProxyInfo;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO IModuleManager_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO IModuleManager_ProxyInfo;


extern const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ];

#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT50_OR_LATER)
#error You need a Windows 2000 or later to run this stub because it uses these features:
#error   /robust command line switch.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will die there with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const MIDL_PROC_FORMAT_STRING __MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure GetType */

			0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x7 ),	/* 7 */
/*  8 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 10 */	NdrFcShort( 0x0 ),	/* 0 */
/* 12 */	NdrFcShort( 0x24 ),	/* 36 */
/* 14 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 16 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 18 */	NdrFcShort( 0x0 ),	/* 0 */
/* 20 */	NdrFcShort( 0x0 ),	/* 0 */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pRet */

/* 24 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 26 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 28 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 30 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 32 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 34 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsTypeSupported */

/* 36 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 38 */	NdrFcLong( 0x0 ),	/* 0 */
/* 42 */	NdrFcShort( 0x8 ),	/* 8 */
/* 44 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 46 */	NdrFcShort( 0x8 ),	/* 8 */
/* 48 */	NdrFcShort( 0x22 ),	/* 34 */
/* 50 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 52 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 54 */	NdrFcShort( 0x0 ),	/* 0 */
/* 56 */	NdrFcShort( 0x0 ),	/* 0 */
/* 58 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 60 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 62 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 64 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pRet */

/* 66 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 68 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 70 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 72 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 74 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 76 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetEventId */

/* 78 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 80 */	NdrFcLong( 0x0 ),	/* 0 */
/* 84 */	NdrFcShort( 0x9 ),	/* 9 */
/* 86 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 88 */	NdrFcShort( 0x0 ),	/* 0 */
/* 90 */	NdrFcShort( 0x24 ),	/* 36 */
/* 92 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 94 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 96 */	NdrFcShort( 0x0 ),	/* 0 */
/* 98 */	NdrFcShort( 0x0 ),	/* 0 */
/* 100 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pRet */

/* 102 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 104 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 106 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 108 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 110 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 112 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetState */

/* 114 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 116 */	NdrFcLong( 0x0 ),	/* 0 */
/* 120 */	NdrFcShort( 0xa ),	/* 10 */
/* 122 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 124 */	NdrFcShort( 0x0 ),	/* 0 */
/* 126 */	NdrFcShort( 0x24 ),	/* 36 */
/* 128 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 130 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 132 */	NdrFcShort( 0x0 ),	/* 0 */
/* 134 */	NdrFcShort( 0x0 ),	/* 0 */
/* 136 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pRet */

/* 138 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 140 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 142 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 144 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 146 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 148 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetProviderId */

/* 150 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 152 */	NdrFcLong( 0x0 ),	/* 0 */
/* 156 */	NdrFcShort( 0xb ),	/* 11 */
/* 158 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 160 */	NdrFcShort( 0x0 ),	/* 0 */
/* 162 */	NdrFcShort( 0x24 ),	/* 36 */
/* 164 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 166 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 168 */	NdrFcShort( 0x0 ),	/* 0 */
/* 170 */	NdrFcShort( 0x0 ),	/* 0 */
/* 172 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pRet */

/* 174 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 176 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 178 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 180 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 182 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 184 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetBroadcast */

/* 186 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 188 */	NdrFcLong( 0x0 ),	/* 0 */
/* 192 */	NdrFcShort( 0xc ),	/* 12 */
/* 194 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 196 */	NdrFcShort( 0x0 ),	/* 0 */
/* 198 */	NdrFcShort( 0x22 ),	/* 34 */
/* 200 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 202 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 204 */	NdrFcShort( 0x0 ),	/* 0 */
/* 206 */	NdrFcShort( 0x0 ),	/* 0 */
/* 208 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pRet */

/* 210 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 212 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 214 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 216 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 218 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 220 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetCancelled */

/* 222 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 224 */	NdrFcLong( 0x0 ),	/* 0 */
/* 228 */	NdrFcShort( 0xd ),	/* 13 */
/* 230 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 234 */	NdrFcShort( 0x22 ),	/* 34 */
/* 236 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 238 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 240 */	NdrFcShort( 0x0 ),	/* 0 */
/* 242 */	NdrFcShort( 0x0 ),	/* 0 */
/* 244 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pRet */

/* 246 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 248 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 250 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 252 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 254 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 256 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Load */

/* 258 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 260 */	NdrFcLong( 0x0 ),	/* 0 */
/* 264 */	NdrFcShort( 0xe ),	/* 14 */
/* 266 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 268 */	NdrFcShort( 0x1c ),	/* 28 */
/* 270 */	NdrFcShort( 0x40 ),	/* 64 */
/* 272 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 274 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 276 */	NdrFcShort( 0x0 ),	/* 0 */
/* 278 */	NdrFcShort( 0x20 ),	/* 32 */
/* 280 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter vData */

/* 282 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 284 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 286 */	NdrFcShort( 0x408 ),	/* Type Offset=1032 */

	/* Parameter pRead */

/* 288 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 290 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 292 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 294 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 296 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 298 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 300 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 302 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 304 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Save */

/* 306 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 308 */	NdrFcLong( 0x0 ),	/* 0 */
/* 312 */	NdrFcShort( 0xf ),	/* 15 */
/* 314 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 316 */	NdrFcShort( 0x1c ),	/* 28 */
/* 318 */	NdrFcShort( 0x40 ),	/* 64 */
/* 320 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 322 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 324 */	NdrFcShort( 0x20 ),	/* 32 */
/* 326 */	NdrFcShort( 0x20 ),	/* 32 */
/* 328 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pData */

/* 330 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 332 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 334 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Parameter pWritten */

/* 336 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 338 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 340 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 342 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 344 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 346 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 348 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 350 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 352 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetSizeMax */

/* 354 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 356 */	NdrFcLong( 0x0 ),	/* 0 */
/* 360 */	NdrFcShort( 0x10 ),	/* 16 */
/* 362 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 364 */	NdrFcShort( 0x1c ),	/* 28 */
/* 366 */	NdrFcShort( 0x40 ),	/* 64 */
/* 368 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 370 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 372 */	NdrFcShort( 0x0 ),	/* 0 */
/* 374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 376 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pSize */

/* 378 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 380 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 382 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 384 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 386 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 388 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 390 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 392 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 394 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnEvent */

/* 396 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 398 */	NdrFcLong( 0x0 ),	/* 0 */
/* 402 */	NdrFcShort( 0x7 ),	/* 7 */
/* 404 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 406 */	NdrFcShort( 0x24 ),	/* 36 */
/* 408 */	NdrFcShort( 0x40 ),	/* 64 */
/* 410 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x5,		/* 5 */
/* 412 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 414 */	NdrFcShort( 0x0 ),	/* 0 */
/* 416 */	NdrFcShort( 0x0 ),	/* 0 */
/* 418 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nSubscriberId */

/* 420 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 422 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 424 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pEvent */

/* 426 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 428 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 430 */	NdrFcShort( 0x428 ),	/* Type Offset=1064 */

	/* Parameter pAction */

/* 432 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 434 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 436 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 438 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 440 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 442 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 444 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 446 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 448 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnShutdown */


	/* Procedure OnShutdown */

/* 450 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 452 */	NdrFcLong( 0x0 ),	/* 0 */
/* 456 */	NdrFcShort( 0x8 ),	/* 8 */
/* 458 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 460 */	NdrFcShort( 0x0 ),	/* 0 */
/* 462 */	NdrFcShort( 0x24 ),	/* 36 */
/* 464 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x2,		/* 2 */
/* 466 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 468 */	NdrFcShort( 0x0 ),	/* 0 */
/* 470 */	NdrFcShort( 0x0 ),	/* 0 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pError */


	/* Parameter pError */

/* 474 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 476 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 478 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */


	/* Return value */

/* 480 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 482 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 484 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure OnEvent */

/* 486 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 488 */	NdrFcLong( 0x0 ),	/* 0 */
/* 492 */	NdrFcShort( 0x7 ),	/* 7 */
/* 494 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 496 */	NdrFcShort( 0x8 ),	/* 8 */
/* 498 */	NdrFcShort( 0x24 ),	/* 36 */
/* 500 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 502 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 504 */	NdrFcShort( 0x0 ),	/* 0 */
/* 506 */	NdrFcShort( 0x0 ),	/* 0 */
/* 508 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nProviderId */

/* 510 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 512 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 514 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pEvent */

/* 516 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 518 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 520 */	NdrFcShort( 0x428 ),	/* Type Offset=1064 */

	/* Parameter pError */

/* 522 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 524 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 526 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 528 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 530 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 532 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RegisterProvider */

/* 534 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 536 */	NdrFcLong( 0x0 ),	/* 0 */
/* 540 */	NdrFcShort( 0x7 ),	/* 7 */
/* 542 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 544 */	NdrFcShort( 0x1c ),	/* 28 */
/* 546 */	NdrFcShort( 0x40 ),	/* 64 */
/* 548 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 550 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 556 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pProvider */

/* 558 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 560 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 562 */	NdrFcShort( 0x43a ),	/* Type Offset=1082 */

	/* Parameter pProviderId */

/* 564 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 566 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 568 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 570 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 572 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 574 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 576 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 578 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 580 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnregisterProvider */

/* 582 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 584 */	NdrFcLong( 0x0 ),	/* 0 */
/* 588 */	NdrFcShort( 0x8 ),	/* 8 */
/* 590 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 592 */	NdrFcShort( 0x8 ),	/* 8 */
/* 594 */	NdrFcShort( 0x24 ),	/* 36 */
/* 596 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 598 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 602 */	NdrFcShort( 0x0 ),	/* 0 */
/* 604 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nProviderId */

/* 606 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 608 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 610 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 612 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 614 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 616 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 618 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 620 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 622 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure RegisterSubscriber */

/* 624 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 626 */	NdrFcLong( 0x0 ),	/* 0 */
/* 630 */	NdrFcShort( 0x9 ),	/* 9 */
/* 632 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 634 */	NdrFcShort( 0x3a ),	/* 58 */
/* 636 */	NdrFcShort( 0x40 ),	/* 64 */
/* 638 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x9,		/* 9 */
/* 640 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 642 */	NdrFcShort( 0x0 ),	/* 0 */
/* 644 */	NdrFcShort( 0x20 ),	/* 32 */
/* 646 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter pSubscriber */

/* 648 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 650 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 652 */	NdrFcShort( 0x44c ),	/* Type Offset=1100 */

	/* Parameter nEventType */

/* 654 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 656 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 658 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nPriority */

/* 660 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 662 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 664 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter vEventStates */

/* 666 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 668 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 670 */	NdrFcShort( 0x408 ),	/* Type Offset=1032 */

	/* Parameter nProviderId */

/* 672 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 674 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 676 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter bMonitorOnly */

/* 678 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 680 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 682 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter pSubscriberId */

/* 684 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 686 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 688 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 690 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 692 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 694 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 696 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 698 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 700 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure UnregisterSubscriber */

/* 702 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 704 */	NdrFcLong( 0x0 ),	/* 0 */
/* 708 */	NdrFcShort( 0xa ),	/* 10 */
/* 710 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 712 */	NdrFcShort( 0x8 ),	/* 8 */
/* 714 */	NdrFcShort( 0x24 ),	/* 36 */
/* 716 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 718 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 720 */	NdrFcShort( 0x0 ),	/* 0 */
/* 722 */	NdrFcShort( 0x0 ),	/* 0 */
/* 724 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nSubscriberId */

/* 726 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 728 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 730 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 732 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 734 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 736 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 738 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 740 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 742 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CreateEvent */

/* 744 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 746 */	NdrFcLong( 0x0 ),	/* 0 */
/* 750 */	NdrFcShort( 0xb ),	/* 11 */
/* 752 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 754 */	NdrFcShort( 0x2a ),	/* 42 */
/* 756 */	NdrFcShort( 0x40 ),	/* 64 */
/* 758 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x6,		/* 6 */
/* 760 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 762 */	NdrFcShort( 0x0 ),	/* 0 */
/* 764 */	NdrFcShort( 0x0 ),	/* 0 */
/* 766 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nProviderId */

/* 768 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 770 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 772 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pEvent */

/* 774 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 776 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 778 */	NdrFcShort( 0x428 ),	/* Type Offset=1064 */

	/* Parameter bBroadcast */

/* 780 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 782 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 784 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter pEventId */

/* 786 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 788 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 790 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 792 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 794 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 796 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 798 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 800 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 802 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CommitEvent */

/* 804 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 806 */	NdrFcLong( 0x0 ),	/* 0 */
/* 810 */	NdrFcShort( 0xc ),	/* 12 */
/* 812 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 814 */	NdrFcShort( 0x8 ),	/* 8 */
/* 816 */	NdrFcShort( 0x24 ),	/* 36 */
/* 818 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 820 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 822 */	NdrFcShort( 0x0 ),	/* 0 */
/* 824 */	NdrFcShort( 0x0 ),	/* 0 */
/* 826 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nSubscriberId */

/* 828 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 830 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 832 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pEvent */

/* 834 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 836 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 838 */	NdrFcShort( 0x428 ),	/* Type Offset=1064 */

	/* Parameter pError */

/* 840 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 842 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 844 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 846 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 848 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 850 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure CancelEvent */

/* 852 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 854 */	NdrFcLong( 0x0 ),	/* 0 */
/* 858 */	NdrFcShort( 0xd ),	/* 13 */
/* 860 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 862 */	NdrFcShort( 0x8 ),	/* 8 */
/* 864 */	NdrFcShort( 0x24 ),	/* 36 */
/* 866 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 868 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 872 */	NdrFcShort( 0x0 ),	/* 0 */
/* 874 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nSubscriberId */

/* 876 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 878 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 880 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pEvent */

/* 882 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 884 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 886 */	NdrFcShort( 0x428 ),	/* Type Offset=1064 */

	/* Parameter pError */

/* 888 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 890 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 892 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 894 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 896 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 898 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadEvent */

/* 900 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 902 */	NdrFcLong( 0x0 ),	/* 0 */
/* 906 */	NdrFcShort( 0x7 ),	/* 7 */
/* 908 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 910 */	NdrFcShort( 0x20 ),	/* 32 */
/* 912 */	NdrFcShort( 0x24 ),	/* 36 */
/* 914 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x7,		/* 7 */
/* 916 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 918 */	NdrFcShort( 0x0 ),	/* 0 */
/* 920 */	NdrFcShort( 0x0 ),	/* 0 */
/* 922 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 924 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 926 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 928 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nIndex */

/* 930 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 932 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 934 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nIndexCheck */

/* 936 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 938 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 940 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nContextId */

/* 942 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 944 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 946 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pEvent */

/* 948 */	NdrFcShort( 0x201b ),	/* Flags:  must size, must free, in, out, srv alloc size=8 */
/* 950 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 952 */	NdrFcShort( 0x45e ),	/* Type Offset=1118 */

	/* Parameter pError */

/* 954 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 956 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 958 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 960 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 962 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 964 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DeleteEvent */

/* 966 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 968 */	NdrFcLong( 0x0 ),	/* 0 */
/* 972 */	NdrFcShort( 0x8 ),	/* 8 */
/* 974 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 976 */	NdrFcShort( 0x18 ),	/* 24 */
/* 978 */	NdrFcShort( 0x24 ),	/* 36 */
/* 980 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 982 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 984 */	NdrFcShort( 0x0 ),	/* 0 */
/* 986 */	NdrFcShort( 0x0 ),	/* 0 */
/* 988 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 990 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 992 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 994 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nIndex */

/* 996 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 998 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1000 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nIndexCheck */

/* 1002 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1004 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1006 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1008 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1010 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1012 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1014 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1016 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1018 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SearchEvents */

/* 1020 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1022 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1026 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1028 */	NdrFcShort( 0x40 ),	/* x86 Stack size/offset = 64 */
/* 1030 */	NdrFcShort( 0x6e ),	/* 110 */
/* 1032 */	NdrFcShort( 0x5c ),	/* 92 */
/* 1034 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0xd,		/* 13 */
/* 1036 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1038 */	NdrFcShort( 0x80 ),	/* 128 */
/* 1040 */	NdrFcShort( 0x80 ),	/* 128 */
/* 1042 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 1044 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1046 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1048 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dtTimeStart */

/* 1050 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1052 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1054 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter dtTimeStop */

/* 1056 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1058 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1060 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter bCreateContext */

/* 1062 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1064 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1066 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter nLimitCount */

/* 1068 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1070 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1072 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pContextId */

/* 1074 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1076 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1078 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pIndexArray */

/* 1080 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1082 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1084 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Parameter pIndexCheckArray */

/* 1086 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1088 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 1090 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Parameter pTimeStampArray */

/* 1092 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1094 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 1096 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Parameter pSequenceNumberArray */

/* 1098 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1100 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 1102 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Parameter pEventCount */

/* 1104 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1106 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 1108 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1110 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1112 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 1114 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1116 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1118 */	NdrFcShort( 0x3c ),	/* x86 Stack size/offset = 60 */
/* 1120 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DeleteSearchContext */

/* 1122 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1124 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1128 */	NdrFcShort( 0xa ),	/* 10 */
/* 1130 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1132 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1134 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1136 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 1138 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1140 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1142 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1144 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 1146 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1148 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1150 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nContextId */

/* 1152 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1154 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1156 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1158 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1160 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1162 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1164 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1166 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1168 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure ClearLog */

/* 1170 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1172 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1176 */	NdrFcShort( 0xb ),	/* 11 */
/* 1178 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1180 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1182 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1184 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x3,		/* 3 */
/* 1186 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1188 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1190 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1192 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 1194 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1196 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1198 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1200 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1202 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1204 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1206 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1208 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1210 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetMaxLogSize */

/* 1212 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1214 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1218 */	NdrFcShort( 0xc ),	/* 12 */
/* 1220 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1222 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1224 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1226 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 1228 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1230 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1232 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1234 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 1236 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1238 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1240 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nSize */

/* 1242 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1244 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1246 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1248 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1250 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1252 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1254 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1256 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1258 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMaxLogSize */

/* 1260 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1262 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1266 */	NdrFcShort( 0xd ),	/* 13 */
/* 1268 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1270 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1272 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1274 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 1276 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1278 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1280 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1282 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 1284 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1286 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1288 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pSize */

/* 1290 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1292 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1294 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1296 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1298 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1300 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1302 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1304 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1306 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetLogEnabled */

/* 1308 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1310 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1314 */	NdrFcShort( 0xe ),	/* 14 */
/* 1316 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1318 */	NdrFcShort( 0xe ),	/* 14 */
/* 1320 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1322 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 1324 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1326 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1328 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1330 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 1332 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1334 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1336 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter bEnabled */

/* 1338 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1340 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1342 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1344 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1346 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1348 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1350 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1352 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1354 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetLogEnabled */

/* 1356 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1358 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1362 */	NdrFcShort( 0xf ),	/* 15 */
/* 1364 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1366 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1368 */	NdrFcShort( 0x3e ),	/* 62 */
/* 1370 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x4,		/* 4 */
/* 1372 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1374 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1376 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1378 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 1380 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1382 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1384 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pEnabled */

/* 1386 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1388 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1390 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1392 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1394 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1396 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1398 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1400 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1402 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadEvent */

/* 1404 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1406 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1410 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1412 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1414 */	NdrFcShort( 0x28 ),	/* 40 */
/* 1416 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1418 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x8,		/* 8 */
/* 1420 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1422 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1424 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1426 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 1428 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1430 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1432 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nLowIndex */

/* 1434 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1436 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1438 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nHighIndex */

/* 1440 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1442 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1444 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nIndexCheck */

/* 1446 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1448 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1450 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nContextId */

/* 1452 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1454 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1456 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pEvent */

/* 1458 */	NdrFcShort( 0x201b ),	/* Flags:  must size, must free, in, out, srv alloc size=8 */
/* 1460 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1462 */	NdrFcShort( 0x45e ),	/* Type Offset=1118 */

	/* Parameter pError */

/* 1464 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1466 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1468 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1470 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1472 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1474 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure DeleteEvent */

/* 1476 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1478 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1482 */	NdrFcShort( 0x11 ),	/* 17 */
/* 1484 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1486 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1488 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1490 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x6,		/* 6 */
/* 1492 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1494 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1496 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1498 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 1500 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1502 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1504 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nLowIndex */

/* 1506 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1508 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1510 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nHighIndex */

/* 1512 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1514 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1516 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nIndexCheck */

/* 1518 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1520 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1522 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1524 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1526 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1528 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1530 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1532 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1534 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SetMaxLogSize */

/* 1536 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1538 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1542 */	NdrFcShort( 0x12 ),	/* 18 */
/* 1544 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1546 */	NdrFcShort( 0x18 ),	/* 24 */
/* 1548 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1550 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 1552 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1554 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1556 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1558 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 1560 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1562 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1564 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nLowSize */

/* 1566 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1568 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1570 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter nHighSize */

/* 1572 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1574 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1576 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1578 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1580 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1582 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1584 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1586 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1588 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure GetMaxLogSize */

/* 1590 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1592 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1596 */	NdrFcShort( 0x13 ),	/* 19 */
/* 1598 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1600 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1602 */	NdrFcShort( 0x5c ),	/* 92 */
/* 1604 */	0x44,		/* Oi2 Flags:  has return, has ext, */
			0x5,		/* 5 */
/* 1606 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 1608 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1610 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1612 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 1614 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1616 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1618 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pLowSize */

/* 1620 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1622 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1624 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pHighSize */

/* 1626 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1628 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1630 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1632 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1634 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1636 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1638 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1640 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1642 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure SearchEvents */

/* 1644 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1646 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1650 */	NdrFcShort( 0x14 ),	/* 20 */
/* 1652 */	NdrFcShort( 0x48 ),	/* x86 Stack size/offset = 72 */
/* 1654 */	NdrFcShort( 0x6e ),	/* 110 */
/* 1656 */	NdrFcShort( 0x5c ),	/* 92 */
/* 1658 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0xf,		/* 15 */
/* 1660 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1662 */	NdrFcShort( 0xc0 ),	/* 192 */
/* 1664 */	NdrFcShort( 0xc0 ),	/* 192 */
/* 1666 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 1668 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1670 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1672 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter dtTimeStart */

/* 1674 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1676 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1678 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter dtTimeStop */

/* 1680 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1682 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1684 */	0xc,		/* FC_DOUBLE */
			0x0,		/* 0 */

	/* Parameter bCreateContext */

/* 1686 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1688 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1690 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter nLimitCount */

/* 1692 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1694 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1696 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pContextId */

/* 1698 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1700 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1702 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pIndexLowArray */

/* 1704 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1706 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1708 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Parameter pIndexHighArray */

/* 1710 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1712 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 1714 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Parameter pIndexCheckArray */

/* 1716 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1718 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 1720 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Parameter pEventTypeArray */

/* 1722 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1724 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 1726 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Parameter pTimeStampArray */

/* 1728 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1730 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 1732 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Parameter pSequenceNumberArray */

/* 1734 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1736 */	NdrFcShort( 0x38 ),	/* x86 Stack size/offset = 56 */
/* 1738 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Parameter pEventCount */

/* 1740 */	NdrFcShort( 0x158 ),	/* Flags:  in, out, base type, simple ref, */
/* 1742 */	NdrFcShort( 0x3c ),	/* x86 Stack size/offset = 60 */
/* 1744 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1746 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1748 */	NdrFcShort( 0x40 ),	/* x86 Stack size/offset = 64 */
/* 1750 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1752 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1754 */	NdrFcShort( 0x44 ),	/* x86 Stack size/offset = 68 */
/* 1756 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure LoadEventBatch */

/* 1758 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1760 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1764 */	NdrFcShort( 0x15 ),	/* 21 */
/* 1766 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 1768 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1770 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1772 */	0x47,		/* Oi2 Flags:  srv must size, clt must size, has return, has ext, */
			0x9,		/* 9 */
/* 1774 */	0x8,		/* 8 */
			0x7,		/* Ext Flags:  new corr desc, clt corr check, srv corr check, */
/* 1776 */	NdrFcShort( 0x40 ),	/* 64 */
/* 1778 */	NdrFcShort( 0xa0 ),	/* 160 */
/* 1780 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter nEventType */

/* 1782 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1784 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1786 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pIndexLowArray */

/* 1788 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1790 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1792 */	NdrFcShort( 0x408 ),	/* Type Offset=1032 */

	/* Parameter pIndexHighArray */

/* 1794 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1796 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1798 */	NdrFcShort( 0x408 ),	/* Type Offset=1032 */

	/* Parameter pIndexCheckArray */

/* 1800 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 1802 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1804 */	NdrFcShort( 0x408 ),	/* Type Offset=1032 */

	/* Parameter nContextId */

/* 1806 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1808 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1810 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter pEventTypeArray */

/* 1812 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1814 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 1816 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Parameter pEventArray */

/* 1818 */	NdrFcShort( 0x11b ),	/* Flags:  must size, must free, in, out, simple ref, */
/* 1820 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 1822 */	NdrFcShort( 0x41e ),	/* Type Offset=1054 */

	/* Parameter pError */

/* 1824 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1826 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 1828 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1830 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1832 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 1834 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Load */

/* 1836 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1838 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1842 */	NdrFcShort( 0x7 ),	/* 7 */
/* 1844 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1846 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1848 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1850 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1852 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1854 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1856 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1858 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter sModuleName */

/* 1860 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1862 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1864 */	NdrFcShort( 0x46a ),	/* Type Offset=1130 */

	/* Parameter bPersist */

/* 1866 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1868 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1870 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1872 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1874 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1876 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1878 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1880 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1882 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure Unload */

/* 1884 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1886 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1890 */	NdrFcShort( 0x8 ),	/* 8 */
/* 1892 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 1894 */	NdrFcShort( 0x6 ),	/* 6 */
/* 1896 */	NdrFcShort( 0x24 ),	/* 36 */
/* 1898 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x4,		/* 4 */
/* 1900 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1902 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1904 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1906 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter sModuleName */

/* 1908 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1910 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1912 */	NdrFcShort( 0x46a ),	/* Type Offset=1130 */

	/* Parameter bPersist */

/* 1914 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 1916 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1918 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Parameter pError */

/* 1920 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1922 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1924 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 1926 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1928 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1930 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Procedure IsLoaded */

/* 1932 */	0x33,		/* FC_AUTO_HANDLE */
			0x6c,		/* Old Flags:  object, Oi2 */
/* 1934 */	NdrFcLong( 0x0 ),	/* 0 */
/* 1938 */	NdrFcShort( 0x9 ),	/* 9 */
/* 1940 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 1942 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1944 */	NdrFcShort( 0x22 ),	/* 34 */
/* 1946 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0x3,		/* 3 */
/* 1948 */	0x8,		/* 8 */
			0x5,		/* Ext Flags:  new corr desc, srv corr check, */
/* 1950 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1952 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1954 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter sModuleName */

/* 1956 */	NdrFcShort( 0x8b ),	/* Flags:  must size, must free, in, by val, */
/* 1958 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 1960 */	NdrFcShort( 0x46a ),	/* Type Offset=1130 */

	/* Parameter pLoaded */

/* 1962 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 1964 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 1966 */	0x6,		/* FC_SHORT */
			0x0,		/* 0 */

	/* Return value */

/* 1968 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 1970 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 1972 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const MIDL_TYPE_FORMAT_STRING __MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  4 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/*  8 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 10 */	
			0x12, 0x0,	/* FC_UP */
/* 12 */	NdrFcShort( 0x3e8 ),	/* Offset= 1000 (1012) */
/* 14 */	
			0x2b,		/* FC_NON_ENCAPSULATED_UNION */
			0x9,		/* FC_ULONG */
/* 16 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 18 */	NdrFcShort( 0xfff8 ),	/* -8 */
/* 20 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 22 */	NdrFcShort( 0x2 ),	/* Offset= 2 (24) */
/* 24 */	NdrFcShort( 0x10 ),	/* 16 */
/* 26 */	NdrFcShort( 0x2f ),	/* 47 */
/* 28 */	NdrFcLong( 0x14 ),	/* 20 */
/* 32 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 34 */	NdrFcLong( 0x3 ),	/* 3 */
/* 38 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 40 */	NdrFcLong( 0x11 ),	/* 17 */
/* 44 */	NdrFcShort( 0x8001 ),	/* Simple arm type: FC_BYTE */
/* 46 */	NdrFcLong( 0x2 ),	/* 2 */
/* 50 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 52 */	NdrFcLong( 0x4 ),	/* 4 */
/* 56 */	NdrFcShort( 0x800a ),	/* Simple arm type: FC_FLOAT */
/* 58 */	NdrFcLong( 0x5 ),	/* 5 */
/* 62 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 64 */	NdrFcLong( 0xb ),	/* 11 */
/* 68 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 70 */	NdrFcLong( 0xa ),	/* 10 */
/* 74 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 76 */	NdrFcLong( 0x6 ),	/* 6 */
/* 80 */	NdrFcShort( 0xe8 ),	/* Offset= 232 (312) */
/* 82 */	NdrFcLong( 0x7 ),	/* 7 */
/* 86 */	NdrFcShort( 0x800c ),	/* Simple arm type: FC_DOUBLE */
/* 88 */	NdrFcLong( 0x8 ),	/* 8 */
/* 92 */	NdrFcShort( 0xe2 ),	/* Offset= 226 (318) */
/* 94 */	NdrFcLong( 0xd ),	/* 13 */
/* 98 */	NdrFcShort( 0xf6 ),	/* Offset= 246 (344) */
/* 100 */	NdrFcLong( 0x9 ),	/* 9 */
/* 104 */	NdrFcShort( 0x102 ),	/* Offset= 258 (362) */
/* 106 */	NdrFcLong( 0x2000 ),	/* 8192 */
/* 110 */	NdrFcShort( 0x10e ),	/* Offset= 270 (380) */
/* 112 */	NdrFcLong( 0x24 ),	/* 36 */
/* 116 */	NdrFcShort( 0x336 ),	/* Offset= 822 (938) */
/* 118 */	NdrFcLong( 0x4024 ),	/* 16420 */
/* 122 */	NdrFcShort( 0x330 ),	/* Offset= 816 (938) */
/* 124 */	NdrFcLong( 0x4011 ),	/* 16401 */
/* 128 */	NdrFcShort( 0x32e ),	/* Offset= 814 (942) */
/* 130 */	NdrFcLong( 0x4002 ),	/* 16386 */
/* 134 */	NdrFcShort( 0x32c ),	/* Offset= 812 (946) */
/* 136 */	NdrFcLong( 0x4003 ),	/* 16387 */
/* 140 */	NdrFcShort( 0x32a ),	/* Offset= 810 (950) */
/* 142 */	NdrFcLong( 0x4014 ),	/* 16404 */
/* 146 */	NdrFcShort( 0x328 ),	/* Offset= 808 (954) */
/* 148 */	NdrFcLong( 0x4004 ),	/* 16388 */
/* 152 */	NdrFcShort( 0x326 ),	/* Offset= 806 (958) */
/* 154 */	NdrFcLong( 0x4005 ),	/* 16389 */
/* 158 */	NdrFcShort( 0x324 ),	/* Offset= 804 (962) */
/* 160 */	NdrFcLong( 0x400b ),	/* 16395 */
/* 164 */	NdrFcShort( 0x30e ),	/* Offset= 782 (946) */
/* 166 */	NdrFcLong( 0x400a ),	/* 16394 */
/* 170 */	NdrFcShort( 0x30c ),	/* Offset= 780 (950) */
/* 172 */	NdrFcLong( 0x4006 ),	/* 16390 */
/* 176 */	NdrFcShort( 0x316 ),	/* Offset= 790 (966) */
/* 178 */	NdrFcLong( 0x4007 ),	/* 16391 */
/* 182 */	NdrFcShort( 0x30c ),	/* Offset= 780 (962) */
/* 184 */	NdrFcLong( 0x4008 ),	/* 16392 */
/* 188 */	NdrFcShort( 0x30e ),	/* Offset= 782 (970) */
/* 190 */	NdrFcLong( 0x400d ),	/* 16397 */
/* 194 */	NdrFcShort( 0x30c ),	/* Offset= 780 (974) */
/* 196 */	NdrFcLong( 0x4009 ),	/* 16393 */
/* 200 */	NdrFcShort( 0x30a ),	/* Offset= 778 (978) */
/* 202 */	NdrFcLong( 0x6000 ),	/* 24576 */
/* 206 */	NdrFcShort( 0x308 ),	/* Offset= 776 (982) */
/* 208 */	NdrFcLong( 0x400c ),	/* 16396 */
/* 212 */	NdrFcShort( 0x306 ),	/* Offset= 774 (986) */
/* 214 */	NdrFcLong( 0x10 ),	/* 16 */
/* 218 */	NdrFcShort( 0x8002 ),	/* Simple arm type: FC_CHAR */
/* 220 */	NdrFcLong( 0x12 ),	/* 18 */
/* 224 */	NdrFcShort( 0x8006 ),	/* Simple arm type: FC_SHORT */
/* 226 */	NdrFcLong( 0x13 ),	/* 19 */
/* 230 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 232 */	NdrFcLong( 0x15 ),	/* 21 */
/* 236 */	NdrFcShort( 0x800b ),	/* Simple arm type: FC_HYPER */
/* 238 */	NdrFcLong( 0x16 ),	/* 22 */
/* 242 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 244 */	NdrFcLong( 0x17 ),	/* 23 */
/* 248 */	NdrFcShort( 0x8008 ),	/* Simple arm type: FC_LONG */
/* 250 */	NdrFcLong( 0xe ),	/* 14 */
/* 254 */	NdrFcShort( 0x2e4 ),	/* Offset= 740 (994) */
/* 256 */	NdrFcLong( 0x400e ),	/* 16398 */
/* 260 */	NdrFcShort( 0x2e8 ),	/* Offset= 744 (1004) */
/* 262 */	NdrFcLong( 0x4010 ),	/* 16400 */
/* 266 */	NdrFcShort( 0x2e6 ),	/* Offset= 742 (1008) */
/* 268 */	NdrFcLong( 0x4012 ),	/* 16402 */
/* 272 */	NdrFcShort( 0x2a2 ),	/* Offset= 674 (946) */
/* 274 */	NdrFcLong( 0x4013 ),	/* 16403 */
/* 278 */	NdrFcShort( 0x2a0 ),	/* Offset= 672 (950) */
/* 280 */	NdrFcLong( 0x4015 ),	/* 16405 */
/* 284 */	NdrFcShort( 0x29e ),	/* Offset= 670 (954) */
/* 286 */	NdrFcLong( 0x4016 ),	/* 16406 */
/* 290 */	NdrFcShort( 0x294 ),	/* Offset= 660 (950) */
/* 292 */	NdrFcLong( 0x4017 ),	/* 16407 */
/* 296 */	NdrFcShort( 0x28e ),	/* Offset= 654 (950) */
/* 298 */	NdrFcLong( 0x0 ),	/* 0 */
/* 302 */	NdrFcShort( 0x0 ),	/* Offset= 0 (302) */
/* 304 */	NdrFcLong( 0x1 ),	/* 1 */
/* 308 */	NdrFcShort( 0x0 ),	/* Offset= 0 (308) */
/* 310 */	NdrFcShort( 0xffff ),	/* Offset= -1 (309) */
/* 312 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 314 */	NdrFcShort( 0x8 ),	/* 8 */
/* 316 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 318 */	
			0x12, 0x0,	/* FC_UP */
/* 320 */	NdrFcShort( 0xe ),	/* Offset= 14 (334) */
/* 322 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 324 */	NdrFcShort( 0x2 ),	/* 2 */
/* 326 */	0x9,		/* Corr desc: FC_ULONG */
			0x0,		/*  */
/* 328 */	NdrFcShort( 0xfffc ),	/* -4 */
/* 330 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 332 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 334 */	
			0x17,		/* FC_CSTRUCT */
			0x3,		/* 3 */
/* 336 */	NdrFcShort( 0x8 ),	/* 8 */
/* 338 */	NdrFcShort( 0xfff0 ),	/* Offset= -16 (322) */
/* 340 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 342 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 344 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 346 */	NdrFcLong( 0x0 ),	/* 0 */
/* 350 */	NdrFcShort( 0x0 ),	/* 0 */
/* 352 */	NdrFcShort( 0x0 ),	/* 0 */
/* 354 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 356 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 358 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 360 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 362 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 364 */	NdrFcLong( 0x20400 ),	/* 132096 */
/* 368 */	NdrFcShort( 0x0 ),	/* 0 */
/* 370 */	NdrFcShort( 0x0 ),	/* 0 */
/* 372 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 374 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 376 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 378 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 380 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 382 */	NdrFcShort( 0x2 ),	/* Offset= 2 (384) */
/* 384 */	
			0x12, 0x0,	/* FC_UP */
/* 386 */	NdrFcShort( 0x216 ),	/* Offset= 534 (920) */
/* 388 */	
			0x2a,		/* FC_ENCAPSULATED_UNION */
			0x49,		/* 73 */
/* 390 */	NdrFcShort( 0x18 ),	/* 24 */
/* 392 */	NdrFcShort( 0xa ),	/* 10 */
/* 394 */	NdrFcLong( 0x8 ),	/* 8 */
/* 398 */	NdrFcShort( 0x5a ),	/* Offset= 90 (488) */
/* 400 */	NdrFcLong( 0xd ),	/* 13 */
/* 404 */	NdrFcShort( 0x7e ),	/* Offset= 126 (530) */
/* 406 */	NdrFcLong( 0x9 ),	/* 9 */
/* 410 */	NdrFcShort( 0x9e ),	/* Offset= 158 (568) */
/* 412 */	NdrFcLong( 0xc ),	/* 12 */
/* 416 */	NdrFcShort( 0xc8 ),	/* Offset= 200 (616) */
/* 418 */	NdrFcLong( 0x24 ),	/* 36 */
/* 422 */	NdrFcShort( 0x124 ),	/* Offset= 292 (714) */
/* 424 */	NdrFcLong( 0x800d ),	/* 32781 */
/* 428 */	NdrFcShort( 0x140 ),	/* Offset= 320 (748) */
/* 430 */	NdrFcLong( 0x10 ),	/* 16 */
/* 434 */	NdrFcShort( 0x15a ),	/* Offset= 346 (780) */
/* 436 */	NdrFcLong( 0x2 ),	/* 2 */
/* 440 */	NdrFcShort( 0x174 ),	/* Offset= 372 (812) */
/* 442 */	NdrFcLong( 0x3 ),	/* 3 */
/* 446 */	NdrFcShort( 0x18e ),	/* Offset= 398 (844) */
/* 448 */	NdrFcLong( 0x14 ),	/* 20 */
/* 452 */	NdrFcShort( 0x1a8 ),	/* Offset= 424 (876) */
/* 454 */	NdrFcShort( 0xffff ),	/* Offset= -1 (453) */
/* 456 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 458 */	NdrFcShort( 0x4 ),	/* 4 */
/* 460 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 462 */	NdrFcShort( 0x0 ),	/* 0 */
/* 464 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 466 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 468 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 470 */	NdrFcShort( 0x4 ),	/* 4 */
/* 472 */	NdrFcShort( 0x0 ),	/* 0 */
/* 474 */	NdrFcShort( 0x1 ),	/* 1 */
/* 476 */	NdrFcShort( 0x0 ),	/* 0 */
/* 478 */	NdrFcShort( 0x0 ),	/* 0 */
/* 480 */	0x12, 0x0,	/* FC_UP */
/* 482 */	NdrFcShort( 0xff6c ),	/* Offset= -148 (334) */
/* 484 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 486 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 488 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 490 */	NdrFcShort( 0x8 ),	/* 8 */
/* 492 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 494 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 496 */	NdrFcShort( 0x4 ),	/* 4 */
/* 498 */	NdrFcShort( 0x4 ),	/* 4 */
/* 500 */	0x11, 0x0,	/* FC_RP */
/* 502 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (456) */
/* 504 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 506 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 508 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 510 */	NdrFcShort( 0x0 ),	/* 0 */
/* 512 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 514 */	NdrFcShort( 0x0 ),	/* 0 */
/* 516 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 518 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 522 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 524 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 526 */	NdrFcShort( 0xff4a ),	/* Offset= -182 (344) */
/* 528 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 530 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 532 */	NdrFcShort( 0x8 ),	/* 8 */
/* 534 */	NdrFcShort( 0x0 ),	/* 0 */
/* 536 */	NdrFcShort( 0x6 ),	/* Offset= 6 (542) */
/* 538 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 540 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 542 */	
			0x11, 0x0,	/* FC_RP */
/* 544 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (508) */
/* 546 */	
			0x21,		/* FC_BOGUS_ARRAY */
			0x3,		/* 3 */
/* 548 */	NdrFcShort( 0x0 ),	/* 0 */
/* 550 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 552 */	NdrFcShort( 0x0 ),	/* 0 */
/* 554 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 556 */	NdrFcLong( 0xffffffff ),	/* -1 */
/* 560 */	NdrFcShort( 0x0 ),	/* Corr flags:  */
/* 562 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 564 */	NdrFcShort( 0xff36 ),	/* Offset= -202 (362) */
/* 566 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 568 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 570 */	NdrFcShort( 0x8 ),	/* 8 */
/* 572 */	NdrFcShort( 0x0 ),	/* 0 */
/* 574 */	NdrFcShort( 0x6 ),	/* Offset= 6 (580) */
/* 576 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 578 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 580 */	
			0x11, 0x0,	/* FC_RP */
/* 582 */	NdrFcShort( 0xffdc ),	/* Offset= -36 (546) */
/* 584 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 586 */	NdrFcShort( 0x4 ),	/* 4 */
/* 588 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 590 */	NdrFcShort( 0x0 ),	/* 0 */
/* 592 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 594 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 596 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 598 */	NdrFcShort( 0x4 ),	/* 4 */
/* 600 */	NdrFcShort( 0x0 ),	/* 0 */
/* 602 */	NdrFcShort( 0x1 ),	/* 1 */
/* 604 */	NdrFcShort( 0x0 ),	/* 0 */
/* 606 */	NdrFcShort( 0x0 ),	/* 0 */
/* 608 */	0x12, 0x0,	/* FC_UP */
/* 610 */	NdrFcShort( 0x192 ),	/* Offset= 402 (1012) */
/* 612 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 614 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 616 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 618 */	NdrFcShort( 0x8 ),	/* 8 */
/* 620 */	NdrFcShort( 0x0 ),	/* 0 */
/* 622 */	NdrFcShort( 0x6 ),	/* Offset= 6 (628) */
/* 624 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 626 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 628 */	
			0x11, 0x0,	/* FC_RP */
/* 630 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (584) */
/* 632 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 634 */	NdrFcLong( 0x2f ),	/* 47 */
/* 638 */	NdrFcShort( 0x0 ),	/* 0 */
/* 640 */	NdrFcShort( 0x0 ),	/* 0 */
/* 642 */	0xc0,		/* 192 */
			0x0,		/* 0 */
/* 644 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 646 */	0x0,		/* 0 */
			0x0,		/* 0 */
/* 648 */	0x0,		/* 0 */
			0x46,		/* 70 */
/* 650 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 652 */	NdrFcShort( 0x1 ),	/* 1 */
/* 654 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 656 */	NdrFcShort( 0x4 ),	/* 4 */
/* 658 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 660 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 662 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 664 */	NdrFcShort( 0x10 ),	/* 16 */
/* 666 */	NdrFcShort( 0x0 ),	/* 0 */
/* 668 */	NdrFcShort( 0xa ),	/* Offset= 10 (678) */
/* 670 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 672 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 674 */	NdrFcShort( 0xffd6 ),	/* Offset= -42 (632) */
/* 676 */	0x36,		/* FC_POINTER */
			0x5b,		/* FC_END */
/* 678 */	
			0x12, 0x0,	/* FC_UP */
/* 680 */	NdrFcShort( 0xffe2 ),	/* Offset= -30 (650) */
/* 682 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 684 */	NdrFcShort( 0x4 ),	/* 4 */
/* 686 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 688 */	NdrFcShort( 0x0 ),	/* 0 */
/* 690 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 692 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 694 */	
			0x48,		/* FC_VARIABLE_REPEAT */
			0x49,		/* FC_FIXED_OFFSET */
/* 696 */	NdrFcShort( 0x4 ),	/* 4 */
/* 698 */	NdrFcShort( 0x0 ),	/* 0 */
/* 700 */	NdrFcShort( 0x1 ),	/* 1 */
/* 702 */	NdrFcShort( 0x0 ),	/* 0 */
/* 704 */	NdrFcShort( 0x0 ),	/* 0 */
/* 706 */	0x12, 0x0,	/* FC_UP */
/* 708 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (662) */
/* 710 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 712 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 714 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 716 */	NdrFcShort( 0x8 ),	/* 8 */
/* 718 */	NdrFcShort( 0x0 ),	/* 0 */
/* 720 */	NdrFcShort( 0x6 ),	/* Offset= 6 (726) */
/* 722 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 724 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 726 */	
			0x11, 0x0,	/* FC_RP */
/* 728 */	NdrFcShort( 0xffd2 ),	/* Offset= -46 (682) */
/* 730 */	
			0x1d,		/* FC_SMFARRAY */
			0x0,		/* 0 */
/* 732 */	NdrFcShort( 0x8 ),	/* 8 */
/* 734 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 736 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 738 */	NdrFcShort( 0x10 ),	/* 16 */
/* 740 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 742 */	0x6,		/* FC_SHORT */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 744 */	0x0,		/* 0 */
			NdrFcShort( 0xfff1 ),	/* Offset= -15 (730) */
			0x5b,		/* FC_END */
/* 748 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 750 */	NdrFcShort( 0x18 ),	/* 24 */
/* 752 */	NdrFcShort( 0x0 ),	/* 0 */
/* 754 */	NdrFcShort( 0xa ),	/* Offset= 10 (764) */
/* 756 */	0x8,		/* FC_LONG */
			0x36,		/* FC_POINTER */
/* 758 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 760 */	NdrFcShort( 0xffe8 ),	/* Offset= -24 (736) */
/* 762 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 764 */	
			0x11, 0x0,	/* FC_RP */
/* 766 */	NdrFcShort( 0xfefe ),	/* Offset= -258 (508) */
/* 768 */	
			0x1b,		/* FC_CARRAY */
			0x0,		/* 0 */
/* 770 */	NdrFcShort( 0x1 ),	/* 1 */
/* 772 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 774 */	NdrFcShort( 0x0 ),	/* 0 */
/* 776 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 778 */	0x1,		/* FC_BYTE */
			0x5b,		/* FC_END */
/* 780 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 782 */	NdrFcShort( 0x8 ),	/* 8 */
/* 784 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 786 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 788 */	NdrFcShort( 0x4 ),	/* 4 */
/* 790 */	NdrFcShort( 0x4 ),	/* 4 */
/* 792 */	0x12, 0x0,	/* FC_UP */
/* 794 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (768) */
/* 796 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 798 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 800 */	
			0x1b,		/* FC_CARRAY */
			0x1,		/* 1 */
/* 802 */	NdrFcShort( 0x2 ),	/* 2 */
/* 804 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 806 */	NdrFcShort( 0x0 ),	/* 0 */
/* 808 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 810 */	0x6,		/* FC_SHORT */
			0x5b,		/* FC_END */
/* 812 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 814 */	NdrFcShort( 0x8 ),	/* 8 */
/* 816 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 818 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 820 */	NdrFcShort( 0x4 ),	/* 4 */
/* 822 */	NdrFcShort( 0x4 ),	/* 4 */
/* 824 */	0x12, 0x0,	/* FC_UP */
/* 826 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (800) */
/* 828 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 830 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 832 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 834 */	NdrFcShort( 0x4 ),	/* 4 */
/* 836 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 838 */	NdrFcShort( 0x0 ),	/* 0 */
/* 840 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 842 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 844 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 846 */	NdrFcShort( 0x8 ),	/* 8 */
/* 848 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 850 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 852 */	NdrFcShort( 0x4 ),	/* 4 */
/* 854 */	NdrFcShort( 0x4 ),	/* 4 */
/* 856 */	0x12, 0x0,	/* FC_UP */
/* 858 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (832) */
/* 860 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 862 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 864 */	
			0x1b,		/* FC_CARRAY */
			0x7,		/* 7 */
/* 866 */	NdrFcShort( 0x8 ),	/* 8 */
/* 868 */	0x19,		/* Corr desc:  field pointer, FC_ULONG */
			0x0,		/*  */
/* 870 */	NdrFcShort( 0x0 ),	/* 0 */
/* 872 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 874 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 876 */	
			0x16,		/* FC_PSTRUCT */
			0x3,		/* 3 */
/* 878 */	NdrFcShort( 0x8 ),	/* 8 */
/* 880 */	
			0x4b,		/* FC_PP */
			0x5c,		/* FC_PAD */
/* 882 */	
			0x46,		/* FC_NO_REPEAT */
			0x5c,		/* FC_PAD */
/* 884 */	NdrFcShort( 0x4 ),	/* 4 */
/* 886 */	NdrFcShort( 0x4 ),	/* 4 */
/* 888 */	0x12, 0x0,	/* FC_UP */
/* 890 */	NdrFcShort( 0xffe6 ),	/* Offset= -26 (864) */
/* 892 */	
			0x5b,		/* FC_END */

			0x8,		/* FC_LONG */
/* 894 */	0x8,		/* FC_LONG */
			0x5b,		/* FC_END */
/* 896 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 898 */	NdrFcShort( 0x8 ),	/* 8 */
/* 900 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 902 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 904 */	
			0x1b,		/* FC_CARRAY */
			0x3,		/* 3 */
/* 906 */	NdrFcShort( 0x8 ),	/* 8 */
/* 908 */	0x7,		/* Corr desc: FC_USHORT */
			0x0,		/*  */
/* 910 */	NdrFcShort( 0xffd8 ),	/* -40 */
/* 912 */	NdrFcShort( 0x1 ),	/* Corr flags:  early, */
/* 914 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 916 */	NdrFcShort( 0xffec ),	/* Offset= -20 (896) */
/* 918 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 920 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 922 */	NdrFcShort( 0x28 ),	/* 40 */
/* 924 */	NdrFcShort( 0xffec ),	/* Offset= -20 (904) */
/* 926 */	NdrFcShort( 0x0 ),	/* Offset= 0 (926) */
/* 928 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 930 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 932 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 934 */	NdrFcShort( 0xfdde ),	/* Offset= -546 (388) */
/* 936 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 938 */	
			0x12, 0x0,	/* FC_UP */
/* 940 */	NdrFcShort( 0xfeea ),	/* Offset= -278 (662) */
/* 942 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 944 */	0x1,		/* FC_BYTE */
			0x5c,		/* FC_PAD */
/* 946 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 948 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 950 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 952 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 954 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 956 */	0xb,		/* FC_HYPER */
			0x5c,		/* FC_PAD */
/* 958 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 960 */	0xa,		/* FC_FLOAT */
			0x5c,		/* FC_PAD */
/* 962 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 964 */	0xc,		/* FC_DOUBLE */
			0x5c,		/* FC_PAD */
/* 966 */	
			0x12, 0x0,	/* FC_UP */
/* 968 */	NdrFcShort( 0xfd70 ),	/* Offset= -656 (312) */
/* 970 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 972 */	NdrFcShort( 0xfd72 ),	/* Offset= -654 (318) */
/* 974 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 976 */	NdrFcShort( 0xfd88 ),	/* Offset= -632 (344) */
/* 978 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 980 */	NdrFcShort( 0xfd96 ),	/* Offset= -618 (362) */
/* 982 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 984 */	NdrFcShort( 0xfda4 ),	/* Offset= -604 (380) */
/* 986 */	
			0x12, 0x10,	/* FC_UP [pointer_deref] */
/* 988 */	NdrFcShort( 0x2 ),	/* Offset= 2 (990) */
/* 990 */	
			0x12, 0x0,	/* FC_UP */
/* 992 */	NdrFcShort( 0x14 ),	/* Offset= 20 (1012) */
/* 994 */	
			0x15,		/* FC_STRUCT */
			0x7,		/* 7 */
/* 996 */	NdrFcShort( 0x10 ),	/* 16 */
/* 998 */	0x6,		/* FC_SHORT */
			0x1,		/* FC_BYTE */
/* 1000 */	0x1,		/* FC_BYTE */
			0x8,		/* FC_LONG */
/* 1002 */	0xb,		/* FC_HYPER */
			0x5b,		/* FC_END */
/* 1004 */	
			0x12, 0x0,	/* FC_UP */
/* 1006 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (994) */
/* 1008 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 1010 */	0x2,		/* FC_CHAR */
			0x5c,		/* FC_PAD */
/* 1012 */	
			0x1a,		/* FC_BOGUS_STRUCT */
			0x7,		/* 7 */
/* 1014 */	NdrFcShort( 0x20 ),	/* 32 */
/* 1016 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1018 */	NdrFcShort( 0x0 ),	/* Offset= 0 (1018) */
/* 1020 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 1022 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1024 */	0x6,		/* FC_SHORT */
			0x6,		/* FC_SHORT */
/* 1026 */	0x4c,		/* FC_EMBEDDED_COMPLEX */
			0x0,		/* 0 */
/* 1028 */	NdrFcShort( 0xfc0a ),	/* Offset= -1014 (14) */
/* 1030 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 1032 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1034 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1036 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1038 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1040 */	NdrFcShort( 0xfbfa ),	/* Offset= -1030 (10) */
/* 1042 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 1044 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */
/* 1046 */	
			0x11, 0x0,	/* FC_RP */
/* 1048 */	NdrFcShort( 0x6 ),	/* Offset= 6 (1054) */
/* 1050 */	
			0x13, 0x0,	/* FC_OP */
/* 1052 */	NdrFcShort( 0xffd8 ),	/* Offset= -40 (1012) */
/* 1054 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1056 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1058 */	NdrFcShort( 0x10 ),	/* 16 */
/* 1060 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1062 */	NdrFcShort( 0xfff4 ),	/* Offset= -12 (1050) */
/* 1064 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1066 */	NdrFcLong( 0x3be31fe ),	/* 62796286 */
/* 1070 */	NdrFcShort( 0x6526 ),	/* 25894 */
/* 1072 */	NdrFcShort( 0x4d9c ),	/* 19868 */
/* 1074 */	0xb1,		/* 177 */
			0x97,		/* 151 */
/* 1076 */	0x4a,		/* 74 */
			0x3e,		/* 62 */
/* 1078 */	0x5d,		/* 93 */
			0xcf,		/* 207 */
/* 1080 */	0xf6,		/* 246 */
			0x96,		/* 150 */
/* 1082 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1084 */	NdrFcLong( 0x6ea122b6 ),	/* 1856053942 */
/* 1088 */	NdrFcShort( 0x69ff ),	/* 27135 */
/* 1090 */	NdrFcShort( 0x4cf4 ),	/* 19700 */
/* 1092 */	0x91,		/* 145 */
			0x96,		/* 150 */
/* 1094 */	0x4,		/* 4 */
			0x19,		/* 25 */
/* 1096 */	0xfb,		/* 251 */
			0x38,		/* 56 */
/* 1098 */	0x18,		/* 24 */
			0x73,		/* 115 */
/* 1100 */	
			0x2f,		/* FC_IP */
			0x5a,		/* FC_CONSTANT_IID */
/* 1102 */	NdrFcLong( 0xf6a717ae ),	/* -156821586 */
/* 1106 */	NdrFcShort( 0x805a ),	/* -32678 */
/* 1108 */	NdrFcShort( 0x442b ),	/* 17451 */
/* 1110 */	0x86,		/* 134 */
			0xd7,		/* 215 */
/* 1112 */	0xf5,		/* 245 */
			0xa3,		/* 163 */
/* 1114 */	0x5,		/* 5 */
			0x2f,		/* 47 */
/* 1116 */	0xe3,		/* 227 */
			0x36,		/* 54 */
/* 1118 */	
			0x11, 0x14,	/* FC_RP [alloced_on_stack] [pointer_deref] */
/* 1120 */	NdrFcShort( 0xffc8 ),	/* Offset= -56 (1064) */
/* 1122 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/* 1124 */	0x6,		/* FC_SHORT */
			0x5c,		/* FC_PAD */
/* 1126 */	
			0x11, 0x0,	/* FC_RP */
/* 1128 */	NdrFcShort( 0xffa0 ),	/* Offset= -96 (1032) */
/* 1130 */	0xb4,		/* FC_USER_MARSHAL */
			0x83,		/* 131 */
/* 1132 */	NdrFcShort( 0x1 ),	/* 1 */
/* 1134 */	NdrFcShort( 0x4 ),	/* 4 */
/* 1136 */	NdrFcShort( 0x0 ),	/* 0 */
/* 1138 */	NdrFcShort( 0xfccc ),	/* Offset= -820 (318) */

			0x0
        }
    };

static const USER_MARSHAL_ROUTINE_QUADRUPLE UserMarshalRoutines[ WIRE_MARSHAL_TABLE_SIZE ] = 
        {
            
            {
            VARIANT_UserSize
            ,VARIANT_UserMarshal
            ,VARIANT_UserUnmarshal
            ,VARIANT_UserFree
            },
            {
            BSTR_UserSize
            ,BSTR_UserMarshal
            ,BSTR_UserUnmarshal
            ,BSTR_UserFree
            }

        };



/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IDispatch, ver. 0.0,
   GUID={0x00020400,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: IEventEx, ver. 0.0,
   GUID={0x03BE31FE,0x6526,0x4D9C,{0xB1,0x97,0x4A,0x3E,0x5D,0xCF,0xF6,0x96}} */

#pragma code_seg(".orpc")
static const unsigned short IEventEx_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    36,
    78,
    114,
    150,
    186,
    222
    };

static const MIDL_STUBLESS_PROXY_INFO IEventEx_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEventEx_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEventEx_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEventEx_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IEventExProxyVtbl = 
{
    &IEventEx_ProxyInfo,
    &IID_IEventEx,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IEventEx::GetType */ ,
    (void *) (INT_PTR) -1 /* IEventEx::IsTypeSupported */ ,
    (void *) (INT_PTR) -1 /* IEventEx::GetEventId */ ,
    (void *) (INT_PTR) -1 /* IEventEx::GetState */ ,
    (void *) (INT_PTR) -1 /* IEventEx::GetProviderId */ ,
    (void *) (INT_PTR) -1 /* IEventEx::GetBroadcast */ ,
    (void *) (INT_PTR) -1 /* IEventEx::GetCancelled */
};


static const PRPC_STUB_FUNCTION IEventEx_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IEventExStubVtbl =
{
    &IID_IEventEx,
    &IEventEx_ServerInfo,
    14,
    &IEventEx_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: ISerializableEventEx, ver. 0.0,
   GUID={0x942929EB,0x0954,0x4C92,{0xA8,0xF0,0xDD,0x3D,0x3A,0x05,0x1C,0xA9}} */

#pragma code_seg(".orpc")
static const unsigned short ISerializableEventEx_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    0,
    36,
    78,
    114,
    150,
    186,
    222,
    258,
    306,
    354
    };

static const MIDL_STUBLESS_PROXY_INFO ISerializableEventEx_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ISerializableEventEx_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ISerializableEventEx_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ISerializableEventEx_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(17) _ISerializableEventExProxyVtbl = 
{
    &ISerializableEventEx_ProxyInfo,
    &IID_ISerializableEventEx,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IEventEx::GetType */ ,
    (void *) (INT_PTR) -1 /* IEventEx::IsTypeSupported */ ,
    (void *) (INT_PTR) -1 /* IEventEx::GetEventId */ ,
    (void *) (INT_PTR) -1 /* IEventEx::GetState */ ,
    (void *) (INT_PTR) -1 /* IEventEx::GetProviderId */ ,
    (void *) (INT_PTR) -1 /* IEventEx::GetBroadcast */ ,
    (void *) (INT_PTR) -1 /* IEventEx::GetCancelled */ ,
    (void *) (INT_PTR) -1 /* ISerializableEventEx::Load */ ,
    (void *) (INT_PTR) -1 /* ISerializableEventEx::Save */ ,
    (void *) (INT_PTR) -1 /* ISerializableEventEx::GetSizeMax */
};


static const PRPC_STUB_FUNCTION ISerializableEventEx_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _ISerializableEventExStubVtbl =
{
    &IID_ISerializableEventEx,
    &ISerializableEventEx_ServerInfo,
    17,
    &ISerializableEventEx_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: ISubscriberEx, ver. 0.0,
   GUID={0xF6A717AE,0x805A,0x442B,{0x86,0xD7,0xF5,0xA3,0x05,0x2F,0xE3,0x36}} */

#pragma code_seg(".orpc")
static const unsigned short ISubscriberEx_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    396,
    450
    };

static const MIDL_STUBLESS_PROXY_INFO ISubscriberEx_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ISubscriberEx_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ISubscriberEx_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ISubscriberEx_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _ISubscriberExProxyVtbl = 
{
    &ISubscriberEx_ProxyInfo,
    &IID_ISubscriberEx,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* ISubscriberEx::OnEvent */ ,
    (void *) (INT_PTR) -1 /* ISubscriberEx::OnShutdown */
};


static const PRPC_STUB_FUNCTION ISubscriberEx_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _ISubscriberExStubVtbl =
{
    &IID_ISubscriberEx,
    &ISubscriberEx_ServerInfo,
    9,
    &ISubscriberEx_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IProviderEx, ver. 0.0,
   GUID={0x6EA122B6,0x69FF,0x4CF4,{0x91,0x96,0x04,0x19,0xFB,0x38,0x18,0x73}} */

#pragma code_seg(".orpc")
static const unsigned short IProviderEx_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    486,
    450
    };

static const MIDL_STUBLESS_PROXY_INFO IProviderEx_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IProviderEx_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IProviderEx_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IProviderEx_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(9) _IProviderExProxyVtbl = 
{
    &IProviderEx_ProxyInfo,
    &IID_IProviderEx,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IProviderEx::OnEvent */ ,
    (void *) (INT_PTR) -1 /* IProviderEx::OnShutdown */
};


static const PRPC_STUB_FUNCTION IProviderEx_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IProviderExStubVtbl =
{
    &IID_IProviderEx,
    &IProviderEx_ServerInfo,
    9,
    &IProviderEx_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IEventManager, ver. 0.0,
   GUID={0x958C5835,0x0F5F,0x424F,{0xBE,0x62,0x8F,0x2A,0x17,0x65,0x7E,0x4B}} */

#pragma code_seg(".orpc")
static const unsigned short IEventManager_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    534,
    582,
    624,
    702,
    744,
    804,
    852
    };

static const MIDL_STUBLESS_PROXY_INFO IEventManager_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IEventManager_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IEventManager_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IEventManager_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(14) _IEventManagerProxyVtbl = 
{
    &IEventManager_ProxyInfo,
    &IID_IEventManager,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IEventManager::RegisterProvider */ ,
    (void *) (INT_PTR) -1 /* IEventManager::UnregisterProvider */ ,
    (void *) (INT_PTR) -1 /* IEventManager::RegisterSubscriber */ ,
    (void *) (INT_PTR) -1 /* IEventManager::UnregisterSubscriber */ ,
    (void *) (INT_PTR) -1 /* IEventManager::CreateEvent */ ,
    (void *) (INT_PTR) -1 /* IEventManager::CommitEvent */ ,
    (void *) (INT_PTR) -1 /* IEventManager::CancelEvent */
};


static const PRPC_STUB_FUNCTION IEventManager_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IEventManagerStubVtbl =
{
    &IID_IEventManager,
    &IEventManager_ServerInfo,
    14,
    &IEventManager_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: ILogManager, ver. 0.0,
   GUID={0x374AA35A,0x71B9,0x49AE,{0x99,0x8F,0xEC,0x0B,0x6A,0x41,0x44,0x51}} */

#pragma code_seg(".orpc")
static const unsigned short ILogManager_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    900,
    966,
    1020,
    1122,
    1170,
    1212,
    1260,
    1308,
    1356
    };

static const MIDL_STUBLESS_PROXY_INFO ILogManager_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ILogManager_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ILogManager_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ILogManager_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(16) _ILogManagerProxyVtbl = 
{
    &ILogManager_ProxyInfo,
    &IID_ILogManager,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* ILogManager::LoadEvent */ ,
    (void *) (INT_PTR) -1 /* ILogManager::DeleteEvent */ ,
    (void *) (INT_PTR) -1 /* ILogManager::SearchEvents */ ,
    (void *) (INT_PTR) -1 /* ILogManager::DeleteSearchContext */ ,
    (void *) (INT_PTR) -1 /* ILogManager::ClearLog */ ,
    (void *) (INT_PTR) -1 /* ILogManager::SetMaxLogSize */ ,
    (void *) (INT_PTR) -1 /* ILogManager::GetMaxLogSize */ ,
    (void *) (INT_PTR) -1 /* ILogManager::SetLogEnabled */ ,
    (void *) (INT_PTR) -1 /* ILogManager::GetLogEnabled */
};


static const PRPC_STUB_FUNCTION ILogManager_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _ILogManagerStubVtbl =
{
    &IID_ILogManager,
    &ILogManager_ServerInfo,
    16,
    &ILogManager_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: ILogManager2, ver. 0.0,
   GUID={0xD90EF5C6,0x89D8,0x486a,{0x8E,0xB7,0x3E,0x4A,0x84,0xF3,0xBF,0xC0}} */

#pragma code_seg(".orpc")
static const unsigned short ILogManager2_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    900,
    966,
    1020,
    1122,
    1170,
    1212,
    1260,
    1308,
    1356,
    1404,
    1476,
    1536,
    1590,
    1644,
    1758
    };

static const MIDL_STUBLESS_PROXY_INFO ILogManager2_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &ILogManager2_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ILogManager2_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &ILogManager2_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(22) _ILogManager2ProxyVtbl = 
{
    &ILogManager2_ProxyInfo,
    &IID_ILogManager2,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* ILogManager::LoadEvent */ ,
    (void *) (INT_PTR) -1 /* ILogManager::DeleteEvent */ ,
    (void *) (INT_PTR) -1 /* ILogManager::SearchEvents */ ,
    (void *) (INT_PTR) -1 /* ILogManager::DeleteSearchContext */ ,
    (void *) (INT_PTR) -1 /* ILogManager::ClearLog */ ,
    (void *) (INT_PTR) -1 /* ILogManager::SetMaxLogSize */ ,
    (void *) (INT_PTR) -1 /* ILogManager::GetMaxLogSize */ ,
    (void *) (INT_PTR) -1 /* ILogManager::SetLogEnabled */ ,
    (void *) (INT_PTR) -1 /* ILogManager::GetLogEnabled */ ,
    (void *) (INT_PTR) -1 /* ILogManager2::LoadEvent */ ,
    (void *) (INT_PTR) -1 /* ILogManager2::DeleteEvent */ ,
    (void *) (INT_PTR) -1 /* ILogManager2::SetMaxLogSize */ ,
    (void *) (INT_PTR) -1 /* ILogManager2::GetMaxLogSize */ ,
    (void *) (INT_PTR) -1 /* ILogManager2::SearchEvents */ ,
    (void *) (INT_PTR) -1 /* ILogManager2::LoadEventBatch */
};


static const PRPC_STUB_FUNCTION ILogManager2_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _ILogManager2StubVtbl =
{
    &IID_ILogManager2,
    &ILogManager2_ServerInfo,
    22,
    &ILogManager2_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};


/* Object interface: IModuleManager, ver. 0.0,
   GUID={0x072553DD,0x20A3,0x4CF0,{0xB4,0x31,0x48,0xFE,0x5C,0x91,0x32,0x83}} */

#pragma code_seg(".orpc")
static const unsigned short IModuleManager_FormatStringOffsetTable[] =
    {
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    (unsigned short) -1,
    1836,
    1884,
    1932
    };

static const MIDL_STUBLESS_PROXY_INFO IModuleManager_ProxyInfo =
    {
    &Object_StubDesc,
    __MIDL_ProcFormatString.Format,
    &IModuleManager_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO IModuleManager_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    __MIDL_ProcFormatString.Format,
    &IModuleManager_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(10) _IModuleManagerProxyVtbl = 
{
    &IModuleManager_ProxyInfo,
    &IID_IModuleManager,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfoCount */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetTypeInfo */ ,
    0 /* (void *) (INT_PTR) -1 /* IDispatch::GetIDsOfNames */ ,
    0 /* IDispatch_Invoke_Proxy */ ,
    (void *) (INT_PTR) -1 /* IModuleManager::Load */ ,
    (void *) (INT_PTR) -1 /* IModuleManager::Unload */ ,
    (void *) (INT_PTR) -1 /* IModuleManager::IsLoaded */
};


static const PRPC_STUB_FUNCTION IModuleManager_table[] =
{
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    STUB_FORWARDING_FUNCTION,
    NdrStubCall2,
    NdrStubCall2,
    NdrStubCall2
};

CInterfaceStubVtbl _IModuleManagerStubVtbl =
{
    &IID_IModuleManager,
    &IModuleManager_ServerInfo,
    10,
    &IModuleManager_table[-3],
    CStdStubBuffer_DELEGATING_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    __MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x50002, /* Ndr library version */
    0,
    0x6000169, /* MIDL Version 6.0.361 */
    0,
    UserMarshalRoutines,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0   /* Reserved5 */
    };

const CInterfaceProxyVtbl * _ccEventManager_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_IEventManagerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ILogManagerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ISubscriberExProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IProviderExProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ILogManager2ProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IModuleManagerProxyVtbl,
    ( CInterfaceProxyVtbl *) &_ISerializableEventExProxyVtbl,
    ( CInterfaceProxyVtbl *) &_IEventExProxyVtbl,
    0
};

const CInterfaceStubVtbl * _ccEventManager_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_IEventManagerStubVtbl,
    ( CInterfaceStubVtbl *) &_ILogManagerStubVtbl,
    ( CInterfaceStubVtbl *) &_ISubscriberExStubVtbl,
    ( CInterfaceStubVtbl *) &_IProviderExStubVtbl,
    ( CInterfaceStubVtbl *) &_ILogManager2StubVtbl,
    ( CInterfaceStubVtbl *) &_IModuleManagerStubVtbl,
    ( CInterfaceStubVtbl *) &_ISerializableEventExStubVtbl,
    ( CInterfaceStubVtbl *) &_IEventExStubVtbl,
    0
};

PCInterfaceName const _ccEventManager_InterfaceNamesList[] = 
{
    "IEventManager",
    "ILogManager",
    "ISubscriberEx",
    "IProviderEx",
    "ILogManager2",
    "IModuleManager",
    "ISerializableEventEx",
    "IEventEx",
    0
};

const IID *  _ccEventManager_BaseIIDList[] = 
{
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    &IID_IDispatch,
    0
};


#define _ccEventManager_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _ccEventManager, pIID, n)

int __stdcall _ccEventManager_IID_Lookup( const IID * pIID, int * pIndex )
{
    IID_BS_LOOKUP_SETUP

    IID_BS_LOOKUP_INITIAL_TEST( _ccEventManager, 8, 4 )
    IID_BS_LOOKUP_NEXT_TEST( _ccEventManager, 2 )
    IID_BS_LOOKUP_NEXT_TEST( _ccEventManager, 1 )
    IID_BS_LOOKUP_RETURN_RESULT( _ccEventManager, 8, *pIndex )
    
}

const ExtendedProxyFileInfo ccEventManager_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _ccEventManager_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _ccEventManager_StubVtblList,
    (const PCInterfaceName * ) & _ccEventManager_InterfaceNamesList,
    (const IID ** ) & _ccEventManager_BaseIIDList,
    & _ccEventManager_IID_Lookup, 
    8,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

