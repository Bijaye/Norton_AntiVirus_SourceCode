// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.
//----------------------------------------------------------------------------
// Provides Registry services over IPC.
//----------------------------------------------------------------------------


#ifndef REGISTRY_SERVER_H_INCLUDED
#define REGISTRY_SERVER_H_INCLUDED


#ifdef __cplusplus
extern "C"
{
#endif

void RegistryServer( void* );
void RegistryServerShutdown();

#ifdef __cplusplus
}
#endif

#endif // REGISTRY_SERVER_H_INCLUDED
