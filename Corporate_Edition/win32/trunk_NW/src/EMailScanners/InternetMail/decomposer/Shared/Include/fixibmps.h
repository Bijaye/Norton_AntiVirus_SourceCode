// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifndef __FIX_POSIX_SOURCE
#define __FIX_POSIX_SOURCE

/********************************************************************/
/* This file is to fix the error where the system complains about   */
/* the type fd_set not being defined when _POSIX_SOURCE is defined. */
/* Include this file as the first #include file in the source file  */
/* and this complaint will disappear.                               */
/*                                                                  */
/* This hack is needed because of a defect in IBM's header files    */
/* for V3.x (and possibly beyond) of Visual Age for C++.  It will   */
/* manifest itself on the AS/400 platform, currently.               */
/********************************************************************/
#if defined(OS400)

#ifdef _POSIX_SOURCE
#define _POSIX_SOURCE_UNDEFED__Q_ _POSIX_SOURCE
#undef _POSIX_SOURCE
#endif

#include <sys/types.h>

#ifdef _POSIX_SOURCE_UNDEFED__Q_
#define _POSIX_SOURCE _POSIX_SOURCE_UNDEFED__Q_
#undef _POSIX_SOURCE_UNDEFED__Q_
#endif

/* The old V3R7 compiler that we build with for CM does not define
'timespec' in time.h. This causes us build errors. Fix with this kludge. */

#include <time.h>
#ifndef _TIMESPEC_T_
#define _TIMESPEC_T_
typedef struct timespec {
	time_t tv_sec;
	long tv_nsec;
} timespec_t;
#endif

#endif

#endif
