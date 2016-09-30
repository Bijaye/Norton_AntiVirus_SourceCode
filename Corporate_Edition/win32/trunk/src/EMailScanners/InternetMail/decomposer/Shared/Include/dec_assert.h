//#############################################################################
//#
//#     dec_assert.h is used to redefine assert for platforms that 
//#		require another assert definition in debug mode.
//#
// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
//#     Copyright 2004, 2005 Symantec Corporation. All rights reserved.
//#
//#     Revisions
//#     ---------
//#     1/23/2004 -- Thomas Hurtt -- Initial Revision
//#
//#############################################################################
#ifndef _DEC_ASSERT_H_
#define _DEC_ASSERT_H_

#include <assert.h>
#if	!defined(NDEBUG)
	#if defined(SYM_NLM)
		#include <nwdebug.h>
		#include <nwconio.h>

		void inline _dec_assert( const char * exp, const char * file, int line)
		{
			if(!exp)
			{
				ConsolePrintf("Assertion failed in: %S, line: %d\n", file, line);
				EnterDebugger();
			}
		}
				
		#define dec_assert(exp) _dec_assert(#exp, __FILE__, __LINE__)

		// including nwconio.h causes TRUE and FALSE to be undefined
		#ifndef TRUE
			#define TRUE 1
		#endif
		#ifndef FALSE
			#define FALSE 0
		#endif

	#else
		#define dec_assert(exp) assert(exp)
	#endif
#else
	#define dec_assert(exp) (void(0))
#endif

#endif // _DEC_ASSERT_H_
