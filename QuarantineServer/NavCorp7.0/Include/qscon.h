/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Apr 06 15:28:30 1999
 */
/* Compiler settings for C:\work\Norton_AntiVirus\QuarantineServer\QsConsole\qscon.idl:
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

#ifndef __qscon_h__
#define __qscon_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IQCResultItem_FWD_DEFINED__
#define __IQCResultItem_FWD_DEFINED__
typedef interface IQCResultItem IQCResultItem;
#endif 	/* __IQCResultItem_FWD_DEFINED__ */


#ifndef __QSConsole_FWD_DEFINED__
#define __QSConsole_FWD_DEFINED__

#ifdef __cplusplus
typedef class QSConsole QSConsole;
#else
typedef struct QSConsole QSConsole;
#endif /* __cplusplus */

#endif 	/* __QSConsole_FWD_DEFINED__ */


#ifndef __QSConsoleAbout_FWD_DEFINED__
#define __QSConsoleAbout_FWD_DEFINED__

#ifdef __cplusplus
typedef class QSConsoleAbout QSConsoleAbout;
#else
typedef struct QSConsoleAbout QSConsoleAbout;
#endif /* __cplusplus */

#endif 	/* __QSConsoleAbout_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IQCResultItem_INTERFACE_DEFINED__
#define __IQCResultItem_INTERFACE_DEFINED__

/* interface IQCResultItem */
/* [object][uuid] */ 


EXTERN_C const IID IID_IQCResultItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A25EB152-E8FA-11d2-A44E-00105AA739C9")
    IQCResultItem : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetItemIDs( 
            /* [out] */ ULONG __RPC_FAR *ulCount,
            /* [out] */ ULONG __RPC_FAR *__RPC_FAR *ulItemArray) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetQserverItem( 
            /* [in] */ ULONG ulItemID,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *pItem) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IQCResultItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IQCResultItem __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IQCResultItem __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IQCResultItem __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetItemIDs )( 
            IQCResultItem __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *ulCount,
            /* [out] */ ULONG __RPC_FAR *__RPC_FAR *ulItemArray);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetQserverItem )( 
            IQCResultItem __RPC_FAR * This,
            /* [in] */ ULONG ulItemID,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *pItem);
        
        END_INTERFACE
    } IQCResultItemVtbl;

    interface IQCResultItem
    {
        CONST_VTBL struct IQCResultItemVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IQCResultItem_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IQCResultItem_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IQCResultItem_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IQCResultItem_GetItemIDs(This,ulCount,ulItemArray)	\
    (This)->lpVtbl -> GetItemIDs(This,ulCount,ulItemArray)

#define IQCResultItem_GetQserverItem(This,ulItemID,pItem)	\
    (This)->lpVtbl -> GetQserverItem(This,ulItemID,pItem)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IQCResultItem_GetItemIDs_Proxy( 
    IQCResultItem __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *ulCount,
    /* [out] */ ULONG __RPC_FAR *__RPC_FAR *ulItemArray);


void __RPC_STUB IQCResultItem_GetItemIDs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IQCResultItem_GetQserverItem_Proxy( 
    IQCResultItem __RPC_FAR * This,
    /* [in] */ ULONG ulItemID,
    /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *pItem);


void __RPC_STUB IQCResultItem_GetQserverItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IQCResultItem_INTERFACE_DEFINED__ */



#ifndef __QSCONLib_LIBRARY_DEFINED__
#define __QSCONLib_LIBRARY_DEFINED__

/* library QSCONLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_QSCONLib;

EXTERN_C const CLSID CLSID_QSConsole;

#ifdef __cplusplus

class DECLSPEC_UUID("26140C85-B554-11D2-8F45-3078302C2030")
QSConsole;
#endif

EXTERN_C const CLSID CLSID_QSConsoleAbout;

#ifdef __cplusplus

class DECLSPEC_UUID("26140C86-B554-11D2-8F45-3078302C2030")
QSConsoleAbout;
#endif
#endif /* __QSCONLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
