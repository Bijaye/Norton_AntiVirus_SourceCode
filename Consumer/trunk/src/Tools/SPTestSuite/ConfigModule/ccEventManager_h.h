////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////


/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ccEventManager_h_h__
#define __ccEventManager_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IEventEx_FWD_DEFINED__
#define __IEventEx_FWD_DEFINED__
typedef interface IEventEx IEventEx;
#endif 	/* __IEventEx_FWD_DEFINED__ */


#ifndef __ISerializableEventEx_FWD_DEFINED__
#define __ISerializableEventEx_FWD_DEFINED__
typedef interface ISerializableEventEx ISerializableEventEx;
#endif 	/* __ISerializableEventEx_FWD_DEFINED__ */


#ifndef __ISubscriberEx_FWD_DEFINED__
#define __ISubscriberEx_FWD_DEFINED__
typedef interface ISubscriberEx ISubscriberEx;
#endif 	/* __ISubscriberEx_FWD_DEFINED__ */


#ifndef __IProviderEx_FWD_DEFINED__
#define __IProviderEx_FWD_DEFINED__
typedef interface IProviderEx IProviderEx;
#endif 	/* __IProviderEx_FWD_DEFINED__ */


#ifndef __IEventManager_FWD_DEFINED__
#define __IEventManager_FWD_DEFINED__
typedef interface IEventManager IEventManager;
#endif 	/* __IEventManager_FWD_DEFINED__ */


#ifndef __ILogManager_FWD_DEFINED__
#define __ILogManager_FWD_DEFINED__
typedef interface ILogManager ILogManager;
#endif 	/* __ILogManager_FWD_DEFINED__ */


#ifndef __ILogManager2_FWD_DEFINED__
#define __ILogManager2_FWD_DEFINED__
typedef interface ILogManager2 ILogManager2;
#endif 	/* __ILogManager2_FWD_DEFINED__ */


#ifndef __IModuleManager_FWD_DEFINED__
#define __IModuleManager_FWD_DEFINED__
typedef interface IModuleManager IModuleManager;
#endif 	/* __IModuleManager_FWD_DEFINED__ */


#ifndef __IEventEx_FWD_DEFINED__
#define __IEventEx_FWD_DEFINED__
typedef interface IEventEx IEventEx;
#endif 	/* __IEventEx_FWD_DEFINED__ */


#ifndef __ISerializableEventEx_FWD_DEFINED__
#define __ISerializableEventEx_FWD_DEFINED__
typedef interface ISerializableEventEx ISerializableEventEx;
#endif 	/* __ISerializableEventEx_FWD_DEFINED__ */


#ifndef __IProviderEx_FWD_DEFINED__
#define __IProviderEx_FWD_DEFINED__
typedef interface IProviderEx IProviderEx;
#endif 	/* __IProviderEx_FWD_DEFINED__ */


#ifndef __ISubscriberEx_FWD_DEFINED__
#define __ISubscriberEx_FWD_DEFINED__
typedef interface ISubscriberEx ISubscriberEx;
#endif 	/* __ISubscriberEx_FWD_DEFINED__ */


#ifndef __IEventManager_FWD_DEFINED__
#define __IEventManager_FWD_DEFINED__
typedef interface IEventManager IEventManager;
#endif 	/* __IEventManager_FWD_DEFINED__ */


#ifndef __ILogManager_FWD_DEFINED__
#define __ILogManager_FWD_DEFINED__
typedef interface ILogManager ILogManager;
#endif 	/* __ILogManager_FWD_DEFINED__ */


#ifndef __ILogManager2_FWD_DEFINED__
#define __ILogManager2_FWD_DEFINED__
typedef interface ILogManager2 ILogManager2;
#endif 	/* __ILogManager2_FWD_DEFINED__ */


#ifndef __IModuleManager_FWD_DEFINED__
#define __IModuleManager_FWD_DEFINED__
typedef interface IModuleManager IModuleManager;
#endif 	/* __IModuleManager_FWD_DEFINED__ */


#ifndef __EventManager_FWD_DEFINED__
#define __EventManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class EventManager EventManager;
#else
typedef struct EventManager EventManager;
#endif /* __cplusplus */

#endif 	/* __EventManager_FWD_DEFINED__ */


#ifndef __LogManager_FWD_DEFINED__
#define __LogManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class LogManager LogManager;
#else
typedef struct LogManager LogManager;
#endif /* __cplusplus */

#endif 	/* __LogManager_FWD_DEFINED__ */


#ifndef __ModuleManager_FWD_DEFINED__
#define __ModuleManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class ModuleManager ModuleManager;
#else
typedef struct ModuleManager ModuleManager;
#endif /* __cplusplus */

#endif 	/* __ModuleManager_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

#ifndef __IEventEx_INTERFACE_DEFINED__
#define __IEventEx_INTERFACE_DEFINED__

