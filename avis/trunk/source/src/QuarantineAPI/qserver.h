/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Wed Apr 07 16:16:04 1999
 */
/* Compiler settings for C:\work\Norton_AntiVirus\QuarantineServer\Qserver\qserver.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
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

#ifndef __qserver_h__
#define __qserver_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IQuarantineServerItem_FWD_DEFINED__
#define __IQuarantineServerItem_FWD_DEFINED__
typedef interface IQuarantineServerItem IQuarantineServerItem;
#endif 	/* __IQuarantineServerItem_FWD_DEFINED__ */


#ifndef __IEnumQuarantineServerItems_FWD_DEFINED__
#define __IEnumQuarantineServerItems_FWD_DEFINED__
typedef interface IEnumQuarantineServerItems IEnumQuarantineServerItems;
#endif 	/* __IEnumQuarantineServerItems_FWD_DEFINED__ */


#ifndef __IEnumQserverItemsVariant_FWD_DEFINED__
#define __IEnumQserverItemsVariant_FWD_DEFINED__
typedef interface IEnumQserverItemsVariant IEnumQserverItemsVariant;
#endif 	/* __IEnumQserverItemsVariant_FWD_DEFINED__ */


#ifndef __ICopyItemData_FWD_DEFINED__
#define __ICopyItemData_FWD_DEFINED__
typedef interface ICopyItemData ICopyItemData;
#endif 	/* __ICopyItemData_FWD_DEFINED__ */


#ifndef __IQserverConfig_FWD_DEFINED__
#define __IQserverConfig_FWD_DEFINED__
typedef interface IQserverConfig IQserverConfig;
#endif 	/* __IQserverConfig_FWD_DEFINED__ */


#ifndef __IQuarantineServer_FWD_DEFINED__
#define __IQuarantineServer_FWD_DEFINED__
typedef interface IQuarantineServer IQuarantineServer;
#endif 	/* __IQuarantineServer_FWD_DEFINED__ */


#ifndef __QuarantineServer_FWD_DEFINED__
#define __QuarantineServer_FWD_DEFINED__

#ifdef __cplusplus
typedef class QuarantineServer QuarantineServer;
#else
typedef struct QuarantineServer QuarantineServer;
#endif /* __cplusplus */

#endif 	/* __QuarantineServer_FWD_DEFINED__ */


#ifndef __QuarantineServerItem_FWD_DEFINED__
#define __QuarantineServerItem_FWD_DEFINED__

#ifdef __cplusplus
typedef class QuarantineServerItem QuarantineServerItem;
#else
typedef struct QuarantineServerItem QuarantineServerItem;
#endif /* __cplusplus */

#endif 	/* __QuarantineServerItem_FWD_DEFINED__ */


#ifndef __EnumQserverItemsVariant_FWD_DEFINED__
#define __EnumQserverItemsVariant_FWD_DEFINED__

#ifdef __cplusplus
typedef class EnumQserverItemsVariant EnumQserverItemsVariant;
#else
typedef struct EnumQserverItemsVariant EnumQserverItemsVariant;
#endif /* __cplusplus */

#endif 	/* __EnumQserverItemsVariant_FWD_DEFINED__ */


#ifndef __QserverConfig_FWD_DEFINED__
#define __QserverConfig_FWD_DEFINED__

#ifdef __cplusplus
typedef class QserverConfig QserverConfig;
#else
typedef struct QserverConfig QserverConfig;
#endif /* __cplusplus */

#endif 	/* __QserverConfig_FWD_DEFINED__ */


#ifndef __CopyItemData_FWD_DEFINED__
#define __CopyItemData_FWD_DEFINED__

#ifdef __cplusplus
typedef class CopyItemData CopyItemData;
#else
typedef struct CopyItemData CopyItemData;
#endif /* __cplusplus */

#endif 	/* __CopyItemData_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IQuarantineServerItem_INTERFACE_DEFINED__
#define __IQuarantineServerItem_INTERFACE_DEFINED__

