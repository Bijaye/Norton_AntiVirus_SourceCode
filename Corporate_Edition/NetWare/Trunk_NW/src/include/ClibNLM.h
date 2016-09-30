// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Common definitions for Clib based code.

#ifndef _ClibNLM_h_
#define _ClibNLM_h_

#ifdef __cplusplus
extern "C" {
#endif

#include <nlm/nwerrno.h>

#include "winBaseTypes.h"

#define REG_OPTION_NON_VOLATILE		(0x00000000L)	 // Key is preserved

typedef DWORD ACCESS_MASK;
typedef ACCESS_MASK REGSAM;

#ifndef DEBUGFLAGTYPE
#define DEBUGFLAGTYPE unsigned long long
#endif

void Real_dprintf( const char* format,...);
void Real_dvprintf(const char* format,va_list args);
void Real_dprintfTagged( DEBUGFLAGTYPE dwTag,const char *format,...);
void Real_dvprintfTagged(DEBUGFLAGTYPE dwTag,const char *format,va_list args);

#ifdef __cplusplus
}
#endif

#endif // _ClibNLM_h_