/* interface IEventEx */
/* [unique][helpstring][nonextensible][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_IEventEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("03BE31FE-6526-4D9C-B197-4A3E5DCFF696")
    IEventEx : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetType( 
            /* [retval][out] */ long *pRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsTypeSupported( 
            /* [in] */ long nEventType,
            /* [retval][out] */ VARIANT_BOOL *pRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetEventId( 
            /* [retval][out] */ long *pRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetState( 
            /* [retval][out] */ long *pRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProviderId( 
            /* [retval][out] */ long *pRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetBroadcast( 
            /* [retval][out] */ VARIANT_BOOL *pRet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCancelled( 
            /* [retval][out] */ VARIANT_BOOL *pRet) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEventEx * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEventEx * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEventEx * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IEventEx * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IEventEx * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IEventEx * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IEventEx * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetType )( 
            IEventEx * This,
            /* [retval][out] */ long *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsTypeSupported )( 
            IEventEx * This,
            /* [in] */ long nEventType,
            /* [retval][out] */ VARIANT_BOOL *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetEventId )( 
            IEventEx * This,
            /* [retval][out] */ long *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetState )( 
            IEventEx * This,
            /* [retval][out] */ long *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProviderId )( 
            IEventEx * This,
            /* [retval][out] */ long *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetBroadcast )( 
            IEventEx * This,
            /* [retval][out] */ VARIANT_BOOL *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCancelled )( 
            IEventEx * This,
            /* [retval][out] */ VARIANT_BOOL *pRet);
        
        END_INTERFACE
    } IEventExVtbl;

    interface IEventEx
    {
        CONST_VTBL struct IEventExVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventEx_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventEx_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventEx_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventEx_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEventEx_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEventEx_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEventEx_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEventEx_GetType(This,pRet)	\
    (This)->lpVtbl -> GetType(This,pRet)

#define IEventEx_IsTypeSupported(This,nEventType,pRet)	\
    (This)->lpVtbl -> IsTypeSupported(This,nEventType,pRet)

#define IEventEx_GetEventId(This,pRet)	\
    (This)->lpVtbl -> GetEventId(This,pRet)

#define IEventEx_GetState(This,pRet)	\
    (This)->lpVtbl -> GetState(This,pRet)

#define IEventEx_GetProviderId(This,pRet)	\
    (This)->lpVtbl -> GetProviderId(This,pRet)

#define IEventEx_GetBroadcast(This,pRet)	\
    (This)->lpVtbl -> GetBroadcast(This,pRet)

#define IEventEx_GetCancelled(This,pRet)	\
    (This)->lpVtbl -> GetCancelled(This,pRet)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventEx_GetType_Proxy( 
    IEventEx * This,
    /* [retval][out] */ long *pRet);


void __RPC_STUB IEventEx_GetType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventEx_IsTypeSupported_Proxy( 
    IEventEx * This,
    /* [in] */ long nEventType,
    /* [retval][out] */ VARIANT_BOOL *pRet);


void __RPC_STUB IEventEx_IsTypeSupported_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventEx_GetEventId_Proxy( 
    IEventEx * This,
    /* [retval][out] */ long *pRet);


void __RPC_STUB IEventEx_GetEventId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventEx_GetState_Proxy( 
    IEventEx * This,
    /* [retval][out] */ long *pRet);


void __RPC_STUB IEventEx_GetState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventEx_GetProviderId_Proxy( 
    IEventEx * This,
    /* [retval][out] */ long *pRet);


void __RPC_STUB IEventEx_GetProviderId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventEx_GetBroadcast_Proxy( 
    IEventEx * This,
    /* [retval][out] */ VARIANT_BOOL *pRet);


void __RPC_STUB IEventEx_GetBroadcast_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventEx_GetCancelled_Proxy( 
    IEventEx * This,
    /* [retval][out] */ VARIANT_BOOL *pRet);


void __RPC_STUB IEventEx_GetCancelled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEventEx_INTERFACE_DEFINED__ */


#ifndef __ISerializableEventEx_INTERFACE_DEFINED__
#define __ISerializableEventEx_INTERFACE_DEFINED__

/* interface ISerializableEventEx */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_ISerializableEventEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("942929EB-0954-4C92-A8F0-DD3D3A051CA9")
    ISerializableEventEx : public IEventEx
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Load( 
            /* [in] */ VARIANT vData,
            /* [out][in] */ long *pRead,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [out][in] */ VARIANT *pData,
            /* [out][in] */ long *pWritten,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSizeMax( 
            /* [out][in] */ long *pSize,
            /* [retval][out] */ long *pError) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISerializableEventExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISerializableEventEx * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISerializableEventEx * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISerializableEventEx * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISerializableEventEx * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISerializableEventEx * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISerializableEventEx * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISerializableEventEx * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetType )( 
            ISerializableEventEx * This,
            /* [retval][out] */ long *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsTypeSupported )( 
            ISerializableEventEx * This,
            /* [in] */ long nEventType,
            /* [retval][out] */ VARIANT_BOOL *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetEventId )( 
            ISerializableEventEx * This,
            /* [retval][out] */ long *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetState )( 
            ISerializableEventEx * This,
            /* [retval][out] */ long *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProviderId )( 
            ISerializableEventEx * This,
            /* [retval][out] */ long *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetBroadcast )( 
            ISerializableEventEx * This,
            /* [retval][out] */ VARIANT_BOOL *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCancelled )( 
            ISerializableEventEx * This,
            /* [retval][out] */ VARIANT_BOOL *pRet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Load )( 
            ISerializableEventEx * This,
            /* [in] */ VARIANT vData,
            /* [out][in] */ long *pRead,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            ISerializableEventEx * This,
            /* [out][in] */ VARIANT *pData,
            /* [out][in] */ long *pWritten,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSizeMax )( 
            ISerializableEventEx * This,
            /* [out][in] */ long *pSize,
            /* [retval][out] */ long *pError);
        
        END_INTERFACE
    } ISerializableEventExVtbl;

    interface ISerializableEventEx
    {
        CONST_VTBL struct ISerializableEventExVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISerializableEventEx_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISerializableEventEx_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISerializableEventEx_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISerializableEventEx_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISerializableEventEx_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISerializableEventEx_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISerializableEventEx_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISerializableEventEx_GetType(This,pRet)	\
    (This)->lpVtbl -> GetType(This,pRet)

#define ISerializableEventEx_IsTypeSupported(This,nEventType,pRet)	\
    (This)->lpVtbl -> IsTypeSupported(This,nEventType,pRet)

#define ISerializableEventEx_GetEventId(This,pRet)	\
    (This)->lpVtbl -> GetEventId(This,pRet)

#define ISerializableEventEx_GetState(This,pRet)	\
    (This)->lpVtbl -> GetState(This,pRet)

#define ISerializableEventEx_GetProviderId(This,pRet)	\
    (This)->lpVtbl -> GetProviderId(This,pRet)

#define ISerializableEventEx_GetBroadcast(This,pRet)	\
    (This)->lpVtbl -> GetBroadcast(This,pRet)

#define ISerializableEventEx_GetCancelled(This,pRet)	\
    (This)->lpVtbl -> GetCancelled(This,pRet)


#define ISerializableEventEx_Load(This,vData,pRead,pError)	\
    (This)->lpVtbl -> Load(This,vData,pRead,pError)

#define ISerializableEventEx_Save(This,pData,pWritten,pError)	\
    (This)->lpVtbl -> Save(This,pData,pWritten,pError)

#define ISerializableEventEx_GetSizeMax(This,pSize,pError)	\
    (This)->lpVtbl -> GetSizeMax(This,pSize,pError)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISerializableEventEx_Load_Proxy( 
    ISerializableEventEx * This,
    /* [in] */ VARIANT vData,
    /* [out][in] */ long *pRead,
    /* [retval][out] */ long *pError);


void __RPC_STUB ISerializableEventEx_Load_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISerializableEventEx_Save_Proxy( 
    ISerializableEventEx * This,
    /* [out][in] */ VARIANT *pData,
    /* [out][in] */ long *pWritten,
    /* [retval][out] */ long *pError);


void __RPC_STUB ISerializableEventEx_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISerializableEventEx_GetSizeMax_Proxy( 
    ISerializableEventEx * This,
    /* [out][in] */ long *pSize,
    /* [retval][out] */ long *pError);


void __RPC_STUB ISerializableEventEx_GetSizeMax_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISerializableEventEx_INTERFACE_DEFINED__ */


#ifndef __ISubscriberEx_INTERFACE_DEFINED__
#define __ISubscriberEx_INTERFACE_DEFINED__

/* interface ISubscriberEx */
/* [unique][helpstring][nonextensible][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_ISubscriberEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F6A717AE-805A-442B-86D7-F5A3052FE336")
    ISubscriberEx : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnEvent( 
            /* [in] */ long nSubscriberId,
            /* [in] */ IEventEx *pEvent,
            /* [out][in] */ long *pAction,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnShutdown( 
            /* [retval][out] */ long *pError) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISubscriberExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISubscriberEx * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISubscriberEx * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISubscriberEx * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISubscriberEx * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISubscriberEx * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISubscriberEx * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISubscriberEx * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnEvent )( 
            ISubscriberEx * This,
            /* [in] */ long nSubscriberId,
            /* [in] */ IEventEx *pEvent,
            /* [out][in] */ long *pAction,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnShutdown )( 
            ISubscriberEx * This,
            /* [retval][out] */ long *pError);
        
        END_INTERFACE
    } ISubscriberExVtbl;

    interface ISubscriberEx
    {
        CONST_VTBL struct ISubscriberExVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISubscriberEx_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISubscriberEx_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISubscriberEx_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISubscriberEx_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISubscriberEx_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISubscriberEx_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISubscriberEx_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISubscriberEx_OnEvent(This,nSubscriberId,pEvent,pAction,pError)	\
    (This)->lpVtbl -> OnEvent(This,nSubscriberId,pEvent,pAction,pError)

#define ISubscriberEx_OnShutdown(This,pError)	\
    (This)->lpVtbl -> OnShutdown(This,pError)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISubscriberEx_OnEvent_Proxy( 
    ISubscriberEx * This,
    /* [in] */ long nSubscriberId,
    /* [in] */ IEventEx *pEvent,
    /* [out][in] */ long *pAction,
    /* [retval][out] */ long *pError);


void __RPC_STUB ISubscriberEx_OnEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISubscriberEx_OnShutdown_Proxy( 
    ISubscriberEx * This,
    /* [retval][out] */ long *pError);


void __RPC_STUB ISubscriberEx_OnShutdown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISubscriberEx_INTERFACE_DEFINED__ */


#ifndef __IProviderEx_INTERFACE_DEFINED__
#define __IProviderEx_INTERFACE_DEFINED__

/* interface IProviderEx */
/* [unique][helpstring][nonextensible][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_IProviderEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6EA122B6-69FF-4CF4-9196-0419FB381873")
    IProviderEx : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnEvent( 
            /* [in] */ long nProviderId,
            /* [in] */ IEventEx *pEvent,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnShutdown( 
            /* [retval][out] */ long *pError) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IProviderExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProviderEx * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProviderEx * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProviderEx * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IProviderEx * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IProviderEx * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IProviderEx * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IProviderEx * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnEvent )( 
            IProviderEx * This,
            /* [in] */ long nProviderId,
            /* [in] */ IEventEx *pEvent,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnShutdown )( 
            IProviderEx * This,
            /* [retval][out] */ long *pError);
        
        END_INTERFACE
    } IProviderExVtbl;

    interface IProviderEx
    {
        CONST_VTBL struct IProviderExVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProviderEx_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IProviderEx_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IProviderEx_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IProviderEx_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IProviderEx_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IProviderEx_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IProviderEx_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IProviderEx_OnEvent(This,nProviderId,pEvent,pError)	\
    (This)->lpVtbl -> OnEvent(This,nProviderId,pEvent,pError)

#define IProviderEx_OnShutdown(This,pError)	\
    (This)->lpVtbl -> OnShutdown(This,pError)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IProviderEx_OnEvent_Proxy( 
    IProviderEx * This,
    /* [in] */ long nProviderId,
    /* [in] */ IEventEx *pEvent,
    /* [retval][out] */ long *pError);


void __RPC_STUB IProviderEx_OnEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IProviderEx_OnShutdown_Proxy( 
    IProviderEx * This,
    /* [retval][out] */ long *pError);


void __RPC_STUB IProviderEx_OnShutdown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IProviderEx_INTERFACE_DEFINED__ */


#ifndef __IEventManager_INTERFACE_DEFINED__
#define __IEventManager_INTERFACE_DEFINED__

/* interface IEventManager */
/* [unique][helpstring][nonextensible][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_IEventManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("958C5835-0F5F-424F-BE62-8F2A17657E4B")
    IEventManager : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegisterProvider( 
            /* [in] */ IProviderEx *pProvider,
            /* [out][in] */ long *pProviderId,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnregisterProvider( 
            /* [in] */ long nProviderId,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegisterSubscriber( 
            /* [in] */ ISubscriberEx *pSubscriber,
            /* [in] */ long nEventType,
            /* [in] */ long nPriority,
            /* [in] */ VARIANT vEventStates,
            /* [in] */ long nProviderId,
            /* [in] */ VARIANT_BOOL bMonitorOnly,
            /* [out][in] */ long *pSubscriberId,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnregisterSubscriber( 
            /* [in] */ long nSubscriberId,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateEvent( 
            /* [in] */ long nProviderId,
            /* [in] */ IEventEx *pEvent,
            VARIANT_BOOL bBroadcast,
            /* [out][in] */ long *pEventId,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CommitEvent( 
            /* [in] */ long nSubscriberId,
            /* [in] */ IEventEx *pEvent,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CancelEvent( 
            /* [in] */ long nSubscriberId,
            /* [in] */ IEventEx *pEvent,
            /* [retval][out] */ long *pError) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEventManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEventManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEventManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEventManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IEventManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IEventManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IEventManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IEventManager * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegisterProvider )( 
            IEventManager * This,
            /* [in] */ IProviderEx *pProvider,
            /* [out][in] */ long *pProviderId,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnregisterProvider )( 
            IEventManager * This,
            /* [in] */ long nProviderId,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegisterSubscriber )( 
            IEventManager * This,
            /* [in] */ ISubscriberEx *pSubscriber,
            /* [in] */ long nEventType,
            /* [in] */ long nPriority,
            /* [in] */ VARIANT vEventStates,
            /* [in] */ long nProviderId,
            /* [in] */ VARIANT_BOOL bMonitorOnly,
            /* [out][in] */ long *pSubscriberId,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnregisterSubscriber )( 
            IEventManager * This,
            /* [in] */ long nSubscriberId,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateEvent )( 
            IEventManager * This,
            /* [in] */ long nProviderId,
            /* [in] */ IEventEx *pEvent,
            VARIANT_BOOL bBroadcast,
            /* [out][in] */ long *pEventId,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CommitEvent )( 
            IEventManager * This,
            /* [in] */ long nSubscriberId,
            /* [in] */ IEventEx *pEvent,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CancelEvent )( 
            IEventManager * This,
            /* [in] */ long nSubscriberId,
            /* [in] */ IEventEx *pEvent,
            /* [retval][out] */ long *pError);
        
        END_INTERFACE
    } IEventManagerVtbl;

    interface IEventManager
    {
        CONST_VTBL struct IEventManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEventManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEventManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEventManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEventManager_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IEventManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IEventManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IEventManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IEventManager_RegisterProvider(This,pProvider,pProviderId,pError)	\
    (This)->lpVtbl -> RegisterProvider(This,pProvider,pProviderId,pError)

#define IEventManager_UnregisterProvider(This,nProviderId,pError)	\
    (This)->lpVtbl -> UnregisterProvider(This,nProviderId,pError)

#define IEventManager_RegisterSubscriber(This,pSubscriber,nEventType,nPriority,vEventStates,nProviderId,bMonitorOnly,pSubscriberId,pError)	\
    (This)->lpVtbl -> RegisterSubscriber(This,pSubscriber,nEventType,nPriority,vEventStates,nProviderId,bMonitorOnly,pSubscriberId,pError)

#define IEventManager_UnregisterSubscriber(This,nSubscriberId,pError)	\
    (This)->lpVtbl -> UnregisterSubscriber(This,nSubscriberId,pError)

#define IEventManager_CreateEvent(This,nProviderId,pEvent,bBroadcast,pEventId,pError)	\
    (This)->lpVtbl -> CreateEvent(This,nProviderId,pEvent,bBroadcast,pEventId,pError)

#define IEventManager_CommitEvent(This,nSubscriberId,pEvent,pError)	\
    (This)->lpVtbl -> CommitEvent(This,nSubscriberId,pEvent,pError)

#define IEventManager_CancelEvent(This,nSubscriberId,pEvent,pError)	\
    (This)->lpVtbl -> CancelEvent(This,nSubscriberId,pEvent,pError)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventManager_RegisterProvider_Proxy( 
    IEventManager * This,
    /* [in] */ IProviderEx *pProvider,
    /* [out][in] */ long *pProviderId,
    /* [retval][out] */ long *pError);


void __RPC_STUB IEventManager_RegisterProvider_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventManager_UnregisterProvider_Proxy( 
    IEventManager * This,
    /* [in] */ long nProviderId,
    /* [retval][out] */ long *pError);


void __RPC_STUB IEventManager_UnregisterProvider_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventManager_RegisterSubscriber_Proxy( 
    IEventManager * This,
    /* [in] */ ISubscriberEx *pSubscriber,
    /* [in] */ long nEventType,
    /* [in] */ long nPriority,
    /* [in] */ VARIANT vEventStates,
    /* [in] */ long nProviderId,
    /* [in] */ VARIANT_BOOL bMonitorOnly,
    /* [out][in] */ long *pSubscriberId,
    /* [retval][out] */ long *pError);


void __RPC_STUB IEventManager_RegisterSubscriber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventManager_UnregisterSubscriber_Proxy( 
    IEventManager * This,
    /* [in] */ long nSubscriberId,
    /* [retval][out] */ long *pError);


void __RPC_STUB IEventManager_UnregisterSubscriber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventManager_CreateEvent_Proxy( 
    IEventManager * This,
    /* [in] */ long nProviderId,
    /* [in] */ IEventEx *pEvent,
    VARIANT_BOOL bBroadcast,
    /* [out][in] */ long *pEventId,
    /* [retval][out] */ long *pError);


void __RPC_STUB IEventManager_CreateEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventManager_CommitEvent_Proxy( 
    IEventManager * This,
    /* [in] */ long nSubscriberId,
    /* [in] */ IEventEx *pEvent,
    /* [retval][out] */ long *pError);


void __RPC_STUB IEventManager_CommitEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IEventManager_CancelEvent_Proxy( 
    IEventManager * This,
    /* [in] */ long nSubscriberId,
    /* [in] */ IEventEx *pEvent,
    /* [retval][out] */ long *pError);


void __RPC_STUB IEventManager_CancelEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEventManager_INTERFACE_DEFINED__ */


#ifndef __ILogManager_INTERFACE_DEFINED__
#define __ILogManager_INTERFACE_DEFINED__

/* interface ILogManager */
/* [unique][helpstring][nonextensible][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_ILogManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("374AA35A-71B9-49AE-998F-EC0B6A414451")
    ILogManager : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadEvent( 
            /* [in] */ long nEventType,
            /* [in] */ long nIndex,
            /* [in] */ long nIndexCheck,
            /* [in] */ long nContextId,
            /* [out][in] */ IEventEx **pEvent,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteEvent( 
            /* [in] */ long nEventType,
            /* [in] */ long nIndex,
            /* [in] */ long nIndexCheck,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SearchEvents( 
            /* [in] */ long nEventType,
            /* [in] */ DATE dtTimeStart,
            /* [in] */ DATE dtTimeStop,
            /* [in] */ VARIANT_BOOL bCreateContext,
            /* [in] */ long nLimitCount,
            /* [out][in] */ long *pContextId,
            /* [out][in] */ VARIANT *pIndexArray,
            /* [out][in] */ VARIANT *pIndexCheckArray,
            /* [out][in] */ VARIANT *pTimeStampArray,
            /* [out][in] */ VARIANT *pSequenceNumberArray,
            /* [out][in] */ long *pEventCount,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteSearchContext( 
            /* [in] */ long nEventType,
            /* [in] */ long nContextId,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearLog( 
            /* [in] */ long nEventType,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMaxLogSize( 
            /* [in] */ long nEventType,
            /* [in] */ long nSize,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMaxLogSize( 
            /* [in] */ long nEventType,
            /* [out][in] */ long *pSize,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetLogEnabled( 
            /* [in] */ long nEventType,
            /* [in] */ VARIANT_BOOL bEnabled,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLogEnabled( 
            /* [in] */ long nEventType,
            /* [out][in] */ VARIANT_BOOL *pEnabled,
            /* [retval][out] */ long *pError) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILogManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILogManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILogManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILogManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ILogManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ILogManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ILogManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ILogManager * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadEvent )( 
            ILogManager * This,
            /* [in] */ long nEventType,
            /* [in] */ long nIndex,
            /* [in] */ long nIndexCheck,
            /* [in] */ long nContextId,
            /* [out][in] */ IEventEx **pEvent,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteEvent )( 
            ILogManager * This,
            /* [in] */ long nEventType,
            /* [in] */ long nIndex,
            /* [in] */ long nIndexCheck,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SearchEvents )( 
            ILogManager * This,
            /* [in] */ long nEventType,
            /* [in] */ DATE dtTimeStart,
            /* [in] */ DATE dtTimeStop,
            /* [in] */ VARIANT_BOOL bCreateContext,
            /* [in] */ long nLimitCount,
            /* [out][in] */ long *pContextId,
            /* [out][in] */ VARIANT *pIndexArray,
            /* [out][in] */ VARIANT *pIndexCheckArray,
            /* [out][in] */ VARIANT *pTimeStampArray,
            /* [out][in] */ VARIANT *pSequenceNumberArray,
            /* [out][in] */ long *pEventCount,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteSearchContext )( 
            ILogManager * This,
            /* [in] */ long nEventType,
            /* [in] */ long nContextId,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearLog )( 
            ILogManager * This,
            /* [in] */ long nEventType,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMaxLogSize )( 
            ILogManager * This,
            /* [in] */ long nEventType,
            /* [in] */ long nSize,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMaxLogSize )( 
            ILogManager * This,
            /* [in] */ long nEventType,
            /* [out][in] */ long *pSize,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLogEnabled )( 
            ILogManager * This,
            /* [in] */ long nEventType,
            /* [in] */ VARIANT_BOOL bEnabled,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLogEnabled )( 
            ILogManager * This,
            /* [in] */ long nEventType,
            /* [out][in] */ VARIANT_BOOL *pEnabled,
            /* [retval][out] */ long *pError);
        
        END_INTERFACE
    } ILogManagerVtbl;

    interface ILogManager
    {
        CONST_VTBL struct ILogManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILogManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILogManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILogManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILogManager_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ILogManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ILogManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ILogManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ILogManager_LoadEvent(This,nEventType,nIndex,nIndexCheck,nContextId,pEvent,pError)	\
    (This)->lpVtbl -> LoadEvent(This,nEventType,nIndex,nIndexCheck,nContextId,pEvent,pError)

#define ILogManager_DeleteEvent(This,nEventType,nIndex,nIndexCheck,pError)	\
    (This)->lpVtbl -> DeleteEvent(This,nEventType,nIndex,nIndexCheck,pError)

#define ILogManager_SearchEvents(This,nEventType,dtTimeStart,dtTimeStop,bCreateContext,nLimitCount,pContextId,pIndexArray,pIndexCheckArray,pTimeStampArray,pSequenceNumberArray,pEventCount,pError)	\
    (This)->lpVtbl -> SearchEvents(This,nEventType,dtTimeStart,dtTimeStop,bCreateContext,nLimitCount,pContextId,pIndexArray,pIndexCheckArray,pTimeStampArray,pSequenceNumberArray,pEventCount,pError)

#define ILogManager_DeleteSearchContext(This,nEventType,nContextId,pError)	\
    (This)->lpVtbl -> DeleteSearchContext(This,nEventType,nContextId,pError)

#define ILogManager_ClearLog(This,nEventType,pError)	\
    (This)->lpVtbl -> ClearLog(This,nEventType,pError)

#define ILogManager_SetMaxLogSize(This,nEventType,nSize,pError)	\
    (This)->lpVtbl -> SetMaxLogSize(This,nEventType,nSize,pError)

#define ILogManager_GetMaxLogSize(This,nEventType,pSize,pError)	\
    (This)->lpVtbl -> GetMaxLogSize(This,nEventType,pSize,pError)

#define ILogManager_SetLogEnabled(This,nEventType,bEnabled,pError)	\
    (This)->lpVtbl -> SetLogEnabled(This,nEventType,bEnabled,pError)

#define ILogManager_GetLogEnabled(This,nEventType,pEnabled,pError)	\
    (This)->lpVtbl -> GetLogEnabled(This,nEventType,pEnabled,pError)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager_LoadEvent_Proxy( 
    ILogManager * This,
    /* [in] */ long nEventType,
    /* [in] */ long nIndex,
    /* [in] */ long nIndexCheck,
    /* [in] */ long nContextId,
    /* [out][in] */ IEventEx **pEvent,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager_LoadEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager_DeleteEvent_Proxy( 
    ILogManager * This,
    /* [in] */ long nEventType,
    /* [in] */ long nIndex,
    /* [in] */ long nIndexCheck,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager_DeleteEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager_SearchEvents_Proxy( 
    ILogManager * This,
    /* [in] */ long nEventType,
    /* [in] */ DATE dtTimeStart,
    /* [in] */ DATE dtTimeStop,
    /* [in] */ VARIANT_BOOL bCreateContext,
    /* [in] */ long nLimitCount,
    /* [out][in] */ long *pContextId,
    /* [out][in] */ VARIANT *pIndexArray,
    /* [out][in] */ VARIANT *pIndexCheckArray,
    /* [out][in] */ VARIANT *pTimeStampArray,
    /* [out][in] */ VARIANT *pSequenceNumberArray,
    /* [out][in] */ long *pEventCount,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager_SearchEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager_DeleteSearchContext_Proxy( 
    ILogManager * This,
    /* [in] */ long nEventType,
    /* [in] */ long nContextId,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager_DeleteSearchContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager_ClearLog_Proxy( 
    ILogManager * This,
    /* [in] */ long nEventType,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager_ClearLog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager_SetMaxLogSize_Proxy( 
    ILogManager * This,
    /* [in] */ long nEventType,
    /* [in] */ long nSize,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager_SetMaxLogSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager_GetMaxLogSize_Proxy( 
    ILogManager * This,
    /* [in] */ long nEventType,
    /* [out][in] */ long *pSize,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager_GetMaxLogSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager_SetLogEnabled_Proxy( 
    ILogManager * This,
    /* [in] */ long nEventType,
    /* [in] */ VARIANT_BOOL bEnabled,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager_SetLogEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager_GetLogEnabled_Proxy( 
    ILogManager * This,
    /* [in] */ long nEventType,
    /* [out][in] */ VARIANT_BOOL *pEnabled,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager_GetLogEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILogManager_INTERFACE_DEFINED__ */


#ifndef __ILogManager2_INTERFACE_DEFINED__
#define __ILogManager2_INTERFACE_DEFINED__

/* interface ILogManager2 */
/* [unique][helpstring][nonextensible][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_ILogManager2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D90EF5C6-89D8-486a-8EB7-3E4A84F3BFC0")
    ILogManager2 : public ILogManager
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadEvent( 
            /* [in] */ long nEventType,
            /* [in] */ long nLowIndex,
            /* [in] */ long nHighIndex,
            /* [in] */ long nIndexCheck,
            /* [in] */ long nContextId,
            /* [out][in] */ IEventEx **pEvent,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteEvent( 
            /* [in] */ long nEventType,
            /* [in] */ long nLowIndex,
            /* [in] */ long nHighIndex,
            /* [in] */ long nIndexCheck,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetMaxLogSize( 
            /* [in] */ long nEventType,
            /* [in] */ long nLowSize,
            /* [in] */ long nHighSize,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMaxLogSize( 
            /* [in] */ long nEventType,
            /* [out][in] */ long *pLowSize,
            /* [out][in] */ long *pHighSize,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SearchEvents( 
            /* [in] */ long nEventType,
            /* [in] */ DATE dtTimeStart,
            /* [in] */ DATE dtTimeStop,
            /* [in] */ VARIANT_BOOL bCreateContext,
            /* [in] */ long nLimitCount,
            /* [out][in] */ long *pContextId,
            /* [out][in] */ VARIANT *pIndexLowArray,
            /* [out][in] */ VARIANT *pIndexHighArray,
            /* [out][in] */ VARIANT *pIndexCheckArray,
            /* [out][in] */ VARIANT *pEventTypeArray,
            /* [out][in] */ VARIANT *pTimeStampArray,
            /* [out][in] */ VARIANT *pSequenceNumberArray,
            /* [out][in] */ long *pEventCount,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoadEventBatch( 
            /* [in] */ long nEventType,
            /* [in] */ VARIANT *pIndexLowArray,
            /* [in] */ VARIANT *pIndexHighArray,
            /* [in] */ VARIANT *pIndexCheckArray,
            /* [in] */ long nContextId,
            /* [out][in] */ VARIANT *pEventTypeArray,
            /* [out][in] */ VARIANT *pEventArray,
            /* [retval][out] */ long *pError) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILogManager2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILogManager2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILogManager2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILogManager2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ILogManager2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ILogManager2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ILogManager2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ILogManager2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadEvent )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [in] */ long nIndex,
            /* [in] */ long nIndexCheck,
            /* [in] */ long nContextId,
            /* [out][in] */ IEventEx **pEvent,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteEvent )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [in] */ long nIndex,
            /* [in] */ long nIndexCheck,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SearchEvents )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [in] */ DATE dtTimeStart,
            /* [in] */ DATE dtTimeStop,
            /* [in] */ VARIANT_BOOL bCreateContext,
            /* [in] */ long nLimitCount,
            /* [out][in] */ long *pContextId,
            /* [out][in] */ VARIANT *pIndexArray,
            /* [out][in] */ VARIANT *pIndexCheckArray,
            /* [out][in] */ VARIANT *pTimeStampArray,
            /* [out][in] */ VARIANT *pSequenceNumberArray,
            /* [out][in] */ long *pEventCount,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteSearchContext )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [in] */ long nContextId,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearLog )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMaxLogSize )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [in] */ long nSize,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMaxLogSize )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [out][in] */ long *pSize,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetLogEnabled )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [in] */ VARIANT_BOOL bEnabled,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetLogEnabled )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [out][in] */ VARIANT_BOOL *pEnabled,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadEvent )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [in] */ long nLowIndex,
            /* [in] */ long nHighIndex,
            /* [in] */ long nIndexCheck,
            /* [in] */ long nContextId,
            /* [out][in] */ IEventEx **pEvent,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteEvent )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [in] */ long nLowIndex,
            /* [in] */ long nHighIndex,
            /* [in] */ long nIndexCheck,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetMaxLogSize )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [in] */ long nLowSize,
            /* [in] */ long nHighSize,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetMaxLogSize )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [out][in] */ long *pLowSize,
            /* [out][in] */ long *pHighSize,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SearchEvents )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [in] */ DATE dtTimeStart,
            /* [in] */ DATE dtTimeStop,
            /* [in] */ VARIANT_BOOL bCreateContext,
            /* [in] */ long nLimitCount,
            /* [out][in] */ long *pContextId,
            /* [out][in] */ VARIANT *pIndexLowArray,
            /* [out][in] */ VARIANT *pIndexHighArray,
            /* [out][in] */ VARIANT *pIndexCheckArray,
            /* [out][in] */ VARIANT *pEventTypeArray,
            /* [out][in] */ VARIANT *pTimeStampArray,
            /* [out][in] */ VARIANT *pSequenceNumberArray,
            /* [out][in] */ long *pEventCount,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoadEventBatch )( 
            ILogManager2 * This,
            /* [in] */ long nEventType,
            /* [in] */ VARIANT *pIndexLowArray,
            /* [in] */ VARIANT *pIndexHighArray,
            /* [in] */ VARIANT *pIndexCheckArray,
            /* [in] */ long nContextId,
            /* [out][in] */ VARIANT *pEventTypeArray,
            /* [out][in] */ VARIANT *pEventArray,
            /* [retval][out] */ long *pError);
        
        END_INTERFACE
    } ILogManager2Vtbl;

    interface ILogManager2
    {
        CONST_VTBL struct ILogManager2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILogManager2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILogManager2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILogManager2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILogManager2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ILogManager2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ILogManager2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ILogManager2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ILogManager2_LoadEvent(This,nEventType,nIndex,nIndexCheck,nContextId,pEvent,pError)	\
    (This)->lpVtbl -> LoadEvent(This,nEventType,nIndex,nIndexCheck,nContextId,pEvent,pError)

#define ILogManager2_DeleteEvent(This,nEventType,nIndex,nIndexCheck,pError)	\
    (This)->lpVtbl -> DeleteEvent(This,nEventType,nIndex,nIndexCheck,pError)

#define ILogManager2_SearchEvents(This,nEventType,dtTimeStart,dtTimeStop,bCreateContext,nLimitCount,pContextId,pIndexArray,pIndexCheckArray,pTimeStampArray,pSequenceNumberArray,pEventCount,pError)	\
    (This)->lpVtbl -> SearchEvents(This,nEventType,dtTimeStart,dtTimeStop,bCreateContext,nLimitCount,pContextId,pIndexArray,pIndexCheckArray,pTimeStampArray,pSequenceNumberArray,pEventCount,pError)

#define ILogManager2_DeleteSearchContext(This,nEventType,nContextId,pError)	\
    (This)->lpVtbl -> DeleteSearchContext(This,nEventType,nContextId,pError)

#define ILogManager2_ClearLog(This,nEventType,pError)	\
    (This)->lpVtbl -> ClearLog(This,nEventType,pError)

#define ILogManager2_SetMaxLogSize(This,nEventType,nSize,pError)	\
    (This)->lpVtbl -> SetMaxLogSize(This,nEventType,nSize,pError)

#define ILogManager2_GetMaxLogSize(This,nEventType,pSize,pError)	\
    (This)->lpVtbl -> GetMaxLogSize(This,nEventType,pSize,pError)

#define ILogManager2_SetLogEnabled(This,nEventType,bEnabled,pError)	\
    (This)->lpVtbl -> SetLogEnabled(This,nEventType,bEnabled,pError)

#define ILogManager2_GetLogEnabled(This,nEventType,pEnabled,pError)	\
    (This)->lpVtbl -> GetLogEnabled(This,nEventType,pEnabled,pError)


#define ILogManager2_LoadEvent(This,nEventType,nLowIndex,nHighIndex,nIndexCheck,nContextId,pEvent,pError)	\
    (This)->lpVtbl -> LoadEvent(This,nEventType,nLowIndex,nHighIndex,nIndexCheck,nContextId,pEvent,pError)

#define ILogManager2_DeleteEvent(This,nEventType,nLowIndex,nHighIndex,nIndexCheck,pError)	\
    (This)->lpVtbl -> DeleteEvent(This,nEventType,nLowIndex,nHighIndex,nIndexCheck,pError)

#define ILogManager2_SetMaxLogSize(This,nEventType,nLowSize,nHighSize,pError)	\
    (This)->lpVtbl -> SetMaxLogSize(This,nEventType,nLowSize,nHighSize,pError)

#define ILogManager2_GetMaxLogSize(This,nEventType,pLowSize,pHighSize,pError)	\
    (This)->lpVtbl -> GetMaxLogSize(This,nEventType,pLowSize,pHighSize,pError)

#define ILogManager2_SearchEvents(This,nEventType,dtTimeStart,dtTimeStop,bCreateContext,nLimitCount,pContextId,pIndexLowArray,pIndexHighArray,pIndexCheckArray,pEventTypeArray,pTimeStampArray,pSequenceNumberArray,pEventCount,pError)	\
    (This)->lpVtbl -> SearchEvents(This,nEventType,dtTimeStart,dtTimeStop,bCreateContext,nLimitCount,pContextId,pIndexLowArray,pIndexHighArray,pIndexCheckArray,pEventTypeArray,pTimeStampArray,pSequenceNumberArray,pEventCount,pError)

#define ILogManager2_LoadEventBatch(This,nEventType,pIndexLowArray,pIndexHighArray,pIndexCheckArray,nContextId,pEventTypeArray,pEventArray,pError)	\
    (This)->lpVtbl -> LoadEventBatch(This,nEventType,pIndexLowArray,pIndexHighArray,pIndexCheckArray,nContextId,pEventTypeArray,pEventArray,pError)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager2_LoadEvent_Proxy( 
    ILogManager2 * This,
    /* [in] */ long nEventType,
    /* [in] */ long nLowIndex,
    /* [in] */ long nHighIndex,
    /* [in] */ long nIndexCheck,
    /* [in] */ long nContextId,
    /* [out][in] */ IEventEx **pEvent,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager2_LoadEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager2_DeleteEvent_Proxy( 
    ILogManager2 * This,
    /* [in] */ long nEventType,
    /* [in] */ long nLowIndex,
    /* [in] */ long nHighIndex,
    /* [in] */ long nIndexCheck,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager2_DeleteEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager2_SetMaxLogSize_Proxy( 
    ILogManager2 * This,
    /* [in] */ long nEventType,
    /* [in] */ long nLowSize,
    /* [in] */ long nHighSize,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager2_SetMaxLogSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager2_GetMaxLogSize_Proxy( 
    ILogManager2 * This,
    /* [in] */ long nEventType,
    /* [out][in] */ long *pLowSize,
    /* [out][in] */ long *pHighSize,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager2_GetMaxLogSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager2_SearchEvents_Proxy( 
    ILogManager2 * This,
    /* [in] */ long nEventType,
    /* [in] */ DATE dtTimeStart,
    /* [in] */ DATE dtTimeStop,
    /* [in] */ VARIANT_BOOL bCreateContext,
    /* [in] */ long nLimitCount,
    /* [out][in] */ long *pContextId,
    /* [out][in] */ VARIANT *pIndexLowArray,
    /* [out][in] */ VARIANT *pIndexHighArray,
    /* [out][in] */ VARIANT *pIndexCheckArray,
    /* [out][in] */ VARIANT *pEventTypeArray,
    /* [out][in] */ VARIANT *pTimeStampArray,
    /* [out][in] */ VARIANT *pSequenceNumberArray,
    /* [out][in] */ long *pEventCount,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager2_SearchEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ILogManager2_LoadEventBatch_Proxy( 
    ILogManager2 * This,
    /* [in] */ long nEventType,
    /* [in] */ VARIANT *pIndexLowArray,
    /* [in] */ VARIANT *pIndexHighArray,
    /* [in] */ VARIANT *pIndexCheckArray,
    /* [in] */ long nContextId,
    /* [out][in] */ VARIANT *pEventTypeArray,
    /* [out][in] */ VARIANT *pEventArray,
    /* [retval][out] */ long *pError);


void __RPC_STUB ILogManager2_LoadEventBatch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILogManager2_INTERFACE_DEFINED__ */


#ifndef __IModuleManager_INTERFACE_DEFINED__
#define __IModuleManager_INTERFACE_DEFINED__

/* interface IModuleManager */
/* [unique][helpstring][nonextensible][oleautomation][dual][uuid][object] */ 


EXTERN_C const IID IID_IModuleManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("072553DD-20A3-4CF0-B431-48FE5C913283")
    IModuleManager : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Load( 
            /* [in] */ BSTR sModuleName,
            /* [in] */ VARIANT_BOOL bPersist,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Unload( 
            /* [in] */ BSTR sModuleName,
            /* [in] */ VARIANT_BOOL bPersist,
            /* [retval][out] */ long *pError) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IsLoaded( 
            /* [in] */ BSTR sModuleName,
            /* [retval][out] */ VARIANT_BOOL *pLoaded) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IModuleManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IModuleManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IModuleManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IModuleManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IModuleManager * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IModuleManager * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IModuleManager * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IModuleManager * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Load )( 
            IModuleManager * This,
            /* [in] */ BSTR sModuleName,
            /* [in] */ VARIANT_BOOL bPersist,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Unload )( 
            IModuleManager * This,
            /* [in] */ BSTR sModuleName,
            /* [in] */ VARIANT_BOOL bPersist,
            /* [retval][out] */ long *pError);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *IsLoaded )( 
            IModuleManager * This,
            /* [in] */ BSTR sModuleName,
            /* [retval][out] */ VARIANT_BOOL *pLoaded);
        
        END_INTERFACE
    } IModuleManagerVtbl;

    interface IModuleManager
    {
        CONST_VTBL struct IModuleManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IModuleManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IModuleManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IModuleManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IModuleManager_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IModuleManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IModuleManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IModuleManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IModuleManager_Load(This,sModuleName,bPersist,pError)	\
    (This)->lpVtbl -> Load(This,sModuleName,bPersist,pError)

#define IModuleManager_Unload(This,sModuleName,bPersist,pError)	\
    (This)->lpVtbl -> Unload(This,sModuleName,bPersist,pError)

#define IModuleManager_IsLoaded(This,sModuleName,pLoaded)	\
    (This)->lpVtbl -> IsLoaded(This,sModuleName,pLoaded)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IModuleManager_Load_Proxy( 
    IModuleManager * This,
    /* [in] */ BSTR sModuleName,
    /* [in] */ VARIANT_BOOL bPersist,
    /* [retval][out] */ long *pError);


void __RPC_STUB IModuleManager_Load_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IModuleManager_Unload_Proxy( 
    IModuleManager * This,
    /* [in] */ BSTR sModuleName,
    /* [in] */ VARIANT_BOOL bPersist,
    /* [retval][out] */ long *pError);


void __RPC_STUB IModuleManager_Unload_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IModuleManager_IsLoaded_Proxy( 
    IModuleManager * This,
    /* [in] */ BSTR sModuleName,
    /* [retval][out] */ VARIANT_BOOL *pLoaded);


void __RPC_STUB IModuleManager_IsLoaded_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IModuleManager_INTERFACE_DEFINED__ */



#ifndef __ccEvtMgrLib_LIBRARY_DEFINED__
#define __ccEvtMgrLib_LIBRARY_DEFINED__

/* library ccEvtMgrLib */
/* [helpstring][version][uuid] */ 










EXTERN_C const IID LIBID_ccEvtMgrLib;

EXTERN_C const CLSID CLSID_EventManager;

#ifdef __cplusplus

class DECLSPEC_UUID("49BB73EE-2C2F-445E-82E3-E6E3380285BF")
EventManager;
#endif

EXTERN_C const CLSID CLSID_LogManager;

#ifdef __cplusplus

class DECLSPEC_UUID("B8E914C1-A516-421F-B413-B32B3FA3F18F")
LogManager;
#endif

EXTERN_C const CLSID CLSID_ModuleManager;

#ifdef __cplusplus

class DECLSPEC_UUID("311CF1A1-872A-4ED5-943F-058C886E2F7F")
ModuleManager;
#endif
#endif /* __ccEvtMgrLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


