/* proto.h                                                                             */

#ifndef  __PROTO__
#define  __PROTO__
#include <stdlib.h>
#include <stdio.h>
#include <share.h>
#include <string.h>
#include <io.h>

#if 0
typedef unsigned long BIGUINT;
typedef          long BIGINT;
#define BIGUINT_U "%u"
#define BIGUINT_UNL "%u"
#define BIGUINT_DU  "%d %u"
#define BIGUINT_DUNL "%d %u\n"
#else
typedef unsigned __int64 BIGUINT;
typedef          __int64 BIGINT;
#define BIGUINT_U "%I64u"
#define BIGUINT_UNL "%I64d\n"
#define BIGUINT_DU  "%d %I64u"
#define BIGUINT_DUNL "%d %I64u\n"
#endif

/*-------------------------------------------------------------------------------------*/
/* GLOBAL AUTOSIG DEFINITIONS                                                          */
/*-------------------------------------------------------------------------------------*/

#define  NMAX          5
#if      !defined(MAX_PATH)
#define  MAX_PATH      (518*2) /* dbcs */
#endif

/*-------------------------------------------------------------------------------------*/

#endif                                 /* __PROTO__                               */