/* interface IQuarantineServerItem */
/* [restricted][unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IQuarantineServerItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3DA647F1-ABFB-11D2-8E6D-281ED2000000")
    IQuarantineServerItem : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetValue( 
            /* [in] */ BSTR bstrFieldName,
            /* [out] */ VARIANT __RPC_FAR *v) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetValue( 
            /* [in] */ BSTR bstrFieldName,
            /* [in] */ VARIANT v) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetAllValues( 
            /* [out] */ VARIANT __RPC_FAR *v) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Commit( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IQuarantineServerItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IQuarantineServerItem __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IQuarantineServerItem __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IQuarantineServerItem __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetValue )( 
            IQuarantineServerItem __RPC_FAR * This,
            /* [in] */ BSTR bstrFieldName,
            /* [out] */ VARIANT __RPC_FAR *v);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetValue )( 
            IQuarantineServerItem __RPC_FAR * This,
            /* [in] */ BSTR bstrFieldName,
            /* [in] */ VARIANT v);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAllValues )( 
            IQuarantineServerItem __RPC_FAR * This,
            /* [out] */ VARIANT __RPC_FAR *v);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Commit )( 
            IQuarantineServerItem __RPC_FAR * This);
        
        END_INTERFACE
    } IQuarantineServerItemVtbl;

    interface IQuarantineServerItem
    {
        CONST_VTBL struct IQuarantineServerItemVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IQuarantineServerItem_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IQuarantineServerItem_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IQuarantineServerItem_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IQuarantineServerItem_GetValue(This,bstrFieldName,v)	\
    (This)->lpVtbl -> GetValue(This,bstrFieldName,v)

#define IQuarantineServerItem_SetValue(This,bstrFieldName,v)	\
    (This)->lpVtbl -> SetValue(This,bstrFieldName,v)

#define IQuarantineServerItem_GetAllValues(This,v)	\
    (This)->lpVtbl -> GetAllValues(This,v)

#define IQuarantineServerItem_Commit(This)	\
    (This)->lpVtbl -> Commit(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQuarantineServerItem_GetValue_Proxy( 
    IQuarantineServerItem __RPC_FAR * This,
    /* [in] */ BSTR bstrFieldName,
    /* [out] */ VARIANT __RPC_FAR *v);


void __RPC_STUB IQuarantineServerItem_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQuarantineServerItem_SetValue_Proxy( 
    IQuarantineServerItem __RPC_FAR * This,
    /* [in] */ BSTR bstrFieldName,
    /* [in] */ VARIANT v);


void __RPC_STUB IQuarantineServerItem_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQuarantineServerItem_GetAllValues_Proxy( 
    IQuarantineServerItem __RPC_FAR * This,
    /* [out] */ VARIANT __RPC_FAR *v);


void __RPC_STUB IQuarantineServerItem_GetAllValues_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQuarantineServerItem_Commit_Proxy( 
    IQuarantineServerItem __RPC_FAR * This);


void __RPC_STUB IQuarantineServerItem_Commit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IQuarantineServerItem_INTERFACE_DEFINED__ */


#ifndef __IEnumQuarantineServerItems_INTERFACE_DEFINED__
#define __IEnumQuarantineServerItems_INTERFACE_DEFINED__

/* interface IEnumQuarantineServerItems */
/* [restricted][unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IEnumQuarantineServerItems;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A3298930-8967-11d2-8E5F-2A0712000000")
    IEnumQuarantineServerItems : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IQuarantineServerItem __RPC_FAR *__RPC_FAR *aQserverItems,
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumQuarantineServerItems __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumQuarantineServerItemsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumQuarantineServerItems __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumQuarantineServerItems __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumQuarantineServerItems __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumQuarantineServerItems __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IQuarantineServerItem __RPC_FAR *__RPC_FAR *aQserverItems,
            /* [out] */ ULONG __RPC_FAR *pceltFetched);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumQuarantineServerItems __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumQuarantineServerItems __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumQuarantineServerItems __RPC_FAR * This,
            /* [out] */ IEnumQuarantineServerItems __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IEnumQuarantineServerItemsVtbl;

    interface IEnumQuarantineServerItems
    {
        CONST_VTBL struct IEnumQuarantineServerItemsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumQuarantineServerItems_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumQuarantineServerItems_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumQuarantineServerItems_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumQuarantineServerItems_Next(This,celt,aQserverItems,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,aQserverItems,pceltFetched)

#define IEnumQuarantineServerItems_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumQuarantineServerItems_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumQuarantineServerItems_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumQuarantineServerItems_Next_Proxy( 
    IEnumQuarantineServerItems __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IQuarantineServerItem __RPC_FAR *__RPC_FAR *aQserverItems,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumQuarantineServerItems_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumQuarantineServerItems_Skip_Proxy( 
    IEnumQuarantineServerItems __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumQuarantineServerItems_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumQuarantineServerItems_Reset_Proxy( 
    IEnumQuarantineServerItems __RPC_FAR * This);


void __RPC_STUB IEnumQuarantineServerItems_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumQuarantineServerItems_Clone_Proxy( 
    IEnumQuarantineServerItems __RPC_FAR * This,
    /* [out] */ IEnumQuarantineServerItems __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumQuarantineServerItems_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumQuarantineServerItems_INTERFACE_DEFINED__ */


#ifndef __IEnumQserverItemsVariant_INTERFACE_DEFINED__
#define __IEnumQserverItemsVariant_INTERFACE_DEFINED__

/* interface IEnumQserverItemsVariant */
/* [restricted][unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IEnumQserverItemsVariant;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1F5FCDD6-B099-11D2-B65E-0020AFEED321")
    IEnumQserverItemsVariant : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ VARIANT __RPC_FAR *aQserverItems,
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumQserverItemsVariant __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumQserverItemsVariantVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IEnumQserverItemsVariant __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IEnumQserverItemsVariant __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IEnumQserverItemsVariant __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            IEnumQserverItemsVariant __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ VARIANT __RPC_FAR *aQserverItems,
            /* [out] */ ULONG __RPC_FAR *pceltFetched);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            IEnumQserverItemsVariant __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            IEnumQserverItemsVariant __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            IEnumQserverItemsVariant __RPC_FAR * This,
            /* [out] */ IEnumQserverItemsVariant __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } IEnumQserverItemsVariantVtbl;

    interface IEnumQserverItemsVariant
    {
        CONST_VTBL struct IEnumQserverItemsVariantVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumQserverItemsVariant_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumQserverItemsVariant_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumQserverItemsVariant_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumQserverItemsVariant_Next(This,celt,aQserverItems,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,aQserverItems,pceltFetched)

#define IEnumQserverItemsVariant_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumQserverItemsVariant_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumQserverItemsVariant_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumQserverItemsVariant_Next_Proxy( 
    IEnumQserverItemsVariant __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ VARIANT __RPC_FAR *aQserverItems,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB IEnumQserverItemsVariant_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumQserverItemsVariant_Skip_Proxy( 
    IEnumQserverItemsVariant __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumQserverItemsVariant_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumQserverItemsVariant_Reset_Proxy( 
    IEnumQserverItemsVariant __RPC_FAR * This);


void __RPC_STUB IEnumQserverItemsVariant_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IEnumQserverItemsVariant_Clone_Proxy( 
    IEnumQserverItemsVariant __RPC_FAR * This,
    /* [out] */ IEnumQserverItemsVariant __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB IEnumQserverItemsVariant_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumQserverItemsVariant_INTERFACE_DEFINED__ */


#ifndef __ICopyItemData_INTERFACE_DEFINED__
#define __ICopyItemData_INTERFACE_DEFINED__

/* interface ICopyItemData */
/* [restricted][hidden][unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ICopyItemData;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E04E15C8-BD46-11D2-B661-0020AFEED321")
    ICopyItemData : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ ULONG ulFileID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSize( 
            /* [out] */ ULONG __RPC_FAR *pulFileSize) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Read( 
            /* [in] */ ULONG ulBufferSize,
            /* [out] */ ULONG __RPC_FAR *pulBytesRead,
            /* [length_is][size_is][out] */ BYTE __RPC_FAR *lpBuffer) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICopyItemDataVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICopyItemData __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICopyItemData __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICopyItemData __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            ICopyItemData __RPC_FAR * This,
            /* [in] */ ULONG ulFileID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetSize )( 
            ICopyItemData __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pulFileSize);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Read )( 
            ICopyItemData __RPC_FAR * This,
            /* [in] */ ULONG ulBufferSize,
            /* [out] */ ULONG __RPC_FAR *pulBytesRead,
            /* [length_is][size_is][out] */ BYTE __RPC_FAR *lpBuffer);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            ICopyItemData __RPC_FAR * This);
        
        END_INTERFACE
    } ICopyItemDataVtbl;

    interface ICopyItemData
    {
        CONST_VTBL struct ICopyItemDataVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICopyItemData_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICopyItemData_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICopyItemData_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICopyItemData_Open(This,ulFileID)	\
    (This)->lpVtbl -> Open(This,ulFileID)

#define ICopyItemData_GetSize(This,pulFileSize)	\
    (This)->lpVtbl -> GetSize(This,pulFileSize)

#define ICopyItemData_Read(This,ulBufferSize,pulBytesRead,lpBuffer)	\
    (This)->lpVtbl -> Read(This,ulBufferSize,pulBytesRead,lpBuffer)

#define ICopyItemData_Close(This)	\
    (This)->lpVtbl -> Close(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICopyItemData_Open_Proxy( 
    ICopyItemData __RPC_FAR * This,
    /* [in] */ ULONG ulFileID);


void __RPC_STUB ICopyItemData_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICopyItemData_GetSize_Proxy( 
    ICopyItemData __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pulFileSize);


void __RPC_STUB ICopyItemData_GetSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICopyItemData_Read_Proxy( 
    ICopyItemData __RPC_FAR * This,
    /* [in] */ ULONG ulBufferSize,
    /* [out] */ ULONG __RPC_FAR *pulBytesRead,
    /* [length_is][size_is][out] */ BYTE __RPC_FAR *lpBuffer);


void __RPC_STUB ICopyItemData_Read_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICopyItemData_Close_Proxy( 
    ICopyItemData __RPC_FAR * This);


void __RPC_STUB ICopyItemData_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICopyItemData_INTERFACE_DEFINED__ */


#ifndef __IQserverConfig_INTERFACE_DEFINED__
#define __IQserverConfig_INTERFACE_DEFINED__

/* interface IQserverConfig */
/* [restricted][unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IQserverConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F7BFE116-B480-11D2-B65E-0020AFEED321")
    IQserverConfig : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetValue( 
            /* [in] */ BSTR bstrFieldName,
            /* [out] */ VARIANT __RPC_FAR *v) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetValue( 
            /* [in] */ BSTR bstrFieldName,
            /* [in] */ VARIANT v) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Detach( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IQserverConfigVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IQserverConfig __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IQserverConfig __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IQserverConfig __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetValue )( 
            IQserverConfig __RPC_FAR * This,
            /* [in] */ BSTR bstrFieldName,
            /* [out] */ VARIANT __RPC_FAR *v);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetValue )( 
            IQserverConfig __RPC_FAR * This,
            /* [in] */ BSTR bstrFieldName,
            /* [in] */ VARIANT v);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Detach )( 
            IQserverConfig __RPC_FAR * This);
        
        END_INTERFACE
    } IQserverConfigVtbl;

    interface IQserverConfig
    {
        CONST_VTBL struct IQserverConfigVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IQserverConfig_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IQserverConfig_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IQserverConfig_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IQserverConfig_GetValue(This,bstrFieldName,v)	\
    (This)->lpVtbl -> GetValue(This,bstrFieldName,v)

#define IQserverConfig_SetValue(This,bstrFieldName,v)	\
    (This)->lpVtbl -> SetValue(This,bstrFieldName,v)

#define IQserverConfig_Detach(This)	\
    (This)->lpVtbl -> Detach(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQserverConfig_GetValue_Proxy( 
    IQserverConfig __RPC_FAR * This,
    /* [in] */ BSTR bstrFieldName,
    /* [out] */ VARIANT __RPC_FAR *v);


void __RPC_STUB IQserverConfig_GetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQserverConfig_SetValue_Proxy( 
    IQserverConfig __RPC_FAR * This,
    /* [in] */ BSTR bstrFieldName,
    /* [in] */ VARIANT v);


void __RPC_STUB IQserverConfig_SetValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQserverConfig_Detach_Proxy( 
    IQserverConfig __RPC_FAR * This);


void __RPC_STUB IQserverConfig_Detach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IQserverConfig_INTERFACE_DEFINED__ */


#ifndef __IQuarantineServer_INTERFACE_DEFINED__
#define __IQuarantineServer_INTERFACE_DEFINED__

/* interface IQuarantineServer */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IQuarantineServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C60B015F-894B-11D2-8E5F-2A0712000000")
    IQuarantineServer : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumItems( 
            /* [out] */ IEnumQuarantineServerItems __RPC_FAR *__RPC_FAR *pEnum) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCopyInterface( 
            /* [out] */ ICopyItemData __RPC_FAR *__RPC_FAR *pCopy) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RemoveItem( 
            /* [in] */ ULONG ulFileID) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RemoveAllItems( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumItemsVariant( 
            /* [in] */ VARIANT __RPC_FAR *va,
            /* [out] */ IEnumQserverItemsVariant __RPC_FAR *__RPC_FAR *pEnum) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetQuarantineItem( 
            /* [in] */ ULONG ulItem,
            /* [out] */ IQuarantineServerItem __RPC_FAR *__RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Config( 
            /* [out] */ IQserverConfig __RPC_FAR *__RPC_FAR *pConfig) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetItemCount( 
            /* [out] */ ULONG __RPC_FAR *pulCount) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IQuarantineServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IQuarantineServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IQuarantineServer __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IQuarantineServer __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumItems )( 
            IQuarantineServer __RPC_FAR * This,
            /* [out] */ IEnumQuarantineServerItems __RPC_FAR *__RPC_FAR *pEnum);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetCopyInterface )( 
            IQuarantineServer __RPC_FAR * This,
            /* [out] */ ICopyItemData __RPC_FAR *__RPC_FAR *pCopy);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemoveItem )( 
            IQuarantineServer __RPC_FAR * This,
            /* [in] */ ULONG ulFileID);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemoveAllItems )( 
            IQuarantineServer __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumItemsVariant )( 
            IQuarantineServer __RPC_FAR * This,
            /* [in] */ VARIANT __RPC_FAR *va,
            /* [out] */ IEnumQserverItemsVariant __RPC_FAR *__RPC_FAR *pEnum);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetQuarantineItem )( 
            IQuarantineServer __RPC_FAR * This,
            /* [in] */ ULONG ulItem,
            /* [out] */ IQuarantineServerItem __RPC_FAR *__RPC_FAR *pItem);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Config )( 
            IQuarantineServer __RPC_FAR * This,
            /* [out] */ IQserverConfig __RPC_FAR *__RPC_FAR *pConfig);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetItemCount )( 
            IQuarantineServer __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pulCount);
        
        END_INTERFACE
    } IQuarantineServerVtbl;

    interface IQuarantineServer
    {
        CONST_VTBL struct IQuarantineServerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IQuarantineServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IQuarantineServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IQuarantineServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IQuarantineServer_EnumItems(This,pEnum)	\
    (This)->lpVtbl -> EnumItems(This,pEnum)

#define IQuarantineServer_GetCopyInterface(This,pCopy)	\
    (This)->lpVtbl -> GetCopyInterface(This,pCopy)

#define IQuarantineServer_RemoveItem(This,ulFileID)	\
    (This)->lpVtbl -> RemoveItem(This,ulFileID)

#define IQuarantineServer_RemoveAllItems(This)	\
    (This)->lpVtbl -> RemoveAllItems(This)

#define IQuarantineServer_EnumItemsVariant(This,va,pEnum)	\
    (This)->lpVtbl -> EnumItemsVariant(This,va,pEnum)

#define IQuarantineServer_GetQuarantineItem(This,ulItem,pItem)	\
    (This)->lpVtbl -> GetQuarantineItem(This,ulItem,pItem)

#define IQuarantineServer_Config(This,pConfig)	\
    (This)->lpVtbl -> Config(This,pConfig)

#define IQuarantineServer_GetItemCount(This,pulCount)	\
    (This)->lpVtbl -> GetItemCount(This,pulCount)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQuarantineServer_EnumItems_Proxy( 
    IQuarantineServer __RPC_FAR * This,
    /* [out] */ IEnumQuarantineServerItems __RPC_FAR *__RPC_FAR *pEnum);


void __RPC_STUB IQuarantineServer_EnumItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQuarantineServer_GetCopyInterface_Proxy( 
    IQuarantineServer __RPC_FAR * This,
    /* [out] */ ICopyItemData __RPC_FAR *__RPC_FAR *pCopy);


void __RPC_STUB IQuarantineServer_GetCopyInterface_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQuarantineServer_RemoveItem_Proxy( 
    IQuarantineServer __RPC_FAR * This,
    /* [in] */ ULONG ulFileID);


void __RPC_STUB IQuarantineServer_RemoveItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQuarantineServer_RemoveAllItems_Proxy( 
    IQuarantineServer __RPC_FAR * This);


void __RPC_STUB IQuarantineServer_RemoveAllItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQuarantineServer_EnumItemsVariant_Proxy( 
    IQuarantineServer __RPC_FAR * This,
    /* [in] */ VARIANT __RPC_FAR *va,
    /* [out] */ IEnumQserverItemsVariant __RPC_FAR *__RPC_FAR *pEnum);


void __RPC_STUB IQuarantineServer_EnumItemsVariant_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQuarantineServer_GetQuarantineItem_Proxy( 
    IQuarantineServer __RPC_FAR * This,
    /* [in] */ ULONG ulItem,
    /* [out] */ IQuarantineServerItem __RPC_FAR *__RPC_FAR *pItem);


void __RPC_STUB IQuarantineServer_GetQuarantineItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQuarantineServer_Config_Proxy( 
    IQuarantineServer __RPC_FAR * This,
    /* [out] */ IQserverConfig __RPC_FAR *__RPC_FAR *pConfig);


void __RPC_STUB IQuarantineServer_Config_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IQuarantineServer_GetItemCount_Proxy( 
    IQuarantineServer __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pulCount);


void __RPC_STUB IQuarantineServer_GetItemCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IQuarantineServer_INTERFACE_DEFINED__ */



#ifndef __QSERVERLib_LIBRARY_DEFINED__
#define __QSERVERLib_LIBRARY_DEFINED__

/* library QSERVERLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_QSERVERLib;

EXTERN_C const CLSID CLSID_QuarantineServer;

#ifdef __cplusplus

class DECLSPEC_UUID("C60B0160-894B-11D2-8E5F-2A0712000000")
QuarantineServer;
#endif

EXTERN_C const CLSID CLSID_QuarantineServerItem;

#ifdef __cplusplus

class DECLSPEC_UUID("3DA647F2-ABFB-11D2-8E6D-281ED2000000")
QuarantineServerItem;
#endif

EXTERN_C const CLSID CLSID_EnumQserverItemsVariant;

#ifdef __cplusplus

class DECLSPEC_UUID("1F5FCDD7-B099-11D2-B65E-0020AFEED321")
EnumQserverItemsVariant;
#endif

EXTERN_C const CLSID CLSID_QserverConfig;

#ifdef __cplusplus

class DECLSPEC_UUID("F7BFE117-B480-11D2-B65E-0020AFEED321")
QserverConfig;
#endif

EXTERN_C const CLSID CLSID_CopyItemData;

#ifdef __cplusplus

class DECLSPEC_UUID("E04E15C9-BD46-11D2-B661-0020AFEED321")
CopyItemData;
#endif
#endif /* __QSERVERLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
