/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 2.00.0102 */
/* at Mon Oct 28 17:28:21 1996
 */
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"

#ifndef __Link_h__
#define __Link_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __Link_INTERFACE_DEFINED__
#define __Link_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: Link
 * at Mon Oct 28 17:28:21 1996
 * using MIDL 2.00.0102
 ****************************************/
/* [implicit_handle][version][uuid] */ 


			/* size is 4 */
unsigned long Link_SendCommand( 
    /* [in] */ unsigned long dwSize,
    /* [size_is][in] */ byte __RPC_FAR *pPacket);

			/* size is 4 */
unsigned long Link_SendStringA( 
    /* [string][in] */ byte __RPC_FAR *lpszString);

			/* size is 4 */
unsigned long Link_SendStringW( 
    /* [string][in] */ wchar_t __RPC_FAR *lpszString);


extern handle_t Linksrv_handle;


extern RPC_IF_HANDLE Link_v1_0_c_ifspec;
extern RPC_IF_HANDLE Link_v1_0_s_ifspec;
#endif /* __Link_INTERFACE_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
