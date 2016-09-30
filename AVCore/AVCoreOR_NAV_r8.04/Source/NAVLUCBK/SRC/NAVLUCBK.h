/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Tue Dec 29 20:36:12 1998
 */
/* Compiler settings for C:\Work\NAVLUCALLBACK\NavLUCallback.idl:
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

#ifndef __NavLUCallback_h__
#define __NavLUCallback_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __luNavCallBack_FWD_DEFINED__
#define __luNavCallBack_FWD_DEFINED__

#ifdef __cplusplus
typedef class luNavCallBack luNavCallBack;
#else
typedef struct luNavCallBack luNavCallBack;
#endif /* __cplusplus */

#endif 	/* __luNavCallBack_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 


#ifndef __NAVLUCALLBACKLib_LIBRARY_DEFINED__
#define __NAVLUCALLBACKLib_LIBRARY_DEFINED__

/* library NAVLUCALLBACKLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_NAVLUCALLBACKLib;

EXTERN_C const CLSID CLSID_luNavCallBack;

#ifdef __cplusplus

class DECLSPEC_UUID("09C9DBC1-893D-11D2-B40A-00600831DD76")
luNavCallBack;
#endif
#endif /* __NAVLUCALLBACKLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
