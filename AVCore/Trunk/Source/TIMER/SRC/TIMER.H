/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 2.00.0102 */
/* at Mon Oct 28 17:26:45 1996
 */
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"

#ifndef __timer_h__
#define __timer_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __Timer_INTERFACE_DEFINED__
#define __Timer_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: Timer
 * at Mon Oct 28 17:26:45 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [implicit_handle][version][uuid] */ 


			/* size is 1 */
boolean Server_LogMessageA( 
    /* [in] */ unsigned long dwMilliseconds,
    /* [string][in] */ byte __RPC_FAR *lpszMessage);

			/* size is 1 */
boolean Server_LogMessageW( 
    /* [in] */ unsigned long dwMilliseconds,
    /* [string][in] */ wchar_t __RPC_FAR *lpszMessage);


extern handle_t Timersrv_handle;


extern RPC_IF_HANDLE Timer_v1_0_c_ifspec;
extern RPC_IF_HANDLE Timer_v1_0_s_ifspec;
#endif /* __Timer_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
