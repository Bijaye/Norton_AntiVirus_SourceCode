////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////


/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Wed Apr 26 19:42:02 2006
 */
/* Compiler settings for \ScanTest\Sdk\Cc\include\ccEvtMgr\ccEventManager.idl:
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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IEventEx,0x03BE31FE,0x6526,0x4D9C,0xB1,0x97,0x4A,0x3E,0x5D,0xCF,0xF6,0x96);


MIDL_DEFINE_GUID(IID, IID_ISerializableEventEx,0x942929EB,0x0954,0x4C92,0xA8,0xF0,0xDD,0x3D,0x3A,0x05,0x1C,0xA9);


MIDL_DEFINE_GUID(IID, IID_ISubscriberEx,0xF6A717AE,0x805A,0x442B,0x86,0xD7,0xF5,0xA3,0x05,0x2F,0xE3,0x36);


MIDL_DEFINE_GUID(IID, IID_IProviderEx,0x6EA122B6,0x69FF,0x4CF4,0x91,0x96,0x04,0x19,0xFB,0x38,0x18,0x73);


MIDL_DEFINE_GUID(IID, IID_IEventManager,0x958C5835,0x0F5F,0x424F,0xBE,0x62,0x8F,0x2A,0x17,0x65,0x7E,0x4B);


MIDL_DEFINE_GUID(IID, IID_ILogManager,0x374AA35A,0x71B9,0x49AE,0x99,0x8F,0xEC,0x0B,0x6A,0x41,0x44,0x51);


MIDL_DEFINE_GUID(IID, IID_ILogManager2,0xD90EF5C6,0x89D8,0x486a,0x8E,0xB7,0x3E,0x4A,0x84,0xF3,0xBF,0xC0);


MIDL_DEFINE_GUID(IID, IID_IModuleManager,0x072553DD,0x20A3,0x4CF0,0xB4,0x31,0x48,0xFE,0x5C,0x91,0x32,0x83);


MIDL_DEFINE_GUID(IID, LIBID_ccEvtMgrLib,0x60681DC5,0x21B2,0x4264,0xB1,0xF1,0xE1,0x28,0x98,0x19,0xE0,0x23);


MIDL_DEFINE_GUID(CLSID, CLSID_EventManager,0x49BB73EE,0x2C2F,0x445E,0x82,0xE3,0xE6,0xE3,0x38,0x02,0x85,0xBF);


MIDL_DEFINE_GUID(CLSID, CLSID_LogManager,0xB8E914C1,0xA516,0x421F,0xB4,0x13,0xB3,0x2B,0x3F,0xA3,0xF1,0x8F);


MIDL_DEFINE_GUID(CLSID, CLSID_ModuleManager,0x311CF1A1,0x872A,0x4ED5,0x94,0x3F,0x05,0x8C,0x88,0x6E,0x2F,0x7F);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

