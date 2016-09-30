// Copyright 2004 Symantec Corporation. All rights reserved.

#ifndef _ndkNLMFunctions_h_
#define _ndkNLMFunctions_h_

#include "ndkDefinitions.h"

#ifdef __cplusplus
extern "C" {
#endif

/*NLMFunctions*/
//AllocateResourceTag
//ImportSymbol
//RegisterForEvent
//UnimportSymbol
//UnregisterForEvent

//this is the only function in this category that might possibly need a stub, the rest will definitly not be needed.
LONG AllocateResourceTag  (   
   LONG   NLMHandle,    
   BYTE   *descriptionString,    
   LONG   resourceType);

#ifdef __cplusplus
}
#endif
	
#endif // _ndkNLMFunctions_h_
