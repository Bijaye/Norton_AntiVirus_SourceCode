

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0361 */
/* at Sun Jun 26 17:07:37 2005
 */
/* Compiler settings for .\IWPLUCallback.idl:
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


#ifndef __IWPLUCallback_h__
#define __IWPLUCallback_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __luIWPCallback_FWD_DEFINED__
#define __luIWPCallback_FWD_DEFINED__

#ifdef __cplusplus
typedef class luIWPCallback luIWPCallback;
#else
typedef struct luIWPCallback luIWPCallback;
#endif /* __cplusplus */

#endif 	/* __luIWPCallback_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __IWPLUCallbackLib_LIBRARY_DEFINED__
#define __IWPLUCallbackLib_LIBRARY_DEFINED__

/* library IWPLUCallbackLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_IWPLUCallbackLib;

EXTERN_C const CLSID CLSID_luIWPCallback;

#ifdef __cplusplus

class DECLSPEC_UUID("AAB62FF5-ADA5-4C99-8BEA-64EF0C3F50F3")
luIWPCallback;
#endif
#endif /* __IWPLUCallbackLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


